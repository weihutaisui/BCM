#ifndef __CALCSignalY_H__
#define __CALCSignalY_H__

#include <stdint.h>


//Type definitions
typedef int                   INT;
typedef unsigned int          UINT;

typedef int32_t          LONG;
typedef uint32_t         ULONG;
typedef uint64_t         ULONGLONG;
typedef int64_t          LONGLONG;


typedef int16_t                 SHORT;
typedef uint16_t        USHORT;
typedef unsigned char         UCHAR;
typedef char                  CHAR;
typedef unsigned char         BOOLEAN;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif


//16_bit Complex 
typedef struct _C_16_
{
   SHORT  r;
   SHORT  i;
}C_16;

// Definition of structure
typedef struct _SignalYArguments_S
{
   UINT     lmo_counter;
   UINT     descriptor_addr;  
   USHORT   numberOfSymbales;   
   UINT     min_cp_threshold;         // last cp threshold (noise , calculated by Probe I)
   BOOLEAN  firstTime2_calc_cp;
   UCHAR    reg_pp_cc_maxf_th_scl;    // pp_cc_maxf_th_scl set in the PHY register
   UCHAR    g_pp_cc_maxf_th_scl;      // pp_cc_maxf_th_scl default value.      
   UCHAR    g_cp_min;                    
   UCHAR    g_cp_max;
   UCHAR    reg_pp_cc_avg_length;      // pp_cc_avg_lengthset in the PHY register.
}__attribute__((packed,aligned(4))) SignalYArguments_S; 


typedef struct _SignalYResults_S
{
   UINT   lmo_counter;
   UCHAR  cp;
   UCHAR  cc_offset;
   UCHAR  ac_offset;
   UCHAR  cp_min_ac_delay_offset;
   UCHAR  enableHT;
   UCHAR  cp_overflow;
   UCHAR  reg_pp_cc_maxf_th_scl;
} __attribute__((packed,aligned(4))) SignalYResults_S;


typedef struct _CalculateSignalYReport_S
{
   SignalYArguments_S       SignalYArguments;
   SignalYResults_S         SignalYResults;
}CalculateSignalYReport_S;




// Function
void PHY_CalculateXXLength( CalculateSignalYReport_S*  a_pCalculateSignalYReport);

#endif
