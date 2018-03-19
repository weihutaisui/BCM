/*
  <:copyright-BRCM:2017:proprietary:standard
  
     Copyright (c) 2017 Broadcom 
     All Rights Reserved
  
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
*/

/****************************************************************************
 *
 * pon_drv_serdes_util.c -- Bcm Pon driver: utilities for SerDes fine tuning
 * and debug
 *
 * Description:
 *  Originated at Mixed Signals team 
 *
 * Authors: Akiva Sadovski, Vitaly Zborovski 
 *
 * $Revision: 1.1 $
 *
 * 2017.July: updated by VZ
 *****************************************************************************/

#include <linux/delay.h>
#include <linux/kernel.h>
#include "pon_drv.h"
#include "gpon_ag.h"
#include "ru_types.h"
#include "pon_drv_serdes_util.h" 
#include "ru.h"
#include "boardparms.h"

#if defined(CONFIG_BCM96846)     // name structure
#define CHOP(A, B)     B
#define WAN_SERDES     SERDES_STATUS
#elif defined(CONFIG_BCM96856)
#define CHOP(A, B)     B
#else
#define CHOP(A, B)     A##B
#endif

void writePONSerdesReg(uint16_t lane, uint16_t address, uint16_t value,uint16_t  mask)   
{
    uint32_t serdes_addr =(lane<<16) | address;  
    uint16_t mask_inv  = ~mask;
    uint16_t rd_serdes = 0 ;

    RU_FIELD_WRITE(0, PMI, LP_1, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_ADDR), serdes_addr );
    
    RU_FIELD_WRITE(0, PMI, LP_2, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_WRDATA)   , value   );
    RU_FIELD_WRITE(0, PMI, LP_2, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_MASKDATA) , mask_inv );

    RU_FIELD_WRITE(0, PMI, LP_0, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_WRITE),1);  // Select Write access     
    RU_FIELD_WRITE(0, PMI, LP_0, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_EN)   ,1);  // Initiate SerDes reg access 

    udelay(5);
    RU_FIELD_WRITE(0, PMI, LP_0, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_EN)   ,0);
    RU_FIELD_WRITE(0, PMI, LP_0, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_WRITE),0);

    if (Debug) 
    {
        printk("\n --> Write-SerDes: Address =0x%08x  Data=0x%04x  Mask=0x%04x [mask_inv=0x%04x]", serdes_addr,value, mask, mask_inv) ;
        rd_serdes = readPONSerdesReg(lane, address); 
        printk("\n     READ back   : Address =0x%08x  Data=0x%04x \n",serdes_addr, rd_serdes);
    }
} // End of 'writePONSerdesReg()' function
/////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t readPONSerdesReg(uint16_t lane, uint16_t address) 
{
    uint32_t serdes_addr =(lane<<16) | address; 
    uint32_t rd32_data = 0 ;

    RU_FIELD_WRITE(0, PMI, LP_1, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_ADDR), serdes_addr );
    
    RU_FIELD_WRITE(0, PMI, LP_0, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_WRITE),0); // Select Read access
    RU_FIELD_WRITE(0, PMI, LP_0, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_EN)  ,1); // Initiate SerDes reg access 
    udelay(5);
   
    RU_REG_READ(0, PMI, LP_3,rd32_data );

    RU_FIELD_WRITE(0, PMI, LP_0, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_EN)   ,0);
    udelay(5);

    return (rd32_data & 0xffff) ;
} // End of 'readPONSerdesReg()' function
/////////////////////////////////////////////////////////////////////////////////////////////////////

void PCSwriteSerdes(uint16_t lane, uint16_t address, uint16_t value,uint16_t  mask)   
{
#if defined(CONFIG_BCM963158)
    uint32_t serdes_addr =(lane<<16) | address;  
    uint16_t mask_inv  = ~mask;
    uint16_t rd_serdes = 0 ;
    uint32_t wr32_data = 0 ;

    RU_REG_WRITE(0, PMI, LP_0, wr32_data);

    RU_FIELD_WRITE(0, PMI, LP_1, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_ADDR), serdes_addr );
    
    RU_FIELD_WRITE(0, PMI, LP_2, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_WRDATA)   , value   );
    RU_FIELD_WRITE(0, PMI, LP_2, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_MASKDATA) , mask_inv );

    wr32_data = 5 ; // Write  PCS-register : { [2]pcs_pmi_lp_en = 1 , [1]misc_pmi_lp_en = 0, [0]pmi_lp_write = 1}
    RU_REG_WRITE(0, PMI, LP_0, wr32_data);

    udelay(5);
    wr32_data = 0 ; // end pmi_lp transaction
    RU_REG_WRITE(0, PMI, LP_0, wr32_data);

    if (Debug) 
    {
        printk("\n --> Write-SerDes: Address =0x%08x  Data=0x%04x  Mask=0x%04x [mask_inv=0x%04x]", serdes_addr,value, mask, mask_inv) ;
        rd_serdes = PCSreadSerdes(lane, address); 
        printk("\n     READ back   : Address =0x%08x  Data=0x%04x \n",serdes_addr, rd_serdes);
    }
#else
    return;
#endif
} // End of 'PCSwriteSerdes()' function
/////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t PCSreadSerdes(uint16_t lane, uint16_t address) 
{
#if defined(CONFIG_BCM963158)
    uint32_t serdes_addr =(lane<<16) | address; 
    uint32_t rd32_data = 0 ;
    uint32_t wr32_data = 0 ;
    RU_REG_WRITE(0, PMI, LP_0, wr32_data);
    RU_FIELD_WRITE(0, PMI, LP_1, CHOP(CR_XGWAN_TOP_WAN_MISC_, PMI_LP_ADDR), serdes_addr );

    wr32_data = 4 ; // Read PCS-register : { [2]pcs_pmi_lp_en = 1 , [1]misc_pmi_lp_en = 0, [0]pmi_lp_write = 0}
    RU_REG_WRITE(0, PMI, LP_0, wr32_data);
    
    udelay(5);
    RU_REG_READ(0, PMI, LP_4, rd32_data );  // read back PCS reg value from 'WAN_TOP_PMI_LP_4'

    wr32_data = 0 ; // end pmi_lp transaction
    RU_REG_WRITE(0, PMI, LP_0, wr32_data);

    return (rd32_data & 0xffff) ;
#else
    return 0xDEAD;
#endif
} // End of 'PCSreadSerdes()' function
/////////////////////////////////////////////////////////////////////////////////////////////////////

void sgb_rescal_init(void)
{
   uint32_t rd32_data = 0x0   ;
   uint32_t wr32_data = 0x8000; 
#if defined(CONFIG_BCM963158)
    RU_REG_WRITE  (0, RESCAL, AL_CFG, wr32_data );
    udelay(100);
    WAN_TOP_READ_32(0x020, rd32_data);
#else
    RU_REG_WRITE(0,RESCAL ,CFG, wr32_data );  //CFG_WAN_RESCAL_RSTB = 1
    udelay(100);
#if defined(CONFIG_BCM96846)
    RU_REG_READ(0,RESCAL ,STATUS0,rd32_data );         
#else
    RU_REG_READ(0,RESCAL ,STATUS_0,rd32_data );         
#endif
#endif
    __logInfo("\n WAN_TOP_RESCAL_STATUS_0 =0x%x", rd32_data);

} //END of 'sgb_rescal_init()' function


/////////////////////////////////////////////////////////////////////////////////////////////////////

void set_clkp1_offset( uint8_t desired_p_d_offset)      /* clkp1_offset (p-d) */
{
    uint8_t  step, cnt ,d_location, now_p_location , next_p_location  ;                   
    uint16_t rd_serdes ;

    desired_p_d_offset = desired_p_d_offset % 64      ; /* offset smaller than UI */
    rd_serdes       = readPONSerdesReg(0x0800, 0xd007); /* Read current slicer possitions */
    d_location      = (rd_serdes & 0x7f)              ; /* {0xd007}cnt_bin_d_dreg[06:00] */
    now_p_location  = (rd_serdes & 0x7f00) >> 8       ; /* {0xd008}cnt_bin_p1_dreg[14:08] */
    next_p_location = d_location + desired_p_d_offset ;

    /* calculate number of movement steps and direction */
    if (next_p_location >= now_p_location)
    {
        step = next_p_location - now_p_location ;
        writePONSerdesReg(0x0800,0xd004,0x2401,0x74ff) ; /*  rx_pi_phase_step_dir= 0x1 (Increment,right shift) ,rx_pi_slicers_en= 0x2 ,step_cnt = 1  */
        /* printk("\n Increment steps  = %d", step  ); */

    } 
    else  
    {
        step = now_p_location - next_p_location ;
        writePONSerdesReg(0x0800,0xd004,0x2001,0x74ff) ; /*  rx_pi_phase_step_dir= 0x0 ( Decrements, left shift) ,rx_pi_slicers_en= 0x2 ,step_cnt = 1 */
        /* printk("\n Decrement steps  = %d", step  ); */
    }   

    /* move the Slicer(P) the required steps */
    for ( cnt = 0 ; cnt < step ; cnt++ ) 
    {
        writePONSerdesReg(0x0800,0xd004,0x0200,0x0200) ; // rx_pi_manual_strobe= 0x1 = Increments/Decrements by 1 every strobe
    }


    if (Debug)  /* Verify rx-pi adjustment correctly */
    {
        rd_serdes       = readPONSerdesReg(0x0800, 0xd007); // Read adjusted slicer possitions
        d_location      = (rd_serdes & 0x7f)              ; // {0xd007}cnt_bin_d_dreg[06:00]
        now_p_location  = (rd_serdes & 0x7f00) >> 8       ; // {0xd008}cnt_bin_p1_dreg[14:08]

        printk("\n 0xd007 =0x%04x", rd_serdes );
        printk("\n d_location =%d",d_location  );
        printk("\n now_p_location =%d",now_p_location  );
        printk("\n Done Slicer-P-Adjustment" );

    }
} // End of 'set_clkp1_offset()' function

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_clk90_offset( uint8_t desired_m1_d_offset)       /* clk90 offset (m1-d) */ 
{
    uint8_t  step, cnt ,d_location, now_m1_location, next_m1_location  ;                   
    uint16_t rd_serdes ;

    desired_m1_d_offset = desired_m1_d_offset % 64     ; /* offset smaller than UI */
    rd_serdes        = readPONSerdesReg(0x0800, 0xd009); /* Read current slicer possitions */
    now_m1_location  = (rd_serdes & 0x7f)              ; /* {0xd009}cnt_bin_m1_mreg[06:00] */
    d_location       = (rd_serdes & 0x7f00) >> 8       ; /* {0xd009}cnt_bin_d_mreg[14:08]  */
    next_m1_location = d_location + desired_m1_d_offset;

    /* calculate number of movement steps and direction */
    if (next_m1_location >= now_m1_location)      
    {
        step = next_m1_location - now_m1_location ;
        writePONSerdesReg(0x0800,0xd004,0x4401,0x74ff) ; /*  rx_pi_phase_step_dir= 0x1 (Increment,right shift) ,rx_pi_slicers_en= 0x4 ,step_cnt = 1  */
        /* printk("\n Increment steps  = %d", step  ); */

    } 
    else  
    {
        step = now_m1_location - next_m1_location ;
        writePONSerdesReg(0x0800,0xd004,0x4001,0x74ff) ; /*  rx_pi_phase_step_dir= 0x0 ( Decrements, left shift) ,rx_pi_slicers_en= 0x4 ,step_cnt = 1 */
        /* printk("\n Decrement steps  = %d", step  ); */
    }   

    /* move the Slicer(P) the required steps  */
    for ( cnt = 0 ; cnt < step ; cnt++ ) 
    {
        writePONSerdesReg(0x0800,0xd004,0x0200,0x0200) ; // rx_pi_manual_strobe= 0x1 = Increments/Decrements by 1 every strobe
    }

    if(Debug) /* Verify rx-pi adjustment correctly */
    {        
        rd_serdes        = readPONSerdesReg(0x0800, 0xd009); // Read adjusted slicer possitions
        now_m1_location  = (rd_serdes & 0x7f)              ; // {0xd009}cnt_bin_m1_mreg[06:00]
        d_location       = (rd_serdes & 0x7f00) >> 8       ; // {0xd009}cnt_bin_d_mreg[14:08]
        next_m1_location = d_location + desired_m1_d_offset;


        printk("\n 0xd009 =0x%04x", rd_serdes );
        printk("\n now_m1_location =%d",now_m1_location  );
        printk("\n d_location =%d",d_location  );
        printk("\n Done Slicer-M1-Adjustment" );
    }

} // END of 'set_clk90_offset( )'
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void rx_pi_spacing(uint8_t desired_m1_d_offset, uint8_t desired_p_d_offset )
{
    writePONSerdesReg(0x800, 0xd004, 0x0800, 0x0800) ; // Freeze RX-PI , rx_pi_manual_mode = 1
    set_clk90_offset(desired_m1_d_offset);             // clk90_offset
    set_clkp1_offset(desired_p_d_offset );             // clkp1_offset
    writePONSerdesReg(0x800, 0xd004, 0x0000, 0x7800) ; // UnFreeze RX-PI, rx_pi_manual_mode = 0 
} // End of 'rx_pi_spacing()' function

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Configure the PLL/clock input selection:  LCREF (50MHz) or PADREF (155.52MHz or 156.25MHz) */
  // since refclk 78Mhz is not supported, pll0_lcref is hard wired to 0 ==> lcref_sel should always select ppl1_lcref
  // set refin to 1 to select lcref_clk (50MHz)
  // afe constraint: refin_en ==1 and refout_en ==1 => illegal settin
void set_pll_refclk(uint8_t pll_refclk)
{
    uint32_t rd32_data;

    if (pll_refclk == LCREF)  // -->  WAN_SERDES_PLL_CTL = 0x105 
    {
        __logDebug("\n PLL RefClock = LCREF (50MHz) ");
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL1_LCREF_SEL), 0); // 0 = select pll1_lcref. 1 = pll0_lcref. 
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL1_REFIN_EN),  1); // Reference select. 0 = select pad_pll1_refclkp/n. 1 - select pll1_lcrefp/n. 
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL0_LCREF_SEL), 1); // 0 = pll0_lcref.  1 = pll1_lcref. 
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL0_REFIN_EN),  1); // Reference select: 0 = select pad_pll0_refclkp/n. 1 = select pll0_lcrefp/n. 
    }
    else // pll_refclk = PADREF  -->  WAN_SERDES_PLL_CTL =0x400 
    {
        __logDebug("\n PLL RefClock = PADREF (155.52MHz or 156.25MHz)");
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL1_LCREF_SEL), 1);  
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL1_REFIN_EN),  0); 
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL0_LCREF_SEL), 0);  
        RU_FIELD_WRITE(0, WAN_SERDES, PLL_CTL, CHOP(CFG_, PLL0_REFIN_EN),  0); 
    }

    if(Debug)
    {
        RU_REG_READ(0, WAN_SERDES, PLL_CTL,rd32_data ); 
        printk("\n WAN_SERDES_PLL_CTL = 0x%x", rd32_data);
    } // End of 'if(Debug)'


    /* wait for stable Ref Clk before continue */
    udelay(100);

} // End of 'pll_refclk_sel()' function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Disable/Bypass Signal-Detect  */
void rx_sigdet_dis(void)
{

//   0xD0C1 bits 6,5 and 0
//   energy_detect_frc_val -> 1
//   energy_detect_frc -> 1
//   afe_signal_detect_dis -> 1
    writePONSerdesReg(0x0800, 0xD0C1, 0x0061, 0x0061); 

//   0xD0C2 bits 0-2 and 4-6:
//   los_thresh -> 0
//   signal_detect_thresh -> 0  

    writePONSerdesReg(0x0800, 0xD0C2, 0x0000, 0x0077);

//  0xD0C3 bits 6-7:
//  analog_sd_override -> 1
    writePONSerdesReg(0x0800, 0xD0C3, 0x0040, 0x00C0);

} // END of 'rx_sigdet_dis()' function















////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
  gearbox_reg_dump: 
  Read WAN-Gearbox registers. based on their address
  future update -> to read the values based on reg.name using "RU_REG_READ(i,b,r,rv);" function
*/


// old address
//#define WAN_START_ADDRS 0x80144000
//#define WAN_END_ADDRS   0x801440c8
#define WAN_START_ADDRS 0x000
#define WAN_END_ADDRS   0x0c8


void gearbox_reg_dump(void)
{
    uint32_t rd_data, addr;

    printk("\n------------------GEARBOX REGISTERS DUMP---------------------- "); 

    for(addr = WAN_START_ADDRS; addr <= WAN_END_ADDRS; addr=addr + 4 )
    {   
        WAN_TOP_READ_32(addr, rd_data);   
        printk("\nReg{0x%x} = 0x%x",addr, rd_data);
    }

}

/*
    serdes_reg_dump
    Read the values of SerDes internal registers based on their address
*/
void serdes_reg_dump(void)
{
    uint16_t rd_serdes, addr_serdes ;

    printk("\n------------------SerDes REGISTERS DUMP---------------------- "); 
    printk("\nDSC_Block:");
    for (addr_serdes = 0xd000; addr_serdes <= 0xd053 ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nTX_PI_LBE ");
    for (addr_serdes = 0xd070; addr_serdes <= 0xd07c ; addr_serdes=addr_serdes+1  ) 
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nCKRST_CTRL ");
    for (addr_serdes = 0xd080; addr_serdes <= 0xd08E ; addr_serdes=addr_serdes+1  ) 
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nAMS_RX/TX_CTRL ");
    for (addr_serdes = 0xd090; addr_serdes <= 0xd0A9 ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nAMS_COM:PLL0 [0x0800]");
    for (addr_serdes = 0xd0B0; addr_serdes <= 0xd0BA ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nAMS_COM:PLL1 [0x0900]");
    for (addr_serdes = 0xd0B0; addr_serdes <= 0xd0BA ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0900, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nSIGDET, TLB_RX/TX, DIG_COM , PATT_GEN, TX_FED ");
    for (addr_serdes = 0xD0C0; addr_serdes <= 0xD11B ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nPLL_CAL_COM:PLL0 [0x0800]");
    for (addr_serdes = 0xD120; addr_serdes <= 0xD129 ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nPLL_CAL_COM:PLL1 [0x0900]");
    for (addr_serdes = 0xD120; addr_serdes <= 0xD129 ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0900, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nCORE_PLL_COM:PLL0 [0x0800]");
    for (addr_serdes = 0xD150; addr_serdes <= 0xD159 ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0800, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
    printk("\nCORE_PLL_COM:PLL1 [0x0900]");
    for (addr_serdes = 0xD150; addr_serdes <= 0xD159 ; addr_serdes=addr_serdes+1  )
    {   
        rd_serdes=readPONSerdesReg(0x0900, addr_serdes)  ;    
        printk("\n[0x%x] = 0x%x", addr_serdes ,rd_serdes );
    }
}
