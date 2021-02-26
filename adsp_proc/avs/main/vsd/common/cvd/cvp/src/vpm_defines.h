/**========================================================================
 @file vpm_defines.h
 
 @brief This header file contains macro definitions used by the
 the static service and vocproc session objects
 
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_defines.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#ifndef _VPM_DEFINES_H_
#define _VPM_DEFINES_H_

#include "adsp_vparams_api.h"
#include "adsp_vcmn_api.h"
#include "adsp_vpm_api.h"
#include "adsp_afe_service_commands.h"
#include "vss_public_if.h"

#include "vccm_api.h"
#include "vss_private_if.h"
#include "cvd_cal_common_i.h"
#include "cvd_cal_log_i.h"

/****************************************************************************
 * VPM DEFINES                                                              *
 ****************************************************************************/

#define VPM_MAX_SVC_CMD_Q_ELEMENTS  16
#define VPM_MAX_SVC_RSP_Q_ELEMENTS   8

#define VPM_MAX_NUM_SESSIONS ( 4 )

#define VPM_MAX_NUM_CMD_RSP (16)

#define VPM_MAX_NUM_CHANNEL_BITS      (32)

#define VPM_VERSION_V ( 0x00000000 )
/**<
 * TODO: Need to come up with a versioning scheme that will sustain for
 * generations.
 */

#define CVP_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[%d]", rc, 0, 0 ); } }

/* This macro is to be used to inspect the APR return code whenever CVP
 * calls APR to send a packet.
 *
 * When CVP encounters an APR communication error for sending a packet to an
 * APR service who resides in a different domain than CVP, an error message
 * will be printed and the error is ignored. This is the phase one SSR
 * implementation for handling APR communication errors due to that CVP's
 * client's subsystem encounters SSR. In the future, CVP may need to use a
 * timer based retry mechanism to keep on trying to communicate with the remote
 * client until CVP knows that the remote client is no longer available.
 *
 * When CVP encounters an APR communication error for sending a packet to an
 * APR service who resides in the same domain as CVP, CVP_PANIC_ON_ERROR will
 * be called.
 */
#define CVP_COMM_ERROR( rc, dst_addr ) \
  { if ( ( rc ) && \
         ( APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, dst_addr ) != \
           APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, cvp_my_addr ) ) ) \
    { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "CVP APR comm error 0x%08X, \
                                    dst_addr = 0x%04X", rc, dst_addr ); } \
    else \
    { CVP_PANIC_ON_ERROR( rc ); } \
  }

#define VPM_MIN_VOLUME_MB_SUPPORTED ( -10000 )
/**< Minimum volume supported is -10000 mB. Based on system team library limition. */

#define VPM_MAX_VOLUME_MB_SUPPORTED ( 2000 )
/**< Maximum volume supported is 2000 mB. Based on system team library limition. */

#define VPM_VOLUME_INTERPOLATION_STEPS ( 100 )
/**< Number of interval steps between successive mB. */

#define VPM_MAX_VP3_DATA_LEN ( 1024 )
/**< Maximum length (in bytes) of VP3 data suppported. */

#define VPM_MAX_UI_PROP_DATA_LEN ( 12 )
/**<
 * Maximum length (in bytes) of a UI property.
 *
 * UI prop data can hold only enable/disable params
 * If UI properties intend to hold more params,
 * this size need to be increased
 */

#define VPM_MAX_UI_PROP ( 20 )
/**< Maximum number of UI properties that can be cached. */

#define VPM_MUTE_ENABLE ( 1 )
#define VPM_MUTE_DISABLE ( 0 )

/**< Module ID for VP3 data. 
  This remains same irrespective of topology. */
#define VPM_VP3_MOD_ID ( 0x00010EF6 )

/**< Param ID for VP3 data. 
  This remains same irrespective of topology. */
#define VPM_VP3_PARAM_ID ( 0x00010E03 )

#define VPM_VP3_SIZE_PARAM_ID ( 0x00010E1A )

#define VPM_DEFAULT_RX_PP_SR ( 8000 ) /* Default post-proc sample rate. */
#define VPM_DEFAULT_TX_PP_SR ( 8000 ) /* Default pre-proc sample rate. */

#define VPM_DEFAULT_DEC_SR ( 8000 ) /* Default dec sample rate. */
#define VPM_DEFAULT_ENC_SR ( 8000 ) /* Default enc sample rate. */

#define VPM_COMMON_NUM_CAL_COLUMNS ( 3 ) /* BACKWARD COMPATIBILITY */
/**< Number of common cal columns is 3: network ID, Tx PP sampling rate,
     Rx PP sampling rate. */

#define VPM_VOLUME_NUM_CAL_COLUMNS ( 4 ) /* BACKWARD COMPATIBILITY */
/**< Number of volume cal columns is 4: network ID, Tx PP sampling rate,
     Rx PP sampling rate, and Rx volume index. */

#define VPM_MAX_NUM_MATCHING_COMMON_CAL_ENTRIES ( 1 << VPM_COMMON_NUM_CAL_COLUMNS ) /* BACKWARD COMPATIBILITY */
/**< Max number of matching common calibration entries. */

#define VPM_MAX_NUM_MATCHING_VOLUME_CAL_ENTRIES ( 1 << VPM_VOLUME_NUM_CAL_COLUMNS ) /* BACKWARD COMPATIBILITY */
/**< Max number of matching volume calibration entries. */

#define VPM_NUM_STATIC_CAL_COLUMNS ( 7 )
/**< Number of static cal columns is 7: network ID, Tx PP sampling rate, Rx
     PP sampling rate, Tx vocoder operating mode, Rx vocoder operating mode,
     media ID, and feature. If more columns are added, this value has to be
     increased. */

#define VPM_NUM_DYNAMIC_CAL_COLUMNS ( 8 )
/**< Number of dynamic cal columns is 8: network ID, Tx PP sampling rate, Rx
     PP sampling rate, Rx volume index, Tx vocoder operating mode, Rx vocoder
     operating mode, media ID, and feature. If more columns are added, this
     value has to be increased. */

#define VPM_NUM_STREAM_CAL_FROM_COMMON_CAL_DATA ( 1 )
  /**< Number of Rx gain cal data that is fetched from common cal 
       Follows the module name and param's supported.  
         MODULE NAME        ->       PARAM
	   VOICE_MODULE_RX_GAIN -> VOICE_PARAM_GAIN */

#define VPM_SOUND_DEVICE_DIRECTION_RX ( VSS_IVOCPROC_DIRECTION_RX )
#define VPM_SOUND_DEVICE_DIRECTION_TX ( VSS_IVOCPROC_DIRECTION_TX )
#define VPM_SOUND_DEVICE_DIRECTION_RX_TX ( VSS_IVOCPROC_DIRECTION_RX_TX )
#define VPM_SOUND_DEVICE_DIRECTION_INVALID ( 0xFFFF )

#define VPM_MAX_NUM_DEVICE_PAIRS_OF_VP3_STORED ( 5 )
/**< Maximum number of device pairs in which CVP will store the VP3 data
     for. */


#define VPM_SESSION_INVALID ( NULL )

#define VPM_MAX_NUM_TOPO_PARAMS ( 1 )
/**< Maximum number of topology parameters supported in OOB registration. */
#define VPM_MAX_MPPS_SCALE_FACTOR ( 50 )
/**< Maximum scale factor for MPPS voting. */
#define VPM_MIN_MPPS_SCALE_FACTOR ( 10 )
/**< Min scale factor for MPPS voting. */
#define VPM_MAX_BW_SCALE_FACTOR ( 50 )
/**< Maximum scale factor for BW voting. */
#define VPM_MIN_BW_SCALE_FACTOR ( 5 )
/**< Min scale factor for BW voting. */
#define VPM_NUM_CHANNELS (8)
/**< Number of channels for AFE port. */

#define VPM_SESSION_CREATE_MPPS ( 70 )
/**< Max MPPS required for VPM session creation */

#endif /* #ifndef _VPM_DEFINES_H_ */
