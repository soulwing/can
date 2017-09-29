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
#include <sys/mount.h>

#include "dstring.h"
#include "config.h"
#include "tmpfs.h"

#define TMPFS_TYPE  "tmpfs"


int mount_tmpfs(const char *root_path, const char * const *paths)
{
  int i = 0;
  int rc = 0;
  while (paths[i] != 0) {
    rc = mount_tmpfs_at_path(root_path, paths[i]);
    if (rc != 0) return rc;
    i++;
  }
  return 0;
}

static int mount_tmpfs_at_path(const char *root_path, const char *path)
{
  String *mount_path = dstr_init(root_path);
  dstr_append(mount_path, path);

  int rc = mount(TMPFS_TYPE, dstr_text(mount_path), TMPFS_TYPE, 
        MS_NOEXEC | MS_NOSUID, "size=101468k,mode=755");

  dstr_free(mount_path);
  return rc;
}

