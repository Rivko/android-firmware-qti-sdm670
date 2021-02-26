#ifndef __VSS_IPLAYBACK_PUBLIC_IF_H__
#define __VSS_IPLAYBACK_PUBLIC_IF_H__

/**
  @file vss_iplayback_public_if.h
  @brief This file contains the APR API definitions for In-Call Music Delivery
  related functions.
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

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_iplayback_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
07/19/11    nrc    Initial revision.

===========================================================================*/


#include "apr_comdef.h"

/****************************************************************************
 * PLAYBACK DEFINES                                                         *
 ****************************************************************************/

/** @addtogroup weakb_cvd_incall_playback_value
@{ */
/** Default AFE port ID. */
#define VSS_IPLAYBACK_PORT_ID_DEFAULT ( 0xFFFF )
/** @} */ /* end_addtogroup weakb_cvd_incall_playback_value */


/****************************************************************************
 * IN-CALL MUSIC DELIVERY APIS                                              *
 ****************************************************************************/

/** @ingroup cvd_incall_playback_cmd_start
  Starts the in-call music delivery on the Tx voice path by establishing a
  connection between the Tx voice stream and the specified AFE port. The voice
  stream is allowed to receive music from the audio path.

  @apr_msgpayload{vss_iplayback_cmd_start_t}
  @table{weak__vss__iplayback__cmd__start__t}

  @detdesc
  The audio path is used to inject music to the Tx voice path. Configuring
  the AFE port to nonlive mode allows the voice modules to throttle transfer
  jitters. 
  @par
  Only one playback session can be active on a particular module instance at
  any given time.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.

  @inputfile{group__weakb__cvd__incall__playback__value.tex}
*/
#define VSS_IPLAYBACK_CMD_START ( 0x000112BD )

/* Type definition for vss_iplayback_cmd_start_t. */
typedef struct vss_iplayback_cmd_start_t vss_iplayback_cmd_start_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iplayback_cmd_start_t
@{ */
/* Payload structure for the VSS_IPLAYBACK_CMD_START command.
*/
struct vss_iplayback_cmd_start_t
{
  uint16_t port_id;
  /**< ID of the AFE port from which the audio samples are available.

       @values
       - Refer to @xrefcond{Q6,80-NF774-3,80-NA610-3}
       - If a port ID is not supplied, set this field to
         #VSS_IVOCPROC_PORT_ID_NONE
       - To use the default AFE pseudoport (0x8005), set this field to
         #VSS_IPLAYBACK_PORT_ID_DEFAULT @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iplayback_cmd_start_t */


/** @ingroup cvd_incall_playback_cmd_stop
  Stops the in-call music delivery on the Tx voice path.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IPLAYBACK_CMD_STOP ( 0x00011239 )


#endif /* __VSS_IPLAYBACK_PUBLIC_IF_H__ */

