/****************************************************************************
*
* <:copyright-BRCM:2014:DUAL/GPL:standard
* 
*    Copyright (c) 2014 Broadcom 
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :>
*
*****************************************************************************/
/**
*
*  @file    bosFile.h
*
*  @brief   Contains the BOS File interface.
*
****************************************************************************/
/**
*  @defgroup   bosFile   BOS File
*
*  @brief      Provides FILE I/O access.
*
*  The BOS File interface provides access to file based I/O.
*
*  Typical client side usage:
*
*  @code
*  @endcode
*
*  Typical server side usage:
*
*  @code
*  @endcode
*
****************************************************************************/

#if !defined( BOSFILE_H )
#define BOSFILE_H           /**< Include Guard                           */

/* ---- Include Files ---------------------------------------------------- */

#if !defined( BOSCFG_H )
#  include <bosCfg.h>
#endif

#if !defined( BOSTYPES_H )
#  include <bosTypes.h>
#endif

#if !BOS_CFG_FILE
#error   BOS File module is not selected in bosCfg.h
#endif

#if BOS_OS_ECOS
#  include <../eCos/bosFileECOS.h>
#elif BOS_OS_PSOS
#  include <../pSOS/bosFilePSOS.h>
#elif BOS_OS_VXWORKS
#  include <../VxWorks/bosFileVxWorks.h>
#elif BOS_OS_WIN32
#  include <../Win32/bosFileWin32.h>
#elif BOS_OS_LINUXKERNEL
#  include <../LinuxKernel/bosFileLinuxKernel.h>
#elif BOS_OS_LINUXUSER
#  include <../LinuxUser/bosFileLinuxUser.h>
#else
#  error Unknown OS configuration from bosCfg.h
#endif

#if BOS_CFG_TASK
#  include <bosTask.h>
#endif

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @addtogroup bosFile
 * @{
 */
/* ---- Constants and Types ---------------------------------------------- */

/**
 * Function index, used with the error handler to report which function
 * an error originated from.
 *
 * These constants would typically only be used if you provide your own
 * error handler.
 *
 * @note If you change anything in this enumeration, please ensure that
 *       you make the corresponding updates in the bosFileInitFunctionNames
 *       routine.
 */

typedef enum
{
   BOS_FILE_INIT_FUNC_ID,        /**< bosFileInit               */
   BOS_FILE_TERM_FUNC_ID,        /**< bosFileTerm               */
   BOS_FILE_OPEN_FUNC_ID,        /**< bosFileOpen               */
   BOS_FILE_CLOSE_FUNC_ID,       /**< bosFileClose              */
   BOS_FILE_READ_FUNC_ID,        /**< bosFileRead               */
   BOS_FILE_WRITE_FUNC_ID,       /**< bosFileWrite              */
   BOS_FILE_SEEK_FUNC_ID,       /**< bosFileSeek              */

   BOS_FILE_NUM_FUNC_ID   /**< Number of functions in the Sem module.   */

} BOS_FILE_FUNC_ID;

/* ---- Variable Externs ------------------------------------------------- */

/**
 * Contains string versions of the file function names. This table is
 * indexed using values from the BOS_FILE_FUNC_ID enumeration.
 */

extern const char *gBosFileFunctionName[];

/* ---- Function Prototypes ---------------------------------------------- */
#if ( BOS_CFG_RESET_SUPPORT )

   #if ( !BUILDING_BOS)

   #else

   #endif

#endif   /* BOS_CFG_RESET_SUPPORT */


/***************************************************************************/
/**
*  Initializes the BOS File module.
*
*  @return  BOS_STATUS_OK if the initialization was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosFileInit( void );

/***************************************************************************/
/**
*  Terminates the BOS File module.
*
*  @return  BOS_STATUS_OK if the termination was successful, or an error
*           code if some type of error occured.
*/

BOS_STATUS bosFileTerm( void );

/*
*****************************************************************************
 * Function Name: bosFileOpen
 * Description  : Opens a BOS_FILE
 * Arguments    : fileP - Pointer to an uninitializaed BOS_FILE object
                  fileName - Name of file to open
                  flags - File open option flags
 * Returns      : BOS_STATUS_OK	 = successful
 *                BOS_STATUS_ERR = failed
*****************************************************************************
*/
BOS_STATUS bosFileOpen
(    
   BOS_FILE *fileP,
   const char *fileName,   
   BOS_UINT32 flags
);

/*
*****************************************************************************
 * Function Name: bosFileClose
 * Description  : Closes a BOS_FILE
 * Arguments    : fileP - Pointer to a valid BOS_FILE object

 * Returns      : BOS_STATUS_OK	 = successful
 *                BOS_STATUS_ERR = failed
*****************************************************************************
*/
BOS_STATUS bosFileClose
(
   BOS_FILE *fileP
);

/*
*****************************************************************************
 * Function Name: bosFileRead
 * Description  : Reads data from a BOS_FILE.
 * Arguments    : fileP - Pointer to a valid BOS_FILE object
                  pData - Pointer to buffer to which data is to be written
                  numBytes - Number of bytes to read
                  
 * Returns      : Number of bytes Wrutten
*****************************************************************************
*/
BOS_UINT32 bosFileRead
(
   BOS_FILE *fileP,
   char * pData,
   BOS_UINT32 numBytes
);

/*
*****************************************************************************
 * Function Name: bosFileWrite
 * Description  : Writes data to the BOS_FILE.
 * Arguments    : fileP - Pointer to a valid BOS_FILE object
                  pData - Pointer to the data to be written
                  numBytes - Number of bytes to write
                  
 * Returns      : Number of bytes Wrutten
*****************************************************************************
*/
BOS_SINT32 bosFileWrite
( 
   BOS_FILE *fileP,
   char * pData,
   BOS_UINT32 numBytes
);

/*
*****************************************************************************
 * Function Name: bosFileSeek
 * Description  : Seek to specific offset in a BOS_FILE
 * Arguments    : fileP  - Pointer to a valid BOS_FILE object
                  offset - Offset to seek to
                  whence - seek mode ( see BOS_FILE_SEEK_XXX modes )
 * Returns      : BOS_STATUS_OK if succeeded
*****************************************************************************
*/
BOS_STATUS bosFileSeek
(    
   BOS_FILE *fileP,
   BOS_SINT64 offset,   
   BOS_SINT32 whence
);


/** @} */
#if defined( __cplusplus )
}
#endif

#endif /* BOSFILE_H */

