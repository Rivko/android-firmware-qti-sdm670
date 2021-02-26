#ifndef __VSS_IVOLUME_PUBLIC_IF_H__
#define __VSS_IVOLUME_PUBLIC_IF_H__

/**
  @file vss_ivolume_public_if.h
  @brief This file contains the APR API definitions for volume related 
  functions.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_ivolume_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"

/****************************************************************************
 * VOLUME DEFINES                                                           *
 ****************************************************************************/
/** @addtogroup weakb_cvd_volume_values
@{ */
/*---------------------------------------------------------------------------*
 * Volume Directions                                                         *
 *---------------------------------------------------------------------------*/
/** Direction is the Tx path. */
#define VSS_IVOLUME_DIRECTION_TX ( 0 )

/** Direction is the Rx path. */
#define VSS_IVOLUME_DIRECTION_RX ( 1 )

/*--------------------------------------------------------------------------*
 * Mute Modes                                                               *
 *--------------------------------------------------------------------------*/
/** Disables mute. */
#define VSS_IVOLUME_MUTE_OFF ( 0 )

/** Enables mute. */
#define VSS_IVOLUME_MUTE_ON ( 1 )

/** @} */ /* end_addtogroup weakb_cvd_volume_values */


/****************************************************************************
 * MUTE API                                                                 *
 ****************************************************************************/

/** @ingroup cvd_volume_cmd_set_mute 
  Changes the mute setting. The new mute setting will be applied over the
  specified ramp duration.

  @note1hang This command applies to both CVS and CVP.
  
  @apr_msgpayload{vss_ivolume_cmd_mute_v2_t}
  @table{weak__vss__ivolume__cmd__mute__v2__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.

  @inputfile{group__weakb__cvd__volume__values.tex}
*/
#define VSS_IVOLUME_CMD_MUTE_V2 ( 0x0001138B )

/* Type definition for vss_ivolume_cmd_mute_v2_t. */
typedef struct vss_ivolume_cmd_mute_v2_t vss_ivolume_cmd_mute_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivolume_cmd_mute_v2_t
@{ */
/* Payload structure for the VSS_IVOLUME_CMD_MUTE command.
*/
struct vss_ivolume_cmd_mute_v2_t
{
  uint16_t direction;
  /**< Specifies the path on which Mute mode is set.

       @values
       - #VSS_IVOLUME_DIRECTION_TX
       - #VSS_IVOLUME_DIRECTION_RX

       To apply the same setting over both Tx and Rx (bidirectional), the
       client must send a separate command on each path. */

  uint16_t mute_flag;
  /**< Specifies whether Mute mode is enabled.

       @values
       - #VSS_IVOLUME_MUTE_OFF
       - #VSS_IVOLUME_MUTE_ON @tablebulletend */

  uint16_t ramp_duration_ms;
  /**< Period over which the volume is muted or unmuted.

       @values 0 to 5000 ms

       For an immediate effect of mute/unmute without ramping, set this
       field to to zero. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivolume_cmd_mute_v2_t */


/****************************************************************************
 * VOLUME APIS                                                              *
 ****************************************************************************/

/** @ingroup cvd_volume_cmd_set_num_vol_steps
  Sets the total number of volume steps.

  @note1hang This command is supported by the CVP module only.

  @apr_msgpayload{vss_ivolume_cmd_set_number_of_steps_t}
  @table{weak__vss__ivolume__cmd__set__number__of__steps__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS ( 0x000112C1 )

/* Type definition for vss_ivolume_cmd_set_number_of_steps_t.  */
typedef struct vss_ivolume_cmd_set_number_of_steps_t vss_ivolume_cmd_set_number_of_steps_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivolume_cmd_set_number_of_steps_t
@{ */
/* Payload structure for the VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS command.
*/
struct vss_ivolume_cmd_set_number_of_steps_t
{
  uint32_t value;
    /**< Number of volume steps. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivolume_cmd_set_number_of_steps_t */

/** @ingroup cvd_volume_cmd_set_vol_step
  Sets the volume step.

  @note1hang This command is supported by the CVP module only.

  @apr_msgpayload{vss_ivolume_cmd_set_step_t}
  @table{weak__vss__ivolume__cmd__set__step__t}

  @detdesc
  This command applies the volume setting over the specified ramp duration in
  milliseconds.
  @par
  The default number of volume steps is set to the number of volume calibration
  indices found in the registered volume calibration data.
  @par
  #VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS must be called after volume calibration
  data registration to override the default number of volume steps.
  @par
  The volume calibration data contains n volume indices (0 to n-1 where 0 is
  the lowest volume), where n is defined by the client. This applies
  to this command and #VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX.
  This command behaves equivalently to VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX
  when the number of volume steps is n.
  @par
  When the number of volume steps is set as equal to the number of indices in
  the registered volume calibration data, this command is effectively a linear
  mapping.
  @par
  When the number of volume steps is set as greater than the number of indices
  in the registered volume calibration data, this command causes interpolation
  of the volume level, as well as a search of the closest matching volume level
  in the registered volume calibration data to apply.
  @par
  When the number of volume steps is set as less than the number of indices in
  the registered volume calibration data, #VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS
  and VSS_IVOLUME_CMD_SET_STEP will fail.
  @par
  You are responsible for ensuring that the client-required number of volume
  steps and the number of volume indices in the volume calibration data match,
  unless interpolation is intended.
  @par
  #VSS_IVOLUME_CMD_SET_STEP applies the volume setting over the specified
  ramp duration in milliseconds.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IVOLUME_CMD_SET_STEP ( 0x000112C2 )

/* Type definition for vss_ivolume_cmd_set_step_t. */
typedef struct vss_ivolume_cmd_set_step_t vss_ivolume_cmd_set_step_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivolume_cmd_set_step_t
@{ */
/* Payload structure for the VSS_IVOLUME_CMD_SET_STEP command.
*/
struct vss_ivolume_cmd_set_step_t
{
  uint16_t direction;
  /**< Specifies the direction in which to apply the volume command.

       @values #VSS_IVOLUME_DIRECTION_RX */

  uint32_t value;
  /**< Volume step used to find the corresponding linear volume and 
       the best match index in the registered volume calibration table. */

  uint16_t ramp_duration_ms;
  /**< Period over which volume changes are applied.

       @values 0 to 5000 ms */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivolume_cmd_set_step_t */


#endif /* __VSS_IVOLUME_PUBLIC_IF_H__ */

