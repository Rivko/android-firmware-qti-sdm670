/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "mmid.h"

using namespace std;

static pthread_t g_accept_tid;
static pthread_t g_waiting_event_tid;
static pthread_t g_msg_handle_tid;

static msg_queue_t g_msg_queue;
static sem_t g_msg_sem;
static sem_t g_sem_accept_ready;
static int g_max_fd = -1;

/*
 * used to store modules and it's sock id
 * key:module name
 */
unordered_map < string, int >g_sock_client_map;

/* used for clients that launched successfully;
 * Its content is part of g_modules
 */
static list < module_info * >g_supported_clients;

/*
 * used to store modules and it's sock id
 * key:module name
 */
unordered_map < string, app_status_t * >g_apps_map;

app_status_t *get_app_status(string name) {
    app_status_t *app = g_apps_map[name];

    if(app == NULL) {
        //new
        app = (app_status_t *) malloc(sizeof(app_status_t));
        if(app == NULL) {
            ALOGE(" out of memory!");
        } else {
            app->activated = false;
            app->proc_id = -1;
            strlcpy(app->app_name, name.c_str(), sizeof(app->app_name));
            g_apps_map[name] = app;
        }

    }
    return app;
}

static const char *supported_app_list[] = {
    APP_MM_AUDIO_FTM,
    APP_FTMDAEMON
};

bool is_app_supported(char *name) {

    for(int i = 0; i < sizeof(supported_app_list) / sizeof(char *); i++) {
        if(!strcmp(supported_app_list[i], name)) {
            return true;
        }
    }
    return false;
}

static module_info *get_module_by_name(char *module_name) {
    module_info *mod = NULL;

    list < module_info * >::iterator iter;
    for(iter = g_supported_clients.begin(); iter != g_supported_clients.end(); iter++) {
        mod = (module_info *) (*iter);

        if(mod != NULL && !strcmp(mod->module, module_name)) {
            return mod;
        }
    }

    return NULL;
}

/**set the module fd*/
static void module_set_fd(char *module, int fd) {
    module_info *mi = NULL;

    if(module == NULL || (mi = get_module_by_name(module)) == NULL) {
        ALOGE("%s", !module ? "Invalid parameter" : "Not find module");
        return;
    }
    /* if no lib_name just set the FD */
    ALOGI("Client connect success: set socket fd(%d) to module:[%s]\n", fd, mi->module);
    mi->socket_fd = fd;
    g_sock_client_map[mi->module] = fd;

    if(fd > g_max_fd)
        g_max_fd = fd;
}

static void *server_accepting_thread(void *) {
    signal(SIGUSR1, signal_handler);

    int client_fd;
    struct sockaddr_un addr;
    socklen_t addr_size = sizeof(addr);
    int ret;

    int sockfd = create_socket(MMI_SOCKET);

    if(sockfd < 0) {
        ALOGE("create socket failed\n");
        return NULL;
    }

    listen(sockfd, 8);
    sem_post(&g_sem_accept_ready);

    msg_t msg;

    ALOGI("thread(server_accepting_thread) started\n");
    while(1) {
        client_fd = -1;
        memset(&msg, 0, sizeof(msg));
        if((client_fd = accept(sockfd, (struct sockaddr *) &addr, &addr_size))
           < 0) {
            ALOGE("accept fail, error=%s\n", strerror(errno));
            continue;
        }

        TEMP_FAILURE_RETRY(recv(client_fd, &msg, sizeof(msg), MSG_WAITALL));
        ALOGI("connect success for module=[%s], cmd=%d, subcmd=%s, socdetFd=%d\n",
              msg.module, msg.cmd, msg.subcmd, client_fd);

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
    char bootmode[256] = { 0 };
    property_get("ro.bootmode", bootmode, NULL);

    FD_ZERO(&fds);
    fd_lists.clear();
    if(!g_supported_clients.empty()) {
        list < module_info * >::iterator iter;
        for(iter = g_supported_clients.begin(); iter != g_supported_clients.end(); iter++) {
            module_info *mod = (module_info *) (*iter);

            if(mod != NULL && mod->socket_fd > 0) {
                FD_SET(mod->socket_fd, &fds);
                fd_lists.push_back(mod);
            }
        }
    }

    if(fd_lists.empty()) {
        ALOGI("wait for client connection...\n");
        usleep(1000 * 1000);
        return -1;
    }
    if(!strncmp(bootmode, "ffbm-02", 7)){
        retval = select(g_max_fd + 1, &fds, NULL, NULL, &tv);
    }else{
        retval = select(g_max_fd + 1, &fds, NULL, NULL, NULL);
    }
    switch (retval) {
    case -1:
        ALOGE("select failed, error=%s\n", strerror(errno));
        break;
    case 0:
        ALOGI("select timeout: wait for receiving msg");
        break;
    default:
        int i = 0;

        for(iter = fd_lists.begin(); iter != fd_lists.end(); iter++) {
            module_info *mod = (module_info *) (*iter);

            if(mod == NULL)
                continue;

            int fd = mod->socket_fd;

            if(FD_ISSET(fd, &fds)) {
                msg = (msg_t *) malloc(sizeof(msg_t));
                if(msg == NULL) {
                    ALOGE("out of memory, abort the current request, error=%s\n", strerror(errno));
                    break;
                }

                int ret = TEMP_FAILURE_RETRY(recv(fd, msg, sizeof(msg_t), MSG_WAITALL));

                i++;
                if(ret <= 0) {
                    ALOGE("recv fail for module=[%s], error=%s\n", mod->module, strerror(errno));
                    close(mod->socket_fd);
                    mod->socket_fd = -1;
                } else {
                    ALOGI("mmi recv msg: moduld=[%s], cmd=%d, subcmd=%s, msg_id=%d, msg=%s, msg_size=%d, result=%d\n",
                          msg->module, msg->cmd, msg->subcmd, msg->msg_id, msg->msg, msg->length, msg->result);
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

    ALOGI("thread(msg_waiting_thread) started\n");
    while(1) {
        msg_waiting();
    }
    return NULL;
}
/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
static void *msg_process_in_thread(void *req) {

    int ret = 0;

    if(req == NULL)
        return NULL;

    msg_t *preq = (msg_t *) req;

    /**Initial resp header*/
    module_info *mod = get_module_by_name(preq->module);

    if(mod != NULL) {
        ALOGI(" msg(module=[%s], cmd=%d, subcmd=%s)  start to handle", preq->module, preq->cmd,
              preq->subcmd);

        switch (preq->cmd) {
        case CMD_RUN_APP:
            handle_run_app(preq, mod);
            break;
        case CMD_STOP_APP:
            handle_stop_app(preq, mod);
            break;

        default:
            ALOGE("msg(module=[%s], cmd=%d, subcmd=%s) no need to handle", preq->module, preq->cmd, preq->subcmd);
            break;
        }
    }

     /**Free the req momery*/
    if(preq != NULL)
        free(preq);

    return NULL;
}

static void *msg_handle_thread(void *) {
    signal(SIGUSR1, signal_handler);

    msg_t *msg;
    module_info *mod;
    bool is_free_in_thread = false;


    ALOGI("thread(msg_handle_thread) started\n");
    while(1) {

        sem_wait(&g_msg_sem);
        dequeue_msg(&g_msg_queue, &msg);

        if(msg != NULL) {
            ALOGI("mmi recv msg: moduld=[%s], cmd=%d, subcmd=%s, msg_id=%d, msg=%s, msg_size=%d, result=%d\n",
                  msg->module, msg->cmd, msg->subcmd, msg->msg_id, msg->msg, msg->length, msg->result);

            mod = get_module_by_name(msg->module);
            if(mod == NULL) {
                ALOGE("Received invalid module, module=%s", "null");
                continue;
            }

            switch (msg->cmd) {
            case CMD_HELLO:
                handle_hello(msg, mod);
                break;
            case CMD_RUN_APP:
            case CMD_STOP_APP:
                /**Start one single thread for the request */
                pthread_t ptid;
                pthread_create(&ptid, NULL, msg_process_in_thread, msg);
                is_free_in_thread = true;
                break;
            case CMD_CHARGER_CTRL:
                handle_charger_ctrl(msg, mod);
                break;
            case CMD_SHIPMODE_CTRL:
                handle_shipmode_ctrl(msg, mod);
                break;
            case CMD_SMB_CTRL:
                handle_smb_ctrl(msg, mod);
                break;
            case CMD_WIFI_CTRL:
                handle_wifi_ctrl(msg, mod);
                break;

            default:
                ALOGE("msg(module=[%s], cmd=%d, subcmd=%s) no need to handle", msg->module, msg->cmd, msg->subcmd);
                break;
            }

            //free the memory
            if(!is_free_in_thread)
                free(msg);
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
        ALOGI("The child(pid=%d) exit with code %d\n", pid, WEXITSTATUS(status));

    }
    flag = 0;
}

static int createSocketThreads() {
    int retval = -1;

    ALOGI("start create threads");

    retval = pthread_create(&g_accept_tid, NULL, server_accepting_thread, NULL);
    if(retval < 0) {
        ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    ALOGD("create accepting thread(thread id=%lu) for accepting client connection\n", g_accept_tid);

    retval = pthread_create(&g_waiting_event_tid, NULL, msg_waiting_thread, NULL);
    if(retval < 0) {
        ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    ALOGD("create waiting thread(thread id=%lu) for waiting msg\n", g_waiting_event_tid);

    retval = pthread_create(&g_msg_handle_tid, NULL, msg_handle_thread, NULL);
    if(retval < 0) {
        ALOGE("create thread fail, error=%s", strerror(errno));
        return -1;
    }
    ALOGD("create handle thread(thread id=%lu) for handle msg\n", g_msg_handle_tid);

    sem_wait(&g_sem_accept_ready);
    return 0;
}

static bool init() {
    /**Common init*/
    sem_init(&g_msg_sem, 0, 0);
    sem_init(&g_sem_accept_ready, 0, 0);
    return true;
}

static void init_supported_clients() {

    g_supported_clients.clear();

    /**Init Diag module*/
    module_info *mod = new module_info((char *) CLIENT_QMMI_NAME);

    g_supported_clients.push_back(mod);
}

int main(int argc, char **argv) {
    signal(SIGCHLD, exit_handler);
    int ret = -1;

    /** Disable FFBM function
   * Only display FFBM mode string on main UI
   */
    ALOGI("mmi daemon start, enjoy!!!!");
    init();
    init_supported_clients();

    ALOGI("mmi daemon create threads!");
    createSocketThreads();

    pthread_join(g_accept_tid, NULL);
    pthread_join(g_waiting_event_tid, NULL);
    pthread_join(g_msg_handle_tid, NULL);

  out:
    ALOGI("finished exit");
    exit(1);
}
