#ifndef SERVREG_MONITOR_REX_H
#define SERVREG_MONITOR_REX_H
/*
#============================================================================
#  Name:
#    servreg_monitor_rex.h 
#
#  Description:
#    Service Registry Monitor(srm) feature.
#    Clients register to get event and sync notifications for services they are
#    interested in. 
#
# Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "rex.h"

#include "servreg_common.h"
#include "servreg_monitor.h"

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
} SERVREG_MON_SIGREX;

/** =====================================================================
 * Function:
 *     servreg_register_listener_rex
 *
 * Description:
 *     Register a listener to get event notifications for a service it is interested in
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name. This is the service that the listener is interested in.
 *     signal        : NHLOS specific notification signal information
 *     mask          : NHLOS specific notification mask information
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT static inline servreg_register_listener_rex(SERVREG_MON_HANDLE sr_mon_handle, rex_sigs_type mask)
{
   SERVREG_MON_SIGREX sigex;
   sigex.signal = rex_self();
   sigex.mask = mask;
   return servreg_register_listener(sr_mon_handle, SERVREG_SIGEX_TYPE_SIGREX, &sigex);
}

/** =====================================================================
 * Function:
 *     servreg_deregister_listener_rex
 *
 * Description:
 *     De-Register a listener to get event notifications for a service
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *     signal        : NHLOS specific notification signal information
 *     mask          : NHLOS specific notification mask information
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 *
 * Note : 
 *     If clients are no longer interested in the service AND they have de-registered their listeners
 *     using servreg_deregister_listener(), they can free the handle by calling servreg_free_monitor_handle().
 * =====================================================================  */
SERVREG_RESULT static inline servreg_deregister_listener_rex(SERVREG_MON_HANDLE sr_mon_handle, rex_sigs_type mask)
{
   SERVREG_MON_SIGREX sigex;
   sigex.signal = rex_self();
   sigex.mask = mask;
   return servreg_deregister_listener(sr_mon_handle, SERVREG_SIGEX_TYPE_SIGREX, &sigex);
}

#if defined(__cplusplus)
}
#endif

#endif
