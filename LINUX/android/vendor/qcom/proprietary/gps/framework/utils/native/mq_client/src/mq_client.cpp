/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 mq-client

 GENERAL DESCRIPTION
 This component implements the client connection functionality

 Copyright (c) 2012-2014 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.

 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <new>

// define FD_SETSIZE to any number you like to handle more clients. default is 64 or 256
#include <sys/types.h>

#include <sys/socket.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>

#include <mq_client/mq_client.h>
#include <base_util/postcard.h>
#include <base_util/sync.h>

#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}

#ifndef IZAT_OFFLINE
const char* const mq_server_name="/dev/socket/location/mq/location-mq-s";
#endif

namespace qc_loc_fw
{

MessageQueueServiceCallback::~MessageQueueServiceCallback()
{
}

MessageQueueClient::~MessageQueueClient()
{
}

class MessageQueueClientImpl: public MessageQueueClient
{
private:
  static const char * const TAG;
public:
  MessageQueueClientImpl();
  ~MessageQueueClientImpl();

  virtual int setServerNameDup(const char * const name);
  virtual int connect(const bool name_in_file_system);
  virtual int send(const MemoryStreamBase * const buffer);
  virtual int run_block(MessageQueueServiceCallback * const callback);
  virtual int shutdown();

protected:
  char * m_server_name;
  int m_socket;
  Mutex * m_mutex;

  MessageQueueServiceCallback * m_callback;
};

const char * const MessageQueueClientImpl::TAG = "MessageQ_Client";

MessageQueueClientImpl::MessageQueueClientImpl() :
    m_server_name(0), m_socket(-1), m_mutex(0), m_callback(0)
{
  m_mutex = Mutex::createInstance("MessageQueueClientImpl",false);
  if(0 == m_mutex)
  {
    log_error(TAG, "Cannot allocate mutex for MessageQueueClientImpl");
  }
}

MessageQueueClientImpl::~MessageQueueClientImpl()
{
  delete[] m_server_name;
  m_server_name = 0;

  {
    AutoLock autolock(m_mutex);
    if(m_socket >= 0)
    {
      int loop_count = 0;
      int outstanding_count = 0;
      int ret_val = 0;

      do
      {
        outstanding_count = 0;

        ret_val = ioctl(m_socket, SIOCOUTQ, &outstanding_count);

        // wait until message is being retrieved
        if (ret_val != 0)
        {
          log_error(TAG, "MessageQueueClient destructor: ioctl SIOCOUTQ failed %s",
                    strerror (errno));
          break;
        }

        if (outstanding_count <= 0)
        {
          log_debug (TAG, "MessageQueueClient destructor: ioctl SIOCOUTQ count: %d",
                     outstanding_count);
          break;
        }

        // sleep for 100 micro seconds in each loop
        ret_val = usleep (100);
        // sleep failed, break out
        if (ret_val != 0)
        {
          log_error(TAG, "MessageQueueClient destructor: usleep failed %s",
                    strerror (errno));
          break;
        }

        loop_count ++;
        // wait at most 2 second to flush out the data
        if (loop_count > 20)
        {
          log_error(TAG, "MessageQueueClient destructor: "
                         "socket data not read out fully after %d ms, return",
                    (loop_count * 100));
          break;
        }
      } while (1);

      log_verbose (TAG, "MessageQueueClient destructor: close socket after loop cnt %d",
                   loop_count);

      close(m_socket);
      m_socket = -1;
    }
  }

  delete m_mutex;
}

int MessageQueueClientImpl::shutdown()
{
  // multiple access. called by any thread which wishes to shutdown communication
  // shutdown closes the communication, but not closing the file descriptor
  // this prevents a race condition caused between 'closing' the file descriptor
  // and OS file descriptor reuse
  AutoLock autolock(m_mutex);
  errno = 0;
  if( 0 != ::shutdown(m_socket, SHUT_RDWR))
  {
    log_error(TAG, "shutdown failed. errno %d, [%s]", errno, strerror(errno));
    return 1;
  }
  return 0;
}

int MessageQueueClientImpl::setServerNameDup(const char * const name)
{
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, name);
    size_t length = strlen(name);
    BREAK_IF_ZERO(3, length);
    m_server_name = new (std::nothrow) char[length + 1];
    BREAK_IF_ZERO(4, m_server_name);
    memcpy(m_server_name, name, length + 1);

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "setServerNameDup failed %d", result);
  }
  return result;
}

int MessageQueueClientImpl::connect(const bool name_in_file_system)
{
  int result = 1;
  do
  {
    struct sockaddr_un server_address;
    log_verbose(TAG, "creating client socket");

    {
      AutoLock autolock(m_mutex);
      if((m_socket = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
      {
        log_error(TAG, "socket error: %d, [%s]", errno, strerror(errno));
        result = 2;
        break;
      }
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;

    if(name_in_file_system)
    {
      snprintf(server_address.sun_path, sizeof(server_address.sun_path), "%s", m_server_name);
    }
    else
    {
      // linux 2.2+ supports abstract namespace for unix domain sockets
      // this is slightly better than on-file-system domain sockets because many times
      // the file system is read only for mobile devices
      snprintf(server_address.sun_path, sizeof(server_address.sun_path), " %s", m_server_name);
      server_address.sun_path[0] = 0;
    }

    log_info(TAG, "connecting to server [%s]", m_server_name);

    if(::connect(m_socket, (struct sockaddr *)(void*)&server_address, sizeof(server_address)) != 0)
    {
      log_error(TAG, "connect error: %d, [%s]", errno, strerror(errno));
      result = 3;
      break;
    }

    log_info(TAG, "connected");

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "connect failed %d", result);

    AutoLock autolock(m_mutex);
    // close socket directly when we fail to connect
    // this is to allow re-use of the same mq client implementation
    // in case we fail to connect this time
    if(m_socket >= 0)
    {
      close(m_socket);
      m_socket = -1;
    }
  }
  return result;
}

int MessageQueueClientImpl::send(const MemoryStreamBase * const buffer)
{
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, buffer);

    size_t total_data_sent = 0;
    size_t total_data_length = buffer->getSize();

    //log_verbose(TAG, "data length to send %u", (unsigned int)total_data_length);

    result = 100;
    while (total_data_sent < total_data_length)
    {
      int socket_copy = -1;
      {
        AutoLock autolock(m_mutex);
        BREAK_IF_NON_ZERO(101, autolock.ZeroIfLocked());

        socket_copy = m_socket;
      }

      // MSG_NOSIGNAL prevents generation of SIGPIPE in case the remote closes the socket
      ssize_t data_sent = ::send(socket_copy, buffer->getBuffer() + total_data_sent, total_data_length - total_data_sent,
          MSG_NOSIGNAL);

      if(-1 == data_sent)
      {
        log_error(TAG, "send error: %d, [%s]", errno, strerror(errno));
        result = 102;
        break;
      }
      else
      {
        total_data_sent += data_sent;

        //log_verbose(TAG, "data length just sent %u, total to %u", (unsigned int)result, (unsigned int)total_data_sent);
      }
    }

    if(100 != result)
    {
      break;
    }

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "send failed %d", result);
  }
  return result;
}

int MessageQueueClientImpl::run_block(MessageQueueServiceCallback * const callback)
{
  m_callback = callback;
  char * buffer = 0;
  InMemoryStream * new_buffer = 0;
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, callback);

    result = 100;
    while (true)
    {
      int socket_copy = -1;
      {
        AutoLock autolock(m_mutex);
        BREAK_IF_NON_ZERO(101, autolock.ZeroIfLocked());

        socket_copy = m_socket;
      }

      PostcardBase::UINT32 card_length = 0;
      ssize_t nbytes = recv(socket_copy, &card_length, sizeof(card_length), MSG_WAITALL);
      if(-1 == nbytes)
      {
        log_error(TAG, "read error: %d, [%s]", errno, strerror(errno));
        result = 102;
        break;
      }
      else if(sizeof(card_length) != (size_t) nbytes)
      {
        result = 103;
        break;
      }

      // KlocWorks suggests putting some practical limits on the length
      // 1GB sounds like a reasonable limit to our IPC system
      if(card_length >= 0x3fffffff)
      {
        result = 104;
        break;
      }

      BREAK_IF_ZERO(105, card_length);
      buffer = new (std::nothrow) char[card_length + sizeof(card_length)];
      BREAK_IF_ZERO(106, buffer);

      *(PostcardBase::UINT32 *) buffer = card_length;

      nbytes = recv(m_socket, buffer + sizeof(card_length), card_length, MSG_WAITALL);
      if(-1 == nbytes)
      {
        log_error(TAG, "read error: %d, [%s]", errno, strerror(errno));
        result = 107;
        break;
      }
      else if(card_length != (PostcardBase::UINT32) nbytes)
      {
        result = 108;
        break;
      }

      new_buffer = InMemoryStream::createInstance();
      BREAK_IF_ZERO(109, new_buffer);

      BREAK_IF_NON_ZERO(110, new_buffer->setBufferOwnership((const void **)&buffer, card_length + sizeof(card_length)));

      int rc = callback->newMsg(new_buffer);
      new_buffer = 0;

      if(MessageQueueServiceCallback::RC_NO_ERROR_CONTINUE == rc)
      {
        // do nothing, just continue the loop
      }
      else if(MessageQueueServiceCallback::RC_NO_ERROR_STOP_LOOP == rc)
      {
        // break, without modifying result code (indicating no error)
        break;
      }
      else
      {
        // modify the result code, indicating an error, and then break
        result = 111;
        break;
      }

    } // while(true)

  } while (0);

  if(buffer)
  {
    delete[] buffer;
    buffer = 0;
  }
  if(new_buffer)
  {
    delete new_buffer;
    new_buffer = 0;
  }

  if(100 == result)
  {
    log_info(TAG, "run finished without error");
    result = 0;
  }
  // there should be no way we can reach here with result == 0
  // if there has been nothing, we'd be still in that infinite loop,
  // waiting for incoming messages
  else
  {
    log_error(TAG, "run failed %d", result);
  }
  return result;
}

MessageQueueClient * MessageQueueClient::createInstance()
{
  return new (std::nothrow) MessageQueueClientImpl();
}

} // namespace qc_loc_fw
