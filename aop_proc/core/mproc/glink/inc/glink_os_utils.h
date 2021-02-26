#ifndef GLINK_OS_UTILS_H
#define GLINK_OS_UTILS_H

/*===========================================================================

                      G-Link OS Specific types/functions


 Copyright (c) 2014-2015 by QUALCOMM Technologies, Incorporated.  
 All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/mproc/glink/inc/glink_os_utils.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/16    dks     Initial version. 
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "glink.h"
#include "rex.h"
#include "err.h"
#include <string.h>
#include "assert.h"
#ifndef ASSERT
#define ASSERT assert
#endif

#define ROUNDUP64(d) (((d) + 7) & (~7))
#define ROUNDUP32(d) (((d) + 3) & (~3))

#define ALIGNED32(d) ((d&0x3) == 0)
#define ALIGNED64(d) ((d&0x7) == 0)
#define ALIGNED(x,d) (ALIGNED##x(d))

#define GLINK_LOG_ERROR_EVENT(...)         
#define glink_os_string_copy strncpy
#define glink_os_register_isr(ID,pfnIRQ,pArg) rex_set_interrupt_vector(ID, pfnIRQ)
#define glink_os_clear_interrupt(pId) InterruptController_ClearPending(pId)
#define glink_os_enable_interrupt(pId) rex_enable_interrupt(pId)
#define glink_os_disable_interrupt(pId) rex_disable_interrupt(pId)

typedef uint32 glink_os_ipc_intr_type;
typedef void ( *glink_os_isr_cb_fn )( int );
typedef rex_crit_sect_type os_cs_type;

void glink_os_send_interrupt( glink_os_ipc_intr_type id );
void glink_os_mem_set(void* ptr,int value,unsigned int size);

/* Copies up-to copy_len bytes from src to dst. Returns the bytes actually
 * copied */
#define glink_os_mem_copy(dst, dst_len, src, copy_len) \
            memcpy(dst, src, copy_len)

#define glink_os_cs_init(sem) rex_init_crit_sect(sem)
#define glink_os_cs_acquire(sem) rex_enter_crit_sect(sem)
#define glink_os_cs_release(sem) rex_leave_crit_sect(sem)
#define glink_os_cs_deinit(sem) rex_del_crit_sect(sem)

#endif /* GLINK_OS_UTILS_H */
