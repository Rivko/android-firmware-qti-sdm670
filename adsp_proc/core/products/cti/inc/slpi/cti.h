#ifndef CTI_H
#define CTI_H
/*==============================================================================
  FILE:         cti.h
  
  OVERVIEW:     This file provides the externs and declarations needed for
                target and image specific code

  DEPENDENCIES: None

                Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/products/cti/inc/slpi/cti.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "DALStdDef.h"
#include "DDIChipInfo.h"
#include "cti_top.h"

/* ==================================================================
                            Macros
   ================================================================== */
#define PROCESSOR_SELECT_MASK  SLPI_SELECT_VALUE ///<Processor select, which JTAG is expected to set in Shared IMEM

#define CTI_INTERRUPT_PORT 173                   ///<173 is for sdm845 CTI11_TRIGOUT->SLPI wakeup interrupt.

#undef ENABLE_ISLAND_CLOCKS_FOR_JTAG
#define ENABLE_ISLAND_CLOCKS_FOR_JTAG 1         ///<Flag to enable island clocks on CTI interrupt. For SSC Island only

#define ISLAND_PROCESSOR 1

#define CTI_INTERRUPT_PORT_2 7                  ///<7 is normal CTI interrupt for sdm845 SLPI

/* ==================================================================
                        Function Declarations
   ================================================================== */
#ifdef CTI_INTERRUPT_PORT_2
///Wrapper to normal CTI_Wakeup_Handler. Second interrupt registration needs a separate function.
extern void CTI_Wakeup_Handler_wrapper(uint32 param); 
#endif

#endif //CTI_H
