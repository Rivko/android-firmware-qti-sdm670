#ifndef CTI_H
#define CTI_H
/*==============================================================================
  FILE:         cti.h
  
  OVERVIEW:     This file provides the externs and declarations needed for
                target specific code

  DEPENDENCIES: None

                Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/products/cti/inc/adsp/cti.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "DALStdDef.h"
#include "DDIChipInfo.h"
#include "cti_top.h"

/* ==================================================================
                            Macros
   ================================================================== */
#define PROCESSOR_SELECT_MASK ADSP_SELECT_VALUE  ///<Processor select, which JTAG is expected to set in Shared IMEM

#define CTI_INTERRUPT_PORT 31                   ///<31 is for sdm845 ADSP



#endif //CTI_H