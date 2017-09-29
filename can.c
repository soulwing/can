/*
 * Copyright (c) 2017 Carl Harris Jr, Michael Irwin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>

#include "dstring.h"
#include "aufs.h"
#include "proc.h"
#include "tmpfs.h"
#include "netns.h"
#include "config.h"

#define CHILD_STACK_BYTES   (1024*1024)

int child_fn(void *arg);

int main(int argc, char * const argv[])
{
  void *child_stack = malloc(CHILD_STACK_BYTES);

  if (conf_init(argc, argv) != 0) {
    conf_usage(argv[0], stderr);
    exit(EXIT_FAILURE);
  }

  pid_t child_pid = clone(child_fn, child_stack + CHILD_STACK_BYTES, 
      CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, NULL);
  if (child_pid == -1) {
    perror("error cloning process");
    exit(EXIT_FAILURE);
  }

  waitpid(child_pid, NULL, 0);
  exit(EXIT_SUCCESS);
}

int child_fn(void *arg)
{

  /* set network namespace if specified */
  const char *netns_name = conf_netns_name();
  if (netns_name != NULL) {
    if (set_netns(netns_name) != 0) {
      perror("error setting network namespace");
      exit(EXIT_FAILURE);
    }
  }

  /* set hostname for our can, if desired */
  const char *hostname = conf_host_name();
  if (hostname != NULL) {
    if (unshare(CLONE_NEWUTS) != 0) {
      perror("error creating UTS namespace");
      exit(EXIT_FAILURE);
    }
    if (sethostname(hostname, strlen(hostname)) != 0) {
      perror("error setting hostname");
      exit(EXIT_FAILURE);
    }
  }

  /* don't want a shared root mount point in our namespace */
  if (mount(NULL, "/", NULL, MS_PRIVATE, NULL) != 0) {
    perror("error unsharing root filesystem");
    exit(EXIT_FAILURE);
  }

  /* don't want other namespaces to see our /proc mount */
  if (mount(NULL, PROC_PATH, NULL, MS_PRIVATE, NULL) != 0) {
    perror("error unsharing proc filesystem");
    exit(EXIT_FAILURE);
  }

  const char *root_path = conf_root_path();
  if (root_path != NULL) {
  
    /* mount a layered AUFS fileystem */
    const char *aufs_path = conf_aufs_path();
    if (aufs_path != NULL) {
      if (mount_aufs(aufs_path, root_path) != 0) {
        perror("error mounting container root filesystem");
        exit(EXIT_FAILURE);
      }
    }

    /* mount proc filesystem for our can */
    if (mount_proc(root_path) != 0) {
      perror("error mounting container /proc filesystem");
      exit(EXIT_FAILURE);
    }    

    /* use tmpfs for paths with transient stuff in them, if desired */
    if (mount_tmpfs(root_path, conf_tmp_paths()) != 0) {
      perror("error mounting container /tmp filesystem");
      exit(EXIT_FAILURE);          
    }
  
    /* set root filesystem for our can */
    if (chroot(root_path) != 0) {
      perror("error changing root filesystem");
      exit(EXIT_FAILURE);
    }

    if (chdir("/") != 0) {
      perror("error setting root as current directory");
      exit(EXIT_FAILURE);
    }
  }
  else {    // no chroot
    /* mount proc filesystem for our can */
    if (mount_proc("/") != 0) {
      perror("error mounting container /proc filesystem");
      exit(EXIT_FAILURE);
    }    
  }

  /* execute the specified command */
  char * const *argv = conf_command_argv();
  if (execv(argv[0], argv) != 0) {
    perror("error executing command");
    exit(EXIT_FAILURE);
  }

}
