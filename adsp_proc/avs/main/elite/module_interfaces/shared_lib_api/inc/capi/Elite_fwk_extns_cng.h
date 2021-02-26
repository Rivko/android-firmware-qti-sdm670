#ifndef ELITE_FWK_EXTNS_CNG_H
#define ELITE_FWK_EXTNS_CNG_H

/* ======================================================================== */
/**
@file Elite_fwk_extns_cng.h

@brief Framework extension header for comfort noise generator

  This file defines the parameters, events and other behaviors associated
  with vocoders
*/

/* =========================================================================
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
  ========================================================================== */

/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   07/16/16   sw      (Tech Comm) Merged Doxygen comments from 8953.2.7.
   09/28/15   ka      Initial Version.
   ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#include "Elite_CAPI_V2_types.h"

/** @addtogroup capiv2_ext_cng
@{ */

/** Unique identifier of the custom framework extension for the comfort noise
  generator (CNG).
 */
#define FWK_EXTN_CNG                              (0x00010E94)

/*------------------------------------------------------------------------------
 * Parameter IDs
 *----------------------------------------------------------------------------*/

/** ID of the parameter that indicates whether the Tx path is muted.

  @msgpayload{voc_param_cng_tx_mute_t}
  @table{weak__voc__param__cng__tx__mute__t}
 */
#define VOC_PARAM_CNG_TX_MUTE                     (0x00010E95)

typedef struct voc_param_cng_tx_mute_t voc_param_cng_tx_mute_t;

/** @weakgroup weak_voc_param_cng_tx_mute_t
@{ */
struct voc_param_cng_tx_mute_t
{
   uint16_t is_mute;
   /**< Indicates whether mute is enabled.

        @values
        - 0 -- Disabled
        - 1 -- Enabled @tablebulletend */
};
/** @} *//* end_weakgroup weak_voc_param_cng_tx_mute_t */


/** ID of the parameter that indicates whether TTY is detected on the Rx path.

  @msgpayload{voc_param_cng_rx_tty_detected_t}
  @table{weak__voc__param__cng__rx__tty__detected__t}
 */
#define VOC_PARAM_CNG_RX_TTY_DETECTED             (0x00010E96)

typedef struct voc_param_cng_rx_tty_detected_t voc_param_cng_rx_tty_detected_t;

/** @weakgroup weak_voc_param_cng_rx_tty_detected_t
@{ */
struct voc_param_cng_rx_tty_detected_t
{
   uint16_t is_detected;
   /**< Indicates whether TTY is detected.

        @values
        - 0 -- Not detected
        - 1 -- Detected @tablebulletend */
};
/** @} *//* end_weakgroup weak_voc_param_cng_rx_tty_detected_t */


/** ID of the parameter that indicates whether TTY mode is set to FULL.

  For the TTY modes, refer to vss_itty_cmd_set_tty_mode_t in 
  @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{voc_param_cng_is_tty_mode_full_t}
  @table{weak__voc__param__cng__is__tty__mode__full__t}
 */
/* vss_itty_cmd_set_tty_mode_t from CVD replaces vsm_tty_mode_t (adsp_vsm_api.h) */
#define VOC_PARAM_CNG_IS_TTY_MODE_FULL           (0x00010E97)

typedef struct voc_param_cng_is_tty_mode_full_t voc_param_cng_is_tty_mode_full_t;

/** @weakgroup weak_voc_param_cng_is_tty_mode_full_t
@{ */
struct voc_param_cng_is_tty_mode_full_t
{
   uint16_t is_tty_mode_full;
   /**< Indicates whether TTY mode is FULL.

        @values
        - 0 -- Not FULL
        - 1 -- FULL @tablebulletend */
};
/** @} *//* end_weakgroup weak_voc_param_cng_is_tty_mode_full_t */

/** @} *//* end_addtogroup capiv2_ext_cng */

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //ELITE_FWK_EXTNS_CNG_H
