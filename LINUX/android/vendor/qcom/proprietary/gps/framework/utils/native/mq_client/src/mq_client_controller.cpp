/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 mq client controller base implementation

 GENERAL DESCRIPTION
 This component implements the client connection functionality

 Copyright (c) 2012 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.

 Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <new>
#include <base_util/log.h>
#include <mq_client/mq_client.h>
#include <base_util/sync.h>
#include <base_util/array.h>
#include <base_util/postcard.h>
#include <mq_client/mq_client_controller.h>

#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO_RC(ERR,RC,X) if(0!=((RC)=(X))) {result = (ERR); break;}

namespace qc_loc_fw
{
const int MqClientControllerBase::MIN_TX_ID = 0;
const int MqClientControllerBase::INVALID_TX_ID = -1;
const int MAX_TX_ID = 10000000;

bool TimerCallbackEntry::operator ==(const TimerCallbackEntry & rhs) const
{
  // note only the 'module' and '*data' are compared
  // timeout value is not part of the equation
  if((module == rhs.module) && ((*data) == (*rhs.data)))
  {
    return true;
  }
  return false;
}

MqMsgWrapper::MqMsgWrapper(InPostcard * const in_card) :
        m_in_card(in_card)
{
}

MqMsgWrapper::~MqMsgWrapper()
{
  delete m_in_card;
  m_in_card = 0;
}

MqMsgWrapper * MqMsgWrapper::createInstance(InPostcard * const in_card)
{
  return new (std::nothrow) MqMsgWrapper(in_card);
}

class MqClientIpcReciver: public MessageQueueServiceCallback, public Runnable
{
private:
  BlockingQueue * m_pLocalMsgQueue;
  const char * TAG;
  MessageQueueClient * m_conn;
  const char * const m_name;
public:
  MqClientIpcReciver(BlockingQueue * const pLocalMsgQueue, MessageQueueClient * const conn,
      const char * const clientName, const char * const logTag) :
          m_pLocalMsgQueue(pLocalMsgQueue),
          TAG(logTag),
          m_conn(conn),
          m_name(clientName)
  {
    int result = 1;
    do
    {
      BREAK_IF_ZERO(2, m_pLocalMsgQueue);
      BREAK_IF_ZERO(3, TAG);
      BREAK_IF_ZERO(4, m_conn);
      BREAK_IF_ZERO(5, m_name);

      result = 0;
    } while (0);

    if(0 != TAG)
    {
      TAG = "MqClientIpcReciver";
    }

    if(0 != result)
    {
      log_error(TAG, "MqClientIpcReciver constructor failed %d", result);
    }
  }

  ~MqClientIpcReciver()
  {
  }

  virtual void run()
  {
    OutPostcard * reg_card = 0;
    int result = 1;
    do
    {
      reg_card = OutPostcard::createInstance();
      BREAK_IF_ZERO(4, reg_card);
      BREAK_IF_NON_ZERO(5, reg_card->init());
      BREAK_IF_NON_ZERO(6, reg_card->addString("TO", "SERVER"));
      BREAK_IF_NON_ZERO(7, reg_card->addString("FROM", m_name));
      BREAK_IF_NON_ZERO(8, reg_card->addString("REQ", "REGISTER"));
      BREAK_IF_NON_ZERO(9, reg_card->finalize());
      BREAK_IF_NON_ZERO(10, m_conn->send(reg_card->getEncodedBuffer()));
      delete reg_card;
      reg_card = 0;

      // should block here, calling newMsg on arrival of new messages
      BREAK_IF_NON_ZERO(11, m_conn->run_block(this));

      result = 0;
    } while (0);

    delete reg_card;
    reg_card = 0;

    if(0 == m_pLocalMsgQueue->close())
    {
      // the handler thread would be notified about this closure
      log_info(TAG, "incoming ipc queue closed");
    }
    else
    {
      // we're dead. this shall not happen
      log_error(TAG, "incoming ipc queue close failed");
    }

    if(0 != result)
    {
      log_error(TAG, "run error %d", result);
    }
  }

  virtual int newMsg(InMemoryStream *new_buffer)
  {
    MqMsgWrapper * incoming_msg = 0;
    InPostcard * card = 0;
    int result = 1;
    do
    {
      log_verbose(TAG, "new IPC message");
      BREAK_IF_ZERO(2, new_buffer);

      // after this line, 'new_buffer' is now part of 'card'
      // it will be deleted when we delete 'card'
      card = InPostcard::createInstance(new_buffer);
      BREAK_IF_ZERO(3, card);

      const char * req_origin = 0;
      const char * req = 0;
      const char * resp = 0;
      const char * info = 0;
      BREAK_IF_NON_ZERO(4, card->getString("FROM", &req_origin));

      // TX-ID is optional, but please do consider adding one!
      int tx_id = MqClientControllerBase::INVALID_TX_ID;
      BREAK_IF_NON_ZERO(5, card->getInt32Default("TX-ID", tx_id));

      if((0 != card->getString("REQ", &req)) && (0 != card->getString("RESP", &resp))
          && (0 != card->getString("INFO", &info)))
      {
        result = 6;
        break;
      }

      log_debug(TAG, "FROM: %s", req_origin);
      if(0 != req)
      {
        log_debug(TAG, "REQ:  %s", req);
      }
      else if(0 != resp)
      {
        log_debug(TAG, "RESP:  %s", resp);
      }
      else if(0 != info)
      {
        log_debug(TAG, "INFO:  %s", info);
      }

      if(MqClientControllerBase::INVALID_TX_ID != tx_id)
      {
        log_debug(TAG, "TX-ID:  %d", tx_id);
      }

      if(0 != req)
      {
        if(0 == strcmp(req, "KILL"))
        {
          log_info(TAG, "received KILL request, closing service");
          result = RC_NO_ERROR_STOP_LOOP;
        }
      }

      incoming_msg = MqMsgWrapper::createInstance(card);

      // insert 'card' into our local message queue
      if(0 == m_pLocalMsgQueue->push(incoming_msg))
      {
        // 'card' as well as the msg are now owned by the local message queue
        // we shall not touch anything related to them, including those
        // strings
        incoming_msg = 0;
        card = 0;
      }

      if(RC_NO_ERROR_STOP_LOOP == result)
      {
        break;
      }

      result = 0;
    } while (0);

    delete card;
    card = 0;
    delete incoming_msg;
    incoming_msg = 0;

    if((0 != result) && (RC_NO_ERROR_STOP_LOOP != result))
    {
      log_error(TAG, "newMsg error %d", result);
    }

    if(result < RC_NO_ERROR_STOP_LOOP)
    {
      // if the error is not too bad, just swallow it and continue
      result = RC_NO_ERROR_CONTINUE;
    }
    return result;
  }
};

TimerDataInterface::~TimerDataInterface()
{
  // dummy destructor
}

MqClientFunctionalModuleBase::~MqClientFunctionalModuleBase()
{
  // dummy destructor
}

MqClientControllerBase::MqClientControllerBase(const char * const tag, const char * const name,
    const char * const server_socket_name, const char * const config_name) :
        TAG(tag),
        m_name(name),
        m_server_socket_name(server_socket_name),
        m_config_name(config_name),
        m_last_tx_id(MIN_TX_ID),
        m_state(CS_DESTROYED),
        m_ipc_receiver_thread(0),
        m_ipc_handler_thread(0),
        m_local_msg_queue(0),
        m_conn(0),
        m_config(0)
{
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, TAG);
    BREAK_IF_ZERO(3, m_name);
    BREAK_IF_ZERO(4, m_server_socket_name);
    // it is okay to have no config file
    //BREAK_IF_ZERO(5, m_config_name);

    m_state = CS_NOT_READY_YET;
    result = 0;
  } while (0);

  if(0 == TAG)
  {
    TAG = "MqClientControllerBase";
  }

  if(0 != result)
  {
    log_error(TAG, "MqClientControllerBase failed %d", result);
  }
}

MqClientControllerBase::~MqClientControllerBase()
{
  // we do not notify peers at destruction, for they
  // probably have already been destructed!
  m_peers.flush();

  m_timers.flush();

  // delete thread object, which internally delete the ipc receiver object
  delete m_ipc_receiver_thread;
  // delete thread object, but do not delete the embedded runnable object, which is 'this'
  delete m_ipc_handler_thread;
  // delete the shared queue object
  delete m_local_msg_queue;
  delete m_conn;
  delete m_config;
}

int MqClientControllerBase::init()
{
  MqClientIpcReciver * receiver = 0;
  int result = 1;
  do
  {
    if(CS_NOT_READY_YET != m_state)
    {
      result = 2;
      break;
    }

    if(0 != m_config_name)
    {
      m_config = ConfigFile::createInstance(m_config_name);
      BREAK_IF_ZERO(3, m_config);
      // don't break out if config is not loaded.
      log_debug(TAG, "m_config loaded %d", m_config->loaded());
    }

    m_conn = MessageQueueClient::createInstance();
    BREAK_IF_ZERO(5, m_conn);
    m_conn->setServerNameDup(m_server_socket_name);
    bool isConnected = false;
    int connect_retry_count = 0;
    result = 100;
    do
    {
      if(0 == m_conn->connect())
      {
        isConnected = true;
      }
      else
      {
        ++connect_retry_count;
        if(connect_retry_count < 10)
        {
          log_warning(TAG, "retry count %d", connect_retry_count);
          sleep(5);
        }
        else
        {
          log_warning(TAG, "retry count exceeded limit, abort");
          result = 101;
          break;
        }
      }
    } while (!isConnected);
    if(result != 100)
    {
      break;
    }

    m_local_msg_queue = BlockingQueue::createInstance("LMSQ");
    BREAK_IF_ZERO(10, m_local_msg_queue);

    receiver = new MqClientIpcReciver(m_local_msg_queue, m_conn, m_name, TAG);
    BREAK_IF_ZERO(11, receiver);
    // note we give the ownership of 'receiver' to this thread, asking it to delete 'receiver'
    // when we destruct the thread
    m_ipc_receiver_thread = Thread::createInstance(TAG, receiver, true);
    BREAK_IF_ZERO(12, m_ipc_receiver_thread);
    receiver = 0;

    // note we do not give ownership of ourselves to this thread
    m_ipc_handler_thread = Thread::createInstance(TAG, this, false);
    BREAK_IF_ZERO(13, m_ipc_handler_thread);

    m_state = CS_READY;

    if(0 != _init())
    {
      m_state = CS_NOT_READY_YET;
      result = 20;
      break;
    }

    result = 0;
  } while (0);

  if(0 != receiver)
  {
    // we reach here only when there something goes wrong!
    delete receiver;
  }

  if(0 != result)
  {
    log_error(TAG, "init failed %d", result);
  }
  return result;
}

int MqClientControllerBase::sendIpcMessage(const MemoryStreamBase * const buffer)
{
  int result = 1;
  do
  {
    if(CS_READY != m_state)
    {
      result = 2;
      break;
    }
    BREAK_IF_ZERO(3, buffer);
    BREAK_IF_NON_ZERO(4, m_conn->send(buffer));

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "sendIpcMessage failed %d", result);
  }
  return result;
}

const char * MqClientControllerBase::getSelfIpcName() const
{
  return m_name;
}

int MqClientControllerBase::sendLocalMessage(qc_loc_fw::OutPostcard * const msg)
{
  qc_loc_fw::InPostcard * incard = 0;
  MqMsgWrapper * wrapper = 0;

  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, msg);
    incard = qc_loc_fw::InPostcard::createInstance(msg);
    wrapper = MqMsgWrapper::createInstance(incard);
    BREAK_IF_ZERO(3, wrapper);
    BREAK_IF_NON_ZERO(4, m_local_msg_queue->push(wrapper));
    // now wrapper object is pushed into the queue, so we should 'forget' about it
    incard = 0;
    wrapper = 0;

    result = 0;
  } while (0);

  delete incard;
  incard = 0;
  delete wrapper;
  wrapper = 0;

  if(0 != result)
  {
    log_error(TAG, "sendLocalMessage failed %d", result);
  }
  return result;
}

int MqClientControllerBase::getNextTxId()
{
  ++m_last_tx_id;

  if(m_last_tx_id > MAX_TX_ID)
  {
    m_last_tx_id = MIN_TX_ID;
    log_info(TAG, "getNextTxId rollback");
  }
  return m_last_tx_id;
}

int MqClientControllerBase::setLocalTimer(const unsigned int timeout_in_sec,
    MqClientFunctionalModuleBase * const module, const TimerDataInterface * const data)
{
  int result = 1;
  do
  {
    TimeDiff diff;
    BREAK_IF_NON_ZERO(1, diff.add_sec(timeout_in_sec));

    BREAK_IF_NON_ZERO(2, setLocalTimer(diff, module, data));

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "setLocalTimer rel failed %d", result);
  }
  return 0;
}

int MqClientControllerBase::setLocalTimer(const TimeDiff & timeout_diff, MqClientFunctionalModuleBase * const module,
    const TimerDataInterface * const data)
{
  int result = 1;
  do
  {
    if(CS_READY != m_state)
    {
      result = 2;
      break;
    }

    BREAK_IF_ZERO(3, module);

    TimerCallbackEntry callback;
    callback.module = module;
    callback.data = data;
    callback.absolute_timeout = Timestamp() + timeout_diff;

    if(!callback.absolute_timeout.is_valid_and_default())
    {
      result = 4;
      break;
    }

    BREAK_IF_NON_ZERO(5, m_timers.add(callback));

    log_verbose(TAG, "setLocalTimer : [%f] sec, %d timers in queue after insertion", timeout_diff.get_total_sec(),
        m_timers.getSize());

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "setLocalTimer abs failed %d", result);
  }
  return 0;
}

int MqClientControllerBase::removeLocalTimer(const MqClientFunctionalModuleBase * const module,
    TimerDataInterface * const data)
{
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, module);

    TimerCallbackEntry callback;
    // we have no intention to modify content of 'module', but we need a temp obj to remove the
    // call back entry
    callback.module = const_cast<MqClientFunctionalModuleBase *>(module);
    callback.data = data;

    // erase all instances that matches with the callback
    for(List<TimerCallbackEntry>::Iterator it = m_timers.begin(); it != m_timers.end();)
    {
      if(*it == callback)
      {
        it = m_timers.erase(it);
      }
      else
      {
        ++it;
      }
    }

    log_verbose(TAG, "removeLocalTimer : %d timers in queue after removal", m_timers.getSize());

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "removeLocalTimer failed %d", result);
  }
  return 0;
}

void MqClientControllerBase::_process(InPostcard * const dummy_msg)
{
  log_error(TAG, "_process without timestamp. inherit this one to handle incoming IPC messages");
  delete dummy_msg;
}

void MqClientControllerBase::_process(MqMsgWrapper * const msg_wrapper)
{
  // this is just a place holder for child classes who only have the older interface
  // for newer child classes, directly inherit this function and implement logic
  // just remember to delete the wrapper, which in turn deletes the postcard, at the end of
  // your implementation

  // note: for the traditional interface,_process(InPostcard*), the child is supposed to
  // delete the card!
  _process(msg_wrapper->m_in_card);

  // since the child should already deleted the card, let's not delete it again
  msg_wrapper->m_in_card = 0;
  delete msg_wrapper;
}

int MqClientControllerBase::addPeer(MqClientControllerBase * const peer)
{
  int result = 1;
  do
  {
    BREAK_IF_ZERO(2, peer);
    BREAK_IF_NON_ZERO(3, m_peers.add(peer));

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "addPeer failed %d", result);
  }
  return result;
}

int MqClientControllerBase::launch()
{
  int result = 1;
  do
  {
    if(CS_READY != m_state)
    {
      result = 2;
      break;
    }

    BREAK_IF_NON_ZERO(3, m_ipc_receiver_thread->launch());
    BREAK_IF_NON_ZERO(4, m_ipc_handler_thread->launch());

    // we could potentially move to a new state here, but please note
    // that we cannot safely inspect/modify the state variable any more,
    // since we have not just one, but two more threads running

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "launch failed %d", result);
  }
  return result;
}

int MqClientControllerBase::kill()
{
  // multiple access
  // called by peer threads and also main thread to kill this controller (both receiver and handler threads)
  if(0 != m_conn)
  {
    // shutting down the socket wakes up receiver thread, so it would close the local message queue, which
    // in turn notifies the handler thread
    (void) m_conn->shutdown();
  }
  return 0;
}

int MqClientControllerBase::join()
{
  // call by main thread while the threads are running
  if(0 != m_ipc_receiver_thread)
  {
    if(0 != m_ipc_receiver_thread->join())
    {
      log_warning(TAG, "IPC receiver thread not terminated properly");
    }
  }
  else
  {
    log_warning(TAG, "IPC receiver thread is null");
  }

  if(0 != m_ipc_handler_thread)
  {
    if(0 != m_ipc_handler_thread->join())
    {
      log_warning(TAG, "IPC handler thread not terminated properly");
    }
  }
  else
  {
    log_warning(TAG, "IPC handler thread is null");
  }
  // there is no meaningful error to return from here
  return 0;
}

int MqClientControllerBase::getNearestDeadline(bool & fgDeadlineSet, Timestamp & timeout)
{
  fgDeadlineSet = false;
  timeout.invalidate();

  int result = 1;
  do
  {
    //log_verbose(TAG, "getNearestDeadline : %d timers in queue", m_timers.getSize());

    if(CS_READY != m_state)
    {
      // do nothing if we're not in the right state
      result = 0;
      break;
    }

    int size = m_timers.getSize();
    if(size > 0)
    {
      // go through linearly
      // 1. fire all timers that have already timed out
      // 2. find the lowest timeout value

      List<TimerCallbackEntry>::Iterator it = m_timers.begin();
      while (it != m_timers.end())
      {
        // make a copy of *it, for we are going to delete *it BEFORE calling timer call back
        // this is to make sure we can insert the same timer registration INSIDE the call back routine
        const TimerCallbackEntry & timer = *it;
        Timestamp now;
        if(now >= timer.absolute_timeout)
        {
          // make a copy of *it, for we are going to delete *it BEFORE calling timer call back
          // this is to make sure we can insert the same timer registration INSIDE the call back routine
          const TimerCallbackEntry timer_copy = *it;

          m_timers.erase(it);

          // timeout !
          timer_copy.module->timerCallback(timer_copy.data);

          // restart the iteration, just in case another timer got inserted inside the timeout handler
          it = m_timers.begin();
        }
        else
        {
          // note: it's possible that all timers have timed out
          // and we never go into this section
          if(fgDeadlineSet)
          {
            if(timer.absolute_timeout < timeout)
            {
              timeout = timer.absolute_timeout;
            }
          }
          else
          {
            timeout = timer.absolute_timeout;
            fgDeadlineSet = true;
          }

          // step to the next one
          ++it;
        }
      }
    }
    else
    {
      // no timeout set
    }

    if(fgDeadlineSet && (!timeout.is_valid_and_default()))
    {
      // if deadline is set, it should be valid and set to default clock
      result = 10;
      break;
    }

    result = 0;
  } while (0);

  if(0 != result)
  {
    log_error(TAG, "getNearestDeadline failed %d", result);
  }
  return result;
}

void MqClientControllerBase::run()
{
  int result = 1;
  bool is_queue_closed = false;
  // enter event loop only after all initialization has been done
  if(CS_READY == m_state)
  {
    log_info(TAG, "Client controller entering event loop");
    while (true)
    {
      bool fgDeadlineSet = false;
      Timestamp timeout(false);
      if(CS_READY == m_state)
      {
        //log_verbose(TAG, "run: checking deadline. state %d", m_state);

        // check timers only if we're in the READY state
        if(0 != getNearestDeadline(fgDeadlineSet, timeout))
        {
          result = 2;
          break;
        }
      }
      else
      {
        log_verbose(TAG, "run: skipping deadline checking. state %d", m_state);
      }

      // diff is reset to 0, by default
      TimeDiff time_diff;
      if(fgDeadlineSet)
      {
        Timestamp now;
        if(now < timeout)
        {
          time_diff = timeout - now;
        }
        else
        {
          // diff is 0
        }

        if(time_diff.is_valid() && (time_diff.get_total_sec() >= 0))
        {
          log_verbose(TAG, "run: waiting for %f sec", time_diff.get_total_sec());
        }
        else
        {
          // something is wrong, diff should be calculated okay since we already checked
          // now is smaller than timeout
          result = 3;
          break;
        }
      }
      else
      {
        // set time difference to be invalid, indicating we're going to wait forever
        time_diff.reset(false);
        log_verbose(TAG, "run: waiting for msg");
      }

      // note that time_diff could be in any of these three states:
      // 1. invalid, we are going to wait forever
      // 2. valid but 0, we are going to pop the message without any wait
      // 3. valid and not 0, we are going to wait for incoming message for some time
      void * ptr = 0;
      if(0 == m_local_msg_queue->pop(&ptr, time_diff, &is_queue_closed))
      {
        if(0 != ptr)
        {
          // get a message
          MqMsgWrapper * incoming_msg = reinterpret_cast<MqMsgWrapper *>(ptr);
          // note that the child is supposed to handle and then delete the postcard
          // (incoming_msg->m_in_card) as well as the wrapper
          _process(incoming_msg);
          ptr = 0;
        }
        else
        {
          // timeout or queue closed?
          if(is_queue_closed)
          {
            // queue closed case
            result = 0;
            break;
          }
          else if(!time_diff.is_valid())
          {
            // we cannot be sure what caused this, but probably
            log_warning(TAG, "Client controller waken up without timeout or new message");
          }
          else
          {
            // timeout case
            // note we don't have to do anything here, for this is a loop.
            // simply wait for the next call to getNearestDeadline
          }
        }
      }
      else
      {
        result = 4;
        break;
      }
    }
    log_info(TAG, "Client controller leaving event loop");
  }
  else
  {
    // wrong state
    result = 10;
  }

  // STOP local message queue
  // further push would fail, and further pop would NOT block
  if(!is_queue_closed)
  {
    (void) m_local_msg_queue->close();
  }

  _shutdown();

  m_state = CS_DESTROYED;

  // notify all peers about our death
  for (List<MqClientControllerBase *>::Iterator it = m_peers.begin(); it != m_peers.end(); ++it)
  {
    MqClientControllerBase * const peer = *it;
    if(0 != peer)
    {
      // killing all peers (from within this thread context!)
      (void) peer->kill();
    }
    else
    {
      log_error(TAG, "peer is null");
    }
  }

  if(0 != result)
  {
    log_error(TAG, "run failed: %d", result);
  }
}

} // namespace qc_loc_fw

