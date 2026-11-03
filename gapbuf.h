#if defined (__cplusplus)
# pragma once
#endif
#
#if !defined (_gapbuf_h_)
# define _gapbuf_h_ 1
#
# /* Define platform macros */
# if defined (__linux__) || \
     defined (__gnu_linux__)
#  define GAPBUF_PLATFORM "linux"
#  define GAPBUF_PLATFORM_LINUX 1
#
# elif defined (__APPLE__) || \
       defined (__MACH__)
#  define GAPBUF_PLATFORM "apple"
#  define GAPBUF_PLATFORM_APPLE 1
#
# elif defined (_WIN16)      || \
       defined (_WIN32)      || \
       defined (_WIN64)      || \
       defined (__MINGW32__) || \
       defined (__MINGW64__) || \
       defined (__CYGWIN__)  || \
       defined (__MSYS__)
#  define GAPBUF_PLATFORM "windows"
#  define GAPBUF_PLATFORM_WINDOWS 1
#
# elif defined (__bsdi__)       || \
       defined (__FreeBSD__)    || \
       defined (__NetBSD__)     || \
       defined (__DragonFly__)  || \
       defined (__MidnightBSD__)
#  define GAPBUF_PLATFORM "bsd"
#  define GAPBUF_PLATFORM_BSD 1
#
# else
#  error /* No valid platform found */
# endif
#
# include <stddef.h>
# include <stdint.h>
#
#
# if defined (__cplusplus)

extern "C" {

# endif /* __cplusplus */

/* 'struct gapbuf_s':
 * This structure defines memory layout of gap-buffer data structure.
 *
 * 'gb_buf':
 * Fixed-size array of 2 character pointers.
 * - '[0]' : first character of the buffer.
 * - '[1]' : last character of the buffer.
 *
 * 'gb_gap':
 * Fixed-size array of 2 character pointers.
 * - '[0]' : beginning of the buffer cursor. 
 * - '[1]' : end of the buffer cursor.
 *
 * Example 01.:
 * Imagine the gap-buffer of length: 16. We insert a new string: "Hello, world!".
 * The layout would look like this:
 * 'gb_buf' : [0]                                          [1]
 * 'gb_gap' :                                        [0]   [1]
 *   data   : [H][e][l][l][o][,][ ][w][o][r][l][d][!][ ][ ][ ]
 *
 * Example 02:
 * The same scenario as before; length: 16, string: "Hello, world!".
 * But this time, the cursor is shifted by 2 places to the LEFT.
 * The layout would look like this:
 * 'gb_buf' : [0]                                          [1]
 * 'gb_gap' :                                  [0]   [1]
 *   data   : [H][e][l][l][o][,][ ][w][o][r][l][ ][ ][ ][d][!]
 * */

struct gapbuf_s {

    char *gb_buf[2]; /* 'gb_buf':
                      * - [0] : begin
                      * - [1] : end
                      * */
    

    char *gb_gap[2]; /* 'gb_gap':
                      * - [0] : begin
                      * - [1] : end
                      * */

};

/* create new buffer */
int gb_create(struct gapbuf_s *, const size_t);

/* increase the size of the buffer */
int gb_increase(struct gapbuf_s *, const size_t);

/* decrease the size of the buffer */
int gb_decrease(struct gapbuf_s *, const size_t);

/* release buffer */
int gb_free(struct gapbuf_s *);

/* clear buffer data */
int gb_clear(struct gapbuf_s *);

/* insert character into the buffer under the current cursor */
int gb_insert(struct gapbuf_s *, const char);

/* insert null-termianted c-string into the buffer under the current cursor */
int gb_insert_cstr(struct gapbuf_s *, const char *);

/* frontal extraction of character from the buffer (backspace-style) */
int gb_extract_front(struct gapbuf_s *, char *);

/* move the cursor up */
int gb_up(struct gapbuf_s *);

/* move the cursor down */
int gb_down(struct gapbuf_s *);

/* move the cursor left */
int gb_left(struct gapbuf_s *);

/* move the cursor right */
int gb_right(struct gapbuf_s *);

/* convert the buffer to null-terminated c-string */
int gb_to_cstr(struct gapbuf_s *, char **);

# if defined (__cplusplus)

}

# endif /* __cplusplus */
#
#endif /* _gapbuf_h_ */
#
#if defined (GAPBUF_IMPLEMENTATION)
# if !defined (_gapbuf_impl_h_)
#  define _gapbuf_impl_h_ 1
#
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#
#  if defined (__cplusplus)

extern "C" {

#  endif /* __cplusplus */

/* create new buffer */
int gb_create(struct gapbuf_s *pBuf, const size_t size) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* size-check */
    if (!size) { return (0); }

    /* alloc new buffer data */
    char *data = (char *) calloc(size, sizeof(char));
    if (!data) { return (0); }

    /* return new 'pBuf' */
    *pBuf = (struct gapbuf_s) {
        .gb_buf = {
            [0] = data,
            [1] = data + size
        },
        .gb_gap = {
            [0] = data,
            [1] = data + size
        }
    };

    /* success */
    return (1);
}


/* increase the size of the buffer */
int gb_increase(struct gapbuf_s *pBuf, const size_t size) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* size-check */
    if (!size) { return (0); }

    /* get the length of the 'pBuf's used space */
    size_t front = pBuf->gb_gap[0] - pBuf->gb_buf[0],
            back = pBuf->gb_buf[1] - pBuf->gb_gap[1];
    size_t length = front + back;

    /* skip the operation if 'size' is less-or-equal to 'length' */
    if (size <= length) { return (1); }

    /* alloc. new 'tmp' buffer */
    char *tmp = (char *) calloc(size, sizeof(char));
    if (!tmp) { return (0); }

    /* copy front characters from 'pBuf' to 'tmp' */
    for (size_t i = 0; i < front; i++) {
        tmp[i] = *(pBuf->gb_buf[0] + i);
    }

    /* copy back characters from 'pBuf' to 'tmp' */
    for (size_t i = 0; i < back; i++) {
        tmp[size - i] = *(pBuf->gb_buf[1] - i);
    }

    /* release 'pBuf' and assign new data */
    gb_free(pBuf);
    *pBuf = (struct gapbuf_s) {
        .gb_buf = {
            [0] = tmp,
            [1] = tmp + size
        },
        .gb_gap = {
            [0] = tmp + front,
            [1] = tmp + size - back
        }
    };

    /* success */
    return (1);
}


/* decrease the size of the buffer */
int gb_decrease(struct gapbuf_s *pBuf, const size_t size) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* size-check */
    if (!size) { return (0); }

    /* get the length of the 'pBuf's used space */
    size_t front = pBuf->gb_gap[0] - pBuf->gb_buf[0],
            back = pBuf->gb_buf[1] - pBuf->gb_gap[1];
    size_t length = front + back;

    /* skip the operation if 'size' is greater-or-equal to 'length' */
    if (size >= length) { return (1); }

    /* alloc. new 'tmp' buffer */
    char *tmp = (char *) calloc(size, sizeof(char));
    if (!tmp) { return (0); }

    /* copy front characters from 'pBuf' to 'tmp' */
    for (size_t i = 0; i < front; i++) {
        tmp[i] = *(pBuf->gb_buf[0] + i);
    }

    /* copy back characters from 'pBuf' to 'tmp' */
    for (size_t i = 0; i < back; i++) {
        tmp[size - i] = *(pBuf->gb_buf[1] - i);
    }

    /* release 'pBuf' and assign new data */
    gb_free(pBuf);
    *pBuf = (struct gapbuf_s) {
        .gb_buf = {
            [0] = tmp,
            [1] = tmp + size
        },
        .gb_gap = {
            [0] = tmp + front,
            [1] = tmp + size - back
        }
    };

    /* success */
    return (1);
}


/* release buffer */
int gb_free(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* clear 'pBuf' memory */
    memset(pBuf->gb_buf[0], 0, pBuf->gb_buf[1] - pBuf->gb_buf[0]);

    /* release 'pBuf' memory */
    free(pBuf->gb_buf[0]);

    /* success */
    return (1);
}


/* clear buffer data */
int gb_clear(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* re-alloc 'pBuf' into new, empty buffer */
    gb_free(pBuf); gb_create(pBuf, 1);

    /* success */
    return (1);
}


/* insert character into the buffer under the current cursor */
int gb_insert(struct gapbuf_s *pBuf, const char c) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* increase if necessary */
    if (pBuf->gb_gap[0] == pBuf->gb_gap[1]) {
        gb_increase(pBuf, (pBuf->gb_buf[1] - pBuf->gb_buf[0]) * 2);
    }

    /* insert 'c' into 'gb_buf' under 'gb_gap[0]' */
    *(pBuf->gb_gap[0]++) = c;

    /* success */
    return (1);
}


/* insert null-termianted c-string into the buffer under the current cursor */
int gb_insert_cstr(struct gapbuf_s *pBuf, const char *pStr) {
    /* null-check */
    if (!pBuf) { return (0); }
    if (!pStr) { return (0); }

    /* O(N) insertion */
    for (size_t i = 0; pStr[i]; i++) {
        gb_insert(pBuf, pStr[i]);
    }

    /* success */
    return (1);
}


/* extract single character from the buffer (backspace-style) */
int gb_backspace(struct gapbuf_s *pBuf, char *pOut) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* space-check */
    size_t front = pBuf->gb_gap[0] - pBuf->gb_buf[0];
    if (front > 0) {
        /* return only if possible */
        if (pOut) { *pOut = *pBuf->gb_gap[0]; }

        /* clear and move */
        *(pBuf->gb_gap[0])-- = 0;
    } 
   
    /* decrease if necessary */
    size_t gsize = pBuf->gb_gap[1] - pBuf->gb_gap[0];
    size_t bsize = pBuf->gb_buf[1] - pBuf->gb_buf[0];
    if (gsize < bsize / 4) {
        gb_decrease(pBuf, bsize / 2);
    }

    /* success */
    return (1);
}


/* move the cursor up */
int gb_up(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* success */
    return (1);
}


/* move the cursor down */
int gb_down(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* success */
    return (1);
}


/* move the cursor left */
int gb_left(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }
    
    /* space-check */
    size_t front = pBuf->gb_gap[0] - pBuf->gb_buf[0];
    if (front > 0) {
        *(pBuf->gb_gap[1]--) = *(--pBuf->gb_gap[0]); *(pBuf->gb_gap[0]) = 0;
    }

    /* success */
    return (1);
}


/* move the cursor right */
int gb_right(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }
    
    /* space-check */
    size_t back = pBuf->gb_buf[1] - pBuf->gb_gap[1];
    if (back > 0) {
        *(pBuf->gb_gap[0]++) = *(++pBuf->gb_gap[1]); *(pBuf->gb_gap[1]) = 0;
    }

    /* success */
    return (1);
}


/* convert the buffer to null-terminated c-string */
int gb_to_cstr(struct gapbuf_s *pBuf, char **pStr) {
    /* null-check */
    if (!pBuf) { return (0); }
    if (!pStr) { return (0); }

    /* pivots */
    char *sb = pBuf->gb_buf[0], *eb = pBuf->gb_buf[1];
    char *sg = pBuf->gb_gap[0], *eg = pBuf->gb_gap[1];

    /* create c-string */
    char *tmp = (char *) calloc((eb - sb) + (eg - sg) + 1, sizeof(char));
    if (!tmp) { return (0); }

    /* return 'tmp' to 'pStr' */
    *pStr = tmp;

    /* copy 'pBuf' to 'tmp' */
    while (sb < sg) {
        *tmp++ = *sb++;
    }

    eg++;
    while (eg <= eb) {
        *tmp++ = *eg++;
    }

    /* success */
    return (1);
}

#  if defined (__cplusplus)

}

#  endif /* __cplusplus */
#
# endif /* _gapbuf_impl_h_ */
#endif /* GAPBUF_IMPLEMENTATION */

