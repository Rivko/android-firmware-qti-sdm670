;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
; Copyright (c) 2017 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
;======================================================================
; 64 bit SMC call for responses from TEE to QSEE
;======================================================================

  .global  tee_ipc_return_from_inbound_asm
  .global  tee_ipc_outbound_asm

  .section TEE64_SMC, "ax", %progbits
  .p2align 5

;======================================================
;tee_ipc_return_from_inbound_asm(smc)
;tee_ipc_outbound_asm(smc)
;======================================================
.type tee_ipc_return_from_inbound_asm, %function
.type tee_ipc_outbound_asm, %function
tee_ipc_return_from_inbound_asm:
tee_ipc_outbound_asm:
  ; Store smc structure address on stack
  str   x0, [sp, #-16]!
  
  ; Load registers from smc structure
  ldp   x4, x5, [x0, #32]
  ldp   x2, x3, [x0, #16]
  ldp   x0, x1, [x0, #0]

  smc   #0

  ; Pop smc structure address
  ldr   x9, [sp], #16
  
  ; Store register values in smc structure
  stp   x4, x5, [x9, #32]
  stp   x2, x3, [x9, #16]
  stp   x0, x1, [x9, #0]
  mov   x0, x9
  ret

