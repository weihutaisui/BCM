/******************************************************************************
 *
 *	Copyright (c) 2012	Broadcom Corporation
 *	All Rights Reserved
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
/***************************************************************************
 *
 *	   Copyright (c) 2008-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *	Description: Non-generated libmoca CLI functions
 *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mocalib.h"
#include "mocalib-cli.h"
#include "mocaint.h"
#if defined(STANDALONE)
#include "printf.h"
#include "moca.h"
#include "scratch.h"
#endif

static int mocacli_get_init_handler(void * handle, char ** pp_parms, int num_parms);
static int mocacli_get_config_handler(void * handle, char ** pp_parms, int num_parms);
static int mocacli_get_group_handler(void * handle, char ** pp_parms, int num_parms);
static int mocacli_get_groupall_handler(void * handle, char ** pp_parms, int num_parms);
static int mocacli_get_handler( void * handle, int argc, char **argv );
static int mocacli_set_handler( void * handle, int argc, char **argv );
static int mocacli_do_handler( void * handle, int argc, char **argv );

char *g_prfDesc[22]={"RX Unicast", "RX Broadcast ", "RX Map ", "TX Unicast ", "TX Broadcast ",
                         "TX Map ", "RX Unicast VLPER ", "RX Unicast NPER ", "RX Broadcast VLPER",
                         "RX Broadcast NPER ", "RX Map 2.0 ", "RX OFDMA ", "TX Unicast VLPER",
                         "TX Unicast NPER ", "TX Broadcast VLPER ", "TX Broadcast NPER ", "TX Map 2.0", "TX OFDMA",
                         "RX Unicast NPER Secondary Channel ", "RX Unicast VLPER Secondary Channel ", "TX Unicast NPER Secondary Channel ", "TX Unicast VLPER Secondary Channel "};

MOCALIB_CLI int mocacli_get_char_array(char *string, char *out, uint32_t size)
{
   uint32_t len = (uint32_t)strlen(string);

   if (size == 0)
   {
      fprintf(stderr, "Size 0 buffer passed to get_char_array\n");
      return(-2);
   }

   if ( len > (size-1) )
   {
      fprintf(stderr, "String length %d is larger than the array size %d\n", len, size);
      return(-1);
   }
   
   memset (out, '\0', size);
 
   strncpy(out, string, (size -1));

 
   return (0);
}



MOCALIB_CLI int mocap_set_bits(uint32_t * out, uint32_t start, uint32_t end, uint32_t size)
{
   uint32_t maxbit = (size * 8) - 1;
   uint32_t i;
   
   if ((start > maxbit) || (end > maxbit) || (start > end))
   {
      fprintf(stderr, "Bits values out of range\n");
      return(-1);
   }
   for (i = start; i <= end; i++)
   {
      out[i/32] |= 0x80000000 >> (i % 32);
   }

   return(0);
}

MOCALIB_CLI int mocacli_get_bits(char * string, uint32_t * out, uint32_t size)
{
   uint32_t len = (uint32_t)strlen(string);
   uint32_t i;
   uint32_t start = 0;
   uint32_t end = 0;
   uint32_t digit = 0;
   uint32_t is_end = 0;
   int ret = 0;

   memset (out, 0, size);

   // The user can input a string without spaces that defines which bits
   // should be set. For example a string "0-3,5,7,22-24" for a 4-byte value
   // equates to:
   // 1111 0101 0000 0000 0000 0011 1000 0000
   // 0xF5000380
   for (i = 0; (i < len) && (ret == 0); i++)
   {
      // Parse the sting one character at a time
      if ((string[i] >= '0') && (string[i] <= '9'))
      {
         digit = (digit*10) + (string[i] - '0');
      }
      else if (string[i] == ',')
      {
         if (is_end)
            end = digit;
         else
            start = end = digit;

         ret = mocap_set_bits(out, start, end, size);

         start = 0;
         end = 0;
         digit = 0;
         is_end = 0;
      }
      else if (string[i] == '-')
      {
         start = digit;
         is_end = 1;
         digit = 0;
      }
      else
      {
         ret = -2;
      }
   }

   if (is_end)
      end = digit;
   else
      start = end = digit;

   ret = mocap_set_bits(out, start, end, size);

   return(ret);
   
}

MOCALIB_CLI int mocacli_get_uint32(char * string, uint32_t * out)
{
   char * pszEnd;
   
   *out = (uint32_t) strtoul ( string, &pszEnd, 0 );
      
   if( *pszEnd != '\0' )
   {
      return(-1);
   }

   return(0);
}

   
MOCALIB_CLI int mocacli_get_int32(char * string, int32_t * out)
{
   char * pszEnd;
   
   *out = (int32_t) strtol ( string, &pszEnd, 0 );

   if( *pszEnd != '\0' )
   {
      return(-1);
   }

   return(0);
}

MOCALIB_CLI int mocacli_get_uint16(char * string, uint16_t * out)
{
   char * pszEnd;
   
   *out = (uint16_t) strtoul ( string, &pszEnd, 0 );

   if( *pszEnd != '\0' )
   {
      return(-1);
   }

   return(0);
}

   
MOCALIB_CLI int mocacli_get_int16(char * string, int16_t * out)
{
   char * pszEnd;
   
   *out = (int16_t) strtol ( string, &pszEnd, 0 );

   if( *pszEnd != '\0' )
   {
      return(-1);
   }

   return(0);
}

MOCALIB_CLI int mocacli_get_uint8(char * string, uint8_t * out)
{
   char * pszEnd;
   
   *out = (uint8_t) strtoul ( string, &pszEnd, 0 );

   if( *pszEnd != '\0' )
   {
      return(-1);
   }

   return(0);
}

   
MOCALIB_CLI int mocacli_get_int8(char * string, int8_t * out)
{
   char * pszEnd;
   
   *out = (int8_t) strtol ( string, &pszEnd, 0 );

   if( *pszEnd != '\0' )
   {
      return(-1);
   }

   return(0);
}

MOCALIB_CLI int mocacli_get_start_end(char *** pp_parms, int * num_parms, uint32_t * start, uint32_t * end)
{
   int ret;
   
   ret = mocacli_get_uint32(**pp_parms, start);
   if (ret != 0) return ret;

   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if (*num_parms > 0)
   {
      if ((*pp_parms == NULL) || (**pp_parms == NULL))
         return (-1);

      ret = mocacli_get_uint32(**pp_parms, end);
      if (ret == 0)
      {
         *pp_parms = *pp_parms + 1;
         *num_parms = *num_parms - 1;
      }
      else
      {
         *end = *start;
      }
   }
   else
   {
      *end = *start;
   }

   return(0);
}
   

MOCALIB_CLI int mocacli_get_macaddr(char * string, macaddr_t * out)
{
   char * mac;
   char * mac1;
   uint32_t i;
   macaddr_t addr;

   mac = string;
   
   for ( i = 0; i < sizeof(macaddr_t); i++ ) 
   {
      addr.addr[i] = (uint8_t)( strtol( mac, &mac1, 16) );
      if ((i < (sizeof(macaddr_t) - 1)) && (*mac1 != ':') && (*mac1 != '.'))
         return (-1);
      if (mac == mac1)
         return (-1);

      mac1++;
      mac = mac1;
   }

   MOCA_MACADDR_COPY(out, &addr);

   return (0);
}

MOCALIB_CLI int mocacli_get_macaddr_array(char *** pp_parms, int * num_parms, macaddr_t * array, int32_t div)
{
   uint32_t  start;
   uint32_t  end;
   macaddr_t value;
   int       ret;
   
   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_macaddr(**pp_parms, &value);

   if (ret != 0) return ret;
   
   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_start_end(pp_parms, num_parms, &start, &end);
   if (ret != 0) return ret;

   start = start/div;
   end = end/div;

   while (start <= end)
   {
      array[start++] = value;
   }

   return (0);
}

MOCALIB_CLI int mocacli_get_uint8_array(char *** pp_parms, int * num_parms, uint8_t * array, int32_t div)
{
   uint32_t start;
   uint32_t end;
   uint8_t  value;
   int      ret;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
   {
      return (-1);
   }
   ret = mocacli_get_uint8(**pp_parms, &value);
   if (ret != 0) return ret;

   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_start_end(pp_parms, num_parms, &start, &end);
   if (ret != 0) return ret;

   start = start/div;
   end = end/div;

   while (start <= end)
   {
      array[start++] = value;
   }

   return (0);
}


MOCALIB_CLI int mocacli_get_int8_array(char *** pp_parms, int * num_parms, int8_t * array, int32_t div)
{
   uint32_t start;
   uint32_t end;
   int8_t   value;
   int      ret;
   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_int8(**pp_parms, &value);
   if (ret != 0) return ret;

   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_start_end(pp_parms, num_parms, &start, &end);
   if (ret != 0) return ret;

   start = start/div;
   end = end/div;

   while (start <= end)
   {
      array[start++] = value;
   }

   return (0);
}

MOCALIB_CLI int mocacli_get_uint16_array(char *** pp_parms, int * num_parms, uint16_t * array, int32_t div)
{
   uint32_t start;
   uint32_t end;
   uint16_t value;
   int      ret;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
   {
      return (-1);
   }
   ret = mocacli_get_uint16(**pp_parms, &value);
   if (ret != 0) return ret;

   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_start_end(pp_parms, num_parms, &start, &end);
   if (ret != 0) return ret;

   start = start/div;
   end = end/div;

   while (start <= end)
   {
      array[start++] = value;
   }

   return (0);
}


MOCALIB_CLI int mocacli_get_int16_array(char *** pp_parms, int * num_parms, int16_t * array, int32_t div)
{
   uint32_t start;
   uint32_t end;
   int16_t  value;
   int      ret;
   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_int16(**pp_parms, &value);
   if (ret != 0) return ret;

   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_start_end(pp_parms, num_parms, &start, &end);
   if (ret != 0) return ret;

   start = start/div;
   end = end/div;

   while (start <= end)
   {
      array[start++] = value;
   }

   return (0);
}

MOCALIB_CLI int mocacli_get_uint32_array(char *** pp_parms, int * num_parms, uint32_t * array, int32_t div)
{
   uint32_t start;
   uint32_t end;
   uint32_t value;
   int      ret;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_uint32(**pp_parms, &value);
   if (ret != 0) return ret;

   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_start_end(pp_parms, num_parms, &start, &end);
   if (ret != 0) return ret;

   start = start/div;
   end = end/div;

   while (start <= end)
   {
      array[start++] = value;
   }

   return (0);
}


MOCALIB_CLI int mocacli_get_int32_array(char *** pp_parms, int * num_parms, int32_t * array, int32_t div)
{
   uint32_t start;
   uint32_t end;
   int32_t  value;
   int      ret;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_int32(**pp_parms, &value);
   if (ret != 0) return ret;

   *pp_parms = *pp_parms + 1;
   *num_parms = *num_parms - 1;

   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
      return (-1);

   ret = mocacli_get_start_end(pp_parms, num_parms, &start, &end);
   if (ret != 0) return ret;

   start = start/div;
   end = end/div;

   while (start <= end)
   {
      array[start++] = value;
   }

   return (0);
}

MOCALIB_CLI int mocacli_get_uint32_var_array(char *** pp_parms, int * num_parms, uint32_t * array, uint32_t size)
{
   uint32_t value;
   int      ret;
   int      total_params = 0;


   if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0) || (array == NULL))
      return (-1);

   memset (array, 0x0, size);

   while (total_params < (int)(size / sizeof(uint32_t)))
   {
      if ((*pp_parms == NULL) || (**pp_parms == NULL) || (*num_parms == 0))
         break;

      ret = mocacli_get_uint32(**pp_parms, &value);
      if (ret != 0) 
         return (total_params == 0);

      *pp_parms = *pp_parms + 1;
      *num_parms = *num_parms - 1;

      array[total_params++] = value;
   }

   return (total_params == 0);
}

MOCALIB_CLI int mocacli_is_node_connected(void * handle, int nodeid)
{
   int ret = 0;
   struct moca_network_status nws;

    if (nodeid >= MOCA_MAX_NODES )
      return (ret);
    ret = moca_get_network_status(handle, &nws);
    if (ret == 0)
     if (nws.connected_nodes & (0x1 << nodeid))
      return(1) ;

   return(ret);
} 

MOCALIB_CLI macaddr_t mocacli_node_to_mac(void * handle, uint32_t index)
{
   struct moca_gen_node_status gns;
   int ret = 0;

   memset(&gns, 0, sizeof(gns));
   
   if (index < MOCA_MAX_NODES)
      ret = moca_get_gen_node_status(handle, index, &gns);

   if (ret != 0)
      memset(&gns, 0, sizeof(gns));

   return(gns.eui);
}

MOCALIB_CLI uint32_t mocacli_find_uint32_t(char ** pp_parms, uint32_t num_parms, const char * p_string)
{
   int ret = 0;
   uint32_t value = 0;

   while ((ret == 0) && (num_parms > 0))
   {
      if (!strcmp(*pp_parms, p_string))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         ret = mocacli_get_uint32(*pp_parms, &value);

         return(value);
      }
      else
      {
         pp_parms++;
         num_parms--;
      }
   }

   return(value);
}


FN_COMMAND_HANDLER g_HelpHandler = NULL;

MOCALIB_CLI void mocacli_register_help_handler(FN_COMMAND_HANDLER pFn)
{
   g_HelpHandler = pFn;
}

static int mocacli_print_help( void );
static int mocacli_print_pvt_help( void );

MOCALIB_CLI int moca_print_help( void)
{
    mocacli_print_help();
    return(0);
}

MOCALIB_CLI int moca_print_pvt_help( void)
{
    mocacli_print_pvt_help();
    return(0);
}


MOCALIB_CLI int HelpHandler(void * handle, int argc, char **argv)
{
   int ret = 0;
   if (g_HelpHandler != NULL)
      ret = (g_HelpHandler)(handle, argc, argv);
   else
      ret = mocacli_print_help();

   return(ret);
}

#include "mocalib-cli-gen.c"

MOCALIB_CLI int mocacli_get_nvram_handler( void * handle, char ** pp_parms, int num_parms )
{
   int nRet = 0;
#if defined(STANDALONE)
   char *cli = SCRATCH_GetBuffer();
   uint32_t max_len = MAX_MOCA_SETTINGS;
#else
   static char cli[20480];
   uint32_t max_len = sizeof(cli);
#endif

   moca_write_nvram(handle, cli, max_len); 
   printf("%s \n", cli );

#if defined(STANDALONE)
   SCRATCH_FreeBuffer();
#endif

   return( nRet );
} /* GetNVHandler */

MOCALIB_CLI int mocacli_do_pqos_update_flow_handler( void * handle, char ** pp_parms, int num_parms )
{
   int ret = 0;
   struct moca_pqos_update_flow_out pqos_update_flow;
   struct moca_pqos_update_flow_in pqos_update_flow_in;
   struct moca_pqos_query_out pqos_query;
   int found_flow_id = 0;
   int first_param = 1;

   memset(&pqos_update_flow_in, 0, sizeof(pqos_update_flow_in));

   if ((num_parms != 0) && !strcmp(*pp_parms, "help"))
   {
      mocacli_do_pqos_update_flow_help();
      pp_parms++;
      num_parms--;
      return(0);
   }
   while ((ret == 0) && (num_parms > 0))
   {
      if (!strcmp(*pp_parms, "flow_id"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__flow_id_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_macaddr(*pp_parms, &pqos_update_flow_in.flow_id);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__flow_id_help();
               return(ret);
            }
            found_flow_id = 1;

            if (!first_param)
            {
               printf("Warning: flow_id must be the first option specified.\n");
            }
            ret = moca_do_pqos_query(handle, pqos_update_flow_in.flow_id, &pqos_query);
            if ((ret != 0) ||
                (pqos_query.response_code != MOCA_L2_SUCCESS))
            {
               printf("Flow ID %02x:%02x:%02x:%02x:%02x:%02x query error (%d/%d)\n", 
                  MOCA_DISPLAY_MAC(pqos_update_flow_in.flow_id), ret, pqos_query.response_code);
               return(0);
            }
            if (MOCA_MACADDR_IS_NULL(&pqos_query.packet_da))
            {
               printf("Flow ID %02x:%02x:%02x:%02x:%02x:%02x doesn't exist\n", 
                  MOCA_DISPLAY_MAC(pqos_update_flow_in.flow_id));
               return(0);
            }
            
            /* Set the default values to the current parameter settings */
            MOCA_MACADDR_COPY(&pqos_update_flow_in.ingress_mac, &pqos_query.ingress_node);
            MOCA_MACADDR_COPY(&pqos_update_flow_in.egress_mac, &pqos_query.egress_node);
            pqos_update_flow_in.packet_size = pqos_query.packet_size;
            pqos_update_flow_in.flow_tag = pqos_query.flow_tag;
            pqos_update_flow_in.peak_data_rate = pqos_query.peak_data_rate;
            pqos_update_flow_in.lease_time = (pqos_query.lease_time == 0) ? pqos_query.lease_time : pqos_query.lease_time_left;
            pqos_update_flow_in.burst_size = pqos_query.burst_size;
            pqos_update_flow_in.max_latency = pqos_query.max_latency;
            pqos_update_flow_in.short_term_avg_ratio = pqos_query.short_term_avg_ratio;
            pqos_update_flow_in.max_retry = pqos_query.max_retry;
            pqos_update_flow_in.flow_per = pqos_query.flow_per;
            pqos_update_flow_in.in_order_delivery = pqos_query.in_order_delivery;
            pqos_update_flow_in.traffic_protocol = pqos_query.traffic_protocol;
         }
      }
      else if (!strcmp(*pp_parms, "packet_size"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__packet_size_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.packet_size);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__packet_size_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "flow_tag"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__flow_tag_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.flow_tag);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__flow_tag_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "peak_data_rate"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__peak_data_rate_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.peak_data_rate);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__peak_data_rate_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "lease_time"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__lease_time_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.lease_time);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__lease_time_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "burst_size"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__burst_size_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.burst_size);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__burst_size_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "max_latency"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__max_latency_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.max_latency);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__max_latency_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "short_term_avg_ratio"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__short_term_avg_ratio_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.short_term_avg_ratio);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__short_term_avg_ratio_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "max_retry"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__max_retry_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.max_retry);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__max_retry_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "flow_per"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__flow_per_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.flow_per);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__flow_per_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "in_order_delivery"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__in_order_delivery_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.in_order_delivery);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__in_order_delivery_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "traffic_protocol"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__traffic_protocol_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &pqos_update_flow_in.traffic_protocol);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_pqos_update_flow__traffic_protocol_help();
               return(ret);
            }
         }
      }
      else if (!strcmp(*pp_parms, "ingress_mac"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         printf("Parameter %s Not Supported\n", *pp_parms);

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__ingress_mac_help();
            pp_parms++;
            num_parms--;
         }
         return(0);
      }
      else if (!strcmp(*pp_parms, "egress_mac"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         printf("Parameter %s Not Supported\n", *pp_parms);

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__egress_mac_help();
            pp_parms++;
            num_parms--;
         }
         return(0);
      }
      else if (!strcmp(*pp_parms, "reserved"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         printf("Parameter %s Not Supported\n", *pp_parms);

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_pqos_update_flow__reserved_help();
            pp_parms++;
            num_parms--;
         }
         return(0);
      }
      else
      {
         mocacli_do_pqos_update_flow_help();
         return(0);
      }

      first_param = 0;
   }

   if (!found_flow_id)
   {
      printf("Missing flow_id parameter.\n");
      return(0);
   }

   memset(&pqos_update_flow, 0, sizeof(pqos_update_flow));

   ret = moca_do_pqos_update_flow(handle, &pqos_update_flow_in, &pqos_update_flow);

   if (ret == 0)
      mocacli_print_pqos_update_flow(&pqos_update_flow);
   else
      printf("Error %d\n", ret);

   return(ret);
}


MOCALIB_CLI uint8_t mocacli_get_subcarrier(uint32_t * p_bit_loading, uint32_t sub_carrier)
{
   uint8_t value = 0;

   value = (uint8_t)(p_bit_loading[sub_carrier/8] >> (28 - ((sub_carrier % 8) * 4))) & 0xF;

   return(value);
}

MOCALIB_CLI void mocacli_print_subcarriers(int32_t start, int32_t end, uint32_t * p_bit_loading1, uint32_t * p_bit_loading2)
{
   int32_t dir;
   int32_t subCarrier1;
   int32_t subCarrier2;
   uint32_t print_nums = 1;
   if (start < end)
      dir = 1;
   else
      dir = -1;

   subCarrier2 = start;
   for (subCarrier1 = start; 
        start < end ? subCarrier1 <= end : subCarrier1 >= end; 
        subCarrier1 += dir) 
   {
      if (print_nums)
      {
         printf("%3d - %3d:  ", subCarrier1, subCarrier1 + (32 * dir) - dir );
         print_nums = 0;
      }
      printf ("%x", mocacli_get_subcarrier(p_bit_loading1, subCarrier1)) ;
      if (((subCarrier1 + (dir > 0 ? 1 : 0)) % 32) == 0) {
         if (p_bit_loading2 != NULL)
         {
            printf ("   ") ;
            /* Display the second Bit Loading */
            for (; subCarrier2 >= 0; subCarrier2 += dir) {
               printf ("%x", mocacli_get_subcarrier(p_bit_loading2, subCarrier2)) ;
               if (((subCarrier2 + (dir > 0 ? 1 : 0)) % 32) == 0) {
                  subCarrier2 += dir;
                  break ;
               }
            } /* for (secSubCarrier) */
         }
         printf ("\n") ;
         print_nums = 1;
      } /* if (subCarrier) */
   } /* for (subCarrier) */

}
   
MOCALIB_CLI void mocacli_print_bit_loading(uint32_t * p_bit_loading1, uint32_t * p_bit_loading2, uint32_t num_carriers)
{
   /* If the number of carriers is 256, it's 1.1 otherwise it's 2.0.
      We want to display the sub-carriers in order of increasing
      frequency. The array holds the sub-carriers starting with SC
      index 0. 
      For 1.1, the SCs should be displayed as: 127-0,255-128
      For 2.0, the SCs should be displayed as : 256-511, 0-255

      There are 8 sub-carriers per u32. */

   if (num_carriers == MOCA_MAX_SUB_CARRIERS_1_1)
   {
      mocacli_print_subcarriers(127, 0, p_bit_loading1, p_bit_loading2);
      mocacli_print_subcarriers(255, 128, p_bit_loading1, p_bit_loading2);
   }
   else if (num_carriers == MOCA_MAX_SUB_CARRIERS)
   {
      mocacli_print_subcarriers(256, 511, p_bit_loading1, p_bit_loading2);
      mocacli_print_subcarriers(0, 255, p_bit_loading1, p_bit_loading2);
   }
   else
   {
      printf("Unsupported number of sub-carriers %d\n", num_carriers);
   }

}

MOCALIB_CLI int  divByPrecisionFractional(int a_val, int a_precision)
{
   int l_tmp= (a_val < 0)?-a_val:a_val;
   return ((100*( (l_tmp) & ((1<<a_precision)-1) ))>> a_precision);
}



MOCALIB_CLI int mocacli_get_gen_node_ext_status_handler( void * handle, char ** pp_parms, int num_parms )
{
   int ret = 0;
   struct moca_gen_node_ext_status gen_node_ext_status;
   int found_index = 0;
   int found_profile_type = 0;
   struct moca_gen_node_ext_status_in gen_node_ext_status_in;
   struct moca_interface_status interface_status;
   uint32_t central_freq = 0;

   memset(&gen_node_ext_status_in, 0, sizeof(gen_node_ext_status_in));

   if ((num_parms != 0) && !strcmp(*pp_parms, "help"))
   {
      mocacli_get_gen_node_ext_status_help();
      pp_parms++;
      num_parms--;
      return(0);
   }
   while ((ret == 0) && (num_parms > 0))
   {
      if (!strcmp(*pp_parms, "index"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_gen_node_ext_status__index_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &gen_node_ext_status_in.index);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_gen_node_ext_status__index_help();
               return(ret);
            }
            found_index = 1;
         }
      }
      else if (!strcmp(*pp_parms, "profile_type"))
      {
         pp_parms++;
         num_parms--;

         if (num_parms == 0) continue;

         if (!strcmp(*pp_parms, "help"))
         {
            mocacli_gen_node_ext_status__profile_type_help();
            pp_parms++;
            num_parms--;
            return(0);
         }
         else
         {
            ret = mocacli_get_uint32(*pp_parms, &gen_node_ext_status_in.profile_type);
            pp_parms++;
            num_parms--;
            if (ret != 0)
            {
               mocacli_gen_node_ext_status__profile_type_help();
               return(ret);
            }
            found_profile_type = 1;
         }
      }
      else
      {
         mocacli_get_gen_node_ext_status_help();
         return(0);
      }

   }

   if (!found_index)
   {
      printf("Missing index parameter.\n");
      return(0);
   }
   if (!found_profile_type)
   {
      printf("Missing profile_type parameter.\n");
      return(0);
   }

   memset(&gen_node_ext_status, 0, sizeof(gen_node_ext_status));

   ret = moca_get_gen_node_ext_status(handle, &gen_node_ext_status_in, &gen_node_ext_status);

   if (MOCA_API_SUCCESS == moca_get_interface_status(handle, &interface_status))
   {
      // Profiles 18 and up are for the secondary channel
      if (MOCA_PROFILE_IS_ON_PRIMARY_CH(gen_node_ext_status_in.profile_type))
         central_freq = interface_status.primary_channel * 25;
      else
         central_freq = interface_status.secondary_channel * 25;
   }

   if (ret == 0 && (gen_node_ext_status.nbas != 0) )
   {
      printf("== gen_node_ext_status =============================== \n");
      printf("node           : %d  %02X:%02X:%02X:%02X:%02X:%02X\n",
         gen_node_ext_status_in.index, MOCA_DISPLAY_MAC(mocacli_node_to_mac(handle, gen_node_ext_status_in.index)));
      printf("profile_type   : %s (%u)\n", g_prfDesc[gen_node_ext_status_in.profile_type], gen_node_ext_status_in.profile_type);
      printf("central_freq   : %u MHz\n", central_freq);
      printf("nbas           : %u  ( 0x%x )\n", gen_node_ext_status.nbas, gen_node_ext_status.nbas);
      printf("preamble_type  : %u  ( 0x%x )\n", (gen_node_ext_status.preamble_type == 0 ? 1 : (gen_node_ext_status.preamble_type == 1 ? 1 : (gen_node_ext_status.preamble_type == 2 ? 2 : (gen_node_ext_status.preamble_type == 12 ? 0 : (gen_node_ext_status.preamble_type - 1))))) , (gen_node_ext_status.preamble_type == 0 ? 1 : (gen_node_ext_status.preamble_type == 1 ? 1 : (gen_node_ext_status.preamble_type == 2 ? 2 : (gen_node_ext_status.preamble_type == 12 ? 0 : (gen_node_ext_status.preamble_type - 1))))) );
      printf("cp             : %u  ( 0x%x )\n", gen_node_ext_status.cp, gen_node_ext_status.cp);
      printf("tx_power       : %d dBm \n", gen_node_ext_status.tx_power);
      printf("tx_backoff     : %d dB \n", gen_node_ext_status.tx_backoff);
      printf("rx_power       : %d.%02d dBm\n", (gen_node_ext_status.rx_power/ (1 << 4)), divByPrecisionFractional(gen_node_ext_status.rx_power, 4));
      printf("rx_backoff     : %d dB\n", gen_node_ext_status.rx_backoff);
      printf("agc address    : %u  ( 0x%x )\n", gen_node_ext_status.agc_address, gen_node_ext_status.agc_address);
      printf("avg_snr        : %d.%03d \n", gen_node_ext_status.avg_snr>>8, (gen_node_ext_status.avg_snr&0xFF)*1000 / 256);
      printf("phy_rate       : %d Mbps \n", gen_node_ext_status.phy_rate);
      printf("turbo_status   : %u  ( 0x%x )\n", gen_node_ext_status.turbo_status, gen_node_ext_status.turbo_status);
      printf("bit_loading[64]: \n");
      mocacli_print_bit_loading(&gen_node_ext_status.bit_loading[0], NULL, 
         (gen_node_ext_status_in.profile_type > MOCA_EXT_STATUS_PROFILE_TX_MAP ? MOCA_MAX_SUB_CARRIERS : MOCA_MAX_SUB_CARRIERS_1_1));
      printf("== end gen_node_ext_status =========================== \n\n");

   }
   else if (ret != 0)
      printf("Error %d\n", ret);
   
   return(ret);
}

#define BCM3450_PACNTL 0x18
#define BCM3450_LNACNTL  0x14
#define BCM3450_VERSION  0x4


int printRxGainExtended(void * handle, struct moca_drv_info *info)
{
  struct moca_rx_gain_params  rxGainParams={0x4924,0};
  struct moca_amp_reg    amp_reg;
  int ret=0; 
  
   if((info->hw_rev >=  MOCA_CHIP_20_3) || ((info->chip_id & 0xFFF000F0) == 0x680000C0))
   {
      ret = moca_get_rx_gain_params(handle, 0, &rxGainParams); 
      if(ret <0)
         return ret;
   }
   else
   {
      if(moca_get_amp_reg(handle, BCM3450_LNACNTL, &amp_reg) ==0)
      {
         rxGainParams.lna_ctrl_reg = amp_reg.value;
      }

      if(moca_get_amp_reg(handle, BCM3450_VERSION, &amp_reg) ==0)
      {
         rxGainParams.is3451 = (amp_reg.value & 0x100)>> 8;
      }
   }
   
   if(rxGainParams.is3451)
   {
      printf ("\n3451 Fields                                                              \n"); 
   }
   else
   {
      printf ("\n3450 Fields                                                              \n"); 
   }
   printf ("============                                                               \n");
   printf ("LNA IBIAS:                0x%04x\n",(rxGainParams.lna_ctrl_reg  & 0x7000)>>12);   /*14 -12*/
   printf ("LNA STG1 GM:         	  0x%04x\n",(rxGainParams.lna_ctrl_reg  & 0xe00)>>9);   /*11 -09*/
   printf ("LNA STG1 CASC BASE:       0x%04x\n",(rxGainParams.lna_ctrl_reg  & 0x1c0)>>6);   /*8 -6*/
   printf ("LNA STG2 AMP:             0x%04x\n",(rxGainParams.lna_ctrl_reg  & 0x38) >>3);   /*5 -3*/
   printf ("LNA STG2 BUF:             0x%04x\n",rxGainParams.lna_ctrl_reg   & 0x7 );   /*0 -2*/
   return ret;
}

MOCALIB_CLI_GET int mocacli_get_rx_gain_params_handler( void * handle, char ** pp_parms, int num_parms )
{
   int       ret = 0,ret1=0,tmp;
   uint32_t  rx_gain_agc_table[128],help=0;
   uint32_t  tableIndex=0,index, extended=0,bonding;
   struct moca_drv_info info;
   
   if(num_parms < 0)
   {
      mocacli_rx_gain_params__table_index_help();
      mocacli_rx_gain_agc_table__table_index_help();
      mocacli_get_rx_gain_agc_table_help();
   }

   
   while(num_parms != 0)
   {
      if (!strcmp(*pp_parms, "help"))
      {
         help=1;      
      }
      else if (!strcmp(*pp_parms, "extended"))
      {
         extended=1;
         ret=0;
      }
      else 
      {
         ret = mocacli_get_uint32(*pp_parms, &tableIndex);
      }

      pp_parms++;
      num_parms--;
      if((ret != 0) || (help))
      {
         mocacli_get_rx_gain_params_help();
         return ret;
      } 
   }
   
   if (handle != NULL)
   {
      ret = moca_get_bonding(handle, &bonding);
   
      if ((ret != 0) || (tableIndex > 3) || (bonding == 0))
      {
         tableIndex = 0;
      }

      ret1 = moca_get_drv_info(handle, 0, &info);  
      memset(&rx_gain_agc_table, 0, sizeof(rx_gain_agc_table));
      ret = __moca_get_rx_gain_agc_table(handle, &tableIndex, &rx_gain_agc_table[0], sizeof(rx_gain_agc_table));


      
      
      if ((ret >= 0) &&  (ret1 >= 0))
      {
         printf ("                             RX Gain Parameters                            \n");
         printf ("                           =======================                         \n");

         if(extended)
         {
            printRxGainExtended(handle, &info);      
         }

         printf ("\nTable Index: %d                                                            \n\n",tableIndex);

         if((info.hw_rev < MOCA_CHIP_20_3 ) && (extended == 0))
         {
            printf ("              3450                                       Soc                             \n") ;			
            printf ("========  =============        ==========================================================\n");
            printf ("|index |  | 3450 LNA  |        |RF PGA  GAIN   | IF LPF GAIN     |       IF PGA  GAIN   |\n") ;
            printf ("========  =============        ==========================================================\n");

            for (index = 0; index < 128; index++) 
            {

               printf ("| %04d |  | 0x%04x    |        |  0x%04x       |  0x%04x         |       0x%04x         |                                                        \n", 
                  index,
                  (rx_gain_agc_table[index] & 0x1e000) >> 13,
                  (rx_gain_agc_table[index] & 0x1e00) >> 9, /*9 - 12 (RFPGA)*/
                  (rx_gain_agc_table[index] & 0x1c0) >> 6, /*6 - 8 (IFLPF)*/
                  (rx_gain_agc_table[index] & 0x3f) ); /*0 - 5 (IFPGA)*/
               printf ("========  =============        ==========================================================\n");

            }					   
         }
         else if ((info.hw_rev < MOCA_CHIP_20_3 ) && (extended == 1))
         {
            printf ("              3450                                       Soc                             \n") ;			
            printf ("========  =============        ==========================================================================\n");
            printf ("|index |  | 3450 LNA  |        |RF PGA  GAIN   | IF LPF GAIN     |       IF PGA  GAIN   |Total Gain (dB)\n") ;
            
            printf ("========  =============        ==========================================================================\n");

            for (index = 0; index < 128; index++) 
            {
               tmp = (((rx_gain_agc_table[index] & 0x7FE0000)>>17) & (1 <<9))? -1 *  (0x400 -((rx_gain_agc_table[index] & 0x7FE0000)>>17)):((rx_gain_agc_table[index] & 0x7FE0000)>>17);
               int tmp1 = (tmp * 1881) / 10000;
               int tmp2 = (tmp * 1881) - tmp1 * 10000 ;
               tmp1 = (tmp <0)? -tmp1:tmp1;
               tmp2 = (tmp <0)? -tmp2:tmp2;

               printf ("| %04d |  | 0x%04x    |        |  0x%04x       |  0x%04x         |       0x%04x         | %c%d.%04d          \n",    
                  index,
                  (rx_gain_agc_table[index] & 0x1e000) >> 13,
                  (rx_gain_agc_table[index] & 0x1e00) >> 9, /*9 - 12 (RFPGA)*/
                  (rx_gain_agc_table[index] & 0x1c0) >> 6, /*6 - 8 (IFLPF)*/
                  (rx_gain_agc_table[index] & 0x3f),       /*0 - 5 (IFPGA)*/
                  (tmp < 0)?'-':' ',tmp1,tmp2); 

               printf ("========  =============        ==========================================================================\n");



            }					   
         }
         else if ((info.hw_rev >= MOCA_CHIP_20_3 ) && (extended == 1))
         {
            printf ("              3450                                       Soc                             \n") ;			
            printf ("========  =============        =========================================================================================\n");
            printf ("|index |  | 3450 LNA  |        |RF PGA  GAIN   | IF LPF GAIN     | IF MIXER  GAIN |     IF PGA  GAIN   |Total Gain (dB) \n") ;
            printf ("========  =============        =========================================================================================\n");
            for (index = 0; index < 128; index++) 
            {
               tmp = (((rx_gain_agc_table[index] & 0x1FF80000)>>19) & (1 <<9))? -1 *  (0x400 -((rx_gain_agc_table[index] & 0x1FF80000)>>19)):((rx_gain_agc_table[index] & 0x1FF80000)>>19);
               int tmp1 = (tmp * 1881) / 10000;
               int tmp2 = (tmp * 1881) - tmp1 * 10000 ;
               tmp1 = (tmp <0)? -tmp1:tmp1;
               tmp2 = (tmp <0)? -tmp2:tmp2;


               printf ("| %04d |  | 0x%04x    |        |  0x%04x       |  0x%04x         |   0x%04x       |     0x%04x         | %c%d.%04d           \n", 											
                  index,
                  (rx_gain_agc_table[index] & 0x78000) >> 15, /*18 - 15 (RFPGA)*/
                  (rx_gain_agc_table[index] & 0x7e00) >> 9,  /*9 - 14 (RFPGA)*/
                  (rx_gain_agc_table[index] & 0x180) >> 7,  /*7 - 8 (LPF_Gain0)*/
                  (rx_gain_agc_table[index] & 0x60)>>5 ,   /*6 - 5 (Mixer)*/ 
                  (rx_gain_agc_table[index] & 0x1f),       /*PGA_Gain0 0-4*/ 
                  (tmp < 0)?'-':' ',tmp1,tmp2); 
                
               printf ("========  =============        =========================================================================================\n");
             }	
         }
         else if ((info.hw_rev >= MOCA_CHIP_20_3 ) && (extended == 0))
         {
            printf ("              3450                                       Soc                             \n") ;			
            printf ("========  =============        =========================================================================\n");
            printf ("|index |  | 3450 LNA  |        |RF PGA  GAIN   | IF LPF GAIN     | IF MIXER  GAIN |     IF PGA  GAIN   \n") ;
            printf ("========  =============        =========================================================================\n");
            for (index = 0; index < 128; index++) 
            {
               printf ("| %04d |  | 0x%04x    |        |  0x%04x       |  0x%04x         |   0x%04x       |     0x%04x         \n", 											
                  index,
                  (rx_gain_agc_table[index] & 0x78000) >> 15, /*18 - 15 (RFPGA)*/
                  (rx_gain_agc_table[index] & 0x7e00) >> 9,  /*9 - 14 (RFPGA)*/
                  (rx_gain_agc_table[index] & 0x180) >> 7,  /*7 - 8 (LPF_Gain0)*/
                  (rx_gain_agc_table[index] & 0x60)>>5 ,   /*6 - 5 (Mixer)*/ 
                  (rx_gain_agc_table[index] & 0x1f)       /*PGA_Gain0 0-4*/ 
                  ); 
               printf ("========  =============        ==========================================================\n");
            }
         }
         else
         {

         }

         }
         else 
         {
            printf("Error %d %d \n", ret,ret1);
         }
   }
   return(ret);
}


MOCALIB_CLI_GET int mocacli_get_rx_gain_agc_table_handler( void * handle, char ** pp_parms, int num_parms )
{
   return 0;
}


MOCALIB_CLI_GET int mocacli_TxPowerGeneralParamsPrint( void * handle,struct moca_drv_info *info, struct moca_tx_power_params *a_tx_power_params)
{
  uint32_t  pa_ctrl_reg=0x4F16;
  uint32_t  is3451=0;
  struct moca_amp_reg  amp_reg;

                                             
   if((info->hw_rev >=  MOCA_CHIP_20_3) || ((info->chip_id & 0xFFF000F0) == 0x680000C0))
   {
      is3451 = a_tx_power_params->is3451;
      pa_ctrl_reg = a_tx_power_params->pa_ctrl_reg;
   }
   else
   {
      if(moca_get_amp_reg(handle, BCM3450_PACNTL, &amp_reg) ==0)
      {
         pa_ctrl_reg = amp_reg.value;
      }
      
      if(moca_get_amp_reg(handle, BCM3450_VERSION, &amp_reg) ==0)
      {
         is3451 = (amp_reg.value & 0x100)>> 8;
      }
   }
   if(is3451)
   {
      printf ("\n3451 General Fields                        \n"); 
   }
   else
   {
      printf ("\n3450 General Fields                        \n"); 
   }
   
   printf ("=====================                           \n");
   printf ("PA BIAS current:\t\t0x%04x\n",(pa_ctrl_reg & 0xC000000) >>26);		 
   printf ("PA In ATTN Off:\t\t\t0x%04x\n",(pa_ctrl_reg & 0x8000)>>15);		 
   printf ("PA RDEG:\t\t\t0x%04x\n", (pa_ctrl_reg &0x7800) >>11 );		 
   printf ("PA current CONT:\t\t0x%04x\n",(pa_ctrl_reg & 0x7E0) >>5);
   printf ("PA current FOLLOWER:\t\t0x%04x\n",(pa_ctrl_reg &0x1c) >>2 );
   printf ("PA OPAMP BOOST:\t\t\t0x%04x\n",(pa_ctrl_reg &2) >>1);
   return 0;
}



char *g_channelMode[3]={"BCN", "PRM", "SEC"};

MOCALIB_CLI_GET int mocacli_get_tx_power_params_handler( void * handle, char ** pp_parms, int num_parms )
{
   int ret = 0,ret1=0;
   struct moca_tx_power_params tx_power_params;
   int found_channel = 0;
   uint32_t extended=0,index,bonding;
   struct moca_tx_power_params_in  tx_power_params_in={0,0}; 
   struct moca_drv_info info;

   // Only for removing warning 
   if (num_parms < 0)
   {
      mocacli_tx_power_params__channelMode_help();
      mocacli_tx_power_params__txTableIndex_help();
   }

   if ((num_parms != 0) && !strcmp(*pp_parms, "help"))
   {
      mocacli_get_tx_power_params_help();
      pp_parms++;
      num_parms--;
      return(0);
   }

   while ((ret == 0) && (num_parms > 0))
   {
      uint32_t help=0;    
         if (!strcmp(*pp_parms, "help"))
         {
            help=1;    
         }
         else if (!strcmp(*pp_parms, "extended"))
         {
            extended=1;
            ret=0;
         }
         else if (!strcmp(*pp_parms, "channelMode"))
         {
            pp_parms++;
            num_parms--;
            ret = mocacli_get_uint32(*pp_parms, &tx_power_params_in.channelMode);
            found_channel = 1;
         }
         else if (!strcmp(*pp_parms, "txTableIndex"))
         {
            pp_parms++;
            num_parms--;
            ret = mocacli_get_uint32(*pp_parms, &tx_power_params_in.txTableIndex);
            ret1 = moca_get_bonding(handle, &bonding);

            if ((ret != 0) || (ret1 != 0) || ((tx_power_params_in.txTableIndex & 0xF) > 7) || (bonding==0))
            {
               tx_power_params_in.txTableIndex=0;
            }
         }
         else
         {
         }
         
         pp_parms++;
         num_parms--;
         
         if ((ret != 0) || (help))
            {
               mocacli_get_tx_power_params_help();
               return(ret);
            }
      }

   if (!found_channel)
   {
      printf("Missing channel parameter.\n");
      return(0);
   }

   if (handle != NULL)
   {
      memset(&tx_power_params, 0, sizeof(tx_power_params));

      ret = moca_get_tx_power_params(handle, &tx_power_params_in, &tx_power_params);
      ret1 = moca_get_drv_info(handle, 0, &info);  

      if ((ret == 0) && (ret1 == 0)){

         printf ("                             TX Power Parameters                           \n");
         printf ("                           =======================                         \n");
         
         if(extended)
         {
            mocacli_TxPowerGeneralParamsPrint(handle,&info,&tx_power_params);
         }
         
         printf ("\n\n");
         if((tx_power_params_in.txTableIndex &0xF0 >>4)) // Bonded 
         {
            printf ("Table Mode:\t\t\t%s\n","Bonded");
            printf ("Table Index:\t\t\t%d\n",tx_power_params_in.txTableIndex &0xF);
         }
         else  // Single 
         {
            printf ("%s channel:\t\t\t%d\n",g_channelMode[tx_power_params.channelMode], tx_power_params.channel);
            printf ("Table Mode:\t\t\t%s\n","Single");
            printf ("Table Index:\t\t\t%d\n",tx_power_params_in.txTableIndex &0xF);
            printf ("Max Tx power:\t\t\t%d\n",(-1 * tx_power_params.user_reduce_power) +3);
            printf ("Max TX channel tune:\t\t%d\n",-1*tx_power_params.channel_reduce_tune);		 
         }
         printf ("\n"); 
         

         if(info.hw_rev < MOCA_CHIP_20_3)
         {
               printf ("                3450                                       Soc                                   \n");		 
               printf ("==========    ============   ====================================================================\n");		 
               printf ("|Back off|    |3450 PA   |   | PA Driver Gain Control | PA Driver Max Gain |    TX Digital Gain |\n");
               printf ("==========    ============   ====================================================================\n");		

               for (index = 0; index < tx_power_params.table_max_index; index++) 
               {
                  if(index < (tx_power_params.user_reduce_power +  tx_power_params.channel_reduce_tune))
                  {
                     printf ("|        |    | 0x%04x   |   |      0x%04x            |     0x%04x         |     0x%04x         |\n",
                        tx_power_params.tx_table[index] >> 12,
                        tx_power_params.tx_table[index] &0xFF,
                        tx_power_params.pad_ctrl_deg,
                        ((tx_power_params_in.txTableIndex) && (tx_power_params.tx_table[index]==0))?0:tx_power_params.tx_digital_gain);
                  }
                  else
                  {
                     printf ("| %04d   |    | 0x%04x   |   |      0x%04x            |     0x%04x         |     0x%04x         |\n",
                        index - (tx_power_params.user_reduce_power +  tx_power_params.channel_reduce_tune),
                        tx_power_params.tx_table[index] >> 12,
                        tx_power_params.tx_table[index] &0xFF,
                        tx_power_params.pad_ctrl_deg,
                        ((tx_power_params_in.txTableIndex) && (tx_power_params.tx_table[index]==0))?0:tx_power_params.tx_digital_gain);
                  }
                  
                  printf ("==========    ============   ====================================================================\n");		 
               }
         }
         else
         {
            printf ("                3450                                             Soc                                             \n");		 
            printf ("==========    ============   =====================================================================================\n");		 
            printf ("|Back off|    |3450 PA   |   | DAC Coarse Power Back off Control |  DAC Gain Back off Control |  TX Digital Gain |\n");
            printf ("==========    ============   =====================================================================================\n");		

            for (index = 0; index < tx_power_params.table_max_index; index++) 
            {
               if(index < (tx_power_params.user_reduce_power +  tx_power_params.channel_reduce_tune))
               {
                  printf ("|        |    | 0x%04x   |   |      0x%04x                       |     0x%04x                 |         0x%04x   |\n",      
                     (tx_power_params.tx_table[index] & 0xF800)>> 11,
                     (tx_power_params.tx_table[index] & 0x7c0) >>6 ,
                     tx_power_params.tx_table[index]  & 0x3F,
                     tx_power_params.tx_digital_gain);

               }
               else
               {
                  printf ("| %04d   |    | 0x%04x   |   |      0x%04x                       |     0x%04x                 |         0x%04x   |\n",      
                     index - (tx_power_params.user_reduce_power +  tx_power_params.channel_reduce_tune),
                     (tx_power_params.tx_table[index] & 0xF800)>> 11,
                     (tx_power_params.tx_table[index] & 0x7c0) >>6 ,
                     tx_power_params.tx_table[index]  & 0x3F,
                     tx_power_params.tx_digital_gain);

               }
               printf ("==========    ============   =====================================================================================\n");		
            }

         }
         
      } else {
         printf("Error %d\n", ret);
      }
   }
   return(ret);
}


MOCALIB_CLI int mocacli_get_network_handler( void * handle, char ** pp_parms, int num_parms )
{
   int ret = 0;
  char **tmpPtr = pp_parms; 
   int nump = num_parms; 
   int i,j; 
   char pstr[4][MAX_STRING_LENGTH]; 
   char *gOptmp[4]; 


   while (nump > 0)
   {
       if ((nump != 0) && !strcmp(*tmpPtr, "help"))
      {
         printf("The Network group of parameters provide information about the MoCA network .\n");
         printf("IE list for this group: \n"); 
         printf("======================= \n"); 
         printf("listening_freq_mask\n"); 
         printf("taboo_channels\n"); 
         printf("gen_node_status\n"); 
         printf("gen_node_ext_status\n"); 
         printf("node_stats\n"); 
         printf("node_stats_ext\n"); 
         printf("network_status\n"); 
         printf("start_ulmo\n"); 
         return(0); 
      }
      tmpPtr++;
      nump--;
  }

   for( i = 0 ; i < 4; i++ )
   {
       gOptmp[i]=&pstr[i][0];
   }

  ret = mocacli_get_listening_freq_mask_handler(handle, pp_parms, num_parms );

  ret = mocacli_get_taboo_channels_handler(handle, pp_parms, num_parms );

  ret = mocacli_get_network_status_handler(handle, pp_parms, num_parms );

  ret = mocacli_get_start_ulmo_handler(handle, pp_parms, num_parms );

  for ( i = 0; i <= 15; i++ )
  {
     ret = mocacli_is_node_connected(handle,i); 
     if (ret != 0)   
     { 
         printf("\nnode: %d \n",i); 
         printf("============\n");
         sprintf(gOptmp[0],"%d", i);
         ret = mocacli_get_gen_node_status_handler(handle, gOptmp, 1);
         strcpy(gOptmp[0], "index");
         sprintf(gOptmp[1], "%d", i);
         ret = mocacli_get_node_stats_handler(handle, gOptmp, 2);
         ret = mocacli_get_node_stats_ext_handler(handle, gOptmp, 2);
         for( j = 0 ; j <= 17; j++ )
         {
      
           strcpy (gOptmp[0],"index"); 
           sprintf(gOptmp[1], "%d", i); 
           strcpy (gOptmp[2],"profile_type"); 
           sprintf(gOptmp[3], "%d", j); 
           ret = mocacli_get_gen_node_ext_status_handler(handle, gOptmp, 4 );
        }
    }
  }
   return(ret);
}

MOCALIB_CLI COMMAND_INFO * mocacli_get_cmds(int *ncmds)
{
   *ncmds = sizeof(g_Cmds)/sizeof(g_Cmds[0]);
   return(g_Cmds);
}


MOCALIB_CLI static int mocacli_get_init_handler(void * handle, char ** pp_parms, int num_parms)
{
   int i;
   int nRet = 0;

   if( (num_parms != 0) && !strcmp(*pp_parms, "help") )
   {
      printf("List the init parameters. \n");
      printf("mocap get --init <options>\n"); 
      printf("options:\n"); 
      printf("group  <group name> \n");
      pp_parms++;
      num_parms--;
      return(0);
   }
   if( (num_parms != 0) && !strcmp(*pp_parms, "group") )
   {
      pp_parms++;
      num_parms--;
      while ((nRet == 0) && (num_parms > 0)) 
      {
         printf("\n\n");
         printf("GROUP:  %s \n", *pp_parms ); 
         printf("======================================\n");
         for (i = 0; i < NUM_GET_INITCFG; i++)
         {
            if ( (!strcmp(*pp_parms, get_initcfg[i].grpname)) && (get_initcfg[i].binit == 1))
            {
               nRet = get_initcfg[i].fn(handle, pp_parms, num_parms);
            }
         }

         pp_parms++;
         num_parms--;
      }
      return(0);
   }
   for (i = 0; i < NUM_GET_INITCFG; i++)
   {
      if ( get_initcfg[i].binit == 1 )
      {
         nRet = get_initcfg[i].fn(handle, pp_parms, num_parms);
      }
   }
   return( nRet );
} /* mocacli_get_init_handler */

MOCALIB_CLI static int mocacli_get_config_handler(void * handle, char ** pp_parms, int num_parms)
{
   int i;
   int nRet = 0;

   if( (num_parms != 0) && !strcmp(*pp_parms, "help") )
   {
      printf("List the config parameters. \n");
      printf("mocap get --init <options>\n"); 
      printf("options:\n"); 
      printf("group  <group name> \n");
      pp_parms++;
      num_parms--;
      return(0);
   }
   if( (num_parms != 0) && !strcmp(*pp_parms, "group") )
   {
      pp_parms++;
      num_parms--;
      while ((nRet == 0) && (num_parms > 0)) 
      {
         printf("\n\n");
         printf("GROUP:  %s \n", *pp_parms ); 
         printf("======================================\n");
         for (i = 0; i < NUM_GET_INITCFG; i++)
         {
            if ( (!strcmp(*pp_parms, get_initcfg[i].grpname)) && (get_initcfg[i].bconfig == 1))
            {
               nRet = get_initcfg[i].fn(handle, pp_parms, num_parms);
            }
         }

         pp_parms++;
         num_parms--;
      }
      return(0);
   }
   for (i = 0; i < NUM_GET_INITCFG; i++)
   {
      if ( get_initcfg[i].bconfig == 1 )
      {
         nRet = get_initcfg[i].fn(handle, pp_parms, num_parms);
      }
   }

   return( nRet );
} /* mocacli_get_config_handler */

MOCALIB_CLI static int mocacli_get_group_handler(void * handle, char ** pp_parms, int num_parms)
{
   int i;
   int nRet = 0;

   if( (num_parms != 0) && !strcmp(*pp_parms, "help") )
   {
      printf("Group name: \n");
      printf("===========\n");
      for (i = 0; i < NUM_GET_GROUP; i++)
          printf("%s \n", getgrp_options[i].string ); 
      pp_parms++;
      num_parms--;
      return(0);
   }
   while( (nRet == 0) && (num_parms > 0) )
   {
      for (i = 0; i < NUM_GET_GROUP; i++)
      {
         if( !strcmp(*pp_parms, getgrp_options[i].string) )
         {
            printf("GROUP:  %s \n", getgrp_options[i].string ); 
            printf("================================================\n");
            nRet = getgrp_options[i].get_fn(handle, pp_parms, num_parms);
            break;
          }
      }

      if (i == NUM_GET_GROUP && strcmp(*pp_parms, "help")) {
         printf("Group '%s' not found.\n", *pp_parms);
         return(-1);
      }

      num_parms--;
      pp_parms++;
   }
   return( nRet );
} /* mocacli_get_group_handler */

MOCALIB_CLI static int mocacli_get_groupall_handler(void * handle, char ** pp_parms, int num_parms)
{
   int i;
   int nRet = 0;

   for (i = 0; i < NUM_GET_GROUP; i++)
   {
      if ( (strcmp(getgrp_options[i].string, "lab") != 0)) 
      {  
         printf("\n\n");
         printf("GROUP:  %s \n", getgrp_options[i].string ); 
         printf("================================================ \n");
         nRet = getgrp_options[i].get_fn(handle, pp_parms, num_parms);
      }
   }

   return( nRet );
} /* mocacli_get_groupall_handler */

MOCALIB_CLI static int getNumParms(int argc, char **argv)
{
   int i=0;
   while (argc--)
   {
      if ((argv[0][0] == '-') && (argv[0][1] == '-'))
         break;
      i++;
      argv++;
   }
   return(i);
}

MOCALIB_CLI static int mocacli_string_match(char * str1, char * str2)
{
   return(!strcmp(str1, str2) && (strlen(str1) == strlen(str2)));
}

MOCALIB_CLI static int mocacli_get_handler( void * handle, int argc, char **argv )
{
   int i;
   int nRet = 0;
   int numParms;
   int co = 0;

   while(argc)
   {
      if ((argv[0][1] != '-') || (argv[0][0] != '-'))
         return(co);
      numParms = getNumParms(argc-1, argv+1);
      co += numParms;
      for (i = 0; i < NUM_CLI_OPTIONS; i++)
      {
         if ( (cli_options[i].get_fn != NULL) &&
              (mocacli_string_match(argv[0], cli_options[i].string) ||
               ((cli_options[i].alias != NULL) && mocacli_string_match(argv[0], cli_options[i].alias))))
         {
            argv++;
            argc--;
            co++;
            nRet = cli_options[i].get_fn(handle, argv, numParms);
            break;
         }
      }

      if (i == NUM_CLI_OPTIONS) {
         printf("Option '%s' not found.\n", argv[0]);
         return(-1);
      }

      argv += numParms;
      argc -= numParms;
   }
   if (nRet == 0)
      return(co);
   return( nRet );
} /* mocacli_get_handler */

MOCALIB_CLI static int mocacli_set_handler( void * handle, int argc, char **argv )
{
   int i;
   int numParms;
   int stpcheck = 0;
   int checkStart = 0;
  
   char **tmpArgv = argv;
   int tmpArgc = argc;
   int nRet = 0;
   int co = 0;

   while (tmpArgc)
   {
      if (!strcmp(tmpArgv[0], "--stop") )
      {
         stpcheck = 1;
      }
      if( (!strcmp(tmpArgv[0], "--start" )) ||
          (!strcmp(tmpArgv[0], "--restart")) )
      {
         char * pStartCmd = tmpArgv[0];

         checkStart = 1;

         // Move this argument to the end of the list so
         // that restarts/starts are executed last
         if (tmpArgc > 1)
         {
            for (i = 0; i < (tmpArgc - 1); i++)
            {
               tmpArgv[i] = tmpArgv[i + 1];
            }
            tmpArgv[tmpArgc - 1] = pStartCmd;
         }
      }
      if ( (!strcmp(tmpArgv[0], "--rf_band" )) ||
           (!strcmp(tmpArgv[0], "--device_class" )))
      {
         if (tmpArgc > 1) 
         {
            char *arg0 = tmpArgv[0];
            char *arg1 = tmpArgv[1];

            // Move this argument and its parameter to the 
            //  beginning of the list.
            // Changes trigger a restore_default, but subsequent
            //  parameters are set properly

            for (i = argc-tmpArgc; i > 0; i--)
            {
               argv[i+1] = argv[i-1];
            }

            argv[0] = arg0;
            argv[1] = arg1;
         }
      }
      tmpArgc--;
      tmpArgv++;
   }

   while( (nRet == 0) && argc)
   {
      if ((argv[0][1] != '-') || (argv[0][0] != '-'))
         return(co);

      numParms = getNumParms(argc-1, argv+1);
      co += numParms;
      for (i = 0; i < NUM_CLI_OPTIONS; i++)
      {
         if ( (cli_options[i].set_fn != NULL) &&
              (mocacli_string_match(argv[0], cli_options[i].string) ||
               ((cli_options[i].alias != NULL) && mocacli_string_match(argv[0], cli_options[i].alias))))
         {
            argv++;
            argc--;
            co++;
            nRet = cli_options[i].set_fn(handle, argv, numParms);
            if (nRet != 0)  //if command failed move back to print the right error message below
              argv--;
            break;
         }
      }

      if (nRet!= 0)
      {
            printf("Warning: Command failed for Option '%s'.\n", argv[0]);
            if ((stpcheck == 1) && (checkStart == 1))
               nRet =0;
      }

      if (i == NUM_CLI_OPTIONS) {
         printf("Option '%s' not found.\n", argv[0]);
         return(-1);
      }

      argv += numParms;
      argc -= numParms;
   }

   if ( checkStart == 0 && gCheckinit == 1 )
   {
      mocacli_print_init_note();
   }
   if (nRet == 0)
      return(co);
   return( nRet );
} /* mocacli_set_handler */

MOCALIB_CLI static int mocacli_do_handler( void * handle, int argc, char **argv )
{
   int i;
   int nRet = 0;
   int numParms;
   int co = 0;

   while( (nRet == 0) && argc)
   {
      if ((argv[0][1] != '-') || (argv[0][0] != '-'))
         return(co);

      numParms = getNumParms(argc-1, argv+1);
      co += numParms;
      for (i = 0; i < NUM_CLI_OPTIONS; i++)
      {
         if ( (cli_options[i].do_fn != NULL) &&
              (mocacli_string_match(argv[0], cli_options[i].string) ||
               ((cli_options[i].alias != NULL) && mocacli_string_match(argv[0], cli_options[i].alias))))
         {
            argv++;
            argc--;
            co++;

            nRet = cli_options[i].do_fn(handle, argv, numParms);
            break;
         }
      }

      if (i == NUM_CLI_OPTIONS) {
         printf("Option '%s' not found.\n", argv[0]);
         return(-1);
      }

      argv += numParms;
      argc -= numParms;
   }

   if (nRet == 0)
      return(co);
   return( nRet );

} /* DoHandler */


