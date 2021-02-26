#ifndef _PDCTS_QDI_H_
#define _PDCTS_QDI_H_
/*==============================================================================
  FILE:         pdcts_qdi.h

  OVERVIEW:     PDC timestamp QDI Layer Header File

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/timestamp/src/client/pdcts_qdi.h#2 $
$DateTime: 2017/11/05 22:36:52 $
==============================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "qurt_qdi_driver.h"

/*==============================================================================
                              DEFINES & TYPES
 =============================================================================*/
/* QDI opener data structure */
typedef struct pdcts_qdi_opener_s 
{
  qurt_qdi_obj_t  qdiobj;
}pdcts_qdi_opener;

/* Invocation object */
typedef struct pdcts_qdi_data_s 
{
  qurt_qdi_obj_t  qdiobj;   /* Must be first */

  /* User data below */

}pdcts_qdi_data;

/* QDI driver name */
#define PDCTS_QDI_DRIVER_NAME   "/qdi/pdcTS"

/* QDI function options */
#define PDCTS_QDI_GET_DATA          (0+QDI_PRIVATE)
#define PDCTS_QDI_GET_ISLAND_DATA   (1+QDI_PRIVATE)
#define PDCTS_QDI_ENABLE            (2+QDI_PRIVATE)

/*==============================================================================
                            EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * pdcTimestamp_QDIInvoke
 * 
 * @brief Main QDI interface function to invoke user PD function calls 
 *  
 * @return Status from each function call (see functions for values) 
 */
int pdcTimestamp_QDIInvoke(int             client_handle,
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
 * pdcTimestamp_QDIInit
 * 
 * @brief QDI init function
 */
void pdcTimestamp_QDIInit(void);

/**
 * pdcTimestamp_QDIOpen
 * 
 * @brief Main QDI function to open the QDI interface 
 *  
 * @return Handle value or error if negative
 */
int pdcTimestamp_QDIOpen(int client_handle, pdcts_qdi_data *me);

/**
 * pdcTimestamp_QDIRelease
 * 
 * @brief Release function for QDI registration.
 * 
 * @param obj: QDI object to relase 
 */ 
void pdcTimestamp_QDIRelease(qurt_qdi_obj_t *obj);

#endif /* _PDCTS_QDI_H_ */
