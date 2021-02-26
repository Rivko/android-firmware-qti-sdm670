#ifndef __VSS_IHDVOICE_PRIVATE_IF_H__
#define __VSS_IHDVOICE_PRIVATE_IF_H__

/**
  @file vss_ihdvoice_public_if.h
  @brief This file contains the API definitions for HD Voice features.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the 
      vss_ihdvoice_public_if group description in the CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
   Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include "apr_comdef.h"

/** @addtogroup vss_ihdvoice_private_if
@{ */

/****************************************************************************
 * HD Voice Private APIS                                    *
 ****************************************************************************/

/**
  Command issued by the MVM to attached vocprocs, to obtain the feature, enable mode,
  and Rx sampling rate, for a given system configuration, if it exists.

  @par Payload
  #vss_ihdvoice_cmd_get_config_t

  @return
  #APRV2_IBASIC_RSP_RESULT
 
  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_GET_CONFIG ( 0x000130AA )

/** @brief Type definition for #vss_icommon_cmd_get_hdvoice_config_t.
*/
typedef struct vss_ihdvoice_cmd_get_config_t vss_ihdvoice_cmd_get_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IHDVOICE_CMD_GET_CONFIG command.
*/
struct vss_ihdvoice_cmd_get_config_t
{
  uint32_t network_id;
    /** Network ID. Supported values: \n
        see @latexonly \hyperref[cal_network_ID]{Calibration Network IDs}
        @endlatexonly. */
  uint32_t media_id;
    /** Cal media ID requested by the client. Supported values: \n
        see @latexonly \hyperref[media_ID]{Media Type IDs}@endlatexonly. */
  uint32_t rx_voc_op_mode;
    /** Rx vocoder operating mode. Supported values: \n
        see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
        Modes}@endlatexonly. */
  uint32_t tx_voc_op_mode;
    /** Tx vocoder operating mode. Supported values: \n
        see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
        Modes}@endlatexonly. */
  uint32_t rx_pp_sr;
    /** RX post-processing blocks sampling rate. */
  uint32_t tx_pp_sr;
    /** TX pre-processing blocks sampling rate. */
  uint32_t feature_id;
    /** Calibration feature. Supported values: \n
        see @latexonly \hyperref[cal_feature]{Calibration Features}
        @endlatexonly). */
}
#include "apr_pack_end.h"
;

/**
  Response to the #VSS_IHDVOICE_RSP_GET_CONFIG command.

  @par Payload
  #vss_ihdvoice_rsp_get_config_t

  @return
  None.

  @dependencies
  None.
*/

#define VSS_IHDVOICE_RSP_GET_CONFIG ( 0x000130AB )

/** @brief Type definition for #vss_ihdvoice_rsp_get_config_t.
*/
typedef struct vss_ihdvoice_rsp_get_config_t vss_ihdvoice_rsp_get_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IHDVOICE_RSP_GET_CONFIG command.
*/
struct vss_ihdvoice_rsp_get_config_t
{
  uint32_t feature_id;
    /** Calibration feature. Supported values: \n
        see @latexonly \hyperref[cal_feature]{Calibration Features}
        @endlatexonly). */
  uint32_t enable_mode;
    /**< The feature enablement mode. Supported values: \n
         - 0 –- VSS_PARAM_FEATURE_DEFAULT_OFF \n
         - 1 –- VSS_PARAM_FEATURE_DEFAULT_ON \n
         - 2 –- VSS_PARAM_FEATURE_FORCED_OFF \n
         - 3 –- VSS_PARAM_FEATURE_FORCED_ON */
  uint32_t rx_pp_sr;
    /** RX post-processing blocks sampling rate. */
  uint32_t tx_pp_sr;
    /** TX post-processing blocks sampling rate. */

}
#include "apr_pack_end.h"
;

/**
  Command issued by the MVM, to set the HD Voice configuration on the stream
  to enable or disable a HD Voice feature

  @par Payload
  #vss_ihdvoice_cmd_set_config_t

  @return
  #APRV2_IBASIC_RSP_RESULT
 
  @dependencies
  None.
*/
#define VSS_IHDVOICE_CMD_SET_CONFIG ( 0x000130AC )

/** @brief Type definition for #vss_ihdvoice_cmd_set_config_t.
*/
typedef struct vss_ihdvoice_cmd_set_config_t vss_ihdvoice_cmd_set_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IHDVOICE_CMD_SET_CONFIG command.
*/
struct vss_ihdvoice_cmd_set_config_t
{
  uint32_t feature_id;
    /** Calibration feature. Supported values: \n
        see @latexonly \hyperref[cal_feature]{Calibration Features}
        @endlatexonly). */
}
#include "apr_pack_end.h"
;

#endif /* __VSS_IHDVOICE_PRIVATE_IF_H__ */

