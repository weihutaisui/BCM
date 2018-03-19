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

#define MOCA_MACADDR_IS_MCAST(mac)       ((((mac)->addr[0]&1) == 0x01))

GCAP_GEN static void showUsage()
{
    printf("Usage: GCAP.113 [-f 0/1/2] [-a mc_addr] [-d mc_addr] [-l] [-c] [-h]\n\
Set or report multicast filter mode and update addresses in the receive filter.\n\
\n\
Options:\n\
 -f 0/1/2   Sets the multicast filtering mode.\n\
                0, disable mcast filtering \n\
                1, enable mcast filtering\n\
                2, special mode: to enable the mcast filtering based on DFID only.\n\
 -a mc_addr	Adds a multicast address to the receive filter (01:34:56:78:9a:bc) \n\
 -d mc_addr	Deletes a multicast address from the receive filter (01:34:56:78:9a:bc)\n\
 -l      Displays the multicast filtering mode and addresses\n\
 -c      Restores  the default configuration\n\
 -h      Display this help and exit\n");
}

GCAP_GEN int GCAP_113_main(int argc, char **argv)
{
    int ret = 0;

    macaddr_t mc_filter[MOCA_MAX_ECL_MCFILTERS];
    struct moca_mcfilter_addentry  mcfil;
    struct moca_mcfilter_delentry   delmac;
    uint32_t i;
    void *ctx;
    uint32_t filtermode = 0;
    macaddr_t  pMacAddr;
    struct moca_mcfilter_table mcfilter_table; 

    char *  chipId      = NULL;     // -i option
    int     report      = 0;        // -l option
    int     defconfig   = 0;        // -c option 
    int     addopt      = 0;        // -a option
    char *  macadd      = NULL;     // -a option
    int     setfilter   = 0;        // -f option
    char *  filterEn    = NULL;     // -f option

    // ----------- Parse parameters
    
    if (argc < 2)
    {
        printf( "Error!  Missing parameter \n");
        return(-2);
    }

#if defined(STANDALONE)
   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
           i++;
           chipId = argv[i];
       }
       else if (strcmp(argv[i], "-l") == 0)
       {
          report = 1;
       }
       else if (strcmp(argv[i], "-c") == 0)
       {
          defconfig = 1;
       }
       else if (strcmp(argv[i], "-f") == 0)
       {
          i++;
          filterEn = argv[i];
          setfilter = 1;
       }
       else if (strcmp(argv[i], "-a") == 0)
       {
          i++;
          macadd = argv[i];
          if (macadd == NULL)
          {
             printf( "Error!missing mac address to add\n");
             return(-1);
          }
          addopt = 1;
       }
       else if (strcmp(argv[i], "-d") == 0)
       {
          i++;
          macadd = argv[i];
          if (macadd == NULL)
          {
             printf( "Error!missing mac address to delete\n");
             return(-1);
          }
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
    while((ret = getopt(argc, argv, "hi:a:f:d:lc")) != -1) 
    {
       switch(ret)
       {
          case 'i':
            chipId = optarg;
            break;         
          case 'a':
            macadd = optarg;
            if (macadd == NULL)
            {
               printf( "Error!missing mac address to add\n");
               return(-1);
            }
            addopt = 1;
            break;         
          case 'f':
            filterEn= optarg;
            setfilter = 1;
            break; 
          case 'd':
            macadd = optarg;
            if (macadd == NULL)
            {
               printf( "Error!missing mac address to delete\n");
               return(-1);
            }
            break;   
          case 'l':
            report = 1;
            break; 
          case 'c':
            defconfig = 1;
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
    memset (&pMacAddr, 0, sizeof(pMacAddr));
    memset(&mc_filter[0], 0x0, sizeof (mc_filter));
    memset(&mcfilter_table, 0, sizeof(mcfilter_table));
    if (filterEn != NULL)
    {
      if (strcmp(filterEn, "0") == 0)
      {
         filtermode = 0;
      }
      else if (strcmp(filterEn, "1") == 0)
      {
         filtermode = 1;
      }
      else if (strcmp(filterEn, "2") == 0)
      {
         filtermode = 2;
      }
      else
      {
         printf( "Error!  Illegal option for filtermode. Value should be 0, 1 or 2\n");
         return(-1);
      }
   }
    // ----------- Initialize
    ctx = moca_open(chipId);

    if (!ctx)
    {
        printf( "Error!  Unable to connect to moca instance\n");
        return(-3);
    }

    //Display filter and multicast addresses
   if (report)
   {
      ret = moca_get_mcfilter_enable(ctx, &filtermode);
      if (ret != MOCA_API_SUCCESS)
      {
         printf( "Error! Internal failure\n");
         moca_close(ctx);
         return(-3);
      }
      printf("Filter mode is %s %d \n",filtermode==1?"Enabled":filtermode==0?"Disabled":"DFID", filtermode);  
      if ( filtermode != 0 )
      {
         ret = moca_get_mcfilter_table(ctx,  &mcfilter_table);
         if (ret == MOCA_API_SUCCESS )  
         {
            printf ("      MoCA Filter multicast Table\n");
            printf ("=======================================================\n");
            printf (" EntryId      MAC Address\n") ;
            printf ("---------     -------------\n");
            for (i = 0; i < MOCA_MAX_ECL_MCFILTERS; i++) {
              if (!MOCA_MACADDR_IS_NULL(&mcfilter_table.addr[i]))
              {
                 printf ("%3d           %02X:%02X:%02X:%02X:%02X:%02X ",
                                      (i), MOCA_DISPLAY_MAC(mcfilter_table.addr[i])); 
                 printf ("\n") ;
              }
            }
            printf ("=======================================================\n");	 
            ret = 0;
         }
         else
         {
            printf( "Error! Internal failure\n");
            moca_close(ctx);
            return(-4);
         }
      }
   }
   
   //Restore default configuration 
   if (defconfig)
   {   
      ret = moca_set_mcfilter_enable(ctx, 0);
      if (ret != MOCA_API_SUCCESS)
      {
         printf( "Error! Internal failure\n");
         moca_close(ctx);
         return(-5);
      }
      ret = moca_set_mcfilter_clear_table(ctx);
      if (ret != MOCA_API_SUCCESS)
      {
         printf( "Error! Internal failure\n");
         moca_close(ctx);
         return(-5);
      }
      printf("Success!! Restore to default configuration\n");
   }

   if (setfilter)
   {
      ret = moca_set_mcfilter_enable(ctx, filtermode);
      if (ret != MOCA_API_SUCCESS)
      {
         printf( "Error! Internal failure\n");
         moca_close(ctx);
         return(-7);
      }
   }

   if (macadd != NULL )
   {  
      if (moca_parse_mac_addr ( macadd, &pMacAddr ) != 0 )           
      {
         printf( "Error!  invalid parameter for mac adrress filter\n");
         moca_close(ctx);
         return(-8);
      }
      
      if (!MOCA_MACADDR_IS_MCAST(&pMacAddr))
      {
         printf( "Error! Not a multicast address\n");
         moca_close(ctx);
         return(-9);
      } 

      //Check if the address already exist
      if (addopt)
      {
         MOCA_MACADDR_COPY(&mcfil.addr, &pMacAddr);
         ret = moca_set_mcfilter_addentry(ctx, &mcfil);
         if (ret != MOCA_API_SUCCESS)
         {
            moca_close(ctx);
            return(-12);
         }
      }
      else
      {
         MOCA_MACADDR_COPY(&delmac.addr, &pMacAddr);
         ret = moca_set_mcfilter_delentry(ctx, &delmac);
         if (ret != MOCA_API_SUCCESS)
         {
            moca_close(ctx);
            return(-14);
         }
      }
   }
 
    moca_close(ctx);

    return(0);
}


