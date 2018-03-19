/***************************************************************************
 *
 *     Copyright (c) 2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *  Description: MoCA TPCAP diagnostics -- TPCAP capture test functions
 *
 <:label-BRCM:2013:proprietary:standard

  This program is the proprietary software of Broadcom and/or its
  licensors, and may only be used, duplicated, modified or distributed pursuant
  to the terms and conditions of a separate, written license agreement executed
  between you and Broadcom (an "Authorized License").  Except as set forth in
  an Authorized License, Broadcom grants no license (express or implied), right
  to use, or waiver of any kind with respect to the Software, and Broadcom
  expressly reserves all rights in and to the Software and all intellectual
  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,

  1. This program, including its structure, sequence and organization,
     constitutes the valuable trade secrets of Broadcom, and you shall use
     all reasonable efforts to protect the confidentiality thereof, and to
     use this information only in connection with your use of Broadcom
     integrated circuit products.

  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
     PERFORMANCE OF THE SOFTWARE.

  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
     LIMITED REMEDY.
:>
 ***************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>

#if !defined(STANDALONE)
#include <error.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#endif

#include "moca_os.h"
#include "mocalib.h"
#include "mocaint.h"

#if defined(CHIP_63268) ||  defined(CHIP_63138) ||   defined(CHIP_63148) || defined(CHIP_4908)
#include <board.h>
#endif

#if defined(STANDALONE)
#define sleep(x) usleep(x*1000000)
#define usleep(x) vTaskDelay(x/1000/portTICK_RATE_MS);
#endif


#define IS_28_NM_CHIP    (\
   ((vers.chip_id & 0xFFFF0000) == 0x74450000) || \
   ((vers.chip_id & 0xFFFF0000) == 0x73660000) || \
   ((vers.chip_id & 0xFFFF0000) == 0x74380000) || \
   ((vers.chip_id & 0xFFFF0000) == 0x74390000) || \
   ((vers.chip_id & 0x0FFFFF00) == 0x07437100) || \
   ((vers.chip_id & 0xFFFF0000) == 0x74370000) || \
   ((vers.chip_id & 0xFFFF0000) == 0x43710000) || \
   ((vers.chip_id & 0xFFFF0000) == 0x73640000) || \
   ((vers.chip_id & 0xFFFF0000) == 0x33900000) || \
   ((vers.chip_id & 0xFFFF0000) == 0x71450000))

   
// CAPTURE_MODES    
#define CAPTURE_WAIT_MODE_ONCE_STOP_FW_HOST      0 
#define CAPTURE_WAIT_MODE_MULTI_CAPTURING        1
#define CAPTURE_WAIT_MODE_DEF                    CAPTURE_WAIT_MODE_ONCE_STOP_FW_HOST 

#define CAPTURE_WAIT_MODE_MULTI_CAPTURING_ENDLESS_NUM        10   
 
        
   
   
static void * g_mocaHandle = NULL;
static unsigned int  test = 0;
static unsigned int  spidev = 0;
struct moca_drv_info vers;

unsigned int TPCAPBASE = 0x10401400;
unsigned int MEMC_ARB0_BASE = 0x103b1000; // this varies by chip, this is the 7420 value
unsigned int MEMC_SCBARB_0_CMD_QUEUE_0 = 0;
unsigned int MEMC_ARB_0_RR_QUANTUM_4 = 0;
unsigned int MEMC_ARB_0_RR_QUANTUM_5 = 0;
unsigned int MOCA_EXTRAS_BASE = 0x102a1400;
unsigned int SUN_RGR_BASE = 0x10400800;
unsigned int SUN_RG_BASE = 0x10400800;
unsigned int SUN_TOP_CTRL_BASE = 0x10404000;
unsigned int SUN_L2_BASE = 0x10401800;
unsigned int MOCA_PHY_BASE = 0x10288000;
unsigned int MOCA_TESTPORT_NUMBER = 0x13;

unsigned int  TPCAP_CONTROL;
unsigned int  TPCAP_BLOCK_SELECT;
unsigned int  TPCAP_DEST_ADDR;
unsigned int  TPCAP_SIZE;
unsigned int  TPCAP_SIZE_STATUS;
unsigned int  TPCAP_SW_STOP_TRIGGER;
unsigned int  TPCAP_START_TRIGGER_MASK_HI;
unsigned int  TPCAP_START_TRIGGER_MASK_LO;
unsigned int  TPCAP_START_TRIGGER_VALUE_HI;
unsigned int  TPCAP_START_TRIGGER_VALUE_LO;
unsigned int  TPCAP_STOP_TRIGGER_MASK_HI;
unsigned int  TPCAP_STOP_TRIGGER_MASK_LO;
unsigned int  TPCAP_STOP_TRIGGER_VALUE_HI;
unsigned int  TPCAP_STOP_TRIGGER_VALUE_LO;
unsigned int  PLL_MOCA_PLL_CHANNEL_CTRL_CH1;


unsigned int  SUN_RGR_SW_RESET_1;
unsigned int  SUN_TOP_CTRL_TEST_PORT_CTRL;
unsigned int  SUN_L2_CPU_CLEAR;

unsigned int  TPCAP_SEL_ADDR;

unsigned int  MOCA_PHY_BASE;
unsigned int  MOCA_TRX_BASE;
unsigned int  MOCA_PHY_BURST_TX_RX_REG_2;
unsigned int  MOCA_PHY_FIXD_CMN_TEST_PORTS = 0;
unsigned int  MOCA_PHY_FIXD_CLK_SEL = 0;



unsigned int  MOCA_PHY_BURST_CMN_INIT_GEN = 0;
unsigned int  CLKGEN_SYS_CTRL_INST_POWER_SWITCH_MEMORY = 0;


unsigned int  MEMC_ARB_0_CLIENT_INFO_65;
unsigned int  TPCAP_SEL_ENCODED_TPCAP_EN=0;
unsigned int  BRST_CMN_TRXDIG_WRITE_DATA_R07=0;




unsigned int startoffset = 0;
unsigned int read_offset = 0;

#if !defined(CHIP_63268) && !defined(STANDALONE) && !defined(CHIP_63138) && !defined(CHIP_63148) && !defined(CHIP_4908)
static unsigned int tpcap_mapphys(unsigned int addr)
{
   static unsigned int base = 0xffffffff;
   static int fd = -1;
   static int pagesize = 0, pagemask;
   static void *mapping;

   if(fd == -1)
   {
      fd = open("/dev/mem", O_RDWR | O_SYNC);

      if(fd < 0)
      {
         printf("can't open /dev/mem: %s\n", strerror(errno));
         exit(1);
      }

      pagesize = getpagesize();
      pagemask = ~(pagesize - 1);
   }

   if((addr & pagemask) == (base & pagemask))
      return((unsigned int)mapping + (addr & ~pagemask));

   if(mapping)
      munmap(mapping, pagesize);

   base = addr & pagemask;

   mapping = mmap(NULL, pagesize, (PROT_READ | PROT_WRITE),
                  MAP_SHARED, fd, base);

   if(mapping == (void *) - 1)
   {
      printf("mmap failed: %s\n", strerror(errno));
      exit(1);
   }

   return((unsigned int)mapping + (addr & ~pagemask));
}

static unsigned int regr(unsigned int addr)
{
   unsigned int *x = (unsigned int *)tpcap_mapphys(addr);
//    printf("READ: %X\n", addr);
   return(*x);
}

void regw(unsigned int addr, unsigned int val)
{
   unsigned int *x = (unsigned int *)tpcap_mapphys(addr);
//    printf("WRITE: %X\n", addr);

   *x = val;
}

#elif !defined(STANDALONE)

int memfile = 0;

static unsigned int regr(unsigned int addr)
{
   BOARD_IOCTL_PARMS ioctlParms;
   unsigned int buf;

   memset(&ioctlParms, 0, sizeof(ioctlParms));

   if (memfile == 0)
   {
      memfile = open("/dev/brcmboard", O_RDWR);

      if (memfile == -1)
      {
         printf("Could not open /dev/brcmboard\n");
         return -1;
      }
   }

   ioctlParms.offset = addr;
   ioctlParms.strLen = 4;
   ioctlParms.result = spidev;
   ioctlParms.buf = (char *)&buf;

   if (ioctl(memfile, BOARD_IOCTL_SPI_SLAVE_READ, &ioctlParms) < 0)
   {
      printf("spi slave device read failed (%08X).\n", addr);
      return -1;
   }

//   printf("READ : %08X=%08X\n", addr, buf);

   return (buf);
}

void regw(unsigned int addr, unsigned int val)
{
   BOARD_IOCTL_PARMS ioctlParms;

   memset(&ioctlParms, 0, sizeof(ioctlParms));

//   printf("WRITE: %08X=%08X\n", addr, val);
   if (memfile == 0)
   {
      memfile = open("/dev/brcmboard", O_RDWR);

      if (memfile == -1)
      {
         printf("Could not open /dev/brcmboard\n");
         return;
      }
   }

   ioctlParms.offset = addr;
   ioctlParms.result = val;
   ioctlParms.strLen = 4;
   ioctlParms.buf = spidev;

   if (ioctl(memfile, BOARD_IOCTL_SPI_SLAVE_WRITE, &ioctlParms) < 0)
   {
      printf("spi slave device write failed (%X).\n", addr);
      return;
   }
}

#else // STANDALONE

void regw(unsigned int addr, unsigned int val)
{
   *(unsigned int *)addr = val;
}

static unsigned int regr(unsigned int addr)
{
   return (*(unsigned int *)addr);
}

#endif

unsigned int TPCAP_ReadMem(unsigned int paddr)
{
   return(regr(paddr));
}

MOCALIB_CLI void TPCAP_Start(unsigned int phy_probe, unsigned int tpcap_tp_sel, unsigned int num_samples,
                 unsigned int dest_addr, unsigned int testPortDataSel, unsigned int testPortControlSel, unsigned int testPortAdcRate, unsigned int testPortClockSel,
                 unsigned int lpbk, unsigned int wait, unsigned int phy_num)
{
   regw(SUN_RGR_SW_RESET_1, 1); //  reset tpcap
   sleep(3);                   //  should add time
   regw(SUN_RGR_SW_RESET_1, 0); // take TPCAP out of reset

   regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) & 0xFFFFFFFE); // SUNDRY.TPCAP.CONTROL.CAPT_ENABLE = 0   ' clear condition

   if (TPCAP_DEST_ADDR)
   {
      regw(TPCAP_DEST_ADDR, dest_addr);
   }

   regw(TPCAP_SIZE, num_samples * 4);

   regw(TPCAP_START_TRIGGER_MASK_HI, 0xFFFFFFFF);
   regw(TPCAP_START_TRIGGER_MASK_LO, 0xFFFFFFFF);
   regw(TPCAP_START_TRIGGER_VALUE_HI, 0);
   regw(TPCAP_START_TRIGGER_VALUE_LO, 0);


   regw(TPCAP_STOP_TRIGGER_MASK_HI, 0xFFFFFFFF);
   regw(TPCAP_STOP_TRIGGER_MASK_LO, 0xFFFFFFFF);
   regw(TPCAP_STOP_TRIGGER_VALUE_HI, 0);
   regw(TPCAP_STOP_TRIGGER_VALUE_LO, 0);

   regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) & 0xFFFF8FFF); // SUNDRY.TPCAP.CONTROL.DEMUX_MODE = 0 ' no demuxing at source
//#if __BYTE_ORDER == __LITTLE_ENDIAN
   //  regw(TPCAP_CONTROL, (regr(TPCAP_CONTROL) & 0xFFFFFF7F) | 0x80); //SUNDRY.TPCAP.CONTROL.LITTLE_ENDIAN = 1 ' 1= Little Endian
//#else
   regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) & 0xFFFFFF7F); //SUNDRY.TPCAP.CONTROL.LITTLE_ENDIAN = 0 ' 0= Big Endian
//#endif
   regw(TPCAP_CONTROL, (regr(TPCAP_CONTROL) & 0xFFFFFCFF) | 0x300); // SUNDRY.TPCAP.CONTROL.CAPT_WIDTH = 3 ' 0= 8 bits, 1= 16 bits, 2= 32 bits, 3= 64 bits
   regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) & 0xFFFFFFBF); // SUNDRY.TPCAP.CONTROL.ENA_PTRN_STOP_TRIGGER = 0
   regw(TPCAP_CONTROL, (regr(TPCAP_CONTROL) & 0xFFFFFFDF) | 0x20); // SUNDRY.TPCAP.CONTROL.ENA_CTRL_STOP_TRIGGER = 1
   regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) & 0xFFFFFFEF); // SUNDRY.TPCAP.CONTROL.ENA_PTRN_START_TRIGGER = 0
   regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) & 0xFFFFFFF7); //SUNDRY.TPCAP.CONTROL.ENA_CTRL_START_TRIGGER = 0
   regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) & 0xFFFFFFFB); // SUNDRY.TPCAP.CONTROL.ENA_PTRN_CLK_ENA = 0
   regw(TPCAP_CONTROL, (regr(TPCAP_CONTROL) & 0xFFFFFFFD) | 0x2); // SUNDRY.TPCAP.CONTROL.ENA_CTRL_CLK_ENA = 1 ' The clock enable TP_AUX[0] is valid only if the ENA_CTRL_CLK_ENA bit is set.

   if (MOCA_PHY_BURST_TX_RX_REG_2)
      regw(MOCA_PHY_BURST_TX_RX_REG_2, regr(TPCAP_CONTROL) | (1 << 17)); // MOCA_PHY_BURST_TX_RX_REG_2.tpcap_en
   else
      regw(MOCA_PHY_BURST_CMN_INIT_GEN, regr(MOCA_PHY_BURST_CMN_INIT_GEN) | (1 << 17)); //  xxxx.tpcap_en = 1

   regw(TPCAP_SEL_ADDR, 0 );

   regw(TPCAP_SEL_ADDR, (regr(TPCAP_SEL_ADDR) | 0x8) ); // tpcap_overlay_sel  = 1

   // Mux out the MOCA probes
   if (tpcap_tp_sel == 0) // phy probes fast (0) test ports slow (1)
   {
      unsigned int clockSel = 0;
      regw(TPCAP_SEL_ADDR, (regr(TPCAP_SEL_ADDR) & 0xFFFFFFE8) | phy_probe); // MOCA_TOP.MOCA.MOCA_EXTRAS.MOCA_EXTRAS.TPCAP_SEL.sel = phy_probe

      regw(TPCAP_SEL_ADDR, (regr(TPCAP_SEL_ADDR) & 0xFFFFFFEF) ); // tpcap_tp_sel = 0

      /*
      MSB TP clock select, this clock is muxed on bit 31 of tp_out,
      The clocks are:
      Clock sel:
      0 - data_dac_clk200
      1 -  data_adc_clk200
      2 - mii_tx_clk
      3 - network_clk
      4 - sys_clk
      5 - phy_clk
      6 - rssi_clk
      7 - phypll_clk1_div2
      Reset value: 0x0
      */
      switch (phy_probe)
      {
      case 0: //0 - ADC out (ADC clock of 200 Mhz)
         clockSel = 1;
         break;

      case 1: //1 - Slicer in (PHY_CLK)
      case 3: //3 - Autocorrelation out (PHY_CLK)
      case 4: //4 - Phase Rotator out (PHY_CLK) - WIN
      case 7: //7 - FFT out (PHY_CLK)
         
		if (((vers.chip_id & 0xFFFF00F0) == 0x339000B0)  && (phy_num ==1))	
		{	
			clockSel = 2;
		}		 
		else
		{
			clockSel = 5;
		}	
         
		 if ((vers.chip_id & 0xFFFF00F0) == 0x339000B0)
		 {
		    unsigned int  phy_clock = ((regr(PLL_MOCA_PLL_CHANNEL_CTRL_CH1) >> 1) & 0xFF); 
		     printf("The PHY clock should be reduce to 330MHz. current val=%x, desired value=0xB. You should run moca with mocap set --phy_clock 330\n",phy_clock);
			 printf("After you finish to capture, reboot the card  in order to return to regular phy clock\n");
		 }
		 break;

      case 2: //2 - MPC  (sys_clock)
      case 5: //5 - MPD (sys_clock)
         clockSel = 4;
         break;

      case 6: //6 - phy_start + MPI (line_clk_derived)

         if(IS_28_NM_CHIP)
         {
            clockSel = 7; //(data_dac_clk200)
         }
         else
         {
            clockSel = 0; //(data_dac_clk200)
         }

         break;
      };
      printf("clock sel(%X).\n", clockSel);
	  if(MOCA_PHY_FIXD_CLK_SEL)
	  {
		  regw(MOCA_PHY_FIXD_CLK_SEL,  (clockSel << 15)); // set to mac or phy TP, 00 = mac, 01 phy, 11 moca signature
	  }
	  else
	  {
		regw(MOCA_PHY_FIXD_CMN_TEST_PORTS,  (clockSel << 15)); // set to mac or phy TP, 00 = mac, 01 phy, 11 moca signature
	  }  		

   }
   else // test ports - slow
   {
      regw(TPCAP_SEL_ADDR, (regr(TPCAP_SEL_ADDR) | 0x30) ); // tpcap_tp_sel = 1 , tpcap_force_clken = 1

      //TBD  tp_ac_cc_sel ,  tp_ma_pc_sel
      regw(MOCA_PHY_FIXD_CMN_TEST_PORTS,   (testPortClockSel << 15) | (testPortAdcRate << 9) | (testPortControlSel << 5) | (testPortDataSel << 0) );
	  if(MOCA_PHY_FIXD_CLK_SEL)
	  {
		regw(MOCA_PHY_FIXD_CLK_SEL,  (testPortClockSel << 15));
	  }	
   }

   
   
   regw(SUN_TOP_CTRL_TEST_PORT_CTRL, MOCA_TESTPORT_NUMBER); //SUNDRY.SUN_TOP_CTRL.TEST_PORT_CTRL = &h13&   ' select MOCA on SUN_TOP testport

   if(BRST_CMN_TRXDIG_WRITE_DATA_R07)
   {
	   if (phy_num ==1 ){ 
		regw(BRST_CMN_TRXDIG_WRITE_DATA_R07, 0xFFFFFFFF); // In 3390B0 the setting of PHY number is set via TRX 
	   }
	   else {
		regw(BRST_CMN_TRXDIG_WRITE_DATA_R07, 0); // In 3390B0 the setting of PHY number is set via TRX    
	   }
   } 	   
   
   regw(TPCAP_BLOCK_SELECT, TPCAP_SEL_ENCODED_TPCAP_EN); // SUNDRY.TPCAP.BLOCK_SELECT.encoded_tpcap_en = 0	' Select MoCA as TPCAP input. In few types it set the PHY number. 

   
   
   
   // Set up memory client for TPCAP (client #65)
   if (MEMC_ARB_0_CLIENT_INFO_65)
   {
      regw(MEMC_ARB_0_CLIENT_INFO_65, (regr(MEMC_ARB_0_CLIENT_INFO_65) & 0xFFFFFF7F) | 0x80); //MEMC_ARB_0.CLIENT_INFO_65.RR_EN = 1   			' basic way to enable this client
      regw(MEMC_ARB_0_CLIENT_INFO_65, (regr(MEMC_ARB_0_CLIENT_INFO_65) & 0xFFE000FF) | 0x1FFF00); //MEMC_ARB_0.CLIENT_INFO_65.BO_VAL = &h1FFF&  	'&H1fff  '!!! reduce dflt 0x1fff to increase BW
   }
   else if (MEMC_SCBARB_0_CMD_QUEUE_0)
   {
      // see email from Rayond Woo regarding the these settings:

      regw(MEMC_SCBARB_0_CMD_QUEUE_0, (regr(MEMC_SCBARB_0_CMD_QUEUE_0) | 0x8000)); // client 15 is tpcap
      regw(MEMC_ARB_0_RR_QUANTUM_4, (regr(MEMC_ARB_0_RR_QUANTUM_4) & 0xFC00FFFF ) | (0xf << 16));
      regw(MEMC_ARB_0_RR_QUANTUM_5, (regr(MEMC_ARB_0_RR_QUANTUM_5) & 0xFFFFFC00 ) | 0xf);
   }

   // clear interrupt
   regw(SUN_L2_CPU_CLEAR, (regr(SUN_L2_CPU_CLEAR) & ~(1 << 22)) | (1 << 22)); //SUNDRY.SUN_L2.CPU_CLEAR.TPCAP_FIFO_OVERFLOW = 1
   regw(SUN_L2_CPU_CLEAR, regr(SUN_L2_CPU_CLEAR) & ~(1 << 22)); //SUNDRY.SUN_L2.CPU_CLEAR.TPCAP_FIFO_OVERFLOW = 0

   regw(TPCAP_CONTROL, (regr(TPCAP_CONTROL) & 0xFFFEFFFF) | (lpbk << 16)); // SUNDRY.TPCAP.CONTROL.LOOP_BACK_ENABLE

   // start capture, now that configuration is complete.
   regw(TPCAP_CONTROL, (regr(TPCAP_CONTROL) & 0xFFFFFFFE) | 1); // SUNDRY.TPCAP.CONTROL.CAPT_ENABLE = 1

   printf ("TPCAP configured for MoCA\n");
   /*
       if (wait)
       {
           printf ("Waiting for TPCAP to finish capturing ...\n");

           // Poll on TPCAP to finish capturing
           while (regr(TPCAP_CONTROL) & 1)
           {
               usleep(100000);
           }

           printf ("Capture complete\n");
       } */
}

MOCALIB_CLI void TPCAP_Init(unsigned int chipid, unsigned int phy_num)
{

   if ((chipid & 0xFFFF0000) == 0x73400000)
   {
      //1.1 moca
      MEMC_ARB0_BASE = 0x10501000;
      TPCAP_SEL_ADDR = (MOCA_EXTRAS_BASE + 0x5c);
      MOCA_PHY_BURST_TX_RX_REG_2 = (MOCA_PHY_BASE + 0xcc);

   }
   else if ((chipid & 0xFFFF0000) == 0x73440000)
   {
      //1.1 moca
      TPCAPBASE = 0x10400c00;
      SUN_RGR_BASE = 0x10400400;
      MEMC_ARB0_BASE = 0x10a01000;
      SUN_TOP_CTRL_TEST_PORT_CTRL = 0x10404380;
      SUN_L2_BASE = 0x10403000;
      TPCAP_SEL_ADDR = (MOCA_EXTRAS_BASE + 0x5c);
      MOCA_PHY_BURST_TX_RX_REG_2 = (MOCA_PHY_BASE + 0xcc);

   }
   else if (((chipid & 0xFFFF0000) == 0x73460000) ||
            ((chipid & 0xFFFF0000) == 0x74220000) ||
            ((chipid & 0xFFFF00FF) == 0x742500A0))
   {
      //1.1 moca
      TPCAPBASE = 0x10400c00;
      SUN_RGR_BASE = 0x10400400;
      MEMC_ARB0_BASE = 0x103b1000;
      SUN_TOP_CTRL_TEST_PORT_CTRL = 0x10404380;
      SUN_L2_BASE = 0x10403000;
      MOCA_TESTPORT_NUMBER = 0x2f;
      TPCAP_SEL_ADDR = (MOCA_EXTRAS_BASE + 0x5c);
      MOCA_PHY_BURST_TX_RX_REG_2 = (MOCA_PHY_BASE + 0xcc);

   }
   else if ((chipid & 0xFFFF00F0) == 0x742500b0)
   {
      TPCAPBASE = 0x10400c00;
      MEMC_ARB0_BASE = 0x103b1000;
      MOCA_EXTRAS_BASE = 0x10ffec00;
      SUN_RGR_BASE = 0x10400400;
      SUN_TOP_CTRL_BASE = 0x10404000;
      SUN_L2_BASE = 0x10403000;
      MOCA_PHY_BASE = 0x10fe0000;
      MOCA_TESTPORT_NUMBER = 0x2f;

      SUN_TOP_CTRL_TEST_PORT_CTRL = 0x10404380;
      TPCAP_SEL_ADDR = (MOCA_EXTRAS_BASE + 0x2c);
      MOCA_PHY_BURST_CMN_INIT_GEN = (MOCA_PHY_BASE + 0x20a4);

      MOCA_PHY_BURST_TX_RX_REG_2 = 0;
   }
   else if ((chipid & 0xFFFF0000) == 0x74350000)
   {
      TPCAPBASE = 0x10400c00;
      MEMC_ARB0_BASE = 0x103b1000;
      MOCA_EXTRAS_BASE = 0x10ffec00;
      SUN_RG_BASE = 0x10400400;   // SUN_GR
      SUN_RGR_BASE = 0;
      SUN_TOP_CTRL_BASE = 0x10404000;
      SUN_L2_BASE = 0x10403000;
      MOCA_PHY_BASE = 0x10fe0000;
      MOCA_TESTPORT_NUMBER = 0x2f;

      SUN_TOP_CTRL_TEST_PORT_CTRL = 0x10404380;

      if((chipid & 0xFFFF00F0) == 0x743500A0)
      {
         TPCAP_SEL_ADDR = (MOCA_EXTRAS_BASE + 0x2c);
      }
      else
      {
         TPCAP_SEL_ADDR = (MOCA_PHY_BASE + 0x14);
      }

      MOCA_PHY_BURST_CMN_INIT_GEN = (MOCA_PHY_BASE + 0x20a4);

      MOCA_PHY_BURST_TX_RX_REG_2 = 0;
   }
   else if (((chipid & 0xFFFF0000) == 0x74290000) ||
            ((chipid & 0xFFFFFF00) == 0x7429500))
   {
      TPCAPBASE = 0x10400c00;
      MEMC_ARB0_BASE = 0x103b1000;
      MOCA_EXTRAS_BASE = 0x10bfec00;
      SUN_RG_BASE = 0;
      SUN_RGR_BASE = 0x10400400;
      SUN_TOP_CTRL_BASE = 0x10404000;
      SUN_L2_BASE = 0x10403000;
      MOCA_PHY_BASE = 0x10be0000;
      MOCA_TESTPORT_NUMBER = 0x2f;

      SUN_TOP_CTRL_TEST_PORT_CTRL = 0x10404380;
      TPCAP_SEL_ADDR = (MOCA_EXTRAS_BASE + 0x2c);
      MOCA_PHY_BURST_CMN_INIT_GEN = (MOCA_PHY_BASE + 0x20a4);

      MOCA_PHY_BURST_TX_RX_REG_2 = 0;
   }
   else if (((chipid & 0xFFFF0000) == 0x74450000) ||
            ((chipid & 0xFFFF0000) == 0x74390000) ||
            ((chipid & 0xFFFF0000) == 0x74380000) ||
            ((chipid & 0xFFFF0000) == 0x73660000) ||
            ((chipid & 0xFFFF0000) == 0x33900000) ||
            ((chipid & 0xFFFF0000) == 0x73640000) ||
            ((vers.chip_id & 0x0FFFFF00) == 0x07437100) ||
            ((vers.chip_id & 0xFFFF0000) == 0x43710000) )
   {
      TPCAPBASE = 0xF0401800;  

      if (((chipid & 0xFFFF00F0) == 0x744500A0)  || ((chipid & 0xFFFF00F0) == 0x744500B0))
      {
         MEMC_ARB0_BASE = 0xF03b1000;
      }
      else if (((chipid & 0xFFFF00F0) == 0x744500C0)  || ((chipid & 0xFFFF00F0) == 0x744500D0) ||  ((chipid & 0xFFFF00F0) == 0x744500E0) || 
          ((chipid & 0xFFFF00F0) == 0x743900B0)  )
      {
         MEMC_ARB0_BASE = 0xF1101000;
      }
      else if (((chipid & 0xFFFF00F0) == 0x736600B0)  || ((chipid & 0xFFFF00F0) == 0x736600C0))
      {
         MEMC_ARB0_BASE = 0xF0501000;
      }
      else if ( ((chipid & 0xFFFF00F0) == 0x743900A0) ||
                ((chipid & 0xFFFF0000) == 0x74380000) ||
                ((chipid & 0xFFFF00F0) == 0x736600A0) ||
                ((chipid & 0xFFFF0000) == 0x73640000) ||
                ((vers.chip_id & 0x0FFFFF00) == 0x07437100) ||
                ((vers.chip_id & 0xFFFF0000) == 0x43710000))
      {
         MEMC_ARB0_BASE = 0xF0901000;
      }
      else
      {
         MEMC_ARB0_BASE = 0; // should be support   
         if ((chipid & 0xFFFF0000) == 0x33900000)
         {
            MEMC_SCBARB_0_CMD_QUEUE_0 = 0xf15014e0;
            MEMC_ARB_0_RR_QUANTUM_4 = 0xf1500360;
            MEMC_ARB_0_RR_QUANTUM_5 = 0xf1500364;
         }
      }

      if ((chipid & 0xFFFF00F0) == 0x339000B0)
      {		  
         MOCA_EXTRAS_BASE = 0xf13ff000;  
		 PLL_MOCA_PLL_CHANNEL_CTRL_CH1 = 0xf0480170;
	  }	 
      else if ((chipid & 0xFFFF00F0) == 0x339000A0)
	  {
         MOCA_EXTRAS_BASE = 0xf13fec00; 
	  }	 
      else
	  {
         MOCA_EXTRAS_BASE = 0xf0ffec00;
	  } 	 
	  
      SUN_RGR_BASE = 0;            
      SUN_RG_BASE = 0xf0401000;    // It should be SUN_GR_BASE  not SUN_RG_BASE   
      SUN_TOP_CTRL_BASE = 0xf0404000;
      SUN_L2_BASE = 0xf0403000;
      MOCA_PHY_BASE = 0xf0fe0000;

      if (((chipid & 0xFFFF00F0) == 0x339000A0))
      {
         MOCA_PHY_BASE = 0xf13e0000;
      }
      else if (((chipid & 0xFFFF00F0) == 0x339000B0) && (phy_num == 1))   
      {
         MOCA_PHY_BASE = 0xf13d8000;
		 MOCA_PHY_FIXD_CLK_SEL =  0xf13e000c;   
      }
      else if (((chipid & 0xFFFF00F0) == 0x339000B0) && (phy_num == 0))   
      {
         MOCA_PHY_BASE = 0xf13e0000;      
      }
	  
      if (((chipid & 0xFFFF00F0) == 0x736600B0) || 
          ((chipid & 0xFFFF00F0) == 0x736600C0) ||
          ((chipid & 0xFFFF00F0) == 0x743900B0) ||
          ((chipid & 0xFFFF0000) == 0x73640000))
      {
         MOCA_TESTPORT_NUMBER = 40;
      }
      else
      {
         MOCA_TESTPORT_NUMBER = 0x2f;    
      }

      if ((chipid & 0xFFFF00F0) == 0x339000B0)
      {   
         MOCA_TRX_BASE = 0xf13fe000;      
         BRST_CMN_TRXDIG_WRITE_DATA_R07 = (MOCA_TRX_BASE + 0xC0); 
      }

      SUN_TOP_CTRL_TEST_PORT_CTRL = 0xf0404380;       
      TPCAP_SEL_ADDR = (MOCA_PHY_BASE + 0x14);        

      if ((chipid & 0xFFFF00F0) == 0x339000B0)
         MOCA_PHY_BURST_CMN_INIT_GEN = (MOCA_PHY_BASE + 0x2098);  
      else
         MOCA_PHY_BURST_CMN_INIT_GEN = (MOCA_PHY_BASE + 0x209c);  

      MOCA_PHY_BURST_TX_RX_REG_2 = 0;             
   }
   else if ((chipid & 0xFFFF0000) == 0x71450000)
   {
      TPCAPBASE = 0xF0401800;
      MEMC_ARB0_BASE = 0xF03b1000;
      MOCA_EXTRAS_BASE = 0xd7dfec00;
      SUN_RGR_BASE = 0;
      SUN_RG_BASE = 0xf0401000;
      SUN_TOP_CTRL_BASE = 0xf0404000;
      SUN_L2_BASE = 0xf0403000;
      MOCA_PHY_BASE = 0xd7de0000;
      MOCA_TESTPORT_NUMBER = 0x2f;

      SUN_TOP_CTRL_TEST_PORT_CTRL = 0xf0404380;
      TPCAP_SEL_ADDR = (MOCA_PHY_BASE + 0x14);
      MOCA_PHY_BURST_CMN_INIT_GEN = (MOCA_PHY_BASE + 0x209c);

      MOCA_PHY_BURST_TX_RX_REG_2 = 0;
   }
   else if ((chipid & 0xFFF00000) == 0x68000000)
   {
      if ((chipid & 0xFFF000F0) == 0x680000C0)
      {
         TPCAPBASE = 0x10440000;
      }
      else
      {
         TPCAPBASE = 0x10408000;
      }

      MEMC_ARB0_BASE = 0;
      MOCA_EXTRAS_BASE = 0x107fec00;

      SUN_RG_BASE = 0x10400400;
      SUN_RGR_BASE = 0;
      SUN_TOP_CTRL_BASE = 0x10404000;
      SUN_L2_BASE = 0x10403000;

      if (phy_num == 1)
      {
         MOCA_PHY_BASE = 0x107d8000;
         MOCA_TESTPORT_NUMBER = 21;
         TPCAP_SEL_ENCODED_TPCAP_EN = 1;
      }
      else
      {
         MOCA_PHY_BASE = 0x107e0000;
         MOCA_TESTPORT_NUMBER = 20;
      }


      TPCAP_SEL_ADDR = (MOCA_PHY_BASE + 0x14);

      SUN_TOP_CTRL_TEST_PORT_CTRL = 0x10404380;
      MOCA_PHY_BURST_CMN_INIT_GEN = (MOCA_PHY_BASE + 0x20a4);

      MOCA_PHY_BURST_TX_RX_REG_2 = 0;
      CLKGEN_SYS_CTRL_INST_POWER_SWITCH_MEMORY = 0x10100164;
   }
   else
   {
      SUN_TOP_CTRL_TEST_PORT_CTRL = (SUN_TOP_CTRL_BASE + 0x200);
      TPCAP_SEL_ADDR = (MOCA_EXTRAS_BASE + 0x5c);
      MOCA_PHY_BURST_TX_RX_REG_2 = (MOCA_PHY_BASE + 0xcc);
   }

   MOCA_PHY_FIXD_CMN_TEST_PORTS = MOCA_PHY_BASE + 0xc;

   if ((chipid & 0xFFF00000) == 0x68000000)
   {
      TPCAP_CONTROL = (TPCAPBASE + 0x00);
      TPCAP_BLOCK_SELECT = (TPCAPBASE + 0x04);
      TPCAP_DEST_ADDR = 0;
      TPCAP_SIZE = (TPCAPBASE + 0x08);
      TPCAP_SIZE_STATUS = (TPCAPBASE + 0x0c);
      TPCAP_START_TRIGGER_MASK_HI = (TPCAPBASE + 0x50);
      TPCAP_START_TRIGGER_MASK_LO = (TPCAPBASE + 0x54);
      TPCAP_START_TRIGGER_VALUE_HI = (TPCAPBASE + 0x58);
      TPCAP_START_TRIGGER_VALUE_LO = (TPCAPBASE + 0x5c);
      TPCAP_STOP_TRIGGER_MASK_HI = (TPCAPBASE + 0x60);
      TPCAP_STOP_TRIGGER_MASK_LO = (TPCAPBASE + 0x64);
      TPCAP_STOP_TRIGGER_VALUE_HI = (TPCAPBASE + 0x68);
      TPCAP_STOP_TRIGGER_VALUE_LO = (TPCAPBASE + 0x6c);
      TPCAP_SW_STOP_TRIGGER = (TPCAPBASE + 0x80);
      MEMC_ARB_0_CLIENT_INFO_65 = 0;
   }
   else
   {
      TPCAP_CONTROL = (TPCAPBASE + 0x00);
      TPCAP_BLOCK_SELECT = (TPCAPBASE + 0x04);
      TPCAP_DEST_ADDR = (TPCAPBASE + 0x08);
      TPCAP_SIZE = (TPCAPBASE + 0x0c);
      TPCAP_SIZE_STATUS = (TPCAPBASE + 0x10);
      TPCAP_START_TRIGGER_MASK_HI = (TPCAPBASE + 0x54);
      TPCAP_START_TRIGGER_MASK_LO = (TPCAPBASE + 0x58);
      TPCAP_START_TRIGGER_VALUE_HI = (TPCAPBASE + 0x5c);
      TPCAP_START_TRIGGER_VALUE_LO = (TPCAPBASE + 0x60);
      TPCAP_STOP_TRIGGER_MASK_HI = (TPCAPBASE + 0x64);
      TPCAP_STOP_TRIGGER_MASK_LO = (TPCAPBASE + 0x68);
      TPCAP_STOP_TRIGGER_VALUE_HI = (TPCAPBASE + 0x6c);
      TPCAP_STOP_TRIGGER_VALUE_LO = (TPCAPBASE + 0x70);
      TPCAP_SW_STOP_TRIGGER = (TPCAPBASE + 0x84);
      if(MEMC_ARB0_BASE !=0)
         MEMC_ARB_0_CLIENT_INFO_65 = (MEMC_ARB0_BASE + 0x0108);
      else
         MEMC_ARB_0_CLIENT_INFO_65 = 0;
   }

   if (SUN_RGR_BASE)
      SUN_RGR_SW_RESET_1 = (SUN_RGR_BASE + 0x10);
   else
      SUN_RGR_SW_RESET_1 = (SUN_RG_BASE + 0xc);

   SUN_L2_CPU_CLEAR = (SUN_L2_BASE + 0x08);

}

MOCALIB_CLI int TpcapDumpHandler( const char* fname, unsigned int display , unsigned int testPortClockSel)
{
   unsigned int paddr = 0;
   unsigned int i;
   unsigned int buffersize = 0;

#if !defined(STANDALONE)
   FILE *fp;

   fp = fopen(fname, "w");

   if (!fp)
   {
      fprintf(stderr, "Unable to open file %s for writing\n", fname);
      return(-1);
   }

#endif

   moca_get_miscval(g_mocaHandle, &paddr);
   moca_get_miscval2(g_mocaHandle, &buffersize);
   moca_set_wdog_enable(g_mocaHandle, 0);

   if (paddr == 0)
   {
      fprintf(stderr, "No tpcap buffer found\n");
      return(0);
   }

   //wait for STOP_TRIGGER to go to zero
   printf("Waiting for stopTrigger to finish (%x) , cur=0x%08x\n", vers.chip_id, regr(TPCAP_SW_STOP_TRIGGER));

   while( 1 )
   {
      unsigned int val;
      val = regr(TPCAP_SW_STOP_TRIGGER);

      if (  (val & 0x1) == 0 )
         break;

      sleep(1);
   }

   //wait for CNTRL to go to zero
   printf("Waiting for TPCAP_CONTROL to finish , cur=0x%08x\n", regr(TPCAP_CONTROL));

   while( 1 )
   {
      unsigned int val;
      val = regr(TPCAP_CONTROL);

      if (  (val & 0x1) == 0 )
         break;

      sleep(1);
   }

    // Wait until the FW finish to print all the relevant profiles.  
	sleep(1);
   
   if(read_offset==0)
   {
      startoffset = regr(TPCAP_SIZE_STATUS);
   }

   printf("Reading from buffer at address 0x%08X...\n", paddr);
   printf("Buffer offset/size : 0x%08X / 0x%08X\n", startoffset, buffersize);

   if (startoffset & 3) // something's wrong here, not word aligned.  This will never happen...
      startoffset = 0;

   if (startoffset == 0)
      printf("WARNING: startoffset = 0, dumping buffer from beginning\n");

   startoffset = startoffset % buffersize;

   //printf("Writing file...\n");

   if ((vers.chip_id &0xFFF000F0) == 0x680000B0)
   {
      startoffset = startoffset/2;
   }


   if (paddr)
   {
      if ((vers.chip_id & 0xFFF00000) == 0x68000000)
      {
         regw(TPCAP_CONTROL, regr(TPCAP_CONTROL) | (1 << 16)); // READ_MODE
      }


      if (display == 2)
      {
         // print header
         fprintf(fp, "$comment\nTOOL:   simvision       11.10-s055\n$end\n\n\n$date\n    Apr 21, 2013 11:20:28\n$end\n\n$timescale\n    1ps\n$end\n\n\n$scope module top $end\n$var integer 32 ! tp_out [31:0] $end\n$upscope $end\n\n$enddefinitions $end\n#1000000000\n$dumpvars\nb000100000001 !\n$end \n");
      }

      int numWords = buffersize / 4;

      for (i = 0; i < numWords; i++)
      {
         unsigned int val;

         if ((vers.chip_id & 0xFFF000F0) == 0x680000B0)
         {
            /* In the 6802, we need to read from two buffer areas, alternately */
            if ((i & 0x1) == 0)
               val = TPCAP_ReadMem(paddr+(((i/2)*4 + startoffset) % (buffersize /2)) );
            else
               val = TPCAP_ReadMem((paddr+ 0x2000) + (((i/2)*4 + startoffset) % (buffersize/2)));
         }
         else if ((vers.chip_id & 0xFFF000F0) == 0x680000C0)
         {
            // The C0 has a really crazy buffer layout for tpcap:
            // even samples from 0-16k   -> located at DMEM0
            //  odd samples from 0-16k   -> located at DMEM0
            // even samples from 16k-24k -> located at DMEM1
            //  odd samples from 16k-24k -> located at DMEM1
            // even samples from 24k-32k -> located at DMEM2
            //  odd samples from 24k-32k -> located at DMEM2
            // (using sample number in comment above.  Offset is 4x)

            int sampleoffset = (i*4+startoffset) % buffersize; // the theoretical offset of the sample, if they were arrayed in memory sequentially (they are not)
            int blockoffset;
            int blocksize;

            if (sampleoffset < 64*1024)
            {
               blockoffset = 0;
               blocksize = 32*1024; // size of DMEM0 & DMEM1 in bytes
            }
            else if (sampleoffset < 96*1024)
            {
               blockoffset = 64*1024;
               blocksize = 16*1024; // size of DMEM2 & DMEM3
            }
            else
            {
               blockoffset = 96*1024;
               blocksize = 16*1024; // size of DMEM4 & DMEM5
            }

            sampleoffset -= blockoffset; // calculate the offset from the start of the block

            if ((i & 0x1) == 0)
               val = TPCAP_ReadMem(paddr+blockoffset + ((sampleoffset/2) & 0xFFFFFFFC));
            else
               val = TPCAP_ReadMem(paddr+blockoffset + blocksize + ((sampleoffset/2) & 0xFFFFFFFC));
         }
         else
         {
            val = TPCAP_ReadMem(paddr + (i * 4 + startoffset) % buffersize);
         }

#if !defined (STANDALONE)

         if ( (i % (numWords / 10)) == 0 )
         {
            //fflush(fp);
            printf("Writing file...%d%%\n", (i * 100 + numWords) / numWords);
            //sleep(2);
         }

#endif

         /*            if (val == 0xAAAAAAAA)
                         break;  */

         if (display == 1)
            fprintf(fp, "%6d\t%08X\t%6d\t%6d\n", i, val, (short)(val >> 16), (short)(val & 0xFFFF) );
         else if (display == 0)
            fprintf(fp, "%04X %04X\n", val >> 16, val & 0xFFFF);
         else if (display == 2)
         {
            int j;
            static unsigned long long sample = 1000000000;


            fprintf(fp, "#%lld\n", sample);

            fprintf(fp, "b");

            for(j = 31; j >= 0; j--)
            {
               fprintf(fp, "%d" , (val & (1 << j)) == 0 ? 0 : 1 );
            }

            fprintf(fp, " !\n");

            if (( testPortClockSel == 5) && (!(IS_28_NM_CHIP)))
            {
               sample += 3333;
            }
            else
            {
               sample += 2500;
            }

         }

      }
   }

   printf("Done\n");
#if !defined(STANDALONE)
   fclose(fp);
#endif

   return(0);
}


MOCALIB_CLI void printhelp()
{
   printf(
      "    tpcap [--bursttype <burst_type>] [--stoptype <0-no, 1-CRC, 2-timeout,3-bad probe result> ] [--nsamples <val>] [--port <val>] [--direction <0-Tx, 1-Rx, 3-loopback, 4-All>] [--memalloc] \n"
      "          [--capture_wait_mode <0-once capturing stop all modules , 1-multi capturing] [--capture_wait_mode_capture_num <0-endless, others-capture numbers] [--phy_probe <cap_phy_probe>] [--wait] \n"
      "          [--node <node_id>] [--lpbk <val>] [--phy_num <val>] [--testPortClockSel <val>] [--testPortAdcRate <val>] [--testPortDataSel <val>] [--testPortControlSel <val>]  [--bursttypes <bitwise of multiple burst_types>] \n"
	  "          [--noAssert] [--nbursts <val>] [--burstStopType <burst_stop_type>] \n" 
      "    tpcap dump [--file <name>] [--display <0,1,2>] [--force <0,1>]\n"
      "    --dispaly   can be 0-2  Meaning : 	\n"
      "     0   normal print (default) 04X 04X\n"
      "     1   printing with sampling index\n"
      "     2   print for 'port=1' option. Prints the file in waveform format\n"
      "    --nbursts can be used to record specific number of bursts\n"
      "    --bursttype can be 0-24 Meaning : 	\n"
      "     0   BT_UNUSED0 -    \n"
      "     1   BT_BEACON -     \n"
      "     2   BT_DIVERSITY -           Diversity Mode profile in 50 MHz\n"
      "     3   BT_PROBE_I -             50M Probe I\n"
      "     4   BT_PROBE_II -            \n"
      "     5   BT_PROBE_III -            \n"
      "     6   BT_MAP -                 50M Map\n"
      "     7   BT_UNICAST -             Unicast 50M profile \n"
      "     8   BT_BROADCAST -           50M Braodcast\n"
      "     9   BT_20_EVM_PROBE -       \n"
      "     10  BT_BRCM_CALIBRATION -    BROADCOM PROPRIETARY: Calibration burst\n"
      "     11  BT_UNUSED2 -             \n"
      "     12  BT_20_MAP_100 -          MAP profile in MoCA 2.0 PHY\n"
      "     13  BT_20_UC_NPER -          NPER Unicast profile in MoCA 2.0 PHY (1e-6)\n"
      "     14  BT_20_GCD_NPER -         NPER GCD profile in MoCA 2.0 PHY    (1e-6)\n"
      "     15  BT_20_OFDMA -            OFDMA profile in MoCA 2.0 PHY\n"
      "     16  BT_20_UC_VLPER -         VLPER Unicast profile in MoCA 2.0 PHY.  (1e-8)\n"
      "     17  BT_20_GCD_VLPER -        VLPER GCD profile in MoCA 2.0 PHY.      (1e-8) \n"
      "     18  BT_CB_UC_NPER -          NPER Unicast profile in channel bonding\n"
      "     19  BT_CB_EVM_PROBE -        EVM Probe profile in channel bonding\n"
      "     20  BT_CB_UC_VLPER -         VLPER Unicast profile in channel bonding\n"
      "     21  BT_DIVERSITY_100 -       Diversity Mode profile in MoCA 2.0 100 MHz  channel \n"
      "     22  BT_DIVERSITY_2 -         Diversity Mode profile in Secondary Channel of a bonded link \n"
      "     23  BT_EVM_SIMPLE_OFDMA -    EVM OFDMA Simple         \n"
      "     24  BT_BRCM_EVM_PROBE_ALL_ZERO -   BROADCOM PROPRIETARY: OFDMA - 0 NBAS in all sub-carriers (used in Tx Only)\n"
      "    \n"
      "    --phy_probe can be 0-7 Meaning : 	\n"
      "    0 - ADC out (ADC clock of 200 Mhz)\n"
      "    1 - Slicer in (PHY_CLK)\n"
      "    2 - MPC (SYS_CLK)\n"
      "    3 - Autocorrelation out (PHY_CLK)\n"
      "    4 - Phase Rotator out (PHY_CLK) - WIN \n"
      "    5 - MPD (line_clk_derived)\n"
      "    6 - Tx out @200MHz (DAC_CLK)\n"
      "    7 - FFT out (PHY_CLK)\n"
      "    Examples:\n"
      "    Port=0 (ADC)  : tpcap --wait\n"
      "    Port=0 (MPD)  : tpcap --wait --phy_probe 5\n"
      "    Port=1 (Logic): tpcap --port 1 --testPortDataSel 13 --testPortControlSel 4 --testPortClockSel 5 --nsamples 4194304 --wait --display 2\n"
   );
}

void tpcapDone(void *userArg)
{
   //printf("TPCAP done!!!!\n");
   test = 1;
}

#if !defined(CHIP_63268) && !defined(STANDALONE)
unsigned int tpcap_allocMem(unsigned int size)
{
   FILE *fp = fopen("/proc/iomem", "r");
   char name[1024];
   char str[1024];
   unsigned int start = 0;
   unsigned int end;

   if (!fp)
   {
      printf("Unable to open /proc/iomem\n");
      exit(1);
   }

   while(!feof(fp))
   {
      fgets(str, 1024, fp);
      sscanf(str, "%08X-%08X : %s\n", &start, &end, name);

      if (strstr(name, "bmem.0") != NULL)
      {
         if (end - start > size)
            break;
      }

      start = 0;
   }

   fclose(fp);

   if ((start == 0) && (IS_28_NM_CHIP))
   {
      start = 0x10000000;
   }

   return(start);
}
#else
MOCALIB_CLI unsigned int tpcap_allocMem(unsigned int size)
{
   unsigned int reg;

   // Enable the TPCAP memories on the 680x
   if (CLKGEN_SYS_CTRL_INST_POWER_SWITCH_MEMORY)
   {
      reg = regr(CLKGEN_SYS_CTRL_INST_POWER_SWITCH_MEMORY);

      if (reg & 0x1)
      {
         regw(CLKGEN_SYS_CTRL_INST_POWER_SWITCH_MEMORY, 2);
         sleep(1);
      }

      if (reg & 0x2)
      {
         regw(CLKGEN_SYS_CTRL_INST_POWER_SWITCH_MEMORY, 0);
         sleep(1);
      }
   }

   return(TPCAPBASE + 0x100);
}
#endif



static unsigned int g_phy_num=0;

#define MAX_NSAMPLES_FOR_MEMALLOC		524288

void tpcap_cleanup(void)
{
   if (g_mocaHandle)
   {
      moca_close(g_mocaHandle);
      g_mocaHandle = NULL;
   }
}

#ifndef STANDALONE
int main(int argc, const char **argv)
#else
MOCALIB_CLI int tpcap_main(int argc, const char **argv)
#endif
{

   int dump = 0;
   unsigned int display = 0;
   unsigned int force = 1;
   unsigned int direction = 3; // default for all directions
   int CaptureBurstType = 0;
   int stopBurstType=0;
   int stoptype = 1; //default stop tpcap on CRC
   unsigned int capture_wait_mode = CAPTURE_WAIT_MODE_DEF; // stop all
   unsigned int capture_wait_mode_capture_num = CAPTURE_WAIT_MODE_MULTI_CAPTURING_ENDLESS_NUM; 
   unsigned int l_capture_number=0;
   int nsamples = 0;
   int nbursts  = 0;
   unsigned int paddr;
   unsigned int node = 16;
   unsigned int port = 0; // 1 for test ports 0 for fast PHY data
   unsigned int testPortDataSel = 0;
   unsigned int testPortControlSel = 0;
   unsigned int testPortClockSel = 0;
   unsigned int testPortAdcRate = 0;
   unsigned int lpbk = 1;
   unsigned int wait = 0;
   unsigned int fwAssert = 1;
   unsigned int memalloc = 0;
   unsigned int cont_FW = 0;
   unsigned int phy_num = 0;
   char *       p_moca_ifname = NULL;
   unsigned int i;
   uint32_t      burstTime;
   int ret;

   const char* fname = "tpcapCapture.txt";
   char  burst_time[20];
   char  fname_time [100];
   
   int phy_probe = 0;

   argv++;
   argc--;

   if (argc && !strcmp(*argv, "wan"))
   {
      p_moca_ifname = "moca0";
      spidev = 0;
      argv++;
      argc--;
   }
   else if (argc && !strcmp(*argv, "lan"))
   {
      p_moca_ifname = "moca1";
      spidev = 1;
      argv++;
      argc--;
   }

   g_mocaHandle = moca_open( p_moca_ifname );

   if (!g_mocaHandle )
   {
      printf("Unable to connect to mocad\n");
      return(-1);
   }

   if (argc && !strcmp(*argv, "dump"))
   {
      dump = 1;

      argv++;
      argc--;
   }

   while (argc)
   {
      if( !strcmp(*argv, "--memalloc") )
      {
         memalloc = 1;
         nsamples = MAX_NSAMPLES_FOR_MEMALLOC;
      }
      
	  else if( !dump && !strcmp(*argv, "--capture_wait_mode") )
      {
         argv++;
         argc--;

         if (argc)
            capture_wait_mode = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((capture_wait_mode < 0) || (capture_wait_mode > 1))
            printf("WARNING: capture_wait_mode should be in the range of 0-1\n");
      }


      else if( !dump && !strcmp(*argv, "--capture_wait_mode_capture_num") )
      {
         argv++;
         argc--;

         if (argc)
            capture_wait_mode_capture_num = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }


	  

      else if( !dump && !strcmp(*argv, "--nsamples") )
      {
         argv++;
         argc--;

         if (argc)
            nsamples = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--nbursts") )
      {
         argv++;
         argc--;

         if (argc)
            nbursts = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--lpbk") )
      {
         argv++;
         argc--;

         if (argc)
            lpbk = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--testPortClockSel") )
      {
         argv++;
         argc--;

         if (argc)
            testPortClockSel = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--testPortAdcRate") )
      {
         argv++;
         argc--;

         if (argc)
            testPortAdcRate = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--port") )
      {
         argv++;
         argc--;

         if (argc)
            port = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--bursttype") )
      {
         argv++;
         argc--;

         if (argc){
            CaptureBurstType = (1 << strtol(*argv, NULL, 0));
		 } 	
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((CaptureBurstType < 1) || (CaptureBurstType > (1<< 24)))
            printf("WARNING: CaptureBurstType should be in the range of 1-24\n");
      }

      else if( !dump && !strcmp(*argv, "--bursttypes") )
      {
         argv++;
         argc--;

         if (argc)
            CaptureBurstType = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }



	   else if( !dump && !strcmp(*argv, "--burstStopType") )
      {
         argv++;
         argc--;

         if (argc){
            stopBurstType = (1 << strtol(*argv, NULL, 0));
		   } 	
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((stopBurstType < 1) || (stopBurstType > (1<< 24)))
            printf("WARNING: stop burst CaptureBurstType should be in the range of 1-24\n");
      }


	  



      else if( !dump && !strcmp(*argv, "--stoptype") )
      {
         argv++;
         argc--;

         if (argc)
            stoptype = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((stoptype < 0) || (stoptype > 3))
            printf("WARNING: stoptype should be in the range of 0-3\n");
      }
      else if( !strcmp(*argv, "--phy_probe") )
      {
         argv++;
         argc--;

         if (argc)
            phy_probe = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((phy_probe < 0) || (phy_probe > 7))
            printf("WARNING: phy_probe should be in the range of 0-7\n");
      }
      else if( !dump && !strcmp(*argv, "--node") )
      {
         argv++;
         argc--;

         if (argc)
            node = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((node < 0) || (node > 15))
            printf("WARNING: node should be in the range of 0-15\n");
      }
      else if( !dump && !strcmp(*argv, "--wait") )
      {
         wait = 1;
      }
      else if( !dump && !strcmp(*argv, "--noAssert") )
      {
         fwAssert = 0;
      }
      else if( !dump && !strcmp(*argv, "--phy_num") )
      {
         argv++;
         argc--;

         if (argc)
            phy_num = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--testPortDataSel") )
      {
         argv++;
         argc--;

         if (argc)
            testPortDataSel = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !dump && !strcmp(*argv, "--testPortControlSel") )
      {
         argv++;
         argc--;

         if (argc)
            testPortControlSel = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }
      }
      else if( !strcmp(*argv, "--file") )
      {
         argv++;
         argc--;

         if (argc)
            fname = *argv; //fp = fopen(*argv, "w");
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

      }
      else if( !strcmp(*argv, "--direction") )
      {
         argv++;
         argc--;

         if (argc)
            direction = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((direction < 0) || (direction > 3))
            printf("WARNING: direction should be in the range of 0-3\n");
      }
      else if( !strcmp(*argv, "--display") )
      {
         argv++;
         argc--;

         if (argc)
            display = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((display < 0) || (display > 2))
            printf("WARNING: display should be in the range of 0-2\n");
      }

      else if( !strcmp(*argv, "--force") )
      {
         argv++;
         argc--;

         if (argc)
            force = strtol(*argv, NULL, 0);
         else
         {
            printhelp();
            tpcap_cleanup();
            return(0);
         }

         if ((force < 0) || (force > 1))
            printf("WARNING: force should be in the range of 0-1\n");
      }



      else
      {
         printf("Unknown option: %s\n", *argv);
         printhelp();
         tpcap_cleanup();
         return(0);
      }

      argc--;
      argv++;
   }


   if (moca_get_drv_info( g_mocaHandle, 0, &vers))
   {
      printf("Error obtaining chip type\n");
      tpcap_cleanup();
      return(-1);
   }

   

   printf("Detected chip ID: 0x%X\n", vers.chip_id);

   if (!dump)
   {
      g_phy_num= phy_num;
   }

   if ((vers.chip_id & 0xFFF000F0) == 0x680000B0)
      nsamples = 4096;
   else if((vers.chip_id & 0xFFF000F0) == 0x680000C0)
      nsamples = 4096*8;
   else if (nsamples == 0) // nsamples wasn't set by a CLI argument
   {
      nsamples = 524288 * 4; //1024*1024/2; //1sample = 5 ns    --> 400,000 samples 1ms , --> 2^22 samples ~10ms
   }

   // Set capture_wait_mode_capture_num - depen on memory size  // ZADA
   if (capture_wait_mode == CAPTURE_WAIT_MODE_MULTI_CAPTURING)
   {
		cont_FW =1;
		fwAssert = 0;
   }
   else 
   {	   
		cont_FW =0;
   }	
   
   do {

   TPCAP_Init(vers.chip_id, g_phy_num);
           strcpy(fname_time , fname);

   if (!dump)
   {
      struct moca_lab_tpcap dp;

      printf("Disabling auto-restart after ASSERT\n");
      __moca_set_assert_restart(g_mocaHandle, 0);

      if (wait)
         moca_register_tpcap_done_cb(g_mocaHandle, tpcapDone, NULL);

		  
		  if(l_capture_number ==0) // only at the beginning 
		  {
      printf("Allocating memory...%d bytes\n", nsamples * 4 + 256);

#if !defined(STANDALONE)

      if (memalloc)
         MoCAOS_AllocPhysMem((nsamples * 4) + 256, (unsigned int **)&paddr);
      else
#endif
         paddr = tpcap_allocMem(nsamples * 4 + 256);

      paddr = (paddr + 255) & 0xFFFFFF00; // do some alignment

      if (paddr == 0)
      {
         fprintf(stderr, "Unable to allocate memory\n");
         tpcap_cleanup();
         return(-1);
      }

      // The 680x chips use RO memory for TPCAP
      if ((vers.chip_id & 0xFFF00000) != 0x68000000)
      {
         for (i = 0; i < nsamples * 4; i += 4)
            regw(paddr + i, 0);
      }

      printf("Memory allocated at %08x\n", (unsigned int)paddr);
		  }	

		  dp.type = ((phy_num << 16) |(direction << 13) | (cont_FW << 12) | (stoptype << 8) | node );
      dp.enable = nbursts<<8;
      dp.enable |= 1; // 1 means start
	  dp.type_2= CaptureBurstType;


      if (stopBurstType){
        dp.stopBurstType = stopBurstType;
      }
      else {
        dp.stopBurstType = CaptureBurstType;
      } 


      printf("Starting capture:paddr=%x, testPortDataSel=%x, testPortControlSel=%x, testPortAdcRate=%x, testPortClockSel=%x,wait=%x, phy_num=%x,phy_probe=%x,port-%x \n", paddr, testPortDataSel, testPortControlSel, testPortAdcRate, testPortClockSel,  wait, phy_num,phy_probe, port);

      if ((vers.chip_id & 0xFFF00000) == 0x68000000)
         TPCAP_Start(phy_probe, port /*1 for test ports  0 for phy probes (fast)*/, 0xFFFFFFF0, paddr, testPortDataSel, testPortControlSel, testPortAdcRate, testPortClockSel, 0, wait, phy_num);
      else
         TPCAP_Start(phy_probe, port /*1 for test ports  0 for phy probes (fast)*/, nsamples, paddr, testPortDataSel, testPortControlSel, testPortAdcRate, testPortClockSel, lpbk, wait, phy_num);

      if (wait)
      {
         if (0 != moca_init_evt(g_mocaHandle))
         {
            printf("Error starting event loop\n");
            tpcap_cleanup();
            return(-1);
         }
      }

      //printf("Notifying firmware...\n");
      moca_set_lab_tpcap( g_mocaHandle, &dp);

      moca_set_miscval(g_mocaHandle, paddr);
      moca_set_miscval2(g_mocaHandle, nsamples * 4);

      if (wait)
      {
         printf("Waiting for results\n");
			 test=0;

         moca_wait_for_event(g_mocaHandle, 0xFFFFFFF0 / 10, &test);
   			 if (capture_wait_mode == CAPTURE_WAIT_MODE_MULTI_CAPTURING)
			 {				 
				printf("Waiting capture_wait_mode %s for results\n", burst_time);
				ret= moca_get_tpcap_capture_time(g_mocaHandle,&burstTime);
				if (ret ==0)
				{
					sprintf (burst_time,"_%x", burstTime);
					strcat(fname_time , burst_time);		
					printf("Waiting %s for results 111\n", burst_time);
				}
				else 
				{
				   printf ("Error %x\n",ret);
				}
			 }


         printf("Got tpcap stop trigger \n");

         if (( force==1) && (IS_28_NM_CHIP))
         {
            regw(TPCAP_SEL_ADDR, (regr(TPCAP_SEL_ADDR) & 0xFFFFFFEF) ); // tpcap_tp_sel = 0

            regw(TPCAP_SW_STOP_TRIGGER, 1);

            for(i=0; i < 300; i++ );

            startoffset = regr(TPCAP_SIZE_STATUS); // When force stop trigger through the PHY the size status is set to 0.
            read_offset=1;

            regw(SUN_RGR_SW_RESET_1, 1); //  reset tpcap
            printf("Force stop trigger through system TPCAP's registers start offset=%x \n", startoffset);
            regw(SUN_RGR_SW_RESET_1, 0); //  reset tpcap
         }
         else
         {
            	  if(MOCA_PHY_FIXD_CLK_SEL)
				  {
					regw(MOCA_PHY_FIXD_CLK_SEL,  (5 << 15));
				  }	
				  else
				  {
					regw(MOCA_PHY_FIXD_CMN_TEST_PORTS,  (5 << 15)); // after stopping TPCAP update the clock to 5
				  } 
			
			regw(TPCAP_SW_STOP_TRIGGER, 1);
         }

		 dp.type = ( (phy_num << 16) | (direction << 13) | (cont_FW << 12) | (stoptype << 8) | node );
         dp.enable = nbursts<<8;
         dp.enable |= 0; // 1 means finalize
		 dp.type_2= CaptureBurstType;

      if (stopBurstType){
        dp.stopBurstType = stopBurstType;
      }
      else {
        dp.stopBurstType = CaptureBurstType;
      } 

         printf("Notifying firmware on tpcap stop...\n");
         moca_set_lab_tpcap( g_mocaHandle, &dp);

         //printf("Dumping data..\n");
			 
			 
			 TpcapDumpHandler(fname_time , display, testPortClockSel);

         if ( fwAssert == 1 )
         {
            //generate assert in FW
				dp.type = ( (phy_num << 16) | (direction << 13) | (cont_FW << 12) | (stoptype << 8) | node );
            dp.enable = nbursts<<8 | 0; // 1 means finalize
            dp.enable |= 2; // 2 means assert
			dp.type_2= CaptureBurstType;
			if (stopBurstType){
				dp.stopBurstType = stopBurstType;
			}
			else {
				dp.stopBurstType = CaptureBurstType;
			} 

			
            moca_set_lab_tpcap( g_mocaHandle, &dp);
         }
      }
   }

   if (dump)
   {
      printf("Dumping data..\n");

      // stop the TPCAP
      if ( IS_28_NM_CHIP)
      {
         regw(TPCAP_SEL_ADDR, (regr(TPCAP_SEL_ADDR) & 0xFFFFFFEF) ); // tpcap_tp_sel = 0
         regw(TPCAP_SW_STOP_TRIGGER, 1);

         for(i=0; i < 300; i++ );

         startoffset = regr(TPCAP_SIZE_STATUS); // When force stop trigger through the PHY the size status is set to 0.
         read_offset=1;

         regw(SUN_RGR_SW_RESET_1, 1); //  reset tpcap
         printf("Force stop trigger through system TPCAP's registers start offset=%x\n", startoffset);
         regw(SUN_RGR_SW_RESET_1, 0); //  reset tpcap
      }
      else
      {
           	  if(MOCA_PHY_FIXD_CLK_SEL)
			  {
				regw(MOCA_PHY_FIXD_CLK_SEL,  (5 << 15));
			  }	
			  else
			  {
				regw(MOCA_PHY_FIXD_CMN_TEST_PORTS,  (5 << 15)); // after stopping TPCAP update the clock to 5
			  } 
			regw(TPCAP_SW_STOP_TRIGGER, 1);
      }

      TpcapDumpHandler(fname, display, testPortClockSel);
   }
	   l_capture_number ++;
	   //printf("Parameters  wait=%d, capture_wait_mode=%d, l_capture_number=%d, capture_wait_mode_capture_num=%d  \n",wait,capture_wait_mode, l_capture_number,capture_wait_mode_capture_num );
   } while ( 
		(wait) 															&&
		(!dump)                                                         &&
        (capture_wait_mode == CAPTURE_WAIT_MODE_MULTI_CAPTURING)		&&
		(l_capture_number < capture_wait_mode_capture_num)
   );	
   tpcap_cleanup();
   return(0);
}
