/*==============================================================================
  FILE:         pdcQdi_user.c

  OVERVIEW:     This file provides PDC API functionality for user PD's.
                This is compiled as part of any user PD

  DEPENDENCIES: Kernel/target QDI support for multi-PD
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/src/client_user/pdcQdi_user.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "pdcQdi_root.h"
#include "pdc_types.h"

/*==============================================================================
                              INTERNAL VARIABLES
 =============================================================================*/
/* QDI Client Handle to communicate with Guest OS layer. This variable needs to
 * be located in island section memory, but is initialized while in normal
 * operational mode */
static int g_pdcIntQDIClientHandle = -1;

/*==============================================================================
                         PDC INTERRUPT DRIVER API FUNCTIONS
 =============================================================================*/
/*
 *  pdcInt_config
 */
pdc_status_type pdcInt_config(uint32               subsystemInterrupt,
                              pdc_trigger_config   *config)
{
  pdc_status_type status;

  status = qurt_qdi_handle_invoke(g_pdcIntQDIClientHandle,
                                  PDC_QDI_INT_CONFIG,
                                  subsystemInterrupt,
                                  config);
  return(status);
}

/*
 *  pdcInt_enable
 */
pdc_status_type pdcInt_enable(uint32 subsystemInterrupt)
{
  pdc_status_type status;

  status = qurt_qdi_handle_invoke(g_pdcIntQDIClientHandle,
                                  PDC_QDI_INT_ENABLE,
                                  subsystemInterrupt);
  return(status);
}

/*
 *  pdcInt_disable
 */
pdc_status_type pdcInt_disable(uint32 subsystemInterrupt)
{
  pdc_status_type status;

  status = qurt_qdi_handle_invoke(g_pdcIntQDIClientHandle,
                                  PDC_QDI_INT_DISABLE,
                                  subsystemInterrupt);
  return(status);
}

/*==============================================================================
                          PDC GPIO DRIVER API FUNCTIONS
 =============================================================================*/
/*
 * pdcGpio_config
 */
pdc_status_type pdcGpio_config(uint32              gpioNumber,
                               pdc_trigger_config  *config,
                               uint32              *subsystemInt,
                               uint32              *secondarySubsystemInt)
{
  pdc_status_type status;

  status = qurt_qdi_handle_invoke(g_pdcIntQDIClientHandle,
                                  PDC_QDI_GPIO_CONFIG,
                                  gpioNumber,
                                  config,
                                  subsystemInt,
                                  secondarySubsystemInt);
  return(status);
}

/*
 * pdcGpio_unconfig
 */
pdc_status_type pdcGpio_unconfig(uint32 gpioNumber)
{
  pdc_status_type status;

  status = qurt_qdi_handle_invoke(g_pdcIntQDIClientHandle,
                                  PDC_QDI_GPIO_UNCONFIG,
                                  gpioNumber);
  return(status);
}

/*
 * pdcGpio_enable
 */
pdc_status_type pdcGpio_enable(uint32 gpioNumber)
{
  pdc_status_type status;

  status = qurt_qdi_handle_invoke(g_pdcIntQDIClientHandle,
                                  PDC_QDI_GPIO_ENABLE,
                                  gpioNumber);
  return(status);
}

/*
 * pdcGpio_disable
 */
pdc_status_type pdcGpio_disable(uint32 gpioNumber)
{
  pdc_status_type status;

  status = qurt_qdi_handle_invoke(g_pdcIntQDIClientHandle,
                                  PDC_QDI_GPIO_DISABLE,
                                  gpioNumber);
  return(status);
}

/*==============================================================================
                          PDC GENERAL DRIVER API FUNCTIONS
 =============================================================================*/
/**
 * pdc_clientQdiInit
 * 
 * @brief Client init function that run from each user PD that requires driver support
 */
void pdc_clientQdiInit(void)
{
  int clientPID = qurt_getpid();

  /* This function should not be called from guest OS PD.  A 0 value indicates
   * that is was. */
  CORE_VERIFY(0 != clientPID);

  /* Ensure this function is only called once per PD init */
  CORE_VERIFY(g_pdcIntQDIClientHandle < 0);
  
  /* Open a QDI handler for the driver API's */
  g_pdcIntQDIClientHandle = qurt_qdi_open(PDCINT_QDI_DRIVER_NAME);
  
  CORE_VERIFY(g_pdcIntQDIClientHandle >= 0);
  
  return;
}

