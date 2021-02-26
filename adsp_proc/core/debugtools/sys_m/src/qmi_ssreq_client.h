#ifndef QMI_SSREQ_CLIENT_H
#define QMI_SSREQ_CLIENT_H
/*===========================================================================

           QMI_SSREQ_CLIENT . H

  Copyright (c) 2014 -16 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/debugtools/sys_m/src/qmi_ssreq_client.h#1 $

    when       who     what, where, why
   --------   ---     ---------------------------------------------------------- 
   11/04/14   rks      Added reason code support with SSREQ req
   10/09/14   rks     added API ssreq_ser_check_for_shutdown_support()
   06/26/14   rks      Initial Version
   
   ===========================================================================*/

/*===========================================================================
   
                        INCLUDE FILES FOR MODULE
   
   ===========================================================================*/

#include "qmi_csi.h"
#include "qmi_client.h"
#include "sys_m_reset.h"

/*===========================================================================

  FUNCTION:  get_ssreq_client_init_status

===========================================================================*/
/*!
    @brief
    Provides the status for ssreq qmi service polling.

    @detail
    Provides the status for SSREQ qmi service availability.

    @return
    get_ssreq_client_init_status variable value, which is updated in ssreq_qmi_poll_task().

*/
/*=========================================================================*/
boolean get_ssreq_client_init_status( void );
/*===========================================================================

  FUNCTION:  ssreq_process_request

===========================================================================*/
/*!
    @brief
    Send a request to the SSREQ server at APSS to process SSREQ cmd

    @detail

    @return
    SSREQ_SUCCESS   : request was succesful
    SSREQ_FAILURE   : request failed
    SSREQ_IN_PROCESS: already a request is in process
    
    *Note*: This API is used only in sys_m.c and called inside SYS_M_LOCK lock
    So right now do not have its own mutex created for concurrency protection
*/
/*=========================================================================*/
ssreq_status_t ssreq_process_request(
              uint32 cmd, ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
              ssreq_qmi_request_res_code_type res_code);

/*===========================================================================

  FUNCTION:  ssreq_ser_check_for_shutdown_support

===========================================================================*/

/*!
   @brief
    Check if SSREQ Service on HLOS supports the shutdown request or not
    based on the SSREQ IDL minor version

    @detail

    @return
    TRUE  : if supports
    FALSE : if don't support

*/
boolean ssreq_ser_check_for_shutdown_support(void);
/*===========================================================================

  FUNCTION:  ssreq_client_ind_cb

===========================================================================*/
/*!
    @brief
    Helper function for the client indication callback

    @detail
    This function send back the indication result to the 
    orignal request initiator client if registered the
    ssreq_qmi_ind_cb callback

    @return
    None

*/
/*=========================================================================*/

void ssreq_client_ind_cb
(
 qmi_client_type                client_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
);

#endif  /* QMI_SSREQ_CLIENT_H */
