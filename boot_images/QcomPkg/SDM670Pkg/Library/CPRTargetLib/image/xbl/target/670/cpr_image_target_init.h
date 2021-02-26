/**
 * @file:  cpr_image_target_init.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/CPRTargetLib/image/xbl/target/670/cpr_image_target_init.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_IMAGE_TARGET_INIT_H
#define CPR_IMAGE_TARGET_INIT_H

//******************************************************************************
// Default Enablement Structures
//******************************************************************************
struct cpr_cfg_funcs;

typedef enum
{
    CPR_INIT_NONE, /* used to disable CPR */
    CPR_INIT_OPEN_LOOP,
    CPR_INIT_SW_CLOSED_LOOP,
    CPR_INIT_HW_OPEN_LOOP,  /*used to share cpr info via SMEM with out initializing any of the CPR hw */
    CPR_INIT_HW_CLOSED_LOOP,/*used to share cpr info via SMEM with out initializing any of the CPR hw */

    CPR_INIT_FUNCS_COUNT
} cpr_init_funcs_id;

struct cpr_cfg_funcs* cpr_image_target_get_init_func(cpr_init_funcs_id func_id);

#endif

