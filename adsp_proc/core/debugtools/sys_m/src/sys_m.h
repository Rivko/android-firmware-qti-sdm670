#ifndef SYS_M_H
#define SYS_M_H
/*===========================================================================

           S Y S _ M . H

DESCRIPTION

Copyright (c) 2011-17 by Qualcomm Technologies Incorporated. All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/debugtools/sys_m/src/sys_m.h#2 $

===========================================================================*/
#include "qmi_csi.h"
#include "rcevt.h"
#include "tms_utils_msg.h"
#include "timer.h"

#define SYS_M_SMEM_SSR_REASON_LPASS0  SMEM_SSR_REASON_LPASS0
#define SYS_M_SMEM_SSR_REASON_DSPS0  SMEM_SSR_REASON_DSPS0
#define SMEM_SSR_REASON_MSS0  SMEM_SSR_REASON_MSS0

extern int sys_m_shutdown_delay_secs;

typedef enum SYS_M_COMMUNICATION_T
{
    SYS_M_SMSM_P2P       = 0,
    SYS_M_QMI            = 1
} SYS_M_COMMUNICATION_TYPE;




//begin shutdown
void sys_m_shutdown(SYS_M_COMMUNICATION_TYPE);

//shutdown function
void sys_m_shutdown_internal(void);

/**
  call the registered callback to release all SMEM spinlock
  after entering into STM while gracefull shutdown.

  @return
   TRUE : Success
   FALSE: Failure

  @dependencies
  None.
*/
void sys_m_shutdown_terminate_in_STM(timer_cb_data_type delay);

/**
 *
 * @brief sys_m_signal_modules
 *
 * Signal interested modules of a system monitor event.  After all clients have completed
 * notification, acknowledge to apps
 *
*/
void sys_m_signal_modules(RCEVT_HANDLE handle);


#define SYS_M_SMP2P_PORT_OUT        "slave-kernel"
#define SYS_M_SMP2P_PORT_IN         "master-kernel"
#define SYS_M_XPU_ERR_STRING        "sys_m:xpu_error"
#define SYS_M_SMP2P_TZ_PORT_IN      "tz2mpss_smp2p"


#endif  /* SYS_M_H */
