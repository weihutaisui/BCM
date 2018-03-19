/**
 * \addtogroup common
 */
/*@{*/

/***************************************************
 * Header name: gigle_version.h
 *
 * Copyright 2017 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 * First written on 18/10/2010 by Toni Homedes
 *
 ***************************************************/
/** \file gigle_version.h
 *
 * \brief This contains the version number and description
 *
 * Version 0.0.1 - 20/10/2010
 *       First compilable version
 *
 * $Id: gigle_version.h 299852 2011-12-01 03:46:47Z $
 ****************************************************/

/* FILE-CSTYLED */

#ifndef VERSION_H_
#define VERSION_H_

/***************************************************
 *                 Public Defines Section
 ***************************************************/

/** \brief Version branch */
#define VERSION_BRANCH  (6) // WIFI

/** \brief Version major */
#define VERSION_MAJOR   (2)

/** \brief Version revison */
#define VERSION_REVISION (0)

/** \brief Version minor */
#define VERSION_MINOR   (9)

/*************** HW version defines ******************/
// only the lower 16 bits can be used to identify hw version

//Gigle-WiFi
#define HWVER_RELEASE_MII_GGL541AC 0x80000701
#define HWVER_RELEASE_WBROADCOM_A2XMIIHPAV 0x80000703

/************** Release defines ***********************/

#ifdef RELEASE
  /* */
#endif // endif

#define RAW_SOCKET_INPUT_IF            "vlan7"
#define RAW_SOCKET_OUTPUT_IF           "vlan7"

//#define PRECONFIGURE_NVRAM_ON_GIGLED

#define FW_VERSION (VERSION_BRANCH << 24 | VERSION_MAJOR << 16 | VERSION_REVISION << 8 | VERSION_MINOR)

#else /*VERSION_H_*/
#error "Header file __FILE__ has already been included!"
#endif /*VERSION_H_*/

/*@}*/
