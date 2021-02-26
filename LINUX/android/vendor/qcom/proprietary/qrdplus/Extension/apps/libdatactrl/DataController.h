/*************************************************
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************/

#ifndef _DATA_CONTROLLER_H
#define _DATA_CONTROLLER_H
#include <sysutils/SocketClient.h>
#include <string>
#define NETID_INVALID UINT_MAX

class DataController {
public:
   DataController();
   int blockAllData();
   int unblockAllData();
   unsigned checkAppInWhitelist(SocketClient *cli);

  static const char* BACKGROUND_DATA_PROPERTY ;
  static const char* BACKGROUND_DATA_WHITELIST ;
  static const char* IPTABLES_PATH;
  static const char* IP6TABLES_PATH;
  static const std::string IPV4_VALUE_1;
  static const std::string IPV4_VALUE_2;
  static const std::string IPV6_VALUE_1;
  static const std::string IPV6_VALUE_2;

protected:
   enum IptIpVersion { IptIpV4, IptIpV6 };
   int runIpxtablesCmd(const char *cmd);
   int runIptablesCmd(const char *cmd,IptIpVersion iptVersion);

private:
   static const int  MAX_CMD_ARGS;
   static const int  MAX_CMD_LEN;
};
#endif
