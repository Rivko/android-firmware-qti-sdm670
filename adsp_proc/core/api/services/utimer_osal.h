#ifndef __UTIMER_OSAL_H__
#define __UTIMER_OSAL_H__

#include "uTimetick.h"

/* uTimetick definition */
typedef uTimetick_type              utimer_timetick_type;   /* uTimetick_type is 64 bit from utimetick.h */

/* Notify object and data types */
typedef void *                      utimer_osal_notify_obj_ptr;
typedef unsigned long int           utimer_osal_notify_data;

#endif /* __UTIMER_OSAL_H__ */