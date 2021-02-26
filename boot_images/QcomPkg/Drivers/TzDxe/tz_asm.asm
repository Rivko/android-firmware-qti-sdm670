;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                 TZBSPTEST ASM FUNCTIONS
;
; Copyright (c) 2012, 2014 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
; Qualcomm Technologies Proprietary and Confidential.
;   
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
; 09/04/14   sm      Added tz_smc_armv8
; 12/01/12   shl     renamed
; 06/08/10   tk      Initial revision.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;    GET tzt_asm.inc

    EXPORT tz_get_version
    EXPORT tz_slowcall
    EXPORT tz_fastcall
    EXPORT tz_armv8_smc_call

    MACRO
    SMCCALL
100 smc     0x0
    adds    r0, #0         ; Check return value sign
    bgt     %b100          ; If return value was positive redo smc
    MEND

    CODE32
    PRESERVE8

    AREA TZT_ASM, align=4, CODE, READONLY


; int tz_get_version(uint32* version)
tz_get_version FUNCTION
    push    {r4, lr}
    mov     r4, r0              ; Save pointer to version
    mov     r0, #0
    orr     r0, r0, #(1 << 8)   ; val 2 in bits 8-11 means fastcall
    SMCCALL
    cmp     r0, #0              ; If request successful,
    streq   r1, [r4]            ; then store the version to argument ptr.
    pop     {r4, pc}
    ENDFUNC

; int tz_slowcall(uint32 cmd_addr)
; r0 - Is fixed to argument count 1
; r1 - Unique context id (stack pointer will do)
; r2 - Pointer to command memory area
tz_slowcall FUNCTION
    mov     r2,r0
    mov     r0,#1
    mov     r1,sp
    SMCCALL
    bx      lr
    ENDFUNC

; int tz_fastcall(uint32 id, uint32 token, uint32 r0, uint32 r1, ...)
; r0  - id, TZ SYSCALL id, lowest nibble contains the number of arguments.
; r1  - token, SMP token.
; r2  - First argument to TZ syscall (R0, optional).
; r3  - Second argument to TZ syscall (R1, optional).
; ... - Stacked arguments, TZ arguments from R2-R9 (optional)
tz_fastcall FUNCTION
    push    {lr}
    orr     r0, r0, #(2 << 8)   ; val 2 in bits 8-11 means fastcall
    and     lr, r0, #0xff       ; last nibble is the arg count

    cmp     lr, #2      ; if (args <= 2)
    bgt     %f3
    SMCCALL
    pop     {pc}
3
    cmp     lr, #3      ; if (args == 3)
    bne     %f4
    push    {r4}
    add     lr, sp, #8
    ldmia   lr, {r4}
    SMCCALL
    pop     {r4, pc}
4
    cmp     lr, #4      ; if (args == 4)
    bne     %f5
    push    {r4-r5}
    add     lr, sp, #12
    ldmia   lr, {r4-r5}
    SMCCALL
    pop     {r4-r5, pc}
5
    cmp     lr, #5      ; if (args == 5)
    bne     %f6
    push    {r4-r6}
    add     lr, sp, #16
    ldmia   lr, {r4-r6}
    SMCCALL
    pop     {r4-r6, pc}
6
    cmp     lr, #6      ; if (args == 6)
    bne     %f7
    push    {r4-r7}
    add     lr, sp, #20
    ldmia   lr, {r4-r7}
    SMCCALL
    pop     {r4-r7, pc}
7
    cmp     lr, #7      ; if (args == 7)
    bne     %f8
    push    {r4-r8}
    add     lr, sp, #24
    ldmia   lr, {r4-r8}
    SMCCALL
    pop     {r4-r8, pc}
8
    cmp     lr, #8      ; if (args == 8)
    bne     %f9
    push    {r4-r9}
    add     lr, sp, #28
    ldmia   lr, {r4-r9}
    SMCCALL
    pop     {r4-r9, pc}
9
    cmp     lr, #9      ; if (args == 9)
    bne     %f10
    push    {r4-r10}
    add     lr, sp, #32
    ldmia   lr, {r4-r10}
    SMCCALL
    pop     {r4-r10, pc}
10
    cmp     lr, #10     ; if (args == 10)
    bne     %f11
    push    {r4-r11}
    add     lr, sp, #36
    ldmia   lr, {r4-r11}
    SMCCALL
    pop     {r4-r11, pc}
11
    mvn     r0, #0      ; Too many args
    pop     {pc}
    ENDFUNC

; int tz_armv8_smc_call(uint32 smc_id, uint32 param_id, uint32 parameters[4], uint32 *trusted_os_id)
tz_armv8_smc_call FUNCTION
    push    {r10,r9,lr}
    and     lr, r1, #0xf  ; last bit determines number of args
    cmp     lr, #10
    bgt     %f10

    push    {r4-r6}
    mov     lr, r2
    mov     r10, r2
    mov     r9, r3
    ldmia   lr, {r2-r5}
    SMCCALL
    stmia   r10, {r1-r3}
    str     r6, [r9]
    pop     {r4-r6}
    b       end

10
    mvn     r0, #0      ; Too many args
    b       end

end
    pop     {r10,r9,pc}
    ENDFUNC

    END
