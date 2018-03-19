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

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.27 <node_addr> [-h]\n\
Trigger Out-of-Order LMO requests for a specified node in the network.\n\
\n\
Options:\n\
 <node_addr>  Node MAC Address format for which Out-of-Order LMO\n\
              is requested (01:23:45:67:89:ab)\n\
  -h   Display this help and exit\n");
}



GCAP_GEN int GCAP_27_main(int argc, char **argv)
{
    int ret = MOCA_API_SUCCESS;
    int i;
    struct moca_network_status nws;
    struct moca_interface_status status;
    struct moca_gen_node_status   node_status;
    macaddr_t                     mac;
    uint32_t                         ooo_nodemask = 0;
    uint32_t                         ooo_nid = 0;
    void *ctx;

    chipId = NULL;

    // ----------- Parse parameters
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
    if (argc < 2)
    {
       printf( "Error!  Missing parameter node_addr\n");
       return(-2);
    }
    else if(moca_parse_mac_addr( argv[1], &mac) != 0 )
    {
       printf( "Error!  Invalid parameter for node_addr\n");
       return(-3);
    }

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    // ----------- Get info
    ret = moca_get_interface_status(ctx, &status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure\n");
        moca_close(ctx);
        return(-3);
    }

    if (status.link_status != MOCA_LINK_UP)
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
   
          /* get status entry for each node */
      for(i = 0; i < MOCA_MAX_NODES; i++) 
      {        
         if(! (nws.connected_nodes & (1 << i)))
             continue;

         ret = moca_get_gen_node_status(ctx, i, (struct moca_gen_node_status *)&node_status);

         if ((ret == 0) &&
             (memcmp(&mac, &node_status.eui, sizeof(macaddr_t)) == 0) &&
             ((node_status.protocol_support >> 24) >= MoCA_VERSION_1_1))
         {           
             ooo_nodemask = (1 << i);
             ooo_nid = i;
             break;
         }
      }

      if (ooo_nodemask == 0)
      {
         printf( "Error! No such MAC address\n");
         moca_close(ctx);
         return(-6);
      }    


    // ----------- Output Data   

    ret = moca_set_ooo_lmo(ctx, ooo_nid);
    
    // ----------- Finish

    moca_close(ctx);

    return(0);
}

