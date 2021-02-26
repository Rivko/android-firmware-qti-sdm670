#ifndef GLINK_QOS_H
#define GLINK_QOS_H

/**
 * @file glink_qos.h
 *
 * Public API for the GLink-QoS
 */

/** \defgroup glink GLink
 * \ingroup MPROC
 *
 * GLink reliable, in-order, datagram-based interprocessor communication
 * over a set of supported transports (Shared Memory, UART, BAM, HSIC)
 *
 * All ports preserve message boundaries across the interprocessor channel; one
 * write into the port exactly matches one read from the port.
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2015 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/mproc/glink_qos.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/15   an      Initial release of GLink QoS API
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"

/*================= RESTRICTED API ==========================*/
/** Note that these API are only supported for QoS clients. No other
 * client may use these as a generic API as it may lead to QoS rejection 
 * for latency-sensistive clients */

 
/** 
 * This API allows the client to register a latency QoS request with Glink.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 * @param[in]   latency_us  Requested packet latency in microseconds
 * @param[in]   pkt_size    Requested packet size
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_qos_latency
(
  glink_handle_type handle,
  uint32 latency_us,
  size_t pkt_size
);


/** 
 * This API allows the client to deregister QoS request with Glink.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_qos_cancel
(
  glink_handle_type handle
);


/** 
 * Client to indicate the start of transmission, accounting for the ramp-up time
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_qos_start
(
  glink_handle_type handle
);


/** 
 * Provides information about how much time G-Link requires to ramp up clocks 
 * or voltage rails to ensure the QoS requirement.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 * @param[in]   pkt_size    Requested packet size
 *
 * @return      QoS ram-up time in microseconds, -1 if QoS is not registered
 *
 * @sideeffects  None
 */
uint32 glink_qos_get_ramp_time
(
  glink_handle_type handle,
  uint32 latency_us,
  size_t pkt_size
);

#ifdef __cplusplus
}
#endif

#endif //GLINK_QOS_H

