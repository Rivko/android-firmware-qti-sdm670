/******************************************************************************
#  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_npb_encode.h

  @brief

*/


#ifndef QCRIL_QMI_NPB_ENCODE_H
#define QCRIL_QMI_NPB_ENCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <comdef.h>
#include <pb.h>

boolean qcril_qmi_encode_npb(pb_ostream_t *os, const pb_field_t pb_fields[], const void *msg);

#ifdef __cplusplus
}
#endif

#endif
