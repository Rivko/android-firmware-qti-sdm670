/**
 * @file:  cpr_smem.h
 * @brief: 
 * 
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/inc/cpr_smem.h#1 $
 * $Change: 16759227 $
 */
#ifndef CPR_SMEM_H
#define CPR_SMEM_H

#include "cpr_cfg.h"
#include "cpr_data.h"

void cpr_smem_deserialize_config(cpr_rail* rail, cpr_rail_state* state,
        cpr_aging_cfg *agingCfg);
void cpr_smem_serialize_config(cpr_rail_state* state, boolean append);

#endif
