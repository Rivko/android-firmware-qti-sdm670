/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 XTRA-T WiFi mq-server

 GENERAL DESCRIPTION
 This components implements the per-clinet information

 Copyright (c) 2012,2014 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.
 =============================================================================*/
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <base_util/postcard.h>
#include "mq_client_info.h"

#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO_RC(ERR,RC,X) if(0!=(RC=(X))) {result = (ERR); break;}

static const char * TAG = "MessageQueueClientInfo";


MessageQueueClientInfo::MessageQueueClientInfo() :
    m_socket(-1), m_name(0), m_is_read_buffer_ready(false), m_read_buffer(0), m_read_buffer_extract(0)
{
  memset(&m_client_addr, 0, sizeof(m_client_addr));
  memset(&m_credentials, 0, sizeof(m_credentials));
}

MessageQueueClientInfo::~MessageQueueClientInfo()
{
  close();

  delete[] m_name;
  m_name = 0;

  LOOKUP_LIST::Iterator list_iter = mSocketNameAliasList.begin();
  while (list_iter != mSocketNameAliasList.end())
  {
    char * alias = (char *) (*list_iter);
    delete[] alias;

    ++list_iter;
  }

  mSocketNameAliasList.flush();
}

int MessageQueueClientInfo::setNameDup(const char * const name)
{
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, name);
    size_t length = strlen(name);
    BREAK_IF_ZERO(3, length);

    char * new_name = new char[length + 1];
    BREAK_IF_ZERO(2, new_name);
    memcpy(new_name, name, length + 1);

    if(m_name)
    {
      delete[] m_name;
    }
    m_name = new_name;

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "setNameDup failed %d", result);
  }
  return result;
}

const char * MessageQueueClientInfo::getName()
{
  return m_name;
}

int MessageQueueClientInfo::addAlias(const char * const name)
{
  int result = 1;

  do
  {
    BREAK_IF_ZERO(1, name);

    // check if the alias is already added
    BREAK_IF_NON_ZERO(0, hasAlias(name));

    size_t length = strlen(name);
    BREAK_IF_ZERO(2, length);

    char * new_name = new char[length + 1];
    BREAK_IF_ZERO(3, new_name);
    memcpy(new_name, name, length + 1);

    BREAK_IF_NON_ZERO(4, mSocketNameAliasList.add(new_name));
    result = 0;
  } while (0);

  if (0 != result)
  {
    log_error(TAG, "addAlias failed %d", result);
  }

  return result;
}

bool MessageQueueClientInfo::hasAlias(const char * const name) const
{
  int result = 1;
  bool aliasFound = false;

  do
  {
    BREAK_IF_ZERO(1, name);

    LOOKUP_LIST::ConstIterator list_iter = mSocketNameAliasList.begin();
    while (list_iter != mSocketNameAliasList.end())
    {
      char * alias = (char *) (*list_iter);
      if (0 == strncmp(alias, name, strlen(name)))
      {
        aliasFound = true;
        break;
      }

      ++list_iter;
    }

    result = 0;
  } while (0);

  if (0 != result)
  {
    log_error(TAG, "hasAlias failed %d", result);
  }

  return aliasFound;
}

void MessageQueueClientInfo::setSocket(const int s, const sockaddr_un addr, const ucred cred)
{
  m_socket = s;
  m_client_addr = addr;
  m_credentials = cred;
}

int MessageQueueClientInfo::getSocket()
{
  return m_socket;
}

InMemoryStream * MessageQueueClientInfo::getReadBufferIfReady()
{
  InMemoryStream * result = 0;
  if(m_is_read_buffer_ready)
  {
    // we transfer the ownership of the read buffer to 'result'!
    result = InMemoryStream::createInstance(m_read_buffer);
    // deleting the buffer won't release the memory buffer now
    delete m_read_buffer;
    m_read_buffer = 0;
    m_is_read_buffer_ready = false;

    if(0 == result)
    {
      log_error(TAG, "getReadBufferIfReady: message lost");
    }
  }
  return result;
}

int MessageQueueClientInfo::deliverMessage(InMemoryStream * new_buffer)
{
  if(new_buffer)
  {
    // reset get cursor to 0 before adding it into the queue
    new_buffer->setGetCursor(0);
    return m_write_queue.add(new_buffer);
  }
  return 1;
}

void MessageQueueClientInfo::close()
{
  if(m_socket >= 0)
  {
    ::close(m_socket);
    m_socket = -1;
  }

  delete m_read_buffer_extract;
  m_read_buffer_extract = 0;

  delete m_read_buffer;
  m_read_buffer = 0;

  m_is_read_buffer_ready = false;

  for (WriteQueue::Iterator it = m_write_queue.begin(); it != m_write_queue.end(); ++it)
  {
    InMemoryStream * info = *it;
    delete info;
  }

  m_write_queue.flush();
}

bool MessageQueueClientInfo::IsWriteQueueReady()
{
  return (m_write_queue.getSize() != 0);
}

int MessageQueueClientInfo::read()
{
  int result = 1;
  do
  {
    log_verbose(TAG, "%s: read", m_name);

    if(m_socket < 0)
    {
      result = 2;
      break;
    }

    if(0 == m_read_buffer)
    {
      m_read_buffer = OutMemoryStream::createInstance();
    }
    BREAK_IF_ZERO(3, m_read_buffer);
    if(0 == m_read_buffer_extract)
    {
      m_read_buffer_extract = InMemoryStream::createInstance();
    }
    BREAK_IF_ZERO(4, m_read_buffer_extract);

    // reset in-stream to cover the whole input buffer
    BREAK_IF_NON_ZERO(5, m_read_buffer_extract->setBufferNoDup(m_read_buffer->getBuffer(), m_read_buffer->getSize()));

    char temp[65536] = { 0 };
    size_t read_in_length = sizeof(temp);

    // size_t is 8-byte on 64-bit desktop linux
    PostcardBase::UINT32 card_length = 0;
    if(m_read_buffer->getSize() >= sizeof(card_length))
    {
      // we already have the card length available
      BREAK_IF_NON_ZERO(6, m_read_buffer_extract->extract(&card_length, sizeof(card_length)));
      BREAK_IF_ZERO(7, card_length);
      if(card_length + sizeof(card_length) > m_read_buffer_extract->getSize())
      {
        // we still need to read so many bytes from the socket
        read_in_length = card_length + sizeof(card_length) - m_read_buffer_extract->getSize();
      }
      else
      {
        // something is wrong, we shouldn't have anything more than the card itself
        result = 8;
        break;
      }
    }
    else
    {
      // we don't even have the card length yet, just try to read just that part in
      read_in_length = sizeof(card_length) - m_read_buffer->getSize();
    }

    if(read_in_length > sizeof(temp))
    {
      read_in_length = sizeof(temp);
    }

    ssize_t nbytes = recv(m_socket, temp, read_in_length, 0);

    if(-1 == nbytes)
    {
      log_error(TAG, "read error: %d, [%s]", errno, strerror(errno));
      result = 11;
      break;
    }
    else if(0 == nbytes)
    {
      // peer shutdown
      log_info(TAG, "client [%s] shutdown", m_name);
      close();
    }
    else
    {
      log_verbose(TAG, "just read %u bytes", (unsigned int) nbytes);

      BREAK_IF_NON_ZERO(9, m_read_buffer->append(temp, nbytes));
      if(card_length != 0)
      {
        log_verbose(TAG, "card length %u + %u bytes, current size %u bytes", (unsigned int) card_length,
            (unsigned int) sizeof(card_length), (unsigned int) m_read_buffer->getSize());
        if(card_length + sizeof(card_length) == m_read_buffer->getSize())
        {
          // we have one complete card now
          m_is_read_buffer_ready = true;
        }
      }
      else
      {
        // this is our first read, just check if the length is non zero
        memcpy(&card_length, temp, sizeof(card_length));

        BREAK_IF_ZERO(12, card_length);
      }
    }

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "read failed %d", result);
    close();
  }

  return result;
}

int MessageQueueClientInfo::write()
{
  int result = 1;
  do
  {
    log_verbose(TAG, "%s: write", m_name);

    if(m_socket < 0)
    {
      result = 2;
      break;
    }

    if(m_write_queue.getSize() == 0)
    {
      log_warning(TAG, "write: [%s] queue is empty", getName());
      result = 3;
      break;
    }

    InMemoryStream * out = 0;
    BREAK_IF_NON_ZERO(3, m_write_queue.top(&out));
    BREAK_IF_ZERO(4, out);

    size_t bytes_to_send = out->getSize() - out->getGetCursor();
    BREAK_IF_ZERO(5, bytes_to_send);

    // MSG_NOSIGNAL prevents generation of SIGPIPE in case the remote closes the socket
    ssize_t nbytes = ::send(m_socket, out->getBuffer() + out->getGetCursor(), bytes_to_send, MSG_NOSIGNAL);
    if(-1 == nbytes)
    {
      result = 6;
      break;
    }
    else
    {
      if(nbytes == (ssize_t) bytes_to_send)
      {
        // this has been written completely
        BREAK_IF_NON_ZERO(7, m_write_queue.pop(&out));
        // delete the memory buffer along with the in-memory-stream
        delete out;
        out = 0;
      }
      else
      {
        // move forward
        BREAK_IF_NON_ZERO(8, out->setGetCursor(out->getGetCursor() + nbytes));
      }

      const int bytes_left = (int) bytes_to_send - (int) nbytes;
      log_debug(TAG, "write to [%s], %d bytes finished, %d bytes left", getName(), (int) nbytes, bytes_left);
    }

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "write failed %d", result);
    close();
  }

  return result;

}

int MessageQueueClientInfo::error()
{
  log_verbose(TAG, "%s: error", m_name);
  return 0;
}


