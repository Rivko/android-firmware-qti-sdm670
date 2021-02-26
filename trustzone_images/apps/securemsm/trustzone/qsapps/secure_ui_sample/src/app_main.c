/*
@file app_main.c
@brief Secure UI Sample App main entry point.

*/

/*===========================================================================
 Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
 10/20/17    ac     Added command for TA clean up before tz_app_shutdown
 07/17/16    dr     Factor out and split sample code into SUI and TUI dialogs samples
 07/29/15    ng     Enable GP dialogs on 8996
 02/05/15    rz     Modified according to the secure indicator changes in dialogParams
 08/14/14    rz     Logo is read before the indicator
 01/30/14    rk     Removed fs.h and added qsee_fs.h
 05/26/13	 sn		Initial Version
===========================================================================*/

#include <comdef.h>
#include <string.h>
#include <stdio.h>
#include <stringl.h>
#include "qsee_log.h"
#include "SecureUI.h"
#include "SecureUILib.h"

#define COMMAND_UNSUPPORTED_ERROR (-100)

//define ranges for commands used in demo and test scenarios for secure UI

//low level API (sec_ui) demo commands
#define CMD_ID_FIRST_SUI_DEMO 0
#define CMD_ID_LAST_SUI_DEMO  9

//GP dialogs (qsee_tui_dialog) demo commands
#define CMD_ID_FIRST_TUI_DIALOGS_DEMO 10
#define CMD_ID_LAST_TUI_DIALOGS_DEMO  19

//optional test support commands
#define CMD_ID_FIRST_TEST_SUPPORT 100
#define CMD_ID_LAST_TEST_SUPPORT  100

//TA clean up command
#define CMD_ID_CONTEXT_CLEAN_UP  50


//functions declarations
void sui_app_init(void);
void sui_app_cmd_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
void sui_app_shutdown(void);

void tui_dialogs_app_init(void);
void tui_dialogs_app_cmd_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
void tui_dialogs_app_shutdown(void);

void sui_test_cmd_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

void context_clean_up(void);

struct send_cmd{
  uint32_t cmd_id;
};

struct send_cmd_rsp{
  int32_t status;
};


///////////////////////////////////////////////////////////////////////////////

void tz_app_init(void) {
	sui_app_init();
	tui_dialogs_app_init();
}

void tz_app_cmd_handler(void* cmd, uint32_t cmdlen,
                        void* rsp, uint32_t rsplen)
{
  struct send_cmd *cmd_ptr = (struct send_cmd *)cmd;
  struct send_cmd_rsp *rsp_ptr = (struct send_cmd_rsp *)rsp;
  int32_t sec_ui_retval = 0;

  qsee_log(QSEE_LOG_MSG_DEBUG, "TZ App cmd handler @ 0x%08X", tz_app_cmd_handler);

  if ( NULL == cmd || NULL == rsp) {
	qsee_log(QSEE_LOG_MSG_ERROR, "cmd handler received null cmd/rsp buffers");
    return;
  }

  if ((cmdlen < sizeof(struct send_cmd)) || (rsplen < sizeof(struct send_cmd_rsp))) {
	qsee_log(QSEE_LOG_MSG_ERROR, "cmd handler received too short cmd/rsp buffers %d vs %d, %d vs %d",cmdlen, sizeof(struct send_cmd),rsplen ,sizeof(struct send_cmd_rsp));
    return;
  }
  


  qsee_log(QSEE_LOG_MSG_DEBUG, "cmd_id = %u", cmd_ptr->cmd_id);

  secure_memset(rsp, 0, rsplen);
  rsp_ptr->status = 0;

  if (secUiProcessCmd(cmd,cmdlen,rsp,rsplen)) {
	  //this command was intended and processed by secure UI internally - no action is needed
	  return;
  }
  else if (cmd_ptr->cmd_id >= CMD_ID_FIRST_SUI_DEMO && cmd_ptr->cmd_id<=CMD_ID_LAST_SUI_DEMO) {
	  sui_app_cmd_handler(cmd, cmdlen, rsp, rsplen);
  }
  else if (cmd_ptr->cmd_id >= CMD_ID_FIRST_TUI_DIALOGS_DEMO && cmd_ptr->cmd_id<=CMD_ID_LAST_TUI_DIALOGS_DEMO) {
	  tui_dialogs_app_cmd_handler(cmd, cmdlen, rsp, rsplen);
  }
  else if (cmd_ptr->cmd_id == CMD_ID_CONTEXT_CLEAN_UP) {
          context_clean_up();
  }
#ifdef SUI_TEST_SUPPORT
  else if (cmd_ptr->cmd_id >= CMD_ID_FIRST_TEST_SUPPORT && cmd_ptr->cmd_id<=CMD_ID_LAST_TEST_SUPPORT) {
	  sui_test_cmd_handler(cmd, cmdlen, rsp, rsplen);
  }
#endif //SUI_TEST_SUPPORT
  else {
      rsp_ptr->status = COMMAND_UNSUPPORTED_ERROR;
      qsee_log(QSEE_LOG_MSG_DEBUG, "Command %u is not supported by any of the Secure UI demos or tests", cmd_ptr->cmd_id);
  }
}

void context_clean_up(void)
{
  tui_dialogs_app_shutdown();
  sui_app_shutdown();
  int32_t status = sec_ui_hlos_release();
  if(status != 0){
    qsee_log(QSEE_LOG_MSG_ERROR, "sec_ui_hlos_release returned %d", status);
  }
}

void tz_app_shutdown(void)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "secure_ui_sample shutdown");
}
