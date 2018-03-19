/**
 * \addtogroup common
 */
/*@{*/

/***************************************************
 * Header name: gigerror.h
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
 * First written on 13/07/2006 by I. Barrera
 *
 ***************************************************/
/** \file gigerror.h
 *
 * \brief This contains the definition of errors
 *
 * All error definitions for Concorde modules must
 * be contained here
 *
 * $Id: gigerror.h 299852 2011-12-01 03:46:47Z $
 **************************************************/

/* FILE-CSTYLED */

#ifndef GIGERROR_H_
#define GIGERROR_H_

/***************************************************
 *                 Public Constants Section
 ***************************************************/

/** \brief Error codes. */
enum {
  GIG_ERR_OK             =   0, /**< Result is OK */
  GIG_ERR_ERROR          =  -1, /**< Result is a generic (undefined) error */
  GIG_ERR_MQUEUE_FULL    =  -2, /**< Full queue error. Used when sending to a mqueue (GigOS) */
  GIG_ERR_MQUEUE_EMPTY   =  -3, /**< Empty queue error. Used when receiving from a mqueue (GigOS) */
  GIG_ERR_INV            =  -4, /**< Invalid parameter */
  GIG_ERR_DISCARDED      =  -5, /**< Discarded log (log level not high enough to be displayed - GigLog) */
  GIG_ERR_EOF            =  -6, /**< End of input hit (only used by the GigLog parser so far) */
  GIG_ERR_SYNTAX         =  -7, /**< Syntax error of a command (GigLog) */
  GIG_ERR_NG             =  -8, /**< Wrong result of a command (GigLog) */
  GIG_ERR_PENDING        =  -9, /**< Operation didn't finish, still pending. */
  GIG_ERR_PARTIAL        = -10, /**< Partially fit (Bwmgr) */
  GIG_ERR_NOBANDWIDTH    = -11, /**< No bandwidth available (Bwmgr) */
  GIG_ERR_NORESOURCES    = -12, /**< No resources available (Bwmgr) */
  GIG_ERR_REQS_INCORRECT = -13, /**< Incorrect requirements (Bwmgr) */
  GIG_ERR_NOT_SUPPORTED  = -14, /**< Not supported function. */
  GIG_ERR_BUSY           = -15, /**< The requested operation can't be accepted because target module is busy */
  GIG_ERR_HOLD           = -16, /**< The received packet wasn't processed by its registered client: HPAV will keep it */
  GIG_ERR_NOT_FOUND      = -17, /**< Not found (Tonemapboss). */
  GIG_ERR_TIMEOUT        = -18 /**< Timeout */
};

/***************************************************
 *                 Public Defines Section
 ***************************************************/

/** \brief Type for the returned error code from functions. */
typedef int t_gig_error;

#else /*GIGERROR_H_*/
#error "Header file __FILE__ has already been included!"
#endif /*GIGERROR_H_*/

/*@}*/
