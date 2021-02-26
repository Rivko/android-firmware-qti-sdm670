/*============================================================================== 
  FILE:         pdc_hwio.c

  OVERVIEW:     PDC hwio init

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/hwio/pdc_hwio.c#2 $
$DateTime: 2017/10/16 10:14:04 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "pdc_hwio.h"
#include "DDIChipInfo.h"

/*==============================================================================
                         EXTERNAL GLOBAL VARIABLES
 =============================================================================*/
extern uint32 g_pdcHWIObase;

/*==============================================================================
                           EXTERNAL FUNCTIONS
 =============================================================================*/
void pdcHwio_initialize(void)
{
  int32 offset = 0;

#if defined(PDC_IMAGE_MSS) || defined (PDC_IMAGE_CDSP)
  DalChipInfoVersionType  version = DalChipInfo_ChipVersion();

  if(version >= CHIPINFO_VERSION(2, 0))
  {
    /* Adjust offset based on version */
#ifdef PDC_IMAGE_CDSP
    offset = -0x20000;
#else
    offset = 0x10000;
#endif
  }
#endif

  g_pdcHWIObase = (int32)PDC_HWIO_BASE_ADDR + offset;

  return;
}
