#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

#include <mocalib.h>
#include <mocaint.h>
#include "GCAP_Common.h"


static char *chipId = NULL;    // -i option


GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.105  [-h]\n\
Report the privacy keys used in the network.\n\
\n\
Options:\n\
  -h   Display this help and exit\n");
}

GCAP_GEN int GCAP_105_main(int argc, char **argv)
{
    int ret;
    
    uint32_t privacyEn;
    struct moca_interface_status status;
    struct moca_network_status nws;
    struct moca_key_times kt;
    struct moca_current_keys ck;
    struct moca_pmk_initial_key pmkinitial;
    struct moca_mmk_key mmk_key;
    struct moca_aes_mm_key aes_mm_key;
    struct moca_aes_pm_key aes_pm_key;
    struct moca_permanent_salt a_salt;
    struct moca_aes_pmk_initial_key aes_pmkIni;
    uint32_t i;
    void *ctx;

    chipId = NULL;

    moca_gcap_init();

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
        printf( "Error! Internal failure 1\n");
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
        printf( "Error! Internal failure-2\n");
        moca_close(ctx);
        return(-5);
    }

    ret = moca_get_privacy_en(ctx, &privacyEn);

    if (privacyEn == MOCA_PRIVACY_EN_DEF)
    {
        printf( "Error! Privacy is not enabled!\n");
        moca_close(ctx);
        return(-6); 
    }
    
   ret = moca_get_key_times(ctx, &kt);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-3\n");
        moca_close(ctx);
        return(-7);
    }

      ret = moca_get_current_keys(ctx, &ck);
      if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-4\n");
        moca_close(ctx);
        return(-8);
    }
          
    ret = moca_get_pmk_initial_key(ctx, &pmkinitial);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-5\n");
        moca_close(ctx);
        return(-9);
    }

    ret = moca_get_mmk_key(ctx, &mmk_key);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-6\n");
        moca_close(ctx);
        return(-10);
    }
    ret = moca_get_aes_mm_key(ctx, &aes_mm_key);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-7\n");
        moca_close(ctx);
        return(-11);
    }

   ret = moca_get_aes_pm_key(ctx, &aes_pm_key);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-8\n");
        moca_close(ctx);
        return(-12);
    }

    ret = moca_get_permanent_salt(ctx, &a_salt);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-9\n");
        moca_close(ctx);
        return(-13);
    }

    ret = moca_get_aes_pmk_initial_key(ctx, &aes_pmkIni);
    if (ret != MOCA_API_SUCCESS)
    {
        printf( "Error! Internal failure-10\n");
        moca_close(ctx);
        return(-14);
    }

   printf("MMK value :");
   printf("%08X %08X \n", mmk_key.mmk_key_hi, mmk_key.mmk_key_lo);

   printf("PMK \n");
   printf("  Initial value: %08X  %08X \n", pmkinitial.pmk_initial_key_hi, pmkinitial.pmk_initial_key_lo);
   printf("  Even Key value: ");
   printf("%08X %08X ", ck.pmk_even_key[1], ck.pmk_even_key[0]);
   if ( ck.pmk_even_key[0] != 0 )   
   {
      printf("%s\n", kt.pmk_even_odd==0?"(ACTIVE)":"");
   }
   else
      printf("\n");
   printf("  Odd Key value: ");
   printf("%08X %08X ", ck.pmk_odd_key[1], ck.pmk_odd_key[0]);
   if ( ck.pmk_odd_key[0] != 0 )   
   {
      printf("%s\n", kt.pmk_even_odd==1?"(ACTIVE)":"");
   }
   else
      printf("\n");
   printf("TEK \n");
   printf("  Even Key value: ");
   printf("%08X %08X ", ck.tek_even_key[1], ck.tek_even_key[0]);
   if ( ck.tek_even_key[0] != 0 )
   {
       printf("%s\n", kt.tek_even_odd==0?"(ACTIVE)":"");
   }
   else
      printf("\n");
   printf("  Odd Key value: ");
   printf("%08X %08X ",ck.tek_odd_key[1], ck.tek_odd_key[0]);
   if ( ck.tek_odd_key[0] != 0 )
   {   
      printf("%s\n", kt.tek_even_odd==1?"(ACTIVE)":"");
   }
   else
      printf("\n");
   printf("Permanent Salt Value:");
   for (i = 3; i > 0; i--) {
      printf("%08X ", a_salt.aes_salt[i-1]);
   }
   printf("\n");
   
   printf("AMMK value: ");
   for (i = 4; i >0; i--) {
      printf("%08X ", aes_mm_key.val[i-1]);
   }
   printf("\n");

   printf("APMK  \n");
   printf("  Initial value: ");
   for (i = 4; i > 0; i--) {
      printf("%08X ", aes_pmkIni.val[i-1]);
   }
   printf("\n");
  
   printf("  Even Key value: ");
   for (i = 4; i >0; i--) {
      printf("%08X ", ck.aes_pmk_even_key[i-1]);
   }
   if (ck.aes_pmk_even_key[0] != 0 )
   {
      printf("%s\n", kt.aes_pmk_even_odd==0?"(ACTIVE)":"");
   }
   else
      printf("\n");
   printf("  Odd Key value: ");
   for (i = 4; i > 0; i--) {
      printf("%08X ", ck.aes_pmk_odd_key[i-1]);
   }
   if (ck.aes_pmk_odd_key[0] != 0 )
   {   
      printf("%s\n", kt.aes_pmk_even_odd==1?"(ACTIVE)":"");
   } 
   else
      printf("\n"); 
   printf("ATEK  \n");
   printf("  Even Key value: ");
   for (i = 4; i > 0; i--) {
      printf("%08X ", ck.aes_tek_even_key[i-1]);
   }
   if (ck.aes_tek_even_key[0] != 0 )
   {     
      printf("%s\n", kt.aes_tek_even_odd==0?"(ACTIVE)":"");
   }
   else
      printf("\n");
   printf("  Odd Key value: ");
   for (i = 4; i > 0; i--) {
      printf("%08X ", ck.aes_tek_odd_key[i-1]);
   }
   if (ck.aes_tek_odd_key[0] != 0 )
   {    
      printf("%s\n", kt.aes_tek_even_odd==1?"(ACTIVE)":"");
   }
   printf("\n");


   moca_close(ctx);

    return(0);
}


