#ifndef __VSS_IMVM_PUBLIC_IF_H__
#define __VSS_IMVM_PUBLIC_IF_H__

/**
  @file vss_imvm_public_if.h
  @brief This file contains the APR API definitions for Multimode Voice Manager 
  (MVM) module.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2012, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_imvm_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * MULTIMODE VOICE MANAGER CORE APIS                                        *
 ****************************************************************************/

/** @ingroup cvd_mvm_cmd_create_passive_ctrl
  Creates a new passive control MVM session.

  @apr_msgpayload
  The payload is the session name string used to identify the full control
  MVM session for which a handle is to be obtained. The string size, including
  the NULL termination character, is limited to 31 characters.

  @detdesc
  Passive MVM sessions give clients access to UI controls, such as
  @if OEM_only Teletypewriter (TTY) and @endif
  Wide Voice, as well as connections between streams and vocprocs. The
  corresponding full control sessions internally manage additional system
  policies such as network settings and voice timing parameter settings.
  @par
  Use the session name, <i>default modem voice</i>, to obtain a handle to the
  default MVM session for a circuit-switched voice call.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_CREATE_PASSIVE_CONTROL_SESSION ( 0x000110FF )

/** @ingroup cvd_mvm_cmd_create_full_ctrl
  Creates a new full control MVM session.

  @apr_msgpayload
  The payload is the session name string given to the full control session
  being created. The string size, including the NULL termination character, is
  limited to 31 characters.
  @par
  If a name is supplied, it must be NULL terminated. A size zero name is
  interpreted as a nameless session where only the handle is used to access
  the session.

  @detdesc
  Full MVM sessions give clients full control over the voice system, including
  network settings, voice timing parameter settings, UI controls, and stream
  vocproc connections.
  @par
  The session name, <i>default modem voice</i>, is reserved for the default MVM
  session for a circuit-switched voice call and cannot be given to any new full
  control sessions. Multiple full control MVM sessions cannot be created with
  the same name.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000110FE )

/** @ingroup cvd_mvm_cmd_set_policy_dual
  Sets the dual control policy on the MVM.

  @apr_msgpayload{vss_imvm_cmd_set_policy_dual_control_t}
  @table{weak__vss__imvm__cmd__set__policy__dual__control__t}

  @detdesc
  In Dual Control mode, the MVM state is controlled by both the modem and the
  applications. Dual control policy must be set on the MVM immediately after
  session creation and before starting voice.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL ( 0x00011327 )

/* Type definition for vss_imvm_cmd_set_policy_dual_control_t. */
typedef struct vss_imvm_cmd_set_policy_dual_control_t vss_imvm_cmd_set_policy_dual_control_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_set_policy_dual_control_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL command.
*/
struct vss_imvm_cmd_set_policy_dual_control_t
{
  bool_t enable_flag;
  /**< Specifies whether dual control policy is enabled (TRUE). */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_set_policy_dual_control_t */

/** @ingroup cvd_mvm_cmd_attach_stream
  Attaches a stream to the MVM. The MVM symmetrically connects this
  stream to all the vocprocs currently attached to it.

  @apr_msgpayload{vss_imvm_cmd_attach_stream_t}
  @table{weak__vss__imvm__cmd__attach__stream__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_ATTACH_STREAM ( 0x0001123C )

/* Type definition for vss_imvm_cmd_attach_stream_t. */
typedef struct vss_imvm_cmd_attach_stream_t vss_imvm_cmd_attach_stream_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_attach_stream_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_ATTACH_STREAM command.
*/
struct vss_imvm_cmd_attach_stream_t
{
  uint16_t handle;
    /**< Stream handle to attach to the MVM. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_attach_stream_t */

/** @ingroup cvd_mvm_cmd_detach_stream
  Detaches a stream from the MVM. The MVM disconnects this stream from
  all the vocprocs to which it is currently connected.

  @apr_msgpayload{vss_imvm_cmd_detach_stream_t}
  @table{weak__vss__imvm__cmd__detach__stream__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_DETACH_STREAM ( 0x0001123D )

/* Type definition for vss_imvm_cmd_detach_stream_t. */
typedef struct vss_imvm_cmd_detach_stream_t vss_imvm_cmd_detach_stream_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_detach_stream_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_DETACH_STREAM command.
*/
struct vss_imvm_cmd_detach_stream_t
{
  uint16_t handle;
    /**< Stream handle to detach from the MVM. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_detach_stream_t */

/** @ingroup cvd_mvm_cmd_attach_vocproc
  Attaches a vocproc to the MVM. The MVM symmetrically connects this
  vocproc to all the streams currently attached to it.

  @apr_msgpayload{vss_imvm_cmd_attach_vocproc_t}
  @table{weak__vss__imvm__cmd__attach__vocproc__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_ATTACH_VOCPROC ( 0x0001123E )

/* Type definition for vss_imvm_cmd_attach_vocproc_t. */
typedef struct vss_imvm_cmd_attach_vocproc_t vss_imvm_cmd_attach_vocproc_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_attach_vocproc_t
@{ */
/* Payload structure for the VSS_IMVM_CMD_ATTACH_VOCPROC command.
*/
struct vss_imvm_cmd_attach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to attach to the MVM. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_attach_vocproc_t */

/** @ingroup cvd_mvm_cmd_detach_vocproc
  Detaches a vocproc from the MVM. The MVM disconnects this vocproc from
  all the streams to which it is currently connected.

  @apr_msgpayload{vss_imvm_cmd_detach_vocproc_t}
  @table{weak__vss__imvm__cmd__detach__vocproc__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_DETACH_VOCPROC ( 0x0001123F )

/* Type definition for vss_imvm_cmd_detach_vocproc_t. */
typedef struct vss_imvm_cmd_detach_vocproc_t vss_imvm_cmd_detach_vocproc_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_detach_vocproc_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_DETACH_VOCPROC command.
*/
struct vss_imvm_cmd_detach_vocproc_t
{
  uint16_t handle;
    /**< Vocproc handle to detach from the MVM. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_detach_vocproc_t */

/** @ingroup cvd_mvm_cmd_start_voice
  Starts a voice call. Start mode activates the vocproc and stream.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_START_VOICE ( 0x00011190 )

/** @ingroup cvd_mvm_cmd_standby_voice
  Places a voice call into Standby mode, which suspends the stream but keeps
  the vocproc active.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_STANDBY_VOICE ( 0x00011191 )

/** @ingroup cvd_mvm_cmd_pause_voice
  Pauses a voice call. Pause mode suspends the stream and vocproc
  without reinitializing the stream or vocproc.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_PAUSE_VOICE ( 0x0001137D )

/** @ingroup cvd_mvm_cmd_stop_voice
  Stops a voice call. Stop mode suspends the stream and vocproc activities,
  and reinitializes the stream and vocproc.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5})

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_STOP_VOICE ( 0x00011192 )


/****************************************************************************
 * MULTIMODE VOICE MANAGER CORE MODEM APIS                                  *
 ****************************************************************************/

/** @ingroup cvd_mvm_cmd_modem_start_voice
  Starts a voice call. Start mode activates the vocproc and stream.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_MODEM_START_VOICE ( 0x00011193 )

/** @ingroup cvd_mvm_cmd_modem_standby_voice
  Places a voice call into Standby mode, which suspends the stream but keeps
  the vocproc active.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_MODEM_STANDBY_VOICE ( 0x00011194 )

/** @ingroup cvd_mvm_cmd_modem_stop_voice
  Stops a voice call. Stop mode suspends the stream and vocproc activities.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_MODEM_STOP_VOICE ( 0x00011195 )


/****************************************************************************
 * MULTIMODE VOICE MANAGER CALIBRATION FRAMEWORK APIS                       *
 ****************************************************************************/

/** @ingroup cvd_mvm_cmd_set_network_calib
  Sets the network ID to be used for calibration. This command is supported
  for full control clients only.

  @apr_msgpayload{vss_imvm_cmd_set_cal_network_t}
  @table{weak__vss__imvm__cmd__set__cal__network__t}

  @detdesc
  Stream and vocproc calibration for voice calls is dependent on the network.
  The CVD client must provide the network ID to the MVM, which propagates it
  to the streams and vocprocs attached to it for purposes of indexing into
  the calibration table and to apply the corresponding calibration data subset.
  @par
  If a network ID is not provided to the MVM by the client, no calibration data
  is applied by the streams and vocprocs attached to the MVM.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_CAL_NETWORK ( 0x0001137A )

/* Type definition for vss_imvm_cmd_set_cal_network_t. */
typedef struct vss_imvm_cmd_set_cal_network_t vss_imvm_cmd_set_cal_network_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_set_cal_network_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_SET_CAL_NETWORK command.
*/
struct vss_imvm_cmd_set_cal_network_t
{
  uint32_t network_id;
  /**< Calibration network ID.

       @values See @xhyperref{hdr:CalNetworkID,Network ID column values} */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_set_cal_network_t */

/** @ingroup cvd_mvm_cmd_set_cal_media_type
  Sets the media type to be used for calibration. This command is supported
  for full control clients only.

  @apr_msgpayload{vss_imvm_cmd_set_cal_media_type_t}
  @table{weak__vss__imvm__cmd__set__cal__media__type__t} 

  @detdesc
  Stream and vocproc calibration is dependent on the Tx/Rx sampling rate. It
  is also dependent on the vocoder class if such calibration data is provided
  to the CVD.
  @par
  The CVD client must provide the calibration media type to the MVM, which
  uses it for two purposes:
  - Determining the Tx and Rx sample rates at which the system runs and for
    which the system is calibrated.
     - For fixed sample rate vocoders, the rates are then propagated to the
       streams and vocprocs attached to the system. Their sample rates are
       configured and indexed into the calibration table to apply the
       corresponding calibration data subset.
     - For variable sample rate vocoders, the MVM cannot determine the sample
       rates from the media type alone and requires the sample rates to be
       provided by the client; see #VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE.
  - Determining the vocoder class and propagating it to the streams and
    vocprocs attached to the system. The class is indexed into the calibration
    table to apply the corresponding calibration data subset.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE ( 0x0001137B )

/* Type definition for vss_imvm_cmd_set_cal_media_type_t. */
typedef struct vss_imvm_cmd_set_cal_media_type_t vss_imvm_cmd_set_cal_media_type_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_set_cal_media_type_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE command.
*/
struct vss_imvm_cmd_set_cal_media_type_t
{
  uint32_t media_id;
  /**< Media ID.

       @values See Section @xref{dox:MediaDefinitions} */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_set_cal_media_type_t */

/** @ingroup cvd_mvm_cmd_set_max_voc_samp_rate
  Sets the maximum sampling rates to be used for vocoders that support variable
  sampling rates.

  @apr_msgpayload{vss_imvm_cmd_set_max_var_voc_sampling_rate_t}
  @table{weak__vss__imvm__cmd__set__max__var__voc__sampling__rate__t} 

  @detdesc
    @if OEM_only This command has no effect in the following situations:
  - The #VSS_MEDIA_ID_EVS media type is not used via
    #VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE.
  - The payload parameters are invalid.
    @endif
  @if ISV_only This command has no effect if the payload parameters are
  invalid.@endif
  @par
  This command takes effect in the following situations:
  - If the vocoder supports variable sampling rates and no maximum is
    provided to the MVM (i.e., this command is not called), the default
    maximum sampling rate for the media type is used.
  - If the sampling rate specified for the payload is 0, it is the default
    maximum sampling rate for the media type.
  @par
  Reconfiguration of postprocessing sampling rates might occur after this
  command is sent because the actual sampling rate set up on the system is an
  interal policy of the CVD.
  @par
  This command supports any combination of the valid sampling rates for Rx and
  Tx. @newpage

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE ( 0x0001137C )

/* Type definition for vss_imvm_cmd_set_max_var_voc_sampling_rate_t. */
typedef struct vss_imvm_cmd_set_max_var_voc_sampling_rate_t vss_imvm_cmd_set_max_var_voc_sampling_rate_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_set_max_var_voc_sampling_rate_t 
@{ */
/* Payload structure of the VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE command.
*/
struct vss_imvm_cmd_set_max_var_voc_sampling_rate_t
{
  uint32_t rx;
  /**< Rx sampling rate.

       @values
       - 0 (Default)
       - 8 kHz (NB)
       - 16 kHz (WB)
       - 32 kHz (SWB)
       - 48 kHz (FB) @tablebulletend */

  uint32_t tx;
  /**< Tx sampling rate.

       @values
       - 0 (Default)
       - 8 kHz (NB)
       - 16 kHz (WB)
       - 32 kHz (SWB)
       - 48 kHz (FB) @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_set_max_var_voc_sampling_rate_t */

#endif /* __VSS_IMVM_PUBLIC_IF_H__ */

