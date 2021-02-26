;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
; Copyright (c) 2015 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
; Enable branch prediction
;============================================================================
.type mmu_set_cache_level,%function
mmu_set_cache_level:
  mov     r0, #0x2
  MCR     p15, 2, r0, c0, c0, 0

  bx      lr


;============================================================================
; Enable branch prediction
;============================================================================
.type mmu_enable_branch_p,%function
mmu_enable_branch_p:
  MRC     p15, 0, r0, c1, c0, 0  ; Read SCTLR into R0
  orr     r0, r0, #0x00000800
  MCR     p15, 0, r0, c1, c0, 0  ; Write R0 to SCTLR

  bx      lr

;============================================================================
; Enable TeX Remap
;   tex remap scheme described in tzbsp_mmu_config.h
;============================================================================
.type mmu_enable_tex_remap,%function
mmu_enable_tex_remap:
  MCR     p15, 0, r0, c10, c2, 1 ; Write R0 to NMRR
  MCR     p15, 0, r1, c10, c2, 0 ; Write R1 to PRRR

  ; Enable TEX remap
  MRC     p15, 0, r0, c1, c0, 0  ; Read SCTLR into R0
  orr     r0, r0, #0x10000000
  MCR     p15, 0, r0, c1, c0, 0  ; Write R0 to SCTLR

  bx      lr

;============================================================================
; mmu_set_ttbrs
;
; ARGS
;   r0 low 32 bits of base
;   r1 high 32 bits of base
;
; DESCRIPTION
;   Sets a value that defines the Translation Table Base boundary that will
;   be used by TTB Registers, and loads Translation Table Base Register 0
;   with the base address of page table.
;   Default value of N after power up is 0 for backward compatible. Any
;   change to the Translation Table Base Register 0 needs to be paired with
;   write to Translation Table Base Control Register.
;============================================================================
.type mmu_set_ttbrs,%function
mmu_set_ttbrs:
  ;TTBCR - TTB Control Register
  ; 2:0 - N - Size of TTBR0 translation table
  ;      0b000 - 16KB (default)
  ;      0b001 -  8KB
  ;      0b010 -  4KB
  ;      0b011 -  2KB
  ;      0b100 -  1KB
  ;      0b101 - 512B
  ;      0b110 - 256B
  ;      0b111 - 128B
  mov     r2, #0x0020
  mcr     p15, 0, r2, c2, c0, 2

  ;TTBR0 - Translation Table Base Register 0
  ; 6 IRGN[0] Inner Region Cache Policy
  ;   (see IRGN[1] for definition of full field)
  ; 5 NOS Not Outer Shareable
  ;   1 = inner shareable
  ;   0 = outer shareable
  ; 4:3 ORGN[1:0] Outer Region Cache
  ;   Policy
  ;   0b00 = non-cacheable
  ;   0b01 = copy-back, write-allocate
  ;   0b10 = write-through
  ;   0b11 = copy-back, write-no-allocate
  ; 1 SH Shareable
  ;   0 = Private memory region
  ;   1 = Shareable memory region
  ; 0 IRGN[1] Inner Region Cache Policy
  ;   0b00 = non-cacheable
  ;   0b01 = copy-back, write-allocate
  ;   0b10 = write-through
  ;   0b11 = copy-back, write-no-allocate
  mcr     p15, 0, r0, c2, c0, 0
  bx      lr

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
  dsb
  mcr     p15, 0, r0, c8, c3, 0   ; TLBIALLIS operation, ignoring the value in r0
  mcr     p15, 0, r0, c7, c1, 6   ; BPIALLIS operation, ignoring the value in r0
  dsb
  isb
  bx      lr

;============================================================================
; mmu_set_dom_access
;
; ARGS
;   permissions
;     Contains 16 2 bit Domain fields
;
;      00 = No Access - Any access to this domain causes a domain fault
;      01 = Client - Accesses checked against access permissions in TLB entry
;      10 = Reserved - Any access to this domain causes a domain fault
;      11 = Master - No checking, no permission fault can be generated
;
;  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
;  |D15|D14|D13|D12|D11|D10|D09|D08|D07|D06|D05|D04|D03|D02|D01|D00|
;  +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
;   struct
;   {
;      D15_Domain int:2
;      D14_Domain int:2
;      D13_Domain int:2
;      D12_Domain int:2
;      D11_Domain int:2
;      D10_Domain int:2
;      D9_Domain int:2
;      D8_Domain int:2
;      D7_Domain int:2
;      D6_Domain int:2
;      D5_Domain int:2
;      D4_Domain int:2
;      D3_Domain int:2
;      D2_Domain int:2
;      D1_Domain int:2
;      D0_Domain int:2
;   }
;
; DESCRIPTION
;   Sets the Access permissions for the 16 domains.
;
;============================================================================
.type mmu_set_dom_access,%function
mmu_set_dom_access:
  mcr     p15, 0, r0, c3, c0, 0
  bx      lr

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
  mrc     p15, 0, r0, c1, c0, 0   ;  Read System Control register
  orr     r0, r0, # (1 << 2)      ;  C bit - Enable Data Caches
  orr     r0, r0, # (1 << 12)     ;  I bit - Enable Instruction Caches
  mcr     p15, 0, r0, c1, c0, 0   ;  Write System Control register
  bx      lr

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
  dsb
  isb
  mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
  orr     r0, r0, # 0x1              ; Set MMU Enable bit
  mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
  isb
  isb
  bx      lr

;============================================================================
; enable_el1_mmu
;
;  DESCRIPTION -
;        Initializes the ARM MMU.
;                1- sets a default value in the control register
;                2- sets the physical address of the page table
;                3- populates the page_table with section entries
;                4- sets the domain access permissions
;                5- flushes out cache and write buffer
;                6- then enables the mmu
;                7- then enable data and instruction cache
;============================================================================
  .extern cache_flush
  .global enable_el1_mmu
.type enable_el1_mmu,%function
enable_el1_mmu:
  push    {r1, r2, r4, lr}
  push    {r0, r3}
  bl      cache_flush
  pop     {r0, r3}

  bl      mmu_set_ttbrs
  pop     {r0, r1}
  bl      mmu_enable_tex_remap    ; Enable TEX Remapping
  bl      mmu_enable_branch_p     ; Enable branch prediction
  bl      mmu_set_cache_level     ; Set the cache level
  bl      mmu_invalidate_tlb      ; Invalidate instruction and data TLB

  mov     r0, #0x5555
  movt    r0, #0x5555
  bl      mmu_set_dom_access      ; set all domains for client access
  bl      mmu_enable_i_and_d_cache
  bl      mmu_enable
  mov     r0, #0

  pop     {r4, pc}

.end
