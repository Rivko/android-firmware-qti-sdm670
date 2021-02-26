/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <sys/wait.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <logwrap/logwrap.h>
#include <sys/system_properties.h>
#include <sysutils/SocketClient.h>
#include <stdlib.h>
#include "DataCtl.h"
#include "NetdConstants.h"

const int DataCtl::MAX_CMD_ARGS = 32;
const int DataCtl::MAX_CMD_LEN = 1024;

const char *DataCtl::IPTABLES_PATH = "/system/bin/iptables";
const char *DataCtl::IP6TABLES_PATH = "/system/bin/ip6tables";

const std::string DataCtl::DATA_CHANEL[] = { "rmnet+", "pdp+", "ppp+", "uwbr+",
		"wimax+", "vsnet+", "ccmni+", "usb+" };

DataCtl::DataCtl(void) {
}

int DataCtl::runIpxtablesCmd(const char *cmd) {
	int res = 0;
	char newcmd[MAX_CMD_LEN];

	snprintf(newcmd, sizeof(newcmd), "-w %s", cmd);
	ALOGV(" runIpxtablesCmd(cmd=%s)", newcmd);
	res |= runIptablesCmd(newcmd, IptIpV4);
	res |= runIptablesCmd(newcmd, IptIpV6);
	return res;
}

int DataCtl::runIptablesCmd(const char *cmd, IptIpVersion iptVersion) {
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

	argc = 0;
	while ((tmp = strsep(&next, " "))) {
		argv[argc++] = tmp;
		if (argc >= MAX_CMD_ARGS) {
			ALOGE("iptables argument overflow");
			return -1;
		}
	}

	argv[argc] = NULL;
	res = android_fork_execvp(argc, (char **) argv, &status, false, true);
	res = res || !WIFEXITED(status) || WEXITSTATUS(status);
	if (res) {
		ALOGE("runIptablesCmd(): res=%d status=%d failed %s", res, status,
				fullCmd.c_str());
	}
	ALOGI("runIptablesCmd %s", fullCmd.c_str());
	return res;
}

bool DataCtl::prepare(std::string blackListName) {
	int res = 0;

	std::string iptCmd = "-w -N " + blackListName;
	res = runIptablesCmd(iptCmd.c_str(), IptIpV4);
	ALOGE("prepare add res: %d", res);
	iptCmd = "-w -D OUTPUT -j " + blackListName;
	res = runIptablesCmd(iptCmd.c_str(), IptIpV4);
	ALOGE("prepare remove res: %d", res);
	/**Add blacklist to OUTPUT*/
	iptCmd = "-w -A OUTPUT -j " + blackListName;
	res = runIptablesCmd(iptCmd.c_str(), IptIpV4);
	if (res) {
		ALOGE("prepare res: %d, -A output exit!", res);
		return false;
	}

	/**Flush blacklist*/
	iptCmd = "-w -F " + blackListName;
	res = runIptablesCmd(iptCmd.c_str(), IptIpV4);
	if (res) {
		ALOGE("prepare res: %d, -F blacklist exit!", res);
		return false;
	}
	return true;
}

bool DataCtl::enableMms(char *blackListUids) {
	ALOGI("%s: start with ", __FUNCTION__);
	std::string name = "blacklist-mms";
	if (blackListUids == NULL) {
		ALOGE("enableMms NULL point exit!");
		return false;
	}

	int res = prepare(name);
	if (!res) {
		ALOGE("enableMms res: %d, prepare failed exit!", res);
		return false;
	}
	char *outer_ptr = NULL;
	char *s = strtok_r(blackListUids, "|", &outer_ptr);
	std::string iptCmd = "";
	while (s) {
		ALOGE("current uid is :%s", s);
		iptCmd = "-A " + name + " -m owner --uid-owner " + std::string(s)
				+ " -d 10.0.0.200 -j DROP";
		res = runIptablesCmd(iptCmd.c_str(), IptIpV4);
		if (res) {
			ALOGE("enableMms res: %d, 200 exit!", res);
			return false;
		}

		iptCmd = "-A " + name + " -m owner --uid-owner " + std::string(s)
				+ " -d 10.0.0.172 -j DROP";
		res = runIptablesCmd(iptCmd.c_str(), IptIpV4);
		if (res) {
			ALOGE("enableMms res: %d, 172 exit!", res);
			return false;
		}
		s = strtok_r(NULL, "|", &outer_ptr);
	}

	return 0;
}

bool DataCtl::enableData(char *blackListUids) {
	ALOGI("%s: start with ", __FUNCTION__);
	std::string name = "blacklist-data";
	if (blackListUids == NULL) {
		ALOGE("enableData NULL point exit!");
		return false;
	}

	int res = prepare(name);

	if (!res) {
		ALOGE("enableData res: %d, prepare failed exit!", res);
		return false;
	}

	char *outer_ptr = NULL;
	char *s = strtok_r(blackListUids, "|", &outer_ptr);
	std::string iptCmd = "";
	while (s) {
		ALOGE("current uid is :%s", s);
		int j = 0;
		for (j = 0; j < MAX_DATA_CHANEL; j++) {
			iptCmd = "-A " + name + " -m owner --uid-owner " + std::string(s)
					+ " -o " + DATA_CHANEL[j] + " -j REJECT";
			res = runIptablesCmd(iptCmd.c_str(), IptIpV4);
			if (res) {
				ALOGE("enableData run cmd: %s, with res:%d", iptCmd.c_str(),
						res);
				return false;
			}
		}
		s = strtok_r(NULL, "|", &outer_ptr);
	}
	return true;
}
