/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/******************************************************************************
  @file    qmi_ril_file_observer.c

  DESCRIPTION
    monitor the state of file and inform the changes
    to the interested clients

******************************************************************************/

#define __STDC_FORMAT_MACROS 1

#define TAG "File_Observer"
extern "C"
{
    #include "qcrili.h"
    #include "qcril_other.h"
    #include "qcril_file_utils.h"
#include <comdef.h>
}

#define QMI_RIL_FILE_OBSERVER_DUMMY_FILE DATA_PARTITION_ROOT "radio/dummy_file"
#define QMI_RIL_FILE_OBSERVER_MAX_BUF_SIZE  (256)
#include <pthread.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <framework/Log.h>
#include "qmi_ril_file_observer.h"
#include "qcril_qmi_singleton_agent.h"

#ifdef QMI_RIL_UTF
extern "C" uint32 qcril_get_time_milliseconds();
#endif

#if defined(FEATURE_TARGET_GLIBC_x86) || defined(QMI_RIL_UTF)
   extern "C" size_t strlcat(char *, const char *, size_t);
   extern "C" size_t strlcpy(char *, const char *, size_t);
#endif

static int         inotify_instance_id;
const static int   MAX_REGISTERED_CLIENT = 5;
const static char *QMI_RIL_FILE_OBSERVER_THREAD_NAME = "file_observer";
const static int   QMI_RIL_INOTIFY_BUF_LEN = (MAX_REGISTERED_CLIENT * \
                                    (sizeof(struct inotify_event) + \
                                    QMI_RIL_FILE_OBSERVER_FILE_NAME_SIZE + 1));


class QmiRilFileObserver : public qcril_qmi_singleton_agent<QmiRilFileObserver>
{
public:

    class qmi_ril_file_observer_client_table
    {
        public:
            char *file_name;
            int   inotify_file_fd;
            qmi_ril_file_observer_state_change_handler hdlr;
    };

    int register_for_change_event(char *file_name,
                                  qmi_ril_file_observer_state_change_handler hdlr);
    int deregister_for_change_event(char *file_name,
                                qmi_ril_file_observer_state_change_handler hdlr);
    int init();

private:

    void thread_func();
    void inform_registered_client_on_change_event(int inotify_file_fd,
                                         qmi_ril_file_observer_change_event event);
    qmi_ril_file_observer_change_event convert_event_ril_event(uint32_t event_mask);
    void dummy_file_init();
    void dummy_file_update();
    static void dummy_file_change_event_hndlr(const qmi_ril_file_observer_change_event event);

    QmiRilFileObserver();
    QmiRilFileObserver(QmiRilFileObserver const&):
            qcril_qmi_singleton_agent<QmiRilFileObserver>(
                                QMI_RIL_FILE_OBSERVER_THREAD_NAME){};

private:

    /* currently the design expects a smaller number of clients.
     * If client list increases, array should be moved to
     * hash table */
    qmi_ril_file_observer_client_table client_table[MAX_REGISTERED_CLIENT];
    void *dummy_file_fd;
    pthread_mutex_t     file_observer_config_mutex;
    pthread_mutexattr_t file_observer_config_mutex_attr;

friend class qcril_qmi_singleton_agent<QmiRilFileObserver>;
};

//===========================================================================
// QmiRilFileObserver::register_for_state_change
//===========================================================================
int QmiRilFileObserver::register_for_change_event(char *file_name,
                                    qmi_ril_file_observer_state_change_handler hdlr)
{
    int result = -1;
    int inotify_file_fd;

    if (!hdlr || !file_name)
    {
        QCRIL_LOG_ERROR("Invalid Parameter");
    }
    else
    {
        pthread_mutex_lock(&file_observer_config_mutex);
        for (int i = 0; i < MAX_REGISTERED_CLIENT ; i++)
        {
            if (client_table[i].hdlr == NULL)
            {
                inotify_file_fd = inotify_add_watch(inotify_instance_id,
                                                    file_name, IN_ALL_EVENTS);
                if (inotify_file_fd != -1)
                {
                    client_table[i].hdlr             = hdlr;
                    client_table[i].file_name        = file_name;
                    client_table[i].inotify_file_fd  = inotify_file_fd;
                    result = 0;
                    dummy_file_update();
                    QCRIL_LOG_DEBUG("add watch for %s",
                                    file_name);

                }
                else
                {
                    QCRIL_LOG_DEBUG("Could not add watch for %s; error=%d",
                                    file_name, inotify_file_fd);
                }
                break;
            }
        }
        pthread_mutex_unlock(&file_observer_config_mutex);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
} // QmiRilFileObserver::register_for_state_change

//===========================================================================
// QmiRilFileObserver::deregister_for_state_change
//===========================================================================
int QmiRilFileObserver::deregister_for_change_event(char *file_name,
                                qmi_ril_file_observer_state_change_handler hdlr)
{
   int result = -1;

   if (!hdlr)
   {
      QCRIL_LOG_ERROR("Invalid Parameter");
   }
   else
   {
      pthread_mutex_lock(&file_observer_config_mutex);
      for (int i = 0; i < MAX_REGISTERED_CLIENT; i++)
      {
         if ((client_table[i].hdlr == hdlr) &&
                (client_table[i].file_name == file_name))
         {
             client_table[i].hdlr             = NULL;
             client_table[i].file_name        = NULL;
             inotify_rm_watch(inotify_instance_id,
                              client_table[i].inotify_file_fd);
             client_table[i].inotify_file_fd  = -1;
             result = 0;
             dummy_file_update();
             break;
         }
      }
      pthread_mutex_unlock(&file_observer_config_mutex);
   }

   QCRIL_LOG_FUNC_RETURN_WITH_RET(result);
   return result;
} // QmiRilFileObserver::deregister_for_state_change

//===========================================================================
// QmiRilFileObserver::QmiRilFileObserver
//===========================================================================
QmiRilFileObserver::QmiRilFileObserver() :
    qcril_qmi_singleton_agent<QmiRilFileObserver>(QMI_RIL_FILE_OBSERVER_THREAD_NAME)
{
   memset(client_table, 0, sizeof(client_table));
   pthread_mutexattr_init(&file_observer_config_mutex_attr);
   pthread_mutex_init(&file_observer_config_mutex,
                               &file_observer_config_mutex_attr);
} // QmiRilFileObserver::QmiRilFileObserver

//===========================================================================
// QmiRilFileObserver::dummy_file_init
//===========================================================================
void QmiRilFileObserver::dummy_file_init()
{
    dummy_file_fd = qcril_file_create_file((char *)QMI_RIL_FILE_OBSERVER_DUMMY_FILE);
    if (dummy_file_fd)
    {
        register_for_change_event((char *)QMI_RIL_FILE_OBSERVER_DUMMY_FILE,
                                  dummy_file_change_event_hndlr);
    }
}

//===========================================================================
// QmiRilFileObserver::dummy_file_change_event_hndlr
//===========================================================================
void QmiRilFileObserver::dummy_file_change_event_hndlr
(
    const qmi_ril_file_observer_change_event event
)
{
    QCRIL_LOG_DEBUG("change in file entry");
    QCRIL_NOTUSED(event);
    return;
}

//===========================================================================
// QmiRilFileObserver::dummy_file_update
//===========================================================================
void QmiRilFileObserver::dummy_file_update()
{
    static unsigned char counter = 1;
    qcril_file_write_bytes_from_front(dummy_file_fd, &counter, 1);
    counter++;
}

//===========================================================================
// QmiRilFileObserver::init
//===========================================================================
int QmiRilFileObserver::init()
{
    int ret = E_SUCCESS;

    if (is_inited())
    {
       QCRIL_LOG_ERROR("file monitor is already inited");
    }
    else
    {
        inotify_instance_id = inotify_init();
        if (inotify_instance_id == -1)
        {
            QCRIL_LOG_ERROR("inotify init failed");
            ret = E_FAILURE;
        }
        else
        {
            dummy_file_init();
            ret = qcril_qmi_singleton_agent<QmiRilFileObserver>::init();
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

//===========================================================================
// QmiRilFileObserver::convert_event_ril_event
//===========================================================================
qmi_ril_file_observer_change_event QmiRilFileObserver::convert_event_ril_event
(
    uint32_t event_mask
)
{
    qmi_ril_file_observer_change_event event = QMI_RIL_FILE_OBSERVER_CHANGE_INVALID;

    QCRIL_LOG_INFO("event_mask : %d", event_mask);
    if (event_mask & IN_CLOSE_NOWRITE)
    {
        event = QMI_RIL_FILE_OBSERVER_CHANGE_CLOSE_NO_WRITE;
    }
    else if (event_mask & IN_CLOSE_WRITE)
    {
        event = QMI_RIL_FILE_OBSERVER_CHANGE_CLOSE_WRITE;
    }
    else if (event_mask & IN_CREATE)
    {
        event = QMI_RIL_FILE_OBSERVER_CHANGE_CREATED;
    }
    else if (event_mask & IN_DELETE)
    {
        event = QMI_RIL_FILE_OBSERVER_CHANGE_DELETED;
    }
    else if (event_mask & IN_DELETE_SELF)
    {
        event = QMI_RIL_FILE_OBSERVER_CHANGE_DELETED;
    }
    else if (event_mask & IN_MODIFY)
    {
        event = QMI_RIL_FILE_OBSERVER_CHANGE_MODIFIED;
    }

    QCRIL_LOG_INFO("event : %d", event);
    return event;
}

//===========================================================================
// QmiRilFileObserver::thread_func
//===========================================================================
void QmiRilFileObserver::thread_func()
{
    QCRIL_LOG_FUNC_ENTRY();
    unsigned char rd_buf[QMI_RIL_INOTIFY_BUF_LEN];
    unsigned char *temp = NULL;
    int   buf_size = 0;
    struct inotify_event *event;
    qmi_ril_file_observer_change_event event_type;

    memset(rd_buf, 0, QMI_RIL_INOTIFY_BUF_LEN);

    while (TRUE)
    {
        buf_size = read(inotify_instance_id, rd_buf, sizeof(rd_buf));
        QCRIL_LOG_INFO("buf_size : %d", buf_size);

        temp = rd_buf;
        while(temp < rd_buf + buf_size)
        {
            event = (struct inotify_event *)temp;
            event_type = convert_event_ril_event(event->mask);
            if (event_type != QMI_RIL_FILE_OBSERVER_CHANGE_INVALID)
            {
                inform_registered_client_on_change_event(event->wd,
                                                         event_type);
            }
            temp = temp + (sizeof(struct inotify_event) + event->len);
        }
    }

    QCRIL_LOG_FUNC_RETURN();
} // QmiRilFileObserver::thread_func

//===========================================================================
// QmiRilFileObserver::inform_register_client_pil_change
//===========================================================================
void QmiRilFileObserver::inform_registered_client_on_change_event
(
    int inotify_file_fd,
    qmi_ril_file_observer_change_event event
)
{
    for (int i=0; i<MAX_REGISTERED_CLIENT ; i++)
    {
        if (client_table[i].inotify_file_fd == inotify_file_fd)
        {
            client_table[i].hdlr(event);
            break;
        }
    }

    return;

} // inform_registered_client_on_change_event

//===========================================================================
// qmi_ril_file_observer_init
//===========================================================================
int qmi_ril_file_observer_init()
{
    return QmiRilFileObserver::get_instance()->init();
} // qmi_ril_file_observer_init

//===========================================================================
// qmi_ril_file_observer_register_for_change_event
//===========================================================================
int qmi_ril_file_observer_register_for_change_event
(
    char *file_name,
    qmi_ril_file_observer_state_change_handler hdlr
)
{
    return QmiRilFileObserver::get_instance()->register_for_change_event(
                                                                    file_name,
                                                                    hdlr);
} // qmi_ril_file_observer_register_for_change_event

//===========================================================================
// qmi_ril_file_observer_deregister_for_change_event
//===========================================================================
int qmi_ril_file_observer_deregister_for_change_event
(
    char *file_name,
    qmi_ril_file_observer_state_change_handler hdlr
)
{
    return QmiRilFileObserver::get_instance()->deregister_for_change_event(
                                                                    file_name,
                                                                    hdlr);
} // qmi_ril_file_observer_deregister_for_change_event
