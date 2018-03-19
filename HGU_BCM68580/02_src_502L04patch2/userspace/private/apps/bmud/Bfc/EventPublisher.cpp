//****************************************************************************
//
//  Copyright (c) 2001  Broadcom Corporation
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
//  Filename:       EventPublisher.cpp
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
// EventPublisher.cpp
//
#include "EventPublisher.h"
#include "CompletionHandlerACT.h"

#include "MessageLog.h"

BcmEventPublisher::BcmEventPublisher()
  :
    fSubscriberMmap()
{
    CallTrace("BcmEventPublisher", "BcmEventPublisher");
}


BcmEventPublisher::BcmEventPublisher( const BcmEventPublisher& rhs )
  :
    fSubscriberMmap()
{
    CallTrace("BcmEventPublisher", "BcmEventPublisher copy constr");
    
    // diab C++ compiler has problems with initializer lists and stl 
    // objects...so we assign instead of copy.
    fSubscriberMmap = rhs.fSubscriberMmap;
}


BcmEventPublisher::~BcmEventPublisher()
{
    CallTrace("BcmEventPublisher", "~BcmEventPublisher");
}


BcmEventPublisher& BcmEventPublisher::operator=( const BcmEventPublisher& rhs )
{
    CallTrace("BcmEventPublisher", "operator=");

    if( this == &rhs )
    {
        // special case: assignment to self
        return *this;
    }

    fSubscriberMmap = rhs.fSubscriberMmap;
    return *this;
}


bool BcmEventPublisher::operator<( const BcmEventPublisher& rhs ) const
{
    CallTrace("BcmEventPublisher", "operator<");
    
    return (fSubscriberMmap < rhs.fSubscriberMmap);
}

bool BcmEventPublisher::operator==( const BcmEventPublisher& rhs ) const
{
    CallTrace("BcmEventPublisher", "operator==");
    
    return (fSubscriberMmap == rhs.fSubscriberMmap);
}


void BcmEventPublisher::PublishEvent( unsigned int event_code )
{
    CallTrace("BcmEventPublisher", "PublishEvent");

    if( !fSubscriberMmap.empty() )
    {
        SubscriberMmapIt rng_it = fSubscriberMmap.lower_bound( event_code );
        SubscriberMmapIt rng_end = fSubscriberMmap.upper_bound( event_code );
        BcmCompletionHandlerACT* pACT;
        while( rng_it != rng_end )
        {
            // found a subscriber. notify the subscriber.
            pACT = (*rng_it).second;
            if( pACT )
            {
                // call virtual DerivedPublishEvent() to perform
                // derived class specific event filtering and
                // to dispatch the event.
                DerivedPublishEvent( event_code, pACT );
            }

            ++rng_it;
        }
    }
}


void BcmEventPublisher::DerivedPublishEvent( const unsigned int event_code, 
    BcmCompletionHandlerACT* pACT )
{
    CallTrace("BcmEventPublisher", "DerivedPublishEvent");

    // default implementation does not perform additional event filtering.
    // simply notify the subscriber.
    pACT->HandleEvent( event_code );
}


void BcmEventPublisher::Subscribe( const unsigned int event_code, BcmCompletionHandlerACT* pACT )
{
    CallTrace("BcmEventPublisher", "Subscribe");
    
    bool dupe_subscriber = false;
    if( !fSubscriberMmap.empty() )
    {
        SubscriberMmapIt rng_it = fSubscriberMmap.lower_bound( event_code );
        SubscriberMmapIt rng_end = fSubscriberMmap.upper_bound( event_code );        
        while( rng_it != rng_end )
        {
            // found subscriber(s) for this event code.
            if( (*rng_it).second == pACT )
            {
                // dupe subscriber.  note it and break out of loop.
                dupe_subscriber = true;
                break;
            }
            ++rng_it;
        }
    }

    if( !dupe_subscriber )
    {
        fSubscriberMmap.insert( SubscriberPair( event_code, pACT ) );
    }
}

void BcmEventPublisher::UnSubscribe( const unsigned int event_code, BcmCompletionHandlerACT* pACT )
{
    CallTrace("BcmEventPublisher", "UnSubscribe");

    if( !fSubscriberMmap.empty() )
    {
        SubscriberMmapIt rng_it = fSubscriberMmap.lower_bound( event_code );
        SubscriberMmapIt rng_end = fSubscriberMmap.upper_bound( event_code );        
        while( rng_it != rng_end )
        {
            // found subscriber(s) for this event code.
            if( (*rng_it).second == pACT )
            {
                // matching subscriber found.  erase it and return
                // NOTE: we are assuming that no duplicate subscribers have
                // been added to fSubscriberMmap.  Subscribe() should prevent
                // duplicate subscribers from being added to fSubscriberMmap.
                // 
                fSubscriberMmap.erase( rng_it );
                return;
            }
            ++rng_it;
        }
    }
}
