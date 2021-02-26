/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 XTRA-T WiFi mq-server

 GENERAL DESCRIPTION
 This header declares the per-clinet information

 Copyright (c) 2012,2014 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.
 =============================================================================*/
#ifndef __XTRAT_WIFI_MESSAGE_QUEUE_CLIENT_INFO_H__
#define __XTRAT_WIFI_MESSAGE_QUEUE_CLIENT_INFO_H__

#include <sys/socket.h>
#include <sys/un.h>

#include <base_util/queue.h>
#include <base_util/postcard.h>
#include <base_util/memorystream.h>
#include <base_util/list.h>

using namespace qc_loc_fw;


class MessageQueueClientInfo
{
public:
  MessageQueueClientInfo();
  virtual ~MessageQueueClientInfo();

  int setNameDup(const char * const name);
  int addAlias(const char * const name);
  bool hasAlias(const char * const name) const;
  const char * getName();

  void setSocket(const int s, const sockaddr_un addr, const ucred cred);
  int getSocket();

  int read();
  int write();
  int error();

  InMemoryStream * getReadBufferIfReady();
  bool IsWriteQueueReady();

  int deliverMessage(InMemoryStream * new_buffer);

private:
  int m_socket;
  char* m_name;

  // A connection with the location-mq server is allowed to
  // have multiple names associated with it.
  // Reason to allow this is that a connection can be just a proxy
  // to send/receive messages on behalf of other modules in the
  // same process. This reduces the number of resources required
  // for a connection and full bandwidth usage of a connection.

  // Usage: To add alias to a connection send message to SERVER
  // "TO" SERVER"
  // "FROM" <client_name>
  // "REQ" "ADD_ALIAS"
  // "ALIAS" <alias>

  typedef qc_loc_fw::List<char *> LOOKUP_LIST;
  LOOKUP_LIST mSocketNameAliasList;

  bool m_is_read_buffer_ready;
  OutMemoryStream * m_read_buffer;
  InMemoryStream * m_read_buffer_extract;

  typedef Queue<InMemoryStream *> WriteQueue;
  WriteQueue m_write_queue;

  sockaddr_un m_client_addr;
  ucred m_credentials;

  void close();

  //void flush ();
  //int push(OutMemoryStream * const pOut);
  //InMemoryStream * pop();

};



#endif //#ifndef __XTRAT_WIFI_MESSAGE_QUEUE_CLIENT_INFO_H__
