/***********************************************************************
 * fs_protected_memory.h
 *
 * Section markers for EFS protected memory.
 *
 * Copyright (C) 2007-2009,2013 QUALCOMM Technologies, Inc.
 *
 * Files that contain private memory include this header to mark the
 * sections appropriately.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/inc/fs_protected_memory.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-06-17   wek   Move only ZI buffers to DDR.
2013-04-26   wek   Move all buffers to DDR when compiling for boot.
2009-05-21   sh    Changed featurizing to exclude gcc.
2009-05-18   rp    Featurized for ARM compiler.
2008-10-17   yog   Featurized pragma directive to exclude for FS_STANDALONE.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-07-05   umr   Create

===========================================================================*/

#ifndef __FS_PROTECTED_MEMORY_H__
#define __FS_PROTECTED_MEMORY_H__

static inline void fs_efs2_unprotect_memory (void)
{
}

static inline void fs_efs2_protect_memory (void)
{
}

#endif /* not __FS_PROTECTED_MEMORY_H__ */
