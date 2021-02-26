// ****************************************************************************
// QUALCOMM Proprietary Design Data
// Copyright (c) 2014, Qualcomm Technologies Incorporated. All rights reserved.
// ****************************************************************************
// ================================================================================
//                                 EDIT HISTORY
//
// $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/x64/ddrss_mem_rdwr_llvm.s#1 $
// $DateTime: 2017/09/18 09:26:13 $
// $Author: pwbldsvc $
// ================================================================================
// when       who          what, where, why
// --------   ---          --------------------------------------------------------
// 05/11/14   arindamm     First edit history header. Add new entries at top.
// ================================================================================
                        .section DDRSS_MEM_RDWR, "ax", %progbits

//====================================================================================================
// void __blocksCopy(uint32 *source, uint32 *destination, uint32 num_words)//
// Register usage:
// Passed parameters:
//   X0: uint32 *source
//   X1: uint32 *destination
//   X2: uint32 num_words. Also = number of uint32s. Should be an even number.
// Function-internal register usage:
//   X3: Number of uint64s to copy.
//====================================================================================================
                        .global  __blocksCopy
                        .align 0x4
.type __blocksScopy, %function
__blocksCopy:
                sub     sp, sp, #0x70

// Save passed and working registers
                stp     x0, x1,   [sp, #0x00]
                stp     x2, x3,   [sp, #0x10]
                stp     x4, x5,   [sp, #0x20]
                stp     x6, x7,   [sp, #0x30]
                stp     x8, x9,   [sp, #0x40]
                stp     x10, x11, [sp, #0x50]
                str     x30,      [sp, #0x60]

// X3 = Number of uint64s to be copied = half the number of uint32s as supplied by caller function.
                LSR     x3, x2, #1

load_store_next_64_bytes:
// Load 64 bytes into registers {x4-x11}, bump source pointer X0 by 64 bytes.
                ldp     x4, x5,   [x0], #0x10
                ldp     x6, x7,   [x0], #0x10
                ldp     x8, x9,   [x0], #0x10
                ldp     x10, x11, [x0], #0x10

// Store 64 bytes from registers {x4-x11}, bump destination pointer X0 by 64 bytes.
                stp     x4, x5,   [x1], #0x10
                stp     x6, x7,   [x1], #0x10
                stp     x8, x9,   [x1], #0x10
                stp     x10, x11, [x1], #0x10

// One iteration copies 8 uint64s, decrement counter by 8 and repeat loop.
                subs    x3, x3, #8
                b.ne    load_store_next_64_bytes

// Restore passed and working registers
                ldp     x0, x1,   [sp, #0x00]
                ldp     x2, x3,   [sp, #0x10]
                ldp     x4, x5,   [sp, #0x20]
                ldp     x6, x7,   [sp, #0x30]
                ldp     x8, x9,   [sp, #0x40]
                ldp     x10, x11, [sp, #0x50]
                ldr     x30,      [sp, #0x60]

                add     sp, sp, #0x70
                ret
__endBlocksCopy:

                       .end

