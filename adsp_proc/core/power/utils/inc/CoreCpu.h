/*==============================================================================
@file CoreCpu.h

This file provides the public API for CPU related functions.
The list of function is expected to grow as need arises.

Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreCpu.h#1 $
==============================================================================*/
#ifndef CORE_POWERUTILS_CPU_H
#define CORE_POWERUTILS_CPU_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * @brief Return the core id for the core executing this function. The
 *        range start from 0 to N-1 where N is number of cores on the
 *        given processor. Implementation will vary based on underlying
 *        HW architecture.
 */
unsigned int Core_GetCurrCoreID(void);

#ifdef __cplusplus
}
#endif

#endif /* CORECPU_H */
