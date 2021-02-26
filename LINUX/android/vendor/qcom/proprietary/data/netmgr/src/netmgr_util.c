/******************************************************************************

                        N E T M G R _ U T I L . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_util.c
  @brief   Netowrk Manager Utility Functions Implementation File

  DESCRIPTION
  Implementation file for NetMgr utility functions.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  #include <hardware_legacy/power.h>
  #include <cutils/properties.h>
#endif

#include "netmgr_util.h"
#include "netmgr_config.h"
#include "ds_string.h"

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define NETMGR_UTIL_MAX_V4_ADDR_BUF_SIZE (19)
#define NETMGR_UTIL_MAX_V6_ADDR_BUF_SIZE (44)

#define NETMGR_UTIL_MAX_V4_MASK          (32)
#define NETMGR_UTIL_INET_PTON_SUCCESS    (1)

#define NETMGR_UTIL_IPV6_ADDR_SIZE_IN_BYTES  (16)

/* Control function entry/exit debug messages */
boolean function_debug = FALSE;

FILE*   logging_fd = NULL;

#if defined(NETMGR_LOG_TO_FILE)
boolean log_to_file = TRUE;
#else
boolean log_to_file = FALSE;
#endif

#ifndef PROPERTY_VALUE_MAX
  #define PROPERTY_VALUE_MAX 256
#endif

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  #define NETMGR_WAKELOCK_NAME             "netmgr_wl"
  #define NETMGR_UTIL_WAKELOCK_SLEEP_TIME  (1)
  #define NETMGR_UTIL_WAKELOCK_MAX_RETRIES (5)

  #define NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT         "persist.data.netmgr.wl.timeout"
  #define NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT_DEFAULT "2"
  #define NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT_SIZE    (2)

  static int wakelock_acquired = FALSE;
  static pthread_mutex_t  wakelock_mutex = PTHREAD_MUTEX_INITIALIZER;
  static timer_t netmgr_wakelock_timer;

  static int wl_ref_count = 0;
  static int wl_feature_disable = FALSE;
#endif

/*===========================================================================
  FUNCTION  netmgr_util_to_hex
===========================================================================*/
/*!
@brief
  Returns the character representation for the given hex number

@return
  Character corresponding to the given hex number

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL char netmgr_util_to_hex(unsigned int x)
{
  char c = '0';
  static char to_hex_tbl[] =
  {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
  };

  if (x < 16)
  {
    c = to_hex_tbl[x];
  }

  return c;
}

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
/*===========================================================================
  FUNCTION  netmgr_util_get_wakelock_timeout
===========================================================================*/
/*!
@brief
  Reads the persist property to determine the timeout value for holding
  wakelock in seconds

@return
  timeout - wakelock timeout in seconds

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
LOCAL int
netmgr_util_get_wakelock_timeout( void )
{
  static int ret = 0;
  char args[PROPERTY_VALUE_MAX];
  static int queried = FALSE;

  if (queried)
  {
    goto bail;
  }
  /* Retrieve value of NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT */
  memset(args, 0, sizeof(args));
  ret = property_get( NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT, args,
                      NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT_DEFAULT );

  if (ret > NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT_SIZE)
  {
    netmgr_log_err( "property [%s] has size [%d] that exceeds max [%d] default[%s]\n",
                    NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT,
                    ret,
                    NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT_SIZE,
                    NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT_DEFAULT );
    ret = atoi(NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT_DEFAULT);
  }
  else
  {
    netmgr_log_med( "property [%s] value[%s]",
                    NETMGR_UTIL_PERSIST_WAKELOCK_TIMEOUT, args );
    ret = atoi(args);
  }
  queried = TRUE;

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_util_delete_wakelock_timer
===========================================================================*/
/*!
@brief
  Delete and stop the wake lock timer.

@return
  - None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_util_delete_wakelock_timer( void )
{
  timer_delete(netmgr_wakelock_timer);
}

/*===========================================================================
  FUNCTION  netmgr_util_wakelock_timeout
===========================================================================*/
/*!
@brief
  Call back fuction that will be called when the timer expires.

@return
  - None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static void
netmgr_util_wakelock_timeout(union sigval sv)
{
  (void) sv;
  if (wl_ref_count > 0)
  {
    /* If wakelock timer expires and there ref count is still more than zero
       force release the wakelock */
    netmgr_log_med("%s(): Wakelock timer expired, releasing pending wakelocks...", __func__);
    wl_ref_count = 1;
    netmgr_util_wakelock_release();
  }
}

/*===========================================================================
  FUNCTION  netmgr_util_start_wakelock_timer
===========================================================================*/
/*!
@brief
  Start the wake lock timer. We will receive a call back when the timer
  expires using which we can release the wake lock.

@return
  - SUCCESS - if timer started successfully
  - FAILURE - if failed to start timer

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_start_wakelock_timer( void )
{
  int ret = NETMGR_FAILURE;
  struct itimerspec wakelock_timeout_sec;

  static int setup_complete = FALSE;
  static struct sigevent se;

  if (FALSE == setup_complete)
  {
    memset(&se, 0, sizeof(se));
    se.sigev_value.sival_int = 1;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_function = netmgr_util_wakelock_timeout;
    se.sigev_notify_attributes = NULL;
    setup_complete = TRUE;
  }

  if( timer_create(CLOCK_REALTIME, &se, &netmgr_wakelock_timer) != 0 )
  {
    netmgr_log_err("%s:Error creating timer\n", __func__);
    goto bail;
  }

  memset(&wakelock_timeout_sec, 0x0,sizeof(wakelock_timeout_sec));
  wakelock_timeout_sec.it_value.tv_sec = netmgr_util_get_wakelock_timeout();

  if( timer_settime(netmgr_wakelock_timer, 0, &wakelock_timeout_sec, NULL) != 0 )
  {
    netmgr_log_err("%s: settime failed\n",__func__ );
    goto bail;
  }

  ret = NETMGR_SUCCESS;
bail:
  return ret;
}
#endif

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

#ifdef NETMGR_TEST
/*===========================================================================
  FUNCTION  netmgr_debug_malloc
===========================================================================*/
/*!
@brief
  Debug wrapper for malloc

@return
  void* - Pointer to heap memeory allocation

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void * netmgr_debug_malloc( size_t memsize )
{
  void* ptr;
  //netmgr_log_med("netmgr_debug_malloc: memsize=%d\n", (int)memsize);
  ptr = malloc( memsize );
  netmgr_log_med("netmgr_debug_malloc: ptr=%p  memsize=%d\n", ptr, (int)memsize);
  return ptr;
}

/*===========================================================================
  FUNCTION  netmgr_debug_free
===========================================================================*/
/*!
@brief
  Debug wrapper for free

@return
  None

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_debug_free( void* ptr )
{
  netmgr_log_med("netmgr_debug_free: ptr=%p\n", ptr);
  free( ptr );
}
#endif /* NETMGR_TEST */


/*===========================================================================
  FUNCTION  netmgr_daemonize
===========================================================================*/
/*!
@brief
 Performs typical tasks required to run a program as a daemon process.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - Original program will exit and a child is forked which will continue
      execution as the daemon.
*/
/*=========================================================================*/
void netmgr_daemonize (void)
{
  pid_t pid;
  pid_t sid;

  /* Fork and exit parent process to ensure that process is not a process
  ** group leader.
  */
  if ((pid = fork()) > 0)
  {
    exit(0);
  }

  if (pid < 0)
  {
    /* Could not create child process. Exit */
    NETMGR_ABORT("netmgr_daemonize: Could not create child process\n");
    return;
  }

  /* Become session group leader to disassociate from controlling terminal */
  sid = setsid();

  if (sid < 0)
  {
    NETMGR_ABORT("netmgr_daemonize: setsid() failed\n");
    return;
  }

  /* Set file mode creation mask to 0, to avoid having permissions of created
  ** files being inadvertently changed.
  */
  (void)umask(0);

  /* Change directory to root */
  if ((chdir("/")) < 0)
  {
    NETMGR_ABORT("netmgr_daemonize: chdir to root failed\n");
    return;
  }

  /* Redirect stdin, stdout and stderr to /dev/null. If running as a daemon,
  ** it is assumed that logging will be to syslog.
  */
  if (freopen("/dev/null", "r", stdin) == NULL)
  {
    NETMGR_ABORT("netmgr_daemonize: freopen of stdin failed\n");
    return;
  }

  if (freopen("/dev/null", "w", stdout) == NULL)
  {
    NETMGR_ABORT("netmgr_daemonize: freopen of stdout failed\n");
    return;
  }

  if (freopen("/dev/null", "w", stderr) == NULL)
  {
    NETMGR_ABORT("netmgr_daemonize: freopen of stderr failed\n");
    return;
  }
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_util_convert_ip_addr_to_str
===========================================================================*/
/*!
@brief
  Converts the given Netmgr IP address type to a string

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_ip_addr_to_str
(
  netmgr_ip_address_t  *ip,
  unsigned int         prefix_len,
  char                 *buf,
  unsigned int         buf_len
)
{
  int ret = NETMGR_FAILURE;;

  if (!ip || !buf)
  {
    netmgr_log_err("invalid input");
    goto bail;
  }

  if (NETMGR_IPV4_ADDR == ip->type &&
      buf_len < NETMGR_UTIL_MAX_V4_ADDR_BUF_SIZE)
  {
    netmgr_log_err("insufficient storage [%u] for V4 address", buf_len);
    goto bail;
  }

  if (NETMGR_IPV6_ADDR == ip->type &&
      buf_len < NETMGR_UTIL_MAX_V6_ADDR_BUF_SIZE)
  {
    netmgr_log_err("insufficient storage [%u] for V6 address", buf_len);
    goto bail;
  }

  switch (ip->type)
  {
    case NETMGR_IPV4_ADDR:
      if (prefix_len)
      {
        snprintf(buf,
                 buf_len,
                 "%d.%d.%d.%d/%d",
                 (unsigned char)(ip->addr.v4),
                 (unsigned char)(ip->addr.v4 >> 8),
                 (unsigned char)(ip->addr.v4 >> 16),
                 (unsigned char)(ip->addr.v4 >> 24),
                 prefix_len);
      }
      else
      {
        snprintf(buf,
                 buf_len,
                 "%d.%d.%d.%d",
                 (unsigned char)(ip->addr.v4),
                 (unsigned char)(ip->addr.v4 >> 8),
                 (unsigned char)(ip->addr.v4 >> 16),
                 (unsigned char)(ip->addr.v4 >> 24));
      }
      break;

    case NETMGR_IPV6_ADDR:
      if (prefix_len)
      {
        snprintf(buf,
                 buf_len,
                 "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x/%d",
                 (unsigned char)(ip->addr.v6_addr8[0]),
                 (unsigned char)(ip->addr.v6_addr8[1]),
                 (unsigned char)(ip->addr.v6_addr8[2]),
                 (unsigned char)(ip->addr.v6_addr8[3]),
                 (unsigned char)(ip->addr.v6_addr8[4]),
                 (unsigned char)(ip->addr.v6_addr8[5]),
                 (unsigned char)(ip->addr.v6_addr8[6]),
                 (unsigned char)(ip->addr.v6_addr8[7]),
                 (unsigned char)(ip->addr.v6_addr8[8]),
                 (unsigned char)(ip->addr.v6_addr8[9]),
                 (unsigned char)(ip->addr.v6_addr8[10]),
                 (unsigned char)(ip->addr.v6_addr8[11]),
                 (unsigned char)(ip->addr.v6_addr8[12]),
                 (unsigned char)(ip->addr.v6_addr8[13]),
                 (unsigned char)(ip->addr.v6_addr8[14]),
                 (unsigned char)(ip->addr.v6_addr8[15]),
                 prefix_len);
      }
      else
      {
        snprintf(buf,
                 buf_len,
                 "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                 (unsigned char)(ip->addr.v6_addr8[0]),
                 (unsigned char)(ip->addr.v6_addr8[1]),
                 (unsigned char)(ip->addr.v6_addr8[2]),
                 (unsigned char)(ip->addr.v6_addr8[3]),
                 (unsigned char)(ip->addr.v6_addr8[4]),
                 (unsigned char)(ip->addr.v6_addr8[5]),
                 (unsigned char)(ip->addr.v6_addr8[6]),
                 (unsigned char)(ip->addr.v6_addr8[7]),
                 (unsigned char)(ip->addr.v6_addr8[8]),
                 (unsigned char)(ip->addr.v6_addr8[9]),
                 (unsigned char)(ip->addr.v6_addr8[10]),
                 (unsigned char)(ip->addr.v6_addr8[11]),
                 (unsigned char)(ip->addr.v6_addr8[12]),
                 (unsigned char)(ip->addr.v6_addr8[13]),
                 (unsigned char)(ip->addr.v6_addr8[14]),
                 (unsigned char)(ip->addr.v6_addr8[15]));
      }

      break;

    default:
      netmgr_log_err("netmgr_util_convert_ip_addr_to_str: unknown address type=%d",
                     ip->type);
      goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_util_convert_qmi_ip_addr_to_str
===========================================================================*/
/*!
@brief
  Converts the given QMI IP address type to a string

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_qmi_ip_addr_to_str
(
  wds_ip_addr_type_v01 *ip,
  char                 *buf,
  unsigned int         buf_len
)
{
  int ret = NETMGR_FAILURE;;

  if (!ip || !buf)
  {
    netmgr_log_err("invalid input");
    goto bail;
  }

  if (WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01 == ip->addr_family &&
      buf_len < NETMGR_UTIL_MAX_V4_ADDR_BUF_SIZE)
  {
    netmgr_log_err("insufficient storage [%u] for V4 address", buf_len);
    goto bail;
  }

  if (WDS_DATA_CALL_ADDR_FAMILY_IPV6_V01 == ip->addr_family &&
      buf_len < NETMGR_UTIL_MAX_V6_ADDR_BUF_SIZE)
  {
    netmgr_log_err("insufficient storage [%u] for V6 address", buf_len);
    goto bail;
  }

  switch (ip->addr_family)
  {
    case WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01:
      snprintf(buf,
               buf_len,
               "%d.%d.%d.%d",
               ip->ip_addr[3],
               ip->ip_addr[2],
               ip->ip_addr[1],
               ip->ip_addr[0]);
      break;

    case WDS_DATA_CALL_ADDR_FAMILY_IPV6_V01:
      snprintf(buf,
               buf_len,
               "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
               ip->ip_addr[0],
               ip->ip_addr[1],
               ip->ip_addr[2],
               ip->ip_addr[3],
               ip->ip_addr[4],
               ip->ip_addr[5],
               ip->ip_addr[6],
               ip->ip_addr[7],
               ip->ip_addr[8],
               ip->ip_addr[9],
               ip->ip_addr[10],
               ip->ip_addr[11],
               ip->ip_addr[12],
               ip->ip_addr[13],
               ip->ip_addr[14],
               ip->ip_addr[15]);
      break;

    default:
      netmgr_log_err("netmgr_util_convert_qmi_ip_addr_to_str: unknown address type=%d",
                     ip->addr_family);
      goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_util_convert_qmi_ip_addr_range_to_str_range
===========================================================================*/
/*!
@brief
  Given QMI address type range converts to a string with address mask.

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_qmi_ip_addr_range_to_str_range
(
  wds_ip_addr_type_v01  *start_ip,
  wds_ip_addr_type_v01  *end_ip,
  char                  *buf,
  unsigned int          buf_len
)
{
  int      ret = NETMGR_FAILURE, reti = NETMGR_SUCCESS;
  int      mask = 0, i = 0, count = 0, j = 0;
  char     start_addr[NETMGR_MAX_STR_LENGTH] = "";
  char     end_addr[NETMGR_MAX_STR_LENGTH] = "";
  struct   in6_addr start_v6_addr, end_v6_addr;
  uint8_t  xor_val;
  uint32_t xor_val_v4;

  NETMGR_LOG_FUNC_ENTRY;

  if (!buf || (!start_ip && !end_ip))
  {
    netmgr_log_err("%s(): Invalid params!\n", __func__);
    goto bail;
  }

  if (NULL != start_ip)
  {
    /* Check whether the buffer length is sufficient to hold a valid
     * address */
    if (WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01 == start_ip->addr_family &&
          buf_len < NETMGR_UTIL_MAX_V4_ADDR_BUF_SIZE)
    {
      netmgr_log_err("%s(): insufficient storage [%u] for V4 address", __func__, buf_len);
      goto bail;
    }

    if (WDS_DATA_CALL_ADDR_FAMILY_IPV6_V01 == start_ip->addr_family &&
          buf_len < NETMGR_UTIL_MAX_V6_ADDR_BUF_SIZE)
    {
      netmgr_log_err("%s(): insufficient storage [%u] for V6 address", __func__, buf_len);
      goto bail;
    }
  }
  else if (NULL != end_ip)
  {
    /* If start_ip was NULL we need to check for buffer size validity
     * with end_ip */
    if (WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01 == end_ip->addr_family &&
          buf_len < NETMGR_UTIL_MAX_V4_ADDR_BUF_SIZE)
    {
      netmgr_log_err("%s(): insufficient storage [%u] for V4 address", __func__, buf_len);
      goto bail;
    }

    if (WDS_DATA_CALL_ADDR_FAMILY_IPV6_V01 == end_ip->addr_family &&
          buf_len < NETMGR_UTIL_MAX_V6_ADDR_BUF_SIZE)
    {
      netmgr_log_err("%s(): insufficient storage [%u] for V6 address", __func__, buf_len);
      goto bail;
    }
  }

  /* If only one of the start or end addresses is valid, return the
   * same */
  if (NULL == start_ip)
  {
    /* Convert the end_ip address to string and return */
    if ( NETMGR_SUCCESS
           != netmgr_util_convert_qmi_ip_addr_to_str(end_ip,
                                                     buf,
                                                     buf_len) )
    {
      /* Failed to convert end address to string */
      netmgr_log_err("%s(): Error converting address to string!\n", __func__);
      goto bail;
    }
  }
  else if (NULL == end_ip)
  {
    /* Convert the start address to string and return */
    if ( NETMGR_SUCCESS
           != netmgr_util_convert_qmi_ip_addr_to_str(start_ip,
                                                     buf,
                                                     buf_len) )

    {
      /* Failed to convert start address to string */
      netmgr_log_err("%s(): Error converting address to string!\n", __func__);
      goto bail;
    }
  }
  else
  {
    /* Get the string representations of the start and
     * end addresses */
    if ( NETMGR_SUCCESS
          != netmgr_util_convert_qmi_ip_addr_to_str(start_ip,
                                                    start_addr,
                                                    sizeof(start_addr)) )
    {
      netmgr_log_err("%s(): Failed to convert start address to string!\n",
                     __func__);
      goto bail;
    }

    netmgr_log_low("%s(): Start address: %s", __func__, start_addr);

    if ( NETMGR_SUCCESS
          != netmgr_util_convert_qmi_ip_addr_to_str(end_ip,
                                                    end_addr,
                                                    sizeof(end_addr)) )
    {
      netmgr_log_err("%s(): Failed to convert end address to string!\n",
                     __func__);
      goto bail;
    }

    netmgr_log_low("%s(): End address: %s", __func__, end_addr);

    switch ( start_ip->addr_family )
    {
      case WDS_DATA_CALL_ADDR_FAMILY_IPV4_V01:
      {
        /* If the start and end addresses are the same, return the
         * string representation of any one of them */
        if ( start_ip->ip_addr_len == end_ip->ip_addr_len &&
             !memcmp(start_ip->ip_addr, end_ip->ip_addr, start_ip->ip_addr_len) )
        {
          netmgr_log_low("%s(): Start and end addresses are same, returning start address!\n",
                         __func__);
          strlcpy(buf, start_addr, buf_len);
          reti = NETMGR_SUCCESS;
          break;
        }
        else
        {
          uint32_t start_ipv4_addr, end_ipv4_addr;

          start_ipv4_addr = start_ip->ip_addr[3] << 24 |
                            start_ip->ip_addr[2] << 16 |
                            start_ip->ip_addr[1] << 8  |
                            start_ip->ip_addr[0];

          end_ipv4_addr = end_ip->ip_addr[3] << 24 |
                          end_ip->ip_addr[2] << 16 |
                          end_ip->ip_addr[1] << 8  |
                          end_ip->ip_addr[0];

          /* To get the address range we need to:
           * 1. XOR the start and end addresses
           * 2. Count the number of 0s in the result till we encounter
           *    the first mismatching bit */
          xor_val_v4 = (start_ipv4_addr) ^ (end_ipv4_addr);

          for (j = 0; j < NETMGR_UTIL_MAX_V4_MASK; j++)
          {
            /* Left shift the bits till we encounter a '1' which
             * will be the position of the first mismatching bit
             * in the xor result. We will increment the mask till
             * this point */
            if ((uint32_t) 0x80000000 == (xor_val_v4 & (uint32_t) 0x80000000))
            {
              break;
            }

            xor_val_v4 = (xor_val_v4) << 1;
          }

          /* The mask value will be the number of 0's in the
           * XOR result till the first mismatch is encountered
           * which will be the value of 'j' */
          mask = j;

          /* Append the mask to the start address */
          snprintf(buf,
                   buf_len,
                   "%s/%d",
                   start_addr,
                   mask);
        }
      }
      break;

      case WDS_DATA_CALL_ADDR_FAMILY_IPV6_V01:
      {
        /* Check if the start and end addresses are the same. V6 addresses are stored
         * as strings so we can do string comparison */
        if (!strcmp(start_addr, end_addr))
        {
          netmgr_log_low("%s(): Start and end addresses are same, returning start address!\n",
                         __func__);
          strlcpy(buf, start_addr, buf_len);
          reti = NETMGR_SUCCESS;
          break;
        }
        else
        {
          /* Convert the V6 address into in6_addr type for easier calculation of
           * prefixes */
          if ( NETMGR_UTIL_INET_PTON_SUCCESS !=
                 inet_pton(AF_INET6, start_addr, &start_v6_addr) )
          {
            /* inet_pton returns '1' if it was successfully able to convert
             * the address */
            netmgr_log_err("%s(): Failed to convert start V6 address string to in6_addr type!",
                           __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          if ( NETMGR_UTIL_INET_PTON_SUCCESS !=
                 inet_pton(AF_INET6, end_addr, &end_v6_addr) )
          {
            netmgr_log_err("%s(): Failed to convert end V6 address string to in6_addr type!",
                           __func__);
            reti = NETMGR_FAILURE;
            break;
          }

          /* Calculate address mask and append to start address */
          for (i = 0; i < NETMGR_UTIL_IPV6_ADDR_SIZE_IN_BYTES; i++)
          {
            /* Do byte wise comparison and stop at the first mismatching byte */
            if (start_v6_addr.s6_addr[i] != end_v6_addr.s6_addr[i])
            {
              /* Now bitwise XOR the bytes to find the exact position
               * of the mismatching bit */
              xor_val = start_v6_addr.s6_addr[i] ^ end_v6_addr.s6_addr[i];

              for (j = 0; j < 8; j++)
              {
                /* Left shift till we encounter a '1' which
                 * will be the position of the mismatching bit in
                 * the byte */
                if (0x80 == (xor_val & 0x80))
                {
                  break;
                }

                xor_val = (xor_val) << 1;
              }

              /* Update the mask by adding the number of 0's in the byte
               * till the point we hit the mismatching bit */
              mask += j;
            }
            else
            {
              /* The bytes match so increment the mask by 8 */
              mask += 8;
              continue;
            }

            /* Clear out the remaining bits in the mismatching byte
             * from j'th position */
            start_v6_addr.s6_addr[i] = ( ( start_v6_addr.s6_addr[i] >> (7 - j) ) << (7 - j) );

            /* Reset all the remaining bytes from (i+1) onwards */
            i++;
            while (i < NETMGR_UTIL_IPV6_ADDR_SIZE_IN_BYTES)
            {
              start_v6_addr.s6_addr[i] = 0;
              i++;
            }
          }

          /* Get back the address in string format after processing the mask */
          memset(start_addr, 0, sizeof(start_addr));
          if ( NULL ==
                inet_ntop(AF_INET6, &start_v6_addr, start_addr, sizeof(start_addr)) )
          {
            /* If inet_ntop fails to get the string representation for the V6 address
             * then we will also return failure */
            netmgr_log_err("%s(): Failed to convert V6 address to string! [%s]",
                           __func__, strerror(errno));
            reti = NETMGR_FAILURE;
            break;
          }

          snprintf(buf,
                   buf_len,
                   "%s/%d",
                   start_addr,
                   mask);
        }
      }
      break;

      default:
        {
          netmgr_log_err("%s(): Unknown address type=%d!\n",
                         __func__, start_ip->addr_family);
          reti = NETMGR_FAILURE;
        }
        break;
    }
  }

  if (NETMGR_FAILURE == reti)
  {
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_ENTRY;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_util_convert_qmi_ipsec_key_to_str
===========================================================================*/
/*!
@brief
  Converts the given IPSec hash/crypto key to a string

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_convert_qmi_ipsec_key_to_str
(
  uint8_t       *key,
  uint32_t      key_len,
  char          *buf,
  unsigned int  buf_len
)
{
  int ret = NETMGR_FAILURE;;
  uint32_t i = 0;

  if (!key || !buf)
  {
    netmgr_log_err("invalid input");
    goto bail;
  }

  /* Need to prepend 0x and each digit in key translates to two hex characters */
  if (buf_len < (unsigned int)(key_len * 2 + 3))
  {
    netmgr_log_err("insufficient storage [%u] for key size=%d", buf_len, key_len);
    goto bail;
  }

  *buf++ = '0';
  *buf++ = 'x';

  for (i = 0; i < key_len; ++i)
  {
    *buf++ = netmgr_util_to_hex((key[i] >> 4) & 0x0F);
    *buf++ = netmgr_util_to_hex(key[i] & 0x0F);
  }

  *buf = '\0';

  ret = NETMGR_SUCCESS;

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_util_get_ipsec_proto_str
===========================================================================*/
/*!
@brief
  Returns the string representation of the given IPSec protocol

@return
  Pointer to the IPSec proto string on success or NULL on error

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
const char *netmgr_util_get_ipsec_proto_str
(
  wds_ipsec_sa_protocol_enum_v01  proto
)
{
  const char *proto_str = NULL;

  switch (proto)
  {
    case WDS_IPSEC_SA_PROTOCOL_IPSEC_AH_V01:
      proto_str = "ah";
      break;

    case WDS_IPSEC_SA_PROTOCOL_IPSEC_ESP_V01:
      proto_str = "esp";
      break;

    default:
      break;
  }

  return proto_str;
}

/*===========================================================================
  FUNCTION  netmgr_util_get_ipsec_algo_str
===========================================================================*/
/*!
@brief
  Returns the string representation of the given IPSec algorithm

@return
  Pointer to the IPSec algo string on success or NULL on error

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
const char *netmgr_util_get_ipsec_algo_str
(
  netmgr_ipsec_algo_type                       type,
  wds_get_ipsec_static_sa_config_resp_msg_v01  *sa_config
)
{
  const char *algo_str = NULL;
  wds_ipsec_crypto_algo_enum_v01 algo;

  NETMGR_LOG_FUNC_ENTRY;

  switch (type)
  {
    case NETMGR_IPSEC_ALGO_HASH:
    {
      algo = sa_config->hash_algo;

      if (WDS_IPSEC_CRYPTO_ALGO_AES128_V01 == algo)
      {
        algo_str = "xcbc(aes)";
      }
      else if (WDS_IPSEC_CRYPTO_ALGO_SHA_V01 == algo)
      {
        algo_str = "hmac(sha1)";
      }
      else if (WDS_IPSEC_CRYPTO_ALGO_MD5_V01 == algo)
      {
        algo_str = "hmac(md5)";
      }
      else if (WDS_IPSEC_CRYPTO_ALGO_SHA256_V01 == algo)
      {
        algo_str = "hmac(sha256)";
      }
    }
      break;

    case NETMGR_IPSEC_ALGO_CRYPTO:
    {
      algo = sa_config->crypto_algo;

      if (WDS_IPSEC_CRYPTO_ALGO_AES128_V01 == algo ||
          WDS_IPSEC_CRYPTO_ALGO_AES256_V01 == algo)
      {
        if (!(sa_config->aes_mode_valid) ||
             (WDS_IPSEC_AES_MODE_CBC_V01 == sa_config->aes_mode))
        {
          algo_str = "cbc(aes)";
        }
        else if (WDS_IPSEC_AES_MODE_CTR_V01 == sa_config->aes_mode)
        {
          algo_str = "rfc3686(ctr(aes))";
        }
        else
        {
          netmgr_log_err("%s(): invalid AES mode=%d received",
                         __func__, sa_config->aes_mode);
          break;
        }
      }
      else if (WDS_IPSEC_CRYPTO_ALGO_DES_V01 == algo)
      {
        algo_str = "cbc(des)";
      }
      else if (WDS_IPSEC_CRYPTO_ALGO_3DES_V01 == algo)
      {
        algo_str = "cbc(des3_ede)";
      }
    }
    break;

    default:
      break;
  }

  NETMGR_LOG_FUNC_EXIT;
  return algo_str;
}

/*===========================================================================
  FUNCTION  netmgr_util_get_ipsec_mode_str
===========================================================================*/
/*!
@brief
  Returns the string representation of the given IPSec encapsulation mode

@return
  Pointer to the IPSec mode string on success or NULL on error

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
const char *netmgr_util_get_ipsec_mode_str
(
  wds_ipsec_sa_encapsulate_enum_v01  mode
)
{
  const char *mode_str = NULL;

  switch (mode)
  {
    case WDS_IPSEC_SA_ENCAPSULATE_TUNNEL_V01:
      mode_str = "tunnel";
      break;

    default:
      break;
  }

  return mode_str;
}

/*===========================================================================
  FUNCTION  netmgr_util_circ_list_init
===========================================================================*/
/*!
@brief
  Initializes the given circular list

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_circ_list_init
(
  netmgr_util_circ_list_type  *clist
)
{
  int rc = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if (!clist)
  {
    netmgr_log_err("invalid input");
    goto bail;
  }

  clist->r = 0;
  clist->w = 0;
  memset(clist->data, 0, sizeof(clist->data));

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_util_circ_list_destroy
===========================================================================*/
/*!
@brief
  Frees any remaining data on the list before initializing it

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_circ_list_destroy
(
  netmgr_util_circ_list_type  *clist
)
{
  int rc = NETMGR_FAILURE;
  int i;

  NETMGR_LOG_FUNC_ENTRY;

  if (!clist)
  {
    netmgr_log_err("invalid input");
    goto bail;
  }

  /* If there are any elements in the list */
  if (clist->r != clist->w)
  {
    if (clist->r > clist->w)
    {
      for (i = clist->r; i < NETMGR_UTIL_MAX_CIRC_LIST_SIZE+1; ++i)
      {
        ds_free(clist->data[i]);
      }
      /* Start from 0 */
      clist->r = 0;
    }

    for (i = clist->r; i < clist->w; ++i)
    {
      ds_free(clist->data[i]);
    }
  }

  rc = netmgr_util_circ_list_init(clist);

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_util_enq_circ_list
===========================================================================*/
/*!
@brief
  Enqueues data into the circular list

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_enq_circ_list
(
  netmgr_util_circ_list_type  *clist,
  void                        *data
)
{
  int rc = NETMGR_FAILURE, num_free = 0;

  NETMGR_LOG_FUNC_ENTRY;

  if (!clist || !data)
  {
    netmgr_log_err("invalid input");
    goto bail;
  }

  if (clist->w >= clist->r)
  {
    num_free = NETMGR_UTIL_MAX_CIRC_LIST_SIZE - (clist->w - clist->r);
  }
  else
  {
    num_free = clist->r - clist->w - 1;
  }

  if (function_debug)
  {
    netmgr_log_med("r=%d, w=%d, free=%d",
                   clist->r, clist->w, num_free);
  }

  if (0 == num_free)
  {
    netmgr_log_err("no more free space to store data=%p", data);
    goto bail;
  }

  clist->data[clist->w++] = data;

  if (clist->w > NETMGR_UTIL_MAX_CIRC_LIST_SIZE)
  {
    clist->w = 0;
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_util_deq_circ_list
===========================================================================*/
/*!
@brief
  Dequeues data from the circular list and returns it via the out pointer

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_util_deq_circ_list
(
  netmgr_util_circ_list_type  *clist,
  void                        **data
)
{
  int rc = NETMGR_FAILURE, num_avail = 0;

  NETMGR_LOG_FUNC_ENTRY;

  if (!clist || !data)
  {
    netmgr_log_err("invalid input");
    goto bail;
  }

  if (clist->w >= clist->r)
  {
    num_avail = clist->w - clist->r;
  }
  else
  {
    num_avail = NETMGR_UTIL_MAX_CIRC_LIST_SIZE + 1 - clist->r + clist->w;
  }

  if (function_debug)
  {
    netmgr_log_med("r=%d, w=%d, avail=%d",
                   clist->r, clist->w, num_avail);
  }

  if (0 == num_avail)
  {
    netmgr_log_err("no more data available on list=%p", clist);
    goto bail;
  }

  *data = clist->data[clist->r++];

  if (clist->r > NETMGR_UTIL_MAX_CIRC_LIST_SIZE)
  {
    clist->r = 0;
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}
#endif /* FEATURE_DATA_IWLAN */

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
/*===========================================================================
  FUNCTION  netmgr_util_wakelock_acquire
===========================================================================*/
/*!
@brief
  Acquires the netmgr wakelock.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_acquire()
{
  /* If the wakelock timeout property is set to zero we will disable the wakelock
     acquring and release feature. This is added for debugging capability */
  if (FALSE == wl_feature_disable)
  {
    if (0 == netmgr_util_get_wakelock_timeout())
    {
      wl_feature_disable = TRUE;
      return;
    }
  }
  else
  {
    return;
  }

  NETMGR_LOCK_MUTEX(wakelock_mutex);

  /* To safeguard against possible dangling ref counts also maintain a timer which would release
     the wakelock if ref count is greater than zero */
  if (wl_ref_count > 0)
  {
    netmgr_util_delete_wakelock_timer();
    netmgr_log_med("%s(): resetting timer for wakelock", __func__);
  }

  if (NETMGR_FAILURE == netmgr_util_start_wakelock_timer())
  {
    netmgr_log_err("%s(): failed to create timer [%d:%s]",
                   __func__, errno, strerror(errno));

    if (wl_ref_count > 0)
    {
      /* Reset the wakelock ref count to 1 and release the wakelock */
      wl_ref_count = 1;
      NETMGR_UNLOCK_MUTEX(wakelock_mutex);
      netmgr_util_wakelock_release();
      return;
    }
    goto bail;
  }

  /* Acquire wakelock only when the ref count is zero
     Otherwise just increment the ref count */
  if (0 == wl_ref_count)
  {
    netmgr_log_med("%s(): Acquiring wakelock [%s]", __func__, NETMGR_WAKELOCK_NAME);
    if (acquire_wake_lock(PARTIAL_WAKE_LOCK, NETMGR_WAKELOCK_NAME) < 0)
    {
      netmgr_log_err("%s: failed to acquire wake lock [%d:%s]",
                     __func__, errno, strerror(errno));
      netmgr_util_delete_wakelock_timer();
      goto bail;
    }
  }

  wl_ref_count++;

  netmgr_log_med("%s(): Waklock ref_count [%d]", __func__, wl_ref_count);

bail:
  NETMGR_UNLOCK_MUTEX(wakelock_mutex);
}

/*===========================================================================
  FUNCTION  netmgr_util_wakelock_release
===========================================================================*/
/*!
@brief
  Releases the netmgr wakelock.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_release()
{
  int counter = 0;

  /* If the wakelock timeout property is set to zero we will disable the wakelock
     acquring and release feature. This is added for debugging capability */
  if (FALSE == wl_feature_disable)
  {
    if (0 == netmgr_util_get_wakelock_timeout())
    {
      wl_feature_disable = TRUE;
      return;
    }
  }
  else
  {
    return;
  }

  NETMGR_LOCK_MUTEX(wakelock_mutex);

  /* Release wakelock only when the ref count is zero
     Otherwise just decrement the ref count */
  if (1 == wl_ref_count)
  {
    netmgr_util_delete_wakelock_timer();
    do
    {
      netmgr_log_med("%s(): Releasing wakelock [%s]", __func__, NETMGR_WAKELOCK_NAME);
      if (release_wake_lock(NETMGR_WAKELOCK_NAME) < 0)
      {
        netmgr_log_err("%s: failed to release wake lock [%d:%s]. Retrying...%d",
                       __func__, errno, strerror(errno), counter);
        sleep(NETMGR_UTIL_WAKELOCK_SLEEP_TIME);
        ++counter;
        continue;
      }

      break;
    } while (NETMGR_UTIL_WAKELOCK_MAX_RETRIES > counter);
  }

  if (wl_ref_count > 0)
  {
    wl_ref_count--;
    netmgr_log_med("%s(): Waklock ref_count [%d]", __func__, wl_ref_count);
  }
  else
  {
    netmgr_log_low("%s(): wakelock already released", __func__);
  }

bail:
  NETMGR_UNLOCK_MUTEX(wakelock_mutex);
}

/*===========================================================================
  FUNCTION  netmgr_util_wakelock_force_release
===========================================================================*/
/*!
@brief
  Releases the netmgr wakelock forcefully if multiple wl refs are held.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_force_release()
{
  NETMGR_LOCK_MUTEX(wakelock_mutex);

  if(wl_ref_count <= 0)
  {
     NETMGR_UNLOCK_MUTEX(wakelock_mutex);
     netmgr_log_low("%s(): wakelock already released", __func__);
     return;
  }

  wl_ref_count = 1;
  NETMGR_UNLOCK_MUTEX(wakelock_mutex);

  netmgr_util_wakelock_release();
  return;
}

#elif defined(NETMGR_OFFTARGET)

/*===========================================================================
  FUNCTION  netmgr_util_wakelock_force_release
===========================================================================*/
/*!
@brief
  Releases the netmgr wakelock forcefully if multiple wl refs are held.

@return
  none

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void netmgr_util_wakelock_force_release()
{
  return;
}

#endif
