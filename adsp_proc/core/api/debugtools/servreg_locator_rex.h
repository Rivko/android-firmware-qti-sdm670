#ifndef SERVREG_LOCATOR_REX_H
#define SERVREG_LOCATOR_REX_H
/*
#============================================================================
#  Name:
#    servreg_locator_rex.h 
#
#  Description:
#    Service Registry Locator feature.
#
# Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/

#include "stdlib.h"
#include "rex.h"

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
   rex_tcb_type* signal;
   rex_sigs_type mask;
} SERVREG_SIGEX_SIGREX;

/** =====================================================================
 * Function:
 *     servreg_register_servloc_availability_signal_rex
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
SERVREG_RESULT static inline servreg_register_servloc_availability_signal_rex(rex_sigs_type mask)
{
   SERVREG_SIGEX_SIGREX sigex;
   sigex.signal = rex_self();
   sigex.mask = mask;
   return servreg_register_servloc_availability_signal(SERVREG_SIGEX_TYPE_SIGREX, &sigex);
}

#if defined(__cplusplus)
}
#endif

#endif
