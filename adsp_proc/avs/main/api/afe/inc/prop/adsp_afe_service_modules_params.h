#ifndef _ADSP_AFE_SERVICE_MODULES_PARAMS_H_
#define _ADSP_AFE_SERVICE_MODULES_PARAMS_H_

/*========================================================================*/
/**
@file adsp_afe_service_modules_params.h

This file contains AFE service module ID and param ID definitions proprietary
to Qualcomm Technologies, Inc.
 */
/*===========================================================================
NOTE: The description above does not appear in the PDF.
      Any descriptions that display in the PDF are located in the
      AFE_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
============================================================================ */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/afe/inc/prop/adsp_afe_service_modules_params.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/03/16   unni    Moved module related details to separate files
09/02/16    sw     (Tech Pubs) Merged Doxygen comments from AVS 2.7;
                    edited comments/markup for AVS 2.8.
08/03/16    sj     Separate header file for proprietary module and param ID's
========================================================================== */

#include "aanc.h"
#include "sw_mad.h"
#include "speaker_protection.h"
#include "rmc.h"
#include "sidetone_iir_filter.h"

/** @addtogroup afe_topo_ids
@{ */

/** ID of the AFE Tx topology with software microphone activity detection in
    Low Power mode.

    - Software Microphone Activity Detection (AFE_MODULE_ID_SW_MAD)
 */
#define AFE_TX_TOPOLOGY_ID_SWMAD_HWMAD                       0x00012E19

/** ID of the AFE Tx topology with software microphone activity detection in
    High Power mode.

    - Software Microphone Activity Detection (AFE_MODULE_ID_SW_MAD)
 */
#define AFE_TX_TOPOLOGY_ID_SWMAD                       0x00012E18

/** ID of the AFE Tx topology with adaptive active noise control.

    - Adaptive ANC (AFE_MODULE_AANC)
 */
#define AFE_TX_TOPOLOGY_ID_AANC                       0x0001025B

/** ID of the AFE Tx topology with Software Duty Cycle Reset and
    Software MAD.

    - Software Duty Cycle Reset (#AFE_MODULE_SW_DUTY_CYCLE)
    - Software MAD              (AFE_MODULE_ID_SW_MAD)
*/
#define AFE_TX_TOPOLOGY_ID_SW_DUTY_CYCLE_MAD           0x00013093

/** ID of the AFE Tx topology with RMC.

    - Remove Click Noise (AFE_MODULE_RMC)
 */
#define AFE_TX_TOPOLOGY_ID_RMC                       0x000112F9

/** ID of the AFE Tx topology with Speaker Protection version2 in Feedback
    mode.

    - Feedback Speaker Protection V2 Thermal VI
      (AFE_MODULE_SPEAKER_PROTECTION_V2_TH_VI)
    - Feedback Speaker Protection V2 Excursion VI
      (AFE_MODULE_SPEAKER_PROTECTION_V2_EX_VI)
 */
#define AFE_TX_TOPOLOGY_ID_FBSP_VI_V2_PROC             0x0001025C

/** ID of the AFE Rx topology with Speaker Protection version 2 in Feedback mode.

    - Feedback Speaker Protection V2 Rx (AFE_MODULE_SPEAKER_PROTECTION_V2_RX)
 */
#define AFE_RX_TOPOLOGY_ID_FBSP_RX_V2_PROC             0x0001025D

/** ID of the AFE Rx topology with Speaker Protection version 2 in Feedforward
    mode.

    - Feedback Speaker Protection V2 Rx (AFE_MODULE_SPEAKER_PROTECTION_V2_RX)
 */
#define AFE_RX_TOPOLOGY_ID_FFSP_RX_V2_PROC             0x0001025E

/** @} */ /*end_addtogroup afe_topo_ids */

/** @addtogroup afe_cmd_start_dtmf
@{ */
/** Starts DTMF on an Rx port and pseudoport.

  @apr_hdr_fields
    Opcode -- AFE_PORTS_CMD_DTMF_CTL

  @apr_msgpayload{afe_ports_cmd_dtmf_ctl_t}
    @table{weak__afe__ports__cmd__dtmf__ctl__t}
    Following this structure in memory are num_ports number of port_ids. If
    num_ports is greater than one, the reserved field is one of the port_ids;
    otherwise, the reserved field must be set to 0.
    @par
    When multiple ports are involved, the acknowledgment indicates the status:
    - SUCCESS -- DTMF is initiated on all ports
    - FAILURE -- DTMF failed on one or more ports (might not be all ports)

  @detdesc
    When enabling DTMF on more than one port, specify num_ports with the number
    of ports where DTMF is to be enabled. The port ID of each port is to follow
    the port ID field in the command structure. @newpage
    @par
    For multiple ports, the return value of the command indicates whether there
    was a failure (DTMF might be successful on some ports and fail on others).
    If a return status is important, send the command to each port
    individually.
    @par
    This command is accepted only on ports with sampling rates of:
    - #AFE_PORT_SAMPLE_RATE_8K
    - #AFE_PORT_SAMPLE_RATE_16K
    - #AFE_PORT_SAMPLE_RATE_48K

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The port must be active.
     - If the port is inactive and a DTMF command is received, DTMF is not
       played.
     - If the port is active, a new command must be received to play DTMF
       later.
 */
#define AFE_PORTS_CMD_DTMF_CTL                                   0x00010102

/* Structure for the AFE_PORTS_CMD_DTMF_CTL command. */
typedef struct afe_ports_cmd_dtmf_ctl_t afe_ports_cmd_dtmf_ctl_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_ports_cmd_dtmf_ctl_t
@{ */
/* Payload of the AFE_PORTS_CMD_DTMF_CTL command, which starts DTMF on
    any port.
 */
struct afe_ports_cmd_dtmf_ctl_t
{
    int64_t                   dtmf_duration_in_ms;
    /**< Duration of the DTMF tone in milliseconds. Duration includes both
         ramp up and ramp down.

         @values
         - -1 -- Infinite tone
         - 0 -- Disable or stop infinite tone
         - > 0 -- Finite duration @tablebulletend */

    uint16_t                  dtmf_high_freq;
    /**< DTMF high tone frequency.

         @values 1209, 1336, 1477, 1633 Hz */

    uint16_t                  dtmf_low_freq;
    /**< DTMF low tone frequency.

         @values 697, 770, 852, 941 Hz */

    uint16_t                  dtmf_gain;
    /**< Gain applied to a DTMF tone.

         @values Any number (in Q13 format) */

    uint16_t                  num_ports;
    /**< Number of ports following this structure in memory.

         @values @ge 1 */

    uint16_t                  port_ids;
    /**< Destination ports (array).

         For DTMF on multiple ports, port_ids is populated num_ports times, and
         they follow this element. */

    uint16_t                  reserved;
    /**< This field must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /*end_weakgroup weak_afe_ports_cmd_dtmf_ctl_t */
/** @} */ /*end_addtogroup afe_cmd_start_dtmf */


#endif /* _ADSP_AFE_SERVICE_MODULES_PARAMS_H_ */