/*============================================================================
  Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
============================================================================*/
#ifndef __DECODE_GLOBAL_HEADER_H__
#define __DECODE_GLOBAL_HEADER_H__

#include "sva_model_parser.h"
#include "sva_model_parser_utils.h"

#if defined(__cplusplus)
extern "C" {
#endif

int sva_model_global_header_decoder(uint8 *buffer, SVA_GlobalHeaderType *model);

#if defined(__cplusplus)
};
#endif


#endif /* __DECODE_GLOBAL_HEADER_H__ */
