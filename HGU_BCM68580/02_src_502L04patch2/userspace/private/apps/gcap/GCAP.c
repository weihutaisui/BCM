#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#if !defined(STANDALONE)
#include <libgen.h>
#endif
#include <mocalib.h>
#include "GCAP_Common.h"

#if defined(STANDALONE)
#define basename(x) x
#endif

static char *chipId = NULL;    // -i option
static int show_version = 0; // -v option
static int generate_links = 0; // -g option

typedef int (*gcap_fn)(int argc, char **argv);

typedef struct
{
   char *cmdstr;
   gcap_fn cmdfn;
   char *description;
} functions;


extern int GCAP_01_main(int argc, char **argv);
extern int GCAP_02_main(int argc, char **argv);
extern int GCAP_03_main(int argc, char **argv);
extern int GCAP_04_main(int argc, char **argv);
extern int GCAP_05_main(int argc, char **argv);
extern int GCAP_06_main(int argc, char **argv);
extern int GCAP_07_main(int argc, char **argv);
extern int GCAP_08_main(int argc, char **argv);
extern int GCAP_09_main(int argc, char **argv);
extern int GCAP_10_main(int argc, char **argv);
extern int GCAP_13_main(int argc, char **argv);
extern int GCAP_14_main(int argc, char **argv);
extern int GCAP_15_main(int argc, char **argv);
extern int GCAP_16_main(int argc, char **argv);
extern int GCAP_17_main(int argc, char **argv);
extern int GCAP_18_main(int argc, char **argv);
extern int GCAP_19_main(int argc, char **argv);
extern int GCAP_20_main(int argc, char **argv);
extern int GCAP_21_main(int argc, char **argv);
extern int GCAP_22_main(int argc, char **argv);
extern int GCAP_23_main(int argc, char **argv);
extern int GCAP_24_main(int argc, char **argv);
extern int GCAP_25_main(int argc, char **argv);
extern int GCAP_26_main(int argc, char **argv);
extern int GCAP_27_main(int argc, char **argv);
extern int GCAP_30_main(int argc, char **argv);
extern int GCAP_32_main(int argc, char **argv);
extern int GCAP_34_main(int argc, char **argv);
extern int GCAP_35_main(int argc, char **argv);
extern int GCAP_37_main(int argc, char **argv);
extern int GCAP_38_main(int argc, char **argv);
extern int GCAP_39_main(int argc, char **argv);
extern int GCAP_40_main(int argc, char **argv);
extern int GCAP_42_main(int argc, char **argv);
extern int GCAP_43_main(int argc, char **argv);
extern int GCAP_101_main(int argc, char **argv);
extern int GCAP_102_main(int argc, char **argv);
extern int GCAP_103_main(int argc, char **argv);
extern int GCAP_104_main(int argc, char **argv);
extern int GCAP_105_main(int argc, char **argv);
extern int GCAP_106_main(int argc, char **argv);
extern int GCAP_107_main(int argc, char **argv);
extern int GCAP_108_main(int argc, char **argv);
extern int GCAP_109_main(int argc, char **argv);
extern int GCAP_110_main(int argc, char **argv);
extern int GCAP_111_main(int argc, char **argv);
extern int GCAP_112_main(int argc, char **argv);
extern int GCAP_113_main(int argc, char **argv);
extern int GCAP_114_main(int argc, char **argv);
extern int GCAP_115_main(int argc, char **argv);
extern int GCAP_116_main(int argc, char **argv);
extern int GCAP_117_main(int argc, char **argv);
extern int GCAP_118_main(int argc, char **argv);
extern int GCAP_119_main(int argc, char **argv);
extern int GCAP_120_main(int argc, char **argv);
extern int GCAP_121_main(int argc, char **argv);
extern int GCAP_122_main(int argc, char **argv);
extern int GCAP_123_main(int argc, char **argv);
extern int GCAP_124_main(int argc, char **argv);
extern int GCAP_125_main(int argc, char **argv);
extern int GCAP_126_main(int argc, char **argv);
extern int GCAP_128_main(int argc, char **argv);
extern int GCAP_130_main(int argc, char **argv);
extern int GCAP_131_main(int argc, char **argv);
extern int GCAP_132_main(int argc, char **argv);
extern int GCAP_133_main(int argc, char **argv);
extern int GCAP_134_main(int argc, char **argv);
extern int GCAP_901_main(int argc, char **argv);


extern int DCAP_02_main(int argc, char **argv);
extern int DCAP_03_main(int argc, char **argv);
extern int DCAP_08_main(int argc, char **argv);
extern int DCAP_101_main(int argc, char **argv);
extern int DCAP_102_main(int argc, char **argv);
extern int DCAP_103_main(int argc, char **argv);
extern int DCAP_104_main(int argc, char **argv);
extern int DCAP_105_main(int argc, char **argv);
extern int DCAP_107_main(int argc, char **argv);
extern int DCAP_109_main(int argc, char **argv);
extern int DCAP_110_main(int argc, char **argv);
extern int DCAP_111_main(int argc, char **argv);
extern int DCAP_112_main(int argc, char **argv);
extern int DCAP_115_main(int argc, char **argv);
extern int DCAP_116_main(int argc, char **argv);
extern int DCAP_122_main(int argc, char **argv);
extern int DCAP_123_main(int argc, char **argv);
extern int DCAP_124_main(int argc, char **argv);
extern int DCAP_125_main(int argc, char **argv);
extern int DCAP_132_main(int argc, char **argv);
extern int DCAP_16_main(int argc, char **argv);
extern int DCAP_18_main(int argc, char **argv);
extern int DCAP_19_main(int argc, char **argv);
extern int DCAP_35_main(int argc, char **argv);
extern int DCAP_37_main(int argc, char **argv);
extern int DCAP_38_main(int argc, char **argv);
extern int DCAP_40_main(int argc, char **argv);
extern int DCAP_41_main(int argc, char **argv);
extern int DCAP_42_main(int argc, char **argv);
extern int DCAP_44_main(int argc, char **argv);
extern int DCAP_45_main(int argc, char **argv);
extern int DCAP_46_main(int argc, char **argv);


functions dcaps[] = {
  { "DCAP.02", DCAP_02_main, "DCAP.02 - Set Golden Node to operate at a single frequency or network search."},
  { "DCAP.03", DCAP_03_main, "DCAP.03 - Set Golden Node to operate in continuous power TX mode."},
  { "DCAP.08", DCAP_08_main, "DCAP.08 - Set or report LOF of the node."},
  { "DCAP.16", DCAP_16_main, "DCAP.16 - Enable/disable privacy and set the password."},
  { "DCAP.18", DCAP_18_main, "DCAP.18 - Report the Privacy Status and the password on the node's MoCA interface."},
  { "DCAP.19", DCAP_19_main, "DCAP.19 - Set the DUT to default configuration and restart the DUT."},
  { "DCAP.35", DCAP_35_main, "DCAP.35 - Report or switch network search among D-low, D-High, and Extended D band."},
  { "DCAP.37", DCAP_37_main, "DCAP.37 - Set DUT to operate in Preferred NC mode."},
  { "DCAP.38", DCAP_38_main, "DCAP.38 - Set DUT to operate in CW mode."},
  { "DCAP.40", DCAP_40_main, "DCAP.40 - Send reset request to all the nodes in the network."},
  { "DCAP.41", DCAP_41_main, "DCAP.41 - Set node to operate in non-transmit mode."},
  { "DCAP.42", DCAP_42_main, "DCAP.42 - Set or report RLAPMEN and RLAPM."},
  { "DCAP.44", DCAP_44_main, "DCAP.44 - Set node to operate in loopback mode."},
  { "DCAP.45", DCAP_45_main, "DCAP.45 - Set Entry Node capability of the node"},
  { "DCAP.46", DCAP_46_main, "DCAP.46 - Set or report SAPMEN, SAPM and ARPL_THLD"},  
  { "DCAP.101", DCAP_101_main, "DCAP.101 - Set the PER mode for the local node."},
  { "DCAP.102", DCAP_102_main, "DCAP.102 - Report the PER mode for the local node."},
  { "DCAP.103", DCAP_103_main, "DCAP.103 - Set or report beacon power of the node."},
  { "DCAP.104", DCAP_104_main, "DCAP.104 - Set or report GN TPC mode."},
  { "DCAP.105", DCAP_105_main, "DCAP.105 - Set MoCA Receive attenuator."},
  { "DCAP.107", DCAP_107_main, "DCAP.107 - Set or report the TLP mode of the node."},
  { "DCAP.109", DCAP_109_main, "DCAP.109 - Disable or Enable TURBO_MODE of the node."},
  { "DCAP.110", DCAP_110_main, "DCAP.110 - Report TURBO_MODE of the node."},
  { "DCAP.111", DCAP_111_main, "DCAP.111 - Set POLICING of the node."},
  { "DCAP.112", DCAP_112_main, "DCAP.112 - Report the POLICING of the node."},
  { "DCAP.115", DCAP_115_main, "DCAP.115 - Report the power state capabilities of the node."},
  { "DCAP.116", DCAP_116_main, "DCAP.116 - Request the node to transition to a different power state."},
  { "DCAP.122", DCAP_122_main, "DCAP.122 - Configure SEQ_NUMmr."},
  { "DCAP.123", DCAP_123_main, "DCAP.123 - Report/Set the LOF_UPDATE of the node."},
  { "DCAP.124", DCAP_124_main, "DCAP.124 - Report the node current power state"},
  { "DCAP.125", DCAP_125_main, "DCAP.125 - Set the retransmission priorities mask."}, 
  { "DCAP.132", DCAP_132_main, "DCAP.132 - Report Power state events."},
};

#define NCMDS_DCAP (sizeof(dcaps)/sizeof(functions))

functions gcaps[] = {

   { "GCAP.01", GCAP_01_main, "GCAP.01 - Report time-stamps whenever key exchanges happen between nodes."},
   { "GCAP.02", GCAP_02_main, "GCAP.02 - Set Golden Node to operate at a single frequency or network search."},
   { "GCAP.03", GCAP_03_main, "GCAP.03 - Set Golden Node to operate in continuous power TX mode."},
   { "GCAP.04", GCAP_04_main, "GCAP.04 - Report NC node ID and MAC address."},
   { "GCAP.05", GCAP_05_main, "GCAP.05 - Report backup NC node ID and MAC address."},
   { "GCAP.06", GCAP_06_main, "GCAP.06 - Report bit rates between the GN & other nodes."},
   { "GCAP.07", GCAP_07_main, "GCAP.07 - Reports beacon information whenever a valid beacon is detected."},
   { "GCAP.08", GCAP_08_main, "GCAP.08 - Set/Clear LOF (Last Operational Frequency)."},
   { "GCAP.09", GCAP_09_main, "GCAP.09 - Set or Report Taboo frequency mask."},
   { "GCAP.10", GCAP_10_main, "GCAP.10 - Display the current tuned frequency of the primary, secondary, and beacon channel."},
   { "GCAP.13", GCAP_13_main, "GCAP.13 - Report Node ID, MAC address and total number of nodes in the network."},
   { "GCAP.14", GCAP_14_main, "GCAP.14 - Report frequency offset between Golden Node & DUT."},
   { "GCAP.15", GCAP_15_main, "GCAP.15 - Report the time it takes to complete LMO cycle with a DUT."},
   { "GCAP.16", GCAP_16_main, "GCAP.16 - Enable/Disable privacy and set the password."},
   { "GCAP.17", GCAP_17_main, "GCAP.17 - Report time-stamps whenever nodes join in or drop out."},
   { "GCAP.18", GCAP_18_main, "GCAP.18 - Read back the privacy password."},
   { "GCAP.19", GCAP_19_main, "GCAP.19 - Reset to default configuration."},
   { "GCAP.20", GCAP_20_main, "GCAP.20 - Create/Update PQoS flows on any node."},
   { "GCAP.21", GCAP_21_main, "GCAP.21 - Delete PQoS flows on any node."},
   { "GCAP.22", GCAP_22_main, "GCAP.22 - Display all existing PQoS flows on any node."},
   { "GCAP.23", GCAP_23_main, "GCAP.23 - Query any PQoS flow in the network."},
   { "GCAP.24", GCAP_24_main, "GCAP.24 - Read or clear packet aggregation and fragmentation statistic."},
   { "GCAP.25", GCAP_25_main, "GCAP.25 - Initiate an FMR request fro a specified node."},
   { "GCAP.26", GCAP_26_main, "GCAP.26 - Display the current network (Beacon) MoCA version."},
   { "GCAP.27", GCAP_27_main, "GCAP.27 - Transmit an Out-of-Order Request Protocol IE."},
   { "GCAP.30", GCAP_30_main, "GCAP.30 - Report time-stamps whenever Network MoCA version changes happen."},
   { "GCAP.32", GCAP_32_main, "GCAP.32 - Set/Cancel the upper limit of the constellation of a particular node."},
   { "GCAP.34", GCAP_34_main, "GCAP.34 - Set Highest Aggregation Level."},
   { "GCAP.35", GCAP_35_main, "GCAP.35 - Switch network search band among D-low, D-High, and Extended D band."},
   { "GCAP.37", GCAP_37_main, "GCAP.37 - Turn ON/OFF Preferred NC mode."},
   { "GCAP.38", GCAP_38_main, "GCAP.38 - Set Golden Node to operate in continuous power TX mode."},
   { "GCAP.39", GCAP_39_main, "GCAP.39 - Report bit rates and modulation profiles between GN & other nodes."},
   { "GCAP.40", GCAP_40_main, "GCAP.40 - Initiate an MR transaction Request to the specified nodes."},
   { "GCAP.42", GCAP_42_main, "GCAP.42 - Turn RLAPM on/off."},
   { "GCAP.43", GCAP_43_main, "GCAP.43 - Set Golden Node Transmit Power Backoff to xx dB"}, 
   { "GCAP.101", GCAP_101_main, "GCAP.101 - Set the PER mode for the local node."},
   { "GCAP.102", GCAP_102_main, "GCAP.102 - Report the PER mode for the local node."},
   { "GCAP.103", GCAP_103_main, "GCAP.103 - Set Golden Node to operate with beacon backoff enabled."},
   { "GCAP.104", GCAP_104_main, "GCAP.104 - Set or report GN TPC mode."},
   { "GCAP.105", GCAP_105_main, "GCAP.105 - Report privacy keys used in the network."},
   { "GCAP.106", GCAP_106_main, "GCAP.106 - Set or report the POLICING of the node."},
   { "GCAP.107", GCAP_107_main, "GCAP.107 - Set or report the TLP mode of the node."},
   { "GCAP.108", GCAP_108_main, "GCAP.108 - Force CP Length to requested value."},
   { "GCAP.109", GCAP_109_main, "GCAP.109 - Disable or Enable TURBO_MODE of the node."},
   { "GCAP.110", GCAP_110_main, "GCAP.110 - Report TURBO_MODE of the node."},
   { "GCAP.111", GCAP_111_main, "GCAP.111 - Set POLICING of the node."},
   { "GCAP.112", GCAP_112_main, "GCAP.112 - Report the POLICING of the node."},
   { "GCAP.113", GCAP_113_main, "GCAP.113 - Set or report multicast filter mode and update addresses in the receive filter."},
   { "GCAP.114", GCAP_114_main, "GCAP.114 - Report the current power state of the node."},
   { "GCAP.115", GCAP_115_main, "GCAP.115 - Report the power state capabilities of the node."},
   { "GCAP.116", GCAP_116_main, "GCAP.116 - Request the node to transition to a different power state."},
   { "GCAP.117", GCAP_117_main, "GCAP.117 - Request Device Discovery for nodes in the network."},
   { "GCAP.118", GCAP_118_main, "GCAP.118 - Enables a node to request alternate channel EVM probe assessment."},
   { "GCAP.119", GCAP_119_main, "GCAP.119 - Request a node to transmit a Receiver Determined Probe (RDP)."},
   { "GCAP.120", GCAP_120_main, "GCAP.120 - Request a Loopback Probe LMO from the NC."},
   { "GCAP.121", GCAP_121_main, "GCAP.121 - Request a Silent Probe LMO from the NC."},
   { "GCAP.122", GCAP_122_main, "GCAP.122 - Configure SEQ_NUMmr."},
   { "GCAP.123", GCAP_123_main, "GCAP.123 - Report/Set the LOF_UPDATE of the node."},
   { "GCAP.124", GCAP_124_main, "GCAP.124 - Report the node current power state"},
   { "GCAP.125", GCAP_125_main, "GCAP.125 - Set the retransmission priorities mask."},
   { "GCAP.126", GCAP_126_main, "GCAP.126 - Report the current retransmission mode & retransmission priority mask."},
   { "GCAP.128", GCAP_128_main, "GCAP.128 - Request NC to transition to Unsolicited Probe Report state."},
   { "GCAP.130", GCAP_130_main, "GCAP.130 - Display selected values from OFDMA Subchannel Definition Table."},
   { "GCAP.131", GCAP_131_main, "GCAP.131 - Display selected values from OFDMA Subchannel Assignment Table."},
   { "GCAP.132", GCAP_132_main, "GCAP.132 - Report Power state events."},
   { "GCAP.133", GCAP_133_main, "GCAP.133 - Set GN to drop 1 out of every Nth MSDU."},
   { "GCAP.134", GCAP_134_main, "GCAP.134 - Set or report GN beaconing during network search."},
   { "GCAP.901", GCAP_901_main, "GCAP.901 - Set PSM (internal power-save) mode."},
};

#define NCMDS (sizeof(gcaps)/sizeof(functions))

GCAP_GEN static int print_version(void * ctx)
{
   struct moca_node_status ns;
   struct moca_drv_info    di;
   int ret = MOCA_API_SUCCESS;
   
   memset (&ns, 0, sizeof(ns));
   moca_get_node_status(ctx, &ns);
   ret = moca_get_drv_info(ctx, 0, &di);

   if (ret != MOCA_API_SUCCESS)
   {
      printf( "GetVersion error\n" ) ;
   }
   else
   {
      uint32_t coreversionMajor, coreversionMinor, coreversionBuild ;
      coreversionMajor = ns.moca_sw_version_major;
      coreversionMinor = ns.moca_sw_version_minor ;
      coreversionBuild = ns.moca_sw_version_rev;
      printf ("\nMoCA Version\n") ;
      printf ("-----------------------\n") ;
      printf ("MoCA coreHWVersion   : 0x%x\n", di.chip_id) ;
      printf ("MoCA coreSWVersion   : %d.%d.%d\n", coreversionMajor, coreversionMinor, 
                   coreversionBuild) ;
      printf ("MoCA self version    : 0x%x\n", ns.self_moca_version) ;
      printf ("MoCA driver version  : %d.%d.%x\n", di.version >> 16, di.version & 0xFFFF,
                   di.build_number) ;
      printf ("-----------------------\n") ;
   }
   return(0);
}

#if defined(STANDALONE)
GCAP_GEN int GCAP_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    int ret = MOCA_API_SUCCESS;
    void *ctx;
    int i = 0;
    char *base=basename(argv[0]);
   
    // ----------- Parse parameters

    // command name linked to GCAP
    if ((strcmp(base, "GCAP") != 0) && (strcmp(base, "DCAP") != 0))
    {
       if (base[0] == 'G') // GCAPS
       {
          for(i=0;i<NCMDS;i++)
             if ((strcmp(base, gcaps[i].cmdstr) == 0) &&
                 (strlen(base) == strlen(gcaps[i].cmdstr)))
                return(gcaps[i].cmdfn(argc, argv));
         if (i >= NCMDS)
         {
            printf("%s does not exist \n", base);
            return(0);
         }
       }
       if (base[0] == 'D') // DCAPS
          for(i=0;i<NCMDS_DCAP;i++)
             if ((strcmp(base, dcaps[i].cmdstr) == 0) &&
                 (strlen(base) == strlen(dcaps[i].cmdstr)))
                return(dcaps[i].cmdfn(argc, argv));
          if (i >= NCMDS)
          {
             printf("%s does not exist \n", base);
             return(0);
          }
    }

    // command name passed as first parameter of GCAP
    if ((argc >= 2) && (argv[1][1] != '-'))
    {
       base=basename(argv[1]);
       if (base[0] == 'G') // GCAPS
          for(i=0;i<NCMDS;i++)
             if ((strcmp(base, gcaps[i].cmdstr) == 0) &&
                 (strlen(base) == strlen(gcaps[i].cmdstr)))
                return(gcaps[i].cmdfn(argc-1, &argv[1]));

       if (base[0] == 'D') // DCAPS
          for(i=0;i<NCMDS_DCAP;i++)
             if ((strcmp(base, dcaps[i].cmdstr) == 0) &&
                 (strlen(base) == strlen(dcaps[i].cmdstr)))
                return(dcaps[i].cmdfn(argc-1, &argv[1]));

    } 

   show_version = 0;
   generate_links = 0;
   chipId = NULL;

#if defined(STANDALONE)
   for (i=1; i < argc; i++)
   {
       if (strcmp(argv[i], "-i") == 0)
       {
          i++;
          if (i < argc) {
              chipId = argv[i];
          }
          else {
              printf("Error! Missing interface name for -i option\n");
              return(0);
          }
       }
       else if (strcmp(argv[i], "-v") == 0)
       {
          show_version = 1;
       }
       else if (strcmp(argv[i], "-l") == 0)
       {
          generate_links = 1;
       }
   } 

#else
    opterr = 0;
    while((ret = getopt(argc, argv, "lvi:")) != -1) 
    {
       switch(ret) 
       {
          case 'i':
             chipId = optarg;
             break;
          case 'v':
             show_version = 1;
             break;
          case 'l':
             generate_links = 1;
             break;
          case '?':
             return(-1);
             break;
          default:
             break;
       }
    }
#endif

    // help, version, etc.
    if (show_version)
    {
       ctx = moca_open(chipId);

       if (!ctx)
       {
          printf( "Error!  Unable to connect to moca instance\n");
          return(-2);
       }
       ret = print_version(ctx);
       moca_close(ctx);
       return(ret);
    }
#ifndef STANDALONE
    else if (generate_links)
    {
       for(i=0;i<NCMDS;i++)
       {
          char cmd[64];
          sprintf(cmd, "ln -s GCAP %s", gcaps[i].cmdstr);
          system(cmd);
       }

       for(i=0;i<NCMDS_DCAP;i++)
       {
          char cmd[64];
          sprintf(cmd, "ln -s GCAP %s", dcaps[i].cmdstr);
          system(cmd);
       }
    }
#endif
    else
    {
       if (base[0] == 'G') // GCAPS
       {
          printf(
          "Summary of GCAP Commands\n"\
          "=========================================================================\n");

          for(i=0;i<NCMDS;i++)
             printf("%s\n",gcaps[i].description);
       }

       if (base[0] == 'D') // DCAPS
       {
          printf(
          "Summary of DCAP Commands\n"\
          "=========================================================================\n");

          for(i=0;i<NCMDS_DCAP;i++)
            printf("%s\n",dcaps[i].description);
       }
       printf("=========================================================================\n");
    }
    return(0);
}

