/******************************************************************************
#  Copyright (c) 2008-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_log.c
  @brief   qcril qmi - logging utilities

  DESCRIPTION

******************************************************************************/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#define QCRIL_IS_LOG_TO_FILE_ENABLED         "persist.vendor.radio.ril_log_enabled"
#define QCRIL_IS_LOG_TO_FILE_INTERVAL        "persist.vendor.radio.ril_log_interval"
#define QCRIL_IS_LOG_TO_FILE_DEF_INTERVAL    (5)


#ifdef QMI_RIL_UTF
#define QCRIL_IPC_RILD_SOCKET_PATH_PREFIX    "./rild_sync_"
#else
#define QCRIL_IPC_RILD_SOCKET_PATH_PREFIX    "/dev/socket/qmux_radio/rild_sync_"
#endif
#define QCRIL_IPC_MAX_SOCKET_PATH_LENGTH     48
#define QCRIL_IPC_BIND_RETRY_MAX_ATTEMPTS    5

#define qcril_log_print_ril_message_payload(format, ...) qcril_log_print_ril_message_payload_func("$$$$$$ " format, ##__VA_ARGS__)
#ifdef QMI_RIL_UTF
#include <netinet/in.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#else
#include <utils/Log.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#endif

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <cutils/properties.h>
#include <pthread.h>
#include <framework/qcril_event.h>
#include <translators/android/utils.h>
#include "qcril_log.h"
#include "qcril_am.h"
#include "qcril_other.h"
#include <sys/un.h>

// Required for glibc compile
#include <limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#ifdef LOG_TAG
#undef LOG_TAG // It might be defined differently in diag header files
#endif
#define LOG_TAG "RILQ"

#define QCRIL_LOG_FILE DATA_PARTITION_ROOT "radio/ril_log"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

FILE *rild_fp;
__thread char log_buf[ QCRIL_MAX_LOG_MSG_SIZE ];
__thread char log_fmt[ QCRIL_MAX_LOG_MSG_SIZE ];
__thread char thread_name[ QMI_RIL_THREAD_NAME_MAX_SIZE ];
#ifdef QMI_RIL_UTF
char log_buf_raw[ QCRIL_MAX_LOG_MSG_SIZE ];
#endif
pthread_mutex_t log_lock_mutex;
pthread_mutex_t log_timer_mutex;
boolean diag_init_complete = FALSE;

/* Flag that controls whether QCRIL debug messages logged on ADB or not */
boolean qcril_log_adb_on = FALSE;

/* Flag that controls whether QCRIL message payload logged on QXDM log or not */
boolean qcril_log_ril_msg_payload_log_on = FALSE;

static pid_t qcril_qmi_instance_log_id;
boolean qcril_is_internal_token(RIL_Token token);

int qcril_log_early_radio_power_multiple_rild_process_complete_num;
uint8_t qcril_log_early_radio_power_multiple_rild_process_timer_valid;

typedef struct
{
 qcril_instance_id_e_type rild_instance_id;
 size_t rild_addrlen;
 struct sockaddr_un rild_addr;
}other_rild_addr_info_type;

static struct inter_rild_info_type
{
  int info_valid;
  int my_sockid;
  other_rild_addr_info_type *other_rilds_addr_info;
  int other_rilds_addr_info_len;
  pthread_t recv_thread_id;
  pthread_attr_t recv_thread_attr;
  pthread_mutex_t send_lock_mutex;
}inter_rild_info;

typedef struct ipc_send_recv_data_info
{
  qcril_instance_id_e_type rild_instance_id;
  ipc_message_id_type message_id;
  char payload[QCRIL_MAX_IPC_PAYLOAD_SIZE];
  int payload_length;
}ipc_send_recv_data_info_type;

typedef struct
{
    int is_valid;
    pthread_t thread_id;
    char thread_name[QMI_RIL_THREAD_NAME_MAX_SIZE];
} qmi_ril_thread_name_info_type;

#ifndef QMI_RIL_UTF
static
#endif
qmi_ril_thread_name_info_type qmi_ril_thread_name_info[QMI_RIL_THREAD_INFO_MAX_SIZE];

static int qmi_ril_log_enabled;
static int qmi_ril_log_timer_interval;
static uint32 qmi_ril_log_timer_id;
static int qcril_log_additional_logging_enabled = 0;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES


===========================================================================*/

static void* qcril_multiple_rild_ipc_recv_func(void *arg);
static void qcril_multiple_rild_ipc_signal_handler_sigusr1(int arg);
static int qcril_ipc_evaluate_rilds_socket_paths(char *rild_socket_name);
static void qcril_log_print_ril_message_payload_func(const char* format, ...)
                                  __attribute__ ((format (printf, 1, 2)));

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*=========================================================================
  FUNCTION:  qcril_log_init

===========================================================================*/
/*!
    @brief
    Initialization for logging.

    @return
    None
*/
/*=========================================================================*/
void qcril_log_init
(
  void
)
{
  char args[ PROPERTY_VALUE_MAX ];
  int len;
  char *end_ptr;
  unsigned long ret_val;

  qcril_qmi_instance_log_id = getpid();

  ret_val = 0;
  remove(QCRIL_LOG_FILE);
  property_get( QCRIL_IS_LOG_TO_FILE_ENABLED, args, "" );
  len = strlen( args );
  if ( len > 0 )
  {
    ret_val = strtoul( args, &end_ptr, 0 );
    if ( ( errno == ERANGE ) && ( ( ret_val == ULONG_MAX ) || ( ret_val == 0 ) ) )
    {
    #ifndef QMI_RIL_UTF
      RLOGE( "Fail to convert ril_log_enabled setting %s", args );
    #endif
    }
    else if (ret_val)
    {
        rild_fp = fopen(QCRIL_LOG_FILE, "w");
        if ( !rild_fp )
        {
      #ifndef QMI_RIL_UTF
          RLOGE( "Fail to create %s for QCRIL logging\n",QCRIL_LOG_FILE );
      #endif
        }
        else
        {
             setvbuf ( rild_fp , NULL , _IOFBF , 1024 );
      #ifndef QMI_RIL_UTF
            RLOGE( "%s for QCRIL logging created successfully\n",QCRIL_LOG_FILE );
      #endif
            qmi_ril_log_enabled = 1;
        }
    }
  }

  if(!ret_val)
  {
      rild_fp = NULL;
    #ifndef QMI_RIL_UTF
      RLOGE( "log to %s for QCRIL logging is not enabled\n",QCRIL_LOG_FILE );
    #endif
  }

  /* Initialize Diag for QCRIL logging */
  ret_val = Diag_LSM_Init(NULL);
  if ( !ret_val )
  {
#ifndef QMI_RIL_UTF
    RLOGE( "Fail to initialize Diag for QCRIL logging\n" );
#endif
  }
  else
  {
    diag_init_complete = TRUE;
  }

  QCRIL_LOG_DEBUG ( "qcril_log_init() 1" );

  property_get( QCRIL_LOG_ADB_ON, args, "" );
  len = strlen( args );
  if ( len > 0 )
  {
    ret_val = strtoul( args, &end_ptr, 0 );
    if ( ( errno == ERANGE ) && ( ( ret_val == ULONG_MAX ) || ( ret_val == 0 ) ) )
    {
      QCRIL_LOG_ERROR( "Fail to convert adb_log_on setting %s", args );
    }
    else if ( ret_val > 1 )
    {
      QCRIL_LOG_ERROR( "Invalid saved adb_log_on setting %ld, use default", ret_val );
    }
    else
    {
      qcril_log_adb_on = ( boolean ) ret_val;
    }
  }

  #ifdef FEATURE_QCRIL_ADB_LOG_ON
  qcril_log_adb_enabled = TRUE;
  #endif /* FEATURE_QCRIL_ADB_LOG_ON */

  QCRIL_LOG_DEBUG ( "qcril_log_init() 2" );

  QCRIL_LOG_DEBUG( "adb_log_on = %d", (int) qcril_log_adb_on );

  /* Save ADB Log Enabled setting to system property */
  QCRIL_SNPRINTF( args, sizeof( args ), "%d", (int) qcril_log_adb_on );
  if ( property_set( QCRIL_LOG_ADB_ON, args ) != E_SUCCESS )
  {
    QCRIL_LOG_ERROR( "Fail to save %s to system property", QCRIL_LOG_ADB_ON );
  }

  /* Fetching RIL additional logging property */
  qmi_ril_get_property_value_from_integer (QCRIL_LOG_ADDITIONAL_LOG_ON,
                                           &qcril_log_additional_logging_enabled,
                                           QMI_RIL_ZERO);

  /* Fetching RIL payload dumping property */
  property_get( QCRIL_LOG_RIL_PAYLOAD_LOG_ON, args, "" );
  len = strlen( args );
  if ( len > 0 )
  {
    ret_val = strtoul( args, &end_ptr, 0 );
    if ( ( errno == ERANGE ) && ( ( ret_val == ULONG_MAX ) || ( ret_val == 0 ) ) )
    {
      QCRIL_LOG_ERROR( "Fail to convert ril_msg_payload_on setting %s\n", args );
    }
    else if ( ret_val > 1 )
    {
      QCRIL_LOG_ERROR( "Invalid saved ril_msg_payload_on setting %ld, use default\n", ret_val );
    }
    else
    {
      qcril_log_ril_msg_payload_log_on = ( boolean ) ret_val;
    }
  }

  QCRIL_LOG_DEBUG ( "qcril_log_init() 3" );

  QCRIL_LOG_DEBUG( "ril_msg_payload_on = %d", (int) qcril_log_ril_msg_payload_log_on );

  /* Save RIL Payload Log Enabled setting to system property */
  QCRIL_SNPRINTF( args, sizeof( args ), "%d", qcril_log_ril_msg_payload_log_on );
  if ( property_set( QCRIL_LOG_RIL_PAYLOAD_LOG_ON, args ) != E_SUCCESS )
  {
    QCRIL_LOG_ERROR( "Fail to save %s to system property\n", QCRIL_LOG_RIL_PAYLOAD_LOG_ON );
  }

  pthread_mutexattr_t mtx_atr;
  pthread_mutexattr_init(&mtx_atr);
  pthread_mutex_init(&log_lock_mutex, &mtx_atr);
  pthread_mutex_init(&log_timer_mutex, &mtx_atr);

  qcril_log_reset_early_radio_power_req();

} /* qcril_log_init */


void qcril_log_cancel_log_timer()
{
    pthread_mutex_lock(&log_timer_mutex);
    if(qmi_ril_log_enabled && qmi_ril_log_timer_id)
    {
        qcril_cancel_timed_callback((void *)(uintptr_t)qmi_ril_log_timer_id);
        qmi_ril_log_timer_id = QMI_RIL_ZERO;
    }
    pthread_mutex_unlock(&log_timer_mutex);
}

void qcril_log_timer_expiry_cb(void * params)
{
    struct timeval tv;
    struct timespec ts;
    struct tm* tm = NULL;
    char time_string[40];
    long milliseconds;
    QCRIL_NOTUSED(params);

    if(qmi_ril_is_feature_supported(QMI_RIL_FEATURE_POSIX_CLOCKS))
    {
        clock_gettime(CLOCK_MONOTONIC,
                      &ts);
        tv.tv_sec = ts.tv_sec;
        tv.tv_usec = ts.tv_nsec/1000;
    }
    else
    {
        gettimeofday(&tv,
                     NULL);
    }
    tm = localtime (&tv.tv_sec);

    if(NULL != tm)
    {
        strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", tm);
        milliseconds = tv.tv_usec / 1000;

        pthread_mutex_lock(&log_timer_mutex);
        qmi_ril_log_timer_id = QMI_RIL_ZERO;

        pthread_mutex_lock(&log_lock_mutex);
        if (rild_fp)
        {
            fflush(rild_fp);
            fprintf (rild_fp, "Timestamp : %s.%03ld\n", time_string, milliseconds);
        }
        pthread_mutex_unlock(&log_lock_mutex);

        qcril_log_timer_setup();
        pthread_mutex_unlock(&log_timer_mutex);
    }
}

void qcril_log_timer_setup( void )
{
    struct timeval log_timeout;
    struct timeval *log_timeout_ptr;

    log_timeout_ptr = NULL;

    if( qmi_ril_log_timer_interval )
    {
        log_timeout.tv_sec = qmi_ril_log_timer_interval;
        log_timeout.tv_usec = QMI_RIL_ZERO;
        log_timeout_ptr = &log_timeout;
    }
    qcril_setup_timed_callback( QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                                qcril_log_timer_expiry_cb,
                                log_timeout_ptr, &qmi_ril_log_timer_id);
}

/*=========================================================================
  FUNCTION:  qcril_log_timer_init

===========================================================================*/
/*!
    @brief
    Initialization for log timer.

    @return
    None
*/
/*=========================================================================*/
void qcril_log_timer_init( void )
{
    char args[ PROPERTY_VALUE_MAX ];
    int len;
    char *end_ptr;
    unsigned long ret_val;

    pthread_mutex_lock(&log_timer_mutex);
    if( qmi_ril_log_enabled )
    {
        ret_val = QCRIL_IS_LOG_TO_FILE_DEF_INTERVAL;
        property_get( QCRIL_IS_LOG_TO_FILE_INTERVAL, args, "" );
        len = strlen( args );
        if ( len > 0 )
        {
          ret_val = strtoul( args, &end_ptr, 0 );
          if ( ( errno == ERANGE ) && ( ( ret_val == ULONG_MAX ) || ( ret_val == 0 ) ) )
          {
            ret_val = QCRIL_IS_LOG_TO_FILE_DEF_INTERVAL;
          #ifndef QMI_RIL_UTF
            RLOGE( "Fail to convert ril_log_interval setting %s", args );
          #endif
          }
        }
        qmi_ril_log_timer_interval = ret_val;
        #ifndef QMI_RIL_UTF
        RLOGE( "using %d for qmi_ril_log_timer_interval", qmi_ril_log_timer_interval);
        #endif
        qcril_log_timer_setup();
    }
    pthread_mutex_unlock(&log_timer_mutex);

} /* qcril_log_timer_init */


/*=========================================================================
  FUNCTION:  qcril_log_cleanup

===========================================================================*/
/*!
    @brief
    Cleanup for logging.

    @return
    None
*/
/*=========================================================================*/
void qcril_log_cleanup
(
  void
)
{
    pthread_mutex_lock(&log_lock_mutex);
    if (rild_fp)
    {
        fclose(rild_fp);
        rild_fp = NULL;
    }
    pthread_mutex_unlock(&log_lock_mutex);

} /* qcril_log_cleanup */


/*=========================================================================
  FUNCTION:  qcril_format_log_msg

===========================================================================*/
/*!
    @brief
    Format debug message for logging.

    @return
    None
*/
/*=========================================================================*/
void qcril_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  const char *fmt,
  ...
)
{
  va_list ap;


  va_start( ap, fmt );

  if ( NULL != buf_ptr && buf_size > 0 )
  {
      vsnprintf( buf_ptr, buf_size, fmt, ap );
  }

  va_end( ap );

} /* qcril_format_log_msg */


/*=========================================================================
  FUNCTION:  qcril_log_call_flow_packet

===========================================================================*/
/*!
    @brief
    Log the call flow packet.

    @return
    None
*/
/*=========================================================================*/
void qcril_log_call_flow_packet
(
  qcril_call_flow_subsystem_e_type src_subsystem,
  qcril_call_flow_subsystem_e_type dest_subsystem,
  qcril_call_flow_arrow_e_type arrow,
  char *label
)
{
  #if !defined(FEATURE_UNIT_TEST) && !defined(QMI_RIL_UTF)
  qcril_call_flow_log_packet_type *log_buf;
  uint16 label_len, log_packet_size = 0;

  /* Calculate the size of the log packet */
  label_len = strlen( label );
  log_packet_size = sizeof( qcril_call_flow_log_packet_type ) + label_len;

  /* Allocate log buffer */
  log_buf = ( qcril_call_flow_log_packet_type * ) log_alloc( LOG_QCRIL_CALL_FLOW_C, log_packet_size );

  if ( log_buf != NULL )
  {
    /* Fill in the log buffer */
    log_buf->src_subsystem = (uint8) src_subsystem;
    log_buf->dest_subsystem = (uint8) dest_subsystem;
    log_buf->arrow = (uint8) arrow;
    log_buf->label[ 0 ] = '\0';
    if ( label_len > 0 )
    {
      memcpy( (void *) log_buf->label, label, label_len + 1 );
    }

    /* Commit log buffer */
    log_commit( log_buf );
  }
  #endif /* !FEATURE_UNIT_TEST */

} /* qcril_log_call_flow_packet */

#ifndef QMI_RIL_UTF
/*=========================================================================
  FUNCTION:  qcril_log_msg_to_adb

===========================================================================*/
/*!
    @brief
    Log debug message to ADB.

    @return
    None
*/
/*=========================================================================*/
void qcril_log_msg_to_adb
(
  int  lvl,
  char *msg_ptr
)
{
  switch ( lvl )
  {
      case MSG_LEGACY_ERROR:
      case MSG_LEGACY_FATAL:
#ifndef QMI_RIL_UTF
        RLOGE( "(%d/%d): %s",
               (int)qmi_ril_get_process_instance_id(),
               (int)qcril_qmi_instance_log_id,
               msg_ptr );
#endif
        break;

      case MSG_LEGACY_HIGH:                         // fall through
        RLOGW_IF( qcril_log_adb_on, "(%d/%d): %s",
                  (int)qmi_ril_get_process_instance_id(),
                  (int)qcril_qmi_instance_log_id,
                  msg_ptr );
        break;

      case MSG_LEGACY_ESSENTIAL:
        RLOGI( "(%d/%d):%s",
               (int)qmi_ril_get_process_instance_id(),
               (int)qcril_qmi_instance_log_id,
                msg_ptr );
        break;

      case MSG_LEGACY_MED:
        RLOGI_IF( qcril_log_adb_on, "(%d/%d): %s",
                  (int)qmi_ril_get_process_instance_id(),
                  (int)qcril_qmi_instance_log_id,
                  msg_ptr );
        break;

      default:
        RLOGV_IF( qcril_log_adb_on, "(%d/%d): %s",
                  (int)qmi_ril_get_process_instance_id(),
                  (int)qcril_qmi_instance_log_id,
                  msg_ptr );
        break;
  }

} /* qcril_log_msg_to_adb */

#endif

const char *qcril_log_ril_errno_to_str(RIL_Errno ril_errno)
{
  switch(ril_errno)
  {
    case RIL_E_SUCCESS:
      return "Success";

    case RIL_E_RADIO_NOT_AVAILABLE:
      return "Radio Not Available";

    case RIL_E_GENERIC_FAILURE:
      return "Generic Failure";

    case RIL_E_PASSWORD_INCORRECT:
      return "Password Incorrect";

    case RIL_E_SIM_PIN2:
      return "SIM Pin2";

    case RIL_E_SIM_PUK2:
      return "SIM Puk2";

    case RIL_E_REQUEST_NOT_SUPPORTED:
      return "Request Not Supported";

    case RIL_E_CANCELLED:
      return "Cancell`ed";

    case RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL:
      return "OP Not Allowed During Voice Call";

    case RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW:
      return "OP Not Allowed Before Reg To NW";

    case RIL_E_SMS_SEND_FAIL_RETRY:
      return "SMS Send Fail Retry";

    case RIL_E_SIM_ABSENT:
      return "SIM Absent";

    case RIL_E_SUBSCRIPTION_NOT_AVAILABLE:
      return "Subscription Not Available";

    case RIL_E_MODE_NOT_SUPPORTED:
      return "Mode Not Supported";

    case RIL_E_FDN_CHECK_FAILURE:
      return "FDN Check Failure";

    case RIL_E_ILLEGAL_SIM_OR_ME:
      return "Illegal SIM or ME";

    case RIL_E_MISSING_RESOURCE:
      return "Missing resource";

    case RIL_E_NO_SUCH_ELEMENT:
      return "No such element";

    case RIL_E_DIAL_MODIFIED_TO_USSD:
      return "Dial modified to USSD";

    case RIL_E_DIAL_MODIFIED_TO_SS:
      return "Dial modified to SS";

    case RIL_E_DIAL_MODIFIED_TO_DIAL:
      return "Dial modified to Dial";

    case RIL_E_USSD_MODIFIED_TO_DIAL:
      return "USSD modified to Dial";

    case RIL_E_USSD_MODIFIED_TO_SS:
      return "USSD modified to SS";

    case RIL_E_USSD_MODIFIED_TO_USSD:
      return "USSD modified to USSD";

    case RIL_E_SS_MODIFIED_TO_DIAL:
      return "SS modified to Dial";

    case RIL_E_SS_MODIFIED_TO_USSD:
      return "SS modified to USSD";

    case RIL_E_SUBSCRIPTION_NOT_SUPPORTED:
      return "Subscription not supported";

    case RIL_E_SS_MODIFIED_TO_SS:
      return "SS modified to SS";

#if defined(RIL_REQUEST_START_LCE) ||  defined(RIL_REQUEST_STOP_LCE)
    case RIL_E_LCE_NOT_SUPPORTED:
      return "LCE not supported";
#endif
    default:
      break;

  }
  return "unknown";
}

/*===========================================================================

  FUNCTION:  qcril_log_ril_radio_state_to_str

===========================================================================*/
/*!
    @brief
    Convert Radio State to string

    @return
    Radio state in readable string format
*/
/*=========================================================================*/
const char *qcril_log_ril_radio_state_to_str(RIL_RadioState radio_state)
{
  switch (radio_state)
  {
    case RADIO_STATE_OFF:
      return "Radio Off";
    case RADIO_STATE_UNAVAILABLE:
      return "Radio Unavailable";
    case RADIO_STATE_ON:
      return "Radio On";
    default:
      break;
  }
  return "<unspecified>";
}


//============================================================================
// FUNCTION: qcril_ipc_init
//
// DESCRIPTION:
// create and bind sockets to respective ports, create receiver thread
// used for piping logs from 2nd ril instance to 1st instance so they can
// both be sent to diag
// Also used to send RADIO_POWER state change messages to sync RADIO_POWER
// state changes between multiple RILs.
//
// RETURN: None
//============================================================================
//
void qcril_ipc_init()
{
    int sockfd=0,rc=0,len=0,number_of_tries=0,bind_result=FALSE;
    struct sockaddr_un local;
    char server[QCRIL_IPC_MAX_SOCKET_PATH_LENGTH];

    signal(SIGUSR1,qcril_multiple_rild_ipc_signal_handler_sigusr1);
    memset(&inter_rild_info,0,sizeof(inter_rild_info));
    rc = qcril_ipc_evaluate_rilds_socket_paths(server);
    if(rc)
    {
        unlink (server);

        //mutex initialization
        pthread_mutex_init(&inter_rild_info.send_lock_mutex, NULL);

        //server initialization
        do
        {
          if (number_of_tries != 0)
          {
            sleep(1);
          }
          QCRIL_LOG_DEBUG("IPC socket init try # %d", number_of_tries);
          if ((sockfd = socket(AF_UNIX,SOCK_DGRAM,0)) >= 0)
          {
            local.sun_family = AF_UNIX;
            strlcpy(local.sun_path, server, sizeof(local.sun_path));
            len = strlen(server) + sizeof(local.sun_family);

            if (bind(sockfd,(struct sockaddr *)&local, len) >= 0)
            {
                inter_rild_info.my_sockid = sockfd;
                pthread_attr_init(&inter_rild_info.recv_thread_attr);
                pthread_attr_setdetachstate(&inter_rild_info.recv_thread_attr, PTHREAD_CREATE_JOINABLE);
#ifdef QMI_RIL_UTF
                if(!utf_pthread_create_handler(&inter_rild_info.recv_thread_id,&inter_rild_info.recv_thread_attr,
                                  qcril_multiple_rild_ipc_recv_func,(void*) &inter_rild_info))
#else
                if(!pthread_create(&inter_rild_info.recv_thread_id,&inter_rild_info.recv_thread_attr,
                                  qcril_multiple_rild_ipc_recv_func,(void*) &inter_rild_info))
#endif
                {
                    qmi_ril_set_thread_name(inter_rild_info.recv_thread_id, QMI_RIL_IPC_RECEIVER_THREAD_NAME);
                    inter_rild_info.info_valid = TRUE;
                }
                else
                {
                    QCRIL_LOG_ERROR("unable to spawn dedicated thread for rild IPC");
                    close(sockfd);
                }
                bind_result = TRUE;
            }
            else
            {
                QCRIL_LOG_ERROR("unable to bind socket for rild IPC");
                close(sockfd);
            }
          }
          else
          {
            QCRIL_LOG_ERROR("unable to open socket for rild IPC");
          }
          number_of_tries++;
        } while((bind_result == FALSE) && (number_of_tries < QCRIL_IPC_BIND_RETRY_MAX_ATTEMPTS));
    }
    else if(inter_rild_info.other_rilds_addr_info)
    {
        qcril_free(inter_rild_info.other_rilds_addr_info);
        inter_rild_info.other_rilds_addr_info_len = QMI_RIL_ZERO;
        inter_rild_info.other_rilds_addr_info = NULL;
    }
}

//============================================================================
// FUNCTION: qcril_ipc_release
//
// DESCRIPTION:
// release resources used to create inter-ril communication socket
//
// RETURN: None
//============================================================================
//
void qcril_ipc_release()
{
  if(inter_rild_info.info_valid)
  {
    inter_rild_info.info_valid = FALSE;
    pthread_mutex_destroy(&inter_rild_info.send_lock_mutex);
    pthread_kill(inter_rild_info.recv_thread_id,SIGUSR1);
    pthread_join(inter_rild_info.recv_thread_id,NULL);
    pthread_attr_destroy(&inter_rild_info.recv_thread_attr);
    close(inter_rild_info.my_sockid);

    if(inter_rild_info.other_rilds_addr_info)
    {
        qcril_free(inter_rild_info.other_rilds_addr_info);
        inter_rild_info.other_rilds_addr_info_len = QMI_RIL_ZERO;
        inter_rild_info.other_rilds_addr_info = NULL;
    }
  }
}

//============================================================================
// FUNCTION: qcril_ipc_evaluate_rilds_socket_paths
//
// DESCRIPTION:
// Evaluate socket paths for the current rild and the other rild's in mutiple rild scenario
//rild_socket_name will be updated to the current rild's socket path
//
// RETURN: FALSE If evaluation ended up with a error, TRUE otherwise
//============================================================================
//
int qcril_ipc_evaluate_rilds_socket_paths(char *rild_socket_name)
{
    int iter_i;
    int iter_other_rilds_addr_info;
    int num_of_rilds;
    int result = TRUE;
    struct sockaddr_un remote;

    num_of_rilds = qmi_ril_retrieve_number_of_rilds();

    iter_other_rilds_addr_info = 0;
    inter_rild_info.other_rilds_addr_info_len = num_of_rilds - 1;
    inter_rild_info.other_rilds_addr_info = qcril_malloc(sizeof(other_rild_addr_info_type) * (inter_rild_info.other_rilds_addr_info_len));
    if(inter_rild_info.other_rilds_addr_info)
    {
        for(iter_i = 0; iter_i < num_of_rilds; iter_i++)
        {
            if( iter_i == (int) qmi_ril_get_process_instance_id() )
            {
                snprintf( rild_socket_name, QCRIL_IPC_MAX_SOCKET_PATH_LENGTH, "%s%d", QCRIL_IPC_RILD_SOCKET_PATH_PREFIX,iter_i );
            }
            else
            {
                memset(&remote, 0, sizeof(remote));
                remote.sun_family = AF_UNIX;
                snprintf( remote.sun_path, sizeof(remote.sun_path), "%s%d", QCRIL_IPC_RILD_SOCKET_PATH_PREFIX,iter_i );
                inter_rild_info.other_rilds_addr_info[iter_other_rilds_addr_info].rild_addrlen =
                   strlen(remote.sun_path) + sizeof(remote.sun_family);
                inter_rild_info.other_rilds_addr_info[iter_other_rilds_addr_info].rild_addr = remote;
                inter_rild_info.other_rilds_addr_info[iter_other_rilds_addr_info].rild_instance_id = iter_i;
                iter_other_rilds_addr_info++;
            }
        }
    }
    else
    {
        result = FALSE;
        QCRIL_LOG_FATAL("Fail to allocate memory for inter_rild_info.other_rilds_addr_info");
    }
    return result;
} //qcril_ipc_evaluate_rilds_socket_paths

// prepare for sending radio power sync state to other rild
void qcril_multiple_rild_ipc_radio_power_propagation_helper_func(int is_genuine_signal)
{
    int iter_i;
    int num_of_rilds;

    if(inter_rild_info.other_rilds_addr_info)
    {
        num_of_rilds = qmi_ril_retrieve_number_of_rilds();

        for(iter_i = 0; iter_i < num_of_rilds; iter_i++)
        {
            if( iter_i != (int) qmi_ril_get_process_instance_id() )
            {
                qcril_multiple_rild_ipc_send_func(IPC_MESSAGE_RADIO_POWER, &is_genuine_signal, sizeof(is_genuine_signal), iter_i);
            }
        }
    }
}

int qcril_multiple_rild_ipc_send_func(ipc_message_id_type message_id, void * payload, int payload_length, int dest_rild_instance_id) //generic send function
{
    ipc_send_recv_data_info_type send_data;
    int iter_i;
    int match = FALSE;

    if(inter_rild_info.info_valid)
    {
          pthread_mutex_lock(&inter_rild_info.send_lock_mutex);
          if(inter_rild_info.other_rilds_addr_info)
          {
              for(iter_i = 0 ; iter_i < inter_rild_info.other_rilds_addr_info_len && !match; iter_i++)
              {
                  if(dest_rild_instance_id == (int) inter_rild_info.other_rilds_addr_info[iter_i].rild_instance_id)
                  {
                      match = TRUE;
                  }
              }
          }

          if(match)
          {
              memset(&send_data, 0, sizeof(send_data));         //HEADER = rild_instance_id + message_id
              send_data.rild_instance_id = qmi_ril_get_process_instance_id();
              send_data.message_id = message_id;
              send_data.payload_length = sizeof(send_data.message_id) + sizeof(send_data.rild_instance_id);

              if(NULL != payload && QMI_RIL_ZERO != payload_length)
              {
                  memcpy(&send_data.payload, payload, payload_length);
                  send_data.payload_length += payload_length;
              }
              sendto(inter_rild_info.my_sockid, &send_data, send_data.payload_length, 0,
                    (struct sockaddr *)(&inter_rild_info.other_rilds_addr_info[iter_i - 1].rild_addr),
                    inter_rild_info.other_rilds_addr_info[iter_i -1].rild_addrlen);
          }

          pthread_mutex_unlock(&inter_rild_info.send_lock_mutex);
    }
    return 0;
}

void* qcril_multiple_rild_ipc_recv_func(void *arg) //generic recv function
{
    int sockfd = ((struct inter_rild_info_type*)arg)->my_sockid;
    struct sockaddr_storage source_addr;
    socklen_t source_addr_len = 0;
    int received_buffer_length = 0;

    int radio_power_is_genuine_signal;
    ipc_send_recv_data_info_type recv_data;

    source_addr_len = sizeof(source_addr);
    while(1)
    {
        memset(&source_addr,0,sizeof(source_addr));
        memset(&recv_data, 0, sizeof(recv_data));
        if ((received_buffer_length = recvfrom(sockfd, &recv_data, sizeof(recv_data) , 0,(struct sockaddr *)&source_addr, &source_addr_len)) == -1)
        {
            close(sockfd);
            break;
        }

#ifdef QMI_RIL_UTF
        if ( received_buffer_length == 6 )
        {
          if (strncmp((char*)&recv_data, "reset", 6) == 0)
          {
            close(sockfd);
            qmi_ril_clear_thread_name(pthread_self());
            pthread_exit(NULL);
          }
        }
#endif
        switch(recv_data.message_id)
        {
            case IPC_MESSAGE_RADIO_POWER:
                radio_power_is_genuine_signal = *((int *) recv_data.payload);
                qcril_qmi_nas_handle_multiple_rild_radio_power_state_propagation(radio_power_is_genuine_signal);
                break;

            case IPC_MESSAGE_AM_CALL_STATE:
#ifndef QMI_RIL_UTF
                qcril_am_handle_event( QCRIL_AM_EVENT_INTER_RIL_CALL_STATE,
                                       (qcril_am_call_state_type*)recv_data.payload );
#endif
                break;

            default:
                break;
        }
    }

    qmi_ril_clear_thread_name(pthread_self());
    return NULL;
}

void qcril_multiple_rild_ipc_signal_handler_sigusr1(int arg)
{
    QCRIL_NOTUSED(arg);
    return;
}

//===========================================================================
// qmi_ril_set_thread_name
//===========================================================================
void qmi_ril_set_thread_name(pthread_t thread_id, const char *thread_name)
{
    pthread_setname_np(thread_id, thread_name);
    int iter_i = 0;

    for(iter_i = 0; iter_i < QMI_RIL_THREAD_INFO_MAX_SIZE; iter_i++)
    {
        if(FALSE == qmi_ril_thread_name_info[iter_i].is_valid)
        {
            qmi_ril_thread_name_info[iter_i].is_valid = TRUE;
            qmi_ril_thread_name_info[iter_i].thread_id = thread_id;
            strlcpy(qmi_ril_thread_name_info[iter_i].thread_name, thread_name, QMI_RIL_THREAD_NAME_MAX_SIZE);
            break;
        }
    }

} //qmi_ril_set_thread_name

//===========================================================================
// qmi_ril_get_thread_name
//===========================================================================
int qmi_ril_get_thread_name(pthread_t thread_id, char *thread_name)
{
#ifndef __ANDROID__
    pthread_getname_np(thread_id, thread_name, QMI_RIL_THREAD_NAME_MAX_SIZE);
    int res = TRUE;
#else
    int iter_i = 0,res = FALSE;

    for(iter_i = 0; iter_i < QMI_RIL_THREAD_INFO_MAX_SIZE; iter_i++)
    {
        if(TRUE == qmi_ril_thread_name_info[iter_i].is_valid && thread_id == qmi_ril_thread_name_info[iter_i].thread_id)
        {
            strlcpy(thread_name, qmi_ril_thread_name_info[iter_i].thread_name, QMI_RIL_THREAD_NAME_MAX_SIZE);
            res = TRUE;
            break;
        }
    }
#endif

    return res;
} //qmi_ril_get_thread_name

//===========================================================================
// qmi_ril_clear_thread_name
//===========================================================================
void qmi_ril_clear_thread_name(pthread_t thread_id)
{
    int iter_i = 0;

    for(iter_i = 0; iter_i < QMI_RIL_THREAD_INFO_MAX_SIZE; iter_i++)
    {
        if(TRUE == qmi_ril_thread_name_info[iter_i].is_valid && thread_id == qmi_ril_thread_name_info[iter_i].thread_id)
        {
            qmi_ril_thread_name_info[iter_i].is_valid = FALSE;
            break;
        }
    }

} //qmi_ril_clear_thread_name

/*===========================================================================

  FUNCTION: qcril_log_print_ril_message_payload_func

===========================================================================*/
/*!
    @brief
    Dump one line of RIL payload to the log

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_ril_message_payload_func(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  char buffer[QCRIL_MAX_LOG_MSG_SIZE];
  memset(buffer, 0, sizeof(buffer));
  vsnprintf(buffer, QCRIL_MAX_LOG_MSG_SIZE, format, args);
  QCRIL_LOG_INFO("%s", buffer);
  va_end(args);
} /* qcril_log_print_ril_message_payload_func */

/*===========================================================================

  FUNCTION: qcril_log_print_string_array

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is char**

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_string_array(char** ptr, size_t length)
{
  unsigned int i;
  for (i = 0; i < length; ++i)
  {
    if (ptr[i] == NULL)
      break;
    else if (ptr[i])
      qcril_log_print_ril_message_payload("(char**)Payload[%d] %s", i, ptr[i]);
  }
} /* qcril_log_print_string_array */

/*===========================================================================

  FUNCTION: qcril_log_print_int

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is int

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_int(int* ptr)
{
  qcril_log_print_ril_message_payload("(int)Payload = %d", *ptr);
} /* qcril_log_print_int */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_Dial

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_Dial

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_Dial(const char* header, const RIL_Dial* ptr)
{
  if (ptr->address)
    qcril_log_print_ril_message_payload("%saddress %s", header, ptr->address);
  qcril_log_print_ril_message_payload("%sclir = %d", header, ptr->clir);
  if (ptr->uusInfo)
  {
    qcril_log_print_ril_message_payload("%suusInfo = (RIL_UUS_Info*)malloc(sizeof(RIL_UUS_Info));", header);
    qcril_log_print_ril_message_payload("%suusInfo->uusType = %d", header, ptr->uusInfo->uusType);
    qcril_log_print_ril_message_payload("%suusInfo->uusDcs = %d", header, ptr->uusInfo->uusDcs);
    qcril_log_print_ril_message_payload("%suusInfo->uusLength = %d", header, ptr->uusInfo->uusLength);
    if (ptr->uusInfo->uusData)
      qcril_log_print_ril_message_payload("%suusInfo->uusData %s", header, ptr->uusInfo->uusData);
  }
} /* qcril_log_print_RIL_Dial */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_Call

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_Call

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_Call(const char* header, const RIL_Call* ptr)
{
  qcril_log_print_ril_message_payload("%sstate = %d", header, ptr->state);
  qcril_log_print_ril_message_payload("%sindex = %d", header, ptr->index);
  qcril_log_print_ril_message_payload("%stoa = %d", header, ptr->toa);
  qcril_log_print_ril_message_payload("%sisMpty = %u", header, ptr->isMpty);
  qcril_log_print_ril_message_payload("%sisMT = %u", header, ptr->isMT);
  qcril_log_print_ril_message_payload("%sals = %u", header, ptr->als);
  qcril_log_print_ril_message_payload("%sisVoice = %u", header, ptr->isVoice);
  qcril_log_print_ril_message_payload("%sisVoicePrivacy = %u", header, ptr->isVoicePrivacy);
  if (ptr->number)
    qcril_log_print_ril_message_payload("%snumber %s", header, ptr->number);
  qcril_log_print_ril_message_payload("%snumberPresentation = %d", header, ptr->numberPresentation);
  if (ptr->name)
    qcril_log_print_ril_message_payload("%sname %s", header, ptr->name);
  qcril_log_print_ril_message_payload("%snamePresentation = %d", header, ptr->namePresentation);
  if (ptr->uusInfo)
  {
    qcril_log_print_ril_message_payload("%suusInfo = (RIL_UUS_Info*)malloc(sizeof(RIL_UUS_Info));", header);
    qcril_log_print_ril_message_payload("%suusInfo->uusType = %d", header, ptr->uusInfo->uusType);
    qcril_log_print_ril_message_payload("%suusInfo->uusDcs = %d", header, ptr->uusInfo->uusDcs);
    qcril_log_print_ril_message_payload("%suusInfo->uusLength = %d", header, ptr->uusInfo->uusLength);
    if (ptr->uusInfo->uusData)
      qcril_log_print_ril_message_payload("%suusInfo->uusData %s", header, ptr->uusInfo->uusData);
  }
} /* qcril_log_print_RIL_Call */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_SignalStrength

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_SignalStrength

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_SignalStrength (char* header, const RIL_SignalStrength* ptr)
{
  qcril_log_print_ril_message_payload("%sGW_SignalStrength.signalStrength = %d",
                               header, ptr->GW_SignalStrength.signalStrength);
  qcril_log_print_ril_message_payload("%sGW_SignalStrength.bitErrorRate = %d",
                               header, ptr->GW_SignalStrength.bitErrorRate);
  qcril_log_print_ril_message_payload("%sCDMA_SignalStrength.dbm = %d",
                               header, ptr->CDMA_SignalStrength.dbm);
  qcril_log_print_ril_message_payload("%sCDMA_SignalStrength.ecio = %d",
                               header, ptr->CDMA_SignalStrength.ecio);
  qcril_log_print_ril_message_payload("%sEVDO_SignalStrength.dbm = %d",
                               header, ptr->EVDO_SignalStrength.dbm);
  qcril_log_print_ril_message_payload("%sEVDO_SignalStrength.ecio = %d",
                               header, ptr->EVDO_SignalStrength.ecio);
  qcril_log_print_ril_message_payload("%sEVDO_SignalStrength.signalNoiseRatio = %d",
                               header, ptr->EVDO_SignalStrength.signalNoiseRatio);
  qcril_log_print_ril_message_payload("%sLTE_SignalStrength.signalStrength = %d",
                               header, ptr->LTE_SignalStrength.signalStrength);
  qcril_log_print_ril_message_payload("%sLTE_SignalStrength.rsrp = %d",
                               header, ptr->LTE_SignalStrength.rsrp);
  qcril_log_print_ril_message_payload("%sLTE_SignalStrength.rsrq = %d",
                               header, ptr->LTE_SignalStrength.rsrq);
  qcril_log_print_ril_message_payload("%sLTE_SignalStrength.rssnr = %d",
                               header, ptr->LTE_SignalStrength.rssnr);
  qcril_log_print_ril_message_payload("%sLTE_SignalStrength.cqi = %d",
                               header, ptr->LTE_SignalStrength.cqi);
#ifndef QMI_RIL_UTF
  qcril_log_print_ril_message_payload("%sTD_SCDMA_SignalStrength.rscp = %d",
                               header, ptr->TD_SCDMA_SignalStrength.rscp);
#endif
} /* qcril_log_print_RIL_SignalStrength */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_CallForwardInfo

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_CallForwardInfo

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_CallForwardInfo(const char* header, const RIL_CallForwardInfo* ptr)
{
  qcril_log_print_ril_message_payload("%sstatus = %d", header, ptr->status);
  qcril_log_print_ril_message_payload("%sreason = %d", header, ptr->reason);
  qcril_log_print_ril_message_payload("%sserviceClass = %d", header, ptr->serviceClass);
  qcril_log_print_ril_message_payload("%stoa = %d", header, ptr->toa);

  if (ptr->number)
    qcril_log_print_ril_message_payload("%snumber %s", header, ptr->number);

  qcril_log_print_ril_message_payload("%stimeSeconds = %d", header, ptr->timeSeconds);
} /* qcril_log_print_RIL_CallForwardInfo */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_Call_array

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is an array of RIL_Call

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_Call_array(const RIL_Call** ptr, size_t datalen)
{
  size_t i;
  for (i = 0; i < datalen/4; ++i)
  {
    if (ptr[i])
    {
      char header [512];
      int n = snprintf(header, 512, "(RIL_Call**)Payload[%zu]->", i);
      if (n > 0)
        qcril_log_print_RIL_Call(header, ptr[i]);
    }
  }
} /* qcril_log_print_RIL_Call_array */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_CDMA_SignalInfoRecord

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_CDMA_SignalInfoRecord

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_CDMA_SignalInfoRecord(char* header, const RIL_CDMA_SignalInfoRecord* ptr)
{
  qcril_log_print_ril_message_payload("%sisPresent = %u", header, ptr->isPresent);
  qcril_log_print_ril_message_payload("%ssignalType = %u", header, ptr->signalType);
  qcril_log_print_ril_message_payload("%salertPitch = %u", header, ptr->alertPitch);
  qcril_log_print_ril_message_payload("%ssignal = %u", header, ptr->signal);
} /* qcril_log_print_RIL_CDMA_SignalInfoRecord */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_CallForwardInfo_array

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is an array of RIL_CallForwardInfo

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_CallForwardInfo_array(const RIL_CallForwardInfo** ptr, size_t datalen)
{
  size_t i;
  for (i = 0; i < datalen/4; ++i)
  {
    if (ptr[i])
    {
      char header [512];
      int n = snprintf(header, 512, "(RIL_CallForwardInfo**)Payload[%zu]->", i);
      if (n > 0)
        qcril_log_print_RIL_CallForwardInfo(header, ptr[i]);
    }
  }
} /* qcril_log_print_RIL_CallForwardInfo_array */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_IMS_SMS_Message

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_IMS_SMS_Message(char* header, const RIL_IMS_SMS_Message* ptr)
{
  qcril_log_print_ril_message_payload("%stech = %d", header, ptr->tech);
  qcril_log_print_ril_message_payload("%sretry = %u", header, ptr->retry);
  qcril_log_print_ril_message_payload("%smessageRef = %d", header, ptr->messageRef);
  // TODO: array out of bounds
  if ( (RADIO_TECH_3GPP2 == ptr->tech) && ptr->message.cdmaMessage)
  {
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->uTeleserviceID = %d",
                                 header, ptr->message.cdmaMessage->uTeleserviceID);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->bIsServicePresent = %u",
                                 header, ptr->message.cdmaMessage->bIsServicePresent);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->uServicecategory = %d",
                                 header, ptr->message.cdmaMessage->uServicecategory);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->sAddress.digit_mode = %d",
                                 header, ptr->message.cdmaMessage->sAddress.digit_mode);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->sAddress.number_mode = %d",
                                 header, ptr->message.cdmaMessage->sAddress.number_mode);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->sAddress.number_type = %d",
                                 header, ptr->message.cdmaMessage->sAddress.number_type);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->sAddress.number_plan = %d",
                                 header, ptr->message.cdmaMessage->sAddress.number_plan);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->sAddress.number_of_digits = %u",
                                 header, ptr->message.cdmaMessage->sAddress.number_of_digits);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->sAddress.digits~%s",
                                 header, ptr->message.cdmaMessage->sAddress.digits);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->subaddressType = %d",
                                 header, ptr->message.cdmaMessage->sSubAddress.subaddressType);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->odd = %u",
                                 header, ptr->message.cdmaMessage->sSubAddress.odd);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->number_of_digits = %u",
                                 header, ptr->message.cdmaMessage->sSubAddress.number_of_digits);
    qcril_log_print_ril_message_payload("%smessage.cdmaMessage->digits~%s",
                                 header, ptr->message.cdmaMessage->sSubAddress.digits);
  }
  if ((RADIO_TECH_3GPP == ptr->tech) && ptr->message.gsmMessage)
  {
    unsigned int i;
    for (i = 0; i < 2; ++i)
    {
      if (ptr->message.gsmMessage[i])
        qcril_log_print_ril_message_payload("%smessage.gsmMessage[%u] %s",
                                     header, i, ptr->message.gsmMessage[i]);
    }
  }
} /* qcril_log_print_RIL_IMS_SMS_Message */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_CardStatus_v6

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_CardStatus_v6

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_CardStatus_v6(const char* header, const RIL_CardStatus_v6* ptr)
{
  qcril_log_print_ril_message_payload("%scard_state = %d", header, ptr->card_state);
  qcril_log_print_ril_message_payload("%suniversal_pin_state = %d", header, ptr->universal_pin_state);
  qcril_log_print_ril_message_payload("%sgsm_umts_subscription_app_index = %d",
                               header, ptr->gsm_umts_subscription_app_index);
  qcril_log_print_ril_message_payload("%scdma_subscription_app_index = %d",
                               header, ptr->cdma_subscription_app_index);
  qcril_log_print_ril_message_payload("%sims_subscription_app_index = %d",
                               header, ptr->ims_subscription_app_index);
  qcril_log_print_ril_message_payload("%snum_applications = %d", header, ptr->num_applications);
  int i;
  for (i = 0; i < ptr->num_applications; ++i)
  {
    qcril_log_print_ril_message_payload("%s->applications[%u].app_type = %d",
                                 header, i, ptr->applications[i].app_type);
    qcril_log_print_ril_message_payload("%s->applications[%u].app_state = %d",
                                 header, i, ptr->applications[i].app_state);
    qcril_log_print_ril_message_payload("%s->applications[%u].perso_substate = %d",
                                 header, i, ptr->applications[i].perso_substate);
    if (ptr->applications[i].aid_ptr)
      qcril_log_print_ril_message_payload("%s->applications[%u].aid_ptr %s",
                                   header, i, ptr->applications[i].aid_ptr);
    if (ptr->applications[i].app_label_ptr)
      qcril_log_print_ril_message_payload("%s->applications[%u].app_label_ptr %s",
                                   header, i, ptr->applications[i].app_label_ptr);
    qcril_log_print_ril_message_payload("%s->applications[%u].pin1_replaced = %d",
                                 header, i, ptr->applications[i].pin1_replaced);
    qcril_log_print_ril_message_payload("%s->applications[%u].pin1 = %d",
                                 header, i, ptr->applications[i].pin1);
    qcril_log_print_ril_message_payload("%s->applications[%u].pin2 = %d",
                                 header, i, ptr->applications[i].pin2);
  }
} /* qcril_log_print_RIL_CardStatus_v6 */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_SMS_Response

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_SMS_Response

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_SMS_Response(const char* header, const RIL_SMS_Response* ptr)
{
  qcril_log_print_ril_message_payload("%smessageRef = %d", header, ptr->messageRef);
  if (ptr->ackPDU)
    qcril_log_print_ril_message_payload("%sackPDU %s", header, ptr->ackPDU);
  qcril_log_print_ril_message_payload("%serrorCode = %d", header, ptr->errorCode);
} /* qcril_log_print_RIL_SMS_Response */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_SelectUiccSub

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_SelectUiccSub

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_SelectUiccSub(const char* header, const RIL_SelectUiccSub* ptr)
{
  qcril_log_print_ril_message_payload("%sslot = %d", header, ptr->slot);
  qcril_log_print_ril_message_payload("%sapp_index = %d", header, ptr->app_index);
  qcril_log_print_ril_message_payload("%ssub_type = %d", header, ptr->sub_type);
  qcril_log_print_ril_message_payload("%sact_status = %d", header, ptr->act_status);
} /* qcril_log_print_RIL_SelectUiccSub */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_StkCcUnsolSsResponse

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_StkCcUnsolSsResponse

    @return
    void
*/
/*=========================================================================*/
void qcril_log_print_RIL_StkCcUnsolSsResponse(const char* header, const RIL_StkCcUnsolSsResponse** ptr_a)
{
  const RIL_StkCcUnsolSsResponse* ptr = *ptr_a;
  qcril_log_print_ril_message_payload("%sserviceType = %d", header, ptr->serviceType);
  qcril_log_print_ril_message_payload("%srequestType = %d", header, ptr->requestType);
  qcril_log_print_ril_message_payload("%steleserviceType = %d", header, ptr->teleserviceType);
  qcril_log_print_ril_message_payload("%sserviceClass = %d", header, ptr->serviceClass);
  qcril_log_print_ril_message_payload("%sresult = %d", header, ptr->result);
  unsigned int i;
  for (i = 0; i < SS_INFO_MAX; ++i)
    qcril_log_print_ril_message_payload("%sssInfo[%u] = %d", header, i, ptr->ssInfo[i]);
  qcril_log_print_ril_message_payload("%scfData.numValidIndexes = %d",
                               header, ptr->cfData.numValidIndexes);
  for (i = 0; i < NUM_SERVICE_CLASSES; ++i)
  {
    qcril_log_print_ril_message_payload("%scfData.cfInfo[%u].status = %d",
                                 header, i, ptr->cfData.cfInfo[i].status);
    qcril_log_print_ril_message_payload("%scfData.cfInfo[%u].reason = %d",
                                 header, i, ptr->cfData.cfInfo[i].reason);
    qcril_log_print_ril_message_payload("%scfData.cfInfo[%u].serviceClass = %d",
                                 header, i, ptr->cfData.cfInfo[i].serviceClass);
    qcril_log_print_ril_message_payload("%scfData.cfInfo[%u].toa = %d",
                                 header, i, ptr->cfData.cfInfo[i].toa);
    //TODO: array out of bounds
    if (ptr->cfData.cfInfo[i].number)
      qcril_log_print_ril_message_payload("%scfData.cfInfo[%u].number %s",
                                   header, i, ptr->cfData.cfInfo[i].number);
    qcril_log_print_ril_message_payload("%scfData.cfInfo[%u].timeSeconds = %d",
                                 header, i, ptr->cfData.cfInfo[i].timeSeconds);
  }
} /* qcril_log_print_RIL_StkCcUnsolSsResponse */

/*===========================================================================

  FUNCTION: qcril_log_print_RIL_PCO_Data

===========================================================================*/
/*!
    @brief
    Dump RIL payload if the payload type is RIL_PCO_Data

    @return
    void
*/
/*=========================================================================*/
#if (QCRIL_RIL_VERSION >= 14)
void qcril_log_print_RIL_PCO_Data(const char* header, const RIL_PCO_Data** ptr_a)
{
  const RIL_PCO_Data* ptr = *ptr_a;
  qcril_log_print_ril_message_payload("%scid = %d", header, ptr->cid);
  qcril_log_print_ril_message_payload("%sbearer_proto = %s", header,
          ptr->bearer_proto);
  qcril_log_print_ril_message_payload("%spco_id = %d", header, ptr->pco_id);
  qcril_log_print_ril_message_payload("%scontents_length = %d", header,
          ptr->contents_length);
} /* qcril_log_print_RIL_PCO_Data */
#endif

/*!
    @brief
    Dump RIL payload if the payload type is RIL_CellIdentity_v16

    @return
    void
*/
void qcril_log_print_RIL_CellIdentity_v16(const char* header, const RIL_CellIdentity_v16 *cellIdentity) {
  if(cellIdentity) {
    qcril_log_print_ril_message_payload("%scellIdentity.cellInfoType = %d",
                                                    header, cellIdentity->cellInfoType);
    switch(cellIdentity->cellInfoType) {
      case RIL_CELL_INFO_TYPE_GSM:
        qcril_log_print_ril_message_payload("%scellIdentity.mcc  = %d",
                                                header, cellIdentity->cellIdentityGsm.mcc);
        qcril_log_print_ril_message_payload("%scellIdentity.mnc  = %d",
                                                header, cellIdentity->cellIdentityGsm.mnc);
        qcril_log_print_ril_message_payload("%scellIdentity.lac  = %d",
                                                header, cellIdentity->cellIdentityGsm.lac);
        qcril_log_print_ril_message_payload("%scellIdentity.cid  = %d",
                                                header, cellIdentity->cellIdentityGsm.cid);
        qcril_log_print_ril_message_payload("%scellIdentity.arfcn  = %d",
                                                header, cellIdentity->cellIdentityGsm.arfcn);
        qcril_log_print_ril_message_payload("%scellIdentity.bsic  = %d",
                                                header, cellIdentity->cellIdentityGsm.bsic);
      break;
      case RIL_CELL_INFO_TYPE_CDMA:
        qcril_log_print_ril_message_payload("%scellIdentity.networkId  = %d",
                                                header, cellIdentity->cellIdentityCdma.networkId);
        qcril_log_print_ril_message_payload("%scellIdentity.systemId  = %d",
                                                header, cellIdentity->cellIdentityCdma.systemId);
        qcril_log_print_ril_message_payload("%scellIdentity.basestationId  = %d",
                                                header, cellIdentity->cellIdentityCdma.basestationId);
        qcril_log_print_ril_message_payload("%scellIdentity.longitude  = %d",
                                                header, cellIdentity->cellIdentityCdma.longitude);
        qcril_log_print_ril_message_payload("%scellIdentity.latitude  = %d",
                                                header, cellIdentity->cellIdentityCdma.latitude);
      break;

      case RIL_CELL_INFO_TYPE_LTE:
        qcril_log_print_ril_message_payload("%scellIdentity.mcc  = %d",
                                                header, cellIdentity->cellIdentityLte.mcc);
        qcril_log_print_ril_message_payload("%scellIdentity.mnc  = %d",
                                                header, cellIdentity->cellIdentityLte.mnc);
        qcril_log_print_ril_message_payload("%scellIdentity.ci  = %d",
                                                header, cellIdentity->cellIdentityLte.ci);
        qcril_log_print_ril_message_payload("%scellIdentity.pci  = %d",
                                                header, cellIdentity->cellIdentityLte.pci);
        qcril_log_print_ril_message_payload("%scellIdentity.tac  = %d",
                                                header, cellIdentity->cellIdentityLte.tac);
        qcril_log_print_ril_message_payload("%scellIdentity.earfcn  = %d",
                                                header, cellIdentity->cellIdentityLte.earfcn);
      break;

      case RIL_CELL_INFO_TYPE_WCDMA:
        qcril_log_print_ril_message_payload("%scellIdentity.mcc  = %d",
                                                header, cellIdentity->cellIdentityWcdma.mcc);
        qcril_log_print_ril_message_payload("%scellIdentity.mnc  = %d",
                                                header, cellIdentity->cellIdentityWcdma.mnc);
        qcril_log_print_ril_message_payload("%scellIdentity.lac  = %d",
                                                header, cellIdentity->cellIdentityWcdma.lac);
        qcril_log_print_ril_message_payload("%scellIdentity.cid  = %d",
                                                header, cellIdentity->cellIdentityWcdma.cid);
        qcril_log_print_ril_message_payload("%scellIdentity.psc  = %d",
                                                header, cellIdentity->cellIdentityWcdma.psc);
      break;

      case RIL_CELL_INFO_TYPE_TD_SCDMA:
        qcril_log_print_ril_message_payload("%scellIdentity.mcc  = %d",
                                                header, cellIdentity->cellIdentityTdscdma.mcc);
        qcril_log_print_ril_message_payload("%scellIdentity.mnc  = %d",
                                                header, cellIdentity->cellIdentityTdscdma.mnc);
        qcril_log_print_ril_message_payload("%scellIdentity.lac  = %d",
                                                header, cellIdentity->cellIdentityTdscdma.lac);
        qcril_log_print_ril_message_payload("%scellIdentity.cid  = %d",
                                                header, cellIdentity->cellIdentityTdscdma.cid);
        qcril_log_print_ril_message_payload("%scellIdentity.cpid  = %d",
                                                header, cellIdentity->cellIdentityTdscdma.cpid);
      break;

      default:
      break;
    }
  }
}

/*
 * qcril_log_print_RIL_DataRegistrationStateResponse
 * @brief Dump RIL payload Of Data_Registration RIL request
 *
 *  @return void
*/
void qcril_log_print_RIL_DataRegistrationStateResponse(const char* header,
  const RIL_DataRegistrationStateResponse *data_reg) {
  if(data_reg) {
    //regState
    qcril_log_print_ril_message_payload("%sregState = %d",header, data_reg->regState);
    qcril_log_print_ril_message_payload("%srat = %d",header, data_reg->rat);
    qcril_log_print_ril_message_payload("%sreasonDataDenied = %d",header, data_reg->reasonDataDenied);
    qcril_log_print_ril_message_payload("%smaxDataCalls = %d",header, data_reg->maxDataCalls);
    qcril_log_print_RIL_CellIdentity_v16(header, &data_reg->cellIdentity);
  }
}

/*
 * qcril_log_print_RIL_VoiceRegistrationStateResponse
 * @brief Dump RIL payload Of Voice_Registration RIL request
 *
 *  @return void
*/
void qcril_log_print_RIL_VoiceRegistrationStateResponse(const char *header,
  const RIL_VoiceRegistrationStateResponse *voice_reg) {
  if(voice_reg) {
    qcril_log_print_ril_message_payload("%sregState = %d",header, voice_reg->regState);
    qcril_log_print_ril_message_payload("%srat = %d",header, voice_reg->rat);
    qcril_log_print_ril_message_payload("%scssSupported = %d",header, voice_reg->cssSupported);
    qcril_log_print_ril_message_payload("%sroamingIndicator = %d",header, voice_reg->roamingIndicator);
    qcril_log_print_ril_message_payload("%ssystemIsInPrl = %d",header, voice_reg->systemIsInPrl);
    qcril_log_print_ril_message_payload("%sdefaultRoamingIndicator = %d",header, voice_reg->defaultRoamingIndicator);
    qcril_log_print_ril_message_payload("%sreasonForDenial = %d",header, voice_reg->reasonForDenial);
    qcril_log_print_RIL_CellIdentity_v16(header, &voice_reg->cellIdentity);
  }
}

/*===========================================================================

FUNCTION: qcril_log_print_ril_message

===========================================================================*/
/*!
  @brief
  print content of RIL message name and payload to the log

  @return
  void
  */
/*=========================================================================*/
void qcril_log_print_ril_message(qcril_evt_e_type message_id, RIL__MsgType message_type, void* data,
                                  size_t datalen, RIL_Errno error)
{
  if (!qcril_log_ril_msg_payload_log_on)
  {
    return;
  }
  qcril_log_print_ril_message_payload("%s", "Begin of RIL Message");
  qcril_log_print_ril_message_payload("Message ID: %d, Message Type: %d, Data Length: %u, Error: %d",
                               (unsigned int)message_id, message_type, (unsigned int)datalen, error);
  qcril_log_print_ril_message_payload("Message name: %s", qcril_log_lookup_event_name(message_id));

  int is_request_or_unsol = 0;
  if ((message_type == RIL__MSG_TYPE__REQUEST) || (message_type == RIL__MSG_TYPE__UNSOL_RESPONSE))
  {
    is_request_or_unsol = 1;
  }
  if (data)
  {
    switch (qcril_event_get_android_request(message_id))
    {
      /* Switch Cases are sorted by payload type*/
      /* data = NULL, response = NULL */
      case RIL_REQUEST_ANSWER:
      case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
      case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
      case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED:
      case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
      case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
      case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
      case RIL_REQUEST_CONFERENCE:
      case RIL_UNSOL_UNKOWN:
        break; /* data = NULL, response = NULL */

      /* data = int*, response = NULL */
      case RIL_REQUEST_HANGUP:
      case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
      case RIL_REQUEST_RADIO_POWER:
      case RIL_REQUEST_SEPARATE_CONNECTION:
      case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE:
      case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE:
      case RIL_REQUEST_SCREEN_STATE:
      case RIL_REQUEST_SET_TTY_MODE:
      case RIL_REQUEST_SMS_ACKNOWLEDGE:
      case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED:
      case RIL_UNSOL_SRVCC_STATE_NOTIFY:
        if (is_request_or_unsol == 1)
        {
          int* ptr = (int*) data;
          qcril_log_print_int(ptr);
        }
        break; /* data = int*, response = NULL */

      /* data = NULL, response = int* */
      case RIL_REQUEST_VOICE_RADIO_TECH:
      case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
      case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
      case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE:
      case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE:
      case RIL_REQUEST_IMS_REGISTRATION_STATE:
      case RIL_REQUEST_QUERY_CLIP:
        if (is_request_or_unsol == 0)
        {
          int* ptr = (int*) data;
          qcril_log_print_int(ptr);
        }
        break; /* data = NULL, response = int* */

      /* data = char**, response = int*/
      case RIL_REQUEST_QUERY_FACILITY_LOCK:
        if (is_request_or_unsol == 1)
        {
          char** ptr = (char**) data;
          qcril_log_print_string_array(ptr, 4);
        }
        else
        {
          int* ptr = (int*) data;
          qcril_log_print_int(ptr);
        }
        break;
      case RIL_REQUEST_SET_FACILITY_LOCK:
        if (is_request_or_unsol == 1)
        {
          char** ptr = (char**) data;
          qcril_log_print_string_array(ptr, 5);
        }
        else
        {
          int* ptr = (int*) data;
          qcril_log_print_int(ptr);
        }
        break;
      case RIL_REQUEST_VOICE_REGISTRATION_STATE:
        if (is_request_or_unsol == 0)
        {
#if (QCRIL_RIL_VERSION < 15)
            char** ptr = (char**) data;
            qcril_log_print_string_array(ptr, 15);
#else
            const RIL_VoiceRegistrationStateResponse *ptr =
              (RIL_VoiceRegistrationStateResponse *) data;
            qcril_log_print_RIL_VoiceRegistrationStateResponse(
              "(RIL_VoiceRegistrationStateResponse *)Payload->",ptr);
#endif
        }
        break;
      case RIL_REQUEST_DATA_REGISTRATION_STATE:
        if (is_request_or_unsol == 0)
        {
#if (QCRIL_RIL_VERSION < 15)
            char** ptr = (char**) data;
            qcril_log_print_string_array(ptr, 11);
#else
            const RIL_DataRegistrationStateResponse *ptr =
              (RIL_DataRegistrationStateResponse *) data;
            qcril_log_print_RIL_DataRegistrationStateResponse(
              "(RIL_DataRegistrationStateResponse *)Payload->",ptr);
#endif
        }
        break;
      case RIL_REQUEST_OPERATOR:
        if (is_request_or_unsol == 0)
        {
          char** ptr = (char**) data;
          qcril_log_print_string_array(ptr, 3);
        }
        break;
      case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
        if (is_request_or_unsol == 0)
        {
          char** ptr = (char**) data;
          // TODO: find the number of available networks n. argument should be 4*n
          qcril_log_print_string_array(ptr, 4);
        }
        break; /* data = char**, response = int*/

      /* data = char**, response = NULL */
      case RIL_UNSOL_ON_USSD:
        if (is_request_or_unsol == 1)
        {
          char** ptr = (char**) data;
          qcril_log_print_string_array(ptr, 2);
        }
        break; /* data = char**, response = NULL */


      /* data = RIL_Dial*, response = NULL */
      case RIL_REQUEST_DIAL:
        if (is_request_or_unsol == 1)
        {
          const RIL_Dial* ptr = (const RIL_Dial*) data;
          qcril_log_print_RIL_Dial("(RIL_Dial*)Payload->", ptr);
        }
        break; /* data = RIL_Dial*, response = NULL */

      /* data = NULL, response = RIL_Call** */
      case RIL_REQUEST_GET_CURRENT_CALLS:
        if (is_request_or_unsol == 0)
        {
          const RIL_Call** ptr = (const RIL_Call**) data;
          qcril_log_print_RIL_Call_array(ptr, datalen);
        }
        break; /* data = NULL, response = RIL_Call** */

      /* data = NULL, response = RIL_SignalStrength* */
      case RIL_REQUEST_SIGNAL_STRENGTH:
        if (is_request_or_unsol == 0)
        {
          const RIL_SignalStrength* ptr = (const RIL_SignalStrength*)data;
          qcril_log_print_RIL_SignalStrength("(RIL_SignalStrength*)Payload->", ptr);
        }
        break; /* data = NULL, response = RIL_SignalStrength* */

      /* data = RIL_SignalStrength*, response = NULL */
      case RIL_UNSOL_SIGNAL_STRENGTH:
        if (is_request_or_unsol == 1)
        {
          const RIL_SignalStrength* ptr = (const RIL_SignalStrength*)data;
          qcril_log_print_RIL_SignalStrength("(RIL_SignalStrength*)Payload->", ptr);
        }
        break; /* data = RIL_SignalStrength*, response = NULL */

      /* data = NULL for GSM, data = RIL_CDMA_SignalInfoRecord* for CDMA, response = NULL */
      case RIL_UNSOL_CALL_RING:
        if (is_request_or_unsol == 1)
        {
          const RIL_CDMA_SignalInfoRecord* ptr = (const RIL_CDMA_SignalInfoRecord*) data;
          if (ptr)
            qcril_log_print_RIL_CDMA_SignalInfoRecord("(RIL_CDMA_SignalInfoRecord*)Payload->", ptr);
        }
        break; /* data = NULL for GSM, RIL_CDMA_SignalInfoRecord* for CDMA, response = NULL */

      /* data = RIL_CallForwardInfo*, response = RIL_CallForwardInfo** */
      case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
        if (is_request_or_unsol == 1)
        {
          const RIL_CallForwardInfo* ptr = (RIL_CallForwardInfo*)data;
          qcril_log_print_RIL_CallForwardInfo("(RIL_CallForwardInfo*)Payload->", ptr);
        }
        else
        {
          const RIL_CallForwardInfo** ptr = (const RIL_CallForwardInfo**)data;
          qcril_log_print_RIL_CallForwardInfo_array(ptr, datalen);
        }
        break; /* data = RIL_CallForwardInfo*, response = RIL_CallForwardInfo** */

      /* data = RIL_CallForwardInfo*, response = NULL */
      case RIL_REQUEST_SET_CALL_FORWARD:
        if (is_request_or_unsol == 1)
        {
          const RIL_CallForwardInfo* ptr = (RIL_CallForwardInfo*)data;
          qcril_log_print_RIL_CallForwardInfo("(RIL_CallForwardInfo*)Payload->", ptr);
        }
        break; /* data = RIL_CallForwardInfo*, response = NULL */

      /* data = byte[] or char*, response = NULL */
      case RIL_UNSOL_OEM_HOOK_RAW:
      case RIL_UNSOL_RESPONSE_NEW_SMS:
        if (is_request_or_unsol == 1)
        {
          const char* ptr = (const char*) data;
          qcril_log_print_ril_message_payload("Payload %s", ptr);
        }
        break; /* data = byte[] or char*, response = NULL */

      /* data = char**, response = RIL_SMS_Response* */
      case RIL_REQUEST_SEND_SMS:
        if (is_request_or_unsol == 1)
        {
          char** ptr = (char**) data;
          qcril_log_print_string_array(ptr, 2);
        }
        else
        {
          const RIL_SMS_Response* ptr = (RIL_SMS_Response*)data;
          qcril_log_print_RIL_SMS_Response("(RIL_SMS_Response)Payload->", ptr);
        }
        break; /* data = char**, response = RIL_SMS_Response* */

      /* data = NULL, response = RIL_CardStatus_v6* */
      case RIL_REQUEST_GET_SIM_STATUS:
        if (is_request_or_unsol == 0)
        {
          const RIL_CardStatus_v6* ptr = (RIL_CardStatus_v6*)data;
          qcril_log_print_RIL_CardStatus_v6("(RIL_CardStatus_v6*)Payload->", ptr);
        }
        break; /* data = NULL, response = RIL_CardStatus_v6* */

      /* data = RIL_IMS_SMS_Message*, response = RIL_SMS_Response* */
      case RIL_REQUEST_IMS_SEND_SMS:
        if (is_request_or_unsol == 0)
        {
          const RIL_SMS_Response* ptr = (RIL_SMS_Response*)data;
          qcril_log_print_RIL_SMS_Response("(RIL_SMS_Response)Payload->", ptr);
        }
        else
        {
          const RIL_IMS_SMS_Message* ptr = (RIL_IMS_SMS_Message*)data;
          qcril_log_print_RIL_IMS_SMS_Message("(RIL_IMS_SMS_Message*)Payload->", ptr);
        }
        break; /* data = RIL_IMS_SMS_Message*, response = RIL_SMS_Response* */

      /* data = RIL_SelectUiccSub*, response = NULL */
      case RIL_REQUEST_SET_UICC_SUBSCRIPTION:
        if (is_request_or_unsol == 1)
        {
          const RIL_SelectUiccSub* ptr = (RIL_SelectUiccSub*)data;
          qcril_log_print_RIL_SelectUiccSub("(RIL_SelectUiccSub*)Payload->", ptr);
        }
        break; /* data = RIL_SelectUiccSub*, response = NULL */

      /* data = RIL_StkCcUnsolSsResponse*, response = NULL */
      case RIL_UNSOL_ON_SS:
        if (is_request_or_unsol == 1)
        {
          const RIL_StkCcUnsolSsResponse* ptr = (RIL_StkCcUnsolSsResponse*)data;
          qcril_log_print_RIL_StkCcUnsolSsResponse("(RIL_StkCcUnsolSsResponse*)Payload->", &ptr);
        }
        break; /* data = RIL_StkCcUnsolSsResponse*, response = NULL */

#if (QCRIL_RIL_VERSION >= 14)
      /* data = RIL_PCO_Data */
      case RIL_UNSOL_PCO_DATA:
        if (is_request_or_unsol == 1)
        {
            const RIL_PCO_Data *ptr = (RIL_PCO_Data*)data;
            qcril_log_print_RIL_PCO_Data("(RIL_PCO_Data*)Payload->", &ptr);
        }
        break;
#endif

      default:
        qcril_log_print_ril_message_payload("%s %s", "Unsupported payload printing for RIL Message:",
                                     qcril_log_lookup_event_name(message_id));
        break;
    }
  }
  qcril_log_print_ril_message_payload("%s", "End of RIL Message");
} /* qcril_log_print_ril_message */

/*===========================================================================

FUNCTION: qcril_log_is_additional_log_on

===========================================================================*/
/*!
  @brief
  Checks if additional RIL logging is needed

  @return
  local variable value qcril_log_additional_logging_enabled

  */
/*=========================================================================*/
boolean qcril_log_is_additional_log_on(void)
{
  boolean ret_val;
  ret_val = qcril_log_additional_logging_enabled;
  return ret_val;
} // qcril_log_is_additional_log_on()

/*===========================================================================

FUNCTION: qcril_log_update_early_radio_power_req

===========================================================================*/
/*!
  @brief
  Check if any RADIO_POWER request came on other RILD and notifed to current
  RIL instance.

  @return

  */
/*=========================================================================*/
void qcril_log_update_early_radio_power_req(int is_genuine_signal)
{
    if(is_genuine_signal)
    {
        qcril_log_early_radio_power_multiple_rild_process_timer_valid = TRUE;
        QCRIL_LOG_INFO( "Early radio power req status %d", (int) qcril_log_early_radio_power_multiple_rild_process_timer_valid );
        qcril_log_early_radio_power_multiple_rild_process_complete_num++;
        QCRIL_LOG_INFO( "Number of radio power req status %d", (int) qcril_log_early_radio_power_multiple_rild_process_complete_num );
    }
}

uint8_t qcril_log_early_radio_power_status()
{
    QCRIL_LOG_INFO( "Early radio power req status %d", (int) qcril_log_early_radio_power_multiple_rild_process_timer_valid );
    return qcril_log_early_radio_power_multiple_rild_process_timer_valid;
}

int qcril_log_number_of_early_radio_power_req()
{
    QCRIL_LOG_INFO( "Number of radio power req status %d", (int) qcril_log_early_radio_power_multiple_rild_process_complete_num );
    return qcril_log_early_radio_power_multiple_rild_process_complete_num;
}

void qcril_log_reset_early_radio_power_req()
{
    qcril_log_early_radio_power_multiple_rild_process_timer_valid = FALSE;
    qcril_log_early_radio_power_multiple_rild_process_complete_num = 0;
}


