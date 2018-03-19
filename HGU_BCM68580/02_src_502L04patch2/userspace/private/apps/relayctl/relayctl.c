/************************************************************************
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

/*
 *
 *
 *  INCOMPLETE --  USERSPACE APP TO CONTROL RELAY BOARDS
 *
 * usage:
 *    relayctl 
 * 
 */

#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <err.h>
#include <stdio.h>
#include <sys/mman.h>
#include <strings.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


#define MAX_CMD_LEN 512

int devmem;

int
main (int argc, char **argv)
{
  int i, j;
  unsigned int relays = 0;
  unsigned int * io_base;

  devmem = open ("/dev/mem", O_RDWR | O_SYNC);
  io_base =
    mmap (NULL, 0x400, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED,
	  devmem, 0x10000000);
  if (io_base <= 0)
    {
      err (5, "mmap failed");
    }

  i = setpriority(PRIO_PROCESS,0,-20);

  // initialize GPIO hardware
  io_base[ 0x98/4 ] = 0;
  io_base[ 0xa0/4 ] = 0;
  io_base[ 0x9c/4 ] = 0;
  io_base[ 0x8c/4 ] = 0; // data register always zero

  for (i = 1 ; i < argc ; i++) 
  {
	if (strcmp(argv[i],"x") == 0) {
		relays = 0;
        } else if (sscanf(argv[i],"r%d",&j) == 1) {
		relays |= 1 << (j-1);
	} else if (strcmp(argv[i],"M") == 0) {
  		io_base[ 0x84/4 ] &= ~relays;
	} else if (strcmp(argv[i],"B") == 0) {
  		io_base[ 0x84/4 ] |= relays;
        } else if (sscanf(argv[i],"w%d",&j) == 1) {
		usleep(j * 1000);
	} else {
		fprintf(stderr,"?? %s ??\n",argv[i]);
		exit(1);
	}
  }
  return (0);
}

