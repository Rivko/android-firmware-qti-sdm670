//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                     Boot Aarch64 Utility Assembly File
//
// GENERAL DESCRIPTION
//	This file contains the assembly instructions for miscelleaneous functions 
//   in Boot
//
// Copyright 2013-2014 by Qualcomm Technologies, Incorporated.
//                           All Rights Reserved.
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                           EDIT HISTORY FOR FILE
//
// This section contains comments describing changes made to the module.
// Notice that changes are listed in reverse chronological order.
//
//
// when       who     what, where, why
// --------   ---     --------------------------------------------------------
// 01/16/14   plc     Remove qmemset for now to enable virtio boot chain
// 11/01/13   plc     Initial revision for 64-bit MSM8994
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

//============================================================================
//
//                             MODULE EXPORTS
//
//============================================================================

    // Export the symbols defined in this module
    //EXPORT  qmemset


//============================================================================
//
//                             MODULE DATA AREA
//
//============================================================================

  .section BOOT_UTIL_ASM, "ax", %progbits
  .p2align 8    
    //CODE
        
//======================================================================
//
// void __aeabi_memset(void *dst, uint32 size, uint8 c)
// void __aeabi_memclr(uint32 *dst, uint32 size)
// void __aeabi_memclr4(uint32 *dst, uint32 size)
// void __aeabi_memclr8(uint64 *dst, uint32 size)
// void __rt_memclr(uint32 *dst, uint32 size)
// void __rt_memclr_w(uint32 *dst, uint32 size)
//
// Fast replacement for C library memset(), optimized using the stm
// instruction to generate INCR8 AXI transactions, and replacing all
// 8-bit memory accesses with 32-bit accesses.
// 
// Parameters:
//   r0 contains the destination address
//   r1 is the size in bytes to be set or cleared
//   r2 is the byte pattern to fill the destination memory
//
//======================================================================
//
// void qmemset(uint32 *dst, uint32 val, uint32 len)
// 
// Parameters:
//   r0 contains the destination address
//   r1 is the byte pattern to fill the destination memory
//   r2 is the size in bytes to be set
//
//======================================================================
//qmemset:
 //  ret

   .end

