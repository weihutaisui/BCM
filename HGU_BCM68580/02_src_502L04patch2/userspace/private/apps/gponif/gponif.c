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
 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <getopt.h>

#include <gponif_api.h>
#include "gponif.h"

#ifdef BUILD_STATIC
int gponif_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    char *ifname = NULL;
    char name[GPONIF_NAME_MAX];
    int ret = 0, exclusive = 0;
    u8 delop = 0, showop = 0;
    u32 temp = 0, gem = 0;
    u8 gem_map_arr[MAX_GEM_VALUE + 1];
    int i=0, c;
    
    memset(gem_map_arr, 0, sizeof(gem_map_arr));

    while ((c = getopt_long(argc, argv, "a:c::d:g:h:m:n:r:s:uv", 
           longopts, 0)) != EOF) {

        switch (c) {

        case 'a': 
            opt_a++; exclusive++;
            ifname = optarg;
            break;

        case 'c':
            opt_c++; exclusive++;
            ifname = optarg;
            if (optarg == NULL) {
                /* Take care of the case where optional argument 
                   is given with spaces */
                if ((argv[optind] != NULL) && isalpha(*argv[optind])) {
                    ifname = argv[optind]; 
                }
            }
            break;

        case 'd': 
            opt_d++; exclusive++; 
            ifname = optarg;
            if (strcmp(ifname, "all") == 0) {
                delop = OP_ALL;
            }
            break;

        case 'm': 
            opt_m++; exclusive++;
            gem = atoi(optarg);
            break;

        case 'n': 
            opt_n++; exclusive++;
            gem = atoi(optarg);
            break;

        case 'r': 
            opt_r++; exclusive++; 
            ifname = optarg;
            break;

        case 's': 
            opt_s++; exclusive++;
            ifname = optarg;
            if (strcmp(ifname, "all") == 0) {
                showop = OP_ALL;
            }
            break;

        case 'g': 
            opt_g++; 
            /* Check if first gem_id given is "all" */ 
            if (strcmp(optarg, "all")) {
                /* 1st gemid is not "all" */
                temp = atoi(optarg);
                if (temp <= MAX_GEM_VALUE) {
                    gem_map_arr[temp] = 1;
                } else {
                    fprintf(stderr, "The gem id %d is not valid", temp);    
                    return -1;
                }
                while (argv[optind] != NULL)
                {
                    temp = atoi(argv[optind]);
                    if (temp <= MAX_GEM_VALUE) {
                        gem_map_arr[temp] = 1;
                    } else {
                        fprintf(stderr, "The gem id %d is not valid", temp);    
                        return  -1;
                    }
                    optind++;
                }
            } else {
                /* all gemids, so set gem_map to all 1s */
                for (i=0; i <= MAX_GEM_VALUE;i++) 
                {
                    gem_map_arr[i] = 1;
                }
            }
            break;

        case 'h': opt_h++; exclusive++; break;

        case 'u': opt_u++; exclusive++; break;

        case 'v': opt_v++; exclusive++; break;

        case '?':
            fprintf(stderr, usage_msg);
            return -2;

        } /* End of switch */
    } /* End of while */

    /* options check */
    if (exclusive > 1 || exclusive == 0) {
        fprintf(stderr, usage_msg);
        return -2;
    }

    if (opt_v) {
        printf(version);
        return 0;
    }

    if (opt_u) {
        printf(usage_msg);
        return 0;
    }

    if (opt_h) {
        printf(usage_msg);
        printf(help_msg);
        return 0;
    }

    gponif_init();
    
    if (opt_a && opt_g) {
        for (i=0; i <= MAX_GEM_VALUE;i++) 
        {
           if (gem_map_arr[i] == 1)
           {
               gponif_addgemidx(ifname, i);
           }
        }
    } else if (opt_c) {
        gponif_create(ifname);
    } else if(opt_d) {
        if (delop != OP_ALL) {
            gponif_delete(ifname);
        } else {
            gponif_delete_all();
        }
    } else if (opt_m) {
        gponif_setmcastgem(gem);
    } else if (opt_n) {
        name[0] = '\0';
        gponif_get_ifname(gem, name);
        printf("gem id: %d, if name: %s \n", gem, name);
    } else if (opt_r && opt_g) {        
        for (i=0; i <= MAX_GEM_VALUE;i++) 
        {
           if (gem_map_arr[i] == 1)
           {
               gponif_remgemidx(ifname, i);
           }
        }
    } else if (opt_s) {
        if (showop != OP_ALL) {
            gponif_show(ifname);
        } else {
            gponif_show_all();
        }
    } else {
        printf(usage_msg);
    }

    gponif_close();
    return ret;
}
