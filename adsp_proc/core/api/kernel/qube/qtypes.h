#ifndef QTYPES_H
#define QTYPES_H
/*=============================================================================

                 qtypes.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qtypes API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1/api/kernel/qube/qtypes.h#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include <stdint.h>
#include <stddef.h>

/*=============================================================================
                        CONSTANTS AND MACROS DECLARATIONS
=============================================================================*/
#define QTHREAD_NAME_LEN       16 
#define QTHREAD_MIN_STACKSIZE  4096
#define QTHREAD_MAX_STACKSIZE  1048576
#define QTHREAD_MIN_PRIORITY   0
#define QTHREAD_MAX_PRIORITY   252
#define QTHREAD_STATUS_DELETED 0xbabe
#define INVALID_ADDR	       (~0UL)

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

typedef uint32_t  qube_addr_t;
typedef uint32_t  qobject_t;

/**
 * Mutex type
 *
 * Local mutex is only accessable within a PD and shared mutex
 * can be used across PD 
 */ 
typedef enum {
    QMUTEX_LOCAL=0,
    QMUTEX_SHARED,
    QMUTEX_PRIORITY
} qmutex_type_t;

typedef struct {
 qmutex_type_t type;
} qmutex_attr_t;

/**
 * Permission to access an object, currrently it only applies to the qube
 * memory region object, specifying the permission for the dest PD to access
 * the memory region 
 */
typedef enum {
        QPERM_READ=0x4,
        QPERM_WRITE=0x2,
        QPERM_EXECUTE=0x1,
        QPERM_FULL=QPERM_READ|QPERM_WRITE|QPERM_EXECUTE,
} qperm_t;

#endif /* QTYPES_H */
