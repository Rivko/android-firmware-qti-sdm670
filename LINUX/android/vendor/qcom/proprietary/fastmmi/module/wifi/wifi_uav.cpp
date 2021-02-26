/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "mmi_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include <sys/types.h>

#define NAME_SIZE 20
#define BUF_SIZE 50
/**
 * Defined supported command list here.And put into extra_cmd_list,
 * so server could get it.
 */
#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define MODULE_NAME "WiFi"
#define MODULE_AUTHOR "Qualcomm Technologies, Inc."
#define RUN_PID -1
#define DNSMASQ_D "dnsmasq"
#define HOSTAPD_D "hostapd"

static pthread_mutex_t g_mutex;

static const char *extra_cmd_list[] = {
};

struct wifi_info {
    char ssid[64];
    char passwd[64];
    char ip[64];
    char mac[64];
};

static struct wifi_info wifi_result;

enum test_case {
    CASE_WIFIDAEMON,
    CASE_WIFISSID,
    CASE_WIFIPASSWD,
    CASE_WIFIWLANDRV,
    CASE_WIFIGETIP,
    CASE_WIFIMAC,
};

struct case_run {
    enum test_case t;
      int32_t(*run_testp) (const mmi_module_t * module, unordered_map < string, string > &params);
};

struct proc_list {
    pid_t pid;
    pid_t ppid;
    char name[NAME_SIZE];
    char status;
    struct proc_list *next;
};

static struct proc_list *inport_list(char *);
static int show_info(struct proc_list *, char *);
static int read_info(char *, struct proc_list *);
static void free_list(struct proc_list *);
static int is_num(char *);

static int32_t run_daemon(const mmi_module_t * module, unordered_map < string, string > &params);
static int32_t run_ssid(const mmi_module_t * module, unordered_map < string, string > &params);
static int32_t run_passwd(const mmi_module_t * module, unordered_map < string, string > &params);
static int32_t run_wlandrv(const mmi_module_t * module, unordered_map < string, string > &params);
static int32_t run_getip(const mmi_module_t * module, unordered_map < string, string > &params);
static int32_t run_wifimac(const mmi_module_t * module, unordered_map < string, string > &params);

static struct case_run case_func[] = {
    {CASE_WIFIDAEMON, run_daemon},
    {CASE_WIFISSID, run_ssid},
    {CASE_WIFIPASSWD, run_passwd},
    {CASE_WIFIWLANDRV, run_wlandrv},
    {CASE_WIFIGETIP, run_getip},
    {CASE_WIFIMAC, run_wifimac},
};

/*
 * just for ps -aux
 * author :
 */

static int Isinproc(char *input) {
    struct proc_list *head = inport_list("/proc");
    int ret = FAILED;

    if(head == NULL) {
        printf("inport list failed !\n");
        return ret;
    }
    ret = show_info(head, input);
    free_list(head);
    return ret;
}

static struct proc_list *inport_list(char *dir) {
    DIR *dsptr;
    struct dirent *dptr;
    struct proc_list *head = NULL, *ptr = NULL;

    if((dsptr = opendir(dir)) == NULL) {
        printf("Error open the dir %s\n", dir);
        return NULL;
    }

    /*read all the dir and file under directory "dir" */
    while((dptr = readdir(dsptr)) != NULL) {
        if(is_num(dptr->d_name) == 0) {
            struct proc_list *item = (struct proc_list *) malloc(sizeof(struct proc_list));

            if(item == NULL) {
                printf("memory allocate error !\n");
                free_list(head);
                return NULL;
            }
            memset(item, 0, sizeof(struct proc_list));
            if(head == NULL) {
                head = ptr = item;
            } else {
                ptr->next = item;
                ptr = ptr->next;
            }
            item->pid = atoi(dptr->d_name);
            if(read_info(dptr->d_name, item) != 0) {
                free_list(head);
                return NULL;
            }
        }
    }
    closedir(dsptr);
    return head;
}

static int read_info(char *d_name, struct proc_list *item) {
    char dir[20];
    char name[NAME_SIZE];
    char buffer[BUF_SIZE];
    FILE *fptr;

    memset(dir, 0, sizeof(dir));
    snprintf(dir, sizeof(dir), "%s%s", "/proc/", d_name);
    chdir(dir);
    fptr = fopen("status", "r");
    if(fptr == NULL) {
        printf("Error reading %s%s\n", dir, "status");
        return -1;
    }
    while(fgets(buffer, BUF_SIZE - 1, fptr)) {
        if(strstr(buffer, "Name:")) {
            sscanf(buffer, "%s%s", name, item->name);
        } else if(strstr(buffer, "PPid:")) {
            sscanf(buffer, "%s%d", name, &(item->ppid));
        } else if(strstr(buffer, "State:")) {
            sscanf(buffer, "%s %c", name, &(item->status));
        }
    }
    fclose(fptr);
    return 0;
}

static void free_list(struct proc_list *head) {
    struct proc_list *ptr;

    while(head != NULL) {
        ptr = head;
        head = head->next;
        free(ptr);
    }
}

static int show_info(struct proc_list *head, char *input) {
    struct proc_list *ptr;
    int ret = FAILED;

    for(ptr = head; ptr != NULL; ptr = ptr->next) {
        if(NULL != strstr(ptr->name, input)) {
            ret = SUCCESS;
        }
    }
    return ret;
}

static int is_num(char *d_name) {
    int i, size;

    size = strlen(d_name);
    if(size == 0)
        return -1;

    /* determine if the dir name is number */
    for(i = 0; i < size; i++)
        if(d_name[i] < '0' || d_name[i] > '9')
            return -1;
    return 0;
}

/**
 * Defined case run in mmi mode,this mode support UI.
 * @return, 0 -success; -1
 */
static int32_t module_run_mmi(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

/**
 * Defined case run in PCBA mode, fully automatically.
 *
 */
static int32_t module_run_pcba(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

/**
 * Defined OEM specified run cases.
 *
 */
static int32_t run_daemon(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    /* step 1 get the running progress name */
    if((SUCCESS == Isinproc(DNSMASQ_D)) && (SUCCESS == Isinproc(HOSTAPD_D))) {
        return SUCCESS;
    }
    return FAILED;
}

static int32_t run_ssid(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    char ssid[64];
    char cmd[100] = { 0 };
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "awk -F= '{if($1==\"ssid\") print $2}' /etc/hostapd.conf > /tmp/ssid");
    system(cmd);
    memset(ssid, 0, sizeof(ssid));
    if(read_file("/tmp/ssid", ssid, sizeof(ssid))) {
        ALOGE("%s read /tmp/ssid error!", __FUNCTION__);
        return FAILED;
    }
    if(strlen(ssid) <= 0) {
        ALOGE("%s ssid was invalidity!", __FUNCTION__);
        return FAILED;
    }
    memset(wifi_result.ssid, 0, sizeof(wifi_result.ssid));
    memcpy(wifi_result.ssid, ssid, strlen(ssid));
    return SUCCESS;
}

static int32_t run_passwd(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    char passwd[64];
    char cmd[100] = { 0 };
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "awk -F= '{if($1==\"wpa_passphrase\") print $2}' /etc/hostapd.conf > /tmp/passwd");
    system(cmd);
    memset(passwd, 0, sizeof(passwd));
    if(read_file("/tmp/passwd", passwd, sizeof(passwd))) {
        ALOGE("%s read /tmp/passwd error!", __FUNCTION__);
        return FAILED;
    }
    if(strlen(passwd) <= 0) {
        ALOGE("%s passwd was invalidity!", __FUNCTION__);
        return FAILED;
    }
    memset(wifi_result.passwd, 0, sizeof(wifi_result.passwd));
    memcpy(wifi_result.passwd, passwd, strlen(passwd));
    return SUCCESS;
}

static int32_t run_wlandrv(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));
    if(read_file("/proc/modules", buffer, sizeof(buffer))) {
        ALOGE("%s can not open /proc/modules", __FUNCTION__);
        return FAILED;
    }
    if(!strstr(buffer, "wlan")) {
        ALOGE("%s can not find wlan module\n", __FUNCTION__);
        return FAILED;
    }
    return SUCCESS;
}

static int32_t run_getip(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    char ip[64];

    system("ifconfig wlan0 | sed -e '/.*inet addr:/!d;s///;s/ .*//' > /tmp/myip");
    memset(ip, 0, sizeof(ip));
    if(read_file("/tmp/myip", ip, sizeof(ip))) {
        ALOGE("%s can not open /tmp/myip", __FUNCTION__);
        return FAILED;
    }
    if(strlen(ip) < 7) {
        ALOGE("%s the wlan's ip error", __FUNCTION__);
        return FAILED;
    }
    memset(wifi_result.ip, 0, sizeof(wifi_result.ip));
    memcpy(wifi_result.ip, ip, strlen(ip));
    return SUCCESS;
}

static int32_t run_wifimac(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start:%s", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    char wlanmacaddr[20];

    /* step 1 check the wlan0 node */
    if(!check_file_exist(get_value("wifi_address"))) {
        ALOGE("%s cannot find wlan0 node\n", __FUNCTION__);
        return FAILED;
    }
    /* step 2 get the wlan mac address */
    memset(wlanmacaddr, 0, sizeof(wlanmacaddr));
    if(read_file(get_value("wifi_address"), wlanmacaddr, sizeof(wlanmacaddr))) {
        ALOGE("%s read mac address error\n", __FUNCTION__);
        return FAILED;
    }
    /* step 3 report the result */
    memcpy(wifi_result.mac, wlanmacaddr, sizeof(wlanmacaddr));
    return SUCCESS;
}

static int32_t module_init(const mmi_module_t * module, unordered_map < string, string > &params) {
    ALOGI("%s start", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    /* init wifi_result struct */
    memset(&wifi_result, 0, sizeof(wifi_result));
    /* init the pthread mutex */
    pthread_mutex_init(&g_mutex, NULL);
    if(!check_file_exist(get_value("wifi_address"))) {
        ALOGE("%s wlan0 node was not running,we are runnnig it now ", __FUNCTION__);
        system("modprobe wlan0");
        sleep(1);
        if(!check_file_exist(get_value("wifi_address"))) {
            ALOGE("%s wlan node was in trouble! ", __FUNCTION__);
            return FAILED;
        }
    }
    return SUCCESS;
}

static int32_t module_deinit(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    system("rmmod wlan");
    return SUCCESS;
}

static int32_t module_stop(const mmi_module_t * module) {
    ALOGI("%s start.", __FUNCTION__);
    if(module == NULL) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    kill_thread(module->run_pid);
    return SUCCESS;
}

/**
 * Before call Run function, caller should call module_init first to initialize the module.
 * the "cmd" passd in MUST be defined in cmd_list ,mmi_agent will validate the cmd before run.
 *
 */
static int32_t module_run(const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params) {
    ALOGI("%s start.", __FUNCTION__);
    int ret = -1;
    int i = -1;

    if(!module || !cmd) {
        ALOGE("%s NULL point  received ", __FUNCTION__);
        return FAILED;
    }
    char buf[1024] = { 0 };

    for(i = 0; i < sizeof(case_func) / sizeof(case_run); i++) {
        pthread_mutex_lock(&g_mutex);
        ret = case_func[i].run_testp(module, params);
        pthread_mutex_unlock(&g_mutex);
        if(FAILED == ret) {
            break;
        }
    }
    pthread_mutex_lock(&g_mutex);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, 8 + strlen(wifi_result.ssid), "SSID : %s\n", wifi_result.ssid);
    strlcat(buf, "\n", sizeof(buf));
    strlcat(buf, "passwd : ", sizeof(buf));
    strlcat(buf, wifi_result.passwd, sizeof(buf));
    strlcat(buf, "\n", sizeof(buf));
    strlcat(buf, "ip : ", sizeof(buf));
    strlcat(buf, wifi_result.ip, sizeof(buf));
    strlcat(buf, "\n", sizeof(buf));
    strlcat(buf, "Mac Addr : ", sizeof(buf));
    strlcat(buf, wifi_result.mac, sizeof(buf));
    strlcat(buf, "\n", sizeof(buf));
    pthread_mutex_unlock(&g_mutex);

    if(!strcmp(cmd, SUBCMD_MMI)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_MMI, buf, strlen(buf), PRINT_DATA);
    } else if(!strcmp(cmd, SUBCMD_PCBA)) {
        module->cb_print(params[KEY_MODULE_NAME].c_str(), SUBCMD_PCBA, buf, strlen(buf), PRINT_DATA);
    }
    /** Default RUN mmi*/
    return ret;
}

/**
 * Methods must be implemented by module.
 */
static struct mmi_module_methods_t module_methods = {
    .module_init = module_init,
    .module_deinit = module_deinit,
    .module_run = module_run,
    .module_stop = module_stop,
};

/**
 * Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
 * and the fields of this data structure must be initialize in strictly sequence as definition,
 * please don't change the sequence as g++ not supported in CPP file.
 */
mmi_module_t MMI_MODULE_INFO_SYM = {
    .version_major = VERSION_MAJOR,
    .version_minor = VERSION_MINOR,
    .name = MODULE_NAME,
    .author = MODULE_AUTHOR,
    .methods = &module_methods,
    .module_handle = NULL,
    .supported_cmd_list = NULL,
    .supported_cmd_list_size = 0,
    .cb_print = NULL, /**it is initialized by mmi agent*/
    .run_pid = RUN_PID,
};
