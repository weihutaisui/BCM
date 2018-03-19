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
    printf("Usage: GCAP.108 [-h] <CPlength> \n\
    Force CP Length to requested value. Requested value will be\n\
    rounded up to the next even number.\n\
\n\
Options:\n\
  -h                 Display this help and exit\n\
  <CPlength>   CP length, value between 64 and 255\n\
                       odd values will be rounded up to next even value\n");
}



GCAP_GEN int GCAP_108_main(int argc, char **argv)
{

    int ret;  
    uint32_t cplength;
    void *ctx;

    chipId = NULL;

    // ----------- Parse parameters

#if defined(STANDALONE)
   int i;

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
    
    while((ret = getopt(argc, argv, "hi")) != -1) 
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

    if ((argc < 2) || (argv[1] == NULL))
    {
        printf( "Error!  Missing paramter CP Length\n");
        return(-2);	
    }
    else
    {
        cplength= (uint32_t) atoi(argv[1]);
    }   
    if ( (cplength > 0 && cplength < 64) || (cplength > 255) )
    {
       printf( "Error!wrong value %d, CPLength should be between 64 and 255\n", cplength);
        return(-3);
    }

    if ( (cplength % 2 ) != 0 )
    {
       cplength +=1;
       printf("CPLength will be rounded up to the next even value %d\n", cplength);
    }
    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-4);
    }

    // ----------- Get info

      ret = __moca_set_cplength(ctx, cplength);
      if (ret != 0)
      {
         printf( "Error!internal failure\n");
         moca_close(ctx);
         return(-5);
      }
    // ----------- Finish

    moca_close(ctx);

    return(0);
}


