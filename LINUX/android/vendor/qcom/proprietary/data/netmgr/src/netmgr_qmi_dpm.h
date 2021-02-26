#ifndef _NETMGR_QMI_DPM_H_
#define _NETMGR_QMI_DPM_H_
/**
  @file
  netmgr_qmi_dpm.h

  @brief
  Netmgr QMI DPM (QMI Data Port Mapper service) component.

  @details
  Netmgr QMI DPM component is used to configure embedded ports on the modem.
  The configuration specifies how many ports to open, what are the control
  ports and what are the data ports (hw-accel and software data ports). It
  can also specify the mapping between control and data ports. The configuration
  is choosen through configdb or hard-coded.

  The following functions are exported:
  netmgr_qmi_dpm_open_port()
  netmgr_qmi_dpm_close_port()

  The ports are opened using QCCI APIs (they must go over IPC-router since
  QMUXD transport would not be ready yet).

*/
/*===========================================================================

  Copyright (c) 2013,2016 Qualcomm Technologies, Inc. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/13   hm/nd   Initial version

===========================================================================*/

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

typedef enum
{
  NETMGR_QMI_DPM_INVALID_EV = -1,
  NETMGR_QMI_DPM_OOS_EV,
  NETMGR_QMI_DPM_IS_EV,
  NETMGR_QMI_DPM_MAX_EV
} netmgr_dpm_event_t;

/*===========================================================================
  FUNCTION:  netmgr_qmi_dpm_initialize
===========================================================================*/
/** @ingroup netmgr_dpm

    @brief
    Initializes the QMI DPM module.

    @details
    Performs the QCCI client library initialization and sends the open
    request to the Modem

    @params None.
    @return NETMGR_SUCCESS: in case of success.
    @return NETMGR_FAILURE: in case of error.
    @dependencies None.
*/
void netmgr_qmi_dpm_initalize(void);


/*===========================================================================
  FUNCTION:  netmgr_qmi_dpm_initialize_blocking
===========================================================================*/
/** @ingroup netmgr_dpm

    @brief
    Initializes the QMI DPM module. Use this function only from the main
    thread init sequence and not anywhere else.

    @details
    This is a wrapper function netmgr_qmi_dpm_initialize_main over
    netmgr_qmi_dpm_initialize and this function only additionally
    waits for the DPM service to be UP before rquesting a service
    client from QCCI. This is to avoid tight looping in the cases
    where the modem is not presumed to be UP and running always.

    @params None.
    @return NETMGR_SUCCESS: in case of success.
    @return NETMGR_FAILURE: in case of error.
    @dependencies None.
*/
int netmgr_qmi_dpm_initialize_blocking(void);

/*===========================================================================
  FUNCTION:  netmgr_qmi_dpm_log_state
===========================================================================*/
/** @ingroup netmgr_dpm

    @brief
    Prints debug messages about QMI DPM state in netmgr.

    @details
    Prints information on whether QMI DPM is inited, what was the last
    message sent etc.

    @params None.
    @return None.
*/
void netmgr_qmi_dpm_log_state(void);

/*===========================================================================
  FUNCTION:  netmgr_qmi_dpm_process_sys_event
===========================================================================*/
/** @ingroup netmgr_dpm

    @brief
    Called in response to DPM events as part of command executor module

    @params None.
    @return None.
*/
/*=========================================================================*/
void netmgr_qmi_dpm_process_sys_event(netmgr_dpm_event_t evt);

#endif /* _NETMGR_QMI_DPM_H_ */

