
#include "boardparms.h"
#include "boardparms_voice.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_VOICE_BOARD_ID_NAMES    20

static int check_ext_intr(void)
{
    int i = 0, rc = 0, first = 1, check, ret = 0;
    void* token = NULL;
    unsigned short extintr, gpio;


    for(;;)
    {
        rc = BpGetExtIntrNumGpio(i, &token, &extintr, &gpio);
        if( rc == BP_MAX_ITEM_EXCEEDED )
                break;
        else if( rc == BP_SUCCESS )
        {
            if( gpio == BP_NOT_DEFINED )
            {
#ifdef CONFIG_CHECK_EXT_INT_GPIO
                printf("Error no gpio number defined for external interrupt %d!\n", extintr&BP_EXT_INTR_NUM_MASK);
                ret = -1;
                break;
#endif
            }
            /* interrupt bp id is NONE to clear anything from template, ignore it */
            if( extintr == BP_EXT_INTR_NONE || gpio == BP_GPIO_NONE )
                continue;

            check = BpCheckExtIntr(extintr, gpio, first);
            first = 0;
            if( check == BP_GPIO_ALREADY_USED_AS_INTERRUPT ) 
            {
                printf("gpio %d for external interrupt %d is already used!\n", gpio&BP_GPIO_NUM_MASK, extintr&BP_EXT_INTR_NUM_MASK);
                ret = -1;
                break;
            }

            if( check == BP_INTERRUPT_ALREADY_DEFINED ) 
            {
                printf("external interrupt %d is already used!\n", extintr&BP_EXT_INTR_NUM_MASK);
                ret = -1;
                break;
            }

            if( check == BP_INVALID_GPIO ) 
            {
                printf("invalid gpio 0x%x for external interrupt %d!\n", gpio, extintr&BP_EXT_INTR_NUM_MASK);
                ret = -1;
                break;
            }

            if( check == BP_INVALID_INTERRUPT ) 
            {
                printf("invalid external interrupt %d!\n", extintr&BP_EXT_INTR_NUM_MASK);
                ret = -1;
                break;
            }
        }
        else 
        {
            token = NULL;
            i++;
        }
    }

    return ret;
}

int main (int argc, char **argv)
{

   char boardids[512];
   char boardid[32][BP_BOARD_ID_LEN];
   char voiceBoardids[BP_BOARD_ID_LEN * MAX_VOICE_BOARD_ID_NAMES];   
   char *opname;
   char *sname;
   int ret = 0;
   int i, n, op;
   int nerrs;
   int id, nboards, nVoiceBoards, idVoice;
   int start;
   unsigned short Function[BP_PINMUX_MAX];
   unsigned int Muxinfo[BP_PINMUX_MAX];
   nboards = BpGetBoardIds (boardids, 512);
   
   for (i = 0; i < nboards; i++)
   {
      printf ("found %s\n", &boardids[BP_BOARD_ID_LEN * i]);
      if (strlen(&boardids[BP_BOARD_ID_LEN * i]) > BP_BOARD_ID_LEN) 
      {
         printf ("TOO LONG!\n");
         exit(1);
      }
      strcpy (boardid[i], &boardids[BP_BOARD_ID_LEN * i]);
   }
   
   for (id = 0; id < nboards; id++)
   {
      printf ("\ncheck %s\n", boardid[id]);
      if( BpSetBoardId (boardid[id]) != BP_SUCCESS )
      {
          fprintf(stderr,"%s: Interface check failed\n", argv[0]);
          exit(1);
      }

      /* Get number of voice boards */
      nVoiceBoards = BpGetVoiceBoardIds(voiceBoardids, MAX_VOICE_BOARD_ID_NAMES, boardid[id]);   
      
      /* If no voice boards supported, set index to -1 to iterate through loop once */
      idVoice = (( nVoiceBoards )? 0 : -1) ;
      
      for( ; idVoice<nVoiceBoards; idVoice++ ) 
      {
         if( nVoiceBoards )
         {
            BpSetVoiceBoardId(&voiceBoardids[idVoice * BP_BOARD_ID_LEN]);
            printf ("\n+VoiceBoard %s\n", &voiceBoardids[idVoice * BP_BOARD_ID_LEN]);
         }
         
         start = 1;
         ret = BpGetAllPinmux (BP_PINMUX_MAX, &n, &nerrs, Function, Muxinfo);
         for (i = 0 ; i < n ; i++) {
               if (Function[i] & BP_GPIO_SERIAL) {
                  sname = "serial";
               } else {
                  sname = "gpio  ";
               }
               op = Muxinfo[i] & BP_PINMUX_OP_MASK;
               switch (op)
               {
               case BP_PINMUX_SWLED :
                  opname = "SWLED";
                  break;
               case BP_PINMUX_SWGPIO :
                  opname = "SWGPIO";
                  break;
               case BP_PINMUX_HWLED :
                  opname = "HWLED";
                  break;
               case BP_PINMUX_DIRECT_HWLED :
                  opname = "DIRECT_HWLED";
                  break;
               default :
                  opname = "?";
               }
               printf ("%s GPIO %d Mux %d Pin %d OP %d %s\n",
                       sname,
                       Function[i] & BP_GPIO_NUM_MASK,
                       (Muxinfo[i] & BP_PINMUX_VAL_MASK) >> BP_PINMUX_VAL_SHIFT,
                       Muxinfo[i] & BP_PINMUX_PIN_MASK, op >> BP_PINMUX_OP_SHIFT,
                       opname);
         }
         if (nerrs) {
            fprintf(stderr,"%s: Errors were found\n", argv[0]);
            exit(1);
         }
      }

      /* check external interrupt mapping */
      if( check_ext_intr() != 0 ) {
          fprintf(stderr,"%s: External interrupt check failed\n", argv[0]);
          exit(1);
      }
   }
   return(ret);
}

