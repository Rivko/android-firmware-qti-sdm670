#ifndef GLINK_RPM_H
#define GLINK_RPM_H

/**
 * @file glink_rpm.h
 *
 * Public API for the GLink-RPM
 */

/** \defgroup glink GLink
 * \ingroup SMD
 *
 * GLink reliable, in-order, datagram-based interprocessor communication
 * over a set of supported transport (Shared Memory, UART, BAM, HSIC)
 *
 * All ports preserve message boundaries across the interprocessor channel; one
 * write into the port exactly matches one read from the port.
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/15   an      Added glink_wait_link_down
01/14/15   rli     Remove comdef.h 
10/28/14   bm      Initial release of GLink RPM API
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "stddef.h"

/*================= RESTRICTED API ==========================*/
/** Note that these API are only supported for RPM client. No other
 * client may use these as a generic API as the underlying implementation
 * works only for RPM ports */

 
/** 
 * This API allows the RPM client to poll the transport for any new data
 * that might have come in. It would *usually* be used when incoming interrupts
 * are disabled.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_rpm_rx_poll
(
  glink_handle_type handle
);

/** 
 * This API allows the RPM client to mask/unmask rx interrupts
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[in]   mask        Whether to mask or un-mask the incoming interrupt
 *
 * @param[in]   platform_struct Any platform specific into that transport may
                                require
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_rpm_mask_rx_interrupt
(
  glink_handle_type handle,
  boolean           mask,
  void              *platform_struct
);

/** 
 * This API waits until link is down.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_wait_link_down
(
  glink_handle_type handle
);

#ifdef __cplusplus
}
#endif

#endif //GLINK_RPM_H

