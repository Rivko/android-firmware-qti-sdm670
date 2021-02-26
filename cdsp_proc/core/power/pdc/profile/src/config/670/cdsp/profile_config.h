#ifndef _PROFILE_CONFIG_H_
#define _PROFILE_CONFIG_H_
/*============================================================================
  FILE:         profile_config.h
  
  OVERVIEW:     This file provides the types for the PDC profiling target specific info
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/profile/src/config/670/cdsp/profile_config.h#1 $
$DateTime: 2018/07/30 01:21:36 $
============================================================================*/
#include "comdef.h"

/*===========================================================================
 *                            TYPES AND DEFINITIONS
 *===========================================================================*/
/* Profile units are defined via a map which calls given macros defined later */
#define PROFILE_ENUM_MAP(F)                                                     \
  /*enum type                     enum val  Enable */                           \
  F(PDC_PROFILE_POWER_DOWN_START, 0x01,     TRUE)  /* Sequence start */         \
  F(PDC_PROFILE_POWER_UP_END,     0x02,     TRUE)  /* Sequence done */          \
  F(PDC_PROFILE_POWER_DOWN_END,   0x04,     TRUE)  /* PCTL5 Power down done */  \
  F(PDC_PROFILE_INVALID,          0x08,     FALSE) /* Invalid for HW bug */     \
  F(PDC_PROFILE_POWER_UP_START,   0x10,     TRUE)  /* Alt power up start */

#endif /* _PROFILE_CONFIG_H_ */

