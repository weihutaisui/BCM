
#ifndef ADSL_MIB_HELPER_H
#define ADSL_MIB_HELPER_H

#include <AdslMibDef.h>
#include <asn1.h>
#include <agtMib.h>

#define ADSL_MIB_IFNAME                  "dsl0"
#define ADSL_MIB_INV_SERIAL_NUMBER       "00000000000000000000000000000001"
#define ADSL_MIB_INV_VERSION_NUMBER      "Broadcom DSL Version "
#define ADSL_MIB_VENDOR_ID_G_DMT         "B500BDCM0000"
#define ADSL_MIB_VENDOR_ID_T1_413        "544D"
#define ADSL_MIB_CHAN_INTERVAL_NUM_MIN   1
#define ADSL_MIB_CHAN_INTERVAL_NUM_MAX   96
#define ADSL_MIB_CONFIG_PROFILE_NAME     ""
#define ADSL_MIB_ALARM_ATUC_INIT_FAIL_ENABLE  1
#define ADSL_MIB_ALARM_ATUC_INIT_FAIL_DISABLE 2
#define ADSL_MIB_ALARM_ROW_ACTIVE             1
#define ADSL_MIB_ALARM_ROW_NOT_IN_SERVICE     2
#define ADSL_MIB_ALARM_ROW_NOT_READY          3

/* ONLY THESE TABLES are supported by this ATUR-agent */
#define DO_ADSLATURPHYSTABLE
#define DO_ADSLATURCHANTABLE
#define DO_ADSLATURPERFDATATABLE
#define DO_ADSLATURCHANPERFDATATABLE
#define DO_ADSLATURINTERVALTABLE
#define DO_ADSLATURCHANINTERVALTABLE
#define DO_ADSLLINETABLE
#define DO_ADSLATUCPHYSTABLE
#define DO_ADSLLINEALARMCONFPROFILETABLE

#undef DO_ADSLLINECONFPROFILETABLE
#undef DO_ADSLATUCPERFDATATABLE
#undef DO_ADSLATUCINTERVALTABLE
#undef DO_ADSLATUCCHANPERFDATATABLE
#undef DO_ADSLATUCCHANINTERVALTABLE
#undef DO_ADSLATUCCHANTABLE

#if 0
#define ADSLMIB_DEBUG(fmt)        printf fmt;printf("\n");
#else
#define ADSLMIB_DEBUG(fmt)
#endif

/* current status bit definitions */
#define ADSL_MIB_CURR_STATUS_NO_DEFECT                0x1
#define ADSL_MIB_CURR_STATUS_LOSS_OF_FRAMING          0x2
#define ADSL_MIB_CURR_STATUS_LOSS_OF_SIGNAL           0x4
#define ADSL_MIB_CURR_STATUS_LOSS_OF_POWER            0x8
#define ADSL_MIB_CURR_STATUS_LOSS_OF_SIGNAL_QUALITY   0x10

/*
 * column number definitions for table adslAturPhysTable 
 */
typedef struct adslMib_adslAturPhysTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned char   adslAturInvSerialNumber[32];
    unsigned char   adslAturInvVendorID[16];
    unsigned char   adslAturInvVersionNumber[16];
    int             adslAturCurrSnrMgn;
    unsigned long   adslAturCurrAtn;
    int             adslAturCurrStatus;
    int             adslAturCurrOutputPwr;
    unsigned long   adslAturCurrAttainableRate;

    struct adslMib_adslAturPhysTable *next;     /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATURPHYSTABLE, *pADSL_MIB_ADSLATURPHYSTABLE;
/*
 * column number definitions for table adslLineConfProfileTable 
 */
typedef struct adslMib_adslLineConfProfileTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned char   adslLineConfProfileName[32];
    int             adslAtucConfRateMode;
    int             adslAtucConfRateChanRatio;
    int             adslAtucConfTargetSnrMgn;
    int             adslAtucConfMaxSnrMgn;
    int             adslAtucConfMinSnrMgn;
    int             adslAtucConfDownshiftSnrMgn;
    int             adslAtucConfUpshiftSnrMgn;
    int             adslAtucConfMinUpshiftTime;
    int             adslAtucConfMinDownshiftTime;
    unsigned int    adslAtucChanConfFastMinTxRate;
    unsigned int    adslAtucChanConfInterleaveMinTxRate;
    unsigned int    adslAtucChanConfFastMaxTxRate;
    unsigned int    adslAtucChanConfInterleaveMaxTxRate;
    int             adslAtucChanConfMaxInterleaveDelay;
    int             adslAturConfRateMode;
    int             adslAturConfRateChanRatio;
    int             adslAturConfTargetSnrMgn;
    int             adslAturConfMaxSnrMgn;
    int             adslAturConfMinSnrMgn;
    int             adslAturConfDownshiftSnrMgn;
    int             adslAturConfUpshiftSnrMgn;
    int             adslAturConfMinUpshiftTime;
    int             adslAturConfMinDownshiftTime;
    unsigned int    adslAturChanConfFastMinTxRate;
    unsigned int    adslAturChanConfInterleaveMinTxRate;
    unsigned int    adslAturChanConfFastMaxTxRate;
    unsigned int    adslAturChanConfInterleaveMaxTxRate;
    int             adslAturChanConfMaxInterleaveDelay;
    int             adslLineConfProfileRowStatus;

    struct adslMib_adslLineConfProfileTable *next;      /* Pointer to the next item in the table (linked list).
                                                         * ** For a single ADSL port, this points to NULL
                                                         * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLLINECONFPROFILETABLE, *pADSL_MIB_ADSLLINECONFPROFILETABLE;
/*
 * column number definitions for table adslAturChanTable 
 */
typedef struct adslMib_adslAturChanTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned long   adslAturChanInterleaveDelay;
    unsigned long   adslAturChanCurrTxRate;
    unsigned long   adslAturChanPrevTxRate;
    unsigned long   adslAturChanCrcBlockLength;

    struct adslMib_adslAturChanTable *next;     /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATURCHANTABLE, *pADSL_MIB_ADSLATURCHANTABLE;
/*
 * column number definitions for table adslAtucPerfDataTable 
 */
typedef struct adslMib_adslAtucPerfDataTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned long   adslAtucPerfLofs;
    unsigned long   adslAtucPerfLoss;
    unsigned long   adslAtucPerfLols;
    unsigned long   adslAtucPerfLprs;
    unsigned long   adslAtucPerfESs;
    unsigned long   adslAtucPerfInits;
    int             adslAtucPerfValidIntervals;
    int             adslAtucPerfInvalidIntervals;
    unsigned long   adslAtucPerfCurr15MinTimeElapsed;
    unsigned long   adslAtucPerfCurr15MinLofs;
    unsigned long   adslAtucPerfCurr15MinLoss;
    unsigned long   adslAtucPerfCurr15MinLols;
    unsigned long   adslAtucPerfCurr15MinLprs;
    unsigned long   adslAtucPerfCurr15MinESs;
    unsigned long   adslAtucPerfCurr15MinInits;
    unsigned long   adslAtucPerfCurr1DayTimeElapsed;
    unsigned long   adslAtucPerfCurr1DayLofs;
    unsigned long   adslAtucPerfCurr1DayLoss;
    unsigned long   adslAtucPerfCurr1DayLols;
    unsigned long   adslAtucPerfCurr1DayLprs;
    unsigned long   adslAtucPerfCurr1DayESs;
    unsigned long   adslAtucPerfCurr1DayInits;
    int             adslAtucPerfPrev1DayMoniSecs;
    unsigned long   adslAtucPerfPrev1DayLofs;
    unsigned long   adslAtucPerfPrev1DayLoss;
    unsigned long   adslAtucPerfPrev1DayLols;
    unsigned long   adslAtucPerfPrev1DayLprs;
    unsigned long   adslAtucPerfPrev1DayESs;
    unsigned long   adslAtucPerfPrev1DayInits;

    struct adslMib_adslAtucPerfDataTable *next; /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATUCPERFDATATABLE, *pADSL_MIB_ADSLATUCPERFDATATABLE;
/*
 * column number definitions for table adslAtucIntervalTable 
 */
typedef struct adslMib_adslAtucIntervalTable {
    void           *pTableBeginning;
    int             ifIndex;

    int             adslAtucIntervalNumber;
    unsigned long   adslAtucIntervalLofs;
    unsigned long   adslAtucIntervalLoss;
    unsigned long   adslAtucIntervalLols;
    unsigned long   adslAtucIntervalLprs;
    unsigned long   adslAtucIntervalESs;
    unsigned long   adslAtucIntervalInits;
    int             adslAtucIntervalValidData;

    struct adslMib_adslAtucIntervalTable *next; /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATUCINTERVALTABLE, *pADSL_MIB_ADSLATUCINTERVALTABLE;
/*
 * column number definitions for table adslAturPerfDataTable 
 */
typedef struct adslMib_adslAturPerfDataTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned long   adslAturPerfLofs;
    unsigned long   adslAturPerfLoss;
    unsigned long   adslAturPerfLprs;
    unsigned long   adslAturPerfESs;
    int             adslAturPerfValidIntervals;
    int             adslAturPerfInvalidIntervals;
    unsigned long   adslAturPerfCurr15MinTimeElapsed;
    unsigned long   adslAturPerfCurr15MinLofs;
    unsigned long   adslAturPerfCurr15MinLoss;
    unsigned long   adslAturPerfCurr15MinLprs;
    unsigned long   adslAturPerfCurr15MinESs;
    unsigned long   adslAturPerfCurr1DayTimeElapsed;
    unsigned long   adslAturPerfCurr1DayLofs;
    unsigned long   adslAturPerfCurr1DayLoss;
    unsigned long   adslAturPerfCurr1DayLprs;
    unsigned long   adslAturPerfCurr1DayESs;
    int             adslAturPerfPrev1DayMoniSecs;
    unsigned long   adslAturPerfPrev1DayLofs;
    unsigned long   adslAturPerfPrev1DayLoss;
    unsigned long   adslAturPerfPrev1DayLprs;
    unsigned long   adslAturPerfPrev1DayESs;

    struct adslMib_adslAturPerfDataTable *next; /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATURPERFDATATABLE, *pADSL_MIB_ADSLATURPERFDATATABLE;
/*
 * column number definitions for table adslLineAlarmConfProfileTable 
 */
typedef struct adslMib_adslLineAlarmConfProfileTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned char   adslLineAlarmConfProfileName[32];
    int             adslAtucThresh15MinLofs;
    int             adslAtucThresh15MinLoss;
    int             adslAtucThresh15MinLols;
    int             adslAtucThresh15MinLprs;
    int             adslAtucThresh15MinESs;
    unsigned int    adslAtucThreshFastRateUp;
    unsigned int    adslAtucThreshInterleaveRateUp;
    unsigned int    adslAtucThreshFastRateDown;
    unsigned int    adslAtucThreshInterleaveRateDown;
    int             adslAtucInitFailureTrapEnable;
    int             adslAturThresh15MinLofs;
    int             adslAturThresh15MinLoss;
    int             adslAturThresh15MinLprs;
    int             adslAturThresh15MinESs;
    unsigned int    adslAturThreshFastRateUp;
    unsigned int    adslAturThreshInterleaveRateUp;
    unsigned int    adslAturThreshFastRateDown;
    unsigned int    adslAturThreshInterleaveRateDown;
    int             adslLineAlarmConfProfileRowStatus;

    struct adslMib_adslLineAlarmConfProfileTable *next; /* Pointer to the next item in the table (linked list).
                                                         * ** For a single ADSL port, this points to NULL
                                                         * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLLINEALARMCONFPROFILETABLE,
    *pADSL_MIB_ADSLLINEALARMCONFPROFILETABLE;
/*
 * column number definitions for table adslAturChanPerfDataTable 
 */
typedef struct adslMib_adslAturChanPerfDataTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned long   adslAturChanReceivedBlks;
    unsigned long   adslAturChanTransmittedBlks;
    unsigned long   adslAturChanCorrectedBlks;
    unsigned long   adslAturChanUncorrectBlks;
    int             adslAturChanPerfValidIntervals;
    int             adslAturChanPerfInvalidIntervals;
    unsigned long   adslAturChanPerfCurr15MinTimeElapsed;
    unsigned long   adslAturChanPerfCurr15MinReceivedBlks;
    unsigned long   adslAturChanPerfCurr15MinTransmittedBlks;
    unsigned long   adslAturChanPerfCurr15MinCorrectedBlks;
    unsigned long   adslAturChanPerfCurr15MinUncorrectBlks;
    unsigned long   adslAturChanPerfCurr1DayTimeElapsed;
    unsigned long   adslAturChanPerfCurr1DayReceivedBlks;
    unsigned long   adslAturChanPerfCurr1DayTransmittedBlks;
    unsigned long   adslAturChanPerfCurr1DayCorrectedBlks;
    unsigned long   adslAturChanPerfCurr1DayUncorrectBlks;
    int             adslAturChanPerfPrev1DayMoniSecs;
    unsigned long   adslAturChanPerfPrev1DayReceivedBlks;
    unsigned long   adslAturChanPerfPrev1DayTransmittedBlks;
    unsigned long   adslAturChanPerfPrev1DayCorrectedBlks;
    unsigned long   adslAturChanPerfPrev1DayUncorrectBlks;

    struct adslMib_adslAturChanPerfDataTable *next;     /* Pointer to the next item in the table (linked list).
                                                         * ** For a single ADSL port, this points to NULL
                                                         * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATURCHANPERFDATATABLE, *pADSL_MIB_ADSLATURCHANPERFDATATABLE;

/*
 * column number definitions for table adslAturIntervalTable 
 */
typedef struct adslMib_adslAturIntervalTable {
    void           *pTableBeginning;
    int             ifIndex;
    int             adslAturIntervalNumber;

    unsigned long   adslAturIntervalLofs;
    unsigned long   adslAturIntervalLoss;
    unsigned long   adslAturIntervalLprs;
    unsigned long   adslAturIntervalESs;
    int             adslAturIntervalValidData;

    struct adslMib_adslAturIntervalTable *next; /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATURINTERVALTABLE, *pADSL_MIB_ADSLATURINTERVALTABLE;
/*
 * column number definitions for table adslAtucPhysTable 
 */
typedef struct adslMib_adslAtucPhysTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned char   adslAtucInvSerialNumber[32];
    unsigned char   adslAtucInvVendorID[16];
    unsigned char   adslAtucInvVersionNumber[16];
    int             adslAtucCurrSnrMgn;
    unsigned long   adslAtucCurrAtn;
    int             adslAtucCurrStatus;
    int             adslAtucCurrOutputPwr;
    unsigned long   adslAtucCurrAttainableRate;

    struct adslMib_adslAtucPhysTable *next;     /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATUCPHYSTABLE, *pADSL_MIB_ADSLATUCPHYSTABLE;
/*
 * column number definitions for table adslAtucChanPerfDataTable 
 */
typedef struct adslMib_adslAtucChanPerfDataTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned long   adslAtucChanReceivedBlks;
    unsigned long   adslAtucChanTransmittedBlks;
    unsigned long   adslAtucChanCorrectedBlks;
    unsigned long   adslAtucChanUncorrectBlks;
    int             adslAtucChanPerfValidIntervals;
    int             adslAtucChanPerfInvalidIntervals;
    unsigned long   adslAtucChanPerfCurr15MinTimeElapsed;
    unsigned long   adslAtucChanPerfCurr15MinReceivedBlks;
    unsigned long   adslAtucChanPerfCurr15MinTransmittedBlks;
    unsigned long   adslAtucChanPerfCurr15MinCorrectedBlks;
    unsigned long   adslAtucChanPerfCurr15MinUncorrectBlks;
    unsigned long   adslAtucChanPerfCurr1DayTimeElapsed;
    unsigned long   adslAtucChanPerfCurr1DayReceivedBlks;
    unsigned long   adslAtucChanPerfCurr1DayTransmittedBlks;
    unsigned long   adslAtucChanPerfCurr1DayCorrectedBlks;
    unsigned long   adslAtucChanPerfCurr1DayUncorrectBlks;
    int             adslAtucChanPerfPrev1DayMoniSecs;
    unsigned long   adslAtucChanPerfPrev1DayReceivedBlks;
    unsigned long   adslAtucChanPerfPrev1DayTransmittedBlks;
    unsigned long   adslAtucChanPerfPrev1DayCorrectedBlks;
    unsigned long   adslAtucChanPerfPrev1DayUncorrectBlks;

    struct adslMib_adslAtucChanPerfDataTable *next;     /* Pointer to the next item in the table (linked list).
                                                         * ** For a single ADSL port, this points to NULL
                                                         * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATUCCHANPERFDATATABLE, *pADSL_MIB_ADSLATUCCHANPERFDATATABLE;

/*
 * column number definitions for table adslAturChanIntervalTable 
 */
typedef struct adslMib_adslAturChanIntervalTable {
    void           *pTableBeginning;
    int             ifIndex;

    int             adslAturChanIntervalNumber;
    unsigned long   adslAturChanIntervalReceivedBlks;
    unsigned long   adslAturChanIntervalTransmittedBlks;
    unsigned long   adslAturChanIntervalCorrectedBlks;
    unsigned long   adslAturChanIntervalUncorrectBlks;
    int             adslAturChanIntervalValidData;

    struct adslMib_adslAturChanIntervalTable *next;     /* Pointer to the next item in the table (linked list).
                                                         * ** For a single ADSL port, this points to NULL
                                                         * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATURCHANINTERVALTABLE, *pADSL_MIB_ADSLATURCHANINTERVALTABLE;
/*
 * column number definitions for table adslAtucChanIntervalTable 
 */
typedef struct adslMib_adslAtucChanIntervalTable {
    void           *pTableBeginning;
    int             ifIndex;

    int             adslAtucChanIntervalNumber;
    unsigned long   adslAtucChanIntervalReceivedBlks;
    unsigned long   adslAtucChanIntervalTransmittedBlks;
    unsigned long   adslAtucChanIntervalCorrectedBlks;
    unsigned long   adslAtucChanIntervalUncorrectBlks;
    int             adslAtucChanIntervalValidData;

    struct adslMib_adslAtucChanIntervalTable *next;     /* Pointer to the next item in the table (linked list).
                                                         * ** For a single ADSL port, this points to NULL
                                                         * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATUCCHANINTERVALTABLE, *pADSL_MIB_ADSLATUCCHANINTERVALTABLE;
/*
 * column number definitions for table adslLineTable 
 */
typedef struct adslMib_adslLineTable {
    void           *pTableBeginning;
    int             ifIndex;

    int             adslLineCoding;
    int             adslLineType;
    oid             adslLineSpecific[MAX_OID_LEN];
    unsigned char   adslLineConfProfile[32];
    unsigned char   adslLineAlarmConfProfile[32];

    struct adslMib_adslLineTable *next; /* Pointer to the next item in the table (linked list).
                                         * ** For a single ADSL port, this points to NULL
                                         * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLLINETABLE, *pADSL_MIB_ADSLLINETABLE;
/*
 * column number definitions for table adslAtucChanTable 
 */
typedef struct adslMib_adslAtucChanTable {
    void           *pTableBeginning;
    int             ifIndex;

    unsigned long   adslAtucChanInterleaveDelay;
    unsigned long   adslAtucChanCurrTxRate;
    unsigned long   adslAtucChanPrevTxRate;
    unsigned long   adslAtucChanCrcBlockLength;

    struct adslMib_adslAtucChanTable *next;     /* Pointer to the next item in the table (linked list).
                                                 * ** For a single ADSL port, this points to NULL
                                                 * ** so we have only one entry in the table. */
} ADSL_MIB_ADSLATUCCHANTABLE, *pADSL_MIB_ADSLATUCCHANTABLE;
void           
adslMibHelper_filladslAturPhysTable(pADSL_MIB_ADSLATURPHYSTABLE pTable);
void           
adslMibHelper_filladslLineConfProfileTable
(pADSL_MIB_ADSLLINECONFPROFILETABLE pTable);
void           
adslMibHelper_filladslAturChanTable(pADSL_MIB_ADSLATURCHANTABLE pTable);
void           
adslMibHelper_filladslAtucPerfDataTable(pADSL_MIB_ADSLATUCPERFDATATABLE
                                        pTable);
void           
adslMibHelper_filladslAtucIntervalTable(pADSL_MIB_ADSLATUCINTERVALTABLE
                                        pTable);
void           
adslMibHelper_filladslAturPerfDataTable(pADSL_MIB_ADSLATURPERFDATATABLE
                                        pTable);
void           
adslMibHelper_filladslLineAlarmConfProfileTable
(pADSL_MIB_ADSLLINEALARMCONFPROFILETABLE pTable);
void           
adslMibHelper_filladslAturChanPerfDataTable
(pADSL_MIB_ADSLATURCHANPERFDATATABLE pTable);
void           
adslMibHelper_filladslAturIntervalTable(pADSL_MIB_ADSLATURINTERVALTABLE
                                        pTable);
void           
adslMibHelper_filladslAtucPhysTable(pADSL_MIB_ADSLATUCPHYSTABLE pTable);
void           
adslMibHelper_filladslAtucChanPerfDataTable
(pADSL_MIB_ADSLATUCCHANPERFDATATABLE pTable);
void           
adslMibHelper_filladslAturChanIntervalTable
(pADSL_MIB_ADSLATURCHANINTERVALTABLE pTable);
void           
adslMibHelper_filladslAtucChanIntervalTable
(pADSL_MIB_ADSLATUCCHANINTERVALTABLE pTable);
void            adslMibHelper_filladslLineTable(pADSL_MIB_ADSLLINETABLE
                                                pTable);
void           
adslMibHelper_filladslAtucChanTable(pADSL_MIB_ADSLATUCCHANTABLE pTable);

int adslMibHelper_getIfCount(void);
int getAdslAdminStatus(void);
int getAdslOperStatus(void);
unsigned long getAdslIfSpeed(void);
int setAdslAdminStatus(int status);

extern int BcmAdsl_GetObjectValue(char *objId, int objIdLen, char *dataBuf, long *dataBufLen);
extern int BcmAdsl_GetVersion(adslVersionInfo *pAdslVer);
extern int BcmAdsl_ConnectionStart(void);
extern int BcmAdsl_ConnectionStop(void);
extern int BcmAdsl_Configure(adslCfgProfile *pAdslCfg);
extern int getAdslLineTrellisMode(void);
extern int setAdslLineTrellisMode(int mode);

#endif                          /* ADSL_MIB_HELPER_H */
