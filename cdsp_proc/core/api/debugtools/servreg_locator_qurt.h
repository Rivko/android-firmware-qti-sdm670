#ifndef SERVREG_LOCATOR_QURT_H
#define SERVREG_LOCATOR_QURT_H
/*
#============================================================================
#  Name:
#    servreg_locator_qurt.h 
#
#  Description:
#    Service Registry Locator feature.
#
# Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/

#include "stdlib.h"
#include "qurt.h"

#include "servreg_common.h"
#include "servreg_locator.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** =====================================================================
API, NHLOS Specific Structure
* =====================================================================  */
typedef struct
{
   qurt_anysignal_t* signal;
   unsigned int mask;
} SERVREG_LOC_SIGQURT;

/** =====================================================================
 * Function:
 *     servreg_register_servloc_availability_signal_qurt
 *
 * Description:
 *     Clients register to get notified when Service Locator feature is available
 *
 * Parameters:
 *     signal    : NHLOS specific notification signal information
 *     mask      : NHLOS specific notification mask information
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT static inline servreg_register_servloc_availability_signal_qurt(qurt_anysignal_t* signal, uint32_t mask)
{
   SERVREG_LOC_SIGQURT sigex;
   sigex.signal = signal;
   sigex.mask = mask;
   return servreg_register_servloc_availability_signal(SERVREG_SIGEX_TYPE_SIGQURT, &sigex);
}

#if defined(__cplusplus)
}
#endif

#endif