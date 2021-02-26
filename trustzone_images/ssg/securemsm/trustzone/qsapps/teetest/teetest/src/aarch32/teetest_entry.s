;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
; Copyright (c) 2015 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====

;======================================================================
; TEE Test application entry point
;
; MMU is off
; Assume entry in SVC mode
;======================================================================

  .global  __main
  .global  _main
  .global  tee_test_unhandled_exception
  .global  tee_test_vector_table
  .global  tee_test_l1_table
  .global  tee_test_c_entry
  .global  get_svc_stack
  .global  get_und_stack
  .global  get_abt_stack
  .global  get_fiq_stack
  .global  get_irq_stack

  .global  tee_int_enable
  .global  tee_int_disable
  .global  tee_int_restore

  .global  interrupt_exit

  .section TZTEETESTAPPENTRYCODE, "ax", %progbits
  .p2align 8
  .code 32

#define TEE_TEST_ABT_STACK_SIZE     0x0FFC
#define TEE_TEST_L1_TABLE_SIZE      0x4000

; CPSR CPU modes
.set CPSR_MODE_USR, 0x10
.set CPSR_MODE_FIQ, 0x11
.set CPSR_MODE_IRQ, 0x12
.set CPSR_MODE_SVC, 0x13
.set CPSR_MODE_ABT, 0x17
.set CPSR_MODE_UND, 0x1b
.set CPSR_MODE_SYS, 0x1f

; -----------------------------------------------------------------------
; Main entry function
; -----------------------------------------------------------------------

.type ENTRY,  %function
__main:
_main:

  ; Program the vector table ASAP
  adr     r12, tee_test_vector_table
  mcr     p15, 0, r12, c12, c0, 0
  isb

  ; Save R12
  mov     r12, r0

  ; -----------------------------------------------------------------------
  ; Set up a stack pointer in Supervisor, Undefined, Abort mode.
  ; -----------------------------------------------------------------------
  blx     get_svc_stack
  mov     r13, r0

  cps     #CPSR_MODE_UND
  blx     get_und_stack
  mov     r13, r0

  cps     #CPSR_MODE_ABT
  blx     get_abt_stack
  mov     r13, r0

  ; -----------------------------------------------------------------------
  ; Set up a stack pointer in FIQ and IRQ mode
  ; -----------------------------------------------------------------------
  cps     #CPSR_MODE_FIQ
  blx     get_fiq_stack
  mov     r13, r0

  cps     #CPSR_MODE_IRQ
  blx     get_irq_stack
  mov     r13, r0

  ; -----------------------------------------------------------------------
  ; Switch back to SVC
  ; -----------------------------------------------------------------------
  cps     #CPSR_MODE_SVC

  ; -----------------------------------------------------------------------
  ; Restore R0
  ; -----------------------------------------------------------------------
  mov     r0, r12

  ; -----------------------------------------------------------------------
  ; Jump to 'C' entry point veneer
  ; -----------------------------------------------------------------------
  blx     tee_test_c_entry

;======================================================================

; -----------------------------------------------------------------------
; Abort vectors for the TEE
; -----------------------------------------------------------------------
  .p2align 4
.type tee_test_vector_table, %function
tee_test_vector_table:
  b       _main                        ;Reset
  b       tee_test_unhandled_exception ;Undefined instruction
  b       tee_test_unhandled_exception ;SWI
  b       tee_test_unhandled_exception ;Prefetch
  b       tee_test_unhandled_exception ;Data abort
  b       tee_test_unhandled_exception ;Not used
  b       tee_test_irq                 ;IRQ
  b       tee_test_fiq                 ;FIQ

.type tee_test_unhandled_exception, %function
tee_test_unhandled_exception:
  b       tee_test_unhandled_exception

.type tee_test_irq, %function
tee_test_irq:
  push    {r0-r1}
  mrs     r1, spsr
  orr     r0, r1, #(1<<6)|(1<<7)|(1<<8)       ; Set A/I/F bits.
  msr     spsr_cxsf, r0
  ldr     r0, =interrupt_exit
  mov     r1, #1
  str     r1, [r0]
  pop     {r0-r1}
  subs    pc, r14, #4

.type tee_test_fiq, %function
tee_test_fiq:
  mrs     r9, spsr
#ifdef TEETEST_GIC_V3
; This is a work around for recurring FIQ on MSM8998
; On GICv3, FIQ is non-secure interrupts but here in teetest
; there is an FIQ (number 1023 = no interrupt) that triggers
; each time interrupts are enabled.
;
; Here, we are just masking off FIQs, leave IRQs enabled so that the
; interrupts triggered by the CGuestVM can be received as IRQs

  orr     r8, r9, #(1<<6)               ; Mask FIQ only.
  msr     spsr_cxsf, r8
#else
; On GICv2 (QGICDR) FIQ = secure interrupt, so these are the interrupts
; that CGuestVM is triggering
  orr     r8, r9, #(1<<6)|(1<<7)|(1<<8) ; Mask Abort, IRQ and FIQ.
  msr     spsr_cxsf, r8
  ldr     r8, =interrupt_exit
  mov     r9, #2
  str     r9, [r8]
#endif
  subs    pc, r14, #4


;======================================================================

; -----------------------------------------------------------------------
; Utility functions
; -----------------------------------------------------------------------
.type tee_int_enable, %function
tee_int_enable:
  mov     r1, #(1<<6)|(1<<7)|(1<<8)           ; Only care about A/I/F bits.
  mrs     r0, cpsr                            ; Read the status register.
  bic     r0, r0, r1                          ; Clear A/I/F bits.
  msr     cpsr_cx, r0                         ; Apply the new int mask.
  bx      lr

.type tee_int_disable, %function
tee_int_disable:
  mrs     r1, cpsr                            ; Read the status register.
  and     r0, r1, #(1<<6)|(1<<7)|(1<<8)       ; Only care about A/I/F bits.
  orr     r1, r1, #(1<<6)|(1<<7)|(1<<8)       ; Set A/I/F bits.
  msr     cpsr_cx, r1                         ; Apply the new int mask.
  ; R0 contains the A/I/F bits before masking
  bx      lr

.type tee_int_restore, %function
tee_int_restore:
  and     r0, r0, #(1<<6)|(1<<7)|(1<<8)       ; Only care about A/I/F bits.
  mrs     r1, cpsr                            ; Read the status register.
  bic     r1, r1, #(1<<6)|(1<<7)|(1<<8)       ; Clear A/I/F bits.
  orr     r1, r1, r0                          ; Set requested A/I/F bits.
  msr     cpsr_cx, r1                         ; Write control & extension field
  bx      lr

;======================================================================

  .section RW_DATA, "aw", %progbits

  .p2align 2
interrupt_exit:
  .word   0x0                                 ; return to the TEE EL1

  .end
