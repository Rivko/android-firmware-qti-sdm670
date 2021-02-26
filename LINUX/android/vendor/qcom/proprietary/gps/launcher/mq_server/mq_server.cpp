/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 XTRA-T WiFi mq-server

 GENERAL DESCRIPTION
 This component implements the server core select loop

 Copyright (c) 2012,2014 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.
 =============================================================================*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <new>

// define FD_SETSIZE to any number you like to handle more clients. default is 64 or 256
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>

#include "mq_server.h"
#include "mq_client_info.h"

#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO_RC(ERR,RC,X) if(0!=(RC=(X))) {result = (ERR); break;}


MessageQueueServer::~MessageQueueServer()
{
}

class MessageQueueServerImpl: public MessageQueueServer
{
private:
  static const char * const TAG;
public:
  MessageQueueServerImpl(const bool name_in_file_system);
  ~MessageQueueServerImpl();

  int setServerNameDup(const char * const name);
  int openListenerSocket();
  void run();

protected:
  bool m_continue;
  typedef List<MessageQueueClientInfo *> ClientInfoListType;
  ClientInfoListType m_clientInfoList;

  int getNextTxId();

  int FlushClosedClient();
  int lookupAndDeliver(const char * const name, InMemoryStream * new_buffer);
  int prepareFdSets(int & nfds, fd_set & read_set, fd_set & write_set, fd_set & error_set);
  int getSocketError(const int socket);
  void accept();
  int postprocessFdSets(fd_set & read_set, fd_set & write_set, fd_set & error_set);

  int ProcessNewMessage(MessageQueueClientInfo * const info, InMemoryStream * new_buffer);
  void BroadcastClientRegistration(const char * const name);
  const int MIN_TX_ID;
  const int MAX_TX_ID;
  int m_last_tx_id;

  char * m_server_name;
  int m_listener_socket;
  bool m_name_in_file_system;
};

const char * const MessageQueueServerImpl::TAG = "MessageQ";

MessageQueueServerImpl::MessageQueueServerImpl(const bool name_in_file_system) :
    m_continue(true),MIN_TX_ID(0),MAX_TX_ID(10000000),m_last_tx_id(MIN_TX_ID),
    m_server_name(0), m_listener_socket(-1), m_name_in_file_system(name_in_file_system)
{
}

MessageQueueServerImpl::~MessageQueueServerImpl()
{
  log_warning(TAG, "deleting server instance");

  for (ClientInfoListType::Iterator it = m_clientInfoList.begin(); it != m_clientInfoList.end(); ++it)
  {
    MessageQueueClientInfo * info = *it;
    const char * const client_name = info->getName();
    log_info(TAG, "deleting client [%s]", client_name);
    delete info;
    *it = 0;
  }

  if(m_server_name)
  {
    if(m_name_in_file_system)
    {
      (void) unlink(m_server_name);
    }
  }

  if(m_listener_socket > 0)
  {
    (void) close(m_listener_socket);
    m_listener_socket = -1;
  }

  delete[] m_server_name;
  m_server_name = 0;
}

int MessageQueueServerImpl::setServerNameDup(const char * const name)
{
  int result = 1;
  do
  {
    BREAK_IF_NON_ZERO(2, m_server_name);
    BREAK_IF_ZERO(3, name);
    size_t length = strlen(name);
    BREAK_IF_ZERO(4, length);
    m_server_name = new char[length + 1];
    BREAK_IF_ZERO(5, m_server_name);
    memcpy(m_server_name, name, length + 1);

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "setServerNameDup failed %d", result);
  }
  return result;
}

int MessageQueueServerImpl::FlushClosedClient()
{

  int result = 1;
  MessageQueueClientInfo * info = 0;
  do
  {
    bool closed_socket_found = false;
    do
    {
      closed_socket_found = false;
      result = 100;
      for (ClientInfoListType::Iterator it = m_clientInfoList.begin(); it != m_clientInfoList.end(); ++it)
      {
        info = *it;
        if(info->getSocket() < 0)
        {
          closed_socket_found = true;
          // after this removal, the iterator we have at hand is no longer accurate.
          // must try from the beginning again
          BREAK_IF_NON_ZERO(101, m_clientInfoList.remove(info));
          log_debug(TAG, "removing client [%s] for the socket is already closed", info->getName());
          delete info;
          info = 0;
          break;
        }
      }
    } while (closed_socket_found);

    if(100 != result)
    {
      // something went wrong in the search
      break;
    }

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "FlushClosedClient failed %d", result);
  }
  return result;
}

int MessageQueueServerImpl::lookupAndDeliver(const char * const name, InMemoryStream * new_buffer)
{
  int result = 1;
  MessageQueueClientInfo * info = 0;
  do
  {
    BREAK_IF_ZERO(2, name);
    BREAK_IF_ZERO(3, new_buffer);

    result = 100;
    for (ClientInfoListType::Iterator it = m_clientInfoList.begin(); it != m_clientInfoList.end(); ++it)
    {
      info = *it;
      const char * const client_name = info->getName();
      if(0 != client_name)
      {
        if((0 == strcmp(client_name, name)) ||
           (info->hasAlias(name)))
        {
          // Create a new copy
          InMemoryStream * copy_buffer = new_buffer->clone();
          if (NULL != copy_buffer) {
            log_verbose(TAG, "lookupAndDeliver delivering message to client %s ", name);
            info->deliverMessage(copy_buffer);
          }
        }
        else
        {
          info = 0;
        }
      }
      else
      {
        // null client name: how could this happen?
        result = 101;
        break;
      }
    }

    if(100 != result)
    {
      // something went wrong in the search
      break;
    }



    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "looking up of [%s] failed %d", name, result);
  }
  return result;
}

int MessageQueueServerImpl::openListenerSocket()
{
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, m_server_name);

    struct sockaddr_un server_address;

    log_verbose(TAG, "creating server socket");
    if((m_listener_socket = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      log_error(TAG, "socket error: %d, [%s]", errno, strerror(errno));
      result = 3;
      break;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;

    if(m_name_in_file_system)
    {
      log_verbose(TAG, "binding to [%s]", m_server_name);

      (void) unlink(m_server_name);
      (void) snprintf(server_address.sun_path, sizeof(server_address.sun_path), "%s", m_server_name);
    }
    else
    {
      // linux 2.2+ supports abstract namespace for unix domain sockets
      // this is slightly better than on-file-system domain sockets because many times
      // the file system is read only for mobile devices
      (void) snprintf(server_address.sun_path, sizeof(server_address.sun_path), " %s", m_server_name);
      server_address.sun_path[0] = 0;
    }

    log_verbose(TAG, "binding server socket to name [%s]", m_server_name);
    // if this is a normal, visible-on-file-system, unix domain socket, we should unlink
    // before binding. this releases the resource occupied by that file and ensures
    // successful binding . note unlink is not necessary if we're using abstract namespace
    if(bind(m_listener_socket, (const struct sockaddr *)(void*)&server_address, sizeof(server_address)) != 0)
    {
      log_error(TAG, "bind error: %d, [%s]", errno, strerror(errno));
      result = 4;
      close(m_listener_socket);
      m_listener_socket = -1;
      break;
    }

    // note that we have to call fchmod AFTER we bind to the file
    // or nothing would happen
    // Although it's potentially possible for race condition between bind (which creates that node) and
    // listen (which starts listening to it), this socket should be created inside a folder which prevent
    // access from others
    // note it's actually 0770, not 0x770...the number is in octant...
    if(0 != chmod(m_server_name, 0770))
    {
      log_error(TAG, "error setting permission for socket file: %d, [%s]", errno, strerror(errno));
      result = 5;
      break;
    }

    if(listen(m_listener_socket, 5) != 0)
    {
      log_error(TAG, "listen error: %d, [%s]", errno, strerror(errno));
      result = 6;
      break;
    }

    result = 0;
  } while (0);

  if(0 != result)
  {
    if(m_listener_socket > 0)
    {
      close(m_listener_socket);
      m_listener_socket = -1;
    }

    if(m_server_name)
    {
      if(m_name_in_file_system)
      {
        unlink(m_server_name);
      }
    }

    log_error(TAG, "openListenerSocket failed %d", result);
  }
  return result;
}

int MessageQueueServerImpl::prepareFdSets(int & nfds, fd_set & read_set, fd_set & write_set, fd_set & error_set)
{
  nfds = 0;
  FD_ZERO(&read_set);
  FD_ZERO(&write_set);
  FD_ZERO(&error_set);

  FD_SET(m_listener_socket, &read_set);
  FD_SET(m_listener_socket, &error_set);
  nfds = m_listener_socket;

  MessageQueueClientInfo * info = 0;
  for (ClientInfoListType::Iterator it = m_clientInfoList.begin(); it != m_clientInfoList.end(); ++it)
  {
    info = *it;
    int s = info->getSocket();
    if(s >= 0)
    {
      FD_SET(s, &read_set);
      FD_SET(s, &error_set);

      if(info->IsWriteQueueReady())
      {
        FD_SET(s, &write_set);
      }

      if(s > nfds)
      {
        nfds = s;
      }
    }
    else
    {
      // the socket for this client has been closed, skip it
      log_warning(TAG, "prepareFdSets: socket for client [%s] already closed", info->getName());
    }
  }

  ++nfds;
  return 0;
}

int MessageQueueServerImpl::getSocketError(const int socket)
{
  int error = 0;
  socklen_t length = sizeof(error);
  if(0 == getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &length))
  {
    return error;
  }
  else
  {
    log_error(TAG, "getsockopt error: %d, [%s]", errno, strerror(errno));
    return 0;
  }
}

void MessageQueueServerImpl::accept()
{
  int result = 1;
  int new_socket = -1;
  MessageQueueClientInfo * new_client = 0;
  do
  {
    log_info(TAG, "client connection");

    sockaddr_un client_addr;
    socklen_t length = sizeof(client_addr);
    new_socket = ::accept(m_listener_socket, (sockaddr *)(void*)&client_addr, &length);
    if(new_socket < 0)
    {
      log_error(TAG, "accept socket error: %d, [%s]", errno, strerror(errno));
      result = 2;
      break;
    }

    // set NON-BLOCK
    int options = fcntl(new_socket, F_GETFL);
    if(options < 0)
    {
      log_error(TAG, "fcntl get error: %d, [%s]", errno, strerror(errno));
      result = 3;
      break;
    }
    if(fcntl(new_socket, F_SETFL, options | O_NONBLOCK) < 0)
    {
      log_error(TAG, "fcntl set error: %d, [%s]", errno, strerror(errno));
      result = 4;
      break;
    }

    // get credential
    ucred cred;
    length = sizeof(cred);
    if(0 != getsockopt(new_socket, SOL_SOCKET, SO_PEERCRED, &cred, &length))
    {
      log_error(TAG, "get peer cred error: %d, [%s]", errno, strerror(errno));
      result = 5;
      break;
    }
    log_debug(TAG, "new client pid %u, uid %d, gid %d", cred.pid, cred.uid, cred.gid);

    new_client = new MessageQueueClientInfo();
    BREAK_IF_ZERO(6, new_client);

    char temp_name[256];
    snprintf(temp_name, sizeof(temp_name), "NOT_REGISTERED:%d", cred.pid);
    BREAK_IF_NON_ZERO(7, new_client->setNameDup(temp_name));
    new_client->setSocket(new_socket, client_addr, cred);
    new_socket = -1;

    BREAK_IF_NON_ZERO(8, m_clientInfoList.add(new_client));
    new_client = 0;

    result = 0;
  } while (0);

  if(0 != result)
  {
    if(new_client)
    {
      delete new_client;
    }
    if(new_socket >= 0)
    {
      close(new_socket);
    }
    log_error(TAG, "accept failed %d", result);
  }
}

int MessageQueueServerImpl::postprocessFdSets(fd_set & read_set, fd_set & write_set, fd_set & error_set)
{
  // verify error
  if(FD_ISSET(m_listener_socket, &error_set))
  {
    const int error = getSocketError(m_listener_socket);
    log_error(TAG, "listener socket error: %d, [%s]", error, strerror(error));
  }

  if(FD_ISSET(m_listener_socket, &read_set))
  {
    accept();
  }

  MessageQueueClientInfo * info = 0;
  for (ClientInfoListType::Iterator it = m_clientInfoList.begin(); it != m_clientInfoList.end(); ++it)
  {
    info = *it;
    int s = info->getSocket();
    if(s >= 0)
    {
      if(FD_ISSET(s, &error_set))
      {
        info->error();
      }
      if(FD_ISSET(s, &write_set))
      {
        info->write();
      }
      if(FD_ISSET(s, &read_set))
      {
        info->read();
        InMemoryStream * const new_buffer = info->getReadBufferIfReady();
        if(new_buffer)
        {
          // new message, let's try to process it
          if(0 != ProcessNewMessage(info, new_buffer))
          {
            // something wrong
          }
        }
        else
        {
          // nothing, we haven't gotten a complete message yet
        }
      }
    }
    else
    {
      log_warning(TAG, "postprocessFdSets: socket for client [%s] already closed", info->getName());
    }
  }
  return 0;
}

int MessageQueueServerImpl::ProcessNewMessage(MessageQueueClientInfo * const info, InMemoryStream * new_buffer)
{
  InPostcard * card = 0;
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, info);
    BREAK_IF_ZERO(3, new_buffer);

    card = InPostcard::createInstance();
    BREAK_IF_ZERO(4, card);

    BREAK_IF_NON_ZERO(5, card->init(new_buffer->getBuffer(), new_buffer->getCapacity()));

    // you must have a "TO" field!
    const char * to = 0;
    BREAK_IF_NON_ZERO(6, card->getString("TO", &to));
    BREAK_IF_ZERO(7, to);

    if(0 == strcmp(to, "SERVER"))
    {
      log_debug(TAG, "ProcessNewMessage: [%s] server command", info->getName());

      // this is for server to process
      const char * req = 0;
      BREAK_IF_NON_ZERO(8, card->getString("REQ", &req));
      BREAK_IF_ZERO(9, req);

      if(0 == strcmp(req, "REGISTER"))
      {
        log_debug(TAG, "ProcessNewMessage: [%s] register to server", info->getName());

        const char * from = 0;
        BREAK_IF_NON_ZERO(10, card->getString("FROM", &from));
        BREAK_IF_ZERO(11, from);
        BREAK_IF_NON_ZERO(12, info->setNameDup(from));

        log_debug(TAG, "ProcessNewMessage: new client name [%s]", info->getName());
        // Below function is called to send broadcast to all location-mq
        // registered client for subscription request
        BroadcastClientRegistration(info->getName());
      }
      else if(0 == strcmp(req, "ADD-ALIAS"))
      {
        const char * alias = 0;
        BREAK_IF_NON_ZERO(13, card->getString("ALIAS", &alias));
        BREAK_IF_ZERO(14, alias);
        BREAK_IF_NON_ZERO(15, info->addAlias(alias));

        log_debug(TAG, "ProcessNewMessage: add alias [%s] to connection [%s]",
                  alias, info->getName());

        BroadcastClientRegistration(alias);
      }
      else if(0 == strcmp(req, "KILL"))
      {
        log_debug(TAG, "ProcessNewMessage: [%s] killing server", info->getName());
        m_continue = false;
      }
      else
      {
        // unknown request to server
        log_error(TAG, "ProcessNewMessage: [%s] unknown server request [%s]", info->getName(), req);
      }
    }
    else
    {
      // not to server, so it must be intended for some other client
     // MessageQueueClientInfo * target_info = 0;
      BREAK_IF_NON_ZERO(20, lookupAndDeliver (to, new_buffer));

    }

    result = 0;
  } while (0);

  if(card)
  {
    delete card;
    card = 0;
  }
  if(new_buffer)
  {
    // this should release the memory buffer with the stream object
    delete new_buffer;
    new_buffer = 0;
  }

  if(0 != result)
  {
    log_error(TAG, "ProcessNewMessage failed %d", result);
  }
  return result;
}


int MessageQueueServerImpl::getNextTxId()
{
  ++m_last_tx_id;

  if(m_last_tx_id > MAX_TX_ID)
  {
    m_last_tx_id = MIN_TX_ID;
    log_info(TAG, "getNextTxId rollback");
  }
  return m_last_tx_id;
}


void MessageQueueServerImpl::BroadcastClientRegistration(const char * const name)
{
  int result = 1;
  MessageQueueClientInfo * info = 0;
  OutPostcard* card = 0;

  log_verbose(TAG, "Request from client %s for registration", name);
  for (ClientInfoListType::Iterator it = m_clientInfoList.begin(); it != m_clientInfoList.end(); ++it)
  {
    card = OutPostcard::createInstance();
    info = *it;
    int tx_id = getNextTxId();

    BREAK_IF_ZERO(12, card);
    BREAK_IF_NON_ZERO(13,card->init());
    BREAK_IF_NON_ZERO(14,card->addString("TO", info->getName()));
    BREAK_IF_NON_ZERO(15,card->addString("FROM", "LOCATION-MQ"));
    BREAK_IF_NON_ZERO(16,card->addInt32("TX-ID",tx_id));
    BREAK_IF_NON_ZERO(17,card->addString("INFO", "REGISTER-EVENT"));
    BREAK_IF_NON_ZERO(19,card->addString("CLIENT", name));
    BREAK_IF_NON_ZERO(18,card->finalize ());
    OutMemoryStream* os_temp = card->getInternalBuffer();
    InMemoryStream* is_temp = InMemoryStream::createInstance(os_temp);
    if (is_temp == NULL)
    {
      log_error(TAG, "BroadcastClientRegistration: Memory allocation failed");
      result = 101;
      break;
    }
    // transfer ownership of the memory buffer to the target client
    info->deliverMessage(is_temp);
    if(card)
    {
      delete card;
      card = 0;
    }
    result = 0;
  }

  if(card)
  {
    delete card;
    card = 0;
  }

  if(0 != result)
  {
    log_error(TAG, "BroadcastClientRegistration failed %d", result);
  }
}

void MessageQueueServerImpl::run()
{
  m_continue = true;
  while (m_continue)
  {
    int result = 1;
    do
    {
      fd_set read_set;
      fd_set write_set;
      fd_set error_set;

      // maximum numbered socket + 1
      int nfds = 0;

      BREAK_IF_NON_ZERO(9, FlushClosedClient());
      BREAK_IF_NON_ZERO(10, prepareFdSets(nfds, read_set, write_set, error_set));

      int wait_result = select(nfds, &read_set, &write_set, &error_set, 0);
      if(wait_result < 0)
      {
        log_error(TAG, "select error: %d, [%s]", errno, strerror(errno));
        result = 11;
        break;
      }
      log_verbose(TAG, "wait result %d", wait_result);
      BREAK_IF_NON_ZERO(11, postprocessFdSets(read_set,write_set,error_set));

      result = 0;
    } while (0);

    if(0 != result)
    {
      log_error(TAG, "server queue main loop failed %d", result);
    }
  }
  log_warning(TAG, "leaving server loop");
}

MessageQueueServer * MessageQueueServer::createInstance(const bool name_in_file_system)
{
  return new (std::nothrow) MessageQueueServerImpl(name_in_file_system);
}

