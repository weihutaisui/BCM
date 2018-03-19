//****************************************************************************
//
//  $Id: LnxTimeout.h 1.5 2006/08/02 19:50:47Z taskiran Release $
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
// LnxTiouet provides the timeout ability for the Operating System
// Abstraction Layer classes. When an Event times out or a Timer expires,
// that's really me doing it.
//
// When an object (Event or Timer) wants me to keep a clock on it, it will
// register with me.
// To indicate a timeout, I'll call a function of the registered object
//
//****************************************************************************

#ifndef LNXTIMEOUT_H
#define LNXTIMEOUT_H

#include <time.h>
#include <sys/time.h>
#include "typedefs.h"
#include "MessageLog.h"

//********************** Include Files ***************************************


//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Class Declaration ***********************************


class BcmLinuxTimeout
{
public:
    
    // return a pointer to the BcmLinuxTimeout object for my thread
    static BcmLinuxTimeout * GetMyTimeout(void);

    // HEY HEY HEY
    // Who is gonna call this?
    static void DestroyAllTimeouts (void);

    enum LnxToClass
    {
        ltc_event,      // BcmEvent
        ltc_timer,      // BcmTimer
//        ltc_mutex,      // BcmMutexSemaphore
//        ltc_ctsem,      // BcmCountingSemaphore
//        ltc_msqq,       // BcmMessageQueue
        ltc_evtset      // BcmEventSet
    };
    
    // AddTimeout
    // When the time expires, I'll call the appropriate obj->function()
    //
    // arguments:
    //      duration_ms - the number of milliseconds in the time period
    //      class_type - the class of the object making the request
    //      obj_ptr - ptr to the object making the request
    // returns:
    //      id of timer I'll use - can be used later to cancel timer
    //      
    ulong AddTimeout (ulong duration_ms, LnxToClass class_type, void * obj_ptr);

    // cancel a timeout

    //
    // arguments:
    //      timer identifier
    // returns:
    //      true = I found the timeout & removed it
    //      false = I couldn't find timeout record - bad timer id?
    bool CancelTimeout (ulong timer_id);

    // Destructor.  
    // 
    ~BcmLinuxTimeout(void);

protected:
    // My assigned name.
    char *pfName;
	    // Controls message logging.
    BcmMessageLogSettings fMessageLogSettings;

private:
    // Constructor.  
    // 
    BcmLinuxTimeout(pthread_t tid);

    typedef struct to_element
    {
        ulong timerid;
        LnxToClass classType;
        void * objPtr;
        to_element * next_ele;
    } 
    toele_t;

    typedef struct to_node
    {
        ulong msecs;
        toele_t * first_ele;
        to_node * next_node;
    }
    tonode_t;



    // Start the thread's timer to expire in the given amount of time
    // arguments:
    //      duration_ms = the length of the time interval
    //      record_start: 
    //          true = set fStartTime to current system tick counter
    //          false = don't disturb fStartTime
    //          this should be false if just temporarily stopped timer to mess with tree
    void StartTimer (ulong duration_ms, bool record_start);

    // Stop the thread's timer 
    void StopTimer(void);

    // Return the number of milliseconds that have elapsed since fStartTime
    long TimeElapsed (void);


    // I had to stop the timer to mess with the timeout tree
    // start it up again
    // argument: root_changed: true = I just changed the root of the tree
    void RestartTimer(bool root_changed);

    // Try to find a node (on the indicated timeout tree) corresponding to the given time
    // return:
    //      if found appropriate tree node, return pointer to it
    //      else return NULL
    tonode_t * FindActiveNode (ulong duration_ms);

    // Stick the given node into the appropriate place on the timeout tree
    // Note: this should only be called if the timeout tree is NON-NULL
    //
    // arguments: 
    //      anode: node to be inserted
    //      duration_ms: amount of time until anode should be processed
    // return:
    //      true = inserted as root node
    //      false otherwise
    bool InsertNode  (tonode_t * anode, ulong duration_ms);

    // Remove a timer element from the timeout tree
    //
    // several cases for victim element:
    //      1. one of many on a node, don't have to remove node from tree
    //      2. only one on a node, have to remove node from tree
    //          A. node in midst of tree
    //          B. node is root, 
    //              i. others in tree
    //              ii. no others in tree
    //
    // arguments:
    //      thisnode: points to the node containing the element to remove
    //      prevnode: points to node "under" the node to be removed
    //          == NULL if node is root
    //      victim: points to the element to remove
    //      prevele: points to the victim element 
    //
    // returns:
    //      true if removed root node (due to removing this element)
    //      false otherwise
    bool RemoveElement (tonode_t * thisnode, tonode_t * prevnode, 
                        toele_t * victim, toele_t * prevele);

    // Remove a node from the timeout tree, and return the node to the free pool
    //
    // arguments: 
    //      thisnode: pointer to the node to be removed
    //      prevnode: points to node "under" the node to be removed
    //          == NULL if node is root
    //
    // return:
    //      true if removed root node
    //      false otherwise
    bool RemoveNode (tonode_t * thisnode, tonode_t * prevnode);
    

    // functions to access my nodes & elements
    tonode_t * GetNode (void);
    void ReleaseNode (tonode_t * anode);
    toele_t * GetElement (void);
    void ReleaseElement (toele_t * anele);

    // Called when a timeout occurs
    // I will set it up such that this function will be called when the timer expires
    // dole out timeout notification(s) as appropriate
    // start time for next notification(s)
    static void LTOAlarmHandler (int signal_number);

    // Process the timeout that just occurred
    void ProcessTimeout (void);


    enum
    {
        NUM_TO_TIMEOUT_OBJECTS = 50,              // corresponds to max number threads
        NUM_TO_NODES = 50,                        // max different timeouts for one thread
        NUM_TO_ELEMENTS = NUM_TO_NODES            // max total timeouts for one thread
    };

    tonode_t * node_pool, * orig_node_ptr;
    toele_t * ele_pool, * orig_ele_ptr;
    tonode_t * fRoot;

    typedef struct grovestruct
    {
        pthread_t tid;
        BcmLinuxTimeout * toobj;
    } togrove_t;
    static togrove_t fGrove [NUM_TO_TIMEOUT_OBJECTS];
    static bool fGroveInitted;
    static bool fSignalInitted;

    ulong fNextTimerId;
    bool fProcessingTimeout;

    struct timespec fSystemBeginTime;
    unsigned long fStartTime;   // Start time of itimer, user large timer.
    unsigned long fStartTimeCurrentTimer;  // Start time of itimer using rootnode.
    timer_t fTimerid;   // Returned value from timer_create
        
    static const ulong fTicksPerMs;
};




#endif


