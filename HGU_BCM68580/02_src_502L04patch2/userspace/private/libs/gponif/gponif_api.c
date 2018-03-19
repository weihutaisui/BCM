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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/ethtool.h>

#include <bcmnet.h>
#include "gponif_api.h"

typedef unsigned char u8;
typedef unsigned int u32;

static int init_done = 0;
static int skfd;
static struct ifreq ifr;
static struct gponif_data ifdata;
static struct gponif_data *g = &ifdata;

/* Init the gponif */
int gponif_init(void)
{
    if (init_done) {
        fprintf(stderr, "Already Initialized \n");
        return 0; 
    }

    /* Open a basic socket */
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket open error");
        return -1;
    }
    
    /* Get the name -> if_index mapping for GPON i/f */
    strcpy(ifr.ifr_name, "bcmsw");
    if (ioctl(skfd, SIOCGIFINDEX, &ifr) < 0 ) {
        close(skfd);
        fprintf(stderr, "gpon interface does not exist");
        return -1;
    }
 
    /* */
    ifr.ifr_data = (char *)&ifdata;
    
    init_done = 1;
    
    return 0;
}

/* close the socket */
void gponif_close(void)
{
    init_done = 0;
    close(skfd);
}

void dump_gemidx_map()
{
    int i;
    u8 found = 0;
    printf("GemIdx Map: ");
    for (i = 0; i <= MAX_GEM_VALUE; i++) 
    {
        if (g->gem_map_arr[i] == 1) 
        {
            printf(" %d ", i);
            found = 1;
        }
    }
    if (!found) 
    {
        printf("No gem idx set");
    }
    printf("\n");
}

u8 isAnyGemIdxAssigned()
{
    int i;

    for (i = 0; i <= MAX_GEM_VALUE; i++) 
    {
        if (g->gem_map_arr[i] == 1) 
        {
            return 1;
        }
    }
    return 0;
}

#if MAX_GEM_VALUE < 32
/* Add one or more gems to the given gpon virtual i/f */
int gponif_addgem(char *ifname, u32 gem_map)
{
    int err = -1;
    int i;
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }
    
    if (strcmp(ifname, "all") == 0) {
        perror("\"all\" is a reserved name.");
        return -1;
    }
    
    for (i = 0; i <= MAX_GEM_VALUE; i++) 
    {
        if ((gem_map >> i) & 1) 
        {
            if ((err = gponif_addgemidx(ifname, i)))
            {
                fprintf(stderr, "gponif_addgemidx returned error for gemidx %d err %d\n", i, err);
                return err;
            }
        }
    }
 
    return err;
}

/* Remove one or more gems from the given gpon virtual i/f */
int gponif_remgem(char *ifname, u32 gem_map)
{
    int err = -1;
    int i;
 
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }

    if (strcmp(ifname, "all") == 0) {
        perror("\"all\" is a reserved name.");
        return -1;
    }

    for (i = 0; i <= MAX_GEM_VALUE; i++) 
    {
        if ((gem_map >> i) & 1) 
        {
            if ((err = gponif_remgemidx(ifname, i)))
            {
                fprintf(stderr, "gponif_remgemidx returned error for gemidx %d err %d\n", i, err);
                return err;
            }
        }
    }

    return err;
}
#endif

/* Add one or more gems to the given gpon virtual i/f */
int gponif_addgemidx(char *ifname, u32 gemIdx)
{
    int err = -1;
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }
    
    if (strcmp(ifname, "all") == 0) {
        perror("\"all\" is a reserved name.");
        return -1;
    }
    
    if (gemIdx >= CONFIG_BCM_MAX_GEM_PORTS ) {
        fprintf(stderr, "Error : gponif_addgemidx() gemIds <%d> is out of range [0..%d]\n", gemIdx, CONFIG_BCM_MAX_GEM_PORTS - 1);
        return -1;
    }

    strcpy(g->ifname, ifname);

    /* Get the current gemid map */
    g->op = GETGEMIDMAP;
    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }
//printf("gponif_addgemidx: The cur gemidx map is ");
//dump_gemidx_map();

    /* Add the gems to the given GPON virtual i/f */
    g->op = SETGEMIDMAP;
    g->gem_map_arr[gemIdx] = 1;

//printf("gponif_addgemidx: The new gemidx map after adding gemidx %d is ", gemIdx);
//dump_gemidx_map();

    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }
    
    return err;
}

/* Remove one or more gems from the given gpon virtual i/f */
int gponif_remgemidx(char *ifname, u32 gemIdx)
{
    int err = -1;
 
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }

    if (strcmp(ifname, "all") == 0) {
        perror("\"all\" is a reserved name.");
        return -1;
    }

    if (gemIdx >= CONFIG_BCM_MAX_GEM_PORTS ) {
        fprintf(stderr, "Error : gponif_remgemidx() gemIds <%d> is out of range [0..%d]\n", gemIdx, CONFIG_BCM_MAX_GEM_PORTS - 1);
        return -1;
    }

    strcpy(g->ifname, ifname);

    /* Get the current gemid map */
    g->op = GETGEMIDMAP;
    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }
//printf("gponif_remgemidx: The cur gemidx map is ");
//dump_gemidx_map();

    /* Remove the gems from the given GPON virtual i/f */
    g->gem_map_arr[gemIdx] = 0;
    
//printf("gponif_remgemidx: The new gemidx map after resetting gemidx %d is ", gemIdx);
//dump_gemidx_map();
    if(!isAnyGemIdxAssigned()) {
        perror("Warning: No more gem ids assigned for this interface \n"); 
    }
    
    g->op = SETGEMIDMAP;
    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }
 
    return err;
}
/* Add one or more gems to the given gpon virtual i/f */
int gponif_setmcastgem(u32 gem_id)
{
    int err = -1;
    unsigned char gem_map[CONFIG_BCM_MAX_GEM_PORTS];
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }
    if (gem_id > MAX_GEM_VALUE ) {
        fprintf(stderr, "Error : gponif_setmcastgem() Invalid gem_id <%d>\n",gem_id);
        return -1;
    }
    /* Add the gems to the given GPON virtual i/f */
    g->op = SETMCASTGEMID;
    g->gem_map_arr[gem_id] = 1;
    // store gem_map_arr to gem_map
    memcpy(gem_map,g->gem_map_arr,sizeof(gem_map));
    // clear gem_map_arr
    memset(g->gem_map_arr,0,sizeof(g->gem_map_arr));
    // assign only multicast gem to gem_map_arr
    g->gem_map_arr[gem_id] = 1;
    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }
    // copy back gem_map to gem_map_arr
    memcpy(g->gem_map_arr,gem_map,sizeof(gem_map));
//printf("gponif_setmcastgem: The new gemidx map after setting gemidx %d is ", gem_id);
//dump_gemidx_map();
    
    return err;
}

int gponif_create(char *ifname)
{
    int err = -1;
 
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }

    if (ifname != NULL) {
        if (strcmp(ifname, "all") == 0) {
            perror("\"all\" is a reserved name.");
            return -1;
        }
    }

    /* Create a GPON virtual interface */
    g->op = CREATEGPONVPORT;
    if (ifname == NULL) {
        /* Indicate that ifname is NULL */
        g->ifname[0] = 0;
    } else {
        strcpy(g->ifname, ifname);
    }

    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }

    return err;
}

int gponif_delete(char *ifname)
{
    int err = -1;
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }
 
    /* Delete the given GPON virtual interface */
    g->op = DELETEGPONVPORT;
    strcpy(g->ifname, ifname);
    
    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }
    
    return err;
}

int gponif_delete_all(void)
{
    int err = -1;
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }
 
    /* Delete all gpon virtual interfaces */
    g->op = DELETEALLGPONVPORTS;
    
    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "ioctl command return error!\n");
        return err;
    }
    
    return err;
}

int gponif_show(char *ifname)
{
    int err = -1, i = 0;
    u8 first = 1;
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }

    /* Indicate the operation is GETGEMIDMAP */
    g->op = GETGEMIDMAP;
    strcpy(g->ifname, ifname);
 
    /* Get the GEM IDs of given i/f */
    if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
        fprintf(stderr, "command return error!\n");
        return err;
    }
    fprintf(stderr, "i/f name: %s \n", ifname);
    fprintf(stderr, "gem ids: ");
    for (i = 0; i <= MAX_GEM_VALUE; i++) {
        if (g->gem_map_arr[i]) {
            if (first) {
                first = 0;
                fprintf(stderr, "%d", i);
            } else {
                fprintf(stderr, ", %d", i);
            }
        }
    }
    fprintf(stderr, "\n");
    
    return err;
}

int gponif_show_all(void)
{
    int err = -1, i = 0, j = 0;
    u8 first = 1;
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }

    /* Indicate the operation is GETGEMIDMAP */
    g->op = GETGEMIDMAP;
    
 
    /* */ 
    /* Get the GEM IDs and ifnames of all gpon virtual interfaces */
    for(i = 0; i < MAX_GPON_IFS; i++) {
        /* Indicate enet driver to use the ifnumber */
        g->ifname[0] = 0; 
        /* ifid of the interface */
        g->ifnumber = i;
        /* Get the interface details */
        if((err = ioctl(skfd, SIOCGPONIF, &ifr))) {
            fprintf(stderr, "command return error!\n");
            return err;
        }
        /* Display the details if an interface exists with the given ifid */
        if (g->ifname[0] != 0) {
            fprintf(stderr, "i/f name: %s \n", g->ifname);
            fprintf(stderr, "gem ids: ");
            first = 1;
            for (j = 0; j <= MAX_GEM_VALUE; j++) {
                if (g->gem_map_arr[j]) {
                    if (first) {
                        first = 0;
                        fprintf(stderr, "%d", j);
                    } else {
                        fprintf(stderr, ", %d", j);
                    }
                }
            }
            fprintf(stderr, "\n");
        }
    }
    
    return err;
}

int gponif_get_ifname(const u32 gem, char *ifname)
{
    int err = -1, i = 0, j = 0, found = 0;
    
    if (!init_done)
    {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }

    /* Indicate the operation is GETGEMIDMAP */
    g->op = GETGEMIDMAP;
    
 
    /* */ 
    /* Get the GEM IDs and ifnames of all gpon virtual interfaces */
    for (i = 0; i < MAX_GPON_IFS && found == 0; i++)
    {
        /* Indicate enet driver to use the ifnumber */
        g->ifname[0] = 0; 
        /* ifid of the interface */
        g->ifnumber = i;
        /* Get the interface details */
        if((err = ioctl(skfd, SIOCGPONIF, &ifr)))
        {
            fprintf(stderr, "command return error!\n");
            return err;
        }
        /* find name of interface that contains the given gem id */
        if (g->ifname[0] != 0)
        {
            for (j = 0; j <= MAX_GEM_VALUE && found == 0; j++)
            {
                if (g->gem_map_arr[j] && (u32) j == gem)
                {
                    found = 1;
                    strcpy(ifname, g->ifname);
                }
            }
        }
    }

    /* if ifname cannot be found return error */
    if (found == 0)
        err = -1;

    return err;
}

int gponif_get_number_of_gems(const char *ifname, u32 *gem_num)
{
    int err = -1, i = 0;
    u32 num = 0;
    
    if (!init_done) {
        fprintf(stderr, "Not yet initialized. \n");
        return -1;
    }

    /* Indicate the operation is GETGEMIDMAP */
    g->op = GETGEMIDMAP;
    strcpy(g->ifname, ifname);
 
    /* Get the GEM IDs of given i/f */
    if((err = ioctl(skfd, SIOCGPONIF, &ifr)))
    {
        fprintf(stderr, "command return error!\n");
        return err;
    }

    for (i = 0; i <= MAX_GEM_VALUE; i++)
    {
        if (g->gem_map_arr[i])
        {
            num++;
        }
    }

    *gem_num = num;
    
    return err;
}



