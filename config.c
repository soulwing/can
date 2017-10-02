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

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define DEFAULT_COMMAND     "/bin/sh"
#define MAX_TMP_PATHS       31

static int use_mount_ns = 1;
static int use_tmpfs = 1;
static char * netns_name = 0;
static const char *host_name = 0;

static const char *root_path;
static const char *aufs_path;

static int tmp_paths_count = 0;
static const char *tmp_paths[MAX_TMP_PATHS + 1];

static char * const *command_argv = 0;

static char * const default_command_argv[] = { DEFAULT_COMMAND, 0 };
static const char * opts = "n:h:r:f:t:";

static struct option long_opts[] = 
{
  { "no-mount-ns", no_argument, &use_mount_ns, 0 },
  { "tmpfs", required_argument, 0, 't' },
  { "netns", required_argument, 0, 'n' },
  { "hostname", required_argument, 0, 'h' },
  { "chroot", required_argument, 0, 'r' },
  { "fs", required_argument, 0, 'f' },
  { 0, 0, 0, 0 }
};

void conf_usage(const char *argv0, FILE *out)
{
  fprintf(out, "usage: %s [options] [command...]\n", argv0);
  fprintf(out, "options\n");
  fprintf(out, "  -f path, --fs path         filesystem location for AUFS branches\n");
  fprintf(out, "  -r path, --chroot path     root path for the can\n");
  fprintf(out, "  -t path, --tmpfs path      mount a tmpfs at the given path");
  fprintf(out, "  -n name, --netns name      name of an existing network namespace for the can\n");
  fprintf(out, "  -h name, --hostname name   host name for the can\n");
  fprintf(out, "  --no-mount-ns              don't use a mount namespace\n");
  fflush(out);
}

int conf_init(int argc, char * const argv[])
{
  int c;
  int option_index = 0;

  opterr = 0;
  while ((c = getopt_long(argc, argv, opts, long_opts, 
      &option_index)) != -1) {
    switch (c) {
      case 0:
        break;
      case 'n':
        netns_name = optarg;
        break;
      case 'h':
        host_name = optarg;
        break;
      case 'r':
        root_path = optarg;
        break;
      case 'f':
        aufs_path = optarg;
        break;
      case 't':
        if (tmp_paths_count < MAX_TMP_PATHS) {
          tmp_paths[tmp_paths_count++] = optarg;
        }
        else {
          fprintf(stderr, "too many tmpfs paths; %s will not use tmpfs\n", optarg);
        }
        break;
      case '?':
      default:
        return -1;
    }

  }

  if (root_path != NULL) {
    use_mount_ns = 1;
  }

  if (optind < argc) {
    command_argv = argv + optind;
  }

  return 0;
}

int conf_use_mountns(void)
{
  return use_mount_ns;
}

const char * conf_netns_name(void)
{
  return netns_name;
}

const char * conf_host_name(void)
{
  return host_name;
}

const char * conf_root_path(void)
{
  return root_path;
}

const char * conf_aufs_path(void)
{
  return aufs_path;
}

const char * const *conf_tmp_paths(void)
{
  return tmp_paths;
}

char * const *conf_command_argv(void)
{
  if (command_argv == 0) return default_command_argv;
  return command_argv;
}
