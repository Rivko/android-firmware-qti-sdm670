/*************************************************
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************/

#include <sys/wait.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <logwrap/logwrap.h>
#include <sys/system_properties.h>
#include <sysutils/SocketClient.h>
#include <stdlib.h>
#include "DataController.h"
#include "NetdConstants.h"


const int  DataController::MAX_CMD_ARGS = 32;
const int  DataController::MAX_CMD_LEN = 1024;

const char* DataController::BACKGROUND_DATA_PROPERTY = "sys.background.data.disable";
const char* DataController::BACKGROUND_DATA_WHITELIST = "sys.background.exception.app";

const char* DataController::IPTABLES_PATH = "/system/bin/iptables";
const char* DataController::IP6TABLES_PATH = "/system/bin/ip6tables";

const std::string DataController::IPV4_VALUE_1 = "8.8.8.4";
const std::string DataController::IPV4_VALUE_2 = "8.8.8.8";
const std::string DataController::IPV6_VALUE_1 = "2001:4860:4860::884";
const std::string DataController::IPV6_VALUE_2 = "2001:4860:4860::8888";

DataController::DataController(void) {
}

int DataController::runIpxtablesCmd(const char *cmd) {
    int res = 0;
    char newcmd[MAX_CMD_LEN];

    snprintf(newcmd, sizeof(newcmd), "-w %s", cmd);
    ALOGV(" runIpxtablesCmd(cmd=%s)", newcmd);
    res |= runIptablesCmd(newcmd,  IptIpV4);
    res |= runIptablesCmd(newcmd,  IptIpV6);
    return res;
}

int DataController::runIptablesCmd(const char *cmd,
                                        IptIpVersion iptVersion) {
    char buffer[MAX_CMD_LEN];
    const char *argv[MAX_CMD_ARGS];
    int argc = 0;
    char *next = buffer;
    char *tmp;
    int res;
    int status = 0;

    std::string fullCmd = cmd;

    fullCmd.insert(0, " ");
    fullCmd.insert(0, iptVersion == IptIpV4 ? IPTABLES_PATH : IP6TABLES_PATH);

    memset(buffer, '\0', sizeof(buffer));
    strlcpy(buffer, fullCmd.c_str(), sizeof(buffer));

    if (buffer[sizeof(buffer) - 1]) {
        ALOGE("iptables command too long");
        return -1;
    }

    argc = 0;
    while ((tmp = strsep(&next, " "))) {
        argv[argc++] = tmp;
        if (argc >= MAX_CMD_ARGS) {
            ALOGE("iptables argument overflow");
            return -1;
        }
    }

    argv[argc] = NULL;
    res = android_fork_execvp(argc, (char **)argv, &status, false,
            true);
    res = res || !WIFEXITED(status) || WEXITSTATUS(status);
    if (res) {
      ALOGE("runIptablesCmd(): res=%d status=%d failed %s", res, status,
            fullCmd.c_str());
    }
    ALOGD("runIptablesCmd %s",fullCmd.c_str());
    return res;
}

int DataController::blockAllData() {
    static char property[PROP_VALUE_MAX];
    if ((__system_property_get(BACKGROUND_DATA_PROPERTY, property) > 0)
                && strcmp(property, "true") == 0) {
        int res = 0;
        std::string iptCmd = "-I oem_out -d " + IPV4_VALUE_1 + " -p udp --dport 53 -j DROP";
        res |= runIptablesCmd(iptCmd.c_str(), IptIpV4);

        iptCmd = "-I oem_out -d " + IPV6_VALUE_1 + " -p udp --dport 53 -j DROP";
        res |= runIptablesCmd(iptCmd.c_str(), IptIpV6);

        if (res) {
            ALOGE("Failed to add zero balance block rule for 8.8.8.4");
            return -1;
        } else {
            ALOGI("Passed to add zero balance block rule for 8.8.8.4");
        }

        res = 0;

        iptCmd = "-I oem_out -d " + IPV4_VALUE_2 + " -p udp --dport 53 -j DROP";
        res |= runIptablesCmd(iptCmd.c_str(), IptIpV4);

        iptCmd = "-I oem_out -d " + IPV6_VALUE_2 + " -p udp --dport 53 -j DROP";
        res |= runIptablesCmd(iptCmd.c_str(), IptIpV6);

        if (res) {
            ALOGE("Failed to add zero balance block rule for 8.8.8.8");
            return -1;
        } else {
            ALOGI("Passed to add zero balance block rule for 8.8.8.8");
        }
    }
    return 0;
}

int DataController::unblockAllData() {
    static char property[PROP_VALUE_MAX];
    if ((__system_property_get(BACKGROUND_DATA_PROPERTY, property) > 0)
            && strcmp(property, "false") == 0) {
        std::string iptCmd = "-F oem_out";
        if (runIpxtablesCmd(iptCmd.c_str())) {
            ALOGE("Failed to flush zero balance block rules");
            return -1;
        } else {
            ALOGI("Passed to flush zero balance block rules");
        }
    }
    return 0;
}

unsigned DataController::checkAppInWhitelist(SocketClient *cli) {
    uid_t ruid = cli->getUid();
    //Zero balance handling
    static char property[PROP_VALUE_MAX];
    if (__system_property_get(BACKGROUND_DATA_PROPERTY, property) > 0) {
        if (strcmp(property, "true") == 0) {
            //if it is in whitelist allow
            static char appUid[PROP_VALUE_MAX];
            char *ptr;
            if (__system_property_get(BACKGROUND_DATA_WHITELIST, appUid) > 0) {
                short found = 0;
                //Get all the uids for exception, parse
                char *token = strtok_r (appUid,",", &ptr);
                while (token != NULL) {
                    if (((uid_t)atoi(token) == ruid)) {
                        ALOGI("Zero Balance: checkAppInWhitelist:in whitelist. Allow uid: %u",ruid);
                        found = 1;
                        break;
                    }
                    token = strtok_r (NULL, ",", &ptr);
                }
                if(found == 0) {
                    ALOGI("Zero Balance: checkAppInWhitelist:not in whitelist. Block Uid: %u",ruid);
                    return NETID_INVALID;
                }
            }
        }
    }
    return 1;
}
