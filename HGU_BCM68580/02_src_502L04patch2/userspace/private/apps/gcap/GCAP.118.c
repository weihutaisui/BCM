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
static char *mask = NULL;      // -d option
static char *chnlId = NULL;    // -c option


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.118 -n <nodeId> -t <EVM|QUIET> -c <Channel Num> -d <destNodeId> [-h]\n\
Enables a node to request alternate channel EVM probe assessment\n\
or alternate channel QUIET line assessment.\n\
\n\
Options:\n\
  -n <nodeId>       Specify the source node id.\n\
  -t <EVM|QUIET>    Specify the type of ACA, can be EVM or QUIET \n\
  -c <Channel Num>  Specify the channel number. \n\
  -d <destNodeId>   Specify the nodeId of single reporting node. \n\
  -h     Display this help and exit\n");
}

GCAP_GEN static void aca_print_report(struct moca_aca_out *in)
{
   uint32_t i;

   printf("ACA Success\n");
   printf("Assessment Type         %s(%d)\n", (in->aca_type == 1)? "QUIET":"EVM", in->aca_type);
   printf("Rx Status:              %d\n", in->rx_status);
   if ( in->aca_type == 2 )
   {
      printf("Tx Status:              %d\n", in->tx_status);
      printf("Total Relative Power:   %d dB\n", in->relative_power);
   }
   printf("Total Power:            %d dBm\n", in->total_power);
   printf("Power profile: ");
   for (i = 0; i < 512; i++) {
      printf("%4d ", in->power_profile[i]);
      if (i % 8 == 7) printf("\n                    ");
   }
   printf("\n");  

}

GCAP_GEN static const char *aca_error_name(uint32_t l2_error)
{
   switch(l2_error) {
      case 0:
         return("Success! LMO request completed");
      case 1:
         return("LMO request failed to tune");
      case 2:
         return("Failed: NO EVM PROBE");
      case 3:
         return("ACA request Failed");
   }
   return(NULL);
}

GCAP_GEN int GCAP_118_main(int argc, char **argv)
{
    int ret;
    struct moca_interface_status ifstatus;
    struct moca_network_status netstatus;
    struct moca_gen_node_status gsn;
    void *ctx;
    uint32_t maskbit;
    char *pszEnd = NULL;
    struct moca_aca_in in;
    struct moca_aca_out out;

    chipId = NULL;
    type = NULL;
    node = NULL;
    mask = NULL;
    chnlId = NULL;

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
      else if (strcmp(argv[i], "-d") == 0)
      {
         i++;
         mask = argv[i];
      }
      else if (strcmp(argv[i], "-c") == 0)
      {
         i++;
         chnlId = argv[i];
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
      
    while((ret = getopt(argc, argv, "hi:t:n:c:d:")) != -1) 
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
        case 'd':
            mask = optarg;
            break;
        case 'c':
            chnlId = optarg;
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
    memset(&in, 0, sizeof(in));

    if (type == NULL)
    {
        printf( "Error!  Missing [-t type] value\n");
        return(-1);
    }
    if (strcmp(type, "EVM") == 0)
    {
       in.type = 2;
    }
    else if (strcmp(type, "QUIET") == 0)
    {
       in.type = 1;
    }
    else
    {
       printf( "Error! Invalid parameter for [-t type]\n");
       return(-2);
    }


    if (node == NULL)
    {
       printf( "Error! Missing [-n node] value\n");
       return(-3);
    }

    in.src_node = strtoul(node, &pszEnd, 0);
    if( *pszEnd != '\0' )
    {
       printf( "Error! Invalid parameter for [-n node]\n");
       return(-31);
    }

    if (mask == NULL)
    {
       printf( "Error! Missing [-d destNodeId] value\n");
       return(-4);
    }

    maskbit = strtoul(mask, &pszEnd, 0);
    if( *pszEnd != '\0' )
    {
       printf( "Error! Invalid parameter for [-d destNodeId ]\n");
       return(-41);
    }
    in.dest_nodemask = (1 <<  maskbit);

    if (chnlId == NULL)
    {
       printf( "Error! Missing [-c Channel Num] value\n");
       return(-5);
    }

    in.channel = strtoul(chnlId, &pszEnd, 0);
    if( *pszEnd != '\0' )
    {
       printf( "Error! Invalid parameter for [-c Channel Num ]\n");
       return(-51);
    }
    if (in.channel > 64)
    {
       fprintf( stderr, "Error! Invalid channel number (0-64)\n");
       return(-52);
    }


    // ----------- Initialize

    in.num_probes = 1;

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
        return(-9);
    }

    if(! (netstatus.connected_nodes & (1 << in.src_node )))
    {
        printf( "Error! Source node must have the Link Up\n");
        moca_close(ctx);
        return(-10);
    }

    if(! (netstatus.connected_nodes & (1 << maskbit )))
    {
        printf( "Error! Requested node must have the Link Up\n");
        moca_close(ctx);
        return(-11);
    }

    ret = moca_get_gen_node_status(ctx, in.src_node, &gsn);
    if (ret != MOCA_API_SUCCESS)
    {
       printf( "Error!  Interface failure\n");
       moca_close(ctx);
       return(-12);
    }
    if ((gsn.protocol_support >> 24) < MoCA_VERSION_2_0)
    {
      printf( "Error! The Tx node must be a 2.0 node \n");
      moca_close(ctx);
      return(-13);
   }

   ret = moca_get_gen_node_status(ctx, maskbit, &gsn);
   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error!	Interface failure\n");
      moca_close(ctx);
      return(-14);
   }
   if ((gsn.protocol_support >> 24) < MoCA_VERSION_2_0) 
   {
      printf( "Error! The Rx node must be a 2.0 node \n");
      moca_close(ctx);
      return(-15);
   }

    ret = moca_do_aca(ctx, &in, &out);
    if (ret == MOCA_API_SUCCESS) 
   {
      printf( "%s\n", aca_error_name(out.aca_status));
      if (out.aca_status == 0)
         aca_print_report(&out);

   }
   else
   {
      printf( "moca_do_aca returned error %d\n", ret ) ;
   }
 
    // ----------- Finish
    moca_close(ctx);

    return(0);
}


