#ifndef QCRIL_UIM_HTTP_H
#define QCRIL_UIM_HTTP_H
/*===========================================================================

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/16   ks      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "user_identity_module_http_v01.h"
#include "qcril_uim.h"

/*===========================================================================

                           TYPES

===========================================================================*/

typedef enum
{
  QCRIL_UIM_HTTP_RESULT_SUCCESS = 0,
  QCRIL_UIM_HTTP_RESULT_FAILURE = 1,
  QCRIL_UIM_HTTP_RESULT_CONFRM_CODE_MISSING = 2
} qcril_uim_http_result_type;

typedef struct
{
  const char   * headerName;
  const char   * headerValue;
} qcril_uim_http_custom_header_req_type;

typedef struct
{
  char   * headerName;
  char   * headerValue;
} qcril_uim_http_custom_header_type;

typedef struct
{
  int32_t                             tokenId;
  uint8_t                           * payload;
  uint32_t                            payload_len;
  char                              * contentType;
  qcril_uim_http_custom_header_type * customHeaders;
  uint8_t                             no_of_headers;
  char                              * url;
} qcril_uim_http_transaction_ind_type;

typedef struct
{
  int32_t                                 tokenId;
  qcril_uim_http_result_type              result;
  const uint8_t                         * payload;
  uint32_t                                payload_len;
  qcril_uim_http_custom_header_req_type * customHeaders;
  uint8_t                                 no_of_headers;
} qcril_uim_http_transaction_req_type;


/*===========================================================================

                           FUNCTIONS

===========================================================================*/
/*===========================================================================

  FUNCTION:  qcril_uim_http_init

===========================================================================*/
void qcril_uim_http_init
(
  void
);


/*===========================================================================

  FUNCTION:  qcril_uim_http_release

===========================================================================*/
void qcril_uim_http_release
(
  void
);


/*===========================================================================

  FUNCTION:  qcril_uim_http_client_transaction_request

===========================================================================*/
void qcril_uim_http_client_transaction_request
(
  void                                 * token_ptr,
  qcril_uim_http_transaction_req_type  * http_req_ptr
);

/*===========================================================================

FUNCTION:  qcril_uim_lpa_http_txn_completed_response

===========================================================================*/
void qcril_uim_lpa_http_txn_completed_response
(
  void                       * token_ptr,
  qcril_uim_http_result_type   result
);

/*===========================================================================

FUNCTION:  qcril_uim_lpa_http_txn_indication

===========================================================================*/
void qcril_uim_lpa_http_txn_indication
(
  qcril_uim_http_transaction_ind_type * txn_ind_ptr
);

/*===========================================================================

FUNCTION:  qcril_uim_remote_client_register_service

===========================================================================*/
uint8_t qcril_uim_lpa_register_service
(
  int instanceId
);

#endif /* QCRIL_UIM_HTTP_H */

