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
//  Filename:       NonVolDevice.cpp
//  Author:         David Pullen
//  Creation Date:  Feb 9, 2000
//
//****************************************************************************
//  Description:
//      This is the abstract base class for all kinds of hardware-specific
//      Non-Volatile Storage Devices.  There will be hardware-specific and
//      target-specific derived classes that handle the appropriate
//      functionality.
//
//      Note that the settings are independant of the device to which they are
//      being stored, allowing the same application settings to be used on
//      multiple target platforms and devices!
//
//      The derived class will generally be placed in the a BSP or other h/w
//      directory, since it is hardware/target-specific.
//
//      This class provides the common API, as well as some helper methods, for
//      all of the NonVol Devices.
//
//      All NonVolDevices will have the following 8-byte header layout:
//
//        +--------+----------+--------------------+
//        | Length | Checksum | {Settings data...} |
//        +--------+----------+--------------------+
//             4         4          Length - 8
//
//      The Checksum and Length fields are calculated during Write(), and are
//      stored in network order.
//
//      The Length field tells how many bytes of header and app data are
//      contained.  This field must always come first, and will always be at
//      least 8.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "NonVolDevice.h"

#include "OctetBuffer.h"

#include "OperatingSystemFactory.h"
#include "MutexSemaphore.h"
#include "MessageQueue.h"

#include "crc.h"

#include <assert.h>

//********************** Local Types *****************************************

// Passed in the message queue to the thread for async writing operation.  These
// mirror the interesting/required parameters to WriteSync().
typedef struct
{
    const BcmNonVolSettings *pSettings;
    BcmNonVolSettings::NonVolSection section;
    bool unconditional;
} AsyncWriteInfo;

//********************** Local Constants *************************************

// The commands we support via the message queue.
enum
{
    kWriteNonVol = 0,
    kExitThread,
};

//********************** Local Variables *************************************

//********************** Class Method Implementations ************************

// Set this to NULL initially.  The constructor will store the "this" pointer
// here when it runs.
BcmNonVolDevice *BcmNonVolDevice::pfSingletonInstance = NULL;


// Initializing Constructor.  Initializes the state of the object.
//
// Parameters:
//      dynamicSizeBytes - the maximum number of bytes of data that the
//                         device can hold for dynamic settings.
//      permanentSizeBytes - the maximum number of bytes of data that the
//                           device can hold for permanent settings.
//
// Returns:  N/A
//
BcmNonVolDevice::BcmNonVolDevice(uint32 dynamicSizeBytes, uint32 permanentSizeBytes) :
    BcmThread("NonVol Device Async Helper", false, BcmOperatingSystem::kLowNormalPriority),
    fDynamicSizeBytes(dynamicSizeBytes),
    fPermanentSizeBytes(permanentSizeBytes),
    pfMutex(NULL),
    fMessageLogSettings("BcmNonVolDevice")
{
    CallTrace("BcmNonVolDevice", "BcmNonVolDevice");

    // Create the mutex that I will use for protection against re-entrancy.
    pfMutex = BcmOperatingSystemFactory::NewMutexSemaphore("NonVol Device Mutex");

    if (pfMutex == NULL)
    {
        gFatalErrorMsg(fMessageLogSettings, "BcmNonVolDevice")
            << "Failed to create my mutex!" << endl;

        assert(0);
    }

    // This is created in Initialize().
    pfMessageQueue = NULL;
    
    // Calculate this.
    fMaxPhysicalSizeBytes = fDynamicSizeBytes;
    if (fMaxPhysicalSizeBytes < fPermanentSizeBytes)
    {
        fMaxPhysicalSizeBytes = fPermanentSizeBytes;
    }

    fDynamicBytesUsed = 0;
    fPermanentBytesUsed = 0;
    fDynamicChecksum = 0;
    fPermanentChecksum = 0;
    
    fDynamicCrc32 = 0;
    fPermanentCrc32 = 0;
    
    fDynamicNumWrites = 0;
    fPermanentNumWrites = 0;

    // Create the buffer for the settings data.  We want to do this early on to
    // ensure that we will be able to read/write settings.
    pfBuffer = new BcmOctetBuffer(fMaxPhysicalSizeBytes);

    // Start my thread.
    pfOperatingSystem->BeginThread(this, 3 * 1024);

    // Set up the singleton.
    if (pfSingletonInstance != NULL)
    {
        gWarningMsg(fMessageLogSettings, "BcmNonVolDevice")
            << "Singleton pointer is not NULL!  There are multiple non-vol devices!  Leaving the singleton pointer alone..." << endl;
    }
    else
    {
        gInfoMsg(fMessageLogSettings, "BcmNonVolDevice")
            << "Setting up singleton pointer." << endl;

        pfSingletonInstance = this;
    }

}


// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmNonVolDevice::~BcmNonVolDevice()
{
    CallTrace("BcmNonVolDevice", "~BcmNonVolDevice");

    // Clear the singleton pointer.
    if (pfSingletonInstance == this)
    {
        gInfoMsg(fMessageLogSettings, "~BcmNonVolDevice")
            << "Clearing the singleton pointer." << endl;

        pfSingletonInstance = NULL;
    }
    else
    {
        gWarningMsg(fMessageLogSettings, "~BcmNonVolDevice")
            << "I'm not the singleton instance!  Leaving the singleton pointer alone..." << endl;
    }

    // Tell my thread to exit.
    if (pfMessageQueue != NULL)
    {
        pfMessageQueue->Send(kExitThread);
    }

    // Wait for the thread to exit.
    WaitForThread();

    // Delete my mutex.
    delete pfMutex;
    pfMutex = NULL;

    // Delete my octet buffer.
    delete pfBuffer;
    pfBuffer = NULL;
}


// This method locks out anyone else from calling read/write, to ensure that
// some critical "thing" doesn't occur during a read or write.  Usually,
// this is done just before rebooting the system, to ensure that we don't
// reboot during a write to non-vol.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmNonVolDevice::Lock(void)
{
    CallTrace("BcmNonVolDevice", "Lock");

    // Try to lock my mutex.
    if (pfMutex->Lock() == false)
    {
        gErrorMsg(fMessageLogSettings, "Lock")
            << "Failed to lock my mutex!" << endl;
    }
}


// This method releases the device for use by others, unlocking access to it.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmNonVolDevice::Unlock(void)
{
    CallTrace("BcmNonVolDevice", "Unlock");

    // Unlock my mutex.
    if (pfMutex->Unlock() == false)
    {
        gWarningMsg(fMessageLogSettings, "Unlock")
            << "Failed to unlock my mutex!" << endl;
    }
}


// This is the main entrypoint to get a NonVol Device to write the specified
// settings object to a flat buffer and then to the device.  This method
// does the common header value setup required (priming the checksum with
// 0's, and setting the length to 0), then it calls the settings object to
// get it to write itself to the buffer.  When this returns, the length
// field of the header is filled in with the correct value, the checksum is
// calculated and stored.
//
// Then it calls WriteImpl(), which gives the derived class the opportunity
// to write the buffer to the device.
//
// Alternatively, a buffer can be passed in if you want to bypass writing to
// the device.  This is useful for transferring the settings data to an
// outside device (via TFTP, Telnet, console, etc).
//
// Parameters:
//      settings - the settings that are to be written to the device.
//      section - the section that is being written.
//      mode - tells whether you want the settings to be written
//             synchronously (in your thread context) or asynchronously
//             (in the background).
//      pBuffer - Optional pointer to a buffer into which the data is to be
//                written.  If NULL, then one will be created and filled
//                with data from the settings, then written to the device.
//
// Returns:
//      true if successful.
//      false if there was a problem (usually the settings are too big to
//          fit, or there was a hardware failure).
//
bool BcmNonVolDevice::Write(const BcmNonVolSettings &settings,
                            BcmNonVolSettings::NonVolSection section,
                            WriteMode mode, BcmOctetBuffer *pBuffer)
{
    AsyncWriteInfo *pAsyncWriteInfo;
    bool result;
    bool unconditional = false;

    CallTrace("BcmNonVolDevice", "Write");
    
    // If the user wants an unconditional write, note this
    if ((mode == kWriteModeUnconditionalSync) || (mode == kWriteModeUnconditionalAsync))
    {
        unconditional = true;
    }

    // If the user wants to do this synchronously, then just call the WriteSync
    // method.
    if ((mode == kWriteModeSync) || (mode == kWriteModeUnconditionalSync))
    {
        return WriteSync(settings, section, pBuffer, unconditional);
    }

    // If the user wants async operation, then they can't specify a buffer,
    // since that buffer might not be valid beyond this context.
    if (pBuffer != NULL)
    {
        gErrorMsg(fMessageLogSettings, "Write")
            << "Can't use async write mode when writing to a user-supplied buffer!" << endl;

        return false;
    }

    // Make sure my thread is still running.
    if (pfMessageQueue == NULL)
    {
        gErrorMsg(fMessageLogSettings, "Write")
            << "My thread isn't running!  Can't write in async mode!" << endl;

        return false;
    }

    // Create a struct and fill in the fields.
    pAsyncWriteInfo = new AsyncWriteInfo;
    pAsyncWriteInfo->pSettings = &settings;
    pAsyncWriteInfo->section = section;
    pAsyncWriteInfo->unconditional = unconditional;

    // Queue this for processing by my thread.
    result = pfMessageQueue->Send(kWriteNonVol, pAsyncWriteInfo);

    if (!result)
    {
        gErrorMsg(fMessageLogSettings, "Write")
            << "Failed to send message for async write!" << endl;

        delete pAsyncWriteInfo;
    }

    return result;
}


// This is the main entrypoint to get a NonVol Device to read the data from
// the device and hand it to the settings object.  This method does the
// common header validation (verifying the checksum, etc.).
//
// It calls ReadImpl(), which gives the derived class the opportunity to
// read the buffer from the device, then hands the resultant buffer to the
// settings object.
//
// Alternatively, a buffer of data can be passed in if you want to bypass
// reading from the device.  This is useful for transferring the settings
// data from an outside device (via TFTP, Telnet, console, etc).
//
// Parameters:
//      settings - the settings that are to be read from the device.
//      section - the section to be read.
//      pBuffer - Optional pointer to a buffer containing the data to be
//                used.  If NULL, then one will be created and filled with
//                data from the device.
//
// Returns:
//      true if successful.
//      false if there was a problem (checksum failed, hardware failure,
//          the settings could not parse the data, etc.).
//
bool BcmNonVolDevice::Read(BcmNonVolSettings &settings,
                           BcmNonVolSettings::NonVolSection section,
                           BcmOctetBuffer *pBuffer)
{
    unsigned int offset;
    uint32 settingsLength, checksum;
    BcmNonVolSettings *pParentSettings;
    bool result = true;
    bool resetDefaults = true;

    CallTrace("BcmNonVolDevice", "Read");

    // PR6478 - get exclusive access to the nonvol via the new access lock
    // object.  The lock will be released automatically when we exit this
    // method.
    BcmNonVolSettingsAccessLock nonvolAccessLock;

    // Try to lock my mutex.
    if (pfMutex->Lock() == false)
    {
        gErrorMsg(fMessageLogSettings, "Read")
            << "Failed to lock my mutex!" << endl;

        return false;
    }

    // If the user specified a buffer, then I use it and bypass calling the
    // derived class.
    if (pBuffer != NULL)
    {
        gInfoMsg(fMessageLogSettings, "Read")
            << "The client specified a buffer; the settings will be read from the buffer, and not from the device." << endl;

        // What if their buffer isn't the right size?  I'll continue anyway,
        // but things may not work.
        if (pBuffer->BufferSize() != fMaxPhysicalSizeBytes)
        {
            gWarningMsg(fMessageLogSettings, "Read")
                << "The buffer size (" << pBuffer->BufferSize() 
                << " bytes) does not match the device size (" << fMaxPhysicalSizeBytes
                << " bytes).  This may not work correctly..." << endl;
        }
    }
    else
    {
        // Use my buffer instead.  Make sure my buffer is empty since I will be
        // reading from the device.
        pBuffer = pfBuffer;
        pBuffer->Empty();
    
        // First, get the device to read the data into the buffer.
        result = ReadImpl(*pBuffer, section);

        // Update statistics.
        if (section == BcmNonVolSettings::kDynamicSection)
        {
            fDynamicBytesUsed = pBuffer->BytesUsed();
        }
        else
        {
            fPermanentBytesUsed = pBuffer->BytesUsed();
        }
        
        if (result == false)
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "Read") 
                << "Failed to read the buffer from the device!  Resetting the entire section to defaults." << endl;
        }
    }

    // Find the parent of the settings object.  This is the one that I need
    // to operate on.  Presumably, it will call the child objects that it
    // contains.
    pParentSettings = &settings;
    while (pParentSettings->GetParent() != NULL)
    {
        pParentSettings = pParentSettings->GetParent();
    }

    if (result == true)
    {
        // Read the Section Header fields from the buffer, including the Section
        // Length and Section Checksum.  These will be used to verify the
        // contents of the section.
        // Assume that the length is correct; it should match the number of used
        // bytes in the octet buffer.  Since the derived class uses the length
        // field, this should be valid.  Pull the length and checksum fields
        // from the buffer so that the offset will be primed to the first byte
        // of settings data.
        offset = 0;
        if ((pBuffer->NetworkRead(offset, settingsLength) == false) ||
            (pBuffer->NetworkRead(offset, checksum) == false))
        {
            gErrorMsg(fMessageLogSettings, "Read")
                << "Failed to read the Section Length and Checksum from the buffer!  Buffer too small.  Resetting the entire section to defaults." << endl;

            result = false;
        }
        else
        {
            gInfoMsg(fMessageLogSettings, "Read") 
                << "Section Length is " << settingsLength
                << " bytes; Section Checksum is " << checksum << " ("
                << inHex(checksum) << ")\n\n";

            // PR1660/PR1661/PR1662 - Unconditionally print the checksums for
            // the settings, even in the slim load.  This is used by
            // manufacturing to determine whether nonvol has been modified.
            gLogMessageRaw << "Checksum for ";
            if (section == BcmNonVolSettings::kDynamicSection)
            {
                gLogMessageRaw << "dynamic";
                fDynamicChecksum = checksum;
            }
            else
            {
                gLogMessageRaw << "permanent";
                fPermanentChecksum = checksum;
            }
            gLogMessageRaw << " settings:  " << inHex(checksum) << endl;
/*
            if ((pgMessageLogStream != NULL) && 
                (fMessageLogSettings.InfoMsgLogging()))
            {
                pBuffer->Print(*pgMessageLogStream, true);
            }
*/
            // Now verify that the checksum is correct.
            if (CalculateChecksum(*pBuffer) != 0)
            {
                // Log the error.
                gErrorMsg(fMessageLogSettings, "Read") 
                    << "Section Checksum verification failed!  Resetting the entire section to defaults." << endl;

                result = false;
            }
            else
            {
                // Finally, let the settings object try to crack the data in
                // the buffer.
                result = pParentSettings->ReadFrom(*pBuffer, offset, section);
                if (result == false)
                {
                    // This does not cause the whole section to reset.
                    resetDefaults = false;
                }
                else
                {
                    // Make sure all of the bytes were consumed.  If not, then
                    // the settings may have been invalid.  This is not reported
                    // as an error, and does not cause the settings to be reset.
                    if (offset != pBuffer->BytesUsed())
                    {
                        gWarningMsg(fMessageLogSettings, "Read")
                            << "Extra bytes in the buffer!  Section Length was invalid (got "
                            << pBuffer->BytesUsed() << ", used " << offset << ")!" << endl;
                    }
                }
            }
        }
    }
    
    // If we are satisfied with the data we read then calculate the section
    // CRC32's.  These are used in WriteSync to skip writing flash if nothing
    // has changed.
    if (result == true)
    {
        if (section == BcmNonVolSettings::kDynamicSection)
        {
            fDynamicCrc32 = FastCrc32 (pBuffer->AccessBuffer(), pBuffer->BytesUsed());
            
            gInfoMsg(fMessageLogSettings, "Read") 
                << "Calculated " << inHex(fDynamicCrc32) << " as the dynamic CRC32." << endl;
        }
        else
        {
            fPermanentCrc32 = FastCrc32 (pBuffer->AccessBuffer(), pBuffer->BytesUsed());
            
            gInfoMsg(fMessageLogSettings, "Read") 
                << "Calculated " << inHex(fPermanentCrc32) << " as the permanent CRC32." << endl;
        }
    }

    // If we encountered a problem and need to reset, then do so.
    if ((result == false) && (resetDefaults == true))
    {
        // Reset all of the groups in this section.
        pParentSettings->ResetDefaults(section);
    }

    // Unlock my mutex.
    if (pfMutex->Unlock() == false)
    {
        gWarningMsg(fMessageLogSettings, "Read")
            << "Failed to unlock my mutex!" << endl;
    }

    return result;
}


// Causes the device to wipe out the specified settings section, resetting
// it to its 'virgin' state.  This can be implemented as erasing flash
// blocks, deleting files, setting memory to 0's or 1's, etc., depending on
// how things are stored on the device.
//
// Parameters:
//      section - the section to be cleared.
//
// Returns:
//      true if successful.
//      false if there was a problem (checksum failed, hardware failure,
//          the settings could not parse the data, etc.).
//
bool BcmNonVolDevice::ClearDevice(BcmNonVolSettings::NonVolSection section)
{
    bool result;

    CallTrace("BcmNonVolDevice", "ClearDevice");

    // Try to lock my mutex.
    if (pfMutex->Lock() == false)
    {
        gErrorMsg(fMessageLogSettings, "ClearDevice")
            << "Failed to lock my mutex!" << endl;

        return false;
    }

    result = ClearDeviceImpl(section);

    // Unlock my mutex.
    if (pfMutex->Unlock() == false)
    {
        gWarningMsg(fMessageLogSettings, "ClearDevice")
            << "Failed to unlock my mutex!" << endl;
    }

    return result;
}


// Returns the pointer to the singleton non-vol device.  Most objects in the
// system will use this method rather than being passed a pointer to the
// non-vol device object.
//
// NOTE:  This can return NULL if a singleton has not been set up for the
//        system, so you must check for this condition.
//
// NOTE:  You must not delete this object!
//
// Parameters:  None.
//
// Returns:
//      A pointer to the non-vol device that should be used by the system.
//
BcmNonVolDevice *BcmNonVolDevice::GetSingletonInstance(void)
{
    CallTrace("BcmNonVolDevice", "GetSingletonInstance");

    if (pfSingletonInstance == NULL)
    {
        gLogMessageRaw
            << "BcmNonVolDevice::GetSingletonInstance:  WARNING - the singleton instance is NULL, and someone is accessing it!" << endl;
    }
    
    return pfSingletonInstance;
}


// This is the synchronous version of Write().  It can be called from Write
// or from my thread.
//
// This is the main entrypoint to get a NonVol Device to write the specified
// settings object to a flat buffer and then to the device.  This method
// does the common header value setup required (priming the checksum with
// 0's, and setting the length to 0), then it calls the settings object to
// get it to write itself to the buffer.  When this returns, the length
// field of the header is filled in with the correct value, the checksum is
// calculated and stored.
//
// Then it calls WriteImpl(), which gives the derived class the opportunity
// to write the buffer to the device.
//
// Alternatively, a buffer can be passed in if you want to bypass writing to
// the device.  This is useful for transferring the settings data to an
// outside device (via TFTP, Telnet, console, etc).
//
// Parameters:
//      settings - the settings that are to be written to the device.
//      section - the section that is being written.
//      pBuffer - Optional pointer to a buffer into which the data is to be
//                written.  If NULL, then one will be created and filled
//                with data from the settings, then written to the device.
//
// Returns:
//      true if successful.
//      false if there was a problem (usually the settings are too big to
//          fit, or there was a hardware failure).
//
bool BcmNonVolDevice::WriteSync(const BcmNonVolSettings &settings,
                                BcmNonVolSettings::NonVolSection section,
                                BcmOctetBuffer *pBuffer,
                                bool unconditional)
{
    uint32 settingsLength;
    uint32 checksum;
    unsigned int offset;
    const BcmNonVolSettings *pParentSettings;
    bool result;
    bool bypassWrite = false;

    CallTrace("BcmNonVolDevice", "WriteSync");

    // create a new scope for the non-vol mutex so it does not unnecessarily
    // remain locked during the flash write. 
    {
        // PR6478 - get exclusive access to the nonvol via the new access lock
        // object.  The lock will be released automatically when we exit this
        // method.
        BcmNonVolSettingsAccessLock nonvolAccessLock;

        // Try to lock my mutex.
        if (pfMutex->Lock() == false)
        {
            gErrorMsg(fMessageLogSettings, "WriteSync")
                << "Failed to lock my mutex!" << endl;

            return false;
        }

        // If the user specified a buffer, then I use it and bypass calling the
        // derived class.
        if (pBuffer != NULL)
        {
            gInfoMsg(fMessageLogSettings, "WriteSync")
                << "The client specified a buffer; the settings will be written into the buffer, and not to the device." << endl;

            bypassWrite = true;

            // What if their buffer isn't the right size?  I'll continue anyway,
            // but things may not work.
            if (pBuffer->BufferSize() != fMaxPhysicalSizeBytes)
            {
                gWarningMsg(fMessageLogSettings, "WriteSync")
                    << "The buffer size (" << pBuffer->BufferSize() 
                    << " bytes) does not match the device size (" << fMaxPhysicalSizeBytes
                    << " bytes).  This may not work correctly..." << endl;
            }
        }
        else
        {
            // Use my buffer instead.
            pBuffer = pfBuffer;
        }

        // Make sure the buffer is empty.
        pBuffer->Empty();

        // Find the parent of the settings object.  This is the one that I need to
        // operate on.  Presumably, it will call the child objects that it contains.
        pParentSettings = &settings;
        while (pParentSettings->GetParent() != NULL)
        {
            pParentSettings = pParentSettings->GetParent();
        }

        // I need to reserve space in the buffer to handle the Section Header.  This
        // includes the Section Length and Section Checksum fields.  For now, just
        // pre-set the length and checksum fields to 0, leaving room for them to be
        // filled in later.  If either of those fail, then there's a serious problem
        // with the buffer.
        settingsLength = 0;
        checksum = 0;
        if (!pBuffer->NetworkAddToEnd(settingsLength) ||
            !pBuffer->NetworkAddToEnd(checksum))
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "WriteSync") 
                << "Buffer too small to hold the Section Header!" << endl;

            // Unlock my mutex.
            if (pfMutex->Unlock() == false)
            {
                gWarningMsg(fMessageLogSettings, "WriteSync")
                    << "Failed to unlock my mutex!" << endl;
            }

            return false;
        }

        // Now give the settings a chance to append its data to the buffer.  If that
        // failed, then bail with a message.
        if (!pParentSettings->WriteTo(*pBuffer, section))
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "WriteSync") 
                << "Buffer too small to hold the settings!" << endl;

            // Unlock my mutex.
            if (pfMutex->Unlock() == false)
            {
                gWarningMsg(fMessageLogSettings, "WriteSync")
                    << "Failed to unlock my mutex!" << endl;
            }

            return false;
        }

        // Next, set the length field now that the buffer contains all of the data.
        offset = 0;
        settingsLength = (uint32) pBuffer->BytesUsed();
        if (!pBuffer->NetworkOverwrite(offset, settingsLength))
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "WriteSync") 
                << "Failed to write the Section Length to the buffer!" << endl;

            // Unlock my mutex.
            if (pfMutex->Unlock() == false)
            {
                gWarningMsg(fMessageLogSettings, "WriteSync")
                    << "Failed to unlock my mutex!" << endl;
            }

            return false;
        }

        // Then calculate the checksum over the buffer and store that value.
        offset = sizeof(settingsLength);
        checksum = CalculateChecksum(*pBuffer);
        if (!pBuffer->NetworkOverwrite(offset, checksum))
        {
            // Log the error.
            gErrorMsg(fMessageLogSettings, "WriteSync") 
                << "Failed to write the Section Checksum to the buffer!" << endl;

            // Unlock my mutex.
            if (pfMutex->Unlock() == false)
            {
                gWarningMsg(fMessageLogSettings, "WriteSync")
                    << "Failed to unlock my mutex!" << endl;
            }

            return false;
        }

        gInfoMsg(fMessageLogSettings, "WriteSync") 
            << "Calculated " << inHex(checksum) << " as the Section Checksum.\n\n";
 
        unsigned long crc32 = FastCrc32 (pBuffer->AccessBuffer(), pBuffer->BytesUsed());
        
        gInfoMsg(fMessageLogSettings, "WriteSync") 
            << "Calculated " << inHex(crc32) << " as the Section CRC32.\n\n";
/*            
        if ((pgMessageLogStream != NULL) && (fMessageLogSettings.InfoMsgLogging()))
        {
            pBuffer->Print(*pgMessageLogStream, true);
        }
*/
        // Update statistics.
        if (section == BcmNonVolSettings::kDynamicSection)
        {
            if (unconditional == false)
            {
                if ((crc32 == fDynamicCrc32) && (checksum == fDynamicChecksum))
                {
                    gInfoMsg(fMessageLogSettings, "WriteSync") 
                        << "dynamic checksum (" << inHex(fDynamicChecksum) << " & CRC (" << inHex(fDynamicCrc32) << ") match, not writing NV." << endl;
                    bypassWrite = true;
                }
                else
                {
                    gInfoMsg(fMessageLogSettings, "WriteSync") 
                        << "dynamic checksum (" << inHex(fDynamicChecksum) << " or CRC (" << inHex(fDynamicCrc32) << ") mismatch, writing NV." << endl;
                }
            }
            else
            {
                gInfoMsg(fMessageLogSettings, "WriteSync") 
                    << "unconditional write, not comparing CRC." << endl;
            }
            
            fDynamicBytesUsed = pBuffer->BytesUsed();
            fDynamicChecksum = checksum;
            fDynamicCrc32 = crc32;
        }
        else
        {
            if (unconditional == false)
            {
                if ((crc32 == fPermanentCrc32) && (checksum == fPermanentChecksum))
                {
                    gInfoMsg(fMessageLogSettings, "WriteSync") 
                        << "permanent checksum (" << inHex(fPermanentChecksum) << " & CRC (" << inHex(fPermanentCrc32) << ") match, not writing NV." << endl;
                    bypassWrite = true;
                }
                else
                {
                    gInfoMsg(fMessageLogSettings, "WriteSync") 
                        << "permanent checksum (" << inHex(fPermanentChecksum) << " or CRC (" << inHex(fPermanentCrc32) << ") mismatch, writing NV." << endl;
                }
            }
            else
            {
                gInfoMsg(fMessageLogSettings, "WriteSync") 
                    << "unconditional write, not comparing CRC." << endl;
            }
            
            fPermanentBytesUsed = pBuffer->BytesUsed();
            fPermanentChecksum = checksum;
            fPermanentCrc32 = crc32;
        }
    }

    // If the user passed in a buffer, then I don't write it to the device
    if (!bypassWrite)
    {
        // Finally, give the buffer to the derived class so that it can store
        // it in the device.
        result = WriteImpl(*pBuffer, section);
        if (result == false )
        {
            ((BcmNonVolSettings *)pParentSettings)->FlushNonVitalDynamicInformation();
        }
        else
        {
            // If the write succeeded, count it.
            if (section == BcmNonVolSettings::kDynamicSection)
            {
                fDynamicNumWrites++;
            }
            else
            {
                fPermanentNumWrites++;
            }
        }
    }
    else
    {
        result = true;
    }

    // Unlock my mutex.
    if (pfMutex->Unlock() == false)
    {
        gWarningMsg(fMessageLogSettings, "WriteSync")
            << "Failed to unlock my mutex!" << endl;
    }

    return result;
}


// Thread constructor - this is the first method called after the thread has
// been spawned, and is where the thread should create all OS objects.  This
// has to be done here, rather than in the object's constructor, because
// some OS objects must be created in the context of the thread that will
// use them.  The object's constructor is still running in the context of
// the thread that created this object.
//
// The default implementation simply returns true, allowing derived classes
// that don't have any initialization to use the default.
//
// Parameters:  None.
//
// Returns:
//      true if successful and ThreadMain() should be called.
//      false if there was a problem (couldn't create an OS object, etc.)
//          and ThreadMain() should not be called.
//
bool BcmNonVolDevice::Initialize(void)
{
    CallTrace("BcmNonVolDevice", "Initialize");

    pfMessageQueue = pfOperatingSystem->NewMessageQueue("NonVol Device Thread's Message Queue");

    if (pfMessageQueue == NULL)
    {
        gErrorMsg(fMessageLogSettings, "Initialize")
            <<  "Failed to create my message queue!" << endl;

        return false;
    }

    return true;
}


// This is the main body of the thread's code.  This is the only method
// that absolutely must be provided in the derived class (since there is no
// reasonable default behavior).
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmNonVolDevice::ThreadMain(void)
{
    bool result;
    unsigned int messageCode;
    void *pData;
    AsyncWriteInfo *pAsyncWriteInfo;

    CallTrace("BcmNonVolDevice", "ThreadMain");

    while (1)
    {
        result = pfMessageQueue->Receive(messageCode, pData);

        if (result)
        {
            // If we're being told to exit, then do so.
            if (messageCode == kExitThread)
            {
                break;
            }

            // Otherwise, write nonvol.
            gInfoMsg(fMessageLogSettings, "ThreadMain")
                << "Beginning async write of NonVol settings..." << endl;

            // Cast the message parameter to its correct type.
            pAsyncWriteInfo = (AsyncWriteInfo *) pData;

            // Call the helper.
            result = WriteSync(*(pAsyncWriteInfo->pSettings), pAsyncWriteInfo->section, NULL, pAsyncWriteInfo->unconditional);

            // Get rid of the parameter data.
            delete pAsyncWriteInfo;

            if (!result)
            {
                gErrorMsg(fMessageLogSettings, "ThreadMain")
                    << "Async NonVol write failed!" << endl;
            }
            else
            {
                gInfoMsg(fMessageLogSettings, "ThreadMain")
                    << "Finished async write of NonVol settings; success!" << endl;
            }
        }
    }

    // Clear out any remnants from the message queue.
    while (pfMessageQueue->NumberOfMessages() > 0)
    {
        result = pfMessageQueue->Receive(messageCode, pData);

        if (result && (messageCode == kWriteNonVol))
        {
            pAsyncWriteInfo = (AsyncWriteInfo *) pData;
            delete pAsyncWriteInfo;
        }
    }
}


// Thread destructor - this is the last method called when the thread is
// exiting, and is where the thread should delete all of the OS objects that
// it created.
//
// The default implementation does nothing, allowing derived classes that
// don't have any deinitialization to use the default.
//
// Parameters:  None.
//
// Returns:  Nothing.
//
void BcmNonVolDevice::Deinitialize(void)
{
    CallTrace("BcmNonVolDevice", "Deinitialize");

    delete pfMessageQueue;
    pfMessageQueue = NULL;
}


// The implementation of the checksum method.  We have implemented the CRC32
// algorithm specified in RFC????.
//
// Note that if you are creating the 32-bit checksum, the appropriate field
// in the buffer must be pre-set to 0's before calling this method, and only
// that field may be changed after the checksum is stored.
//
// If you are verifying the checksum, then the checksum field in the buffer
// must contain the previously calculated value.  If valid, then this method
// will return 0.
//
// If any other fields are changed, then the checksum must be recalculated.
//
// Parameters:
//      buffer - the buffer over which the checksum should be computed.
//
// Returns:
//      The checksum value that was calculated.
//
uint32 BcmNonVolDevice::CalculateChecksum(const BcmOctetBuffer &buffer)
{
    uint32 checksum;
    uint32 value32;
    uint16 value16;
    uint8 value8;
    unsigned int offset;

    CallTrace("BcmNonVolDevice", "CalculateChecksum");

    // Treat the buffer as an array of uint32 values, summing them together.
    checksum = 0;
    offset = 0;
    while (buffer.NetworkRead(offset, value32) == true)
    {
        checksum += value32;
    }

    // The buffer may not contain a complete uint32 at the end, so I need to
    // treat the remaining bytes specially.  Try to pull them in to a uint32,
    // with 0 padding for the missing bytes.
    value16 = 0;
    buffer.NetworkRead(offset, value16);

    value8 = 0;
    buffer.NetworkRead(offset, value8);

    value32 = value16;
    value32 <<= 8;
    value32 |= value8;
    value32 <<= 8;

    checksum += value32;

    // Take the one's complement of the value so that they can verify the
    // checksum by simply running it again.
    checksum = ~checksum;

    // Now return the checksum.
    return checksum;
}


