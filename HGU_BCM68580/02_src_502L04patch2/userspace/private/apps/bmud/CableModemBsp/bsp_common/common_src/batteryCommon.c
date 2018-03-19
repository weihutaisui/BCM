//****************************************************************************
//
// Copyright (c) 2008-2013 Broadcom Corporation
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
//  Filename:       batteryCommon.c
//  Author:         Tom Johnson
//  Creation Date:  April 3, 2008
//
//****************************************************************************
//  Description:
//      Implements common battery code for all platforms
//
//****************************************************************************
#include "bcmtypes.h"
#include "bcmos.h"
#include "battery.h"
#include "MergedNonVolDefaults.h"
#include "batteryDischargeTables.h"		// this should ONLY be included HERE!!!!
#include <stdlib.h>

// The numerical constants here are from
// the thermistor data sheet and may be overridden in CustomerNonVolDefaults.h
// if a different thermistor is used from the one specified in the reference
// design.
#ifndef  THERMISTOR_Ro
    #define THERMISTOR_Ro 10000 // 10k ohms
#endif
#ifndef THERMISTOR_To
    #define THERMISTOR_To 298.15
#endif
#ifndef THERMISTOR_B
    #define THERMISTOR_B 3380.0
#endif
#ifndef THERMISTOR_LADDER_RESISTANCE
    #define THERMISTOR_LADDER_RESISTANCE 10000 // 10k ohms, typically same as Ro
#endif
#ifdef BCM3385
	#ifndef THERM_RUPPER
		#define THERM_RUPPER 12000
	#endif
	#ifndef THERM_RLOWER
		#define THERM_RLOWER 1750
	#endif
	#define THERM_VREF 1.6
#endif
float BltAdc2DegC( uint16 adcVal, uint16 limitingADC )
{
	static float result = 25.0;
	if( adcVal > limitingADC ) 
	{
		HalDebugPrint (ZONE_TEST1, "  adcVal > %d, capping.\n", limitingADC);
		adcVal = limitingADC; //brrrrrr
	}
	if( adcVal > 0 )
	{
		float R;
#ifndef BCM3385
		float pctFS;
		pctFS = (float)adcVal/(float)(limitingADC + 1);
		R = (THERMISTOR_LADDER_RESISTANCE * pctFS) / (1 - pctFS);
#else
		float adcV, vratio;
		adcV = THERM_VREF/8*(7*(float)adcVal/1024+1);
		vratio = adcV/THERM_VREF;
		R = vratio*THERM_RUPPER/(1-vratio) - THERM_RLOWER;
		HalDebugPrint(ZONE_TEST1, "BltAdc2DegC::3385 therm calculation:  adc = %d, adcV = %0.4f, vratio = %0.6f, R = %0.6f\n", adcVal, adcV, vratio, R);
#endif
		result = 1 / ((log(R / THERMISTOR_Ro) / THERMISTOR_B) + (1 / THERMISTOR_To)) - 273.15; 
	}
	else
		HalDebugPrint (ZONE_TEST1, "  adcVal = 0, returning nominal temp.\n");
	return result;
}

float PctToV( const TBatDischargeTable *table, float pct )
{
	if( !table ) 
	{
		HalDebugPrint (ZONE_TEST1, "PctToV: NULL table pointer\n");
		return 0;
	}
	else
	{
		const TBatDchgTableEntry *tbl = table->table;
		if( pct >= 100 ) return tbl[0].voltage;
		if( pct <= 0 ) return tbl[100].voltage;
		else
		{
			float lower = tbl[100-(int)pct].voltage;
			float upper = tbl[100-((int)pct+1)].voltage;
			float result = lower + (upper-lower)*(pct - (int)pct);
			return result * (float)bspNumSeriesCellsSupported()/(float)table->seriesCells;
		}
	}
}

float  VToPct( const TBatDischargeTable *table, float volts )
{
	if( !table )
	{
		HalDebugPrint (ZONE_TEST1, "VToPct: NULL table pointer\n");
		return 0;
	}
	else
	{
		const TBatDchgTableEntry *tbl = table->table;
		float result = 0.0;
		volts = volts * (float)table->seriesCells/(float)bspNumSeriesCellsSupported();
		//HalDebugPrint (ZONE_TEST1, "VToPct: seriesCells = %d, bspSupports = %d, v = %f\n", table->seriesCells, bspNumSeriesCellsSupported(), volts);
		if( volts >= tbl[0].voltage ) 
			result = 100.0;
		else if( volts <= tbl[100].voltage ) 
			result = 0.0;
		else
		{
			// find the lower bounding entry
			int ix;
			for( ix = 1; ix <= 100; ix++ )
				if( tbl[ix].voltage <= volts ) break;
			if( tbl[ix].voltage == volts )
				result = 100-ix;
			else
				result = (100-ix) + (volts - tbl[ix].voltage)/(tbl[ix-1].voltage - tbl[ix].voltage);
		}
		//HalDebugPrint (ZONE_TEST1, "VToPct: returning %f pct\n", result);
		return result;
	}
}

float  PctToWh( const TBatDischargeTable *table, float pct )
{
	if( !table )
	{
		HalDebugPrint (ZONE_TEST1, "PctToWh: NULL table pointer\n");
		return 0;
	}
	else
	{
		const TBatDchgTableEntry *tbl = table->table;
		float answer = 0.0;
		//NDumpDischargeTable(table,5);
		if( pct >= 100 ) 
			answer = tbl[0].whRemaining;
		else if( pct <= 0 ) 
			answer = tbl[100].whRemaining;
		else
		{
			int lidx = 100-(int)pct;
			int uidx = 100-((int)pct+1);
			float lower = tbl[lidx].whRemaining;
			float upper = tbl[uidx].whRemaining;
			//HalDebugPrint (ZONE_TEST1, "PctToWh: upper = %f (%d), lower = %f (%d), answer = %f\n", upper, uidx, lower, lidx, answer);
			answer = lower + (upper-lower)*(pct - (int)pct);
		}
		//HalDebugPrint (ZONE_TEST1, "PctToWh: returning %f wH\n", answer);
		return answer;
	}
}

float  VToWh( const TBatDischargeTable *table, float volts )
{
	float pct = VToPct(table,volts);
	//HalDebugPrint (ZONE_TEST1, "VToWh: calling PctToWh with %f\%\n", pct);
	return PctToWh(table,pct);
}

void CopyDischargeTable( TBatDischargeTable *toTable, const TBatDischargeTable *fromTable)
{
	if (toTable && fromTable) {
		memcpy(toTable, fromTable, sizeof(TBatDischargeTable));
	}
}

TBatDischargeTable *CreateDischargeTable(const TBatDischargeTable *fromTable)
{
	TBatDischargeTable *result = malloc(sizeof(TBatDischargeTable));
	if (result) {
		memset(result, 0, sizeof(TBatDischargeTable));
		CopyDischargeTable(result, fromTable);
	}
	return result;
}

void ScaleDischargeTable(TBatDischargeTable *inTable, int epromCapacity)
{
	if (epromCapacity != inTable->designCapacity)
	{
		int ix;
		float wHCorrection = (float)epromCapacity / (float)inTable->designCapacity;
		HalDebugPrint(ZONE_TEST1,"ScaleDischargeTable: Readjusting watt hours using %0.4f\n", wHCorrection);
		TBatDchgTableEntry *dte = inTable->table;
		inTable->maxWh *= wHCorrection;
		inTable->totalMinutes *= wHCorrection;
		HalDebugPrint(ZONE_TEST1,"ScaleDischargeTable: PDC=%d, TDC=%d\n", epromCapacity,(int)inTable->designCapacity);
		for( ix = 0; ix < 101; ix++)
		{
			dte[ix].whRemaining *= wHCorrection;
		}
	}
	HalDebugPrint(ZONE_TEST1,"ScaleDischargeTable: Setting PDC/PAC=%d\n", epromCapacity);
	inTable->designCapacity = (float)epromCapacity;
	inTable->actualCapacity = (float)epromCapacity;
}

void DumpCompressedDischargeTable( const TNonVolBatteryInfo *nvi )
{
#if !(BCM_REDUCED_IMAGE_SIZE)
	int ix;
	printf("maxWh           = %f\n", nvi->maxWh);
	printf("Design capacity = %d\n", nvi->DesignCapacity);
	printf("Est Life Rem    = %d\n", nvi->EstLifeRemaining);
	printf("total seconds   = %d\n", (int)nvi->totalSeconds);
	for( ix = 0; ix <= 100; ix++) {
		printf("  %4d %8d\n", 100-ix, nvi->dchgVoltages[ix]);
	}
#endif
}

void CompressDischargeTable(const TBatDischargeTable *fromTable, TNonVolBatteryInfo *nvi)
{
	int ix;
	HalDebugPrint (ZONE_TEST1, "COMPRESSING DISCHARGE TABLE...\n");
	nvi->maxWh 		  	  = fromTable->maxWh;
	nvi->DesignCapacity   = (uint16)(fromTable->designCapacity);
	nvi->EstLifeRemaining = (uint8)(fromTable->actualCapacity/fromTable->designCapacity*100.0);
	nvi->seriesResistance = fromTable->batZ;
	nvi->totalSeconds     = (uint32)(fromTable->totalMinutes * 60.0);

	for (ix = 0; ix <= 100; ix++) {
		nvi->dchgVoltages[ix] = (uint16)(fromTable->table[ix].voltage * 1000);
	}
	//DumpCompressedDischargeTable(nvi);
	//HalDebugPrint (ZONE_TEST1, "DONE COMPRESSING DISCHARGE TABLE...\n");
}

void DecompressDischargeTable(const TNonVolBatteryInfo *nvi, TBatDischargeTable *toTable)
{
	float wattsPerPct;
	int ix;
	if (!toTable) return;

	HalDebugPrint (ZONE_TEST1, "DECOMPRESSING DISCHARGE TABLE...\n");
	//DumpCompressedDischargeTable(nvi);
	toTable->maxWh          = nvi->maxWh;
	toTable->designCapacity = (float)nvi->DesignCapacity;
	toTable->actualCapacity = (float)nvi->DesignCapacity * (float)nvi->EstLifeRemaining/100.0;
	toTable->totalMinutes   = (float)nvi->totalSeconds / 60.0;
	toTable->batZ   		= nvi->seriesResistance;

	wattsPerPct = toTable->maxWh/100.0;
	for (ix = 0; ix <= 100; ix++) {
		toTable->table[ix].voltage 	   = (float)(nvi->dchgVoltages[ix])/1000.0;
		toTable->table[ix].whRemaining = (100-ix) * wattsPerPct;
	}
	//HalDebugPrint (ZONE_TEST1, "DONE DECOMPRESSING DISCHARGE TABLE...\n");
	//NDumpDischargeTable(toTable, 5);
}

void DumpDischargeTable( const TBatDischargeTable *table )
{
#if !(BCM_REDUCED_IMAGE_SIZE)
	int ix;
	printf("Design capacity = %f\n", table->designCapacity);
	printf("Actual capacity = %f\n", table->actualCapacity);
	printf("Total minutes   = %f\n", table->totalMinutes);
	printf("Resistance      = %d\n", table->batZ);
	printf("Max Watt-hours  = %f\n", table->maxWh);
	printf("Series cells    = %d\n", (int)table->seriesCells);
	printf("Parallel chains = %d\n", (int)table->parallelChains);
	for( ix = 0; ix <= 100; ix++) {
		printf("  %4d %10.3f %10.3f\n", 100-ix, table->table[ix].voltage, table->table[ix].whRemaining);
	}
#endif
}

void NDumpDischargeTable( const TBatDischargeTable *table, int entries )
{
#if !(BCM_REDUCED_IMAGE_SIZE)
	int ix;
	printf("Design capacity = %f\n", table->designCapacity);
	printf("Actual capacity = %f\n", table->actualCapacity);
	printf("Total minutes   = %f\n", table->totalMinutes);
	printf("Max Watt-hours  = %f\n", table->maxWh);
	printf("Series cells    = %d\n", (int)table->seriesCells);
	printf("Parallel chains = %d\n", (int)table->parallelChains);
	for( ix = 0; ix < entries; ix++) {
		printf("  %4d %10.3f %10.3f\n", 100-ix, table->table[ix].voltage, table->table[ix].whRemaining);
	}
#endif
}

// returns a better default discharge table than the current discharge table if one exists
const TBatDischargeTable *FindBestDefaultDischargeTable(int capacity, int seriesCells, int parallelChains)
{
	// capacity is the EPROM-stated design capacity, vs contains particulars about #series and #parallel cells
	// looks through all discharge tables in defaultDischargeTables[] looking for the best match
	const TBatDischargeTable * result = (const TBatDischargeTable *)0;
	int maxTables = sizeof(defaultDischargeTables)/sizeof(TBatDischargeTable *);
	struct {
		const TBatDischargeTable *table;
		int 					 isCandidate;
	} candidates[maxTables];
	int candidatesFound = 0, bestCandidate = -1;
	int ix;
	// intialize candidates array
//	HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: Max tables = %d, sizeof candidates = %d\n", maxTables, sizeof(candidates) );
	memset(candidates, 0, sizeof(candidates));
	// next, look for default table[s] that match the number of series cells
	for(ix = 0; ix < maxTables; ix++) {
		if (defaultDischargeTables[ix]->seriesCells == seriesCells) {
			HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: table %d is candidate %d (same # series cells)\n", ix, candidatesFound );
			candidates[candidatesFound].table = defaultDischargeTables[ix];
			candidates[candidatesFound].isCandidate = 1;
			candidatesFound++;
		}
		else
		{
			HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: table %d is not a candidate (wrong # series cells - %d vs %d)\n", 
						   ix, defaultDischargeTables[ix]->seriesCells, seriesCells);
		}
	}
	// assuming one or more candidates were found, keep only the ones with
	// the same number of parallel chains
	if (candidatesFound > 0) {
		float bestDelta = (float)(-(INT_MAX));
		int initialCandidates = candidatesFound;
		for (ix = 0; ix < initialCandidates; ix++) {
//			HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: Checking entry %d, remaining candidates = %d\n", ix, candidatesFound);
			if (candidates[ix].table->parallelChains != parallelChains) {
				HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: Candidate %d is no longer a candidate (different # parallel chains- %d vs %d)\n", 
							   ix, candidates[ix].table->parallelChains, parallelChains);
				candidates[ix].isCandidate = -1;	// "-1" indicates failed parallel chains comparison
				candidatesFound--;
			}
		}
//		HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: Done removing bad parallel chain candidates\n"); 
		// if no candidates had the same # parallel chains, re-allow all previously found candiates
		if( candidatesFound <= 0 ) {
			candidatesFound = 0;
			HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: No candidates had same # parallel chains, resetting candidates.\n");
			for (ix = 0; ix < initialCandidates; ix++ ) {
				if ( candidates[ix].isCandidate == -1 )	{
					candidates[ix].isCandidate = 1;
					candidatesFound++;
				}
			}
		}
//		HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: Done resetting candidates (if necessary)\n"); 
		// assuming any candidates remain, pick the candidate that most closely
		// matches the design capacity of the target battery
		if ( candidatesFound > 0 ) {
//			HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: Picking best candidate from remaining candidates...\n"); 
			// find the first candidate and set bestDelta based on that candidate
			for ( ix = 0; ix < initialCandidates; ix++) {
				if ( candidates[ix].isCandidate == 1 ) {
					bestDelta = fabs(capacity - candidates[ix].table->designCapacity);
					bestCandidate = ix;
					HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: Best delta initialized to candidate %d (%.1f mAh)\n", ix, bestDelta);
					break;
				}
			}
//			HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: done finding initial best candidate...\n"); 
			// continue across all remaining candidates looking for a better match
			for( ++ix; ix < initialCandidates; ix++ ) {
				if ( (candidates[ix].isCandidate == 1) && (fabs(capacity - candidates[ix].table->designCapacity) < bestDelta) ) {
					bestDelta = fabs(capacity - candidates[ix].table->designCapacity);
					bestCandidate = ix;
					HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: New best candidate: %d (%.1f mAh)\n", ix, bestDelta);
				}
			}
//			HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: done finding best candidate...\n"); 
		}
	}
//	HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: final check of best candidate...\n"); 
	if (bestCandidate >= 0) 
	{
		// a better replacement has been found
		HalDebugPrint(ZONE_TEST1,"FindBestDefaultDischargeTable: Candidate %d is the best match\n", bestCandidate);
		result = candidates[bestCandidate].table;
	}
	else
	{
		HalDebugPrint (ZONE_TEST1, "FindBestDefaultDischargeTable: No potential replacement candidates located\n");
	}
	return result;
}

