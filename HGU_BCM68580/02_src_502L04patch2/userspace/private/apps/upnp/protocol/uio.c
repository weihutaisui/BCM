/*
 *
 * <:copyright-BRCM:2012:proprietary:standard
 * 
 *    Copyright (c) 2012 Broadcom 
 *    All Rights Reserved
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 * 
*/


#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"

#define UIO_FILE     (1<<0)
#define UIO_STRING   (1<<2)
#define UIO_DYNAMIC  (1<<3)

/*
  
  Define UIO_TRACE to track the creation and deletion of uio
  structures when you suspect a leak.  When this symbol is defined,
  each UFILE structure will be tracked with a unique identifier.  That
  identifier will br printed when the structure is allocated and when
  it is freed.  If you see the identifiers grow much beyond 2 or 3,
  then you probably are leaking UFILE structures.

  #define UIO_TRACE 1
*/

#ifdef UIO_TRACE 
fd_set uio_fds;
#endif

struct _ufile {
#ifdef UIO_TRACE 
    int identifier;
#endif
    int flags;
    int bytes_written;
    union {
	FILE *fp;
	struct {
	    char *buffer;
	    int avail;
	} str;
    } io;
};

UFILE *uio_new()
{
    UFILE *up;
#ifdef UIO_TRACE
    int i;
#endif

    up = (UFILE *) cmsMem_alloc(sizeof(UFILE), ALLOC_ZEROIZE);
    if (up) {
#ifdef UIO_TRACE
	for (i = 0; i < 64; i++) {
	    if (!FD_ISSET(i, &uio_fds)) {
		up->index = i;
		FD_SET(up->index, &uio_fds);
		printf("%s %d\n", __FUNCTION__, up->index); 
		break;
	    }
	}
#endif
    }
    return up;
}

void uio_delete(UFILE *up)
{
#ifdef UIO_TRACE
    printf("%s %d\n", __FUNCTION__, up->index); 
    FD_CLR(up->index, &uio_fds);
#endif

    cmsMem_free(up);
}


UFILE *udopen(int fd)
{
    UFILE *up = NULL;
    FILE *fp;

    fp = fdopen(fd, "w");
    if (fp != NULL) {
	up = uio_new();
	if (up) {
	    up->flags |= UIO_FILE;
	    up->io.fp = fp;
	} else {
	    fclose(fp);
	}
    }
	       
    return up;
}


UFILE *ufopen(const char *fname)
{
    UFILE *up = NULL;
    FILE *fp;

    fp = fopen(fname, "w");
    if (fp != NULL) {
	up = uio_new();
	if (up) {
	    up->flags |= UIO_FILE;
	    up->io.fp = fp;
	} 
    }
	       
    return up;
}


UFILE *usopen(char *str, int maxlen)
{
    UFILE *up;

    up = uio_new();
    if (up) {
	up->flags |= UIO_STRING;
	if (str) {
	    up->io.str.buffer = str;
	    up->io.str.avail = maxlen;
	} else {
	    if (maxlen <= 0)
		maxlen = 1024;
	    up->flags |= UIO_DYNAMIC;
	    up->io.str.buffer = cmsMem_alloc(maxlen, ALLOC_ZEROIZE);
	    up->io.str.avail = maxlen;
	}
    } 
	       
    return up;
}


int uprintf(UFILE *up, char *fmt, ...)
{
    va_list ap;
    int len, n = 0;
    char *buf;

    va_start(ap, fmt);
    if (up->flags & UIO_STRING) {
	while (TRUE) {
	    buf = &up->io.str.buffer[up->bytes_written];
	    len = up->io.str.avail - up->bytes_written;
	    n = vsnprintf(buf, len, fmt, ap);
	    if (n < 0 || n > len) {
		/* we tried to overwrite the end of the buffer */
		if (up->flags & UIO_DYNAMIC) {
		    buf = cmsMem_realloc(up->io.str.buffer, up->io.str.avail + 256);
#ifdef UIO_TRACE
		    printf("cmsMem_alloc was %d, now %d\n", up->io.str.avail, up->io.str.avail + 256);
#endif
		    if (buf) {
			up->io.str.buffer = buf;
			up->io.str.avail += 256;
			continue;
		    }
		}
	    } else {
		up->bytes_written += n;
	    }
	    break;
	} /* end while */
    } else if (up->flags & UIO_FILE) {
	n = vfprintf(up->io.fp, fmt, ap);
	if (n > 0) {
	    up->bytes_written += n;
	}
    }
    va_end(ap);

    return n;
}


void uclose(UFILE *up)
{
    if (up->flags & UIO_FILE) {
	fclose(up->io.fp);
    } else if (up->flags & UIO_DYNAMIC) {
	cmsMem_free(up->io.str.buffer);
    }
    uio_delete(up);
}


int utell(UFILE *up)
{
    return up->bytes_written;
}


char *ubuffer(UFILE *up)
{
    char *buf = NULL;

    if (up->flags & UIO_STRING) {
	buf = up->io.str.buffer;
    } else {
	UPNP_ERROR(("Called %s on UFILE that does not use buffers.\n", __FUNCTION__));
    }
    return buf;
}


int ufileno(UFILE *up)
{
    int fd = 0;

    if (up->flags & UIO_FILE) {
	fd = fileno(up->io.fp);
    }  else {
	UPNP_ERROR(("Called %s on UFILE that does not use file descriptors.\n", __FUNCTION__));
    }
    return fd;
}


void uflush(UFILE *up)
{
    if (up->flags & UIO_FILE) {
	fflush(up->io.fp);
    } else if (up->flags & UIO_STRING) {
	up->bytes_written = 0;
    }
}
