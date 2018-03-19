/***********************************************************************
 *
 *  Copyright (c) 2012-2013  Broadcom Corporation
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

#include "cms.h"

void resetParameter(SampleParameterInfo *parameter, SampleSetInfo *smpInfo)
{ 
   int reportSamples;
   if ( parameter == NULL || smpInfo == NULL)
      return;

   if ( parameter->values )
   {
      int i;
      for ( i = 0 ; i < parameter->valueCount ; i++ )
      {
         SAFE_FREE(parameter->values[i]);
      }
      free(parameter->values);
      parameter->values = NULL;
   }

   reportSamples = smpInfo->reportSamples;

   SAFE_FREE(parameter->sampleSeconds);
   SAFE_FREE(parameter->suspectData);

   parameter->values = (char **)malloc(sizeof(char *) * reportSamples);
   parameter->sampleSeconds = (UINT32 *)malloc(sizeof(UINT32) * reportSamples);
   parameter->suspectData = (UINT32 *)malloc(sizeof(UINT32) * reportSamples);

   if ( CHECK_ALLOC_FAIL3(parameter->values, parameter->sampleSeconds, parameter->suspectData) )
   {
      SAFE_FREE(parameter->values);
      SAFE_FREE(parameter->sampleSeconds);
      SAFE_FREE(parameter->suspectData);
      return;
   }

   memset(parameter->values, 0, sizeof(char *) * reportSamples);
   memset(parameter->sampleSeconds, 0, sizeof(UINT32) * reportSamples);
   memset(parameter->suspectData, 0, sizeof(UINT32) * reportSamples);
   parameter->valueCount = smpInfo->sampleCount;
}

void updateParameter(SampleParameterInfo *currParam, SampleParameterInfo *newParam, SampleSetInfo *smpInfo)
{
   int reportSamples, valueCount, i;

   /* this update is based on the new reportSamples of SampleSet. */
   if ( newParam == NULL && smpInfo != NULL)
   {
      char **newValues;
      UINT32 *newSuspectData, *newSampleSeconds;

      reportSamples = smpInfo->reportSamples;
      valueCount = currParam->valueCount;

      /* allocate new buffer*/
      newSampleSeconds = (UINT32 *)malloc(sizeof(UINT32)*reportSamples);
      newSuspectData= (UINT32 *)malloc(sizeof(UINT32)*reportSamples);
      newValues = (char **)malloc(sizeof(char*)*reportSamples);
      if ( CHECK_ALLOC_FAIL3(newSampleSeconds, newSampleSeconds, newValues))
      {
         SAFE_FREE(newSampleSeconds);
         SAFE_FREE(newSuspectData);
         SAFE_FREE(newValues);
         return;
      }

      memset(newSampleSeconds, 0, sizeof(UINT32) * reportSamples);
      memset(newSuspectData, 0, sizeof(UINT32) * reportSamples);
      memset(newValues, 0, sizeof(char *) * reportSamples);
      
      // keep collected data
      for ( i = 0 ; i < reportSamples && i < valueCount; i++ )
      {
         if (currParam->sampleSeconds)
            newSampleSeconds[i] = currParam->sampleSeconds[i];
         if (currParam->suspectData)
            newSuspectData[i] = currParam->suspectData[i];
         if (currParam->values && currParam->values[i])
         {
            newValues[i] = strdup(currParam->values[i]);
            free(currParam->values[i]);
         }
      }
      // clean up rest of data in old values buffer
      if (currParam->values)
      {
         for ( ; i < valueCount ; i ++ )
         {
            SAFE_FREE(currParam->values[i]);
         }
      }

      SAFE_FREE(currParam->values);
      SAFE_FREE(currParam->sampleSeconds);
      SAFE_FREE(currParam->suspectData);

      currParam->values = newValues;
      currParam->sampleSeconds = newSampleSeconds;
      currParam->suspectData = newSuspectData;
   }
   else if ( newParam != NULL )
   {
      valueCount = currParam->valueCount;
      memcpy(currParam->name, newParam->name, BUFLEN_256); 
      resetParameter(currParam, smpInfo);
   }
}

void destroyParameter(SampleParameterInfo *parameter)
{
   if ( parameter == NULL )
      return;
   
   if ( parameter->values != NULL )
   {
      int i;
      for ( i = 0 ; i < parameter->valueCount ; i++ )
      {
         if ( parameter->values[i] )
            free(parameter->values[i]);
         parameter->values[i] = NULL;
      }
      free(parameter->values);
   }

   SAFE_FREE(parameter->sampleSeconds);
   SAFE_FREE(parameter->suspectData);

   free(parameter);
}

void parameterOutput(SampleParameterInfo *parameter, SampleSetInfo *smpset)
{
   char fileName[BUFLEN_64];
   FILE *output;
   int i, valueCount;

   if ( parameter == NULL || smpset == NULL )
      return;

   sprintf(fileName, PERIODIC_STATS_SAMPLE_FILENAME_PREFIX"%d_%d", smpset->id, parameter->id);
   output = fopen(fileName, "w");
   if ( output == NULL )
      return;

   valueCount = parameter->valueCount;
   fprintf(output, "#%s\n", parameter->name);

   /* Values */
   fprintf(output, "values=");
   for ( i = 0 ; i < valueCount ; i++ )
   {
      fprintf(output,"%s,", parameter->values[i]);
   }

   /* SuspectData */
   fprintf(output, "\nsuspectData=");
   for ( i = 0 ; i < valueCount ; i++ )
   {
      fprintf(output,"%u,", parameter->suspectData[i]);
   }
   
   /* SampleSeconds */
   fprintf(output, "\nsampleSeconds=");
   for ( i = 0 ; i < valueCount ; i++ )
   {
      fprintf(output, "%u,", parameter->sampleSeconds[i]);
   }
   fprintf(output, "\n");

   fclose(output);
}

