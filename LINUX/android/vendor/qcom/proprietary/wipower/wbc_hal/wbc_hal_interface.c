/*=========================================================================
  wbc_hal_interface.c
  DESCRIPTION
  Implementation of Wipower Battery Control HAL

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  =========================================================================*/

/*=========================================================================
  INCLUDES
  =========================================================================*/
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <cutils/uevent.h>
#include <cutils/properties.h>
#include "wbc_hal_interface.h"
#include "wbc_psy.h"

#define LOG_TAG "wbc_hal"
#include <cutils/log.h>

#define MAX_PROP_LEN    32
#define DEBUG

/*=========================================================================
  DECLARATIONS AND DEFINITIONS
  =========================================================================*/

struct wbc_hal_state_info {
	wbc_wipower_capable_t		wipower_capable;
	wbc_ptu_presence_t			ptu_presence;
	wbc_wipower_charging_t		wipower_charging;
	wbc_chg_complete_t			chg_complete;
	wbc_usb_charging_present_t	usb_charging_present;
	wbc_battery_overheat_t		battery_overheat;
};

struct psy_prop_status {
	bool	battery_present;
	bool	battery_therm_mitigation;
	bool	battery_charging;
	bool	battery_full;

	bool	usb_present;
	bool	dc_present;
	bool	is_dc_wipower;

	bool    battery_overheat;
};

struct wbc_hal_instance {
	wbc_callback_t			*pClientCallbackFunc;
	bool				is_inited;
	pthread_t			uevent_thread;
	int				log_level;
	struct wbc_hal_state_info	si;
	struct psy_prop_status		props;
};
static struct wbc_hal_instance hal_instance;

/*=========================================================================
  FUNCTION DEFINTIONS
  =========================================================================*/
static void dump_props(struct psy_prop_status *props)
{
	if (hal_instance.log_level) {
		ALOGD("%s %d\n", "battery_present", props->battery_present);
		ALOGD("%s %d\n", "battery_therm_mitigation", props->battery_therm_mitigation);
		ALOGD("%s %d\n", "battery_charging", props->battery_charging);
		ALOGD("%s %d\n", "battery_full", props->battery_full);
		ALOGD("%s %d\n", "usb_present", props->usb_present);
		ALOGD("%s %d\n", "dc_present", props->dc_present);
		ALOGD("%s %d\n", "is_dc_wipower", props->is_dc_wipower);
	}
}

static void dump_stateinfo(struct wbc_hal_state_info *si)
{
	if (hal_instance.log_level) {
		ALOGD("%s %d\n", "wipower_capable", si->wipower_capable);
		ALOGD("%s %d\n", "ptu_presence", si->ptu_presence);
		ALOGD("%s %d\n", "wipower_charging", si->wipower_charging);
		ALOGD("%s %d\n", "chg_complete", si->chg_complete);
		ALOGD("%s %d\n", "usb_charging_present", si->usb_charging_present);
		ALOGD("%s %d\n", "battery overheat", si->battery_overheat);
	}
}

static void determine_wipower_capable(struct psy_prop_status *props,
						struct wbc_hal_state_info *si)
{
	if (props->is_dc_wipower == true)
		si->wipower_capable = WBC_WIPOWER_CAPABLE;
	else
		si->wipower_capable = WBC_WIPOWER_INCAPABLE;
}

static void determine_ptu_presence(struct psy_prop_status *props,
						struct wbc_hal_state_info *si)
{
	if (props->is_dc_wipower == true && props->dc_present == true)
		si->ptu_presence = WBC_PTU_STATUS_PRESENT;
	else
		si->ptu_presence = WBC_PTU_STATUS_NOT_PRESENT;
}

static void determine_wipower_charging(struct psy_prop_status *props,
						struct wbc_hal_state_info *si)
{
	if (props->is_dc_wipower
		&& props->dc_present
		&& !props->usb_present
		&& props->battery_present
		&& props->battery_charging)
		si->wipower_charging = WBC_WIPOWER_STATUS_CHARGING_ACTIVE;
	else
		si->wipower_charging = WBC_WIPOWER_STATUS_NOT_CHARGING;
}

static void determine_chg_complete(struct psy_prop_status *props,
						struct wbc_hal_state_info *si)
{
	if (props->is_dc_wipower
		&& props->battery_present
		&& props->battery_full)
		si->chg_complete = WBC_BATTERY_STATUS_CHARGE_COMPLETE;
	else
		si->chg_complete = WBC_BATTERY_STATUS_CHARGE_NOT_COMPLETE;
}

static void determine_usb_charging_present(struct psy_prop_status *props,
						struct wbc_hal_state_info *si)
{
	if (props->usb_present)
		si->usb_charging_present = WBC_USB_CHARGING_PRESENT;
	else
		si->usb_charging_present = WBC_USB_CHARGING_NOT_PRESENT;
}

static void determine_battery_overheat(struct psy_prop_status *props,
						struct wbc_hal_state_info *si)
{
	if (props->battery_overheat)
		si->battery_overheat = WBC_BATTERY_OVERHEAT;
	else
		si->battery_overheat = WBC_BATTERY_NOT_OVERHEAT;
}

static bool is_battery_charging(const char *event)
{
	if (!strncmp(event, "Charging", 8))
		return true;
	else
		return false;
}

static bool is_battery_full(const char *event)
{
	if (!strncmp(event, "Full", 4))
		return true;
	else
		return false;
}

static bool is_battery_overheating(const char *event)
{
    if (!strncmp(event, "Overheat", 8))
        return true;
    return false;
}

static bool is_dc_wipower(const char *event)
{
	if (!strncmp(event, "Wipower", 7)) {
		return true;
	}
	return false;
}

static bool is_battery_therm_mitigation(int val)
{
	if (val == 3)
		return true;
	else
		return false;
}

static void parse_battery(struct psy_prop_status *props, const char *const event, int nbytes)
{
	const char *p = event;

	while (p - event < nbytes) {
		if (!strncmp(p, PSY_STATUS, strlen(PSY_STATUS))) {
			p += strlen(PSY_STATUS);
			props->battery_charging = is_battery_charging(p);
			props->battery_full = is_battery_full(p);
		} else if (!strncmp(p, PSY_PRESENT, strlen(PSY_PRESENT))) {
			p += strlen(PSY_PRESENT);
			props->battery_present = atoi(p);
		} else if (!strncmp(p, PSY_HEALTH, strlen(PSY_HEALTH))) {
			p += strlen(PSY_HEALTH);
			props->battery_overheat = is_battery_overheating(p);
		} else if (!strncmp(p, PSY_SYSTEM_TEMP_LEVEL, strlen(PSY_SYSTEM_TEMP_LEVEL))) {
			p += strlen(PSY_SYSTEM_TEMP_LEVEL);
			props->battery_therm_mitigation
				= is_battery_therm_mitigation(atoi(p));
		}
		p += strlen(p) + 1; //move to next token after '\0'
	}
}

static void parse_usb(struct psy_prop_status *props, const char *const event, int nbytes)
{
	const char *p = event;

	while (p - event < nbytes) {
		if (!strncmp(p, PSY_PRESENT, strlen(PSY_PRESENT))) {
			p += strlen(PSY_PRESENT);
			props->usb_present = atoi(p);
		}
		p += strlen(p) + 1; //move to next token after '\0'
	}
}

static void parse_dc(struct psy_prop_status *props, const char *const event, int nbytes)
{
	const char *p = event;

	while (p - event < nbytes) {
		if (!strncmp(p, PSY_PRESENT, strlen(PSY_PRESENT))) {
			p += strlen(PSY_PRESENT);
			props->dc_present = atoi(p);
		} else if (!strncmp(p, PSY_REAL_TYPE, strlen(PSY_REAL_TYPE))) {
			p += strlen(PSY_REAL_TYPE);
			props->is_dc_wipower = is_dc_wipower(p);
		}
		p += strlen(p) + 1; //move to next token after '\0'
	}
}

static void* uevent_thread_func(void *param)
{
	struct wbc_hal_instance *hal_instance
		= (struct wbc_hal_instance *)param;
	struct wbc_hal_state_info prev_si;
	const int RCV_BUF_MAX_SIZE = 64 * 1024;
	const int EVT_MAX_LEN = 4 * 1024;
	char event[EVT_MAX_LEN + 1];
	int ueventFd;
	ssize_t nbytes;
	wbc_event_t wbc_event;

	ueventFd = wbc_uevent_open_socket(RCV_BUF_MAX_SIZE, true);
	if (ueventFd < 0) {
		ALOGE("Err, uevent_open_socket failed, errno:%d", errno);
		return NULL;
	}

	for (;;) {
		nbytes = uevent_kernel_multicast_recv(ueventFd,
				event, EVT_MAX_LEN);
		if (nbytes <= 0) {
			continue;
		} else if (nbytes > EVT_MAX_LEN) {
			// too big data
			continue;
		}
		event[nbytes] = '\0';
		if (strstr(event, "battery")) {
			parse_battery(&hal_instance->props, event, nbytes);
		} else if (strstr(event, "usb")) {
			parse_usb(&hal_instance->props, event, nbytes);
		} else if (strstr(event, "dc")) {
			parse_dc(&hal_instance->props, event, nbytes);
		} else {
			continue;
		}

		prev_si = hal_instance->si;

		determine_usb_charging_present(&hal_instance->props, &hal_instance->si);
		determine_battery_overheat(&hal_instance->props, &hal_instance->si);
		determine_wipower_capable(&hal_instance->props, &hal_instance->si);
		determine_ptu_presence(&hal_instance->props, &hal_instance->si);
		determine_wipower_charging(&hal_instance->props, &hal_instance->si);
		determine_chg_complete(&hal_instance->props, &hal_instance->si);

		dump_props(&hal_instance->props);
		dump_stateinfo(&hal_instance->si);

		if (prev_si.usb_charging_present != hal_instance->si.usb_charging_present) {
			wbc_event.event_type = WBC_EVENT_TYPE_USB_CHARGING_PRESENT;
			wbc_event.u.usb_charging_present = hal_instance->si.usb_charging_present;
			hal_instance->pClientCallbackFunc(&wbc_event);
		}
		if (prev_si.battery_overheat != hal_instance->si.battery_overheat) {
			/*
			 * Need to repeatedly generate Over Temp Protection events due to the 3x recoverable
			 * latching faults.
			 * TODO: If an Over Temp Protection event is generated only on the "OTP" state change
			 * then only the first Over Temp Protection will cause an alert. Subsequent Over Temp
			 * Protection on MTP reconnection will not result in Over Temp Protection state change
			 * since the WBC is not re-initialized on PRU alerts.
			 */
			wbc_event.event_type = WBC_EVENT_TYPE_BATTERY_OVERHEAT;
			wbc_event.u.battery_overheat = hal_instance->si.battery_overheat;
			hal_instance->pClientCallbackFunc(&wbc_event);
		}
		if (prev_si.wipower_capable != hal_instance->si.wipower_capable) {
			wbc_event.event_type = WBC_EVENT_TYPE_WIPOWER_CAPABLE_STATUS;
			wbc_event.u.wipower_capable = hal_instance->si.wipower_capable;
			hal_instance->pClientCallbackFunc(&wbc_event);
		}
		if (prev_si.ptu_presence != hal_instance->si.ptu_presence) {
			wbc_event.event_type = WBC_EVENT_TYPE_PTU_PRESENCE_STATUS;
			wbc_event.u.ptu_presence = hal_instance->si.ptu_presence;
			hal_instance->pClientCallbackFunc(&wbc_event);
		}
		if (prev_si.wipower_charging != hal_instance->si.wipower_charging) {
			wbc_event.event_type = WBC_EVENT_TYPE_WIPOWER_CHARGING_ACTIVE_STATUS;
			wbc_event.u.wipower_charging = hal_instance->si.wipower_charging;
			hal_instance->pClientCallbackFunc(&wbc_event);
		}
		if (prev_si.chg_complete != hal_instance->si.chg_complete) {
			wbc_event.event_type = WBC_EVENT_TYPE_CHARGE_COMPLETE;
			wbc_event.u.chg_complete = hal_instance->si.chg_complete;
			hal_instance->pClientCallbackFunc(&wbc_event);
		}
	}

	return NULL;
}

void read_initial_props(struct psy_prop_status *props)
{
	char prop_str[MAX_PROP_LEN];
	int prop_int;
	int rc;

	props->is_dc_wipower = false;
	rc = read_from_file(DC_PSY, REAL_TYPE, prop_str, MAX_PROP_LEN - 1);
	if (rc < 0) {
		ALOGE("Couldnt read %s rc = %d\n", REAL_TYPE, rc);
	} else {
		prop_str[rc] = '\0';
		props->is_dc_wipower = is_dc_wipower(prop_str);
	}

	props->battery_present = false;
	rc = read_int_from_file(BATT_PSY, PRESENT, &prop_int);
	if (rc < 0) {
		ALOGE("Couldnt read battery %s rc = %d\n", PRESENT, rc);
	} else {
		props->battery_present = prop_int;
	}

	props->battery_therm_mitigation = false;
	rc = read_int_from_file(BATT_PSY, SYSTEM_TEMP_LEVEL, &prop_int);
	if (rc < 0) {
		ALOGE("Couldnt read battery %s rc = %d\n", SYSTEM_TEMP_LEVEL, rc);
	} else {
		props->battery_therm_mitigation
			= is_battery_therm_mitigation(prop_int);
	}

	props->battery_full = false;
	props->battery_charging = false;
	rc = read_from_file(BATT_PSY, STATUS, prop_str, MAX_PROP_LEN - 1);
	if (rc < 0) {
		ALOGE("Couldnt read battery %s rc = %d\n", STATUS, rc);
	} else {
		prop_str[rc] = '\0';
		props->battery_charging = is_battery_charging(prop_str);
		props->battery_full = is_battery_full(prop_str);
	}

	props->usb_present = false;
	rc = read_int_from_file(USB_PSY, PRESENT, &prop_int);
	if (rc < 0) {
		ALOGE("Couldnt read USB %s rc = %d\n", PRESENT, rc);
	} else {
		props->usb_present = prop_int;
	}

	props->dc_present = false;
	rc = read_int_from_file(DC_PSY, PRESENT, &prop_int);
	if (rc < 0) {
		ALOGE("Couldnt read DC %s rc = %d\n", PRESENT, rc);
	} else {
		props->dc_present = prop_int;
	}
}

wbc_error_t wbc_init(wbc_callback_t *pCallbackFunc)
{
	wbc_error_t status = WBC_ERROR_NONE;
	char log_level_str[PROPERTY_VALUE_MAX] = "";

	hal_instance.pClientCallbackFunc = pCallbackFunc;
	hal_instance.is_inited = true;

	if (property_get("persist.wbc.log_level", log_level_str, "0")) {
		hal_instance.log_level = atoi(log_level_str);
	}

	read_initial_props(&hal_instance.props);

	determine_wipower_capable(&hal_instance.props, &hal_instance.si);
	determine_ptu_presence(&hal_instance.props, &hal_instance.si);
	determine_wipower_charging(&hal_instance.props, &hal_instance.si);
	determine_chg_complete(&hal_instance.props, &hal_instance.si);
	determine_usb_charging_present(&hal_instance.props, &hal_instance.si);
	determine_battery_overheat(&hal_instance.props, &hal_instance.si);
	dump_props(&hal_instance.props);
	dump_stateinfo(&hal_instance.si);

	int ret = pthread_create(&hal_instance.uevent_thread, NULL,
			uevent_thread_func, &hal_instance);
	if (ret) {
		ALOGE("Err: Thread creation failed\n");
		status = WBC_ERROR_UNDEFINED;
	}

	return status;
}

wbc_error_t wbc_finish()
{
	return WBC_ERROR_NONE;
}

void wbc_hal_echo(int value)
{
	ALOGI("-HAL-: echo called, value: %d\n", value);
}

wbc_ptu_presence_t wbc_get_ptu_presence(void)
{
	return hal_instance.si.ptu_presence;
}

wbc_wipower_capable_t wbc_get_wipower_capable(void)
{
	return hal_instance.si.wipower_capable;
}

wbc_wipower_charging_t wbc_get_wipower_charging(void)
{
	return hal_instance.si.wipower_charging;
}

wbc_chg_complete_t wbc_get_chg_complete(void)
{
	return hal_instance.si.chg_complete;
}

wbc_usb_charging_present_t wbc_get_usb_charging_present(void)
{
	return hal_instance.si.usb_charging_present;
}

wbc_battery_overheat_t wbc_get_battery_overheat(void)
{
	return hal_instance.si.battery_overheat;
}

// Entry point of DLib
const wbc_lib_interface_t WBC_LIB_INTERFACE = {
	sizeof(wbc_lib_interface_t),
	wbc_init,
	wbc_finish,
	wbc_hal_echo,
	wbc_get_wipower_capable,
	wbc_get_ptu_presence,
	wbc_get_wipower_charging,
	wbc_get_chg_complete,
	wbc_get_usb_charging_present,
	wbc_get_battery_overheat,
};

/*=========================================================================
  END OF FILE
  =========================================================================*/
