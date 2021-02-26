/*=========================================================================

                     Shared Memory Driver OS Source File

 Copyright (c)  2011-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/14   bm      64bit support
04/23/13   sm      Removed string copy macro.
03/21/13   bm      Implement memcpy routine to ensure word aligned access
08/03/12   bm      Add function smd_initialize_static for SMD static lib init
04/10/12   vm      Added pool tags for memory allocations
08/02/11   tl      Print ERR_FATAL messages in both checked and free builds
07/12/11   bm      + Moved NULL interrupt definitions to this file
                   + Typedefined smd_os_proc_type and smd_os_intr_type.
07/06/11   tl      Removed unused SMD_EVENT_SIG
04/08/11   tl      Use regular spinlocks, rather than stack-queued spinlocks
01/28/11   tl      Initial EA support for SMD
===========================================================================*/

#ifndef SMD_OS_H
#define SMD_OS_H

/*===========================================================================
                                INCLUDE FILES
===========================================================================*/
#include "com_dtypes.h"
#include "comdef.h"
#include "string.h"
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*===========================================================================
                          EXTERNAL MACRO DEFINITIONS
===========================================================================*/
void *smd_32b_algn_memcpy
(
 void *dest,
 void *src,
 unsigned int size
);

#define SMD_READ_SMEM_1( addr ) ( *( ( uint8* )( addr ) ) )
                                                                                
#define SMD_READ_SMEM_2( addr ) ( *( ( uint16* )( addr ) ) )
                                                                                
#define SMD_READ_SMEM_4( addr ) ( *( ( uint32* )( addr ) ) )
                                                                                
#define SMD_WRITE_SMEM_1( addr, val ) \
  ( *( ( uint8 *)( addr ) )  = ( uint8 )( val ) )   
                                                                                
#define SMD_WRITE_SMEM_2( addr, val ) \
  ( *( ( uint16 *)( addr ) ) = ( uint16 )( val ) )  
                                                                                
#define SMD_WRITE_SMEM_4( addr, val ) \
  ( *( ( uint32 *)( addr ) ) = ( uint32 )( val ) )  
                                                                                
#define SMD_MEMSET_SMEM( addr, val, size )     SetMem( addr, val, size )
                                                                                
#define SMD_MEMCPY_FROM_SMEM( dst, src, size ) smd_32b_algn_memcpy( (void*)(dst), (void*)(src), size )
                                                                                
#define SMD_MEMCPY_TO_SMEM( dst, src, size )   smd_32b_algn_memcpy( (void*)(dst), (void*)(src), size )

#define SMD_MEMCPY_FROM_TO_SMEM( dst, src, size ) smd_32b_algn_memcpy( (void*)(dst), (void*)(src), size )

/* -------------------------------------------------------------
   Memory barrier
------------------------------------------------------------- */
#define SMD_MEMORY_BARRIER()  ArmDataMemoryBarrier() 

#ifndef MIN
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

/** NULL Interrupt definitions */
#define SMD_NULL_IN_INTERRUPT  ( 0 )
#define SMD_NULL_OUT_INTERRUPT (SMD_NULL_IN_INTERRUPT)

#define UNREFERENCED_PARAMETER(x) do{} while(0);

#define strcmp AsciiStrCmp
#define strlen AsciiStrLen
/*=========================================================================== 
                               String functions
===========================================================================*/
int strncmp(const char *str1, const char *str2, size_t num);

/*===========================================================================
                                TYPE DEFINES
===========================================================================*/
/** OS abstraction over proc/intr types */
typedef uint32 smd_os_proc_type;
typedef uint32* smd_os_intr_type;


/*===========================================================================
                                ERROR HANDLING
===========================================================================*/

#define ERR_FATAL(fmt, a, b, c) \
  do { \
    DebugPrint(EFI_D_ERROR, fmt, a, b, c); \
    DebugAssert(__FILE__, __LINE__, fmt); \
    for(;;) {} \
  } while(0)

#define ERR(fmt, a, b, c) \
  do { \
    DebugPrint(EFI_D_ERROR, fmt, a, b, c); \
  } while(0)

/** SMD Lite DAL specific OS context structure */
typedef struct smd_os_context_struct
{
  /** Critical section to protect SMD Lite open port list */
  void* open_list_cs;   /* Not used in UEFI */
} smd_os_context_type;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* SMD_OS_H */
