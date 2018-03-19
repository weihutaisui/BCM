#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>
#include "GCAP_Common.h"

static char *chipId = NULL;    // -i option
static char *nodeMac = NULL;   // -s option

struct list_rsp_cb_arg
{
   void * ctx;
   uint32_t * count;
};


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.25 [-s MAC_adress] [-f] [-h]\n\
Initiate an FMR request and report back OFDMb values received \n\
in response to the FMR request.\n\
\n\
Options:\n\
 [-s MAC_adress] Specific node MAC Address (12:34:56:78:9a:bc)\n\
                If not specified, all nodes information will be\n\
                displayed.\n\
 [-f]          Force type 1 FMR request\n\
  -h   Display this help and exit\n");
}


GCAP_GEN static int mocacli_list_active_nodes( void * ctx, uint32_t * nodeMask20, uint32_t * nodeMask11 )
{
   int i;
   struct moca_network_status nws;
   struct moca_gen_node_status gsn;
   int ret;

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   *nodeMask20 = 0;
   *nodeMask11 = 0;

   /* get status entry for each node */
   printf("Node ID   MAC Addr\n");
   for(i = 0; i < MOCA_MAX_NODES; i++) {
      if(! (nws.connected_nodes & (1 << i)))
         continue;

      ret = moca_get_gen_node_status(ctx, i, &gsn);
      if (ret == MOCA_API_SUCCESS)
      {
         printf ("%2d        %02x:%02x:%02x:%02x:%02x:%02x\n",
               i, MOCA_DISPLAY_MAC(gsn.eui));
      }
     
      if ((gsn.protocol_support >> 24) >= MoCA_VERSION_2_0)
         *nodeMask20 |= (1 << i);
      else if ((gsn.protocol_support >> 24) == MoCA_VERSION_1_1)
         *nodeMask11 |= (1 << i);
   }

   printf("----------------------------\n");
   printf("Management GN Node ID = %u\n", nws.node_id);

   return 0;
}

static uint16_t fmr11info[MOCA_MAX_NODES][MOCA_MAX_NODES];

GCAP_GEN static void fmr11_process_data(void *arg, struct moca_fmr_init_out *in)
{
   int i, j;
   uint32_t *responded_node;
   uint16_t *fmrinfo_node;

   for (i = 0; i < MOCA_MAX_NODES; i++)
   {
      responded_node = (void *)&in->responded_node_0 + ((sizeof(in->responded_node_0)*i) + (sizeof(in->fmrinfo_node_0)*i));
      fmrinfo_node = (uint16_t *)(responded_node + 1);
      if (*responded_node != 0xFF)
      {
         for (j = 0; j < MOCA_MAX_NODES; j++ )
         {
            fmr11info[*responded_node][j] = fmrinfo_node[j];
         }
      }
   }
}

GCAP_GEN static void print_fmr_info_11(void * ctx, uint32_t nodemask)
{
   int i, j;
   uint32_t nodeMask11;
   uint32_t nodeMask20;
   uint32_t connectedNodes;

   printf("----------------------------\n");
   mocacli_list_active_nodes(ctx, &nodeMask20, &nodeMask11);

   if (nodemask == 0)
      connectedNodes = (nodeMask20 | nodeMask11);
   else
      connectedNodes = nodemask;

   printf("OFDMb/GAP\n");
   printf("Tx\\Rx     0        1        2        3        4        5        6        7");
   for (i = 0; i < MOCA_MAX_NODES; i++)
   {
      if (connectedNodes & (1 << i))
      {
         printf("\n  %2d", i);
         for (j = 0; j < 8; j++ )
         {
            printf("  %4d/%02d", fmr11info[i][j] & 0x7FF, fmr11info[i][j] >> 11);
         }
      }
   }

#if (MOCA_MAX_NODES > 8)
   printf("\n          8        9       10       11       12       13        14      15");
   for (i = 0; i < MOCA_MAX_NODES; i++)
   {
      if (connectedNodes & (1 << i))
      {
         printf("\n  %2d", i);
         for (j = 8; j < 16; j++ )
         {
            printf("  %4d/%02d", fmr11info[i][j] & 0x7FF, fmr11info[i][j] >> 11);
         }
      }
   }
#endif
   printf("\n");

}

GCAP_GEN static void print_fmr_info_20(void * ctx, uint32_t nodemask, struct moca_fmr_20_out * pData)
{
   int i, j;
   struct moca_fmr_20_out * pNodeData;
   uint32_t nodeDataSize = (uint32_t)(&pData->node1_gap_nper[0]) - (uint32_t)(&pData->node0_gap_nper[0]);
   uint32_t nodeMask11;
   uint32_t nodeMask20;
      
   printf("----------------------------\n");
   mocacli_list_active_nodes(ctx, &nodeMask20, &nodeMask11);

   if (nodemask != 0)
      nodeMask20 = nodemask;

   printf("OFDMb/GAP\n");
   printf("Tx\\Rx     0        1        2        3        4        5        6        7");
   for (i = 0; i < MOCA_MAX_NODES; i++)
   {
      if (nodeMask20 & (1 << i))
      {
         printf("\n%02d  NPER", i);
         pNodeData = (struct moca_fmr_20_out * )((uint32_t)pData + (i * nodeDataSize));
         for (j = 0; j < 8; j++ )
         {
            if (nodeMask20 & (1 << j))
               printf("  %4d/%02d", pNodeData->node0_ofdmb_nper[j], pNodeData->node0_gap_nper[j]);
            else
               printf("     0/0 ");
         }
         printf("\n%02d VLPER", i);
         pNodeData = (struct moca_fmr_20_out * )((uint32_t)pData + (i * nodeDataSize));
         for (j = 0; j < 8; j++ )
         {
            printf("  %4d/%02d", pNodeData->node0_ofdmb_vlper[j], pNodeData->node0_gap_vlper[j]);
         }
         printf("\n%02d 50MHz", i);
         pNodeData = (struct moca_fmr_20_out * )((uint32_t)pData + (i * nodeDataSize));
         for (j = 0; j < 8; j++ )
         {
            if (i == j)
               printf("  %4d/%02d", pNodeData->node0_ofdmb_gcd, pNodeData->node0_gap_gcd);
            else if (nodeMask11 & (1 << j))
               printf("  %4d/%02d", pNodeData->node0_ofdmb_nper[j], pNodeData->node0_gap_nper[j]);
            else
               printf("     0/0 ");
         }
      }
   }
   
   printf("\n          8        9       10       11       12       13        14      15");
   for (i = 0; i < MOCA_MAX_NODES; i++)
   {
      if (nodeMask20 & (1 << i))
      {
         printf("\n%02d  NPER", i);
         pNodeData = (struct moca_fmr_20_out * )((uint32_t)pData + (i * nodeDataSize));
         for (j = 8; j < 16; j++ )
         {
            if (nodeMask20 & (1 << j))
               printf("  %4d/%02d", pNodeData->node0_ofdmb_nper[j], pNodeData->node0_gap_nper[j]);
            else
               printf("     0/0 ");
         }
         printf("\n%02d VLPER", i);
         pNodeData = (struct moca_fmr_20_out * )((uint32_t)pData + (i * nodeDataSize));
         for (j = 8; j < 16; j++ )
         {
            printf("  %4d/%02d", pNodeData->node0_ofdmb_vlper[j], pNodeData->node0_gap_vlper[j]);
         }
         printf("\n%02d 50MHz", i);
         pNodeData = (struct moca_fmr_20_out * )((uint32_t)pData + (i * nodeDataSize));
         for (j = 8; j < 16; j++ )
         {
            if (i == j)
               printf("  %4d/%02d", pNodeData->node0_ofdmb_gcd, pNodeData->node0_gap_gcd);
            else if (nodeMask11 & (1 << j))
               printf("  %4d/%02d", pNodeData->node0_ofdmb_nper[j], pNodeData->node0_gap_nper[j]);
            else
               printf("     0/0 ");
         }
      }
   }
   printf("\n");

   for (i = 0; i < MOCA_MAX_NODES; i++)
   {
      if (nodeMask20 & (1 << i))
      {
         pNodeData = (struct moca_fmr_20_out * )((uint32_t)pData + (i * nodeDataSize));

         printf("\nThere %s %d OFDMA definition table%s for node %d\n",
            (pNodeData->node0_ofdma_def_tab_num == 1 ? "is" : "are"),
            pNodeData->node0_ofdma_def_tab_num,             
            (pNodeData->node0_ofdma_def_tab_num == 1 ? "" : "s"), i);

         for (j = 0; (j < pNodeData->node0_ofdma_def_tab_num) && (j < 4); j++)
         {
            printf(" OFDMA Definition ID %d:\n", j);
            printf(" Bitmask:         0x%04x\n", pNodeData->node0_ofdma_tab_node_bitmask[j]);
            printf(" Time gap:        %d\n", pNodeData->node0_ofdma_tab_gap[j]);
            printf(" Bits/symbol/sub: %04d\n", pNodeData->node0_ofdma_tab_bps[j]);
            printf(" Sub-Channels:    %d\n", pNodeData->node0_ofdma_tab_num_subchan[j]);
         }
      }
   }
   printf("\n");

}

GCAP_GEN int GCAP_25_main(int argc, char **argv)
{
   int                           ret;
   void                          *ctx;
   struct moca_interface_status  if_status;
   struct moca_network_status    nws;
   macaddr_t                     mac;
   struct moca_gen_node_status   node_status;
   struct moca_fmr_init_out      fmr11_data;
   struct moca_fmr_20_out        fmr20_data;
   uint32_t                      fmr_nodemask = 0;
   int                           i;
   uint32_t                      do_11_fmr = 0;
   uint32_t                      required_version;

   chipId = NULL;
   nodeMac = NULL;

#if defined(STANDALONE)
   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
           i++;
           chipId = argv[i];
       }
       else if (strcmp(argv[i], "-s") == 0)
       {
          i++;
          nodeMac = argv[i];
       }
       else if (strcmp(argv[i], "-f") == 0)
       {
          do_11_fmr = 1;
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

   moca_gcap_init();

   // ----------- Parse parameters
   opterr = 0;
 
   while((ret = getopt(argc, argv, "hi:s:f")) != -1) 
   {
      switch(ret) 
      {
      case 'i':
         chipId = optarg;
         break;
      case 's':
         nodeMac = optarg;
         break;
      case 'f':
         do_11_fmr = 1;
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
   memset (&fmr11info, 0x00, sizeof(fmr11info));
   memset (&mac, 0, sizeof(mac));

   if (nodeMac != NULL)
   {
      if ( moca_parse_mac_addr( nodeMac, &mac) != 0 )
      {        
         printf( "Error!  invalid parameter for node_addr\n");
         return(-1);
      }
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
       return(-4);
   }
   
   
   if ((mac.addr[0] == 0) && (mac.addr[1] == 0) &&
       (mac.addr[2] == 0) && (mac.addr[3] == 0) &&
       (mac.addr[4] == 0) && (mac.addr[5] == 0) )
   {
      // Do FMR for all addresses
      fmr_nodemask = 0;
   }
   else
   {
      if (do_11_fmr)
         required_version = MoCA_VERSION_1_1;
      else
         required_version = MoCA_VERSION_2_0;
         
      /* get status entry for each node */
      for(i = 0; i < MOCA_MAX_NODES; i++) 
      {        
         if(! (nws.connected_nodes & (1 << i)))
             continue;

         ret = moca_get_gen_node_status(ctx, i, (struct moca_gen_node_status *)&node_status);

         if ((ret == 0) &&
             (memcmp(&mac, &node_status.eui, sizeof(macaddr_t)) == 0) &&
             ((node_status.protocol_support >> 24) >= required_version))
         {           
             fmr_nodemask = (1 << i);
             break;
         }
      }

      if (fmr_nodemask == 0)
      {
         printf( "Error! No such MAC address\n");
         moca_close(ctx);
         return(-6);
      }            
   }


   if (do_11_fmr)
   {
      ret = moca_do_fmr_init(ctx, fmr_nodemask, &fmr11_data);
      if (ret == MOCA_API_SUCCESS)
      {
         fmr11_process_data(ctx, &fmr11_data);
         print_fmr_info_11(ctx, fmr_nodemask);
      }
   }
   else
   {
      ret = moca_do_fmr_20(ctx, fmr_nodemask, &fmr20_data);
      if (ret == MOCA_API_SUCCESS)
      {
         print_fmr_info_20(ctx, fmr_nodemask, &fmr20_data);
      }
   }
   
   if (ret != MOCA_API_SUCCESS) 
   {
       printf( "Error!  Unable to get FMR\n" ) ;
       moca_close(ctx);
       return(-5);
   }


   // ----------- Finish
   moca_close(ctx);

   return(0);
}
