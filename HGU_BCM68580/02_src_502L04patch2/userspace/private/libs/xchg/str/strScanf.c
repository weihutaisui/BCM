/***************************************************************************
*    <:copyright-BRCM:2006:proprietary:standard
*    
*       Copyright (c) 2006 Broadcom 
*       All Rights Reserved
*    
*     This program is the proprietary software of Broadcom and/or its
*     licensors, and may only be used, duplicated, modified or distributed pursuant
*     to the terms and conditions of a separate, written license agreement executed
*     between you and Broadcom (an "Authorized License").  Except as set forth in
*     an Authorized License, Broadcom grants no license (express or implied), right
*     to use, or waiver of any kind with respect to the Software, and Broadcom
*     expressly reserves all rights in and to the Software and all intellectual
*     property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
*     NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
*     BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*    
*     Except as expressly set forth in the Authorized License,
*    
*     1. This program, including its structure, sequence and organization,
*        constitutes the valuable trade secrets of Broadcom, and you shall use
*        all reasonable efforts to protect the confidentiality thereof, and to
*        use this information only in connection with your use of Broadcom
*        integrated circuit products.
*    
*     2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*        AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*        WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
*        RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
*        ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
*        FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
*        COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
*        TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
*        PERFORMANCE OF THE SOFTWARE.
*    
*     3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
*        ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
*        INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
*        WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*        IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
*        OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
*        SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
*        SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
*        LIMITED REMEDY.
*    :>
****************************************************************************
*
*    Filename: strScanf.c
*
****************************************************************************
*    Description:
*
*      Provides safe versions of the scanf(...) family of functions. A bare
*      scanf("%s", buf) call does not limit the size of the input string and
*      could easily overrun the provided buffer. The functions in this
*      module add the %S conversion specifier that takes the size of the
*      provided buffer as an argument and insures the buffer does not get
*      overrun.
*
****************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>

/* NEW_FORMAT_SIZE calculates the worst case number of bytes needed for the
 * translated format string so that it will not overrun. The worst case is
 * a call such as strScanf("%S", buf, 10000). The format string would be
 * translated to "%9999s", which is three times bigger than the original
 * format string. This is why we limit the buffer size and how the buffer
 * size relates to the new format string size. */
#define NEW_FORMAT_SIZE (3*strlen(format)+1)
#define MAX_BUF_SIZE 10000
#define MAX_VAR_ARGS 10

/*
*****************************************************************************
** FUNCTION:   parseArgs
**
** PURPOSE: This private function does all the format string translation
**          and variable argument list fixing for the strScanf() family of
**          functions contained in this module. The %S type conversion is
**          changed to a %XXs where XX is the size of the buffer provided.
**
** PARAMETERS:
**    oldFormat: The format list passed to the strScanf() family function.
**    newFormat: The new format list to be passed to the scanf() family
**               function.
**    newFormatSize: The size of the newFormat buffer in bytes.
**    oldArgs: The variable argument list passed to the strScanf() family
**             function.
**    newArgs: The array of new arguments to be passed to the scanf() family
**             function.
**
** RETURNS: The number of variable arguments to be passed on to xscanf() or
**          -1 on failure.
**
** NOTE:
*****************************************************************************
*/
static int parseArgs(const char *oldFormat, char *newFormat, int newFormatSize,
                     va_list oldArgs, void *newArgs[MAX_VAR_ARGS])
{
   int i, j;
   const char *p;

   /* Create the new format string and argument list. */
   for(p = oldFormat, i = 0, j = 0;
       (*p != '\0') && (i < (newFormatSize-1)) && (j < MAX_VAR_ARGS);
       p++)
   {
      if(*p == '%')
      {
         /* Got a type conversion */
         switch(*(p+1))
         {
            case 'S':
            {
               /* Got the special %S type conversion */
               int bufSize;

               newArgs[j] = va_arg(oldArgs, void *);
               bufSize = va_arg(oldArgs, int);

               /* Make sure we can't overrun newFormat. */
               if(bufSize > MAX_BUF_SIZE)
               {
                  fprintf(stderr, "WARNING: strScanf() limiting buffer size.\n");
                  bufSize = MAX_BUF_SIZE;
               }

               j++;
               /* Leave room for ending '\0' */
               i += sprintf(&newFormat[i], "%%%ds", (bufSize-1));
               p++;
            }
            break;

            case 's':
            {
               /* Got the unwated %s type conversion */
               fprintf(stderr, "ERROR: strScanf() got illegal %%s conversion.\n");
               return -1;
            }
            break;

            case '%':
            case '*':
            {
               /* Not actually a type conversion or should be ignored */
               newFormat[i++] = *p++;
               newFormat[i++] = *p;
            }
            break;

            default:
            {
               /* Got some other type conversion */
               newFormat[i++] = *p++;
               newFormat[i++] = *p;
               newArgs[j++] = va_arg(oldArgs, void *);
            }
            break;
         }
      }
      else
      {
         /* Not a type conversion */
         newFormat[i++] = *p;
      }
   }

   /* Error checking */
   if(i > (newFormatSize-1))
   {
      fprintf(stderr, "ERROR: strScanf() generated format string is too big.\n");
      return -1;
   }
   else
   {
      newFormat[i] = '\0';
   }

   if(j > MAX_VAR_ARGS)
   {
      fprintf(stderr, "ERROR: strScanf() too many variable arguments.\n");
      return -1;
   }

   return j;
}

/*
*****************************************************************************
** FUNCTION:   strScanf
**
** PURPOSE:    This is a safe version of the stdio.h library function,
**             scanf(). The standard library version of scanf() is not safe
**             because the input line can always overrun the provided buffer
**             when using the %s type conversion unless certain precautions
**             are taken. Here we translate what would be unsafe scanf()
**             calls to safe scanf() calls.
**
** PARAMETERS: The arguments to this function are exactly the same as for
**             the standard scanf() function, but for an important exception.
**             The function introduces a new type conversion, %S. The %S type
**             conversion indicates that the next argument from the variable
**             length argument list is a pointer to a buffer in which to store
**             the input line and that the argument after that is the size of
**             the buffer in bytes. For example, a call to scanf() might be:
**                scanf("%s", buf);
**             The equivalent call to strScanf() would be:
**                strScanf( "%S", buf, sizeof(buf) );
**             An attempt to use the standard %s type conversion will result
**             in an error. Type conversions such as %*s or %100s are still
**             allowed.
**
** RETURNS:    The number of input items converted and assigned or EOF on
**             error.
**
** NOTE:       This function imposes a maximum number of arguments on the
**             variable argument list and the size of buffers must be less
**             than MAX_BUF_SIZE bytes.
*****************************************************************************
*/
int strScanf(const char *format, ...)
{
   char newFormat[NEW_FORMAT_SIZE];
   va_list args;
   void *newArgs[MAX_VAR_ARGS]; /* Maximum number of supported variable arguments */
   int numArgs;
   int status = EOF;

   va_start(args, format);
   if((numArgs = parseArgs(format, newFormat, NEW_FORMAT_SIZE,
                           args, newArgs)) == -1)
   {
      /* Argument parsing error. */
      va_end(args);
      return status;
   }
   else
   {
      va_end(args);
   }

   /* Call scanf with the new format string and argument list */
   switch(numArgs)
   {
      case 0:
         status = scanf(newFormat);
         break;
      case 1:
         status = scanf(newFormat, newArgs[0]);
         break;
      case 2:
         status = scanf(newFormat, newArgs[0], newArgs[1]);
         break;
      case 3:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2]);
         break;
      case 4:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2],
                                   newArgs[3]);
         break;
      case 5:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2],
                                   newArgs[3], newArgs[4]);
         break;
      case 6:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2],
                                   newArgs[3], newArgs[4], newArgs[5]);
         break;
      case 7:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2],
                                   newArgs[3], newArgs[4], newArgs[5],
                                   newArgs[6]);
         break;
      case 8:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2],
                                   newArgs[3], newArgs[4], newArgs[5],
                                   newArgs[6], newArgs[7]);
         break;
      case 9:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2],
                                   newArgs[3], newArgs[4], newArgs[5],
                                   newArgs[6], newArgs[7], newArgs[8]);
         break;
      case 10:
         status = scanf(newFormat, newArgs[0], newArgs[1], newArgs[2],
                                   newArgs[3], newArgs[4], newArgs[5],
                                   newArgs[6], newArgs[7], newArgs[8],
                                   newArgs[9]);
         break;
      default:
         fprintf(stderr, "ERROR: strScanf() MAX_VAR_ARGS is set wrong.\n");
         return status;
   }

   return status;
}

/*
*****************************************************************************
** FUNCTION:   strFScanf
**
** PURPOSE:    This is a safe version of the stdio.h library function,
**             fscanf(). The standard library version of fscanf() is not safe
**             because the input line can always overrun the provided buffer
**             when using the %s type conversion unless certain precautions
**             are taken. Here we translate what would be unsafe fscanf()
**             calls to safe fscanf() calls.
**
** PARAMETERS: The arguments to this function are exactly the same as for
**             the standard fscanf() function, but for an important exception.
**             The function introduces a new type conversion, %S. The %S type
**             conversion indicates that the next argument from the variable
**             length argument list is a pointer to a buffer in which to store
**             the input line and that the argument after that is the size of
**             the buffer in bytes. For example, a call to fscanf() might be:
**                fscanf(stream, "%s", buf);
**             The equivalent call to strFScanf() would be:
**                strFScanf( stream, "%S", buf, sizeof(buf) );
**             An attempt to use the standard %s type conversion will result
**             in an error. Type conversions such as %*s or %100s are still
**             allowed.
**
** RETURNS:    The number of input items converted and assigned or EOF on
**             error.
**
** NOTE:       This function imposes a maximum number of arguments on the
**             variable argument list and the size of buffers must be less
**             than MAX_BUF_SIZE bytes.
*****************************************************************************
*/
int strFScanf(FILE *stream, const char *format, ...)
{
   char newFormat[NEW_FORMAT_SIZE];
   va_list args;
   void *newArgs[MAX_VAR_ARGS]; /* Maximum number of supported variable arguments */
   int numArgs;
   int status = EOF;

   va_start(args, format);
   if((numArgs = parseArgs(format, newFormat, NEW_FORMAT_SIZE,
                           args, newArgs)) == -1)
   {
      /* Argument parsing error. */
      va_end(args);
      return status;
   }
   else
   {
      va_end(args);
   }

   /* Call scanf with the new format string and argument list */
   switch(numArgs)
   {
      case 0:
         status = fscanf(stream, newFormat);
         break;
      case 1:
         status = fscanf(stream, newFormat, newArgs[0]);
         break;
      case 2:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1]);
         break;
      case 3:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2]);
         break;
      case 4:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                            newArgs[3]);
         break;
      case 5:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                            newArgs[3], newArgs[4]);
         break;
      case 6:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                            newArgs[3], newArgs[4], newArgs[5]);
         break;
      case 7:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                            newArgs[3], newArgs[4], newArgs[5],
                                            newArgs[6]);
         break;
      case 8:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                            newArgs[3], newArgs[4], newArgs[5],
                                            newArgs[6], newArgs[7]);
         break;
      case 9:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                            newArgs[3], newArgs[4], newArgs[5],
                                            newArgs[6], newArgs[7], newArgs[8]);
         break;
      case 10:
         status = fscanf(stream, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                            newArgs[3], newArgs[4], newArgs[5],
                                            newArgs[6], newArgs[7], newArgs[8],
                                            newArgs[9]);
         break;
      default:
         fprintf(stderr, "ERROR: strScanf() MAX_VAR_ARGS is set wrong.\n");
         return status;
   }

   return status;
}

/*
*****************************************************************************
** FUNCTION:   strSScanf
**
** PURPOSE:    This is a safe version of the stdio.h library function,
**             sscanf(). The standard library version of sscanf() is not safe
**             because the input line can always overrun the provided buffer
**             when using the %s type conversion unless certain precautions
**             are taken. Here we translate what would be unsafe sscanf()
**             calls to safe sscanf() calls.
**
** PARAMETERS: The arguments to this function are exactly the same as for
**             the standard sscanf() function, but for an important exception.
**             The function introduces a new type conversion, %S. The %S type
**             conversion indicates that the next argument from the variable
**             length argument list is a pointer to a buffer in which to store
**             the input line and that the argument after that is the size of
**             the buffer in bytes. For example, a call to sscanf() might be:
**                sscanf(s, "%s", buf);
**             The equivalent call to strSScanf() would be:
**                strSScanf( s, "%S", buf, sizeof(buf) );
**             An attempt to use the standard %s type conversion will result
**             in an error. Type conversions such as %*s or %100s are still
**             allowed.
**
** RETURNS:    The number of input items converted and assigned or EOF on
**             error.
**
** NOTE:       This function imposes a maximum number of arguments on the
**             variable argument list and the size of buffers must be less
**             than MAX_BUF_SIZE bytes.
*****************************************************************************
*/
int strSScanf(char *s, const char *format, ...)
{
   char newFormat[NEW_FORMAT_SIZE];
   va_list args;
   void *newArgs[MAX_VAR_ARGS]; /* Maximum number of supported variable arguments */
   int numArgs;
   int status = EOF;

   va_start(args, format);
   if((numArgs = parseArgs(format, newFormat, NEW_FORMAT_SIZE,
                           args, newArgs)) == -1)
   {
      /* Argument parsing error. */
      va_end(args);
      return status;
   }
   else
   {
      va_end(args);
   }

   /* Call scanf with the new format string and argument list */
   switch(numArgs)
   {
      case 0:
         status = sscanf(s, newFormat);
         break;
      case 1:
         status = sscanf(s, newFormat, newArgs[0]);
         break;
      case 2:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1]);
         break;
      case 3:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2]);
         break;
      case 4:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                       newArgs[3]);
         break;
      case 5:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                       newArgs[3], newArgs[4]);
         break;
      case 6:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                       newArgs[3], newArgs[4], newArgs[5]);
         break;
      case 7:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                       newArgs[3], newArgs[4], newArgs[5],
                                       newArgs[6]);
         break;
      case 8:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                       newArgs[3], newArgs[4], newArgs[5],
                                       newArgs[6], newArgs[7]);
         break;
      case 9:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                       newArgs[3], newArgs[4], newArgs[5],
                                       newArgs[6], newArgs[7], newArgs[8]);
         break;
      case 10:
         status = sscanf(s, newFormat, newArgs[0], newArgs[1], newArgs[2],
                                       newArgs[3], newArgs[4], newArgs[5],
                                       newArgs[6], newArgs[7], newArgs[8],
                                       newArgs[9]);
         break;
      default:
         fprintf(stderr, "ERROR: strScanf() MAX_VAR_ARGS is set wrong.\n");
         return status;
   }

   return status;
}
