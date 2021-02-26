/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __INTERFACE__
#define __INTERFACE__
#include "common.h"

#define SYS_USB_ONLINE "/sys/class/power_supply/usb/present"
#define SYS_SHIP_MODE "/sys/module/qpnp_power_on/parameters/ship_mode_en"
#define SYS_POWER_STATE "/sys/power/state"

bool is_usb_attached();
bool enter_ship_mode();
void set_display_power_mode(int mode);
bool enter_suspend();
#endif
