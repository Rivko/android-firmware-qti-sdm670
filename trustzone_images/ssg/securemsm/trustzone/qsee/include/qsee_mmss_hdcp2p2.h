#ifndef QSEE_MMSS_HDCP2P2_H
#define QSEE_MMSS_HDCP2P2_H

/*============================================================================
Copyright (c) 2015-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsee/include/qsee_mmss_hdcp2p2.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     --------------------------------------------------------
06/29/16   sh      add mink api internal function qsee_write_key_riv_enable()
04/06/15   sk      Initial Revision

=========================================================================== */
#include <comdef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    HDCP_VERSION_1_4 = 0x2001,
    HDCP_VERSION_2_2 = 0x2002
} qsee_hdcp_version_t;

typedef enum {
    HDCP_TX_HDMI = 0x8001,
    HDCP_TX_DP   = 0x8002
} qsee_hdcp_device_t;

/*
  Set/reset and enable/disable the key and riv for hdcp-hdmi
  Parameters:
    hdcpVersion   - (in) HDCP version to write key
    deviceType    - (in) HDCP HW type(hdmi,dp) to write key
    enableEncryption- (in) true for enable; false for disable
    key           - (in) key to write on H/W in HDCP registers
    keyLen        - (in) length of key
    riv           - (in) random IV to set to HDCP h/w
    rivLen        - (in) length of random IV   
*/
int32_t qsee_write_hdcp_key_riv_enable(const uint32_t hdcpVersion,
                                       const uint32_t deviceType,
                                       const bool enableEncryption,
                                       const void *key, size_t keyLen,
                                       const void *riv, size_t rivLen);

#endif /*QSEE_MMSS_HDCP2P2_H*/

