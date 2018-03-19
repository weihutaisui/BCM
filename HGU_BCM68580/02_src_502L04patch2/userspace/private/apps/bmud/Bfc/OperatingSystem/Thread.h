//****************************************************************************
//
//  Copyright (c) 1999  Broadcom Corporation
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
//  Filename:       Thread.h
//  Author:         David Pullen
//  Creation Date:  Feb 18, 1999
//
//****************************************************************************

#ifndef THREAD_H
#define THREAD_H

//********************** Include Files ***************************************

#include "OperatingSystem.h"

#include "MessageLog.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

//********************** Class Declaration ***********************************


/** \ingroup OsWrappers
*
*   This is the base class from which all application threads are derived.
*   It handles most of the guts of creating and starting a new thread in an
*   OS-agnostic way, and provides common storage and functionality required
*   by all threads.
*
*   A thread (or task, as it is called in some operating systems) is the
*   basic unit of execution in the system.  All systems have at least one
*   thread, even if it's just the OS startup thread.  If a system is
*   sufficiently complex, then it may be desirable to subdivide it into a
*   number of separate threads that run independently (or that coordinate
*   with each other).  In this case, the application's root thread may just
*   create and start all of the other threads, and then suspend itself.
*
*   Note that threads are not OS-specific; they are application-specific,
*   which allows them to be portable and OS-agnostic.  As a result, there
*   are no OS-specific derived classes here like there are for the other
*   OS Wrapper classes.  The OS functions that control thread attributes
*   and state are delegated to another class (BcmOperatingSystem), which
*   does have OS-specific subclasses.
*
*   Threads maintain their independence from any particular OS by only
*   making use of the abstract base classes of objects like BcmEvent, 
*   BcmTimer, etc.
*/
class BcmThread
{
public:

    /// Constructor.  Initializes the state of the object, acquiring a
    /// BcmOperatingSystem instance for later use, and setting up required state
    /// information.
    ///
    /// \param
    ///      pThreadName - the name of this thread (for debugging purposes).
    /// \param
    ///      startSuspended - set to true if the thread should suspend itself
    ///                       after initializing, or false if it should run.
    /// \param
    ///      initialPriority - the thread priority that should be used.
    ///
    BcmThread(const char *pThreadName, bool startSuspended = false,
              BcmOperatingSystem::ThreadPriority initialPriority = BcmOperatingSystem::kNormalPriority);

    /// Destructor.  If the thread is still running, it will be stopped, allowing
    /// OS objects to be destroyed (note - this is actually done by the derived
    /// class when it calls WaitForThread()).
    ///
    /// Note that WaitForThread() MUST be called by the destructor of the derived
    /// class rather than here!  This is because once the base class destructor
    /// is called, the data members and virtual methods in the derived class are
    /// no longer valid!
    ///
    virtual ~BcmThread();

    /// -----------------------------------------------------------------------
    /// EXTERNAL THREAD CONTROL METHODS:
    ///      These methods allow any thread to control my state.
    /// -----------------------------------------------------------------------

    /// Causes the thread to stop executing (blocked) until Resume() is called.
    /// A thread can suspend itself or another thread, though it cannot resume
    /// itself - some other thread must release it to run again.
    ///
    /// This only works if the thread's current state is kRunning.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    inline bool Suspend(void);

    /// Causes the thread to be released from its suspended state.  Note that it
    /// may or may not begin running immediately upon being released, depending
    /// on its priority level.
    ///
    /// This only works if the thread's current state is kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    inline bool Resume(void);

    /// NOTE:  I'm having a hard time implementing these two methods in a uniform
    ///        way on all operating systems.  In particular, Win32 doesn't allow
    ///        one thread to do these to another thread cleanly.  It only allows
    ///        a thread to terminate itself.
    ///
    ///        For now, I will disallow the use of these methods until someone
    ///        can prove that they are necessary.  Then I'll worry about how to
    ///        implement them in Win32.
    
    /// Causes a thread to be terminated in its tracks.  This is generally not
    /// recommended because it may not give the thread a chance to free memory
    /// and other resources that were allocated.  You should only call this under
    /// catostrophic conditions where the system needs to be shut down or there
    /// is a rogue thread that won't respond to events, etc.
    ///
    /// Unfinished...this may not be true.  I suspect that this can be done
    /// gracefully by setting an internal state and restarting the thread, which
    /// causes it to de-init and exit.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (what would cause this?).
    ///
    ///inline bool Terminate(void);

    /// Allows a thread that has exited to be restarted.  In general, we don't
    /// recommend this - instead, delete the original thread, allowing it to
    /// exit gracefully, then create a new one.
    ///
    /// Unfinished...this may not be true.  I suspect that this can be done
    /// gracefully by setting an internal state and restarting the thread, which
    /// causes it to de-init, then start over again.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (what would cause this?).
    ///
    ///inline bool Restart(void);

    /// Sets the priority of the thread to the value specified.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \param
    ///      priority - the new thread priority to use.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    inline bool SetPriority(BcmOperatingSystem::ThreadPriority priority);

    /// Increases the priority of the thread by one level, if possible.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    inline bool IncreasePriority(void);

    /// Decreases the priority of the thread by one level, if possible.
    ///
    /// This only works if the thread's current state is kRunning or kSuspended.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem.
    ///
    inline bool DecreasePriority(void);

    /// For a CPU that has multiple cores (such as BMIPS4350), and an OS that
    /// supports SMP (such as eCos), this method allows you to set the CPU
    /// affinity for the thread.
    ///
    /// By default, a thread has its affinity set to the "primary" or master
    /// CPU.  It can be useful to move a thread to the "secondary" CPU, though
    /// doing so introduces another level of reentrancy issues that have to
    /// be addressed, so this should be done only after careful consideration.
    ///
    /// A thread can be configured to run on multiple CPUs; again, this should
    /// only be done after careful consideration.
    ///
    /// \note Not all OSs support SMP; this base class provides a dummy
    ///       implementation for these OSs which prints a warning and returns
    ///       false.
    ///
    /// Added for PR11084.
    ///
    /// \param
    ///     cpuAffinityMask - a bitmask indicating which CPUs the thread should
    ///                       be allowed to run on.  Bit 0 is always the primary
    ///                       CPU.  Use the kThreadAffinity... constants from
    ///                       OperatingSystem.h for this parameter.
    ///
    /// \retval
    ///     true if the CPU affinity was set.
    /// \retval
    ///     false if the CPU affinity was not set (CPU doesn't have multiple
    ///     cores, OS doesn't support SMP, invalid bitmask, etc).
    ///
    inline bool SetCpuAffinity(uint32 cpuAffinityMask);

    /// Returns the current state of the thread (running, suspended, etc.).
    ///
    /// \return
    ///      The thread's current state.
    ///
    inline BcmOperatingSystem::ThreadState GetState(void) const;

    /// Returns the name of the thread (for debugging purposes).
    ///
    /// \return
    ///      The thread name.
    ///
    inline const char *Name(void) const;

protected:

    /// This method MUST be called by the destructor of the most derived class!
    /// It does all of the real work to ensure that the thread has exited
    /// correctly and has had a chance to delete all of its OS objects.
    ///
    void WaitForThread(void);
    
    /// ========================================================================
    /// These methods are called by the BcmThread base class (ThreadEntrypoint())
    /// once the thread has been spawned.
    /// ========================================================================

    /// Thread constructor - this is the first method called after the thread has
    /// been spawned, and is where the thread should create all OS objects.  This
    /// has to be done here, rather than in the object's constructor, because
    /// some OS objects must be created in the context of the thread that will
    /// use them.  The object's constructor is still running in the context of
    /// the thread that created this object.
    ///
    /// The default implementation simply returns true, allowing derived classes
    /// that don't have any initialization to use the default.
    ///
    /// \retval
    ///      true if successful and ThreadMain() should be called.
    /// \retval
    ///      false if there was a problem (couldn't create an OS object, etc.)
    ///          and ThreadMain() should not be called.
    ///
    virtual bool Initialize(void);

    /// This is the main body of the thread's code.  This is the only method
    /// that absolutely must be provided in the derived class (since there is no
    /// reasonable default behavior).
    ///
    virtual void ThreadMain(void) = 0;

    /// Thread destructor - this is the last method called when the thread is
    /// exiting, and is where the thread should delete all of the OS objects that
    /// it created.
    ///
    /// The default implementation does nothing, allowing derived classes that
    /// don't have any deinitialization to use the default.
    ///
    virtual void Deinitialize(void);

protected:

    /// The instance of the operating system that should be used when creating
    /// OS components (events, semaphores, etc.).  This is created for you
    /// in the base class constructor, and deleted in the base class destructor.
    ///
    /// NOTE:  This pointer should not be accessible beyond the scope of the
    ///        thread that owns it!  You don't want other threads/objects to have
    ///        absolute control over your thread's state!
    BcmOperatingSystem *pfOperatingSystem;

    /// This controls the level of message log output.
    BcmMessageLogSettings fMessageLogSettings;

private:

    /// The derived class' name.
    char *pfThreadName;

    /// Used when the thread starts up.
    bool fStartSuspended;
    BcmOperatingSystem::ThreadPriority fInitialPriority;

    /// Tells ThreadEntrypoint() how to behave.
    BcmOperatingSystem::ThreadEntrypointMode fThreadEntrypointMode;

public:

    /// ========================================================================
    /// NOTE:  These methods should only be called by BcmOperatingSystem (and
    ///        derived classes)!
    /// ========================================================================

    /// This is the main thread entrypoint that is called by the
    /// BcmOperatingSystem once the thread has started.  It handles basic thread
    /// setup and configuration, calling the Initialize(), ThreadMain(), and
    /// Deinitialize() methods.  It sets the initial thread priority, and
    /// suspends the thread after calling Initialize() (if startSuspended was
    /// set to true in the constructor).
    ///
    /// When this method exits, the thread has been cleaned up and stopped.
    ///
    /// \param
    ///      pSemaphore - pointer to an optional counting semaphore.  The thread
    ///                   will call Release() once it has finished initializing.
    ///
    void ThreadEntrypoint(BcmCountingSemaphore *pSemaphore = NULL);

    /// This function allows the BcmOperatingSystem instance to set the mode
    /// characteristics for ThreadEntrypoint().
    ///
    /// NOTE:  This method should only be called by BcmOperatingSystem!
    ///
    /// \param
    ///      mode - the mode that should be used.
    ///
    inline void SetThreadEntrypointMode(BcmOperatingSystem::ThreadEntrypointMode mode);

    /// This function retrieves the ThreadEntrypoint mode.
    ///
    /// \return
    ///      The current mode.
    ///
    inline BcmOperatingSystem::ThreadEntrypointMode GetThreadEntrypointMode(void) const;

    /// This function retrieves the "start suspended" mode.
    ///
    /// \return
    ///      The start suspended flag.
    ///
    inline bool StartingSuspended(void) const;

private:

    /// Default Constructor.  Not supported.
    BcmThread(void);

    /// Copy Constructor.  Not supported.
    BcmThread(const BcmThread &otherInstance);

    /// Assignment operator.  Not supported.
    BcmThread & operator = (const BcmThread &otherInstance);

};


//********************** Inline Method Implementations ***********************


/// Causes the thread to stop executing (blocked) until Resume() is called.
/// A thread can suspend itself or another thread, though it cannot resume
/// itself - some other thread must release it to run again.
///
/// This only works if the thread's current state is kRunning.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem.
///
inline bool BcmThread::Suspend(void)
{
    return pfOperatingSystem->Suspend();
}


/// Causes the thread to be released from its suspended state.  Note that it
/// may or may not begin running immediately upon being released, depending
/// on its priority level.
///
/// This only works if the thread's current state is kSuspended.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem.
///
inline bool BcmThread::Resume(void)
{
    return pfOperatingSystem->Resume();
}


/// Causes a thread to be terminated in its tracks.  This is generally not
/// recommended because it may not give the thread a chance to free memory
/// and other resources that were allocated.  You should only call this under
/// catostrophic conditions where the system needs to be shut down or there
/// is a rogue thread that won't respond to events, etc.
///
/// Unfinished...this may not be true.  I suspect that this can be done
/// gracefully by setting an internal state and restarting the thread, which
/// causes it to de-init and exit.
///
/// This only works if the thread's current state is kRunning or kSuspended.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (what would cause this?).
///
/*inline bool BcmThread::Terminate(void)
{
    return pfOperatingSystem->Terminate();
}*/


/// Allows a thread that has exited to be restarted.  In general, we don't
/// recommend this - instead, delete the original thread, allowing it to
/// exit gracefully, then create a new one.
///
/// Unfinished...this may not be true.  I suspect that this can be done
/// gracefully by setting an internal state and restarting the thread, which
/// causes it to de-init, then start over again.
///
/// This only works if the thread's current state is kRunning or kSuspended.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem (what would cause this?).
///
/*inline bool BcmThread::Restart(void)
{
    return pfOperatingSystem->Restart();
}*/


/// Sets the priority of the thread to the value specified.
///
/// This only works if the thread's current state is kRunning or kSuspended.
///
/// \param
///      priority - the new thread priority to use.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem.
///
inline bool BcmThread::SetPriority(BcmOperatingSystem::ThreadPriority priority)
{
    return pfOperatingSystem->SetPriority(priority);
}


/// Increases the priority of the thread by one level, if possible.
///
/// This only works if the thread's current state is kRunning or kSuspended.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem.
///
inline bool BcmThread::IncreasePriority(void)
{
    return pfOperatingSystem->IncreasePriority();
}


/// Decreases the priority of the thread by one level, if possible.
///
/// This only works if the thread's current state is kRunning or kSuspended.
///
/// \retval
///      true if successful.
/// \retval
///      false if there was a problem.
///
inline bool BcmThread::DecreasePriority(void)
{
    return pfOperatingSystem->DecreasePriority();
}


// Added for PR11084.
inline bool BcmThread::SetCpuAffinity(uint32 cpuAffinityMask)
{
    return pfOperatingSystem->SetCpuAffinity(cpuAffinityMask);
}


/// Returns the current state of the thread.
///
/// \return
///      The thread's current state.
///
inline BcmOperatingSystem::ThreadState BcmThread::GetState(void) const
{
    return pfOperatingSystem->GetThreadState();
}


/// Returns the name of the thread (for debugging purposes).
///
/// \return
///      The thread name.
///
inline const char *BcmThread::Name(void) const
{
    return pfThreadName;
}


/// This function allows the BcmOperatingSystem instance to set the mode
/// characteristics for ThreadEntrypoint().
///
/// NOTE:  This method should only be called by BcmOperatingSystem!
///
/// \param
///      mode - the mode that should be used.
///
inline void BcmThread::SetThreadEntrypointMode(BcmOperatingSystem::ThreadEntrypointMode mode)
{
    fThreadEntrypointMode = mode;
}


/// This function retrieves the ThreadEntrypoint mode.
///
/// \return
///      The current mode.
///
inline BcmOperatingSystem::ThreadEntrypointMode BcmThread::GetThreadEntrypointMode(void) const
{
    return fThreadEntrypointMode;
}


/// This function retrieves the "start suspended" mode.
///
/// \return
///      The start suspended flag.
///
inline bool BcmThread::StartingSuspended(void) const
{
    return fStartSuspended;
}


#endif


