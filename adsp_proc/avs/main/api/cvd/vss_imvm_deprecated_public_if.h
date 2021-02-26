#ifndef __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_imvm_deprecated_public_if.h
  @brief This file contains the deprecated APR API definitions for Multimode
  Manager (MVM) module.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2012, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_imvm_deprecated_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
03/13/12     AT    Initial version.

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup cvd_mvm_deprecated
@{ */

/****************************************************************************
 * MULTIMODE VOICE MANAGER CORE APIS                                        *
 ****************************************************************************/

/** Deprecated. Use #VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL.

  Sets modem control policy on the MVM. In Modem Control mode, the MVM state
  is controlled by both the modem and the applications. Modem control policy
  must be set on the MVM immediately after session creation and before starting
  voice.

  @apr_msgpayload{vss_imvm_cmd_set_modem_voice_control_t}
  @table{weak__vss__imvm__cmd__set__modem__voice__control__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_MODEM_VOICE_CONTROL ( 0x000112D8 )

/* Type definition for vss_imvm_cmd_set_modem_voice_control_t. */
typedef struct vss_imvm_cmd_set_modem_voice_control_t vss_imvm_cmd_set_modem_voice_control_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_set_modem_voice_control_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_SET_MODEM_VOICE_CONTROL command.
*/
struct vss_imvm_cmd_set_modem_voice_control_t
{
  bool_t enable_flag;
  /**< Specifies whether modem state machine control is enabled (TRUE). */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_set_modem_voice_control_t */
/** @} */ /* end_addtogroup cvd_mvm_deprecated */


#endif /* __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__ */

