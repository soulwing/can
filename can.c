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
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>

#include "dstring.h"
#include "aufs.h"
#include "proc.h"
#include "tmpfs.h"
#include "config.h"


int child_fn(void *arg);

int main(int argc, char * const argv[])
{
  void *child_stack = malloc(CHILD_STACK_BYTES);

  pid_t child_pid = clone(child_fn, child_stack + CHILD_STACK_BYTES, 
      CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS | SIGCHLD, NULL);
  if (child_pid == -1) {
    perror("error cloning process");
    exit(EXIT_FAILURE);
  }

  waitpid(child_pid, NULL, 0);
  exit(EXIT_SUCCESS);
}

int child_fn(void *arg)
{
  char * const argv[2] = { EXEC_PATH, NULL };

  int ns = open(NET_NAMESPACE_PATH, O_RDONLY);
  if (ns == -1) {
    perror("error opening network namespace handle");
    exit(EXIT_FAILURE);
  }

  if (setns(ns, CLONE_NEWNET) != 0) {
    perror("error setting network namespace");
    exit(EXIT_FAILURE);
  }

  if (close(ns) != 0) {
    perror("error closing network namespace handle");
  }
  
  if (mount(NULL, "/", NULL, MS_PRIVATE, NULL) != 0) {
    perror("error unsharing root filesystem");
    exit(EXIT_FAILURE);
  }

  if (mount(NULL, PROC_PATH, NULL, MS_PRIVATE, NULL) != 0) {
    perror("error unsharing proc filesystem");
    exit(EXIT_FAILURE);
  }

  if (mount_aufs(ROOT_MOUNT_POINT) != 0) {
    perror("error mounting container root filesystem");
    exit(EXIT_FAILURE);
  }

  if (mount_proc(ROOT_MOUNT_POINT) != 0) {
    perror("error mounting container proc filesystem");
    exit(EXIT_FAILURE);
  }

  if (mount_tmpfs(ROUTE_MOUNT_POINT, "/tmp") != 0) {
    perror("error mounting container temp filesystem");
    exit(EXIT_FAILURE);    
  }
  
  if (chroot(ROOT_MOUNT_POINT) != 0) {
    perror("error changing root filesystem");
    exit(EXIT_FAILURE);
  }

  if (chdir("/") != 0) {
    perror("error setting root as current directory");
    exit(EXIT_FAILURE);
  }

  if (sethostname(HOST_NAME, strlen(HOST_NAME)) != 0) {
    perror("error setting hostname");
    exit(EXIT_FAILURE);
  }

  if (execv(EXEC_PATH, argv) != 0) {
    perror("error executing shell");
    exit(EXIT_FAILURE);
  }

}
