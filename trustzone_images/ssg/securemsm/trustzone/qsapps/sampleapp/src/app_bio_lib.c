/** @file app_bio_lib.c
 *  @brief This file implements basic biometric lib interface testing
 */
/*===========================================================================
  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_bio_lib.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/15   el      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "biometric_result.h"
#include "biometric_interrupt.h"
#include "qsee_log.h"
#include "qsee_fs.h" // open(), write()
#include "stringl.h" // strlcpy(), timesafe_strncmp()


// ---------------  Testing Utilities -------------------------------------------------------------------------------------

#define BIO_ASSERT_EQUAL(actual, expected)		    \
if((actual) != (expected))							\
{													\
	qsee_log(QSEE_LOG_MSG_ERROR, "-----------------------------------------------------------------------");      \
	qsee_log(QSEE_LOG_MSG_ERROR, "ASSERTION FAILED in %s line %u", __FUNCTION__, __LINE__);	                      \
	qsee_log(QSEE_LOG_MSG_ERROR, "Expression: %s evaluates to: 0x%X instead of: %s", #actual, actual, #expected); \
	qsee_log(QSEE_LOG_MSG_ERROR, "-----------------------------------------------------------------------");      \
	return false;									\
}													\

#define BIO_ASSERT_TRUE(expression) BIO_ASSERT_EQUAL(true, expression)


#define VERIFY_EXPECTED_RESULT(actual, expected)                 \
BIO_ASSERT_EQUAL(actual.method,         expected.method)         \
BIO_ASSERT_EQUAL(actual.result,         expected.result)         \
BIO_ASSERT_EQUAL(actual.user_id,        expected.user_id)        \
BIO_ASSERT_EQUAL(actual.user_entity_id, expected.user_entity_id) \
BIO_ASSERT_EQUAL(actual.transaction_id, expected.transaction_id) \


static       bio_result g_read_result =   {BIO_AUTH_METHOD_NUMBER,   false, BIO_NA, BIO_NA, BIO_NA};
static const bio_result g_fp_result_1 =   {BIO_FINGERPRINT_MATCHING, true,  59,     20006,  0x1234567812345678};
static const bio_result g_fp_result_2 =   {BIO_FINGERPRINT_MATCHING, true,  60,     20145,  0};
static const bio_result g_fp_result_3 =   {BIO_FINGERPRINT_MATCHING, false, BIO_NA, BIO_NA, BIO_NA};
static const bio_result g_iris_result_1 = {BIO_IRIS_MATCHING,        true,  78,     123,    2};
static const bio_result g_iris_result_2 = {BIO_IRIS_MATCHING,        false, BIO_NA, BIO_NA, 3};

// current application name, taken from app_main.c
extern const char *TA_APP_NAME;

#define EXTENSION_DATA_SIZE 128
static uint8_t extension_data_buffer_out[EXTENSION_DATA_SIZE] = {0x12};
static uint8_t extension_data_buffer_in [EXTENSION_DATA_SIZE] = {0};

#define MAX_APP_NAME_LEN 32
static char authenticator_name_buffer[MAX_APP_NAME_LEN] = {0};

static bio_buffer g_extension_data_out = {extension_data_buffer_out, EXTENSION_DATA_SIZE};
static bio_buffer g_extension_data_in  = {extension_data_buffer_in,  EXTENSION_DATA_SIZE};
static bio_buffer authenticator_name   = {(uint8_t*)authenticator_name_buffer, MAX_APP_NAME_LEN};

typedef struct test_result_
{
	unsigned int passed;
	unsigned int failed;
	unsigned int total;

} test_result;


static void set_up(void)
{
	// reset global testing structures
	g_read_result.method = BIO_AUTH_METHOD_NUMBER;
	g_read_result.result = false;
	g_read_result.user_id = BIO_NA;
	g_read_result.user_entity_id = BIO_NA;
	g_read_result.transaction_id = BIO_NA;

	secure_memset(extension_data_buffer_in, 0, EXTENSION_DATA_SIZE);
	secure_memset(authenticator_name_buffer,  0, MAX_APP_NAME_LEN);

	g_extension_data_in.data = extension_data_buffer_in;
	g_extension_data_in.size = EXTENSION_DATA_SIZE;
	
	authenticator_name.data = (uint8_t*)authenticator_name_buffer;
	authenticator_name.data[0] = '\0';
	authenticator_name.size = MAX_APP_NAME_LEN;
}


static void tear_down(void)
{
	;
}

static void execute_test_case(bool (*test_case)(void), const char* test_case_name, test_result* result)
{
	bool test_case_result = true;
	
	// common operations before any test begin
	set_up();
	
	// execute the test case function
	test_case_result = test_case();

	// common clean up operations after each test is done.
	tear_down();
	
	qsee_log(QSEE_LOG_MSG_DEBUG, "TEST: %s %s", test_case_name, test_case_result ? "PASSED" : "FAILED");
	
	// update counters
	result->total++;
	
	if(test_case_result)
	{
		result->passed++;
	}
	else
	{
		result->failed++;
	}
}


// ---------------  Test Cases -------------------------------------------------------------------------------------



static bool basic_set_get(void)
{
	uint64_t uptime = 0;

	// send 'g_fp_result_1'
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, &g_extension_data_out), BIO_NO_ERROR);
	
	// get into 'g_read_result'
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	
	// verify 'g_fp_result_1' and 'g_read_result' hold the same data.
	VERIFY_EXPECTED_RESULT(g_read_result, g_fp_result_1);
	
	// verify the uptime counter
	BIO_ASSERT_TRUE(uptime > 0);
	
	// verify the extension data
	BIO_ASSERT_EQUAL(timesafe_memcmp(extension_data_buffer_in, extension_data_buffer_out, EXTENSION_DATA_SIZE), 0);

	// verify sender's name
	BIO_ASSERT_EQUAL(timesafe_strncmp(authenticator_name_buffer, TA_APP_NAME, MAX_APP_NAME_LEN), 0);
	
	return true;
}


static bool set_different_methods(void)
{
	uint64_t uptime = 0;

	// send FP and IRIS results
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, &g_extension_data_out), BIO_NO_ERROR);
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_iris_result_1, &g_extension_data_out), BIO_NO_ERROR);
	
	// get into 'g_read_result'
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	VERIFY_EXPECTED_RESULT(g_read_result, g_fp_result_1);

	g_read_result.method = BIO_IRIS_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	VERIFY_EXPECTED_RESULT(g_read_result, g_iris_result_1);
	
	return true;
}


static bool set_without_extension_data(void)
{
	uint64_t uptime = 0;

	// send 'g_fp_result_1'
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, NULL), BIO_NO_ERROR);
	
	// get into 'g_read_result'
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	
	// verify 'g_fp_result_1' and 'g_read_result' hold the same data.
	VERIFY_EXPECTED_RESULT(g_read_result, g_fp_result_1);
	
	// verify the uptime counter
	BIO_ASSERT_TRUE(uptime > 0);

	// verify sender's name
	BIO_ASSERT_EQUAL(timesafe_strncmp(authenticator_name_buffer, TA_APP_NAME, MAX_APP_NAME_LEN), 0);
	
	return true;
}


static bool get_without_extension_data(void)
{
	uint64_t uptime = 0;

	// send 'g_fp_result_1'
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, &g_extension_data_out), BIO_NO_ERROR);
	
	// get into 'g_read_result'
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, NULL, &authenticator_name), BIO_NO_ERROR);
	
	// verify 'g_fp_result_1' and 'g_read_result' hold the same data.
	VERIFY_EXPECTED_RESULT(g_read_result, g_fp_result_1);
	
	// verify the uptime counter
	BIO_ASSERT_TRUE(uptime > 0);

	// verify sender's name
	BIO_ASSERT_EQUAL(timesafe_strncmp(authenticator_name_buffer, TA_APP_NAME, MAX_APP_NAME_LEN), 0);
	
	return true;
}


static bool get_without_sender_name(void)
{
	uint64_t uptime = 0;

	// send 'g_fp_result_1'
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, &g_extension_data_out), BIO_NO_ERROR);
	
	// get into 'g_read_result'
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, NULL), BIO_NO_ERROR);
	
	// verify 'g_fp_result_1' and 'g_read_result' hold the same data.
	VERIFY_EXPECTED_RESULT(g_read_result, g_fp_result_1);
	
	// verify the uptime counter
	BIO_ASSERT_TRUE(uptime > 0);
	
	// verify the extension data
	BIO_ASSERT_EQUAL(timesafe_memcmp(extension_data_buffer_in, extension_data_buffer_out, EXTENSION_DATA_SIZE), 0);

	return true;
}


static bool multiple_set(void)
{
	uint64_t uptime = 0;

	// set several results of the same category
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, &g_extension_data_out), BIO_NO_ERROR);
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_2, &g_extension_data_out), BIO_NO_ERROR);
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_3, NULL), BIO_NO_ERROR);
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_iris_result_1, &g_extension_data_out), BIO_NO_ERROR);
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_iris_result_2, NULL), BIO_NO_ERROR);
	
	// verify g_fp_result_3 is what we got for FP category (no extension data)
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	g_extension_data_in.size = 0; // indicate 0 size buffer for ext. data and this should be OK since data size is 0 as well.
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	VERIFY_EXPECTED_RESULT(g_read_result, g_fp_result_3);
	BIO_ASSERT_EQUAL(g_extension_data_in.size, 0);

	// verify g_iris_result_2 is what we got for IRIS category (no extension data)
	g_read_result.method = BIO_IRIS_MATCHING;
	g_extension_data_in.size = 0; // indicate 0 size buffer for ext. data and this should be OK since data size is 0 as well.
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	VERIFY_EXPECTED_RESULT(g_read_result, g_iris_result_2);
	BIO_ASSERT_EQUAL(g_extension_data_in.size, 0);

	return true;
}


static bool null_uptime_counter(void)
{
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, NULL, &g_extension_data_in, &authenticator_name), BIO_INVALID_INPUT);
	
	return true;
}


static bool invalid_category(void)
{
	uint64_t uptime = 0;

	// try invalid category 'BIO_AUTH_METHOD_NUMBER' for set
	bio_result result = {BIO_AUTH_METHOD_NUMBER, true, BIO_NA, BIO_NA, BIO_NA};
	BIO_ASSERT_EQUAL(bio_set_auth_result(&result, NULL), BIO_INVALID_INPUT);
	
	// try invalid category 'BIO_AUTH_METHOD_NUMBER' for get
	g_read_result.method = BIO_AUTH_METHOD_NUMBER;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_INVALID_INPUT);

	return true;
}


static bool set_very_big_extension_data(void)
{
	bio_buffer big_ext_data = {extension_data_buffer_out, EXTENSION_DATA_SIZE*2};
	bio_result result = {BIO_FINGERPRINT_MATCHING, false, BIO_NA, BIO_NA, BIO_NA};
	BIO_ASSERT_EQUAL(bio_set_auth_result(&result, &big_ext_data), BIO_INTERNAL_ERROR);
	
	return true;
}

static bool set_two_different_extension_datas(void)
{
	uint8_t extension_data_buffer_out_1[] = {0x12, 0x34, 0x56, 0x78, 0x01, 0x00, 0x00, 0x00};
	bio_buffer extension_data_out_1 = {extension_data_buffer_out_1, sizeof(extension_data_buffer_out_1)};
	uint8_t extension_data_buffer_out_2[] = {0x12, 0x34, 0x56, 0x78};
	bio_buffer extension_data_out_2 = {extension_data_buffer_out_2, sizeof(extension_data_buffer_out_2)};

	uint64_t uptime = 0;

	// set with large ext. data
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, &extension_data_out_1), BIO_NO_ERROR);

	// set with smaller ext. data
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_iris_result_1, &extension_data_out_2), BIO_NO_ERROR);

	// get into 'g_read_result' and verify
	g_read_result.method = BIO_FINGERPRINT_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	VERIFY_EXPECTED_RESULT(g_read_result, g_fp_result_1);
	BIO_ASSERT_EQUAL(timesafe_memcmp(extension_data_buffer_in, extension_data_buffer_out_1, sizeof(extension_data_buffer_out_1)), 0);

	// get into 'g_read_result' and verify
	g_read_result.method = BIO_IRIS_MATCHING;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_NO_ERROR);
	VERIFY_EXPECTED_RESULT(g_read_result, g_iris_result_1);
	BIO_ASSERT_EQUAL(timesafe_memcmp(extension_data_buffer_in, extension_data_buffer_out_2, sizeof(extension_data_buffer_out_2)), 0);

	return true;
}

static bool get_with_short_extension_data_buffer(void)
{
	uint64_t uptime = 0;
	
	// set with ext. data of 'EXTENSION_DATA_SIZE' bytes.
	BIO_ASSERT_EQUAL(bio_set_auth_result(&g_fp_result_1, &g_extension_data_out), BIO_NO_ERROR);
	
	// get with shorter ext. data buffer.
	g_read_result.method = BIO_AUTH_METHOD_NUMBER;
	g_extension_data_in.size = EXTENSION_DATA_SIZE/2;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &authenticator_name), BIO_INVALID_INPUT);
	
	return true;
}


static bool get_with_short_sender_name_buffer(void)
{
	uint64_t uptime = 0;
	bio_buffer app_name = {(uint8_t*)authenticator_name_buffer, 2}; // only 2 bytes available for app name
	g_read_result.method = BIO_AUTH_METHOD_NUMBER;
	BIO_ASSERT_EQUAL(bio_get_auth_result(&g_read_result, &uptime, &g_extension_data_in, &app_name), BIO_INVALID_INPUT);
	
	return true;
}

static bool wait_for_file_event(void)
{
	bio_hlos_event_config event_config;
	
	strlcpy(event_config.file_poll.file_name, "/sys/bus/clockevents/devices/clockevent0/uevent", sizeof(event_config.file_poll.file_name));
	BIO_ASSERT_EQUAL(bio_wait_for_hlos_event(BIO_HLOS_EVENT_FILE_POLL, &event_config, 700), BIO_NO_ERROR);

	return true;
}


static bool wait_for_file_event_timeout(void)
{
	bio_hlos_event_config event_config;
	
	// try to read dev/null. should yield a timeout.
	strlcpy(event_config.file_poll.file_name, "/dev/null", sizeof(event_config.file_poll.file_name));
	BIO_ASSERT_EQUAL(bio_wait_for_hlos_event(BIO_HLOS_EVENT_FILE_POLL, &event_config, 1500), BIO_TIMEOUT_EXPIRATION);

	return true;
}


static bool wait_for_event_NULL_data(void)
{
	BIO_ASSERT_EQUAL(bio_wait_for_hlos_event(BIO_HLOS_EVENT_FILE_POLL, NULL, 2000), BIO_INVALID_INPUT);
	
	return true;
}

static bool wait_for_event_0_timeout(void)
{
	bio_hlos_event_config event_config;
	
	BIO_ASSERT_EQUAL(bio_wait_for_hlos_event(BIO_HLOS_EVENT_FILE_POLL, &event_config, 0), BIO_INVALID_INPUT);

	return true;
}


static bool wait_for_file_event_no_such_file(void)
{
	bio_hlos_event_config event_config;
	
	// point a file called 'c'
	event_config.file_poll.file_name[0] = 'f';
	event_config.file_poll.file_name[1] = '\0';

	// since there is no such file - verify we got 
	BIO_ASSERT_EQUAL(bio_wait_for_hlos_event(BIO_HLOS_EVENT_FILE_POLL, &event_config, 1500), BIO_FILE_ACCESS_ERROR);
	
	return true;
}


static bool wait_for_file_event_invalid_file_name(void)
{
	bio_hlos_event_config event_config;
	size_t i = 0;
	
	event_config.file_poll.file_name[0] = '\0'; // empty file name
	BIO_ASSERT_EQUAL(bio_wait_for_hlos_event(BIO_HLOS_EVENT_FILE_POLL, &event_config, 1500), BIO_INVALID_INPUT);

	for(i=0 ; i<sizeof(event_config.file_poll.file_name) ; i++)
	{
		event_config.file_poll.file_name[i] = 'c'; // non-NULL terminated string
	}
	
	BIO_ASSERT_EQUAL(bio_wait_for_hlos_event(BIO_HLOS_EVENT_FILE_POLL, &event_config, 1500), BIO_INVALID_INPUT);

	return true;
}


int run_bio_lib_tests(void)
{
	// this structure is being updated by 'execute_test_case'
	test_result results = {0,0,0};

	// L1: basic functionality
	execute_test_case(basic_set_get,              "basic_set_get",              &results);
	execute_test_case(set_different_methods,      "set_different_methods",      &results);
	execute_test_case(set_without_extension_data, "set_without_extension_data", &results);
	execute_test_case(get_without_extension_data, "get_without_extension_data", &results);
	execute_test_case(get_without_sender_name,    "get_without_sender_name",    &results);
	execute_test_case(multiple_set,               "multiple_set",               &results);
	execute_test_case(wait_for_file_event,        "wait_for_file_event",        &results);
	execute_test_case(wait_for_file_event_timeout,"wait_for_file_event_timeout",&results);
	
	// L2: invalid input
	execute_test_case(null_uptime_counter,      "null_uptime_counter",        &results);
	execute_test_case(invalid_category,         "invalid_category",           &results);
	execute_test_case(wait_for_event_NULL_data, "wait_for_event_NULL_data",   &results);
	execute_test_case(wait_for_event_0_timeout, "wait_for_event_0_timeout",   &results);
	execute_test_case(wait_for_file_event_no_such_file,      "wait_for_file_event_no_such_file",        &results);
	execute_test_case(wait_for_file_event_invalid_file_name, "wait_for_file_event_invalid_file_name",   &results);

	// L3: memory buffers management
	execute_test_case(set_very_big_extension_data,          "set_very_big_extension_data",          &results);
	execute_test_case(set_two_different_extension_datas,    "set_two_different_extension_datas",    &results);
	execute_test_case(get_with_short_extension_data_buffer, "get_with_short_extension_data_buffer", &results);
	execute_test_case(get_with_short_sender_name_buffer,    "get_with_short_sender_name_buffer",    &results);
	
	qsee_log(QSEE_LOG_MSG_DEBUG, "biometric library testing results: [%u/%u passed] [%u/%u failed]", results.passed, results.total, results.failed, results.total);

	// return '0' if and only if all tests passed successfully.
	return (results.passed == results.total ? 0 : -1);
}
