/*
 *
 * Files within the tarball may be zlib streams or gzip files
 *
 * Copyright (C) 2016, Broadcom. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: tar.c 290048 2011-10-15 06:42:01Z $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include <zlib.h>

#define	ROUNDUP(x, y)		((((unsigned long)(x)+((y)-1))/(y))*(y))

/* Tar file constants  */
#ifndef MAJOR
#define MAJOR(dev) (((dev)>>8)&0xff)
#define MINOR(dev) ((dev)&0xff)
#endif

enum { NAME_SIZE = 100 }; /* because gcc won't let me use 'static const int' */

/* POSIX tar Header Block, from POSIX 1003.1-1990  */
struct TarHeader
{
                                /* byte offset */
	char name[NAME_SIZE];         /*   0-99 */
	char mode[8];                 /* 100-107 */
	char uid[8];                  /* 108-115 */
	char gid[8];                  /* 116-123 */
	char size[12];                /* 124-135 */
	char mtime[12];               /* 136-147 */
	char chksum[8];               /* 148-155 */
	char typeflag;                /* 156-156 */
	char linkname[NAME_SIZE];     /* 157-256 */
	char magic[6];                /* 257-262 */
	char version[2];              /* 263-264 */
	char uname[32];               /* 265-296 */
	char gname[32];               /* 297-328 */
	char devmajor[8];             /* 329-336 */
	char devminor[8];             /* 337-344 */
	char prefix[155];             /* 345-499 */
	char padding[12];             /* 500-512 (pad to exactly the TAR_BLOCK_SIZE) */
};
typedef struct TarHeader TarHeader;


/* A few useful constants */
#define TAR_MAGIC          "ustar"        /* ustar and a null */
#define TAR_VERSION        "  "           /* Be compatable with GNU tar format */
static const int TAR_MAGIC_LEN = 6;
static const int TAR_VERSION_LEN = 2;
static const int TAR_BLOCK_SIZE = 512;

/* A nice enum with all the possible tar file content types */
enum TarFileType 
{
	REGTYPE  = '0',            /* regular file */
	REGTYPE0 = '\0',           /* regular file (ancient bug compat)*/
	LNKTYPE  = '1',            /* hard link */
	SYMTYPE  = '2',            /* symbolic link */
	CHRTYPE  = '3',            /* character special */
	BLKTYPE  = '4',            /* block special */
	DIRTYPE  = '5',            /* directory */
	FIFOTYPE = '6',            /* FIFO special */
	CONTTYPE = '7',            /* reserved */
	GNULONGLINK = 'K',         /* GNU long (>100 chars) link name */
	GNULONGNAME = 'L',         /* GNU long (>100 chars) file name */
};
typedef enum TarFileType TarFileType;

/* This struct ignores magic, non-numeric user name, 
 * non-numeric group name, and the checksum, since
 * these are all ignored by BusyBox tar. */ 
struct TarInfo
{
	size_t           size;           /* Size of file */
	size_t           offset;         /* Current offset */
        unsigned char *  data;           /* Data */
};
typedef struct TarInfo TarInfo;

static TarHeader *tarball = NULL;

static void *
wcopy(void *dest, const void *src, size_t n)
{
	void *_dest = dest;
	unsigned int *dest_uint = (unsigned int *)dest;
	unsigned int *src_uint = (unsigned int *)src;

	while (n >= sizeof(unsigned int)) {
		*dest_uint++ = *src_uint++;
		n -= sizeof(unsigned int);
	}

	return _dest;
}

int
tar_fgetc(FILE *stream)
{
	TarInfo *info = (TarInfo *) stream;

	if (info->offset >= info->size)
		return EOF;
	else
		return (int) info->data[info->offset++];
}

int
tar_fclose(FILE *stream)
{
	TarInfo *info = (TarInfo *) stream;

	free(info->data);
	free(info);

	return 0;
}

/* check if the suggested address is a valid tarbase address */
int
tarbase_valid(char *ptr)
{
	TarHeader header;

	wcopy(&header, ptr, TAR_BLOCK_SIZE);
	if (strncmp(header.magic, TAR_MAGIC TAR_VERSION, TAR_MAGIC_LEN + TAR_VERSION_LEN))
		return 0;
	return 1;
}


#ifdef __CONFIG_LIBZ__

#define	GET_BYTE(b) \
{ \
	if (len >= srclen) \
		goto out; \
	b = src[len++]; \
}

static int
tar_unzip(unsigned char *dst, int *dstlen, unsigned char *src, int srclen)
{
	int len = 0;
	int rc = -1;

	int i, j;
	unsigned char magic[2];
	unsigned char method;
	unsigned char flags;
	unsigned char data;

	/* Check gzip magic */
	GET_BYTE(magic[0]);
	GET_BYTE(magic[1]);
	if (magic[0] != 0x1f || magic[1] != 0x8b)
		goto out;

	/* Check method */
	GET_BYTE(method);
	if (method != 8)
		goto out;

	/* Check flags */
	GET_BYTE(flags);
	if (flags & 0xE0)
		goto out;

	/* Skip 6 bytes for time stamp, flag and OS info */
	len += 6;
	if (len > srclen)
		goto out;

	/* Skip extra fields */
	if (flags & 0x04) {
		GET_BYTE(i);
		GET_BYTE(j);
		i += (j << 8);

		len += i;
		if (len > srclen)
			goto out;
	}

	/* Skip original name */
	if (flags & 0x08) {
		while (1) {
			GET_BYTE(data);
			if (data == 0)
				break;
		}
	}

	/* Skip comment */
	if (flags & 0x10) {
		while (1) {
			GET_BYTE(data);
			if (data == 0)
				break;
		}
	}

	/* Skip head crc */
	if (flags & 0x02) {
		len += 2;
		if (len > srclen)
			goto out;
	}

	/* Call zlib uncompress */
	src += len;
	srclen -= len;

	/* Shell we check crc32 ? */
	return uncompress(dst, (long *)dstlen, src, srclen);

out:
	return rc;
}
#endif /* __CONFIG_LIBZ__ */

FILE *
tar_fopen(const char *path, const char *mode)
{
	char *ptr = (char *) tarball;
	TarHeader header;
	TarInfo *info = NULL;
	unsigned char *src = NULL, *dst = NULL;
	int size;
	int srclen;

	if (!tarball)
		return NULL;

	for (;;) {
	    	if (!tarbase_valid(ptr)) {
			fprintf(stderr, "tar_fopen: bad magic %s\n", header.magic);
			return NULL;
		}
		wcopy(&header, ptr, TAR_BLOCK_SIZE);
		ptr += TAR_BLOCK_SIZE;
		switch (header.typeflag) {
		case REGTYPE:
		case REGTYPE0:
			size = strtol(header.size, NULL, 8);
			if (strcmp(header.name, path)) {
				ptr += ROUNDUP(size, TAR_BLOCK_SIZE);
				continue;
			}

			/* Allocate private state */
			if (!(info = malloc(sizeof(TarInfo))))
				goto error;
			info->offset = 0;

			/* Copy file to memory */
			srclen = ROUNDUP(size, sizeof(unsigned int));
			if (!(src = malloc(srclen))) {
				perror("malloc");
				goto error;
			}
			wcopy(src, ptr, srclen);

#ifdef __CONFIG_LIBZ__
			/* Check for gzip header */
			if (src[0] == 0x1f && src[1] == 0x8b) {
				int dstlen;

				/* Get original file size */
				info->size = src[size - 1] << 24;
				info->size += src[size - 2] << 16;
				info->size += src[size - 3] << 8;
				info->size += src[size - 4];

				dstlen = ROUNDUP(info->size, sizeof(unsigned int));
				if (!(dst = malloc(dstlen))) {
					perror("malloc");
					goto error;
				}

				/* Inflate */
				if (tar_unzip(dst, &dstlen, src, size)) {
					fprintf(stderr, "tar_fopen: %s: error during decompression\n", path);
					goto error;
				}
				free(src);
				info->data = dst;
			}
			else
#endif /* __CONFIG_LIBZ__ */
			{
				info->size = size;
				info->data = src;
			}
			return (FILE *) info;
		case DIRTYPE:
			continue;
		default:
			fprintf(stderr, "tar_fopen: unhandled type %c\n", header.typeflag);
			goto error;
		}
	}

 error:
	if (dst)
		free(dst);
	if (src)
		free(src);
	if (info)
		free(info);
	return NULL;
}		

void
tar_base(unsigned long adrs)
{
	tarball = (TarHeader *) adrs;
}

#ifdef linux
int
main(int argc, char *argv[])
{
	char *buf;
	size_t offset = 0, len = 0, ret;
	FILE *fp;
	int c;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <path>\n", argv[0]);
		return -1;
	}

	do {
		if (offset >= len) {
			len += 1000;
			if (!(buf = realloc(tarball, len))) {
				perror("realloc");
				break;
			}
			tarball = (TarHeader *) buf;
		}
		ret = fread(&buf[offset], 1, len - offset, stdin);
		offset += ret;
	} while (ret);

	if ((fp = tar_fopen(argv[1], "r"))) {
		while ((c = tar_fgetc(fp)) != EOF)
			putchar(c);
		tar_fclose(fp);
	}

	if (tarball)
		free(tarball);
	return 0;
}
#endif
