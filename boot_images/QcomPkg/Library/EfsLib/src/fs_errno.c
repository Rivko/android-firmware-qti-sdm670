/***********************************************************************
 * fs_errno.c
 *
 * Implements function to get the address of efs_errno variable.
 *
 * Copyright (C) 2011 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-11-03   dks   Fix compiler error for boot.
2011-10-03   dks   Create.

===========================================================================*/

#include "fs_public.h"
#include "fs_err.h"
#include "fs_osal.h"
#include "fs_config_i.h"

#ifdef FEATURE_REX_OPAQUE_TCB_APIS
  extern int *rex_get_errno_addr (void);
#endif

/* Get the errno pointer value */
int*
efs_get_errno_address (void)
{
#ifdef FEATURE_REX_OPAQUE_TCB_APIS
  return rex_get_errno_addr ();
#else
  fs_os_tcb_type *tcb;

  tcb = fs_os_self ();
  if (tcb == NULL)
  {
    FS_ERR_FATAL ("null tcb !!!",0,0,0);
  }

  return &(tcb->err_num);
#endif
}

