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
    printf("Usage: GCAP.121 [-t timeslot] [-h]\n\
Request a Silent Probe LMO from the NC.\n\
\n\
Options:\n\
  -h     Display this help and exit\n\
  -t     timeslot, specify the timeslot of the silent probe\n");
}


GCAP_GEN int GCAP_121_main(int argc, char **argv)
{
    int ret;
    struct moca_probe_request probe_req;
    struct moca_interface_status ifstatus;
    void *ctx;
    uint32_t timeslot_set = 0;

    chipId = NULL;
    
    // ----------- Parse parameters

    probe_req.probe_type = MOCA_PROBE_REQUEST_TYPE_SILENT;

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
         probe_req.timeslots = strtoul(argv[i+1], NULL, 0);
         timeslot_set = 1;
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
    while((ret = getopt(argc, argv, "hi:t:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case 't':
            probe_req.timeslots = strtoul(optarg, NULL, 0);
            timeslot_set = 1;
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
    if (timeslot_set != 1)
    {
        printf( "Error!  Missing timeslot value\n");
        return(-1);
    }
    
    // ----------- Initialize

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-2);
    }

    // ----------- Get info

    ret = moca_get_interface_status(ctx, &ifstatus);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Interface failure\n");
        moca_close(ctx);
        return(-3);
    }

    if (ifstatus.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-4);
    }
        
    ret = __moca_set_probe_request(ctx, &probe_req);    

    if (ret == MOCA_API_SUCCESS)
    {
        printf("Success! Request completed.\n");
    }
    else
    {
        printf("Error! Probe request not sent\n");
    }

    // ----------- Finish

    moca_close(ctx);

    return(0);
}


