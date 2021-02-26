#ifndef VPTX_SENSORS_H
#define VPTX_SENSORS_H

/*============================================================================
  FILE:          vptx_sensors.h

  OVERVIEW:      This file has the data structure for sensors interface for fpro v2 algorithm.

  DEPENDENCIES:  None

Copyright (c) 2014, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/voice_proc_tx_sensors/src/vptx_sensors.h#2 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2014-07-14 rishabht   Initial revision.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "vptx_sensors_api.h"
#include "sns_facing.pb.h"
#include "EliteQmiClientIf.h"



// structure used to request for registration to qmi
typedef struct qmi_obj_t
{
   elite_qmi_client_type               user_handle;                 // User handle issued by QMI.
   qurt_elite_signal_t*                reg_sig;                     // Signal used to complete registration with Sensors service
   qurt_elite_signal_t*                ind_sig;                     // Signal set each time there's an indication from Sensors
   sns_facing_event_state              holding_pattern;             // Holding pattern information received from Sensors.
   uint8_t                             instance_id;                 // Instance ID issued by Sensors manager, for the client (Vptx)
                                                                    // to maintain and cross-reference.
   uint32_t                            session_num;                 // vptx session number
} qmi_obj_t;

#endif /* #ifndef VPTX_SENSORS_H */
