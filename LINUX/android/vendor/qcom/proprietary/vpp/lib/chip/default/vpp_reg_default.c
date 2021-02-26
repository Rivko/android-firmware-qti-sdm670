/*!
 * @file vpp_reg_default.c
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * @services
 */

#include <sys/types.h>
#include "vpp_reg.h"

t_StVppUsecase ** ppstVppUsecase_GetRegistry(uint32_t *o_pu32Cnt)
{
    *o_pu32Cnt = 0;
    return NULL;
}

t_StVppUsecase * pstVppUsecase_GetAuto()
{
    return NULL;
}

