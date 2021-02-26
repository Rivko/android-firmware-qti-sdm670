/*=============================================================================

FILE:         QdssTFunnel.c

DESCRIPTION:

================================================================================
                Copyright 2015 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/QdssDxe/QdssTFunnel.c#1 $
==============================================================================*/

#include "HalQdssTFunnel.h"

void QdssTFunnel_PreInit(void)
{
  HAL_qdss_tfunnel_HalInit();
  return;
}

void QdssTFunnel_Init(void)
{
  HAL_qdss_tfunnel_EnableStm(); //Enable port and funnels for STM
  return;
}

