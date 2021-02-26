/*
 * ---------------------------------------------------------------------------
 *  Copyright (c) 2009-2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/rtc.h>
#include <errno.h>
#include <signal.h>
#include <cutils/properties.h>
/* For AID_SYSTEM */
#include <private/android_filesystem_config.h>

#include "qmi_client.h"
#include "qmi_idl_lib_internal.h"
#include "time_service_v01.h"
#include "wlan_time_service_v01.h"
#include "localdefs.h"

#include "time_genoff_i.h"
#include <sys/prctl.h>
#include <sys/capability.h>

static const char *PROPERTY_DELTA_TIME = "persist.delta_time.enable";
static const char *PROPERTY_DEBUG_LOGS = "debug.time_services.enable";
static const char *PROPERTY_ALARM_BOOT = "ro.alarm_boot";
static const char *PROPERTY_DECRYPT = "vold.decrypt";
static qmi_client_type time_client;
static qmi_client_type notifier;
static qmi_cci_os_signal_type os_signals;
static pthread_t qmi_clnt_thread;

static qmi_client_type wlan_time_client;
static qmi_client_type wlan_notifier;
static qmi_cci_os_signal_type wlan_os_signals;
static pthread_t wlan_qmi_clnt_thread;

static time_genoff_struct_type ats_bases[ATS_MAX];
static pthread_mutex_t genoff_mutex;
static uint8_t time_genoff_set_id;
static uint8_t modem_qmi_initialized = 0;
static uint8_t wlan_qmi_initialized = 0;
static uint8_t rcvd_modem_tod = 0;
static int disable_logging;
static int property_false;
static int property_true;
static int64_t delta_ms_boot = 0;
static int64_t rtc_ms_boot = 0;

static cb_indication_t  indication;
static pthread_cond_t	read_cond;

static cb_indication_t  wlan_indication;
static pthread_cond_t   wlan_read_cond;

static pthread_mutex_t qmi_clnt_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t qmi_clnt_cond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t wlan_qmi_clnt_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t wlan_qmi_clnt_cond = PTHREAD_COND_INITIALIZER;

qmi_cci_os_signal_type os_params;
qmi_cci_os_signal_type wlan_os_params;
/* This array lists the bases which have to be sent to MODEM */
static int genoff_update_to_modem[] = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0};
/* This array lists the bases which can receive update from MODEM */
static int genoff_update_from_modem[] = {0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0,
					 0, 0, 0, 1, 0};
/* This array lists the bases, on whose update  the TOD base changes*/
static int genoff_update_tod[] = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/* This array lists the bases on which updates are allowed */
static int genoff_valid_base[] = {0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1};
/* This array lists the bases, which can be updated from APPS */
static int genoff_update_from_apps[] = {0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1,
					1, 1, 0, 0, 0};
/* This array lists the bases which have to be sent to WLAN */
static int genoff_update_to_wlan[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                        0, 0, 0, 0, 1};
/* This array lists the bases which can receive update from WLAN */
static int genoff_update_from_wlan[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                         0, 0, 0, 0, 0};

static int time_persistent_memory_opr (const char *file_name,
		time_persistant_opr_type rd_wr, int64_t *data_ptr)
{
	char fname_data[120];
	char fname_persist[120];
	char pval_time[PROPERTY_VALUE_MAX];
	char pval_alarm[PROPERTY_VALUE_MAX];
	char pval_decrypt[PROPERTY_VALUE_MAX];
	int fd;

	/* location where offset is to be stored */
	snprintf(fname_persist, 120, "%s/%s", PERSIST_OFFSET_LOCATION, file_name);
	snprintf(fname_data, 120, "%s/%s", OFFSET_LOCATION, file_name);

	if (!disable_logging)
		TIME_LOGD("Daemon:Opening File: %s %s\n", fname_persist, fname_data);

	switch(rd_wr){
		case TIME_READ_MEMORY:
			if (!disable_logging)
				TIME_LOGD("Daemon:%s:Genoff Read operation \n",
					__func__);

			property_get(PROPERTY_DELTA_TIME, pval_time, "false");
			property_get(PROPERTY_ALARM_BOOT, pval_alarm, "false");
			property_get(PROPERTY_DECRYPT, pval_decrypt, "");

			if (!strcmp(pval_alarm, "true") || !strcmp(pval_time, "true")
				|| !strcmp(pval_decrypt, "trigger_restart_min_framework")) {
				fd = open(fname_persist,O_RDONLY);
				if (!disable_logging)
					TIME_LOGD("Daemon:Open File: %s\n", fname_persist);
			} else {
				fd = open(fname_data,O_RDONLY);
				if (!disable_logging)
					TIME_LOGD("Daemon:Open File: %s\n", fname_data);
			}
			if (fd < 0) {
				if (!disable_logging)
					TIME_LOGD("Daemon:Unable to open file"
					       "for read\n");
				goto fail_operation;
			}
			if (read(fd, (int64_t *)data_ptr,
						sizeof(int64_t)) < 0) {
				if (!disable_logging)
					TIME_LOGD("Daemon:%s:Error reading from"
					       "file\n", __func__);
				close(fd);
				goto fail_operation;
			}
			break;

		case TIME_WRITE_MEMORY:
			if (!disable_logging)
				TIME_LOGD("Daemon:%s:Genoff write operation \n",
					__func__);
			if ((fd = open(fname_data, O_RDWR | O_SYNC)) < 0) {
				if (!disable_logging)
					TIME_LOGD("Daemon:Unable to open file,"
						"creating file %s\n", fname_data);
				if ((fd = open(fname_data, O_CREAT | O_RDWR |
						O_SYNC, 0644)) < 0) {
					if (!disable_logging)
						TIME_LOGD("Daemon:Unable to create"
							"file, exiting\n");
					goto fail_operation;
				}
			}
			if (write(fd, (int64_t *)data_ptr,
					sizeof(int64_t)) < 0) {
				if (!disable_logging)
					TIME_LOGD("Daemon:%s:Error writing to"
						"file\n", __func__);
				close(fd);
				goto fail_operation;
			}

			close(fd);

			if ((fd = open(fname_persist, O_RDWR | O_SYNC)) < 0) {
				if (!disable_logging)
					TIME_LOGD("Daemon:Unable to open file,"
					       "creating file %s\n", fname_persist);
				if ((fd = open(fname_persist, O_CREAT | O_RDWR |
							O_SYNC,	0644)) < 0) {
					if (!disable_logging)
						TIME_LOGD("Daemon:Unable to create"
						       "file, exiting\n");
					goto fail_operation;
				}
			}
			if (write(fd, (int64_t *)data_ptr,
						sizeof(int64_t)) < 0) {
				if (!disable_logging)
					TIME_LOGD("Daemon:%s:Error writing to"
						"file\n", __func__);
				close(fd);
				goto fail_operation;
			}
			break;
		default:
			return -EINVAL;
	}
	close(fd);
	return 0;

fail_operation:
	return -EINVAL;
}

static void
genoff_pre_init(time_genoff_ptr time_genoff,time_bases_type base)
{
	time_genoff->initialized = 0;
	time_genoff->reqd_base_genoff = base;
	time_genoff->generic_offset = 0;
	time_genoff->bases_type =  base;
	time_genoff->init_func = NULL;
	time_genoff->per_storage_spec.initialized = 0;
	if (!disable_logging)
		TIME_LOGD("Daemon:%s::Base = %d\n", __func__, base);
}

static int genoff_post_init(time_genoff_ptr time_genoff)
{
	int rc;

	if (time_genoff->init_func != NULL) {
		rc = time_genoff->init_func();
		if (rc) {
			TIME_LOGE("Daemon:%s:Init func failed\n", __func__);
			return -EINVAL;
		}
	}

	if (time_genoff->per_storage_spec.initialized == 1) {
		/* Read from the generic offset */
		rc = time_persistent_memory_opr(
				time_genoff->per_storage_spec.f_name,
				TIME_READ_MEMORY, &(time_genoff->generic_offset));
		if (rc) {
			if (!disable_logging)
				TIME_LOGD("Daemon:%s:Error in accessing storage\n",
					__func__);
			time_genoff->generic_offset = 0;
		}
	}

	time_genoff->initialized = 1;

	return 0;
}

static void genoff_set_generic_offset(time_genoff_ptr time_genoff,
		int64_t offset)
{
	int64_t delta_ms;

	delta_ms = offset - time_genoff->generic_offset;
	time_genoff->generic_offset = offset;
}

static time_genoff_ptr genoff_get_pointer(time_bases_type base)
{
	if (base >= ATS_MAX)
		return NULL;

	return &(ats_bases[base]) ;
}

static int genoff_check_offset(time_genoff_info_type *pargs)
{
	time_genoff_ptr ptime_genoff;
	ptime_genoff = genoff_get_pointer(pargs->base);
	if (ptime_genoff == NULL) {
		TIME_LOGE("Daemon:%s:Genoff memory not allocated\n", __func__);
		return -EINVAL;
	}
	*(uint64_t *)pargs->ts_val =((ptime_genoff->generic_offset) != 0) ? 1 : 0;
	if (!disable_logging)
		TIME_LOGD("offset is: %d for base: %d",*(uint64_t *)pargs->ts_val,pargs->base);
	return 0;
}

static void
genoff_updates_per_storage(time_genoff_ptr time_genoff, char *name,
		int64_t thold)
{
	time_genoff->per_storage_spec.initialized = 1;
	time_genoff->per_storage_spec.threshold   = thold;
	strlcpy(time_genoff->per_storage_spec.f_name, name, FILE_NAME_MAX);

	if (!disable_logging)
		TIME_LOGD("Daemon: Storage Name: %s \n",
			time_genoff->per_storage_spec.f_name);
} /* time_genoff_updates_per_storage */


void genoff_persistent_update(time_genoff_ptr ptime_genoff, int64_t delta_ms)
{
	int rc;

	if (ptime_genoff->per_storage_spec.initialized == 1) {
		if (!disable_logging)
			TIME_LOGD("Daemon:%s: Writing genoff = %llu to memory\n",
				__func__, ptime_genoff->generic_offset);

		rc = time_persistent_memory_opr(
				ptime_genoff->per_storage_spec.f_name,
				TIME_WRITE_MEMORY,
				&ptime_genoff->generic_offset);
		if (rc) {
			if (!disable_logging)
				TIME_LOGD("Daemon:%s:or in accessing storage\n",
					__func__);
		}
	}
}

static int rtc_get(int64_t *msecs)
{
	int rc, fd;
	time_t secs = 0;
	struct tm rtc_tm;

	fd = open("/dev/rtc0", O_RDONLY);
	if(fd < 0) {
		TIME_LOGE("Daemon:%s: Unable to open RTC device\n", __func__);
		return -EINVAL;
	}

	rc = ioctl(fd,RTC_RD_TIME,&rtc_tm);
	if(rc < 0) {
		TIME_LOGE("Daemon:%s: Unable to read from RTC device\n",
								__func__);
		goto fail_rtc;
	}

	if (!disable_logging) {
		TIME_LOGD("Daemon:%s: Time read from RTC -- MM/DD/YY HH:MM:SS"
			"%d/%d/%d %d:%d:%d\n", __func__, rtc_tm.tm_mon,
			rtc_tm.tm_mday, rtc_tm.tm_year, rtc_tm.tm_hour,
			rtc_tm.tm_min, rtc_tm.tm_sec);
	}

	/* Convert the time to UTC and then to milliseconds and store	it */
	secs = timegm(&rtc_tm);
	if (!disable_logging)
		TIME_LOGD("Daemon %s: seconds = %ld\n", __func__, secs);

	if(secs < 0) {
		TIME_LOGE("Daemon:Invalid RTC seconds = %ld\n", secs);

		goto fail_rtc;
	}

	*msecs = SEC_TO_MSEC(secs);

	close(fd);
	return 0;

fail_rtc:
	close(fd);
	return -EINVAL;
}

static void qt_get(unsigned long long *msecs)
{
	unsigned long long ticks_qt = 0;

#if __aarch64__
	asm volatile("mrs %0, cntvct_el0" : "=r" (ticks_qt));
#else
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (ticks_qt));
#endif

	*msecs = ticks_qt/CLK_FREQ_KHZ;
	if (!disable_logging)
		TIME_LOGD("Daemon:Value read from QTimer mseconds = %llu\n", *msecs);
}

static int
genoff_send_wlan(time_genoff_info_type *genoff_args)
{
	uint64_t time_value;
	int rc;
	apps_set_time_on_wlan_req_msg_v01 time_req_msg;
	apps_set_time_on_wlan_resp_msg_v01 time_resp_msg;
	unsigned long long qt_msecs = 0;

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Sending data to WLAN !\n", __func__);

	/* Do a genoff opr call */
	genoff_args->unit = TIME_MSEC;
	time_value =  *(uint64_t *)genoff_args->ts_val;
	time_value -= (uint64_t)SEC_TO_MSEC(MODEM_EPOCH_DIFFERENCE);

	qt_get(&qt_msecs);

	time_value += qt_msecs + delta_ms_boot;

	if (!disable_logging)
		TIME_LOGD("Daemon: Base = %d, Value being sent to WLAN = %llu\n",
			genoff_args->base, time_value);

	time_req_msg.base = genoff_args->base;
	time_req_msg.time = time_value;
	time_req_msg.timetick = qt_msecs;

	time_resp_msg.resp.result = QMI_RESULT_SUCCESS;
	time_resp_msg.resp.error = QMI_ERR_NONE;

	rc = qmi_client_send_msg_sync(wlan_time_client,
			QMI_APPS_SET_TIME_ON_WLAN_BASE_REQ_V01, &time_req_msg,
			sizeof(time_req_msg), &time_resp_msg,
			sizeof(time_resp_msg), 1000);
	if (rc != QMI_NO_ERR || time_resp_msg.resp.result !=
			QMI_RESULT_SUCCESS){
		TIME_LOGE("Daemon: %s: message send failed. Error Code %d\n",
				__func__, time_resp_msg.resp.error);
		rc = -EINVAL;
		goto exit;
	}

	rc = 0;
exit:
	return rc;
}

static int genoff_set(time_genoff_info_type *pargs)
{
	unsigned long long qt_msecs;
	int64_t delta_ms, ts_val_secs;
	int rc = 0;
	time_genoff_ptr ptime_genoff, ptime_genoff_tod, ptime_genoff_wlan;
	time_genoff_info_type genoff_update;

	ptime_genoff = genoff_get_pointer(pargs->base);
	if (ptime_genoff == NULL) {
		TIME_LOGE("Daemon:%s:Genoff memory not allocated\n", __func__);
		return -EINVAL;
	}
	if (ptime_genoff->initialized == 0) {
		TIME_LOGE("Daemon:%s:Genoff not initialized\n", __func__);
		return -EINVAL;
	}

	/* Get QTimer time */
	qt_get(&qt_msecs);

	if (!disable_logging) {
		TIME_LOGD("Daemon:Value read from RTC mseconds on boot = %lld\n", rtc_ms_boot);
		TIME_LOGD("Daemon:Delta read on boot mseconds = %lld\n", delta_ms_boot);
	}
	/* delta_ms = new time - qtimer time - delta at boot*/
	delta_ms = *(uint64_t *)pargs->ts_val - qt_msecs - delta_ms_boot;
	if (!disable_logging)
		TIME_LOGD("Daemon:new time %lld \n",
			*(uint64_t *)pargs->ts_val);
	ptime_genoff->generic_offset = delta_ms;

	if (!disable_logging)
		TIME_LOGD("Daemon: delta %lld genoff %lld \n", delta_ms,
			ptime_genoff->generic_offset);
	genoff_persistent_update(ptime_genoff, delta_ms);

	if (genoff_update_tod[ptime_genoff->bases_type] &&
			ptime_genoff->bases_type != ATS_TOD) {
		/* Update the TOD offset */
		if (!disable_logging)
			TIME_LOGD("Updating the TOD offset\n");
		ptime_genoff_tod = genoff_get_pointer(ATS_TOD);
		if (ptime_genoff_tod == NULL) {
			TIME_LOGE("Daemon:%s:TOD Genoff memory not allocated\n", __func__);
			return -EINVAL;
		}
		ptime_genoff_tod->generic_offset =
					ptime_genoff->generic_offset;
		genoff_persistent_update(ptime_genoff_tod, delta_ms);
	}

	if (ptime_genoff->bases_type == ATS_USER && !rcvd_modem_tod) {
		/* Update the WLAN offset */
		if (!disable_logging)
			TIME_LOGD("Daemon:%s: Updating the WLAN offset\n",
								__func__);
		ptime_genoff_wlan = genoff_get_pointer(ATS_WLAN);
		if (ptime_genoff_wlan == NULL) {
			TIME_LOGE("Daemon:%s:WLAN Genoff memory not allocated\n", __func__);
			return -EINVAL;
		}
		ptime_genoff_wlan->generic_offset =
					ptime_genoff->generic_offset;
		genoff_persistent_update(ptime_genoff_wlan, delta_ms);

		genoff_update.base = ATS_WLAN;
		genoff_update.unit = TIME_MSEC;
		genoff_update.operation  = T_SET;
		if (genoff_update_to_wlan[genoff_update.base] &&
						wlan_qmi_initialized) {
			if (!disable_logging)
				TIME_LOGD("Daemon:%s: Update to wlan bit set\n",
									__func__);
			ts_val_secs = ptime_genoff_wlan->generic_offset;
			genoff_update.ts_val = &ts_val_secs;
			if ((rc = genoff_send_wlan(&genoff_update)) < 0)
				TIME_LOGE("Daemon:Unable to send data to WLAN\n");
		}
	}

	return 0;
}

static int genoff_get(time_genoff_info_type *pargs)
{
	time_genoff_ptr ptime_genoff;
	unsigned long long qt_msecs = 0;

	if (!disable_logging)
		TIME_LOGD("Daemon: genoff get for %d\n", pargs->base);
	ptime_genoff = genoff_get_pointer(pargs->base);
	if (ptime_genoff == NULL) {
		TIME_LOGE("Daemon:%s:Genoff memory not allocated\n",
								__func__);
		return -EINVAL;
	}

	if (ptime_genoff->initialized == 0) {
		TIME_LOGE("Daemon:%s:Bases not initialized\n", __func__);
		goto fail_time_get;
	}

	qt_get(&qt_msecs);

	if (!disable_logging) {
		TIME_LOGD("Daemon:Value read from RTC mseconds on boot = %lld\n", rtc_ms_boot);
		TIME_LOGD("Daemon:Value read from QTimer mseconds = %llu\n", qt_msecs);
		TIME_LOGD("Daemon:Value read from generic offset = %lld\n",
						ptime_genoff->generic_offset);
		TIME_LOGD("Daemon:Delta read on boot mseconds = %lld\n", delta_ms_boot);
	}


	/* Add QTimer time to the offset */
	*(uint64_t *)pargs->ts_val = ptime_genoff->generic_offset + qt_msecs + delta_ms_boot;

	if (!disable_logging)
		TIME_LOGD("Daemon:Final Time = %llu\n", *(uint64_t *)pargs->ts_val);
	return 0;

fail_time_get:
	*(uint64_t *)pargs->ts_val = 0;
	return -EINVAL;
}

static int genoff_opr(time_genoff_info_type *pargs)
{
	time_genoff_info_type temp_genoff_args;
	uint64_t ts_val_secs;
	int rc = 0;

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Base = %d, val = %llu, operation = %d",
			__func__, pargs->base, *(uint64_t *)(pargs->ts_val),
			pargs->operation);

	if (pargs->operation == T_GET) {
		switch(pargs->unit) {
			case TIME_MSEC:
				rc = genoff_get(pargs);
				break;
			case TIME_SECS:
				rc = genoff_get(pargs);
				/* Convert the time to secs */
				*(uint64_t *)pargs->ts_val =
					(uint64_t)MSEC_TO_SEC(*(uint64_t *)pargs->ts_val);
				break;
			case TIME_JULIAN:
				temp_genoff_args.base        = pargs->base;
				temp_genoff_args.ts_val      = &ts_val_secs;
				temp_genoff_args.unit        = TIME_SECS;
				temp_genoff_args.operation   = T_GET;
				rc = genoff_get(&temp_genoff_args);
				/* convert the time to julian */
				pargs->ts_val =
					gmtime((time_t *)&ts_val_secs);
				break;
			default:
				TIME_LOGE("Daemon:%s:Invalid time unit %d",
						__func__, pargs->unit);
				return -EINVAL;
				break;
		}
	}
	else if (pargs->operation == T_SET) {
		switch(pargs->unit) {
			case TIME_MSEC:
				rc = genoff_set(pargs);
				break;
			case TIME_SECS:
				/* Convert the time to msecs */
				ts_val_secs =
					SEC_TO_MSEC(*(uint64_t *)pargs->ts_val);
				temp_genoff_args.base        = pargs->base;
				temp_genoff_args.ts_val      = &ts_val_secs;
				temp_genoff_args.unit        = TIME_MSEC;
				temp_genoff_args.operation   = T_SET;
				rc = genoff_opr(&temp_genoff_args);
				break;
			case TIME_JULIAN:
				/* Convert input time to UTC */
				ts_val_secs =
					mktime((struct tm *)pargs->ts_val);
				temp_genoff_args.base        = pargs->base;
				temp_genoff_args.ts_val      = &ts_val_secs;
				temp_genoff_args.unit        = TIME_SECS;
				temp_genoff_args.operation   = T_SET;
				rc = genoff_opr(&temp_genoff_args);
				break;
			default:
				TIME_LOGE("Daemon:%s:Invalid time unit %d",
						__func__, pargs->unit);
				return -EINVAL;
				break;
		}
	}
	else if (pargs->operation == T_IS_SET) {
		rc = genoff_check_offset(pargs);
	}

	return rc;
}

static void
genoff_add_base_subsys(time_genoff_ptr time_genoff,
		time_bases_type base)
{
	time_genoff->subsys_base = base;
	time_genoff->reqd_base_genoff = 1;
}

static int ats_bases_init(time_bases_type time_base,
		time_bases_type subsys_base, char *f_name,
				time_genoff_ptr ptime_genoff)
{
	int rc;

	genoff_pre_init(ptime_genoff, time_base);
	/* subsys_base = "parent" base */
	genoff_add_base_subsys(ptime_genoff, subsys_base);
	genoff_updates_per_storage(ptime_genoff, f_name,
			TIME_GENOFF_UPDATE_THRESHOLD_MS);

	rc = genoff_post_init(ptime_genoff);
	if (rc) {
		TIME_LOGE("Daemon:%s: Post init failed for base = %d\n",
							__func__, time_base);
		return -EINVAL;
	}

	return 0;
}

static int ats_rtc_init(time_genoff_ptr ptime_genoff)
{
	int fd, rc;
	struct tm rtc_tm;
	time_t secs = 0;
	int64_t msecs = 0;

	memset(&rtc_tm, 0, sizeof(struct tm));
	genoff_pre_init(ptime_genoff, ATS_RTC);

	fd = open("/dev/rtc0", O_RDONLY);
	if(fd < 0) {
		TIME_LOGE("Daemon:%s: Unable to open RTC device\n", __func__);
		return -EINVAL;
	}

	rc = ioctl(fd,RTC_RD_TIME,&rtc_tm);
	if(rc < 0) {
		TIME_LOGE("Daemon:%s: Unable to read from RTC device\n",
								__func__);
		close(fd);
		return -EINVAL;
	}

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Time read from RTC -- year = %d, month = %d,"
			"day = %d\n", __func__, rtc_tm.tm_year,
			rtc_tm.tm_mon, rtc_tm.tm_mday);

	close(fd);

	/* Convert the time to UTC and then to milliseconds and store it */
	secs = timegm(&rtc_tm);
	if (!disable_logging)
		TIME_LOGD("Daemon %s: seconds = %ld\n", __func__, secs);

	if(secs < 0) {
		TIME_LOGE("Daemon:Invalid RTC seconds = %ld\n", secs);
		return -EBADMSG;
	}

	msecs = SEC_TO_MSEC(secs);
	if (!disable_logging)
		TIME_LOGD("Daemon:Value read from RTC seconds = %lld\n", msecs);
	genoff_set_generic_offset(ptime_genoff, msecs);

	rc = genoff_post_init(ptime_genoff);
	if (rc) {
		TIME_LOGE("Daemon:%s: Genoff post_init operation failed\n",
								__func__);
		return -EINVAL;
	}

	return 0;
}

static int genoff_init_config(void)
{
	int i, rc;
	char f_name[FILE_NAME_MAX];

	/* Initialize RTC values */
	rc = ats_rtc_init(&ats_bases[0]);
	if (rc && (rc != -EBADMSG)) {
		TIME_LOGE("Daemon:%s: RTC initilization failed\n", __func__);
		return -EINVAL;
	} else {
		if (!disable_logging)
			TIME_LOGD("Daemon:%s: ATS_RTC initialized with rc: %d\n", __func__, rc);
	}

	/* Initialize the other offsets */
	for(i = 1; i < ATS_MAX; i++) {
		snprintf(f_name, FILE_NAME_MAX, "ats_%d", i);
		rc = ats_bases_init(i, ATS_RTC, f_name, &ats_bases[i]);
		if (rc) {
			TIME_LOGE("Daemon:%s: Init failed for base = %d\n",
								__func__, i);
			return -EINVAL;
		}
	}

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Other bases initilized, exiting genoff_init\n",
								__func__);

	return 0;
}

static int genoff_boot_tod_init()
{
	uint64_t tod_value;
	int res, rc;
	struct timespec ts;
	struct timeval tv;
	struct __user_cap_header_struct capheader;
	struct __user_cap_data_struct capdata[2];

	time_genoff_info_type tod_genoff;

	tod_genoff.base = ATS_TOD;
	tod_genoff.unit = TIME_MSEC;
	tod_genoff.operation  = T_GET;
	tod_genoff.ts_val = &tod_value;
	res = genoff_opr(&tod_genoff);
	if (res < 0)
		return -EINVAL;

	ts.tv_sec = (time_t) MSEC_TO_SEC(tod_value);
	ts.tv_nsec = (long) ((tod_value % 1000LL) * 1000000LL);

	/* Update System time */

	tv.tv_sec = ts.tv_sec;
	tv.tv_usec = ts.tv_nsec / 1000LL;

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Updating system time to sec=%ld, usec"
			"=%ld\n", __func__, tv.tv_sec, tv.tv_usec);
	memset(&capheader, 0, sizeof(capheader));
	memset(&capdata, 0, sizeof(capdata));
	capheader.version = _LINUX_CAPABILITY_VERSION_3;
	capheader.pid = 0;
	capdata[CAP_TO_INDEX(CAP_SYS_TIME)].permitted |= CAP_TO_MASK(CAP_SYS_TIME);
	capdata[CAP_TO_INDEX(CAP_SYS_TIME)].effective |= CAP_TO_MASK(CAP_SYS_TIME);

	if ((rc = capset(&capheader, capdata)) < 0) {
		TIME_LOGE("Daemon:%s:capset failed: %s, rc = %d\n",
			__func__, strerror(errno), rc);
	}

	errno = 0;
	if (settimeofday(&tv, NULL) != 0) {
		TIME_LOGE("Daemon:%s: Unable to set clock to sec=%ld"
				"usec=%ld, errno=%d\n", __func__, tv.tv_sec,
				tv.tv_usec, errno);
		return -EINVAL;
	}

	return 0;
}

static int
genoff_send_modem(time_genoff_info_type *genoff_args)
{
	uint64_t time_value;
	int rc;
	time_set_genoff_offset_req_msg_v01 time_req_msg;
	time_set_genoff_offset_resp_msg_v01 time_resp_msg;

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Sending data to MODEM !\n", __func__);

	/* Do a genoff opr call */
	genoff_args->unit = TIME_MSEC;
	time_value =  *(uint64_t *)genoff_args->ts_val;
	time_value -= (uint64_t)SEC_TO_MSEC(MODEM_EPOCH_DIFFERENCE);

	if (!disable_logging)
		TIME_LOGD("Daemon: Base = %d, Value being sent to MODEM = %llu\n",
			genoff_args->base, time_value);

	time_req_msg.base = genoff_args->base;
	time_req_msg.generic_offset = time_value;

	time_resp_msg.resp.result = QMI_RESULT_SUCCESS;
	time_resp_msg.resp.error = QMI_ERR_NONE;

	/* Locking to syncronize with indication read */
	pthread_mutex_lock(&indication.lock);
	rc = qmi_client_send_msg_sync(time_client,
			QMI_TIME_GENOFF_SET_REQ_MSG_V01, &time_req_msg,
			sizeof(time_req_msg), &time_resp_msg,
			sizeof(time_resp_msg), 1000);
	if (rc != QMI_NO_ERR || time_resp_msg.resp.result !=
			QMI_RESULT_SUCCESS){
		TIME_LOGE("Daemon: message send failed. Error Code %d\n",
				time_resp_msg.resp.error);
		rc = -EINVAL;
		goto exit;
	}

	rc = 0;
exit:
	pthread_mutex_unlock(&indication.lock);
	return rc;
}

static void genoff_handler(void *recv_arg)
{
	int rc;
	int recv_id = *(int *)recv_arg;
	struct send_recv_struct to_recv;
	time_genoff_info_type genoff_args;
	time_genoff_ptr ptime_genoff;
	char pval[PROPERTY_VALUE_MAX] = "enable";

	property_get(PROPERTY_DEBUG_LOGS, pval, "enable");
	if (!strcmp(pval, "true")) {
		if(!property_true) {
			disable_logging = 0;
			property_true = 1;
			property_false = 0;
		}
	} else if (!strcmp(pval, "false")){
		if(!property_false) {
			disable_logging = 1;
			property_false = 1;
			property_true = 0;
		}
	}

	/*
	 * Initialize base to 0 (Invalid )
	 */
	to_recv.base = 0;

	/*
	 * Receive data from the library
	 * Format: base, unit, operation, value (for set operation)
	 */
	if (recv(recv_id, (void *)&to_recv, sizeof(to_recv), 0) < 0) {
		TIME_LOGE("Daemon:Unable to recv data from client\n");
		goto error_invalid_input;
	}

	if (to_recv.base >= ATS_MAX) {
		TIME_LOGE("Daemon: Invalid base received\n");
		to_recv.result = -EINVAL;
		to_recv.base = to_recv.unit = to_recv.value = 0;
		if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
			TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);
		goto error_invalid_input;
	}

	if (genoff_update_from_apps[to_recv.base] == 0 &&
			to_recv.operation == T_SET) {
		TIME_LOGE("Daemon:%s: Update is not allowed for "
				"offset[%d]\n", __func__, to_recv.base);
		to_recv.result = -EPERM;
		to_recv.base = to_recv.unit = to_recv.value = 0;
		if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
			TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);

		goto error_invalid_input;
	}

	if (!disable_logging) {
		TIME_LOGD("Daemon:Received base = %d, unit = %d, operation = %d,"
				"value = %llu\n", to_recv.base, to_recv.unit,
					to_recv.operation, to_recv.value);
	}

	genoff_args.base = to_recv.base;
	genoff_args.unit = to_recv.unit;
	genoff_args.operation = to_recv.operation;
	genoff_args.ts_val = (uint64_t *)&to_recv.value;

	/* Check if a valid base update is received */
	if (!genoff_valid_base[to_recv.base]) {
		TIME_LOGE("Daemon: Operation on this base is not supported\n");
		to_recv.result = -EINVAL;
		to_recv.base = to_recv.unit = to_recv.value = 0;
		if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
			TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);
		goto error_invalid_input;
	}

	if (genoff_args.operation == T_DISABLE)
		disable_logging = 1;
	else if (genoff_args.operation == T_ENABLE)
		disable_logging = 0;

	pthread_mutex_lock(&genoff_mutex);
	to_recv.result = genoff_opr(&genoff_args);
	pthread_mutex_unlock(&genoff_mutex);

	switch (genoff_args.operation) {
		case T_GET:
			if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
				TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);
			break;
		case T_SET:
			/* Send data, result back to library */
			if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
				TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);
			/* Send the data to MODEM, if to_modem bit is set */
			ptime_genoff = genoff_get_pointer(to_recv.base);
			if (ptime_genoff == NULL) {
				TIME_LOGE("Daemon:%s:Genoff memory not allocated\n", __func__);
				break;
			}
			if (genoff_update_to_modem[ptime_genoff->bases_type] &&
					modem_qmi_initialized) {
				if (!disable_logging)
					TIME_LOGD("Daemon:Update to modem bit set\n");
				/* Send only offset to modem instead of full-time */
				*(uint64_t *)genoff_args.ts_val = ptime_genoff->generic_offset;
				if ((rc = genoff_send_modem(&genoff_args)) < 0)
					TIME_LOGE("Daemon:Unable to send data to MODEM\n");
			}
			break;
		case T_IS_SET:
			if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
				TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);
			break;
		case T_DISABLE:
			if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
				TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);
			break;
		case T_ENABLE:
			if (send(recv_id, &to_recv, sizeof(to_recv), 0) < 0)
				TIME_LOGE("Daemon:Send to client failed %d\n",
						errno);
			break;
		default:
			TIME_LOGE("Daemon:%s: Invalid option\n", __func__);
			break;
	}

error_invalid_input:
	close(recv_id);
	pthread_exit(NULL);
}


/* Callback indication function for time update from modem */
static void tod_update_ind_cb(qmi_client_type handle, unsigned long msg_id,
		unsigned char *buffer, int buffer_len, void *indication_data)
{
	int rc;
	uint64_t time_value;
	time_genoff_ptr ptime_genoff;
	time_update_indication_message_v01 ind_buff;
	time_genoff_info_type genoff_update;

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Got Update from modem msg_id %lu", __func__,
			msg_id);

	if ((msg_id != QMI_TIME_ATS_1X_UPDATE_IND_MSG_V01) &&
		(msg_id != QMI_TIME_ATS_GPS_UPDATE_IND_MSG_V01) &&
		(msg_id != QMI_TIME_ATS_SECURE_UPDATE_IND_MSG_V01) &&
		(msg_id != QMI_TIME_ATS_TOD_UPDATE_IND_MSG_V01))
		return;

	pthread_mutex_lock(&indication.lock);

	pthread_mutex_lock(&genoff_mutex);
	rc = qmi_client_message_decode(handle, QMI_IDL_INDICATION, msg_id,
			buffer, buffer_len, &ind_buff,
			sizeof(time_update_indication_message_v01));
	if (rc != QMI_NO_ERR) {
		TIME_LOGE("Daemon:%s: TOD update message decode error\n",
								__func__);
		goto error;
	}

	if (ind_buff.base == ATS_TOD)
		ptime_genoff = genoff_get_pointer(ATS_TOD_MODEM);
	else
		ptime_genoff = genoff_get_pointer(ind_buff.base);

	if (ptime_genoff == NULL) {
		TIME_LOGE("Daemon:%s: ptime_genoff is NULL\n", __func__);
		goto error;
	}

	if (genoff_update_from_modem[ptime_genoff->bases_type] == 0) {
		TIME_LOGE("Daemon:%s: Ignore time update from MODEM,"
			"for base = %d\n", __func__,
					ptime_genoff->bases_type);
		goto error;
	}

	indication.offset = ind_buff.base;
	indication.ind_received = 1;
	pthread_cond_signal(&read_cond);

error:
	pthread_mutex_unlock(&genoff_mutex);
	pthread_mutex_unlock(&indication.lock);

}

static void genoff_wlan_qmi_error_cb(qmi_client_type clnt, qmi_client_error_type error, void *error_cb_data)
{
	int rc;

	if (clnt == NULL)
		return;

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Error callback called \n", __func__);

	pthread_mutex_lock(&wlan_qmi_clnt_mutex);
	wlan_qmi_initialized = 0;
	pthread_cond_signal(&wlan_qmi_clnt_cond);
	pthread_mutex_unlock(&wlan_qmi_clnt_mutex);
}

static void genoff_qmi_error_cb(qmi_client_type clnt, qmi_client_error_type error, void *error_cb_data)
{
	int rc;

	if (clnt == NULL)
		return;

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Error callback called \n", __func__);

	pthread_mutex_lock(&qmi_clnt_mutex);
	modem_qmi_initialized = 0;
	pthread_cond_signal(&qmi_clnt_cond);
	pthread_mutex_unlock(&qmi_clnt_mutex);
}

void * genoff_wlan_qmi_service_request()
{
	int i = 0, rc, retry_count;
	unsigned int num_entries = 1, num_services;
	qmi_idl_service_object_type time_service_object;
	qmi_service_info info[10];
	apps_set_time_on_wlan_req_msg_v01 time_req;
	apps_set_time_on_wlan_resp_msg_v01 time_resp;
	time_genoff_ptr ptime_genoff;
	unsigned long long qt_msecs = 0;

	/* Used for sending initial time to WLAN */
	struct timeval	user;
	uint64_t time_value;

	while(1){

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Initializing QMI \n", __func__);

	retry_count = 5;

	/* release any old handlers for client */
	if (wlan_notifier) {
		qmi_client_release(wlan_notifier);
		wlan_notifier = NULL;
	}
	if (wlan_time_client) {
		qmi_client_release(wlan_time_client);
		wlan_time_client = NULL;
	}

	time_service_object = wlan_time_get_service_object_v01();
	if (!time_service_object){
		TIME_LOGE("Daemon:%s: wlan_time_get_service_object_v01 failed\n",
				__func__);
		return NULL;
	}

	rc = qmi_client_notifier_init(time_service_object, &wlan_os_params,
				&wlan_notifier);
	if (rc != QMI_NO_ERR) {
		TIME_LOGE("Daemon:%s: WLAN Notifier init failed %d\n", __func__, rc);
		return NULL;
	}

	/* Check if the service is up, if not wait on a signal */
	/* Max. wait time = 5 sec */
	while(1){
		rc = qmi_client_get_service_list( time_service_object, NULL,
				NULL, &num_services);
		if (!disable_logging)
			TIME_LOGD("Daemon:%s: qmi_client_get_service_list returned %d"
				"num_services %d\n", __func__, rc,
				num_services);
		if(rc == QMI_NO_ERR)
			break;

		if(!retry_count){
			TIME_LOGE("Daemon:%s:retry count exceeds limit"
					"QMI init failed continuing without"
					"server\n", __func__);
			qmi_client_release(wlan_notifier);
			wlan_notifier = NULL;
			return NULL;
		}

		/* Using sleep As QMI_CCI_OS_SIGNAL_WAIT is returning quickly
		 * without waiting for timeout
		 */
		sleep(1);

		QMI_CCI_OS_SIGNAL_WAIT(&wlan_os_params, 1000);
		retry_count--;
	}

	/* The server has come up, store the information in info variable */
	rc = qmi_client_get_service_list(time_service_object, info,
			&num_entries, &num_services);
	if (rc != QMI_NO_ERR){
		TIME_LOGE("Daemon:%s: qmi_client_get_service_list returned %d"
				"num_service %d num_entries %d\n", __func__,
				rc, num_services, num_entries);
		qmi_client_release(wlan_notifier);
		wlan_notifier = NULL;
		return NULL;
	}

	rc = qmi_client_init(&info[0], time_service_object, NULL,
					NULL, &wlan_os_signals, &wlan_time_client);
	if (rc != QMI_NO_ERR){
		TIME_LOGE("Daemon:%s: Client init failed %d\n", __func__, rc);
		qmi_client_release(wlan_notifier);
		wlan_notifier = NULL;
		return NULL;
	}

	qmi_client_release(wlan_notifier);
	wlan_notifier = NULL;

	/* Send One message (ATS_WLAN) to WLAN an initial transaction */
	memset(&time_req, 0 , sizeof(apps_set_time_on_wlan_req_msg_v01));
	memset(&time_resp, 0 , sizeof(apps_set_time_on_wlan_resp_msg_v01));
	time_req.base = ATS_WLAN;
	ptime_genoff = genoff_get_pointer(ATS_USER);
	time_value = (uint64_t)ptime_genoff->generic_offset;

	qt_get(&qt_msecs);

	time_value += qt_msecs + delta_ms_boot;
	time_req.time = time_value;
	time_req.timetick = qt_msecs;

	rc = qmi_client_send_msg_sync(wlan_time_client,
			QMI_APPS_SET_TIME_ON_WLAN_BASE_REQ_V01, &time_req,
			sizeof(time_req), &time_resp,sizeof(time_resp), 5000);
	if (rc == QMI_NO_ERR && time_resp.resp.error ==0) {
		if (!disable_logging)
			TIME_LOGD("Daemon:%s: Initial transaction successful\n",
								__func__);
	}else {
		TIME_LOGE("Daemon:%s: Initial transaction failed\n",
								__func__);
		qmi_client_release(wlan_time_client);
		wlan_time_client = NULL;
		return NULL;
	}

	pthread_mutex_lock(&wlan_qmi_clnt_mutex);
	wlan_qmi_initialized = 1;
	pthread_mutex_unlock(&wlan_qmi_clnt_mutex);

	rc = qmi_client_register_error_cb(wlan_time_client, genoff_wlan_qmi_error_cb,
									NULL);
	if (rc != QMI_NO_ERR) {
		TIME_LOGE("Daemon:%s: register error cb failed %d\n",
								__func__, rc);
		qmi_client_release(wlan_time_client);
		wlan_time_client = NULL;
		return NULL;
	}

	/* QMI initialization Done */
	pthread_mutex_lock(&wlan_qmi_clnt_mutex);
	while (wlan_qmi_initialized)
		pthread_cond_wait(&wlan_qmi_clnt_cond, &wlan_qmi_clnt_mutex);
	pthread_mutex_unlock(&wlan_qmi_clnt_mutex);
	}

}

void * genoff_modem_qmi_service_request()
{
	int i = 0, rc, retry_count;
	unsigned int num_entries = 1, num_services;
	qmi_idl_service_object_type time_service_object;
	qmi_service_info info[10];
	time_set_genoff_offset_req_msg_v01 time_req;
	time_set_genoff_offset_resp_msg_v01 time_resp;
	time_genoff_ptr ptime_genoff;

	/* Used for sending initial time to MODEM */
	struct timeval	user;
	uint64_t time_value;

	while(1){

	if (!disable_logging)
		TIME_LOGD("Daemon:%s: Initializing QMI \n", __func__);

	retry_count = 5;

	/* release any old handlers for client */
	if (notifier) {
		qmi_client_release(notifier);
		notifier = NULL;
	}
	if (time_client) {
		qmi_client_release(time_client);
		time_client = NULL;
	}

	time_service_object = time_get_service_object_v01();
	if (!time_service_object){
		TIME_LOGE("Daemon:%s: time_get_service_object_v01 failed\n",
				__func__);
		return NULL;
	}

	rc = qmi_client_notifier_init(time_service_object, &os_params,
				&notifier);
	if (rc != QMI_NO_ERR) {
		TIME_LOGE("Daemon:%s: Notifier init failed %d\n", __func__, rc);
		return NULL;
	}

	/* Check if the service is up, if not wait on a signal */
	/* Max. wait time = 5 sec */
	while(1){
		rc = qmi_client_get_service_list( time_service_object, NULL,
				NULL, &num_services);
		if (!disable_logging)
			TIME_LOGD("Daemon:%s: qmi_client_get_service_list returned %d"
				"num_services %d\n", __func__, rc,
				num_services);
		if(rc == QMI_NO_ERR)
			break;

		if(!retry_count){
			TIME_LOGE("Daemon:%s:retry count exceeds limit"
					"QMI init failed continuing without"
					"server\n", __func__);
			qmi_client_release(notifier);
			notifier = NULL;
			return NULL;
		}

		/* Using sleep As QMI_CCI_OS_SIGNAL_WAIT is returning quickly
		 * without waiting for timeout
		 */
		sleep(1);

		QMI_CCI_OS_SIGNAL_WAIT(&os_params, 1000);
		retry_count--;
	}

	/* The server has come up, store the information in info variable */
	rc = qmi_client_get_service_list(time_service_object, info,
			&num_entries, &num_services);
	if (rc != QMI_NO_ERR){
		TIME_LOGE("Daemon:%s: qmi_client_get_service_list returned %d"
				"num_service %d num_entries %d\n", __func__,
				rc, num_services, num_entries);
		qmi_client_release(notifier);
		notifier = NULL;
		return NULL;
	}

	rc = qmi_client_init(&info[0], time_service_object, tod_update_ind_cb,
					NULL, &os_signals, &time_client);
	if (rc != QMI_NO_ERR){
		TIME_LOGE("Daemon:%s: Client init failed %d\n", __func__, rc);
		qmi_client_release(notifier);
		notifier = NULL;
		return NULL;
	}

	qmi_client_release(notifier);
	notifier = NULL;

	/* Send One message (ATS_USER) to Modem an initial transaction */
	memset(&time_req, 0 , sizeof(time_set_genoff_offset_req_msg_v01));
	memset(&time_resp, 0 , sizeof(time_set_genoff_offset_resp_msg_v01));
	time_req.base = ATS_USER;
	ptime_genoff = genoff_get_pointer(time_req.base);
	time_value = (uint64_t)ptime_genoff->generic_offset;
	time_value -= (uint64_t)SEC_TO_MSEC(MODEM_EPOCH_DIFFERENCE);
	time_req.generic_offset = time_value;

	rc = qmi_client_send_msg_sync(time_client,
			QMI_TIME_GENOFF_SET_REQ_MSG_V01, &time_req,
			sizeof(time_req), &time_resp,sizeof(time_resp), 5000);
	if (rc == QMI_NO_ERR && time_resp.resp.error ==0) {
		if (!disable_logging)
			TIME_LOGD("Daemon:%s: Initial transaction successful\n",
								__func__);
	}else {
		TIME_LOGE("Daemon:%s: Initial transaction failed\n",
								__func__);
		qmi_client_release(time_client);
		time_client = NULL;
		return NULL;
	}

	pthread_mutex_lock(&qmi_clnt_mutex);
	modem_qmi_initialized = 1;
	pthread_mutex_unlock(&qmi_clnt_mutex);

	rc = qmi_client_register_error_cb(time_client, genoff_qmi_error_cb,
									NULL);
	if (rc != QMI_NO_ERR) {
		TIME_LOGE("Daemon:%s: register error cb failed %d\n",
								__func__, rc);
		qmi_client_release(time_client);
		time_client = NULL;
		return NULL;
	}

	/* QMI initialization Done */
	pthread_mutex_lock(&qmi_clnt_mutex);
	while (modem_qmi_initialized)
		pthread_cond_wait(&qmi_clnt_cond, &qmi_clnt_mutex);
	pthread_mutex_unlock(&qmi_clnt_mutex);
	}

}

int genoff_wlan_qmi_init()
{
	int rc;

	rc = pthread_create(&wlan_qmi_clnt_thread, NULL,
				genoff_wlan_qmi_service_request, NULL);
	if (0 != rc) {
		TIME_LOGE("Daemon:%s: Failed to request wlan qmi service\n",
								__func__);
		return -EINVAL;
	}
	return 0;
}

int genoff_modem_qmi_init()
{
	int rc;

	rc = pthread_create(&qmi_clnt_thread, NULL,
				genoff_modem_qmi_service_request, NULL);
	if (0 != rc) {
		TIME_LOGE("Daemon:%s: Failed to request modem qmi service\n",
								__func__);
		return -EINVAL;
	}
	return 0;
}

/* Thread for handling connections from genoff library */
static void conn_handler(void *recv_arg)
{
	int rc;
	int sock_id, recv_id, length, recv_val;
	struct sockaddr_un time_socket, time_recv;
	pthread_t time_thread;

	/* Start a server to accept connections from the shared library */
	sock_id = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock_id < 0) {
		TIME_LOGE("Daemon: Unable to create socket:time_genoff\n");
		pthread_exit(NULL);
	}
	time_socket.sun_family = AF_UNIX;
	strlcpy(time_socket.sun_path, GENOFF_SOCKET_NAME, UNIX_PATH_MAX);

	/* abstract domain socket */
	time_socket.sun_path[0] = 0;
	length = strlen(GENOFF_SOCKET_NAME) + sizeof(time_socket.sun_family);

	/* Remove any existing socket with the same name */
	unlink(time_socket.sun_path);
	if (bind(sock_id, (struct sockaddr *)&time_socket, length) < 0) {
		TIME_LOGE("Daemon: Unable to bind socket:time_genoff\n");
		goto err_close_socket;
	}

	if (listen(sock_id, GENOFF_MAX_CONCURRENT_CONN) < 0) {
		TIME_LOGE("Daemon: Unable to listen on socket:time_genoff\n");
		goto err_close_socket;
	}

	if (!disable_logging)
		TIME_LOGD("Daemon: Time-services: All initializations done\n");

	while (1) {
		/* Loop to accept connections from the shared library */
		if (!disable_logging)
			TIME_LOGE("Daemon: Time-services: Waiting to accept"
							"connection\n");
		errno = 0;
		recv_val = sizeof( (struct sockaddr *) &time_recv);
		recv_id = accept(sock_id, (struct sockaddr *)&time_recv,
								&recv_val);
		if (recv_id < 0) {
			TIME_LOGE("Daemon: Unable to accept connection:"
							"time_genoff errno: %d\n", errno);
			continue;
		}

		if (!disable_logging)
			TIME_LOGD("Daemon: Connection accepted:time_genoff\n");

		/* Thread to handle this request */
		rc = pthread_create(&time_thread, NULL,
				(void *)&genoff_handler, (void *)&recv_id);
		if (rc < 0)
			TIME_LOGE("Daemon: Cannot create pthread:"
							"time_genoff\n");
		pthread_join(time_thread, NULL);
	}

err_close_socket:
	close(sock_id);
	pthread_exit(NULL);
}


/* Thread to handle full read */
static void read_offset(void *recv_arg)
{
	int rc = 0;
	time_get_genoff_offset_req_msg_v01 time_request;
	static time_get_genoff_offset_resp_msg_v01 resp_message;
	time_genoff_info_type genoff_update;
	time_genoff_ptr ptime_genoff;

	while (1) {
		pthread_mutex_lock(&indication.lock);
		if (!indication.ind_received)
			pthread_cond_wait(&read_cond,
						&indication.lock);

		if (!disable_logging)
			TIME_LOGD("Daemon:%s: Sending read request for "
				"offset %d\n", __func__, indication.offset);

		memset(&time_request, 0 , sizeof(time_request));
		memset(&resp_message, 0, sizeof(resp_message));

		time_request.base = indication.offset;
		rc = qmi_client_send_msg_sync(time_client,
				QMI_TIME_GENOFF_GET_REQ_MSG_V01,
				&time_request, sizeof(time_request),
				&resp_message, sizeof(resp_message), 1000);

		if (rc == QMI_NO_ERR &&	resp_message.resp.error == 0) {
			resp_message.generic_offset +=
				(uint64_t)SEC_TO_MSEC(MODEM_EPOCH_DIFFERENCE);
			if (!disable_logging)
				TIME_LOGD("Daemon:%s:Time received %llu\n",
					__func__,
					resp_message.generic_offset);
		} else {
			TIME_LOGE("Daemon:%s: Error in reading "
					"full time ignoring update rc=%d"
					" resp=%d ", __func__, rc,
					resp_message.resp.error);
			pthread_mutex_unlock(&indication.lock);
			indication.ind_received = 0;
			continue;
		}

		/* Update the offset */
		pthread_mutex_lock(&genoff_mutex);
		if (resp_message.base == ATS_TOD)
			genoff_update.base = ATS_TOD_MODEM;
		else
			genoff_update.base = resp_message.base;
		genoff_update.unit = TIME_MSEC;
		genoff_update.operation  = T_SET;
		genoff_update.ts_val = &resp_message.generic_offset;
		rc = genoff_opr(&genoff_update);
		if (rc) {
			TIME_LOGE("Daemon:%s: Offset update failed\n",
							__func__);
		} else {
			if (!disable_logging)
				TIME_LOGE("Daemon:%s: offset %d updated\n",
					__func__, genoff_update.base);
		}

		/* Update ATS_WLAN offset */
		if (resp_message.base == ATS_TOD) {
			genoff_update.base = ATS_WLAN;
			genoff_update.unit = TIME_MSEC;
			genoff_update.operation  = T_SET;
			genoff_update.ts_val = &resp_message.generic_offset;
			rc = genoff_opr(&genoff_update);
			if (rc) {
				TIME_LOGE("Daemon:%s: WLAN Offset update failed\n",
								__func__);
			} else {
				if (!disable_logging)
					TIME_LOGD("Daemon:%s: offset %d updated\n",
						__func__, genoff_update.base);
				rcvd_modem_tod = 1;
			}

			if (genoff_update_to_wlan[genoff_update.base] &&
					wlan_qmi_initialized) {
				if (!disable_logging)
					TIME_LOGD("Daemon:%s: Update to wlan bit set\n",
								__func__);
				/* Send full-time to WLAN*/
				ptime_genoff = genoff_get_pointer(ATS_WLAN);
				if (ptime_genoff != NULL) {
					*(uint64_t *)genoff_update.ts_val = ptime_genoff->generic_offset;
					if ((rc = genoff_send_wlan(&genoff_update)) < 0)
						TIME_LOGE("Daemon:Unable to send data to WLAN\n");
				}
			}
		}
		indication.ind_received = 0;
		pthread_mutex_unlock(&genoff_mutex);
		pthread_mutex_unlock(&indication.lock);

	}
}

int main(void)
{
	int rc;
	int sock_id, recv_id, length, recv_val;
	struct sockaddr_un time_socket, time_recv;
	pthread_t time_thread, connection_mgr, read_time;
	unsigned long long qt_msecs = 0;
	uint32_t base;
	time_genoff_ptr ptime_genoff;
	char pval[PROPERTY_VALUE_MAX] = "enable";

	property_get(PROPERTY_DEBUG_LOGS, pval, "enable");
	if (!strcmp(pval, "true")) {
			disable_logging = 0;
	} else if (!strcmp(pval, "false")) {
			disable_logging = 1;
	}

	if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) != 0) {
		TIME_LOGE("Set keepcaps failed!");
	}

	/* Initialization done changing UID/GID */
	rc = setgid(AID_NET_RAW);
	if (rc < 0) {
		TIME_LOGE("Error changing gid:%d\n", rc);
		TIME_LOGE("Time-services exiting\n");
		exit(-1);
	}

	rc = setuid(AID_SYSTEM);
	if (rc < 0) {
		TIME_LOGE("Error changing uid:%d\n", rc);
		TIME_LOGE("Time-services exiting\n");
		exit(-1);
	}

	/* We expect socket write failure so ignore SIGPIPE */
	signal(SIGPIPE, SIG_IGN);
	/* Initialization of bases */
	rc = genoff_init_config();
	if (rc) {
		TIME_LOGE("Daemon: Unable to initialize bases, exiting\n");
		goto error_return;
	}

	if (rc = pthread_mutex_init(&genoff_mutex,NULL)) {
		TIME_LOGE("Daemon: Pthread mutex init failed\n");
		goto error_return;
	}

	/* Get RTC time */
	if (rtc_get(&rtc_ms_boot)) {
		TIME_LOGE("Daemon:%s: RTC read failed\n", __func__);
		return -EINVAL;
	}

	if (!disable_logging)
		TIME_LOGD("Daemon:Value read from RTC mseconds = %lld\n", rtc_ms_boot);

	qt_get(&qt_msecs);

	delta_ms_boot = rtc_ms_boot - qt_msecs;

	if (rtc_ms_boot <= RTC_MIN_VALUE_TO_RESET_OFFSET) {
		for (base = 0; base < ATS_MAX; base++) {
			if (genoff_valid_base[base]) {
				/* Update the offsets */
				ptime_genoff = genoff_get_pointer(base);
				if (ptime_genoff->generic_offset != 0) {
					ptime_genoff->generic_offset = 0;
					genoff_persistent_update(ptime_genoff, 0);
				}
			}
		}
	}

	/* Initialize TOD if stored previously */
	if (genoff_boot_tod_init() < 0)
		TIME_LOGE("Daemon: Unable to set TOD at boot up\n");

	rc = genoff_wlan_qmi_init();
	if (rc)
		TIME_LOGE("Daemon: WLAN QMI init failed, QMI calls inactive\n");

	rc = genoff_modem_qmi_init();
	if (rc)
		TIME_LOGE("Daemon: QMI init failed, QMI calls inactive\n");

	/* Create a thread for full time reading as inside indication we
	 * cannot send QMI request.
	 */

	rc = pthread_create(&read_time, NULL, (void *)read_offset, NULL);
	if (rc < 0) {
		TIME_LOGE("Daemon: Cannot create thread: read thread\n");
		goto error_return;
	}

	/* Create  a thread to manage socket connection */
	rc = pthread_create(&connection_mgr, NULL, (void *)conn_handler,
			NULL);
	if (rc < 0 ) {
		TIME_LOGE("Daemon: Cannot create thread:connection_mgr\n");
		pthread_kill(read_time, SIGKILL);
		goto error_return;
	}

	rc = pthread_join(connection_mgr, NULL);
	pthread_kill(read_time, SIGKILL);
	TIME_LOGE("Daemon: Connection thread joined closing QMI connection"
			"and exiting ...\n");

error_return:
	if (modem_qmi_initialized) {
		qmi_client_release(time_client);
		time_client = NULL;
	}
	return rc;
}
