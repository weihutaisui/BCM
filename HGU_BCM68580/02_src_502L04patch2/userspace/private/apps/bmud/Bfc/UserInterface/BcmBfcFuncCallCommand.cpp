//****************************************************************************
//
// Copyright (c) 2009 Broadcom Corporation
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
//
//  Filename:       BcmBfcFuncCallCommand.cpp
//  Author:         dpullen
//  Creation Date:  9/25/2009
//
//****************************************************************************

//********************** Include Files ***************************************

// My api and definitions...
#include "BcmBfcFuncCallCommand.h"

#include "MessageLog.h"

#include "UnsignedIntCommandParameter.h"
#include "FlagCommandParameter.h"
#include "StringSetCommandParameter.h"

//********************** Local Types *****************************************

//********************** Local Constants *************************************

// These are parameters for the "call {function}" command
enum
{
    kCallFunction_FuncType = 0,
    kCallFunction_ReturnValue,
    kCallFunction_Address,
    kCallFunction_Parameter1,
    kCallFunction_Parameter2,
    kCallFunction_Parameter3,
    kCallFunction_Parameter4,
    kCallFunction_Parameter5,
    kCallFunction_Parameter6,
    kCallFunction_Parameter7,
    kCallFunction_Parameter8,
    kCallFunction_Parameter9,
};

//********************** Local Variables *************************************

//********************** Local Functions *************************************

//********************** Class Method Implementations ************************


BcmBfcFuncCallCommand::BcmBfcFuncCallCommand(void) :
    BcmCommand()
{
    // Set protected and private members to a known state.
    //
    // This is the one field that I don't know and can't set.  It's up to the
    // code that creates me to give me a unique id value.
    //fCommandId = kCallFunction;

    fIsEnabled = true;
    fCommandFunctionWithParms = CommandHandler;

    // The rest I can do on my own.
    pfCommandName = "call";
    pfCommandParameterList = new BcmCommandParameterList;

    // UNFINISHED - I'd like to be able to new/delete objects, but that's rather
    // tricky.  Leave it out for now...
    pfCommandParameterList->AddOrderDependent(
        new BcmStringSetCommandParameter("function|malloc|free", kCallFunction_FuncType));
        //new BcmStringSetCommandParameter("function|malloc|free|new|delete", kCallFunction_FuncType));

    pfCommandParameterList->AddOrderDependent(
        new BcmFlagCommandParameter('r', kCallFunction_ReturnValue, true));
    pfCommandParameterList->AddOrderDependent(
        new BcmFlagCommandParameter('a', kCallFunction_Address, true,
                                    new BcmUnsignedIntCommandParameter(kCallFunction_Address, false, "FuncAddress")));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter1, true, "Param1"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter2, true, "Param2"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter3, true, "Param3"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter4, true, "Param4"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter5, true, "Param5"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter6, true, "Param6"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter7, true, "Param7"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter8, true, "Param8"));
    pfCommandParameterList->AddOrderDependent(
        new BcmUnsignedIntCommandParameter(kCallFunction_Parameter9, true, "Param9"));
    pfCommandHelp = 
        "This command allows you to call an arbitrary function in the code.  It "
        "also provides specific support for common functions (such as malloc/free).  "
        "When calling a function, you specify the address of the function (which "
        "must come from the MAP file for your image).  You can also specify up to "
        "9 parameters to the function.  Additionally, you can specify whether or "
        "not the function returns anything, allowing the return value to be printed.\n"
        "\n"
        "NOTE:  While this command is very useful for tricky debug situations in the "
        "field, IT IS SERIOUSLY UNSAFE!  You can easily crash the system you are "
        "trying to debug if you specify a function address or parameter incorrectly.  "
        "You could even brick the system if the function happens to write to permanent "
        "nonvol at the wrong time, or corrupts the bootloader, or...you get the picture.  "
        "Make sure you are working with a valid MAP file for the image that is running, "
        "as this is your only hope for getting things right!  Also, make sure you "
        "provide exactly the number of function parameters that are needed for the "
        "function.  Entering too few or too many parameters will cause registers and "
        "stack state to be wrong for the function, and will lead to mayhem.\n"
        "\n"
        "Functions that require more than 9 parameters cannot be called, and they are "
        "unlikely candidates for this type of thing anyway.  We chose to allow 9 because "
        "C++ methods use the first parameter as the object pointer (e.g. the 'this' "
        "pointer), which allows up to 8 real parameters to the method.\n"
        "\n"
        "The first command parameter must be one of the following:\n"
        "  'function' - causes the function address (specified by -a) to be "
        "called with however many function parameters are specified.\n"
        "  'malloc' - causes malloc() to be called with the value specified by "
        "the first function parameter.\n"
        "  'free' - causes free() to be called with the value specified by the "
        "first function parameter.\n"
        "\n"
        "The remaining command parameters are:\n"
        "  -r indicates that the function returns a value, and that the return "
        "value should be printed.  If left out, the return value (if any) is not "
        "printed.  The value is not interpreted, it is simply printed as a uint32 "
        "value in decimal and hex.\n"
        "  -a specifies the address of the function to be called.  This is only "
        "used for 'call function', since the other named functions know which "
        "address they are calling.\n"
        "  Param1..9 specify up to 9 parameters to be passed to the function.  Make "
        "sure you specify the correct number of function parameters (and that they "
        "are in the right order).  Note that when calling C++ methods, the first "
        "parameter is used to hold the object instance (e.g. the 'this' pointer).  "
        "For functions that take no parameters, don't specify any parameters here.\n"
        "\n"
        "One of the interesting aspects of this command is that it allows you to "
        "allocate memory, write to it, call a function with it, then free it.  "

        // UNFINISHED since I don't support new/delete yet, don't write anything
        // about it.
//        "Even more interesting is that you can create a C++ object, then call "
//        "methods of that object (or pass it to other objects/functions), and "
//        "delete the object when you are done."
        ;
    pfExamples =
        "Calls malloc(1024); the allocated memory address is printed:\n"
        "  call malloc -r 1024\n"
        "Calls free(0x80001234) to free memory that had previously been allocated:\n"
        "  call free 0x80001234\n"
        "Calls the function at address 0x80010080 with no parameters.  Return value not printed:\n"
        "  call func -a 0x80010080\n"
        "Calls the function at address 0x800100a0 with a single parameter (value 0x01) and prints the return value:\n"
        "  call func -r -a 0x800100a0 0x01\n"
        "Calls the function at address 0x80020020 with 4 parameters, and prints the return value:\n"
        "  call func -r -a 0x80020020 0x01 0x0202 0x030303 0x04040404\n"
        "Calls the function at address 0x80020020 with 9 parameters, and prints the return value:\n"
        "  call func -r -a 0x80020020 0x11 0x22 0x33 0x44 0x55 0x66 0x77 0x88 0x99\n"
        ;
}


BcmBfcFuncCallCommand::~BcmBfcFuncCallCommand()
{
    // Nothing to do.
}


void BcmBfcFuncCallCommand::CommandHandler(void *pInstanceValue,
                                           const BcmCommand &command)
{
    // These correspond to the kCallFunction_FuncType parameter.
    enum
    {
        kCallFunctionAddress = 0,
        kCallMalloc = 1,
        kCallFree = 2,
    };

    const BcmCommandParameter *pFuncTypeParameter = command.pfCommandParameterList->Find(kCallFunction_FuncType);
    const BcmCommandParameter *pReturnValueParameter = command.pfCommandParameterList->Find(kCallFunction_ReturnValue);
    const BcmCommandParameter *pFuncAddressParameter = command.pfCommandParameterList->Find(kCallFunction_Address);
    const BcmCommandParameter *pFuncParameterValue1 = command.pfCommandParameterList->Find(kCallFunction_Parameter1);
    const BcmCommandParameter *pFuncParameterValue2 = command.pfCommandParameterList->Find(kCallFunction_Parameter2);
    const BcmCommandParameter *pFuncParameterValue3 = command.pfCommandParameterList->Find(kCallFunction_Parameter3);
    const BcmCommandParameter *pFuncParameterValue4 = command.pfCommandParameterList->Find(kCallFunction_Parameter4);
    const BcmCommandParameter *pFuncParameterValue5 = command.pfCommandParameterList->Find(kCallFunction_Parameter5);
    const BcmCommandParameter *pFuncParameterValue6 = command.pfCommandParameterList->Find(kCallFunction_Parameter6);
    const BcmCommandParameter *pFuncParameterValue7 = command.pfCommandParameterList->Find(kCallFunction_Parameter7);
    const BcmCommandParameter *pFuncParameterValue8 = command.pfCommandParameterList->Find(kCallFunction_Parameter8);
    const BcmCommandParameter *pFuncParameterValue9 = command.pfCommandParameterList->Find(kCallFunction_Parameter9);

    uint32 returnValue = 0;
    bool printReturnValue = pReturnValueParameter->WasParsed();

    // Check the function type to see what we are supposed to do.
    if (pFuncTypeParameter->AsUnsignedInt() == kCallMalloc)
    {
        if (pFuncParameterValue1->WasParsed())
        {
            gLogMessageRaw
                << "Calling malloc(" << pFuncParameterValue1->AsUnsignedInt() << ")\n" << flush;

            returnValue = (uint32) malloc(pFuncParameterValue1->AsUnsignedInt());

            // Force -r to be specified, just in case the user left
            // it out...
            printReturnValue = true;
        }
        else
        {
            gLogMessageRaw
                << "ERROR - Param1 (malloc size) not specified!\n";
        }
    }
    else if (pFuncTypeParameter->AsUnsignedInt() == kCallFree)
    {
        if (pFuncParameterValue1->WasParsed())
        {
            gLogMessageRaw
                << "Calling free(" << inHex(pFuncParameterValue1->AsUnsignedInt()) << ")\n" << flush;

            free((void *) (pFuncParameterValue1->AsUnsignedInt()));
        }
    }
    else if (pFuncTypeParameter->AsUnsignedInt() == kCallFunctionAddress)
    {
        if (pFuncAddressParameter->WasParsed())
        {
            // Now comes the tedious part.  I need to have 10 different
            // function prototypes, each with a different number of
            // parameters (including one with 0 parameters).
            if (pFuncParameterValue9->WasParsed())
            {
                typedef uint32 (*Func9Args)(uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32);

                Func9Args function = (Func9Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue3->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue4->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue5->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue6->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue7->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue8->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue9->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt(),
                                       pFuncParameterValue3->AsUnsignedInt(),
                                       pFuncParameterValue4->AsUnsignedInt(),
                                       pFuncParameterValue5->AsUnsignedInt(),
                                       pFuncParameterValue6->AsUnsignedInt(),
                                       pFuncParameterValue7->AsUnsignedInt(),
                                       pFuncParameterValue8->AsUnsignedInt(),
                                       pFuncParameterValue9->AsUnsignedInt());
            }
            else if (pFuncParameterValue8->WasParsed())
            {
                typedef uint32 (*Func8Args)(uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32);

                Func8Args function = (Func8Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue3->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue4->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue5->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue6->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue7->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue8->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt(),
                                       pFuncParameterValue3->AsUnsignedInt(),
                                       pFuncParameterValue4->AsUnsignedInt(),
                                       pFuncParameterValue5->AsUnsignedInt(),
                                       pFuncParameterValue6->AsUnsignedInt(),
                                       pFuncParameterValue7->AsUnsignedInt(),
                                       pFuncParameterValue8->AsUnsignedInt());
            }
            else if (pFuncParameterValue7->WasParsed())
            {
                typedef uint32 (*Func7Args)(uint32, uint32, uint32, uint32, uint32, uint32, uint32);

                Func7Args function = (Func7Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue3->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue4->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue5->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue6->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue7->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt(),
                                       pFuncParameterValue3->AsUnsignedInt(),
                                       pFuncParameterValue4->AsUnsignedInt(),
                                       pFuncParameterValue5->AsUnsignedInt(),
                                       pFuncParameterValue6->AsUnsignedInt(),
                                       pFuncParameterValue7->AsUnsignedInt());
            }
            else if (pFuncParameterValue6->WasParsed())
            {
                typedef uint32 (*Func6Args)(uint32, uint32, uint32, uint32, uint32, uint32);

                Func6Args function = (Func6Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue3->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue4->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue5->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue6->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt(),
                                       pFuncParameterValue3->AsUnsignedInt(),
                                       pFuncParameterValue4->AsUnsignedInt(),
                                       pFuncParameterValue5->AsUnsignedInt(),
                                       pFuncParameterValue6->AsUnsignedInt());
            }
            else if (pFuncParameterValue5->WasParsed())
            {
                typedef uint32 (*Func5Args)(uint32, uint32, uint32, uint32, uint32);

                Func5Args function = (Func5Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue3->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue4->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue5->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt(),
                                       pFuncParameterValue3->AsUnsignedInt(),
                                       pFuncParameterValue4->AsUnsignedInt(),
                                       pFuncParameterValue5->AsUnsignedInt());
            }
            else if (pFuncParameterValue4->WasParsed())
            {
                typedef uint32 (*Func4Args)(uint32, uint32, uint32, uint32);

                Func4Args function = (Func4Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue3->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue4->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt(),
                                       pFuncParameterValue3->AsUnsignedInt(),
                                       pFuncParameterValue4->AsUnsignedInt());
            }
            else if (pFuncParameterValue3->WasParsed())
            {
                typedef uint32 (*Func3Args)(uint32, uint32, uint32);

                Func3Args function = (Func3Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue3->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt(),
                                       pFuncParameterValue3->AsUnsignedInt());
            }
            else if (pFuncParameterValue2->WasParsed())
            {
                typedef uint32 (*Func2Args)(uint32, uint32);

                Func2Args function = (Func2Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ", " << inHex(pFuncParameterValue2->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt(),
                                       pFuncParameterValue2->AsUnsignedInt());
            }
            else if (pFuncParameterValue1->WasParsed())
            {
                typedef uint32 (*Func1Args)(uint32);

                Func1Args function = (Func1Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "(" << inHex(pFuncParameterValue1->AsUnsignedInt())
                    << ")\n" << flush;

                returnValue = function(pFuncParameterValue1->AsUnsignedInt());
            }
            else
            {
                typedef uint32 (*Func0Args)(void);

                Func0Args function = (Func0Args) pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt();

                gLogMessageRaw
                    << "Calling function " << inHex(pFuncAddressParameter->AsFlagParameter()->AsUnsignedInt())
                    << "()\n" << flush;

                returnValue = function();
            }
        }
        else
        {
            gLogMessageRaw
                << "ERROR - function address not specified!\n";
        }
    }
    else
    {
        // This should only happen if the command parameter is
        // changed, but support for the new function type is not
        // implemented.
        gLogMessageRaw 
            << "ERROR - unknown function type parameter!\n";
    }

    if (printReturnValue)
    {
        gLogMessageRaw
            << "Return value = " << inHex(returnValue) << " (" << returnValue << ")\n";
    }
}


