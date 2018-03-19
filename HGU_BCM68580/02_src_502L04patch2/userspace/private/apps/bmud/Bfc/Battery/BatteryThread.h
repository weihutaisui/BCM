//****************************************************************************
//
// Copyright (c) 2005-2013 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
//
//****************************************************************************
//  $Id$
//
//  Filename:       BatteryThread.h
//  Author:         Kevin O'Neal
//  Creation Date:  January 13, 2005
//
//****************************************************************************
//  Description:
//      Thread to manage battery operations
//
//****************************************************************************

#ifndef BATTERYTHREAD_H
#define BATTERYTHREAD_H

// "C" callable API's
#if defined (__cplusplus)
extern "C"
{
#endif

void BatteryTransitionDetected ();

#if defined (__cplusplus)
}
#endif

#if defined (__cplusplus)

//********************** Include Files ***************************************

#include "Thread.h"
#include "MessageQueue.h"
#include "EventPublisher.h"

#include "BcmString.h"
#include <time.h>
#include "BatteryController.h"
//#include "ObjectIdentifier.h"
#include "BatterySoftTimer.h"


//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


class BcmBatteryThread : public BcmThread
{
public:
    /// Singleton accessor
    ///
    static BcmBatteryThread &Singleton ();

    /// A different way of obtaining the singleton instance.  This method
    /// returns the singleton pointer directly and does not lock a mutex.
    /// This makes it usable in ISR context.
    ///
    static BcmBatteryThread *GetSingletonInstance();
    
    /* public API's: */
    
    /// Start the thread.
    ///
    void Start ();
    
    /// Stop the thread.
    ///
    /// \retval
    ///     true if the thread was successfully stopped.
    /// \retval
    ///     false if the thread could not be stopped.
    bool Stop ();
    
    /// Add or remove a soft timer
    ///
    /// \param
    ///      pTimer - Timer to add or remove
    ///
    /// \retval
    ///      true if the timer was successfully added or removed
    /// \retval
    ///      false if the timer was not successfully added or removed
    ///
    bool AddTimer (BcmSoftTimer *pTimer);
    bool RemoveTimer (BcmSoftTimer *pTimer);
    
    /// Print thread info.
    ///
    /// \param
    ///      outStream - the ostream to which the packet should be printed.
    ///
    /// \return
    ///      A reference to the ostream to support operation chaining.
    ///
    ostream &Print (ostream &outStream) const;
    
    /// Print soft timer list.
    ///
    /// \param
    ///      outStream - the ostream to which the packet should be printed.
    ///
    /// \return
    ///      A reference to the ostream to support operation chaining.
    ///
    ostream &PrintTimers (ostream &outStream) const;
    
    /// Events which we publish for interested subscribers
    typedef enum
    {
      kBatterySystemInitializedOK = 0, // BatteryController::Initialize call succeeded
      kBatterySystemInitializationFailed, // BatteryController::Initialize call failed
      kBatteryStatusUpdated, // Called each time the battery is updated
      kSwitchingToBatteryPower, // Transition from utility (A/C) -> battery
      kSwitchingToUtilityPower, // Transition from battery -> utility (A/C)
      kBatteryLow, // Time remaining dropped below threshold
      kBatteryNotLow, // Time remaining raised above threshold
      kBatteryLowPercent, // Low by percentage rather than time (PR ????)
      kBatteryNotLowPercent,
      kBatteryDepleted,
      kBatteryNotDepleted,
      kBatteryBad, // Transition from battery OK to battery bad
      kBatteryOK,  // Transition from battery bad to battery good
      kBatteryMissing, // Transition from battery present to battery missing
      kBatteryPresent, // Transition from battery missing to battery present
      kEnterPowerMgmt,
      kExitPowerMgmt,
      kBatteryOutputOff,
      kBatteryOutputOn,
      
      /* Various temperature thresholds exceeded */
	  kBatteryChargeTemperatureDisabled,
	  kBatteryChargeTemperatureNotDisabled,
	  kBatteryDischargeTemperatureDisabled,
	  kBatteryDischargeTemperatureNotDisabled,
	  kBatteryTemperatureOOB,
  	  kBatteryTemperatureNotOOB,
      kLowPowerHiTemperature,
      kNotLowPowerHiTemperature,

	  // Hardware fault-related events
	  kHardwareFaultDetected,
	  kHardwareFaultNotDetected
    } EventCodes;
    
	enum AlarmConstants {
		kChargeTemperatureInhibitAlarm,
		kChargeTemperatureOKAlarm,
		kDischargeTemperatureInhibitAlarm,
		kDischargeTemperatureOKAlarm,
		kTemperatureLowPowerAlarm,
		kTemperatureNormalPowerAlarm,
		kBadTemperatureAlarm,
		kNotBadTemperatureAlarm,
	};

	/// Accessor for the thread's state, see StateBits bitmask.
    ///
    /// \returns
    ///      a bitmask indicating the state.
    ///
    unsigned long State () const;
    
    /// Accessor for the thread's BcmBatteryController object
    ///
    /// \returns
    ///      a reference to the fBatteryController data member
    ///
    BcmBatteryController &BatteryController (void);
    
    /// Subscribe for battery status event notification.
    ///
    /// \param
    ///      event_code - event code for battery status event of interest.
    ///                  (event codes declared in BatteryStatusEventCodes.h)
    ///
    /// \param
    ///      pACT - pointer to object derived from BcmCompletionHandlerACT. 
    ///          BcmCompletionHandlerACT implements the callback interface.  
    ///          The derived class implements HandleEvent() in a manner which
    ///          dispatches the event to the client object.
    ///
    void SubscribeEventNote (unsigned int EventCode, BcmCompletionHandlerACT* pACT);
    
    ///
    /// End subscription for battery status event notification.
    ///
    /// \param
    ///      event_code - event code for battery status event of interest.
    ///                  (event codes declared in BatteryStatusEventCodes.h)
    ///
    /// \param
    ///      pACT - pointer to object derived from BcmCompletionHandlerACT. 
    ///          BcmCompletionHandlerACT implements the callback interface.  
    ///          The derived class implements HandleEvent() in a manner which
    ///          dispatches the event to the client object.
    ///
    void UnSubscribeEventNote (unsigned int EventCode, BcmCompletionHandlerACT* pACT);
    
    /// Indicates the local time at which the system began using the power
    /// source which is currently in use.
    ///
    /// \returns
    ///     the local time at which the system began using the power
    ///     source which is currently in use.
    ///
    time_t PowerSourceStartTime () const;
    
    /// Indicates the number of times which the system has transitioned from
    /// utility power to battery power (note that reset under battery power
    /// DOES constitute such a transition).
    ///
    /// \returns
    ///     the number of times which the system has transitioned from
    ///     utility power to battery power
    ///
    unsigned long NumTransitionsToBattery () const;
    
    bool BatteryTransitionDetected (void);
    bool SimulateTransitionToBatteryPower (bool Simulate);
    
protected:
    
    /// Power management modes
    typedef enum
    {
      kFullPowerNoReduction = 0,
      kLowPowerModerateReduction,
      kLowPowerMaximumReduction
    } PowerMgmtLevel;
    
    /// Get/Sets the power management level.
    /// \returns
    ///      The level of power savings to be invoked when a power management
    ///      event occurs.
    PowerMgmtLevel PowerManagementLevel () const;
	void PowerManagementLevel (PowerMgmtLevel level, bool ignoreFD = false);
    
    /// Default Constructor.  Starts the thread and waits for it to initialze.
    /// When the constructor returns, you can successfully manipulate the thread.
    /// Note that it is the most-derived class who actually spawns the thread and
    /// waits for it.
    ///
    /// You can set the thread priority as desired (defaults to normal) so that
    /// the thread consumes the appropriate amount of processor time.
    ///
    /// \param
    ///      initialPriority - the priority that you want the thread to have.
    ///
    BcmBatteryThread (const char *pName, BcmOperatingSystem::ThreadPriority initialPriority = BcmOperatingSystem::kNormalPriority);
    
    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    /// Note that the event that was passed to us via SetNotificationEvent() will
    /// not be deleted!  It is the owner's responsibility to do so!
    ///
    virtual ~BcmBatteryThread ();
    
    /// Thread constructor - this is the first method called after the thread has
    /// been spawned, and is where the thread should create all OS objects.  This
    /// has to be done here, rather than in the object's constructor, because
    /// some OS objects must be created in the context of the thread that will
    /// use them.  The object's constructor is still running in the context of
    /// the thread that created this object.
    ///
    /// \retval
    ///      true if successful and ThreadMain() should be called.
    /// \retval
    ///      false if there was a problem (couldn't create an OS object, etc.)
    ///          and ThreadMain() should not be called.
    ///
    virtual bool Initialize (void);

    /// Thread destructor - this is the last method called when the thread is
    /// exiting, and is where the thread should delete all of the OS objects that
    /// it created.
    ///
    virtual void Deinitialize (void);
    
    bool UpdateTimers (void);
    void BatteryTransitionDetectedImpl (void);
    
    /// This is the main body of the thread's code.  This is the only method
    /// that absolutely must be provided in the derived class (since there is no
    /// reasonable default behavior).
    ///
    virtual void ThreadMain (void);
    
    /// Enable / disable console logging of battery data
    ///
    bool LoggingEnabled () const;
    void EnableLogging (bool Enable);
    
    /// Enable / disable console discharge profiling
    ///
    bool ProfilingEnabled () const;
    void EnableProfiling (bool Enable);
    
    /// Call the battery controller and update with the latest information
    /// from the hardware driver
    ///
    void PollBattery ();
    
    /// Check to see if we have just transitioned from utility to battery
    /// or vice versa and take appropriate action if we have.
    ///
    void CheckForPowerSourceTransition ();
    
    /// Do console logging and/or profiling if those are enabled
    ///
    void LogAndProfile ();
    
protected:
    static BcmBatteryThread *pfSingleton;

    /// Tells the thread when to exit (set in the destructor).
    bool fExitThread;
    
    /// Message queue for the battery thread
    BcmMessageQueue *pfBatteryQueue;
    
    /// Used by Stop() to determine when the thread has responded to
    /// the request to stop.
    BcmCountingSemaphore *pfThreadStoppedSemaphore;
    
    /// Event publisher
    BcmEventPublisher fEventPublisher;
    
    /// Timer list
    BatterySoftTimerList fTimers;
    
    /// Timer for battery poll
    BatteryPollSoftTimer fPollTimer;
    
    /// Timer for battery state
    BatteryStateSoftTimer fStateTimer;
    
    /// HW interface object
    BcmBatteryController fBatteryController;
    
    /// Last battery poll
    time_t fLastPollTime;
    
    /// Time we switched battery / utility power
    time_t fLastPowerSourceChangeTime;
    
    /// Flag to log or not to log
    bool fLogging;
    
    /// For discharge profiling
    int fProfileVoltage;
    time_t fProfileTime;
    
    /// Number of times we've gone from utility->battery power
    unsigned long fTransitionsToBattery;
    
    /// Message codes
    enum
    {
      kStartThread = 0,
      kStopThread,
      kUpdateTimers,
      kBatteryTransitionDetected
    };
    
    // Message payloads

private:

    /// Copy Constructor.  Not supported.
    ///
    /// \param
    ///      otherInstance - reference to the object to copy.
    ///
    BcmBatteryThread(const BcmBatteryThread &otherInstance);

    /// Assignment operator.  Not supported.
    ///
    /// \param
    ///      otherInstance - reference to the object to copy.
    ///
    BcmBatteryThread & operator = (const BcmBatteryThread &otherInstance);

	/// Publish an event on behalf of the controller
	void RaiseEvent(int event);

    friend class BcmBatteryCommandTable;
    friend class BcmBatteryATECommandTable;
    friend class BcmBatteryController;
    friend class BcmBatteryNonVolSettings;
    friend class BatterySoftTimerList;
    friend class BatteryPollSoftTimer;
    friend class BatteryStateSoftTimer;
    friend class batteryEngrBaseGroupBridge;
};


//********************** Inline Method Implementations ***********************

inline ostream & operator << (ostream &outStream, const BcmBatteryThread &Thread)
{
  return Thread.Print(outStream);
}

#endif

#endif
