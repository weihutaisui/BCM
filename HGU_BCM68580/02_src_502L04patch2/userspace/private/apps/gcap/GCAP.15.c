#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <moca_os.h>
#include <mocalib.h>
#include "GCAP_Common.h"


static char *chipId = NULL;    // -i option
static int interval = 0;
static int endless = 0;

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.15 [-t <polling duration>] [-h]\n\
Report the time it takes to complete LMO cycle with a DUT.\n\
\n\
Options:\n\
  -t    Set polling duration in number of minutes (default forever until\n\
        Ctrl-C to break)\n\
  -e    Allow polling to continue if link goes down (default polling\n\
        stops when MoCA link goes down)\n\
  -h    Display this help and exit\n");
}

GCAP_GEN static void callback(void *userarg, struct moca_lmo_info *out)
{
    struct moca_gen_node_status nodestatus;
    int ret;
    void *ctx = userarg;
    
    ret = moca_get_gen_node_status(ctx, out->lmo_node_id, &nodestatus);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Internal failure 2\n");
        moca_cancel_event_loop(userarg);
        return;
    }

    printf("LMO node %2d MAC Address=%02x:%02x:%02x:%02x:%02x:%02x  Duration=%2ds  %s\n",
        out->lmo_node_id, 
        MOCA_DISPLAY_MAC(nodestatus.eui),
        ((out->lmo_duration_dsec + 5)/10),
        out->is_lmo_success?"LMO Success":"LMO_Timeout");
}


GCAP_GEN static void link_cb(void * userarg, uint32_t status)
{
    if (status == 0)
    {
        printf("Error! No Link\n");
        moca_cancel_event_loop(userarg);
    }
}


GCAP_GEN int GCAP_15_main(int argc, char **argv)
{
    int ret;
    
    struct moca_interface_status status;
    struct moca_network_status nws;
    void *ctx;

    chipId = NULL;

#if defined(STANDALONE)
   int i;

   for (i=1; i < argc; i++)
   {
      if (strcmp(argv[i], "-i") == 0)
      {
         i++;
         chipId = argv[i];

      }
      else if (strcmp(argv[i], "-t") == 0)
      {
         i++;
         interval = atoi(argv[i]);
      }
      else if (strcmp(argv[i], "-e") == 0)
      {
         endless = 1; 
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
    
    while((ret = getopt(argc, argv, "hei:t:")) != -1) 
    {
        switch(ret)
        {
        case 'e':
            endless = 1;
            break;
        case 'i':
            chipId = optarg;
            break;
        case 't':
            interval = atoi(optarg);
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
    // ----------- Initialize

    ctx=moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    // ----------- Get info
    ret = moca_get_interface_status(ctx, &status);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure\n");
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
        printf( "Error! Internal failure\n");
        moca_close(ctx);
        return(-3);
    }

    printf("Management GN Node ID= %d\n", nws.node_id);
    
    moca_register_lmo_info_cb(ctx, callback, ctx);

    if (endless == 0)
       moca_register_link_up_state_cb(ctx, link_cb, ctx);

    if (interval)
        MoCAOS_Alarm(interval*60);

    ret = moca_event_loop(ctx);

    // ----------- Finish

    moca_close(ctx);

    return(ret);
}

