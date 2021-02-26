#ifndef QMI_MB_H
#define QMI_MB_H
/******************************************************************************
  @file    qmi_mb.h
  @brief   The QMI message bus header file.

  DESCRIPTION
  QMI message bus public API.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_mb_create() needs to be called before sending or receiving of any
  service specific messages

  ---------------------------------------------------------------------------
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/mproc/qmi_mb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#include "stdlib.h"
#include "stdint.h"
#include "qmi_idl_lib.h"

#ifdef __cplusplus
extern "C" {
#endif


#define QMI_MB_NO_ERR                0
#define QMI_MB_INTERNAL_ERR          (-1)
#define QMI_MB_PARAM_ERR             (-2)
#define QMI_MB_MEMORY_ERR            (-3)
#define QMI_MB_ENCODE_ERR            (-4)
#define QMI_MB_INVALID_BUS           (-5)
#define QMI_MB_BUSY_ERR              (-6)
#define QMI_MB_PERM_ERR              (-7)
#define QMI_MB_EAGAIN_ERR            (-8)
#define QMI_MB_INVALID_MEM_ERR       (-9)

typedef struct         qmi_mb_struct *qmi_mb_type;
typedef struct         qmi_mb_struct *qmi_mb_ep_type;
typedef int            qmi_mb_error_type;
typedef struct         qmi_mb_options_struct qmi_mb_options;

typedef enum
{
  /* A new subscriber has joined the message Bus */
  QMI_MB_EVENT_JOIN,

  /* An existing subscriber has stopped listening
   * on the message bus */
  QMI_MB_EVENT_LEAVE,

  /* A previously known endpoint (Publisher or 
   * subscriber) has exited */
  QMI_MB_EVENT_EP_CLOSED,

  /* There is a message to be received. Supported for
   * users initializing using qmi_mb_create
   * See qmi_mb_recv_msg for more info on receiving the 
   * message */
  QMI_MB_EVENT_MSG
} qmi_mb_event_type;

/*=============================================================================
  INTERNAL DEFINES
=============================================================================*/
#define QMI_MB_OPTIONS_SUBGROUP_VALID    (0x1)
#define QMI_MB_OPTIONS_RAW_FLAG_VALID    (0x4)
#define QMI_MB_OPTIONS_DISABLE_RX_VALID  (0x8)

/*=============================================================================
  MACRO  QMI_MB_OPTIONS_INIT
=============================================================================*/
/*!
@brief
  Initialize the QMI MB Options object. Always call this before
  setting any other options field.

@param[in]  opt                 Service options object
*/
/*=========================================================================*/
#define QMI_MB_OPTIONS_INIT(opt) (opt).options_set = 0

/*=============================================================================
  MACRO  QMI_MB_OPTIONS_SET_SUBGROUP_ID
=============================================================================*/
/*!
@brief
  Set the subgroup ID of the service. Default: 0

@param[in]  opt                 Service options object
@param[in]  grp                 subgroup ID of the service
*/
/*=========================================================================*/
#define QMI_MB_OPTIONS_SET_SUBGROUP_ID(opt, grp) \
  do { \
    (opt).subgroup_id = grp;  \
    (opt).options_set |= QMI_MB_OPTIONS_SUBGROUP_VALID; \
  } while(0)

/*=============================================================================
  MACRO  QMI_MB_OPTIONS_SET_RAW_FLAG
=============================================================================*/
/*!
@brief
  Instructs the infrastructure to refrain from encoding or decoding the 
  buffers.

@param[in]  opt                 Service options object
*/
/*=========================================================================*/
#define QMI_MB_OPTIONS_SET_RAW_FLAG(opt) \
  do { \
    (opt).options_set |= QMI_MB_OPTIONS_RAW_FLAG_VALID; \
  } while(0)

/*=============================================================================
  MACRO  QMI_MB_OPTIONS_DISABLE_RECV
=============================================================================*/
/*!
@brief
  Instructs the infrastructure that the user is not interested in messages
  (The user wishes to be a publish-only endpoint)

@param[in]  opt                 Service options object
*/
/*=========================================================================*/
#define QMI_MB_OPTIONS_DISABLE_RECV(opt) \
  do { \
    (opt).options_set |= QMI_MB_OPTIONS_DISABLE_RX_VALID; \
  } while(0)

/*============================================================================
                            CALLBACK FUNCTIONS
============================================================================*/

/*=============================================================================
  CALLBACK FUNCTION qmi_mb_event_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the infrastructure when a join/leave/
  disconnect event has occurred in the underlying transport.

@param[in]   user_handle      Handle used by the infrastructure to
                              identify different clients.
@param[in]   event            Type of event
@param[in]   ep_handle        Handle for the end-point
@param[in]   event_cb_data    Cookie provided

*/
/*=========================================================================*/
typedef void (*qmi_mb_event_cb)
(
 qmi_mb_type user_handle,
 qmi_mb_event_type event,
 qmi_mb_ep_type ep_handle,
 void *event_cb_data
);


/*=============================================================================
  CALLBACK FUNCTION qmi_mb_release_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the infrastructure when a connection
  is fully released.


@param[in]   release_cb_data    cookie provided in qmi_mb_release()

*/
/*=========================================================================*/
typedef void (*qmi_mb_release_cb)
(
 void *release_cb_data
);


/*===========================================================================
  FUNCTION  qmi_mb_create
===========================================================================*/
/*!
@brief

  This function is the preferred method for initializing a message bus handle

@param[in]   service_obj        Service object
@param[in]   options            Options (See QMI_MB_OPTIONS_*)
@param[in]   event_cb           Received event callback function*
@param[in]   event_cb_data      Received event callback user-data
@param[out]  user_handle        Handle used by the infrastructure to
                                identify different clients.
@return
  Sets the user_handle and returns QMI_MB_NO_ERR if successful,
  error code if not successful

@note

  * When a message is received, the event_cb will be called with 
    QMI_MB_EVENT_MSG. The user can then drain the receive queue
    by using qmi_mb_recv_msg.

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_create
(
 qmi_idl_service_object_type service_obj,
 qmi_mb_options *options,
 qmi_mb_event_cb event_cb,
 void *event_cb_data,
 qmi_mb_type *user_handle
);

/*===========================================================================
  FUNCTION  qmi_mb_join
===========================================================================*/
/*!
@brief

  This function is used to join a message bus. Only one connection can be
  made on each user handle.

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.

@return
  Sets the user_handle and returns QMI_MB_NO_ERR if successful,
  error code if not successful

@note

  - Dependencies
    - None

  - Side Effects
    - Connection to the message bus is established
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_join
(
 qmi_mb_type user_handle
);

/*===========================================================================
  FUNCTION  qmi_mb_leave
===========================================================================*/
/*!
@brief

  This function is used to leave a message bus.

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.

@return
  Sets the user_handle and returns QMI_MB_NO_ERR if successful,
  error code if not successful

@note

  - Dependencies
    - None

  - Side Effects
    - Connection to the message bus is destroyed
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_leave
(
 qmi_mb_type user_handle
);

/*===========================================================================
  FUNCTION  qmi_mb_publish
===========================================================================*/
/*!
@brief
  Publishes a QMI message to the bus specified by the handle. Only
  messages specified as indications in the IDL can be sent.

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.
@param[in]   msg_id             Message ID
@param[in]   c_struct           Pointer to the message*
@param[in]   c_struct_len       Length of the message


@return
  QMI_MB_NO_ERR if successful error code otherwise.

@note
  * If the raw flag was set on this handle (See QMI_MB_OPTIONS_SET_RAW_FLAG)
    then this is the encoded message and the framework shall refrain from
    encoding the message. Else, this is the C Structure of the message 
    which shall be encoded by the framework before publishing.

  - Dependencies
    - None

  - Side Effects
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_publish
(
 qmi_mb_type                     user_handle,
 unsigned int                    msg_id,
 void                            *c_struct,
 unsigned int                    c_struct_len
);

/*===========================================================================
  FUNCTION  qmi_mb_send_msg
===========================================================================*/
/*!
@brief
  Sends a unicast QMI message on the specified endpoint handle. Only
  messages specified as indications in the IDL can be sent.

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.
@param[in]   ep_handle          Handle for the end-point specified by rx/event
                                callbacks
@param[in]   msg_id             Message ID
@param[in]   c_struct           Pointer to the message*
@param[in]   c_struct_len       Length of the message

@return
  QMI_MB_NO_ERR if function is successful, error code otherwise.

@note
  * If the raw flag was set on this handle (See QMI_MB_OPTIONS_SET_RAW_FLAG)
    then this is the encoded message and the framework shall refrain from
    encoding the message. Else, this is the C Structure of the message 
    which shall be encoded by the framework before transmitting.

  - Dependencies
    - None

  - Side Effects
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_send_msg
(
 qmi_mb_type                     user_handle,
 qmi_mb_ep_type                  ep_handle,
 unsigned int                    msg_id,
 void                            *c_struct,
 unsigned int                    c_struct_len
);

/*===========================================================================
  FUNCTION  qmi_mb_recv_msg
===========================================================================*/
/*!
@brief
  Receives a QMI message from an endpoint.

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.
@param[out]  sender             Handle for the sender end-point (optional)
@param[out]  msg_id             ID of the message
@param[out]  c_struct           Pointer to the message* **
@param[out]  c_struct_len       Length of the message

@return
  QMI_MB_NO_ERR if function is successful, error code otherwise.

@note
  *  If the raw flag was set on this handle (See QMI_MB_OPTIONS_SET_RAW_FLAG)
     then this is the encoded message and the framework shall refrain from
     decoding the message. Else, this is the C Structure of the message 
     which has been decoded by the infrastructure.
  ** The user should call qmi_mb_free_msg() once they are done with it.

  - Dependencies
    - The user must have initialized using qmi_mb_create to use this
      feature.

  - Side Effects
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_recv_msg
(
  qmi_mb_type                    user_handle, 
  qmi_mb_ep_type                 *sender,
  unsigned int                   *msg_id,
  void                           **c_struct,
  unsigned int                   *c_struct_len
);

/*===========================================================================
  FUNCTION  qmi_mb_free_msg
===========================================================================*/
/*!
@brief
  Free a previously received message

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.
@param[in]   c_struct           Pointer provided by qmi_mb_recv_msg
@param[in]   c_struct_len       Length of the message

@return
  QMI_MB_NO_ERR if function is successful, error code otherwise.

@note
  - Dependencies
    - None

  - Side Effects
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_free_msg
(
  qmi_mb_type                    user_handle, 
  void                           *c_struct,
  unsigned int                   c_struct_len
);

/*===========================================================================
  FUNCTION  qmi_mb_release
===========================================================================*/
/*!
@brief
  Releases the message bus handle.

@param[in]   user_handle        Handle to be released
@param[in]   release_cb         The callback to be called when the 
                                infrastructure successfully releases the
                                handle
@param[in]   user_handle        Cookie which the user will be provided
                                in the release_cb

@return
  QMI_MB_NO_ERR if function is successful, error code otherwise.

@note

  - Dependencies
    - None

  - Side Effects
   - None
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_release
(
 qmi_mb_type         user_handle,
 qmi_mb_release_cb   release_cb,
 void                *release_cb_data
);

/*=============================================================================
  PRIVATE qmi_mb_options_struct
=============================================================================*/
/*!
@brief
  Provide storage class for the options structure. This structure should not
  be directly manipulated. Please use the QMI_MB_OPTIONS_* macros.
*/
/*=========================================================================*/
struct qmi_mb_options_struct
{
  unsigned int        options_set;
  unsigned int        subgroup_id;
  /* Place holders for deprecated options 
   * to maintain binary compatibility */
  void                *deprecated_1;
  void                *deprecated_2;
};

/*=============================================================================
  Include deprecated methods
=============================================================================*/
#include "qmi_mb_deprecated.h"

#ifdef __cplusplus
}
#endif

#endif /* QMI_MB_H  */
