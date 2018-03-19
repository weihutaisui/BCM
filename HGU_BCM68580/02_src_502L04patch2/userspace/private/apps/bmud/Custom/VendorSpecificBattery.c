/***********************************************************************
 *
 *  Copyright (c) 2010  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2012:proprietary:standard

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/
#include "bcmos.h"
#include "VendorSpecificBattery.h"


#ifndef USING_VENDOR_SPECIFIC_PACK
static BOOL IsMyBatteryPack(TBatteryInfo *batInfo)
{
	char *packMfg = (char *)&batInfo->epromInfo.pageCache[0][3];
	char *packModel = (char *)&batInfo->epromInfo.pageCache[0][12];

	/* Example, replace with your own */
	return (!strcmp(packMfg, "MyPackName ") && !strcmp(packModel,"MyModel")) ? TRUE : FALSE;
}

BOOL VendorSpecificValidateBattery(TBatteryInfo *bi, TPackValidation *vs)
{
	if( IsMyBatteryPack(bi) )
	{
		/* Example, replace with your own */
		vs->parallelChains = 2;
		vs->seriesCells = 2;
		vs->generation = 2;
		vs->pagesMatch = 1;
		return TRUE;
	}
	return FALSE;
}

BOOL VendorSpecificDeterminePackCapacity(TBatteryInfo *bi, int *packCapacity)
{
	if( IsMyBatteryPack(bi) )
	{
		/* Example, replace with your own */
		*packCapacity = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL VendorSpecificCompletePackValidation(TBatteryInfo *bi, TPackParams *tpp, float *minChgReqd, int *chargePercentage)
{
	if( IsMyBatteryPack(bi) )
	{
		/* Example, replace with your own */
		HalDebugPrint(ZONE_TEST1,"Processing Vendor Specific pack\n");
		*minChgReqd = 0;
		tpp->doiLimitA	= 0;    // ma
		tpp->doiLimitB	= 0;    // ma
		tpp->doiLimitC	= 0;    // ma
		tpp->doiTimeA	= 0;    // ms
		tpp->doiTimeB	= 0;    // ms
		tpp->pqiChg	= (uint16)(*minChgReqd * 0.4); // ma
		tpp->fciChg	= (uint16)(*minChgReqd);       // ma
		tpp->tciTerm	= 0;    // ma - per battery spec
		tpp->pqvTerm	= 0;    // mv
		tpp->vdepleted	= 0;    // mv (dcvbon)
		tpp->mavV	= 0;    // mv - per battery spec
		tpp->coff_hi	= 0;    // degC
		tpp->coff_lo	= 0;    // degC
		tpp->doff_hi	= 0;    // degC
		tpp->doff_lo	= 0;    // degC
		tpp->thermB	= 0;    // degK
		tpp->thermR0	= 0;	// Ohms
		tpp->thermT0	= 0;	// degC
		*chargePercentage = 0;
		return TRUE;
	}
	return FALSE;
}

const TBatDischargeTable defaultVendorSpecificDischargeTable = 
{
	/* Example, replace with your own */
	2600.0, 2571.0, 713.121, 19.617, 2, 1, 500,
	{{8.401, 19.617}, // 100%	
	{8.361,  19.451}, // 99%	
	{8.340,  19.254}, // 98%	
	{8.320,  19.058}, // 97%	
	{8.300,  18.861}, // 96%	
	{8.281,  18.664}, // 95%	
	{8.262,  18.468}, // 94%	
	{8.244,  18.271}, // 93%	
	{8.225,  18.076}, // 92%	
	{8.207,  17.879}, // 91%	
	{8.189,  17.681}, // 90%	
	{8.171,  17.486}, // 89%	
	{8.154,  17.289}, // 88%	
	{8.136,  17.092}, // 87%	
	{8.119,  16.896}, // 86%	
	{8.102,  16.698}, // 85%	
	{8.086,  16.503}, // 84%	
	{8.069,  16.306}, // 83%	
	{8.053,  16.109}, // 82%	
	{8.037,  15.913}, // 81%	
	{8.021,  15.717}, // 80%	
	{8.006,  15.520}, // 79%	
	{7.991,  15.324}, // 78%	
	{7.976,  15.127}, // 77%	
	{7.961,  14.932}, // 76%	
	{7.946,  14.734}, // 75%	
	{7.932,  14.537}, // 74%	
	{7.918,  14.341}, // 73%	
	{7.905,  14.144}, // 72%	
	{7.891,  13.947}, // 71%	
	{7.877,  13.751}, // 70%	
	{7.864,  13.554}, // 69%	
	{7.851,  13.359}, // 68%	
	{7.838,  13.162}, // 67%	
	{7.825,  12.965}, // 66%	
	{7.812,  12.770}, // 65%	
	{7.799,  12.573}, // 64%	
	{7.785,  12.376}, // 63%	
	{7.772,  12.180}, // 62%	
	{7.759,  11.983}, // 61%	
	{7.745,  11.787}, // 60%	
	{7.731,  11.590}, // 59%	
	{7.717,  11.393}, // 58%	
	{7.703,  11.198}, // 57%	
	{7.690,  11.001}, // 56%	
	{7.678,  10.805}, // 55%	
	{7.667,  10.608}, // 54%	
	{7.657,  10.412}, // 53%	
	{7.647,  10.216}, // 52%	
	{7.639,  10.019}, // 51%	
	{7.630,   9.821}, // 50%	
	{7.623,   9.626}, // 49%	
	{7.615,   9.430}, // 48%	
	{7.608,   9.234}, // 47%	
	{7.601,   9.037}, // 46%	
	{7.595,   8.840}, // 45%	
	{7.589,   8.644}, // 44%	
	{7.582,   8.446}, // 43%	
	{7.577,   8.249}, // 42%	
	{7.571,   8.054}, // 41%	
	{7.566,   7.857}, // 40%	
	{7.561,   7.661}, // 39%	
	{7.555,   7.463}, // 38%	
	{7.551,   7.267}, // 37%	
	{7.546,   7.071}, // 36%	
	{7.542,   6.874}, // 35%	
	{7.538,   6.678}, // 34%	
	{7.534,   6.482}, // 33%	
	{7.530,   6.285}, // 32%	
	{7.526,   6.089}, // 31%	
	{7.523,   5.893}, // 30%	
	{7.519,   5.696}, // 29%	
	{7.515,   5.501}, // 28%	
	{7.512,   5.303}, // 27%	
	{7.508,   5.107}, // 26%	
	{7.503,   4.911}, // 25%	
	{7.499,   4.715}, // 24%	
	{7.493,   4.520}, // 23%	
	{7.487,   4.323}, // 22%	
	{7.480,   4.127}, // 21%	
	{7.473,   3.930}, // 20%	
	{7.465,   3.733}, // 19%	
	{7.456,   3.537}, // 18%	
	{7.444,   3.340}, // 17%	
	{7.429,   3.143}, // 16%	
	{7.412,   2.948}, // 15%	
	{7.394,   2.751}, // 14%	
	{7.374,   2.555}, // 13%	
	{7.351,   2.358}, // 12%	
	{7.339,   2.162}, // 11%	
	{7.332,   1.966}, // 10%	
	{7.325,   1.769}, // 9%	
	{7.315,   1.572}, // 8%	
	{7.303,   1.377}, // 7%	
	{7.288,   1.180}, // 6%	
	{7.269,   0.983}, // 5%	
	{7.237,   0.786}, // 4%	
	{7.155,   0.590}, // 3%	
	{7.006,   0.394}, // 2%	
	{6.773,   0.197}, // 1%	
	{6.081,   0.000}} // 0%	
};
#endif