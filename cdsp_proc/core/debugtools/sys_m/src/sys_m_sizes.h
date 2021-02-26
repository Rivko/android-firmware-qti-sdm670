#ifndef SYS_M_SIZES_H
#define SYS_M_SIZES_H
/*===========================================================================

           S Y S _ M _ SIZES . H

DESCRIPTION

Copyright (c) 2015-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/sys_m/src/sys_m_sizes.h#1 $

===========================================================================*/
#include "subsystem_control_v02.h"

#define EXTRA_SSR_CHAR_ADDN         6
#define MAX_EVENT_SIZE              20
#define ERR_SSR_REASON_SIZE_BYTES   256
#define SUBSYS_EVT_SIZE (QMI_SSCTL_SUB_SYS_NAME_LENGTH_V02 + MAX_EVENT_SIZE + EXTRA_SSR_CHAR_ADDN)

#define SYS_M_DEVICE_NAME_LEN       16
#define SYS_M_LISTEN_WORKER_STACK   2048   /**< Performs Tech Area Supplied Callbacks */

#define SYS_M_NAME_MAX              32     /**< SYS_M_NAME Maximum Length */

#endif  /* SYS_M_SIZES_H */
