/******************************************************************************
 *
 * Copyright (c) 2011   Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2011:proprietary:standard
 * 
 *  This program is the proprietary software of Broadcom and/or its
 *  licensors, and may only be used, duplicated, modified or distributed pursuant
 *  to the terms and conditions of a separate, written license agreement executed
 *  between you and Broadcom (an "Authorized License").  Except as set forth in
 *  an Authorized License, Broadcom grants no license (express or implied), right
 *  to use, or waiver of any kind with respect to the Software, and Broadcom
 *  expressly reserves all rights in and to the Software and all intellectual
 *  property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 *  NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 *  BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 * 
 *  Except as expressly set forth in the Authorized License,
 * 
 *  1. This program, including its structure, sequence and organization,
 *     constitutes the valuable trade secrets of Broadcom, and you shall use
 *     all reasonable efforts to protect the confidentiality thereof, and to
 *     use this information only in connection with your use of Broadcom
 *     integrated circuit products.
 * 
 *  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *     RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *     ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *     FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *     COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *     TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *     PERFORMANCE OF THE SOFTWARE.
 * 
 *  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *     ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *     INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *     WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *     IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *     OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *     SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *     SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *     LIMITED REMEDY.
 * :>
 *
 ************************************************************************/
/***************************************************************************
 * File Name  : mocap.c
 *
 * Description: Linux command line utility that controls the Broadcom
 *              MoCA driver. It does the following:
 *              - starts and stops the MoCA subsystem.
 *              - configures the MoCA subsystem.
 *              - retrieves status of the MoCA subsystem.
 *              - retrieves statistics of the MoCA subsystem.
 ***************************************************************************/

/** Includes. **/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mocalib.h"
#include "mocalib-cli.h"
#include "mocaint.h"

/** Defines. **/
#define TRUE  (1==1)
#define FALSE (!TRUE)

#define MOCAP_VERSION                  "0.0.1"

#if defined(STANDALONE)
#include "cli.h"
#include "printf.h"

static char g_szPgmName [80] = {0} ;
#else
static char g_szPgmName [8] = {0} ;
#endif

static int g_NvModeEnabled = FALSE;
static void * g_mocaHandle = NULL;

struct moca_ifname_map
{
   char  *input;
   char  *ifname;
};


static int HelpHandler( void * handle, int argc, char **argv );

static struct moca_ifname_map g_mocaIfNames[MAX_IFNAME_INPUTS] =
{
   {"0",      "moca0"},
   {"moca0",  "moca0"},
   {"lan",    "moca1"},
   {"1",      "moca1"},
   {"moca1",  "moca1"},
   {"10",     "moca1"},
   {"moca10", "moca1"},
   {"wan",    "moca0"},
   {NULL,    NULL}, /* Keep at end of array for easy searching */
};

static char * getMoCAIfName( char * argv )
{
#if defined(__EMU_HOST_20__)
   return(argv);
#else
   uint32_t i;
   char * pRet = NULL;

   /* Search the g_mocaIfNames array for a match */
   for ( i = 0; g_mocaIfNames[i].input != NULL; i++ )
   {
      if (strcmp(argv, g_mocaIfNames[i].input) == 0)
      {
         pRet = g_mocaIfNames[i].ifname;
         break;
      }
   }

   return (pRet);
#endif
};


/** Parses a moca cli argument type.
 *
 * This function parses "moca" CLI commands returns the argument type of
 * the supplied string.
 *
 * @param pszArg (IN) String pointer to the command line arguments.
 * @param pCmds (IN) Pointer to (global) array of moca commands.
 * @param ppszOptions (IN) Pointer to string array of options.
 * @return One of: ARG_TYPE_COMMAND, ARG_TYPE_OPTION, ARG_TYPE_PARAMETER.
 */
int mocacli_get_arg_type( char *pszArg, PCOMMAND_INFO pCmds, int nCmds, char **ppszOptions )
{
   int nArgType = ARG_TYPE_PARAMETER;
  
   /* See if the argument is a option. */
   if( ppszOptions )
   {
      do
      {
         if( !strcmp( pszArg, *ppszOptions) )
         {
            nArgType = ARG_TYPE_OPTION;
            break;
         }
      } while( *++ppszOptions );
      if ( (nArgType != ARG_TYPE_OPTION) && ((strstr(pszArg, "--")) != NULL) )
         printf("WARNING: Invalid option %s \n", pszArg);
   }

   /* Next, see if the argument is an command. */
   if( nArgType == ARG_TYPE_PARAMETER )
   {
      while( nCmds )
      {
         if( !strcmp( pszArg, pCmds->szCmdName ) )
         {
            nArgType = ARG_TYPE_COMMAND;
            break;
         }

         pCmds++;
         nCmds--;
      }
   }

   /* Otherwise, assume that it is a parameter. */
   return( nArgType );
} /* GetArgType */


/** Parses a moca cli command string.
 *
 * This function parses "moca" CLI string and returns the command that 
 * matches from the pCmds array.
 *
 * @param pszArg (IN) String pointer to the command line arguments.
 * @param pCmds (IN) Pointer to (global) array of moca commands.
 * @return Pointer to PCOMMAND_INFO entry from pCmds matching supplied 
 * string.
 */
PCOMMAND_INFO mocacli_get_command( char *pszArg, PCOMMAND_INFO pCmds, int nCmds )
{
   PCOMMAND_INFO pCmd = NULL;

   while( nCmds )
   {
      if( !strcmp( pszArg, pCmds->szCmdName ) )
      {
         pCmd = pCmds;
         break;
      }

      pCmds++;
      nCmds--;
   }

   return( pCmd );
} /* GetCommand */


/** Processes a moca cli command.
 *
 * Gets the options and option paramters for a command and
 * calls the command handler function to process the command.
 *
 * @param pszPgmName (IN) string pointer to applciation name used for error 
 * prints
 * @param pCmd (IN) Pointer to the command info.
 * @param argc (IN) Number of command line arguments
 * @param argv (IN) String array of command line arguments
 * @param pCmds (IN) Pointer to global array of CLI commands
 * @param pnArgNext (OUT) Argument count of next argument following this 
 *    command's completion
 * @return number of parameters consumed, negative - error
 */
int mocacli_process_command( 
   char * pszPgmName,
   PCOMMAND_INFO pCmd, 
   int argc, 
   char **argv,
   PCOMMAND_INFO pCmds) 
{
   int nRet = 0; 

   nRet = (*pCmd->pfnCmdHandler) (g_mocaHandle, argc, argv);

   return( nRet );
} /* ProcessCommand */



/***************************************************************************
 * Function Name: main
 * Description  : Main program function.
 * Returns      : 0 - success, non-0 - error.
 ***************************************************************************/
#if defined (STANDALONE)
int mocap_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
   int nExitCode = 0;
   PCOMMAND_INFO pCmd;
   int i;
   char *pMoCAIfName = NULL;
   COMMAND_INFO * cmds;
   int ncmds;
#if defined(WIN32)
   char *c;
#endif


#if defined(WIN32)
   // strip the path from the program name
   while (c=strchr(*argv, '\\')) *argv = c+1;
#endif

#if defined(STANDALONE)
   g_NvModeEnabled = CLI_Initialized(); // Don't write startup script commands to flash
   gCheckinit = 0;
#endif

   mocacli_register_help_handler(&HelpHandler);
   cmds = mocacli_get_cmds(&ncmds);

   
   /* Save the name that started this program into a global variable. */
   strncpy( g_szPgmName, *argv, sizeof(g_szPgmName) - 1);

   if( argc == 1 )
   {
      g_mocaHandle = moca_open( NULL );

      HelpHandler( g_mocaHandle, 0, NULL );

      if (g_mocaHandle)
         moca_close( g_mocaHandle );
      
      g_mocaHandle = NULL;

      return(0);
   }
   argc--, argv++;

   /* the first parameter may be a moca interface number. If not present,
    * assume interface 0 */
   if ( argc )
   {
        for (i = 0; (i < 2) && argc; i++) // two possible pre-parameters, -p|-x and ifname
        {
          if (strcmp (*argv, "-p") == 0)
          {
              g_NvModeEnabled = TRUE;
              argc--;
              argv++;
          }
          else if (strcmp (*argv, "-x") == 0)
          {
              g_NvModeEnabled = FALSE;
              argc--;
              argv++;
          }
          else if (pMoCAIfName == NULL)
          {
              pMoCAIfName = getMoCAIfName( *argv );
              if (pMoCAIfName != NULL)
                  argc--, argv++;
              else
                  pMoCAIfName = NULL;

              g_mocaHandle = moca_open( pMoCAIfName );

              if (pMoCAIfName == NULL)
                break;
          }
       }
   }

   // The CLI commands can accept a NULL handle, so run them even if we can't 
   // connect to mocad in case the user is only interested in the help
   // output.
   while( (argc>0) && (nExitCode >= 0) )
   {
      if( mocacli_get_arg_type( *argv, cmds, ncmds, NULL ) == ARG_TYPE_COMMAND )
      {
         pCmd = mocacli_get_command( *argv, cmds, ncmds );
         nExitCode = mocacli_process_command(g_szPgmName, pCmd, --argc, ++argv, cmds);
         if ( nExitCode > 0)
         {
            argv += nExitCode;
            argc -= nExitCode;
         }
      }
      else 
      {
         nExitCode = -2;
         fprintf( stderr, "%s: invalid command (%s)\n", g_szPgmName, argv[0] );
      }
   }

   if (g_mocaHandle != NULL)
   {
      if (g_NvModeEnabled)
      {
         moca_set_persistent(g_mocaHandle);
      }
      moca_close( g_mocaHandle );
      g_mocaHandle = NULL;
   }
   else
   { 
      fprintf(stderr,"%s: Cannot connect to mocad.  Is it running?\n", g_szPgmName );
      nExitCode = -3;
   }
 
   return( nExitCode < 0 ? nExitCode : 0 );
}

static int HelpHandler( void * handle, int argc, char **argv )
{
   unsigned int a,b,c,d;
   int ret = 0;

   moca_get_version(&a, &b, &c, &d);

   if (d)
      printf("\nmocap version: %u.%u.%u.%u\n", a, b, c, d);
   else
      printf("\nmocap version: %u.%u.%u\n", a, b, c);


   printf("\nUsage: mocap <option> <sub-options> [parameters]\n\n"
          "Type 'mocap <option> <sub-option> help' for more information about\n"
          "a particular sub-option.\n\n"
          "Options   Sub-Options\n"
          "-----------------------------------------------\n");

   if ((argc == 1) && !strcmp(argv[0], "advanced"))
   {
      ret++;
      moca_print_pvt_help();
   }
   else
      moca_print_help();

   return(ret);
}

