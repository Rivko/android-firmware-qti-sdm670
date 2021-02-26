/*
 * Copyright (c) 2013-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <sys/statfs.h>
#include "utils.h"
#include "config.h"
#include "input.h"
#include "mmi.h"
#include "module.h"
#include "controller.h"
#include "lang.h"
#ifdef ANDROID
#include "draw.h"
#endif

#include "mmi_cfg.h"
#include "interface.h"

using namespace android;

static const char *boot_mode_string[] = {
    "normal",
    "ffbm-00",
    "ffbm-01",
    "ffbm-02"
};

static const char *test_mode_string[] = {
    "none",
    "pcba",
    "ui",
};

/**global export var*/
char g_res_file[PATH_MAX] = { 0 };

/*
 * This is the layout pool,module could find
 * it's own layout via "layout" param
 * Key:    module name
 * Value:  layout
 */
unordered_map < string, layout * >g_layout_map;

/*
 * used to store all modules
 * key:module name
 */
unordered_map < string, module_info * >g_modules_map;
/*
 * used to store diag,debug module
 */
unordered_map < string, module_info * >g_controller_map;

/*
 * g_ordered_modules is the same thing as g_modules,
 * the different is sorted or not
 */
static list < module_info * >g_ordered_modules;

/*
 * g_nodup_modules: it is a duplicate hast_set from g_ordered_modules with group the module with
 * the same lib_name.
 * Key: lib_name
 * Value: module_info
 */
static unordered_map < string, module_info * >g_nodup_modules_map;

/*
 * used to store modules and it's sock id
 * key:module name
 */
unordered_map < string, int >g_sock_client_map;

sem_t g_data_print_sem;
sem_t g_result_sem;

/* used for clients that launched successfully;
 * Its content is part of g_modules
 */
static list < module_info * >g_clients;

/* home screen list; used for switch main screen by scroll
 */
static list < layout * >g_home_screens;

static pthread_t g_draw_tid;
static pthread_t g_accept_tid;
static pthread_t g_waiting_event_tid;
static pthread_t g_msg_handle_tid;
static sem_t g_sem_exit;
static sem_t g_sem_accept_ready;
static int g_max_fd = -1;

static msg_queue_t g_msg_queue;
static sem_t g_msg_sem;

static layout *g_cur_layout = NULL;
static pthread_mutex_t g_cur_layout_mutex;
static module_info *g_main_module = NULL;
static module_mode_t g_test_mode = TEST_MODE_NONE;
static int shipmode_status = -1;

bool is_main_screen() {
    return g_cur_layout == get_main_layout();
}

layout *get_layout(string layout_name) {
    /*Get the layout */
    return g_layout_map[layout_name];
}

layout *get_layout(module_info * mod) {
    if(mod == NULL) {
        MMI_ALOGE("Invalid parameter, Not main screen OR Null point\n");
        return NULL;
    }

    /*Get the layout */
    return g_layout_map[mod->config_list[KEY_LAYOUT]];
}

layout *get_main_layout() {
    /*Get the layout */
    return get_layout(get_main_module());
}

module_info *get_main_module() {
    return g_main_module;
}

void set_main_module(module_info * mod) {
    g_main_module = mod;
}

layout *acquire_cur_layout() {
    pthread_mutex_lock(&g_cur_layout_mutex);
    return g_cur_layout;
}

void release_cur_layout() {
    pthread_mutex_unlock(&g_cur_layout_mutex);
}

layout *switch_cur_layout_locked(layout * lay, module_info * mod) {

    acquire_cur_layout();
    if(NULL != lay) {
        MMI_ALOGI("switch layout to:'%s'\n", lay->get_layout_path());
    }
    if(lay != NULL) {
        g_cur_layout = lay;
        g_cur_layout->module = mod;
    }
    release_cur_layout();
    return g_cur_layout;
}

static bool is_in_home_screen(layout * lay) {
    list < layout * >::iterator iter;

    for(iter = g_home_screens.begin(); iter != g_home_screens.end(); iter++) {
        layout *laytarget = (layout *) (*iter);

        if(lay == laytarget)
            return true;
    }

    return false;
}

void register_home_screen(layout * lay) {

    if(!is_in_home_screen(lay))
        g_home_screens.push_back(lay);

    MMI_ALOGI("home screen size: %d", g_home_screens.size());
}

bool is_cur_home_screen() {
    return is_in_home_screen(g_cur_layout);
}

void switch_home_screen(bool is_right) {

    layout *target_lay = NULL;

    list < layout * >::iterator iter;

    for(iter = g_home_screens.begin(); iter != g_home_screens.end(); iter++) {
        layout *lay = (layout *) (*iter);

        if(lay == g_cur_layout) {
            target_lay = lay;

            if(is_right) {
                if((++iter) != g_home_screens.end()) {
                    target_lay = (layout *) (*iter);
                }
            } else {
                if(iter != g_home_screens.begin()) {
                    iter--;
                    target_lay = (layout *) (*iter);
                }
            }
            break;
        }
    }

    if(target_lay != NULL) {
        MMI_ALOGI("home target layout: %s", target_lay->get_layout_path());
        switch_cur_layout_locked(target_lay, get_main_module());
        invalidate();
    } else {
        MMI_ALOGW("Not find home screen layout");
    }
}

void set_boot_mode(boot_mode_type mode) {
    int fd;
    const char *dev = NULL;

    module_info *mod = get_main_module();

    if(mod == NULL || mod->config_list["misc_dev"].empty()) {
        MMI_ALOGE("No main module error");
        return;
    }

    dev = mod->config_list["misc_dev"].c_str();
    fd = open(dev, O_WRONLY);
    if(fd < 0) {
        MMI_ALOGE("misc:'%s' open fail, error=%s", MMI_STR(dev), strerror(errno));
    } else {
        if(write(fd, boot_mode_string[mode], strlen(boot_mode_string[mode])) != strlen(boot_mode_string[mode])) {
            MMI_ALOGE("misc:'%s' write fail, error=%s", MMI_STR(dev), strerror(errno));
        } else {
            MMI_ALOGI("Write '%s' to misc:'%s'", boot_mode_string[mode], MMI_STR(dev));
        }
        fsync(fd);
        close(fd);
    }

    /**Add more for compatible new policy*/
    if(mode == BOOT_MODE_NORMAL)
        property_set("sys.boot_mode", "normal");
    else
        property_set("sys.boot_mode", "ffbm");
}

/**Flush result file*/
void flush_result() {
    char tmp[SIZE_1K] = { 0 };
    FILE *fp = NULL;

    if(g_main_module == NULL) {
        MMI_ALOGE("No main layout\n");
        return;
    }

    /*Get the layout */
    layout *lay = get_main_layout();

    if(lay == NULL || lay->m_listview == NULL) {
        MMI_ALOGE("Not find main layout\n");
        return;
    }
    list < item_t * >*item_list = lay->m_listview->get_items();

    fp = fopen(g_res_file, "w+");
    if(fp == NULL) {
        MMI_ALOGE("file(%s) open fail, error=%s\n", g_res_file, strerror(errno));
        return;
    }

    MMI_ALOGI("start to update test result to '%s'", g_res_file);

    list < item_t * >::iterator iter;
    for(iter = item_list->begin(); iter != item_list->end(); iter++) {
        item_t *obj = (item_t *) (*iter);

        /**if the module already tested*/
        if(obj != NULL && obj->mod != NULL && obj->mod->result != ERR_UNKNOW) {
            snprintf(tmp, sizeof(tmp), "\n[%s]\n%s = %lu\n%s = %s\n%s = %3.4f\n", obj->mod->module, KEY_TIMESTAMP_WORDS,
                     obj->mod->last_time, KEY_RESULT_WORDS, obj->mod->result == SUCCESS ? KEY_PASS : KEY_FAIL,
                     KEY_TESTTIME_WORDS, obj->mod->duration);
            strlcat(tmp, obj->mod->data, sizeof(tmp));
            fwrite(tmp, sizeof(char), strlen(tmp), fp);
            MMI_ALOGI("update module:[%s] test result to '%s'", obj->mod->module, g_res_file);
        }
    }

    if(fp != NULL)
        fclose(fp);
}

/**Restore latest result */
static int restore_result(char *filepath) {
    char module[1024] = { 0, };
    char line[1024] = { 0, };
    char indicator = '=';
    module_info *cur_mod = NULL;

    if(filepath == NULL) {
        MMI_ALOGE("Invalid parameter");
        return -1;
    }

    FILE *file = fopen(filepath, "r");

    if(file == NULL) {
        MMI_ALOGE("file(%s) open failed, error=%s\n", filepath, strerror(errno));
        return -1;
    }

    MMI_ALOGI("Parse result file: '%s'\n", filepath);
    while(fgets(line, sizeof(line), file) != NULL) {
        char name[1024] = { 0, }, value[1024] = {
        0,};

        if(line[0] == '#')
            continue;

        if(line[0] == '[') {
            parse_module(line, module, sizeof(module));
            cur_mod = g_modules_map[(string) module];
            MMI_ALOGI("parse module: [%s]\n", module);
            if(cur_mod != NULL)
                memset(cur_mod->data, 0, sizeof(cur_mod->data));    //initial the data field
            continue;
        }

        if(module[0] != '\0' && cur_mod != NULL) {
            parse_value(line, indicator, name, sizeof(name), value, sizeof(value));
            char *pname = trim(name);
            char *pvalue = trim(value);

            if(*pname != '\0' && *pvalue != '\0') {

                if(!strcmp(pname, KEY_TIMESTAMP_WORDS)) {
                    cur_mod->last_time = string_to_ulong(pvalue);
                } else if(!strcmp(pname, KEY_RESULT_WORDS)) {
                    if(!strcmp(pvalue, KEY_PASS)) {
                        cur_mod->result = SUCCESS;
                        MMI_ALOGI("module:[%s] test pass\n", module);
                    } else {
                        cur_mod->result = FAILED;
                        MMI_ALOGI("module:[%s] test fail\n", module);
                    }
                } else if(!strcmp(pname, KEY_TESTTIME_WORDS)) {
                    cur_mod->start_time = cur_mod->last_time;
                } else {
                    strlcat(cur_mod->data, line, sizeof(cur_mod->data));
                }
            }
        }
    }

    fclose(file);
    return 0;
}

static module_info *get_module_by_name(char *module_name) {
    module_info *mod = NULL;

    mod = g_modules_map[(string) module_name];
    if(mod == NULL)
        mod = g_controller_map[(string) module_name];

    return mod;
}

/**set the module fd*/
static void module_set_fd(char *module, int fd) {
    module_info *mi = NULL;

    if(module == NULL || (mi = get_module_by_name(module)) == NULL) {
        MMI_ALOGE("%s", !module ? "Invalid parameter" : "Not find module");
        return;
    }

    string lib_name = mi->config_list[KEY_LIB_NAME];

    if(!lib_name.empty()) {
        /* set all module in the same lib_name to the same FD */
        unordered_map < string, module_info * >::iterator p;
        for(p = g_modules_map.begin(); p != g_modules_map.end(); p++) {
            module_info *mod = (module_info *) (p->second);

            if(mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
               && !mod->config_list[KEY_LIB_NAME].compare(lib_name)) {
                MMI_ALOGI("Set socket fd(%d) to module:[%s]\n", fd, mod->module);
                mod->socket_fd = fd;
                g_sock_client_map[lib_name] = fd;
            }
        }
    } else {
        /* if no lib_name just set the FD */
        MMI_ALOGI("Set socket fd(%d) to module:[%s]\n", fd, mi->module);
        mi->socket_fd = fd;
        g_sock_client_map[mi->module] = fd;
    }

    g_clients.push_back(mi);
    if(fd > g_max_fd)
        g_max_fd = fd;
}

static void *enter_shipmode(void *) {
    signal(SIGUSR1, signal_handler);
    int32_t color[] = { 0x007D7Dff, 0x7D0000ff };

    layout *lay = get_layout(LAYOUT_INDICATOR);

    if(lay == NULL) {
        MMI_ALOGE("Not find layout(%s) ", LAYOUT_INDICATOR);
        return NULL;
    }
    switch_cur_layout_locked(lay, get_main_module());
    button *btn_indicator = lay->find_button_by_name(KEY_INDICATOR);

    if(btn_indicator == NULL) {
        return NULL;
    }
    btn_indicator->set_text(get_string(KEY_UNPLUG_USB_NOTICE));
    invalidate();
    shipmode_status = 1;
    /**start to check usb cable*/
    int i = 0;

    while(is_usb_attached()) {
        MMI_ALOGE("usb cable atatched, please unplug it !");
        usleep(500 * 1000);
        btn_indicator->set_color(color[i++ % 2]);
        invalidate();
    }
    bool ret = enter_ship_mode();

    if(ret) {
        MMI_ALOGI("enter ship mode success, reboot!");
        btn_indicator->set_text(get_string(KEY_PASS));
    } else {
        MMI_ALOGI("enter ship mode fail!");
        btn_indicator->set_text(get_string(KEY_FAIL));
    }
    usleep(1000 * 1000);
    shipmode_status = 0;
    system("reboot -p");
    return NULL;
}

void enter_shipmode_thread() {
    pthread_t tid;

    if(shipmode_status == 1) {
        MMI_ALOGE("ongoing shipmode!");
        return;
    }
    int retval = pthread_create(&tid, NULL, enter_shipmode, NULL);

    if(retval < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return;
    }
}

static void *server_accepting_thread(void *) {
    signal(SIGUSR1, signal_handler);

    int client_fd;
    struct sockaddr_un addr;
    socklen_t addr_size = sizeof(addr);
    int ret;

    ret = mkdir(MMI_SOCKET_DIR, 0644);
    if(ret) {
        MMI_ALOGW("Failed to create '%s', reason=%s\n", MMI_SOCKET_DIR, strerror(errno));
    }

    int sockfd = create_socket(get_value(KEY_MMI_SOCKET));

    if(sockfd < 0) {
        MMI_ALOGE("create socket failed\n");
        return NULL;
    }

    listen(sockfd, 8);
    sem_post(&g_sem_accept_ready);

    msg_t msg;

    MMI_ALOGI("thread(server_accepting_thread) started\n");
    while(1) {
        client_fd = -1;
        memset(&msg, 0, sizeof(msg));
        if((client_fd = accept(sockfd, (struct sockaddr *) &addr, &addr_size)) < 0) {
            MMI_ALOGW("accept fail, error=%s\n", strerror(errno));
            continue;
        }

        TEMP_FAILURE_RETRY(recv(client_fd, &msg, sizeof(msg), MSG_WAITALL));
        MMI_ALOGI("connect success for module=[%s], cmd=%s, subcmd=%s, socdetFd=%d\n",
                  msg.module, MMI_CMD_NAME(msg.cmd), MMI_STR(msg.subcmd), client_fd);

        /*Set to module */
        module_set_fd(msg.module, client_fd);

    }
    return NULL;
}

static int msg_waiting() {

    fd_set fds;
    int retval;


    list < module_info * >fd_lists;
    list < module_info * >::iterator iter;
    msg_t *msg = NULL;

    struct timeval tv;

    /* Wait up to 3 seconds. */
    tv.tv_sec = 3;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    fd_lists.clear();
    if(!g_clients.empty()) {
        list < module_info * >::iterator iter;
        for(iter = g_clients.begin(); iter != g_clients.end(); iter++) {
            module_info *mod = (module_info *) (*iter);

            if(mod != NULL && mod->socket_fd > 0) {
                FD_SET(mod->socket_fd, &fds);
                fd_lists.push_back(mod);
            }
        }
    }

    if(fd_lists.empty()) {
        MMI_ALOGW("wait for client connection...\n");
        usleep(1000 * 1000);
        return -1;
    }
    retval = select(g_max_fd + 1, &fds, NULL, NULL, &tv);
    switch (retval) {
    case -1:
        MMI_ALOGE("select failed, error=%s\n", strerror(errno));
        break;
    case 0:
        MMI_ALOGI("select timeout: wait for receiving msg");
        break;
    default:
        int i = 0;

        for(iter = fd_lists.begin(); iter != fd_lists.end(); iter++) {
            module_info *mod = (module_info *) (*iter);

            if(mod == NULL)
                continue;

            int fd = mod->socket_fd;

            if(FD_ISSET(fd, &fds)) {
                msg = (msg_t *) zmalloc(sizeof(msg_t));
                if(msg == NULL) {
                    MMI_ALOGE("out of memory, abort the current request, error=%s\n", strerror(errno));
                    break;
                }

                int ret = TEMP_FAILURE_RETRY(recv(fd, msg, sizeof(msg_t), MSG_WAITALL));

                i++;
                if(ret <= 0) {
                    MMI_ALOGE("recv fail for module=[%s], error=%s\n", mod->module, strerror(errno));
                    close(mod->socket_fd);
                    mod->socket_fd = -1;
                } else {
                    MMI_ALOGI
                        ("mmi recv msg: moduld=[%s], cmd=%s, subcmd=%s, msg_id=%s, msg=%s, msg_size=%d, result=%s\n",
                         msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), MMI_PRI_TYPE(msg->msg_id),
                         MMI_STR(msg->msg), msg->length, MMI_RESULT(msg->result));
                    enqueue_msg(&g_msg_queue, msg);
                    sem_post(&g_msg_sem);
                }
            }
        }
        break;
    }
    return 0;
}

static void *msg_waiting_thread(void *) {
    signal(SIGUSR1, signal_handler);

    MMI_ALOGI("thread(msg_waiting_thread) started\n");
    while(1) {
        msg_waiting();
    }
    return NULL;
}

static void *msg_handle_thread(void *) {
    signal(SIGUSR1, signal_handler);

    msg_t *msg;
    module_info *mod;
    layout *curlay;

    MMI_ALOGI("thread(msg_handle_thread) started\n");
    while(1) {

        sem_wait(&g_msg_sem);
        dequeue_msg(&g_msg_queue, &msg);

        if(msg != NULL) {
            MMI_ALOGI("dequeue msg: moduld=[%s], cmd=%s, subcmd=%s, msg_id=%s, msg=%s, msg_size=%d, result=%s\n",
                      msg->module,
                      MMI_CMD_NAME(msg->cmd),
                      MMI_STR(msg->subcmd),
                      MMI_PRI_TYPE(msg->msg_id), MMI_STR(msg->msg), msg->length, MMI_RESULT(msg->result));

            mod = get_module_by_name(msg->module);
            if(mod == NULL) {
                MMI_ALOGW("Received invalid module, module=%s", "null");
                continue;
            }

            switch (msg->cmd) {
            case CMD_CTRL:
                handle_ctr_msg(msg, mod);
                break;
            case CMD_PRINT:
                handle_print(msg, mod);
                break;
            case CMD_QUERY:
                handle_query(msg, mod);
                break;
            case CMD_RUN:
                handle_run(msg, mod);
                break;
            default:
                MMI_ALOGW("msg(module=[%s], cmd=%s, subcmd=%s) no need to handle",
                          msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd));
                break;
            }
        }
    }
    return NULL;
}

static void exit_handler(int num) {
    static int flag = 0;
    int status;

    if(flag == 0)
        usleep(10 * 1000);

    flag = 1;
    int pid = waitpid(-1, &status, WNOHANG);

    if(WIFEXITED(status)) {
        MMI_ALOGI("The child(pid=%d) exit with code %d\n", pid, WEXITSTATUS(status));
     /**Reset pid */
        unordered_map < string, module_info * >::iterator p;
        for(p = g_nodup_modules_map.begin(); p != g_nodup_modules_map.end(); ++p) {
            module_info *mod = (module_info *) (p->second);

            if(mod->pid == pid) {
                mod->pid = -1;
                break;
            }
        }
    }
    flag = 0;
}

static void copy_config_file(void) {
    int ret = FAILED;
    char target_file[PATH_MAX];

    snprintf(target_file, sizeof(target_file), "%s%s", get_value(KEY_FTM_AP_DIR), MMI_PCBA_SYS_CONFIG);
    MMI_ALOGI("copy '%s' to '%s'", MMI_PCBA_CONFIG, target_file);
    ret = copy_file(MMI_PCBA_CONFIG, target_file);
    if(ret != SUCCESS)
        MMI_ALOGE("fail to copy '%s' to '%s'", MMI_PCBA_CONFIG, target_file);
}

static void build_main_ui() {

    module_info *mod = g_modules_map[MAIN_MODULE];

    MMI_ALOGI("start build main ui");
    if(mod == NULL) {
        MMI_ALOGE("Invalid parameter");
        return;
    }

    /**Initial main module*/
    set_main_module(mod);

    /**Turn on backlight*/
    write_file(mod->config_list["sys_backlight"].c_str(), "120");

    layout *lay = get_layout(mod);

    if(lay == NULL) {
        MMI_ALOGE("get layout fail");
        return;
    }

    if(lay->m_listview != NULL && g_ordered_modules.size() > 0) {
        lay->m_listview->set_item_per_page(atoi(mod->config_list["item_display_per_page"].c_str()));
        lay->m_listview->set_items(&g_ordered_modules);
    }

    switch_cur_layout_locked(lay, mod);

    /**register the main screen*/
    register_home_screen(lay);

    string home_screens_str = mod->config_list[HOME_SCREENS];

    if(!home_screens_str.empty()) {
        list < string > screens;
        string_split(home_screens_str, ";", screens);

        list < string >::iterator iter;
        for(iter = screens.begin(); iter != screens.end(); iter++) {
            string layout_name = (string) (*iter);
            layout *layhome = get_layout(layout_name);

            if(layhome != NULL) {
                MMI_ALOGI("register home layout for: %s", layout_name.c_str());
                register_home_screen(layhome);
            }
        }
    }

    update_main_status();
    invalidate();
}

static void init_nodup_map(list < module_info * >*list_modules) {

    if(!list_modules->empty()) {
        list < module_info * >::reverse_iterator iter;

        MMI_ALOGD("total module count: %d\n", list_modules->size());
        for(iter = list_modules->rbegin(); iter != list_modules->rend(); iter++) {
            module_info *mod = (module_info *) (*iter);

            if(mod->config_list[KEY_LIB_NAME] != "")
                g_nodup_modules_map[mod->config_list[KEY_LIB_NAME]] = mod;
        }
    }
}

/**
 *   Start logcat daemon for logging
 *
 */
static void launch_log() {
    char log_file[PATH_MAX] = { 0 };
    snprintf(log_file, sizeof(log_file), "%s%s", get_value(KEY_FTM_AP_DIR), MMI_LOG_FILENAME);
    delete_file(log_file, 8 * SIZE_1M);

    char *log_daemon = (char *) get_value(KEY_MMI_LOG_DAEMON);

    MMI_ALOGI("launch log process: '%s', log file path: '%s'\n", log_daemon, log_file);

    if(log_daemon == NULL) {
        MMI_ALOGE("start log daemon failed\n");
        return;
    }

    int pid = fork();

    if(pid == 0) {
        char *args[6] = { log_daemon, "-v", "time", "-f", (char *) log_file, NULL };

        int res = execv(log_daemon, args);

        if(res < 0) {
            MMI_ALOGE("'%s' exec failed and exit, error=%s\n", log_daemon, strerror(errno));
            exit(1);
        }
    } else if(pid < 0) {
        MMI_ALOGE("fork failed, error=%s\n", strerror(errno));
    } else if(pid > 0) {
    }
}

/**
 *   Start diag daemon for handling diag command from PC tool
 *
 */
static void launch_controller() {

    module_info *mod = g_controller_map[CLIENT_DIAG_NAME];
    char *diag_exe = (char *) get_value(KEY_MMI_DIAG);

    if(mod == NULL || diag_exe == NULL) {
        MMI_ALOGE("Invalid parameter\n");
        return;
    }

    MMI_ALOGI("fock '%s' \n", diag_exe);
    int pid = fork();

    if(pid == 0) {
        char *args[2] = { diag_exe, NULL };

        int res = execv(diag_exe, args);

        if(res < 0) {
            MMI_ALOGE("'%s' exec failed and exit, error=%s\n", diag_exe, strerror(errno));
            exit(1);
        }
    } else if(pid < 0) {
        MMI_ALOGE("fork failed, error=%s\n", strerror(errno));
    } else if(pid > 0) {
        mod->pid = pid;
        MMI_ALOGD("diag process pid=%d\n", pid);
    }
}

static void launch_clients() {

    /* Launch clients */
    unordered_map < string, module_info * >::iterator p;
    for(p = g_nodup_modules_map.begin(); p != g_nodup_modules_map.end(); ++p) {
        module_info *mod = (module_info *) (p->second);

        if(mod != NULL) {
            fork_launch_module(mod);
        }
    }
}

static int start_threads() {
    int retval = -1;

    MMI_ALOGI("start create threads");
#ifdef ANDROID
    retval = pthread_create(&g_draw_tid, NULL, draw_thread, NULL);
    if(retval < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create draw thread(thread id=%lu) for draw screen\n", g_draw_tid);

    retval = create_input_threads();
    if(retval < 0) {
        MMI_ALOGE("create input threads fail.");
        return -1;
    }
#endif
    retval = pthread_create(&g_accept_tid, NULL, server_accepting_thread, NULL);
    if(retval < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create accepting thread(thread id=%lu) for accepting client connection\n", g_accept_tid);

    retval = pthread_create(&g_waiting_event_tid, NULL, msg_waiting_thread, NULL);
    if(retval < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create waiting thread(thread id=%lu) for waiting msg\n", g_waiting_event_tid);

    retval = pthread_create(&g_msg_handle_tid, NULL, msg_handle_thread, NULL);
    if(retval < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create handle thread(thread id=%lu) for handle msg\n", g_msg_handle_tid);

    sem_wait(&g_sem_accept_ready);
    return 0;
}


static void init_lang() {
    MMI_ALOGI("mmi start to loading lang");
    module_info *mod = get_module_by_name(MAIN_MODULE);

    if(mod == NULL) {
        MMI_ALOGE("Not find module by %s", MAIN_MODULE);
        return;
    }

    /*Initial language */
    if(!mod->config_list[KEY_STR_LANGUAGE].empty()) {
        load_lang(mod->config_list[KEY_STR_LANGUAGE].c_str());
    }

    /**Init font size*/
    if(!mod->config_list[KEY_FONTSIZE].empty()) {
        set_font_size(mod->config_list[KEY_FONTSIZE].c_str());
    }
}

static void init_layout(unordered_map < string, layout * >&layout_map) {

    struct dirent *de;
    DIR *dir;
    char layout_path[PATH_MAX] = { 0 };

    layout_map.clear();
    /*Initial layout */
    dir = opendir(MMI_LAYOUT_BASE_DIR);
    if(dir != 0) {
        while((de = readdir(dir))) {
            if(!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
                continue;

            snprintf(layout_path, sizeof(layout_path), "%s%s", MMI_LAYOUT_BASE_DIR, de->d_name);
            layout *lay = new layout(layout_path);

            if(load_layout(layout_path, lay) == 0) {
                layout_map[de->d_name] = lay;
                MMI_ALOGI("Loading layout file(%s) success", de->d_name);
            } else {
                MMI_ALOGE("Loading layout file(%s) fail", de->d_name);
                if(lay != NULL)
                    delete lay;
            }
        }
        closedir(dir);
    } else {
        MMI_ALOGE("dir(%s) open fail, error=%s", MMI_LAYOUT_BASE_DIR, strerror(errno));
    }
}

static void init_controller() {

    g_controller_map.clear();

     /**Init Diag module*/
    module_info *mod = new module_info((char *) CLIENT_DIAG_NAME);

    g_controller_map[CLIENT_DIAG_NAME] = mod;

     /**Init debug module*/
    mod = new module_info((char *) CLIENT_DEBUG_NAME);
    g_controller_map[CLIENT_DEBUG_NAME] = mod;
}

static void init_module_mode(list < module_info * >*list_modules) {

    uint32_t i = -1;
    char boot_mode[PROPERTY_VALUE_MAX] = { 0 };

    /**Get mode from config file*/
    module_info *mod = g_modules_map[MAIN_MODULE];

    if(mod == NULL || mod->config_list[KEY_TESTMODE] == "") {
        MMI_ALOGE("%s", !mod ? "Not find module" : "Not find test mode");
        return;
    }
    const char *testmode = mod->config_list[KEY_TESTMODE].c_str();

    for(i = 0; i < sizeof(test_mode_string) / sizeof(char *); i++) {
        if(!strcmp(test_mode_string[i], testmode)) {
            g_test_mode = (module_mode_t) i;
            break;
        }
    }
#ifdef ANDROID
   /**Get mode from misc boot mode,overwrite the config file mode*/
    property_get("ro.bootmode", boot_mode, "normal");
    for(i = 0; i < sizeof(boot_mode_string) / sizeof(char *); i++) {
        if(!strcmp(boot_mode_string[i], boot_mode)) {
            g_test_mode = (module_mode_t) i;
            break;
        }
    }
#endif
    if(!list_modules->empty()) {
        list < module_info * >::iterator iter;
        for(iter = list_modules->begin(); iter != list_modules->end(); iter++) {
            module_info *mod = (module_info *) (*iter);

            if(mod != NULL)
                mod->mode = g_test_mode;
        }
    }
#ifdef ANDROID
    MMI_ALOGD("fastmmi testmode: %s", boot_mode_string[g_test_mode]);
#else
    MMI_ALOGD("fastmmi testmode: %s", test_mode_string[g_test_mode]);
#endif
}

static int init_config(const char *cfg) {

    int ret = -1;
    char cfg_tmp[128];

    /*Initialize configuration */
    ret = load_config(cfg, &g_modules_map, &g_ordered_modules);
    if(ret < 0) {
        MMI_ALOGE("Loading config(%s) fail", cfg);
        return -1;
    }

    /*Load lang */
    init_lang();

    /*Load more layout */
    init_layout(g_layout_map);

    /*Initial all module running mode */
    init_module_mode(&g_ordered_modules);

    /*Initial the duplicate module map */
    init_nodup_map(&g_ordered_modules);

    /*init the result file name */
    // Res filename should not contain the extension of the config file...
    strlcpy(cfg_tmp, cfg, sizeof(cfg_tmp));
    cfg_tmp[127] = NULL;
    char *p = strrchr(cfg_tmp, '/');

    if(p != NULL) {
        string res_file_name = string(p);
        size_t last_index = res_file_name.find_last_of(".");

        if(last_index != string::npos) {
            res_file_name = res_file_name.substr(1, last_index - 1);
        }
        res_file_name = string(get_value(KEY_FTM_AP_DIR)) + res_file_name + ".res";
        strlcpy(g_res_file, res_file_name.c_str(), sizeof(g_res_file));
    }
    MMI_ALOGI("test result file: '%s'\n", g_res_file);
    return 0;
}

static void clean_resource() {

    /**Stop threads before clean module info*/
    kill_thread(g_waiting_event_tid);
    /**Clean Layout*/
    unordered_map < string, layout * >::iterator p2;
    for(p2 = g_layout_map.begin(); p2 != g_layout_map.end(); ++p2) {
        layout *lay = (layout *) (p2->second);

        MMI_ALOGI("Clean layout: %s\n", ((string) p2->first).c_str());
        if(lay != NULL) {
            lay->clear_locked();
            delete lay;

            lay = NULL;
        }
    }

     /**Clean Module info*/
    while(g_ordered_modules.begin() != g_ordered_modules.end()) {
        module_info *tmp = *g_ordered_modules.begin();

        if(tmp != NULL) {
            MMI_ALOGI("Clean module:[%s]\n", tmp->module);
            delete tmp;

            g_ordered_modules.erase(g_ordered_modules.begin());
        }
    }

   /**Clean static */
    g_nodup_modules_map.clear();
    g_ordered_modules.clear();
    g_modules_map.clear();
    g_layout_map.clear();
    g_clients.clear();
}

/**Check if need skip pass module **/
static bool is_skip_pass() {
    bool ret = false;

   /**check global configuration*/
    module_info *mod = get_main_module();

    if(mod != NULL && !mod->config_list["skip_if_autorun_passed"].empty()) {
        if(!mod->config_list["skip_if_autorun_passed"].compare("1"))
            ret = true;
    }

    MMI_ALOGI("mmi be configured %s", ret ? "skip pass" : "not skip pass");
    return ret;
}

/**autostart config **/
static bool is_autostart() {
    bool ret = false;
    char buf[64] = { 0 };
    char auto_config[PATH_MAX];

    /**check file .autostart*/
    snprintf(auto_config, sizeof(auto_config), "%s%s", get_value(KEY_FTM_AP_DIR), AUTOSTART_CONFIG);
    MMI_ALOGI("auto test config file: %s", auto_config);
    if(!read_file(auto_config, buf, sizeof(buf)) && !strcmp(buf, KEY_ASCII_TRUE)) {
        ret = true;
    }

   /**check global configuration*/
    if(!ret) {
        module_info *mod = get_main_module();

        if(mod != NULL && !mod->config_list["autorun_enable"].empty()) {
            if(!mod->config_list["autorun_enable"].compare("1"))
                ret = true;
        }
    }
    MMI_ALOGI("mmi be configured to %s test mode", ret ? "automatically" : "not automatically");
    return ret;
}

/*Start all test
**if automation ==1, then only start support automation test cases
**if automation == 0, start all test cases.
**/
void start_all(bool automation) {
    list < module_info * >::iterator iter;
    for(iter = g_ordered_modules.begin(); iter != g_ordered_modules.end(); iter++) {
        module_info *mod = (module_info *) (*iter);

        if(mod != NULL && (!automation || (automation && !mod->config_list[KEY_AUTOMATION].compare("1")))) {
            module_exec_pcba(mod);
        }
    }
}

/*Start all autorun module test
**/
void start_autorun(bool skip_pass) {
    list < module_info * >::iterator iter;
    for(iter = g_ordered_modules.begin(); iter != g_ordered_modules.end(); iter++) {
        module_info *mod = (module_info *) (*iter);

        if(mod != NULL && (!mod->config_list[KEY_AUTOMATION].compare("1"))) {
            if(skip_pass && mod->result == SUCCESS)
                continue;

            MMI_ALOGI("mmi start to autorun module:%s", mod->module);
            module_exec_pcba(mod);
        }
    }
}


static bool check_fd_exist(int fd) {
    list < module_info * >::iterator iter;
    for(iter = g_clients.begin(); iter != g_clients.end(); iter++) {
        module_info *tmod = (module_info *) (*iter);

        if(tmod->socket_fd == fd)
            return true;
    }

    return false;
}


static bool pre_config() {

    /** Initial path config from xml file,this should be done in the begin*/
    parse_strings(MMI_PATH_CONFIG);

    return true;
}

static bool post_config() {
    MMI_ALOGI("start post config");
    /**Restore the latest result*/
    restore_result(g_res_file);
    return true;
}
static void reconfig_clients() {
    list < module_info * >::iterator iter;
    for(iter = g_ordered_modules.begin(); iter != g_ordered_modules.end(); iter++) {
        module_info *mod = (module_info *) (*iter);

        if(mod != NULL && !mod->config_list[KEY_LIB_NAME].empty()
           && g_sock_client_map[mod->config_list[KEY_LIB_NAME]] > 0) {
            mod->socket_fd = g_sock_client_map[mod->config_list[KEY_LIB_NAME]];
            if(!check_fd_exist(mod->socket_fd)) {
                g_clients.push_back(mod);
                MMI_ALOGI("socket fd=%d for module:[%s]", mod->socket_fd, mod->module);
            } else {
                MMI_ALOGW("socket fd:%d not exist", mod->socket_fd);
            }
        }
    }

    g_clients.push_back(g_controller_map[CLIENT_DIAG_NAME]);
    MMI_ALOGI("socket fd=%d for module:[%s]",
              g_controller_map[CLIENT_DIAG_NAME]->socket_fd, g_controller_map[CLIENT_DIAG_NAME]->module);
}

int reconfig(const char *cfg) {
    int ret = -1;

    if(cfg == NULL) {
        MMI_ALOGE("Invalid parameter");
        return -1;
    }

    clean_resource();

    /*Initialize configuration */
    ret = init_config(cfg);
    if(ret < 0) {
        MMI_ALOGE("init config failed");
        return -1;
    }

    if(!post_config()) {
        MMI_ALOGE("post config failed");
        return -1;
    }

    /*Initial the MMI screen */
    build_main_ui();

    /*Reconfig client */
    reconfig_clients();

    layout *lay = get_main_layout();

    if(lay == NULL || lay->m_listview == NULL) {
        MMI_ALOGE("%s", !lay ? "Not find layout" : "Not find listview in layout");
        return -1;
    }

    ret = pthread_create(&g_waiting_event_tid, NULL, msg_waiting_thread, NULL);
    if(ret < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    MMI_ALOGD("create waiting thread(thread id=%lu) for waiting msg\n", g_waiting_event_tid);

    return lay->m_listview->get_items()->size();
}

int display_ffbm_string(void) {
    char layout_path[PATH_MAX] = { 0 };
    layout *lay = new layout(layout_path);
    int ret = FAILED;

    MMI_ALOGI("FFBM has been disabled, show FFBM mode only!!!!");
    /**logging to dmesg for debuging boot time*/
    MMI_KLOGE("FFBM has been disabled, show FFBM mode only!!!!");

    sem_init(&g_sem_exit, 0, 0);

    /**Init draw */
    if(!init_draw()) {
        MMI_ALOGE("init draw env failed!");
        return FAILED;
    }

    /*Load strings */
    load_lang("en");

    /*Load main UI layout file */
    snprintf(layout_path, sizeof(layout_path), "%s%s", MMI_LAYOUT_BASE_DIR, "main.xml");
    if(load_layout(layout_path, lay) == 0) {
        MMI_ALOGI("Loading main layout file success");
    } else {
        MMI_ALOGE("Loading main layout file fail");
        if(lay != NULL)
            delete lay;
        return FAILED;
    }

    /*Start draw thread to draw main UI */
    ret = pthread_create(&g_draw_tid, NULL, draw_thread, NULL);
    if(ret < 0) {
        MMI_ALOGE("create thread fail, error=%s", strerror(errno));
        return FAILED;
    }
    MMI_ALOGD("create draw thread(thread id=%lu) for draw screen\n", g_draw_tid);

    /**Turn on backlight*/
    write_file("/sys/class/leds/lcd-backlight/brightness", "120");

    /*Set current layout to main UI layout */
    g_cur_layout = lay;

    /*Refresh the screen to display UI */
    invalidate();

    return SUCCESS;
}

static void light_green_LED(void) {
    while(1) {
        write_file("/sys/class/leds/green/brightness", "255");
        usleep(500 * 1000);
        write_file("/sys/class/leds/green/brightness", "0");
        usleep(500 * 1000);
    }
}

int main(int argc, char **argv) {

    signal(SIGCHLD, exit_handler);
    int ret = -1;

    /** Disable FFBM function
     * Only display FFBM mode string on main UI
     */
    MMI_PROC_TYPE(MMI_PROC_TYPE_MMI);

    MMI_ALOGI("mmi starting...., enjoy!!!!");
    /**logging to dmesg for debuging boot time*/
    MMI_KLOGE("mmi starting...., enjoy!!!!");


    if(display_ffbm_string() != SUCCESS){
        MMI_ALOGE("Fail to display FFBM string on main ui");
    }
    if(!pre_config()) {
        MMI_ALOGE("config failed");
        return -1;
    }

    init_controller();
    //launch_controller();

    sem_wait(&g_sem_exit);
 //   write_file(WAKE_UNLOCK, "mmi");
  out:sem_close(&g_sem_exit);
    return 0;
}
