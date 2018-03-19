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
//  Filename:       BatteryThread.cpp
//  Author:         Kevin O'Neal
//  Creation Date:  January 13, 2005
//
//****************************************************************************
//  Description:
//      Thread to manage battery operations
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "BatteryThread.h"
#include "BatteryNonVolSettings.h"
#include "NonVolDevice.h"
#include "BatteryCommandTable.h"

// Other objects of interest.
#include "CountingSemaphore.h"
#include "OperatingSystemFactory.h"
#include "SystemTime.h"

#include "MergedNonVolDefaults.h"
#include "MutexSemaphore.h"
#if BFC_INCLUDE_PMCAVS_SUPPORT
    #include "BfcPmcAvsManager.h"
#endif

#ifdef BFC_INCLUDE_THERMAL_MONITOR_SUPPORT
	#include "ThermalMonitorThread.h"		// PR 14749
#endif

#if (POWER_MGMT)
    #include "PowerManagementThread.h"
#endif

#if (BFC_DIAGNOSTICS_SUPPORT)
    #include "BfcDiagnosticsThread.h"
    #include "BatteryDiagnostic.h"
#endif


//********************** Local Types *****************************************

//********************** Local Constants *************************************
//#define BCM_ALLOC_TRACK

//********************** Local Variables *************************************
#if (!BCM_REDUCED_IMAGE_SIZE)
	static bool pvtLifeTestCommandsAdded = false;
#endif

BcmBatteryThread *BcmBatteryThread::pfSingleton = NULL;

//********************** Local Macros *************************************
#ifdef BCM_ALLOC_TRACK
// This macro does some GNU/MIPS-specific assembler stuff to read the return
// address from the current function.  On the MIPS (with GNU compiler/assembler)
// the return address is stored in R31/ra/$31.
//
// NOTE:  This only works for MIPS, and may only work for GNU compiler/assembler.
//        Hopefully any non-compatible system will result in a compile time
//        error notifying of this incompatibility.
#define ReadFunctionReturnAddress()                                                 \
__extension__                                                                       \
({                                                                                  \
    unsigned int __returnAddress;                                                   \
                                                                                    \
    /* Move the value stored in R31 to the variable __returnAddress, which */       \
    /* should be a register (not actually a memory address).               */       \
    /*    __returnAddress = R31 + R0 (which is hard wired to 0).           */       \
    __asm__ volatile ("add %0, $31, $0" : "=d" (__returnAddress));                  \
                                                                                    \
    /* This causes the value in __returnAddress to be returned from the macro */    \
    /* as though it were the return value from a function.                    */    \
    __returnAddress;                                                                \
})
#endif

// "C" callable API which is called in response to a battery power transition
// interrupt (for platforms which support this).
void BatteryTransitionDetected ()
{
    BcmBatteryThread *pBatteryThread = BcmBatteryThread::GetSingletonInstance();

    if (pBatteryThread)
    {
        pBatteryThread->BatteryTransitionDetected();
    }
}

//********************** Class Method Implementations ************************


BcmBatteryThread &BcmBatteryThread::Singleton()
{
  // PR 9604: Prevent race condition instantiating singleton object
  static BcmMutexSemaphore *pMutex = BcmOperatingSystemFactory::NewMutexSemaphore("BcmBatteryThread::Singleton mutex");
  BcmMutexSemaphoreAutoLock Lock(pMutex);
  
  if (pfSingleton == NULL)
    pfSingleton = new BcmBatteryThread ("Battery Management Thread");
    
  return *pfSingleton;
}


BcmBatteryThread *BcmBatteryThread::GetSingletonInstance()
{
  return pfSingleton;
}


// Default Constructor.  Starts the thread and waits for it to initialze.
// When the constructor returns, you can successfully manipulate the thread.
// Note that it is the most-derived class who actually spawns the thread and
// waits for it.
//
// You can set the thread priority as desired (defaults to normal) so that
// the thread consumes the appropriate amount of processor time.
//
// Parameters:
//      initialPriority - the priority that you want the thread to have.
//
// Returns:  N/A
//
BcmBatteryThread::BcmBatteryThread (const char *pName, BcmOperatingSystem::ThreadPriority initialPriority) :
    BcmThread(pName, false, initialPriority),
    fTimers(this)
{
  fMessageLogSettings.SetModuleName("BatteryThread");
//  gLogMessageRaw << "Battery Thread Constructor...." << endl;

  fExitThread = false;
  
  time_t Now;
  time (&Now);
  
  fLastPollTime = Now - GetSystemStartTime();
  fLastPowerSourceChangeTime = fLastPollTime;
  
  pfThreadStoppedSemaphore = NULL;
  
  fTransitionsToBattery = 0;
  fLogging = false;
  fProfileVoltage = 0;
  fProfileTime = 0;
  
  AddTimer(&fPollTimer);
  AddTimer(&fStateTimer);
  
//  gLogMessageRaw << "Leaving Battery thread constructor...." << endl;
#if BFC_INCLUDE_PMCAVS_SUPPORT
    BcmBfcPmcAvsManagerThread::Singleton();
#endif
}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Note that the event that was passed to us via SetNotificationEvent() will
// not be deleted!  It is the owner's responsibility to do so!
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmBatteryThread::~BcmBatteryThread()
{
  fExitThread = true;
  
  // Send a message to the thread to wake it up.
  if (pfBatteryQueue == NULL)
    return;
  
  pfBatteryQueue->Send(kStopThread);
  
  // Wait for the thread to exit
  WaitForThread();
}


void BcmBatteryThread::Start ()
{
  if (GetState() != BcmOperatingSystem::kNotStarted)
    return;
    
  // Initialize the battery hardware while in the caller's (probably startup)
  // thread context.
//  gLogMessageRaw << "Battery Thread Start: Initializing controller...." << endl;
  fBatteryController.Initialize();
//  gLogMessageRaw << "Battery Thread Start: Controller initialized...beginning thread" << endl;

  // PR 8764:  Bump stack size from 4k to 6k, we've seen one or two occasions
  // where we get down to only ~400 byte margin w/ 4k
  //
  // PR 13944, 13999:  Certain applications use more stack when responding to 
  // battery events within the context of this thread.  Increase stack to 16k.
  //
  // More analasys on PRs 13944 and 13999 shows 8k is sufficient.
  if (pfOperatingSystem->BeginThread(this, 1024 * 8) == false)
  {
    gFatalErrorMsg( fMessageLogSettings, "BcmBatteryThread" ) 
        << "Failed to spawn my thread!" << endl;
    assert(0);
  }
  
  #if (BFC_DIAGNOSTICS_SUPPORT)
  BfcDiagnosticsThread::Singleton().AddDiagnostic(new BcmBatteryDiagnostic());
  #endif
  
//  gLogMessageRaw << "Battery Thread Start:Arming poll timer...." << endl;
  
  /* Add timers */
  
  // Make the poll timer a one shot timer.  It will reset itself each time in 
  // its Fire method so we always have the latest timer val from NV
  int pollSeconds = BcmBatteryNonVolSettings::GetSingletonInstance()->PollIntervalSeconds();
  if(pollSeconds < 0 || pollSeconds > 120) pollSeconds = 5;	// reset default if corrupted
  fPollTimer.Arm (pollSeconds * 1000, BcmSoftTimer::kOneShot);
  
  // Don't arm the state timer just yet, it can cause problems right at
  // startup if we detect battery power too quickly.  We'll arm it on the 
  // first poll.
  // Make the state timer a periodic timer every 500 msecs
  // fStateTimer.Arm (500, BcmSoftTimer::kPeriodic);
//  gLogMessageRaw << "Battery Thread Start: Done." << endl;
}


bool BcmBatteryThread::Stop ()
{
  // Wait for the thread to reset.  This means that this call will block
  // until the thread has finished.
  pfThreadStoppedSemaphore->Get();
  pfThreadStoppedSemaphore->Release();
  
  return true;
}

// Thread constructor - this is the first method called after the thread has
// been spawned, and is where the thread should create all OS objects.  This
// has to be done here, rather than in the object's constructor, because
// some OS objects must be created in the context of the thread that will
// use them.  The object's constructor is still running in the context of
// the thread that created this object.
//
// Parameters:  None.
//
// Returns:
//      true if successful and ThreadMain() should be called.
//      false if there was a problem (couldn't create an OS object, etc.)
//          and ThreadMain() should not be called.
//
bool BcmBatteryThread::Initialize(void)
{
  gFuncEntryExitMsg(fMessageLogSettings, "Initialize") << "Entering..." << endl;
  
  pfBatteryQueue = pfOperatingSystem->NewMessageQueue ("BcmBatteryThread message queue");
  
  if (pfBatteryQueue == NULL)
    return false;
  
  pfThreadStoppedSemaphore = BcmOperatingSystemFactory::NewCountingSemaphore(true, 1, 1, "Thread Stopped Semaphore");

  if (pfThreadStoppedSemaphore == NULL)
  {
    gErrorMsg(fMessageLogSettings, "Initialize") << "Failed to create my semaphore!" << endl;
    return false;
  }
  
  if (fBatteryController.Initialized() == true)
    fEventPublisher.PublishEvent (kBatterySystemInitializedOK);
  else
    fEventPublisher.PublishEvent (kBatterySystemInitializationFailed);
  
  return true;
}


// Thread destructor - this is the last method called when the thread is
// exiting, and is where the thread should delete all of the OS objects that
// it created.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmBatteryThread::Deinitialize(void)
{
  gFuncEntryExitMsg(fMessageLogSettings, "Deinitialize") << "Entering..." << endl;

  // Get rid of the OS objects that I created.
  if (pfBatteryQueue)
    delete pfBatteryQueue;
    
  pfBatteryQueue = NULL;
  
  if (pfThreadStoppedSemaphore)
    delete pfThreadStoppedSemaphore;
    
  pfThreadStoppedSemaphore = NULL;
}


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
bool BcmBatteryThread::AddTimer (BcmSoftTimer *pTimer)
{
  return fTimers.AddTimer (pTimer);
}

bool BcmBatteryThread::RemoveTimer (BcmSoftTimer *pTimer)
{
  return fTimers.RemoveTimer (pTimer);
}


// SubscribeEventNote() - Subscribe for battery status event notification.
//
// Parameters:
//      EventCode - event code for battery status event of interest.
//                  (event codes declared in BatteryStatusEventCodes.h)
//
//      pACT - pointer to object derived from BcmCompletionHandlerACT. 
//          BcmCompletionHandlerACT implements the callback interface.  
//          The derived class implements HandleEvent() in a manner which
//          dispatches the event to the client object.
//
void BcmBatteryThread::SubscribeEventNote( unsigned int EventCode, BcmCompletionHandlerACT* pACT)
{
    CallTrace("BcmBatteryThread", "SubscribeEventNote");

    fEventPublisher.Subscribe( EventCode, pACT );
}


// UnSubscribeEventNote() - End subscription for CM DOCSIS status event notification.
//
// Parameters:
//      EventCode - event code for battery status event of interest.
//                  (event codes declared in BatteryStatusEventCodes.h)
//
//      pACT - pointer to object derived from BcmCompletionHandlerACT. 
//          BcmCompletionHandlerACT implements the callback interface.  
//          The derived class implements HandleEvent() in a manner which
//          dispatches the event to the client object.
//
void BcmBatteryThread::UnSubscribeEventNote (unsigned int EventCode, BcmCompletionHandlerACT* pACT)
{
    CallTrace("BcmBatteryThread", "UnSubscribeEventNote");

    fEventPublisher.UnSubscribe( EventCode, pACT );
}


/// Get power management mode.
/// \returns
///      The level of power savings to be invoked when a power management
///      event occurs.
BcmBatteryThread::PowerMgmtLevel BcmBatteryThread::PowerManagementLevel () const
{
  #if (POWER_MGMT)
  
  // Battery thread notion of power mgmt level is different from power mgmt
  // thread notion of level.  If we're not on reduced power, then return
  // 'full power'.
  if (BcmPowerManagementThread::Singleton().DesiredState() == BcmPowerManagementThread::kReducedPower)
  {
    // Translate from power mgmt thread enum values to battery thread
    // enum values.  In fact the PM thread values were chosen to be the 
    // same as the battery thread levels but still best to translate.
    if (BcmPowerManagementThread::Singleton().Level() == BcmPowerManagementThread::kMaximumPowerReduction)
      return kLowPowerMaximumReduction;
    else
      return kLowPowerModerateReduction;
  }
  
  #endif
  
  return kFullPowerNoReduction;
}

/// Set power management mode.  Setting the mode will result
/// in a kPowerMgmt event being dispatched to all interested subscribers.
/// Subscribers may then call the 'get' version of this method to query
/// the current mode and take appropriate action.
///
/// \param
///      Level - the level of power savings to be invoked when a power
///              management event occurs.
///
/// \returns
///      nothing.
///
/// The only entity that actually differentiates between kLowPowerMaximumReduction
/// and kLowPowerModerateReduction is the MTA.  It HAS to maintain two different
/// levels.
/// When we are on adapter power, a request to ENTER power manegement will be
/// ignored.  The BatteryController will call here during life testing, and will
/// pass TRUE in ignoreFD
void BcmBatteryThread::PowerManagementLevel (PowerMgmtLevel level, bool ignoreFD)
{
#ifdef BCM_ALLOC_TRACK
	unsigned int ra = ReadFunctionReturnAddress();
	gLogMessageRaw << "<=============BatteryThread::PowerManagementLevel being changed to " << level << " by 0x" << hex << ra << dec << endl;
#endif

    if( level == PowerManagementLevel())
	{
		  gInfoMsg(fMessageLogSettings, "PowerManagementLevel") << "Prev level = requested level = " << level << " - no action taken" << endl;
			return;
	}
	if (level > kFullPowerNoReduction )
	{
            #if (POWER_MGMT)
            // Translate from battery thread enum values to power mgmt thread
            // enum values.  In fact the PM thread values were chosen to be the 
            // same as the battery thread levels but still best to translate.
            BcmPowerManagementThread::PowerMgmtLevel PmLevel = BcmPowerManagementThread::kMaximumPowerReduction;
            
            if (level == kLowPowerModerateReduction)
              PmLevel = BcmPowerManagementThread::kModeratePowerReduction;
            #endif
            
			// publish the low power event if are on battery and NOT in forced discharge
			// (unless ignoreFD is true)
			if(	fBatteryController.IsInDischargeState()
			    && (ignoreFD || (fBatteryController.State() != kBCForcedDischarge )))
			{
				gInfoMsg(fMessageLogSettings, "PowerManagementLevel") << "Publishing kEnterPowerMgmt event for level " << level << " while on battery" << endl;
                
                #if (POWER_MGMT)
                // Set power mgmt thread level accordingly.  Do this before
                // dispatching the event.  That way the 'enter' event will
                // be processed with the new level, rather than it requiring
                // two passes (enter, then change level).
                BcmPowerManagementThread::Singleton().Level(PmLevel);
                #endif
                
				fEventPublisher.PublishEvent (kEnterPowerMgmt);
			}
			else if(fBatteryController.LowPowerRequiredDueToHighTemperature())
			{
				gInfoMsg(fMessageLogSettings, "PowerManagementLevel") << "Publishing kEnterPowerMgmt event for level " << level << " due to excessive temperature" << endl;
                
                #if (POWER_MGMT)
                // Set power mgmt thread level accordingly.  Do this before
                // dispatching the event.  That way the 'enter' event will
                // be processed with the new level, rather than it requiring
                // two passes (enter, then change level).
                BcmPowerManagementThread::Singleton().Level(PmLevel);
                #endif
                
				fEventPublisher.PublishEvent (kEnterPowerMgmt);
			}
			else
			{
				gInfoMsg(fMessageLogSettings, "PowerManagementLevel") << "On adapter power of in forced discharge - ignoring request for power reduction to level" << level << endl;
			}
	}
	else	// when going OUT of power management, ALWAYS publish the event
	{
		gInfoMsg(fMessageLogSettings, "PowerManagementLevel") << "Publishing kExitPowerMgmt event and returning to level " << level << endl;
		fEventPublisher.PublishEvent (kExitPowerMgmt);
	}
}

ostream &BcmBatteryThread::Print(ostream &outStream) const
{
  #if (!BCM_REDUCED_IMAGE_SIZE) || (BCM_TELNET_SUPPORT)
  time_t AdjustedPollTime = fLastPollTime + GetSystemStartTime();
  time_t AdjustedPowerChangeTime = fLastPowerSourceChangeTime + GetSystemStartTime();
  
  outStream << "Battery Thread Information:" << endl;
  outStream << "    Battery poll period: " << BcmBatteryNonVolSettings::GetSingletonInstance()->PollIntervalSeconds() << " seconds" << endl;
  outStream << "    Last battery poll: " << ctime (&AdjustedPollTime);
  outStream << "    Number of Transitions from utility to battery power: " << NumTransitionsToBattery() << endl;
  outStream << "    Operating on current power source since " << ctime (&AdjustedPowerChangeTime);
  
  if (PowerManagementLevel() == kFullPowerNoReduction)
    outStream << "    Power mgmt (when on battery power): full power / no reduction" << endl;
  else if (PowerManagementLevel() == kLowPowerModerateReduction)
    outStream << "    Power mgmt (when on battery power): low power / moderate reduction" << endl;
  else if (PowerManagementLevel() == kLowPowerMaximumReduction)
    outStream << "    Power mgmt (when on battery power): low power / maximum reduction" << endl;
    
  #endif
  
  return outStream;
}

ostream &BcmBatteryThread::PrintTimers(ostream &outStream) const
{
  outStream << fTimers << endl;
  return outStream;
}


/// Indicates the local time at which the system began using the power
/// source which is currently in use.
///
/// \returns
///     the local time at which the system began using the power
///     source which is currently in use.
///
time_t BcmBatteryThread::PowerSourceStartTime () const
{
  return fLastPowerSourceChangeTime + GetSystemStartTime();
}


/// Indicates the number of times which the system has transitioned from
/// utility power to battery power (note that reset under battery power
/// DOES constitute such a transition).
///
/// \returns
///     the number of times which the system has transitioned from
/// utility power to battery power
///
unsigned long BcmBatteryThread::NumTransitionsToBattery () const
{
  return fTransitionsToBattery;
}


// This method is called via battery ISR for platforms which support it.
bool BcmBatteryThread::BatteryTransitionDetected (void)
{
	if (pfBatteryQueue == NULL)
		return false;

	if (GetState() == BcmOperatingSystem::kNotStarted)
		return false;

	return pfBatteryQueue->Send(kBatteryTransitionDetected, NULL);
}


void BcmBatteryThread::BatteryTransitionDetectedImpl (void)
{
    gInfoMsg(fMessageLogSettings, "BatteryTransitionDetectedImpl") 
        << "Power source transition detected." << endl;

    // Special case: if we are in battery simulation mode via 'event batt'
    // then ignore this indication.
    if ((fBatteryController.fInfo.State == kBCDischarge)
    &&  (fStateTimer.Armed() == false)
    &&  (fPollTimer.Armed() == false))
    {
        gInfoMsg(fMessageLogSettings, "BatteryTransitionDetectedImpl")
            << "Battery simulation in progress - ignoring power source transition." << endl;
            
        return;
    }

	BcmBatteryThread::Singleton().BatteryController().UpdateState();
    BcmBatteryThread::Singleton().CheckForPowerSourceTransition();
}


bool BcmBatteryThread::SimulateTransitionToBatteryPower (bool Simulate)
{
  if (Simulate == true)
  {
    gLogMessageRaw << "Simulating switch to battery power.  *NOT* changing actual power source.\n" << endl;
                  
    // Inhibit polilng
    fStateTimer.Disarm();
    fPollTimer.Disarm();
    
    // Artificially set to "discharge"
    fBatteryController.fInfo.State = kBCDischarge;
                      
    // Check for power source transition, this will publish events
    CheckForPowerSourceTransition();
  }
  else
  {
    gLogMessageRaw << "Ending battery power simulation.  Restoring actual state.\n" << endl;
                                  
    // Restore polling.  State will be reset to actual state.
    PollBattery();
	int pollSeconds = BcmBatteryNonVolSettings::GetSingletonInstance()->PollIntervalSeconds();
	if(pollSeconds < 0 || pollSeconds > 120) pollSeconds = 5;	// reset if corrupted
    fPollTimer.Arm (pollSeconds * 1000, BcmSoftTimer::kOneShot);
  }
  
  return true;
}

/// Enable / disable console logging of battery data
///
bool BcmBatteryThread::LoggingEnabled () const
{
  return fLogging;
}

void BcmBatteryThread::EnableLogging (bool Enable)
{
  fLogging = Enable;
}


/// Enable / disable console discharge profiling
///
bool BcmBatteryThread::ProfilingEnabled () const
{
  if (fProfileVoltage)
    return true;
    
  return false;
}

void BcmBatteryThread::EnableProfiling (bool Enable)
{
  if (Enable)
  {
    // Initialize profile variables
    fProfileVoltage = 65535; // Make sure the first measurement is less than this
    time (&fProfileTime);
    fProfileTime -= GetSystemStartTime();
    
    gLogMessageRaw << "Beginning battery discharge profiling." << endl;
  }
  else
  {
    // Clear profile variables
    fProfileVoltage = 0;
    fProfileTime = 0;
  }
}


// Call the battery controller and update with the latest information
// from the hardware driver
//
void BcmBatteryThread::PollBattery ()
{
//  gInfoMsg(fMessageLogSettings, "PollBattery") << "Enter" << endl;
  time (&fLastPollTime);
  // Adjust last poll time by system start time.  This means we track
  // the time by number of seconds since system time instead of actual
  // time of day, which means we don't get confused if TOD or some other
  // mechanism changes the system time.
  fLastPollTime -= GetSystemStartTime();
#if (!BCM_REDUCED_IMAGE_SIZE)
  if(!pvtLifeTestCommandsAdded) {
	  BcmBatteryCommandTable *cmdTable = BcmBatteryCommandTable::Singleton();
	  if (cmdTable) {
		  if(fBatteryController.BspSupportsLifeTesting()) {
			  gInfoMsg(fMessageLogSettings, "PollBattery") << "  Adding Life Test console commands" << endl;
			  cmdTable->AddLifeTestCommands();
		  }
	  }
	  pvtLifeTestCommandsAdded = true;
  }
#endif
//  gInfoMsg(fMessageLogSettings, "PollBattery") << "  Updating battery" << endl;
  if (fBatteryController.Update() == false)
  {
    gErrorMsg(fMessageLogSettings, "PollBattery") << "Battery HW update failed!" << endl;
    return;
  }
  // every time we update status, we let the rest of the system know that this has happened
  RaiseEvent(kBatteryStatusUpdated);
  
//  gInfoMsg(fMessageLogSettings, "PollBattery") << "  Logging and profiling" << endl;  
  LogAndProfile ();
  
//  gInfoMsg(fMessageLogSettings, "PollBattery") << "Exit" << endl;
  
  // Make the state timer a periodic timer every 500 msecs.  We arm this here
  // instead of in the constructor because if we boot on battery power, we 
  // will try to raise an alarm before the system is ready for us and
  // wind up with a lockup.
  if (fStateTimer.Armed() == false)
    fStateTimer.Arm (500, BcmSoftTimer::kPeriodic);
  
}

// Report a power source change if we go from utility->battery or vice versa.
// We'll also raise/clear the upsAlarmOnBattery alarm based on this check.
void BcmBatteryThread::CheckForPowerSourceTransition ()
{
	static bool onBattery = false;
	static bool lowPower = false;

	if (fBatteryController.OperatingOnBattery() && !onBattery)
	{
		gLogMessageRaw << "Switch to battery power detected." << endl;
		
		onBattery = true;
		fLastPowerSourceChangeTime = fLastPollTime;
		fTransitionsToBattery++;
		
		// Full power is required in forced discharge.
		if(fBatteryController.State() != kBCForcedDischarge)
		{
			lowPower = true;
			gInfoMsg(fMessageLogSettings, "CheckForPowerSourceTransition") << "<-------------Publishing battery power event." << endl;
			fEventPublisher.PublishEvent (kSwitchingToBatteryPower);
			fEventPublisher.PublishEvent (kEnterPowerMgmt);
		}
		gInfoMsg(fMessageLogSettings, "CheckForPowerSourceTransition") << "<-------------Switch to battery power completed." << endl;
	}
	else if (!fBatteryController.OperatingOnBattery())
	{
		if( onBattery )
		{
			gLogMessageRaw << "Switch to utility power detected" << endl;
			onBattery = false;
			fLastPowerSourceChangeTime = fLastPollTime;
			
			// If the battery controller has not been successfully initialized,
			// retry now.  This is primarily for the benefit of Atmel-based designs.
			if (fBatteryController.Initialized() == false)
				fBatteryController.Initialize();
			gInfoMsg(fMessageLogSettings, "CheckForPowerSourceTransition") << "<-------------Publishing utility power event." << endl;
			fEventPublisher.PublishEvent (kSwitchingToUtilityPower);
		}
		// when not operating on battery, we need to see if a low power event (either 
		// raised or cleared) is required due to environmental temperature
		if (fBatteryController.LowPowerRequiredDueToHighTemperature())
		{
			// low power is required - raise the event of we are not already in low power
			if (!lowPower)
			{
					gInfoMsg(fMessageLogSettings, "CheckForPowerSourceTransition") << "<-------------Publishing low power event due to temperature." << endl;
					fEventPublisher.PublishEvent (kEnterPowerMgmt);
					lowPower = true;
			}
		}
		else 
		{
			// low power is NOT required - exit power management if currently operating in low power
			if (lowPower)
			{
				 gInfoMsg(fMessageLogSettings, "CheckForPowerSourceTransition") << "<-------------Publishing exit from low power event due to temperature." << endl;
				 fEventPublisher.PublishEvent (kExitPowerMgmt);
				 lowPower = false;
			}
		}
		//gInfoMsg(fMessageLogSettings, "CheckForPowerSourceTransition") << "<-------------Switch to utility power completed." << endl;
	}
}
      

void BcmBatteryThread::LogAndProfile ()
{
  char DisplayString[80];
  
  if (fLogging)
  {
    sprintf (DisplayString, "%08ld:", fLastPollTime);
    gLogMessageRaw << DisplayString;
    
    for (int Loop = 0; Loop < MAX_BATTERIES; Loop++)
    {
      if (fBatteryController.BatteryPresent(Loop) == false 
	  || fBatteryController.BatteryChargeStateUnknown(Loop) == true)
        continue;
    
      sprintf (DisplayString, "    V%c=%05d F%c=%d L%c=%d D%c=%d", 
      'A' + Loop, (int)fBatteryController.BatteryMeasuredVoltage(Loop), 
      'A' + Loop, fBatteryController.BatteryFullyCharged(Loop), 
      'A' + Loop, fBatteryController.BatteryChargeLow(Loop), 
      'A' + Loop, fBatteryController.BatteryChargeDepleted(Loop));
      gLogMessageRaw << DisplayString;
    }
    
    gLogMessageRaw << endl;
  }
  
  if (fProfileVoltage)
  {
    for (int Loop = 0; Loop < MAX_BATTERIES; Loop++)
    {
      if (fBatteryController.BatterySelected(Loop) == false)
        continue;
        
      int NewVoltage = fBatteryController.BatteryMeasuredVoltage(Loop);
      if (NewVoltage < fProfileVoltage)
      {
        sprintf (DisplayString, "%08ld:", fLastPollTime);
        gLogMessageRaw << DisplayString;
        
        sprintf (DisplayString, "  V%c=%05d C%c=%d%% ET=%d", 
        'A' + Loop, fBatteryController.BatteryMeasuredVoltage(Loop), 
        'A' + Loop, fBatteryController.BatteryPercentCharge(Loop), 
        fBatteryController.EstimatedMinutesRemaining() * 60);
        gLogMessageRaw << DisplayString << endl;
        
        fProfileVoltage = NewVoltage;
        fProfileTime = fLastPollTime;
      }
    }
  }
}

void BcmBatteryThread::RaiseEvent(int event)
{
	//gInfoMsg(fMessageLogSettings, "RaiseEvent") << "<------------Battery thread raising event: " << event << endl;
	fEventPublisher.PublishEvent(event);
}


bool BcmBatteryThread::UpdateTimers (void)
{
	if (pfBatteryQueue == NULL)
		return false;

	if (GetState() == BcmOperatingSystem::kNotStarted)
		return false;

	// If our NV setting for poll time has changed, re-arm the poll timer.
	unsigned long NvPollMsecs = BcmBatteryNonVolSettings::GetSingletonInstance()->PollIntervalSeconds() * 1000;
    if (NvPollMsecs <= 120 && fPollTimer.TimeoutMS() != NvPollMsecs) 
    {
        fPollTimer.Arm (NvPollMsecs, BcmSoftTimer::kOneShot);
        fBatteryController.UpdateMaxHistory(NvPollMsecs/1000);
    }

	return pfBatteryQueue->Send(kUpdateTimers, NULL);
}


// This is the main body of the thread's code.  This is the only method
// that absolutely must be provided in the derived class (since there is no
// reasonable default behavior).
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmBatteryThread::ThreadMain(void)
{
  unsigned int MsgCode;
  void *pMsgBuf = NULL;


  gFuncEntryExitMsg(fMessageLogSettings, "ThreadMain") << "Entering..." << endl;
  
  if (pfBatteryQueue == NULL)
    return;
    
  while (1)
  {
    //gInfoMsg(fMessageLogSettings, "ThreadMain") << "Waiting for someone to post my event." << endl;

    // Wait for a message to arrive on the queue
    if (pfBatteryQueue->Receive (MsgCode, pMsgBuf, BcmMessageQueue::kTimeout, fTimers.GetTimeout()) == true)
    {
      // Process my events here
      switch (MsgCode)
      {
        case kUpdateTimers:
          // Nothing to do other than break out of our wait above.  We can
          // go ahead and fall through to ProcessTimers.
          break;

        case kBatteryTransitionDetected:
          BatteryTransitionDetectedImpl();
          break;
      
        default:
          break;
      }
    }
    
    // If I'm being told to exit the thread, then do so.
    if (fExitThread)
      break;
      
    fTimers.ProcessTimers();
  }

  gFuncEntryExitMsg(fMessageLogSettings, "ThreadMain") << "Exiting..." << endl;
}


/// Accessor for the thread's BcmBatteryController object
///
/// \returns
///      a reference to the fBatteryController data member
BcmBatteryController &BcmBatteryThread::BatteryController (void)
{
  return fBatteryController;
}

