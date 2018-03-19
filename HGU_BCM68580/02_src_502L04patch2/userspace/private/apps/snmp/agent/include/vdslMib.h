#ifndef	_CPE_DSL_MIB_
#define	_CPE_DSL_MIB_

/* required include files (IMPORTS) */
#include	"snmpv2Tc.h"
#include	"snmpv2Smi.h"


#define VDSL_TINY_LEN   64


#define O_vdslMib   1,3,6,1,2,1,10,9,7

#define I_vdslLine  1
#define O_vdslLine  1,3,6,1,2,1,10,97,1

#define I_vdslObject   1
#define O_vdslObject   1,3,6,1,2,1,10,97,1,1

#define I_vdslPhysTable   2
#define O_vdslPhysTable  1,3,6,1,2,1,10,97,1,1,2    

#define I_vdslPhysEntry   1
#define O_vdslPhysEntry  1,3,6,1,2,1,10,97,1,1,2,1

#define I_vdslPhysSide   1
#define O_vdslPhysSide  1,3,6,1,2,1,10,97,1,1,2,1,1

#define I_vdslPhysInvSerialNumber   2
#define O_vdslPhysInvSerialNumber  1,3,6,1,2,1,10,97,1,1,2,1,2

#define I_vdslPhysInvVendorID   3
#define O_vdslPhysInvVendorID  1,3,6,1,2,1,10,97,1,1,2,1,3

#define I_vdslPhysInvVersionNumbe  4
#define O_vdslPhysInvVersionNumbe  1,3,6,1,2,1,10,97,1,1,2,1,4

#define I_vdslPhysCurrSnrMgn   5
#define O_vdslPhysCurrSnrMgn  1,3,6,1,2,1,10,97,1,1,2,1,5

#define I_vdslPhysCurrAtn   6
#define O_vdslPhysCurrAtn  1,3,6,1,2,1,10,97,1,1,2,1,6

#define I_vdslPhysCurrStatus   7
#define O_vdslPhysCurrStatus  1,3,6,1,2,1,10,97,1,1,2,1,7

#define I_vdslPhysCurrOutputPwr  8
#define O_vdslPhysCurrOutputPwr  1,3,6,1,2,1,10,97,1,1,2,1,8


#define I_vdslPhysCurrAttainableRate  9
#define O_vdslPhysCurrAttainableRate  1,3,6,1,2,1,10,97,1,1,2,1,9

#define I_vdslPhysCurrLineRate  10
#define O_vdslPhysCurrLineRate  1,3,6,1,2,1,10,97,1,1,2,1,10

#define I_vdslPerfDataTable   4
#define O_vdslPerfDataTable  1,3,6,1,2,1,10,97,1,1,4  


#define I_vdslPerfDataEntry   1
#define O_vdslPerfDataEntry  1,3,6,1,2,1,10,97,1,1,4 ,1


#define I_vdslPerfDataValidIntervals   1
#define O_vdslPerfDataValidIntervals  1,3,6,1,2,1,10,97,1,1,4 ,1,1

#define I_vdslPerfDataInvalidIntervals  2
#define O_vdslPerfDataInvalidIntervals  1,3,6,1,2,1,10,97,1,1,4 ,1,2

#define I_vdslPerfDataLofs  3
#define O_vdslPerfDataLofs  1,3,6,1,2,1,10,97,1,1,4 ,1,3

#define I_vdslPerfDataLoss  4
#define O_vdslPerfDataLoss  1,3,6,1,2,1,10,97,1,1,4 ,1,4

#define I_vdslPerfDataLprs  5
#define O_vdslPerfDataLprs  1,3,6,1,2,1,10,97,1,1,4 ,1,5

#define I_vdslPerfDataLols  6
#define O_vdslPerfDataLols  1,3,6,1,2,1,10,97,1,1,4 ,1,6

#define I_vdslPerfDataESs  7
#define O_vdslPerfDataESs  1,3,6,1,2,1,10,97,1,1,4 ,1,7

#define I_vdslPerfDataSESs  8
#define O_vdslPerfDataSESs  1,3,6,1,2,1,10,97,1,1,4 ,1,8


#define I_vdslPerfDataUASs 9
#define O_vdslPerfDataUASs  1,3,6,1,2,1,10,97,1,1,4 ,1,9

#define I_vdslPerfDataInits  10
#define O_vdslPerfDataInits  1,3,6,1,2,1,10,97,1,1,4 ,1,10




#define I_vdslLineConfProfileTable   11
#define O_vdslLineConfProfileTable   1,3,6,1,2,1,10,97,1,1,11  


#define I_vdslLineConfProfileEntry   1
#define O_vdslLineConfProfileEntry   1,3,6,1,2,1,10,97,1,1,11,1  

#define I_vdslLineConfDownMaxSnrMgn   6
#define O_vdslLineConfDownMaxSnrMgn   1,3,6,1,2,1,10,97,1,1,11,1,6

#define I_vdslLineConfDownMinSnrMgn  7
#define O_vdslLineConfDownMinSnrMgn  1,3,6,1,2,1,10,97,1,1,11,1,7 

#define I_vdslLineConfDownTargetSnrMgn   8
#define O_vdslLineConfDownTargetSnrMgn   1,3,6,1,2,1,10,97,1,1,11,1,8

#define I_vdslLineConfUpMaxSnrMgn  9
#define O_vdslLineConfUpMaxSnrMgn   1,3,6,1,2,1,10,97,1,1,11,1,9

#define I_vdslLineConfUpMinSnrMgn   10
#define O_vdslLineConfUpMinSnrMgn   1,3,6,1,2,1,10,97,1,1,11,1,10

#define I_vdslLineConfUpTargetSnrMgn   11
#define O_vdslLineConfUpTargetSnrMgn   1,3,6,1,2,1,10,97,1,1,11,1,11

#define I_vdslLineConfDownMaxInterDelay   22
#define O_vdslLineConfDownMaxInterDelay   1,3,6,1,2,1,10,97,1,1,11,1,22 

#define I_vdslLineConfUpMaxInterDelay   23
#define O_vdslLineConfUpMaxInterDelay   1,3,6,1,2,1,10,97,1,1,11,1,23 
/* Put here additional MIB specific include definitions */

typedef struct vdslPhys
{
   int      side;
   char   serialNum[VDSL_TINY_LEN];
   char   verdonID[VDSL_TINY_LEN];
   char   versionNum[VDSL_TINY_LEN];
   int      currSnrMgn;
   long    currAtn;
   int      state;
   int      outPower;
   long   attRate;
   long   currRate;
   struct vdslPhys *next;
   
}  VDSL_PHYS, *pVDSL_PHYS;


typedef struct vdslPhysList 
{
  pVDSL_PHYS pIf;
  pVDSL_PHYS pIfTail;
} VDSL_PHYS_LIST, *pVDSL_PHYS_LIST;

typedef struct vdslPerfData
{
    int    index;
    int    validIntervals;
    int    invalidIntervals;
    long  dataLofs;
    long  dataLoss;
    long  dataLprs;
    long  dataLols;
    long  dataESs;
    long  dataSESs;
    long  dataUASs;
    long  dataInits;
    struct vdslPerfData *next; 
}  VDSL_PERFDATA, *pVDSL_PERFDATA;


typedef struct vdslPerfDataList 
{
  pVDSL_PERFDATA  pIf;
  pVDSL_PERFDATA  pIfTail;
} VDSL_PERFDATA_LIST, *pVDSL_PERFDATA_LIST;

typedef struct vdslConf
{
    int    index;
    long  maxDownSnr;
    long  minDownSnr;
    long  targetDownSnr;
    long  maxUpSnr;
    long  minUpSnr;
    long  targetUpSnr;
    long  maxDownInv;
    long  maxUpInv;
    struct vdslConf *next; 
}  VDSL_CONF, *pVDSL_CONF;


typedef struct vdslConfList 
{
   pVDSL_CONF  pIf;
   pVDSL_CONF  pIfTail;
} VDSL_CONF_LIST, *pVDSL_CONF_LIST;


void init_vdsl_mib(void);
void register_subtrees_of_vdsl_mib(void);
#endif	/* _CPE_DSL_MIB_ */
