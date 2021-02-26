#pragma once
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#if defined(__clang__) || defined(__GNUC__)
#define QCRIL_ERROR_ATTRIBUTE(msg) __attribute__((deprecated(msg)))
#else
#define QCRIL_ERROR_ATTRIBUTE(msg)
#endif

/**
 * ril_request_info base template. This is used to catch unregistered types
 * Specializations are created by using QCRIL_ANDROID_DECLARE_REQUEST.
 */
template <int REQUEST_ID>
struct ril_request_info
{
};


#if defined(__has_attribute)
#   if __has_attribute(argument_with_type_tag) && \
            __has_attribute(pointer_with_type_tag) && \
                __has_attribute(type_tag_for_datatype)
#    define qcril_tag_func(buffer_idx, type_idx) __attribute__((pointer_with_type_tag(qcril_android_tag,buffer_idx,type_idx)))
#    define qcril_tag_struct(type) __attribute__((type_tag_for_datatype(qcril_android_tag, type)))
#   else
#       define qcril_tag_func(buffer_idx, type_idx)
#       define qcril_tag_struct(type)
#   endif
#else
#   define qcril_tag_func(buffer_idx, type_idx)
#   define qcril_tag_struct(type)
#endif

struct ril_request_type
{
    qcril_instance_id_e_type instance_id;
    int req_id_android;
    qcril_evt_e_type req_id;
    const void *payload;
    size_t payload_len;
    RIL_Token t;
};

struct ril_response_type
{
    qcril_instance_id_e_type instance_id;
    int req_id;
    RIL_Token t;
    RIL_Errno e;
    void *response;
    size_t response_len;
};

struct ril_unsol_type
{
    qcril_instance_id_e_type instance_id;
    int unsol_id;
    void *payload;
    size_t payload_len;
};

