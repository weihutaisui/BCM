/******************************************************************************
 *
 * Copyright (c) 2015   Broadcom Corporation
 * All Rights Reserved
 *
 * <:label-BRCM:2009:proprietary:standard
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
 *
 *    Copyright (c) 2008-2009, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Description: MoCA userland daemon
 *
 ***************************************************************************/

#include <errno.h>
#include <moca_os.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#if defined(WIN32)
#include <io.h>

#ifndef O_SYNC
#define O_SYNC 0
#endif

#define open _open
#define read _read
#define write _write
#define close _close


#else
#include <unistd.h>
#endif
#include <fcntl.h>

#if defined(DSL_MOCA)
#include <cms_psp.h>
#endif

#include "mocad-timer.h"

// MOCAD_MPS_USE_OPENSSL - Use OpenSSL
// MOCAD_MPS_USE_OPENSSL_LOW - Use OpenSSL low level EC APIs
#if defined(MOCAD_MPS_USE_OPENSSL) ||  defined(MOCAD_MPS_USE_OPENSSL_LOW)
/* Using OpenSSL */
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ecdh.h>

#define PRIV_KEY_TYPE BIGNUM

#else

/* No crypto lib so MPS is not supported! But we try emulate and test as much as possible without crypto lib. */

typedef struct _privkey {
  unsigned char key[32];
} privkey_t;

#define PRIV_KEY_TYPE privkey_t

#endif

#ifdef MOCAD_MPS_USE_TEST_VECTORS

/* Test Vectors taken from RFC5903 Section 8.1 */
char *privstr1 = "C88F01F5 10D9AC3F 70A292DA A2316DE5 44E9AAB8 AFE84049 C62A9C57 862D1433";
char *pubstr1  = "DAD0B653 94221CF9 B051E1FE CA5787D0 98DFE637 FC90B9EF 945D0C37 72581180"
                 "5271A046 1CDB8252 D61F1C45 6FA3E59A B1F45B33 ACCF5F58 389E0577 B8990BB3";
char *privstr2 = "C6EF9C5D 78AE012A 011164AC B397CE20 88685D8F 06BF9BE0 B283AB46 476BEE53";
char *pubstr2  = "D12DFB52 89C8D4F8 1208B702 70398C34 2296970A 0BCCB74C 736FC755 4494BF63"
                 "56FBF3CA 366CC23E 8157854C 13C58D6A AC23F046 ADA30F83 53E74F33 039872AB";
char *shared_secret_x = "D6840F6B 42F6EDAF D13116E0 E1256520 2FEF8E9E CE7DCE03 812464D0 4B9442DE";

char *hashstr[4] = {
   "b2c6cfeacd5492027c6bea51618c5925",
   "bd7e62655c53402bb7cabdb060ed067a",
   "d086e80408834113f273da209377c19c",
   "f6c943ab8ff81a139f7e6af89ac656c2"
};
int hashstrsz = sizeof(hashstr)/sizeof(hashstr[0]);

/* Calculated test vector from above shared_secret_x and NN GUID hardcoded in mocad_handle_mps_request_mpskey() 
 * Removed MSB 911b2b20 92950171 ab7a39ae 3e099258 
 */
char *mpskeystr = "3e79f06e a27a6495 46d67ac3 88952211";

PRIV_KEY_TYPE private_key;

// NB. Assumes it's a valid hex character
unsigned char ctoi(char c) {
  if (c >= 'a')
    return c - 'a' + 10;
  if (c >= 'A')
    return c - 'A' + 10;
  return c - '0';
}

void keyStr2Bin(unsigned char *bin, int len, char *str) {
  int i, j;
  // NB. Only works for even length strings!
  for (i = 0, j = 0; j < len && str[i] != '\0'; i++) {
    if (!isxdigit(str[i]))
      continue;
    bin[j] = (ctoi(str[i]) << 4) + ctoi(str[i+1]);
    i++;
    j++;
  }
}

#endif // MOCAD_MPS_USE_TEST_VECTORS

#include <mocalib.h>
#include "mocaint.h"
#include "mocad.h"
#include "mocad-gen.h"
#include "mocad-int.h"

#include "mocad-mps.h"

#define MPS_SESSION_LOG(...) do { mocad_log(ctx, L_INFO, __VA_ARGS__); } while (0)

#if MPS_DEBUG_EN
#define MPS_DEBUG(...) do { mocad_log(ctx, L_ALWAYS, __VA_ARGS__); } while (0)
#define MPS_DEBUG2(...) do { mocad_log(ctx, L_ALWAYS, __VA_ARGS__); } while (0)
#define MPS_DEBUG_PRINTKEY(prefix, key, keylen) mocad_mps_debug_printkey(prefix, key, keylen)
void mocad_mps_debug_printkey(char *prefix, unsigned char *key, int keylen) {
  int i;
  printf("%s:\n", prefix);
  for (i = 0; i < keylen; i++) {
    if ((i % 16) == 0)
      printf("\n");
    if ((i % 4) == 0)
      printf(" ");
    printf("%02x:", key[i]);
  }
  printf("\n");
}
#else
#define MPS_DEBUG(...)
#define MPS_DEBUG2(...) 
#define MPS_DEBUG_PRINTKEY(prefix, key, keylen)
#endif



void mocad_handle_mps_timeout(struct mocad_ctx *ctx, void *arg);
void mocad_mps_new_session(struct mocad_ctx *ctx);
void mocad_mps_set_mpsdata(struct mocad_ctx *ctx, unsigned char *pub_key);
void mocad_mps_rehash_pubkey_guid(struct mocad_ctx *ctx, struct moca_mps_data *mpsdata);
void mocad_mps_start_timer(struct mocad_ctx *ctx);

int mocad_read_mps_data(struct mocad_ctx *ctx, unsigned char *paired, char *privacy_en,
                        char *password, int len);
int mocad_write_mps_data(struct mocad_ctx *ctx, unsigned char paired, char privacy_en,
                         const char *password, int len);

void mocad_mps_restore_defaults_internal(struct mocad_ctx *ctx, int read_mps_data, unsigned char paired);

// This is called after the generated defaults. 
// I.e. override the generated defaults here before mocad gets a chance to start using it.
MOCAD_S void mocad_mps_restore_defaults(struct mocad_ctx *ctx) {
   mocad_mps_restore_defaults_internal(ctx, 1, 0);
}

// This is called from restore defaults, including at mocad initialization, as well as from mps_reset
MOCAD_S void mocad_mps_restore_defaults_internal(struct mocad_ctx *ctx, int read_mps_data, unsigned char paired) {
#if (MPS_EN == 1)
   char privacy_en = ctx->any_time.privacy_en;
   unsigned char password[17+1];

#ifdef STANDALONE
   if (ctx->config_flags & MOCA_6802B0_FLAG) {
      ctx->any_time.mps_en = 0;
   }
#endif

   if (read_mps_data) {
      mocad_read_mps_data(ctx, &paired, &privacy_en, (char *) password, sizeof(password));
   }

   // If we succeeded in reading persistent MPS data and we are Paired, then we need to use
   // the persisted Privacy and Password
   if (paired == 1) {
      if (mocad_validate_password(password)) {
         mocad_log(ctx, L_ERR, "Invalid MPS Persisted password\n");
         // Ignore the persisted data if password was invalid! (corrupt data?)
         paired = 0;
      }
      else {
         ctx->privacy_changed = PRIVACY_CHANGED_MPS;
         ctx->any_time.privacy_en = privacy_en;
         mocad_set_anytime_network_password(ctx, password);
         MPS_DEBUG2("MPS Persisted Password %s\n", password);
      }
   }
#else
   ctx->any_time.mps_en = 0;
#endif

   ctx->mps_state = paired;
   ctx->any_time.mps_state = (ctx->mps_state == 1) ? 1 : 0;

   // If Unpaired, and Privacy settings didn't change Manually, use the defaults
   if (ctx->mps_state != 1 && !ctx->privacy_changed) {
      mocad_set_privacy_defaults(ctx);
   }

   /* GUID/MAC address may change due to restore defaults, but we will wait
    * and update Public key hash once core restarts.
    */
}

MOCAD_S void mocad_mps_reset(struct mocad_ctx *ctx) {

   mocad_mps_stop(ctx);

   // Since mps_reset requires MoCA restart, there's 
   // no need to generate new keys & reset trigger (PBState CLEAR) because
   // This also takes care of clearing all internal state in FW and dropping out of the network.

   // Clear persistent MPS data State to 2, meaning MPS Reset
   mocad_write_mps_data(ctx, 2, -1, NULL, 0);

   // Factory defaults
   ctx->privacy_changed = 0;
   mocad_mps_restore_defaults_internal(ctx, 0, 2);
}

MOCAD_S int mocad_mps_button_press(struct mocad_ctx *ctx) {
#if (MPS_EN == 1)
   if (!ctx->any_time.mps_en) {
      mocad_log(ctx, L_WARN, "MPS is not enabled!\n");
      return MOCA_API_ERROR;
   }

   if (__moca_set_mps_trig(ctx, 1)) {
      mocad_log(ctx, L_ERR, "Unable to set MPS Trigger!\n");
      return MOCA_API_ERROR;
   }

   // Start the MPS Timer upon receiving the MPS Trigger Event
   // This is so we also catch the scenario where NC receives MPS Trigger PIE from EN
   // and so we don't start the timer when we are the EN or for any reason
   // must ignore this MPS button press

   // If Unpaired, stop any MPS Unpaired timer running at this time (as a result of
   // previously being connected without MPS to a Privacy Disabled network)
   // Because it would be bizarre for it to expire and we become Paired in the middle of MPS session...
   mocad_mps_stop_unpaired_timer(ctx);

   return 0;
#else
   mocad_log(ctx, L_ERR, "MPS unsupported in this build!\n");
   return MOCA_API_ERROR;
#endif
}

MOCAD_S void mocad_mps_stop(struct mocad_ctx *ctx) {
#if (MPS_EN == 1)
   // stop timers and anything else going on!
   MPS_DEBUG("Stop MPS Timer!\n");
   mocad_timer_remove(ctx, &ctx->timer_head, &ctx->mps_timer);
   mocad_mps_stop_unpaired_timer(ctx);
#endif
}

MOCAD_S void mocad_mps_stop_unpaired_timer(struct mocad_ctx *ctx) {
#if (MPS_EN == 1)
   MPS_DEBUG("Stop MPS Unpaired Timer!\n");
   mocad_timer_remove(ctx, &ctx->timer_head, &ctx->mps_unpaired_timer);
#endif
}

MOCAD_S void mocad_handle_mps_unpaired_timeout(struct mocad_ctx *ctx, void *arg) {
   MPS_DEBUG("MPS Unpaired Timeout!\n");

   ctx->mps_state = 1;
   ctx->any_time.mps_state = 1;
   // Inform FW of Paired state
   if (__moca_set_mps_state(ctx, ctx->mps_state)) {
      mocad_log(ctx, L_ERR, "Unable to set MPS State (%d) in FW\n", ctx->mps_state);
   }

   // Persist it
   mocad_write_mps_data(ctx, 1, ctx->any_time.privacy_en, 
                        ctx->any_time.password.password, sizeof(ctx->any_time.password.password));
}

MOCAD_S void mocad_mps_link_state(struct mocad_ctx *ctx, int new_state) {
#if (MPS_EN == 1)
   if (new_state == LINK_STATE_DOWN) {
      MPS_DEBUG("MPS Link DOWN: MPS State %d\n", ctx->mps_state);
      /* "If the NC drops from the network, the NC MUST set PBState to CLEAR, set MPSReqRx = NO, 
       * reset its MPS timer (if it is running)." 
       * NNs would not be able to drop from the network, so doing this without knowing
       * whether this node is NC or NN is not going to hurt.
       */
      if (__moca_set_mps_trig(ctx, 0)) {
         mocad_log(ctx, L_ERR, "Unable to clear MPS Trigger!\n");
      }

      // It would be odd if we become Paired while link is down, therefore 
      // also stop MPS Unparied timer
      mocad_mps_stop(ctx);

      // Need new session for next time MPS is started
      MPS_SESSION_LOG("MPS Session Ends: Link down.\n");
      mocad_mps_new_session(ctx);

      return;
   }

   if (new_state != LINK_STATE_UP)
      return;

   // LINK_STATE_UP:
   MPS_DEBUG("MPS Link UP: MPS State %d\n", ctx->mps_state);

   // Note: If MPS was used we should become Paired right away
   // This has already been taken care of and persisted when MPS success event arrived

   // If already Paired, then nothing to do.
   if (ctx->mps_state == 1) {
      return;
   }

   // If Privacy is Enabled we should become Paired right away
   if (ctx->any_time.privacy_en) {
      MPS_DEBUG("MPS Link UP: MPS State %d privacy_en %d - set Paired\n", ctx->mps_state, ctx->any_time.privacy_en);
      ctx->mps_state = 1;
      ctx->any_time.mps_state = 1;
      // Inform FW of Paired state
      if (__moca_set_mps_state(ctx, ctx->mps_state)) {
         mocad_log(ctx, L_ERR, "Unable to set MPS State (%d) in FW\n", ctx->mps_state);
      }

      // Persist it
      mocad_write_mps_data(ctx, 1, ctx->any_time.privacy_en, 
                           ctx->any_time.password.password, sizeof(ctx->any_time.password.password));
      return;
   }

   // Otherwise Privacy is Disabled and MPS was not used, therefore we need to start the MPS Unpaired Timer
   MPS_DEBUG("MPS Link UP:  MPS State %d privacy_en %d - set MPS Unpaired Timer for %u seconds!\n", 
             ctx->mps_state, ctx->any_time.privacy_en, ctx->any_time.mps_unpaired_time);
   mocad_timer_add(ctx, &ctx->timer_head, &ctx->mps_unpaired_timer, ctx->any_time.mps_unpaired_time, mocad_handle_mps_unpaired_timeout, NULL);
#endif
}

MOCAD_S void mocad_mps_start_timer(struct mocad_ctx *ctx) {
#if !defined(__EMU_HOST_20__)
   MPS_DEBUG("Set MPS Timer for %u seconds!\n", ctx->any_time.mps_walk_time);
   mocad_timer_add(ctx, &ctx->timer_head, &ctx->mps_timer, ctx->any_time.mps_walk_time, mocad_handle_mps_timeout, NULL);
#else
   // In emulator, select() runs real time(!) while the rest runs on simulation time. 
   // Therefore we generally need more time!
   MPS_DEBUG("Set MPS Timer for %u seconds!\n", ctx->any_time.mps_walk_time * 10);
   mocad_timer_add(ctx, &ctx->timer_head, &ctx->mps_timer, ctx->any_time.mps_walk_time * 10, mocad_handle_mps_timeout, NULL);
#endif
}

MOCAD_S void mocad_handle_mps_init_scan_rec(struct mocad_ctx *ctx) {
   struct moca_mps_init_scan_payload payload;
   MPS_DEBUG("MPS Init Scan Received!\n");


   if (moca_get_mps_init_scan_payload(ctx, &payload)) {
      mocad_log(ctx, L_ERR, "Unable to retrieve MPS Init Scan Payload!\n");
      return;
   }

   ctx->mps_init_scan_tot_nets++;
   if (payload.mps_code & 0x1)
      ctx->mps_init_scan_trig_nets++;

   mocad_log(ctx, L_INFO, "MPS Init Scan Received: Total nets found: %d. %d with MPS Triggered\n",
             ctx->mps_init_scan_tot_nets,
             ctx->mps_init_scan_trig_nets);


#ifdef MPS_DEBUG
   MPS_DEBUG("MPS Init Scan Payload:\n");
   MPS_DEBUG("channel:         %u\n", payload.channel);
   MPS_DEBUG("nc_moca_version: %x\n", payload.nc_moca_version);
   MPS_DEBUG("mps_code:        %x%s\n", payload.mps_code, (payload.mps_code & 0x1) ? ": MPS TRIGGERED" : "");
   MPS_DEBUG("mps_parameters:  %x: %s %s %s\n", 
	     payload.mps_parameters,
	     (payload.mps_parameters & 0x4) ? "Privacy Enabled" : "Privacy Disabled",
	     (payload.mps_parameters & 0x2) ? ", Privacy Receive" : "",
	     (payload.mps_parameters & 0x2) && (payload.mps_parameters & 0x1) ? ", Privacy Down" : "");
   if (payload.network_name[15] != '\0') {
     payload.network_name[15] = '\0';
     MPS_DEBUG("network_name:    %s [truncated]\n", payload.network_name);
   }
   else {
     MPS_DEBUG("network_name:    %s\n", payload.network_name);
   }
#endif
}

MOCAD_S void mocad_handle_mps_privacy_changed(struct mocad_ctx *ctx) {
   struct moca_network_password new_passwd;
   uint32_t priv = ctx->any_time.privacy_en;

   MPS_SESSION_LOG("MPS Session Ends: Success, Privacy Changed Received!\n");

   if (moca_get_privacy_en(ctx, &priv))
      mocad_log(ctx, L_ERR, "Failed to retrieve new privacy setting\n");
   else {
      if (priv != ctx->any_time.privacy_en)
         mocad_log(ctx, L_INFO, "Privacy changed to %u\n", priv);
      ctx->any_time.privacy_en = priv;
   }

   if (__moca_get_network_password(ctx, &new_passwd)) {
      mocad_log(ctx, L_ERR, "Unable to retrieve new password\n");
      return;
   }

   if (mocad_validate_password((const unsigned char *) new_passwd.password)) {
      mocad_log(ctx, L_ERR, "Invalid new password received\n");
      return;
   }

   MPS_DEBUG2("Password is %s\n", new_passwd.password);
   mocad_set_anytime_network_password(ctx, (const unsigned char *)new_passwd.password);
   if (mocad_set_keys(ctx, &new_passwd)) 
      mocad_log(ctx, L_ERR, "Failed to set new keys\n");

   // Now Paired. This has already been set in FW.
   ctx->mps_state = 1;
   ctx->any_time.mps_state = 1;

   mocad_mps_new_session(ctx);

   ctx->privacy_changed = PRIVACY_CHANGED_MPS;
   mocad_write_mps_data(ctx, 1, ctx->any_time.privacy_en, 
                        ctx->any_time.password.password, sizeof(ctx->any_time.password.password));
}

MOCAD_S void mocad_handle_mps_trigger(struct mocad_ctx *ctx) {
   MPS_SESSION_LOG("MPS Session Starts: MPS Trigger received!\n");

   // XX If not already started:
   mocad_mps_start_timer(ctx);
}

MOCAD_S void mocad_handle_mps_timeout(struct mocad_ctx *ctx, void *arg) {
   MPS_SESSION_LOG("MPS Session Ends: Timeout!\n");

   if (__moca_set_mps_trig(ctx, 0)) {
      mocad_log(ctx, L_ERR, "Unable to clear MPS Trigger!\n");
   }

   mocad_mps_new_session(ctx);
}

MOCAD_S void mocad_handle_mps_admission_nochange(struct mocad_ctx *ctx) {
   MPS_SESSION_LOG("MPS Session Ends: Admission with Node Privacy settings!\n");
   mocad_mps_new_session(ctx);

   // Now Paired. This has already been set in FW.
   ctx->mps_state = 1;
   ctx->any_time.mps_state = 1;

   ctx->privacy_changed = PRIVACY_CHANGED_MPS;
   mocad_write_mps_data(ctx, 1, ctx->any_time.privacy_en, 
                        ctx->any_time.password.password, sizeof(ctx->any_time.password.password));
}

MOCAD_S void mocad_handle_mps_pair_failed(struct mocad_ctx *ctx) {
   MPS_SESSION_LOG("MPS Session Ends: Pair Failed!\n");
   mocad_mps_new_session(ctx);
}

#ifdef MOCAD_MPS_USE_OPENSSL
MOCAD_S int bytes_to_public_key(struct mocad_ctx *ctx, EVP_PKEY **pkey, const unsigned char *key_bytes, int key_len, const PRIV_KEY_TYPE *priv_key) {
  EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if(ec_key == NULL) {
    mocad_log(ctx, L_ERR, "EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)\n");
    return -3;
  }

  if (o2i_ECPublicKey(&ec_key, &key_bytes, key_len) == NULL) {
    mocad_log(ctx, L_ERR, "o2i_ECPublicKey error\n");
    ERR_print_errors_fp(stdout);
    // XX leaks ec_key!
    return -1;
  }

  *pkey = EVP_PKEY_new();
  // void EVP_PKEY_free(EVP_PKEY *key);
  if (EVP_PKEY_assign_EC_KEY(*pkey, ec_key) != 1) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_assign_EC_KEY error\n");
    // XX leaks ec_key and pkey!
    return -2;
  }

  if (priv_key != NULL) {
    EC_KEY_set_private_key(ec_key, priv_key); // XX cleanup?
    //MPS_DEBUG("EC_KEY: PUB & PRIV KEY:\n");
  }
  else {
    //MPS_DEBUG("EC_KEY: PUBLIC ONLY KEY:\n");
  }
  //EC_KEY_print_fp(stdout, ec_key, 0);

  // note: ec_key will be freed as part of pkey in caller.
  return 0;
}
#elif defined(MOCAD_MPS_USE_OPENSSL_LOW)
int bytes_to_ec_key(struct mocad_ctx *ctx, EC_KEY **key, const unsigned char *key_bytes, int key_len, const PRIV_KEY_TYPE *priv_key) {
  EC_KEY *ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if(ec_key == NULL) {
    mocad_log(ctx, L_ERR, "EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)\n");
    return -3;
  }

  if (o2i_ECPublicKey(&ec_key, &key_bytes, key_len) == NULL) {
    mocad_log(ctx, L_ERR, "o2i_ECPublicKey error\n");
    EC_KEY_free(ec_key);
    return -1;
  }

  if (priv_key != NULL) {
     EC_KEY_set_private_key(ec_key, priv_key); // XX check return and cleanup?
     //MPS_DEBUG("EC_KEY: PUB & PRIV KEY:\n");
  }
  else {
     //MPS_DEBUG("EC_KEY: PUBLIC ONLY KEY:\n");
  }
  //EC_KEY_print_fp(stdout, ec_key, 0);

  *key = ec_key;
  return 0;
}
#endif

MOCAD_S void mocad_mps_key_compute(struct mocad_ctx *ctx, unsigned char *peer_public_key, int is_nn, unsigned char *nn_guid, unsigned char *mpskey) {
#ifdef MOCAD_MPS_USE_OPENSSL
  const unsigned char *my_key_bytes = &ctx->pub_key[0];
  const PRIV_KEY_TYPE *my_priv_key = ctx->priv_key;

  // Create my key from my_key_bytes, which is uncompressed form (x and y-coordinate) incl tag byte
  int pkey_len = MPS_PUB_KEYLEN;
  EVP_PKEY *my_pkey;
  if (bytes_to_public_key(ctx, &my_pkey, my_key_bytes, pkey_len, my_priv_key) != 0) {
    mocad_log(ctx, L_ERR, "bytes_to_public_key my_pkey\n");
    return; // XX error handling XX malloc/create/free/destroy/ctx etc!
  }

  // Create peer's key from peer_public_key, which is only the x-coordinate (compressed form) without tag byte
  pkey_len = 33;
  unsigned char peer_public_key_bytes[33];
  peer_public_key_bytes[0] = 0x02;
  memcpy(&peer_public_key_bytes[1], peer_public_key, 32);
  EVP_PKEY *peers_key;
  if (bytes_to_public_key(ctx, &peers_key, peer_public_key_bytes, pkey_len, NULL) != 0) {
    mocad_log(ctx, L_ERR, "bytes_to_public_key peers_pkey\n");
    return; // XX error handling XX malloc/create/free/destroy/ctx etc!
  }

  // XX print everything
  // int ECParameters_print_fp(FILE *fp, const EC_KEY *key);

  EVP_PKEY_CTX *dctx;
  unsigned char *dhkey;
  size_t dhkeylen;

  // key derivation
  dctx = EVP_PKEY_CTX_new(my_pkey, NULL);
  if (dctx == NULL) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
  }

  if (EVP_PKEY_derive_init(dctx) != 1) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
  }

  if (EVP_PKEY_derive_set_peer(dctx, peers_key) != 1) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
    ERR_print_errors_fp(stdout);
  }

  if (EVP_PKEY_derive(dctx, NULL, &dhkeylen) != 1) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
    ERR_print_errors_fp(stdout);
  }

  dhkey = OPENSSL_malloc(dhkeylen);
  if (dhkey == NULL) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
  }

  if (EVP_PKEY_derive(dctx, dhkey, &dhkeylen) != 1) {
    mocad_log(ctx, L_ERR, "line %d EVP_PKEY_derive\n", __LINE__);
    ERR_print_errors_fp(stdout);
  }

  //  MPS_DEBUG_PRINTKEY("DHKey", dhkey, dhkeylen);

  // END key derivation

#if defined(MOCAD_MPS_USE_TEST_VECTORS) && !defined(__EMU_HOST_20__) && defined(MOCAD_MPS_USE_OPENSSL)
  unsigned char dhkey_testvec[32];
  keyStr2Bin(dhkey_testvec, sizeof(dhkey_testvec), shared_secret_x);
  if (memcmp(dhkey_testvec, dhkey, sizeof(dhkey_testvec)) == 0) {
    MPS_DEBUG("DHKey Test vec matches: PASS!\n");
  }
  else {
    MPS_DEBUG("DHKey Test vec mismatch: FAIL!\n");
  }
#endif

  // Create MPS Key
  // MPSKey = the 128 LSBs of HMAC-SHA-256 (DHKey, x_(Q_NN )|| GUID of the NN || x_(Q_NC )),
  unsigned char output[EVP_MAX_MD_SIZE];
  unsigned int outputlen = EVP_MAX_MD_SIZE;

  HMAC_CTX hmac_ctx;
  HMAC_CTX_init(&hmac_ctx);
  HMAC_Init_ex(&hmac_ctx, dhkey, dhkeylen, EVP_sha256(), NULL);

  // xQnn
  EC_KEY *nn_ec_key = EVP_PKEY_get1_EC_KEY(is_nn ? my_pkey : peers_key);
  unsigned char *nn_key = NULL;
  int key_len = 32; // only the x part
  if (i2o_ECPublicKey(nn_ec_key, &nn_key) == 0) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
  }
  HMAC_Update(&hmac_ctx, nn_key+1, key_len); // +1 to skip the tag

  // NN's GUID
  HMAC_Update(&hmac_ctx, nn_guid, 8);

  // xQnc
  EC_KEY *nc_ec_key = EVP_PKEY_get1_EC_KEY(is_nn ? peers_key : my_pkey);
  unsigned char *nc_key = NULL;
  if (i2o_ECPublicKey(nc_ec_key, &nc_key) == 0) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
  }
  HMAC_Update(&hmac_ctx, nc_key+1, key_len); // +1 to skip the tag

  HMAC_Final(&hmac_ctx, output, &outputlen);

  MPS_DEBUG_PRINTKEY("Full MPSkey", output, outputlen);

  // Only the 16bytes of LSB are used:
  memcpy(mpskey, output+outputlen-16, 16);

  // END Create MPS Key

  HMAC_CTX_cleanup(&hmac_ctx);

  OPENSSL_free(nn_key);
  OPENSSL_free(nc_key);

  EVP_PKEY_CTX_free(dctx);
  EVP_PKEY_free(peers_key);
  EVP_PKEY_free(my_pkey);

  OPENSSL_free(dhkey);

#elif defined(MOCAD_MPS_USE_OPENSSL_LOW)
  //MOCAD_S void mocad_mps_key_compute(struct mocad_ctx *ctx, unsigned char *peer_public_key, int is_nn, unsigned char *nn_guid, unsigned char *mpskey) {

  EC_KEY *peer_key = NULL;
  EC_KEY *my_key = NULL;
  int field_size;
  int dhkeylen;
  unsigned char *dhkey = NULL;
  int pkey_len = MPS_PUB_KEYLEN;
  const unsigned char *my_key_bytes = &ctx->pub_key[0];
  const PRIV_KEY_TYPE *my_priv_key = ctx->priv_key;

  // re-create our key pair
  if (bytes_to_ec_key(ctx, &my_key, my_key_bytes, pkey_len, my_priv_key) != 0) {
     mocad_log(ctx, L_ERR, "bytes_to_ec_key my_key\n");
     return;
  }

  // create peers pub key from its x-coordinate
  pkey_len = 33;
  unsigned char peer_public_key_bytes[33];
  peer_public_key_bytes[0] = 0x02;
  memcpy(&peer_public_key_bytes[1], peer_public_key, 32);
  if (bytes_to_ec_key(ctx, &peer_key, peer_public_key_bytes, pkey_len, NULL)) {
     mocad_log(ctx, L_ERR, "bytes_to_ec_key my_key\n");
     EC_KEY_free(my_key);
     return;
  }

  // determine size and alloc DH shared secret
  field_size = EC_GROUP_get_degree(EC_KEY_get0_group(my_key));
  dhkeylen = (field_size+7)/8;
  dhkey = OPENSSL_malloc(dhkeylen);
  if (dhkey == NULL) {
     mocad_log(ctx, L_ERR, "malloc dhkey failed\n");
     EC_KEY_free(my_key);
     EC_KEY_free(peer_key);
     return;
  }

  // Derive Diffie Hellman shared secret
  dhkeylen = ECDH_compute_key(dhkey, dhkeylen, EC_KEY_get0_public_key(peer_key), my_key, NULL);
  if (dhkeylen <= 0) {
     mocad_log(ctx, L_ERR, "ECDH_compute_key\n");
     EC_KEY_free(my_key);
     EC_KEY_free(peer_key);
     OPENSSL_free(dhkey);
     return;
  }


  // Create MPS Key
  // MPSKey = the 128 LSBs of HMAC-SHA-256 (DHKey, x_(Q_NN )|| GUID of the NN || x_(Q_NC )),
  unsigned char output[EVP_MAX_MD_SIZE];
  unsigned int outputlen = EVP_MAX_MD_SIZE;

  HMAC_CTX hmac_ctx;
  HMAC_CTX_init(&hmac_ctx);
  HMAC_Init_ex(&hmac_ctx, dhkey, dhkeylen, EVP_sha256(), NULL);

  // xQnn
  EC_KEY *nn_ec_key = is_nn ? my_key : peer_key;
  unsigned char *nn_key = NULL;
  int key_len = 32;
  if (i2o_ECPublicKey(nn_ec_key, &nn_key) == 0) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
    EC_KEY_free(my_key);
    EC_KEY_free(peer_key);
    OPENSSL_free(dhkey);
    HMAC_CTX_cleanup(&hmac_ctx);
    return;
  }
  HMAC_Update(&hmac_ctx, nn_key+1, key_len); // +1 to skip the tag

  // NN's GUID
  HMAC_Update(&hmac_ctx, nn_guid, 8);

  // xQnc
  EC_KEY *nc_ec_key = is_nn ? peer_key : my_key;
  unsigned char *nc_key = NULL;
  if (i2o_ECPublicKey(nc_ec_key, &nc_key) == 0) {
    mocad_log(ctx, L_ERR, "line %d \n", __LINE__);
    EC_KEY_free(my_key);
    EC_KEY_free(peer_key);
    OPENSSL_free(dhkey);
    HMAC_CTX_cleanup(&hmac_ctx);
    return;
  }
  HMAC_Update(&hmac_ctx, nc_key+1, key_len); // +1 to skip the tag

  HMAC_Final(&hmac_ctx, output, &outputlen);

  MPS_DEBUG_PRINTKEY("Full MPSkey", output, outputlen);

  // Only the 16bytes of LSB are used:
  memcpy(mpskey, output+outputlen-16, 16);

  // END Create MPS Key

  HMAC_CTX_cleanup(&hmac_ctx);
  OPENSSL_free(nn_key);
  OPENSSL_free(nc_key);

  EC_KEY_free(my_key);
  EC_KEY_free(peer_key);
  if (dhkey)
     OPENSSL_free(dhkey);


#elif defined(__EMU_HOST_20__) && !defined(__LINUX__)
  //XX if defined(MOCAD_MPS_USE_TEST_VECTORS)

  int nodeid = atoi(ctx->ifname);
  int keyid = nodeid % 2;
  int peers_keyid = !nodeid;
#ifdef MOCAD_MPS_USE_TEST_VECTORS
  unsigned char peers_public_key_shouldbe[32];

  keyStr2Bin(peers_public_key_shouldbe, 32, peers_keyid ? pubstr1 : pubstr2);

  if (memcmp(peer_public_key, peers_public_key_shouldbe, 32) == 0) {
    MPS_DEBUG("Peers Pub Key Test vec matches: PASS!\n");
  }
  else {
    MPS_DEBUG("Peers Pub Key Test vec mismatch: FAIL!\n");
  }

  // Fake the computation since we don't have a crypto library
  keyStr2Bin(mpskey, 16, mpskeystr);
  MPS_DEBUG_PRINTKEY("Substituted test vector MPSKey", mpskey, 16);
#endif

#endif
}

#if defined(STANDALONE) && (defined(MOCAD_MPS_USE_OPENSSL) || defined(MOCAD_MPS_USE_OPENSSL_LOW))

typedef void (* CLI_Work )(unsigned int param);
extern void CLICMD_DoWork(CLI_Work work, unsigned int param, int wait);

#ifdef TIME_MPSKEY_COMPUTE
unsigned int mpskey_compute_started_sec;
unsigned int mpskey_compute_started_usec;
#endif

struct mocad_mps_compute_mpskey_data {
   struct mocad_ctx *mmcmd_ctx;
   struct moca_mps_request_mpskey mmcmd_mpskeyreq;
} mpskey_compute_data;

// Compute MPS Key in clic task
MOCAD_S void mocad_mps_compute_mpskey(unsigned int param) {
  struct mocad_ctx *ctx = mpskey_compute_data.mmcmd_ctx;
  struct moca_mps_request_mpskey *mpskeyreq = &mpskey_compute_data.mmcmd_mpskeyreq;
  struct moca_mps_key mpskey;
  int is_nn = mpskeyreq->is_nn;
  unsigned char *nn_guid = &mpskeyreq->nn_guid[0];

#ifdef TIME_MPSKEY_COMPUTE
  MoCAOS_GetTimeOfDay(&mpskey_compute_started_sec, &mpskey_compute_started_usec);
  xprintf("Start computing MPS key time %u.%06u...\n", mpskey_compute_started_sec, mpskey_compute_started_usec);
#endif

  mocad_mps_key_compute(ctx, &mpskeyreq->public_key[0], is_nn, nn_guid, &mpskey.key[0]);

#ifndef TIME_MPSKEY_COMPUTE
  xprintf("Finished computing MPS key\n");
#else
  unsigned int now_sec;
  unsigned int now_usec;
  MoCAOS_GetTimeOfDay(&now_sec, &now_usec);
  int diff_sec = now_sec - mpskey_compute_started_sec;
  int diff_usec = now_usec - mpskey_compute_started_usec; 
  if (diff_usec < 0) {
     diff_sec--;
     diff_usec = diff_usec + 1000000;
  }
  xprintf("Finished computing MPS key: time %u.%06u, took %u.%06u sec\n", now_sec, now_usec, diff_sec, diff_usec);
#endif
  {
     void *h = moca_open(ctx->ifname);
     if (__moca_set_mps_key(h, &mpskey)) {
        xprintf("ERROR: Failed to set MPS Key\n");
     }
     moca_close(h);
  }
}

MOCAD_S void mocad_handle_mps_request_mpskey(struct mocad_ctx *ctx, struct moca_mps_request_mpskey *mpskeyreq) {
  mocad_log(ctx, L_INFO, "Delay computing MPS key...\n");
  mpskey_compute_data.mmcmd_ctx = ctx;
  memcpy(&mpskey_compute_data.mmcmd_mpskeyreq, mpskeyreq, sizeof(*mpskeyreq));
  // Invoke clic task to do the computation
  CLICMD_DoWork(mocad_mps_compute_mpskey, (unsigned int) &mpskey_compute_data, 0);
}

#else // !STANDALONE
MOCAD_S void mocad_handle_mps_request_mpskey(struct mocad_ctx *ctx, struct moca_mps_request_mpskey *mpskeyreq) {
  struct moca_mps_key mpskey;
  int is_nn = mpskeyreq->is_nn;
  unsigned char *nn_guid = &mpskeyreq->nn_guid[0];

  MPS_DEBUG("MPS Request MPSKey Received as %s\n", is_nn ? "NN" : "NC");
  MPS_DEBUG_PRINTKEY("NN GUID", nn_guid, sizeof(mpskeyreq->nn_guid));
  //  MPS_DEBUG_PRINTKEY("Peer Public Key", &mpskeyreq->public_key[0], sizeof(mpskeyreq->public_key));

#if defined(MOCAD_MPS_USE_TEST_VECTORS) && !defined(__EMU_HOST_20__)
  int keyid = MOCAD_MPS_USE_TEST_VECTORS; // Set with EXTRA_CFLAGS=-DMOCAD_MPS_USE_TEST_VECTORS=0/1
  char *mypubstr;
  char *myprivstr;
  char *peerpubstr;
  // Fake NN GUID to ensure we have the same input parameters that produce our expected output.
  unsigned char nn_guid_v[8] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };

  nn_guid = nn_guid_v;

  if (keyid == 0) {
    mypubstr = pubstr1;
    myprivstr = privstr1;
    peerpubstr = pubstr2;
  }
  else {
    mypubstr = pubstr2;
    myprivstr = privstr2;
    peerpubstr = pubstr1;
  }

  keyStr2Bin(&mpskeyreq->public_key[0], sizeof(mpskeyreq->public_key), peerpubstr);
  MPS_DEBUG_PRINTKEY("Substituted test vector Peer key", &mpskeyreq->public_key[0], sizeof(mpskeyreq->public_key));

  keyStr2Bin(&ctx->pub_key[1], sizeof(ctx->pub_key)-1, mypubstr);  // +1 to skip the tag
  ctx->pub_key[0] = 0x04;
  MPS_DEBUG_PRINTKEY("Substituted test vector My key", &ctx->pub_key[0], sizeof(ctx->pub_key));

#if  defined(MOCAD_MPS_USE_OPENSSL) || defined(MOCAD_MPS_USE_OPENSSL_LOW)
  void *old_priv_key = ctx->priv_key;
  unsigned char privkey[32];
  keyStr2Bin(privkey, sizeof(privkey), myprivstr);
  MPS_DEBUG_PRINTKEY("Substituted test vector Priv key", privkey, sizeof(privkey));
  ctx->priv_key = BN_bin2bn(privkey, 32, NULL);
#endif
#endif

  mocad_mps_key_compute(ctx, &mpskeyreq->public_key[0], is_nn, nn_guid, &mpskey.key[0]);

  if (__moca_set_mps_key(ctx, &mpskey)) {
    mocad_log(ctx, L_ERR, "Failed to set MPS Key\n");
  }

#if defined(MOCAD_MPS_USE_TEST_VECTORS) && !defined(__EMU_HOST_20__) && (defined(MOCAD_MPS_USE_OPENSSL) || defined(MOCAD_MPS_USE_OPENSSL_LOW))
  ctx->priv_key = old_priv_key;

  unsigned char mpskey_testvec[16];
  keyStr2Bin(mpskey_testvec, sizeof(mpskey_testvec), mpskeystr);
  if (memcmp(mpskey_testvec, &mpskey.key[0], 16) == 0) {
    MPS_DEBUG("MPSKey Test vec matches: PASS!\n");
  }
  else {
    MPS_DEBUG("MPSKey Test vec mismatch: FAIL!\n");
  }
#endif

}
#endif

MOCAD_S int mocad_mps_gen_keys(struct mocad_ctx *ctx, unsigned char *pub_key, const PRIV_KEY_TYPE **priv_key) {
#ifdef MOCAD_MPS_USE_OPENSSL
  int ret = -1;
  EVP_PKEY_CTX *param_ctx = NULL;
  EVP_PKEY *params = NULL;
  EVP_PKEY_CTX *key_ctx = NULL;
  EVP_PKEY *my_pkey = NULL;

  param_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
  if (param_ctx == NULL) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_CTX_new_id\n");
    goto cleanup;
  }

  if (EVP_PKEY_paramgen_init(param_ctx) != 1) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_paramgen_init line\n");
    goto cleanup;
  }

  if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(param_ctx, NID_X9_62_prime256v1) != 1) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_CTX_set_ec_paramgen_curve_nid\n");
    goto cleanup;
  }

  if (EVP_PKEY_paramgen(param_ctx, &params) != 1) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_paramgen\n");
    goto cleanup;
  }

  key_ctx = EVP_PKEY_CTX_new(params, NULL);
  if (key_ctx == NULL) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_CTX_new\n");
    goto cleanup;
  }

  if (EVP_PKEY_keygen_init(key_ctx) != 1) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_keygen_init\n");
    goto cleanup;
  }

  ret = EVP_PKEY_keygen(key_ctx, &my_pkey);
  if (ret != 1) {
    mocad_log(ctx, L_ERR, "EVP_PKEY_keygen ret %d\n", ret);
    ret = -1;
    goto cleanup;
  }

  EC_KEY *ec_key = EVP_PKEY_get1_EC_KEY(my_pkey);
  unsigned char *nn_key = NULL; // XX Bad name to have "NN" in it
  if (i2o_ECPublicKey(ec_key, &nn_key) == 0) {
     mocad_log(ctx, L_ERR, "i2o_ECPublicKey ec_key %p nn_key %p\n", ec_key, nn_key);
     goto cleanup;
  }
  *priv_key = EC_KEY_get0_private_key(ec_key);

#if MPS_DEBUG_EN
  printf("Print EC_KEY:\n");
  EC_KEY_print_fp(stdout, ec_key, 0);
#endif
  if (ctx->key_ref != NULL)
     EC_KEY_free(ctx->key_ref);
  ctx->key_ref = ec_key;

  // the entire key, tag plus both x and y-coordinate
  //  MPS_DEBUG("pub_key %p nn_key %p len %d\n", pub_key, nn_key, MPS_PUB_KEYLEN);
  memcpy(pub_key, nn_key, MPS_PUB_KEYLEN);
  ret = 0;
  OPENSSL_free(nn_key);

 cleanup:
  if (param_ctx)
     EVP_PKEY_CTX_free(param_ctx);
  if (params)
     EVP_PKEY_free(params);
  if (key_ctx)
     EVP_PKEY_CTX_free(key_ctx);
  if (my_pkey)
     EVP_PKEY_free(my_pkey);
  return ret;

#elif defined(MOCAD_MPS_USE_OPENSSL_LOW)
//MOCAD_S int mocad_mps_gen_keys(struct mocad_ctx *ctx, unsigned char *pub_key, const PRIV_KEY_TYPE **priv_key) {
  int ret;
  EC_KEY *ec_key = NULL;
  unsigned char *key = NULL;
  int key_len;

  if (ctx->key_ref != NULL) {
     EC_KEY_free(ctx->key_ref);
     ctx->key_ref = NULL;
  }

  ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if (ec_key == NULL) {
     mocad_log(ctx, L_ERR, "EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)\n");
     return -1;
  }

  // one-shot generation of EC key-pair
  ret= EC_KEY_generate_key(ec_key);
  if (ret != 1) {
     mocad_log(ctx, L_ERR, "EC_KEY_generate_key ret %d\n", ret);
     EC_KEY_free(ec_key);
     return -2;
  }

  *priv_key = EC_KEY_get0_private_key(ec_key);

  ret = i2o_ECPublicKey(ec_key, &key);
  if (ret == 0) {
     mocad_log(ctx, L_ERR, "i2o_ECPublicKey ret %d\n", ret);
     EC_KEY_free(ec_key);
     return -3;
  }
  // the entire key, tag plus both x and y-coordinate
  key_len = MPS_PUB_KEYLEN;
  //EC_KEY_print_fp(stdout, ec_key, 0);
  memcpy(pub_key, key, key_len);
  //  MPS_DEBUG("pub_key %p nn_key %p len %d\n", pub_key, nn_key, MPS_PUB_KEYLEN);

  ctx->key_ref = ec_key;
  OPENSSL_free(key);
  return 0;

#elif defined(__EMU_HOST_20__) && !defined(__LINUX__) && defined(MOCAD_MPS_USE_TEST_VECTORS)

  //XX if defined(MOCAD_MPS_USE_TEST_VECTORS)

  int nodeid = atoi(ctx->ifname);
  int keyid = nodeid % 2;

  MPS_DEBUG("Gen key: I'm node %d (%s)\n", nodeid, ctx->ifname);

  keyStr2Bin(&private_key.key[0], sizeof (private_key.key), keyid ? privstr1 : privstr2);
  *priv_key = &private_key;
  keyStr2Bin(pub_key+1, MPS_PUB_KEYLEN-1, keyid ? pubstr1 : pubstr2); // +1 to skip the tag
  *pub_key = 0x04;

  MPS_DEBUG("I get key %d:\n", keyid);
  MPS_DEBUG_PRINTKEY("Private", private_key.key, sizeof(private_key.key));
  MPS_DEBUG_PRINTKEY("Public", pub_key+1, MPS_PUB_KEYLEN-1); // +1 to skip the tag

  return 0;
#else
  return -1;
#endif
}

MOCAD_S void mocad_mps_key_hash(struct mocad_ctx *ctx, unsigned char *hash, unsigned char *key, unsigned char *guid) {
#if defined(MOCAD_MPS_USE_OPENSSL) || defined(MOCAD_MPS_USE_OPENSSL_LOW)
  // 128bit MSB of SHA-256 ( Public Key || GUID of the NN )
  unsigned char output[SHA256_DIGEST_LENGTH];

  SHA256_CTX sha256_ctx;
  SHA256_Init(&sha256_ctx);

  SHA256_Update(&sha256_ctx, key, 32);
  SHA256_Update(&sha256_ctx, guid, 8);

  SHA256_Final(output, &sha256_ctx);

  //  MPS_DEBUG_PRINTKEY("Full Hash", output, SHA256_DIGEST_LENGTH);
  memcpy(hash, output, 16);

#elif defined(__EMU_HOST_20__) && !defined(__LINUX__) && defined(MOCAD_MPS_USE_TEST_VECTORS)

  //XX if defined(MOCAD_MPS_USE_TEST_VECTORS)

  int nodeid = atoi(ctx->ifname);

  MPS_DEBUG("Hash pub key: I'm node %d (%s)\n", nodeid, ctx->ifname);

  if (nodeid >= hashstrsz) {
     MPS_DEBUG("Not enough precomputed hashes for this nodeid %d, MPS will not work as expected!\n", nodeid);
     memset(hash, 0, 16);
     hash[0] = nodeid;
  }
  else {
     keyStr2Bin(&hash[0], 16, hashstr[nodeid]);
     MPS_DEBUG_PRINTKEY("HASH", hash, 16);
  }
#else

  // XX not implemented

#endif
}

MOCAD_S void mocad_mps_init(struct mocad_ctx *ctx) {
#if (MPS_EN == 1)
   MPS_DEBUG2("mocad_mps_init mps_state %d\n", ctx->mps_state);

# if !defined(MOCAD_MPS_USE_OPENSSL) && !defined(MOCAD_MPS_USE_OPENSSL_LOW)
   mocad_log(ctx, L_WARN, "No MPS Crypto - MPS Unsupported!\n");
#  if !defined(MOCAD_MPS_USE_TEST_VECTORS)
   ctx->any_time.mps_en = 0;
#  endif

# elif defined(STANDALONE)
   if (ctx->config_flags & MOCA_6802B0_FLAG) {
      mocad_log(ctx, L_WARN, "6802/3 B0 chip do not support MPS!\n");
      ctx->any_time.mps_en = 0;
   }
# endif

   // Inform FW of Paired state which came from persisted MPS data
   if (__moca_set_mps_state(ctx, (ctx->mps_state == 1) ? 1 : 0)) {
      mocad_log(ctx, L_ERR, "Unable to set MPS State (%d) in FW\n", ctx->mps_state, (ctx->mps_state == 1) ? 1 : 0);
   }
   ctx->any_time.mps_state = (ctx->mps_state == 1) ? 1 : 0;

   mocad_mps_new_session(ctx);
#else
   mocad_log(ctx, L_WARN, "MPS unsupported in this build!\n");
   ctx->any_time.mps_en = 0;
#endif
}

#ifdef STANDALONE

// Compute Pub Key pair in clic task
MOCAD_S void mocad_mps_delayed_gen_keys_cb(unsigned int param) {
  // we can only touch pub_key and priv_key from here!
  struct mocad_ctx *ctx = (struct mocad_ctx *) param;
  unsigned char *pub_key = &ctx->pub_key[0];

  xprintf("MPS Session Init: Delayed New MPS Session, creating new keys\n");

  if (mocad_mps_gen_keys(ctx, pub_key, (const PRIV_KEY_TYPE **)&ctx->priv_key)) {
    xprintf("Unable to generate MPS Public key pair!\n");
    return;
  }

  // Send IE back to mocad task to finalize...
  // Use the clic task's handle since we are running in the clic task.
  xprintf("MPS Session Init: Delayed key creation done!\n");
  {
     void *h = moca_open(ctx->ifname);
     __moca_set_mps_data_ready(h);
     moca_close(h);
  }
}

// Back in mocad task
void mocad_handle_mps_data_ready(struct mocad_ctx *ctx) {
  unsigned char *pub_key = &ctx->pub_key[0];
  MPS_SESSION_LOG("MPS Session Init: Delayed New MPS Session returned to mocad, set MPS data, ready for new MPS Session\n");
  mocad_mps_set_mpsdata(ctx, pub_key);
}

#endif

MOCAD_S void mocad_mps_new_session(struct mocad_ctx *ctx) {
#ifndef STANDALONE
  unsigned char *pub_key = &ctx->pub_key[0];
#else
  if (ctx->config_flags & MOCA_6802B0_FLAG) {
     return;
  }
#endif

  MPS_DEBUG("MPS Session: New MPS Session, new keys\n");

  ctx->mps_init_scan_tot_nets = 0;
  ctx->mps_init_scan_trig_nets = 0;

#ifndef STANDALONE
  if (mocad_mps_gen_keys(ctx, pub_key, (const PRIV_KEY_TYPE **)&ctx->priv_key)) {
    mocad_log(ctx, L_ERR, "Unable to generate MPS Public key pair!\n");
    return;
  }

  mocad_mps_set_mpsdata(ctx, pub_key);
#elif defined(MOCAD_MPS_USE_OPENSSL) || defined(MOCAD_MPS_USE_OPENSSL_LOW)
  MPS_SESSION_LOG("MPS Session Init: Start generating MPS Public key pair...\n");
  // Invoke clic task to do the computation
  CLICMD_DoWork(mocad_mps_delayed_gen_keys_cb, (unsigned int) ctx, 0);
#endif

  MPS_DEBUG("Stop MPS Timer!\n");
  mocad_timer_remove(ctx, &ctx->timer_head, &ctx->mps_timer);
}

MOCAD_S void mocad_mps_set_mpsdata(struct mocad_ctx *ctx, unsigned char *pub_key) {
  struct moca_mps_data mpsdata;

  mpsdata.valid = 1;

  // MPS_DEBUG_PRINTKEY("Full public key", pub_key, sizeof(ctx->pub_key));
  MPS_DEBUG_PRINTKEY("public key x-coordinate", pub_key+1, sizeof(mpsdata.public_key));
  memcpy(&mpsdata.public_key[0], pub_key+1, sizeof(mpsdata.public_key)); // +1 for the format tag

  mocad_mps_rehash_pubkey_guid(ctx, &mpsdata);

  if (__moca_set_mps_data(ctx, &mpsdata)) {
     mocad_log(ctx, L_ERR, "Failed to set mps_data\n");
     mocad_log(ctx, L_ERR, "MPS likely unsupported in this FW\n");
  }
}

MOCAD_S void mocad_mps_rehash_pubkey_guid(struct mocad_ctx *ctx, struct moca_mps_data *mpsdata) {
  unsigned char *pub_key = &ctx->pub_key[0];
  unsigned char guid[8];

  guid[6] = 0; guid[7] = 0;
  memcpy(&guid[0], &ctx->any_time.mac_addr.val.addr[0], sizeof(ctx->any_time.mac_addr.val.addr));

  mocad_mps_key_hash(ctx, &mpsdata->public_key_hash[0], pub_key+1, guid); // +1 for the format tag

  MPS_DEBUG_PRINTKEY("Public key Hash", &mpsdata->public_key_hash[0], sizeof(mpsdata->public_key_hash));
}

/* 
 * Handle persistent MPS data: MPS State (paired/unpaired), Password and Privacy
 */

MOCAD_S void mocad_clear_mps_data(struct mocad_ctx *ctx)
{
   // Clear persistent MPS data State (paired/unpaired), Password and Privacy
   mocad_write_mps_data(ctx, 0, -1, NULL, 0);
}

MOCAD_S void mocad_reread_mps_data(struct mocad_ctx *ctx)
{
   unsigned char password[17+1];
   char privacy_en;
   unsigned char mps_state = 0;
   mocad_read_mps_data(ctx, &mps_state, &privacy_en, 
                       (char *) password, sizeof(password));
   ctx->mps_state = mps_state;
   ctx->any_time.mps_state = (mps_state == 1) ? 1 : 0;
   if (ctx->mps_state == 1) {
      ctx->any_time.privacy_en = privacy_en;
      mocad_set_anytime_network_password(ctx, password);
   }
   else { // mps_state 0 or 2 - use defaults
      mocad_set_privacy_defaults(ctx);
   }
}

// Persisting and reading MPS data
#if defined(DSL_MOCA) /* DSL Code */

/* On writes password 'len' is based on sizeof(struct moca_password.password) which (inexplicably) is 32 bytes
 * The real password is 18 bytes (17 ASCII chars + null-terminator), add 2 bytes for paired and privacy_en
 * Also, cmsPsp_set() will grow/shrink to match the length written. 
 * Always provide the same length:
 * in cmsPsp_get() operations so that read operations will be guaranteed to always succeed,
 * in cmsPsp_set() operations so that we don't risk failing grow operations or fragmenting the scratch area when shrinking.
 */
#define MOCAD_MPSDATA_PSP_SIZE (18+2)

int mocad_read_mps_data(struct mocad_ctx *ctx, unsigned char *paired, char *privacy_en,
                        char *password, int len)
{
   char filename[MOCA_FILENAME_LEN];
   int retlen;
   char buf[MOCAD_MPSDATA_PSP_SIZE];

   sprintf(filename, "MPSDATA%s", ctx->ifname);

   retlen = cmsPsp_get(filename, buf, sizeof(buf));
   if (retlen <= 0)
      goto bad;
   if (len < retlen - 2)
      goto bad;

   *paired = buf[0];
   // Only use the rest if Paired
   if (*paired == 1) {
      *privacy_en = buf[1];
      memcpy(password, buf + 2, retlen - 2);
   }
   mocad_log(ctx, L_INFO, "Found valid MPSDATA: Paired %u Privacy %d Password %d bytes\n",
             *paired, 
             *paired == 1 ? *privacy_en : -1,
             *paired == 1 ? len : -1);
   MPS_DEBUG2("Read new MPS Data Paired %u Privacy %d Password %s (%d bytes)\n", 
             *paired, 
             *paired == 1 ? *privacy_en : -1, 
             *paired == 1 ? password : "N/A", 
             len);

   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read MPSDATA file '%s' (%d)\n", filename, retlen);
   return(-1);
}

int mocad_write_mps_data(struct mocad_ctx *ctx, unsigned char paired, char privacy_en,
                         const char *password, int len)
{
   char filename[MOCA_FILENAME_LEN];
   CmsRet ret;
   char buf[MOCAD_MPSDATA_PSP_SIZE];

   mocad_log(ctx, L_INFO, "Saving new MPSDATA: Paired %u Privacy %d Password %d bytes\n", 
             paired, paired == 1 ? privacy_en : -1, paired == 1 ? len : -1);
   MPS_DEBUG2("Saving new MPS Data Paired %u Privacy %d Password %s (%d bytes)\n", 
             paired, paired == 1 ? privacy_en : -1, paired == 1 ? password : "N/A", len);

   sprintf(filename, "MPSDATA%s", ctx->ifname);

   buf[0] = paired;
   buf[1] = privacy_en;
   // sometimes we're called with NULL password when clearing MPS state
   if (password != NULL && len >= sizeof(buf) - 2)
      memcpy(buf + 2, password, sizeof(buf) - 2);
   else
      memset(buf + 2, 0, sizeof(buf) - 2);

   ret = cmsPsp_set(filename, buf, sizeof(buf));
   if (ret != CMSRET_SUCCESS)
      goto bad;

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write MPSDATA file '%s' (%d)\n", filename, ret);
   return(-1);
}

#elif defined(STANDALONE)
#include "nvram.h"

struct nvram_mps_data {
   unsigned char nmd_paired;
   unsigned char nmd_privacy_en;
   unsigned char nmd_reserved[2];
   unsigned char nmd_password[64]; // Future versions will require 64 char passwords
};

// Can't run out of flash when reading flash (NVRAM) :-)
int mocad_read_mps_data(struct mocad_ctx *ctx, unsigned char *paired, char *privacy_en,
                        char *password, int len)
{
   unsigned int x;
   struct nvram_mps_data *nmd;
   struct nvram_mps_data nmdcopy;
   unsigned int readlen;

   x=NVRAM_Lock();

   nmd = (struct nvram_mps_data *) NVRAM_GetItem(NVRAM_MPS_DATA, &readlen);

   if (!nmd || (readlen != sizeof(*nmd)))
   {
      NVRAM_Unlock(x);
      /* not a serious failure; it will be missing before first pairing */
      mocad_log(ctx, L_INFO, "No MPS Data in NVRAM\n");
      return -1;
   }
   memcpy(&nmdcopy, nmd, sizeof(nmdcopy));
   NVRAM_Unlock(x);

   *paired = nmdcopy.nmd_paired;
   if (*paired == 1) {
      *privacy_en = nmdcopy.nmd_privacy_en;
      memcpy(password, &nmdcopy.nmd_password[0], len);
   }

   mocad_log(ctx, L_INFO, "Found valid MPS Data file: Paired %u Privacy %d Password %d bytes\n",
             *paired, 
             *paired == 1 ? *privacy_en : -1,
             *paired == 1 ? len : -1);
   MPS_DEBUG2("Read new MPS Data Paired %u Privacy %d Password %s (%d bytes)\n", 
             *paired, 
             *paired == 1 ? *privacy_en : -1, 
             *paired == 1 ? password : "N/A", 
             len);
  
   return 0; 
}

int mocad_write_mps_data(struct mocad_ctx *ctx, unsigned char paired, char privacy_en,
                         const char *password, int len)
{
   unsigned int x;
   struct nvram_mps_data nmd;

   mocad_log(ctx, L_INFO, "Saving new MPS Data: Paired %u Privacy %d Password %d bytes\n", 
             paired, paired == 1 ? privacy_en : -1, paired == 1 ? len : -1);
   MPS_DEBUG2("Saving new MPS Data Paired %u Privacy %d Password %s (%d bytes)\n", 
             paired, paired == 1 ? privacy_en : -1, paired == 1 ? password : "N/A", len);

   memset(&nmd, 0, sizeof(nmd));
   nmd.nmd_paired = paired;
   nmd.nmd_privacy_en = privacy_en;
   // sometimes we're called with NULL password when clearing MPS state
   if (password != NULL)
      memcpy(&nmd.nmd_password[0], password, len);

   x=NVRAM_Lock();
   NVRAM_SetItem(NVRAM_MPS_DATA, (unsigned int *)&nmd, sizeof(nmd));
   NVRAM_Unlock(x);
   
   return 0;
}

#else // Linux and Win32 and others

/* Data format:
 * [1 byte Paired] [1 byte Privacy] [x bytes Password]
 * where x is determined by caller, should be sizeof() the password data structure
 */ 

int mocad_read_mps_data(struct mocad_ctx *ctx, unsigned char *paired, char *privacy_en,
                        char *password, int len)
{
   int fd;
   char filename[MOCA_FILENAME_LEN];

   sprintf(filename, MPS_DATA_PATH_FMT, ctx->ifname);
   fd = open(filename, O_RDONLY);
   if(fd < 0)
      goto bad;

   mocad_log(ctx, L_INFO, "Opened MPS Data file\n");

   if (read(fd, paired, 1) < 1)
      goto bad;
   // Only read the rest if Paired
   if (*paired == 1
       && (read(fd, privacy_en, 1) < 1
           || read(fd, password, len) < len))
      goto bad;

   mocad_log(ctx, L_INFO, "Found valid MPS Data file: Paired %u Privacy %d Password %d bytes\n",
             *paired, 
             *paired == 1 ? *privacy_en : -1,
             *paired == 1 ? len : -1);
   MPS_DEBUG2("Read new MPS Data Paired %u Privacy %d Password %s (%d bytes)\n", 
             *paired, 
             *paired == 1 ? *privacy_en : -1, 
             *paired == 1 ? password : "N/A", 
             len);

   close(fd);

   return(0);

bad:
   /* not a serious failure; it will be missing on the first run */
   mocad_log(ctx, L_VERBOSE, "can't read MPS Data file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

int mocad_write_mps_data(struct mocad_ctx *ctx, unsigned char paired, char privacy_en,
                         const char *password, int len)
{
   int fd;
   char filename[MOCA_FILENAME_LEN];

   sprintf(filename, MPS_DATA_PATH_FMT, ctx->ifname);
   fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
   if(fd < 0)
      goto bad;

   mocad_log(ctx, L_INFO, "Saving new MPS Data: Paired %u Privacy %d Password %d bytes\n", 
             paired, paired == 1 ? privacy_en : -1, paired == 1 ? len : -1);
   MPS_DEBUG2("Saving new MPS Data Paired %u Privacy %d Password %s (%d bytes)\n", 
             paired, paired == 1 ? privacy_en : -1, paired == 1 ? password : "N/A", len);
   if (write(fd, &paired, 1) != 1)
      goto bad;
   // Only write the rest if Paired
   if (paired == 1
       && (write(fd, &privacy_en, 1) != 1
           || write(fd, password, len) != len))
      goto bad;

   close(fd);

   return(0);

bad:
   mocad_log(ctx, L_WARN, "can't write MPS Data file '%s'\n", filename);
   if(fd >= 0)
      close(fd);
   return(-1);
}

#endif

