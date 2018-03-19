/*
 * Definitions for Abstract Syntax Notation One, ASN.1
 * As defined in ISO/IS 8824 and ISO/IS 8825
 *
 *
 */
/***********************************************************
 <:label-BRCM:1988:NONE:standard
 
 :>
    Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#ifndef	_ASN1_H_
#define	_ASN1_H_

#ifdef BUILD_SNMP_DEBUG
#include "snmpDebug.h"
#else
#define SNMP_DEBUG(...)
#endif

/* changed by H.H. */
typedef struct {
    unsigned long high;
    unsigned long low;
} Counter64;

/* added by H.H. */
#ifndef SMALL_SUBIDS
typedef unsigned long  oid;
#define MAX_SUBID   0xFFFFFFFF
#else
typedef unsigned char  oid;
#define MAX_SUBID   0xFF
#endif

#define ASN_BOOLEAN     (0x01)
#define ASN_INTEGER     (0x02)
#define ASN_BIT_STR     (0x03)
#define ASN_OCTET_STR       (0x04)
#define ASN_NULL        (0x05)
#define ASN_OBJECT_ID       (0x06)
#define ASN_SEQUENCE        (0x10)

#define ASN_UNIVERSAL       (0x00)
#define ASN_APPLICATION     (0x40)
#define ASN_CONTEXT     (0x80)
#define ASN_PRIVATE     (0xC0)

#define ASN_PRIMITIVE       (0x00)
#define ASN_CONSTRUCTOR     (0x20)

#define ASN_LONG_LEN        (0x80)
#define ASN_EXTENSION_ID    (0x1F)
#define ASN_BIT8        (0x80)

#define IS_CONSTRUCTOR(byte)    ((byte) & ASN_CONSTRUCTOR)
#define IS_EXTENSION_ID(byte)   (((byte) & ASN_EXTENSION_ID) == ASN_EXTENSION_ID)

unsigned char   *asn_parse_int(unsigned char *data, int *datalength,
    unsigned char *type, long *intp, int intsize);
unsigned char   *asn_build_int(unsigned char *data, int *datalength,
    unsigned char type, long *intp, int intsize);
unsigned char   *asn_parse_unsigned_int(unsigned char *data, int *datalength,
    unsigned char *type, unsigned long *intp, int intsize);
unsigned char   *asn_build_unsigned_int(unsigned char *data, int *datalength,
    unsigned char type, unsigned long *intp, int intsize);
unsigned char   *asn_parse_string(unsigned char *data, int *datalength,
    unsigned char *type, unsigned char *string, int *strlength);
unsigned char   *asn_build_string(unsigned char *data, int *datalength,
    unsigned char type, unsigned char *string, int strlength);
unsigned char   *asn_parse_header(unsigned char *data, int *datalength, unsigned char *type);
unsigned char   *asn_build_header(unsigned char *data, int *datalength, unsigned char type, int length);
unsigned char   *asn_build_sequence(unsigned char *data, int *datalength, unsigned char type, int length);
unsigned char   *asn_parse_length(unsigned char *data, unsigned long *datalength);
unsigned char   *asn_build_length(unsigned char *data, int *datalength, int length);
unsigned char   *asn_parse_objid(unsigned char  *data, int *datalength, unsigned char *type, oid *objid, int *objidlength);
unsigned char   *asn_build_objid(unsigned char  *data, int *datalength, unsigned char type, oid *objid, int objidlength);
unsigned char   *asn_parse_null(unsigned char *data, int *datalength, unsigned char *type);
unsigned char   *asn_build_null(unsigned char *data, int *datalength, unsigned char type);
unsigned char   *asn_parse_bitstring(unsigned char *data, int *datalength,
    unsigned char *type, unsigned char *string, int *strlength);
unsigned char   *asn_build_bitstring(unsigned char *data, int *datalength,
    unsigned char type, unsigned char *string, int strlength);
unsigned char   *asn_parse_unsigned_int64(unsigned char *data, int *datalength,
    unsigned char *type, Counter64 *cp, int countersize);
unsigned char   *asn_build_unsigned_int64(unsigned char *data, int *datalength,
    unsigned char type, Counter64 *cp, int countersize);
void asn_parse_subidentifier(unsigned char *data, int datalength, oid *objid, int *objidlength);
#endif /* _ASN1_H_ */
