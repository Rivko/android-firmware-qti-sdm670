#ifndef __VSS_IAVSYNC_PUBLIC_IF_H__
#define __VSS_IAVSYNC_PUBLIC_IF_H__

/**
  @file vss_iavsync_public_if.h
  @brief This file contains the APR API definitions for CVD audio-video
  synchronization information.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_iavsync_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * AV SYNC APIS                                                             *
 ****************************************************************************/

/** @ingroup cvd_avsync_evtclass_rx
  Rx voice path audio-video syncrhonization event class. This class
  comprises one event: #VSS_IAVSYNC_EVT_RX_PATH_DELAY.
*/
#define VSS_IAVSYNC_EVENT_CLASS_RX ( 0x00012EE1 )


/** @ingroup cvd_avsync_evt_rx_path_delay
  Notifies the client of changes in the value of the Rx voice path delay.

  @apr_msgpayload{vss_iavsync_evt_rx_path_delay_t}
  @table{weak__vss__iavsync__evt__rx__path__delay__t}

  @detdesc
  This event is sent to the client under the following circumstances:
   - Every time the module with which the client is registered for this
     event is started.
   - Immediately after registration, if the module is already running.
   - Subsequently whenever the value of the Rx path delay changes.

  @return
  None.

  @dependencies
  To receive this event, the client must first register to listen for the
  #VSS_IAVSYNC_EVENT_CLASS_RX event class with the appropriate module via the
  #VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS command.
  @par
  To stop receiving this event, the client must cancel the
  VSS_IAVSYNC_EVENT_CLASS_RX event class via the
  #VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS command.
*/
#define VSS_IAVSYNC_EVT_RX_PATH_DELAY ( 0x00012EE2 )

/* Type definition for vss_iavsync_evt_rx_path_delay_t. */
typedef struct vss_iavsync_evt_rx_path_delay_t vss_iavsync_evt_rx_path_delay_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iavsync_evt_rx_path_delay_t
@{ */
/* Payload structure for the #VSS_IAVSYNC_EVT_RX_PATH_DELAY event.
*/
struct vss_iavsync_evt_rx_path_delay_t
{
   uint32_t delay_us;     /**< Rx voice path delay in microseconds. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iavsync_evt_rx_path_delay_t */

/****************************************************************************
 * AV SYNC TX APIS                                                          *
 ****************************************************************************/

/** @addtogroup cvd_avsync_evtclass_tx
@{ */
/** Tx voice path audio-video synchronization event class. This class
  comprises one event: #VSS_IAVSYNC_EVT_TX_TIMESTAMP.
*/
#define VSS_IAVSYNC_EVENT_CLASS_TX ( 0x00012EE5 )

/**
  Notifies the client of samples captured at the microphone
  (AVTimertimestamp - total Tx path delay).

  @apr_msgpayload{vss_iavsync_evt_tx_timestamp_t}
  @table{weak__vss__iavsync__evt__tx__timestamp__t}

  @detdesc
  This event is sent to the client before every Tx vocoder packet event.

  @return
  None.

  @dependencies
  To receive this event, the client must first register to listen for
  #VSS_IAVSYNC_EVENT_CLASS_TX with the appropriate module via
  #VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS.
  @par
  To stop receiving this event, the client must cancel
  VSS_IAVSYNC_EVENT_CLASS_TX via the #VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS.
*/
#define VSS_IAVSYNC_EVT_TX_TIMESTAMP ( 0x00012EE6 )

/* Type definition for vss_iavsync_evt_tx_timestamp_t. */
typedef struct vss_iavsync_evt_tx_timestamp_t vss_iavsync_evt_tx_timestamp_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iavsync_evt_tx_timestamp_t
@{ */
/* Payload structure for the #VSS_IAVSYNC_EVT_TX_TIMESTAMP event.
*/
struct vss_iavsync_evt_tx_timestamp_t
{
   uint64_t timestamp_us;     /**< Tx path timestamp (in microseconds) at the
                                   microphone. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_addtogroup vss_iavsync_public_if */
/** @} */ /* end_addtogroup cvd_avsync_evtclass_tx */


#endif /* __VSS_IAVSYNC_PUBLIC_IF_H__ */
