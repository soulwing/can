
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

#define CHILD_STACK_BYTES   (1024*1024)
#define NET_NAMESPACE_PATH  "/var/run/netns/ns0"
#define EXEC_PATH           "/bin/sh"
#define HOST_NAME           "my-container"
#define ROOT_FILESYSTEM     "/var/aufs/mnt"
#define PROC_FILESYSTEM     "/proc"

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

  char * proc_mnt = malloc(strlen(ROOT_FILESYSTEM) + strlen(PROC_FILESYSTEM) + 1);
  strncpy(proc_mnt, ROOT_FILESYSTEM, strlen(ROOT_FILESYSTEM) + 1);
  strncat(proc_mnt, PROC_FILESYSTEM, strlen(PROC_FILESYSTEM) + 1);

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
  
  if (mount(NULL, PROC_FILESYSTEM, NULL, MS_PRIVATE, NULL) != 0) {
    perror("error unsharing proc filesystem");
    exit(EXIT_FAILURE);
  }

  if (mount(NULL, ROOT_FILESYSTEM, NULL, MS_PRIVATE, NULL) != 0) {
    perror("error unsharing root filesystem");
  }

  if (mount("proc", proc_mnt, "proc", 0, NULL) != 0) {
    perror("error mounting proc filesystem");
    exit(EXIT_FAILURE);
  }

  if (chroot(ROOT_FILESYSTEM) != 0) {
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
