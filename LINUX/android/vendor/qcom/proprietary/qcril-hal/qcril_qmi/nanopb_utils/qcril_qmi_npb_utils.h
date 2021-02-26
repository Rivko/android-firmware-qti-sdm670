/******************************************************************************
#  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_npb_utils.h

  @brief

*/


#ifndef QCRIL_QMI_NPB_UTILS_H
#define QCRIL_QMI_NPB_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pb.h>
#include <comdef.h>

typedef struct
{
    const pb_field_t *pos;
    const void *dest_struct;
    void *cur_data_ptr;
} qcril_qmi_npb_field_iter_type;

boolean qcril_qmi_npb_field_init(qcril_qmi_npb_field_iter_type *iter, const pb_field_t *fields, const void *dest_struct);

boolean qcril_qmi_npb_field_next(qcril_qmi_npb_field_iter_type *iter);

boolean qcril_qmi_npb_get_data_size(const pb_field_t fields[], size_t* dataSize);

void qcril_qmi_npb_release(const pb_field_t fields[], void *dest_struct);
#ifdef __cplusplus
}
#endif
#endif
