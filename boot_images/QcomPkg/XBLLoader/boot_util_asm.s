;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                     Boot Utility Assembly File
;
; GENERAL DESCRIPTION
;	This file contains the assembly instructions for miscelleaneous functions 
;   in Boot
;
; Copyright 2007 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 08/18/10   aus     Code Clean-up
; 12/03/07   MJS     Ported from Raptor2 PBL.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the symbols defined in this module
    EXPORT  qmemset


;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    PRESERVE8
    AREA    BOOT_UTIL_ASM, CODE, READONLY
    CODE32
        
;======================================================================
;
; void __aeabi_memset(void *dst, uint32 size, uint8 c)
; void __aeabi_memclr(uint32 *dst, uint32 size)
; void __aeabi_memclr4(uint32 *dst, uint32 size)
; void __aeabi_memclr8(uint64 *dst, uint32 size)
; void __rt_memclr(uint32 *dst, uint32 size)
; void __rt_memclr_w(uint32 *dst, uint32 size)
;
; Fast replacement for C library memset(), optimized using the stm
; instruction to generate INCR8 AXI transactions, and replacing all
; 8-bit memory accesses with 32-bit accesses.
; 
; Parameters:
;   r0 contains the destination address
;   r1 is the size in bytes to be set or cleared
;   r2 is the byte pattern to fill the destination memory
;
;======================================================================
;
; void qmemset(uint32 *dst, uint32 val, uint32 len)
; 
; Parameters:
;   r0 contains the destination address
;   r1 is the byte pattern to fill the destination memory
;   r2 is the size in bytes to be set
;
;======================================================================
qmemset
   ; Re-arrange the parameters AND
   ; Fill the 32-bit value with the byte pattern that we want to set
   and      r3, r1, #0xFF
   mov      r1, r2
   orr      r2, r3, r3, lsl #0x08
   orr      r2, r2, r2, lsl #0x10
   b        _memset_entry
__aeabi_memset
   ; Fill the 32-bit value with the byte pattern that we want to set
   and      r3, r2, #0xFF
   orr      r2, r3, r3, lsl #0x08
   orr      r2, r2, r2, lsl #0x10
   b        _memset_entry
__rt_memclr
__rt_memclr_w
__aeabi_memclr
__aeabi_memclr4
__aeabi_memclr8
   ; If the compiler tells us we are doing a memclr then initialize the
   ; byte pattern to 0
   mov      r2, #0
_memset
_memset_w
_memset_entry
   ; First test if the destination is unaligned and do an initial unaligned
   ; access if so
   ands     r3, r0, #3
   bne      _memset_first_unaligned_bytes
   ; Else test if the total remaining bytes to set is less than a word,
   ; and if so just set the last bytes
   cmp      r1, #4
   bcc      _memset_last_uneven_bytes
_memset_main
   ; Store the work registers on the stack, store 8 registers so we can
   ; do this with a single burst transfer
   stmdb    r13!,{r4-r11}
   ; Initialize all the work registers to the byte pattern
   mov      r3, r2
   mov      r4, r2
   mov      r5, r2
   mov      r6, r2
   mov      r7, r2
   mov      r8, r2
   mov      r9, r2
   ; We will transfer 8 words at a time, so subtract 32 from the total
   ; and set the flags
   subs     r1, r1, #0x20
_memset_loop
   ; If the total is still non-negative then write 8 words to memory
   stmcsia  r0!, {r2-r9}
   ; Subtract 32 from the total again, do this twice in the main loop 
   ; to avoid delays due to pipeline flushing in the tight loop
   subcss   r1, r1, #0x20
   stmcsia  r0!, {r2-r9}
   subcss   r1, r1, #0x20
   ; If the total is still non-negative then transfer more 
   bcs      _memset_loop
   ; Now shift out the total 28 places, even though the total went negative
   ; the lower 5 bits represent the number of bytes still to transfer, and
   ; by shifting by 28 bit 4 goes into the carry bit and bit 3 becomes the 
   ; sign bit
   movs     r1, r1, lsl #0x1C
   stmcsia  r0!, {r2-r5}
   stmmiia  r0!, {r2-r3}
   ; Now shift the total out 2 more bits, bit 2 of the total is now the carry
   ; bit and bit 1 is now the sign bit
   movs     r1, r1, lsl #2
   stmcsia  r0!, {r2}
   ; We can restore the work registers now
   ldmia    r13!,{r4-r11}
   ; If bits 1 and 0 are zero then we are done
   bxeq     lr
   ; Otherwise shift the total back to the right
   mov      r1, r1, lsr #0x1E
_memset_last_uneven_bytes
   ; Take the number of bytes left to copy in r1, multiply by 8, and then
   ; shift our mask by that amount
   mov      r1, r1, LSL #3  ; Multiply by 8 bits per byte
   ldr      r12, =0xFFFFFFFF
   mov      r12, r12, LSL r1
   ; Load the first partial word from memory, mask out the part that will be
   ; set, mask out the part of the memset value that we will use set the
   ; memory, OR the values together and then store back to memory
   ldr      r3, [r0]
   and      r3, r3, r12
   bic      r12, r2, r12
   orr      r3, r3, r12
   str      r3, [r0]
   bx       lr
_memset_first_unaligned_bytes
   ; r3 has the unaligned offset into the first word, subtract from
   ; r0 to get an aligned pointer, subtract (4 minus this value) from 
   ; the total size to transfer; set the flags in case the total size 
   ; just went went negative
   sub      r0, r0, r3
   add      r1, r1, r3
   subs     r1, r1, #4
   ; load the full 32-bit mask for memclr of partial word
   ldr      r12, =0xFFFFFFFF
   ; r3 has unaligned offset, multiply this by 8 and then shift the mask
   ; by this amount
   mov      r3, r3, LSL #3  ; Multiply by 8 bits per byte
   mov      r12, r12, LSL r3
   ; If the total size went negative, the negative of r1 now has the number
   ; of extra bytes that would be copied if we copied all the bytes to the end
   ; of the first word, so negate r1, multiply by 8 and then shift out the
   ; mask by this amount and then shift it back, so that the upper bits of
   ; the mask are now zeros
   rsbmi    r3, r1, #0  ; r3 = -r1
   movmi    r3, r3, LSL #3  ; Multiply by 8 bits per byte
   movmi    r12, r12, LSL r3
   movmi    r12, r12, LSR r3
   ; Load the first partial word from memory, mask out the part that will be
   ; set, mask out the part of the memset value that we will use set the
   ; memory, OR the values together and then store back to memory
   ldr      r3, [r0]
   bic      r3, r3, r12
   and      r12, r2, r12
   orr      r3, r3, r12
   str      r3, [r0], #4
   ; If the total size is still greater than 0 then do aligned memsets
   bgt      _memset_main
   bx       lr

   END

