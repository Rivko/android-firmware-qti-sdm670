#ifndef ELITE_INTF_EXTNS_TTY_H
#define ELITE_INTF_EXTNS_TTY_H

/* ======================================================================== */
/**
@file Elite_intf_extns_tty.h

@brief Interface extension header for vocoders or modules using TTY

  This file defines the parameters, events and other behaviors associated
  with vocoders or modules using TTY
*/

/* =========================================================================
  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All rights reserved.
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

/** @addtogroup capiv2_ext_tty
@{ */

/** Unique identifier of the custom interface extension for TTY. */
#define INTF_EXTN_TTY 0x00010E80

/*------------------------------------------------------------------------------
 * Parameter IDs
 *----------------------------------------------------------------------------*/

/** ID of the parameter that identifies the TTY mode being set.

  The VSS_ITTY_CMD_SET_TTY_MODE command provides the TTY mode, which is then
  passed to the module through this parameter ID. For more information on this
  command, refer to @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{tty_param_mode_t}
  @table{weak__tty__param__mode__t}
 */
#define TTY_PARAM_MODE               (0x00010E72)

typedef struct tty_param_mode_t tty_param_mode_t;

/** @weakgroup weak_tty_param_mode_t
@{ */
struct tty_param_mode_t
{
   uint32_t mode;
   /**< TTY mode to be set.

        @values See #tty_modes_t */
};
/** @} *//* end_weakgroup weak_tty_param_mode_t */

/** Valid TTY modes.
 */
typedef enum tty_modes_t
{
   TTY_MODE_OFF = 0,  /**< TTY OFF or disabled */
   TTY_MODE_HCO,      /**< Hearing carry over mode. */
   TTY_MODE_VCO,      /**< Voice carry over mode. */
   TTY_MODE_FULL      /**< FULL mode. */
}/** @cond */tty_modes_t/** @endcond */;

// The below parameters are specific to cellular text modem (CTM) block

/** ID of the parameter that indicates the CTM block must resynchronize.

  @msgpayload
  None
 */
#define TTY_PARAM_CTM_RESYNC                   (0x00010E73)

/** ID of the parameter that tells the CTM-TX module that the CTM-RX module
  detected an enquiry burst.

  @msgpayload
  None
 */
#define TTY_PARAM_CTM_RX_ENQUIRY_DETECTED      (0x00010E74)

/** ID of the parameter that tells the CTM-TX module that the CTM-RX module
  detected CTM character-related information from the far end.

  @msgpayload
  None
 */
#define TTY_PARAM_CTM_RX_DETECTED              (0x00010E75)

/** ID of the parameter that tells the CTM-RX module that the CTM-TX module
  transmitted a CTM character.

  @msgpayload{tty_char_t}
  @table{weak__tty__char__t}
 */
#define TTY_PARAM_CTM_TX_CHAR_TRANSMITTED     (0x00010E76)

/** ID of the parameter used to push character for out-of-band TTY (OOBTTY).

  @msgpayload{tty_char_t}
  Both CTM and OOBTTY can use this structure.
  @tablens{weak__tty__char__t}
 */
#define TTY_PARAM_CHAR       (0x00010E77)

/* Below structure can be used for both CTM and OOBTTY*/
typedef struct tty_char_t tty_char_t;

/** @weakgroup weak_tty_char_t
@{ */
struct tty_char_t
{
   uint32_t tty_char;   /**< Character in UTF-8 format. */
};
/** @} *//* end_weakgroup weak_tty_char_t */

/*------------------------------------------------------------------------------
 * Events
 *----------------------------------------------------------------------------*/

/** Event raised when TTY is detected in either of the following cases:
  - The Rx module raises the event in case of CTM/OOBTTY.
  - The decoder raises the event for 1x TTY when Rx TTY is detected and
    half-duplex muting on the Tx path is required.

  @msgpayload{rx_tty_detected_t}
  @table{weak__rx__tty__detected__t}
 */
#define TTY_EVT_RX_TTY_DETECTED            (0x00010E78)

typedef struct rx_tty_detected_t rx_tty_detected_t;

/** @weakgroup weak_rx_tty_detected_t
@{ */
struct rx_tty_detected_t
{
   uint32_t rx_tty_detected_flag;
   /**< Indicates whether TTY is detected on the Rx path.

        @values
        TRUE -- TTY is detected on the Rx path
        FALSE -- TTY is not detected on the Rx path (Default)
        @tablebulletend */
};
/** @} *//* end_weakgroup weak_rx_tty_detected_t */

// The below events are specific to CTM block

/** Indicates that an enquiry burst was detected.
  The CTM-RX module raises this event and sends it to the CTM-TX module.

  @msgpayload
  None
 */
#define TTY_EVT_CTM_RX_ENQUIRY_DETECTED    (0x00010E79)

/** Indicates that a CTM character was detected.
  The CTM-RX module raises this event and sends it to the CTM-TX module.

  @msgpayload
  None
 */
#define TTY_EVT_CTM_RX_DETECTED            (0x00010E7A)

/** Indicates that a character was transmitted on the Tx path.
  The CTM-TX module raises this event and sends it to the CTM-RX module.

  @msgpayload
  None
 */
#define TTY_EVT_CTM_CHAR_TRANSMITTED       (0x00010E7B)

/** Indicates that a character was detected.

  The OOBTTY Tx module raises this event on the Tx path. When a service
  receives this event, the service logs the character into the payload
  and sends the character to the upper CVS layer.

  The CTM-TX and CTM-RX modules raise this event on the Tx path and Rx paths,
  respectively. When a service receives this event, the service logs the
  character into the payload of the event.

  @msgpayload{tty_char_t}
  @table{weak__tty__char__t}
 */
#define TTY_EVT_CHAR_DETECTED              (0x00010E7C)

/** Indicates that a character pushed on the Rx path was accepted.
  The OOBTTY Rx module raises this event to the service.

  @msgpayload{tty_char_t}
  @table{weak__tty__char__t} @newpage
 */
#define TTY_EVT_LTETTY_RX_CHAR_ACCEPTED    (0x00010E7D)

/** @} *//* end_addtogroup capiv2_ext_tty */

#ifdef __cplusplus
}
#endif //__cplusplus
#endif
