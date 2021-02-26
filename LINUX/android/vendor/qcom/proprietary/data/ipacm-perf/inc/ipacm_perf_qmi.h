#ifndef _IPACM_PERF_QMI_H_
#define _IPACM_PERF_QMI_H_
/******************************************************************************

                        IPACM_PERF_QMI.H

******************************************************************************/

/******************************************************************************

  @file ipacm_perf_qmi.h
  @brief ipacm_perf_qmi header file.

  DESCRIPTION
  Header file of ipacm_perf_qmi.

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
#include <stdint.h>
#include "qmi_client.h"
#include "wireless_data_service_v01.h"

#define IPACM_PERF_QMI_MAX_TIMEOUT_MS 10000

class ipacm_perf_qmi
{
public:

	/* get ipacm_perf_qmi instance */
	static ipacm_perf_qmi* get_instance();

	/* delete ipacm_perf_qmi instance */
	static void delete_instance();

private:

	ipacm_perf_qmi();

	~ipacm_perf_qmi();

	static ipacm_perf_qmi* p_instance;

	qmi_client_type qmi_wds_client_handle;

	/* qmi callback function */
	static void wds_client_cb
	(
		qmi_client_type user_handle,                    /* QMI user handle       */
		unsigned int    msg_id,                         /* Indicator message ID  */
		void           *ind_buf,                        /* Raw indication data   */
		unsigned int    ind_buf_len,                    /* Raw data length       */
		void           *ind_cb_data                     /* User call back handle */
	);

	/* register QMI_WDS_CONFIGURED_THROUGHPUT_INFO_IND_V01 to WDS server */
	int register_wds_indication
	(
		qmi_client_type client_hdl
	);

};

#endif /* _IPACM_PERF_QMI_H_ */

