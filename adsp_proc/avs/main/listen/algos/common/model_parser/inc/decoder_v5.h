/*============================================================================
  Copyright (c) 2012 - 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
============================================================================*/

#ifndef __DECODE_V5_H__
#define __DECODE_V5_H__

#include "sva_model_parser.h"

#if defined(__cplusplus)
extern "C" {
#endif /* #if defined(__cplusplus) */

int sva_model_decoder_v5(uint8 *buffer, SVA_ModelTypeV5 *model);
int sva_model_header_decoder_v5(uint8 *buffer, SVA_ModelTypeV5 *model);
int sva_model_configuration_decoder_v5(uint8 *buffer, SVA_ModelTypeV5 *model);
int sva_model_keyword_physical_state_decoder_v5(uint8 *buffer, SVA_ModelTypeV5 *model);
int sva_model_user_physical_state_decoder_v5(uint8 *buffer, SVA_ModelTypeV5 *model);
int sva_model_keyword_model_decoder_v5(uint8 *buffer, SVA_ModelTypeV5 *model);
int sva_model_user_model_decoder_v5(uint8 *buffer, SVA_ModelTypeV5 *model);

#if defined(__cplusplus)
};
#endif /* #if defined(__cplusplus) */

#endif /* __DECODE_V5_H__ */
