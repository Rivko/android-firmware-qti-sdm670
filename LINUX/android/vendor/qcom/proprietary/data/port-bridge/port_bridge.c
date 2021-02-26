/**
  @file
  port_bridge.c

  @brief
  Main port_bridge application which bridges communication
  between an external host port and a modem port.
*/

/*===========================================================================
  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#ifdef FEATURE_PB_LINUX_ANDROID
  #include <cutils/properties.h>
#endif

#include "ds_util.h"

#define MAX_RETRY_COUNT     (5)
#define MAX_MSG_SIZE        (16 * 1024)
#define GLINK_RX_INTENT_MIN (1)
#define LOG_FILE_PATH         "/data/vendor/port_bridge/log.txt"
#define USB_FILE_PATH         ("/dev/at_usb0")
#define MDM_FILE_PATH_GLINK   ("/dev/at_mdm0")
#define MDM_FILE_PATH_MHI_START   ("/dev/mhi_")
#define MDM_FILE_PATH_MHI_END     ("pipe_32")
#define MHI_ESOC_FILE_PATH        ("/sys/bus/esoc/devices/esoc0/esoc_link_info")
#define MHI_CTRL_POLLING_INTERVAL (500 * 1000)
#define MAX_PATH_LEN              (255)
#define MAX_SOC_LINK_INFO_LEN     (60)


/* target specific stuff */
struct pb_targetinfo
{
  const char *mdm_chardev;
  const char *usb_chardev;
  void * (*mdm_handler)(void *);
};

/* Seconds to sleep after an unsuccessful open before trying again
   If underlying driver supports blocking open (Ex. GLINK) then
   failure is rare. This is mainly for cases where blocking open
   is not supported */
#define OPEN_FAILURE_SLEEP_TIME (1)

/* Exponential back off sleep factor for write failure retries */
#define WRITE_FAILURE_SLEEP_TIME (1)

// These GLink #defines are not defined in any common header.
// MPROC has confirmed they will not change.
#define GLINK_PKT_IOCTL_MAGIC (0xC3)
#define GLINK_PKT_IOCTL_QUEUE_RX_INTENT \
  _IOW(GLINK_PKT_IOCTL_MAGIC, 0, unsigned int)

#define PORT_BRIDGE_LOG_ENABLED "persist.data.port_bridge.log"
#define PORT_BRIDGE_TRUE_STR    "true"
#define PORT_BRIDGE_FALSE_STR   "false"
boolean port_bridge_log_enabled = FALSE;

FILE* log_file = NULL;

// Log Info, Error, Result
#define LOGI(...)\
{\
  if(port_bridge_log_enabled) {\
    fprintf(log_file, "[thread %p] %30s,%3u (INFO):    ", (void*)pthread_self(), __FUNCTION__, __LINE__);\
    fprintf(log_file, __VA_ARGS__);\
    fflush(log_file);\
  } \
}
#define LOGE(...)\
{\
  if(port_bridge_log_enabled) {\
    fprintf(log_file, "[thread %p] %30s,%3u (ERROR):   ", (void*)pthread_self(), __FUNCTION__, __LINE__);\
    fprintf(log_file, __VA_ARGS__);\
    fflush(log_file);\
  } \
}
#define LOGR(errcode, ...)\
{\
  if(port_bridge_log_enabled) {\
    if(errcode < 0)\
    {\
      fprintf(log_file, "[thread %p] %30s,%3u (FAILURE - %s): ", (void*)pthread_self(), __FUNCTION__, __LINE__, strerror(errno));\
      fprintf(log_file, __VA_ARGS__);\
    }\
    else\
    {\
      fprintf(log_file, "[thread %p] %30s,%3u (SUCCESS): ", (void*)pthread_self(), __FUNCTION__, __LINE__);\
      fprintf(log_file, __VA_ARGS__);\
    }\
    fflush(log_file);\
  } \
}

#define PB_PTHREAD_MUTEX_LOCK( mutex, path )             \
  do {                                                   \
   LOGI("trying to lock [0x%p] %s\n", mutex, path)       \
   if(pthread_mutex_lock(mutex) < 0 )                    \
     LOGE("mutex lock failed [0x%p] %s\n",mutex, path);  \
   LOGI("locked [0x%p] %s\n", mutex, path);              \
  }while (0);

#define PB_PTHREAD_MUTEX_UNLOCK( mutex, path )           \
  do {                                                   \
   LOGI("trying to unlock [0x%p] %s\n", mutex,path)      \
   if(pthread_mutex_unlock(mutex) < 0 )                  \
     LOGE("mutex unlock failed [0x%p] %s\n",mutex, path);\
   LOGI("unlocked [0x%p] %s\n", mutex, path);            \
  }while(0);

const char* usb_file_path    = USB_FILE_PATH;
char mdm_file_path_mhi[MAX_PATH_LEN];

pthread_mutex_t usb_mutex   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mdm_mutex = PTHREAD_MUTEX_INITIALIZER;

int usb_fd = -1;
int mdm_fd = -1;

/* Design Note: Each direction of communication is handled by one thread.  This thread
   manages (opens & closes) the file descriptor of its 'from' (source), and does not manage
   (does not open/close) the file descriptor of its 'to' (destination). */
typedef struct
{
  char* from_file_path;
  char* to_file_path;
  int * from_fd;
  int * to_fd;
  pthread_mutex_t* from_mutex;
  pthread_mutex_t* to_mutex;
} thread_info_t;

/*===========================================================================
  FUNCTION  construct_signal_string
===========================================================================*/
/*!
@brief
  Utility function which formats a signal bitmask into human-redable format.

@param
  str   A char buffer to store the formatted string.
  size  Size of the buffer.  Limits output to this number of characters.
  bit   Bitmask to format.
*/
void construct_signal_string(char* str, size_t size, int bits)
{
  // Print any bits that aren't DTR/CD/RI/RTS as a mask
  unsigned int unaccounted_bits = bits & ~(TIOCM_DTR | TIOCM_CD | TIOCM_RI | TIOCM_RTS);

  // And print DTR/CD/RI/RTS in readable format
  snprintf(str, size - 1, "0x%x%s%s%s%s",
    unaccounted_bits,
    bits & TIOCM_DTR  ? " | DTR"  : "",
    bits & TIOCM_CD   ? " | CD"   : "",
    bits & TIOCM_RI   ? " | RI"   : "",
    bits & TIOCM_RTS  ? " | RTS"  : "");
}

/*===========================================================================
  FUNCTION  close_fd_mutexed
===========================================================================*/
/*!
@brief
  Wrapper function around close() that handles locking/unlocking a mutex associated
  the file descriptor.

@param
  file_path   Path of the file being closed.  Used for logging.
  fd          File descriptor to close.
  mutex       Mutex associated with fd.

@note
  Errors with mutex functions are only logged without exiting the program.  This
  is because a mutex lock or unlock failure is not necessarily fatal or even detrimental to
  the normal operation of port-bridge.
*/
void close_fd_mutexed(const char* file_path, volatile int* const fd, pthread_mutex_t* mutex)
{
  int ret;
  ret = pthread_mutex_lock(mutex);
  if(ret < 0)
  {
    LOGR(ret, "pthread_mutex_lock for '%s' returned %i.  Exiting!\n", file_path, ret);
    exit(1);
  }
  close(*fd);
  *fd = -1;
  ret = pthread_mutex_unlock(mutex);
  if(ret < 0)
  {
    LOGR(ret, "pthread_mutex_unlock for '%s' returned %i.  Exiting!\n", file_path, ret);
    exit(1);
  }
}

/*===========================================================================
  FUNCTION  write_to_device
===========================================================================*/
/*!
@brief
  Utility function to write data to the provided file

@param
  to_fd            File descriptor of the file where data needs
                   to be written
  to_filename      Name of the file where data needs to be written
  bytes_to_write   Number of bytes to be written

@return
  0  on success
  -1 on failure

@note
  In general, errors are logged but considered non-fatal.
*/
/*==========================================================================*/
int write_to_device
(
  int           to_fd,
  const char*   to_filename,
  int           bytes_to_write,
  char*         buffer
)
{
  int num_written           = 0;
  int bytes_remaining       = bytes_to_write;
  int retry_count           = 0;
  int ret = 0;

  /* In case we are writing a very large buffer to the file we need to ensure that
     the complete data was written correctly */
  while (bytes_remaining)
  {

#ifdef PORT_BRIDGE_TEST
    LOGI("usb[%s] to_path[%s]\n", USB_FILE_PATH, to_filename);
    if( !strncmp(USB_FILE_PATH, to_filename, sizeof(USB_FILE_PATH)) )
    {
      int r = rand();
      LOGI("rand [%d]\n",r);
      if( !(r%17) )
      {
        errno = 0;
        num_written = 0;
        LOGI("Setting num_written to [%d]\n", num_written);
      }
      else if( !(r%19) )
      {
        errno= EAGAIN;
        num_written = -1;
        LOGI("Setting num_written [%d] errno[%d]\n", num_written, errno);
      }
      else if( !(r%23))
      {
        errno = ESOCKTNOSUPPORT;
        num_written = -1;
        LOGI("Setting num_written [%d] errno[%d]\n", num_written, errno);
      }
      else
      {
        errno = 0;
        LOGI("[%s] writing to usb \n",to_filename);
        num_written = write(to_fd, buffer, bytes_remaining);
      }
    }
    else
#endif
    {
      num_written = write(to_fd, buffer, bytes_remaining);
    }

    if ( -1 == num_written || 0 == num_written )
    {
      /* We can retry writing if the error code is EAGAIN */
      if ( ( 0 == num_written || EAGAIN == errno ) &&
             MAX_RETRY_COUNT > retry_count )
      {
        LOGI("Failed to write to '%s' due to err [%s] or num_written [%d]\n",
              to_filename, strerror(errno), num_written);
        retry_count++;
        sleep( WRITE_FAILURE_SLEEP_TIME * retry_count );
        LOGI("retrying now...\n");
        continue;
      }
      else
      {
        /* We got a non-recoverable error */
        LOGE("Failed to write to '%s' due to err [%s]\n", to_filename, strerror(errno));
        ret = -1;
        break;
      }
    }

    LOGR(num_written, "write to '%s' returned %i\n", to_filename, num_written);
    bytes_remaining -= num_written;
    buffer += num_written;
    retry_count = 0;
  }

  return ret;
}

/*===========================================================================
  FUNCTION  modem_to_usb_bridge_mhi
===========================================================================*/
/*!
@brief
  Thread function responsible for handling data transfer and control signal
  updates from modem to USB over MHI transport

@param
  thread_info   Pointer to a thread_info_t which tells the function which
                direction of communication it is handling.

@note
  In general, errors are logged but considered non-fatal.
*/
/*==========================================================================*/
void* modem_to_usb_bridge_mhi(void* thread_info)
{
  thread_info_t tinfo           = *((thread_info_t*) thread_info);
  int num_read                  = 0;
  int dummy                     = 0;
  int new_bits                  = 0;
  int old_bits                  = 0;
  int ret                       = 0;
  int max_fd                    = 0;
  int temp_fd                   = -1;
  char buf[MAX_MSG_SIZE + 1];             /* +1 so we always have room for a trailing '\0' */
  char signal_str_buf[MAX_MSG_SIZE + 1];  /* Just need a large enough buffer to hold a signal
                                             printout for debug */
  struct timeval timeout;
  fd_set readfds;

  while (1)
  {
    /* ####### OPEN THE MHI FILE ####### */
    while (*tinfo.from_fd < 0)
    {
      LOGI("Attempting to open '%s'...\n", tinfo.from_file_path);
      temp_fd = open(tinfo.from_file_path, O_RDWR);
      LOGR(temp_fd, "open on '%s' returned %i.\n", tinfo.from_file_path, temp_fd);
      if (temp_fd < 0)
      {
        sleep(OPEN_FAILURE_SLEEP_TIME);
        continue;
      }
      PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
      *tinfo.from_fd = temp_fd;
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);
      max_fd = *tinfo.from_fd;
    }

    /* MHI driver has a different implementation of TIOCMGET ioctl
       The bits are being returned as part of the ioctl call
       and not filled in the address sent in the third argument
       The third argument has no significance */
    LOGI("Attempting TIOCMGET on '%s'...\n", tinfo.from_file_path);

    PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
    new_bits = ioctl(*tinfo.from_fd, TIOCMGET, &dummy);
    PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);

    if (new_bits != old_bits)
    {
      construct_signal_string(signal_str_buf, MAX_MSG_SIZE, new_bits);
      LOGI("TIOCMGET on '%s' returned signals=0x%x (%s)\n",
           tinfo.from_file_path, new_bits, signal_str_buf);
      /* Blindly pass on the entire signal mask; the USB and Modem sides
         should discard any bits they don't care about */
      LOGI("Attempting TIOCMSET on '%s', signals=0x%x (%s)...\n",
            tinfo.from_file_path, new_bits, signal_str_buf);

      PB_PTHREAD_MUTEX_LOCK(tinfo.to_mutex, tinfo.to_file_path);
      ret = ioctl(*tinfo.to_fd, TIOCMSET, &new_bits);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.to_mutex, tinfo.to_file_path);

      old_bits = new_bits;
      LOGR(ret, "TIOCMSET on '%s' returned %i, signals=0x%x (%s)\n",
           tinfo.from_file_path, ret, new_bits, signal_str_buf);
    }

    FD_ZERO(&readfds);
    FD_SET(*tinfo.from_fd, &readfds);

    /* Use a valid timeout value for select to return otherwise we might indefinitely block
       on select and miss control signal updates */
    timeout.tv_sec  = 0;
    timeout.tv_usec = MHI_CTRL_POLLING_INTERVAL;

    ret = select(max_fd + 1, &readfds, NULL, NULL, &timeout);

    if (ret < 0)
    {
      LOGE("select failed with error [%s]", strerror(errno));
      close_fd_mutexed(tinfo.from_file_path, tinfo.from_fd, tinfo.from_mutex);
    }
    else if (ret == 0)
    {
      continue;
    }

    /* Check if there is data to be read */
    if (FD_ISSET(*tinfo.from_fd, &readfds))
    {
      LOGI("Attempting to read from '%s'...\n", tinfo.from_file_path);

      PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
      num_read = read(*tinfo.from_fd, buf, MAX_MSG_SIZE);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);

      LOGR(num_read, "read from '%s' returned %i\n", tinfo.from_file_path, num_read);

      /* If read gave an error then mdm device is no longer available */
      if (num_read < 0)
      {
        close_fd_mutexed(tinfo.from_file_path, tinfo.from_fd, tinfo.from_mutex);
        continue;
      }

      if (num_read > 0)
      {
        buf[num_read] = '\0';
        LOGI("buf contains '%s'\n", buf);

        LOGI("Attempting to write to '%s'...\n", tinfo.to_file_path);
        PB_PTHREAD_MUTEX_LOCK(tinfo.to_mutex, tinfo.to_file_path);
        ret = write_to_device(*tinfo.to_fd, tinfo.to_file_path, num_read, buf);
        PB_PTHREAD_MUTEX_UNLOCK(tinfo.to_mutex, tinfo.to_file_path);
        if (ret < 0)
        {
          close_fd_mutexed(tinfo.to_file_path, tinfo.to_fd, tinfo.to_mutex);
          continue;
        }
      }
    }
  }

  return NULL;
}

/*===========================================================================
  FUNCTION  modem_to_usb_bridge_glink
===========================================================================*/
/*!
@brief
  Thread function responsible for handling data transfer and control signal
  updates from modem to USB over GLINK transport

@param
  thread_info   Pointer to a thread_info_t which tells the function which
                direction of communication it is handling.

@note
  In general, errors are logged but considered non-fatal.
*/
/*==========================================================================*/
void* modem_to_usb_bridge_glink(void* thread_info)
{
  thread_info_t tinfo   = *((thread_info_t*)thread_info);
  int num_read            = 0;
  int signals             = 0;
  int ret                 = 0;
  int temp_fd             = -1;
  unsigned int glink_rx_intent_count = 0;
  struct pollfd pollinfo;
  memset(&pollinfo, 0, sizeof(struct pollfd));

  char buf[MAX_MSG_SIZE + 1]; /* +1 so we always have room for a trailing '\0' */
  char signal_str_buf[MAX_MSG_SIZE + 1];  /* Just need a large enough buffer to hold a signal
                                             printout for debug */

  /* These are the events we care about.  They don't change. */
  pollinfo.events = POLLIN | POLLPRI | POLLERR | POLLHUP;

  while (1)
  {
    /*######## OPEN THE FILE ########*/
    if(*tinfo.from_fd < 0)
    {
      LOGI("Attempting to open '%s'...\n", tinfo.from_file_path);
      temp_fd = open(tinfo.from_file_path, O_RDWR); // Custom blocking open
      LOGR(temp_fd, "Blocking open on '%s' returned %i.\n", tinfo.from_file_path, temp_fd);
      if(temp_fd < 0)
      {
        sleep(OPEN_FAILURE_SLEEP_TIME);
        continue;
      }
      else
      {
        PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
        *tinfo.from_fd = temp_fd;
        PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);
        // Complete success
        pollinfo.fd = mdm_fd;
      }
    }

    //######## USE THE FILE ########
    /* Queue RX intent to allow G-link to receive data */
    while (glink_rx_intent_count < GLINK_RX_INTENT_MIN)
    {
      unsigned int rx_packet_max_size = MAX_MSG_SIZE;

      LOGI("Queuing RX Intent for '%s'\n", tinfo.from_file_path);
      PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
      ret = ioctl(*tinfo.from_fd, GLINK_PKT_IOCTL_QUEUE_RX_INTENT, &rx_packet_max_size);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);
      if (ret)
      {
        LOGE("Error queuing RX intent=%d\n", ret);
        exit(1);
      }
      glink_rx_intent_count++;
    }

    // At this point it's open, so poll for anything we need to do
    pollinfo.revents = 0;
    LOGI("Attempting to poll '%s'...\n", tinfo.from_file_path);
    ret = poll(&pollinfo, 1, -1); // -1 timeout specifies infinite block (default linux behavior)
    LOGR(ret, "poll on '%s' returned %i\n", tinfo.from_file_path, ret);

    if(pollinfo.revents & POLLERR)
    {
      LOGI("POLLERR is set.\n");
    }
    if(pollinfo.revents & POLLHUP)
    {
      LOGI("POLLHUP is set.\n");
    }

    // If the poll didn't work, or if the subsystem isn't available, start over.
    if((ret < 0) || (pollinfo.revents & (POLLERR | POLLHUP)))
    {
      LOGI("Closing: '%s'\n", tinfo.from_file_path);
      close_fd_mutexed(tinfo.from_file_path, tinfo.from_fd, tinfo.from_mutex);

      /*Reset RX_INTENT*/
      glink_rx_intent_count = 0;
      continue;
    }

    // POLLPRI is used to indicate a signal change.
    if(pollinfo.revents & POLLPRI)
    {
      LOGI("POLLPRI is set.\n");
      LOGI("Attempting TIOCMGET on '%s'...\n", tinfo.from_file_path);

      PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
      ret = ioctl(*tinfo.from_fd, TIOCMGET, &signals);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);

      construct_signal_string(signal_str_buf, MAX_MSG_SIZE, signals);
      LOGR(ret, "TIOCMGET on '%s' returned %i, signals=0x%x (%s)\n",
           tinfo.from_file_path, ret, signals, signal_str_buf);

      // Blindly pass on the entire signal mask; the USB and Modem sides should discard
      // any bits they don't care about
      LOGI("Attempting TIOCMSET on '%s', signals=0x%x (%s)...\n",
           tinfo.to_file_path, signals, signal_str_buf);

      PB_PTHREAD_MUTEX_LOCK(tinfo.to_mutex, tinfo.to_file_path);
      ret = ioctl(*tinfo.to_fd, TIOCMSET, &signals);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.to_mutex, tinfo.to_file_path);

      LOGR(ret, "TIOCMSET on '%s' returned %i, signals=0x%x (%s)\n",
           tinfo.to_file_path, ret, signals, signal_str_buf);
    }

    // Read data if we can
    if(pollinfo.revents & POLLIN)
    {
      LOGI("POLLIN is set.\n");
      glink_rx_intent_count--;
      LOGI("Marking rx intent as consumed\n");
      LOGI("Attempting to read from '%s'...\n", tinfo.from_file_path);

      PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
      num_read = read(*tinfo.from_fd, buf, MAX_MSG_SIZE);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);

      LOGR(num_read, "read from '%s' returned %i\n", tinfo.from_file_path, num_read);

      // If read gave us an error, then glink node isn't available anymore.
      if(num_read < 0)
      {
        close_fd_mutexed(tinfo.from_file_path, tinfo.from_fd, tinfo.from_mutex);
        continue;
      }

      // Write to the other end.  Log but otherwise ignore failures.
      if(num_read > 0)
      {
        buf[num_read] = '\0';
        LOGI("buf contains '%s'\n", buf);

        LOGI("Attempting to write to '%s'...\n", tinfo.to_file_path);
        PB_PTHREAD_MUTEX_LOCK(tinfo.to_mutex, tinfo.to_file_path);
        ret = write_to_device(*tinfo.to_fd, tinfo.to_file_path, num_read, buf);
        PB_PTHREAD_MUTEX_UNLOCK(tinfo.to_mutex, tinfo.to_file_path);

        if (ret < 0)
        {
          close_fd_mutexed(tinfo.to_file_path, tinfo.to_fd, tinfo.to_mutex);
          continue;
        }
      }
    }

    // Did something else happen?
    if(pollinfo.revents & ~(pollinfo.events))
    {
      LOGE("An unrecognized event happened!  pollinfo.revents=0x%0x\n", pollinfo.revents);
    }
  }

  return NULL;
}

/*===========================================================================
  FUNCTION  usb_to_modem_bridge
===========================================================================*/
/*!
@brief
  Thread function responsible for handling data transfer and control
  signal updates from USB to modem.

@param
  thread_info   Pointer to a thread_info_t which tells the function which
                direction of communication it is handling.

@note
  In general, errors are logged but considered non-fatal.
*/
/*==========================================================================*/
void* usb_to_modem_bridge(void* thread_info)
{
  thread_info_t tinfo           = *((thread_info_t*) thread_info);
  int num_read                  = 0;
  int signals                   = 0;
  int ret                       = 0;
  int temp_fd                   = -1;
  struct pollfd pollinfo;
  memset(&pollinfo, 0, sizeof(struct pollfd));
  char buf[MAX_MSG_SIZE + 1];             /* +1 so we always have room for a trailing '\0' */
  char signal_str_buf[MAX_MSG_SIZE + 1];  /* Just need a large enough buffer to hold a signal
                                             printout for debug */

  /* These are the events we care about.  They don't change */
  pollinfo.events = POLLIN | POLLPRI | POLLERR | POLLHUP;

  while (1)
  {
    /* ####### OPEN THE USB FILE ####### */
    if (*tinfo.from_fd < 0)
    {
      LOGI("Attempting to open '%s'...\n", tinfo.from_file_path);
      temp_fd = open(tinfo.from_file_path, O_RDWR);  /* Custom blocking open */
      LOGR(temp_fd, "Blocking open on '%s' returned %i.\n",
           tinfo.from_file_path, temp_fd);
      if (temp_fd < 0)
      {
        sleep(OPEN_FAILURE_SLEEP_TIME);
        continue;
      }
      else
      {
        PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
        *tinfo.from_fd = temp_fd;
        PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);
        /* Open completed successfully */
        pollinfo.fd = *tinfo.from_fd;
      }
    }

    /* ####### USE THE USB FILE ####### */

    /* At this point the file is open, so poll for anything we need to do */
    pollinfo.revents = 0;
    LOGI("Attempting to poll '%s'...\n", usb_file_path);
    ret = poll(&pollinfo, 1, -1);  /* -1 timeout translates to infinite block
                                      (standard linux behavior) */
    LOGR(ret, "poll on '%s' returned %i\n", tinfo.from_file_path, ret);

    if (pollinfo.revents & POLLERR)
    {
      LOGI("POLLERR is set.\n");
    }

    if(pollinfo.revents & POLLHUP)
    {
      LOGI("POLLHUP is set.\n");
    }

    /* If poll did not work as expected, or if the subsystem is not available, start over */
    if ((ret < 0) || (pollinfo.revents & (POLLERR | POLLHUP)))
    {
      LOGI("Sending DTR low signal to MODEM");

      signals = signals & ~TIOCM_DTR;

      construct_signal_string(signal_str_buf, MAX_MSG_SIZE, signals);

      /* Set DTR signal to off and pass it to MODEM  */
      LOGI("Attempting TIOCMSET on '%s', signals=0x%x (%s)...\n",
           tinfo.to_file_path, signals, signal_str_buf);

      PB_PTHREAD_MUTEX_LOCK(tinfo.to_mutex, tinfo.to_file_path);
      ret = ioctl(*tinfo.to_fd, TIOCMSET, &signals);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.to_mutex, tinfo.to_file_path);

      LOGR(ret, "TIOCMSET on '%s' returned %i, signals=0x%x (%s)\n",
           tinfo.to_file_path, ret, signals, signal_str_buf);

      LOGI("Closing: '%s'\n", tinfo.from_file_path);
      close_fd_mutexed(tinfo.from_file_path, tinfo.from_fd, tinfo.from_mutex);
      continue;
    }

    /* POLLPRI is used to indicate a signal change. */
    if(pollinfo.revents & POLLPRI)
    {
      LOGI("POLLPRI is set.\n");
      LOGI("Attempting TIOCMGET on '%s'...\n", tinfo.from_file_path);

      PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
      ret = ioctl(*tinfo.from_fd, TIOCMGET, &signals);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);

      construct_signal_string(signal_str_buf, MAX_MSG_SIZE, signals);
      LOGR(ret, "TIOCMGET on '%s' returned %i, signals=0x%x (%s)\n",
           tinfo.from_file_path, ret, signals, signal_str_buf);

      /* Blindly pass on the entire signal mask; the USB and Modem sides
         should discard any bits they don't care about */
      LOGI("Attempting TIOCMSET on '%s', signals=0x%x (%s)...\n",
           tinfo.to_file_path, signals, signal_str_buf);

      PB_PTHREAD_MUTEX_LOCK(tinfo.to_mutex, tinfo.to_file_path);
      ret = ioctl(*tinfo.to_fd, TIOCMSET, &signals);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.to_mutex, tinfo.to_file_path);

      LOGR(ret, "TIOCMSET on '%s' returned %i, signals=0x%x (%s)\n",
           tinfo.to_file_path, ret, signals, signal_str_buf);
    }

    /* Read data if we can */
    if (pollinfo.revents & POLLIN)
    {
      LOGI("POLLIN is set.\n");
      LOGI("Attempting to read from '%s'...\n", tinfo.from_file_path);
      PB_PTHREAD_MUTEX_LOCK(tinfo.from_mutex, tinfo.from_file_path);
      num_read = read(*tinfo.from_fd, buf, MAX_MSG_SIZE);
      PB_PTHREAD_MUTEX_UNLOCK(tinfo.from_mutex, tinfo.from_file_path);
      LOGR(num_read, "read from '%s' returned %i\n", tinfo.from_file_path, num_read);

      /* If read gave an error then usb device is no longer available */
      if (num_read < 0)
      {
        close_fd_mutexed(tinfo.from_file_path, tinfo.from_fd, tinfo.from_mutex);
        continue;
      }

      if (num_read > 0)
      {
        buf[num_read] = '\0';
        LOGI("buf contains '%s'\n", buf);

        LOGI("Attempting to write to '%s'...\n", tinfo.to_file_path);
        PB_PTHREAD_MUTEX_LOCK(tinfo.to_mutex, tinfo.to_file_path);
        ret = write_to_device(*tinfo.to_fd, tinfo.to_file_path, num_read, buf);
        PB_PTHREAD_MUTEX_UNLOCK(tinfo.to_mutex, tinfo.to_file_path);
        if (ret < 0)
        {
          close_fd_mutexed(tinfo.to_file_path, tinfo.to_fd, tinfo.to_mutex);
          continue;
        }
      }
    }

    /* Did something else happen? */
    if(pollinfo.revents & ~(pollinfo.events))
    {
      LOGE("An unrecognized event happened!  pollinfo.revents=0x%0x\n", pollinfo.revents);
    }
  }

  return NULL;
}

/*===========================================================================
  FUNCTION  pb_resolve_target
===========================================================================*/
/*!
@brief
  Queries dsutils for target type. Fills in targetinfo with appropriate
  info.

@note
  pb_resolve_info guarantees that all fields in *targetinfo are filled
  in before returning. Non-zero return code indicates failure and caller
  should assume that *target is invalid and may contain NULL or stale
  pointers.
*/
/*===========================================================================*/
static int pb_resolve_target(struct pb_targetinfo *targetinfo)
{
  ds_target_t target;
  int rc = -1;
  int ret = -1;
  char mhi_esoc_link_info[MAX_SOC_LINK_INFO_LEN];

  if (!targetinfo)
    return -1;

  target = ds_get_target();
  switch (target)
  {
    case DS_TARGET_FUSION_8084_9X45:
    case DS_TARGET_FUSION_8096_9X55:
    case DS_TARGET_FUSION_SDM845_SDX24:
    case DS_TARGET_FUSION_8998_SDX50_SA:

      do
      {
        FILE *fp = NULL;

        memset(mhi_esoc_link_info, 0, MAX_SOC_LINK_INFO_LEN * sizeof(char));
        memset(mdm_file_path_mhi, 0, MAX_PATH_LEN * sizeof(char));

        fp = fopen(MHI_ESOC_FILE_PATH,"r");
        if(NULL == fp)
        {
          LOGI("%s: error opening MHI ESOC file\n", __func__);

          /* Use Legacy MHI pipe id name*/
          ret = snprintf(mdm_file_path_mhi,
                         sizeof(mdm_file_path_mhi),
                         "%s%s",
                         MDM_FILE_PATH_MHI_START,
                         MDM_FILE_PATH_MHI_END);

          if(ret < 0)
          {
            return -1;
          }
        }
        else if (0 == fscanf(fp,"%s", mhi_esoc_link_info))
        {
          LOGI("%s: error reading mhi_esoc_link_info\n", __func__);
          fclose(fp);
          fp = NULL;
          return -1;
        }
        else
        {
          /* Update new MHI pipe id name*/
          ret = snprintf(mdm_file_path_mhi,
                         sizeof(mdm_file_path_mhi),
                         "%s%s_%s",
                         MDM_FILE_PATH_MHI_START,
                         mhi_esoc_link_info,
                         MDM_FILE_PATH_MHI_END);

          LOGI("%s: mdm_file_path_mhi=\'%s\'\n", __func__, mdm_file_path_mhi);
          fclose(fp);
          fp = NULL;

          if(ret < 0)
          {
            return -1;
          }
        }
      }while(0);

      targetinfo->mdm_chardev = mdm_file_path_mhi;
      targetinfo->usb_chardev = USB_FILE_PATH;
      targetinfo->mdm_handler = modem_to_usb_bridge_mhi;
      rc = 0;
      break;

    case DS_TARGET_MSM8996:
    case DS_TARGET_MSM8998:
    case DS_TARGET_MSMNILE:
    case DS_TARGET_SDM845:
    case DS_TARGET_STARLORD:
    case DS_TARGET_STARHAWK:
    case DS_TARGET_WARLOCK:
      targetinfo->mdm_chardev = MDM_FILE_PATH_GLINK;
      targetinfo->usb_chardev = USB_FILE_PATH;
      targetinfo->mdm_handler = modem_to_usb_bridge_glink;
      rc = 0;
      break;

    default:
      LOGE("Unsupported target: %s\n", ds_get_target_str(target));
      rc = -1;
      break;
  }

  if (rc == 0)
  {
    LOGI("Resolved target to: %s\n", ds_get_target_str(target));
    LOGI("Using USB end point: %s\n", targetinfo->usb_chardev);
    LOGI("Using MDM end point: %s\n", targetinfo->mdm_chardev);
    LOGI("Using mdm_handler: %p\n", targetinfo->mdm_handler);
  }
  return rc;
}


/*===========================================================================
  FUNCTION  pb_get_log_enabled
===========================================================================*/
/*!
@brief
  Read android porperty to enable or disable port_bridge logging

@note
  By default we disabel logging unless the android property is set to true.

*/
/*===========================================================================*/
void pb_get_log_enabled()
{
#ifdef FEATURE_PB_LINUX_ANDROID
  char args[PROPERTY_VALUE_MAX];
  int ret;

  port_bridge_log_enabled = FALSE;
  memset(args, 0, sizeof(args));
  ret = property_get(PORT_BRIDGE_LOG_ENABLED,
                     args,
                     PORT_BRIDGE_FALSE_STR);
  if ( ret < (int)sizeof(PORT_BRIDGE_FALSE_STR) &&
       !strncmp(PORT_BRIDGE_TRUE_STR, args, sizeof(PORT_BRIDGE_TRUE_STR)))
  {
    port_bridge_log_enabled = TRUE;
  }
#else
  /* Probably this is not applicable. Adding this for completeness sake */
  port_bridge_log_enabled = TRUE;
#endif
}

/*===========================================================================
  FUNCTION  main
===========================================================================*/
/*!
@brief
  Launches two threads, one for each direction of communication.

@note
  In general, errors are logged but considered non-fatal.
*/
/*===========================================================================*/
int main()
{
  struct pb_targetinfo targetinfo;

  pb_get_log_enabled();

#ifdef PORT_BRIDGE_TEST
  srand( rand() );
  port_bridge_log_enabled = TRUE;
#endif

  if( TRUE == port_bridge_log_enabled )
  {
    // Initialize the log - use stdout as fallback
    log_file = fopen(LOG_FILE_PATH, "w");
    if(!log_file)
    {
      log_file = stdout;
    }
    LOGI("log file is %p.\n", log_file);
  }
  pthread_t mdm_thread, usb_thread;
  thread_info_t mdm_info, usb_info;

  if (pb_resolve_target(&targetinfo) != 0)
  {
    LOGE("Failed to resolve target info. Dying...\n");
    if( TRUE == port_bridge_log_enabled )
    {
      fclose(log_file);
    }
    exit(1);
  }

  /* MDM -> USB info object */
  mdm_info.from_fd        = &mdm_fd;
  mdm_info.to_fd          = &usb_fd;
  mdm_info.to_file_path   = (char *) targetinfo.usb_chardev;
  mdm_info.from_file_path = (char *) targetinfo.mdm_chardev;
  mdm_info.to_mutex       = &usb_mutex;
  mdm_info.from_mutex     = &mdm_mutex;

  /* USB -> MDM info object */
  usb_info.from_fd        = &usb_fd;
  usb_info.to_fd          = &mdm_fd;
  usb_info.from_file_path = (char *) targetinfo.usb_chardev;
  usb_info.to_file_path   = (char *) targetinfo.mdm_chardev;
  usb_info.to_mutex       = &mdm_mutex;
  usb_info.from_mutex     = &usb_mutex;

  pthread_create(&mdm_thread, NULL, targetinfo.mdm_handler, &mdm_info);
  pthread_create(&usb_thread,  NULL, usb_to_modem_bridge, &usb_info);

  LOGI("Thread %p was created for direction %s -> %s\n",
       (void*)mdm_thread, mdm_info.from_file_path, mdm_info.to_file_path);
  LOGI("Thread %p was created for direction %s -> %s\n",
       (void*)usb_thread, usb_info.from_file_path, usb_info.to_file_path);

  pthread_join(mdm_thread,  NULL);
  pthread_join(usb_thread,   NULL);

  if( TRUE == port_bridge_log_enabled )
  {
    /* Cleanup added to be idiomatic; program should run indefinitely */
    fclose(log_file);
  }

  return 0;
}

