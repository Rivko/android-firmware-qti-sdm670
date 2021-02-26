/*
 * Copyright (c) 2013, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cutils/sockets.h>
#include "cutils/misc.h"
#include "cutils/properties.h"
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#define LOG_TAG "WifiFtm"
#include <cutils/log.h>

#define WIFI_FTMD_SOCKET "wififtmd_server"

static const char FTMD_NAME[] = "wifi_ftmd";
static const char FTMD_PROP_NAME[] = "init.svc.wifi_ftmd";
const char* pFtmdPropName = FTMD_PROP_NAME;
const char* pFtmdName = FTMD_NAME;

int ctrl_socket = -1;

static int _wifi_ftmd_start() {
    char ftmd_status[PROPERTY_VALUE_MAX] = { '\0' };
    int count = 20; /* wait at most 2 seconds for completion */
    const prop_info *pi;
    unsigned serial = 0;


    property_set("wifi.ftmd.load", "true");


    if (property_get(pFtmdPropName, ftmd_status, NULL)
            && strcmp(ftmd_status, "running") == 0) {
        ALOGI("wifi_ftmd_status alredy run. leave");
        return 0;
    }

    property_set("ctl.start", pFtmdName);
    sched_yield();

    while (count-- > 0){
        if (property_get(pFtmdPropName, ftmd_status, NULL)
                && strcmp(ftmd_status, "running") == 0) {
            return 0;
        }
        usleep(100000);
    }

    return -1;

}

static int _wifi_ftm_connect_ftmd() {
    int count = 30;
    int client_len, server_len;
    struct sockaddr_un client, server;

    ctrl_socket = socket(PF_UNIX, SOCK_DGRAM, 0);

    if (ctrl_socket < 0) {
        ALOGE("failed to allocate ctrl socket");
        return -1;
    }

    while(count-- > 0){
        if(socket_local_client_connect(ctrl_socket, WIFI_FTMD_SOCKET,
               ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_DGRAM) >= 0){
            return 0;
         }
         usleep(100000);
    }

    ALOGE("failed to connect ctrl socket");
    close(ctrl_socket);
    return -1;
}

int wifi_ftmd_start() {
    if (_wifi_ftmd_start() < 0)
        return -1;

    if (_wifi_ftm_connect_ftmd() < 0) {
        property_set("ctl.stop", pFtmdName);
        return -1;
    }

    return 0;
}

void wifi_ftmd_stop() {
    char ftmd_status[PROPERTY_VALUE_MAX] = { '\0' };

    if (ctrl_socket > 0)
        close(ctrl_socket);

    property_set("wifi.ftmd.load", "false");

    if (property_get(pFtmdPropName, ftmd_status, NULL)
            && strcmp(ftmd_status, "running") == 0) {
        property_set("ctl.stop", pFtmdName);
    }

}


int wifi_ftm_send_cmd(const char* cmd) {

    if (ctrl_socket < 0)
        return -1;

    if (send(ctrl_socket, cmd, strlen(cmd), 0) < 0) {
        ALOGE("send command to ftmd failed");
        return -1;
    }

    return 0;
}

int wifi_ftm_send_cmd(const char* cmd, char* reply, size_t *reply_len) {
    if (ctrl_socket < 0)
        return -1;
    if (send(ctrl_socket, cmd, strlen(cmd), 0) < 0) {
        ALOGE("send command to ftmd failed");
        return -1;
    }
    sleep(1);

    int buf_len = *reply_len;

    int count = 50;

    while(count-- > 0){
        if(property_get("wifi.ftmd.reply", reply, NULL) > 0){
            *reply_len = strlen(reply);
            return 0;
        }
        usleep(100000);
    }

    ALOGE("recv command to ftmd failed %s", strerror(errno));
    return -1;
}

#if 0

int main()
{
    wifi_ftmd_start();
    wifi_ftm_send_cmd("START FTM");
    sleep(5);
    wifi_ftm_send_cmd("SET CHANNEL 1");
    sleep(1);
    wifi_ftm_send_cmd("SET TX_POWER 10");
    sleep(1);
    wifi_ftm_send_cmd("SET TX_RATE MCS_72_2_MBPS");
    wifi_ftm_send_cmd("START TX");
    sleep(2);
    wifi_ftm_send_cmd("STOP TX");
    sleep(2);
    wifi_ftm_send_cmd("START RX");

    wifi_ftm_send_cmd("REPORT RX");

    wifi_ftm_send_cmd("STOP RX");

    wifi_ftm_send_cmd("STOP FTM");
    sleep(300);

    return 0;

}
#endif
