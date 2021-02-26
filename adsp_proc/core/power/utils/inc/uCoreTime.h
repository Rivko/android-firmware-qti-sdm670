#ifndef _U_CORE_TIME_H_
#define _U_CORE_TIME_H_
/*==============================================================================
  FILE:         uCoreTime.h

  OVERVIEW:     Time related function/structure declarations used in island/uImage mode

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/uCoreTime.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef CUST_H
 #include CUST_H
#endif

#ifndef CORE_STATIC_INLINE
  #if defined(__cplusplus)
    #define CORE_STATIC_INLINE static inline
  #else
    #define CORE_STATIC_INLINE static __inline
  #endif
#endif

/** 
 * uCoreTimetick_Get64
 * 
 * @brief Get the current system time as 19.2MHz (XO) 64-bit data-type 
 * @return Current QTimer System time (19.2 MHz tick-rate) 
 */
#if defined(USES_QURT) || defined(FEATURE_QURT)
  #include "comdef.h"
  #include "uTimetick.h"
  
  CORE_STATIC_INLINE uint64 uCoreTimetick_Get64(void)
  { 
    return ((uint64)uTimetick_Get());
  }

#else

  #error "uCoreTImetick_get64 currently only supports Qurt"

#endif // uCoreTimetick_Get64

#ifdef __cplusplus
}
#endif

#endif /* _U_CORE_TIME_H_ */

