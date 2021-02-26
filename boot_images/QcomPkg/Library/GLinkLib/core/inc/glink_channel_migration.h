#ifndef GLINK_CHANNEL_MIGRATION_H
#define GLINK_CHANNEL_MIGRATION_H

/*===========================================================================

                    GLink channel migration Header File


-----------------------------------------------------------------------------
Copyright (c) 2014 - 2015 Qualcomm Technologies Inc.
-----------------------------------------------------------------------------
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/15   bc      Channel migration update/bug fix
05/18/15   bc      Separated channel migration from common core
05/06/15   bc      Fix for channel migration bug
02/20/15   bc      Update/modify channel migration apis
11/14/14   db      Initial version. 
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "glink_transport_if.h"

/*===========================================================================
                        FEATURE DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glink_rx_cmd_ch_remote_open_ch_migration
===========================================================================*/
/** 
 * Receive remote channel open request and start channel migration if needed
 * This function will send ch open ack as result.
 * It will also send new channel open/old channel close cmd
 * if channel migration happens
 *
 * @param[in]    if_ptr    Pointer to interface instance; must be unique
                           for each edge
 * @param[in]    rcid      Remote Channel ID
 * @param[in]    name      String name for logical channel
 * @param[in]    priority  xport priority requested by remote side
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_remote_open_ch_migration
(
  glink_transport_if_type *if_ptr,
  uint32                   rcid,
  const char              *name,
  glink_xport_priority     priority
);
  
/*===========================================================================
  FUNCTION      glink_rx_cmd_ch_open_ack_ch_migration
===========================================================================*/
/** 
 * Receive channel open request ack and start channel migration if needed
 * This function will send new channel open/old channel close cmd
 * if channel migration happens
 * 
 * @param[in]    if_ptr            Pointer to interface instance; must be unique
 *                                 for each edge
 * @param[in]    lcid              Local Channel ID
 * @param[in]    migrated_ch_prio  Negotiated xport priority from remote side
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_open_ack_ch_migration
(
  glink_transport_if_type *if_ptr,
  uint32                   lcid,
  glink_xport_priority     migrated_ch_prio
);

#endif /* GLINK_CHANNEL_MIGRATION_H */
