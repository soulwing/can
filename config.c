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
#include <unistd.h>
#include <getopt.h>

static int use_tmpfs = 1;
static char * netns_name = "ns0";

static const char *host_name = DEFAULT_HOST_NAME;
static const char *root_path = DEFAULT_ROOT_PATH;
static const char *aufs_path = DEFAULT_AUFS_PATH;
static char * const *command_argv = 0;


static char * const default_command_argv[] = { DEFAULT_COMMAND, 0 };
static const char * opts = "tn:h::r:f:";

static struct option long_opts[] = 
{
  { "tmpfs", no_argument, &use_tmpfs, 1 },
  { "netns", required_argument, 0, 'n' },
  { "hostname", optional_argument, 0, 'h' },
  { "root", required_argument, 0, 'r' },
  { "fs", required_argument, 0, 'f' }
};

int conf_init(int argc, char * const argv[])
{
  int c;
  int option_index = 0;

  opterr = 1;
  while ((c = getopt_long(argc, argv, opts, long_opts, 
      &option_index)) != -1) {
    switch (c) {
      case 0:
        break;
      case 't':
        use_tmpfs = 1;
        break;
      case 'n':
        netns_name = optarg;
        break;
      case 'h':
        host_name = optarg != 0 ? optarg : DEFAULT_HOST_NAME;
        break;
      case 'r':
        root_path = optarg;
        break;
      case 'f':
        aufs_path = optarg;
        break;
      case '?':
        return -1;
      default:
        abort();
    }

  }

  if (optind < argc) {
    command_argv = argv + optind;
  }

  printf("use_tmpfs=%d netns_name=%s root_path=%s aufs_path=%s host_name=%s\n",
      use_tmpfs, netns_name, root_path, aufs_path, host_name);

  if (command_argv != 0) {
    int i = 0;
    printf("command: ");
    while (command_argv[i] != 0) {
      printf("%s ", command_argv[i]);
      i++;
    }
  }
    
  return 0;
}

int conf_use_tmpfs(void)
{
  return use_tmpfs;
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

char * const *conf_command_argv(void)
{
  if (command_argv == 0) return default_command_argv;
  return command_argv;
}
