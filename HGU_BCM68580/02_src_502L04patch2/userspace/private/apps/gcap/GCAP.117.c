#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>


#define PQOS_ATTR_FLAG        (1 << 0)
#define AGGR_ATTR_FLAG        (1 << 1)
#define MAP_PROC_ATTR_FLAG    (1 << 2)
#define BONDED_AGGR_ATTR_FLAG (1 << 3)

static char *chipId = NULL;    // -i option


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.117 <mask of NodeIDs> [-h]\n\
Request Device Discovery for nodes in the network.\n\
Default operation is to show attributes for all nodes.\n\
\n\
Options:\n\
  <mask of NodeIDs>  Bitmask of node IDs to request\n\
                     discovery for, no entry specifies\n\
                     all nodes.\n\
    -h               Display this help and exit\n");
}

GCAP_GEN static int printNodeInfo(void *ctx, uint32_t node, struct moca_dd_init_out * p_dd, uint32_t attr_flags)
{
   struct moca_gen_node_status gns;
   int ret;

   ret = moca_get_gen_node_status(ctx, node, &gns);    

   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error!  mocalib failure\n");
      return(-6);
   }

   printf("\nNode %d: %02X:%02X:%02X:%02X:%02X:%02X\n", 
      node, MOCA_DISPLAY_MAC(gns.eui));

   if (attr_flags & PQOS_ATTR_FLAG)
   {
      printf("   Supported Ingress PQoS Flows: %d\n", p_dd->ingress_pqos_flows[node]);
      printf("   Supported Egress PQoS Flows : %d\n", p_dd->egress_pqos_flows[node]);
   }
   if (attr_flags & AGGR_ATTR_FLAG)
   {
      printf("   PDU Number                  : %d\n", p_dd->aggr_pdus[node]);
      printf("   Aggregation Size            : %d KB\n", p_dd->aggr_size[node]/1024);
   }
   if (attr_flags & MAP_PROC_ATTR_FLAG)
      printf("   AE Number                   : %d\n", p_dd->ae_number[node]);

   if (attr_flags & BONDED_AGGR_ATTR_FLAG)
   {
      printf("   Bonded PDU Number           : %d\n", p_dd->aggr_pdus_bonded[node]);
      printf("   Bonded Aggregation Size     : %d KB\n", p_dd->aggr_size_bonded[node]/1024);
   }

   return(0);

}

GCAP_GEN int GCAP_117_main(int argc, char **argv)
{
    int ret;
    
    struct moca_interface_status ifstatus;
    struct moca_dd_init_out dd_info;
    uint32_t node_mask = 0xFFFF;
    uint32_t node_mask_idx = 1;
    uint32_t attr_flags = 0;
    uint32_t i;
    void *ctx;
    char *end;

    chipId = NULL;
 
    // ----------- Parse parameters
#if defined(STANDALONE)

   for (i=1; i < argc; i++)
   {
        if (strcmp(argv[i], "-i") == 0)
        {
           i++;
           chipId = argv[i];
           node_mask_idx += 2;
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            attr_flags |= AGGR_ATTR_FLAG;
            node_mask_idx++;
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            attr_flags |= MAP_PROC_ATTR_FLAG;
            node_mask_idx++;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            attr_flags |= PQOS_ATTR_FLAG;
            node_mask_idx++;
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            attr_flags |= BONDED_AGGR_ATTR_FLAG;
            node_mask_idx++;
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
    
    while((ret = getopt(argc, argv, "hi:ampb")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            node_mask_idx += 2;
            break;
        case 'a':
            attr_flags |= AGGR_ATTR_FLAG;
            node_mask_idx++;
            break;
        case 'm':
            attr_flags |= MAP_PROC_ATTR_FLAG;
            node_mask_idx++;
            break;
        case 'p':
            attr_flags |= PQOS_ATTR_FLAG;
            node_mask_idx++;
            break;
         case 'b':
             attr_flags |= BONDED_AGGR_ATTR_FLAG;
             node_mask_idx++;
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
    if (attr_flags == 0)
       attr_flags = (PQOS_ATTR_FLAG | MAP_PROC_ATTR_FLAG | AGGR_ATTR_FLAG | BONDED_AGGR_ATTR_FLAG);

    // ----------- Initialize

    if (node_mask_idx < argc)
    {
        node_mask = strtoul(argv[node_mask_idx],  &end, 16);
       if (*end != '\0')
         {
                printf( "Error!  Invalid parameter\n");
                return(-2);
          }
    }
    if ((node_mask == 0) || (node_mask > 0xFFFF))
    {
        printf( "Error!  Invalid parameter - bitmask\n");
        return(-3);
    }

    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-4);
    }

    // ----------- Get info

    ret = moca_get_interface_status(ctx, &ifstatus);

    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error!  Interface failure\n");
        moca_close(ctx);
        return(-5);
    }

    if (ifstatus.link_status != MOCA_LINK_UP)
    {
        printf( "Error! No Link\n");
        moca_close(ctx);
        return(-6);
    }
        
    ret = moca_do_dd_init(ctx, node_mask, &dd_info);    

    printf("Result: %s\n", moca_l2_error_name(dd_info.responsecode));

    if (dd_info.responsecode == MOCA_L2_SUCCESS)
    {
       for (i = 0; i < MOCA_MAX_NODES; i++)
       {
           if ((node_mask & (1 << i)) &&
                (dd_info.responded_nodemask & (1 <<i)))
           {
               ret = printNodeInfo(ctx, i, &dd_info, attr_flags);
               if (ret != 0)
                  break;
           }
       }
    }

    // ----------- Finish

    moca_close(ctx);

    return(ret);
}


