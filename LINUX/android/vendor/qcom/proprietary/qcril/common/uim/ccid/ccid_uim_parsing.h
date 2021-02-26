#ifndef CCID_UIM_PARSING_H
#define CCID_UIM_PARSING_H
/*===========================================================================

  Copyright (c) 2011, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/11   at      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "user_identity_module_v01.h"
#include "ccid_uim_daemon.h"

/*===========================================================================

                           DEFINES

===========================================================================*/

/*===========================================================================

                           TYPES

===========================================================================*/

/*=========================================================================

  FUNCTION:  ccid_uim_process_pc_to_rdr_bulk

===========================================================================*/
/*!
    @brief
    Performs the parsing and processing of BULK_OUT requests from BULK
    device node.

    @return
    None.
*/
/*=========================================================================*/
void ccid_uim_process_pc_to_rdr_bulk
(
  int           fd,
  uint8       * rx_buffer_ptr,
  uint16        rx_buffer_len
);


/*=========================================================================

  FUNCTION:  ccid_uim_process_pc_to_rdr_ctrl

===========================================================================*/
/*!
  @brief
    Performs the parsing and processing of CTRL request from CTRL device
    node.

  @return
    None
*/
/*=========================================================================*/
void ccid_uim_process_pc_to_rdr_ctrl
(
  int           fd,
  uint8       * rx_buffer_ptr,
  uint16        rx_buffer_len
);


/*=========================================================================

  FUNCTION:  ccid_uim_process_rdr_to_pc_bulk

===========================================================================*/
/*!
  @brief
    Performs the parsing and processing of response from QMI.

  @return
    None
*/
/*=========================================================================*/
void ccid_uim_process_rdr_to_pc_bulk
(
  int                            fd,
  uim_sap_request_resp_msg_v01 * qmi_response_ptr,
  ccid_uim_cmd_info_type       * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
);


#endif /* CCID_UIM_PARSING_H */

