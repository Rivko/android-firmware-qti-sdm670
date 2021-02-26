/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmid.h"

#define PCBA_REPLY_STR "Test finished, please check the result\n"
#define PCBA_EXTERNAL_LOOPBACK_PASS "External Loopback Test Pass"
#define PCBA_EXTERNAL_LOOPBACK_FAIL "External Loopback Test Fail"
#define PCBA_EXTERNAL_LOOPBACK_POWER "Power"

static void config_wlan() {
    char temp[256] = { 0 };
    snprintf(temp, sizeof(temp), "%s wlan0 up", APP_WIFI_IFCONFIG_BIN);
    system(temp);
}

static void set_con_mode() {
    char tmp[32] = { 0 };
    FILE *fp = NULL;

    fp = fopen("/sys/module/wlan/parameters/con_mode", "w+");
    if(fp == NULL) {
        ALOGE("Fail to set con mode for WLAN\n");
        return;
    }

    snprintf(tmp, sizeof(tmp), "%s", "5");
    fwrite(tmp, sizeof(char), strlen(tmp), fp);

    if(fp != NULL)
        fclose(fp);
}

 /*
  * purpose
  *     check get_ftm_test_config
  *
  * requires
  *
  * guarantees
  *       return 0  success
  */
static void get_ftm_test_config(char *config, int size) {
    char soundCardInfo[256] = { 0 };
    char sound_card[256] = { 0 };
    FILE *fp = NULL;
    char config_path[256] = { 0 };
    if((fp = fopen("/proc/asound/cards", "r")) != NULL) {
        while(fgets(soundCardInfo, sizeof(soundCardInfo), fp) != NULL)
            sscanf(soundCardInfo, "%*s%*s%*s%*s%s", sound_card);
        fclose(fp);

        snprintf(config_path, sizeof(config_path), "%s_%s", FTM_TEST_CONFIG_DEFAULT, sound_card);
    } else {
        ALOGE("file(/proc/asound/cards) open fail, error=%s", strerror(errno));
    }

    if(check_file_exist(config_path)) {
        strlcpy(config, config_path, size);
    } else
        strlcpy(config, FTM_TEST_CONFIG_DEFAULT, size);

    ALOGI("ftm test config file: '%s'", config);
}

  /**
  *    Handle print from client
  */
void run_audio_ftm(msg_t * msg, msg_t * resp) {

    app_status_t *app = get_app_status(msg->subcmd);
    char ftm_config_file[256] = { 0 };
    get_ftm_test_config(ftm_config_file, sizeof(ftm_config_file));

    if(app != NULL) {
        mutex_locker::autolock _L(app->lock);

        char *args[MAX_PARAM_NUM];
        exec_cmd_t execmd;
        char result[SIZE_2K] = { 0 };
        char buf[SIZE_1K] = { 0 };
        char tmp[256] = { 0 };
        int i = 0;

        args[i++] = APP_MM_AUDIO_FTM_BIN;
        args[i++] = "-c";
        args[i++] = ftm_config_file;

        //params from high
        char *saveptr = NULL;
        char *tok = strtok_r(msg->msg, ";", &saveptr);

        while(tok != NULL && i < MAX_PARAM_NUM - 1) {
            args[i++] = tok;
            ALOGI("%s", tok);
            tok = strtok_r(NULL, ";", &saveptr);
        }

        args[i] = NULL;

        memset(&execmd, 0, sizeof(exec_cmd_t));
        execmd.cmd = APP_MM_AUDIO_FTM_BIN;
        execmd.pid = &app->proc_id;
        execmd.exit_str = NULL;
        execmd.result = result;
        execmd.size = sizeof(result);
        execmd.params = (char **) args;

        app->activated = true;
        int ret = exe_cmd(NULL, &execmd);

        if(ret != SUCCESS) {
            ALOGE("command(%s) exec fail", execmd.cmd);
            return;
        }

        resp->result = SUCCESS;
        ALOGI("run_audio_ftm started proc id: %d", app->proc_id);

        //analysis result
        char *p = result;
        char *ptr;
        char *tokptr = NULL;

        while(*p != '\0') {     /*print every line of scan result information */
            ptr = tmp;
            while(*p != '\n' && *p != '\0') {
                *ptr++ = *p++;
            }

            p++;
            *ptr = '\0';

            ALOGI("command:'%s' exec result:%s", execmd.cmd, tmp);
            ptr = strstr(tmp, PCBA_EXTERNAL_LOOPBACK_POWER);
            if(ptr != NULL) {
                strlcat(buf, tmp, strlen(tmp));
                ptr = strtok_r(tmp, ":d", &tokptr);
                if(ptr != NULL)
                    ptr = strtok_r(NULL, ":d", &tokptr);

            }
            ptr = strstr(tmp, PCBA_EXTERNAL_LOOPBACK_PASS);
            if(ptr != NULL) {
                strlcat(buf, PCBA_EXTERNAL_LOOPBACK_PASS, strlen(PCBA_EXTERNAL_LOOPBACK_PASS));
            }
            ptr = strstr(tmp, PCBA_EXTERNAL_LOOPBACK_FAIL);
            if(ptr != NULL) {
                strlcat(buf, PCBA_EXTERNAL_LOOPBACK_FAIL, strlen(PCBA_EXTERNAL_LOOPBACK_FAIL));

            }
        }
    }
}

/**
 *	  Handle print from client
 */
void run_ftm_daemon(msg_t * msg, msg_t * resp) {
    app_status_t *app = get_app_status(msg->subcmd);

    if(app != NULL) {
        mutex_locker::autolock _L(app->lock);

        char *args[MAX_PARAM_NUM];
        int i = 0;

        args[i++] = APP_FTMDAEMON_BIN;

        char *saveptr = NULL;
        char *tok = strtok_r(msg->msg, ";", &saveptr);

        while(tok != NULL && i < MAX_PARAM_NUM - 1) {
            args[i++] = tok;
            ALOGI("%s", tok);
            tok = strtok_r(NULL, ";", &saveptr);
        }


        args[i] = NULL;

        int ret = fork_daemon(APP_FTMDAEMON_BIN, args, &app->proc_id);

        if(!ret) {
            app->activated = true;
            resp->result = SUCCESS;
            ALOGI("ftmdaemon started proc id: %d", app->proc_id);

        }

    }

}

/**
 *	  Handle print from client
 */
void stop_daemon(msg_t * msg, msg_t * resp) {
    app_status_t *app = get_app_status(msg->subcmd);

    if(app != NULL) {
        if(app->activated) {
            kill_proc(app->proc_id);
            ALOGI("stop_daemon killed proc id: %d", app->proc_id);
            app->activated = false;
            app->proc_id = -1;
            resp->result = SUCCESS;
        }
    }
}

/**
 *	  Handle hello from client
 */
void handle_hello(msg_t * msg, module_info * mod) {
}

/**
 *	  Handle run app from client
 */
void handle_run_app(msg_t * msg, module_info * mod) {
    msg_t resp;

    if(msg == NULL || mod == NULL) {
        ALOGE("Invalid param, NULL pointer");
        return;
    }

    ALOGI("start handle handle_run_app:%s for module:[%s] with param: %s", msg->subcmd, msg->module, msg->msg);
    memcpy(&resp, msg, sizeof(msg_t));

    if(!strcmp(msg->subcmd, APP_MM_AUDIO_FTM)) {
        run_audio_ftm(msg, &resp);
    } else if(!strcmp(msg->subcmd, APP_FTMDAEMON)) {
        run_ftm_daemon(msg, &resp);
    } else {
        resp.result = FAILED;
    }

    ALOGI("command:%s handle finished for module:[%s], result=%s", msg->subcmd, msg->module, resp.result);

    //send reply
    send_msg(mod->socket_fd, &resp);
}

/**
 *	  Handle stop app from client
 */
void handle_stop_app(msg_t * msg, module_info * mod) {
    msg_t resp;

    if(msg == NULL || mod == NULL) {
        ALOGE("Invalid param, NULL pointer");
        return;
    }

    ALOGI("start handle handle_stop_app:%s for module:[%s]", msg->subcmd, msg->module);
    memcpy(&resp, msg, sizeof(msg_t));

    if(!strcmp(msg->subcmd, APP_MM_AUDIO_FTM) || !strcmp(msg->subcmd, APP_FTMDAEMON)) {
        stop_daemon(msg, &resp);
    } else {
        resp.result = FAILED;
    }

    ALOGI("command:%s handle finished for module:[%s], result=%s", msg->subcmd, msg->module, resp.result);

    //send reply
    send_msg(mod->socket_fd, &resp);
}

/**
 *	  Handle charger from client
 */
void handle_charger_ctrl(msg_t * msg, module_info * mod) {
    msg_t resp;

    if(msg == NULL || mod == NULL) {
        ALOGE("Invalid param, NULL pointer");
        return;
    }

    ALOGI("start handle command:%s for module:[%s]", msg->subcmd, msg->module);
    memcpy(&resp, msg, sizeof(msg_t));

    if(!strcmp(msg->subcmd, OPS_ON)) {
        charger_enable(true);
    } else {
        charger_enable(false);
    }
    resp.result = SUCCESS;

    ALOGI("command:%s handle finished for module:[%s], result=%s", msg->subcmd, msg->module, resp.result);
    //send reply
    send_msg(mod->socket_fd, &resp);
}

/**
 *	  Handle wifi from client
 */
void handle_wifi_ctrl(msg_t * msg, module_info * mod) {
    msg_t resp;

    if(msg == NULL || mod == NULL) {
        ALOGE("Invalid param, NULL pointer");
        return;
    }

    ALOGI("start handle command:%s for module:[%s]", msg->subcmd, msg->module);
    memcpy(&resp, msg, sizeof(msg_t));

    if(!strcmp(msg->subcmd, OPS_ON)) {
        config_wlan();
        set_con_mode();
    }
    resp.result = SUCCESS;

    ALOGI("command:%s handle finished for module:[%s], result=%s", msg->subcmd, msg->module, resp.result);
    //send reply
    send_msg(mod->socket_fd, &resp);
}

/**
 *	  Handle smb from client
 */
void handle_smb_ctrl(msg_t * msg, module_info * mod) {
    msg_t resp;

    if(msg == NULL || mod == NULL) {
        ALOGE("Invalid param, NULL pointer");
        return;
    }

    ALOGI("start handle command:%s for module:[%s]", msg->subcmd, msg->module);
    memcpy(&resp, msg, sizeof(msg_t));
    resp.result = SUCCESS;

    ALOGI("command:%s handle finished for module:[%s], result=%s", msg->subcmd, msg->module, resp.result);
    //send reply
    send_msg(mod->socket_fd, &resp);
}

/**
 *	  Handle shipmode from client
 */
void handle_shipmode_ctrl(msg_t * msg, module_info * mod) {
    msg_t resp;

    if(msg == NULL || mod == NULL) {
        ALOGE("Invalid param, NULL pointer");
        return;
    }

    ALOGI("start handle command:%s for module:[%s]", msg->subcmd, msg->module);
    memcpy(&resp, msg, sizeof(msg_t));

    if(!strcmp(msg->subcmd, OPS_ON)) {
        enter_ship_mode();
        resp.result = SUCCESS;
    }

    ALOGI("command:%s handle finished for module:[%s], result=%s", msg->subcmd, msg->module, resp.result);
    //send reply
    send_msg(mod->socket_fd, &resp);
}
