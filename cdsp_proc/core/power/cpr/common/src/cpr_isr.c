/**
 * @file:  cpr_isr.c
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/src/cpr_isr.c#1 $
 * $Change: 16759227 $
 */
#include "cpr_cfg.h"
#include "cpr_utils.h"
#include "cpr_rail.h"

uint32 cpr_isr_get_interrupt(cpr_domain_id railId)
{
    cpr_rail* rail = cpr_utils_get_rail( railId );
    return rail->interruptId;
}

void cpr_isr_process(cpr_domain_id railId)
{
    cpr_rail* rail = cpr_utils_get_rail( railId );
    cpr_rail_isr( rail );
}
