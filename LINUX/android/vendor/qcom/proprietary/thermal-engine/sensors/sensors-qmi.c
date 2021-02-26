/*===========================================================================

  sensor-qmi.c

  DESCRIPTION
  QMI TS sensor access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_ts_setup() function should be called before qmi_ts_get_temperature().
  qmi_ts_shutdown() function should be called to clean up resources.

  Copyright (c) 2012-2014,2016,2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "sensors_manager_internal.h"
#include "thermal.h"
#include "sensors-qmi.h"

#include "thermal_sensor_service_v01.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_client_instance_defs.h"

/* Default temperature in degC will be reported back to framework
   when requested sensor is not supported OR QMI register is incomplete */
#define QMI_TS_DEFAULT_TEMP (-273)

/* Specific qmi_ts sensor data */
struct qmi_ts_data {
	pthread_mutex_t mutex;
	pthread_cond_t  condition;
	int threshold_reached;
	struct sensor_info *sensor;
};

/* Help track individual sensors for setup, shutdown, and updates. */
struct qmi_sensor_data {
	const char  *sensor_name;
	const char  *ts_qmi_name;
	int          last_read;
	uint8_t      supported;
	struct qmi_ts_data *data;
	struct qmi_client_info *parent;
};

struct qmi_client_info {
	char                *name;
	qmi_client_type      handle;
	qmi_service_instance instance_id;
	pthread_mutex_t      mtx;
	pthread_t            thread;
	struct qmi_sensor_data *sens_info;
	uint8_t              sens_info_cnt;
};

static struct qmi_client_info modem0_clnt;
static struct qmi_client_info modem1_clnt;

static void qmi_ts_clnt_error_cb(qmi_client_type clnt,
				qmi_client_error_type error,
				void *error_cb_data);

/* Add newly supported sensors here. */
static struct qmi_sensor_data modem0_sensors[] =
{
	{
		.sensor_name = "modem0_pa0",
		.ts_qmi_name = "pa",
		.last_read = CONV(QMI_TS_DEFAULT_TEMP),
		.supported = 0,
		.data = NULL,
		.parent = &modem0_clnt,
	},
	{
		.sensor_name = "modem0_pa1",
		.ts_qmi_name = "pa_1",
		.last_read = CONV(QMI_TS_DEFAULT_TEMP),
		.supported = 0,
		.data = NULL,
		.parent = &modem0_clnt,
	},
	{
		.sensor_name = "modem0_qpa0",
		.ts_qmi_name = "qfe_pa0",
		.last_read = CONV(QMI_TS_DEFAULT_TEMP),
		.supported = 0,
		.data = NULL,
		.parent = &modem0_clnt,
	},
};

static struct qmi_sensor_data modem1_sensors[] =
{
	{
		.sensor_name = "modem1_pa0",
		.ts_qmi_name = "pa",
		.last_read = CONV(QMI_TS_DEFAULT_TEMP),
		.supported = 0,
		.data = NULL,
		.parent = &modem1_clnt,
	},
	{
		.sensor_name = "modem1_pa1",
		.ts_qmi_name = "pa_1",
		.last_read = CONV(QMI_TS_DEFAULT_TEMP),
		.supported = 0,
		.data = NULL,
		.parent = &modem1_clnt,
	},
};

static struct qmi_client_info modem0_clnt = {
	.name  = "MODEM0",
	.instance_id  = 0,
	.mtx          = PTHREAD_MUTEX_INITIALIZER,
	.sens_info     = modem0_sensors,
	.sens_info_cnt = ARRAY_SIZE(modem0_sensors),
};

static struct qmi_client_info modem1_clnt = {
	.name  = "MODEM1",
	.instance_id  = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0,
	.mtx          = PTHREAD_MUTEX_INITIALIZER,
	.sens_info     = modem1_sensors,
	.sens_info_cnt = ARRAY_SIZE(modem1_sensors),
};

static struct qmi_client_info *register_clnts[] = {
	&modem0_clnt,
	&modem1_clnt,
};

static qmi_idl_service_object_type ts_service_object;

/*===========================================================================
FUNCTION notify_qmi_sensor_update

Notify qmi sensor update to thermal-engine sensor framework.

ARGUMENTS
	sens - qmi_sensor which has temperature update

RETURN VALUE
	None
===========================================================================*/
static void notify_qmi_sensor_update(struct qmi_sensor_data *sens)
{
	if (sens->data != NULL) {
		struct qmi_ts_data *qmi_ts = sens->data;
		pthread_mutex_lock(&(qmi_ts->mutex));
		qmi_ts->threshold_reached = 1;
		pthread_cond_broadcast(&(qmi_ts->condition));
		pthread_mutex_unlock(&(qmi_ts->mutex));
	}
}

/*===========================================================================
FUNCTION qmi_ts_update_temperature

Updates the temperature for the thermald_info array.

ARGUMENTS
	clnt - QMI TS service client info
	ind - QMI TS indicator msg.
	notify - 0 - Don't notify interrupt wait thread on update.
		 !0 -  Notify interrupt wait thread on update.
RETURN VALUE
	None
===========================================================================*/
static void qmi_ts_update_temperature(struct qmi_client_info *clnt, ts_temp_report_ind_msg_v01 *ind,
				      uint8_t notify)
{
	int idx = 0;

	if (NULL == ind || NULL == clnt) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	/* Find corresponding struct qmi_ts_data */
	for (idx = 0; idx < clnt->sens_info_cnt; idx++) {
		if (strncmp(clnt->sens_info[idx].ts_qmi_name,
			    ind->sensor_id.sensor_id,
			    QMI_TS_SENSOR_ID_LENGTH_MAX_V01) == 0) {
			break;
		}
	}

	if (idx >= clnt->sens_info_cnt) {
		msg("%s: unknown sensor %s\n", __func__,
		    ind->sensor_id.sensor_id);
		return;
	}

	clnt->sens_info[idx].last_read = CONV((int)ind->temp);

	if (!notify)
		return;

	/* notify the waiting thread */
	dbgmsg("%s: notify update %s %dC\n", __func__,
		ind->sensor_id.sensor_id, (int)ind->temp);
	notify_qmi_sensor_update(&clnt->sens_info[idx]);
}
/*===========================================================================
LOCAL FUNCTION qmi_ts_ind_cb

Handle QMI TS indication callbacks.

ARGUMENTS
	user_handle - QMI client user handle.
	msg_id - Indication message id
	ind_buf - Indication encoded data.
	ind_buf_len - Indication encoded data length.
	ind_cb_data - TS provided callback data.

RETURN VALUE
	None.
===========================================================================*/
static void qmi_ts_ind_cb(qmi_client_type user_handle,
			   unsigned int     msg_id,
			   void            *ind_buf,
			   unsigned int     ind_buf_len,
			   void            *ind_cb_data)
{
	qmi_client_error_type rc = QMI_NO_ERR;
	ts_temp_report_ind_msg_v01  ind_struct;
	int idx = 0;

	if (msg_id == QMI_TS_TEMP_REPORT_IND_V01) {
		memset((void *)&ind_struct, 0,
		       sizeof(ts_temp_report_ind_msg_v01));
		rc = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
					       msg_id, ind_buf, ind_buf_len,
					       &ind_struct,
					       sizeof(ts_temp_report_ind_msg_v01));

		if (rc != QMI_NO_ERR) {
			msg("Error invalid indication message received.\n");
		} else {
			uint8_t notify = 0;

			/* Determine whether indication warrants sensor
			   notification. Only threshold indications should
			   trigger notify update. The exception being special
			   case were seq_num_valid field is used to in
			   combination with current temp request to trigger
			   update to initialize the state of temperature data
			   after successful registration. */
			if ((ind_struct.report_type !=
			     QMI_TS_TEMP_REPORT_CURRENT_TEMP_V01) ||
			    ind_struct.seq_num_valid)
				notify = 1;

			if (ind_struct.temp_valid) {
				dbgmsg("%s: %s %d\n", __func__,
				       ind_struct.sensor_id.sensor_id,
				       (int)ind_struct.temp);
				for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++) {
					if (register_clnts[idx]->handle == user_handle) {
						qmi_ts_update_temperature(register_clnts[idx],
						 &ind_struct, notify);
						break;
					}
				}
			} else {
				msg("Error invalid temperature field.");
			}
		}
	} else {
		printf("\nWarning invalid indication message received.\n");
	}
}

/*===========================================================================
LOCAL FUNCTION modem_ts_reg_notify

Common TS qmi modem register notify function.

ARGUMENTS
	clnt - QMI TS service client handle
	sensor_id - name of sensor_id
	send_current_temp_report - 1 for trigger and immediate sensor reading,
				   0 for set threshold.
	thresh - high_thresh and low_thresh
	seq_num_valid - sets seq_num valid field in request
			0 - disable
			!0 - enable
	seq_num - only valid if seq_num_valid is !0.
		  Used to associate a response with request

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
static int modem_ts_reg_notify(struct qmi_client_info *clnt_info, const char *sensor_id,
				int send_current_temp_report,
				struct thresholds_req_t *thresh,
				uint8_t seq_num_valid, uint32_t seq_num)
{
	int ret = -(EFAULT);
	qmi_client_error_type qmi_error = QMI_NO_ERR;

	ts_register_notification_temp_req_msg_v01  data_req;
	ts_register_notification_temp_resp_msg_v01 data_resp;

	if (!clnt_info || !sensor_id)
		return -(EINVAL);

	memset(&data_req, 0x0, sizeof(data_req));
	strlcpy(data_req.sensor_id.sensor_id, sensor_id,
		QMI_TS_SENSOR_ID_LENGTH_MAX_V01);

	data_req.seq_num_valid = seq_num_valid;
	data_req.seq_num = seq_num;

	if (send_current_temp_report) {
		data_req.send_current_temp_report = 1;
	} else {
		data_req.temp_threshold_high_valid = thresh->high_valid;
		data_req.temp_threshold_high = (float)thresh->high;
		data_req.temp_threshold_low_valid = thresh->low_valid;
		data_req.temp_threshold_low = (float)thresh->low;
	}

	pthread_mutex_lock(&clnt_info->mtx);
	qmi_error = qmi_client_send_msg_sync((qmi_client_type) clnt_info->handle,
				       QMI_TS_REGISTER_NOTIFICATION_TEMP_REQ_V01,
				       &data_req, sizeof(data_req),
				       &data_resp, sizeof(data_resp), 0);
	pthread_mutex_unlock(&clnt_info->mtx);
	if (qmi_error == QMI_NO_ERR) {
		ret = 0;
	} else {
		msg("qmi: qmi_client_send_msg_sync failed. Error %d\n", qmi_error);
	}

	return ret;
}

/*===========================================================================
LOCAL FUNCTION modem_verify_ts_device

Helper function to verify thermal sensor exits on remote QMI
TS service.

ARGUMENTS
	clnt - QMI TS service client handle
	clnt_info - QMI TS service client info data

RETURN VALUE
	0 on success, -(ERRNO) Code on failure.
===========================================================================*/
static int verify_ts_sensor(qmi_client_type clnt,
			     struct qmi_client_info *clnt_info)
{
	int rc;
	int ret = -(EFAULT);
	unsigned int i, j;

	ts_get_sensor_list_resp_msg_v01 *list_resp = NULL;
	ts_register_notification_temp_req_msg_v01 *reg_notify_req = NULL;
	ts_register_notification_temp_resp_msg_v01 *reg_notify_resp = NULL;
	/* Large structs let's not put it on the stack. */

	if (clnt == NULL) {
		ret = -(EINVAL);
		goto error_handler;
	}

	list_resp = malloc(sizeof(ts_get_sensor_list_resp_msg_v01));
	if (list_resp == NULL) {
		msg("%s: Malloc list_resp failure", __func__);
		ret = -(ENOMEM);
		goto error_handler;
	}

	reg_notify_req = malloc(sizeof(ts_register_notification_temp_req_msg_v01));
	if (reg_notify_req == NULL) {
		msg("%s: Malloc reg_notify_req failure", __func__);
		ret = -(ENOMEM);
		goto error_handler;
	}

	reg_notify_resp = malloc(sizeof(ts_register_notification_temp_resp_msg_v01));
	if (reg_notify_resp == NULL) {
		msg("%s: Malloc reg_notify_resp failure", __func__);
		ret = -(ENOMEM);
		goto error_handler;
	}

	memset(list_resp, 0, sizeof(ts_get_sensor_list_resp_msg_v01));
	pthread_mutex_lock(&clnt_info->mtx);
	rc = qmi_client_send_msg_sync(clnt,
				      QMI_TS_GET_SENSOR_LIST_REQ_V01,
				      NULL, 0,
				      list_resp, sizeof(ts_get_sensor_list_resp_msg_v01), 0);
	pthread_mutex_unlock(&clnt_info->mtx);
	if (rc == QMI_NO_ERR) {
		for (i = 0; i < list_resp->sensor_list_len; i++) {
			for (j = 0; j < clnt_info->sens_info_cnt; j++) {
				if (0 == strncasecmp(clnt_info->sens_info[j].\
						     ts_qmi_name,
							list_resp->sensor_list[i].sensor_id,
						QMI_TS_SENSOR_ID_LENGTH_MAX_V01)) {
					/* found matching device name */
					clnt_info->sens_info[j].supported = 1;
					ret = 0;
					break;
				}
			}

			/* Send out current temp notify request to get
			   clients to configure threshold triggers
			   properly. */
			modem_ts_reg_notify(clnt_info, list_resp->sensor_list[i].sensor_id,
					    1, NULL, 1, 0);
		}
	} else {
		msg("%s: QMI send_msg_sync failed with error %d", __func__, rc);
		ret = -(EFAULT);
	}

error_handler:
	if (list_resp != NULL)
		free(list_resp);

	if (reg_notify_req != NULL)
		free(reg_notify_req);

	if (reg_notify_resp != NULL)
		free(reg_notify_resp);

	return ret;
}

/*===========================================================================
LOCAL FUNCTION qmi_register

Helper function to initialize QMI connection to remote service.

ARGUMENTS
	data - QMI TS service client info data
	.

RETURN VALUE
	NULL on exit
===========================================================================*/
static void *qmi_register(void *data)
{
	int rc = -1;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	qmi_client_type notifier = NULL;
	void *clnt_local = NULL;
	struct qmi_client_info *clnt_info = data;

	if (clnt_info == NULL) {
		msg("%s: Invalid argument.", __func__);
		return NULL;
	}

	pthread_mutex_lock(&clnt_info->mtx);
	/* release any old handles for clnt */
	if (clnt_info->handle) {
		qmi_client_release(clnt_info->handle);
		clnt_info->handle = NULL;
	}
	pthread_mutex_unlock(&clnt_info->mtx);

	rc = qmi_client_notifier_init(ts_service_object, &os_params,
				      &notifier);
	if (rc != QMI_NO_ERR) {
		msg("qmi: qmi_client_notifier_init failed.\n");
		goto error_handler;
	}

	info("qmi: Instance id %d for %s TS", clnt_info->instance_id, clnt_info->name);
	while (1) {
		QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
		rc = qmi_client_get_service_instance(ts_service_object,
						     clnt_info->instance_id,
						     &info);
		if(rc == QMI_NO_ERR)
			break;
		/* wait for server to come up */
		QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
	};

	rc = qmi_client_init(&info, ts_service_object, qmi_ts_ind_cb, NULL, NULL,
			     (qmi_client_type *) (&clnt_local));
	if (rc != QMI_NO_ERR) {
		msg("%s thermal sensor service not available.", clnt_info->name);
		goto error_handler;
	}
	/* Verify modem sensor present on service */
	rc = verify_ts_sensor(clnt_local, clnt_info);
	if (rc != 0) {
		qmi_client_release(clnt_local);
		clnt_local = NULL;
		goto error_handler;
	}

	/* best effort register for error */
	qmi_client_register_error_cb(clnt_local, qmi_ts_clnt_error_cb, data);
	clnt_info->handle = clnt_local;
	info("%s thermal sensor service available.\n", clnt_info->name);

error_handler:
	if (notifier != NULL)
		qmi_client_release(notifier);

	return NULL;
}

/*===========================================================================
LOCAL FUNCTION qmi_ts_clnt_error_cb

Callback function called by the QCCI infrastructure when it receives a
REMOVE SERVER message from the modem.

ARGUMENTS
	clnt - QMI TS service client handle
	error - qmi error macro type
	error_cb_data - client cookie passed while registering error callback.

RETURN VALUE
	None.
===========================================================================*/
static void qmi_ts_clnt_error_cb(qmi_client_type clnt,
				 qmi_client_error_type error,
				 void *error_cb_data)
{
	struct qmi_client_info *clnt_info = error_cb_data;
	uint8_t idx;

	if (clnt_info == NULL) {
		msg("%s: Invalid argument.", __func__);
		return;
	}
	if (clnt == NULL)
		return;

	msg("%s: with %d called for clnt %s\n", __func__, error,
		clnt_info->name);

	clnt_info->handle = NULL;
	pthread_join(clnt_info->thread, NULL);
	for (idx = 0; idx < clnt_info->sens_info_cnt; idx++) {
		clnt_info->sens_info[idx].supported = 0;
		clnt_info->sens_info[idx].last_read = CONV(QMI_TS_DEFAULT_TEMP);
		/* Best effort to notify sensor manager for server error cb */
		dbgmsg("%s: qmi remove server cb update for %s\n",
			__func__, clnt_info->sens_info[idx].sensor_name);
		notify_qmi_sensor_update(&clnt_info->sens_info[idx]);
	}
	pthread_create(&(clnt_info->thread), NULL, qmi_register,
		       (void *)clnt_info);
}


/*===========================================================================
FUNCTION modem_ts_temp_request

Function to request sensor read or notify threshold functionality.

ARGUMENTS
	sens_info - qmi sensor data info
	send_current_temp_report - 1 for trigger and immediate sensor reading, 0 for
				   set threshold.
	thresh

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
static int modem_ts_temp_request(struct qmi_sensor_data *sens_info,
			   int send_current_temp_report,
			   struct thresholds_req_t *thresh)
{
	int ret = -(EFAULT);

	if (!sens_info || !sens_info->parent) {
		msg("Invalid input\n");
		return -(EINVAL);
	}

	if ((send_current_temp_report == 0) && ((thresh->high_valid == 1) &&
						(thresh->low_valid == 1)) &&
						(thresh->high <= thresh->low)) {
		msg("Invalid thresh level. High %d, Low %d",
		    thresh->high, thresh->low);
		return -(EINVAL);
	}

	if (!sens_info->parent->handle) {
		msg("%s: TS service failed for %s - QMI registration incomplete\n",
			__func__, sens_info->parent->name);
		return ret;
	}

	if (!sens_info->supported) {
		msg("%s: QMI TS sensor:%s is not supported\n", __func__,
			sens_info->sensor_name);
		return ret;
	}

	ret = modem_ts_reg_notify(sens_info->parent, sens_info->ts_qmi_name,
				send_current_temp_report, thresh, 0, 0);
	if (send_current_temp_report) {
		dbgmsg("%s %s, sensor %s, Get Immediate\n",
		       __func__, (ret) ? ("Failed") : ("Success"), sens_info->ts_qmi_name);
	} else {
		dbgmsg("%s %s, sensor %s, High valid: %s, High %d "
		       "Low valid: %s, Low %d\n",
		       __func__, (ret) ? ("Failed") : ("Success"), sens_info->ts_qmi_name,
		       (thresh->high_valid) ? ("YES") : ("NO"), thresh->high,
		       (thresh->low_valid) ? ("YES") : ("NO"), thresh->low);
	}

	return ret;
}

/*===========================================================================
LOCAL FUNCTION qmi_ts_get_qmi_sensor_data

Helper function for finding qmi_sensor_data information based off of
thermal-engine sensor name..

ARGUMENTS
	sensor - thermal-engine device name for QMI sensor.

RETURN VALUE
	NULL on Failure, valid qmi_sensor_data struct on Success.
===========================================================================*/
static struct qmi_sensor_data *qmi_ts_get_qmi_sensor_data(const char *sensor)
{
	int idx = -1, j;
	uint8_t match = 0;

	/* Find corresponding struct qmi_sensor_data */
	for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++) {
		for (j = 0; j < register_clnts[idx]->sens_info_cnt; j++) {
			if (strncmp(register_clnts[idx]->sens_info[j].sensor_name,
					sensor, MAX_SENSOR_NAME_LEN) == 0) {
				match = 1;
				break;
			}
		}
		if (match)
			break;
	}

	if (idx >= ARRAY_SIZE(register_clnts) || j >= register_clnts[idx]->sens_info_cnt) {
		msg("No matching QMI sensor. inp sensor name:%s\n", sensor);
		return NULL;
	}

	return &register_clnts[idx]->sens_info[j];
}

/*===========================================================================
FUNCTION qmi_ts_setup

QMI TS setup sensor.

ARGUMENTS
	sensor - thermal-engine sensor data

RETURN VALUE
	0 on Failure, 1 on Success
===========================================================================*/
int qmi_ts_setup(struct sensor_info *sensor)
{
	struct qmi_ts_data *qmi_ts = NULL;
	struct qmi_sensor_data *sens_info = NULL;

	sens_info = qmi_ts_get_qmi_sensor_data(sensor->name);
	if (!sens_info) {
		msg("%s: invalid sensor name %s", __func__, sensor->name);
		return 0;
	}

	/* Allocate QMI TS data */
	qmi_ts = (struct qmi_ts_data *) malloc(sizeof(struct qmi_ts_data));
	if (NULL == qmi_ts) {
		msg("%s: malloc failed", __func__);
		return 0;
	}
	memset(qmi_ts, 0, sizeof(struct qmi_ts_data));
	sensor->data = sens_info;

	pthread_mutex_init(&(qmi_ts->mutex), NULL);
	pthread_cond_init(&(qmi_ts->condition), NULL);
	qmi_ts->threshold_reached = 0;
	qmi_ts->sensor = sensor;
	sens_info->data = qmi_ts;

	return 1;
}

/*===========================================================================
FUNCTION qmi_ts_shutdown

QMI TS shutdown sensor.

ARGUMENTS
	sensor - thermal-engine sensor data

RETURN VALUE
	None
===========================================================================*/
void qmi_ts_shutdown(struct sensor_info *sensor)
{
	struct qmi_ts_data *qmi_ts;
	struct qmi_sensor_data *sens_info = NULL;

	if (NULL == sensor ||
	    NULL == sensor->name ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	sens_info = (struct qmi_sensor_data *)sensor->data;

	/* Make sure an indication cannot be issued to a sensor being shutdown */
	qmi_ts = sens_info->data;
	pthread_mutex_lock(&qmi_ts->mutex);
	sens_info->data = NULL;
	sensor->data = NULL;
	pthread_mutex_unlock(&qmi_ts->mutex);

	pthread_mutex_destroy(&qmi_ts->mutex);
	pthread_cond_destroy(&qmi_ts->condition);

	free(qmi_ts);
}

/*===========================================================================
FUNCTION qmi_ts_get_temperature

QMI TS get sensor temperature.

ARGUMENTS
	sensor - thermal-engine sensor data

RETURN VALUE
	Current temperature, CONV(QMI_TS_DEFAULT_TEMP) on failure
===========================================================================*/
int qmi_ts_get_temperature(struct sensor_info *sensor)
{
	int temp = CONV(QMI_TS_DEFAULT_TEMP);
	struct qmi_sensor_data *sens_info = NULL;

	if (NULL == sensor ||
	    NULL == sensor->name ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return temp;
	}

	sens_info = (struct qmi_sensor_data *)sensor->data;

	pthread_mutex_lock(&(sensor->read_mtx));
	temp = sens_info->last_read;

	/* Trigger new read update in case a polling algorithm requires an
	   updated value. */
	modem_ts_temp_request(sens_info, 1, NULL);
	pthread_mutex_unlock(&(sensor->read_mtx));

	return temp;
}

/*===========================================================================
FUNCTION qmi_ts_interrupt_wait

QMI TS sensor wait for interrupt.

ARGUMENTS
	sensor - thermal-engine sensor data

RETURN VALUE
	None
===========================================================================*/
void qmi_ts_interrupt_wait(struct sensor_info *sensor)
{
	struct qmi_ts_data *qmi_ts;
	struct qmi_sensor_data *sens_info = NULL;

	if (NULL == sensor ||
	    NULL == sensor->name ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	if (sensor->interrupt_enable) {
		sens_info = (struct qmi_sensor_data *) sensor->data;
		qmi_ts = (struct qmi_ts_data *) sens_info->data;

		/* Wait for sensor threshold condition */
		pthread_mutex_lock(&(qmi_ts->mutex));
		while (!qmi_ts->threshold_reached) {
			pthread_cond_wait(&(qmi_ts->condition),
					  &(qmi_ts->mutex));
		}
		qmi_ts->threshold_reached = 0;
		pthread_mutex_unlock(&(qmi_ts->mutex));
	}
}

/*===========================================================================
FUNCTION qmi_ts_update_thresholds

QMI TS sensor update interrupt thresholds.

ARGUMENTS
	sensor - thermal-engine sensor data
	thresh - type of threshold change that triggered update

RETURN VALUE
	None
===========================================================================*/
void qmi_ts_update_thresholds(struct sensor_info *sensor,
			       struct thresholds_req_t *thresh)
{
	struct qmi_sensor_data *sens_info = NULL;

	if (NULL == thresh ||
	    NULL == sensor ||
	    NULL == sensor->data ||
	    NULL == sensor->name) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	sens_info = (struct qmi_sensor_data *)sensor->data;

	/* Convert thresholds to Celsius for QMI TS*/
	thresh->high = RCONV(thresh->high);
	thresh->low = RCONV(thresh->low);

	modem_ts_temp_request(sens_info, 0, thresh);
}

/*===========================================================================
FUNCTION modem_ts_qmi_init

Helper function to initialize TS qmi communication to modem.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
int modem_ts_qmi_init(void)
{
	unsigned int idx = 0;
	/* Get the service object for the ts API */
	ts_service_object = ts_get_service_object_v01();
	if (!ts_service_object) {
		msg("qmi: ts_get_service_object failed.\n");
		return -(EFAULT);
	}
	modem1_clnt.instance_id = get_fusion_qmi_client_type();

	for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++)
	{
		/* start thread to connect to remote QMI services */
		pthread_create(&(register_clnts[idx]->thread), NULL, qmi_register,
			       (void*)register_clnts[idx]);
	}

	return 0;
}

/*===========================================================================
FUNCTION modem_qmi_ts_comm_release

Release function for modem communication to clean up resources.
Called after use of client handles is complete.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, -(ERRNO) on failure.
===========================================================================*/
int modem_qmi_ts_comm_release(void)
{
	int rc;
	int ret = 0;
	unsigned int idx = 0;

	for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++) {
		pthread_join(register_clnts[idx]->thread, NULL);
	}

	for (idx = 0; idx < ARRAY_SIZE(register_clnts); idx++) {
		if (register_clnts[idx]->handle) {
			rc = qmi_client_release(register_clnts[idx]->handle);
			if (rc) {
				msg("qmi: qmi_client_release %s clnt failed.\n",
				    register_clnts[idx]->name);
				ret = -1;
			}
			register_clnts[idx]->handle = NULL;
		}
	}

	return ret;
}
