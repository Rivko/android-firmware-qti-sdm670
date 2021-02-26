/**************************************************************
 * Copyright (c)2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

/*===========================================================================

                            EDIT HISTORY FOR FILE

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
06/20/16      dr     Add unit tests support
04/26/16      dr     Initial version

NOTE: This file is shared between TZ and HLOS and should be always kept in sync!
===========================================================================*/

#ifndef APPS_SECUREMSM_TRUSTZONE_QSAPPS_SECURE_UI_SAMPLE_TEST_SUPPORT_H_
#define APPS_SECUREMSM_TRUSTZONE_QSAPPS_SECURE_UI_SAMPLE_TEST_SUPPORT_H_

#include <comdef.h>

/* test command id */
#define SEC_UI_TEST_SUPPORT_CMD100             100

#define SUI_TEST_SUCCESS 0
#define SUI_TEST_FAIL    -1

#define MAX_TOUCH_EVENTS 50

enum {
	SUI_TEST_FUNC_EMULATE_START_TOUCH,
	SUI_TEST_FUNC_EMULATE_TOUCH_EVENTS,
	SUI_TEST_FUNC_RUN_TUI_UT,
	SUI_TEST_FUNC_LAST,
};

typedef struct test_touch_event {
	int32_t x;					/* x coordinate for touch event */
	int32_t y;					/* y coordinate for touch event */
	uint32_t isTouchDown;		/* 1 - touch down, 0 - touch up */
} touch_event_t;

typedef struct test_touch_events {
	struct test_touch_event event[MAX_TOUCH_EVENTS];
	uint32_t numEvents;
} test_touch_events_t;

/* data types used by various commands should be added to this struct */
typedef struct test_data {
	union {
		struct test_touch_events touch_events;
	};
} test_data_t;

typedef struct test_cmd {
	uint32_t cmd_id;
	uint32_t testFunction;
	struct test_data testData;
} test_cmd_t;

#endif /* APPS_SECUREMSM_TRUSTZONE_QSAPPS_SECURE_UI_SAMPLE_TEST_SUPPORT_H_ */
