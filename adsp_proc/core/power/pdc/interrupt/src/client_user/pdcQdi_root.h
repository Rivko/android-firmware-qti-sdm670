#ifndef _PDCINT_QDI_H_
#define _PDCINT_QDI_H_
/*==============================================================================
  FILE:         pdcQdi_root.h

  OVERVIEW:     QDI layer header file

  DEPENDENCIES: None
  
                Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/src/client_user/pdcQdi_root.h#2 $
$DateTime: 2017/11/20 02:13:18 $
==============================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "qurt_qdi_driver.h"

extern const uint32           g_pdcInterruptTableSize;
extern const uint32           g_pdcGPIOMappingSize;

#define PDC_INTMASK_SIZE  ((g_pdcInterruptTableSize / 32 ) + 1)
#define PDC_GPIOMASK_SIZE ((g_pdcGPIOMappingSize / 32 ) + 1)

/*==============================================================================
                              DEFINES & TYPES
 =============================================================================*/
/* QDI opener data structure */
typedef struct pdc_qdi_opener_s 
{
  qurt_qdi_obj_t  qdiobj;
  uint32         *pIntMask;
  uint32         *pGpioMask;
}pdc_qdi_opener;

/* QDI driver name */
#define PDCINT_QDI_DRIVER_NAME "/qdi/pdcInt"

/* QDI function options */
#define PDC_QDI_INT_CONFIG         (0+QDI_PRIVATE)
#define PDC_QDI_INT_ENABLE         (1+QDI_PRIVATE)
#define PDC_QDI_INT_DISABLE        (2+QDI_PRIVATE)

#define PDC_QDI_GPIO_CONFIG        (3+QDI_PRIVATE)
#define PDC_QDI_GPIO_ENABLE        (4+QDI_PRIVATE)
#define PDC_QDI_GPIO_DISABLE       (5+QDI_PRIVATE)
#define PDC_QDI_GPIO_UNCONFIG      (6+QDI_PRIVATE)

/*==============================================================================
                            EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * pdcQdi_invoke
 * 
 * @brief Main QDI interface function to invoke user PD function calls 
 *  
 * @return Status from each function call (see functions for values) 
 */
int pdcQdi_invoke(int             clientHandle,
                  qurt_qdi_obj_t  *pobj,
                  int             method,
                  qurt_qdi_arg_t  a1,
                  qurt_qdi_arg_t  a2,
                  qurt_qdi_arg_t  a3,
                  qurt_qdi_arg_t  a4,
                  qurt_qdi_arg_t  a5,
                  qurt_qdi_arg_t  a6,
                  qurt_qdi_arg_t  a7,
                  qurt_qdi_arg_t  a8,
                  qurt_qdi_arg_t  a9);

/**
 * pdcQdi_initialize
 * 
 * @brief QDI init function for the PDC driver
 */
void pdcQdi_initialize(void);

#endif /* _PDCINT_QDI_H_ */

