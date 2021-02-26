/*=============================================================================

FILE:         HalQdssTFunnelConfig.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#ifndef HALQDSSTFUNNELCONFIG_H_
#define HALQDSSTFUNNELCONFIG_H_

#define QDSS_IN_FUN0_BASE   (0x06041000)
#define QDSS_IN_FUN1_BASE   (0x06042000)
#define QDSS_MERG_FUN_BASE  (0x06045000)

//Funnel configuration is platform specific.
//We need only this configuration to enable STM traffic.
#define QDSS_IN_FUN0        0  // QDSS_IN_FUN0_BASE
#define QDSS_IN_FUN1        1
#define QDSS_MERG_FUN       2  // QDSS_MERG_FUN_BASE
#define QDSS_NUM_FUNNELS    3

#define QDSS_STM_IN_FUN       0   // STM Funnel ID
#define QDSS_IN_FUN_STM_PORT  7   // STM Funnel Port ID


#endif  /* HALQDSSTFUNNELCONFIG_H_ */
