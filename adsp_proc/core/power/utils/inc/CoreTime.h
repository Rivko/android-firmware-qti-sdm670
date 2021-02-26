#ifndef _CORE_TIME_H_
#define _CORE_TIME_H_
/*==============================================================================
  FILE:         CoreTime.h
  
  OVERVIEW:     Time related function/structure declarations

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreTime.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef CUST_H
 #include CUST_H
#endif

#include "CoreVerify.h"

#if defined( __cplusplus )
  #define CORE_STATIC_INLINE static inline
#else
  #define CORE_STATIC_INLINE static __inline
#endif
/* These functions are inlined below because they are in the critical path and
   should be sped up as much as possible.
 */
/**
   @brief CoreTimetick_Get - Get the current system
          time as 32KHz 32-bit data-type
   
   Read the 32 khz timetick value, and return it.
   
   Note: C STD guarantees 32-bit widths for long ints and above

   @return Current 32 KHz timetick
*/
/**
   @brief CoreTimetick_Get64 - Get the current system
          time as 19.2MHz 64-bit data-type
   
   Note: C STD guarantees 64-bit widths for long long ints and above

   @return Current QTimer System time (19.2 MHz tick-rate)
*/
#ifdef TARGET_UEFI

#include "TimetickUEFI.h"

CORE_STATIC_INLINE uint32 CoreTimetick_Get(void)
{ 
  uint32 result;
  Timetick_GetCount( TIMETICK_TIMER_GPT0, &result);
  return result;
}

CORE_STATIC_INLINE uint32 CoreTimetick_Get64(void)
{ 
#warning "CoreTimetick_Get64 unimplemented on UEFI"
  return 0;
}

#elif defined( USES_QURT ) || defined( FEATURE_QURT )
#include <stdint.h>
#include "qurt.h"
#include "timetick.h"

CORE_STATIC_INLINE uint32_t CoreTimetick_Get(void)
{ 
  return timetick_get_safe();
}

CORE_STATIC_INLINE uint64_t CoreTimetick_Get64(void)
{ 
  return qurt_sysclock_get_hw_ticks();
}

#elif defined( USES_BLAST ) || defined( FEATURE_BLAST )
#include "blast.h"
#include "timetick.h"

CORE_STATIC_INLINE uint32 CoreTimetick_Get(void)
{ 
  return timetick_get_safe();
}

CORE_STATIC_INLINE uint64 CoreTimetick_Get64(void)
{ 
#warning "CoreTimetick_Get64 not available for blast"
  return blast_system_sclk_attr_gethwticks();
}

#elif defined(WINSIM) || defined(WIN_DEBUGGER_EXTENSION)
#include "windows.h"
#include "DALStdDef.h"

CORE_STATIC_INLINE uint32 CoreTimetick_Get(void)
{ 
  return GetTickCount();
}

CORE_STATIC_INLINE uint64 CoreTimetick_Get64(void)
{ 
  return GetTickCount64();
}

#elif defined( USES_QNX ) || defined( FEATURE_QNX )
#include "timetick.h"

CORE_STATIC_INLINE uint32 CoreTimetick_Get(void)
{ 
  return timetick_get_safe();
}

CORE_STATIC_INLINE uint64 CoreTimetick_Get64(void)
{ 
  return timetick_get64();
}

#else
#include "DALStdDef.h"
#include "DDITimetick.h"

CORE_STATIC_INLINE uint32 CoreTimetick_Get(void)
{
  DalTimetickTime32Type ticks;
  static DalDeviceHandle *pTickHandle = NULL;

  if ( NULL == pTickHandle )
  {
    CORE_VERIFY( DAL_SUCCESS ==
                 DalTimetick_Attach( "SystemTimer", &pTickHandle ) );

    CORE_VERIFY_PTR( pTickHandle );
  }
  
  DalTimetick_Get( pTickHandle, &ticks );  

  return ticks;
}

CORE_STATIC_INLINE uint64 CoreTimetick_Get64(void)
{
  DalTimetickTime64Type ticks;
  static DalDeviceHandle *pTickHandle = NULL;

  if ( NULL == pTickHandle )
  {
    CORE_VERIFY( DAL_SUCCESS ==
                 DalTimetick_Attach( "SystemTimer", &pTickHandle ) );

    CORE_VERIFY_PTR( pTickHandle );
  }
  
  DalTimetick_GetTimetick64( pTickHandle, &ticks );  

  return ticks;
}

#endif // TARGET_UEFI

#endif /* _CORE_TIME_H_ */
