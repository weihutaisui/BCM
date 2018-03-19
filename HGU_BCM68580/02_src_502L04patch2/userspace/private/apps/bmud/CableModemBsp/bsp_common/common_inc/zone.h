//**************************************************************************
//
//    Copyright 1999  Broadcom Corporation
//    All Rights Reserved
//    No portions of this material may be reproduced in any form without the
//    written permission of:
//             Broadcom Corporation
//             16251 Laguna Canyon Road
//             Irvine, California  92618
//    All information contained in this document is Broadcom Corporation
//    company private, proprietary, and trade secret.
//
//
//
//**************************************************************************
#ifndef _ZONE___
#define _ZONE___

#define DEBUGZONE(x)           (unsigned short)(1 << x)
#define TEXT(x)                (char *)x

#define ZONE_INIT               DEBUGZONE(0)        //  Initialization
#define ZONE_TEST1              DEBUGZONE(1)        //  testing
#define ZONE_TEST2              DEBUGZONE(2)        //  testing
#define ZONE_TEST3              DEBUGZONE(3)        //  testing
#define ZONE_TEST4              DEBUGZONE(4)        //  testing
#define ZONE_TEST5              DEBUGZONE(5)        //  testing
#define ZONE_TEST6              DEBUGZONE(6)        //  testing
#define ZONE_BPI                DEBUGZONE(7)        //  Baseline Privacy
#define ZONE_DOWNSTREAM         DEBUGZONE(8)        //  testing
#define ZONE_UPSTREAM           DEBUGZONE(9)        //  testing
#define ZONE_TUNER              DEBUGZONE(10)       //  testing
#define ZONE_RANGING            DEBUGZONE(11)       //  testing
#define ZONE_TESTSRAM           DEBUGZONE(12)       //  testing
#define ZONE_TESTREG            DEBUGZONE(13)       //  testing
#define ZONE_WARNING            DEBUGZONE(14)       //  Warnings
#define ZONE_ERROR              DEBUGZONE(15)       //  Errors

#endif // __ZONE___




