/**************************************************************
 * Copyright (c)2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **************************************************************/

/*===========================================================================

                            EDIT HISTORY FOR FILE

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
07/17/16      dr     Update sui_test_cmd_handler signature
06/20/15      dr     Add unit tests support
04/26/16      dr     Initial version

===========================================================================*/

#include "test_support.h"

#include "qsee_tui_dialogs.h"
#include "TouchApiDefs.h"
#include <qsee_log.h>

struct send_cmd_rsp{
  int32_t status;
};

extern int32_t run_sui_unittests();
extern sec_touch_cmd_t manage_layout_event(sec_touch_err_t err, struct tsFingerData *fingers, int32_t *timeout);


static sec_touch_cmd_t emulate_touchevent(uint32_t event, uint32_t x, uint32_t y)
{
	int32_t timeout;
	struct tsFingerData fingers = {0};
	fingers.finger[0].event = event;
	fingers.finger[0].x = x;
	fingers.finger[0].y = y;
	return manage_layout_event(SEC_TOUCH_ERR_DATA, &fingers, &timeout);
}

void sui_test_cmd_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen) {
	const struct test_touch_events* tevents = NULL;
	struct tsFingerData fingers = {0};
	int32_t timeout = 0;
	sec_touch_cmd_t touch_res = SEC_TOUCH_CMD_INVALID;
	struct test_cmd *cmd_ptr = (struct test_cmd *)cmd;
	struct send_cmd_rsp *rsp_ptr = (struct send_cmd_rsp *)rsp;

	if ( NULL == cmd || NULL == rsp) {
		qsee_log(QSEE_LOG_MSG_ERROR, "cmd handler received null cmd/rsp buffers");
		return;
	}

	if ((cmdlen < sizeof(struct test_cmd)) || (rsplen < sizeof(struct send_cmd_rsp))) {
		qsee_log(QSEE_LOG_MSG_ERROR, "cmd handler received too short cmd/rsp buffers %d vs %d, %d vs %d",cmdlen, sizeof(struct test_cmd),rsplen ,sizeof(struct send_cmd_rsp));
		return;
	}

	rsp_ptr->status = SUI_TEST_SUCCESS;

	switch (cmd_ptr->testFunction) {
	case SUI_TEST_FUNC_EMULATE_START_TOUCH:
		touch_res = manage_layout_event(SEC_TOUCH_ERR_STARTED, &fingers, &timeout);
		if (touch_res != SEC_TOUCH_CMD_CONTINUE) {
			qsee_log(QSEE_LOG_MSG_ERROR, "%s: Error in SUI_TEST_FUNC_EMULATE_START_TOUCH: %d",__func__, touch_res);
		}
		break;
	case SUI_TEST_FUNC_EMULATE_TOUCH_EVENTS:
		tevents = &(cmd_ptr->testData.touch_events);
		qsee_log(QSEE_LOG_MSG_DEBUG, "%s: SUI_TEST_FUNC_EMULATE_TOUCH_EVENTS - got %d events",
				__func__, tevents->numEvents);
		for (uint32_t i=0; i < tevents->numEvents; i++) {
			uint32_t event_type = tevents->event[i].isTouchDown ? TLAPI_TOUCH_EVENT_DOWN : TLAPI_TOUCH_EVENT_UP;
			touch_res = emulate_touchevent(event_type,tevents->event[i].x,tevents->event[i].y);
			if (touch_res != SEC_TOUCH_CMD_CONTINUE) {
				qsee_log(QSEE_LOG_MSG_ERROR, "%s: Error in SUI_TEST_FUNC_EMULATE_TOUCH_EVENTS - event#:%d, res:%d",
					__func__, i, touch_res);
				rsp_ptr->status = SUI_TEST_FAIL;
			}
		}
		break;
	case SUI_TEST_FUNC_RUN_TUI_UT:
		if (run_sui_unittests() == FALSE) {
			rsp_ptr->status = SUI_TEST_FAIL;
		}
		break;
	default:
		qsee_log(QSEE_LOG_MSG_ERROR, "%s: error - unknown test function: %u", __func__, cmd_ptr->testFunction);
		rsp_ptr->status = SUI_TEST_FAIL;
	}
}


