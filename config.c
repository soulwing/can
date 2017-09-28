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

#include "config.h"

static int use_tmpfs = 1;
static char * const netns_name = "ns0";
static char * const host_name = DEFAULT_HOST_NAME;
static char * const root_path = DEFAULT_ROOT_PATH;
static char * const aufs_path = DEFAULT_AUFS_PATH;
static char * const command_argv[] = { DEFAULT_COMMAND, NULL }


int conf_init(int argc, char * const argv[])
{
  return 0;
}

int conf_use_tmpfs(void)
{
  return use_tmpfs;
}

char * const conf_netns_name(void)
{
  return netns_name;
}

char * const conf_host_name(void)
{
  return host_name;
}

char * const conf_root_path(void)
{
  return root_path;
}

char * const conf_aufs_path(void)
{
  return aufs_path;
}

char ** const conf_command_argv(void)
{
  return command_argv;
}