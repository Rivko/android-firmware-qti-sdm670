#ifndef TZBSP_OEM_H
#define TZBSP_OEM_H

/**
 * Copyright 2015 QUALCOMM Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 */

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $DateTime: 2018/06/20 06:19:00 $
  $Author: pwbldsvc $

  when       who      what, where, why
  --------   ---      ------------------------------------
  10/12/15   rickarde Initial version

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdbool.h>
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct {
  uint32 wQseeFatalErrFlag;
  uint32 wOemFatalErrFlag;
} tzbsp_reset_reason_pair_t;

typedef struct {
  uint32 dwCount;
  tzbsp_reset_reason_pair_t *rst_reason;
} tzbsp_cfg_oem_reset_reason_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief Check if default application sandboxing is enabled
 *
 * @return true if sandbox by default is enabled, false otherwise
*/
bool oem_app_sandbox_default(void);


/**
 * Checks device config for property that can be used to bypass
 * meltdown security vulnerability mitigation on affected targets.
 *
 * @return true if meltdown mitigation should be bypassed
 */
bool oem_bypass_meltdown_mitigation(void);

/** Checks whether crash dump is enable before anti-rollback
 *  fuse is blown in DevCfg binary blob.
 *
 *  @retval true if allowed. false otherwise
 */
bool oem_is_dump_enable_before_arb(void);

/* Get activation and revocation list info from devcfg */
int tzbsp_oem_get_mrc_fuses(uint32* mrc_activation_list, uint32* mrc_revocation_list);

/* Updates MRC related fuses */
int tzbsp_oem_mrc_fuse_config(void);

/** Read the value for enable counter measure property
 *  from devcfg.
 *
 *  @retval false if disabled true otherwise.
 */
boolean oem_enable_counter_measure(void);

/* get public mod data (little endian) from devcfg */
int tzbsp_oem_get_pub_key(uint8** pub_mod_ptr, uint32* pub_mod_len, uint8** pub_exp_ptr, uint32* pub_exp_len);

/** Checks whether kdf fix is enable in DevCdf blob.
 *
 *  @retval true if enable. false otherwise
 */
bool oem_is_kdf_fix_enable(void);

int tzbsp_oem_get_decryption_key(uint8 *l1_key);

int tzbsp_oem_uie_encr_key1_selector(uint8 *oem_decryption_key_selector);

#endif // TZBSP_OEM_H
