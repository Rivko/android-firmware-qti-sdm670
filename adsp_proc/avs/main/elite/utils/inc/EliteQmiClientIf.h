#ifndef ELITEQMICLIENTIF_H
#define ELITEQMICLIENTIF_H

/**
@file EliteQmiClientIf.h

@brief This file provides an API wrapper of QMI Client Interface.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The descriptions that appear in the PDF are maintained in the
      Elite_Core_mainpage.dox file. Contact Tech Pubs for support.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2014 Qualcomm Technologies, Inc. All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/07/14   JV      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

#include "qmi_client.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup elite_qmi_client_datatypes
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
#define ELITE_QMI_NO_ERR                  QMI_NO_ERR
#define ELITE_QMI_INTERNAL_ERR            QMI_INTERNAL_ERR
#define ELITE_QMI_SERVICE_ERR             QMI_SERVICE_ERR
#define ELITE_QMI_TIMEOUT_ERR             QMI_TIMEOUT_ERR
#define ELITE_QMI_EXTENDED_ERR            QMI_EXTENDED_ERR
#define ELITE_QMI_PORT_NOT_OPEN_ERR       QMI_PORT_NOT_OPEN_ERR
#define ELITE_QMI_MEMCOPY_ERROR           QMI_MEMCOPY_ERROR
#define ELITE_QMI_INVALID_TXN             QMI_INVALID_TXN
#define ELITE_QMI_CLIENT_ALLOC_FAILURE    QMI_CLIENT_ALLOC_FAILURE
#define ELITE_QMI_CLIENT_TRANSPORT_ERR    QMI_CLIENT_TRANSPORT_ERR
#define ELITE_QMI_CLIENT_PARAM_ERR        QMI_CLIENT_PARAM_ERR
#define ELITE_QMI_CLIENT_INVALID_CLNT     QMI_CLIENT_INVALID_CLNT
#define ELITE_QMI_CLIENT_FW_NOT_UP        QMI_CLIENT_FW_NOT_UP
#define ELITE_QMI_CLIENT_INVALID_SIG      QMI_CLIENT_INVALID_SIG
#define ELITE_QMI_XPORT_BUSY_ERR          QMI_XPORT_BUSY_ERR

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** QMI Client */
typedef qmi_client_type                elite_qmi_client_type;

/** QMI Client Error */
typedef qmi_client_error_type          elite_qmi_client_error_type;

/** QMI Service Info */
typedef qmi_service_info               elite_qmi_service_info;

/** QMI Client Indicator Callback */
typedef void ( *elite_qmi_client_ind_cb )
(
   elite_qmi_client_type          userHandle,
   unsigned int                   unMsgID,
   void                           *indBuf,
   unsigned int                   unIndBufLen,
   void                           *indCBData
);

/** QMI Client OS Parameters */
typedef qmi_client_os_params           elite_qmi_client_os_params;

/** QMI IDL Service Object Type */
typedef qmi_idl_service_object_type    elite_qmi_idl_service_object_type;

/** QMI Transaction Handle */
typedef qmi_txn_handle                 elite_qmi_txn_handle;

/** QMI Client Receive Message Asynchronously Callback*/
typedef void ( *elite_qmi_client_recv_msg_async_cb )
(
   elite_qmi_client_type           userHandle,
   unsigned int                    unMsgID,
   void                            *pRespCStruct,
   unsigned int                    unRespCStructLen,
   void                            *pRespCBData,
   elite_qmi_client_error_type     transpErr
);

/* QMI message types for ipc_message_encode() and ipc_message_decode() */
/**
   elite_qmi_idl_type_of_message_type
     - QMI message type, distinguishes between request, response and indication.
*/
typedef enum {
  ELITE_QMI_IDL_REQUEST = QMI_IDL_REQUEST,               /**< QMI Request  */
  ELITE_QMI_IDL_RESPONSE = QMI_IDL_RESPONSE,             /**< QMI Response */
  ELITE_QMI_IDL_INDICATION = QMI_IDL_INDICATION,         /**< QMI Indication */
  ELITE_QMI_IDL_NUM_MSG_TYPES = QMI_IDL_NUM_MSG_TYPES    /**< Sentinel for error checking */
} elite_qmi_idl_type_of_message_type;

/** @} */ /* end_addtogroup elite_qmi_datatypes */


/* =======================================================================
**                          Basic QMI Client Interface Functions
** ======================================================================= */

/** @ingroup elite_qmi_client_get_service_list
   Retrieves a list of services capable of handling the service object

   @param[in]      serviceObj          Service object
   @param[out]     pServiceInfoArray   Array to fill
   @param[in/out]  pNumEntries         Number of entries in the array as input
                                       Number of entries filled as output
   @param[out]     pNumServices        Number of known services. If num_services
                                       > num_entries, a larger array may be needed


  @return
  ELITE_QMI_NO_ERR if function is successful, error code otherwise.

  @dependencies
  None. @newpage
 */
static inline elite_qmi_client_error_type elite_qmi_client_get_service_list (
      elite_qmi_idl_service_object_type   serviceObj,
      elite_qmi_service_info              *pServiceInfoArray,
      unsigned int                        *pNumEntries,
      unsigned int                        *pNumServices )
{
   return qmi_client_get_service_list(
            serviceObj,
            pServiceInfoArray,
            pNumEntries,
            pNumServices);
}

/** @ingroup elite_qmi_client_send_msg_async
   Sends an asynchronous QMI service message on the specified connection.
   The callback functi is expected to encode the message before sending it
   through this function.

   @param[in]   userHandle        Handle used by the infrastructure to
   @param[in]   unMsgID           Message ID
   @param[in]   pReqCStruct       Pointer to the request
   @param[in]   unReqCStructLen   Length of the request
   @param[in]   pRespCStruct      Pointer to where the response will be stored
   @param[in]   unRespCStructLen  Length of the response buffer
   @param[in]   respCB            Callback function to handle the response
   @param[in]   pRespCBData       Callback user-data
   @param[out]  pTxnHandle        Handle used to identify the transaction


  @return
  ELITE_QMI_NO_ERR and sets the transaction handle if function is successful,
  error code otherwise.
  ELITE_QMI_SERVICE_ERR if server cannot be reached

  @dependencies
  None. @newpage
 */
static inline elite_qmi_client_error_type elite_qmi_client_send_msg_async (
    elite_qmi_client_type                 userHandle,
    unsigned int                          unMsgID,
    void                                  *pReqCStruct,
    unsigned int                          unReqCStructLen,
    void                                  *pRespCStruct,
    unsigned int                          unRespCStructLen,
    elite_qmi_client_recv_msg_async_cb    respCB,
    void                                  *pRespCBData,
    elite_qmi_txn_handle                  *pTxnHandle )
{
   return qmi_client_send_msg_async (
            userHandle,
            unMsgID,
            pReqCStruct,
            unReqCStructLen,
            pRespCStruct,
            unRespCStructLen,
            respCB,
            pRespCBData,
            pTxnHandle );
}

/** @ingroup elite_qmi_client_init
   This function is used for initializing a connection to a service.

   @param[in]   pServiceInfo       Pointer to an entry in the service_info array
                                   returned by qmi_client_get_service_list()
   @param[in]   serviceObj         Service object
   @param[in]   indCB              Indication callback function
   @param[in]   pIndCBData         Indication callback user-data
   @param[in]   pOSParams          OS-specific parameters. It can be a pointer
                                   to event object, or signal mask and TCB
   @param[out]  pUserHandle        Handle used by the infrastructure to
                                   identify different clients.


  @return
  Sets the user_handle and returns ELITE_QMI_NO_ERR if successful,
  error code if not successful
  ELITE_QMI_SERVICE_ERR if server cannot be reached

  @dependencies
  None. @newpage
 */
static inline elite_qmi_client_error_type elite_qmi_client_init (
   elite_qmi_service_info                    *pServiceInfo,
   elite_qmi_idl_service_object_type         serviceObj,
   elite_qmi_client_ind_cb                   indCB,
   void                                      *pIndCBData,
   elite_qmi_client_os_params                *pOSParams,
   elite_qmi_client_type                     *pUserHandle )
{
   return qmi_client_init (
            pServiceInfo,
            serviceObj,
            indCB,
            pIndCBData,
            pOSParams,
            pUserHandle );
}

/** @ingroup elite_qmi_client_message_decode
   Decodes the body (TLV's) of a QMI message body from the wire format to the
   C structure.

   @param[in]  user_handle   opaque handle
                             object accessor function from service header file.
   @param[in]  req_resp_ind  The type of message: request, response, or indication.
   @param[in]  message_id    Message ID from IDL.
   @param[in]  p_src         Pointer to beginning of first TLV in message.
   @param[in]  src_len       Length of p_src buffer in bytes.
   @param[out] p_dst         Pointer to C structure for decoded data
   @param[in]  dst_len       Length (size) of p_dst C structure in bytes.


  @return
  Sets the user_handle and returns ELITE_QMI_NO_ERR if successful,
  error code if not successful
  ELITE_QMI_SERVICE_ERR if server cannot be reached

  @dependencies
  None. @newpage
 */
static inline elite_qmi_client_error_type elite_qmi_client_message_decode (
   elite_qmi_client_type                   user_handle,
   elite_qmi_idl_type_of_message_type      req_resp_ind,
   unsigned int                            message_id,
   const void                              *p_src,
   unsigned int                            src_len,
   void                                    *p_dst,
   unsigned int                            dst_len)
{
   return qmi_client_message_decode(
            user_handle,
            static_cast<qmi_idl_type_of_message_type>(req_resp_ind),
            message_id,
            p_src,
            src_len,
            p_dst,
            dst_len);
}

/** @ingroup elite_qmi_client_release
   Releases the connection to a service.

   @param[in]   user_handle        Handle used by the infrastructure to


  @return
  Sets the user_handle and returns ELITE_QMI_NO_ERR if successful,
  error code if not successful
  ELITE_QMI_SERVICE_ERR if server cannot be reached

  @dependencies
  None. @newpage
 */
static inline elite_qmi_client_error_type elite_qmi_client_release(
   elite_qmi_client_type user_handle )
{
   return qmi_client_release( user_handle );
}

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEQMICLIENTIF_H

