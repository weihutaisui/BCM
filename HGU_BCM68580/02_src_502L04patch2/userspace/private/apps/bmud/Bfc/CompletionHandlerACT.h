//****************************************************************************
//
//  Copyright (c) 2000  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16251 Laguna Canyon Road
//          Irvine, California 92618
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       CompletionHandlerACT.h
//  Author:         Cliff Danielson
//  Creation Date:  Dec 01, 2000
//
//****************************************************************************
//  Description:
//      This abstract class defines an interface that can be used for pointer
//      based Asynchronous Completion Token subclasses.  Refer to POSA2 book,
//      page 268. 
//
//****************************************************************************
//
// CompletionHandlerACT.h
//
#ifndef COMPLETIONHANDLERACT_H
#define COMPLETIONHANDLERACT_H


typedef unsigned int BcmCompletionEvent;

class BcmCompletionHandlerACT
{
public:
    // Destructor.  
    //
    virtual ~BcmCompletionHandlerACT();

    // HandleEvent - in Asynchronous Completion Token pattern, this function
    //      can be used by the Initiator object (or it's BcmCallbackHandler
    //      helper object to dispatch a specific completion handler that
    //      processes the response from an asynchronous operation.
    //
    // Parameters:
    //      event_code - integer event code which identifies the asynchronous
    //          event type which occurred.  

    virtual void HandleEvent( const BcmCompletionEvent &event_code ) = 0;


};


#endif
