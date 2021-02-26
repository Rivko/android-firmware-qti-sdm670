/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#define UIM_HTTP_HEADER_NAME_MAX_LEN 32
#define UIM_HTTP_HEADER_VALUE_MAX_LEN 256
#define UIM_HTTP_CUST_HEADER_MAX_COUNT 10
#define UIM_HTTP_URL_MAX_LEN 512

typedef enum
{
  LPA_QMI_UIM_HTTP_INVALID_RSP,
  LPA_QMI_UIM_HTTP_TRANSACTION_RSP
} lpa_qmi_uim_http_rsp_id_type;

typedef enum {
  UIM_HTTP_TRANSACTION_SUCCESSFUL = 0x0, 
  UIM_HTTP_UNKNOWN_ERROR = 0x1, 
  UIM_HTTP_SERVER_ERROR = 0x2, /**<  Server error \n  */
  UIM_HTTP_TLS_ERROR = 0x3, /**<  TLS error    \n  */
  UIM_HTTP_NETWORK_ERROR = 0x4, /**<  Network error \n  */
} uim_http_result_type_enum;

typedef struct
{
  char name[UIM_HTTP_HEADER_NAME_MAX_LEN + 1];
  char value[UIM_HTTP_HEADER_VALUE_MAX_LEN + 1];
} uim_http_header_type;

typedef struct
{
  char content_type[UIM_HTTP_HEADER_VALUE_MAX_LEN + 1];
  uint32_t custom_header_len;
  uim_http_header_type custom_header[UIM_HTTP_CUST_HEADER_MAX_COUNT];
} uim_http_req_info_type;

typedef struct
{
  int                          qmi_error_code;
  int                          transp_err;
  int                          token;
  lpa_qmi_uim_http_rsp_id_type rsp_id;
} lpa_qmi_uim_http_rsp_data_type;
