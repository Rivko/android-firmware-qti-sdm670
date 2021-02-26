/***********************************************************************
 * fs_qmi_client.c
 *
 * The EFS Remote Client Module for efs_put and efs_get APIs.
 * Copyright (C) 2011-2015 QUALCOMM Technologies, Inc.
 *
 * This file provides implementation for client module that remotes efs_get
 * and efs_put APIs to the modem processor. The client module runs on a
 * processor which does not have full EFS support and provides support for
 * only efs_get and efs_put APIs, by remoting these calls to the modem
 * processor over Qualcomm MSM Interface(QMI) using the
 * QMI Common Client Interface(QCCI) framework.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/efs_qmi_put_get/client/src/fs_qmi_client.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-25   dks   Add function to read efs items using bigger sized buffer.
2015-04-25   dks   Port to work on qurt
2015-03-25   dks   Cleanup client to use new qmi APIs
2015-02-05   mj    Set the timer_sig in the os_params structure.
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-10-09   nr    Replace banned memory functions with safer versions.
2012-03-23   dks   Add remote processor access check for paths.
2011-11-14   dks   Fix security vulnerabilities.
2011-10-04   dks   Create.
===========================================================================*/

#include "fs_qmi_config_i.h"

#ifdef FEATURE_EFS_QMI_PUT_GET_CLIENT

#include "fs_qmi_std_interface.h"
#include "fs_qmi_util.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "fs_os.h"
#include "fs_os_err.h"
#include "fs_errno.h"
#include "fs_os_msg.h"
#include "fs_os_string.h"
#include "fs_os_timetick.h"
#include "qmi_client.h"
#include "assert.h"
#include "stringl/stringl.h"
#include <string.h>
#ifdef FEATURE_FS_QMI_USE_REX_QMI_CLIENT
  #include "rex.h"
  #include "task.h"
#else
  /* On ADSP, QMI uses Qurt for signalling, so no need for global signal, so
     define own */
  #ifndef FS_OP_COMPLETE_SIG
    #define FS_OP_COMPLETE_SIG 1
  #endif
#endif

/* The client module sends a request to a service instance to handle the
 * efs_put and efs_get commands from the remote filesystem. There can be
 * multiple service instances that can handle this. Currently we support only
 * one service instance. */
#define FS_QMI_EFS_MAX_SERVICES 1

/* Max msecs to wait for reply from the service. 0 is interpreted as infinity
   by QMI. */
#ifndef FS_QMI_EFS_MSG_TIMEOUT_MSEC
  #define FS_QMI_EFS_MSG_TIMEOUT_MSEC  5000
#endif

struct fs_qmi_client_object_type
{
  /* QMI IDL defined request and response structures to be sent to and received
     from the service. QMI can only encode and decode these messages defined in
     IDL which are compiled into C structures in the generated header header
     file. Corresponding generated .c file handles encoding and decoding of
     these structures. The sizes of these structures are big and cannot be put
     on the stack. */
  fs_qmi_efs_put_req_msg  efs_put_request;
  fs_qmi_efs_put_resp_msg efs_put_response;
  fs_qmi_efs_get_req_msg  efs_get_request;
  fs_qmi_efs_get_resp_msg efs_get_response;

  /* Object to contains the tcb and signal information to be supplied to the
     QCCI framework. It is used */
  qmi_client_os_params os_params;

  /* QMI IDL service object, a handle to specify service object of interest.
     Used by service during registration to specify the services offered.
     Used by client to request service instance registered offering these
     services. */
  qmi_idl_service_object_type service_object;

  /* Set to 1 for duration that client is connected to server */
  int client_inited;

  /* Following are state variables meant to help debugging issues involving
     timeout or failed process requests. */

  /* Timetick when the client sent the last request. */
  uint64 last_req_sent_timetick;

  /* Timetick when the service returned with the response. */
  uint64 last_resp_recvd_timetick;
};
static struct fs_qmi_client_object_type fs_qmi_client_obj;


/* Critical section to protect structures in fs_qmi_client_obj from
   simultaneous access */
static fs_os_mutex_t fs_qmi_client_crit_sect;

/* Initialization done by RCINIT */
void
fs_qmi_client_init (void)
{
  memset (&fs_qmi_client_obj, 0x0, sizeof (fs_qmi_client_obj));
  fs_os_mutex_init (&fs_qmi_client_crit_sect);

  fs_os_mutex_lock (&fs_qmi_client_crit_sect);
  fs_qmi_client_obj.service_object = fs_qmi_get_service_object ();
  if (!fs_qmi_client_obj.service_object)    /* Should not fail */
  {
    FS_OS_ERR_FATAL ("QMI get service obj failed. Check .c and .h", 0, 0, 0);
  }

  fs_os_mutex_unlock (&fs_qmi_client_crit_sect);
}

void
fs_qmi_client_init_os_params (qmi_client_os_params *os_params,
                              unsigned int sig, unsigned int timeout_sig)
{
  FS_OS_ASSERT (os_params != NULL);
  memset (os_params, 0x0, sizeof (qmi_client_os_params));

  os_params->sig = sig;
  os_params->timer_sig = timeout_sig;

#ifdef FEATURE_FS_QMI_USE_REX_QMI_CLIENT
  os_params->tcb = rex_self ();
#endif
}

/* fs_qmi_client_open
 * Opens a new connection with the service found from get_service_list. */
static qmi_client_type
fs_qmi_client_open (void)
{
  qmi_client_error_type err;
  qmi_client_type client_handle = NULL;

  fs_qmi_client_init_os_params (&fs_qmi_client_obj.os_params,
                                FS_OP_COMPLETE_SIG, FS_OP_COMPLETE_SIG);

  /* Get client handle from framework for the specified service instance.
     This does not send connect request to service. The actual connect
     request goes when client sends the first request message. */
  err = qmi_client_init_instance (fs_qmi_client_obj.service_object,
                                  QMI_CLIENT_INSTANCE_ANY, NULL, NULL,
                                  &fs_qmi_client_obj.os_params,
                                  (unsigned int) FS_QMI_EFS_MSG_TIMEOUT_MSEC,
                                  &client_handle);
  if (err != QMI_NO_ERR)
  {
    client_handle = NULL;
    FS_OS_MSG_ERROR_1 ("%d QMI client init failed", err);
  }
  else
  {
    FS_OS_MSG_HIGH_1 ("%d QMI client inited", (uint32)client_handle);
    fs_qmi_client_obj.client_inited = 1;
  }

  return client_handle;
}


/* fs_qmi_client_close
 * Release the connection with the service. */
static void
fs_qmi_client_close (qmi_client_type client_handle)
{
  qmi_client_error_type err;

  err = qmi_client_release (client_handle);
  if (err != QMI_NO_ERR)
  {
    FS_OS_MSG_ERROR_1 ("%d QMI client release failed", err);
  }

  fs_qmi_client_obj.client_inited = 0;
}


/* efs_put
 * Implements the API to remote the efs_put call to the modem over QMI. */
#ifdef FS_UNIT_TEST
int
efs_qmi_put (const char *path, void *data, fs_size_t length, int oflag,
             int mode)
#else
int
efs_put (const char *path, void *data, fs_size_t length, int oflag, int mode)
#endif
{
  fs_qmi_efs_put_req_msg *req_msg;
  fs_qmi_efs_put_resp_msg *resp_msg;
  qmi_client_error_type err;
  qmi_client_type client_handle;
  uint32 path_len;
  int result = -1;

  if (!path || !data)
  {
    FS_OS_MSG_ERROR_2 ("%d, %d, put Invalid param", (uint32)path,
                        (uint32) data);
    efs_errno = EINVAL;
    return result;
  }

  path_len = strlen (path);
  if(path_len == 0)
  {
    FS_OS_MSG_ERROR_0 ("put path_len = 0");
    efs_errno = EINVAL;
    return result;
  }

  /* Length including terminating NULL should not be greater than allowed
     length. */
  if (path_len + 1 > FS_QMI_MAX_FILE_PATH)
  {
    FS_OS_MSG_ERROR_2 ("put path_len %d > max %d", path_len + 1,
                       FS_QMI_MAX_FILE_PATH);
    efs_errno = ENAMETOOLONG;
    return result;
  }

  if (length > FS_QMI_MAX_ITEM_SIZE)
  {
    FS_OS_MSG_ERROR_2 ("put data_len %d > max %d", length,
                       FS_QMI_MAX_ITEM_SIZE);
    efs_errno = FS_ERANGE;
    return result;
  }

  fs_os_mutex_lock (&fs_qmi_client_crit_sect);

  /* Open new connection with framework. */
  client_handle = fs_qmi_client_open ();
  if (!client_handle)
  {
    FS_OS_MSG_ERROR_0 ("put qmi client handle NULL");
    result = -1;
    efs_errno = ENODEV;
    goto cleanup_before_open;
  }

  req_msg = &fs_qmi_client_obj.efs_put_request;
  resp_msg = &fs_qmi_client_obj.efs_put_response;

  /* Clear old messages. */
  memset (req_msg, 0, sizeof (fs_qmi_efs_put_req_msg));
  memset (resp_msg, 0, sizeof (fs_qmi_efs_put_resp_msg));

  /* Create the request message. */
  req_msg->path_len = path_len + 1;
  req_msg->data_len = length;

  strlcpy (req_msg->path, path, sizeof (req_msg->path));
  fs_os_memscpy (req_msg->data, req_msg->data_len, data, req_msg->data_len);
  req_msg->oflag = fs_qmi_get_qmi_oflag (oflag);
  req_msg->mode = mode;

  /* The resulting oflag should be non-zero. */
  FS_OS_ASSERT (req_msg->oflag != 0);

  fs_qmi_client_obj.last_req_sent_timetick = fs_os_timetick_get_timetick ();
  FS_OS_MSG_HIGH_1 ("%ul fs_qmi_client put request message sent",
             (uint32) fs_qmi_client_obj.last_req_sent_timetick);

  /* Send request message. */
  err = qmi_client_send_msg_sync (client_handle, FS_QMI_EFS_PUT_REQ,
                                  req_msg, sizeof (fs_qmi_efs_put_req_msg),
                                  resp_msg, sizeof (fs_qmi_efs_put_resp_msg),
                                  (unsigned int) FS_QMI_EFS_MSG_TIMEOUT_MSEC);

  fs_qmi_client_obj.last_resp_recvd_timetick = fs_os_timetick_get_timetick ();
  FS_OS_MSG_HIGH_2 ("%ul %d fs_qmi_client put response message received",
             (uint32) fs_qmi_client_obj.last_resp_recvd_timetick, err);

  if (err != QMI_NO_ERR)
  {
    FS_OS_MSG_ERROR_1 ("%d put client send msg sync failed", err);
    result = -1;
    efs_errno = ENODEV;
    goto cleanup;
  }

  /* Check for service framework errors. */
  if (resp_msg->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    FS_OS_MSG_ERROR_2 ("%d %d QMI service framework error. Check client",
                       resp_msg->resp.result, resp_msg->resp.error);
    result = -1;
    efs_errno = EILSEQ; /* To indicate something wrong in QMI pipe. */
    goto cleanup;
  }

  /* qmi response is a SUCCESS; check for API errors. */
  if (resp_msg->efs_err_num_valid == TRUE)
  {
    efs_errno = fs_qmi_get_efs_errno (resp_msg->efs_err_num);
    FS_OS_MSG_HIGH_1 ("%d efs_put failed on server", resp_msg->efs_err_num);
  }
  else     /* err_num_valid is set to FALSE, so the API was successful. */
  {
    FS_OS_MSG_HIGH_0 ("efs_put succeeded on server");
    result = 0;
  }

cleanup:
  fs_qmi_client_close (client_handle);
cleanup_before_open:
  fs_os_mutex_unlock (&fs_qmi_client_crit_sect);
  return result;
}


/* efs_get
 * Implements the API to remote the efs_get call to the modem. */
#ifdef FS_UNIT_TEST
int
efs_qmi_get (const char *path, void *data, fs_size_t length)
#else
int
efs_get (const char *path, void *data, fs_size_t length)
#endif
{
  fs_qmi_efs_get_req_msg *req_msg;
  fs_qmi_efs_get_resp_msg *resp_msg;
  qmi_client_error_type err;
  qmi_client_type client_handle;
  uint32 path_len;
  int result = -1;

  if (!path || !data)
  {
    FS_OS_MSG_ERROR_2 ("%d, %d, get Invalid param", (uint32) path,
                        (uint32) data);
    efs_errno = EINVAL;
    return result;
  }

  path_len = strlen(path);
  if (path_len == 0)
  {
    FS_OS_MSG_ERROR_0 ("get path_len = 0");
    efs_errno = EINVAL;
    return result;
  }

  /* Length including terminating NULL should not be greater than allowed
     length. */
  if (path_len + 1 > FS_QMI_MAX_FILE_PATH)
  {
    FS_OS_MSG_ERROR_2 ("get path_len %d > max %d", path_len + 1,
                       FS_QMI_MAX_FILE_PATH);
    efs_errno = ENAMETOOLONG;
    return result;
  }

  if (length > FS_QMI_MAX_ITEM_SIZE)
  {
    FS_OS_MSG_ERROR_2 ("get data_len %d > max %d", length,
                       FS_QMI_MAX_ITEM_SIZE);
    efs_errno = FS_ERANGE;
    return result;
  }

  fs_os_mutex_lock (&fs_qmi_client_crit_sect);

  /* Open new connection with framework. */
  client_handle = fs_qmi_client_open ();
  if (!client_handle)
  {
    FS_OS_MSG_ERROR_0 ("get qmi client handle NULL");
    result = -1;
    efs_errno = ENODEV;
    goto cleanup_before_open;
  }

  req_msg = &fs_qmi_client_obj.efs_get_request;
  resp_msg = &fs_qmi_client_obj.efs_get_response;

  /* Clear old messages. */
  memset (req_msg, 0, sizeof (fs_qmi_efs_get_req_msg));
  memset (resp_msg, 0, sizeof (fs_qmi_efs_get_resp_msg));

  /* create request message */
  req_msg->path_len = path_len + 1;
  req_msg->data_length_valid = TRUE;
  req_msg->data_length = length;
  strlcpy (req_msg->path, path, sizeof (req_msg->path));

  fs_qmi_client_obj.last_req_sent_timetick = fs_os_timetick_get_timetick ();
  FS_OS_MSG_HIGH_1 ("%ul fs_qmi_client get request message sent",
             (uint32) fs_qmi_client_obj.last_req_sent_timetick);

  /* Send request message. */
  err = qmi_client_send_msg_sync (client_handle, FS_QMI_EFS_GET_REQ,
                                  req_msg, sizeof (fs_qmi_efs_get_req_msg),
                                  resp_msg, sizeof (fs_qmi_efs_get_resp_msg),
                                  (unsigned int) FS_QMI_EFS_MSG_TIMEOUT_MSEC);

  fs_qmi_client_obj.last_resp_recvd_timetick = fs_os_timetick_get_timetick ();
  FS_OS_MSG_HIGH_2 ("%ul %d fs_qmi_client get response message received",
                    (uint32) fs_qmi_client_obj.last_resp_recvd_timetick, err);

  if (err != QMI_NO_ERR)
  {
    FS_OS_MSG_ERROR_1 ("%d Get QMI client send msg sync failed", err);
    result = -1;
    efs_errno = ENODEV;
    goto cleanup;
  }

  /* Check for service framework errors. */
  if (resp_msg->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    FS_OS_MSG_ERROR_2 ("%d %d QMI service framework error. Check client",
                         resp_msg->resp.result, resp_msg->resp.error);
    result = -1;
    efs_errno = EILSEQ; /* To indicate something wrong in QMI pipe. */
    goto cleanup;
  }

  /* qmi response is a SUCCESS; check for API errors. */
  if (resp_msg->efs_err_num_valid == TRUE)
  {
    efs_errno = fs_qmi_get_efs_errno (resp_msg->efs_err_num);
    FS_OS_MSG_ERROR_1 ("%d efs_get failed on server", resp_msg->efs_err_num);
  }
  else
  {
    /* QMI sync send was a success. Make sure optional fields are filled. */
    if (resp_msg->data_valid == FALSE)
    {
      FS_OS_ERR_FATAL ("err_num & data are both invalid", 0, 0, 0);
    }
    if (resp_msg->data_len > length)
    {
      FS_OS_ERR_FATAL ("%d %d data len > length", resp_msg->data_len,
                       length, 0);
    }
    FS_OS_MSG_HIGH_0 ("efs_get succeeded on server");
    fs_os_memscpy (data, resp_msg->data_len, resp_msg->data, \
                    resp_msg->data_len);
    result = resp_msg->data_len;
  }

cleanup:
  fs_qmi_client_close (client_handle);
cleanup_before_open:
  fs_os_mutex_unlock (&fs_qmi_client_crit_sect);
  return result;
}

#endif /* FEATURE_EFS_QMI_PUT_GET_CLIENT */
