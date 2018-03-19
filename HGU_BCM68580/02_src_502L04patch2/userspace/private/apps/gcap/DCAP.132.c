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

#define   POWER_STATE_EVENT_BCST_REC  0   
#define   POWER_STATE_EVENT_M0_NC        1
#define   POWER_STATE_EVENT_NC_1x         2
#define   POWER_STATE_EVENT_UCST_PEN   3
#define   POWER_STATE_EVENT_TRNS_REQ   4
#define   POWER_STATE_EVENT_WUP_NT      5
#define   POWER_STATE_EVENT_WUP_UR      6
#define   POWER_STATE_EVENT_POWERMODE_MISMATCH 7
#define   POWER_STATE_EVENT_POWERMODE_UNKNOWN  8
#define   POWER_STATE_EVENT_NET_VER 9

static char *chipId = NULL;    // -i option   

static int interval = 100;
static int duration = 0;

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.132 [-t duration] [-v interval] [-h]\n\
Report Power state events.\n\
\n\
Options:\n\
  -t   Set polling duration in number of minutes (default forever until\n\
       Ctrl-C to break)\n\
  -v   Set polling interval in milliseconds.(default 100 ms)\n\
  -h   Display this help and exit\n");
}

GCAP_GEN static void callback(void * userarg, uint32_t event_code)
{
#ifndef STANDALONE
   time_t t;
   struct tm *timex;


   t=time(NULL);
   timex=localtime(&t);

   printf("%02d:%02d:%02d ", timex->tm_hour, timex->tm_min, timex->tm_sec); 
#endif

   switch(event_code) 
   {
      case POWER_STATE_EVENT_BCST_REC:
         printf("Event: POWER_STATE_EVENT_BCST_REC\n");
         break;
      case POWER_STATE_EVENT_M0_NC:
         printf("Event: POWER_STATE_EVENT_M0_NC\n");
         break;
      case POWER_STATE_EVENT_NC_1x:
         printf("Event: POWER_STATE_EVENT_NC_1x\n");
         break;
      case POWER_STATE_EVENT_UCST_PEN:
         printf("Event: POWER_STATE_EVENT_UCST_PEN\n");
         break;
      case POWER_STATE_EVENT_TRNS_REQ:
         printf("Event: POWER_STATE_EVENT_TRNS_REQ\n");
         break;
      case POWER_STATE_EVENT_WUP_NT:
         printf("Event: POWER_STATE_EVENT_WUP_NT\n");
         break;
      case POWER_STATE_EVENT_WUP_UR:
         printf("Event: POWER_STATE_EVENT_WUP_UR\n");
         break;
      case POWER_STATE_EVENT_POWERMODE_MISMATCH:
         printf("Event: POWER_STATE_EVENT_POWERMODE_MISMATCH\n");
         break;
      case POWER_STATE_EVENT_POWERMODE_UNKNOWN:
         printf("Event: POWER_STATE_EVENT_POWERMODE_UNKNOWN\n");
         break;
      case POWER_STATE_EVENT_NET_VER:
         printf("Event: POWER_STATE_EVENT_NET_VER\n");
         break;
      default:
         printf("Unknown power state event \n");
         break;
   }
}

GCAP_GEN int DCAP_132_main(int argc, char **argv)
{
    int ret;    
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
       else if (strcmp(argv[i], "-v") == 0)
       {
          i++;
          interval = atoi(argv[i]);
       }
       else if (strcmp(argv[i], "-t") == 0)
       {
          i++;
          duration = atoi(argv[i]);
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
    
    while((ret = getopt(argc, argv, "hi:v:t:")) != -1) 
    {
        switch(ret)
        {
        case 'i':
            chipId = optarg;
            break;
        case 'v':
            interval = atoi(optarg);
            break;
        case 't':
            duration = atoi(optarg);
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

   printf("Power state events monitoring started\n");
   if (duration != 0)
      printf ("Polling duration: %d \n",duration);
   else
      printf ("Polling duration: indefinite\n");   
   printf("Polling interval: %d milliseconds\n", interval);

   moca_register_power_state_event_cb(ctx, callback, ctx);

   if (duration)
      MoCAOS_Alarm(duration*60);

   ret = moca_event_loop(ctx);


   moca_close(ctx);

   return(0);
}


