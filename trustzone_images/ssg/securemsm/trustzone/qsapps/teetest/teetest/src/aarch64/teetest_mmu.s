;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
; Copyright (c) 2017 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
.set SCTLR_EL1_M_BIT,         0x00000001
.set SCTLR_EL1_A_BIT,         0x00000002
.set SCTLR_EL1_C_BIT,         0x00000004
.set SCTLR_EL1_C_SHFT,        0x2
.set SCTLR_EL1_CP15BEN_BIT,   0x00000020
.set SCTLR_EL1_THEE_BIT,      0x00000040
.set SCTLR_EL1_ITD_BIT,       0x00000080
.set SCTLR_EL1_SED_BIT,       0x00000100
.set SCTLR_EL1_I_BIT,         0x00001000
.set SCTLR_EL1_I_SHFT,        0xC
.set SCTLR_EL1_V_BIT,         0x00002000
.set SCTLR_EL1_NTWI_BIT,      0x00010000
.set SCTLR_EL1_NTWE_BIT,      0x00040000
.set SCTLR_EL1_WXN_BIT,       0x00080000
.set SCTLR_EL1_UWXN_BIT,      0x00100000
.set SCTLR_EL1_EE_BIT,        0x02000000
.set SCTLR_EL1_TRE_BIT,       0x10000000
.set SCTLR_EL1_AFE_BIT,       0x20000000
.set SCTLR_EL1_TE_BIT,        0x40000000

;============================================================================
; Set Cache Level
;============================================================================
.type mmu_set_cache_level,%function
mmu_set_cache_level:
  mov     x0, #0x2
  msr     CSSELR_EL1, x0
  ret

;============================================================================
; Enable branch prediction
;============================================================================
.type mmu_enable_branch_p,%function
mmu_enable_branch_p: 
  mrs     x0, SCTLR_EL1
  orr     x0, x0, #0x00000800 ;this bit is undocumented?
  msr     SCTLR_EL1, x0
  ret

;============================================================================
; mmu_invalidate_tlb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidates the entire Translation Look-aside Buffer (TLB) as a unified
;   operation (Data and Instruction). Invalidates all unlocked entries in the
;   TLB. Causes the prefetch buffer to be flushed. All following instructions
;   are fetched after the TLB invalidation.
;   We should do this before we enable to MMU.
;============================================================================
.type mmu_invalidate_tlb,%function
mmu_invalidate_tlb:
  tlbi    alle1
  dsb     sy
  isb
  ret

;============================================================================
; mmu_enable_i_and_d_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the Cache
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
.type mmu_enable_i_and_d_cache,%function
mmu_enable_i_and_d_cache:
  mrs     x0, SCTLR_EL1           ;  Read System Control register
  
  ;  C bit - Enable Data Caches
  ;  I bit - Enable Instruction Caches
  mov     x1, #(SCTLR_EL1_I_BIT | SCTLR_EL1_C_BIT)
  orr     x0, x0, x1     
  msr     SCTLR_EL1, x0           ;  Write System Control register
  ret

;============================================================================
; mmu_enable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the mmu
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
.type mmu_enable,%function
mmu_enable:
  dsb     sy
  isb
  mov     x0, #(SCTLR_EL1_M_BIT | SCTLR_EL1_C_BIT | SCTLR_EL1_I_BIT)
  msr     SCTLR_EL1, x0
  isb
  ret
  
;============================================================================
; static void set_el1_mmu_sys_regs(uint64 ttbr_addr,
;                                  uint64 mair, uint64 tcr)
;============================================================================
.type set_el1_mmu_sys_regs,%function
set_el1_mmu_sys_regs:
    movz    x3, #0xFFFF
    movk    x3, #0xFFFF, LSL #16     ; TTBR0 must be in 4 GB
    movk    x3, #0xFFFF, LSL #32
    and     x0, x0, x3
    msr     TTBR0_EL1, x0

    msr     MAIR_EL1, x1

    msr     TCR_EL1, x2

    ret

;============================================================================
; static void teetest64_enable_el1_mmu(uint64 ttbr_addr,
;                                       uint64 mair, uint64 tcr)
;============================================================================
.global teetest64_enable_el1_mmu
.type teetest64_enable_el1_mmu,%function
teetest64_enable_el1_mmu:
    mov     x17, x30
    bl      set_el1_mmu_sys_regs    ; Configure MAIR, TTBR0, TCR

    bl      clean_and_inval_all_dcache
    ic      iallu                   ; Invalidate entire instruction cache

    dsb     sy
    isb

    bl      mmu_enable

    mov     x30, x17
    ret

.end
