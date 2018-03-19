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

struct pqos_list_cb_arg {
   void * ctx;
   uint32_t flow_id[32][2];
};

static char *chipId = NULL;    // -i option

GCAP_GEN static void print_delete_help(void)
{
   printf("\"delete\" command syntax:\n");
   printf("   delete [flow_id=xx:xx:xx:xx:xx:xx]\n");
   printf("\"delete\" Parameter:\n");
   printf("   [flow_id]   Flow to delete from network. A default value of\n");
   printf("               01:00:5e:00:01:00 will be used if parameters are not supplied.\n");
}

GCAP_GEN static void print_deleteall_help(void)
{
   printf("\"deleteall\" command syntax:\n");
   printf("   deleteall [tries = #] [maxlist = #]\n");
   printf("\"deleteall\" Parameter:\n");
   printf("   tries     Retry count, number of tries for each PQoS operation before giving up.\n");
   printf("                Default  value is 6.\n");
   printf("   maxlist   Maximum FlowIDs per List, maximum number of FlowIDs to get each L2ME retrieval.\n");
   printf("                Default value is 32.\n");
}

GCAP_GEN static void print_help(char * cmd)
{
   if ((cmd != NULL) && (strncmp(cmd, "delete", strlen(cmd)) == 0))
   {
      print_delete_help();
   }
   else if ((cmd != NULL) && (strncmp(cmd, "deleteall", strlen(cmd)) == 0))
   {
      print_deleteall_help();
   }
   else
   {
      printf("GCAP.21  [-h|--help]  [Command]\n");
      printf("GCAP.21  [-v]  <Command>\n");
      printf("Commands:\n");
      printf("   delete      Delete QoS Flow\n");
      printf("   deleteall   Delete all QoS Flows in the network\n");
   }
}
 

GCAP_GEN int GCAP_21_main(int argc, char **argv)
{
   int ret = MOCA_API_SUCCESS;
   void *ctx;
   int delete_all = 0;
   int i, j;
   unsigned int delcount = 0;
   macaddr_t pqos_delete_req;
   struct moca_pqos_delete_flow_out pqos_delete_rsp;
   struct moca_interface_status if_status;
   struct moca_network_status nws;
   struct moca_pqos_list_in pqos_list_in;
   struct moca_pqos_list_out pqos_list_out;
   uint32_t verbose = 0;
   uint32_t maxTries = 6;
   uint32_t listTries = 0;
   uint32_t nodeTotalFlows = 0;
   uint32_t currentFlowCount = 0;
   uint32_t maxList = MOCA_PQOS_LIST_FLOW_MAX_RETURN_DEF;
   uint32_t pqosCount;
   uint32_t *pulValue ;
   macaddr_t *pmacAddr;
   char * pStrOption;
   char * pStrVal;
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
      printf( "Error!  Missing command - delete/deleteall\n");
      return(-1);
   }

   // ----------- Initialize

   ctx = moca_open(chipId);

   if (!ctx)
   {
      printf( "Error!  Unable to connect to moca instance\n");
      return(-2);
   }

   ret = moca_get_interface_status(ctx, &if_status);

   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error!  Internal failure\n");
      moca_close(ctx);
      return(-3);
   }

   if (if_status.link_status != MOCA_LINK_UP)
   {
      printf( "Error! No Link\n");
      moca_close(ctx);
      return(-4);
   }

   ret = moca_get_network_status(ctx, &nws);
   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error!  Internal failure\n");
      moca_close(ctx);
      return(-5);
   }

   if (strncmp(argv[cmdIndex], "delete", strlen(argv[cmdIndex])) == 0)
   {
      // Use the default value
      pqos_delete_req.addr[0] = 0x01;
      pqos_delete_req.addr[1] = 0x00;
      pqos_delete_req.addr[2] = 0x5E;
      pqos_delete_req.addr[3] = 0x00;
      pqos_delete_req.addr[4] = 0x01;
      pqos_delete_req.addr[5] = 0x00;      
   }
   else if (strncmp(argv[cmdIndex], "deleteall", strlen(argv[cmdIndex])) == 0)
   {
      delete_all = 1;
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
      pmacAddr = NULL;
      pStrOption = argv[i];

      if ((strstr(argv[i], "flow_id=") != NULL) &&
          (delete_all == 0))
      {
         pStrVal = strchr(argv[i], '=') + 1;
         pmacAddr = &pqos_delete_req;
         i++;
      }
      else if ((strstr(argv[i], "tries=") != NULL) &&
               (delete_all != 0))
      {
         pulValue = &maxTries;
         pStrVal = strchr(argv[i], '=') + 1;
      }
      else if ((strstr(argv[i], "maxlist=") != NULL) &&
               (delete_all != 0))
      {
         pulValue = &maxList;
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

         if (pulValue)
            *pulValue = (long) strtoul( pStrVal, &pszEnd, 0 ) ;

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

   if( ret == MOCA_API_SUCCESS ) 
   {
      /* Get the number of total nodes and loop through them all */
      memset(&pqos_list_in, 0, sizeof(pqos_list_in));

      if (ret == MOCA_API_SUCCESS)
      { 
         for (i = 0; 
              nws.connected_nodes; 
              nws.connected_nodes >>= 1, i++)
         {
            if (nws.connected_nodes & 0x1)
            {
               nodeTotalFlows = 0xFFFFFFFF;
               currentFlowCount = 0;
               
               pqos_list_in.ingr_node_id = i;
               pqos_list_in.flow_max_return = maxList;
               pqos_list_in.flow_start_index = 0;
               listTries = 0;

               while ((nodeTotalFlows != 0) && (listTries < maxTries))
               {
                  ret = moca_do_pqos_list(ctx, &pqos_list_in, &pqos_list_out);

                  if ((ret == MOCA_API_SUCCESS) && (pqos_list_out.response_code == MOCA_L2_SUCCESS))
                  {
                     nodeTotalFlows = pqos_list_out.total_flow_id_count;
                     if (verbose)
                     {
                        printf("%d/%d flows returned from node %d\n", 
                           pqos_list_out.num_ret_flow_ids, pqos_list_out.total_flow_id_count, i);
                     }

                     for (j = 0; j < pqos_list_out.num_ret_flow_ids; j++)
                     {
                        if ((pqos_list_out.flowid[j].addr[0] == 0) && 
                            (pqos_list_out.flowid[j].addr[1] == 0) &&                       
                            (pqos_list_out.flowid[j].addr[2] == 0) &&                       
                            (pqos_list_out.flowid[j].addr[3] == 0) &&                       
                            (pqos_list_out.flowid[j].addr[4] == 0) &&                       
                            (pqos_list_out.flowid[j].addr[5] == 0) )
                        {
                           break;
                        }
                        else
                        {
                           if ((memcmp(&pqos_list_out.flowid[j], &pqos_delete_req, sizeof(macaddr_t)) == 0) ||
                               (delete_all==1))
                           {
                              memset (&pqos_delete_req, 0x00, sizeof(pqos_delete_req));
                              memcpy (&pqos_delete_req, &pqos_list_out.flowid[j], sizeof(macaddr_t));

                              pqosCount = 0;
                              ret = MOCA_API_ERROR; /* Initialize to failed value for the retries */
                              pqos_delete_rsp.response_code = MOCA_L2_TRANSACTION_FAILED;

                              while ((ret != MOCA_API_SUCCESS) &&
                                     (pqos_delete_rsp.response_code != MOCA_L2_SUCCESS) &&
                                      (pqosCount < maxTries))
                              {
                                 ret = moca_do_pqos_delete_flow(ctx, pqos_delete_req, &pqos_delete_rsp);

                                 if (verbose)
                                 {
                                    printf("Attempt %d to delete flow %02x:%02x:%02x:%02x:%02x:%02x %s\n",
                                       pqosCount, MOCA_DISPLAY_MAC(pqos_delete_req),
                                       (ret == MOCA_API_SUCCESS ? "Success" : "Failed"));
                                 }
                              }
                              if ((ret == MOCA_API_SUCCESS) && (pqos_delete_rsp.response_code == MOCA_L2_SUCCESS))
                              {
                                 delcount++;
                                 currentFlowCount--;

                                 if (delete_all == 0)
                                 {
                                    moca_close(ctx);
                                    return(0);
                                 }
                              }
                              else
                              {
                                 printf("Error deleting flow %02x:%02x:%02x:%02x:%02x:%02x", 
                                    MOCA_DISPLAY_MAC(pqos_list_out.flowid[j]));
                              }
                           }
                        }
                     }

                     currentFlowCount += pqos_list_out.num_ret_flow_ids;
                     if (currentFlowCount >= pqos_list_out.total_flow_id_count)
                        break;
                  }
                  else
                  {
                     listTries++;
                     if (verbose)
                     {
                        printf("Error listing flows from node %d\n", i);
                     }
                  }
               }
            }
         }
      }

      if ((delcount == 0) && (delete_all != 1))
      {
         printf( "Error! DECISION_FLOW_NOT_FOUND\n");
      }
   } 
   
   // ----------- Finish
   moca_close(ctx);

   return(0);
}
