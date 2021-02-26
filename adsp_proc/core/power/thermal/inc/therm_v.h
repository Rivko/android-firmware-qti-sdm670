#ifndef THERM_V_H
#define THERM_V_H

/*============================================================================
  FILE:         therm_v.h

  OVERVIEW:     This file declares the private interface to thermal SW.

                Copyright (c) 2011-2013, 2015, 2017 QUALCOMM Technologies
                Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1/power/thermal/inc/therm_v.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "qmi_csi.h"
#include "npa_resource.h"

/*
===============================================================================

                             TYPE DEFINITION(S)

===============================================================================
*/
#define THERM_QMI_MITIGATION_DEV_SIG      0x00000001
#define THERM_QMI_TMD_COLD_RSTR_TIMER_SIG 0x00000002

#define THERM_TASK_SIG_MASK (THERM_QMI_MITIGATION_DEV_SIG | \
                             THERM_QMI_TMD_COLD_RSTR_TIMER_SIG)


/*===========================================================================

                     EXTERNAL VARIABLE DECLARATIONS

===========================================================================*/


/*=============================================================================

                           GLOBAL FUNCTION DECLARATIONS

=============================================================================*/
/**
  @brief therm_target_init

  Initializes target specific thermal SW.

*/
extern void therm_target_init(void);

/**
  @brief therm_target_after_task_start_init

  Initialize target specific thermal SW after thermal
  task has started.
*/
extern void therm_target_after_task_start_init(void);

/**
  @brief therm_target_process_sigs

  Process rex signals for target specific SW.

  @param rcvdSigs
*/
extern void therm_target_process_sigs(unsigned int rcvdSigs,
                                      qurt_signal_t *qurt_signal);

/**
  @brief therm_qmi_dev_service_init

  This function initializes the QMI thermal mitigation device
  services handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
extern qmi_csi_service_handle therm_qmi_dev_service_init(qmi_csi_os_params *os_params,
                                                         qurt_signal_t *qurt_signal);

/**
  @brief therm_qmi_dev_service_process_sigs

  Process rex signals for qmi dev service.

  @param rcvdSigs
*/
extern void therm_qmi_dev_service_process_sigs(unsigned int   rcvdSigs,
                                               qurt_signal_t *qurt_signal);

/**
  @brief therm_qmi_sensor_service_init

  This function initializes the QMI thermal sensors services
  handler.

  @param  : os_params: OS parameters.

  @return : qmi_csi_service_handle.
*/
qmi_csi_service_handle therm_qmi_sensor_service_init(qmi_csi_os_params *os_params,
                                                     qurt_signal_t     *qurt_signal);

#ifdef __cplusplus
}
#endif

#endif /* THERM_V_H */
