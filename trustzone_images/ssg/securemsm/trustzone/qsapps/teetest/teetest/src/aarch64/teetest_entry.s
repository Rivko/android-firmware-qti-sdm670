;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
; Copyright (c) 2017 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====

;======================================================================
; TEE Test application entry point
;
; MMU is off
;======================================================================

  .global  __main
  .global  _main
  .global  tee_test_l1_table
  .global  tee_test64_c_entry
  .global  get_el1_stack

  .global  tee_int_enable
  .global  tee_int_disable
  .global  tee_int_restore

  .global  interrupt_exit

  ; DAIF bits
  .set DAIF_D_BIT, 0x200   ; Debug exception mask bit
  .set DAIF_A_BIT, 0x100   ; SError mask bit
  .set DAIF_I_BIT, 0x080   ; IRQ mask bit
  .set DAIF_F_BIT, 0x040   ; FIQ mask bit

  .set DAIFSET_D_BIT, 0x08    ; Debug exception mask bit
  .set DAIFSET_A_BIT, 0x04    ; SError mask bit
  .set DAIFSET_I_BIT, 0x02    ; IRQ mask bit
  .set DAIFSET_F_BIT, 0x01    ; FIQ mask bit

  .set SPSR_A_BIT, 0x100
  .set SPSR_I_BIT, 0x080
  .set SPSR_F_BIT, 0x040

  ; EL1 STACK SIZE
  .set TEE64_EL1_STACK_SIZE, 0x8000

  .extern Image$$TEETEST_EL1_STACK$$ZI$$Base

  .section TZTEE64_TESTAPPENTRYCODE, "ax", %progbits
  .p2align 8

; -----------------------------------------------------------------------
; Main entry function
; -----------------------------------------------------------------------

.type ENTRY,  %function
__main:
_main:

  ; ------------------------------------------------------------------
  ; Program EL1 vector table.  Should happen as early as corruption
  ; of vbar is possible in order to catch any oopsies.
  ; TODO: Have to take care of early exceptions where stack is not
  ;       initialized yet
  ; ------------------------------------------------------------------
  ldr     x17, =tee64_el1_vector_table
  msr     VBAR_EL1, x17

  ; Force flush/refetch of prefetch pipeline
  dsb     sy
  isb

  ; -----------------------------------------------------------------------
  ; Set up a stack pointer for El1h.
  ; -----------------------------------------------------------------------
  ldr   x17, =Image$$TEETEST_EL1_STACK$$ZI$$Base
  mov   sp, x17

  ; -----------------------------------------------------------------------
  ; Jump to 'C' entry point veneer
  ; -----------------------------------------------------------------------
  bl     tee_test_c_entry

;======================================================================

; -----------------------------------------------------------------------
; Abort vectors for the TEE
; -----------------------------------------------------------------------
  .p2align 4
; infinite loop for unhandled exceptions
.type tee_test_unhandled_exception, %function
tee_test_unhandled_exception:
  b       tee_test_unhandled_exception


;======================================================================
; EL1 using SP from EL0 vectors
;======================================================================
    .p2align 11       ; Vector table to be aligned to 0x800 boundary.
tee64_el1_vector_table:
tee64_el1_sp_el0_synch:
    b       tee64_el1_sp_el0_synch

    .p2align 7        ; Each entry to be 0x80 spaced hence 2^7 aligned.
tee64_el1_sp_el0_irq:
    b       tee64_el1_sp_el0_irq

    .p2align 7
tee64_el1_sp_el0_fiq:
    b       tee64_el1_sp_el0_fiq

    .p2align 7
tee64_el1_sp_el0_error:
    b       tee64_el1_sp_el0_error

;======================================================================
; EL1 using SP from EL1 vectors
;======================================================================
    .p2align 7
tee64_el1_sp_el1_synch:
    b       tee64_el1_sp_el1_synch

    .p2align 7
tee64_el1_sp_el1_irq:
    b       tee64_test_irq

    .p2align 7
tee64_el1_sp_el1_fiq:
    b       tee64_test_fiq

    .p2align 7
tee64_el1_sp_el1_error:
    b       tee64_el1_sp_el1_error

;======================================================================
; EL0 AArch64 vectors
;======================================================================
    .p2align 7
tee64_el0_64_synch:
    b       tee64_el0_64_synch

    .p2align 7
tee64_el0_64_irq:
    b       tee64_el0_64_irq

    .p2align 7
tee64_el0_64_fiq:
    b       tee64_el0_64_fiq

    .p2align 7
tee64_el0_64_error:
    b       tee64_el0_64_error

;======================================================================
; EL0 AArch32 vectors
;======================================================================
    .p2align 7
tee64_el0_32_synch:
    b       tee64_el0_32_synch

    .p2align 7
tee64_el0_32_irq:
    b       tee64_el0_32_irq

    .p2align 7
tee64_el0_32_fiq:
    b       tee64_el0_32_fiq

    .p2align 7
tee64_el0_32_error:
    b       tee64_el0_32_error

; Handler for IRQ and FIQ at EL1
    .p2align 7
.type tee64_test_irq, %function
tee64_test_irq:
  stp     x0, x1, [sp, #-16]!

  mrs     x1, spsr_el1
  orr     x0, x1, #(SPSR_I_BIT|SPSR_A_BIT|SPSR_F_BIT)       ; Set A/I/F bits.
  msr     spsr_el1, x0

  ldr     x0, =interrupt_exit
  mov     x1, #1
  str     w1, [x0]

  ldp     x0, x1, [sp] , #16
  eret

.type tee64_test_fiq, %function
tee64_test_fiq:
  stp     x0, x1, [sp, #-16]!

  mrs     x1, spsr_el1
#ifdef TEETEST_GIC_V3
; This is a work around for recurring FIQ on MSM8998
; On GICv3, FIQ is non-secure interrupts but here in teetest
; there is an FIQ (number 1023 = no interrupt) that triggers
; each time interrupts are enabled.
;
; Here, we are just masking off FIQs, leave IRQs enabled so that the
; interrupts triggered by the CGuestVM can be received as IRQs

  orr     x0, x1, #(SPSR_F_BIT)                         ; Mask FIQ only.
  msr     spsr_el1, x0
#else
; On GICv2 (QGICDR) FIQ = secure interrupt, so these are the interrupts
; that CGuestVM is triggering
  orr     x0, x1, #(SPSR_I_BIT|SPSR_A_BIT|SPSR_F_BIT)   ; Mask Abort, IRQ and FIQ.
  msr     spsr_el1, x0

  ldr     x0, =interrupt_exit
  mov     x1, #2
  str     w1, [x0]
#endif
  ldp     x0, x1, [sp], #16
  eret


;======================================================================

; -----------------------------------------------------------------------
; Utility functions
; -----------------------------------------------------------------------
.type tee_int_enable, %function
tee_int_enable:
  ; Adapted from securemsm/trustzone/armlib/v8/src/util_asm.s
  mov     x1, #(DAIF_I_BIT|DAIF_F_BIT|DAIF_A_BIT) ; Only care about A/I/F bits.
  mrs     x0, DAIF                              ; Read the status register.
  bic     x0, x0, x1                            ; Clear A/I/F bits.
  msr     DAIF, x0                              ; Apply the new int mask.
  isb
  ret

.type tee_int_disable, %function
tee_int_disable:
  mrs     x0, DAIF
  and     x0, x0, #DAIF_I_BIT|DAIF_F_BIT|DAIF_A_BIT
  msr     DAIFSet, #DAIFSET_I_BIT|DAIFSET_F_BIT|DAIFSET_A_BIT
  isb
  ; x0 contains the A/I/F bits before masking
  ret

.type tee_int_restore, %function
tee_int_restore:
  ; Copied from securemsm/trustzone/armlib/v8/src/util_asm.s
  mov     x2, #(DAIF_I_BIT|DAIF_F_BIT|DAIF_A_BIT)
  and     x0, x0, x2
  mrs     x1, DAIF
  bic     x1, x1, x2
  orr     x1, x1, x0                  ; Set requested A/I/F bits
  msr     DAIF, x1
  isb
  ret

;======================================================================

  .section RW_DATA, "aw", %progbits

  .p2align 3
interrupt_exit:
  .word   0x0                                 ; return to the TEE EL1

  .end
