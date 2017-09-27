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

#ifndef _DSTRING_H
#define _DSTRING_H

#include <sys/types.h>

#define   dstr_text(s)  (s->s_text)
#define   dstr_len(s) 	(s->s_used)

typedef struct String String;

/*!
 * \brief Creates a new String
 *
 * Creates a new String with the specified initial contents (which may be NULL if an empty
 * string is desired). The contents of \c text are copied into an internal buffer of the
 * String.
 *
 * \param[in] text initial contents of the String; may be NULL to create an empty String
 *
 * \return    pointer to a String structure which must be freed by the caller.
 */
String *dstr_init(const char *text);

/*!
 * \brief Appends text to an existing String
 *
 * Appends the given \c text to \s, expanding the buffer for \c s if necessary. If
 * \c s is NULL, \c s remains unchanged.
 *
 * \param[in] text text to be appended to \c
 */
void dstr_append(String *s, const char *text);

/*!
 * \brief Releases all memory allocated to a String.
 *
 * Releases all memory allocated to \c s. It is an error to reference \c s after
 * this function returns.
 *
 * \param[in] text text to be appended to \c
 *
 */
void dstr_free(String *s);

#endif /* _DSTRING_H */
