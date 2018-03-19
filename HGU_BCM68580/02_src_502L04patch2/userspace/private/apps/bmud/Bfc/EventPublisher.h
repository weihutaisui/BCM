//****************************************************************************
//
//  Copyright (c) 2001  Broadcom Corporation
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//          Broadcom Corporation
//          16215 Alton Parkway
//          Irvine, California 92619
//  All information contained in this document is Broadcom Corporation
//  company private, proprietary, and trade secret.
//
//****************************************************************************
//  $Id$
//
//  Filename:       EventPublisher.h
//  Author:         Cliff Danielson
//  Creation Date:  Apr 13, 2001
//
//****************************************************************************
//  Description:
//      Class BcmEventPublisher defines an interface that can be used to publish
//      asynchronous event notifications to multiple subscribers.  Refer to 
//      POSA2 book, page 268. 
//
//****************************************************************************
//
// EventPublisher.h
//
#ifndef EVENTPUBLISHER_H
#define EVENTPUBLISHER_H

#include "typedefs.h"

#include <map>

class BcmCompletionHandlerACT;

// UNFINISHED - the SubscriberPair typedef needs to be phased out in favor
// of public typedef BcmEventPublisher::KeySubscriberPair
//
typedef pair< const unsigned int, BcmCompletionHandlerACT* > SubscriberPair;

class BcmEventPublisher
{
public:
    // Default constructor.
    //
    BcmEventPublisher();

    // Copy constructor.
    //
    BcmEventPublisher( const BcmEventPublisher& rhs );

    // Destructor.  
    //
    virtual ~BcmEventPublisher();

    // Assignment operator.
    //
    BcmEventPublisher& operator=( const BcmEventPublisher& rhs );

    // Less than operator.
    //
    bool operator<( const BcmEventPublisher& rhs ) const;

    // Equality operator.
    //
    bool operator==( const BcmEventPublisher& rhs ) const;


    // PublishEvent() - Notify interested subscribers that the specified event
    //      has occurrd.
    //
    // Parameters:
    //      event_code - integer event code which identifies the asynchronous
    //          event type which occurred.  
    //
    virtual void PublishEvent( const unsigned int event_code );

    // Subscribe() - Subscribe for notification when the specified event occurs.
    //
    // Parameters:
    //      SubscriberPair
    //      event_code - event code for event of interest.
    //
    //      pACT - pointer to object derived from BcmCompletionHandlerACT. 
    //          BcmCompletionHandlerACT implements the callback interface.  
    //          The derived class implements HandleEvent() in a manner which
    //          dispatches the event to the client object.
    //
    void Subscribe( const unsigned int event_code, BcmCompletionHandlerACT* pACT );
    
    // UnSubscribe() - End subscription for event notification.
    //
    // Parameters:
    //      event_code - event code for event of interest.
    //
    //      pACT - pointer to object derived from BcmCompletionHandlerACT. 
    //          BcmCompletionHandlerACT implements the callback interface.  
    //          The derived class implements HandleEvent() in a manner which
    //          dispatches the event to the client object.
    //
    void UnSubscribe( const unsigned int event_code, BcmCompletionHandlerACT* pACT );

    // NumSubscribers() - Returns the number of subscribers for the specified 
    //      event code.
    //
    // Parameters:
    //      event_code - event code for event of interest.
    //
    // Returns:
    //      unsigned int - number of subscribers for the specified event code.
    //
    unsigned int NumSubscribers( const unsigned int event_code ) const
    { 
        return ( fSubscriberMmap.count( event_code ) ); 
    }


    typedef pair< const unsigned int, BcmCompletionHandlerACT* > KeySubscriberPair;


protected:
    // DerivedPublishEvent() - Called by PublishEvent for each target
    //      < event code, BcmCompletionHandlerACT* > pair found
    //      in order that a derived class may apply additional filtering 
    //      criteria prior to dispatching the event to the ACT.  
    //      Default implementation unconditionslly dispatches the event 
    //      to the specified ACT.
    //
    // Parameters:
    //      event_code - integer event code which identifies the asynchronous
    //          event type which occurred.  
    //
    //      pACT - pointer to object derived from BcmCompletionHandlerACT. 
    //
    virtual void DerivedPublishEvent( const unsigned int event_code, 
        BcmCompletionHandlerACT* pACT );

    typedef multimap< unsigned int, BcmCompletionHandlerACT*, less<unsigned int> > SubscriberMmap;
    typedef SubscriberMmap::iterator SubscriberMmapIt;
    
    SubscriberMmap fSubscriberMmap;
};


#endif
