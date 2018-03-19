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


static char *chipId = NULL;    // -i option


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.119  [-p probe_id] [-n node] [-h]\n\
Request a node to transmit a Receiver Determined Probe (RDP).\n\
\n\
Options:\n\
[-p probe_id]   Pre-defined probe configuration to be used, 1 to 2\n\
[-n node]       Specify the node id, except own node id\n\
  -h     Display this help and exit\n");
}


GCAP_GEN int GCAP_119_main(int argc, char **argv)
{
    int ret;
    struct moca_rxd_lmo_request rxd_req;
    struct moca_interface_status ifstatus;
    struct moca_network_status nws;
    void *ctx;
    uint32_t probe_set = 0;
    uint32_t nid_set = 0;

    chipId = NULL;

    // Until CTP is modified to support secondary channel, use primary channel only.
    rxd_req.channel_id = 0;
    
    // ----------- Parse parameters

    if (argc < 2)
    {
        printf( "Error!  Missing parameter \n");
        return(-2);
    }
     memset(&rxd_req,0, sizeof(rxd_req)); 

#if defined(STANDALONE)
    int i;
    for (i=1; i < argc; i++)
    {
      if (strcmp(argv[i], "-i") == 0)
      {
          i++;
          chipId = argv[i];
      }
      else if (strcmp(argv[i], "-p") == 0)
      {
         rxd_req.probe_id = atoi(argv[i+1]);
         probe_set = 1;
         i++; 
      }
      else if (strcmp(argv[i], "-n") == 0)
      {
         rxd_req.node_id = atoi(argv[i+1]);
         nid_set = 1;
         if (rxd_req.node_id > 15 )
         {
            printf( "Error!  Invalid nodeId: %d\n", rxd_req.node_id);
            return(-1);
         }
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
    while((ret = getopt(argc, argv, "hi:p:n:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case 'p':
            rxd_req.probe_id = atoi(optarg);
            probe_set = 1;
            break;
        case 'n':
            rxd_req.node_id = atoi(optarg);
            nid_set = 1;
            if (rxd_req.node_id > 15 )
            {
               printf( "Error!  Invalid nodeId: %d\n", rxd_req.node_id);
               return(-1);
            }
            break;
        case '?':
            printf( "Error!  Invalid option - %c\n", optopt);
            return(-2);
            break;
        case 'h':
        default:
            showUsage();
            return(0); 
        }
    }
#endif
    if (probe_set != 1)
    {
        printf( "Error!  Missing probe_id value.\n");
        return(-2);
    }
    else if ((rxd_req.probe_id != 1) && (rxd_req.probe_id != 2))
    {
        printf( "Error! Invalid probe id: %d\n", rxd_req.probe_id);
        return(-3);
    }

    if (nid_set != 1)
    {
        printf( "Error!  Missing node_id value\n");
        return(-4);
    }
    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-5);
    }

    // ----------- Get info

    ret = moca_get_interface_status(ctx, &ifstatus);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Interface failure\n");
        moca_close(ctx);
        return(-6);
    }

    if (ifstatus.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-7);
    }

    ret = moca_get_network_status(ctx, &nws);    
    if ( rxd_req.node_id == nws.node_id)
    {
        printf( "Error! Can't use own node id; use other node id\n");
        moca_close(ctx);
        return(-8);
    }
    
    printf("req_rx_det_probe() begins\n");
    printf("probe id: %d - node id: %d\n",rxd_req.probe_id  ,rxd_req.node_id);
    ret = moca_set_rxd_lmo_request(ctx, &rxd_req);   

    if (ret == MOCA_API_SUCCESS)
    {
       printf("Request completed (Node ID: %d, type 4)\n", nws.node_id);
        if (rxd_req.probe_id ==1)
        {
            printf("Success, good Probe received 10, total probes received 10.\n");
        }
        else
        {
            printf("Success, good Probe received 1, total probe received 1.\n");
        }
    }
    else
    {
        printf("Aborted\n");
    }

    // ----------- Finish

    moca_close(ctx);

    return(0);
}


