
/*
 * The olsr.org Optimized Link-State Routing daemon version 2 (olsrd2)
 * Copyright (c) 2004-2015, the olsr.org team - see HISTORY file
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of olsr.org, olsrd nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Visit http://www.olsr.org for more information.
 *
 * If you find this software useful feel free to make a donation
 * to the project. For more information see the website or contact
 * the copyright holders.
 *
 */

/**
 * @file
 */

#ifndef COMMON_STRING_H_
#define COMMON_STRING_H_

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "common/common_types.h"

enum
{
  /*! block size for allocated string arrays */
  STRARRAY_BLOCKSIZE = 64
};

/**
 * Macro to statically initialize a string array
 * @param str string array representation
 */
#define STRARRAY_INIT(str)                                                                                             \
  { .value = (str), .length = sizeof(str) }

/**
 * Represents a string or an array of strings
 * The strings (including the zero byte) are just appended
 * into a large binary buffer. The struct contains a pointer
 * to the first string and the size of the binary buffer
 *
 * typically append operations are done by realloc() calls
 * while remove operations are done with memmove
 */
struct strarray {
  /*! pointer to the first string */
  char *value;

  /*! total length of all strings including zero-bytes */
  size_t length;
};

/**
 * Constant value variant of strarray
 */
struct const_strarray {
  /*! pointer to the first string */
  const char *value;

  /*! total length of all strings including zero-bytes */
  size_t length;
};

EXPORT char *strscpy(char *dest, const char *src, size_t size);
EXPORT char *strscat(char *dest, const char *src, size_t size);
EXPORT char *str_trim(char *ptr);
EXPORT const char *str_hasnextword(const char *buffer, const char *word);
EXPORT const char *str_cpynextword(char *dst, const char *buffer, size_t len);
EXPORT const char *str_skipnextword(const char *src);
EXPORT size_t str_countwords(const char *src);

EXPORT bool str_is_printable(const char *value);

EXPORT int strarray_copy(struct strarray *dst, const struct strarray *src);
EXPORT int strarray_append(struct strarray *, const char *);
EXPORT int strarray_prepend(struct strarray *array, const char *string);
EXPORT void strarray_remove_ext(struct strarray *, char *, bool);

EXPORT char *strarray_get(const struct strarray *array, size_t idx);
EXPORT size_t strarray_get_count(const struct strarray *array);

EXPORT int strarray_cmp(const struct strarray *a1, const struct strarray *a2);

/**
 * @param c character
 * @return true if character is printable, false otherwise
 */
static INLINE bool
str_char_is_printable(char c) {
  unsigned char uc = (unsigned char)c;
  return !(uc < 32 || uc == 127 || uc == 255);
}

/**
 * @param string string
 * @param pattern pattern that might be in the string
 * @return true if string starts with the pattern, false otherwise
 */
static INLINE bool
str_startswith(const char *string, const char *pattern) {
  return strncmp(string, pattern, strlen(pattern)) == 0;
}

/**
 * This function tests if a string starts with a pattern,
 * ignoring upper/lowercase
 * @param string string
 * @param pattern pattern that might be in the string
 * @return true if string starts with the pattern, false otherwise
 */
static INLINE bool
str_startswith_nocase(const char *string, const char *pattern) {
  return strncasecmp(string, pattern, strlen(pattern)) == 0;
}

/**
 * @param string string
 * @param pattern pattern that might be in the string
 * @return true if string ends with the pattern, false otherwise
 */
static INLINE bool
str_endswith(const char *string, const char *pattern) {
  size_t s_len, p_len;

  s_len = strlen(string);
  p_len = strlen(pattern);

  return s_len > p_len && strcmp(&string[s_len - p_len], pattern) == 0;
}

/**
 * This function tests if a string ends with a pattern,
 * ignoring upper/lowercase
 * @param string string
 * @param pattern pattern that might be in the string
 * @return true if string ends with the pattern, false otherwise
 */
static INLINE bool
str_endswith_nocase(const char *string, const char *pattern) {
  size_t s_len, p_len;

  s_len = strlen(string);
  p_len = strlen(pattern);

  return s_len > p_len && strcasecmp(&string[s_len - p_len], pattern) == 0;
}

/**
 * Copy a constant string array into a second array
 * @param dst target array
 * @param src constant source array
 * @return 0 if array was copied, -1 if an error happened
 */
static INLINE int
strarray_copy_c(struct strarray *dst, const struct const_strarray *src) {
  return strarray_copy(dst, (const struct strarray *)src);
}

/**
 * Get a string of an string array
 * @param array source array
 * @param idx index of string to be extracted
 * @return string at the specified index, NULL if not found
 */
static INLINE const char *
strarray_get_c(const struct const_strarray *array, size_t idx) {
  return strarray_get((const struct strarray *)array, idx);
}

/**
 * @param array constant string array
 * @return number of strings in string array
 */
static INLINE size_t
strarray_get_count_c(const struct const_strarray *array) {
  return strarray_get_count((const struct strarray *)array);
}

/**
 * Initialize string array object
 * @param array pointer to string array object
 */
static INLINE void
strarray_init(struct strarray *array) {
  memset(array, 0, sizeof(*array));
}

/**
 * Free memory of string array object
 * @param array pointer to string array object
 */
static INLINE void
strarray_free(struct strarray *array) {
  free(array->value);
  strarray_init(array);
}

/**
 * @param array pointer to string array object
 * @return true if the array is empty, false otherwise
 */
static INLINE bool
strarray_is_empty(const struct strarray *array) {
  return array->value == NULL;
}

/**
 * @param array pointer to constant string array object
 * @return true if the array is empty, false otherwise
 */
static INLINE bool
strarray_is_empty_c(const struct const_strarray *array) {
  return array->value == NULL;
}

/**
 * Remove an element from a string array
 * @param array pointer to string array object
 * @param element an element to be removed from the array
 */
static INLINE void
strarray_remove(struct strarray *array, char *element) {
  strarray_remove_ext(array, element, true);
}

/**
 * @param array pointer to strarray object
 * @return pointer to first string of string array
 */
static INLINE char *
strarray_get_first(const struct strarray *array) {
  return array->value;
}

/**
 * @param array pointer to constant strarray object
 * @return pointer to first string of string array
 */
static INLINE const char *
strarray_get_first_c(const struct const_strarray *array) {
  return array->value;
}

/**
 * Do not call this function for the last string in
 * a string array.
 * @param current pointer to a string in array
 * @return pointer to next string in string array
 */
static INLINE char *
strarray_get_next(char *current) {
  return current + strlen(current) + 1;
}

/**
 * Do not call this function for the last string in
 * a string array.
 * @param current pointer to a string in constant array
 * @return pointer to next string in string array
 */
static INLINE const char *
strarray_get_next_c(const char *current) {
  return current + strlen(current) + 1;
}

/**
 * @param array pointer to strarray object
 * @param current pointer to a string in array
 * @return pointer to next string in string array,
 *   NULL if there is no further string
 */
static INLINE char *
strarray_get_next_safe(const struct strarray *array, char *current) {
  char *next;

  next = current + strlen(current) + 1;
  if (next > array->value + array->length) {
    return NULL;
  }
  return next;
}

/**
 * @param array pointer to constant strarray object
 * @param current pointer to a string in array
 * @return pointer to next string in string array,
 *   NULL if there is no further string
 */
static INLINE const char *
strarray_get_next_safe_c(const struct const_strarray *array, const char *current) {
  const char *next;

  next = current + strlen(current) + 1;
  if (next > array->value + array->length) {
    return NULL;
  }
  return next;
}

/**
 * Compare two constant stringarrays
 * @param a1 pointer to array 1
 * @param a2 pointer to array 2
 * @return <0 if a1 is 'smaller' than a2, >0 if a1 is 'larger' than a2,
 *   0 if both are the same.
 */
static INLINE int
strarray_cmp_c(const struct const_strarray *a1, const struct const_strarray *a2) {
  return strarray_cmp((const struct strarray *)a1, (const struct strarray *)a2);
}

/**
 * Loop over an array of strings. This loop should not be used if elements are
 * removed from the array during the loop.
 *
 * @param array pointer to strarray object
 * @param charptr pointer to loop variable
 */
#define strarray_for_each_element(array, charptr)                                                                      \
  for (charptr = (array)->value; charptr != NULL && (size_t)charptr < (size_t)(array)->value + (array)->length;        \
       charptr += strlen(charptr) + 1)

#endif
