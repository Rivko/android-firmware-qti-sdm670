/*==========================================================================
Description
   Header function for declarations of platform related utils

# Copyright (c) 2016 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/


#ifndef _UTILS_H_
#define _UTILS_H_

typedef enum {
    BT_SOC_DEFAULT = 0,
    BT_SOC_SMD = BT_SOC_DEFAULT,
    BT_SOC_AR3K,
    BT_SOC_ROME,
    BT_SOC_CHEROKEE,
    /* Add chipset type here */
    BT_SOC_RESERVED
}bt_soc_type;

int get_bt_soc_type();

#endif//_UTILS_H_
