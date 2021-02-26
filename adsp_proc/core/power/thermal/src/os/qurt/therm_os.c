/*============================================================================
  @file therm_os.c

  Thermal OS specific functionality.  Task init.

  Copyright (c) 2011-2013,2017 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1/power/thermal/src/os/qurt/therm_os.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "qurt.h"
#include "therm_v.h"
#include "qmi_csi.h"
#include "rcinit.h"

#include "DDIPlatformInfo.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

extern qurt_thread_t thermal_tcb;

static qurt_signal_t therm_qmi_req_signal_qurt;

/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief therm_task

  Thermal Mitigation task.

  @param ignored: Ignored.
*/
void thermal_task
(
  dword ignored
     /* Parameter received from Rex - ignored */
     /*lint -esym(715,ignored)
     ** Have lint not complain about the ignored parameter 'ignored' which is
     ** specified to make this routine match the template for rex_init().
     */
)
{
  if (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI ||
      DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_VIRTIO)
  {
    /* Exit thermal task on RUMI platforms.  Init must return after this handshake call.*/
    rcinit_handshake_startup();
    return;
  }

  qmi_csi_os_params      os_params;
  qmi_csi_service_handle service_handle = {NULL};

  unsigned int rcvdSigs = 0;   /* signals returned from REX */

  /*  Initialize the limits system on this target. */
  therm_target_init();

  /* Start the task */
  rcinit_handshake_startup(); // required rcinit handshake

  qurt_anysignal_init(&therm_qmi_req_signal_qurt);
  os_params.signal = &therm_qmi_req_signal_qurt;
  os_params.sig = THERM_QMI_MITIGATION_DEV_SIG;

  /* Initialize after task start on this target. */
  therm_target_after_task_start_init();

  service_handle = therm_qmi_dev_service_init(&os_params, &therm_qmi_req_signal_qurt);

  while( 1 )
  {
    /* Wait for new signals to be received. */
    rcvdSigs = qurt_signal_wait(&therm_qmi_req_signal_qurt, THERM_TASK_SIG_MASK,
                                QURT_SIGNAL_ATTR_WAIT_ANY);

    /* Process Qurt signal */
    therm_target_process_sigs(rcvdSigs, &therm_qmi_req_signal_qurt);

    therm_qmi_dev_service_process_sigs(rcvdSigs, &therm_qmi_req_signal_qurt);


    if (rcvdSigs & THERM_QMI_MITIGATION_DEV_SIG)
    {
      qurt_signal_clear(&therm_qmi_req_signal_qurt, THERM_QMI_MITIGATION_DEV_SIG);
      qmi_csi_handle_event(service_handle, &os_params);
    }
  }
}

