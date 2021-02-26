#ifndef __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__
#define __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_params_deprecated_public_if.h
  @brief This file contains deprecated module and parameter IDs supported 
  by Core Voice Drivers.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014-2015, 2017 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/

/** @addtogroup cvd_widevoice_deprecated
@{ */

/****************************************************************************
 * MODULE ID DEFINES                                                        *
 ****************************************************************************/

/** Deprecated. Use #VSS_MODULE_FEATURE_CONFIG for configuring features like
  the Wide Voice module.

  Wide Voice module ID.
  This module ID uses one parameter.
*/
#define VSS_MODULE_WIDEVOICE ( 0x00011376 )


/****************************************************************************
 * PARAM ID DEFINES                                                         *
 ****************************************************************************/

/** Deprecated. Use #VSS_PARAM_HDVOICE_CONFIG.

  ID of the parameter used by VSS_MODULE_WIDEVOICE to set the Wide Voice Rx
  sampling rate. The parameter data is a 32-bit value in Hertz.
*/
#define VSS_PARAM_RX_SAMPLING_RATE ( 0x00011377 )

/** @} */ /* end_addtogroup cvd_widevoice_deprecated */

/** @ingroup cvd_common_deprecated
  Deprecated. Use #VSS_ICOMMON_CMD_SET_PARAM_V3.

  ID of the parameter used by #VSS_MODULE_CVD_GENERIC to set the number of
  device channels -- the number of microphones for a Tx session, and the number
  of speaker channels for an Rx session.

  Set this parameter using out-of-band memory in
  #VSS_IVOCPROC_CMD_TOPOLOGY_REGISTER_PARAMS.

  @msgpayload{vss_param_num_dev_channels_t}
  @table{weak__vss__param__num__dev__channels__t}
*/
#define VSS_PARAM_NUM_DEV_CHANNELS ( 0x0001316B )

/* Type definition for #vss_param_num_dev_channels_t. */
typedef struct vss_param_num_dev_channels_t vss_param_num_dev_channels_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_param_num_dev_channels_t
@{ */
/* Structure for specifying payload in #VSS_PARAM_NUM_DEV_CHANNELS.
*/
struct vss_param_num_dev_channels_t
{
   uint16_t tx_num_channels;
   /**< Number of microphones.

        @values
         - #VSS_NUM_DEV_CHANNELS_1
         - #VSS_NUM_DEV_CHANNELS_2
         - #VSS_NUM_DEV_CHANNELS_3
         - #VSS_NUM_DEV_CHANNELS_4
         - #VSS_NUM_DEV_CHANNELS_5
         - #VSS_NUM_DEV_CHANNELS_6
         - #VSS_NUM_DEV_CHANNELS_7
         - #VSS_NUM_DEV_CHANNELS_8 @tablebulletend */

   uint16_t rx_num_channels;
   /**< Number of speaker channels.

        @values #VSS_NUM_DEV_CHANNELS_1 */
}
#include "apr_pack_end.h"
;
/** @} *//* end_weakgroup weak_weak_vss_param_num_dev_channels_t */

#endif /* __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__ */

