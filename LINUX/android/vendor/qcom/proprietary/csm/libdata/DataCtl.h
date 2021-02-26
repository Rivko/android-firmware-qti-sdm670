/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef _DATA_CONTROLLER_H
#define _DATA_CONTROLLER_H
#include <sysutils/SocketClient.h>
#include <string>
#define NETID_INVALID UINT_MAX

class DataCtl {
public:
   DataCtl();
   bool prepare(std::string blackListName);
  bool enableMms(char *blackListUids);
  bool enableData(char *blackListUids);
  static const char* IPTABLES_PATH;
  static const char* IP6TABLES_PATH;

protected:
   enum IptIpVersion { IptIpV4, IptIpV6 };
   int runIpxtablesCmd(const char *cmd);
   int runIptablesCmd(const char *cmd,IptIpVersion iptVersion);

private:
   static const int  MAX_CMD_ARGS;
   static const int  MAX_CMD_LEN;
   static const int  MAX_DATA_CHANEL=8;
   static const std::string  DATA_CHANEL[MAX_DATA_CHANEL];
};
#endif
