/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "interface.h"
#include "view.h"
#include "utils.h"

/*
* purpose
*      check usb online status
*
* requires
*
* guarantees
*       read value = /sys/class/power_supply/usb/present
*       return true when value=1
*/

bool is_usb_attached() {
    char buf[NAME_MAX];
    int ret;

    ret = read_file(SYS_USB_ONLINE, buf, sizeof(buf));
    if(ret < 0) {
        MMI_ALOGE("unable to open power_supply/usb/online file\n");
        return false;
    }

    MMI_ALOGI("usb online is %s\n", buf);
    if(strncmp(buf, "1", 1)) {
        MMI_ALOGE("USB is not online during init\n");
        return false;
    }

    return true;
}

/*
* purpose
*     enter ship mode
*
* requires
*
* guarantees
*       set /sys/module/qpnp_power_on/parameters/ship_mode_en = 1
*       return true when the value set
*/
bool enter_ship_mode() {
    char buf[NAME_MAX];
    int ret;

    /**set ship mode*/
    ret = write_file(SYS_SHIP_MODE, "1");
    if(ret < 0) {
        MMI_ALOGE("unable to open power_supply/usb/online file\n");
        return false;
    }

    /**confirm the  value*/
    ret = read_file(SYS_SHIP_MODE, buf, sizeof(buf));
    if(ret < 0) {
        MMI_ALOGE("unable to open power_supply/usb/online file\n");
        return false;
    }

    MMI_ALOGI("value from ship mode enable  is %s\n", buf);
    if(strcmp(buf, "1")) {
        MMI_ALOGI("USB is not online during init\n");
        return false;
    }

    return true;
}

/*
* purpose
*    enter suspend
*
* requires
*
* guarantees
*       set /sys/power/state = mem
*       return true when the value set
*/
bool enter_suspend() {
    int ret;

    /**enter suspend mode*/
    ret = write_file(SYS_POWER_STATE, "mem");
    if(ret < 0) {
        MMI_ALOGE("unable to enter_suspend \n");
        return false;
    }

    return true;
}
/*
* purpose
*    LCD on / off operate
*
* requires
*    true -  turn off LCD
*    false - turn on LCD
*
* guarantees
*       call setDisplayPowerMode
*       return true when the value set
*/

void set_display_power_mode(int mode) {
    sp < IBinder > dtoken(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    if(dtoken == NULL)
        return;

    MMI_ALOGI("set display power mode to : %d\n", mode);
    SurfaceComposerClient::setDisplayPowerMode(dtoken, mode);
}
