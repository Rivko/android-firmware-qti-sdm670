;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 T R U S T Z O N E      M E M Z I
;
;
; GENERAL DESCRIPTION
;   This file contains the TZ memory zeroing code
;
; Copyright (c) 2012 by Qualcomm Technologies, Incorporated.All Rights Reserved.
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
; 09/10/12   sg      Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    EXPORT tzbsp_memzi

    PRESERVE8
    CODE32

;============================================================================
;
;                             MODULE CODE
;
;============================================================================

;======================================================================
;
; void tzbsp_memzi(void * dst, uint32 size)
;
; Initialize memory to zero
; Parameters:
;   r0 contains the destination address
;   r1 is the size to be set to zero
;======================================================================
    AREA    TZBSP_MEMZI, CODE, READONLY, ALIGN=5
qsee_memzi FUNCTION
   ;Initialize
   vmov.i64 Q0, #0x0
   vmov.i64 Q1, Q0
   mov      r2, #0x0

   cmp      r1,   #0x80           ; check bigger than 128 bytes or not.
   blt      check_zi_32byte_left_over
   
   vmov.i64 Q2, Q0
   vmov.i64 Q3, Q0
   vmov.i64 Q4, Q0
   vmov.i64 Q5, Q0
   vmov.i64 Q6, Q0
   vmov.i64 Q7, Q0
   b        loop_for_128byte_set

check_zi_32byte_left_over
   cmp      r1,   #0x20           ; check bigger than 32 bytes or not.
   bge      loop_for_32byte_set
check_zi_byte_left_over
   cmp      r1,   #0              ; set anything <128 bytes
   bgt      loop_for_byte_set
tzbsp_memzi_end
   bx       lr
loop_for_128byte_set
   vstm     r0!, {Q0-Q7}          ; memzi store 128 bytes each time
   sub      r1,   r1, #0x80       ; decrement size by 128
   cmp      r1,   #0x80        
   bge      loop_for_128byte_set
   blt      check_zi_32byte_left_over   ; check whether there are left over bytes.
loop_for_32byte_set
   vstm     r0!, {Q0-Q1}          ; memzi store 32 bytes each time
   sub      r1,   r1, #0x20       ; decrement size by 32
   cmp      r1,   #0x20
   bge      loop_for_32byte_set
   blt      check_zi_byte_left_over
loop_for_byte_set
   strb     r2,   [r0], #1        ; store 1 bytes each time
   sub      r1,   r1, #0x1
   cmp      r1,   #0x0
   bgt      loop_for_byte_set
   ble      tzbsp_memzi_end         ; end the byte copy.
   ENDFUNC
   END

