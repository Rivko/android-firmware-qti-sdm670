#ifndef __VSS_IRECORD_PUBLIC_IF_H__
#define __VSS_IRECORD_PUBLIC_IF_H__

/**
  @file vss_irecord_public_if.h
  @brief This file contains the APR API definitions for In-Call conversation
  recording related functions.
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

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_irecord_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
07/20/11    nrc    Initial revision.

===========================================================================*/


#include "apr_comdef.h"

/****************************************************************************
 * RECORD DEFINES                                                          *
 ****************************************************************************/

/** @addtogroup weakb_cvd_incall_record_values
@{ */
/** Default AFE port ID. */
#define VSS_IRECORD_PORT_ID_DEFAULT ( 0xFFFF )

/** Indicates no tapping for the specified path. */
#define VSS_IRECORD_TAP_POINT_NONE ( 0x00010F78 )

/** Indicates that the specified path is to be tapped at the end of 
 the stream. */
#define VSS_IRECORD_TAP_POINT_STREAM_END ( 0x00010F79 )

/** Selects Tx on the left channel and Rx on the right channel. */
#define VSS_IRECORD_MODE_TX_RX_STEREO ( 0x00010F7A )

/** Selects mixed Tx and Rx paths. */
#define VSS_IRECORD_MODE_TX_RX_MIXING ( 0x00010F7B )
/** @} */ /* end_addtogroup cvd_incall_rec_values */

/****************************************************************************
 * IN-CALL CONVERSATION RECORDING APIS                                      *
 ****************************************************************************/

/** @ingroup cvd_incall_rec_cmd_start
  Starts the in-call conversation recording by forwarding the
  in-call conversation stream to the specified AFE port.

  @apr_msgpayload{vss_irecord_cmd_start_t}
  @table{weak__vss__irecord__cmd__start__t}

  @detdesc
  The audio path is used to record the voice streams. For recording silence
  before a call or during handovers, the AFE port must be configured to Live
  mode.
  @par
  Only one recording session can be active on a particular module instance at
  any given time.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.

  @inputfile{group__weakb__cvd__incall__record__values.tex}
*/
#define VSS_IRECORD_CMD_START ( 0x000112BE )

/* Type definition for vss_irecord_cmd_start_t. */
typedef struct vss_irecord_cmd_start_t vss_irecord_cmd_start_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_irecord_cmd_start_t
@{ */
/* Payload structure for the VSS_IRECORD_CMD_START command.
*/
struct vss_irecord_cmd_start_t
{
  uint32_t rx_tap_point;
  /**< Tap point to use on the Rx path.

       @values
       - #VSS_IRECORD_TAP_POINT_NONE
       - #VSS_IRECORD_TAP_POINT_STREAM_END @tablebulletend */

  uint32_t tx_tap_point;
  /**< Tap point to use on the Tx path.

       @values
       - #VSS_IRECORD_TAP_POINT_NONE
       - #VSS_IRECORD_TAP_POINT_STREAM_END @tablebulletend */

  uint16_t port_id;
  /**< ID of the AFE port to which the conversation recording stream must be
       sent.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE
       - To use the default AFE pseudoports, set this field to
       #VSS_IRECORD_PORT_ID_DEFAULT:
       - 0x8003 for Rx
       - 0x8004 for Tx @tablebulletend */

  uint32_t mode;
  /**< Recording mode.

       @values
       - #VSS_IRECORD_MODE_TX_RX_STEREO
       - #VSS_IRECORD_MODE_TX_RX_MIXING

       The mode parameter value is ignored if the port_id is set to
       #VSS_IRECORD_PORT_ID_DEFAULT. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_irecord_cmd_start_t */


/** @ingroup cvd_incall_rec_cmd_stop
  Stops the in-call conversation recording.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IRECORD_CMD_STOP ( 0x00011237 )


#endif /* __VSS_IRECORD_PUBLIC_IF_H__ */

