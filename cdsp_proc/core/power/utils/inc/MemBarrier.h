/*==============================================================================
@file MemBarrier.h

Memory barrier macros for multi-core support.

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/MemBarrier.h#1 $
==============================================================================*/
#ifndef MEMBARRIER_H
#define MEMBARRIER_H

#ifdef WINSIM
#include "windows.h"
#else
#endif

#ifdef WINSIM

#define CORE_MEM_BARRIER()      MemoryBarrier()

#else

#define CORE_MEM_BARRIER()     

#endif

#endif /* MEMBARRIER_H */
