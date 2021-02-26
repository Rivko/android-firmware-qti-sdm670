
/*===========================================================================

  Copyright (c) 2011-2012, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/12   at      Updated USB filename for ICS
02/02/12   at      Updated function ccid_uim_send_sap_request
10/20/11   at      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "comdef.h"
#include "user_identity_module_v01.h"
#include "qmi_idl_lib.h"
#include "ccid_uim_daemon.h"
#include "ccid_uim_parsing.h"


/*===========================================================================

                           MACROS

===========================================================================*/

/* Logging macros */
#define LOGI(...) fprintf(stderr, "INFO:  " __VA_ARGS__)
#define LOGE(...) fprintf(stderr, "ERROR: " __VA_ARGS__)

/* USB specific filename info */
#define CCID_UIM_USB_STATE_FILE                    "/sys/class/android_usb/android0/state"
#define CCID_UIM_USB_CONFIG_STR                    "CONFIGURED"
#define CCID_UIM_USB_CONFIG_STR_MAXSIZE            64

/* Android TTY device nodes for the USB CCID Driver */
#define CCID_UIM_TTY_DEVNODE_STR_BULK              "/dev/ccid_bulk"
#define CCID_UIM_TTY_DEVNODE_STR_CTRL              "/dev/ccid_ctrl"

/* Bits per slot per CCID spec */
#define CCID_UIM_NOTIFY_CARD_BITS_PER_SLOT         2
#define CCID_UIM_NOTIFY_CARD_SLOT_STATE            0x01
#define CCID_UIM_NOTIFY_CARD_STATUS_CHANGED        0x02

/* Event masks */
#define CCID_UIM_QMI_EVENT_MASK_CARD_STATUS        (1 << QMI_UIM_EVENT_CARD_STATUS_BIT_V01)
#define CCID_UIM_QMI_EVENT_MASK_SAP_CONN           (1 << QMI_UIM_EVENT_SAP_CONNECTION_BIT_V01)

/* Parameter values in IOCTLs */
#define CCID_UIM_IOCTL_PARAM_NOTIFY_SLOT_CHANGE    0x50
#define CCID_UIM_IOCTL_PARAM_HARDWARE_ERROR        0x51
#define CCID_UIM_IOCTL_PARAM_DTR_CLEARED           0x00
#define CCID_UIM_IOCTL_PARAM_DTR_SET               0x01

/* Default timeout (in milli-seconds) for synchronous QMI message */
#define CCID_UIM_QMI_DEFAULT_TIMEOUT               20000


/* Read size for USB event */
#define CCID_UIM_USB_EVT_BUFFER_SIZE               1024

/* Macros for critical sections */
#define CCID_UIM_ENTER_CRITICAL_SECTION( mutex, log_str )    \
  {                                                          \
    pthread_mutex_lock( mutex );                             \
  }

#define CCID_UIM_LEAVE_CRITICAL_SECTION( mutex, log_str )    \
  {                                                          \
    pthread_mutex_unlock( mutex );                           \
  }

#define QMI_UIM_INIT_TIMEOUT    4

/*===========================================================================

                           GLOBALS

===========================================================================*/

/* Global variable with the card status */
ccid_uim_struct_type    ccid_uim;

/* Global mutex */
static pthread_mutex_t  ccid_uim_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Forward declarations */
static void ccid_qmi_client_indication_cb
(
  qmi_client_type                user_handle,
  unsigned long                  msg_id,
  unsigned char                * ind_buf_ptr,
  int                            ind_buf_len,
  void                         * ind_cb_data
);

static void ccid_qmi_client_async_cb
(
  qmi_client_type                user_handle,
  unsigned long                  msg_id,
  void                         * qmi_response_ptr,
  int                            resp_c_struct_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
);

static qmi_client_os_params  os_params;

/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/


/*=========================================================================

  FUNCTION:  ccid_uim_bulk_thread

===========================================================================*/
/*!
    @brief
    Thread routine that handles read from the BULK device node.

    @return
    None.
*/
/*=========================================================================*/
static void ccid_uim_bulk_thread
(
  void * in_param
)
{
  /* Retrieve global UIM struct pointer used in this thread */
  ccid_uim_struct_type   * ccid_uim_ptr = (ccid_uim_struct_type *)in_param;
  uint8                    rx_buffer[CCID_UIM_BULKOUT_BUFFER_SIZE];

  assert(ccid_uim_ptr != NULL);

  /* Infinite loop blocked on read */
  while (1)
  {
    int fd         = -1;
    int bytes_read = 0;

    memset(&rx_buffer, 0, CCID_UIM_BULKOUT_BUFFER_SIZE);

    CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");
    fd = ccid_uim_ptr->dev_node[0].fd;
    CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

    fcntl(fd, F_SETFL, 0);

    /* Blocking read for BULKOUT message */
    bytes_read = read (fd,
                       (void *) rx_buffer,
                       (size_t) CCID_UIM_BULKOUT_BUFFER_SIZE);

    LOGI("ccid_uim_bulk_thread: bytes_read: 0x%x  0x%x \n", bytes_read,(size_t) CCID_UIM_BULKOUT_BUFFER_SIZE);

    /* Make sure to enter critical section only after unblocking from read() */
    CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

    if ((bytes_read > 0) && (bytes_read <= (int)CCID_UIM_BULKOUT_BUFFER_SIZE))
    {
      /* Parse the read data */
      ccid_uim_process_pc_to_rdr_bulk(ccid_uim_ptr->dev_node[0].fd,
                                      rx_buffer,
                                      (uint16)bytes_read);
    }
    else if(bytes_read <= 0)
    {
      /* Close fd & exit the thread */
      close(ccid_uim_ptr->dev_node[0].fd);
      ccid_uim_ptr->dev_node[0].fd        = -1;
      ccid_uim_ptr->dev_node[0].thread_id = -1;
      break;
    }
    CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");
  }

  LOGE("Exiting ccid_uim_bulk_thread ... \n");
} /* ccid_uim_bulk_thread */


/*=========================================================================

  FUNCTION:  ccid_uim_ctrl_thread

===========================================================================*/
/*!
    @brief
    Thread routine that handles read from the CTRL device node.

    @return
    None.
*/
/*=========================================================================*/
static void ccid_uim_ctrl_thread
(
  void * in_param
)
{
  /* Retrieve global UIM struct pointer used in this thread */
  ccid_uim_struct_type   * ccid_uim_ptr = (ccid_uim_struct_type *)in_param;
  uint8                    rx_buffer[CCID_UIM_CTRL_BUFFER_SIZE];

  assert(ccid_uim_ptr != NULL);

  /* Infinite loop blocked on read */
  while (1)
  {
    int fd         = -1;
    int bytes_read = 0;

    memset(&rx_buffer, 0, CCID_UIM_CTRL_BUFFER_SIZE);

    CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");
    fd = ccid_uim_ptr->dev_node[1].fd;
    CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

    fcntl(fd, F_SETFL, 0);

    /* Blocking read for CTRL message */
    bytes_read = read (fd,
                       (void *) rx_buffer,
                       (size_t) CCID_UIM_CTRL_BUFFER_SIZE);

    LOGI("ccid_uim_ctrl_thread: bytes_read: 0x%x \n", bytes_read);

    /* Make sure to enter critical section only after unblocking from read() */
    CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

    if ((bytes_read > 0) && (bytes_read <= CCID_UIM_CTRL_BUFFER_SIZE))
    {
      /* Parse the read data */
      ccid_uim_process_pc_to_rdr_ctrl(ccid_uim_ptr->dev_node[1].fd,
                                      rx_buffer,
                                      (uint16)bytes_read);
    }
    else if(bytes_read <= 0)
    {
      /* Close fd & exit the thread */
      close(ccid_uim_ptr->dev_node[1].fd);
      ccid_uim_ptr->dev_node[1].fd        = -1;
      ccid_uim_ptr->dev_node[1].thread_id = -1;
      break;
    }
    CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");
  }

  LOGE("Exiting ccid_uim_ctrl_thread ... \n");
} /* ccid_uim_ctrl_thread */


/*=========================================================================

  FUNCTION:  ccid_uim_send_write

===========================================================================*/
/*!
    @brief
    Performs the write command on the specified device node.

    @return
    None.
*/
/*=========================================================================*/
void ccid_uim_send_write
(
  int                           fd,
  uint8                       * tx_buffer_ptr,
  uint16                        tx_buffer_len
)
{
  int       bytes_written = 0;

  if (fd == -1)
  {
    LOGE("Invalid fd, not sending write \n");
    return;
  }

  if (ccid_uim.usb_state != CCID_UIM_USB_STATE_CONNECTED)
  {
    LOGE("usb not connected, not sending write \n");
    return;
  }

  if ((tx_buffer_ptr == NULL) || (tx_buffer_len == 0))
  {
    LOGE("Invalid input: tx_buffer_ptr = %p, tx_buffer_len: 0x%x \n",
         tx_buffer_ptr, tx_buffer_len);
    return;
  }

  /* Send the data */
  while ((bytes_written = write(fd, tx_buffer_ptr, tx_buffer_len)) != -1)
  {
    /* Check if there was en error */
    if (bytes_written < 0)
    {
      LOGE("Error writing on fd: 0x%x, bytes_written: 0x%x\n", fd, bytes_written);
      break;
    }

    /* Check if the entire data was sent */
    if (bytes_written >= tx_buffer_len)
    {
      LOGI("Successfully written (0x%x) bytes on fd: 0x%x \n", bytes_written, fd);
      break;
    }

    /* Shift the data and continue sending */
    tx_buffer_ptr += bytes_written;
    tx_buffer_len -= bytes_written;
  }
} /* ccid_uim_send_write */


/*=========================================================================

  FUNCTION:  ccid_uim_send_ioctl

===========================================================================*/
/*!
    @brief
    Sends the IOCTL on the specified device node regarding the card state.
    The card state that is sent is based on the internal SAP state.

    @return
    None.
*/
/*=========================================================================*/
static void ccid_uim_send_ioctl
(
  int                           fd,
  int                           ioctl_type,
  uint8                       * tx_buffer_ptr,
  uint16                        tx_buffer_len
)
{
  if (fd == -1)
  {
    LOGE("Invalid fd, not sending IOCTL \n");
    return;
  }

  if ((tx_buffer_ptr == NULL) || (tx_buffer_len == 0))
  {
    LOGE("Invalid input: tx_buffer_ptr = %p, tx_buffer_len: 0x%x \n",
         tx_buffer_ptr, tx_buffer_len);
    return;
  }

  /* Send IOCTL */
  if (ioctl(fd, ioctl_type, tx_buffer_ptr) == -1)
  {
    LOGE("Could not send IOCTL ! \n");
    return;
  }
} /* ccid_uim_send_ioctl */


/*=========================================================================

  FUNCTION:  ccid_uim_open_device_nodes

===========================================================================*/
/*!
    @brief
    Opens predefined USB TTY device nodes. If successful, create respective
    threads for reading data on those nodes. Upon failure, respective device
    node is cleaned - fd is closed, thread is terminated & globals updated.

    @return
    TRUE if successful, else FALSE
*/
/*=========================================================================*/
static boolean ccid_uim_open_device_nodes
(
  void
)
{
  /* First open BULK node */
  if (ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].fd == -1)
  {
    ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].fd =
        open(CCID_UIM_TTY_DEVNODE_STR_BULK, O_RDWR);
    if (ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].fd == -1)
    {
      LOGE("Could not open CCID_UIM_TTY_DEVNODE_STR_BULK ! \n");
      return FALSE;
    }

    /* Next create thread for read of BULK device node */
    if (pthread_create(&ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].thread_id,
                       NULL,
                       (void*)ccid_uim_bulk_thread,
                       (void*)&ccid_uim) != 0)
    {
      LOGE("Could not start ccid_uim_bulk_thread ! \n");
      close(ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].fd);
      ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].fd        = -1;
      ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].thread_id = -1;
      return FALSE;
    }
  }

  /* Next open CTRL node */
  if (ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd == -1)
  {
    ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd =
        open(CCID_UIM_TTY_DEVNODE_STR_CTRL, O_RDWR);
    if (ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd == -1)
    {
      LOGE("Could not open CCID_UIM_TTY_DEVNODE_STR_CTRL ! \n");
      return FALSE;
    }

    /* Next create thread for read of CTRL device node */
    if (pthread_create(&ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].thread_id,
                       NULL,
                       (void*)ccid_uim_ctrl_thread,
                       (void*)&ccid_uim) != 0)
    {
      LOGE("Could not start ccid_uim_ctrl_thread ! \n");
         close(ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd);
      ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd        = -1;
      ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].thread_id = -1;
      return FALSE;
    }
  }

  return TRUE;
} /* ccid_uim_open_device_nodes */


/*=========================================================================

  FUNCTION:  ccid_uim_send_sap_connect

===========================================================================*/
/*!
  @brief
    Sends the SAP connect/disconnect QMI request.

  @return
    None
*/
/*=========================================================================*/
static boolean ccid_uim_send_sap_connect
(
  uint8                           card_index,
  boolean                         is_connect
)
{
  qmi_txn_handle                        txn_handle;
  qmi_client_error_type                 qmi_err_code          = 0;
  uim_sap_connection_resp_msg_v01     * qmi_response_ptr      = NULL;
  uim_sap_connection_req_msg_v01        sap_connect_params;

  qmi_response_ptr = (uim_sap_connection_resp_msg_v01*)malloc(sizeof(uim_sap_connection_resp_msg_v01));
  if(qmi_response_ptr == NULL)
  {
    LOGE("Couldn't allocate memory for qmi_response_ptr ! \n");
    return FALSE;
  }

  /* Init parameters */
  memset(qmi_response_ptr, 0, sizeof(uim_sap_connection_resp_msg_v01));
  memset(&sap_connect_params, 0, sizeof(uim_sap_connection_req_msg_v01));

  /* Update parameters */
  if (is_connect)
  {
    sap_connect_params.sap_connect.connect             = UIM_SAP_OP_CONNECT_V01;
    sap_connect_params.intermediate_get_response_valid = TRUE;
    sap_connect_params.intermediate_get_response       = 0x01;
  }
  else
  {
    sap_connect_params.sap_connect.connect   = UIM_SAP_OP_DISCONNECT_V01;
    sap_connect_params.disconnect_mode_valid = TRUE;
    sap_connect_params.disconnect_mode       = UIM_SAP_DISCONNECT_GRACEFULL_V01;
  }

  switch (card_index)
  {
    case 0:
      sap_connect_params.sap_connect.slot = UIM_SLOT_1_V01;
      break;
    case 1:
      sap_connect_params.sap_connect.slot = UIM_SLOT_2_V01;
      break;
    default:
      LOGE("Couldn't allocate memory for qmi_response_ptr ! \n");
      free(qmi_response_ptr);
      return FALSE;
  }

  qmi_err_code = qmi_client_send_msg_async(ccid_uim.qmi_uim_svc_client_ptr,
                                           QMI_UIM_SAP_CONNECTION_REQ_V01,
                                           (void*) &sap_connect_params,
                                           sizeof(uim_sap_connection_req_msg_v01),
                                           (void*) qmi_response_ptr,
                                           sizeof(*qmi_response_ptr),
                                           ccid_qmi_client_async_cb,
                                           NULL,
                                           &txn_handle);

  if(qmi_err_code == QMI_NO_ERR)
  {
    return TRUE;
  }

  /* On error free the allocated response buffer */
  LOGE("Error for QMI_UIM_SAP_CONNECTION_REQ_V01, qmi_err_code: 0x%x \n", qmi_err_code);
  if (qmi_response_ptr)
  {
    free(qmi_response_ptr);
    qmi_response_ptr = NULL;
  }
  return FALSE;
} /* ccid_uim_send_sap_connect */


/*=========================================================================

  FUNCTION:  ccid_uim_send_sap_request

===========================================================================*/
/*!
  @brief
    Sends the SAP APDU QMI request.

  @return
    None
*/
/*=========================================================================*/
boolean ccid_uim_send_sap_request
(
  uim_sap_request_req_msg_v01   * sap_request_req_ptr,
  ccid_uim_cmd_info_type        * cmd_info_ptr,
  boolean                         is_async
)
{
  qmi_txn_handle                        txn_handle;
  qmi_client_error_type                 qmi_err_code          = 0;
  uim_sap_request_resp_msg_v01        * sap_request_resp_ptr  = NULL;

  assert (sap_request_req_ptr != NULL);

  sap_request_resp_ptr =
    (uim_sap_request_resp_msg_v01*)malloc(sizeof(uim_sap_request_resp_msg_v01));
  if(sap_request_resp_ptr == NULL)
  {
    LOGE("Couldn't allocate memory for sap_request_resp_ptr ! \n");
    return FALSE;
  }

  /* Init parameters */
  memset(sap_request_resp_ptr, 0, sizeof(uim_sap_request_resp_msg_v01));

  if (is_async)
  {
    qmi_err_code = qmi_client_send_msg_async(ccid_uim.qmi_uim_svc_client_ptr,
                                             QMI_UIM_SAP_REQUEST_REQ_V01,
                                             (void*) sap_request_req_ptr,
                                             sizeof(*sap_request_req_ptr),
                                             (void*) sap_request_resp_ptr,
                                             sizeof(*sap_request_resp_ptr),
                                             ccid_qmi_client_async_cb,
                                             cmd_info_ptr,
                                             &txn_handle);
  }
  else
  {
    qmi_err_code = qmi_client_send_msg_sync(ccid_uim.qmi_uim_svc_client_ptr,
                                            QMI_UIM_SAP_REQUEST_REQ_V01,
                                            (void*) sap_request_req_ptr,
                                            sizeof(*sap_request_req_ptr),
                                            (void*) sap_request_resp_ptr,
                                            sizeof(*sap_request_resp_ptr),
                                            CCID_UIM_QMI_DEFAULT_TIMEOUT);
  }

  if(qmi_err_code == QMI_NO_ERR)
  {
    return TRUE;
  }

  /* On error free the allocated response buffer */
  LOGE("Error for QMI_UIM_SAP_REQUEST_REQ_V01, qmi_err_code: 0x%x \n", qmi_err_code);
  if (sap_request_resp_ptr)
  {
    free(sap_request_resp_ptr);
    sap_request_resp_ptr = NULL;
  }
  return FALSE;
} /* ccid_uim_send_sap_request */


/*=========================================================================

  FUNCTION:  ccid_uim_process_ind_card_status

===========================================================================*/
/*!
  @brief
    Processes the card indication by updating the global if new card state
    was detected. Note - need to add functionality to voif race condtions
    for sending qmi_uim_sap_connect if card presence was detected, (do
    nothing if we get a card error - we get card error here only when card
    is in non-CCID mode)

  @return
    None
*/
/*=========================================================================*/
static void ccid_uim_process_ind_card_status
(
  uim_status_change_ind_msg_v01   * status_change_ind_ptr
)
{
  uint8   i = 0;

  assert(status_change_ind_ptr != NULL);

  if (!status_change_ind_ptr->card_status_valid)
  {
    LOGE("card_status is not valid, ignoring card_status indication \n");
    return;
  }

  /* Update global in case of new card state or error code */
  for (i = 0;
       i< QMI_UIM_CARDS_MAX_V01 && i < status_change_ind_ptr->card_status.card_info_len;
       i++)
  {
    LOGI("process_ind_card_status: card_info[%d].card_state: 0x%x, error_code: 0x%x\n",
         i,
         status_change_ind_ptr->card_status.card_info[i].card_state,
         status_change_ind_ptr->card_status.card_info[i].error_code);

    if (status_change_ind_ptr->card_status.card_info[i].card_state !=
        ccid_uim.card_info[i].card_state)
    {
      ccid_uim.card_info[i].card_state =
          status_change_ind_ptr->card_status.card_info[i].card_state;
    }
    if (status_change_ind_ptr->card_status.card_info[i].error_code !=
        ccid_uim.card_info[i].card_error_code)
    {
      ccid_uim.card_info[i].card_error_code =
          status_change_ind_ptr->card_status.card_info[i].error_code;
    }
  }
} /* ccid_uim_process_ind_card_status */


/*=========================================================================

  FUNCTION:  ccid_uim_process_ind_sap_connection

===========================================================================*/
/*!
  @brief
    Processes SAP indication by the following steps:
      i.   Update global if new SAP state was detected
      ii.  Send IOCTL if SAP is successfully connected or card error when
           already in a CCID mode

  @return
    None
*/
/*=========================================================================*/
static void ccid_uim_process_ind_sap_connection
(
  uim_sap_connection_ind_msg_v01   * sap_conn_ind_ptr
)
{
  uint8                           slot_index = 0;
  struct ccid_uim_notification    tx_buffer;

  assert(sap_conn_ind_ptr != NULL);

  if (!sap_conn_ind_ptr->sap_connection_event_valid)
  {
    LOGE("sap_status is not valid, ignoring sap_connection indication \n");
    return;
  }

  LOGI("process_ind_sap_connection: slot: 0x%x, sap_state: 0x%x\n",
       sap_conn_ind_ptr->sap_connection_event.slot,
       sap_conn_ind_ptr->sap_connection_event.sap_state);

  /* Parse the slot first */
  switch (sap_conn_ind_ptr->sap_connection_event.slot)
  {
    case UIM_SLOT_1_V01:
      slot_index = 0;
      break;

    case UIM_SLOT_2_V01:
      slot_index = 1;
      break;

    default:
      LOGE("slot is not valid, ignoring sap_connection indication \n");
      return;
  }

  /* Update global in case of new SAP state, send IOCTL on CTRL device node */
  if (sap_conn_ind_ptr->sap_connection_event.sap_state != ccid_uim.card_info[slot_index].sap_state)
  {
    ccid_uim.card_info[slot_index].sap_state = sap_conn_ind_ptr->sap_connection_event.sap_state;

    if (ccid_uim.usb_state != CCID_UIM_USB_STATE_CONNECTED)
    {
      LOGE("USB not connected, skipping IOCTL \n");
      return;
    }

    /* Contruct the buffer for notify card IOCTL */
    memset(&tx_buffer, 0, sizeof(struct ccid_uim_notification));

    /* Note on data encoding per CCID Spec:
       message_type: 1 byte
       bmSlotICCState: 2 bits per slot rounded up to the nearest byte
        bit 0 = slot0 current state,  values: no ICC present = 0, ICC present = 1
        bit 1 = slot0 changed status, values: no change = 0, change = 1 */

    /* Update the message type byte */
    tx_buffer.buf[0] |= CCID_UIM_IOCTL_PARAM_NOTIFY_SLOT_CHANGE;

    /* Update the slot x changed status bit */
    tx_buffer.buf[1] |= CCID_UIM_NOTIFY_CARD_STATUS_CHANGED <<
                          (slot_index * CCID_UIM_NOTIFY_CARD_BITS_PER_SLOT);

    /* Update the slot x current state bit */
    switch (ccid_uim.card_info[slot_index].sap_state)
    {
      case UIM_SAP_STATE_CONNECTED_SUCCESSFULLY_V01:
        tx_buffer.buf[1] |= CCID_UIM_NOTIFY_CARD_SLOT_STATE <<
                              (slot_index * CCID_UIM_NOTIFY_CARD_BITS_PER_SLOT);
       LOGE("Notifying Card Status change \n");
        break;
      case UIM_SAP_STATE_DISCONNECTED_SUCCESSFULLY_V01:
      case UIM_SAP_STATE_CONNECTION_ERROR_V01:
        /* Nothing to do, we have already memset above for current state bit */
        LOGE("SAP disconnected mes \n");
        break;
      default:
        LOGI("Not sending IOCTL, sap_state: 0x%x \n",
             ccid_uim.card_info[slot_index].sap_state);
        return;
    }

    LOGI("tx_buffer.buf[1]: 0x%x \n", tx_buffer.buf[1]);

    /* Send the IOCTL */
    ccid_uim_send_ioctl(ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd,
                        CCID_NOTIFY_CARD,
                        (uint8*)&tx_buffer.buf,
                        sizeof(struct ccid_uim_notification));
  }
} /* ccid_uim_process_ind_sap_connection */


/*=========================================================================

  FUNCTION:  ccid_uim_process_state_usb_disconnected

===========================================================================*/
/*!
  @brief
    Processes the USB connected state by the following steps:
      i.   Currently, we only update the global USB state

  @return
    None
*/
/*=========================================================================*/
static void ccid_uim_process_state_usb_disconnected
(
  void
)
{
  int rx_buffer = 0;

  /* Check if DTR is cleared by the CCID Driver */
  LOGE("SAP disconnected notify DTR change \n");
  ccid_uim_send_ioctl(ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd,
                      CCID_READ_DTR,
                      (uint8*)&rx_buffer,
                      sizeof(int));

  LOGI("DTR value returned: 0x%x\n", rx_buffer);

  if(rx_buffer != CCID_UIM_IOCTL_PARAM_DTR_CLEARED)
  {
    LOGE("DTR not cleared, ignoring USB state \n");
    return;
  }

  /* We dont exit SAP mode if USB cable is disconnected at this point,
     hence do nothing */

} /* ccid_uim_process_state_usb_disconnected */


/*=========================================================================

  FUNCTION:  ccid_uim_process_state_usb_connected

===========================================================================*/
/*!
  @brief
    Processes the USB connected state by the following steps (if card
    presence is detected):
      i.   Open both the device nodes & Spawn 2 threads for read
      ii.  Send qmi_uim_sap_connect

  @return
    None
*/
/*=========================================================================*/
static void ccid_uim_process_state_usb_connected
(
  void
)
{
  uint8     card_index   = 0;
  int       rx_buffer    = 0;

  /* If card is present, open both the device nodes */
  for (card_index = 0; card_index< QMI_UIM_CARDS_MAX_V01; card_index++)
  {
    LOGI("card_info[card_index].card_state: 0x%x, sap_state: 0x%x \n",
         ccid_uim.card_info[card_index].card_state,
         ccid_uim.card_info[card_index].sap_state);

    if (ccid_uim.card_info[card_index].card_state == UIM_CARD_STATE_PRESENT_V01)
    {
      /* Check if we need to open the device nodes */
      if ((ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].fd == -1) ||
          (ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd == -1))
      {
        /* Open both the device nodes, and create the threads to read */
        if (ccid_uim_open_device_nodes() == FALSE)
        {
          LOGE("Could not open device nodes! \n");
          return;
        }
      }

      /* Check if DTR is set by the CCID Driver */
      ccid_uim_send_ioctl(ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_CTRL].fd,
                          CCID_READ_DTR,
                          (uint8*)&rx_buffer,
                          sizeof(int));

      LOGI("DTR value returned: 0x%x\n", rx_buffer);

      if(rx_buffer != CCID_UIM_IOCTL_PARAM_DTR_SET)
      {
        LOGE("DTR not set, ignoring USB state \n");
        return;
      }

      /* Check SAP state */
      if (ccid_uim.card_info[card_index].sap_state ==
            UIM_SAP_STATE_CONNECTED_SUCCESSFULLY_V01)
      {
        LOGI("sap_state is already connected, ignoring USB state\n");
        return;
      }

      /* Send SAP connect QMI request */
      (void)ccid_uim_send_sap_connect(card_index, TRUE);
    }
  }

  return;
} /* ccid_uim_process_state_usb_connected */


/*=========================================================================

  FUNCTION:  ccid_uim_init_usb_socket

===========================================================================*/
/*!
    @brief
    Initializes the Linux Netlink socket over which USB kernel events can
    be listened to.

    @return
    None.
*/
/*=========================================================================*/
static boolean ccid_uim_init_usb_socket
(
  int     * usb_event_sd_ptr
)
{
  struct sockaddr_nl    netlink_addr;

  assert(usb_event_sd_ptr != NULL);

  /* Create the netlink socket */
  *usb_event_sd_ptr = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
  if (*usb_event_sd_ptr == -1)
  {
    LOGE("Unable to create USB event socket: %s\n", strerror(errno));
    return FALSE;
  }

  /* Init the socket address */
  memset(&netlink_addr, 0, sizeof(struct sockaddr_nl));
  netlink_addr.nl_family = AF_NETLINK;
  netlink_addr.nl_pid    = getpid();
  netlink_addr.nl_groups = 0xFFFFFFFF;

  /* Bind the socket to the address */
  if (bind(*usb_event_sd_ptr, (struct sockaddr *)&netlink_addr, sizeof(netlink_addr)) < 0)
  {
    LOGE("Unable to bind USB event socket: %s\n", strerror(errno));
    close(*usb_event_sd_ptr);
    *usb_event_sd_ptr = -1;
    return FALSE;
  }

  return TRUE;
} /* ccid_uim_init_usb_socket */


/*=========================================================================

  FUNCTION:  ccid_uim_get_usbstate_from_sysfile

===========================================================================*/
/*!
    @brief
    Determines the USB state by parsing the read data from the system file
    for USB state. Input USB state pointer is updated after parsing.

    @return
    Appropriate USB state.
*/
/*=========================================================================*/
static ccid_uim_usb_state_type ccid_uim_get_usbstate_from_sysfile
(
  void
)
{
  int                       fd = -1;
  char                      buffer[CCID_UIM_USB_CONFIG_STR_MAXSIZE];
  ccid_uim_usb_state_type   usb_state = CCID_UIM_USB_STATE_UNKNOWN;

  /* Open the file */
  fd = open(CCID_UIM_USB_STATE_FILE, O_RDONLY);
  if(fd < 0)
  {
    LOGE("Error opening %s: %s\n", CCID_UIM_USB_STATE_FILE, strerror(errno));

    /* USB device is not registered yet. Treat this as an unplugged state.
       We will be woken up by the event later when connected */
    return CCID_UIM_USB_STATE_DISCONNECTED;
  }

  /* Parse the buffer */
  memset(buffer, 0, CCID_UIM_USB_CONFIG_STR_MAXSIZE);
  if (read(fd, buffer, CCID_UIM_USB_CONFIG_STR_MAXSIZE - 1) < 0)
  {
    LOGE("Error reading %s: %s\n", CCID_UIM_USB_STATE_FILE, strerror(errno));
    close(fd);
    return CCID_UIM_USB_STATE_ERROR;
  }

  LOGI("Read config string: %s \n", buffer);

  /* If read string is anything other than CCID_UIM_USB_CONFIG_STR,
     treat it as an unplugged state */
  if(!memcmp(buffer, CCID_UIM_USB_CONFIG_STR, strlen(CCID_UIM_USB_CONFIG_STR)))
  {
    usb_state = CCID_UIM_USB_STATE_CONNECTED;
  }
  else
  {
    usb_state = CCID_UIM_USB_STATE_DISCONNECTED;
  }

  LOGI("usb_state: 0x%x \n", usb_state);
  close(fd);

  return usb_state;
} /* ccid_uim_get_usbstate_from_sysfile */


/*=========================================================================

  FUNCTION:  ccid_uim_get_usbstate_from_event

===========================================================================*/
/*!
    @brief
    Determines the USB state by parsing the read data from the buffer
    provided. Input USB state pointer is updated after parsing only if
    appropriate USB string names are found.

    @return
    None.
*/
/*=========================================================================*/
static void ccid_uim_get_usbstate_from_event
(
  uint8                       * buffer_ptr,
  uint16                        buffer_len,
  ccid_uim_usb_state_type     * usb_state_ptr
)
{
  uint8     * current_ptr        = buffer_ptr;
  uint8     * end_ptr            = current_ptr + buffer_len;
  uint8     * subsystem_ptr      = NULL;
  uint8     * switch_name_ptr    = NULL;
  uint8     * switch_state_ptr   = NULL;

  assert(usb_state_ptr != NULL);

  if ((buffer_ptr == NULL) ||
      (buffer_len == 0) ||
      (buffer_len >= CCID_UIM_USB_EVT_BUFFER_SIZE))
  {
    LOGE("Invalid input: buffer_ptr = %p, buffer_len: 0x%x \n",
         buffer_ptr, buffer_len);
    return;
  }

  LOGI("Read config string: %s , length: 0x%x \n", buffer_ptr, buffer_len);

  /* Skip path */
  current_ptr += strlen((const char*)current_ptr) + 1;

  /* Loop through the buffer & note the needed strings */
  while (current_ptr < end_ptr)
  {
    if (!strncmp((const char*)current_ptr, "SUBSYSTEM=", strlen("SUBSYSTEM=")))
    {
      subsystem_ptr = current_ptr + strlen("SUBSYSTEM=");
    }
    else if (!strncmp((const char*)current_ptr, "SWITCH_NAME=", strlen("SWITCH_NAME=")))
    {
      switch_name_ptr = current_ptr + strlen("SWITCH_NAME=");
    }
    else if (!strncmp((const char*)current_ptr, "SWITCH_STATE=", strlen("SWITCH_STATE=")))
    {
      switch_state_ptr = current_ptr + strlen("SWITCH_STATE=");
    }

    current_ptr += strlen((const char*)current_ptr) + 1;
  }

  /* Parse the buffer for USB state */
  if (subsystem_ptr && switch_name_ptr && switch_state_ptr &&
      !strcmp((const char*)subsystem_ptr, "switch") &&
      !strcmp((const char*)switch_name_ptr, "usb_configuration"))
  {
    switch (switch_state_ptr[0])
    {
      case '0':
        *usb_state_ptr = CCID_UIM_USB_STATE_DISCONNECTED;
        break;
      case '1':
        *usb_state_ptr = CCID_UIM_USB_STATE_CONNECTED;
        break;
      default:
        *usb_state_ptr = CCID_UIM_USB_STATE_ERROR;
        break;
    }
    LOGI("New usb_state: 0x%x \n", *usb_state_ptr);
  }
  else
  {
    LOGI("Ignoring event \n");
  }
} /* ccid_uim_get_usbstate_from_event */


/*=========================================================================

  FUNCTION:  ccid_uim_qmi_register_events

===========================================================================*/
/*!
    @brief
    Registers for Card status events.

    @return
    TRUE if successful, else FALSE
*/
/*=========================================================================*/
static boolean ccid_uim_qmi_register_events
(
  void
)
{
  uint8                               i                 = 0;
  qmi_client_error_type               qmi_err_code      = 0;
  uim_get_card_status_resp_msg_v01  * qmi_response_ptr  = NULL;
  uim_event_reg_req_msg_v01           event_reg_params;

  qmi_response_ptr = (uim_get_card_status_resp_msg_v01*)malloc(sizeof(uim_get_card_status_resp_msg_v01));
  if(qmi_response_ptr == NULL)
  {
    LOGE("Couldn't allocate memory for qmi_response_ptr ! \n");
    return FALSE;
  }

  /* Register for events first */
  memset(qmi_response_ptr, 0, sizeof(uim_get_card_status_resp_msg_v01));
  memset(&event_reg_params, 0, sizeof(uim_event_reg_req_msg_v01));

  event_reg_params.event_mask |= CCID_UIM_QMI_EVENT_MASK_CARD_STATUS |
                                   CCID_UIM_QMI_EVENT_MASK_SAP_CONN;

  qmi_err_code = qmi_client_send_msg_sync(ccid_uim.qmi_uim_svc_client_ptr,
                                          QMI_UIM_EVENT_REG_REQ_V01,
                                          (void*) &event_reg_params,
                                          sizeof(uim_event_reg_req_msg_v01),
                                          (void*) qmi_response_ptr,
                                          sizeof(*qmi_response_ptr),
                                          CCID_UIM_QMI_DEFAULT_TIMEOUT);
  if(qmi_err_code != QMI_NO_ERR)
  {
    LOGE("Error for QMI_UIM_EVENT_REG_REQ_V01, qmi_err_code: 0x%x \n", qmi_err_code);
    free(qmi_response_ptr);
    return FALSE;
  }

  /* Get card status for cases where modem is done initializing already */
  memset(qmi_response_ptr, 0, sizeof(uim_get_card_status_resp_msg_v01));
  qmi_err_code = qmi_client_send_msg_sync(ccid_uim.qmi_uim_svc_client_ptr,
                                          QMI_UIM_GET_CARD_STATUS_REQ_V01,
                                          NULL,
                                          0,
                                          (void*) qmi_response_ptr,
                                          sizeof(*qmi_response_ptr),
                                          CCID_UIM_QMI_DEFAULT_TIMEOUT);
  if(qmi_err_code != QMI_NO_ERR)
  {
    LOGE("Error for QMI_UIM_GET_CARD_STATUS_REQ_V01, qmi_err_code: 0x%x \n", qmi_err_code);
    free(qmi_response_ptr);
    return FALSE;
  }

  /* Updated global card status if needed */
  if (!qmi_response_ptr->card_status_valid ||
      (qmi_response_ptr->resp.result != QMI_RESULT_SUCCESS_V01))
  {
    LOGE("card_status is not valid ! \n");
    free(qmi_response_ptr);
    return FALSE;
  }

  /* Update global in case of new card state or error code */
  for (i = 0;
       i< QMI_UIM_CARDS_MAX_V01 && i < qmi_response_ptr->card_status.card_info_len;
       i++)
  {
    if (qmi_response_ptr->card_status.card_info[i].card_state !=
        ccid_uim.card_info[i].card_state)
    {
      ccid_uim.card_info[i].card_state =
          qmi_response_ptr->card_status.card_info[i].card_state;
      LOGI("ccid_uim.card_info[i].card_state: 0x%x\n", ccid_uim.card_info[i].card_state);
    }

    if (qmi_response_ptr->card_status.card_info[i].error_code !=
        ccid_uim.card_info[i].card_error_code)
    {
      ccid_uim.card_info[i].card_error_code =
          qmi_response_ptr->card_status.card_info[i].error_code;
    }
  }

  /* Free the allocated response buffer */
  if (qmi_response_ptr)
  {
    free(qmi_response_ptr);
    qmi_response_ptr = NULL;
  }

  return TRUE;
} /* ccid_uim_qmi_register_events */


/*=========================================================================

  FUNCTION:  ccid_uim_init_qmi

===========================================================================*/
/*!
    @brief
    Finds out the QMI port to be used based on the modem type & initializes
    QMI_UIM service.

    @return
    TRUE if successful, else FALSE
*/
/*=========================================================================*/
static boolean ccid_uim_init_qmi
(
  void
)
{
  qmi_client_error_type          qmi_err_code           = 0;
  qmi_idl_service_object_type    qmi_client_service_obj;

  /* Init QMI_UIM service for CCID */
  qmi_client_service_obj = uim_get_service_object_v01();

  memset(&os_params, 0x00, sizeof(os_params));

  qmi_err_code = qmi_client_init_instance(qmi_client_service_obj,
                                          QMI_CLIENT_INSTANCE_ANY,
                                          ccid_qmi_client_indication_cb,
                                          qmi_client_service_obj,
                                          &os_params,
                                          QMI_UIM_INIT_TIMEOUT,
                                          &ccid_uim.qmi_uim_svc_client_ptr);

  if ((ccid_uim.qmi_uim_svc_client_ptr == NULL) ||
      (qmi_err_code > 0))
  {
    LOGE("Could not register with QMI UIM Service, qmi_uim_svc_client_ptr: %p, qmi_err_code: %d\n",
         ccid_uim.qmi_uim_svc_client_ptr, qmi_err_code);
    ccid_uim.qmi_uim_svc_client_ptr = NULL;
    return FALSE;
  }

  return TRUE;
} /* ccid_uim_init_qmi */


/*=========================================================================

  FUNCTION:  ccid_uim_global_init

===========================================================================*/
/*!
    @brief
    Initializes the global variable.

    @return
    TRUE if successful, else FALSE
*/
/*=========================================================================*/
static void ccid_uim_global_init
(
  void
)
{
  int i = 0;

  /* Initialize the global structure */
  memset(&ccid_uim, 0, sizeof(ccid_uim_struct_type));

  /* Init specific fields if needed */
  ccid_uim.usb_event_sd       = -1;
  ccid_uim.usb_state          = CCID_UIM_USB_STATE_UNKNOWN;

  for (i = 0; i <CCID_UIM_TTY_DEVNODE_MAX ; i++)
  {
    ccid_uim.dev_node[i].fd         = -1;
    ccid_uim.dev_node[i].thread_id  = -1;
  }

} /* ccid_uim_global_init */


/*===========================================================================

                             INTERFACE FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  ccid_qmi_client_async_cb
===========================================================================*/
/*!
@brief
  Callback function that will be called by the QCCI services layer to
  report asynchronous responses. It reports the response to parsing function.

@return
  None.

@note

  - Dependencies

  - Side Effects
*/
/*=========================================================================*/
static void ccid_qmi_client_async_cb
(
  qmi_client_type                user_handle,
  unsigned long                  msg_id,
  void                         * qmi_response_ptr,
  int                            resp_c_struct_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
)
{
  assert(user_handle != NULL);
  assert(qmi_response_ptr != NULL);

  LOGI("ccid_qmi_client_async_cb for msg_id : %lu \n", msg_id);

  CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

  /* Send the the response back to the parser */
  switch (msg_id)
  {
    case QMI_UIM_SAP_REQUEST_RESP_V01:
      ccid_uim_process_rdr_to_pc_bulk(ccid_uim.dev_node[CCID_UIM_TTY_DEVNODE_BULK].fd,
                                      (uim_sap_request_resp_msg_v01*)qmi_response_ptr,
                                      (ccid_uim_cmd_info_type*)resp_cb_data_ptr,
                                      transp_err);
      break;

    case QMI_UIM_SAP_CONNECTION_RESP_V01:
      /* Not handling here since Connected/Error state is always given via indication
         & hence IOCTL will be sent via indication */
      break;

    default:
      LOGE("ccid_qmi_client_async_cb: unsupported msg_id = %lu", msg_id);
      break;
  }

  CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

  /* Free response buffer that was allocated in the request */
  free(qmi_response_ptr);

} /* ccid_qmi_client_async_cb */


/*===========================================================================
  FUNCTION  ccid_qmi_client_indication_cb
===========================================================================*/
/*!
@brief
  Callback function that will be called by the QCCI services layer to
  report asynchronous indications.  This function currently monitors only
  card status & sap indication. It updates the global and decides to send
  the IOCTL to USB CCID Driver or not.

@return
  None.

@note

  - Dependencies

  - Side Effects
*/
/*=========================================================================*/
static void ccid_qmi_client_indication_cb
(
  qmi_client_type                user_handle,
  unsigned long                  msg_id,
  unsigned char                * ind_buf_ptr,
  int                            ind_buf_len,
  void                         * ind_cb_data
)
{
  uint32_t                    decoded_payload_len  =  0;
  void                      * decoded_payload_ptr  =  NULL;
  qmi_client_error_type       qmi_err_code         =  QMI_NO_ERR;

  if(ind_buf_ptr == NULL)
  {
    LOGE("ind_buf_ptr is NULL ! \n");
    return;
  }

  if(ccid_uim.qmi_uim_svc_client_ptr == NULL)
  {
    LOGE("qmi_uim_svc_client_ptr is NULL ! \n");
    return;
  }

  LOGI("QMI UIM Indication, msg_id : %lu \n", msg_id);

  /* Find the length & decode the Indication */
  qmi_idl_get_message_c_struct_len(uim_get_service_object_v01(),
                                   QMI_IDL_INDICATION,
                                   msg_id,
                                   &decoded_payload_len);
  if(decoded_payload_len)
  {
    decoded_payload_ptr = (void*)malloc(decoded_payload_len);
    if (decoded_payload_ptr == NULL)
    {
      LOGE("Failed to allocate buffer for QMI UIM Indication: %lu \n", msg_id);
      return;
    }
  }
  else
  {
    LOGE("Failed to find decoded_payload_len: 0x%x", decoded_payload_len);
    return;
  }

  qmi_err_code = qmi_client_message_decode( ccid_uim.qmi_uim_svc_client_ptr,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf_ptr,
                                            ind_buf_len,
                                            decoded_payload_ptr,
                                            decoded_payload_len);
  if (qmi_err_code != QMI_NO_ERR)
  {
    LOGE("Indication decode failed for msg: %lu with error 0x%x \n", msg_id, qmi_err_code);
    free(decoded_payload_ptr);
    return;
  }

  CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

  /* Determine the indication ID and process appropriately */
  switch(msg_id)
  {
    case QMI_UIM_STATUS_CHANGE_IND_V01:
      ccid_uim_process_ind_card_status((uim_status_change_ind_msg_v01*)decoded_payload_ptr);
      break;

    case QMI_UIM_SAP_CONNECTION_IND_V01:
      ccid_uim_process_ind_sap_connection((uim_sap_connection_ind_msg_v01*)decoded_payload_ptr);
      break;

    default:
      LOGI("Unknown QMI UIM Indication, msg_id : %lu \n", msg_id);
      break;
  }

  CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

  /* Free the allocated buffer */
  if (decoded_payload_ptr)
  {
    free(decoded_payload_ptr);
    decoded_payload_ptr = NULL;
  }

} /* ccid_qmi_client_indication_cb */


/*=========================================================================

  FUNCTION:  ccid_uim_cleanup

===========================================================================*/
/*!
    @brief
    Signal handler that performs the cleanup for the CCID Daemon. This is
    called for the common exit signals we registered with the kernel before.

    @return
    None
*/
/*=========================================================================*/
void ccid_uim_cleanup
(
  int   signal
)
{
  int i = 0;
  qmi_client_error_type               qmi_err_code      = 0;
  uim_get_card_status_resp_msg_v01  * qmi_response_ptr  = NULL;
  uim_event_reg_req_msg_v01           event_reg_params;

  LOGI("ccid_uim_cleanup, signal received: 0x%x \n", signal);

  CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

  /* First disconnect SAP mode on the card */
  for (i = 0; i<QMI_UIM_CARDS_MAX_V01; i++)
  {
    if (ccid_uim.card_info[i].sap_state == UIM_SAP_STATE_CONNECTED_SUCCESSFULLY_V01)
    {
      LOGI("ccid_uim_cleanup, Issuing SAP disconnect ... \n");
      (void)ccid_uim_send_sap_connect(i, FALSE);
    }
  }

  /* Next deinit QMI */
  LOGI("ccid_uim_cleanup, Issuing QMI deinit ... \n");
  if(ccid_uim.qmi_uim_svc_client_ptr)
  {
    qmi_client_release(ccid_uim.qmi_uim_svc_client_ptr);
    ccid_uim.qmi_uim_svc_client_ptr = NULL;
  }

  /* Close the USB event socket */
  if (ccid_uim.usb_event_sd != -1)
  {
    close(ccid_uim.usb_event_sd);
    ccid_uim.usb_event_sd = -1;
  }

  /* Close both fds & terminate the thread */
  for (i = 0; i <CCID_UIM_TTY_DEVNODE_MAX ; i++)
  {
    if (ccid_uim.dev_node[i].fd != -1)
    {
      close(ccid_uim.dev_node[i].fd);
      ccid_uim.dev_node[i].fd        = -1;
      ccid_uim.dev_node[i].thread_id = -1;
    }
  }

  CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

  exit(0);
} /* ccid_uim_cleanup */


/*=========================================================================

  FUNCTION:  main

===========================================================================*/
/*!
    @brief
    Main entry point for the CCID Daemon.

    @return
    None
*/
/*=========================================================================*/
int main (int argc, char **argv)
{
  ccid_uim_usb_state_type   read_usb_state = CCID_UIM_USB_STATE_UNKNOWN;

  /* Init global variable */
  ccid_uim_global_init();

  /* Initialize QMI connection & QMI_UIM service */
  if (!ccid_uim_init_qmi())
  {
    LOGE("ccid_uim_init_qmi failed ! \n");
    goto cleanup_and_exit;
  }

  /* Register a signal handler for daemon exit signals
     Below are the process exit signals that are supported */
  signal(SIGINT,  ccid_uim_cleanup);
  signal(SIGQUIT, ccid_uim_cleanup);
  signal(SIGTERM, ccid_uim_cleanup);
  signal(SIGHUP,  ccid_uim_cleanup);

  /* Register for QMI_UIM events */
  if (!ccid_uim_qmi_register_events())
  {
    LOGE("ccid_uim_qmi_register_events failed ! \n");
    goto cleanup_and_exit;
  }

  /* First check the USB state before proceeding */
  read_usb_state = ccid_uim_get_usbstate_from_sysfile();

  /* Initialize the USB kernel event handling */
  if (!ccid_uim_init_usb_socket(&ccid_uim.usb_event_sd))
  {
    LOGE("ccid_uim_init_usb_socket failed ! \n");
    goto cleanup_and_exit;
  }

  /* Wait on USB event */
  while(1)
  {
    int     usb_event_sd   = -1;
    int     bytes_read     = 0;
    uint8   rx_buffer[CCID_UIM_USB_EVT_BUFFER_SIZE];

    CCID_UIM_ENTER_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

    usb_event_sd = ccid_uim.usb_event_sd;

    /* Process USB state change, if needed */
    if (read_usb_state != ccid_uim.usb_state)
    {
      ccid_uim.usb_state = read_usb_state;

      LOGI("Found USB event: 0x%x \n", ccid_uim.usb_state);

      switch (ccid_uim.usb_state)
      {
        case CCID_UIM_USB_STATE_CONNECTED:
          ccid_uim_process_state_usb_connected();
          break;

        case CCID_UIM_USB_STATE_DISCONNECTED:
          ccid_uim_process_state_usb_disconnected();
          break;

        case CCID_UIM_USB_STATE_ERROR:
          LOGE("CCID_UIM_USB_STATE_ERROR ! Exiting daemon ...\n");
          CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");
          goto cleanup_and_exit;

        default:
          LOGE("Unsupported usb_state ! \n");
          break;
      }
    }

    CCID_UIM_LEAVE_CRITICAL_SECTION(&ccid_uim_mutex, "ccid_uim_mutex");

    /* Init read buffer */
    memset(rx_buffer, 0, sizeof(rx_buffer));

    /* Blocking call on the USB event socket */
    if ((bytes_read = recv(usb_event_sd,
                           rx_buffer,
                           sizeof(rx_buffer) - 1,
                           0)) < 0)
    {
      LOGE("Error receiving USB event (%s) \n", strerror(errno));
      goto cleanup_and_exit;
    }

    LOGI("Received (0x%x) bytes on USB event socket \n", bytes_read);

    /* Parse the read USB event data */
    ccid_uim_get_usbstate_from_event(rx_buffer,
                                     bytes_read,
                                     &read_usb_state);
  }

cleanup_and_exit:
  ccid_uim_cleanup(SIGTERM);

  LOGI(" Exiting daemon ...\n");
  return 0;
} /* main */

