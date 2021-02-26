/*==============================================================================
@file npa_remote_qdi_protocol.h

NPA QDI Remote protocol header

Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1/power/npa/src/remote/npa_remote_qdi_protocol.h#1 $
==============================================================================*/
#ifndef NPA_REMOTE_QDI_PROTOCOL_H
#define NPA_REMOTE_QDI_PROTOCOL_H

#include "qurt_qdi_constants.h"

#define NPA_QDI_RESERVED_0                 (QDI_PRIVATE)
#define NPA_QDI_RESERVED_1                 (QDI_PRIVATE +  1)
#define NPA_QDI_RESERVED_2                 (QDI_PRIVATE +  2)
#define NPA_QDI_RESERVED_3                 (QDI_PRIVATE +  3)
#define NPA_QDI_RESERVED_4                 (QDI_PRIVATE +  4)
#define NPA_QDI_RESERVED_5                 (QDI_PRIVATE +  5)
#define NPA_QDI_RESERVED_6                 (QDI_PRIVATE +  6)
#define NPA_QDI_RESERVED_7                 (QDI_PRIVATE +  7)

#define NPA_QDI_CREATE_SYNC_CLIENT_EX      (QDI_PRIVATE +  8)
#define NPA_QDI_CREATE_ASYNC_CLIENT_CB_EX  (QDI_PRIVATE +  9)
#define NPA_QDI_DESTROY_CLIENT             (QDI_PRIVATE + 10)
#define NPA_QDI_ISSUE_SCALAR_REQUEST       (QDI_PRIVATE + 11)
#define NPA_QDI_MODIFY_REQUIRED_REQUEST    (QDI_PRIVATE + 12)
#define NPA_QDI_ISSUE_IMPULSE_REQUEST      (QDI_PRIVATE + 13)
#define NPA_QDI_ISSUE_VECTOR_REQUEST       (QDI_PRIVATE + 14)
#define NPA_QDI_ISSUE_ISOC_REQUEST         (QDI_PRIVATE + 15)
#define NPA_QDI_ISSUE_LIMIT_MAX_REQUEST    (QDI_PRIVATE + 16)
#define NPA_QDI_COMPLETE_REQUEST           (QDI_PRIVATE + 17)
#define NPA_QDI_CANCEL_REQUEST             (QDI_PRIVATE + 18)
#define NPA_QDI_SET_CLIENT_FORK_PREF       (QDI_PRIVATE + 19)
#define NPA_QDI_GET_CLIENT_FORK_PREF       (QDI_PRIVATE + 20)
#define NPA_QDI_JOIN_REQUEST               (QDI_PRIVATE + 21)
#define NPA_QDI_CREATE_EVENT_CB            (QDI_PRIVATE + 22)
#define NPA_QDI_SET_EVENT_WATERMARKS       (QDI_PRIVATE + 23)
#define NPA_QDI_SET_EVENT_THRESHOLDS       (QDI_PRIVATE + 24)
#define NPA_QDI_DESTROY_EVENT_HANDLE       (QDI_PRIVATE + 25)
#define NPA_QDI_CREATE_QUERY_HANDLE        (QDI_PRIVATE + 26)
#define NPA_QDI_DESTROY_QUERY_HANDLE       (QDI_PRIVATE + 27)
#define NPA_QDI_QUERY                      (QDI_PRIVATE + 28)
#define NPA_QDI_QUERY_BY_NAME              (QDI_PRIVATE + 29)
#define NPA_QDI_QUERY_BY_CLIENT            (QDI_PRIVATE + 30)
#define NPA_QDI_QUERY_BY_EVENT             (QDI_PRIVATE + 31)
#define NPA_QDI_QUERY_RESOURCE_AVAILABLE   (QDI_PRIVATE + 32)
#define NPA_QDI_RESOURCE_AVAILABLE         (QDI_PRIVATE + 33)
#define NPA_QDI_RESOURCES_AVAILABLE        (QDI_PRIVATE + 34)
#define NPA_QDI_GET_USERMALLOC_VECTOR      (QDI_PRIVATE + 35)

#define NPA_QDI_GET_PUBLISHED_RESOURCE_COUNT       NPA_QDI_RESERVED_0
#define NPA_QDI_GET_PUBLISHED_RESOURCE_DEFINITION  NPA_QDI_RESERVED_1


#define NPA_QDI_REMOTE_MAX_RESOURCE_UNITS_LEN   31
#define NPA_QDI_USERMALLOC_VECTOR_LEN           36

#endif /* NPA_REMOTE_QDI_PROTOCOL_H */
