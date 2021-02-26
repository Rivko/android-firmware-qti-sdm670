/*===========================================================================

              RPM native transport UEFI specific features


  Copyright (c) 2014-2016 by QUALCOMM Technologies Incorporated.  
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/xport_rpm/src/xport_rpm_uefi.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/16   ap      Initial version.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_core_if.h"
#include "glink_transport_if.h"
#include "glink_os_utils.h"
#include "glink_internal.h"
#include "glink_vector.h"
#include "glink_rpm.h"
#include "xport_rpm_config.h"
#include "xport_rpm.h"

/* RPM channel descriptor */
typedef struct _xport_rpm_ind_type
{
  uint32 read_ind;
  uint32 write_ind;
} xport_rpm_ind_type;

/* RPM transport context */
typedef struct _xport_rpm_ctx_type
{
  /* context structure should start from trasport interface */
  glink_transport_if_type xport_if; 
  const xport_rpm_config_type *pcfg;
  volatile xport_rpm_ind_type* tx_desc;
  volatile xport_rpm_ind_type* rx_desc;
  char* tx_fifo;
  char* rx_fifo;
  uint32 tx_fifo_size;
  uint32 rx_fifo_size;
  os_cs_type *tx_link_lock;
  os_cs_type *rx_link_lock;
  uint32 pkt_start_ind;
  uint32 pkt_size;
  boolean reset;
  boolean irq_mask;
} xport_rpm_ctx_type;

/*===========================================================================
FUNCTION      glink_wait_link_down
===========================================================================*/
/**

  Returns true if the link is down

  @param[in]  ctx_ptr   Pointer to transport context.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type glink_wait_link_down
(
  glink_handle_type handle
)
{
  xport_rpm_ctx_type *ctx_ptr;

  ASSERT(handle && handle->if_ptr);

  ctx_ptr = (xport_rpm_ctx_type *)handle->if_ptr;

  return (ctx_ptr->tx_desc->write_ind == 0 && 
          ctx_ptr->tx_desc->read_ind == 0  &&
          ctx_ptr->rx_desc->write_ind == 0 &&
          ctx_ptr->rx_desc->read_ind == 0);
}

