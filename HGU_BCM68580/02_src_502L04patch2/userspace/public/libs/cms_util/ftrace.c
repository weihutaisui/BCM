/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:DUAL/GPL:standard

Unless you and Broadcom execute a separate written software license
agreement governing use of this software, this software is licensed
to you under the terms of the GNU General Public License version 2
(the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
with the following added to such license:

   As a special exception, the copyright holders of this software give
   you permission to link this software with independent modules, and
   to copy and distribute the resulting executable under terms of your
   choice, provided that you also meet, for each linked independent
   module, the terms and conditions of the license of that module.
   An independent module is a module which is not derived from this
   software.  The special exception does not apply to any modifications
   of the software.

Not withstanding the above, under no circumstances may you combine
this software in any way with any other Broadcom software provided
under a license other than the GPL, without Broadcom's express prior
written consent.

:>
 *
 ************************************************************************/


#ifdef SUPPORT_FTRACE
#include "cms.h"
#include "cms_util.h"



void cmsFtr_enable(void)
{
   cmsFil_writeToProc(FTRACE_ON, "1");
}

void cmsFtr_disable(void)
{
   cmsFil_writeToProc(FTRACE_ON, "0");
}

void cmsFtr_insertMarker(const char *s)
{
   cmsFil_writeToProc(FTRACE_INSERT_MARKER, s);
}

void cmsFtr_setTraceOption(const char *s)
{
   cmsFil_writeToProc(FTRACE_TRACE_OPTIONS, s);
}

void cmsFtr_doFunctionTracing(void)
{
   cmsFil_writeToProc(FTRACE_CURRENT_TRACER, "function");
}

void cmsFtr_doFunctionGraphTracing(void)
{
   cmsFil_writeToProc(FTRACE_CURRENT_TRACER, "function_graph");
}

void cmsFtr_doNopTracing(void)
{
   cmsFil_writeToProc(FTRACE_CURRENT_TRACER, "nop");
}

#endif /* SUPPORT_FTRACE */
