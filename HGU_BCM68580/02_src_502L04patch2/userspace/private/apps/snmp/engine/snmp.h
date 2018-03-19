/* ====================================================================
 * <:label-BRCM:1997:NONE:standard
 * 
 * :>
 * Copyright (c) 1997 - 2000 
 *                      SMASH, Harrie Hazewinkel.  All rights reserved.
 *
 * This product is developed by Harrie Hazewinkel and updates the
 * original SMUT compiler made as his graduation project at the
 * University of Twente.
 *
 * SMASH is a software package containing an SNMP MIB compiler and
 * an SNMP agent system. The package can be used for development
 * of monolithic SNMP agents and contains a compiler which compiles
 * MIB definitions into C-code to developed an SNMP agent.
 * More information about him and this software product can
 * be found on http://www.simpleweb.org/software/packages/smash.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by Harrie Hazewinkel"
 *
 * 4. The name of the Copyright holder must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Harrie Hazewinkel"
 *    Also acknowledged are:
 *    - The Simple group of the University of Twente,
 *          http://www.simpleweb.org/
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR, ITS DISTRIBUTORS
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================*/
#ifndef _SNMP_H_
#define _SNMP_H_

/* The standard SNMP-ports */
#define SNMP_PORT               161
#define SNMP_TRAP_PORT          162

/* The maximum accepted packet length */
#define SNMP_MAX_MSG_LENGTH             0x8000 // old: 1500
#define SNMP_MAX_COMMUNITY_LENGTH	1500


/* SNMP versions */
#define SNMP_VERSION_1      0
#define SNMP_VERSION_2C     1
#define SNMP_VERSION_2      2
#define SNMP_VERSION_3      3

/* The SNMP Protocol Data Units */
#define SNMP_GET_REQ_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x0)
#define SNMP_GETNEXT_REQ_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x1)
#define SNMP_GET_RSP_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x2)
#define SNMP_SET_REQ_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x3)
#define SNMP_TRP_REQ_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x4)
#define SNMP_GETBULK_REQ_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x5)
#define SNMP_INFORM_REQ_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x6)
#define SNMP_TRP2_REQ_PDU		(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x7)
#define SNMP_REPORT_PDU			(ASN_CONTEXT | ASN_CONSTRUCTOR | 0x8)

/* SNMP version 1 error-codes */
#define SNMP_ERROR_NOERROR		(0x0)
#define SNMP_ERROR_TOOBIG		(0x1)
#define SNMP_ERROR_NOSUCHNAME		(0x2)
#define SNMP_ERROR_BADVALUE		(0x3)
#define SNMP_ERROR_READONLY		(0x4)
#define SNMP_ERROR_GENERR		(0x5)

/* SNMP version 2 error-codes */
#define SNMP_ERROR_NOACCESS		(0x6)
#define SNMP_ERROR_WRONGTYPE		(0x7)
#define SNMP_ERROR_WRONGLENGTH		(0x8)
#define SNMP_ERROR_WRONGENCODING	(0x9)
#define SNMP_ERROR_WRONGVALUE		(0x10)
#define SNMP_ERROR_NOCREATION		(0x11)
#define SNMP_ERROR_INCONSISTENTVALUE	(0x12)
#define SNMP_ERROR_RESOURCEUNAVAILABLE	(0x13)
#define SNMP_ERROR_COMMITFAILED		(0x14)
#define SNMP_ERROR_UNDOFAILED		(0x15)
#define SNMP_ERROR_AUTHORIZATIONERROR	(0x16)
#define SNMP_ERROR_NOTWRITABLE		(0x17)
#define SNMP_ERROR_INCONSISTENTNAME	(0x18)

#define SNMP_TRAP_COLDSTART		(0x0)
#define SNMP_TRAP_WARMSTART		(0x1)
#define SNMP_TRAP_LINKDOWN		(0x2)
#define SNMP_TRAP_LINKUP		(0x3)
#define SNMP_TRAP_AUTHFAIL		(0x4)
#define SNMP_TRAP_EGPNEIGHBORLOSS	(0x5)
#define SNMP_TRAP_ENTERPRISESPECIFIC	(0x6)

/* SNMP VarBinds. */
#define SNMP_NOSUCHOBJECT    (ASN_CONTEXT | ASN_PRIMITIVE | 0x0)
#define SNMP_NOSUCHINSTANCE  (ASN_CONTEXT | ASN_PRIMITIVE | 0x1)
#define SNMP_ENDOFMIBVIEW    (ASN_CONTEXT | ASN_PRIMITIVE | 0x2)


/* Object Syntax - universal types */
#define SNMP_INTEGER     ASN_INTEGER
#define SNMP_STRING      ASN_OCTET_STR
#define SNMP_OBJID       ASN_OBJECT_ID
#define SNMP_NULLOBJ     ASN_NULL
#define SNMP_BITSTRING   ASN_BIT_STR
#define SNMP_BITS        ASN_BIT_STR
#define SNMP_BOOLEAN     ASN_BOOLEAN

/* Object Syntax - application types */
#define SNMP_IPADDRESS   (ASN_APPLICATION | 0)
#define SNMP_COUNTER     (ASN_APPLICATION | 1)
#define SNMP_GAUGE       (ASN_APPLICATION | 2)
#define SNMP_UINTEGER    (ASN_APPLICATION | 2)
#define SNMP_TIMETICKS   (ASN_APPLICATION | 3)
#define SNMP_OPAQUE      (ASN_APPLICATION | 4)
#define SNMP_NSAP        (ASN_APPLICATION | 5)
#define SNMP_COUNTER64   (ASN_APPLICATION | 6)

#define SNMP_LOCK_TIMEOUT  (60 * MSECS_IN_SEC)

/* TBD: Missing defines */
#define SYS_CMD_LEN 256

#endif /* _SNMP_H_ */

