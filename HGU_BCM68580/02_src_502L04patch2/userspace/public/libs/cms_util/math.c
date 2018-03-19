/*************************************************************************
 *
<:copyright-BRCM:2015:DUAL/GPL:standard

   Copyright (c) 2015 Broadcom 
   All Rights Reserved

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

#include <stdint.h>
#include <math.h>
#include "cms_math.h"
#include "cms_log.h"


/*****************************************************************************
*  FUNCTION:  pointOneMicroWattsTodB
*  PURPOSE:   Convert power in 0.1uw unit to dBmW or dBuW in 0.001 dB unit.
*  PARAMETERS:
*      puw - power in 0.1uw unit.
*      isdBmW - whether it is represented as dBmW (1) or dBuW (0).
*  RETURNS:
*      dB * 1000.
*  NOTES:
*      1uw-> -30dBm -> OMCI representation -15000.
*      1uw->   0dBu -> OMCI representation 0.
*****************************************************************************/
SINT32 pointOneMicroWattsTodB(UINT32 puw, UBOOL8 isdBmW)
{
    float p = puw;
    float dbm;
    SINT32 rv;

    if (puw == 0)
    {
        return INT32_MIN;
    }

    if (isdBmW == TRUE)
    {
        p = p / 10000; /* Convert 0.1 uW to mW. */
    }
    else
    {
        p = p / 10;    /* Convert 0.1 uW to uW. */
    }

    dbm = 10 * (logf(p) / logf(10));
    dbm *= 1000;

    rv = (SINT32)dbm;

    return rv;
}

/*****************************************************************************
*  FUNCTION:  convertPointOneMicroWattsToOmcidB
*  PURPOSE:   Convert power in 0.1uw unit to the OMCI representation.
*  PARAMETERS:
*      dir - string indicating which power this is (for debugging).
*      puw - Power in 0.1 micro-watts.
*      min - min allowed value by OMCI data model spec.
*      max - max allowed value by OMCI data model spec.
*      inc - reporting increments (steps) required by OMCI data model spec.
*      isdBmW - whether it is represented as dBmW (1) or dBuW (0).
*  RETURNS:
*      SINT32.
*  NOTES:
*      None.
*****************************************************************************/
SINT32 convertPointOneMicroWattsToOmcidB(const char *dir, UINT32 puw,
  SINT32 min, SINT32 max, SINT32 inc, UBOOL8 isdBmW)
{
    SINT32 rv;

    if (inc == 0)
    {
        cmsLog_error("Invalid inc value %d", inc);
        return min;
    }

    rv = pointOneMicroWattsTodB(puw, isdBmW);
    if (rv == INT32_MIN)
    {
        rv = min;
        return rv;
    }

    rv = rv / inc;
    if (rv < min)
    {
        cmsLog_notice("(%s) %d uW => %d below min, limit to %d",
          dir, puw, rv, min);
        rv = min;
    }
    else if (rv > max)
    {
        cmsLog_notice("(%s) %d uW => %d above max, limit to %d",
          dir, puw, rv, max);
        rv = max;
    }
    else
    {
        cmsLog_debug("(%s) %d uW => %d", dir, puw, rv);
    }

    return rv;
}
