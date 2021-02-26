/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 XTRA-T WiFi mq-server

 GENERAL DESCRIPTION
 This header declares the public interface of mq server core

 Copyright (c) 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc

 Copyright (c) 2012 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.
 =============================================================================*/
#ifndef __XTRAT_WIFI_MESSAGE_QUEUE_SERVER_H__
#define __XTRAT_WIFI_MESSAGE_QUEUE_SERVER_H__

#include <base_util/queue.h>
#include <base_util/memorystream.h>

using namespace qc_loc_fw;

class MessageQueueServer
{
public:
  static MessageQueueServer * createInstance(const bool name_in_file_system = true);
  virtual ~MessageQueueServer() = 0;

  virtual int setServerNameDup(const char * const name) = 0;
  virtual int openListenerSocket() = 0;
  virtual void run() = 0;
};

extern "C" pthread_mutex_t srv_sockt_mutex;
extern "C" pthread_cond_t  srv_sockt_cond;


#endif //#ifndef __XTRAT_WIFI_MESSAGE_QUEUE_SERVER_H__
