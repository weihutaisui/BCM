#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mocalib.h"
#include "GCAP_Common.h"


GCAP_GEN int moca_mac_to_node_id (void * ctx, macaddr_t * mac, uint32_t * node_id)
{
   int ret;
   struct moca_network_status nws;
   struct moca_gen_node_status gns;
   int i;
   int node_found = 0;
   
   if (ctx == NULL || mac == NULL || node_id == NULL)
   {
      return(-1);
   }

   /* get node bitmask */
   moca_get_network_status(ctx, &nws);

   /* find node id */
   for(i = 0; i < MOCA_MAX_NODES; i++)
   {
      if(! (nws.connected_nodes & (1 << i)))
         continue;
      ret = moca_get_gen_node_status(ctx, i, (void *)&gns);
      if (ret == 0)
      {
         if (!memcmp(&gns.eui.addr[0], mac, sizeof(macaddr_t)))
         {
            node_found = 1;
            break;
         }
      }
   }

   if (node_found)
   {
      *node_id = i;
      return(0);
   }
   else
   {
      return(-2);
   }
}

// The alarm handler is here simply to prevent the default alarm handler from 
// printing "Alarm clock" when it goes off
GCAP_GEN void alarmHandler()
{
    exit(0);
}

GCAP_GEN void moca_gcap_init()
{
#ifndef STANDALONE
    signal(SIGALRM, alarmHandler); 
    signal(SIGINT, alarmHandler);
#endif
}

GCAP_GEN int moca_parse_mac_addr ( char * macAddrString, macaddr_t * pMacAddr )
{
   char * mac;
   char * mac1;
   uint32_t i;
   
   mac = macAddrString;
   
   for ( i = 0; i < sizeof(macaddr_t); i++ ) 
   {
      (*pMacAddr).addr[i] = (uint8_t)( strtol( mac, &mac1, 16) );
      if ((i < (sizeof(macaddr_t) - 1)) && (*mac1 != ':') && (*mac1 != '.'))
         return (-1);
      if (mac == mac1)
         return (-1);

      mac1++;
      mac = mac1;
   }

   return (0);
}

GCAP_GEN void printPowerInfo(uint32_t i, uint32_t tx_power, uint32_t pwrstate)
{

   printf("Node %d:  ", i);
   switch(pwrstate)
   {
      case (POWER_M0):
        printf("state=M0 ");
        break;
      case (POWER_M1):
        printf("state=M1 ");
        break;
      case (POWER_M2):
        printf("state=M2 ");
        break;
      case (POWER_M3):
        printf("state=M3 ");
        break;
      default:
        printf("Unknown ");
        break;
   }
   if (pwrstate == POWER_M1)
   {
      printf("Tx-power-variation=%d dB\n",tx_power);
   }
   printf("\n");
}
