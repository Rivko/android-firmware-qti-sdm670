/**
 * @file:  cpr_isr.h
 * @brief:
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/common/inc/cpr_isr.h#1 $
 * $Change: 16759227 $
 */
#ifndef CPR_ISR_H
#define	CPR_ISR_H

#include "cpr_defs.h"

/**
 * Gets the CPR interrupt number for the given rail
 */
uint32 cpr_isr_get_interrupt(cpr_rail_id_t railId);

/**
 * Runs the CPR ISR for the given rail.
 */
void cpr_isr_process(cpr_rail_id_t railId);

#endif

