#ifndef DOG_HB_INTERNAL_H
#define DOG_HB_INTERNAL_H
/*
#============================================================================
#  Name:
#    dog_hb_internal.h
#
#  Description:
#    Watchdog heartbeat monitoring
#
# Copyright (c) 2015-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
#============================================================================
*/

#include "stdint.h"

#if !defined(DOG_HB_EXCLUDE_KERNEL_QURT)
#include "qurt.h"
#include "dog_hb_qurt.h"
#endif

#if !defined(DOG_HB_EXCLUDE_KERNEL_REX)
#include "rex.h"
#include "dog_hb_rex.h"
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

#define DOG_HB_CLIENT_NAME_LEN                    16                             /* Maximum Device Basename Length */

typedef enum
{
   DOG_HB_STATUS_SUCCESS = 0,
   DOG_HB_STATUS_ERROR = -1

} DOG_HB_STATUS;

struct dog_hb_device_s
{
   /* Driver object specific */
   char name[DOG_HB_CLIENT_NAME_LEN];
   int sig_t;                                /* OS Signal type */
   void* sig_p;                              /* Client supplied signal */
   unsigned int sig_m;                       /* Client supplied signal mask*/
   unsigned int ping;                        /* Ping count */

   struct dog_hb_device_s* next;
};

typedef struct dog_hb_device_s dog_hb_device_t, *dog_hb_device_p;

struct dog_hb_queue_s
{
   /* Link for the next queue */
   q_link_type link;

   unsigned int list_num;                     /* List number */
   unsigned int dog_hb_total_q_clients;       /* Total clients registered in this list */
   dog_hb_device_p q_ping_head;               /* Head of the list */;
};
typedef struct dog_hb_queue_s dog_hb_queue_t, *dog_hb_queue_p;

struct dog_hb_internal_s
{
   qurt_mutex_t mutex;                      /* Serialize using PI mutex */
   int mutex_inited;                        /* Flag set if dog_hb.mutex initialized */
   q_type *dog_hb_q;                        /* Queue of linked lists of type dog_hb_queue_p*/
   dog_report_type handle;                  /* Internal dog_hb client handle */
#if !defined(DOG_HB_EXCLUDE_KERNEL_QURT)
   qurt_anysignal_t anysignal;              /* Internal dog_hb client signal */
#else
   unsigned int anysignal;                  /* Internal dog_hb client signal */
#endif
};

typedef struct dog_hb_internal_s dog_hb_internal_t, *dog_hb_internal_p;

/** =====================================================================
* Function:
*     dog_hb_register
*
* Description:
*     Registers clients for watchdog hb monitoring
*
* Parameters:
*     task_name : Client task name
*     sig_t     : Client supplied signal type information. Qurt or Rex signal type
*     sig_p     : Client supplied signal information. Qurt or Rex signal
*     sig_m     : Client supplied mask information.
*
* Returns:
*     dog_report_type : Dog heartbeat handle returned back and should be greater than 0
* =====================================================================  */
dog_report_type dog_hb_register(char const* task_name, DOG_HB_SIG sig_t, void* sig_p, unsigned int sig_m);

/** =====================================================================
 * MACRO:
 *     DOG_HB_SIGNAL_SET_QURT & DOG_HB_SIGNAL_SET_REX
 *
 * Description:
 *     Sets the clients rex or qurt signal with maks value
 *
 * Parameters:
 *     sig_p     : Client supplied signal information. Qurt or Rex signal
 *     sig_m     : Client supplied mask information.
 *
 * Returns:
 *     None
 * =====================================================================  */
#if !defined(DOG_HB_EXCLUDE_KERNEL_QURT)
#define DOG_HB_SIGNAL_SET_QURT(sig_p, sig_m) qurt_anysignal_set(sig_p, sig_m)
#else
#define DOG_HB_SIGNAL_SET_QURT(sig_p, sig_m) 
#endif


#if !defined(DOG_HB_EXCLUDE_KERNEL_REX)
#define DOG_HB_SIGNAL_SET_REX(sig_p, sig_m) rex_set_sigs(sig_p, sig_m)
#else
#define DOG_HB_SIGNAL_SET_REX(sig_p, sig_m) 
#endif

#if defined(__cplusplus)
}
#endif

/* T32 instructions to see the 5 lists in the queue in table format for debugging :
 * Entry with ping = 4 (DOG_HB_PING_ERROR_CNT + 1) is the starved task.

V.FCHAIN %s (*((*(((dog_hb_queue_p)(*(dog_hb_internal.dog_hb_q)).link.next_ptr))).q_ping_head)) ;(*((*(((dog_hb_queue_p)(*(dog_hb_internal.dog_hb_q)).link.next_ptr))).q_ping_head)).next 

V.FCHAIN %s (*((*(((dog_hb_queue_p)(*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr))).q_ping_head)) ;(*((*(((dog_hb_queue_p)(*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr))).q_ping_head)).next 

V.FCHAIN %s (*((*(((dog_hb_queue_p)(*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr))).q_ping_head)) ;(*((*(((dog_hb_queue_p)(*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr))).q_ping_head)).next 

V.FCHAIN %s (*((*(((dog_hb_queue_p)(*((*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr)).next_ptr))).q_ping_head)) ;(*((*(((dog_hb_queue_p)(*((*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr)).next_ptr))).q_ping_head)).next 

V.FCHAIN %s (*((*(((dog_hb_queue_p)(*((*((*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr)).next_ptr)).next_ptr))).q_ping_head)) ;(*((*(((dog_hb_queue_p)(*((*((*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr)).next_ptr)).next_ptr))).q_ping_head)).next 

*/

/* T32 instructions to see the 5 lists in the queue in plain format for debugging :
v.v %s (dog_hb_queue_p)(*(dog_hb_internal.dog_hb_q)).link.next_ptr
v.v %s (dog_hb_queue_p)(*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr
v.v %s (dog_hb_queue_p)(*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr
v.v %s (dog_hb_queue_p)(*((*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr)).next_ptr
v.v %s (dog_hb_queue_p)(*((*((*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr)).next_ptr)).next_ptr

Starved device/task will be residing in the 5th list in the queue
v.v %s (dog_hb_queue_p)(*((*((*((*((*(dog_hb_internal.dog_hb_q)).link.next_ptr)).next_ptr)).next_ptr)).next_ptr)).next_ptr

*/

/* T32 instruction to look at the dog_hb_device_p info of the starved task : 
ERR_FATAL("DOG_HB detects starvation of task 0x%x, triage with its owner(d.dump 0x%x)", device_p_curr, device_p_curr, 0);

From above device_p_curr hex value (0xZZZZZZZZ) do:
v.v %s (dog_hb_device_p)0xZZZZZZZZ

*/

#endif
