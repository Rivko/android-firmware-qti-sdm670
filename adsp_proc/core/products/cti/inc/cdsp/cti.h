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
$Header: //components/rel/core.qdsp6/2.1/products/cti/inc/cdsp/cti.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "DALStdDef.h"
#include "DDIChipInfo.h"
#include "cti_top.h"

/* ==================================================================
                            Macros
   ================================================================== */
#define PROCESSOR_SELECT_MASK CDSP_SELECT_VALUE     ///<Processor select, which JTAG is expected to set in Shared IMEM

#define CTI_INTERRUPT_PORT 31                       ///<31 is for sdm845 CDSP

#endif //CTI_H