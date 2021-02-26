#ifndef SMDL_ECHO_H
#define SMDL_ECHO_H

/**
 * @file smdl_echo.h
 *
 * Internal header file for echo service for SMD Lite
 *
 * This differs from "echo" in one key respect: Data read on one pipe is
 * written on the other. In "echo" mode, there is only one channel,
 * "LOOPBACK". In "echo" mode, there are two channels: "ECHOA" and "ECHOB".
 */

/*==============================================================================
     Copyright (c) 2010 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/10   tl      Updated to SMD Lite v2
01/07/10   tl      Created echo service header file
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smd.h"

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Opens a echo data port on the local processor.
 *
 * @param[in]  name          The name of the port to open.
 * @param[in]  channel_type  The edge on which to open the echo port.
 *
 * @return       @li 0 
 *               @li A negative error code if an error occurred.
 *
 * @sideeffects  Opens an SMD Lite port on the local system.
 */
void smdl_setup_echo
(
  const char       *echo_a,
  const char       *echo_b,
  smd_channel_type  channel_type
);

/**
 * Opens a loopback data port on the local processor.
 *
 * @param[in]  name          The name of the port to open.
 * @param[in]  channel_type  The edge on which to open the loopback port.
 * @param[in]  fifo_size     The FIFO size to use
 *
 * @return       none
 *
 * @sideeffects  Opens an SMD Lite port on the local system.
 */
void smdl_open_loopback
(
  const char       *name,
  smd_channel_type  channel_type,
  uint32            fifo_size
);

#endif /* SMDL_ECHO_H */
