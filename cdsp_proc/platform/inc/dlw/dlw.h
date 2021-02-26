#ifndef DLW_H
#define DLW_H
/*==============================================================================

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#define DLW_RTLD_LAZY       RTLD_LAZY
#define DLW_RTLD_NOW        RTLD_NOW

#define	DLW_RTLD_NEXT       RTLD_NEXT
#define	DLW_RTLD_DEFAULT    RTLD_DEFAULT
#define	DLW_RTLD_SELF       RTLD_SELF

#define dlw_Init            dlinit
#define dlw_Open            dlopen
#define dlw_Close           dlclose
#define dlw_Sym             dlsym
#define dlw_Addr            dladdr
#define dlw_Error           dlerror

#endif // DLW_H
