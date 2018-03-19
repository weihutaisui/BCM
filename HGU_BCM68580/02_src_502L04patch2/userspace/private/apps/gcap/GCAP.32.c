#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#include <mocalib.h>

static char *chipId = NULL;    // -i option
static char *str_node_id = NULL; 
static char *constellation = NULL;

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.32 <ivalue1> <ivalue2> <ivalue3> [-h]\n\
Persistently enforce an upper limit of a the constellation for \n\
all carriers reported in Type I Probe Report and GCD type I \n\
Probe Report to a single node.\n\
\n\
Options:\n\
 <ivalue1>              Node ID to limit.\n\
 <ivalue2>              Operation valid codes:\n\
                        0:  Cancel limit \n\
                        1:  P2P probe report \n\
                        2:  GCD probe report \n\
                        3:  BOTH probe reports (P2P and GCD)\n\
 <ivalue3>              Constellation size (1-10)\n\
  -h                    Display this help and exit\n");
}


GCAP_GEN int GCAP_32_main(int argc, char **argv)
{
   int ret;
   int i;
   void *ctx;
   uint32_t node_id = MOCA_MAX_NODES;
   uint32_t const_limit = 0;
   uint32_t opCode = 4;
   uint32_t nidSet = 0;
   uint32_t opset = 0;
   uint32_t limitset = 0;
   struct moca_interface_status if_status;
   struct moca_max_constellation mc;

   chipId = NULL;
   str_node_id = NULL;
   constellation = NULL;

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
   // ----------- Initialize

   if ((argc > 1) && (argv[1] != NULL))
   {
      for(i=1;i<argc;i++)
      {
         if (argv[i][0] == '-')
         {
            // Ignore
         }
         else if (!nidSet)
         {
            str_node_id = argv[i];
            nidSet = 1;
            node_id = strtoul(str_node_id, NULL, 0);
            if (node_id >= MOCA_MAX_NODES)
            {
               printf( "Error! Invalid Node ID %u\n", node_id);
               return(-1);
            }
         }
         else if (!opset)
         {
             opCode = atoi(argv[i]);
             opset = 1;
             if ( (opCode != 0) && (opCode != 1) &&
                  (opCode != 2) && (opCode != 3) )
             {
                    printf( "Error!  Wrong value should be either 0,1,2 or 3\n");
                    return(-2); 
             }
             if ( (opCode != 0) && (argc < 4) )
             {
                printf( "Error!  Missing parameter <ivalue2> or <ivalue3>\n");
                return(-21);
             }
         }
         else if (!limitset)
         {
            constellation = argv[i];
            limitset = 1;
            const_limit = strtoul(constellation, NULL, 0);
            if ( (const_limit <  1) || (const_limit > 10) )
            {
               printf( "Error! Invalid constellation value %u\n", const_limit);
               return(-3);
            }
         }
       }
   }
   else
   {
      printf( "Error!  Missing parameter <ivalue1>, <ivalue2> or <ivalue3>\n");
      return(-4); 
   }      

   if ( nidSet == 0 || opset == 0 )
   {
      printf( "Error!  Missing parameter <ivalue1>, <ivalue2> or <ivalue3>\n");
      return(-5);    
   }

   ctx = moca_open(chipId);

   if (!ctx)
   {
      printf( "Error!  Unable to connect to moca instance\n");
      return(-6);
   }

   ret = moca_get_interface_status(ctx, &if_status);

   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error!  Internal failure\n");
      moca_close(ctx);
      return(-7);
   }

   if (if_status.link_status != MOCA_LINK_UP)
   {
      printf( "Error! No Link\n");
      moca_close(ctx);
      return(-8);
   }


   ret = moca_get_max_constellation(ctx, node_id, &mc);

   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error!  Internal failure\n");
      moca_close(ctx);
      return(-9);
   }


   /* OFF enables max constellation */
   if( opCode == 0 )
   {
      moca_set_max_constellation_defaults(&mc, 0);
   }
   else
   {
      if (!constellation)
      {
         printf( "Error! Invalid parameter\n");
         moca_close(ctx);         
         return(-10);
      }
      if ( (opCode == 1) || (opCode == 3) )
      {
         if ((const_limit > MOCA_MAX_CONSTELLATION_P2P_LIMIT_50_MAX) ||
             (const_limit > MOCA_MAX_CONSTELLATION_P2P_LIMIT_100_MAX))
         {
            printf( "Error! Invalid P2P constellation value %u\n", const_limit);
            moca_close(ctx);
            return(-11);
         }

         mc.p2p_limit_50  = const_limit;
         mc.p2p_limit_100 = const_limit;
      }

      if ( (opCode == 2) || (opCode == 3) )
      {
         if ((const_limit > MOCA_MAX_CONSTELLATION_GCD_LIMIT_50_MAX) ||
             (const_limit > MOCA_MAX_CONSTELLATION_GCD_LIMIT_100_MAX))
         {
            printf( "Error! Invalid GCD constellation value %u\n", const_limit);
            moca_close(ctx);
            return(-12);
         }

         mc.gcd_limit_50  = const_limit;
         mc.gcd_limit_100 = const_limit;
      }
      
   }

   mc.node_id = node_id;
   
   ret = moca_set_max_constellation(ctx, &mc);
   if (ret != MOCA_API_SUCCESS)
   {
      printf( "Error!  Internal failure\n");
      moca_close(ctx);
      return(-13);
   }

   ret = moca_set_persistent(ctx) ;

   if (ret != 0)
   {
      printf( "Error!  Moca SetPersistent\n");
      moca_close(ctx);
      return(-14);
   }

   // ----------- Finish

   moca_close(ctx);

   return(0);
}
