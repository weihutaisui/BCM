//****************************************************************************
//
//  $Id: LnxTimeout.cpp 1.6 2006/08/02 19:50:45Z taskiran Release $
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
/****************************************************************************
LnxTimeout provides the timeout ability for the Operating System
Abstraction Layer classes. When an Event times out or a Timer expires,
that's really me doing it.

This class works by handling the reception of the SIGALRM signal. I set the
timer, and my handler is executed when the signal is sent
 
Overview:
Under Linux, each process has a "real time" timer - this timer is tied to the amount of 
actual clock time expired (rather than the amount of time a particular process is
actually executing). You can control this timer with the setitimer() function.

Under Pthreads, each thread is also a separate process. So each thread has its
own real time timer.

Each object of this class corresponds to one particular thread, and so to one particular 
real time clock.

A thread may be waiting on more than one timeout at once. Since there is only one time
facility per thread, I need to turn than into multiple timeouts.

It is also possible that a thread has more than one timeout set to expire at once.

To cope with this, I keep a "tree" of the timeout events. Each individual timeout is
an "element"; each time value on the tree corresponds to a "node". A node can indicate 
one or more elements; this allows more than one timeout to have the same value.

Of course, there is one timeout tree per object of this class.
 
****************************************************************************/




//********************** Include Files ***************************************
#include "LnxTimeout.h"
#include "LnxEvent.h"
#include "LnxEventSet.h"
#include "LnxTimer.h"
#include "LnxOperatingSystem.h"
#include "SystemTime.h"
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include <sys/syscall.h>
#define gettid() syscall(__NR_gettid)

// instantiate static data members                           
// toHighWater is the number of Timeout objects matched with threads.  It
// allows us to exit the fGrove array search faster in some cases.
static int toHighWater = 0;
const ulong BcmLinuxTimeout::fTicksPerMs = 1000/CLOCKS_PER_SEC;
BcmLinuxTimeout::togrove_t BcmLinuxTimeout::fGrove [NUM_TO_TIMEOUT_OBJECTS];
bool BcmLinuxTimeout::fGroveInitted = false;
bool BcmLinuxTimeout::fSignalInitted = false;
                                            
                                            
//********************** Global Types ****************************************

//----------------------------------------------------------------------------
// return a pointer to the BcmLinuxTimeout object for my thread
//----------------------------------------------------------------------------
BcmLinuxTimeout * BcmLinuxTimeout::GetMyTimeout(void)
{
    int i;
    // initialize data shared between Timeout objects
    if (!fGroveInitted)
    {
        for (i = 0; i < NUM_TO_TIMEOUT_OBJECTS; ++i)
        {
            fGrove[i].tid = 0;
            fGrove[i].toobj = NULL;
        }
        fGroveInitted = true;
    }

    BcmLinuxTimeout * toptr = NULL;         // value to return
    pthread_t this_tid = pthread_self();

    // search through the set of timeout trees to find the right one
    bool done = false;

    for (i = 0; !done && (i < toHighWater); ++i )
    {
        if (fGrove[i].tid == this_tid)
        {
            toptr = fGrove[i].toobj;
            done = true;
        }
    }
    // no more set up - find a spot for a new one.
    for (i = 0; !done && (i < NUM_TO_TIMEOUT_OBJECTS); ++i )
    {
        if (fGrove[i].tid == 0)
        { 
            fGrove[i].tid = this_tid;
            toptr = fGrove[i].toobj = new BcmLinuxTimeout (this_tid);
            if(i >= toHighWater)
            {
                toHighWater = i + 1;
            }
            done = true;
        }
    }
    return toptr;
}

//----------------------------------------------------------------------------
// Constructor.  
//----------------------------------------------------------------------------
BcmLinuxTimeout::BcmLinuxTimeout(pthread_t tid)
{
	char pString[48];
	memset(pString, 0, sizeof(pString));
	snprintf(pString, sizeof(pString), "Timeout(0x%lx)", (unsigned long)tid);
//    cout << "BcmLinuxTimeout pid = "<< getpid() 
//           << " tid = "<< pthread_self() << endl;
    // Call the helper method to create my object name.
    pfName = BcmOperatingSystem::GenerateName(pString, "Timer");
    
//	gAlwaysMsg(fMessageLogSettings, "BcmLinuxTimeout")
//		<< pfName << " tid = " << hex << (void *)pthread_self() << endl;
    // Set my instance name.
    fMessageLogSettings.SetInstanceName(pfName);
    fMessageLogSettings.SetModuleName("BcmLinuxTimeout");

    fMessageLogSettings.Register();

    fTimerid = 0;
    StopTimer();

    fNextTimerId = 1;
    fRoot = NULL;
    fProcessingTimeout = false;

    // get & initialize my timeout nodes
    node_pool = orig_node_ptr = new tonode_t [NUM_TO_NODES];
    int i;
    for (i = 0; i < NUM_TO_NODES; ++i)
    {
        node_pool[i].first_ele = NULL;
        if (i < (NUM_TO_NODES-1))
            node_pool[i].next_node = &node_pool[i+1];
    }
    node_pool[NUM_TO_NODES-1].next_node = NULL;

    // get & initialize my timeout elements
    ele_pool = orig_ele_ptr = new toele_t [NUM_TO_ELEMENTS];
    for (i = 0; i < NUM_TO_ELEMENTS-1; ++i)
            ele_pool[i].next_ele = &ele_pool[i+1];
    ele_pool[NUM_TO_ELEMENTS-1].next_ele = NULL;

	// Set up the signal handler
    if (!fSignalInitted)
	{
		struct sigaction action;
		action.sa_handler = BcmLinuxTimeout::LTOAlarmHandler;
		sigemptyset (&action.sa_mask);
		sigaddset (&action.sa_mask, SIGUSR1);
		action.sa_flags = SA_RESTART;
		
		sigaction (SIGALRM, &action, NULL);
		fSignalInitted = true;
	}
}

//----------------------------------------------------------------------------
// get rid of all the Timeouts
//----------------------------------------------------------------------------
void BcmLinuxTimeout::DestroyAllTimeouts(void)
{
    bool done = false;
    for (int i = 0; !done && (i < NUM_TO_TIMEOUT_OBJECTS); ++i )
    {
        if (fGrove[i].toobj != NULL)
            delete fGrove[i].toobj;
        else
            done = true;
    }
}

//----------------------------------------------------------------------------
// Destructor.  
//----------------------------------------------------------------------------
BcmLinuxTimeout::~BcmLinuxTimeout(void)
{
    StopTimer();

    pthread_t this_tid = pthread_self();
	int done = false;
    for (int i = 0; !done && (i < NUM_TO_TIMEOUT_OBJECTS); ++i )
    {
        if (fGrove[i].tid == this_tid)
		{
			fGrove[i].toobj = NULL;
			fGrove[i].tid = 0;
			done = true;
		}
	}
    
    delete [] orig_node_ptr;
    delete [] orig_ele_ptr;
}



//----------------------------------------------------------------------------
// return a pointer to an available timeout node
// adjust free pool
//----------------------------------------------------------------------------
BcmLinuxTimeout::tonode_t * BcmLinuxTimeout::GetNode (void)
{
    tonode_t * retval = node_pool;
    node_pool = retval->next_node;
    return retval;
}

//----------------------------------------------------------------------------
// put a timeout node back in the pool of availables
// adjust free pool
//----------------------------------------------------------------------------
void BcmLinuxTimeout::ReleaseNode (tonode_t * recyc)
{
    recyc->next_node = node_pool;
    node_pool = recyc;
}

//----------------------------------------------------------------------------
// return a pointer to an available timeout element
// adjust free pool
//----------------------------------------------------------------------------
BcmLinuxTimeout::toele_t * BcmLinuxTimeout::GetElement (void)
{
    toele_t * retval = ele_pool;
    ele_pool = retval->next_ele;
    return retval;
}

//----------------------------------------------------------------------------
// put a timeout node back in the pool of availables
// adjust free pool
//----------------------------------------------------------------------------
void BcmLinuxTimeout::ReleaseElement (toele_t * recyc)
{
    recyc->next_ele = ele_pool;
    ele_pool = recyc;
}


//----------------------------------------------------------------------------
// Return the number of milliseconds that have elapsed since fStartTime
//----------------------------------------------------------------------------
long BcmLinuxTimeout::TimeElapsed (void)
{
    unsigned long elapsedtime = 0;
    unsigned long msecs = 0;
	
	msecs = SystemTimeMS();
	if(msecs >= fStartTime)
	{
		elapsedtime = msecs - fStartTime;
	}
	else
	{
		// The system time overflowed
		elapsedtime = msecs + (0xffffffff - fStartTime);
		gInfoMsg(fMessageLogSettings, "TimeElapsed")
			<< "fStartTime is greater than msecs!" << endl;
	}
	
	gInfoMsg(fMessageLogSettings, "TimeElapsed")
		<< "fStartTime = " << fStartTime << " msecs = " << msecs << " elapsedtime = " << elapsedtime << endl;
	
    return elapsedtime;
}


//----------------------------------------------------------------------------
// Try to find a node on the timeout tree corresponding to the given time
// return:
//      if found appropriate tree node, return pointer to it
//      else return NULL
//----------------------------------------------------------------------------
BcmLinuxTimeout::tonode_t * BcmLinuxTimeout::FindActiveNode (ulong duration_ms)
{
//    cout << "BcmLinuxTimeout::FindActiveNode " << duration_ms << endl;

    tonode_t * foundnode = NULL;

    // I need to look up the tree and see if there is a spot that corresponds to 
    // my timeout
    
	ulong time_elapsed = TimeElapsed();
	if(fRoot->msecs < time_elapsed)
	{
		gErrorMsg(fMessageLogSettings, "FindActiveNode")
			<< "duration_ms = " << duration_ms << " time_elapsed = " << time_elapsed << " fRoot->msecs = " << fRoot->msecs << endl;
	}
    //  next timeout happens = (root timeout) - elapsed
    ulong next_to = fRoot->msecs - time_elapsed;
    
    if (duration_ms == next_to)
    {
        foundnode = fRoot;
    }
    else if (duration_ms > next_to)
    {
        // walk up the tree, adding up the nodes, until either get or exceed 
        // the right total 
        ulong timetotal = next_to;
        tonode_t * thisnode = fRoot;
        
        for (bool done = false; !done;)
        {
            thisnode = thisnode->next_node;
            if (thisnode == NULL)
                done = true;        // looked at all nodes
            else
            {
                timetotal += thisnode->msecs;
                if (duration_ms == timetotal)
                {
                    done = true;
                    foundnode = thisnode;
                }
                else if (timetotal > duration_ms)   // timetotal just increased by a bunch
                    done = true;                    // no match
                // else duration_ms < timetotal, keep going
            }
        }
    }
    // else duration < next_to, no node exists for that

    return foundnode;
}


//----------------------------------------------------------------------------
// Stick the given node into the appropriate place on the timeout tree
// Adjust tree appropriately
// Note: this should only be called if the timeout tree is NON-NULL
//
// argument: 
//      anode: node to be inserted
//      duration_ms: amount of time until anode should be processed
// return:
//      true = inserted as root node
//      false otherwise
//----------------------------------------------------------------------------
bool BcmLinuxTimeout::InsertNode  (tonode_t * anode, ulong duration_ms)
{
//    cout << "BcmLinuxTimeout::InsertNode" << endl;

    bool newroot = false;

    // First, I need to find out how much time remains until the root node is 
    // processed
	ulong time_elapsed = TimeElapsed();
	if(fRoot->msecs < time_elapsed)
	{
		gErrorMsg(fMessageLogSettings, "InsertNode")
			<< "duration_ms = " << duration_ms << " time_elapsed = " << time_elapsed << " fRoot->msecs = " << fRoot->msecs << endl;
	}
    //  next timeout happens = (root timeout) - elapsed
    ulong next_to = fRoot->msecs - time_elapsed;
    
	gInfoMsg(fMessageLogSettings, "InsertNode")
		<< "duration_ms = " << duration_ms << " next_to = " << next_to << " fRoot->msecs = " << fRoot->msecs << endl;
    if (duration_ms < next_to)
    {   // this node needs to be the root
        newroot = true;

        // adjust timeout for soon-to-be displaced root
        // say X = original root timeout
        //      Y = time elapsed since original timeout started
        //      Z = new root timeout
        //      A = new timeout for soon-to-be-displaced root
        //      A = X - Y - Z
        //      already calculated X - Y : next_to
        fRoot->msecs = next_to - duration_ms;

        // put given node in at root
        anode->next_node = fRoot;
        fRoot = anode;
        anode->msecs = duration_ms;
    }
    else 
    {   // the new node won't be the root node
        // walk up the tree - stop when:
        //      a. find timetotal greater than timeout
        //      b. get to end of tree
        tonode_t * prevnode = fRoot;
        tonode_t * nextnode = prevnode->next_node;
        ulong timetotal = next_to;
        
        for (bool done = false; !done;) 
        {
            if (nextnode != NULL)
            {
                // If the next one is going to be bigger, we want
                // to calculate now.
                timetotal += nextnode->msecs;
            }
            if (nextnode == NULL)
            {   // got to end of tree -> new node has longest timeout
                // put this node into tree
                prevnode->next_node = anode;
                anode->next_node = NULL;
                
                // set time on this node
                anode->msecs = duration_ms - timetotal;
                done = true;
            }
            else if (timetotal > duration_ms )
            {   // oops - just walked past spot for me

                // set time on this node
                // total time "beneath" me is (timetotal - thisnode's time)
                // subtract that from our total duration.
                anode->msecs = duration_ms - (timetotal - nextnode->msecs);

                // adjust time on next node
                nextnode->msecs -= anode->msecs;

                // insert myself into tree
                prevnode->next_node = anode;
                anode->next_node = nextnode;

                done = true;
            }
            else    // still walking up tree
            {
                prevnode = nextnode;
                nextnode = nextnode->next_node;
            }
        }
    }

    return newroot;
}

//----------------------------------------------------------------------------
// Stop the thread's timer 
//----------------------------------------------------------------------------
void BcmLinuxTimeout::StopTimer(void)
{
//    	gAlwaysMsg(fMessageLogSettings, "BcmLinuxTimeout")
//			<< "BcmLinuxTimeout::StopTimer" << endl;
    if (fTimerid)
        timer_delete(fTimerid);
}

//----------------------------------------------------------------------------
// Start the thread's timer to expire in the given amount of time
// arguments:
//      duration_ms = the length of the time interval
//      record_start: 
//          true = set fStartTime to current system tick counter
//          false = don't disturb fStartTime
//          this should be false if just temporarily stopped timer to mess with tree
//----------------------------------------------------------------------------
void BcmLinuxTimeout::StartTimer(ulong duration_ms, bool record_start)
{
    struct sigevent sev;
    struct itimerspec its;

    sev.sigev_notify = SIGEV_THREAD_ID;
    sev._sigev_un._tid = gettid();
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = &fTimerid;
    if (timer_create(CLOCK_REALTIME, &sev, &fTimerid) == -1) {
        gErrorMsg(fMessageLogSettings, "Error timer_create") << endl;
    }

    its.it_value.tv_sec = duration_ms / 1000;
    its.it_value.tv_nsec = (duration_ms % 1000)*1000000 ;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(fTimerid, 0, &its, NULL) == -1) {
        gErrorMsg(fMessageLogSettings, "Error timer_settime") << endl;
    }

    if (record_start)
    {
        // record the current counter value
        fStartTime = SystemTimeMS();
    }
}

//----------------------------------------------------------------------------
// I had to stop the timer to mess with the timeout tree
// start it up again
// argument: inserted_root: true = I added a new node as the root of the tree
//----------------------------------------------------------------------------
void BcmLinuxTimeout::RestartTimer(bool inserted_root)
{
//    gInfoMsg(fMessageLogSettings, "InsertNode")
//		 << "BcmLinuxTimeout::RestartTimer " << inserted_root << endl;
    if (fRoot != NULL)
    {
        if (inserted_root)
        {
            // since have inserted root, need to renew my "start time"
            StartTimer (fRoot->msecs, true);
        }
        else
        {
            // the amount for the new timeout = root interval - elapsed
            // this is true even if I just deleted the previous root
			ulong time_elapsed = TimeElapsed();
			if(fRoot->msecs < time_elapsed)
			{
				gErrorMsg(fMessageLogSettings, "RestartTimer")
					<< "time_elapsed = " << time_elapsed << " fRoot->msecs = " << fRoot->msecs << endl;
			}
            ulong time2go = fRoot->msecs - time_elapsed;
            if (time2go <= fTicksPerMs)
            {   // time out really needs to happen as soon as possible
                time2go = fTicksPerMs + 1;
            }
			// Record how much time is left for the next timeout
			fRoot->msecs = time2go;
			gInfoMsg(fMessageLogSettings, "RestartTimer")
				<< "time_elapsed = " << time_elapsed << " fRoot->msecs = " << fRoot->msecs << " time2go = " << time2go << endl;
            //StartTimer (time2go, false);    // don't mess with recorded start time
			// Every time the timer is started, the system time (not duration!) must be recorded.
            StartTimer (time2go, true); 
        }
    }
	else
	{
        // The timer is running, we don't want it to send us an extra signal.
		StopTimer();
	}
//    gInfoMsg(fMessageLogSettings, "InsertNode")
//		<< "BcmLinuxTimeout::RestartTimer done!" << endl;
}

//----------------------------------------------------------------------------
// AddTimeout
// When the time expires, I'll call the appropriate obj->function()
//
// arguments:
//      duration_ms - the number of milliseconds in the time period
//      class_type - the class of the object making the request
//      obj_ptr - ptr to the object making the request
// returns:
//      id of timer I'll use - can be used later to cancel timer
//          == 0 if some problem
//      
//----------------------------------------------------------------------------
ulong BcmLinuxTimeout::AddTimeout (ulong duration_ms, LnxToClass class_type, void * obj_ptr)
{
	gInfoMsg(fMessageLogSettings, "AddTimeout")
		<< "AddTimeout: duration_ms = " << duration_ms << endl;
	// Why do I need this, because BcmLnxTimer::Stop() does not truely stop the timer!
	// This prevents race condition with the signal handler.
	StopTimer();

    // get a fresh timeout element & fill it up
    toele_t * new_ele = GetElement();
    ulong retval = new_ele->timerid = BcmLinuxTimeout::fNextTimerId++;
    if (!BcmLinuxTimeout::fNextTimerId)
        BcmLinuxTimeout::fNextTimerId = 1;
    new_ele->classType = class_type;
    new_ele->objPtr = obj_ptr;
    new_ele->next_ele = NULL;
    
    if (fRoot == NULL)
    {   // no timeouts currently pending
		gInfoMsg(fMessageLogSettings, "AddTimeout")
			<< "AddTimeout: new root" << endl;

        // get a node for the fRoot of the tree
        fRoot = GetNode();
        fRoot->msecs = duration_ms;
        fRoot->first_ele = new_ele;
        fRoot->next_node = NULL;

        StartTimer (duration_ms, true); // start time, record started time
    }
    else    // timer already running - figure out where to stick in this timeout
    {
		gInfoMsg(fMessageLogSettings, "AddTimeout")
			<< "AddTimeout: not new root. fRoot = " << hex << fRoot << endl;
        bool newroot = false;
        // see if there is already a node on the tree that has the time I want
        tonode_t * got_node = FindActiveNode (duration_ms);
        if (got_node != NULL)
        {   // found an existing node to stick new element on
            // tack new element onto whatever elements are already on the node
            toele_t * p1 = got_node->first_ele;
            for (bool found = false; !found;)
            {
                if (p1->next_ele == NULL)
                {
                    p1->next_ele = new_ele;
                    found = true;
                }
                else
                {
                    p1 = p1->next_ele;
                }
            }
        }
        else    // didn't find an existing node to stick new element on
        {
            tonode_t * new_node = GetNode();    // get a fresh node
			gInfoMsg(fMessageLogSettings, "AddTimeout")
				<< "Got new node:" << hex << new_node << endl;
            new_node->first_ele = new_ele;      // attach element to the node
            newroot = InsertNode (new_node, duration_ms); // put node in timeout tree
			{
				tonode_t * ptr_node = fRoot;
				gInfoMsg(fMessageLogSettings, "AddTimeout")
					<< "Dumping timeout tree:"<< endl;
				while(ptr_node)
				{
					gInfoMsg(fMessageLogSettings, "AddTimeout")
						<< "ptr_node = " << hex << ptr_node  << " ptr_node->msecs = " << dec << ptr_node->msecs << endl;
					ptr_node=ptr_node->next_node;
				}
			}
        }

        RestartTimer (newroot);
    }
    return retval;
}

//----------------------------------------------------------------------------
// Remove a node from the timeout tree, and return the node to the free pool
//
// several cases for victim node:
//      1. no other nodes above
//      2. other nodes above
//
// arguments: 
//      thisnode: pointer to the node to be removed
//      prevnode: points to node "under" the node to be removed
//          that is, it is more towards the root than the node to be removed
//          if prevnode == NULL, need to remove root node
//
// return:
//      true if removed root node
//      false otherwise
//----------------------------------------------------------------------------
bool BcmLinuxTimeout::RemoveNode (tonode_t * thisnode, tonode_t * prevnode)
{
//    cout << "BcmLinuxTimeout::RemoveNode" << endl;
    bool root_removed = false;    // assume there are other nodes

	gInfoMsg(fMessageLogSettings, "RemoveNode")
		<< "thisnode = " << hex << thisnode << " prevnode = " << prevnode << endl;
    
    // adjust the timeout for the next node up the tree
    // add in the time for the node I'm about to remove
    if (thisnode->next_node != NULL)
	{
        thisnode->next_node->msecs += thisnode->msecs;
		gInfoMsg(fMessageLogSettings, "RemoveNode")
			<< "thisnode->msecs = " << dec << thisnode->msecs << " thisnode->next_node->msecs = " << thisnode->next_node->msecs << endl;
	}
    
    if (fRoot == thisnode)
    {   // this is root node
        root_removed = true;
        fRoot = thisnode->next_node;
    }
    else    // this is not root node - so there are other nodes in tree
        prevnode->next_node = thisnode->next_node;  // unlink this node
    
    ReleaseNode (thisnode);
    return root_removed;    
}


//----------------------------------------------------------------------------
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
// returns:
//      true if removed root node (due to removing this element)
//      false otherwise
//----------------------------------------------------------------------------
bool BcmLinuxTimeout::RemoveElement (tonode_t * thisnode, tonode_t * prevnode, 
                                     toele_t * victim, toele_t * prevele)
{
//    cout << "BcmLinuxTimeout::RemoveElement" << endl;
    bool root_removed = false;

    // find out if this is only element on its node
    if ((thisnode->first_ele == victim) && (victim->next_ele == NULL))
    {   // only element on the node - have to remove it from tree
        root_removed = RemoveNode (thisnode, prevnode);
    }
    else    // there is at least one other element on this node (case 1)
    {   
        if (thisnode->first_ele == victim)
        {   // this element is the first on the node 
            thisnode->first_ele = victim->next_ele;
        }
        else    // this is not first element on node
        {
            prevele->next_ele = victim->next_ele;
        }
    }

    ReleaseElement (victim);
    return root_removed;
}

//----------------------------------------------------------------------------
// stop timeout
//
// returns:
//      true = success
//      false = failure
//----------------------------------------------------------------------------
bool BcmLinuxTimeout::CancelTimeout (ulong victim)
{
	// Prevent race condition here
	StopTimer();
    
    bool foundit = false; // assume humiliating failure
    if (fRoot != NULL)
    {

        // look through timeout tree until I find the indicated element
        tonode_t * nodeptr = fRoot;
        tonode_t * prevnode = NULL;
        toele_t * eleptr = fRoot->first_ele;
        toele_t * prevele = NULL;

        bool checked_nodes = false;
        bool checked_eles = false;
        
        // keep going til find it or look at everything
        while (!foundit && !checked_nodes)
        {
            // look at the elements in this node
            while (!foundit && !checked_eles)
            {
                if (eleptr->timerid == victim)
                {
                    foundit = true;   // we're done, successfully
                    RemoveElement (nodeptr, prevnode, eleptr, prevele);
                }
                else
                {
                    prevele = eleptr;
                    eleptr = eleptr->next_ele;
                    if (eleptr == NULL)
                        checked_eles = true;
                }
            }// end while elements

            if (checked_eles)   // if didn't find it yet
            {   // try to look at next node
                prevnode = nodeptr;
                nodeptr = nodeptr->next_node;
                if (nodeptr == NULL)
                    checked_nodes = true;
                else
                {
                    eleptr = nodeptr->first_ele;
                    prevele = NULL;
                    checked_eles = false;
                }
            }   
        }   // end while nodes

        RestartTimer (false);
    }
    return (foundit);
}


//----------------------------------------------------------------------------
// Process the timeout that just occurred
//----------------------------------------------------------------------------
void BcmLinuxTimeout::ProcessTimeout (void)
{
//	gInfoMsg(fMessageLogSettings, "ProcessTimeout")
//		<< "Entering ..."<< endl;

	if (fRoot == NULL)
	{
		gErrorMsg(fMessageLogSettings, "ProcessTimeout")
			 << "BcmLinuxTimeout::ProcessTimeout: Race condition."
             << " Attempting to process timeout that was previously cancelled." 
             << endl;
		return;
	}
//    fProcessingTimeout = true;

    // set the root node to the side to be processed
    // new timeouts may be added while I'm processing the root node - any
    // new additions should not consider the root I'm cleaning
    tonode_t * oldRoot = fRoot;
    fRoot = fRoot->next_node;

    toele_t * ele = oldRoot->first_ele;
    toele_t * nextele;

    while (ele != NULL)
    {
        switch (ele->classType)
        {
            case ltc_event:
            {
                BcmLnxEvent * evt = (BcmLnxEvent *)ele->objPtr;
                evt->Timeout();
            }
            break;
        
            case ltc_timer:
            {
                BcmLnxTimer * tmr = (BcmLnxTimer *)ele->objPtr;
                tmr->Timeout();
            }
            break;
        
            case ltc_evtset:
            {
                BcmLnxEventSet * evtset = (BcmLnxEventSet *)ele->objPtr;
                evtset->Timeout();
            }
            break;
        
            //default:
                // do nothing
        };

        nextele = ele->next_ele;
        ele = nextele;
    }

    // processed the root node, clean it up
    ele = oldRoot->first_ele;;
    while (ele!=NULL)
    {
            nextele = ele->next_ele;
            ReleaseElement (ele);
            ele=nextele;
    }
	gInfoMsg(fMessageLogSettings, "ProcessTimeout")
		<< "Releasing " << hex << oldRoot << endl;
    ReleaseNode (oldRoot);

    if (fRoot != NULL)
    {
        StartTimer (fRoot->msecs, true);    // start timer, record start time
		gInfoMsg(fMessageLogSettings, "ProcessTimeout")
			<< "fRoot!=NULL StartTimer " << dec << fRoot->msecs << " ms"<< endl;
    }
	else
	{
		gInfoMsg(fMessageLogSettings, "ProcessTimeout")
			<< "fRoot == NULL" << endl;
	}
//    fProcessingTimeout = false;
}

//----------------------------------------------------------------------------
// Called upon reception of timeout signal
// Note that this function is called in the context of the thread/process that
// started the timer.
//----------------------------------------------------------------------------
void BcmLinuxTimeout::LTOAlarmHandler (int signal_number)
{

//    cout << "LTOAlarmHandler: pid = " << getpid() 
//         << " tid = " << pthread_self() << endl;

    // find out which timeout object caused this
    pthread_t mytid = pthread_self();
    bool done = false; 
    for (int i = 0; !done && (i <= toHighWater); ++i)
    {
        if (fGrove[i].tid == mytid)
        {
            fGrove[i].toobj->ProcessTimeout();
            done = true;
        }
    }

    if(!done)
    {
        {
	        // looked at all objects without a match
            cout << "LTOAlarmHandler: err! in tid: " << pthread_self() << endl;
        }
    }
}



// ============================================================================
// ======== END ========== END ========== END ========== END ========== END ===
// ============================================================================

