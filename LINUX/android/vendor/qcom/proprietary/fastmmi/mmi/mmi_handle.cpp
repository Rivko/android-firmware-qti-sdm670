/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi.h"
#include "utils.h"
#include "module.h"
#include "lang.h"
#include "controller.h"

/**
 *	  Handle print from client
 */
int handle_print(msg_t * msg, module_info * mod) {

    char tmp[256] = { 0 };
    textview *mod_tv = NULL;
    textview *pcba_tv = NULL;

    if(msg == NULL || mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return -1;
    }

    MMI_ALOGI("handle print start for module=[%s], cmd=%s, subcmd=%s",
              mod->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));

    if(msg->msg_id == DATA || msg->msg_id == PRINT_DATA) {
        strlcpy(mod->data, msg->msg, sizeof(mod->data));
        sem_post(&g_data_print_sem);
    }

    if(msg->msg_id == PRINT || msg->msg_id == PRINT_DATA) {
        layout *lay = get_layout(mod);
        layout *lay_pcba = get_layout(LAYOUT_PCBA);

        if(lay != NULL)
            mod_tv = lay->find_textview_by_name(KEY_DISPLAY);

        if(lay_pcba != NULL)
            pcba_tv = lay_pcba->find_textview_by_name(KEY_DISPLAY_PCBA);

        if(pcba_tv != NULL && !strcmp(msg->subcmd, SUBCMD_PCBA)) {
            snprintf(tmp, sizeof(tmp), "[%s]\n", mod->module);
            pcba_tv->append_text(tmp);
            pcba_tv->append_text(msg->msg);

            MMI_ALOGI("append msg(%s) to textview(%s) for module=[%s]",
                      MMI_STR(msg->msg), pcba_tv->get_name(), mod->module);
        } else if(mod_tv != NULL && !strcmp(msg->subcmd, SUBCMD_MMI)) {
            mod_tv->set_text(msg->msg);

            MMI_ALOGI("set msg(%s) to textview(%s) for module=[%s]",
                      MMI_STR(msg->msg), mod_tv->get_name(), mod->module);
        }

        invalidate();
    }

    MMI_ALOGI("handle print finished for module=[%s], cmd=%s, subcmd=%s",
              mod->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));
    return 0;
}

/**
 *    Handle run response from client
 */
int handle_run(msg_t * msg, module_info * mod) {

    char temp[32] = { 0 };
    int ret, i = -1;

    if(msg == NULL || mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return -1;
    }

    MMI_ALOGI("handle run start for module=[%s], cmd=%s, subcmd=%s",
              msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));

    if(!strcmp(msg->subcmd, SUBCMD_MMI)) {
        mod->running_state = MODULE_IDLE;
        if(is_autoswitch_module(mod)) {
            process_exit(mod, msg->result);
        }
    } else if(!strcmp(msg->subcmd, SUBCMD_PCBA)) {
        if(msg->result != ERR_UNKNOW) {
            mod->result = msg->result;
        }
        /**update time duration*/
        time(&mod->last_time);
        mod->running_state = MODULE_IDLE;
        mod->duration = difftime(mod->last_time, mod->start_time);
        flush_result();
        notify_controller(mod);
        invalidate();
    } else {
        MMI_ALOGE("Received invalid subcmd=%s ", MMI_STR(msg->subcmd));
        ret = -1;
    }

    MMI_ALOGI("handle run finished for module=[%s], cmd=%s, subcmd=%s",
              msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));
    return 0;
}

/**
 *	  Handle print from client
 */
int handle_query(msg_t * msg, module_info * mod) {
    unordered_map < string, string > params;
    char temp[32] = { 0 };
    uint32_t i = -1;

    if(msg == NULL || mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return -1;
    }

    MMI_ALOGI("handle query start for module=[%s], cmd=%s, subcmd=%s",
              msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));

    parse_parameter(msg->msg, params);
    string size_value = params[KEY_STR_EXTRACMDSIZE];

    if(size_value.empty() || atoi(size_value.c_str()) <= 0) {
        MMI_ALOGW("no valid extra command received for module=[%s]", msg->module);
        return -1;
    }

    mod->extracmd.size = atoi(size_value.c_str());
    mod->extracmd.is_valid = true;

    for(i = 0; i < mod->extracmd.size; i++) {
        snprintf(temp, sizeof(temp), "%s%d", KEY_STR_EXTRACMD, i);
        if(params[temp] != "") {
            mod->extracmd.cmd_list.push_back(params[temp]);
            MMI_ALOGI("get extra command=%s for module=[%s]", params[temp].c_str(), msg->module);
        }
    }

    /**Only update the first command to button*/
    layout *lay = get_layout(mod);
    button *obj = lay->find_button_by_name(KEY_STR_EXTRACMD);

    if(obj != NULL) {
        MMI_ALOGI("Set extra cmd(%s) to button(%s)", get_string(mod->extracmd.cmd_list.front()), KEY_STR_EXTRACMD);
        obj->set_text(get_string(mod->extracmd.cmd_list.front()));
    } else {
        MMI_ALOGW("Not find button(%s) in layout(%s)", KEY_STR_EXTRACMD, lay->get_layout_path());
    }

    MMI_ALOGI("handle query finished for module=[%s], cmd=%s, subcmd=%s",
              msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));
    return 0;
}
