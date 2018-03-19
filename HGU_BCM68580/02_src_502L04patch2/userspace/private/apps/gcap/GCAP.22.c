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

struct list_rsp_cb_arg
{
   void * ctx;
   uint32_t * count;
};


GCAP_GEN static void print_list_help(void)
{
   printf("\"list\" command syntax:\n");
   printf("   list ig=xx:xx:xx:xx:xx:xx [start=#] [limit=#]\n");
   printf("\"list\" Parameters:\n");
   printf("   ig          Ingress Node GUID, unique identifier of node at which flow will \n");
   printf("                  enter MoCA network. Default value is 00:00:00:00:00:00\n");
   printf("   [start=#]   Start Index, index of first flow to query at specified node.\n");
   printf("                  Default is 0.\n");
   printf("   [limit=#]   Maximum flows to return. Upper bound on number of flows to return.\n");
   printf("                  Default is 32.\n");

}

GCAP_GEN static void print_listall_help(void)
{
   printf("\"listall\" command syntax:\n");
   printf("   listall [tries = #] [maxlist = #]\n");
   printf("\"listall\" Parameters:\n");
   printf("   tries     Retry count, number of tries for each PQoS operation before giving up.\n");
   printf("                Default  value is 6.\n");
   printf("   maxlist   Maximum FlowIDs per List, maximum number of FlowIDs to get each L2ME retrieval.\n");
   printf("                Default value is 32.\n");
}

GCAP_GEN static void print_help(char * cmd)
{
   if ((cmd != NULL) && (strncmp(cmd, "list", strlen(cmd)) == 0))
   {
      print_list_help();
   }
   else if ((cmd != NULL) && (strncmp(cmd, "listall", strlen(cmd)) == 0))
   {
      print_listall_help();
   }
   else
   {
      printf("GCAP.22  [-h|--help]  [Command]\n");
      printf("GCAP.22  [-v]  <Command>\n");
      printf("Commands:\n");
      printf("   list      List QoS Flow Ingress\n");
      printf("   listall   List all QoS Flows\n");
   }
}
 
GCAP_GEN static void pqos_print_single_list(void *arg, uint32_t start_index, struct moca_pqos_list_out *in)
{
   int i;

   printf("flowCode                : 0x%x (%s)\n", in->response_code, moca_l2_error_name(in->response_code));
   if (in->response_code == MOCA_L2_SUCCESS)
   {
      printf("flowUpdateCount         : %d\n", in->flow_update_count);
      printf("totalFlowIdCount        : %d\n", in->total_flow_id_count);
      printf("numReturnedFlowIds      : %d\n", in->num_ret_flow_ids);

      for (i = 0; i < 32; i++)
      {
         if ((in->flowid[i].addr[0] != 0) || 
             (in->flowid[i].addr[1] != 0) || 
             (in->flowid[i].addr[2] != 0) || 
             (in->flowid[i].addr[3] != 0) || 
             (in->flowid[i].addr[4] != 0) || 
             (in->flowid[i].addr[5] != 0)  )
         {
            printf("flow[%02d]                : %02x:%02x:%02x:%02x:%02x:%02x\n", 
               (i + start_index), MOCA_DISPLAY_MAC(in->flowid[i]));
         }
      }
   }
   return;
}

GCAP_GEN static void pqos_print_listall(void *arg, uint32_t start_index, struct moca_pqos_list_out *in)
{
   int i;


   for (i = 0; i < in->num_ret_flow_ids; i++)
   {
      if ((in->flowid[i].addr[0] != 0) || 
          (in->flowid[i].addr[1] != 0) || 
          (in->flowid[i].addr[2] != 0) || 
          (in->flowid[i].addr[3] != 0) || 
          (in->flowid[i].addr[4] != 0) || 
          (in->flowid[i].addr[5] != 0)  )
      {
         printf("   FlowId: %02x:%02x:%02x:%02x:%02x:%02x  (%d)\n", 
            MOCA_DISPLAY_MAC(in->flowid[i]), (i + start_index));
      }
   }

   return;
}




GCAP_GEN int GCAP_22_main(int argc, char **argv)
{
   int ret = MOCA_API_SUCCESS;
   void *ctx;
   struct moca_pqos_list_in pqos_list_in;
   struct moca_pqos_list_out pqos_list_out;
   uint32_t total_flows = 0;
   int list_all = 0;
   struct moca_interface_status if_status;
   struct moca_network_status nws;
   struct moca_gen_node_status gns;
   int i;
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
           cmdIndex +=2;
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
      printf( "Error!  Missing command - list/listall\n");
      return(-1);
   }

   memset (&pqos_list_in, 0x00, sizeof(pqos_list_in)) ;
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

   if (strncmp(argv[cmdIndex], "list", strlen(argv[cmdIndex])) == 0)
   {
      // Use the default value
      pqos_list_in.ingr_node_id = 0xFFFFFFFF;
   }
   else if (strncmp(argv[cmdIndex], "listall", strlen(argv[cmdIndex])) == 0)
   {
      list_all = 1;
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

      if ((strstr(argv[i], "ig=") != NULL) &&
          (list_all == 0))
      {
         pStrVal = strchr(argv[i], '=') + 1;
         pmacAddr = &pqos_list_in.ingr_node_mac;
      }
      else if ((strstr(argv[i], "start=") != NULL) &&
               (list_all == 0))
      {
         pulValue = &flowIndex;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if ( ((strstr(argv[i], "limit=") != NULL) &&
                     (list_all == 0)) ||
                ((strstr(argv[i], "maxlist=") != NULL) &&
                     (list_all != 0)) )
      {
         pusValue = &maxReturn;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if ((strstr(argv[i], "tries=") != NULL) &&
               (list_all != 0))
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
      if (list_all == 0)
      {
         if (MOCA_MACADDR_IS_NULL(&pqos_list_in.ingr_node_mac))
         {
            printf("Usage:   list ig=xx:xx:xx:xx:xx:xx [start=#] [limit=#]\n");
            printf("ERROR- param 'ig' not found and is required\n");
            moca_close(ctx);
            return(-11);
         }

         pqos_list_in.flow_start_index = flowIndex;
         pqos_list_in.flow_max_return = maxReturn;

         ret = moca_do_pqos_list(ctx, &pqos_list_in, &pqos_list_out);
         if (ret == MOCA_API_SUCCESS)
         {
            pqos_print_single_list(ctx, flowIndex, &pqos_list_out);
         }
         else
         {
            printf( "Error!  No such ingress node MAC address\n");
         }
      }
      else if (list_all == 1)
      {
         ret = moca_get_network_status(ctx, &nws);

         /* Get the number of total nodes and loop through them all */
         if (ret == MOCA_API_SUCCESS)
         {
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
                  printf("IngressGuid: %02x:%02x:%02x:%02x:%02x:%02x   (NodeId %d)\n",
                     MOCA_DISPLAY_MAC(*pmacAddr), i);
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

                        pqos_print_listall(ctx, flowIndex, &pqos_list_out);
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
            }

            printf("\nTotalFlows:  %u\n", total_flows);
         }
      }
   }
     
   // ----------- Finish

   moca_close(ctx);

   return(0);
}
