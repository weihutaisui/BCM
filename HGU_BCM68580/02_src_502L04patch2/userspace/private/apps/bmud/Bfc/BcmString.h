//**************************************************************************
//
//    Copyright 2000 Broadcom Corporation
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
//    Filename: BcmString.h
//    Author:   Kevin O'Neal
//    Creation Date: 18-April-2000
//
//**************************************************************************
//    Description:
//
//		Cablemodem V2 code
//		
//    Define the BcmString type.  Some compilers have issues with the
//    #include <string> construct, which is why this typedef is broken
//    out here instead of being in Snmp/CoreObjs.h
//
//		
//**************************************************************************
//    Revision History:
//
//**************************************************************************

#ifndef BCMSTRING_H
#define BCMSTRING_H

/*
// Use this block to use standard STL strings for the BcmString type
#include <string>
typedef string BcmString;
*/

// Use this block to use the code-size optimized BcmBasicString
// for the BcmString type
#include "BcmBasicString.h"
typedef BcmBasicString BcmString;

#endif
