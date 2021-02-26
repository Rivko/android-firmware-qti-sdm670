#ifndef _USLEEP_QDI_H_
#define _USLEEP_QDI_H_
/*==============================================================================
  FILE:         uSleep_qdi.h

  OVERVIEW:     uSleep QDI Layer Header File

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/client/uSleep_qdi.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "qurt_qdi_driver.h"
#include "uSleep.h"
#include "uSleepi.h"

/*==============================================================================
                              DEFINES & TYPES
 =============================================================================*/
/* QDI opener data structure for uSleep */
typedef struct uSleep_qdi_opener_s 
{
  qurt_qdi_obj_t  qdiobj;
}uSleep_qdi_opener;

/* Invocation object */
typedef struct uSleep_qdi_data_s 
{
  qurt_qdi_obj_t  qdiobj;   /* Must be first */

  /* User defined data */
  uSleep_signals  *client_signals;
  uint32          enterLat;
  uint32          exitLat;
}uSleep_qdi_data;

/* QDI driver name */
#define USLEEP_QDI_DRIVER_NAME "/qdi/uSleep"

/* QDI function options */
#define USLEEP_QDI_EXIT                   (0+QDI_PRIVATE)
#define USLEEP_QDI_RESET_TRANS_STATS      (1+QDI_PRIVATE)

#define USLEEP_QDI_INSERT_CALLBACK        (2+QDI_PRIVATE)
#define USLEEP_QDI_ADD_CALLBACK_LATENCY   (3+QDI_PRIVATE)

/*==============================================================================
                            INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * uSleep_QDIInsertCallback
 * 
 * @brief QDI interface function to add a callback notification function 
 *  
 * @return Status from function call (see function for values)
 */
uint32 uSleep_QDIInsertCallback(int            clientHandle,
                                uSleep_signals *signal);

/**
 * uSleep_QDIAddCallbackLatency
 * 
 * @brief QDI interface function to set the current callback latency of the 
 *        function being registered
 *  
 * @return Status from function call (see function for values)
 */
uint32 uSleep_QDIAddCallbackLatency(int   clientHandle,
                                    int32 enterLat,
                                    int32 exitLat);

/**
 * uSleep_QDIResetTransStats
 * 
 * @brief QDI interface function to reset the transition statistics
 *  
 * @return Status from function call (see function for values)
 */
uint32 uSleep_QDIResetTransStats(int clientHandle);

/*==============================================================================
                            EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * uSleep_QDIInvoke
 * 
 * @brief Main QDI interface function to invoke user PD function calls 
 *  
 * @return Status from each function call (see functions for values) 
 */
int uSleep_QDIInvoke(int             client_handle,
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
 * uSleep_QDIInit
 * 
 * @brief QDI init function for uSleep
 */
void uSleep_QDIInit(void);

/**
 * uSleep_QDIOpen
 * 
 * @brief Main QDI function to open the QDI interface 
 *  
 * @return Handle value or error if negative
 */
int uSleep_QDIOpen(int client_handle, uSleep_qdi_data *me);

/**
 * uSleep_QDIRelease
 * 
 * @brief Release function for QDI registration.
 * 
 * @note This function should not ever be called since we will never
 *       deregister our QDI interface. However, QDI setup requires a release
 *       function, so it is provided here
 * 
 * @param obj: QDI object to relase 
 */ 
void uSleep_QDIRelease(qurt_qdi_obj_t *obj);

#endif /* _USLEEP_QDI_H_ */
