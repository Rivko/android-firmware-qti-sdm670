/*============================================================================
@file npa_remote_rpm_protocol.h

NPA Remote RPM Protocol API 

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
 
$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/remote/npa_remote_rpm_protocol.h#1 $
============================================================================*/
#ifndef NPA_REMOTE_RPM_PROTOCOL_H
#define NPA_REMOTE_RPM_PROTOCOL_H

/* Name of the RPM protocol resource */
#define RPM_PROTOCOL_RESOURCE_NAME "/protocol/rpm/rpm"

/* Sleep set updates resulting from NAS requests are deferred until next
 * wakeup, since applying them inline with the NAS requests would update
 * the sleep set based on the next, rather than current active set.
 * This function, invoked from the scheduler LPR exit function, applies
 * those deferred updates.
 */
void npa_rpm_do_deferred_updates( void ); 

#endif /* NPA_REMOTE_RPM_PROTOCOL_H */
