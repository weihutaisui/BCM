////////////////////////////////////////////////////////////////////////////
//***************************************************************************
//
//  Copyright (c) 2006  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       HttpFormHandlerBatController.h
//  Author:         Tyler White
//  Creation Date:  Mar 27, 2006
//
//****************************************************************************
//  Description:  
//  This class implements the BatController form
//
//****************************************************************************


#ifndef HTTPFORMHANDLERBATCONTROLLER_H
#define HTTPFORMHANDLERBATCONTROLLER_H

//********************** Include Files ***************************************

#include "HttpFormHandler.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Types ****************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************
class BcmHttpBatControllerFormHandler : public BcmHttpFormHandler
{
public:

    // Default Constructor.  Initializes the state of the object...
    //
    // Parameters:  None.
    //
    // Returns:  N/A
    //
    BcmHttpBatControllerFormHandler();

    // Destructor.  Frees up any memory/objects allocated, cleans up internal
    // state.
    //
    // Parameters:  N/A
    //
    // Returns:  N/A
    //
    ~BcmHttpBatControllerFormHandler();

    CfgAction ProcessForm(BcmOctetBuffer queryString, unsigned int queryStringLength, unsigned long &ulErrorCode, BcmHttpConnection &httpConnection);
    bool ProcessAspRequest(char * cControl, BcmOctetBuffer &octetBuffer, bool adminAccess);
    char * FormName(void);
    char * MyNeighborhood(int index);
    char * MyLinkInactive(void);
    char * MyLinkActive(void);

    char * MyGroup(void);
    char * MyMenuLinkInactive(void);
    char * MyMenuLinkActive(void);
    char * HtmlPath(void);
    unsigned long HtmlLength(void);

    unsigned char * MyHtml(void);

private:
    static char * fMyNeighborhood[];
    static char * fMyLinkInactive;
    static char * fMyLinkActive;

    static char * fMyGroup;
    static char * fMyMenuLinkInactive;
    static char * fMyMenuLinkActive;
    static char * fMyPath;
};

//********************** Inline Method Implementations ***********************

#endif
