/***********************************************************************
 *
 *  Copyright (c) 2007  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifndef _GPONIF_H_
#define _GPONIF_H_

typedef unsigned char u8;
typedef unsigned int u32;

#define OP_ALL         1
#define GPONIF_NAME_MAX    128

#define GPONIF_VERSION "0.0.1"
#define GPONIF_RELDATE "August 29, 2008"
#define GPONIF_NAME    "gponif"
static char *version =
GPONIF_NAME ".c:v" GPONIF_VERSION " (" GPONIF_RELDATE ")\n"
"o Pratapa Reddy Vaka (pvaka@broadcom.com). \n";

static const char *usage_msg = 
"Usage: gponif {-c|--create} [if_name]\n"
"       gponif {-d|--delete} <if_name>\n"
"       gponif {-d|--delete} all\n"
"       gponif {-s|--show} <if_name>\n"
"       gponif {-s|--show} all \n"
"       gponif {-n|--name} <gem_id>\n"
"       gponif {-m|--mcast} <gem_id>\n"
"       gponif {-a|--addgem} <if_name> -g{--gemport} <id1> [id2] ...\n"
"       gponif {-r|--remgem} <if_name> -g{--gemport} <id1> [id2] ...\n"
"       gponif {-h|--help} \n";

static const char *help_msg =
"\n"
"       To create the GPON virtual interface gponVoice \n"
"       #gponif -c gponVoice \n"
"\n"
"       To create the GPON virtual interface gponXX \n"
"       #gponif -c \n"
"\n"
"       To delete gpon0: \n"
"       #gponif -d gpon0 \n"
"\n"
"       To delete all gpon virtual interfaces: \n"
"       #gponif -d all \n"
"\n"
"       To show the details of gpon0: \n"
"       #gponif -s gpon0 \n"
"\n"
"       To show the details of all interfaces: \n"
"       #gponif -s all \n"
"\n"
"       To show the name of interface that contains the given gem id: \n"
"       #gponif -n 1 \n"
"\n"
"       To set the given gem id as multicast gem: \n"
"       #gponif -m 1 \n"
"\n"
"       To add physical gem port indices 8 and 13 to gpon0: \n"
"       #gponif -a gpon0 -g 8 13 \n"
"\n"
"       To remove physical gem port indices 2, 8, and 9 from gpon0: \n"
"       #gponif -r gpon0 -g 2 8 9 \n"
"\n"
"       #gponif {-u|--usage}   Show usage\n"
"       #gponif {-v|--version} Show version\n"
"       #gponif {-h|--help}    This message\n"
"\n";

/* Command-line flags. */
unsigned int 
opt_a = 0,    /* Add gem id(s) flag. */
opt_c = 0,    /* create i/f flag. */
opt_d = 0,    /* delete i/f flag. */
opt_n = 0,    /* get name of interface. */
opt_m = 0,    /* set multicast gem_id. */
opt_r = 0,    /* Remove gem id(s) flag. */
opt_s = 0,    /* Show interface(s). */
opt_g = 0,    /* List of gem ids. */
opt_h = 0,    /* Help flag */
opt_u = 0,    /* Usage flag */
opt_v = 0;    /* Version flag */

struct option longopts[] = {
    /* { name  has_arg, *flag  val } */
    {"create",	2, 0, 'c'},	/* Create an interface. */
    {"delete",	1, 0, 'd'},	/* Delete an interface. */
    {"show",	1, 0, 's'},	/* Show the given i/f or all interfaces. */
    {"addgem",	1, 0, 'a'},	/* Add one or more gem_ids to the i/f */
    {"remgem",	1, 0, 'r'},	/* Remove one or more gem_ids from the i/f */
    {"name",    1, 0, 'n'},	/* Show name of interface that contains the given gem id */
    {"mcast",    1, 0, 'm'},/* To set the given gem id as multicast gem */
    {"gemport",   1, 0, 'g'},	/* list of gem ids */
    {"help",	0, 0, 'h'},	/* Give help */
    {"usage",   0, 0, 'u'},	/* Give usage */
    {"version", 0, 0, 'v'},	/* Emit version information. */
    {0, 0, 0, 0}
};

#endif  /* _GPONIF_H_ */
