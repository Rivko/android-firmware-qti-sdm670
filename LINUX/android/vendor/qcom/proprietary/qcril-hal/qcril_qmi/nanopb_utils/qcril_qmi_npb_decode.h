/******************************************************************************
#  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_npb_decode.h

  @brief

*/



#ifndef QCRIL_QMI_NPB_DECODE_H
#define QCRIL_QMI_NPB_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <comdef.h>
#include <pb.h>

boolean qcril_qmi_decode_npb(pb_istream_t *is, const pb_field_t pb_fields[], void* msg);

#ifdef __cplusplus
}
#endif

#endif
