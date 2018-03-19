/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

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
#include "periodicstat.h"
#include "parameter.h"
#include "sampleset.h"

/* reset function will be executed while the SampleInterval is changed */
void resetSampleSet(SampleSetInfo *currSmpInfo, SampleSetInfo *newSmpInfo)
{
   SampleParameterInfo *param;

   if ( currSmpInfo == NULL || newSmpInfo == NULL)
      return;

   currSmpInfo->sampleInterval = newSmpInfo->sampleInterval;
   currSmpInfo->reportSamples = newSmpInfo->reportSamples;
   currSmpInfo->sampleCount = 0;
#if SUPPORT_PERIODICSTATSADV 
   currSmpInfo->fetchSamples = newSmpInfo->fetchSamples;
#endif

   if ( currSmpInfo->sampleSeconds != NULL )
      free(currSmpInfo->sampleSeconds);

   currSmpInfo->sampleSeconds = (UINT32 *)malloc(sizeof(UINT32)*newSmpInfo->reportSamples);
   memset(currSmpInfo->sampleSeconds, 0, sizeof(UINT32)*newSmpInfo->reportSamples);

   for ( param = currSmpInfo->parameters ; param != NULL ; param = param->next )
   {
      resetParameter(param, currSmpInfo);
   }
} 

/* update function will be executed while the ReportSamples is changed */
void updateSampleSet(SampleSetInfo *currSmpInfo, SampleSetInfo *newSmpInfo)
{
   int i, currReportSamples, newReportSamples;
   UINT32 *newSampleSeconds;
   SampleParameterInfo *param;

   if ( currSmpInfo == NULL || newSmpInfo == NULL)
      return;

   currReportSamples = currSmpInfo->reportSamples;
   newReportSamples = newSmpInfo->reportSamples;

   newSampleSeconds = (UINT32 *)malloc(sizeof(UINT32)*newReportSamples);
   
   if ( newSampleSeconds != NULL )
   {
      // keep collected data
      if ( currSmpInfo->sampleSeconds != NULL )
      {
         for ( i = 0 ; i < newReportSamples  && i < currReportSamples ; i++ )
            newSmpInfo->sampleSeconds[i] = currSmpInfo->sampleSeconds[i];
         free(currSmpInfo->sampleSeconds);
      }
      currSmpInfo->sampleSeconds = newSampleSeconds;
   }

   currSmpInfo->reportSamples = newSmpInfo->reportSamples;
   currSmpInfo->sampleCount = 0;
#if SUPPORT_PERIODICSTATSADV 
   currSmpInfo->fetchSamples = newSmpInfo->fetchSamples;
#endif

   for ( param = currSmpInfo->parameters ; param != NULL ; param = param->next )
   {
      updateParameter(param, NULL, newSmpInfo);
   }
}

void destroySampleSet(SampleSetInfo *info)
{
   if ( info != NULL )
   {
      while ( info->parameters )
      {
         SampleParameterInfo *parameter = info->parameters;
         info->parameters = parameter->next;
         destroyParameter(parameter);
      }

      if ( info->sampleSeconds != NULL )
      {
         free(info->sampleSeconds);
      }
      free(info);
   }
}

void insertParameterIntoSampleSet(SampleSetInfo *smpset, SampleParameterInfo *param)
{
   SampleParameterInfo *temp;
   int reportSamples;
   if ( smpset == NULL || param == NULL )
      return;

   for ( temp = smpset->parameters ; temp != NULL ; temp = temp->next )
   {
      if ( temp->id == param->id )
         break;
   }

   if ( temp == NULL )
   {
      reportSamples = smpset->reportSamples;
      if ( param->values == NULL )
      {
         param->values = (char **)malloc(sizeof(char *) * reportSamples);
         memset(param->values, 0, sizeof(char *) * reportSamples);
         param->valueCount = smpset->sampleCount;
      }
      if ( param->suspectData == NULL )
      {
         param->suspectData = (UINT32*)malloc(sizeof(UINT32) * reportSamples);
         memset(param->suspectData, 0, sizeof(UINT32) * reportSamples);
      }
      if ( param->sampleSeconds == NULL )
      {
         param->sampleSeconds = (UINT32*)malloc(sizeof(UINT32) * reportSamples);
         memset(param->sampleSeconds, 0, sizeof(UINT32) * reportSamples);
      }
      param->next = smpset->parameters;
      smpset->parameters = param;
   }
   else // duplicated, delete new one
   {
      destroyParameter(param);
   }
}

void deleteParameterFromSampleSet(SampleSetInfo *smpset, SampleParameterInfo *param)
{
   SampleParameterInfo *temp;
   if ( smpset == NULL || param == NULL )
      return;

   if ( param == smpset->parameters )
   {
      smpset->parameters = param->next;
   }
   else 
   {
      for ( temp = smpset->parameters ; temp != NULL ; temp = temp->next )
      {
         if ( temp->next == param )
         {
            temp->next = param->next;
         }
      }
   }

   destroyParameter(param);
}

void sampleOutput(SampleSetInfo *smpInfo)
{
   int i;
   char fileName[BUFLEN_64];
   SampleParameterInfo *temp;
   FILE *output;

   if ( smpInfo == NULL )
      return;

   sprintf(fileName, PERIODIC_STATS_SAMPLE_FILENAME_PREFIX"%d", smpInfo->id);
   output = fopen(fileName, "w");
   if ( output == NULL )
      return;

   /* ReportStartTime */
   fprintf(output, "ReportStartTime=%s\n", smpInfo->startTime);
   /* ReportEndTime */
   fprintf(output, "ReportEndTime=%s\n", smpInfo->endTime);
   /* SampleSeconds */
   fprintf(output, "SampleSeconds=");
   for ( i = 0 ; i < smpInfo->sampleCount ; i++ )
   {
      fprintf(output, "%u,", smpInfo->sampleSeconds[i]);
   }
   fprintf(output, "\n");
   fclose(output);

   for ( temp = smpInfo->parameters ; temp != NULL ; temp = temp->next )
   {
      parameterOutput(temp, smpInfo);
   }
}

