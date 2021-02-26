/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "agent.h"

static mmi_module_t *g_module = NULL;
static char *g_module_name = NULL;
static int g_sock = -1;
static msg_queue_t g_msg_queue;
static sem_t g_msg_sem;


/**
 * Load the file defined by the variant and if successful
 * return the dlopen handle and the mmi.
 * @return 0 = success, !0 = failure.
 */
static int load_module(const char *path, mmi_module_t ** pModule) {
    int ret;
    void *handle;
    mmi_module_t *module;
    char const *errstr;
    const char *sym = MMI_MODULE_INFO_SYM_AS_STR;

    if(path == NULL) {
        MMI_ALOGE("parameter error, invalid lib path");
        return -1;
    }

    MMI_ALOGI("start to loading lib: '%s'", path);
    if(access(path, R_OK)) {
        MMI_ALOGE("lib(%s) not found, error=%s\n", path, strerror(errno));
        return -1;
    }

    /**
     * load the symbols resolving undefined symbols before
     * dlopen returns. Since RTLD_GLOBAL is not or'd in with
     * RTLD_NOW the external symbols will not be global
     */
    handle = dlopen(path, RTLD_NOW);
    if(handle == NULL) {
        errstr = dlerror();
        MMI_ALOGE("lib(%s) open fail, error=%s\n", path, errstr ? errstr : "unknown");
        ret = -EINVAL;
        goto done;
    }

    /** Get the address of the struct mmi_module_info. */
    module = (mmi_module_t *) dlsym(handle, sym);
    if(module == NULL) {
        MMI_ALOGE("symbol(%s) loading fail, error=%s", sym, (errstr = dlerror())? errstr : "unknown");
        ret = -EINVAL;
        goto done;
    }

    module->module_handle = handle;

    /** success */
    ret = 0;

  done:
    if(ret != 0) {
        module = NULL;
        if(handle != NULL) {
            MMI_ALOGE("lib(%s) loading fail", path);
            dlclose(handle);
            handle = NULL;
        }
    } else {
    }

    *pModule = module;
    if(module != NULL) {
        MMI_ALOGD("'%s' info: majorVer=%d, minorVer=%d, module=[%s], pid=%d, thread id=%lu",
                  path,
                  module->version_major, module->version_minor, module->name, module->run_pid, module->module_handle);
    }

    return ret;
}

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
int print_msg(const char *module_name, const char *subcmd, const char *str, uint32_t size, print_type_t type) {

    msg_t msg;

    if(str == NULL || strlen(str) == 0) {
        MMI_ALOGE("No message need to send, current sock=%d", g_sock);
        return -1;
    }
    memset(&msg, 0, sizeof(msg));

    if(module_name == NULL || strlen(module_name) == 0)
        strlcpy(msg.module, g_module_name, sizeof(msg.module));
    else
        strlcpy(msg.module, module_name, sizeof(msg.module));

    if(subcmd != NULL)
        strlcpy(msg.subcmd, subcmd, sizeof(msg.subcmd));

    strlcpy(msg.msg, str, sizeof(msg.msg));
    msg.length = size;
    msg.msg_id = type;          //Use the field
    msg.cmd = CMD_PRINT;

    MMI_ALOGI("send to mmi msg: moduld=[%s], cmd=%s, subcmd=%s, msg_id=%s, msg=%s, result=%s\n",
              msg.module,
              MMI_CMD_NAME(msg.cmd),
              MMI_STR(msg.subcmd), MMI_PRI_TYPE(msg.msg_id), MMI_STR(msg.msg), MMI_RESULT(msg.result));
    send_msg(g_sock, &msg);
    return 0;
}

/**
 * cmd handler
 * @return 0 = success, !0 = failure.
 */
static void *msg_process_thread(void *req) {

    int ret = 0;

    if(req == NULL)
        return NULL;

    msg_t *preq = (msg_t *) req;
    msg_t resp;

    /**Initial resp header*/
    memcpy(&resp, preq, sizeof(msg_t));
    MMI_ALOGI("process msg: moduld=[%s], cmd=%s, subcmd=%s, msg=%s, msg_size=%d",
              preq->module, MMI_CMD_NAME(preq->cmd), MMI_STR(preq->subcmd), MMI_STR(preq->msg), preq->length);
    switch (preq->cmd) {
    case CMD_INIT:
        ret = handle_init(g_module, preq, &resp);
        break;
    case CMD_DEINIT:
        ret = handle_deinit(g_module, preq, &resp);
        break;

    case CMD_QUERY:
        ret = handle_query(g_module, preq, &resp);
        break;

    case CMD_RUN:
        ret = handle_run(g_module, preq, &resp);
        break;

    case CMD_STOP:
        ret = handle_stop(g_module, preq, &resp);
        break;

    default:
        ret = -1;
        resp.result = FAILED;
        MMI_ALOGE("Received invalid command: %d", preq->cmd);
        break;
    }

    send_msg(g_sock, &resp);

     /**Free the req momery*/
    if(preq != NULL)
        free(preq);

    return NULL;
}

/**
 * Handle message thread to read message from pending message queue
 * @Never return.
 */
static void *msg_handle_thread(void *) {

    list < msg_t * >::iterator iter;
    pthread_t ptid;
    int ret = -1;
    msg_t *msg = NULL;

    MMI_ALOGI("thread(msg_handle_thread) start for module:[%s]", g_module_name);
    while(1) {

        sem_wait(&g_msg_sem);

        dequeue_msg(&g_msg_queue, &msg);
        if(msg != NULL) {
            MMI_ALOGI("dequeue msg: moduld=[%s], cmd=%s, subcmd=%s, msg=%s, msg_size=%d\n",
                      msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), MMI_STR(msg->msg), msg->length);

             /**Start one single thread for each request */
            ret = pthread_create(&ptid, NULL, msg_process_thread, msg);
            if(ret < 0) {
                MMI_ALOGE("thread create fail for handle msg(moduld=[%s], cmd=%s, subcmd=%s), error=%s\n",
                          msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), strerror(errno));
                continue;
            }
        }
    }

    return NULL;
}

/**
 * Receive thread handle function
 * @Never return.
 */
static void *msg_waiting_thread(void *s) {

    int recv_size = -1, sock = -1;
    msg_t *msg = NULL;

    if(s == NULL) {
        MMI_ALOGE("No sock id received");
        return NULL;
    }

    sock = *(int *) s;

    MMI_ALOGI("thread(msg_waiting_thread) start for module:[%s]", g_module_name);
    while(1) {

        /** Receive a reply from the MMI server */
        msg = (msg_t *) zmalloc(sizeof(msg_t));
        if(msg == NULL) {
            MMI_ALOGE("out of memory, abort the current request, error=%s\n", strerror(errno));
            break;
        }

        if((recv_size = recv(sock, msg, sizeof(msg_t), MSG_WAITALL)) < 0) {
            MMI_ALOGE("recv msg fail, error=%s\n", strerror(errno));
            break;
        }

        MMI_ALOGI("from mmi recv msg: moduld=[%s], cmd=%s, subcmd=%s, msg=%s, msg_size=%d\n",
                  msg->module, MMI_CMD_NAME(msg->cmd), MMI_STR(msg->subcmd), MMI_STR(msg->msg), msg->length);
        /** Enquenue the request which handled in a single thread*/
        enqueue_msg(&g_msg_queue, msg);

        /**Notify the handle thread*/
        sem_post(&g_msg_sem);
    }

    return NULL;
}



static void usage() {
    const char *usage = "mmi_agent usage:\n\
                                    mmi_agent -m [module path] \n\
                                    mmi_agent -p [module parameters] \n";

    MMI_ALOGI("%s\n", usage);
}

static void sigterm_handler(int sig) {
    /**Quit*/
    if(g_module != NULL) {
        g_module->methods->module_stop(g_module);
        g_module->methods->module_deinit(g_module);
    }
}

int main(int argc, char *argv[]) {

    int ch = 0, ret = 0;
    const char *lib_path = NULL;
    char pro_name[NAME_MAX] = { 0 };

    unordered_map < string, string > module_params;
    pthread_t pid_wait, pid_handle;

    MMI_PROC_TYPE(MMI_PROC_TYPE_MMI_AGENT);

    signal(SIGTERM, sigterm_handler);

    sem_init(&g_msg_sem, 0, 0);

    /** Initial path config from xml file*/
    parse_strings(MMI_PATH_CONFIG);
    while((ch = getopt(argc, argv, "m:p:")) != EOF) {
        switch (ch) {
        case 'm':
            g_module_name = strdup(optarg);
            if(g_module_name == NULL) {
                MMI_ALOGE("out of memory and exit, error=%s", strerror(errno));
                exit(-1);
            }
            break;
        case 'p':
            parse_parameter(optarg, module_params);
            break;
        default:
            MMI_ALOGE("args error and exit, usage: -m [module path] -p [module parameters]");
            usage();
            exit(-1);
            break;
        }
    }

    if(g_module_name == NULL) {
        MMI_ALOGE("args error, Please specify module");
        goto out;
    }

    MMI_ALOGI("start mmi_agent for module:[%s]", g_module_name);

    lib_path = module_params["lib_path"].c_str();
    if(lib_path == NULL || strlen(lib_path) == 0) {
        MMI_ALOGE("args error, Please specify lib path");
        goto out;
    }

    ret = load_module(lib_path, &g_module);
    if(ret < 0) {
        MMI_ALOGE("lib(%s) loading fail", lib_path);
        goto out;
    }

    snprintf(pro_name, sizeof(pro_name), "mmi_%s", g_module->name);
    ret = prctl(PR_SET_NAME, (unsigned long)pro_name, 0, 0, 0);
    if(ret)
        ALOGE("PR_SET_NAME failed!");

    /** Initial Callback function*/
    g_module->cb_print = print_msg;

    MMI_ALOGI("start connect to server for module:[%s]", g_module_name);
    /** Connect to MMI server via socket*/
    g_sock = connect_server(get_value(KEY_MMI_SOCKET));
    if(g_sock < 0) {
        MMI_ALOGE("connect server(%s) fail.", get_value(KEY_MMI_SOCKET));
        goto out;
    }

     /**Ready, say hello to server*/
    ret = say_hello(g_sock, g_module_name);
    if(ret) {
        MMI_ALOGE("send 'Say Hello' Fail");
        goto out;
    }

    /** Call initialize function when module start*/
    ret = g_module->methods->module_init(g_module, module_params);
    if(ret) {
        MMI_ALOGE("module init fail for module:[%s]", g_module_name);
        goto out;
    }

    ret = pthread_create(&pid_wait, NULL, msg_waiting_thread, &g_sock);
    if(ret < 0) {
        MMI_ALOGE("Can't create msg waiting thread, error=%s\n", strerror(errno));
        goto out;
    }
    MMI_ALOGD("create msg waiting thread(thread id=%lu) for module:[%s]\n", pid_wait, g_module_name);

    ret = pthread_create(&pid_handle, NULL, msg_handle_thread, NULL);
    if(ret < 0) {
        MMI_ALOGE("Can't create msg handle thread, error=%s\n", strerror(errno));
        goto out;
    }
    MMI_ALOGD("create msg handle thread(thread id=%lu) for module:[%s]\n", pid_handle, g_module_name);


    pthread_join(pid_wait, NULL);
    pthread_join(pid_handle, NULL);

  out:
    usleep(rand() % 10000 * 1000);
     /**Something wrong happen, sleep random time let the father process
      could handle SIGCHLD for all childrens. */
    if(g_module_name != NULL) {
        MMI_ALOGE("mmi_agent process start fail for module:[%s]", g_module_name);
        free(g_module_name);
    }
    sem_close(&g_msg_sem);
    exit(1);
}
