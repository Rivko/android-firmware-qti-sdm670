/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "agent.h"

/**
*    Handle INIT command
*/
int handle_init(mmi_module_t * module, msg_t * req, msg_t * resp) {

    int ret = 0;

    unordered_map < string, string > params;

    if(module == NULL || req == NULL || resp == NULL) {
        MMI_ALOGE("Invalid param, NULL pointer");
        return -1;
    }

    MMI_ALOGI("handle init start for module:[%s]", req->module);

    /** Get the input params for module */
    parse_parameter(req->msg, params);
    params[KEY_MODULE_NAME] = req->module;
    ret = module->methods->module_init(module, params);

    /** Store the result to response msg */
    resp->result = ret;

    MMI_ALOGI("handle init finished for module:[%s], result=%s", req->module, MMI_RESULT(ret));

    return ret;
}

/**
*    Handle DEINIT command
*/
int handle_deinit(mmi_module_t * module, msg_t * req, msg_t * resp) {

    int ret = 0;

    if(module == NULL || req == NULL || resp == NULL) {
        MMI_ALOGE("Invalid param, NULL pointer");
        return -1;
    }
    MMI_ALOGI("handle deinit start for module:[%s]", req->module);

    ret = module->methods->module_deinit(module);

    /** Store the result to response msg */
    resp->result = ret;

    MMI_ALOGI("handle deinit finished for module:[%s], result=%s", req->module, MMI_RESULT(ret));

    return ret;
}

/**
*    Handle QUERY command
*/
int handle_query(mmi_module_t * module, msg_t * req, msg_t * resp) {

    int ret = 0;

    if(module == NULL || req == NULL || resp == NULL) {
        MMI_ALOGE("Invalid param, NULL pointer");
        return -1;
    }

    MMI_ALOGI("handle query start for module:[%s]", req->module);
    /**Contruct msg to be sended to server*/
    char *p = resp->msg;

    for(uint32_t i = 0; i < module->supported_cmd_list_size; i++) {
        snprintf(p, sizeof(resp->msg), "%s%d:%s;", KEY_STR_EXTRACMD, i, module->supported_cmd_list[i]);
        p = resp->msg + strlen(resp->msg);
    }
    snprintf(p, sizeof(resp->msg), "%s:%d;", KEY_STR_EXTRACMDSIZE, module->supported_cmd_list_size);
    /** Store the result to response msg */
    resp->result = ret;

    MMI_ALOGI("extra cmd(%s) for moduld=[%s]", MMI_STR(resp->msg), req->module);
    MMI_ALOGI("handle query finished for module:[%s], result=%s", req->module, MMI_RESULT(ret));

    return ret;
}

/**
*    Handle RUN command
*/
int handle_run(mmi_module_t * module, msg_t * req, msg_t * resp) {

    int ret = 0;

    unordered_map < string, string > params;

    if(module == NULL || req == NULL || resp == NULL) {
        MMI_ALOGE("Invalid param, NULL pointer");
        return -1;
    }

    MMI_ALOGI("handle run start for module:[%s]", req->module);

    /** Get the input params for module */
    parse_parameter(req->msg, params);
    params[KEY_MODULE_NAME] = req->module;
    ret = module->methods->module_run(module, req->subcmd, params);

    /** Store the result to response msg */
    resp->result = ret;

    MMI_ALOGI("handle run finished for module:[%s], result=%s", req->module, MMI_RESULT(ret));

    return ret;
}

/**
*    Handle STOP command
*/
int handle_stop(mmi_module_t * module, msg_t * req, msg_t * resp) {

    int ret = 0;

    if(module == NULL || req == NULL || resp == NULL) {
        MMI_ALOGE("Invalid param, NULL pointer");
        return -1;
    }
    MMI_ALOGI("handle stop start for module:[%s]", req->module);

    ret = module->methods->module_stop(module);

    /** Store the result to response msg */
    resp->result = ret;

    MMI_ALOGI("handle stop finished for module:[%s], result=%s", req->module, MMI_RESULT(ret));

    return ret;
}
