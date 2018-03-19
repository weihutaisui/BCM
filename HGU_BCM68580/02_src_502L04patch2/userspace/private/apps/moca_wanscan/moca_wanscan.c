/******************************************************************************
 *
 * Copyright (c) 2013   Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2013:proprietary:standard
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
 ************************************************************************/
/***************************************************************************
 * File Name  : moca_wanscan.c
 *
 * Description: Sample application to control MoCA interface to scan both 
 * Band C4 and D-High for WAN.
 ***************************************************************************/

/** Includes. **/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include "mocalib.h"
#include "mocaint.h"


/** Defines. **/
#define DBG_PRINT if(verbose) printf
#define ERR_PRINT printf

#define MOCA_WAN_SCAN_MAX_BANDS             2
#define MOCA_BAND_C4_SCAN_TIME_DEFAULT      20
#define MOCA_BAND_C4_SCAN_TIME_VARIATION    10
#define MOCA_BAND_DHIGH_SCAN_TIME_DEFAULT   40
#define MOCA_BAND_DHIGH_SCAN_TIME_VARIATION 10


/** Structures. **/
typedef struct 
{
   uint32_t lof;
   int32_t  pco;       // primary channel offset
   int32_t  sco;       // secondary channel offset
   uint32_t duration;  // in seconds
   uint32_t variation; // in seconds, scan time will be random between duration +/- variation
   uint32_t nc_mode;
   uint32_t single_ch_op;
   uint32_t band;
   uint32_t turbo_en;
} moca_wanscan_params_t;


static void showUsage( void )
{
   printf("moca_wanscan [-hv] [-i <interface>] [-c <C4 duration>] [-C <C4 variation>]\n");
   printf("             [-d <D-Hi duration>] [-D <D-Hi variation>]\n\n");
   printf("Options:\n");
   printf("    -h                   Display this help\n");
   printf("    -v                   Enable verbose prints\n");
   printf("    -i <interface>       Specify MoCA interface to control (e.g. moca0)\n");
   printf("    -c <C4 duration>     Specifiy duration in seconds to scan C4.\n");
   printf("                         Default %d seconds\n", MOCA_BAND_C4_SCAN_TIME_DEFAULT);
   printf("    -C <C4 variation>    Specifiy variation of scan time on C4 band.\n");
   printf("                         Default %d seconds\n", MOCA_BAND_C4_SCAN_TIME_VARIATION);
   printf("    -d <D-Hi duration>   Specifiy duration in seconds to scan D-High band.\n");
   printf("                         Default %d seconds\n", MOCA_BAND_DHIGH_SCAN_TIME_DEFAULT);
   printf("    -D <D-Hi variation>  Specifiy variation of scan time on D-High band.\n");
   printf("                         Default %d seconds\n", MOCA_BAND_DHIGH_SCAN_TIME_VARIATION);
   printf("\n");
   printf("Example: moca_wanscan -v -i moca0 -c 15 -C 5 -d 30 -D 10\n");
}

/***************************************************************************
 * Function Name: main
 * Description  : Main program function.
 * Returns      : 0 - success, non-0 - error.
 ***************************************************************************/
int main(int argc, char **argv)
{
   int ret = 0;
   int verbose = 0;
   char * mocaif = NULL;
   void * mocaHandle = NULL;
   uint32_t val = 0;
   int band = 0; // Start in D-High by default
   struct moca_interface_status if_status;
   uint32_t previous_link_status = MOCA_LINK_UP;
   uint32_t sleep_time;
   uint32_t i;

   moca_wanscan_params_t scan_params[MOCA_WAN_SCAN_MAX_BANDS] = 
   {
      { /* Band D-High parameters */ 
         MOCA_LOF_BAND_D_HIGH_DEF,           // lof
         0,                                  // pco
         0,                                  // sco
         MOCA_BAND_DHIGH_SCAN_TIME_DEFAULT,  // duration
         MOCA_BAND_DHIGH_SCAN_TIME_VARIATION,// variation
         0,                                  // nc_mode
         MOCA_SINGLE_CH_OP_NETWORK_SEARCH,   // single_ch_op
         MOCA_RF_BAND_D_HIGH,                // band
         1,                                  // turbo_en
      },
      { /* Band C4 parameters */ 
         MOCA_LOF_BAND_C4_DEF,             // lof
         0,                                // pco
         0,                                // sco
         MOCA_BAND_C4_SCAN_TIME_DEFAULT,   // duration
         MOCA_BAND_C4_SCAN_TIME_VARIATION, // variation
         2,                                // nc_mode
         MOCA_SINGLE_CH_OP_SINGLE,         // single_ch_op
         MOCA_RF_BAND_C4,                  // band
         0,                                // turbo_en
      },
   };

   opterr = 0;

   while((ret = getopt(argc, argv, "hvi:c:d:C:D:")) != -1) 
   {
      switch(ret)
      {
         case 'i':
            mocaif = optarg;
            break;
         case 'c':
            scan_params[1].duration = atoi(optarg);
            break;
         case 'd':
            scan_params[0].duration = atoi(optarg);
            break;
         case 'C':
            scan_params[1].variation = atoi(optarg);
            break;
         case 'D':
            scan_params[0].variation = atoi(optarg);
            break;
         case 'v':
            verbose = 1;
            break;
         case '?':
            fprintf(stderr, "Error!  Invalid option - %c\n", optopt);
            return(-1);
            break;
         case 'h':
         default:
            showUsage();
            return(0); 
      }
   }

   mocaHandle = moca_open(mocaif);

   if (mocaHandle == NULL)
   {
      printf("Unable to connect to moca interface %s\n", mocaif ? mocaif : "default");
      return -1;
   }

   ret = moca_get_lof(mocaHandle, &val);
   if (ret == MOCA_API_SUCCESS)
   {
      DBG_PRINT("Initial LOF = %d\n", val);

      /* Only one frequency in band C4 */
      if (val != MOCA_LOF_BAND_C4_DEF)
      {
         scan_params[0].lof = val;  
         ret = moca_get_primary_ch_offset(mocaHandle, &scan_params[0].pco);
         if (ret != MOCA_API_SUCCESS)
            ERR_PRINT("Error %d getting primary_ch_offset\n", ret);

         ret = moca_get_secondary_ch_offset(mocaHandle, &scan_params[0].sco);
         if (ret != MOCA_API_SUCCESS)
            ERR_PRINT("Error %d getting secondary_ch_offset\n", ret);
      }
      else
      {
         band = 1; // Start in C4
      }
   }

   for (i = 0; i < MOCA_WAN_SCAN_MAX_BANDS; i ++)
   {
      if ( scan_params[i].variation > scan_params[i].duration )
      {
         scan_params[i].variation = scan_params[i].duration;
         DBG_PRINT("WARNING: Forcing variation to %ds for band %d\n", 
            scan_params[i].variation, scan_params[i].band);
      }
   }
   
   DBG_PRINT("Starting in Band %s\n", band ? "C4" : "D-High");
   DBG_PRINT("D-High params: LOF %d  PCO %d  SCO %d  Dur %ds +/- %ds  NC %d\n", 
      scan_params[0].lof, scan_params[0].pco, scan_params[0].sco, scan_params[0].duration,
      scan_params[0].variation, scan_params[0].nc_mode);
   DBG_PRINT("C4 params    : LOF %d  PCO %d  SCO %d  Dur %ds +/- %ds  NC %d\n", 
      scan_params[1].lof, scan_params[1].pco, scan_params[1].sco, scan_params[1].duration,
      scan_params[1].variation, scan_params[1].nc_mode);


   while (1)
   {
      ret = moca_get_interface_status(mocaHandle, &if_status);

      if ((ret == MOCA_API_SUCCESS) && (if_status.link_status == MOCA_LINK_UP))
      {
         MoCAOS_MSleep(1000);

         if (previous_link_status == MOCA_LINK_DOWN)
         {
            ret = moca_set_persistent(mocaHandle);
            DBG_PRINT("moca_set_persistent returned %d\n", ret);

            if (band == 0)
               band = MOCA_WAN_SCAN_MAX_BANDS - 1;
            else
               band--;

            ret = moca_get_lof(mocaHandle, &val);
            if (ret == MOCA_API_SUCCESS)
            {  
               if (scan_params[band].lof != val)
                  DBG_PRINT("New LOF for band %s: %d MHz\n", band ? "C4" : "D-High", val);

               scan_params[band].lof = val;
            }
         }
         previous_link_status = if_status.link_status;
      }
      else
      {
         previous_link_status = MOCA_LINK_DOWN;

         ret = moca_set_stop(mocaHandle);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting stop\n", ret);

         ret = moca_set_rf_band(mocaHandle, scan_params[band].band);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting rf_band\n", ret);

         ret = moca_set_cof(mocaHandle, scan_params[band].lof);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting operating frequency\n", ret);

         ret = moca_set_primary_ch_offset(mocaHandle, scan_params[band].pco);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting primary_ch_offset\n", ret);

         ret = moca_set_secondary_ch_offset(mocaHandle, scan_params[band].sco);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting secondary_ch_offset\n", ret);

         ret = moca_set_nc_mode(mocaHandle, scan_params[band].nc_mode);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting nc_mode\n", ret);

         ret = moca_set_single_channel_operation(mocaHandle, scan_params[band].single_ch_op);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting single_channel_operation\n", ret);

         ret = __moca_set_rf_switch(mocaHandle, 1);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting rf_switch\n", ret);

         ret = moca_set_turbo_en(mocaHandle, scan_params[band].turbo_en);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting turbo_en\n", ret);

         ret = moca_set_start(mocaHandle);
         if (ret != MOCA_API_SUCCESS) ERR_PRINT("Error %d setting start\n", ret);

         sleep_time = scan_params[band].duration - scan_params[band].variation;

         if (scan_params[band].variation != 0)
         {
            sleep_time += MoCAOS_GetRandomValue() % (2 * scan_params[band].variation);
         }
         DBG_PRINT("Waiting %d seconds for link\n", sleep_time);

         MoCAOS_MSleep(sleep_time * 1000);

         band++;
         if (band == MOCA_WAN_SCAN_MAX_BANDS)
            band = 0;
      }
   };

   return(0);
}

