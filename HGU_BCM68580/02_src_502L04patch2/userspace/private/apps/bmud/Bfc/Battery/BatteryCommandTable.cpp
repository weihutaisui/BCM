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
//  Filename:       BatteryCommandTable.cpp
//  Author:         Kevin O'Neal
//  Creation Date:  January 13, 2005
//
//****************************************************************************
//  Description:
//      This derived class handles commands for battery specific commands.
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "BatteryCommandTable.h"
#include "BatteryThread.h"

#include "BoolCommandParameter.h"
#include "StringSetCommandParameter.h"
#include "UnsignedIntCommandParameter.h"
#include "SignedIntCommandParameter.h"
#include "FloatCommandParameter.h"

// PR16908
#include "MergedNonVolDefaults.h"

//********************** Local Types *****************************************
static BcmBatteryCommandTable *pOneAndOnly = NULL;
static BcmBatteryATECommandTable *ateCommands = NULL;

//********************** Local Constants *************************************

static const char *pgTableDescr = "Commands to show/change the Battery specific information.";
static const char *pgAteTableDescr = "Commands to excersize various battery-related auomated tesing functions.";

// Constants for the commands.
enum
{
    kShow = 0,
    kInit,
    kTestMode,
    kLog,
    kProfile,
    kPowerSource,
    kPercentCharge,
    kMinutesRemaining,
    kEnableCharge,
    kEvent,
    kTimers,
    kShowHistory,
    kShowThread,
    kTemperatureDiag,
    kErrorsDiag,
	kReadEeprom,
    kForcedDischarge,
    kStopForcedDischarge,
    kForcedCharge,
    kStopCharge,
	kPackTemp,
	kDiagnoseState,
    kClearChargeErrors,
    kClearFaults,
	kReadGUID
};

// enums for ATE commands 
enum {
	kForceWDTimeout = 0,
	kForceLifeTest,
	kLifeTestState,
	kAbortLifeTest,
	kMeasureImpedance,
	kRawControllerState,
	kDumpDischargeTable,
    kSuspendCharge,
	kTemperatureDiag_BLT,
	kTemperatureDiag_PLT,
	kForceReadGUID,
	kForceReadEPROM
};

enum
{
    kPrimaryParameter = 1,
    kSecondParameter,
    kThirdParameter
};

/* String set values */

// kPowerSource
enum
{
    kUtility = 0,
    kBattery
};


//********************** Local Variables *************************************

//********************** Class Method Implementations ************************


// Default Constructor.  Creates the command table, adds all of the
// commands to it and sets the command handler function.
//
// Parameters:  None.
//
// Returns:  N/A
//
BcmBatteryCommandTable::BcmBatteryCommandTable() :
    BcmCommandTable("Battery Commands", "battery", "battery", pgTableDescr)
{
	if (pOneAndOnly == NULL) {
		pOneAndOnly = this;
	}
	if(ateCommands == NULL) {
		ateCommands = new BcmBatteryATECommandTable;
		if (ateCommands != NULL) {
			ateCommands->AddInstance ("BFC Battery Thread", &BcmBatteryThread::Singleton());
			AddSubtable(ateCommands);
		}
	}

    BcmCommand command;

    // Set up fields that don't change.
    command.fIsEnabled = true;
    command.fIsHidden = false;
    command.fCommandFunctionWithParms = CommandHandler;
    
    command.fCommandId = kInit;
    command.pfCommandName = "init";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Initialize the battery HW.";
    command.pfExamples = "init";
    AddCommand(command);
    
    command.fCommandId = kShow;
    command.pfCommandName = "show";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Display battery information.";
    command.pfExamples = "show";
    AddCommand(command);
    
    command.fCommandId = kEnableCharge;
    command.pfCommandName = "enableCharge";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmBoolCommandParameter(kPrimaryParameter, true, "enable"));
    command.pfCommandHelp = "Enable or disable battery charging.";
    command.pfExamples = "enableCharge true\tEnable charging.\n"
                         "enableCharge false\tDisable charging.";
    AddCommand(command);
    
    command.fCommandId = kLog;
    command.pfCommandName = "logData";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmBoolCommandParameter(kPrimaryParameter, true, "enable"));
    command.pfCommandHelp = "Enable or disable battery logging mode.  In logging mode, "
                            "data retrieved from battery updates will be logged to "
                            "the console.\n";
    command.pfExamples = "logData true\tEnable data logging.\n"
                         "logData false\tDisable data logging.";
    AddCommand(command);
    
    command.fCommandId = kProfile;
    command.pfCommandName = "profileDchg";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmBoolCommandParameter(kPrimaryParameter, true, "enable"));
    command.pfCommandHelp = "Enable or disable battery discharge profiling mode.\n";
    command.pfExamples = "profileDchg true\tEnable discharge profiling.\n"
                         "profileDchg false\tDisable discharge profiling.";
    AddCommand(command);
    
    command.fCommandId = kEvent;
    command.pfCommandName = "event";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("utility|battery", kPrimaryParameter, true));
    command.pfCommandHelp = "Simulates battery system events."
                            "  utility - publish event indicating switch to utility power"
                            "  battery - publish event indicating switch to battery power"
                            "Note that this is a simulation by way of the battery system "
                            "event publisher and does *NOT* affect the actual system state.  "
                            "As a result, command 'event battery' will publish the event, and "
                            "thereby invoke power save mode, however a 'show' command in this "
                            "directory will still display the actual current power source and "
                            "other measured information.";
    command.pfExamples = "event battery\tPublish an event indicating transition to battery power.\n"
                         "event utility\tPublish an event indicating transition to utility power.\n";
    AddCommand(command);
    
    command.fCommandId = kTimers;
    command.pfCommandName = "timers";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Display battery thread timer information.";
    command.pfExamples = "timers";
    AddCommand(command);
    
    command.fCommandId = kShowHistory;
    command.pfCommandName = "samplesShow";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Display battery voltage sample history.";
    command.pfExamples = "samplesShow";
    AddCommand(command);
    
    command.fCommandId = kShowThread;
    command.pfCommandName = "threadShow";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Display battery thread information.";
    command.pfExamples = "threadShow";
    AddCommand(command);
    
    command.fCommandId = kTemperatureDiag;
    command.pfCommandName = "diagTemp";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmSignedIntCommandParameter(kPrimaryParameter, true, "degrees C", -20, 101));
    command.pfCommandHelp = "Instructs the battery controller to report the measured "
                            "temperature as the specified temperature.  This is strictly "
                            "a diagnostic tool to evaluate system behavior without the "
                            "need for an environmental chamber.  Note that setting this "
                            "to 101 degrees C will disable the diagnostic mode and return "
                            "the system to reporting the actual measured temperature.";
    command.pfExamples = "diagTemp 60\tSet the diagnostic temperature to 60 degrees C.\n"
                         "diagTemp 101\tDisable diagnostic mode, return to real temp.\n";
    AddCommand(command);
    
    command.fCommandId = kErrorsDiag;
    command.pfCommandName = "diagErrors";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
    command.pfCommandParameterList->AddOrderDependent(new BcmSignedIntCommandParameter(kSecondParameter, false, "errors", -1, 4));
    command.pfCommandHelp = "Instructs the battery controller to report the number "
                            "of charger errors as the specified number of errors.  This is strictly "
                            "a diagnostic tool to evaluate system behavior without the "
                            "need for a defective battery.  Note that setting this "
                            "to -1 will disable the diagnostic mode and return "
                            "the system to reporting the actual number of errors.";
    command.pfExamples = "diagErrors 1\tSet the diagnostic error count for the specified battery to 1.\n"
                         "diagErrors -1\tDisable diagnostic mode, return to real error count.\n";
    AddCommand(command);

    command.fCommandId = kReadEeprom;
    command.pfCommandName = "readEprom";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
    command.pfCommandParameterList->AddOrderDependent(new BcmSignedIntCommandParameter(kSecondParameter, false, "NumberOfBytes", 1, 128));
    command.pfCommandHelp = "Read the battery eeprom and display content in hex and ascii.";
    command.pfExamples = "readEprom A 32\tRead first 32 bytes of EEPROM from battery A.\n"
                         "readEprom B 64\tRead first 64 bytes of EEPROM from battery B.\n";
    AddCommand(command);

    command.fCommandId = kReadGUID;
    command.pfCommandName = "readGUID";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
    command.pfCommandHelp = "Read the battery GUID and display content in hex.";
    command.pfExamples = "readGUID A\n";
    AddCommand(command);
 
    // PR11539 --  PICO -- Add Forced Discharge/Charge capability  
    command.fCommandId = kForcedDischarge;
    command.pfCommandName = "forcedDchg";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
#ifdef kSupportsFDVTERM
	float	maxVal = 12.6;
	#if (kBatteryNumberOfCells == 2)
		maxVal = 8.4;
	#endif
	command.pfCommandParameterList->AddOrderDependent(new BcmFloatCommandParameter(kSecondParameter, false, "FDVTERM", 0, maxVal));
	command.pfCommandHelp = "Instructs the battery controller to start forced discharge on "
							"a given battery down to a specified voltage. ";
	command.pfExamples = "forcedDchg A 7.0\tForced discharge battery A";
#else
	command.pfCommandHelp = "Instructs the battery controller to start forced discharge on "
							"a given battery. ";
	command.pfExamples = "forcedDchg A\tForced discharge battery A";
#endif
    AddCommand(command);
    
    command.fCommandId = kStopForcedDischarge;
    command.pfCommandName = "stopForcedDchg";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Instructs the battery charger to stop forced discharge battery. ";
    command.pfExamples = "stopForcedDchg\tStop in-progess forced discharge";
    AddCommand(command);
    
    command.fCommandId = kForcedCharge;
    command.pfCommandName = "forcedCharge";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
#ifdef kSupportsFDVTERM     // also supports FCVTERM!
	command.pfCommandParameterList->AddOrderDependent(new BcmFloatCommandParameter(kSecondParameter, false, "FCVTERM", 0, 100));
	command.pfCommandHelp = "Instructs the battery controller to start forced charge on "
							"a given battery up to the specified percentage. ";
	command.pfExamples = "forcedCharge A 80.4\tForced charge battery A to 80.4%";
#else
    command.pfCommandHelp = "Instructs the battery charger to start forced charge on "
                            "a given battery. ";
    command.pfExamples = "forcedCharge A\tForced charge battery A";
#endif
    AddCommand(command);
    
    command.fCommandId = kStopCharge;
    command.pfCommandName = "stopCharge";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Instructs the battery charger to stop charge battery.";
    command.pfExamples = "stopCharge\tStop in-progess charge";
    AddCommand(command);

    command.fCommandId = kPackTemp;
    command.pfCommandName = "packTemp";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
    command.pfCommandHelp = "Returns the current pack temperature for the specified pack (LPV modems) or board temperature (non-LPV modems).";
    command.pfExamples = "packTemp A\tPack temperature for battery A";
    AddCommand(command);

    command.fCommandId = kDiagnoseState;
    command.pfCommandName = "diagnoseState";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Attempts to diagnose the current state of the microcontroller.";
    command.pfExamples = "diagnoseState";
    AddCommand(command);

    command.fCommandId = kClearChargeErrors;
    command.pfCommandName = "clearChgErrs";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
    command.pfCommandHelp = "Clears existing charge errors for the specified battery";
    command.pfExamples = "clearChgErrs A";
    AddCommand(command);
}

// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmBatteryCommandTable::~BcmBatteryCommandTable()
{
	if (ateCommands != NULL)
	{
		RemoveSubtable(ateCommands);
		delete ateCommands;
		ateCommands = NULL;
	}
}


// This is the entrypoint that is called whenever one of my commands is
// entered.
//
// Parameters:
//      pInstanceValue - the instance value to which the command should be
//                       applied.  Instances must be registered with the
//                       command table in order for them to receive the
//                       command.
//      command - the command (with parameters) that was entered by the user.
//
// Returns:  Nothing.
//
void BcmBatteryCommandTable::CommandHandler(void *pInstanceValue, const BcmCommand &command)
{
    BcmBatteryThread *pBattThread = (BcmBatteryThread *) pInstanceValue;

    // Many commands have at least one parameter; retrieve it for them.
    const BcmCommandParameter *pParameter = NULL;

    if (command.pfCommandParameterList != NULL)
    {
        pParameter = command.pfCommandParameterList->Find(kPrimaryParameter);
    }

    switch (command.fCommandId)
    {
        case kInit:
            pBattThread->fBatteryController.Initialize();
            break;
            
        case kShow:
            gLogMessageRaw << pBattThread->BatteryController() << endl;
            break;
            
        case kShowThread:
            gLogMessageRaw << *pBattThread << endl;
            break;
            
        case kShowHistory:
            pBattThread->BatteryController().PrintHistory();
            break;

		case kEnableCharge:
        {
            if (pParameter->WasParsed() == true)
              pBattThread->fBatteryController.BatteryEnableCharging (pParameter->AsBool());
            
            gLogMessageRaw << "Charging enabled: " << pBattThread->fBatteryController.BatteryChargingEnabled() << endl;
            
            break;
        }
      
        case kLog:
        {
            if (pParameter->WasParsed() == true)
              pBattThread->EnableLogging (pParameter->AsBool());
            
            gLogMessageRaw << "Data logging: " << pBattThread->LoggingEnabled() << endl;
            
            break;
        }
        
        case kProfile:
        {
            if (pParameter->WasParsed() == true)
              pBattThread->EnableProfiling (pParameter->AsBool());
            
            gLogMessageRaw << "Discharge profiling: " << pBattThread->ProfilingEnabled() << endl;
            
            break;
        }
        
        case kEvent:
        {
            if (pParameter->WasParsed() == true)
            {
              if (pParameter->AsUnsignedInt() == kUtility)
              {
                BcmBatteryThread::Singleton().SimulateTransitionToBatteryPower(false);
              }
              else if (pParameter->AsUnsignedInt() == kBattery)
              {
                BcmBatteryThread::Singleton().SimulateTransitionToBatteryPower(true); 
              }
              else
                gLogMessageRaw << "Undefined event code " << pParameter->AsUnsignedInt() << endl;
            }
            
            break;
        }
        
        case kTimers:
            pBattThread->PrintTimers(gLogMessageRaw);
            break;
            
        case kTemperatureDiag:
        {
            if (pParameter->WasParsed() == true)
            {
              int DiagTemp = TEMP_MEASUREMENT_NOT_SUPPORTED;
            
              if (pParameter->AsSignedInt() != 101)
              {
                DiagTemp = pParameter->AsSignedInt();
                gLogMessageRaw << "Setting diagnostic temperature to " << DiagTemp << endl;
              }
              else
                gLogMessageRaw << "Temperature diagnostic mode disabled." << endl;
              
              pBattThread->fBatteryController.SetDiagTemperature(DiagTemp);
            }
            
            break;
        }
        
        case kErrorsDiag:
        {
            if (pParameter->WasParsed() == true)
            {
              const BcmCommandParameter *pParameter2 = command.pfCommandParameterList->Find(kSecondParameter);
              
              if ((pParameter2 != NULL) && (pParameter2->WasParsed() == true))
              {
                unsigned int Battery = pParameter->AsUnsignedInt();
                int Errors = pParameter2->AsSignedInt();
                
                gLogMessageRaw << "Setting charger errors for battery " << (char)('A' + Battery) << " to " << Errors << endl;
                pBattThread->fBatteryController.fDiagBatteryErrors[Battery] = Errors;
              }
            }
            
            break;
        }

		case kReadGUID:
		{
            if (pParameter->WasParsed() == true)
            {
				int Battery = pParameter->AsUnsignedInt();
				char output[64];
				uint64 guid = pBattThread->fBatteryController.GetGUID(Battery);
				if(!guid)
					strcpy(output, "unable to read GUID or specified pack not present");
				else
					sprintf(output, "0x%016llx", pBattThread->fBatteryController.GetGUID(Battery)); 
				gLogMessageRaw << "Battery " << (char)('A' + Battery) << " GUID = " << output << endl;
			}
            break;
		}
	    case kReadEeprom:
        {
            if (pParameter->WasParsed() == true)
            {
              const BcmCommandParameter *pParameter2 = command.pfCommandParameterList->Find(kSecondParameter);
              
              if ((pParameter2 != NULL) && (pParameter2->WasParsed() == true))
              {
                int Battery = pParameter->AsUnsignedInt();
                int NumBytes = pParameter2->AsSignedInt();
				char testBuf[NumBytes];
                
                gLogMessageRaw << "Reading EPROM from Battery " << (char)('A' + Battery) << ", Number of Bytes " << NumBytes << ":" << endl;
				
				if(pBattThread->fBatteryController.BatteryEPROMRead(Battery, testBuf, NumBytes))
				{
				   // Print EEPROM Contents.
				   for (int i=0; i<NumBytes; i++)
				   {
					  // Every 16 byte is a line
				      int Mod16 = i%16;
					  int Line  = i/16;

					  // The first byte of the line get the address
				      if (Mod16 == 0) 
                      {
						 gLogMessageRaw << endl << hex << setfill('0') << setw(2) << ((i/16) * 16) << ": " << dec << setfill( ' ' );
					  }
                        
					  // print out hex
					  gLogMessageRaw << hex << setfill('0') << setw(2) << ((uint16)testBuf[i] & 0x00FF) << " " << dec << setfill( ' ' );

					  // print out ASCII
					  if (Mod16 == 15)
					  {
					    gLogMessageRaw << " | ";

						for (int j=0; j<16; j++)
						{
                          if ((testBuf[Line*16+j] < 0x20) || (testBuf[Line*16+j] >= 0x7F))
						    gLogMessageRaw << ".";
						  else
                            gLogMessageRaw << testBuf[Line*16+j];
						}
					  }
					  else if ((i+1 == NumBytes) && (Mod16 != 15))
					  { 
					    for(int j=0; j<(15-Mod16); j++) gLogMessageRaw << "   ";
						gLogMessageRaw << " | ";

						for (int j=0; j<=Mod16; j++)
						{
                          if ((testBuf[Line*16+j] < 0x20) || (testBuf[Line*16+j] >= 0x7F))
						    gLogMessageRaw << ".";
						  else
                            gLogMessageRaw << testBuf[Line*16+j];
						}
					  }
				   }  
				}
				else
				{
				   gLogMessageRaw << "Failed reading battery EPROM or specified pack not present." << endl;
				}
              }
            }
      
            break;
        }

        // PR11539 --  PICO -- Add Forced Discharge/Charge capability
        case kForcedDischarge:
        {
            if (pParameter->WasParsed() == true)
            {
				float fdvterm = 0;
				unsigned int Battery = pParameter->AsUnsignedInt();
			#ifdef kSupportsFDVTERM
				const BcmCommandParameter *pParameter2 = command.pfCommandParameterList->Find(kSecondParameter);
				if ((pParameter2 != NULL) && (pParameter2->WasParsed() == true))
				{
					fdvterm = pParameter2->AsFloat();
				}
			#endif
                if(pBattThread->fBatteryController.BatteryForcedDischarge(Battery,fdvterm))
                {
                    gLogMessageRaw << "Forcing battery " << (char)('A' + Battery) << " to discharge." << endl;;
                }
                else
                {
                    gLogMessageRaw << "Command failed.  Either charger is not in sleep state, specified pack not present, or other errors." << endl;
                }
            }
            break;
        }

        case kStopForcedDischarge:
        {   
            if(pBattThread->fBatteryController.BatteryStopForcedDischarge())
            {
                gLogMessageRaw << "Stop in-progress forced discharge." << endl;
            }
            else
            {
                gLogMessageRaw << "Command failed.  Charger must be in forced discharge state." << endl;
            }
            break;
        }

        case kForcedCharge:
        {
            if (pParameter->WasParsed() == true)
            {
                unsigned int Battery = pParameter->AsUnsignedInt();
			#ifdef kSupportsFDVTERM
				const BcmCommandParameter *pParameter2 = command.pfCommandParameterList->Find(kSecondParameter);
				if ((pParameter2 != NULL) && (pParameter2->WasParsed() == true))
				{
					float fcvterm = pParameter2->AsFloat();
                    if(pBattThread->fBatteryController.BatteryForcedChargePct(Battery, fcvterm))
                        gLogMessageRaw << "Forcing battery " << (char)('A' + Battery) << " to charge to " << fcvterm << "%" << endl;
                    else
                        gLogMessageRaw << "Forced charge command failed or specified pack not present." << endl;
				}
            #else
                if(pBattThread->fBatteryController.BatteryForcedCharge(Battery))
                    gLogMessageRaw << "Forcing battery " << (char)('A' + Battery) << " to charge." << endl;
                else
                    gLogMessageRaw << "Forced charge command failed or specified pack not present." << endl;
			#endif
            }
            break;
        }

        case kStopCharge:
        {
            if(pBattThread->fBatteryController.BatteryStopCharge())
            {
                gLogMessageRaw << "Stop in-progress charge." << endl;
            }
            else
            {
                gLogMessageRaw << "Command failed.  Charger must be in charge state." << endl;
            }  
            break;
        }
		case kPackTemp:
		{
			if (pParameter->WasParsed() == true)
			{
				unsigned int Battery = pParameter->AsUnsignedInt();
				int temperature = pBattThread->fBatteryController.PackTemperature(Battery);
				if(temperature != TEMP_MEASUREMENT_NOT_SUPPORTED)
				{
					gLogMessageRaw << "Pack " << (char)('A' + Battery) << " temperature = " << temperature << "degC" << endl;
				}
				else
				{
					gLogMessageRaw << "No temperature value available or specified pack not present." << endl;
				}
			}
			break;
		}
		case kDiagnoseState:
		{
			BcmBatteryThread::Singleton().BatteryController().DiagnoseState();
			break;
		}
        case kClearChargeErrors:
        {
			if (pParameter->WasParsed() == true)
			{
				unsigned int Battery = pParameter->AsUnsignedInt();
                BcmBatteryThread::Singleton().BatteryController().ClearChargeErrors(Battery);
            }
            break;
        }
        case kClearFaults:
        {
            BcmBatteryThread::Singleton().BatteryController().ClearAllFaults();
            break;
        }
        default:
            gLogMessageRaw << "WARNING - unknown command id (" << command.fCommandId << ")!  Ignoring...\n";
            break;
    }  
}

void BcmBatteryCommandTable::AddLifeTestCommands()
{
	if (ateCommands) {
		BcmCommand command;
		// Set up fields that don't change.
		command.fIsEnabled = true;
		command.fIsHidden = false;
		command.fCommandFunctionWithParms = ateCommands->CommandHandler;
	
		command.fCommandId = kForceLifeTest;
		command.pfCommandName = "forceLifeTest";
		command.pfCommandParameterList = new BcmCommandParameterList;
		command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
		command.pfCommandHelp = "Force a battery life cycle test on the specified battery pack.";
		command.pfExamples = "forceLifeTest A\tForces a life test on pack A";
		ateCommands->AddCommand(command);
	
		command.fCommandId = kLifeTestState;
		command.pfCommandName = "lifeTestState";
		command.pfCommandParameterList = NULL;
		command.pfCommandHelp = "Retrieve the current life test state";
		command.pfExamples = "lifeTestState\tRetrieve the current life test state";
		ateCommands->AddCommand(command);
	
		command.fCommandId = kAbortLifeTest;
		command.pfCommandName = "abortLifeTest";
		command.pfCommandParameterList = NULL;
		command.pfCommandHelp = "Abort an in-process life test - no effect if a life test is not in process.";
		command.pfExamples = "abortLifeTest\tAbort an in-process life test";
		ateCommands->AddCommand(command);

		if( BcmBatteryThread::Singleton().BatteryController().fDriverCalls.pfMakeHWResistanceMeasurement != NULL )
		{
			command.fCommandId = kMeasureImpedance;
			command.pfCommandName = "measImpedance";
			command.pfCommandParameterList = new BcmCommandParameterList;
			command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
            command.pfCommandHelp = "Takes a hardware impedance measurement of the specified battery.";
			command.pfExamples = "measImpedance\tPerform hardware impedance measurement in specified battery";
			ateCommands->AddCommand(command);
		}
        // add hardware fault clearing command to the regular menu (not the ATE menu)
        if(BcmBatteryThread::Singleton().BatteryController().SupportsHardwareFaults())
        {
            command.fCommandId = kClearFaults;
            command.fCommandFunctionWithParms = CommandHandler;
            command.pfCommandName = "clearHWFaults";
            command.pfCommandParameterList = NULL;
            command.pfCommandHelp = "Clears existing hardware faults";
            command.pfExamples = "clearHWFaults";
            AddCommand(command);
        }
	}
}

BcmBatteryCommandTable *BcmBatteryCommandTable::Singleton()
{
	return pOneAndOnly;
}

BcmBatteryATECommandTable::BcmBatteryATECommandTable() :
    BcmCommandTable("ATE commands", "ate", "ate", pgAteTableDescr)
{
    BcmCommand command;

    // Set up fields that don't change.
    command.fIsEnabled = true;
    command.fIsHidden = false;
    command.fCommandFunctionWithParms = CommandHandler;
    
 	// PR16908 - Add Pico watchdog
#ifdef kSupportsBatteryWatchdog
	command.fCommandId = kForceWDTimeout;
	command.pfCommandName = "forceWDtimeout";
	command.pfCommandParameterList = NULL;
	command.pfCommandHelp = "Forces a battery microcontroller watchdog timeout.";
	command.pfExamples = "forceWDtimeout\tForce battery microcontroller watchdog timeout";
	AddCommand(command);
#endif
   
    command.fCommandId = kRawControllerState;
    command.pfCommandName = "rawPicoState";
    command.pfCommandParameterList = NULL;
    command.pfCommandHelp = "Retrieve the raw battery microcontroller state.";
    command.pfExamples = "rawPicoState";
    AddCommand(command);

#if !(BCM_REDUCED_IMAGE_SIZE)
	command.fCommandId = kDumpDischargeTable;
	command.pfCommandName = "dumDchgTbl";
	command.pfCommandParameterList = new BcmCommandParameterList;
	command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
	command.pfCommandHelp = "Dump the specified discharge table.";
	command.pfExamples = "dumDchgTbl A";
	AddCommand(command);
#endif
    
    command.fCommandId = kSuspendCharge;
    command.pfCommandName = "suspCharge";
    command.pfCommandParameterList = new BcmCommandParameterList;
    command.pfCommandParameterList->AddOrderDependent(new BcmBoolCommandParameter(kPrimaryParameter, true, "enable"));
    command.pfCommandHelp = "Suspend or re-allow battery charging.";
    command.pfExamples = "suspCharge true\tSuspend charging.\n"
                         "suspCharge false\tRe-allow charging.";
    AddCommand(command);

	command.fCommandId = kTemperatureDiag_BLT;
	command.pfCommandName = "diagTemp_blt";
	command.pfCommandParameterList = new BcmCommandParameterList;
	command.pfCommandParameterList->AddOrderDependent(new BcmSignedIntCommandParameter(kPrimaryParameter, true, "degrees C", -20, 101));
	command.pfCommandHelp = "Instructs the battery controller to report the BLT "
							"temperature as the specified temperature.  This is strictly "
							"a diagnostic tool to evaluate system behavior without the "
							"need for an environmental chamber.  Note that setting this "
							"to 101 degrees C will disable the diagnostic mode and return "
							"the system to reporting the actual measured temperature.";
	command.pfExamples = "diagTemp_blt 60\tSet the BLT diagnostic temperature to 60 degrees C.\n"
						 "diagTemp_blt 101\tDisable BLT diagnostic mode, return to real temp.\n";
	AddCommand(command);

	command.fCommandId = kTemperatureDiag_PLT;
	command.pfCommandName = "diagTemp_plt";
	command.pfCommandParameterList = new BcmCommandParameterList;
	command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
	command.pfCommandParameterList->AddOrderDependent(new BcmSignedIntCommandParameter(kSecondParameter, true, "degrees C", -20, 101));
	command.pfCommandHelp = "Instructs the battery controller to report the specified PLT "
							"temperature as the specified temperature.  This is strictly "
							"a diagnostic tool to evaluate system behavior without the "
							"need for an environmental chamber.  Note that setting this "
							"to 101 degrees C will disable the diagnostic mode and return "
							"the system to reporting the actual measured temperature.";
	command.pfExamples = "diagTemp_plt A 60\tSet PLT A diagnostic temperature to 60 degrees C.\n"
						 "diagTemp_plt A 101\tDisable PLT A diagnostic mode, return to real temp.\n";
	AddCommand(command);

	command.fCommandId = kForceReadGUID;
	command.pfCommandName = "forceReadGUID";
	command.pfCommandParameterList = new BcmCommandParameterList;
	command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
	command.pfCommandHelp = "Instructs the battery controller to forceably read the GUID for the specified pack.";
	command.pfExamples = "forceReadGUID A\n";
	AddCommand(command);

	command.fCommandId = kForceReadEPROM;
	command.pfCommandName = "forceReadEPROM";
	command.pfCommandParameterList = new BcmCommandParameterList;
	command.pfCommandParameterList->AddOrderDependent(new BcmStringSetCommandParameter("A|B", kPrimaryParameter, false));
	command.pfCommandHelp = "Instructs the battery controller to forceably read the entire EPROM for the specified pack.";
	command.pfExamples = "forceReadEPROM A\n";
	AddCommand(command);
}

// Destructor.  Frees up any memory/objects allocated, cleans up internal
// state.
//
// Parameters:  N/A
//
// Returns:  N/A
//
BcmBatteryATECommandTable::~BcmBatteryATECommandTable()
{
}

// This is the entrypoint that is called whenever one of my commands is
// entered.
//
// Parameters:
//      pInstanceValue - the instance value to which the command should be
//                       applied.  Instances must be registered with the
//                       command table in order for them to receive the
//                       command.
//      command - the command (with parameters) that was entered by the user.
//
// Returns:  Nothing.
//
void BcmBatteryATECommandTable::CommandHandler(void *pInstanceValue, const BcmCommand &command)
{
    // Many commands have at least one parameter; retrieve it for them.
	BcmBatteryThread *pBattThread = (BcmBatteryThread *) pInstanceValue;
    const BcmCommandParameter *pParameter = NULL;

    if (command.pfCommandParameterList != NULL)
    {
        pParameter = command.pfCommandParameterList->Find(kPrimaryParameter);
    }

    switch (command.fCommandId)
    {
		case kForceWDTimeout:
		{
			pBattThread->BatteryController().ForceWatchdogTimeout();
			break;
		}
		case kForceLifeTest:
		{
			if (pParameter->WasParsed() == true)
			{
				unsigned int Battery = pParameter->AsUnsignedInt();
				pBattThread->BatteryController().ForceLifeTest(Battery);
			}
			break;
		}
		case kLifeTestState:
		{
			int state = pBattThread->BatteryController().LifeTestState();
			gLogMessageRaw << "Current life test state = " << state << endl;
			break;
		}
		case kAbortLifeTest:
		{
			int state = pBattThread->BatteryController().LifeTestState();
			gLogMessageRaw << "Current life test state = " << state << ", aborting" << endl;

			pBattThread->BatteryController().AbortLifeTest();
			break;
		}
		case kRawControllerState:
		{
			gLogMessageRaw << "Raw controller state = " << pBattThread->BatteryController().RawControllerState() << endl;
			break;
		}
		case kMeasureImpedance:
		{
			if (pParameter->WasParsed() == true)
			{
				unsigned int Battery = pParameter->AsUnsignedInt();
				float z = 0.0;
				if( pBattThread->BatteryController().MeasureHWImpedance(Battery, z) )
					gLogMessageRaw << "Battery " << Battery << "'s impedance = " << z << " ohms" << endl;
				else
					gLogMessageRaw << "Hardware impedance test failed or specified pack not present." << endl;
			}
			break;
		}
	#if !(BCM_REDUCED_IMAGE_SIZE)
		case kDumpDischargeTable:
		{
			if (pParameter->WasParsed() == true)
			{
				unsigned int Battery = pParameter->AsUnsignedInt();
				pBattThread->BatteryController().DumpBatteryDischargeTable(Battery);
			}
			break;
		}
	#endif
        case kSuspendCharge:
        {
			if (pParameter->WasParsed() == true)
			{
				bool suspend = pParameter->AsBool();
				pBattThread->BatteryController().BatterySuspendCharging(suspend);
			}
			break;
        }
		case kTemperatureDiag_BLT:
			{
				if (pParameter->WasParsed() == true)
				{
					int diagTemp = pParameter->AsSignedInt();
					if(diagTemp == 101)
					{
						diagTemp = TEMP_MEASUREMENT_NOT_SUPPORTED;
						gLogMessageRaw << "BLT diag temperature disabled" << endl;
					}
					else
					{
						gLogMessageRaw << "Setting BLT diag temperature to " << diagTemp << endl;
					}
					pBattThread->fBatteryController.fDiagBLTTemperature = diagTemp;
				}
				break;
			}
		case kTemperatureDiag_PLT:
			{
				if (pParameter->WasParsed() == true)
				{
				  const BcmCommandParameter *pParameter2 = command.pfCommandParameterList->Find(kSecondParameter);

				  if ((pParameter2 != NULL) && (pParameter2->WasParsed() == true))
				  {
					unsigned int Battery = pParameter->AsUnsignedInt();
					int diagTemp = pParameter2->AsSignedInt();
					if( diagTemp == 101)
					{
						diagTemp = TEMP_MEASUREMENT_NOT_SUPPORTED;
						gLogMessageRaw << "PLT " << (char)('A' + Battery) << " diag temperature disabled" << endl;
					}
					else
					{
						gLogMessageRaw << "Setting PLT " << (char)('A' + Battery) << " diag temperature to " << diagTemp << endl;
					}
					pBattThread->fBatteryController.fDiagPLTTemperature[Battery] = diagTemp;
				  }
				}
				break;
			}
		case kForceReadGUID:
			{
				if (pParameter->WasParsed() == true)
				{
					unsigned int Battery = pParameter->AsUnsignedInt();
					uint64 guid;
					if( pBattThread->fBatteryController.ForceReadGUID(Battery, guid))
						gLogMessageRaw << "GUID " << (char)('A' + Battery) << ": 0x" << hex << guid << dec << endl;
					else
						gLogMessageRaw << "Failed to read GUID" << endl;
				}
				break;
			}
		case kForceReadEPROM:
			{
				if (pParameter->WasParsed() == true)
				{
					unsigned int Battery = pParameter->AsUnsignedInt();
					BcmString eprom;
					if( pBattThread->fBatteryController.ForceReadEPROM(Battery, eprom))
						gLogMessageRaw << "EPROM " << (char)('A' + Battery) << ":" << endl << eprom << endl;
					else
						gLogMessageRaw << "Failed to read EPROM" << endl;
				}
				break;
			}
		default:
			gLogMessageRaw << "WARNING - unknown command id (" << command.fCommandId << ")!  Ignoring...\n";
			break;
	}
}
            

