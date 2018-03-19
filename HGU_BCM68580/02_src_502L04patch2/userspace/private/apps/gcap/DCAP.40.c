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

static char *chipId = NULL;    // -i option

#define MR_RST_SUCCESS 0x2
#define MR_RST_FAIL  0x3

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.40 <-n node_mask> [-s seconds] [-h]\n\
Send reset request to all the nodes in the network.\n\
\n\
Options:\n\
  <-n node_mask>  indicates which nodes to send reset command, it must \n\
                             be hexadecimal (e.g 0x123)\n\
  [-s seconds]     Set time in seconds to do Moca reset.Default value is %d\n\
  -h   Display this help and exit\n", MOCA_MOCA_RESET_RESET_TIMER_DEF);
}

GCAP_GEN static void mr_event_callback(void * userarg,uint32_t status)
{
   /*
   0 = Reset Success
   1 = Reset Failed 
   2 = Network Success 
   3 = Network Failed
   */
   switch (status)
   {
      case 0:
         printf("MR_RST_SUCCESS\n\n");
         break;
      case 1:
         printf("MR_RST_FAIL\n\n");
         break;
      case 2:
         printf("MR_NETWORK_SUCCESS\n\n");
         break;
      case 3:
         printf("MR_NETWORK_FAIL\n\n");
         break;
      default:
         printf("Unkown MR event %d\n", status);
         break;
   }

   if (status != 0)
      moca_cancel_event_loop(userarg);
   
}

GCAP_GEN int DCAP_40_main(int argc, char **argv)
{
  int nRet = MOCA_API_SUCCESS;
  void *ctx;
  struct moca_network_status    net_status;
  struct moca_gen_node_status   node_status;
  struct moca_moca_reset_in  in;
  struct moca_moca_reset_out out;
  struct moca_interface_status status;
  int numOfrequestedNodes = 0;
  int BitMasknodeId = 0;
  int i;
  char *end;
  int resetAllNodes = 0;
   
   chipId = NULL; 

   memset (&in, 0x00, sizeof(in)) ;
   memset (&out, 0x00, sizeof(out)) ;
   in.non_def_seq_num = MOCA_MOCA_RESET_NON_DEF_SEQ_NUM_DEF;
   in.reset_timer = MOCA_MOCA_RESET_RESET_TIMER_DEF;
   in.node_mask = 0;
     
    // ----------- Parse parameters
    if ( (argc < 2) || (argc > 5) )
    {
            printf( "Error! Insufficient arguments\n");
            return(-1);
    }
    
#if defined(STANDALONE) 
   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
          i++;
          chipId = argv[i];
       }
       else if (strcmp(argv[i], "-n") == 0)
       {
          BitMasknodeId = strtoul(argv[i+1], &end, 0);
          i++; 
       }
       else if (strcmp(argv[i], "-s") == 0)
       {
          in.reset_timer = atoi(argv[i+1]);
          i++;
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
    opterr = 0;

    while((nRet = getopt(argc, argv, "hi:n:s:")) != -1) 
    {
        switch(nRet)
        {
        case 'i':
            chipId = optarg;
            break;
        case '?':
            printf( "Error!  Invalid option - %c\n", optopt);
            return(-1);
            break;
        case 'n':
            BitMasknodeId = strtoul(optarg, &end, 0);
            break;
        case 's':
            in.reset_timer = atoi(optarg);
            break;
        case 'h':
        default:
            showUsage();
            return(0); 
        }
    }

#endif
    // ----------- Initialize

   ctx = moca_open(chipId);

   if (!ctx)
   {
       printf( "Error!  Unable to connect to moca instance\n");
       return(-3);
   }

   nRet = moca_get_interface_status(ctx, &status);

   if (nRet != MOCA_API_SUCCESS)
   {
       printf( "Error!  Internal failure\n");
       moca_close(ctx);
       return(-4);
   }

   if (status.link_status != MOCA_LINK_UP)
   {
       printf( "Error! No Link\n");
       moca_close(ctx);
       return(-5);
   }  

   nRet = moca_get_network_status(ctx, &net_status);

   if (nRet != 0)
   {
      printf( "Error!internal failure\n");
      moca_close(ctx);
      return(-6);
   }

   /* get status entry for each node */
   for(i = 0; i < MOCA_MAX_NODES; i++) 
   {        
      if(!(net_status.connected_nodes & (1 << i)))
          continue;
      if ((BitMasknodeId & (1 << i)))
      {
         nRet = moca_get_gen_node_status(ctx, i, &node_status);
         if ((nRet == 0) &&  (i != net_status.node_id)  &&       
             ((node_status.protocol_support >> 24) >= MoCA_VERSION_1_1))
         {          
            numOfrequestedNodes++;
            in.node_mask |= (1 << i);
         }
      }
   }  

   if ( numOfrequestedNodes == 0 )
   {
      printf( "No eligible nodes found to be reset\n" ) ;
      moca_close(ctx);
      return(0);
   }

   if (in.node_mask == (net_status.connected_nodes - (1 << net_status.node_id)))
   {
      resetAllNodes = 1;
      moca_register_mr_event_cb(ctx, mr_event_callback, ctx);
   }
   
   nRet = moca_do_moca_reset(ctx, &in, &out);
   if (nRet == MOCA_API_SUCCESS) 
   {
      printf( "Response Code: %s\n", moca_l2_error_name(out.response_code));

      if (resetAllNodes && (out.reset_status != 0))
      {
         MoCAOS_Alarm(MOCA_MOCA_RESET_RESET_TIMER_MAX + 1);
         moca_event_loop(ctx);
      }  
   }
   else
   {
      printf( "moca_do_moca_reset returned error %d\n", nRet ) ;
   }

   moca_close(ctx);
   
   return(0);
}
