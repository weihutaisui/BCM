/**************************************************************!!!
* Project:       XX calculation
*
* Description:  This module calculates the XX length according to Probe 3 results.  
*
* Copyright:    (c)Broadcom 2007 , Confidential and Proprietary
******************************************************************/
#include "CalcSignalY.h"


/*---------------------------------------------------------------------*/
/* General definitions                                                 */
/*---------------------------------------------------------------------*/
#undef  HT_TOGGLING
#define BC_CP_LEN_SMALLER_THEN_UC_ENTR_PATCH
//#define PATCH_ADD_LEN_2_CP 


/*---------------------------------------------------------------------*/
/* Private definitions                                                 */
/*---------------------------------------------------------------------*/
#define OFFSET_LIMIT                                  90
#define CP_MARGIN_ENERGY_WIN                          20  // percent from calculated cp
#define CP_MAX_MARGIN_ENERGY_WIN                      10  
#define CP_MARGIN_CIR_TH                              20  // percent from calculated cp
#define CP_MAX_MARGIN_CIR_TH                          20  // max samples spare added to calculated cp
#define MFC_BITS                                      5    // number of bits of mfc threshold (PHY register)

#ifndef HT_TOGGLING
#define CP_MIN_MARGIN_CIR_TH                          8   // min samples spare added to calculated cp
#define CNT1_INIT                                     3 
#define IR_LAV                                        6 
#define DELTA_1                                       8
#define DELTA_2                                       12
#define N_DELTA_BITS                                  4
#define CNT2_INIT                                     5 

#else
#define CP_MIN_MARGIN_CIR_TH_1                        6   // min samples spare added to calculated cp for relativly flat channel 
#define CP_MIN_MARGIN_CIR_TH_2                        8   // min samples spare added to calculated cp for reflection channel
#define CNT1_INIT                                     2   // min number of samples between CIR peeks.
#define CNT2_INIT                                     5   // number of samples before first peek that are allowed to cross HT_LOW_TH
#define IR_LAV                                        6   // moving average on IR before checking HT_TH 
#define DELTA_1                                       16  // round(0.0625*2^NdeltaBits); %16 = delta_1/2^NdeltaBits is the relative amount of channel energy above MFC_Th for HT_HIGH_TH, the first peak must surpass MFC TH by more than MFC_TH + delta_1
#define DELTA_2                                       16  // round(0.0625*2^NdeltaBits); %16 = delta_2/2^NdeltaBits is the relative amount of channel energy below MFC_Th for HT_LOW_TH, no crossing is allowed above MFC_TH - delta_2 prior to CNT2 before peak.
#define DELTA_3                                       384 // round(1.5*2^NdeltaBits); %384 = In Case tes1 and test 2 do not allow HT, we check if the first peak is greater then the maximal prepeak by a factor of delta3, and if so we allow a change in the MFC Th
#define DELTA_4                                       336 // round(1.3125*2^NdeltaBits); %336 = In Case we want to increase/reduce the MFC Th to extend HT cobverage change threshold by a factor of delta_4 more than needed as margin.
#define DELTA_5                                       368 // round(1.4375*2^NdeltaBits); %368 = In Case we want to increase/reduce the MFC Th to extend HT cobverage make sure that max peak is greather then TH_HIGH by a factor of at least delta_5
#define MFS_TH_MAX                                    16  // round(0.5*2^(MFC_BITS)); %16 = Maximal Value allowed for MFC Threshold sqrt(0.) = 0.707   
#define MFS_TH_MIN                                    8   // round(0.25*2^(MFC_BITS)); %8 = Minimal Value allowed for MFC Threshold sqrt(0.25) = 0.5
#define HT_SNR_TH                                     21474836  // round(10.^(-20/10)*2^31); %21474836 = Allow MFC Threshold Change Only if SNR is greater then 20[dB]
#define N_DELTA_BITS                                  8
#define   MFC_TH_MIN                                  8    // minimal allowed value for mfc threshold.
#define   MFC_TH_MAX                                  16   // maximal allowed value for mfc threshold.
#endif



/*---------------------------------------------------------------------*/
/* Private Macros                                                      */
/*---------------------------------------------------------------------*/
#define FLOOR(a,b)      (INT)((a)/(b))
#define CEILING(a,b)    (INT)(((a)+((b)-1))/(b))  
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))



/*---------------------------------------------------------------------*/
/* Private application                                                 */
/*---------------------------------------------------------------------*/
INT CMPLX_Ceil_Log2_64 (ULONGLONG val);
ULONG CMPLX_Real_Scale_64_Unsigned(LONGLONG op1, INT scale, INT Nbits );
ULONG CMPLX_Scale_and_Round (ULONG val,INT scale);


#if 0
#define CALC_SignalY_START_ADDR  0xfffffff 




////////////////////////////////////////////////////
// NAME:        main
//
///DESCRIPTION: The applications calculates the SNR thresholds 
//              for calculation Bit loading, and write it to file. 
//           
// INPUT:       None
//
// OUTPUT:      None
//
// RETURN:      None
////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    CalculateSignalYReport_S  CalculateSignalYReport;
    // 1. Copy all the trap's parameters and copy it to CalculateSignalYReport.SignalYArguments         

    //2. Copy memory from address CalculateSignalYReport.SignalYArguments.descriptor_addr (length = 1K) to your DRAM.
   // 3. Set this DRAM's address to   CalculateSignalYReport.SignalYArguments.descriptor_addr  
   PHY_CalculateXXLength( &CalculateSignalYReport));
   // 4. copy the lmo counter like this
   CalculateSignalYReport.SignalYResults.lmo_counter =  CalculateSignalYReport.SignalYArguments.lmo_counter;
  // 5. send to the MoCA core only the CalculateSignalYReport.SignalYResults parameters
}

#endif




////////////////////////////////////////////////////////////////////////
//  NAME        : PHY_CalculateXXLength                                                                     
////////////////////////////////////////////////////
void PHY_CalculateXXLength( CalculateSignalYReport_S*  a_pCalculateSignalYReport)
{
   UINT       i,W_K,k,n;
   ULONGLONG  sum,Mk,e0=0,e0Thresholds=0, l_cc_detect_th1 ; // should be 37 bits
   SHORT      EN_idx_start=0,l_EN_idx_start=0,cp_max_EN_idx_start=0;
   UCHAR      l_maxCC_av_len_win_ID=0,tmp;
   SHORT      first_index,last_index;
   UCHAR      cp_old_cir_th,cp_old,cp_spare_cir_th; 
   UCHAR      en_start_cir_th;
   UCHAR      cp_cir_th,cp_spare;
   SHORT      en_start;
   ULONGLONG  NUM=0,DEN=0;
   SHORT      scale_n,scale_d;
   UCHAR      COM_ir_cp;
   ULONG      COM_ir_cp_long;
   CHAR       s_tmp;
   BOOLEAN    l_crossed_TH;
   SHORT      l_CNT1;
   ULONGLONG  H_AV_Max,H_AV_Max_last;
   UCHAR      n_cc_peack;
   SHORT      l_min_cp_EN_idx_start=0;
   C_16*      a_probeIIIResults = ((C_16*)((UINT)a_pCalculateSignalYReport->SignalYArguments.descriptor_addr));
   UCHAR      index_max_cc=0;

#ifdef HT_TOGGLING
   ULONGLONG  Diff_Th,,H_AV_Max_Last_2,MFC_Th_High,HV_Max_PrePeak,MFC_Th_Low,,Diff1,Diff2,Diff3,Diff4,Diff_Bckoff;;
   BOOLEAN    Enable_MFC_Change = TRUE; //Enables changing the MFC detect threshold for limit cases so that HT can be used more often           
   UCHAR      Max_Index_Last ;
   UCHAR      Max_Index_Last_2 ;
   SHORT      MFC_Th_New,MFC_Th_New_High, MFC_Th_New_Low;
   BOOLEAN    l_Enable_HT_1, l_Enable_HT_2;
   UCHAR      Scale_NUM; 
   CHAR       Scale_DEN; 
   ULONG      MFC_Th_Delta; 
   SHORT      Delta_TH_Low=0,Delta_TH_High=0;
#endif


   a_pCalculateSignalYReport->SignalYResults.reg_pp_cc_maxf_th_scl = a_pCalculateSignalYReport->SignalYArguments.reg_pp_cc_maxf_th_scl; 


   // the following is for printing ProbeIII results as received from hardware.





   /********************************************************************* 
   Find cc offset & CP size using Max energy Window Method
   *********************************************************************/
   EN_idx_start=0;  
   W_K = a_pCalculateSignalYReport->SignalYArguments.g_cp_max;

   for(k=0;   ;k++)
   {

      // (2**-k) * CPMax > 1
      for(i=0,Mk=0;i < a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - W_K;i++)    
      {
         for(n=i,sum =0;  n <= (i+ W_K -1);n++)
         {
            sum+=((a_probeIIIResults[n].r*  a_probeIIIResults[n].r)+ (a_probeIIIResults[n].i* a_probeIIIResults[n].i));
         }

         // Find the max Ek
         if(Mk < sum)
         {
            Mk = sum;
            l_EN_idx_start =i;
         } 
      }
      if(k==0)
      {
         e0= Mk;
         // calculate the max energy that the CPU agree to decrease in order to narrow the window size.
         // We divide the result with (2 **31)  because the problem of floating point. The threshold already multiple
         // in 2**31
         e0Thresholds = (((e0 >> 4)* a_pCalculateSignalYReport->SignalYArguments.min_cp_threshold)+(1<<26) ) >>27;
         cp_max_EN_idx_start = l_EN_idx_start;
      }

      if((e0 -Mk) <= e0Thresholds)
      {
         if(((UINT)(1<<k)) >= a_pCalculateSignalYReport->SignalYArguments.g_cp_max)
         {
            EN_idx_start = l_EN_idx_start;
            break;
         }
         else
         {
            // Decrement the W_K
            W_K-= (a_pCalculateSignalYReport->SignalYArguments.g_cp_max >>(k+1));
            EN_idx_start = l_EN_idx_start;
         }
      }
      else
      {
         if(((UINT)(1<<k))>= a_pCalculateSignalYReport->SignalYArguments.g_cp_max)
         {
            W_K+= (a_pCalculateSignalYReport->SignalYArguments.g_cp_max>>(k)); 
            break;
         }
         else
         {
            W_K+= (a_pCalculateSignalYReport->SignalYArguments.g_cp_max>>(k+1)); 
         }
      }
   } // end loop 

#ifdef BC_CP_LEN_SMALLER_THEN_UC_ENTR_PATCH
   // Find the cp window start for cp min
   for(i=0,Mk=0;i < (UINT)a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - a_pCalculateSignalYReport->SignalYArguments.g_cp_min;i++)    
   {
      for(n=i,sum =0;  n <= (i+ a_pCalculateSignalYReport->SignalYArguments.g_cp_min -1);n++)
      {
         sum+=((a_probeIIIResults[n].r*  a_probeIIIResults[n].r)+ (a_probeIIIResults[n].i* a_probeIIIResults[n].i));
      }

      // Find the max Ek
      if(Mk < sum)
      {
         Mk = sum;
         l_min_cp_EN_idx_start =i;
      } 
   }
   // flip l_min_cp_EN_idx_start 
   l_min_cp_EN_idx_start = a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - l_min_cp_EN_idx_start - a_pCalculateSignalYReport->SignalYArguments.g_cp_min;
#endif




   // *********************************************************************
   // Find cc offset & CP size using CIR Tap Threshold Method
   // *********************************************************************
   first_index=last_index=-1;
   for(i=0;i < (UINT)a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales-(IR_LAV-1) ;i++)    
   {
      for(n=i,sum =0;  n <= (i+IR_LAV -1);n++)
      {
         sum+=((a_probeIIIResults[n].r*  a_probeIIIResults[n].r)+ (a_probeIIIResults[n].i* a_probeIIIResults[n].i));
      }

      if( sum >= e0Thresholds )
      {
         if(first_index == -1)
         {
            first_index =i + IR_LAV -1;  
         }
         else
         {
            last_index =i;  
         }
      }
   }

   cp_old_cir_th = last_index - first_index +1; 
   en_start_cir_th = (UCHAR)first_index;
   // *********************************************************************
   // Mimic  HW ACQUISITION Detection Point , Find Offset of AV CC Above TH
   // ********************************************************************* 
   l_crossed_TH =FALSE;
   l_CNT1 = CNT1_INIT;
   H_AV_Max =0;
   H_AV_Max_last =0;
#ifdef HT_TOGGLING
   H_AV_Max_Last_2 =0;
   Max_Index_Last =0;
   Max_Index_Last_2 =0;
#endif

   n_cc_peack =0;
   l_cc_detect_th1 = ((e0 * (a_pCalculateSignalYReport->SignalYArguments.g_pp_cc_maxf_th_scl))>>MFC_BITS);



   for(i=0;i < (UINT)a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - (a_pCalculateSignalYReport->SignalYArguments.reg_pp_cc_avg_length-1)   ;i++)    
   {
      for(n=i,sum =0;  n <= (i+a_pCalculateSignalYReport->SignalYArguments.reg_pp_cc_avg_length -1);n++)
      {
         sum+=((a_probeIIIResults[n].r*  a_probeIIIResults[n].r)+ (a_probeIIIResults[n].i* a_probeIIIResults[n].i));
      }

      if(sum > H_AV_Max)
      {
         H_AV_Max =sum;
         index_max_cc = i;
      }

	  // Find the first threshold crossing 
      if(sum >= l_cc_detect_th1)
      {
         l_maxCC_av_len_win_ID = i;         
         l_crossed_TH =TRUE;
      }
      else if(l_crossed_TH == TRUE)
      {
         if(l_CNT1)
         {
            l_CNT1--;
         }
         else
         {
            l_crossed_TH = FALSE;
            l_CNT1 = CNT1_INIT;
#ifdef HT_TOGGLING
            H_AV_Max_Last_2 =  H_AV_Max_last;
            Max_Index_Last_2 = Max_Index_Last;
            Max_Index_Last = l_maxCC_av_len_win_ID;
#endif
            H_AV_Max_last = H_AV_Max;
            H_AV_Max = 0;
            n_cc_peack++;
         }
      }
      else 
      {
         // do nothing 
      }
   }  
#ifndef HT_TOGGLING
   a_pCalculateSignalYReport->SignalYResults.enableHT =TRUE;
   if(n_cc_peack)
   {

	   // ****************************************************************************
	   // Test 1 to determine if should use HT: first peak is greater then delta1 * TH
	   // **************************************************************************** 
	   sum = H_AV_Max_last - l_cc_detect_th1;  
	   if((sum < ((l_cc_detect_th1 * DELTA_1)>> N_DELTA_BITS)) && ( n_cc_peack > 1))
	   {
		  a_pCalculateSignalYReport->SignalYResults.enableHT =FALSE;
	   }
	   // *****************************************************************************************
	   // Test 2 to determine if should use HT: before first peak no peak larger then delta2 * TH
	   // **************************************************************************************** 
	   for(i=l_maxCC_av_len_win_ID+1+CNT2_INIT ;i < (UINT)a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - a_pCalculateSignalYReport->SignalYArguments.reg_pp_cc_avg_length;i++)    
	   {
		  for(n=i,sum =0;  n <= (i+a_pCalculateSignalYReport->SignalYArguments.reg_pp_cc_avg_length -1);n++)
		  {
			 sum+=((a_probeIIIResults[n].r*  a_probeIIIResults[n].r)+ (a_probeIIIResults[n].i* a_probeIIIResults[n].i));
		  }

		  // Find the first threshold crossing 
		  if(sum >= ((l_cc_detect_th1 * DELTA_2) >> N_DELTA_BITS))
		  {
			 a_pCalculateSignalYReport->SignalYResults.enableHT =FALSE;
		  }
	   }
   }
   else // n_cc_peack ==0
   {
       a_pCalculateSignalYReport->SignalYResults.enableHT =FALSE;
   }

#else
   MFC_Th_New = -1;
   MFC_Th_New_High = -1;
   MFC_Th_New_Low = -1;
   l_Enable_HT_1 =TRUE;
   l_Enable_HT_2 =TRUE;
   if(n_cc_peack)
   {
      // ****************************************************************************
      // Test 1 to determine if should use HT: first peak is greater then delta1 * TH
      // **************************************************************************** 
      Diff_Th = H_AV_Max_last - l_cc_detect_th1;  
      MFC_Th_High = (e0 * DELTA_1) >> N_DELTA_BITS;
      if((Diff_Th < MFC_Th_High) && ( n_cc_peack > 1))
      {
         l_Enable_HT_1 =FALSE;
      }
      // *****************************************************************************************
      // Test 2 to determine if should use HT: before first peak no peak larger then delta2 * TH
      // **************************************************************************************** 
      HV_Max_PrePeak =0;
      MFC_Th_Low = l_cc_detect_th1 - ((e0 * DELTA_2) >> N_DELTA_BITS);
      for(i=l_maxCC_av_len_win_ID+1+CNT2_INIT ;i < a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - a_pCalculateSignalYReport->SignalYArguments.reg_pp_cc_avg_length;i++)    
      {
         for(n=i,sum =0;  n <= (i+a_pCalculateSignalYReport->SignalYArguments.reg_pp_cc_avg_length -1);n++)
         {
            sum+=((a_probeIIIResults[n].r*  a_probeIIIResults[n].r)+ (a_probeIIIResults[n].i* a_probeIIIResults[n].i));
         }

         // Find the first threshold crossing 
         if(sum >= MFC_Th_Low)
         {
            l_Enable_HT_2 =FALSE;
         }
         if(sum > HV_Max_PrePeak)
         {
            HV_Max_PrePeak = sum;
         }
      }
      a_pCalculateSignalYReport->SignalYResults.enableHT = l_Enable_HT_1 & l_Enable_HT_2;
      // *********************************************************************
      // Check if we can change MFC_Th in robust way so that to enable HT.
      // ********************************************************************* 
      if((a_pCalculateSignalYReport->SignalYResults.enableHT == FALSE) && (Enable_MFC_Change == TRUE) && (a_pCalculateSignalYReport->SignalYArguments.min_cp_threshold < HT_SNR_TH))
      {

         //Calc by how much pre peak surpassed TH_LOW
         Diff1 =(HV_Max_PrePeak > MFC_Th_Low)?(HV_Max_PrePeak - MFC_Th_Low):( MFC_Th_Low - HV_Max_PrePeak);


         //Calc by how much first peak surpassed TH_HIGH
         Diff2 =(Diff_Th > MFC_Th_High)?(Diff_Th - MFC_Th_High):( MFC_Th_High - Diff_Th);


         //Calc by how much second peak surpassed TH_HIGH
         Diff3 = (H_AV_Max_Last_2 - MFC_Th_High);

         //Calc by how first peak surpassed TH_HIGH
         Diff4 = (H_AV_Max_last - MFC_Th_Low);


         if( (l_Enable_HT_1==0) && (l_Enable_HT_2==1) ) //First is below TH_HIGH & no Pre Peaks
         {

            //Option #1 Reduce MFC Th so that Peak crosses TH_HIGH
            //Check that reducing MFC_TH does not cause pre peak detection

            if(Diff1 >  ((Diff2*DELTA_5)>> N_DELTA_BITS) )
            {
               //Can Safely refuse MFC Th by |Diff2|*delta_4 without pre peak crossing TH_LOW
               Diff_Bckoff = (Diff2*DELTA_4)>> N_DELTA_BITS;


               //Fix Point Computation for ceil(Diff_Bckoff/e0*2^MFC_Bits)
               Scale_NUM = 61 - CMPLX_Ceil_Log2_64(Diff_Bckoff);
               Scale_DEN = 30 - CMPLX_Ceil_Log2_64(e0);
               MFC_Th_Delta = (ULONG)((ULONGLONG)(Diff_Bckoff <<Scale_NUM)/((ULONG)(e0<< Scale_DEN)));
               if((Scale_NUM - Scale_DEN - MFC_BITS) > 31)
                  MFC_Th_Delta = 0;
               else
                  CMPLX_Real_Scale_32(MFC_Th_Delta, (-(Scale_NUM - Scale_DEN - MFC_BITS)) ,&MFC_Th_Delta);
               MFC_Th_Delta++; 

               //Compute New Threshold
               //MFC_Th_New_Low = acquisition_parameters.MFC_Th - (Diff_Bckoff/e0);
               MFC_Th_New_Low = (SHORT)(a_pCalculateSignalYReport->SignalYResults.reg_pp_cc_maxf_th_scl - MFC_Th_Delta);


               //Compute Margin from Minimal Allowed Threshold
               Delta_TH_Low = MFC_Th_New_Low - MFC_TH_MIN;


               if(MFC_Th_New_Low < MFC_TH_MIN)

                  MFC_Th_New_Low= -1;
            } // (Diff1 >  ((Diff2*DELTA_5)>> N_DELTA_BITS)

            //Option #2 Increase MFC Th so that Peak does not crosses TH_LOW but still Last_2 peak crosses TH_HIGH

            //Check that increasing MFC_TH does not cause miss detection

            if( (Diff3>0) && (Diff3 > ( (Diff4 * DELTA_5) >> N_DELTA_BITS)) )
            {

               //Can Safley increase MFC Th by |Diff4|*delta_4 without pre peak crossing TH_LOW

               //Can Safley refuce MFC Th by |Diff2|*delta_4 without pre peak crossing TH_LOW

               Diff_Bckoff = (Diff4*DELTA_4)>> N_DELTA_BITS;


               //Fix Point Computation for ceil(Diff_Bckoff/e0*2^MFC_BITS)
               Scale_NUM = 61 - CMPLX_Ceil_Log2_64(Diff_Bckoff);
               Scale_DEN = 30 - CMPLX_Ceil_Log2_64(e0);
               MFC_Th_Delta = (ULONG)((ULONGLONG)(Diff_Bckoff <<Scale_NUM)/((ULONG)(e0<< Scale_DEN)));
               if((Scale_NUM - Scale_DEN - MFC_BITS) > 31)
                  MFC_Th_Delta = 0;
               else
                  CMPLX_Real_Scale_32(MFC_Th_Delta,(-(Scale_NUM - Scale_DEN - MFC_BITS)),&MFC_Th_Delta);
               MFC_Th_Delta++; 
               MFC_Th_New_High = (SHORT)(a_pCalculateSignalYReport->SignalYResults.reg_pp_cc_maxf_th_scl + MFC_Th_Delta);

               //MFC_Th_New_High = acquisition_parameters.MFC_Th + (Diff_Bckoff/e0);
               Delta_TH_High = MFC_TH_MAX - MFC_Th_New_High;
               if(MFC_Th_New_High > MFC_TH_MAX)
               {
                  MFC_Th_New_High= -1;
               }
            }


            //Choose New Threshold
            if((MFC_Th_New_High!=-1)&&(MFC_Th_New_Low!=-1))
            {
               //Both Legal
               if(Delta_TH_High > Delta_TH_Low) //Choose TH furthest away from limit
               {
                  MFC_Th_New = MFC_Th_New_High;
                  l_maxCC_av_len_win_ID = Max_Index_Last_2;
               }
               else 
               {
                  MFC_Th_New = MFC_Th_New_Low;
               }

            }
            else if(MFC_Th_New_High!=-1)
            {
               MFC_Th_New = MFC_Th_New_High;
               l_maxCC_av_len_win_ID = Max_Index_Last_2;
            }
            else if(MFC_Th_New_Low!=-1)
            {
               MFC_Th_New = MFC_Th_New_Low;
            }
            else
            {
               //Cross Correlation MFC_TH WARNING !! Multiple Channel Peaks and First CC Peak below TH_HIGH , HighThrouput Disabled. Not Possible to Compute New MFC_TH within Limits.']);
            }

            if((MFC_Th_New_High!=-1)||(MFC_Th_New_Low!=-1))
            {
               a_pCalculateSignalYReport->SignalYResults.enableHT =TRUE;
               a_pCalculateSignalYReport->SignalYResults.reg_pp_cc_maxf_th_scl = MFC_Th_New;
               //Cross Correlation MFC_TH WARNING !! Multiple Channel Peaks and First CC Peak below TH_HIGH , New MFC_TH Computed.']);
            }
         } // (l_Enable_HT_1==0) && (l_Enable_HT_2==1)

         else if((l_Enable_HT_1==TRUE)&&(l_Enable_HT_2==FALSE))//Pre Peak is above TH_LOW (But Below MFC_TH of course)
         {
            //Increase MFC_TH so that Pre Peak does not cross TH_LOW
            //Check that increasing MFC_TH does not cause peak not to cross TH_HIGH
            //Check that Increasing MFC_TH does not cause peak miss detection

            if(Diff2 > ((Diff1*DELTA_5) >> N_DELTA_BITS) )
            {
               //Can Safely Increase MFC Th by |Diff1|*delta_4 without peak miss detection
               Diff_Bckoff = (Diff1*DELTA_4)>> N_DELTA_BITS;


               //Fix Point Computation for ceil(Diff_Bckoff/e0*2^MFC_BITS)
               Scale_NUM = 61 - CMPLX_Ceil_Log2_64(Diff_Bckoff);
               Scale_DEN = 30 - CMPLX_Ceil_Log2_64(e0);
               MFC_Th_Delta = (ULONG)((ULONGLONG)(Diff_Bckoff <<Scale_NUM)/((ULONG)(e0<< Scale_DEN)));
               if((Scale_NUM - Scale_DEN - MFC_BITS) > 31)
                  MFC_Th_Delta = 0;
               else
                  CMPLX_Real_Scale_32(MFC_Th_Delta,(-(Scale_NUM - Scale_DEN - MFC_BITS)),&MFC_Th_Delta);
               MFC_Th_Delta++; 
               MFC_Th_New = (SHORT)(a_pCalculateSignalYReport->SignalYResults.reg_pp_cc_maxf_th_scl + MFC_Th_Delta);

               //MFC_Th_New = acquisition_parameters.MFC_Th + (Diff_Bckoff/e0);
               if(MFC_Th_New > MFC_TH_MAX)
               {
                  MFC_Th_New= -1;
                  //Cross Correlation MFC_TH WARNING !! Pre Peak is above TH_LOW , HighThrouput Disabled. Not Possible to Compute New MFC_TH within Limits.']);
               }		
               else
               {
                  a_pCalculateSignalYReport->SignalYResults.enableHT =TRUE;
                  a_pCalculateSignalYReport->SignalYResults.reg_pp_cc_maxf_th_scl = MFC_Th_New;
                  //Cross Correlation MFC_TH WARNING !! Pre Peak is above TH_LOW New MFC_TH Computed.']);
               }
            } // (Diff2 > ((Diff1*DELTA_5) >> N_DELTA_BITS) )

            //elseif((l_Enable_HT_1==0)&&(l_Enable_HT_2==0)) //Max is below TH_HIGH & //Pre Peak is above TH_LOW
            //Not possible to change TH to Enable HT

         } //((l_Enable_HT_1==TRUE)&&(l_Enable_HT_2==FALSE))
      } // ((a_pCalculateSignalYReport->SignalYResults.enableHT == FALSE) && (Enable_MFC_Change == TRUE) && (a_pCalculateSignalYReport->SignalYArguments.min_cp_threshold < HT_SNR_TH))
   } // (n_cc_peack)
   else
   {
      //ERROR - No CC MFC_Th crossing, Cross Correlation will miss detect during acqusition !!! 
      a_pCalculateSignalYReport->SignalYResults.enableHT =FALSE;
      //Changing MFC Threshold Back To default Value. 
      a_pCalculateSignalYReport->SignalYResults.reg_pp_cc_maxf_th_scl = a_pCalculateSignalYReport->SignalYArguments.g_pp_cc_maxf_th_scl;  
   }

#endif

   if((cp_old_cir_th > a_pCalculateSignalYReport->SignalYArguments.g_cp_max )  && (!a_pCalculateSignalYReport->SignalYArguments.firstTime2_calc_cp))
   {
      a_pCalculateSignalYReport->SignalYResults.cp_overflow = TRUE; 
   }
   else
   {
      a_pCalculateSignalYReport->SignalYResults.cp_overflow = FALSE; 
   }


   // *********************************************************************
   // Select CP size the smaller of CIR_TH & Energy window, select en_start 
   // ********************************************************************* 

   // Energy Window CP: Add max (20%,10 samples) margin to cp but not more then 20 samples
   cp_old = W_K;
   tmp=CEILING ((W_K * CP_MARGIN_ENERGY_WIN),100);
   cp_spare = MIN(tmp,CP_MAX_MARGIN_ENERGY_WIN);
#ifdef PATCH_ADD_LEN_2_CP 
   cp_spare++;
#endif
   cp_spare=(cp_spare & 0x1)? cp_spare+1:cp_spare;
   a_pCalculateSignalYReport->SignalYResults.cp = MIN((W_K+ cp_spare),a_pCalculateSignalYReport->SignalYArguments.g_cp_max);

   // When the cp is smaller then the cp min, then add more spare.
   if ((s_tmp=(a_pCalculateSignalYReport->SignalYArguments.g_cp_min- a_pCalculateSignalYReport->SignalYResults.cp)) > 0)
   {
      a_pCalculateSignalYReport->SignalYResults.cp = a_pCalculateSignalYReport->SignalYArguments.g_cp_min;
      cp_spare+=s_tmp; 
   }

   // CIR TH CP: Add max (30%,10 samples) margin to cp but not more then 20 samples
   tmp=CEILING ((cp_old_cir_th * CP_MARGIN_CIR_TH),100);
#ifdef HT_TOGGLING  
   if(n_cc_peack <=1)
   {
      cp_spare_cir_th = MAX(tmp, CP_MIN_MARGIN_CIR_TH_1);
   }
   else
   {
      cp_spare_cir_th = MAX(tmp, CP_MIN_MARGIN_CIR_TH_2);
   }
#else
   cp_spare_cir_th = MAX(tmp,CP_MIN_MARGIN_CIR_TH);
#endif
   cp_spare_cir_th = MIN(cp_spare_cir_th,CP_MAX_MARGIN_CIR_TH);
#ifdef PATCH_ADD_LEN_2_CP
   cp_spare_cir_th++;
#endif
   cp_spare_cir_th=(cp_spare_cir_th& 0x1)? cp_spare_cir_th+1:cp_spare_cir_th;
   cp_cir_th= MIN((cp_old_cir_th+ cp_spare_cir_th),(INT)a_pCalculateSignalYReport->SignalYArguments.g_cp_max);

   // When the cp is smaller then the cp min, then add more spare.
   if ((s_tmp=(a_pCalculateSignalYReport->SignalYArguments.g_cp_min- cp_cir_th)) > 0)
   {
      cp_cir_th= a_pCalculateSignalYReport->SignalYArguments.g_cp_min;
      cp_spare_cir_th+=s_tmp; 
   }

   // Flip max index
   l_maxCC_av_len_win_ID = a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales -1 - l_maxCC_av_len_win_ID ;

   // Select the smaller cp size.
   //if(a_pCalculateSignalYReport->SignalYArguments.firstTime2_calc_cp)
   if((a_pCalculateSignalYReport->SignalYResults.cp <= cp_cir_th))
   {
      // max energy smaller
      en_start = a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - EN_idx_start - MIN((cp_old + (cp_spare >> 1)), (INT)a_pCalculateSignalYReport->SignalYArguments.g_cp_max);
   }
   else
   {
      // cir th cp is smaller
      en_start = a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - en_start_cir_th - MIN((cp_old_cir_th + (cp_spare_cir_th >> 1)), a_pCalculateSignalYReport->SignalYArguments.g_cp_max); 
      a_pCalculateSignalYReport->SignalYResults.cp = cp_cir_th;
      cp_old = cp_old_cir_th;
   }



   // Limit cp window 
   if( (en_start + a_pCalculateSignalYReport->SignalYResults.cp) >= a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales)
   {
      en_start -= (en_start + a_pCalculateSignalYReport->SignalYResults.cp - a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales );
   }   

   // Limit max cp window 
   cp_max_EN_idx_start = a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales - cp_max_EN_idx_start - a_pCalculateSignalYReport->SignalYArguments.g_cp_max ; 

   // When CP = CP max reset en_start to match max cp. 
   if(a_pCalculateSignalYReport->SignalYResults.cp == a_pCalculateSignalYReport->SignalYArguments.g_cp_max)
   {
      en_start = cp_max_EN_idx_start;
   }

   if(cp_max_EN_idx_start + a_pCalculateSignalYReport->SignalYArguments.g_cp_max >= a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales)
   {
      cp_max_EN_idx_start-= (cp_max_EN_idx_start + a_pCalculateSignalYReport->SignalYArguments.g_cp_max - a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales);
   }

   cp_max_EN_idx_start = (cp_max_EN_idx_start < 0)? 0: cp_max_EN_idx_start; 
   en_start = (en_start < 0)? 0: en_start; 

   // *********************************************************************
   // Compute CC cp_offset & max_cc cp_offset
   // *********************************************************************
   if(en_start > l_maxCC_av_len_win_ID)
   {
      //LOG_E(PHY_TRACE,LOG_E_CODE__PHY_C__13504 , "Invalid values   \n\r\n");       
   }
   
   if(n_cc_peack ==0)
   {
      l_maxCC_av_len_win_ID = index_max_cc; 
      //HOST_LOG_I(MLME_MICA_TRACE, MOCA_2_HOST_STR_ID(MOCA_PHY_STR_LAYER, 28), HOST_LOG_0_PARAM);
   }

   a_pCalculateSignalYReport->SignalYResults.cc_offset = l_maxCC_av_len_win_ID - en_start;
   if(cp_max_EN_idx_start > l_maxCC_av_len_win_ID)
   {
      //LOG_E(PHY_TRACE,LOG_E_CODE__PHY_C__13505 , "Invalid values   \n\r\n");       
   }



   // *********************************************************************
   // Find center of mass for AC offset
   // *********************************************************************
   for(n=(UINT)a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales -en_start -1,i=0; n > ((UINT)a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales -en_start -1-a_pCalculateSignalYReport->SignalYResults.cp);n--,i++)
   {
      sum=((a_probeIIIResults[n].r*  a_probeIIIResults[n].r)+ (a_probeIIIResults[n].i* a_probeIIIResults[n].i));
      NUM+= (sum*i);
      DEN+=sum;
   }
   scale_n = 31 - CMPLX_Ceil_Log2_64 (NUM);
   scale_d = 15 - CMPLX_Ceil_Log2_64 (DEN);
   COM_ir_cp_long =  ((ULONG) CMPLX_Real_Scale_64_Unsigned (NUM, scale_n, 31)) / ((USHORT)CMPLX_Real_Scale_64_Unsigned (DEN, scale_d, 15));
   COM_ir_cp = (UCHAR)CMPLX_Scale_and_Round (COM_ir_cp_long,scale_d - scale_n);

   a_pCalculateSignalYReport->SignalYResults.ac_offset =  COM_ir_cp;
   if(a_pCalculateSignalYReport->SignalYResults.ac_offset > OFFSET_LIMIT)
   {
      en_start += a_pCalculateSignalYReport->SignalYResults.ac_offset - OFFSET_LIMIT; 
      // CP window start too far from COM
      a_pCalculateSignalYReport->SignalYResults.ac_offset = OFFSET_LIMIT;
      if(en_start + a_pCalculateSignalYReport->SignalYResults.cp >= (SHORT)a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales)
      {
         a_pCalculateSignalYReport->SignalYResults.cp -= (en_start + a_pCalculateSignalYReport->SignalYResults.cp - a_pCalculateSignalYReport->SignalYArguments.numberOfSymbales);
      } 
   }

   a_pCalculateSignalYReport->SignalYResults.cp=(a_pCalculateSignalYReport->SignalYResults.cp & 0x1)? a_pCalculateSignalYReport->SignalYResults.cp+1:a_pCalculateSignalYReport->SignalYResults.cp;

#ifdef BC_CP_LEN_SMALLER_THEN_UC_ENTR_PATCH
   a_pCalculateSignalYReport->SignalYResults.cp_min_ac_delay_offset = MAX(0,((SHORT)(COM_ir_cp - (l_min_cp_EN_idx_start - en_start))));
#endif
   
}


////////////////////////////////////////////////////////////////////////
// NAME      : CMPLX_Ceil_Log2_64
// RETURN:     None   
//////////////////////////////////////////////////////////////////////////
INT CMPLX_Ceil_Log2_64 (ULONGLONG val)
{
   INT        msb = 0;
   ULONGLONG  l_val= val;

   while(l_val)
   {
      l_val >>= 1;
      msb++;
   }
   if((ULONGLONG)(1<<(msb -1)) == val)
      return msb-1;
   return msb;
}

////////////////////////////////////////////////////////////////////////
// NAME      : CMPLX_Real_Scale_64
// RETURN:     None   
//////////////////////////////////////////////////////////////////////////
ULONG CMPLX_Real_Scale_64_Unsigned(LONGLONG op1, INT scale, INT Nbits )
{
   ULONGLONG dTmp = (ULONGLONG)(1 << Nbits);
   ULONG      op2;
   if (scale >=0)
   {
      op2 = (LONG)op1 << scale;    
      if (op2 >= ((ULONG)(1 << Nbits)) )
      {
         op2 = (1<< Nbits)-1;
      }
   }
   else
   {
      op1 = op1 >> (-scale);
      if ((ULONGLONG)op1 >= dTmp)
      {
         op2 = (ULONG)(dTmp-1);
      }
      else
      {
         op2 = (ULONG)op1;
      }
   }
   return op2;
}

////////////////////////////////////////////////////////////////////////
// NAME      : CMPLX_Scale_and_Round
// RETURN:     None   
//////////////////////////////////////////////////////////////////////////
ULONG CMPLX_Scale_and_Round (ULONG val,INT scale)
{
   if( scale >= 0)
   {
      return (val <<scale);
   }
   else
   {
      return((val + (1 << (-scale -1))) >> -scale);
   }
}
