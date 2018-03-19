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

/* Rememeber to re-init these for standalone which keeps them in memory since last invocation */
static int reset = 0;          // -r option
static char *chipId = NULL;    // -i option
static char *onoff = "ON";
static int onoff_set = 0;
static int attn = -9999;

GCAP_GEN static void showUsage()
{
    printf("Usage: DCAP.105 [ ON/OFF ] [attenuation in dBs] [-r] [-h]\n\
Set MoCA Receive attenuation.\n\
(Only valid in Const RX mode)\n\
\n\
Options:\n\
 [ON/OFF] To turn MoCA Receive attenuation 'ON' OR 'OFF'. Default is ON.\n\
 [attenuation]\n\
          Level of receiver attenuation in dBs (0 - %d)\n\
  -r      Reset MoCA to make configuration changes effective\n\
  -h      Display this help and exit\n",
           MOCA_CONTINUOUS_RX_MODE_ATTN_MAX);
}

GCAP_GEN int DCAP_105_main(int argc, char **argv)
{
   int i;
   int nRet = MOCA_API_SUCCESS;
   void *ctx;

#if defined(STANDALONE)
   reset = 0;
   chipId = NULL;
   onoff = "ON";
   onoff_set = 0;
   attn = -9999;
#endif

   // ----------- Parse parameters

   for (i=1; i < argc; i++) {
      if ((strcmp(argv[i], "ON")==0) || (strcmp(argv[i], "OFF")==0)) {
         if (onoff_set) {
            fprintf(stderr, "Error! Invalid option (duplicate) %s\n", argv[i]);
            return -1;
         }
         onoff = argv[i];
         onoff_set = 1;
      }
      else if (strcmp(argv[i], "-r") == 0) {
         reset = 1;
      }
      else if (strcmp(argv[i], "-i") == 0) {
         if (i+1 >= argc || argv[i+1][0] == '-') {
            fprintf(stderr, "Error! Option %s needs a parameter\n", argv[i]);
            return -2;
         }
         i++;
         chipId = argv[i];
      }
      else if (strcmp(argv[i], "-h") == 0) {
         showUsage();
         return(0);
      }
      else {
         char *end;

         if (argv[i][0] == '-') {
            fprintf(stderr, "Error! Invalid option %s\n", argv[i]);
            return -3;
         }

         if (attn != -9999) {
            fprintf(stderr, "Error! Invalid option (duplicate) %s\n", argv[i]);
            return -4;
         }

         attn = strtol(argv[i], &end, 10);

         if (*end != '\0') {
            fprintf(stderr, "Error! Invalid option %s\n", argv[i]);
            return -5;
         }
      }
   }

   // ----------- Initialize

   ctx = moca_open(chipId);

   if (!ctx) {
      fprintf(stderr, "Error!  Unable to connect to moca instance\n");
      return -6;
   }

   /* If there are no parameters, just print out the current setting */
   if (argc == 1) {
      int32_t cur_attn = 0;

      nRet = moca_get_continuous_rx_mode_attn(ctx, &cur_attn);
      moca_close(ctx);
      if (nRet != MOCA_API_SUCCESS) {
         fprintf(stderr, "Error!  Internal failure - 1\n");
         return -7;
      }
      if (cur_attn == MOCA_CONTINUOUS_RX_MODE_ATTN_DEF)
         printf("Continous RX mode attenuation is set to OFF.\n");
      else
         printf("Continous RX mode attenuation is set to -%d dB.\n", cur_attn);
      return 0;
   }

   if (strcmp(onoff, "ON") == 0) {
      if (attn < MOCA_CONTINUOUS_RX_MODE_ATTN_MIN || attn > MOCA_CONTINUOUS_RX_MODE_ATTN_MAX) {
         fprintf(stderr, "Error!  Attenuation out of range (0 - %d)\n", MOCA_CONTINUOUS_RX_MODE_ATTN_MAX);
         moca_close(ctx);
         return -8;
      }
   }
   else {
      attn = MOCA_CONTINUOUS_RX_MODE_ATTN_DEF;
   }

   nRet = moca_set_continuous_rx_mode_attn(ctx, attn);
   if (nRet != MOCA_API_SUCCESS) {
      fprintf(stderr, "Error!  Internal failure - 2\n");
      moca_close(ctx);
      return -9;
   }

   if (reset && (moca_set_restart(ctx) != MOCA_API_SUCCESS)) {
      fprintf(stderr, "Error!  Internal - 3\n");
      moca_close(ctx);
      return -10;
   }

   moca_close(ctx);
   return(0);
}
