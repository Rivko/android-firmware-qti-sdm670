#ifndef DOG_VIRTUAL_H
#define DOG_VIRTUAL_H
/*
#============================================================================
#  Name:
#    dog_virtual.h 
#
#  Description:
#    Implements QDI layer for virtual dog
#
# Copyright (c) 2015,2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "qurt.h"
#include "qurt_qdi_driver.h"   /* Including header only in this file */
#include "dog_hb_qurt.h"
#include "dog_timeouts.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define DOG_QDI_DEVICE_NAME_LEN                    QURT_MAX_NAME_LEN              /* INTERNAL, Maximum Device Basename Length */
#define DOG_QDI_DEVICE                             "/dev/dog"                     /* INTERNAL, Device Prefix */
#define DOG_QDI_PING_ERROR_CNT                     3                              /**< INTERNAL, Ping No Response Error Count */
#define DOG_VIRTUAL_TASK_STACK                     2048
#define DOG_VIR_SIGNAL_MASK                        0x1

#define DOG_QDI_HB_REGISTER                        (0+QDI_PRIVATE)
#define DOG_QDI_WAIT                               (1+QDI_PRIVATE)
#define DOG_QDI_PONG                               (2+QDI_PRIVATE)

typedef enum 
{
   DOG_VIRTUAL_SUCCESS        = 0,
   DOG_VIRTUAL_FAILURE        = -1
}DOG_VIRTUAL_RESULT;

/* Virtual Dog QDI Data associated with each user - PD */
struct dog_qdi_device_data_s
{
   qurt_sysenv_procname_t pn;
   char name[DOG_QDI_DEVICE_NAME_LEN];
   qurt_anysignal_t sig_t;
   dog_report_type dog_vir_hb_handle;
};
typedef struct dog_qdi_device_data_s dog_qdi_device_data_t, * dog_qdi_device_data_p;

/* Virtual Dog User PD QDI structure */
struct dog_qdi_device_s
{
   qurt_qdi_obj_t qdiobj;                                                            /* Required to be first */
   dog_qdi_device_data_t dog_user_pd_data;
   struct dog_qdi_device_s *next;
};
typedef struct dog_qdi_device_s dog_qdi_device_t, * dog_qdi_device_p;

/* Virtual Dog Data associated with each user - PD for grace timer*/
struct dog_virtual_grace_timer_data_s
{
   qurt_sysenv_procname_t pn;
   timer_type dog_pd_grace_timer;
   boolean dog_pd_grace_timer_set;
   struct dog_virtual_grace_timer_data_s *next;
};
typedef struct dog_virtual_grace_timer_data_s dog_virtual_grace_timer_data_t, * dog_virtual_grace_timer_data_p;

/** =====================================================================
 * Function:
 *     dog_qdi_invoke
 *
 * Description: 
 *     This gives the canonical form for the arguments to a QDI
 *     driver invocation function.
 * 
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.  
 *
 *     qurt_qdi_obj_t : Points at the qdi_object_t structure
 *                      on which this QDI request is being made.
 *
 *     int method     : The integer QDI method which represents
 *                      the request type.
 *
 *     qurt_qdi_arg_t a1 to a3 :  The first three general purpose arguments
 *                                to the invocation function are passed in
 *                                these slots.
 *
 *     qurt_qdi_arg_t a4 to a9 :   Arguments beyond the first three are
 *                                 passed on the stack.
 * Returns:
 *     -1 for failure and 0 for success
 * =====================================================================  */
int dog_qdi_invoke(int client_handle,
                       qurt_qdi_obj_t* obj,
                       int dog_qdi_method,
                       qurt_qdi_arg_t a1,
                       qurt_qdi_arg_t a2,
                       qurt_qdi_arg_t a3,
                       qurt_qdi_arg_t a4,
                       qurt_qdi_arg_t a5,
                       qurt_qdi_arg_t a6,
                       qurt_qdi_arg_t a7,
                       qurt_qdi_arg_t a8,
                       qurt_qdi_arg_t a9
                      );

/** =====================================================================
 * Function:
 *     dog_qdi_init
 *
 * Description:
 *     Initialization function. Registers a QDI device with the generic QDI object
 *
 * Parameters:
 *     None
 * Returns:
 *     None
 * =====================================================================  */
void dog_qdi_init(void);

#if defined(__cplusplus)
}
#endif

#endif