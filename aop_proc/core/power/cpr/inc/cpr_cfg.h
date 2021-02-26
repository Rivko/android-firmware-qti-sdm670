/**
 * @file:  cpr_cfg.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/cpr/inc/cpr_cfg.h#1 $
 * $Change: 15485400 $
 */
#ifndef CPR_CFG_H
#define	CPR_CFG_H

#include "cpr_types.h"

struct cpr_rail;

/*
 * Static read-only config for rails
 */
typedef struct cpr_rail
{
    cpr_domain_id id;
    uint8 railIdx;
    const char* name;
} cpr_rail;

#endif
