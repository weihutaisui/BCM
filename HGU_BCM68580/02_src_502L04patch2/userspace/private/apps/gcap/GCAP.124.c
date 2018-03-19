#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include <mocaint.h>
#include "GCAP_Common.h"


static char *ndId = NULL;
static char *chipId = NULL;    // -i option

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.124 <nodeid> [-h] \n\
 Report the node current power state\n\
\n\
Options:\n\
 <nodeid> ID of the specified node or 'ALL' for all nodes\n\
  -h     Display this help and exit\n");
}



GCAP_GEN int GCAP_124_main(int argc, char **argv)
{
   int ret = MOCA_API_SUCCESS;
   void *ctx;
   struct moca_interface_status status;
   struct moca_network_status nws;
   uint32_t i;
   uint32_t ndmask;
   uint32_t pset = 0;
   uint32_t allnd = 0;
   uint32_t nodeId = 0;
   struct moca_node_power_state node_pwr_state;

   chipId = NULL;
   ndId = NULL;
  
#if defined(STANDALONE)
   
   for (i=1; i < argc; i++)
   {
     if (strcmp(argv[i], "-i") == 0)
     {
         i++;
         chipId = argv[i];
     }
     else if (strcmp(argv[i], "?") == 0)
     {
        printf( "Error!  Invalid option - %s\n", argv[i]);
        return(-1);
     }
     else if (strcmp(argv[i], "-h") == 0)
     {
        showUsage();
        return(0); 
     }
   } 
#else

   // ----------- Parse parameters
   opterr = 0;

   while((ret = getopt(argc, argv, "hi:")) != -1) 
   {
      switch(ret) 
      {
        case 'i':
           chipId = optarg;
           break;
        case '?':
           printf( "Error!  Invalid option - %c\n", optopt);
           return(-1);
           break;
        case 'h':            
        default:
           showUsage();
           return(0); 
      }
   }
#endif
   if ((argc > 1) && (argv[1] != NULL))
   {
      for(i=1;i<argc;i++)
      {
         if (argv[i][0] == '-')
         {
            // Ignore
         }
         else if (!pset)
         {
            ndId = argv[i];
            pset = 1;
            if( strcmp( ndId, "ALL" )== 0 )
            {
               allnd = 1;  
            }
            else
            {
               nodeId = atoi(ndId);
               if (nodeId  >= MOCA_MAX_NODES) 
               {
                  printf( "Error! Invalid value %d for node ID should be between 0 to 15 or ALL \n", nodeId);
                  return(-2); 
               }
            }
         }
       }
   }
   else
   {
      printf( "Error! You must specify a node ID between 0 to 15 or ALL \n");
      return(-3); 
   }      

   ctx = moca_open(chipId);

    if (!ctx)
    {
       printf( "Error!  Unable to connect to moca instance\n");
       return(-4);
    }

    ret = moca_get_interface_status(ctx, &status);

    if (ret != MOCA_API_SUCCESS)
    {
       printf( "Error! Internal failure\n");
       moca_close(ctx);
       return(-5);
    }

   if (status.link_status != MOCA_LINK_UP)
   {
      printf( "Error! No Link\n");
      moca_close(ctx);
      return(-6);
   }  

   ret = moca_get_network_status(ctx, &nws);
   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error! Internal failure\n");
      moca_close(ctx);
      return(-7);
   }
   memset(&node_pwr_state, 0, sizeof(node_pwr_state));

   if ( allnd == 1)
   {
      i = 0;
      ndmask = nws.connected_nodes;

      while (i < 16)
      {
         if ((ndmask &(1<<i)) != 0)
         {
            ret = moca_get_node_power_state(ctx, i, &node_pwr_state);
            if (ret != MOCA_API_SUCCESS)
            {
               moca_close(ctx);
               printf( "Error!  Internal\n");
               return(-8);
            }
            printPowerInfo(i,node_pwr_state.pwr, node_pwr_state.state);
         }
         else
         {
            ret = moca_get_node_power_state(ctx, i, &node_pwr_state);
            if ((ret == MOCA_API_SUCCESS) &&
                ((node_pwr_state.state == POWER_M2) || (node_pwr_state.state == POWER_M3)))
            {
                printPowerInfo(i,node_pwr_state.pwr, node_pwr_state.state);  
            }
         }
         i++;
      }
   }
   else
   {
      ndmask = nws.connected_nodes;
      if ((ndmask &(1<<nodeId)) != 0)
      {
         ret = moca_get_node_power_state(ctx, nodeId, &node_pwr_state);
         if (ret != MOCA_API_SUCCESS)
         {
            moca_close(ctx);
            printf( "Error!  Internal\n");
            return(-9);
         }
         printPowerInfo(nodeId, node_pwr_state.pwr, node_pwr_state.state);
      }
      else
      {
         ret = moca_get_node_power_state(ctx, nodeId, &node_pwr_state);
         if ( (ret == MOCA_API_SUCCESS) &&
              ((node_pwr_state.state == POWER_M2) || (node_pwr_state.state == POWER_M3)) )
         {
            printPowerInfo(nodeId, node_pwr_state.pwr, node_pwr_state.state);  
         }
         else
         {
            moca_close(ctx);
            printf( "Error!  invalid node id %d\n", nodeId);
            return(-10);                   
         }
      }
   }
   printf("\n");
   
   moca_close(ctx);
   return(0);
}
