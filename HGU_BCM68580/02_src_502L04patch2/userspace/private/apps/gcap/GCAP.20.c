#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include <mocalib-cli.h>
#include <mocaint.h>

#include "GCAP_Common.h"

#ifndef TRUE
#define TRUE  (1==1)
#define FALSE (!TRUE)
#endif

static char *chipId = NULL;    // -i option
static macaddr_t bcast_mac = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
static macaddr_t bcast_pqos_mac = {{0x3f, 0x00, 0x00, 0x00, 0x00, 0x00}};


GCAP_GEN static void pqos_create_response_print(void *arg, struct moca_pqos_create_flow_out *data)
{
   if (data->response_code == MOCA_L2_SUCCESS)
   {
      printf(" DECISION                 : ");
      printf("%s\n", moca_decision_string(data->decision));
      printf(" MAX_PEAK_DATA_RATE       : %d kbps\n", data->peak_data_rate);
      printf(" POST_STPS                : %d\n", data->total_stps);
      printf(" POST_TXPS                : %d\n", data->total_txps);
      printf(" BW_LIMIT_INFO            : %d\n", data->bw_limit_info);
      printf(" MAX_BURST_SIZE           : %d\n", data->burst_size);
      printf(" MAX_SHORT_TERM_AVG_RATIO : %d\n", data->max_short_term_avg_ratio);
   }
   else
   {
      printf(" Error! %s\n", 
         (moca_l2_error_name(data->response_code) == NULL) ? "" : moca_l2_error_name(data->response_code));
      printf(" DECISION                 : ");
      printf("%s\n", moca_decision_string(data->decision));
   }
}


GCAP_GEN static void pqos_update_response_print(void *arg, struct moca_pqos_update_flow_out *data)
{
   if (data->response_code == MOCA_L2_SUCCESS)
   {
      printf(" DECISION                 : ");
      printf("%s\n", moca_decision_string(data->decision));
      printf(" MAX_PEAK_DATA_RATE       : %d kbps\n", data->peak_data_rate);
      printf(" POST_STPS                : %d\n", data->total_stps);
      printf(" POST_TXPS                : %d\n", data->total_txps);
      printf(" BW_LIMIT_INFO            : %d\n", data->bw_limit_info);
      printf(" MAX_BURST_SIZE           : %d\n", data->burst_size);
      printf(" MAX_SHORT_TERM_AVG_RATIO : %d\n", data->max_short_term_avg_ratio);
   }
   else
   {
      printf(" Error! %s\n", 
         (moca_l2_error_name(data->response_code) == NULL) ? "" : moca_l2_error_name(data->response_code));
      printf(" DECISION                 : ");
      printf("%s\n", moca_decision_string(data->decision));
   }
}


GCAP_GEN static void print_create_parameters(void)
{
   printf("\"create\" and \"update\" Parameters:\n");
   printf("flow_id     Flow ID in MAC address format, default value is 01:00:5e:00:01:00\n");
   printf("psize       Packet size in bytes, default value is 800 bytes\n");
   printf("ig          Ingress Node GUID, unique identifier of node in MAC address format\n");
   printf("               (01:23:45:67:89:ab) at which flow will enter MoCA network.\n");
   printf("               default is 00:00:00:00:00:00\n");
   printf("eg          Egress Node GUID, unique identifier of node in MAC address format\n");
   printf("               (01:23:45:67:89:ab) at which flow exits MoCA network, or multicast.\n");
   printf("               default is 00:00:00:00:00:00\n");
   printf("peak        Peak data rate in kbps, default value is 1000 kbps\n");
   printf("opaque      Flow Tag, default value is 0x0\n");
   printf("unicast_da  Unicast MAC DA, MAC address of a physical node to receive the flow;\n");
   printf("               MoCA network must have learned it; If not supplied, the packet DA in the\n");
   printf("               flow create request defaults to the multicast flow ID\n");
   printf("time        Lease Time in seconds, infinite lease time if 0\n");
   printf("               default value is 0\n");
   printf("burst       Burst count, number of packets over which to measure bursts,\n");
   printf("               default value is 2 packets\n");
   printf("ml          Maximum latency in ms, ranges from 1 to 255, or 0 for none specified\n");
   printf("               default value is 0\n");
   printf("star        Short Term Average Ratio, default value is 0\n");
   printf("mnr         Maximum Number Retry for MSDUs, default value is 0\n");
   printf("fp          Flow PER, 0 - NPER   1 - VLPER, default is 0\n");
   printf("idr         In order Delivery Recommended, 0 - no info, 1 - no, 2 - yes, 3 - reserved\n");
   printf("               default is 0\n");
   printf("tp          Traffic Protocol, 0 - no info, 1 - UDP, 2 - TCP, 3 - RTP/UDP,\n");
   printf("                 4 - MPEG-TS/UDP, 5 - HTTP/TCP\n");
   printf("                 default is 0\n");
   printf("icr         Ingress Classification Rule, 0 - DA+VLAN (VLAN tag must be 4 or 5),\n");
   printf("                 4 - DA only, 5 - DA+DSCP, 6 - DA+VLAN,\n");
   printf("                 7 - VlanTagged ? DA+VLAN : DA+DSCP\n");
   printf("                 default is 0\n");
   printf("vt          VLAN Tag, tage on MSDU to match for flow classification, subject to icr\n");
   printf("                 default is 0\n");
   printf("dm          DSCP TOS field match for flow classification, subject to icr\n");
   printf("                 default is 0\n");
}
GCAP_GEN static void print_create_help(void)
{
   printf("\"create\" command syntax:\n");
   printf("create [flow_id=xx:xx:xx:xx:xx:xx] [psize=#] ig=xx:xx:xx:xx:xx:xx eg=xx:xx:xx:xx:xx:xx [opaque=#] "
             "[unicast_da=xx:xx:xx:xx:xx:xx] [peak=#] [time=#] [burst=#] [ml=#] [star=#] [mnr=#] [fp=#] [idr=#] "
             "[tp=#] [icr=#] [vt=#] [dm=#]\n\n");
   print_create_parameters();
}

GCAP_GEN static void print_update_help(void)
{
   printf("\"update\" command syntax:\n");
   printf("update [flow_id=xx:xx:xx:xx:xx:xx] [psize=#] [opaque=#] [unicast_da=xx:xx:xx:xx:xx:xx] [peak=#] "
             "[time=#] [burst=#] [ml=#] [star=#] [mnr=#] [fp=#] [idr=#] [tp=#] [icr=#] [vt=#] [dm=#]\n\n");
   print_create_parameters();
}

GCAP_GEN static void print_help(char * cmd)
{
   if ((cmd != NULL) && (strcmp(cmd, "create") == 0))
   {
      print_create_help();
   }
   else if ((cmd != NULL) && (strcmp(cmd, "update") == 0))
   {
      print_update_help();
   }
   else
   {
      printf("GCAP.20  [-h|--help]  [Command]\n");
      printf("GCAP.20  [-v]  <Command>\n");
      printf("Commands:\n");
      printf("   create    Create QoS Flow\n");
      printf("   update    Update QoS Flow\n");
   }
}
 

GCAP_GEN int GCAP_20_main(int argc, char **argv)
{
   int ret;
   void *ctx;
   int create = 0;
   struct moca_pqos_create_flow_in  pqosc_in;
   struct moca_pqos_create_flow_out pqosc_out;
   struct moca_pqos_update_flow_in  pqosu_in;
   struct moca_pqos_update_flow_out pqosu_out;
   uint32_t *pulValue ;
   macaddr_t *pmacAddr;
   uint32_t packetDAPresent = FALSE;
   uint32_t igPresent = FALSE;
   uint32_t egPresent = FALSE;
   uint32_t igFound = FALSE;
   uint32_t egFound = FALSE;
   int   i;
   char * pStrVal;
   char * pStrOption;
   struct moca_gen_node_status gns;
   struct moca_interface_status if_status;
   struct moca_network_status nws;
   uint32_t node_id;
   uint32_t cmdIndex = 1;
 
   chipId = NULL;

   if ((argc >= 2) && (strcmp(argv[1], "--help") == 0))
   {
      if (argc >= 3)
         print_help(argv[2]);
      else
         print_help(NULL);

      return(0);
   }
#if defined(STANDALONE)

   uint32_t parmsIdx = 1;
   for (parmsIdx=1; parmsIdx < argc; parmsIdx++)
   {
         if (strcmp(argv[parmsIdx], "-i") == 0)
         {
            chipId = argv[parmsIdx+1];
            parmsIdx += 2;
         }
         else if (strcmp(argv[parmsIdx], "-v") == 0)
         {
            // This is a stupid flag whose behavior is undefined by the CTP
            parmsIdx++;
         }
         else if (strcmp(argv[parmsIdx], "?") == 0)
         {
            printf( "Error!  Invalid option - %s\n", argv[parmsIdx]);
            return(-1);
         }
         else if (strcmp(argv[parmsIdx], "-h") == 0)
         {
            print_help(argv[parmsIdx+1]);
            return(0); 
         }
   }
#else

    
   moca_gcap_init();

   // ----------- Parse parameters
   opterr = 0;

   while((ret = getopt(argc, argv, "h:i:v")) != -1) 
   {
      switch(ret) 
      {
         case 'i':
            chipId = optarg;
            cmdIndex += 2;
            break;
         case '?':
            if (optopt == 'h')
               print_help(NULL);
            else
               printf( "Error!  Invalid option - %c\n", optopt);
            return(-2);
            break;
         case 'v':
            // This is a stupid flag whose behavior is undefined by the CTP
            cmdIndex++;
            break;
         case 'h':            
         default:
            print_help(optarg);
            return(0); 
      }
   }
#endif

   if (!((argc - cmdIndex) > 0))
   {
      printf( "Error!  Missing command - create/update\n");
      return(-1);
   }

   // ----------- Initialize

   ctx = moca_open(chipId);

   if (!ctx)
   {
      printf( "Error!  Unable to connect to moca instance\n");
      return(-3);
   }

    ret = moca_get_interface_status(ctx, &if_status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure\n");
        moca_close(ctx);
        return(-4);
    }

    if (if_status.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-5);
    }

   if (strcmp(argv[cmdIndex], "create") == 0)
   {
      create = 1;
      memset(&pqosc_in, 0, sizeof(pqosc_in));
      MOCA_SET_PQOS_CREATE_FLOW_FLOW_ID_DEF(pqosc_in.flow_id);
      MOCA_SET_PQOS_CREATE_FLOW_PACKET_DA_DEF(pqosc_in.packet_da);
      pqosc_in.packet_size = MOCA_PQOS_CREATE_FLOW_PACKET_SIZE_DEF;
      pqosc_in.flow_tag = MOCA_PQOS_CREATE_FLOW_FLOW_TAG_DEF;
      pqosc_in.peak_data_rate = MOCA_PQOS_CREATE_FLOW_PEAK_DATA_RATE_DEF;
      pqosc_in.lease_time = MOCA_PQOS_CREATE_FLOW_LEASE_TIME_DEF;
      pqosc_in.burst_size = MOCA_PQOS_CREATE_FLOW_BURST_SIZE_DEF;
      pqosc_in.vlan_id = MOCA_PQOS_CREATE_FLOW_VLAN_ID_DEF;
      pqosc_in.max_latency = MOCA_PQOS_CREATE_FLOW_MAX_LATENCY_DEF;
      pqosc_in.short_term_avg_ratio = MOCA_PQOS_CREATE_FLOW_SHORT_TERM_AVG_RATIO_DEF;
      pqosc_in.max_retry = MOCA_PQOS_CREATE_FLOW_MAX_RETRY_DEF;
      pqosc_in.flow_per = MOCA_PQOS_CREATE_FLOW_FLOW_PER_DEF;
      pqosc_in.in_order_delivery = MOCA_PQOS_CREATE_FLOW_IN_ORDER_DELIVERY_DEF;
      pqosc_in.traffic_protocol = MOCA_PQOS_CREATE_FLOW_TRAFFIC_PROTOCOL_DEF;
      pqosc_in.ingr_class_rule = MOCA_PQOS_CREATE_FLOW_INGR_CLASS_RULE_DEF;
      pqosc_in.vlan_tag = MOCA_PQOS_CREATE_FLOW_VLAN_TAG_DEF;
      pqosc_in.dscp_moca = MOCA_PQOS_CREATE_FLOW_DSCP_MOCA_DEF;
   }
   else if (strcmp(argv[cmdIndex], "update") == 0)
   {
      macaddr_t query;
      struct moca_pqos_query_out query_rsp;
      
      create = 0;
      memset(&pqosu_in, 0x0, sizeof(pqosu_in));

      // Find the flow_id and query the existing flow
      i = cmdIndex + 1;
      while (i < argc)
      {
         pStrVal = NULL;
         
         if (!strcmp(argv[i], "flow_id"))
         {
            i++;
            pStrVal = argv[i];
         }
         else if ((strstr(argv[i], "flow_id") != NULL) &&
                  (strchr(argv[i], '=') != NULL))
         {
            pStrVal = strchr(argv[i], '=') + 1;
         }
         
         if (pStrVal != NULL)
         { 
            if ( mocacli_get_macaddr (pStrVal, &query) != 0 )
            {
               printf("Error!  Invalid parameter - flow_id\n");
               moca_close(ctx);
               return(-6);
            }
            else
            {
               memcpy(&pqosu_in.flow_id, &query, sizeof(macaddr_t));

               ret = moca_do_pqos_query(ctx, query, &query_rsp);
               if (ret != MOCA_API_SUCCESS)
               {
                  printf("Error!  Query failed\n");
                  moca_close(ctx);
                  return(-7);
               }
            
               if ((query_rsp.response_code == MOCA_L2_SUCCESS) &&
                   ((query_rsp.packet_da.addr[0] != 0) ||
                    (query_rsp.packet_da.addr[1] != 0) ||
                    (query_rsp.packet_da.addr[2] != 0) ||
                    (query_rsp.packet_da.addr[3] != 0) ||
                    (query_rsp.packet_da.addr[4] != 0) ||
                    (query_rsp.packet_da.addr[5] != 0)))
               {
                  MOCA_MACADDR_COPY(&pqosu_in.ingress_mac, &query_rsp.ingress_node);
                  MOCA_MACADDR_COPY(&pqosu_in.egress_mac, &query_rsp.egress_node);
                  pqosu_in.burst_size = query_rsp.burst_size;
                  pqosu_in.flow_tag = query_rsp.flow_tag;
                  if (query_rsp.lease_time_left != 0xFFFFFFFF)
                     pqosu_in.lease_time = query_rsp.lease_time_left;
                  pqosu_in.packet_size = query_rsp.packet_size;
                  pqosu_in.peak_data_rate = query_rsp.peak_data_rate;
                  pqosu_in.max_latency = query_rsp.max_latency;
                  pqosu_in.short_term_avg_ratio = query_rsp.short_term_avg_ratio;
                  pqosu_in.max_retry = query_rsp.max_retry;
                  pqosu_in.flow_per = query_rsp.flow_per;
                  pqosu_in.in_order_delivery = query_rsp.in_order_delivery;
                  pqosu_in.traffic_protocol = query_rsp.traffic_protocol;
               }
               else
               {
                  printf(" DECISION                 : DECISION_FLOW_NOT_FOUND\n");
                  moca_close(ctx);
                  return(-8);
               }
            }
            break;
         }

         i++;
      }
   }
   else
   {
      printf("Error!  Invalid command - %s\n", argv[cmdIndex]);
      moca_close(ctx);
      return(-9);
   }
        
   i = cmdIndex + 1;
   while( (ret == MOCA_API_SUCCESS) && (i < argc) )
   {
      pulValue = NULL;
      pmacAddr = NULL;
      pStrOption = argv[i];

      if( !strcmp(argv[i], "ig") ) 
      {
         igPresent = TRUE;
         pmacAddr = &pqosc_in.ingress_node;
         i++;
         pStrVal = argv[i];

         if (strchr(pStrVal, ':') == NULL)
         {
            pulValue = &node_id;
         }
      }
      else if (strstr(argv[i], "ig=") != NULL)
      {
         igPresent = TRUE;
         pmacAddr = &pqosc_in.ingress_node;
         pStrVal = strchr(argv[i], '=') + 1;

         if (strchr(pStrVal, ':') == NULL)
         {
            pulValue = &node_id;
         }
      }
      else if( !strcmp(argv[i], "eg") )
      {
         egPresent = TRUE;
         pmacAddr = &pqosc_in.egress_node;
         i++;
         pStrVal = argv[i];

         if (strchr(pStrVal, ':') == NULL)
         {
            pulValue = &node_id;
         }
      }
      else if (strstr(argv[i], "eg=") != NULL)
      {
         egPresent = TRUE;
         pmacAddr = &pqosc_in.egress_node;
         pStrVal = strchr(argv[i], '=') + 1;

         if (strchr(pStrVal, ':') == NULL)
         {
            pulValue = &node_id;
         }
      }
      else if( !strcmp(argv[i], "flow_id") )
      {
         // Will already be set for update operation
         pmacAddr = &pqosc_in.flow_id;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "flow_id=") != NULL)
      {
         pmacAddr = &pqosc_in.flow_id;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "packet_da") )
      {
         // Will already be set for update operation
         packetDAPresent = TRUE;
         pmacAddr = &pqosc_in.packet_da;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "unicast_da=") != NULL)
      {
         packetDAPresent = TRUE;
         pmacAddr = &pqosc_in.packet_da;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "peak") )
      {
         pulValue = create ? &pqosc_in.peak_data_rate : &pqosu_in.peak_data_rate;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "peak=") != NULL)
      {
         pulValue = create ? &pqosc_in.peak_data_rate : &pqosu_in.peak_data_rate;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "psize") )
      {
         pulValue = create ? &pqosc_in.packet_size : &pqosu_in.packet_size;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "psize=") != NULL)
      {
         pulValue = create ? &pqosc_in.packet_size : &pqosu_in.packet_size;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "burst") )
      {
         pulValue = create ? &pqosc_in.burst_size : &pqosu_in.burst_size;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "burst=") != NULL)
      {
         pulValue = create ? &pqosc_in.burst_size : &pqosu_in.burst_size;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "time") )
      {
         pulValue = create ? (uint32_t *)&pqosc_in.lease_time : (uint32_t *)&pqosu_in.lease_time;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "time=") != NULL)
      {
         pulValue = create ? (uint32_t *)&pqosc_in.lease_time : (uint32_t *)&pqosu_in.lease_time;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "opaque") )
      {
         pulValue = create ? &pqosc_in.flow_tag : &pqosu_in.flow_tag;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "opaque=") != NULL)
      {
         pulValue = create ? &pqosc_in.flow_tag : &pqosu_in.flow_tag;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "ml") )
      {
         pulValue = create ? &pqosc_in.max_latency : &pqosu_in.max_latency;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "ml=") != NULL)
      {
         pulValue = create ? &pqosc_in.max_latency : &pqosu_in.max_latency;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "star") )
      {
         pulValue = create ? &pqosc_in.short_term_avg_ratio : &pqosu_in.short_term_avg_ratio;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "star=") != NULL)
      {
         pulValue = create ? &pqosc_in.short_term_avg_ratio : &pqosu_in.short_term_avg_ratio;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "mnr") )
      {
         pulValue = create ? &pqosc_in.max_retry : &pqosu_in.max_retry;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "mnr=") != NULL)
      {
         pulValue = create ? &pqosc_in.max_retry : &pqosu_in.max_retry;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "fp") )
      {
         pulValue = create ? &pqosc_in.flow_per : &pqosu_in.flow_per;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "fp=") != NULL)
      {
         pulValue = create ? &pqosc_in.flow_per : &pqosu_in.flow_per;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "idr") )
      {
         pulValue = create ? &pqosc_in.in_order_delivery : &pqosu_in.in_order_delivery;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "idr=") != NULL)
      {
         pulValue = create ? &pqosc_in.in_order_delivery : &pqosu_in.in_order_delivery;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "tp") )
      {
         pulValue = create ? &pqosc_in.traffic_protocol : &pqosu_in.traffic_protocol;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "tp=") != NULL)
      {
         pulValue = create ? &pqosc_in.traffic_protocol : &pqosu_in.traffic_protocol;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "icr") )
      {
         pulValue = &pqosc_in.ingr_class_rule;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "icr=") != NULL)
      {
         pulValue = &pqosc_in.ingr_class_rule;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "vt") )
      {
         pulValue = &pqosc_in.vlan_tag;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "vt=") != NULL)
      {
         pulValue = &pqosc_in.vlan_tag;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if( !strcmp(argv[i], "dm") )
      {
         pulValue = &pqosc_in.dscp_moca;
         i++;
         pStrVal = argv[i];
      }
      else if (strstr(argv[i], "dm=") != NULL)
      {
         pulValue = &pqosc_in.dscp_moca;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else 
      {
         ret = MOCA_API_ERROR;

         printf("Error!  Invalid parameter - %s\n",argv[i]);
         moca_close(ctx);
         return(-10);
      }

      /* The pulValue check must be first because if the ingress or egress
         node is specified using a node ID, both pulValue and pmacAddr
         will be valid pointers. */
      if ( pulValue )
      {
         /* Convert the value for a particular option to an integer. */
         char *pszEnd = NULL;

         if (pulValue)
            *pulValue = (long) strtoul( pStrVal, &pszEnd, 0 ) ;

         if( *pszEnd != '\0' )
         {
            ret = MOCA_API_ERROR;
            printf( "Error!  Invalid parameter for option %s\n", 
               pStrOption ) ;
         }

         if (pmacAddr)
         {
            /* A node ID was specified, but we need to find the MAC address that
               goes with it */
            ret = moca_get_gen_node_status(ctx, *pulValue, &gns);
            if (ret != MOCA_API_SUCCESS)
            {
               printf( "Error!  Invalid node ID %d\n", 
                  *pulValue ) ;
            }
            else
            {
               memcpy(pmacAddr, &gns.eui, sizeof(macaddr_t));
               pmacAddr = NULL; // Clear so that we don't hit the check below
            }
         }
         
         i++;
      }
      if ( pmacAddr )
      {
         if ( mocacli_get_macaddr(pStrVal, pmacAddr) != 0 )
         {
            printf( "Error!  Invalid parameter for option %s\n", 
               pStrOption ) ;
            ret = MOCA_API_ERROR;
         }

         i++;
      }
   }

   if( ret == MOCA_API_SUCCESS ) {

      if (create && !(egPresent && igPresent))
      {
         ret = MOCA_API_ERROR;
         printf( "Error!  Missing mandatory parameter eg or ig\n");
      }
      else if (create)
      {
         /* Make sure that the egress MAC addr exists on the network or 
            that it is the broadcast mac addr of 3f:00:00:00:00:00. */
         if ((memcmp(&bcast_mac, &pqosc_in.egress_node, sizeof(macaddr_t)) == 0) ||
             (memcmp(&bcast_pqos_mac, &pqosc_in.egress_node, sizeof(macaddr_t)) == 0))
         {
            egFound = TRUE;
         }

         ret = moca_get_network_status(ctx, &nws);
         if (ret != MOCA_API_SUCCESS)
         {
            printf( "Error!  Internal error 2\n");
            moca_close(ctx);
            return(-11);
         }

         for (i = 0; 
              nws.connected_nodes; 
              nws.connected_nodes >>= 1, i++)
         {
            if (nws.connected_nodes & 0x1)
            {
               if (moca_get_gen_node_status(ctx, i, &gns) == 0)
               {
                  if(memcmp(&gns.eui, &pqosc_in.egress_node, sizeof(macaddr_t)) == 0)
                  {
                     egFound = TRUE;
                  }
                  if(memcmp(&gns.eui, &pqosc_in.ingress_node, sizeof(macaddr_t)) == 0)
                  {
                     igFound = TRUE;
                  }
               }
            }
         }

         if (!igFound)
         {
            printf( "Error!  No such ingress node MAC address\n");
            fprintf( stderr, " DECISION                 : %s\n", moca_decision_string(MOCA_PQOS_DECISION_INVALID_TSPEC));
            moca_close(ctx);
            return(-12);
         }

         if (!egFound)
         {
            printf( "Error!  No such egress node MAC address\n");
            fprintf( stderr, " DECISION                 : %s\n", moca_decision_string(MOCA_PQOS_DECISION_INVALID_TSPEC));
            moca_close(ctx);
            return(-13);
         }

         if (!packetDAPresent)
         {
            memcpy(&pqosc_in.packet_da, &pqosc_in.flow_id, sizeof(macaddr_t));
         }

         ret = moca_do_pqos_create_flow(ctx, &pqosc_in, &pqosc_out);

         if (ret == MOCA_API_SUCCESS)
            pqos_create_response_print(ctx, &pqosc_out);
         else if (ret == MOCA_API_INVALID_TYPE)
            printf( "Error!  Invalid ingress node ID\n");
         else 
            printf( "Error!  Unable to create\n");
      }
      else
      {
         ret = moca_do_pqos_update_flow(ctx, &pqosu_in, &pqosu_out);
         if (ret == MOCA_API_SUCCESS) {
            pqos_update_response_print(ctx, &pqosu_out);
         }
      }
   }


   // ----------- Finish
   moca_close(ctx);

   return(0);
}
