/*=========================================================================
  wbc_hal_interface.h
  DESCRIPTION
  API declarations for Wipower Battery Control HAL

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  =========================================================================*/

#ifndef WBC_HAL_INTERFACE_H
#define WBC_HAL_INTERFACE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Status return values */
typedef enum {
    //WBC_STATUS_OK = 0,
    //WBC_STATUS_ERROR,
    WBC_ERROR_NONE = 0,
    WBC_ERROR_UNDEFINED
} wbc_error_t;

typedef wbc_error_t wbc_status_t;

/* Indicates whether the device has Wipower */
typedef enum {
    WBC_WIPOWER_INCAPABLE = 0,
    WBC_WIPOWER_CAPABLE,
} wbc_wipower_capable_t;

/* WiPower PTU Presence */
typedef enum {
    WBC_PTU_STATUS_NOT_PRESENT = 0,
    WBC_PTU_STATUS_PRESENT,
} wbc_ptu_presence_t;

/* WiPower charging status */
typedef enum {
    WBC_WIPOWER_STATUS_NOT_CHARGING = 0,
    WBC_WIPOWER_STATUS_CHARGING_ACTIVE,
} wbc_wipower_charging_t;

/* WiPower charging complete status */
typedef enum {
    WBC_BATTERY_STATUS_CHARGE_NOT_COMPLETE = 0,
    WBC_BATTERY_STATUS_CHARGE_COMPLETE,
} wbc_chg_complete_t;

/* USB wire charging status */
typedef enum {
    WBC_USB_CHARGING_NOT_PRESENT = 0,
    WBC_USB_CHARGING_PRESENT,
} wbc_usb_charging_present_t;

/* Battery over heat status */
typedef enum {
    WBC_BATTERY_NOT_OVERHEAT = 0,
    WBC_BATTERY_OVERHEAT,
} wbc_battery_overheat_t;

/* Event type used in callback notification */
typedef enum {
    WBC_EVENT_TYPE_WIPOWER_CAPABLE_STATUS = 1,
    WBC_EVENT_TYPE_PTU_PRESENCE_STATUS,
    WBC_EVENT_TYPE_WIPOWER_CHARGING_ACTIVE_STATUS,
    WBC_EVENT_TYPE_CHARGE_COMPLETE,
    WBC_EVENT_TYPE_USB_CHARGING_PRESENT,
    WBC_EVENT_TYPE_BATTERY_OVERHEAT,
} wbc_event_type_t;

/* WBC callback event */
typedef struct {
    wbc_event_type_t event_type;
    union {
        wbc_wipower_capable_t  wipower_capable;
        wbc_ptu_presence_t     ptu_presence;
        wbc_wipower_charging_t wipower_charging;
        wbc_chg_complete_t     chg_complete;
        wbc_usb_charging_present_t     usb_charging_present;
        wbc_battery_overheat_t     battery_overheat;
    } u;
} wbc_event_t;

/* WBC Callback function for notifying client */
typedef void (wbc_callback_t) (wbc_event_t * p_event);

/* WBC Lib Interface */
typedef struct {
    size_t size;
    wbc_error_t (*wbc_init)(wbc_callback_t *pCallbackFunc);
    wbc_error_t (*wbc_finish)(void);
    void (*wbc_hal_echo)(int value);
    wbc_wipower_capable_t (*wbc_get_wipower_capable)(void);
    wbc_ptu_presence_t (*wbc_get_ptu_presence)(void);
    wbc_wipower_charging_t (*wbc_get_wipower_charging)(void);
    wbc_chg_complete_t (*wbc_get_chg_complete)(void);
    wbc_usb_charging_present_t (*wbc_get_usb_charging_present)(void);
    wbc_battery_overheat_t (*wbc_get_battery_overheat)(void);
} wbc_lib_interface_t;

/* Entry point of DLib */
extern const wbc_lib_interface_t WBC_LIB_INTERFACE;

#ifdef __cplusplus
}
#endif

#endif /* WBC_HAL_INTERFACE_H */

