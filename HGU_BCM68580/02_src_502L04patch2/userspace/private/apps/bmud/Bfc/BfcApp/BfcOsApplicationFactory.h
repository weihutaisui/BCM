//****************************************************************************
//
//  Copyright (c) 2003-2004  Broadcom Corporation
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
//  Filename:       BfcOsApplicationFactory.h
//  Author:         David Pullen
//  Creation Date:  October 27, 2003
//
//****************************************************************************

#ifndef BfcOsApplicationFactory_H
#define BfcOsApplicationFactory_H

//********************** Include Files ***************************************

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

class BcmBfcCompositeApplication;

//********************** Class Declaration ***********************************


/** \ingroup BfcCore BfcPerformance
*
*   This file contains the BFC App factory API that must be implemented
*   in order for OS-specific applications to be included in the BFC
*   framework.  The .cpp file containing the implementation will be in some
*   OS-specific directory.
*/
class BcmBfcOsApplicationFactory
{
public:

    /// This method should create OS-specific application object(s) and register
    /// them with the composite BFC application object that is specified as the
    /// parameter.
    ///
    /// The composite becomes the owner of the application object(s), and will
    /// delete them when the composite is deleted.
    ///
    /// \param
    ///      pCompositeApplication - the composite BFC application object that
    ///                              you should register your application objects
    ///                              with.
    ///
    /// \retval
    ///      true if the applications were created and registered.
    /// \retval
    ///      false if there was a problem.
    ///
    static bool CreateAndRegisterBfcApplications(BcmBfcCompositeApplication *pCompositeApplication);

protected:  // This is a factory class; the constructor and destructor are not
            // meant to be called.

    /// Default Constructor.  Initializes the state of the object...
    ///
    BcmBfcOsApplicationFactory(void);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    ~BcmBfcOsApplicationFactory();

private:

    /// Copy Constructor.  Not supported.
    BcmBfcOsApplicationFactory(const BcmBfcOsApplicationFactory &otherInstance);

    /// Assignment operator.  Not supported.
    BcmBfcOsApplicationFactory & operator = (const BcmBfcOsApplicationFactory &otherInstance);

};


//********************** Inline Method Implementations ***********************


/** \defgroup BfcPerformance BFC Performance Profiling and Measurement

We get a seemingly endless string of requests for various methods of measuring
system performance.  This section describes various performance profiling tools
and features built in to our system, and how to use them.

<H2>Idle Profiling/CPU Utilization</H2>

One of the most frequently requested performance statistics is CPU utilization -
How much CPU does X use?  How much CPU is left for my value-add features?

The answer, of course, is that it depends.  What CPU are you running on?  How
fast is the internal clock running?  What is the system configuration?  Which
interfaces are enabled, and how much traffic is being sent port-to-port?  What
time-critical features (like VoIP) are running?  How much bus/RAM bandwidth is
being consumed by other parts of the chip (such as video decoding)?

In the best case, nearly 100% of the CPU is available for use; in the worst
case, we can usually consume 100% of the CPU.  If we had a faster and more
powerful CPU, we would probably consume 100% of it, too.

While this is a reasonable answer to the question, it is rarely satisfactory.
As a result, we built some tools in to the code that you can use to profile
your own system with whatever constraints you choose.

<H3>How it works</H3>

The Idle Loop Profiler is simply a thread that runs at the lowest priority
possible.  While it is running, it increments a counter.  It receives commands
from the console; when told to start, it clears the counter, starting from 0.
When told to stop, it prints the current counter value.

Note that the counter value <em>does not</em> correspond to CPU cycles,
instructions per second, Hertz, etc.  It is simply the number of times the idle
thread has run through the idle loop.

It is possible to convert this number into CPU instructions by disassembling
the loop and figuring out which instructions are executed.  Note, however, that
this does not correlate to "CPU time" very well, since it ignores the time spent
accessing memory, fetching instructions into the ICache, etc.  This extra "stuff"
is all highly relevant to how much CPU is being consumed.

Instead, we recommend using relative ratios to determine CPU utilization on a
percentage basis.  See below for details.

<H3>Enabling Idle Profiling</H3>

By default, the Idle Profiling code is compiled out.  To enable it, you need
to edit one of the OS-specific BFC Application files and turn on a
<tt>#define</tt>:

    - In <tt>Bfc/ecos/ecosBfcApplication.cpp</tt>, set <tt>IDLE_LOOP_PROFILING</tt>
      to 1.
    - In <tt>Bfc/vxWorks/VxWorksBfcApplication.cpp</tt>, set <tt>IDLE_LOOP_PROFILING</tt>
      to 1.

You only need to modify the file corresponding to the OS for which you are
building.  Once changed, rebuild the image; you <em>do not</em> need to do a
clean build.

<H3>How to use the Idle Loop Profiler</H3>

When you run the image, you will find that 2 new commands are available in the
root directory:

    - <tt>start_idle_profiling</tt> clears the idle loop counter, effectively
      starting the profiling process.
    - <tt>stop_idle_profiling</tt> stops the counting and prints the current
      idle loop counter value.

In order to measure "idle loops" for comparison, you should measure over a fixed
and consistent time period; if you measure for 1 second, and again for 20
seconds, you will find radically different counter values reported.  On the
other hand, if you measure for 2 seconds, and then again for 2 seconds, the
counter values should be relatively close if the CPU load is the same.

This can be done by using the command chaining feature of the BFC console; for
example, this chained command runs idle profiling for 2000ms (2 seconds) and
prints the result:

\code
start ` sleep 2000 ` stop

57037598 'idle loops'
\endcode

To chain commands, simply separate them with a backward single quote character
(`) which is under the tilda (~) on most keyboards.

You may need to boost the console thread priority in order to run the start and
stop commands while the CPU is very busy.  Note that you do not want to change
the idle thread priority!  Doing so will produce invalid results!

<H3>Calculating CPU Utilization</H3>

Now that you know how to include the Idle Profiling code and run the commands,
you need to be able to calculate the amount of idle CPU time.

This requires taking at least 2 measurements; the first is a baseline that
measures the number of idle counts when the CPU isn't doing much (i.e. the count
value when the CPU is idle).  The rest of the measurements capture the count
value when the CPU is doing whatever you find interesting - passing traffic,
downloading an image, walking the MIB, etc.

Then, use the following equations:
<pre>
Count<sub>idle</sub> = start ` sleep 2000 ` stop {when CPU is idle}
Count<sub>busy</sub> = start ` sleep 2000 ` stop {when CPU is busy}

PercentCPU<sub>idle</sub> = (Count<sub>busy</sub> / Count<sub>idle</sub>) * 100
PercentCPU<sub>busy</sub> = 100 - PercentCPU<sub>idle</sub>
</pre>
For example, I ran a test on a BCM3349 for CPU utilization during an FTP
transfer:
<pre>
Count<sub>idle</sub> = 57035600
Count<sub>busy</sub> = 2466283

PercentCPU<sub>idle</sub> = (Count<sub>busy</sub> / Count<sub>idle</sub>) * 100
              = (2466283 / 57035600 ) * 100
              = 4.32% idle
PercentCPU<sub>busy</sub> = 100 - PercentCPU<sub>idle</sub>
              = 100 - 4.32
              = 95.68% busy
</pre>

<H2>TBD</H2>

Any others to document?  UNFINISHED

*/

#endif


