/*
 * Copyright (c) 2014-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi.h"
#include "utils.h"
#include "module.h"
#include "lang.h"
#include "diag_ext.h"

static void do_ctrl_stat(msg_t * resp) {
    char buf[NAME_MAX] = { 0 };
    int count = 0;

    /*Get the layout */
    layout *lay = get_main_layout();

    if(lay == NULL || lay->m_listview == NULL) {
        MMI_ALOGE("No layout for MAIN\n");
        return;
    }
    count = lay->m_listview->get_fail_count();

    snprintf(buf, sizeof(buf), "%s:%d;%s:%d;", KEY_MMI_STAT, MMI_IDLE, KEY_FAIL_COUNT, count);
    MMI_ALOGI("Get test fail count: %d ", count);
    resp->result = SUCCESS;
    strlcpy(resp->msg, buf, sizeof(resp->msg));
}

static void do_ctrl_reconfig(msg_t * resp, const char *cfg) {
    char buf[NAME_MAX] = { 0 };
    int num = reconfig(cfg);

    MMI_ALOGI("Get test case count: %d ", num);

    snprintf(buf, sizeof(buf), "%s:%d;", KEY_CASE_NUM, num);
    resp->result = SUCCESS;
    strlcpy(resp->msg, buf, sizeof(resp->msg));
}

static void do_ctrl_clear(msg_t * resp) {
    layout *lay = get_main_layout();

    if(lay != NULL && lay->m_listview != NULL) {
        lay->m_listview->reset_result();
    }

    resp->result = clear_file(g_res_file);
}

static void do_ctrl_runcase(msg_t * resp, string module_name) {
    module_info *mod = NULL;
    layout *lay = NULL;

    resp->result = FAILED;
    MMI_ALOGI("start test module:[%s] for pcba", MMI_STR(module_name.c_str()));

    if(module_name.empty()) {
        MMI_ALOGE("Invalid parameter, module_name is empty");
        return;
    }

    mod = g_modules_map[module_name];
    if(mod == NULL) {
        MMI_ALOGE("module:[%s] not find", module_name.c_str());
        return;
    }

    lay = get_layout(LAYOUT_PCBA);
    if(lay == NULL) {
        MMI_ALOGE("layout file(%s) not find", LAYOUT_PCBA);
        return;
    }

    /**switch layout to pcba and initial the module*/
    switch_cur_layout_locked(lay, mod);

    layout *curlay = acquire_cur_layout();
    button *obj = curlay->find_button_by_name(KEY_STR_TITLE);

    if(obj != NULL)
        obj->set_text(get_string(KEY_TITLE_PCBA));
    invalidate();
    release_cur_layout();

    module_exec_pcba(mod);
    resp->result = SUCCESS;

}

static void do_ctrl_runall(msg_t * resp) {
    resp->result = SUCCESS;
    start_all(false);
}

static void do_ctrl_listcase(msg_t * resp, const char *filepath) {
    list < item_t * >*items;
    FILE *fd = NULL;
    char buf[64];

    resp->result = FAILED;
    MMI_ALOGI("update listcase to '%s'", filepath);

    fd = fopen(filepath, "w");
    if(!fd) {
        MMI_ALOGE("file(%s) open fail, error=%s", filepath, strerror(errno));
        return;
    }
    /*Get the layout */
    layout *lay = get_main_layout();

    if(lay == NULL || lay->m_listview == NULL || lay->m_listview->get_items() == NULL) {
        MMI_ALOGE("Invalid layout or invalid listcase\n");
        fclose(fd);
        return;
    }

    items = lay->m_listview->get_items();
    list < item_t * >::iterator iter;
    for(iter = items->begin(); iter != items->end(); iter++) {
        item_t *obj = (item_t *) (*iter);

        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%s\n", obj->mod->module);
        buf[sizeof(buf) - 1] = '\0';
        fwrite(buf, strlen(buf), 1, fd);
    }
    fclose(fd);
    fd = NULL;
    resp->result = SUCCESS;

}

static void do_ctrl_exitcase(msg_t * resp, string module_name, string module_result) {
    module_info *mod = NULL;

    resp->result = FAILED;

    if(module_name.empty()) {
        MMI_ALOGE("Invalid parameter, module_name is empty");
        return;
    }

    mod = g_modules_map[module_name];
    if(mod == NULL) {
        MMI_ALOGE("module:[%s] not find", module_name.c_str());
        return;
    }

    if(atoi(module_result.c_str()) == EXIT_TEST_SUCCESS)
        process_exit(mod, SUCCESS);
    else if(atoi(module_result.c_str()) == EXIT_TEST_FAILED)
        process_exit(mod, FAILED);
    else
        process_exit(mod);

    resp->result = SUCCESS;
}

static void do_ctrl_append(msg_t * resp, string module_name, string module_data) {
    module_info *mod = NULL;

    resp->result = FAILED;

    if(module_name.empty()) {
        MMI_ALOGE("Invalid parameter, module_name is empty");
        return;
    }

    mod = g_modules_map[module_name];
    if(mod == NULL) {
        MMI_ALOGE("module:[%s] not find", module_name.c_str());
        return;
    }

    strlcat(mod->data, module_data.c_str(), SIZE_512);
    flush_result();

    resp->result = SUCCESS;
}

static void do_ctrl_utility(msg_t * resp, string utility_id) {
    module_info *mod = NULL;

    resp->result = FAILED;

    if(utility_id.empty()) {
        MMI_ALOGE("Invalid parameter, utility_id is empty");
        return;
    }

    int id = atoi(utility_id.c_str());

    if(id == ID_SHIPMODE) {
        enter_shipmode_thread();
    }

    resp->result = SUCCESS;
}


void notify_controller(module_info * mod) {

    module_info *target_mod = NULL;
    char tmp[SIZE_1K] = { 0 };

    if(mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return;
    }

    snprintf(tmp, sizeof(tmp), "\n[%s]\nTimestamp = %lu\nResult = %s \nTestTime_Sec = %.f \n", mod->module,
             mod->last_time, mod->result == SUCCESS ? KEY_PASS : KEY_FAIL, mod->duration);
    strlcat(tmp, mod->data, sizeof(tmp));

      /**Notify diag */
    target_mod = g_controller_map[CLIENT_DIAG_NAME];
    if(target_mod != NULL) {
        send_cmd(target_mod->socket_fd, CLIENT_DIAG_NAME, CMD_RESULT, NULL, tmp, strlen(tmp));
    }

      /**Notify debug port */
    target_mod = g_controller_map[CLIENT_DEBUG_NAME];
    if(target_mod != NULL) {
        send_cmd(target_mod->socket_fd, CLIENT_DEBUG_NAME, CMD_RESULT, NULL, tmp, strlen(tmp));
    }

}

/**
*	  Handle ctrl command from client
*/
int handle_ctr_msg(msg_t * msg, module_info * mod) {

    unordered_map < string, string > params;
    msg_t resp;
    int sock_fd = -1;

    if(msg == NULL || mod == NULL) {
        MMI_ALOGE("Invalid param, NULL pointer");
        return -1;
    }

    MMI_ALOGI("start handle command:%s for module:[%s]", MMI_STR(msg->subcmd), msg->module);

    sock_fd = mod->socket_fd;

    memcpy(&resp, msg, sizeof(msg_t));
    parse_parameter(msg->msg, params);

    if(!strcmp(msg->subcmd, SUBCMD_STAT)) {
        do_ctrl_stat(&resp);
    } else if(!strcmp(msg->subcmd, SUBCMD_RECONFIG)) {
        do_ctrl_reconfig(&resp, params[KEY_CFG_PATH].c_str());
    } else if(!strcmp(msg->subcmd, SUBCMD_CLEAR)) {
        do_ctrl_clear(&resp);
    } else if(!strcmp(msg->subcmd, SUBCMD_RUNCASE)) {
        do_ctrl_runcase(&resp, params[KEY_CASE_NAME]);
    } else if(!strcmp(msg->subcmd, SUBCMD_RUNALL)) {
        do_ctrl_runall(&resp);
    } else if(!strcmp(msg->subcmd, SUBCMD_LISTCASE)) {
        do_ctrl_listcase(&resp, params[KEY_TESTLIST_PATH].c_str());
    } else if(!strcmp(msg->subcmd, SUBCMD_EXITCASE)) {
        do_ctrl_exitcase(&resp, params[KEY_CASE_NAME], params[KEY_EXIT_RESULT]);
    } else if(!strcmp(msg->subcmd, SUBCMD_APPEND_TO_RES)) {
        do_ctrl_append(&resp, params[KEY_CASE_NAME], params[KEY_CASE_DATA]);
    } else if(!strcmp(msg->subcmd, SUBCMD_UTILITY)) {
        do_ctrl_utility(&resp, params[KEY_UTILITY_ID]);
    } else {
        resp.result = FAILED;
    }

   /**Reconfig will refresh all module_info*/
    if(!strcmp(msg->subcmd, SUBCMD_RECONFIG)) {
        sock_fd = (g_controller_map[CLIENT_DIAG_NAME])->socket_fd;
    }
    MMI_ALOGI("command:%s handle finished for module:[%s], result=%s",
              MMI_STR(msg->subcmd), msg->module, MMI_RESULT(resp.result));

    send_msg(sock_fd, &resp);
    return 0;
}
