/******************************************************************************

                        IPACM_PERF_QMI.CPP

******************************************************************************/

/******************************************************************************

  @file    ipacm_perf_qmi.cpp
  @brief   Qmi module for ipacm_perf

  DESCRIPTION
  Implementation of ipacm_perf_qmi.

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when        who       what, where, why
--------   -----      -------------------------------------------------------
03/05/15   shihuanl   Initial version

******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "ipacm_perf.h"
#include "ipacm_perf_qmi.h"
#include "ipacm_perf_rm.h"

ipacm_perf_qmi* ipacm_perf_qmi::p_instance = NULL;

ipacm_perf_qmi::ipacm_perf_qmi()
{
	qmi_idl_service_object_type qmi_wds_service_object;
	qmi_client_error_type qmi_error;
	qmi_cci_os_signal_type qmi_wds_os_params;

	qmi_wds_client_handle = NULL;

	qmi_wds_service_object = wds_get_service_object_v01();
	if(qmi_wds_service_object == NULL)
	{
		IPACM_PERF_ERR("WDS service object is not available\n");
		return;
	}
	IPACM_PERF_DBG("Got WDS service object\n");

	IPACM_PERF_DBG("Start initing WDS qmi client with timeout: %d s\n", IPACM_PERF_QMI_MAX_TIMEOUT_MS/1000);
	qmi_error = qmi_client_init_instance(qmi_wds_service_object,
								QMI_CLIENT_INSTANCE_ANY,
								ipacm_perf_qmi::wds_client_cb,
								NULL,
								&qmi_wds_os_params,
								IPACM_PERF_QMI_MAX_TIMEOUT_MS,
								&qmi_wds_client_handle);
	if(qmi_error != QMI_NO_ERR)
	{
		IPACM_PERF_ERR("Failed to init WDS qmi client %d\n", qmi_error);
		if(qmi_wds_client_handle != NULL)
		{
			qmi_client_release(qmi_wds_client_handle);
			qmi_wds_client_handle = NULL;
		}
		return;
	}
	IPACM_PERF_DBG("WDS qmi client is inited with handle %d\n", qmi_wds_client_handle);

	if(register_wds_indication(qmi_wds_client_handle) == IPACM_PERF_FAILURE)
	{
		IPACM_PERF_ERR("Failed to register WDS indication\n");
		qmi_client_release(qmi_wds_client_handle);
		qmi_wds_client_handle = NULL;
	}
	return;
}

ipacm_perf_qmi::~ipacm_perf_qmi()
{
	if(qmi_wds_client_handle != NULL)
	{
		qmi_client_release(qmi_wds_client_handle);
	}
}

/* qmi callback function */
void ipacm_perf_qmi::wds_client_cb
(
	qmi_client_type user_handle,                    /* QMI user handle       */
	unsigned int    msg_id,                         /* Indicator message ID  */
	void           *ind_buf,                        /* Raw indication data   */
	unsigned int    ind_buf_len,                    /* Raw data length       */
	void           *ind_cb_data                     /* User call back handle */
)
{
	if(ind_buf == NULL)
	{
		IPACM_PERF_ERR("Failed to receive any data in qmi message\n");
		return;
	}

	IPACM_PERF_DBG("wds_client_cb: user_handle %X msg_id %d ind_buf_len %d\n",
		user_handle, msg_id, ind_buf_len);

	ipacm_perf_throughput data = {0};
	qmi_client_error_type qmi_error;
	void* dest;
	int dest_len = 0;

	switch(msg_id)
	{
	case QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01:
		IPACM_PERF_DBG("Receive QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01\n");
		dest = (void*)&data;
		dest_len = sizeof(data);
		qmi_error = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      dest,
                                      dest_len);
		if(qmi_error != QMI_NO_ERR)
		{
			IPACM_PERF_ERR("Failed to decode qmi message\n");
			return;
		}

		if(data.ul_configured_throughput_valid &&
			data.dl_configured_throughput_valid)
		{
			ipacm_perf_rm* ptr;
			ptr = ipacm_perf_rm::get_instance();
			if(ptr)
			{
				ptr->set_modem_configured_throughput(
						data.ul_configured_throughput,
						data.dl_configured_throughput);
			}
		}
		break;

	default:
		IPACM_PERF_ERR("Receive unexpected msg_id: %d\n", msg_id);
		break;
	}

	return;
}

/* register QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01 to WDS server */
int ipacm_perf_qmi::register_wds_indication
(
	qmi_client_type client_hdl
)
{
	if(client_hdl == NULL)
	{
		IPACM_PERF_ERR("Client handle is not valid\n");
		return IPACM_PERF_FAILURE;
	}

	qmi_client_error_type qmi_error;
	wds_indication_register_req_msg_v01 req;
	wds_indication_register_resp_msg_v01 resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.report_configured_throughput_valid = 1;
	req.report_configured_throughput = 1;

	/* Send report_configured_throughput registration */
	qmi_error = qmi_client_send_msg_sync(client_hdl,
                                QMI_WDS_INDICATION_REGISTER_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                IPACM_PERF_QMI_MAX_TIMEOUT_MS);

	if (qmi_error != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
	{
		IPACM_PERF_ERR("Failed to register indication with qmi_error=%d result=%d\n",
				qmi_error, resp.resp.result);
		return IPACM_PERF_FAILURE;
	}

	return IPACM_PERF_SUCCESS;
}

/* get ipacm_perf_qmi instance */
ipacm_perf_qmi* ipacm_perf_qmi::get_instance()
{
	if(ipacm_perf_qmi::p_instance == NULL)
	{
		ipacm_perf_qmi::p_instance = new ipacm_perf_qmi();
		IPACM_PERF_DBG("Qmi instance pointer: 0x%08x\n", ipacm_perf_qmi::p_instance);
	}
	return ipacm_perf_qmi::p_instance;
}

/* delete ipacm_perf_qmi instance */
void ipacm_perf_qmi::delete_instance()
{
	if(ipacm_perf_qmi::p_instance != NULL)
	{
		delete ipacm_perf_qmi::p_instance;
		IPACM_PERF_DBG("Qmi instance is deleted\n");
		ipacm_perf_qmi::p_instance = NULL;
	}
	return;
}
