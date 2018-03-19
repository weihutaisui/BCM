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
#include "GCAP_Common.h"
#include <mocaint.h>

static char *chipId = NULL;    // -i option

char * ingr_class_rules[8] = 
{
   "DA & VLAN = 4 or 5",
   "(unknown)",
   "(unknown)",
   "(unknown)",
   "DA only",
   "DA & DSCP",
   "DA & VLAN",
   "DA & VLAN or DSCP"
};


GCAP_GEN static void print_query_help(void)
{
   printf("\"query\" command syntax:\n");
   printf("   query [flow_id=xx:xx:xx:xx:xx:xx]\n");
   printf("\"query\" Parameters:\n");
   printf("   flow_id     Flow (in MAC address format) to query on MoCA network.\n");
   printf("                  If no MAC address is specified, the default value is 01:00:5e:00:01:00\n");
}

GCAP_GEN static void print_queryall_help(void)
{
   printf("\"queryall\" command syntax:\n");
   printf("   queryall [tries = #] [maxlist = #]\n");
   printf("\"queryall\" Parameters:\n");
   printf("   tries     Retry count, number of tries for each PQoS operation before giving up.\n");
   printf("                Default  value is 6.\n");
   printf("   maxlist   Maximum FlowIDs per List, maximum number of FlowIDs to get each L2ME retrieval.\n");
   printf("                Default value is 32.\n");
}

GCAP_GEN static void print_help(char * cmd)
{
   if ((cmd != NULL) && (strncmp(cmd, "query", strlen(cmd)) == 0))
   {
      print_query_help();
   }
   else if ((cmd != NULL) && (strncmp(cmd, "queryall", strlen(cmd)) == 0))
   {
      print_queryall_help();
   }
   else
   {
      printf("GCAP.23  [-h|--help]  [Command]\n");
      printf("GCAP.23  [-v]  <Command>\n");
      printf("Commands:\n");
      printf("   query      Query single PQoS Flow\n");
      printf("   queryall   Query all PQoS Flows\n");
   }
}

GCAP_GEN static void pqos_query_response_print(void *arg, struct moca_pqos_query_out *in, uint32_t query_all)
{
   char guidStr[20];
   char flowStr[20];
   
   if ((in->packet_da.addr[0] != 0) ||
       (in->packet_da.addr[1] != 0) ||
       (in->packet_da.addr[2] != 0) ||
       (in->packet_da.addr[3] != 0) ||
       (in->packet_da.addr[4] != 0) ||
       (in->packet_da.addr[5] != 0))
   {
      if (query_all)
      {
         sprintf(guidStr, "%02x:%02x:%02x:%02x:%02x:%02x ", MOCA_DISPLAY_MAC(in->ingress_node));
         sprintf(flowStr, "%02x:%02x:%02x:%02x:%02x:%02x ", MOCA_DISPLAY_MAC(in->flow_id));
      }
      else
      {
         guidStr[0] = 0;
         flowStr[0] = 0;
      }
      
      if (!query_all)
         printf(" FLOW_ID                    : %02x:%02x:%02x:%02x:%02x:%02x\n", 
            MOCA_DISPLAY_MAC(in->flow_id));
      printf("%s%s T_PACKET_SIZE              : %d\n", guidStr, flowStr, in->packet_size);
      if (!query_all)
         printf(" INGRESS_GUID               : %02x:%02x:%02x:%02x:%02x:%02x\n", 
            MOCA_DISPLAY_MAC(in->ingress_node));
      printf("%s%s EGRESS_GUID                : %02x:%02x:%02x:%02x:%02x:%02x\n", 
         guidStr, flowStr, MOCA_DISPLAY_MAC(in->egress_node));
      printf("%s%s FLOW_TAG                   : 0x%08x\n", guidStr, flowStr, in->flow_tag);
      printf("%s%s PACKET_DA                  : %02x:%02x:%02x:%02x:%02x:%02x \n", 
         guidStr, flowStr, MOCA_DISPLAY_MAC(in->packet_da));
      printf("%s%s T_PEAK_DATA_RATE           : %d kbps\n", guidStr, flowStr, in->peak_data_rate);
      printf("%s%s T_LEASE_TIME               : %u\n", guidStr, flowStr, in->lease_time);
      printf("%s%s T_BURST_SIZE               : %d\n", guidStr, flowStr, in->burst_size);
      printf("%s%s LeaseTimeLeft              : %u\n", guidStr, flowStr, in->lease_time_left);
  
      if (in->dest_flow_id != 0)
      {
         printf("%s%s MAXIMUM_LATENCY            : %d msec\n", guidStr, flowStr, in->max_latency);
         printf("%s%s SHORT_TERM_AVERAGE_RATIO   : %d\n", guidStr, flowStr, in->short_term_avg_ratio);
         printf("%s%s MAX_NUMBER_RETRY           : %d\n", guidStr, flowStr, in->max_retry);
         printf("%s%s INGRESS_CLASSIFICATION_RULE: %d (%s)\n", 
            guidStr, flowStr, in->ingr_class_rule, ingr_class_rules[in->ingr_class_rule]);
         printf("%s%s VLAN_TAG                   : %d\n", guidStr, flowStr, in->vlan_tag);
         printf("%s%s DSCP_MOCA                  : %d\n", guidStr, flowStr, in->dscp_moca);
         printf("%s%s FLOW_PER                   : %u (%s)\n", guidStr, flowStr, in->flow_per, in->flow_per ? "VLPER" : "NPER");
         printf("%s%s INORDER_DELIVERY_RECOMM    : %u\n", guidStr, flowStr, in->in_order_delivery);
         printf("%s%s TRAFFIC_PROTOCOL           : %u\n", guidStr, flowStr, in->traffic_protocol);
         printf("%s%s DFID                       : 0x%02X\n", guidStr, flowStr, in->dest_flow_id);
      }
   }
   else
   {
      printf("Error!  No such flow.\n");
   }
}


GCAP_GEN int GCAP_23_main(int argc, char **argv)
{
   int ret = MOCA_API_SUCCESS;
   void *ctx;
   struct moca_pqos_list_in pqos_list_in;
   struct moca_pqos_list_out pqos_list_out;
   macaddr_t pqos_query_in;
   struct moca_pqos_query_out pqos_query_out;
   uint32_t total_flows = 0;
   int query_all = 0;
   struct moca_interface_status if_status;
   struct moca_network_status nws;
   struct moca_gen_node_status gns;
   int i, j;
   uint32_t cmdIndex = 1;
   uint32_t *pulValue ;
   uint16_t *pusValue ;
   macaddr_t *pmacAddr;
   char * pStrOption;
   char * pStrVal;
   int verbose = 0;
   uint32_t tryCount = 0;
   uint32_t maxTries = 6;
   uint32_t flowIndex = 0;
   uint16_t maxReturn = 0;

   chipId = NULL;

#if defined(STANDALONE)
   uint32_t parmsIdx = 1;
   for (parmsIdx=1; parmsIdx < argc; parmsIdx++)
   {
       if (strcmp(argv[parmsIdx], "-i") == 0)
       {
          chipId = argv[parmsIdx+1];
          parmsIdx +=2;
       }
       else if (strcmp(argv[parmsIdx], "-v") == 0)
       {
          verbose = 1;
          parmsIdx++;
       }
       else if (strcmp(argv[parmsIdx], "?") == 0)
       {
          printf( "Error!  Invalid option - %s\n", argv[parmsIdx]);
          return(-1);
       }
       else if (strcmp(argv[parmsIdx], "-h") == 0)
       {
          print_help(argv[parmsIdx + 1]);
          return(0); 
       }
   }
#else

   moca_gcap_init();

   // ----------- Parse parameters
   opterr = 0;

   if ((argc >= 2) && (strcmp(argv[1], "--help") == 0))
   {
      if (argc >= 3)
         print_help(argv[2]);
      else
         print_help(NULL);

      return(0);
   }

   while((ret = getopt(argc, argv, "h:v:i:")) != -1) 
   {
      switch(ret) 
      {
      case 'v':
         verbose = 1;
         cmdIndex++;
         break;
      case 'i':
         chipId = optarg;
         cmdIndex += 2;
         break;
      case '?':
         if (optopt == 'h')
            print_help(NULL);
         else
            printf( "Error!  Invalid option - %c\n", optopt);
         return(-1);
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
      printf( "Error!  Missing command - query/queryall\n");
      return(-1);
   }

   memset(&pqos_list_in, 0, sizeof(pqos_list_in));
   maxReturn = MOCA_PQOS_LIST_FLOW_MAX_RETURN_DEF;
   flowIndex = MOCA_PQOS_LIST_FLOW_START_INDEX_DEF;

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

   if (strncmp(argv[cmdIndex], "query", strlen(argv[cmdIndex])) == 0)
   {
      // Use the default value
      MOCA_SET_PQOS_QUERY_DEF(pqos_query_in);
   }
   else if (strncmp(argv[cmdIndex], "queryall", strlen(argv[cmdIndex])) == 0)
   {
      query_all = 1;
   }
   else
   {
      printf("Error!  Invalid command - %s\n", argv[cmdIndex]);
      moca_close(ctx);
      return(-51);
   }

   i = cmdIndex + 1;
   while( (ret == MOCA_API_SUCCESS) && (i < argc) )
   {
      pulValue = NULL;
      pusValue = NULL;
      pmacAddr = NULL;
      pStrOption = argv[i];

      if ((strstr(argv[i], "flow_id=") != NULL) &&
          (query_all == 0))
      {
         pStrVal = strchr(argv[i], '=') + 1;
         pmacAddr = &pqos_query_in;
      }
      else if ((strstr(argv[i], "maxlist=") != NULL) &&
               (query_all != 0))
      {
         pusValue = &maxReturn;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if ((strstr(argv[i], "tries=") != NULL) &&
               (query_all != 0))
      {
         pulValue = &maxTries;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else 
      {
         ret = MOCA_API_ERROR;

         printf("Error!  Invalid parameter - %s\n",argv[i]);
         moca_close(ctx);
         return(-10);
      }

      if ( pulValue )
      {
         /* Convert the value for a particular option to an integer. */
         char *pszEnd = NULL;

         *pulValue = (long) strtoul( pStrVal, &pszEnd, 0 ) ;

         if( *pszEnd != '\0' )
         {
            ret = MOCA_API_ERROR;
            printf( "Error!  Invalid parameter for option %s\n", 
               pStrOption ) ;
         }
         
         i++;
      }
      if ( pusValue )
      {
         /* Convert the value for a particular option to an integer. */
         char *pszEnd = NULL;

         *pusValue = (uint16_t) strtoul( pStrVal, &pszEnd, 0 ) ;

         if( *pszEnd != '\0' )
         {
            ret = MOCA_API_ERROR;
            printf( "Error!  Invalid parameter for option %s\n", 
               pStrOption ) ;
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

   if (maxReturn > MOCA_PQOS_LIST_FLOW_MAX_RETURN_MAX)
   {
      printf( "Error!  Max flow IDs per list can not exceed %d\n", 
         MOCA_PQOS_LIST_FLOW_MAX_RETURN_MAX);
      moca_close(ctx);
      return(-101);
   }
      
   if( ret == MOCA_API_SUCCESS ) 
   {
      if (query_all == 0)
      {
         ret = moca_do_pqos_query(ctx, pqos_query_in, &pqos_query_out);
         if (ret == MOCA_API_SUCCESS)
         {
            pqos_query_response_print(ctx, &pqos_query_out, query_all);
         }
         else
         {
            printf( "Error!  Invalid flow ID\n");
         }
      }
      else if (query_all == 1)
      {
         ret = moca_get_network_status(ctx, &nws);

         /* Get the number of total nodes and loop through them all */
         if (ret == MOCA_API_SUCCESS)
         {
            printf("IngressGuid       FlowId             Item                       : Value\n");

            for (i = 0; 
                 nws.connected_nodes; 
                 nws.connected_nodes >>= 1, i++)
            {
               pmacAddr = NULL;
               if (nws.connected_nodes & 0x1)
               {
                  ret = moca_get_gen_node_status(ctx, i, &gns);
                  if (ret == MOCA_API_SUCCESS)
                  {
                     pmacAddr = &gns.eui;
                  }
               }

               if (pmacAddr != NULL)
               {
                  flowIndex = 0;
                  MOCA_MACADDR_COPY(&pqos_list_in.ingr_node_mac, pmacAddr);
                  tryCount = 0;

                  do
                  {
                     pqos_list_in.flow_start_index = flowIndex;
                     pqos_list_in.flow_max_return = maxReturn;
                     
                     ret = moca_do_pqos_list(ctx, &pqos_list_in, &pqos_list_out);

                     if ((ret == MOCA_API_SUCCESS) &&
                         (pqos_list_out.response_code == MOCA_L2_SUCCESS))
                     {
                        if (verbose)
                           printf("List node %d ret %d flows out of %d\n",
                              i, pqos_list_out.num_ret_flow_ids, pqos_list_out.total_flow_id_count);

                        for (j = 0; j < pqos_list_out.num_ret_flow_ids; j++)
                        {
                           MOCA_MACADDR_COPY(&pqos_query_in, &pqos_list_out.flowid[j]);
                           ret = moca_do_pqos_query(ctx, pqos_query_in, &pqos_query_out);
                           if (ret == MOCA_API_SUCCESS)
                           {
                              pqos_query_response_print(ctx, &pqos_query_out, query_all);
                           }
                           else
                           {
                              printf( "Error!  Invalid flow ID\n");
                           }
                        }
                        flowIndex += pqos_list_out.num_ret_flow_ids;
                     }
                     else
                     {
                        tryCount++;
                        if (verbose)
                        {
                           printf( "\nError!  Unable to list Node %d (flow count %d/%d)\n",
                              i, flowIndex, pqos_list_out.total_flow_id_count);
                        }
                     }
                  } while ((flowIndex < pqos_list_out.total_flow_id_count) &&
                           (tryCount < maxTries));

                  total_flows += flowIndex;
               }
               printf("\n");
            }

            printf("\nTotalFlows:  %u\n", total_flows);
         }
      }
   }
     
   // ----------- Finish

   moca_close(ctx);

   return(0);
}

