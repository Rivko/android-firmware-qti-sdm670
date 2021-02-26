/*===========================================================================

            GLink SMEM transport generic QoS header

=============================================================================

  @file
    xport_smem_qos_generic.h

    Defines generic Glink SMEM transport QoS routines.

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/xport_smem/inc/xport_smem_qos_generic.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/15   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_list.h"
#include "xport_smem_config.h"

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      xport_smem_get_req_rate_generic
===========================================================================*/
/**

  Provides data rate (mtus/sec) achievable by the transport for given 
  packet size and latency.

  @param[in]  if_ptr     Pointer to transport interface instance.
  @param[in]  latency_us latency in us.
  @param[in]  pkt_size   Size of the packet.

  @return     Data rate (mtus/sec).

  @sideeffects  None.
*/
/*=========================================================================*/
uint32 xport_smem_get_req_rate_generic
(
  glink_transport_if_type *if_ptr,
  uint32                  latency_us,
  size_t                  pkt_size   
);

/*===========================================================================
  FUNCTION      xport_smem_alloc_req_ctx_generic
===========================================================================*/
/**

  Stub.

  @param[in]  if_ptr     Pointer to transport interface instance.
  @param[in]  latency_us latency in us.
  @param[in]  pkt_size   Size of the packet.

  @return     non-NULL value.

  @sideeffects  None.
*/
/*=========================================================================*/
void* xport_smem_alloc_req_ctx_generic
(
  glink_transport_if_type *if_ptr,    
  uint32                  latency_us, 
  size_t                  pkt_size    
);

/*===========================================================================
  FUNCTION      xport_smem_free_req_ctx_generic
===========================================================================*/
/**

  Stub.

  @param[in]  if_ptr     Pointer to transport interface instance.
  @param[in]  qos_ctx    Pointer to the transport QoS context.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_smem_free_req_ctx_generic
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);

/*===========================================================================
  FUNCTION      xport_smem_start_req_generic
===========================================================================*/
/**

  Stub.

  @param[in]  if_ptr     Pointer to transport interface instance.
  @param[in]  qos_ctx    Pointer to the transport QoS context.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_smem_start_req_generic
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);

/*===========================================================================
  FUNCTION      xport_smem_stop_req_generic
===========================================================================*/
/**

  Stub.

  @param[in]  if_ptr     Pointer to transport interface instance.
  @param[in]  qos_ctx    Pointer to the transport QoS context.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_smem_stop_req_generic
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);

/*===========================================================================
  FUNCTION      xport_smem_get_ramp_time_generic
===========================================================================*/
/**

  Stub.

  @param[in]  if_ptr     Pointer to transport interface instance.
  @param[in]  qos_ctx    Pointer to the transport QoS context.

  @return     0.

  @sideeffects  None.
*/
/*=========================================================================*/
uint32 xport_smem_get_ramp_time_generic
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);
