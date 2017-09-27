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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dstring.h"

typedef struct String {
  char *s_text;
  size_t s_size;
  size_t s_used;
} String;

String *dstr_init(const char *text)
{
  String *s = malloc(sizeof(String));
  size_t textlen = strlen(text);
  s->s_size = 2*textlen + 1;
  s->s_used = textlen + 1;
  s->s_text = strdup(text);
  return s; 
}

void dstr_append(String *s, const char *text)
{
  size_t textlen = strlen(text);
  if (s->s_size - s->s_used < textlen) {
    while (s->s_size - s->s_used < textlen) {
      s->s_size *= 2;
    }
    s->s_text = realloc(s->s_text, s->s_size);
  }
  s->s_used += textlen;
  strncat(s->s_text, text, s->s_size - 1);
}

void dstr_free(String *s) {
  free(s->s_text);
  free(s);
}

char * const dstr_text(String *s)
{
  return s->s_text;
}

size_t dstr_len(String *s)
{
  return s->s_used;
}