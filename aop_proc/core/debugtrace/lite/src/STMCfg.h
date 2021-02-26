/*=============================================================================

FILE:         STMCfg.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/lite/src/STMCfg.h#1 $
==============================================================================*/

#ifndef STMCFG_H
#define STMCFG_H

#include "HALcomdef.h"

void STMCfgInit(void);
void STMSetTimestampFreq(uint32 dwFreq);
boolean STMAccessStatusNSNID(void);
#endif
