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
static char *type = NULL;      // -t option
static char *node = NULL;      // -n option
static char *mask = NULL;      // -m option
static uint32_t lmo_count = 0;

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.128 [-t type] [-n node] [-m mask] [-h]\n\
Request NC to transition to Unsolicited Probe Report state\n\
and begin PHY profile state and request another node(s) to \n\
start using profile defined in the probe reports.\n\
\n\
Options:\n\
  -t     Unsolicited Probe Report Type: 0-Unicast, 1-OFDMA\n\
  -n     Specify the node id. Required for type 0-Unicast report\n\
  -m     Node mask for nodes involved (valid bit positions are 0-15).\n\
         Required for type 1-OFDMA report.\n\
  -h     Display this help and exit\n");
}

GCAP_GEN static void link_callback(void * userarg, uint32_t status)
{
    if (status == MOCA_LINK_DOWN)
    {
        printf( "Error! No Link\n");
        moca_cancel_event_loop(userarg);
    }
}

GCAP_GEN static void lmo_callback(void *userarg, struct moca_lmo_info *out)
{

    if (lmo_count < 5)
    {
        if (out->lmo_initial_ls == 0xF)
        {
            if (out->is_lmo_success)
                printf("Success\n");
            else
                printf("Failure\n");

            moca_cancel_event_loop(userarg);
        }
    }
    else
    {
        printf("Timeout\n");
        moca_cancel_event_loop(userarg);
    }

    lmo_count++;
}

GCAP_GEN int GCAP_128_main(int argc, char **argv)
{
    int ret;
    struct moca_interface_status ifstatus;
    struct moca_network_status netstatus;
    struct moca_start_ulmo start_ulmo;
    void *ctx;
    //uint32_t timeslot_set = 0;
    char *pszEnd = NULL;

    chipId = NULL;
    type = NULL;
    node = NULL;
    mask = NULL;

#if defined(STANDALONE)
    int i;
    for (i=1; i < argc; i++)
    {
      if (strcmp(argv[i], "-i") == 0)
      {
          i++;
          chipId = argv[i];
      }
      else if (strcmp(argv[i], "-n") == 0)
      {
         i++;
         node = argv[i];
      }
      else if (strcmp(argv[i], "-t") == 0)
      {
          i++;
          type = argv[i];

      }
      else if (strcmp(argv[i], "-m") == 0)
      {
          i++;
          mask = argv[i];
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
      
    while((ret = getopt(argc, argv, "hi:t:n:m:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case 't':
            type = optarg;
            break;
        case 'n':
            node = optarg;
            break;
        case 'm':
            mask = optarg;
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
    if (type == NULL)
    {
        printf( "Error!  Missing [-t type] value\n");
        return(-1);
    }

    moca_set_start_ulmo_defaults(&start_ulmo, 0);

    start_ulmo.report_type = strtoul(type, &pszEnd, 0 );
 
    if( *pszEnd != '\0' )
    {
        printf( "Error! Invalid parameter for [-t type]\n");
        return(-2);
    }

    // Unicast
    if (start_ulmo.report_type == 0)
    {
        if (node == NULL)
        {
            printf( "Error! Missing [-n node] value\n");
            return(-3);
        }

        start_ulmo.node_id = strtoul(node, &pszEnd, 0);
        if( *pszEnd != '\0' )
        {
            printf( "Error! Invalid parameter for [-n node]\n");
            return(-31);
        }

        if( start_ulmo.node_id >= MOCA_MAX_NODES )
        {
            printf( "Error! Invalid value for [-n node]\n");
            return(-32);
        }
    }
    // OFDMA
    else if (start_ulmo.report_type == 1)
    {
        if (mask == NULL)
        {
            printf( "Error! Missing [-m mask] value\n");
            return(-4);
        }

        start_ulmo.ofdma_node_mask = strtoul(mask, &pszEnd, 0);
        if( *pszEnd != '\0' )
        {
            printf( "Error! Invalid parameter for [-m mask]\n");
            return(-41);
        }
    }
    else
    {
       printf( "Error! Invalid parameter for [-t type]\n");
       return(-5);
    }

    // ----------- Initialize

    memset(&start_ulmo.subcarrier[0], 0xFFFFFFFF, sizeof(start_ulmo.subcarrier));

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-6);
    }

    // ----------- Get info

    ret = moca_get_interface_status(ctx, &ifstatus);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Interface failure\n");
        moca_close(ctx);
        return(-7);
    }

    if (ifstatus.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-8);
    }

    ret = moca_get_network_status(ctx, &netstatus);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Interface failure\n");
        moca_close(ctx);
        return(-81);
    }
    
    if (start_ulmo.report_type == 1)
    {
        if ( (netstatus.node_id != netstatus.nc_node_id) &&
             (netstatus.node_id != netstatus.backup_nc_id) )
        {
            printf( "Error! Only NC or Backup NC can initiate OFDMA LMO\n");
            moca_close(ctx);
            return(-82);
        }

        if ( (start_ulmo.ofdma_node_mask & netstatus.nodes_usable_bitmask) !=
             start_ulmo.ofdma_node_mask )
        {
            printf( "Error! Invalid parameter for [-m mask]\n");
            moca_close(ctx);
            return(-83);
        }
    }
    
    if ((start_ulmo.report_type == 0) &&
        (((1 << start_ulmo.node_id) & netstatus.nodes_usable_bitmask) == 0))
    {
        printf( "Error! Invalid value for [-n node]\n");
        moca_close(ctx);
        return(-84);
    }
    
    moca_register_link_up_state_cb(ctx, link_callback, ctx);
    moca_register_lmo_info_cb(ctx, lmo_callback, ctx);
    lmo_count = 0;

    ret = moca_set_start_ulmo(ctx, &start_ulmo);

    if (ret != MOCA_API_SUCCESS)
    {
        printf("Error! Unable to send probe request\n");
    }
    else
    {
        moca_event_loop(ctx);
    }

    // ----------- Finish
    moca_close(ctx);

    return(0);
}


