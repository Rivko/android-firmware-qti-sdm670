/*
 * Copyright (c) 2014,2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __MMI_AGENT_H__
#define __MMI_AGENT_H__

#include "mmi_module.h"
#include "xmlparse.h"

/**handle socket command*/
int handle_init(mmi_module_t * module, msg_t * req, msg_t * resp);
int handle_deinit(mmi_module_t * module, msg_t * req, msg_t * resp);
int handle_query(mmi_module_t * module, msg_t * req, msg_t * resp);
int handle_run(mmi_module_t * module, msg_t * req, msg_t * resp);
int handle_stop(mmi_module_t * module, msg_t * req, msg_t * resp);
int handle_ctrl(mmi_module_t * module, msg_t * req, msg_t * resp);
#endif
