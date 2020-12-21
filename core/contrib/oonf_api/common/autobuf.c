
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

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include "common/autobuf.h"
#include "common/string.h"

#ifdef RIOT_VERSION
static size_t getpagesize(void)
{
  return 512;
}
#endif

/**
 * @param val original size
 * @param pow2 power of 2 (1024, 4096, ...)
 * @return multiple of pow2 which larger or equal val
 */
static INLINE size_t
ROUND_UP_TO_POWER_OF_2(size_t val, size_t pow2) {
  return (val + pow2 - 1) & ~(pow2 - 1);
}

static int _autobuf_enlarge(struct autobuf *autobuf, size_t new_size);
static void _print_hexline(struct autobuf *out, const void *buffer, size_t length);
static int _vappendf(struct autobuf *autobuf, const char *format, va_list ap, va_list ap2)
  __attribute__((format(printf, 2, 0)));

/**
 * Initialize an autobuffer and allocate a chunk of memory
 * @param autobuf pointer to autobuf object
 * @return -1 if an out-of-memory error happened, 0 otherwise
 */
int
abuf_init(struct autobuf *autobuf) {
  autobuf->_len = 0;
  autobuf->_buf = calloc(1, getpagesize());
  if (autobuf->_buf == NULL) {
    autobuf->_error = true;
    return -1;
  }
  autobuf->_total = getpagesize();
  autobuf->_error = false;
  return 0;
}

/**
 * Free all currently used memory of an autobuffer.
 * The buffer can still be used afterwards !
 * @param autobuf pointer to autobuf object
 */
void
abuf_free(struct autobuf *autobuf) {
  free(autobuf->_buf);
  memset(autobuf, 0, sizeof(*autobuf));
}

/**
 * vprintf()-style function that appends the output to an autobuffer
 * @param autobuf pointer to autobuf object
 * @param format printf format string
 * @param ap variable argument list pointer
 * @return -1 if an out-of-memory error happened,
 *   otherwise it returns the number of written characters
 *   (excluding the \0)
 */
int
abuf_vappendf(struct autobuf *autobuf, const char *format, va_list ap) {
  int result;
  va_list ap2;

  if (!autobuf) {
    return 0;
  }

  va_copy(ap2, ap);
  result = _vappendf(autobuf, format, ap, ap2);
  va_end(ap2);

  return result;
}

/**
 * printf()-style function that appends the output to an autobuffer.
 * The function accepts a variable number of arguments based on the format string.
 * @param autobuf pointer to autobuf object
 * @param fmt printf format string
 * @return -1 if an out-of-memory error happened,
 *   otherwise it returns the number of written characters
 *   (excluding the \0)
 */
int
abuf_appendf(struct autobuf *autobuf, const char *fmt, ...) {
  int rc;
  va_list ap;

  if (autobuf == NULL)
    return 0;

  va_start(ap, fmt);
  rc = abuf_vappendf(autobuf, fmt, ap);
  va_end(ap);
  return rc;
}

/**
 * Appends a null-terminated string to an autobuffer
 * @param autobuf pointer to autobuf object
 * @param s string to append to the buffer
 * @return -1 if an out-of-memory error happened,
 *   otherwise it returns the number of written characters
 *   (excluding the \0)
 */
int
abuf_puts(struct autobuf *autobuf, const char *s) {
  size_t len;

  if (autobuf == NULL || s == NULL)
    return 0;

  len = strlen(s);
  if (_autobuf_enlarge(autobuf, autobuf->_len + len + 1) < 0) {
    return -1;
  }
  strcpy(autobuf->_buf + autobuf->_len, s);
  autobuf->_len += len;
  return len;
}

/**
 * Appends a formatted time string to an autobuffer
 * @param autobuf pointer to autobuf object
 * @param format strftime() format string
 * @param tm pointer to time data
 * @return -1 if an out-of-memory error happened, 0 otherwise
 */
int
abuf_strftime(struct autobuf *autobuf, const char *format, const struct tm *tm) {
  size_t rc;

  if (autobuf == NULL)
    return 0;

  rc = strftime(autobuf->_buf + autobuf->_len, autobuf->_total - autobuf->_len, format, tm);
  if (rc == 0) {
    /* we had an error! Probably the buffer too small. So we add some bytes. */
    if (_autobuf_enlarge(autobuf, autobuf->_total + getpagesize()) < 0) {
      autobuf->_buf[autobuf->_len] = '\0';
      return -1;
    }

    rc = strftime(autobuf->_buf + autobuf->_len, autobuf->_total - autobuf->_len, format, tm);
    if (rc == 0) {
      /* make sure we are null-terminated */
      autobuf->_buf[autobuf->_len] = 0;

      return -1;
    }
  }

  /* add data to length field */
  autobuf->_len += rc;
  return rc;
}

/**
 * Copies a binary buffer to the end of an autobuffer.
 * @param autobuf pointer to autobuf object
 * @param p pointer to memory block to be copied
 * @param len length of memory block
 * @return -1 if an out-of-memory error happened, 0 otherwise
 */
int
abuf_memcpy(struct autobuf *autobuf, const void *p, const size_t len) {
  if (autobuf == NULL || len == 0)
    return 0;

  if (_autobuf_enlarge(autobuf, autobuf->_len + len) < 0) {
    return -1;
  }
  memcpy(autobuf->_buf + autobuf->_len, p, len);
  autobuf->_len += len;

  /* null-terminate autobuf */
  autobuf->_buf[autobuf->_len] = 0;

  return 0;
}

/**
 * Append a memory block to the beginning of an autobuffer.
 * @param autobuf pointer to autobuf object
 * @param p pointer to memory block to be copied as a prefix
 * @param len length of memory block
 * @return -1 if an out-of-memory error happened, 0 otherwise
 */
int
abuf_memcpy_prepend(struct autobuf *autobuf, const void *p, const size_t len) {
  if (autobuf == NULL || len == 0)
    return 0;

  if (_autobuf_enlarge(autobuf, autobuf->_len + len) < 0) {
    return -1;
  }
  memmove(&autobuf->_buf[len], autobuf->_buf, autobuf->_len);
  memcpy(autobuf->_buf, p, len);
  autobuf->_len += len;

  /* null-terminate autobuf */
  autobuf->_buf[autobuf->_len] = 0;

  return 0;
}

/**
 * Remove a prefix from an autobuffer. This function can be used
 * to create an autobuffer based fifo.
 * @param autobuf pointer to autobuf object
 * @param len number of bytes to be removed
 */
void
abuf_pull(struct autobuf *autobuf, size_t len) {
  char *p;
  size_t newsize;

  if (autobuf == NULL || len == 0)
    return;

  if (len != autobuf->_len) {
    memmove(autobuf->_buf, &autobuf->_buf[len], autobuf->_len - len);
  }
  autobuf->_len -= len;

  if (autobuf->_len + getpagesize() > autobuf->_total) {
    /* only reduce buffer size if difference is larger than a chunk */
    return;
  }
  newsize = autobuf->_total -= getpagesize();
  if (newsize < (size_t)getpagesize()) {
    newsize = getpagesize();
  }

  /* generate smaller buffer */
  p = realloc(autobuf->_buf, newsize);
  if (p == NULL) {
    /* keep the longer buffer if we cannot get a smaller one */
    return;
  }
  autobuf->_buf = p;
  autobuf->_total = newsize;
  return;
}

/**
 * Print a hexdump of a buffer to an autobuf and prepends a prefix string
 * to each line.
 * @param out output buffer
 * @param prefix string to prepend to each line
 * @param buffer buffer to be hexdumped
 * @param length length of buffer in bytes
 */
void
abuf_hexdump(struct autobuf *out, const char *prefix, const void *buffer, size_t length) {
  const uint8_t *buf;
  size_t j, l;

  buf = buffer;

  for (j = 0; j < length; j += 32) {
    abuf_appendf(out, "%s%04zx:", prefix, j);

    l = length - j;
    if (l > 32) {
      l = 32;
    }
    _print_hexline(out, &buf[j], l);
    abuf_puts(out, "\n");
  }
}

/**
 * Print a line for a hexdump
 * @param out output buffer
 * @param buffer buffer to be hexdumped
 * @param length length of buffer in bytes
 */
static void
_print_hexline(struct autobuf *out, const void *buffer, size_t length) {
  size_t i;
  const uint8_t *buf = buffer;

  for (i = 0; i < 32; i++) {
    if ((i & 3) == 0) {
      abuf_puts(out, " ");
    }
    if (i < length) {
      abuf_appendf(out, "%02x", (int)(buf[i]));
    }
    else {
      abuf_puts(out, "  ");
    }
  }

  abuf_puts(out, " ");
  for (i = 0; i < length; i++) {
    if (buf[i] >= 32 && buf[i] < 127) {
      abuf_appendf(out, "%c", buf[i]);
    }
    else {
      abuf_puts(out, ".");
    }
  }
}

/**
 * Enlarge an autobuffer if necessary
 * @param autobuf pointer to autobuf object
 * @param new_size number of bytes necessary in autobuffer
 * @return -1 if an out-of-memory error happened, 0 otherwise
 */
static int
_autobuf_enlarge(struct autobuf *autobuf, size_t new_size) {
  char *p;
  size_t roundUpSize;

  new_size++;
  if (new_size > autobuf->_total) {
    roundUpSize = ROUND_UP_TO_POWER_OF_2(new_size + 1, getpagesize());
    p = realloc(autobuf->_buf, roundUpSize);
    if (p == NULL) {
#ifdef WIN32
      WSASetLastError(ENOMEM);
#else
      errno = ENOMEM;
#endif
      autobuf->_error = true;
      return -1;
    }
    autobuf->_buf = p;

    memset(&autobuf->_buf[autobuf->_total], 0, roundUpSize - autobuf->_total);
    autobuf->_total = roundUpSize;
  }
  return 0;
}

/**
 * vprintf()-style function that appends the output to an autobuffer
 * @param autobuf pointer to autobuf object
 * @param format printf format string
 * @param ap variable argument list pointer
 * @param ap2 copy of variable argument list pointer
 * @return -1 if an out-of-memory error happened,
 *   otherwise it returns the number of written characters
 *   (excluding the \0)
 */
static int
_vappendf(struct autobuf *autobuf, const char *format, va_list ap, va_list ap2) {
  int rc;
  size_t min_size;

  if (autobuf == NULL)
    return 0;

  rc = vsnprintf(autobuf->_buf + autobuf->_len, autobuf->_total - autobuf->_len, format, ap);
  if (rc < 0) {
    autobuf->_error = true;
    return rc;
  }
  min_size = autobuf->_len + (size_t)rc;
  if (min_size >= autobuf->_total) {
    if (_autobuf_enlarge(autobuf, min_size) < 0) {
      autobuf->_buf[autobuf->_len] = '\0';
      return -1;
    }
    rc = vsnprintf(autobuf->_buf + autobuf->_len, autobuf->_total - autobuf->_len, format, ap2);
    if (rc < 0) {
      autobuf->_error = true;
      return rc;
    }
  }
  autobuf->_len = min_size;
  return rc;
}
