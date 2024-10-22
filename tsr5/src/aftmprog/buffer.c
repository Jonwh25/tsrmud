/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
/***************************************************************************
 *            Implementation of a dynamically expanding buffer.            *
 *                                                                         *
 *     Inspired by Russ Taylor's <rtaylor@efn.org> buffers in ROM 2.4b2.   *
 *    The buffer is primarily used for null-terminated character strings.  *
 *                                                                         *
 *  A buffer is allocated with buffer_new, written to using buffer_strcat, *
 *  cleared (if needed) using buffer_clear and free'ed using buffer_free.  *
 *                                                                         *
 * If BUFFER_DEBUG is defined, the buffer_strcat call is defined as having *
 * 2 extra parameters, __LINE__ and __FILE__. These are then saved         *
 * to the bug file if an overflow occurs.                                  *
 *                                                                         *
 * Erwin S. Andreasen <erwin@andreasen.org>                                *
 *                                                                         *
 * OVERVIEW: This is a dynamic string buffer, which expands as needed,     *
 * and never overflows. In addition, the string length is saved, so it     *
 * is faster than doing strcats()s.                                        *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include "merc.h"
#include "interp.h"
#include "buffer.h"


#define EMEM_SIZE -1		/* find_mem_size returns this when block is too large */
#define NUL '\0'

extern const int rgSizeList[MAX_MEM_LIST];

/* Find in rgSizeList a memory size at least this long */
int find_mem_size(int min_size)
{
    int i;

    for (i = 0; i < MAX_MEM_LIST; i++)
	if (rgSizeList[i] >= min_size)
	    return rgSizeList[i];

    /* min_size is bigger than biggest allowable size! */

    return EMEM_SIZE;
}

/* Create a new buffer, of at least size bytes */

#ifndef BUFFER_DEBUG		/* no debugging */
BUFFER *__buffer_new(int min_size)
#else				/* debugging - expect filename and line */
BUFFER *__buffer_new(int min_size, const char *file, unsigned line)
#endif

{
    int size;
    BUFFER *buffer;
    char buf[200];		/* for the bug line */

    size = find_mem_size(min_size);

    if (size == EMEM_SIZE) {
#ifdef BUFFER_DEBUG
	sprintf(buf, "Buffer size too big: %d bytes (%s:%u).", min_size, file, line);
#else
	sprintf(buf, "Buffer size too big: %d bytes.", min_size);
#endif

	bug(buf, 0);
	abort();
    }
    buffer = alloc_mem(sizeof(BUFFER));

    buffer->size = size;
    buffer->data = alloc_mem(size);
    buffer->overflowed = FALSE;

    buffer->len = 0;

    return buffer;
}				/* __buf_new */

/* Add a string to a buffer. Expand if necessary */

#ifndef BUFFER_DEBUG		/* no debugging */
void __buffer_strcat(BUFFER * buffer, const char *text)
#else				/* debugging - expect filename and line */
void __buffer_strcat(BUFFER * buffer, const char *text, const char *file, unsigned line)
#endif

{
    int new_size;
    int text_len;
    char *new_data;
    char buf[200];

    if (buffer->overflowed)	/* Do not attempt to add anymore if buffer is already overflowed */
	return;

    if (!text)			/* Adding NULL string ? */
	return;

    text_len = strlen(text);

    if (text_len == 0)		/* Adding empty string ? */
	return;

    /* Will the combined len of the added text and the current text exceed our buffer? */

    if ((text_len + buffer->len + 1) > buffer->size) {	/* expand? */
	new_size = find_mem_size(buffer->size + text_len + 1);
	if (new_size == EMEM_SIZE) {	/* New size too big ? */
#ifdef BUFFER_DEBUG
	    sprintf(buf, "Buffer overflow, wanted %d bytes (%s:%u).", text_len + buffer->len, file, line);
#else
	    sprintf(buf, "Buffer overflow, wanted %d bytes.", text_len + buffer->len);
#endif
	    bug(buf, 0);
	    buffer->overflowed = TRUE;
	    return;
	}
	/* Allocate the new buffer */

	new_data = alloc_mem(new_size);

	/* Copy the current buffer to the new buffer */

	memcpy(new_data, buffer->data, buffer->len);
	free_mem(buffer->data, buffer->size);
	buffer->data = new_data;
	buffer->size = new_size;

    }				/* if */
    memcpy(buffer->data + buffer->len, text, text_len);		/* Start copying */
    buffer->len += text_len;	/* Adjust length */
    buffer->data[buffer->len] = NUL;	/* Null-terminate at new end */

}				/* __buf_strcat */


/* Free a buffer */
void buffer_free(BUFFER * buffer)
{
    /* Free data */
    free_mem(buffer->data, buffer->size);

    /* Free buffer */

    free_mem(buffer, sizeof(BUFFER));
}

/* Clear a buffer's contents, but do not deallocate anything */

void buffer_clear(BUFFER * buffer)
{
    buffer->overflowed = FALSE;
    buffer->len = 0;
}

/* print stuff, append to buffer. safe. */
int bprintf(BUFFER * buffer, char *fmt,...)
{
    char buf[MSL];
    va_list va;
    int res;

    va_start(va, fmt);
    res = vsnprintf(buf, MSL, fmt, va);
    va_end(va);

    if (res >= MSL - 1) {
	buf[0] = NUL;
	bugf("Overflow when printing string %s", fmt);
    } else
	buffer_strcat(buffer, buf);

    return res;
}
