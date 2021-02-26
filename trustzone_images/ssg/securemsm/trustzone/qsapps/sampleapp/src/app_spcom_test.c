/** @file app_spcom_test.c
 * @brief
 * This file contains the definition of QSEE SPCOM testing routines.
 */
/*===========================================================================
  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_spcom_test.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/15   as      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdbool.h>
#include <stringl.h> // secure_memset()
#include <IxErrno.h> // common error codes like 'E_NO_DATA'

#include "com_dtypes.h"
#include "qsee_log.h"
#include "qsee_sync.h"
#include "qsee_spcom.h"


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define LOGD(format, args...)	qsee_printf("app_spcom_test dbg: [%s] " format, __func__, ##args)
#define LOGE(format, args...)	qsee_printf("app_spcom_test err: [%s] " format, __func__, ##args)

/*-------------------------------------------------------------------------
 * Global Variables
 * ----------------------------------------------------------------------*/
static const char *skp_ch_name = "sp_kernel";

/*-------------------------------------------------------------------------
 * Function Implementations
 * ----------------------------------------------------------------------*/
static bool is64bit(void)
{
	 return (sizeof(uint64_t) == sizeof(uintptr_t));
}

static void spcom_test_pass(void)
{
	LOGD("XXXXXX SPCOM TEST PASS XXXXXX");
}

static void spcom_client_test_pass(void)
{
	spcom_test_pass();
}

static volatile int hold_cpu = 0;
static int dummy_counter = 1;

static void spcom_test_fail(void)
{
	 LOGE("XXXXXX SPCOM TEST FAIL XXXXXX");

	while(hold_cpu)
		dummy_counter++;
}

static int client_test_max_loops = 10;

static int spcom_app_client_test(const char *ch_name)
{
	int ret = 0;
	struct spcom_client *client;
	struct spcom_client_info client_info;
	/* dummy command, skp will send response with error code */
	uint32_t msg_id = 0xAA;
	char tx_buf[SPCOM_MAX_REQUEST_SIZE];
	char rx_buf[SPCOM_MAX_RESPONSE_SIZE];
	uint32_t timeout_msec = 0; // use polling mode , timeout infinite.
	bool connected = false;
	int loop = 0;
	int size = 4;
	int connect_timeout_msec = 100;

	LOGD("==== Start ======");

	/*-----------------*/
	/* Register Client */
	/*-----------------*/

	memset(&client_info, 0, sizeof(client_info));
	client_info.ch_name = ch_name;

	client = qsee_spcom_register_client(&client_info);

	if (client == NULL) {
		LOGE("[%s] client is null", __func__);
		spcom_test_fail();
		return -1;
	}

	while (!connected)
	{
		connected = qsee_spcom_client_is_server_connected(client);
		qsee_spin(1000); /* 1 msec delay */
		connect_timeout_msec--;
		if (connect_timeout_msec == 0) {
			 LOGE("spcom connection timeout");
			 spcom_test_fail();
			 return -2;
		}
	}

	LOGD("spcom ch [%s] is connected.", ch_name);

	/*----------------------*/
	/* Client Data Transfer */
	/*----------------------*/

	for (loop = 0; loop < client_test_max_loops ; loop++)
	{
		int i;

		LOGD("Test Loop # [%d]", loop);

		size = ((size+1) % 260); // incremental

		for (i = 0; i < size ; i++) {
			tx_buf[i] = 0xAA + loop + i;
		}
		tx_buf[0]=loop;

		ret = qsee_spcom_client_send_message_sync(client,
						   tx_buf, size,
						   rx_buf, size,
						   timeout_msec);

		LOGD("qsee_spcom_client_send_message_sync() ret [%d]", ret);

		if (ret < 0) {
			qsee_spcom_unregister_client(client);
			spcom_test_fail();
			return -1;
		}

		/* we just test rx/tx data sent.
       	   no loopback data check against skp channel */
		#ifdef TEST_LOOPBACK_DATA
		for (i = 0; i < size ; i++) {
			if (rx_buf[i] != tx_buf[i]) {
				spcom_test_fail();
				return -1;
			}
		}
		#endif

		qsee_spin(1000); /* 1 msec delay */
	} // for loop

	/*-------------------*/
	/* Unregister Client */
	/*-------------------*/

	qsee_spcom_unregister_client(client);

	spcom_test_pass();

	LOGD("==== END ======");

	return 0;
}

int run_spcom_test(void)
{
	 if (!is64bit()) {
		 LOGD("Please run spcom test from 64-bit App");
		 return 0;
	 }

	 return spcom_app_client_test(skp_ch_name);
}

