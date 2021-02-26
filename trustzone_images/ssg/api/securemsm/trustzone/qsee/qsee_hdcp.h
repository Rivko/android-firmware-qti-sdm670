/*===========================================================================
 *  Copyright (c) 2017 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *=========================================================================*/

#ifndef __QSEE_HDCP_H
#define __QSEE_HDCP_H

#include <stdbool.h>
#include <stdint.h>

/* Function used to reference count the enforcement of HDMI HDCP encryption
 * Parameters:
 *    enforce (in) Enforcement vote (0 or 1)
 */
int qsee_enforce_hdmi_hdcp_encryption(unsigned int enforce);

/* Function used to set the enablement of CPI assert
 * Parameters:
 *    cpi_assert_enable  (in) TRUE for enable; FALSE for disable
 */
int qsee_set_hdcp_cpi_assert(bool cpi_assert_enable);

/* Function used to acquire the status of the HDMI link and hardware HDCP
 * Parameters:
 *    hdmi_enable  (out) HDMI output enabled status return location
 *    hdmi_sense   (out) Panel Hot Plug Detect connect status return location
 *    hdcp_auth    (out) HDCP authentication status return location
 */
int qsee_hdmi_status_read(uint32_t *hdmi_enable, uint32_t *hdmi_sense,
                          uint32_t *hdcp_auth);

#endif //__QSEE_HDCP_H
