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
 * 'gb_dat':
 * Pointer to buffer data.
 *
 * 'gb_buf':
 * Fixed-size array of 2 character pointers.
 * Element '[0]' points to the first character of the buffer.
 * Element '[1]' points to the last character of the buffer.
 *
 * Example:
 *  'gb_buf' = "Hello, world!"
 *  'gb_buf[0]' = 'H'
 *  'gb_buf[1]' = '!'
 *
 * 'gb_cur':
 * Pointer to cursor position in the 'gb_buf' (equals to 'gb_gap[0]').
 *
 * 'gb_gap':
 * Fixed-size array of 2 character pointers.
 * Element '[0]' points to the beginning of the buffer cursor. 
 * Element '[1]' points to the end of the buffer cursor.
 *
 * Example:
 *  'gb_buf' = "Hell[o-------], world!"
 *  'gb_buf[0]' = 'o'
 *  'gb_buf[1]' = '-' (last one in the square brackets)
 * */

struct gapbuf_s {

    char *gb_dat; /* 'gb_dat' - pointer to buffer data */

    char *gb_buf[2]; /* 'gb_buf':
                      * - [0] : begin
                      * - [1] : end
                      * */
    

    char *gb_cur; /* gb_cur' - pointer to cursor position in the 'gb_buf' (equals to 'gb_gap[0]') */

    char *gb_gap[2]; /* 'gb_gap':
                      * - [0] : begin
                      * - [1] : end
                      * */

};

/* create new buffer */
int gb_create(struct gapbuf_s *, const size_t);

/* release buffer */
int gb_free(struct gapbuf_s *);

/* clear buffer data */
int gb_clear(struct gapbuf_s *);

/* insert character into the buffer under the current cursor */
int gb_insert(struct gapbuf_s *, const char);

/* insert null-termianted c-string into the buffer under the current cursor */
int gb_insert_c_string(struct gapbuf_s *, const char *);

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
        .gb_dat = data,
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


/* release buffer */
int gb_free(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* clear 'pBuf' memory */
    memset(pBuf->gb_dat, 0, pBuf->gb_buf[1] - pBuf->gb_buf[0]);

    /* release 'pBuf' memory */
    free(pBuf->gb_dat);

    /* success */
    return (1);
}


/* clear buffer data */
int gb_clear(struct gapbuf_s *pBuf) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* re-alloc 'pBuf' into new, empty buffer */
    gb_free(pBuf);
    gb_create(pBuf, 1);

    /* success */
    return (1);
}


/* insert character into the buffer under the current cursor */
int gb_insert(struct gapbuf_s *pBuf, const char c) {
    /* null-check */
    if (!pBuf) { return (0); }

    /* get the 'gb_gap' length */
    size_t gaplen = pBuf->gb_gap[1] - pBuf->gb_gap[0];
    if (!gaplen) {
        /* get the 'gb_buf' length */
        size_t buflen = pBuf->gb_buf[1] - pBuf->gb_buf[0];

        /* resize 'pBuf' by 2x 'gb_buf' length */
        /* ... */
     }

    /* insert 'c' into 'gb_buf' under 'gb_cur' */
    *pBuf->gb_cur = c;
     pBuf->gb_cur++;
     pBuf->gb_gap[0]++;

    /* success */
    return (1);
}


/* insert null-termianted c-string into the buffer under the current cursor */
int gb_insert_c_string(struct gapbuf_s *pBuf, const char *pStr) {
    /* null-check */
    if (!pBuf) { return (0); }
    if (!pStr) { return (0); }

    /* O(N) insertion */
    while (gb_insert(pBuf, *pStr++));

    /* success */
    return (1);
}


/* extract single character from the buffer (backspace-style) */
int gb_extract(struct gapbuf_s *pBuf, char *pOut) {
    /* null-check */
    if (!pBuf) { return (0); }
    if (!pOut) { return (0); }

    /* space-check */
    size_t front = pBuf->gb_gap[0] - pBuf->gb_buf[0];
    if (front > 0) {
        *pOut = *pBuf->gb_cur;
        *pBuf->gb_cur = 0;
         pBuf->gb_cur--;
         pBuf->gb_gap[0]--;
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
        pBuf->gb_gap[0]--;
        pBuf->gb_gap[1]--;
        pBuf->gb_cur = pBuf->gb_gap[0];

        *pBuf->gb_gap[1] = *pBuf->gb_gap[0];
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
        *pBuf->gb_gap[0] = *pBuf->gb_gap[1];
        
        pBuf->gb_gap[0]++;
        pBuf->gb_gap[1]++;
        pBuf->gb_cur = pBuf->gb_gap[0];
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

