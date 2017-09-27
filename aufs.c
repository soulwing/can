/*
 * Copyright (c) 2017 Carl Harris Jr, Michael Irwin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/mount.h>

#include "dstring.h"
#include "aufs.h"

static String * aufs_opts();
static void append_layers(String *opts);

int mount_aufs(const char *path)
{
  String *opts = aufs_opts();

  int rc = mount("root", path, AUFS_TYPE, 0, dstr_text(opts));
  dstr_free(opts);

  return rc;
}

static String * aufs_opts() {
  String *opts = dstr_init(AUFS_CONTAINER_PATH);
  dstr_append(opts, "=rw:");
  dstr_append(opts, AUFS_CONFIG_PATH);
  dstr_append(opts, "=ro");
  append_layers(opts);
  return opts;
}

static void append_layers(String *opts)
{
  struct dirent **namelist;
  
  int n = scandir(AUFS_LAYERS_PATH, &namelist, NULL, versionsort);
  if (n < 0) {
    perror("scandir");
    exit(EXIT_FAILURE);
  }
  while (n--) { 
    char *name = namelist[n]->d_name;
    if (strncmp(name, ".", 2) != 0
        && strncmp(name, "..", 2) != 0) {
      dstr_append(opts, ":");
      dstr_append(opts, AUFS_LAYERS_PATH);
      dstr_append(opts, "/");
      dstr_append(opts, name);
      dstr_append(opts, "=ro");
    }
    free(namelist[n]);
  }
}