//****************************************************************************
//
// Copyright (c) 2000-2009 Broadcom Corporation
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
//  Filename:       NonVolDevice.h
//  Author:         David Pullen
//  Creation Date:  Feb 9, 2000
//
//****************************************************************************

#ifndef NONVOLDEVICE_H
#define NONVOLDEVICE_H

//********************** Include Files ***************************************

// My base class; supports async writes.  I use private inheritance because
// clients should not know that I am a thread.
#include "Thread.h"

// For the portable types.
#include "typedefs.h"

// The abstract base for settings objects (the unit of operation).                        
#include "NonVolSettings.h"

#include "MessageLog.h"

//********************** Global Types ****************************************

//********************** Global Constants ************************************

//********************** Global Variables ************************************

//********************** Forward Declarations ********************************

class BcmMutexSemaphore;
class BcmOctetBuffer;
class BcmMessageQueue;

//********************** Class Declaration ***********************************


/** \ingroup BfcNonVol
*
*      This is the abstract base class for all kinds of hardware-specific
*      Non-Volatile Storage Devices.  There will be hardware-specific and
*      target-specific derived classes that handle the appropriate
*      functionality.
*
*      Note that the settings are independant of the device to which they are
*      being stored, allowing the same application settings to be used on
*      multiple target platforms and devices!
*
*      The derived class will generally be placed in the a BSP or other h/w
*      directory, since it is hardware/target-specific.
*
*      This class provides the common API, as well as some helper methods, for
*      all of the NonVol Devices.
*
*      All NonVolDevices will have the following 8-byte header layout:
*
\verbatim
        +--------+----------+--------------------+
        | Length | Checksum | {Settings data...} |
        +--------+----------+--------------------+
             4         4          Length - 8
\endverbatim
*
*      The Checksum and Length fields are calculated during Write(), and are
*      stored in network order.
*
*      The Length field tells how many bytes of header and app data are
*      contained.  This field must always come first, and will always be at
*      least 8.
*/
class BcmNonVolDevice : private BcmThread
{
public:

    /// Initializing Constructor.  Initializes the state of the object.
    ///
    /// \param
    ///      dynamicSizeBytes - the maximum number of bytes of data that the
    ///                         device can hold for dynamic settings.
    /// \param
    ///      permanentSizeBytes - the maximum number of bytes of data that the
    ///                           device can hold for permanent settings.
    ///
    BcmNonVolDevice(uint32 dynamicSizeBytes, uint32 permanentSizeBytes);

    /// Destructor.  Frees up any memory/objects allocated, cleans up internal
    /// state.
    ///
    virtual ~BcmNonVolDevice();

    /// For use by derived classes to know how many bytes are in the header.
    enum
    {
        kHeaderLengthBytes = 8
    };

    /// Returns the maximum number of bytes that can be stored in the device.
    ///
    /// \param
    ///      section - the section that is being querried.
    ///
    /// \return
    ///      The max size of the settings.
    ///
    inline uint32 GetMaxSize(BcmNonVolSettings::NonVolSection section) const;

    /// This method locks out anyone else from calling read/write, to ensure that
    /// some critical "thing" doesn't occur during a read or write.  Usually,
    /// this is done just before rebooting the system, to ensure that we don't
    /// reboot during a write to non-vol.
    ///
    void Lock(void);

    /// This method releases the device for use by others, unlocking access to it.
    ///
    void Unlock(void);

    /// For use in Write().
    typedef enum
    {
        /// The write will occur synchronously, meaning that you will be
        /// blocked until it completes.  The write can take a long time (several
        /// seconds), especially when it is being stored to certain types of
        /// flash memory.
        kWriteModeSync = 0,

        /// The write will occur asynchronously, meaning that it will be queued
        /// for processing later on, at a lower priority.  Note that the write
        /// may never occur if the power fails or if higher priority threads
        /// consume all of the CPU.
        kWriteModeAsync,
        
        /// The write will occur synchronously, and unconditionally.  This mode
        /// will not perform CRC verification of data when writing - it will
        /// write even if the CRC indicates that the data is unchanged.  
        ///
        /// The point of this is to allow a mode which protects against the
        /// very unlikely but theoretically possible case of the same CRC being 
        /// calculated over two different data sets.
        kWriteModeUnconditionalSync,
        
        /// The write will occur asynchronously, and unconditionally.  This
        /// mode will not perform CRC verification of data when writing - it
        /// will write even if the CRC indicates that data is unchanged.  
        ///
        /// The point of this is to allow a mode which protects against the
        /// very unlikely but theoretically possible case of the same CRC being 
        /// calculated over two different data sets.
        kWriteModeUnconditionalAsync

    } WriteMode;

    /// This is the main entrypoint to get a NonVol Device to write the specified
    /// settings object to a flat buffer and then to the device.  This method
    /// does the common header value setup required (priming the checksum with
    /// 0's, and setting the length to 0), then it calls the settings object to
    /// get it to write itself to the buffer.  When this returns, the length
    /// field of the header is filled in with the correct value, the checksum is
    /// calculated and stored.
    ///
    /// Then it calls WriteImpl(), which gives the derived class the opportunity
    /// to write the buffer to the device.
    ///
    /// Alternatively, a buffer can be passed in if you want to bypass writing to
    /// the device.  This is useful for transferring the settings data to an
    /// outside device (via TFTP, Telnet, console, etc).
    ///
    /// \param
    ///      settings - the settings that are to be written to the device.
    /// \param
    ///      section - the section that is being written.
    /// \param
    ///      mode - tells whether you want the settings to be written
    ///             synchronously (in your thread context) or asynchronously
    ///             (in the background).
    /// \param
    ///      pBuffer - Optional pointer to a buffer into which the data is to be
    ///                written.  If NULL, then one will be created and filled
    ///                with data from the settings, then written to the device.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (usually the settings are too big to
    ///          fit, or there was a hardware failure).
    ///
    bool Write(const BcmNonVolSettings &settings,
               BcmNonVolSettings::NonVolSection section = BcmNonVolSettings::kDynamicSection,
               WriteMode mode = kWriteModeSync, BcmOctetBuffer *pBuffer = NULL);

    /// This is the main entrypoint to get a NonVol Device to read the data from
    /// the device and hand it to the settings object.  This method does the
    /// common header validation (verifying the checksum, etc.).
    ///
    /// It calls ReadImpl(), which gives the derived class the opportunity to
    /// read the buffer from the device, then hands the resultant buffer to the
    /// settings object.
    ///
    /// Alternatively, a buffer of data can be passed in if you want to bypass
    /// reading from the device.  This is useful for transferring the settings
    /// data from an outside device (via TFTP, Telnet, console, etc).
    ///
    /// \param
    ///      settings - the settings that are to be read from the device.
    /// \param
    ///      section - the section to be read.
    /// \param
    ///      pBuffer - Optional pointer to a buffer containing the data to be
    ///                used.  If NULL, then one will be created and filled with
    ///                data from the device.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (checksum failed, hardware failure,
    ///          the settings could not parse the data, etc.).
    ///
    bool Read(BcmNonVolSettings &settings,
              BcmNonVolSettings::NonVolSection section = BcmNonVolSettings::kDynamicSection,
              BcmOctetBuffer *pBuffer = NULL);

    /// Causes the device to wipe out the specified settings section, resetting
    /// it to its 'virgin' state.  This can be implemented as erasing flash
    /// blocks, deleting files, setting memory to 0's or 1's, etc., depending on
    /// how things are stored on the device.
    ///
    /// \param
    ///      section - the section to be cleared.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (checksum failed, hardware failure,
    ///          the settings could not parse the data, etc.).
    ///
    bool ClearDevice(BcmNonVolSettings::NonVolSection section);

    /// Returns the pointer to the singleton non-vol device.  Most objects in the
    /// system will use this method rather than being passed a pointer to the
    /// non-vol device object.  The singleton pointer will be set up in the
    /// base-class constructor.
    ///
    /// \note   This can return NULL if a singleton has not been set up for the
    ///         system, so you must check for this condition.
    ///
    /// \note   You must not delete this object!
    ///
    /// \note   You should not store a pointer to the object that is returned,
    ///         since it may be deleted and replaced with a new one.
    ///
    /// \return
    ///      A pointer to the non-vol device that should be used by the system.
    ///
    static BcmNonVolDevice *GetSingletonInstance(void);

    //@{
    /// Accessors for buffer stats.
    inline uint32 MaxBufferSize(void) { return fMaxPhysicalSizeBytes; }
    inline uint32 DynamicBytesUsed(void) { return fDynamicBytesUsed; }
    inline uint32 PermanentBytesUsed(void) { return fPermanentBytesUsed; }
    inline uint32 DynamicChecksum(void) { return fDynamicChecksum; }
    inline uint32 PermanentChecksum(void) { return fPermanentChecksum; }
    inline uint32 DynamicCrc32(void) { return fDynamicCrc32; }
    inline uint32 PermanentCrc32(void) { return fDynamicCrc32; }
    inline uint32 DynamicNumberOfWrites(void) { return fDynamicNumWrites; }
    inline uint32 PermanentNumberOfWrites(void) { return fPermanentNumWrites; }
    //@}

protected:

    /// This is the synchronous version of Write().  It can be called from Write
    /// or from my thread.
    ///
    /// This is the main entrypoint to get a NonVol Device to write the specified
    /// settings object to a flat buffer and then to the device.  This method
    /// does the common header value setup required (priming the checksum with
    /// 0's, and setting the length to 0), then it calls the settings object to
    /// get it to write itself to the buffer.  When this returns, the length
    /// field of the header is filled in with the correct value, the checksum is
    /// calculated and stored.
    ///
    /// Then it calls WriteImpl(), which gives the derived class the opportunity
    /// to write the buffer to the device.
    ///
    /// Alternatively, a buffer can be passed in if you want to bypass writing to
    /// the device.  This is useful for transferring the settings data to an
    /// outside device (via TFTP, Telnet, console, etc).
    ///
    /// \param
    ///      settings - the settings that are to be written to the device.
    /// \param
    ///      section - the section that is being written.
    /// \param
    ///      pBuffer - Optional pointer to a buffer into which the data is to be
    ///                written.  If NULL, then one will be created and filled
    ///                with data from the settings, then written to the device.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (usually the settings are too big to
    ///          fit, or there was a hardware failure).
    ///
    bool WriteSync(const BcmNonVolSettings &settings,
                   BcmNonVolSettings::NonVolSection section,
                   BcmOctetBuffer *pBuffer, bool unconditional);

protected:

    /// This is where the derived class writes the buffer to the device.  Note
    /// that the derived class may append 0's to the end of the buffer as
    /// needed in order to enforce alignment or size restrictions as required by
    /// the hardware.
    ///
    /// This MUST be implemented by the derived class.
    ///
    /// \param
    ///      outputBuffer - the buffer that should be written.
    /// \param
    ///      section - the section that should be written.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (usually hardware failure).
    ///
    virtual bool WriteImpl(BcmOctetBuffer &outputBuffer,
                           BcmNonVolSettings::NonVolSection section = BcmNonVolSettings::kDynamicSection) = 0;

    /// This is where the derived class reads data from the device into the
    /// buffer.
    ///
    /// This MUST be implemented by the derived class.
    ///
    /// \param
    ///      inputBuffer - the buffer into which the data should be read.
    /// \param
    ///      section - the section that should be read.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (usually hardware failure).
    ///
    virtual bool ReadImpl(BcmOctetBuffer &inputBuffer,
                          BcmNonVolSettings::NonVolSection section = BcmNonVolSettings::kDynamicSection) = 0;

    /// This is where the derived class clears the data from the device.
    ///
    /// This MUST be implemented by the derived class.
    ///
    /// \param
    ///      section - the section to be cleared.
    ///
    /// \retval
    ///      true if successful.
    /// \retval
    ///      false if there was a problem (checksum failed, hardware failure,
    ///          the settings could not parse the data, etc.).
    ///
    virtual bool ClearDeviceImpl(BcmNonVolSettings::NonVolSection section) = 0;

    /// The implementation of the checksum method.  I'm not sure whether or not
    /// this algorithm is based on any RFC or other standard.
    ///
    /// Note that if you are creating the 32-bit checksum, the appropriate field
    /// in the buffer must be pre-set to 0's before calling this method, and only
    /// that field may be changed after the checksum is stored.
    ///
    /// If you are verifying the checksum, then the checksum field in the buffer
    /// must contain the previously calculated value.  If valid, then this method
    /// will return 0.
    ///
    /// If any other fields are changed, then the checksum must be recalculated.
    ///
    /// \param
    ///      outputBuffer - the buffer over which the checksum should be computed.
    ///
    /// \return
    ///      The checksum value that was calculated.
    ///
    uint32 CalculateChecksum(const BcmOctetBuffer &outputBuffer);

protected:

    /// This must be set by the derived class; it tells me how much data the
    /// device can hold.  I use it for buffer allocation.
    uint32 fDynamicSizeBytes;
    uint32 fPermanentSizeBytes;
    uint32 fMaxPhysicalSizeBytes;

    /// Stats on memory usage and other things.
    uint32 fDynamicBytesUsed;
    uint32 fPermanentBytesUsed;
    uint32 fDynamicChecksum;
    uint32 fPermanentChecksum;
    
    /// CRC32 is not stored to flash but calculated at runtime in order to
    /// allow an optimization to not write flash if nothing has changed.  We 
    /// use a CRC32 here instead of the existing checksums because CRC32 is a
    /// more reliable algorithm in detecting change than the simple additive
    /// checksum.
    uint32 fDynamicCrc32;
    uint32 fPermanentCrc32;
    
    /// Counter for the number of times NV has been written.  This is not
    /// stored to flash, it resets to 0 on reboot.  Purely a diagnostic
    /// thing, accesible via the NonVol/stats command.
    uint32 fDynamicNumWrites;
    uint32 fPermanentNumWrites;

    /// The buffer into which the settings will be read/written.
    BcmOctetBuffer *pfBuffer;

    /// To protect against multiple access.
    BcmMutexSemaphore *pfMutex;

    /// Async messaging interface for my thread.
    BcmMessageQueue *pfMessageQueue;

    /// For controlling logging of messages.
    BcmMessageLogSettings fMessageLogSettings;

private:

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
    virtual void ThreadMain(void);

    /// Thread destructor - this is the last method called when the thread is
    /// exiting, and is where the thread should delete all of the OS objects that
    /// it created.
    ///
    /// The default implementation does nothing, allowing derived classes that
    /// don't have any deinitialization to use the default.
    ///
    virtual void Deinitialize(void);

private:

    /// This is the pointer to the singleton instance of this class, which will
    /// be returned by GetSingletonInstance().
    static BcmNonVolDevice *pfSingletonInstance;

private:

    /// Default Constructor.  Not supported.
    BcmNonVolDevice(void);

    /// Copy Constructor.  Not supported.
    BcmNonVolDevice(const BcmNonVolDevice &otherInstance);

    /// Assignment operator.  Not supported.
    BcmNonVolDevice & operator = (const BcmNonVolDevice &otherInstance);

};


//********************** Inline Method Implementations ***********************


inline uint32 BcmNonVolDevice::GetMaxSize(BcmNonVolSettings::NonVolSection section) const
{
    if (section == BcmNonVolSettings::kDynamicSection)
    {
        return fDynamicSizeBytes;
    }

    return fPermanentSizeBytes;
}


#endif


