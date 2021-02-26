;============================================================================
;
;             Arm V7 architecture assembly language MMU support
;
; GENERAL DESCRIPTION
;   This file contains the Arm V7 hierarchichal cache and mmu control    functions.
;
; Copyright 2012 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved 
;============================================================================

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  -----   ----------------------------------------------------------
; 03/27/13  dh      Add mmu_get_page_table_base and memory barriers
; 03/13/13  kedara  Added api mmu_set_dacr.
; 03/01/13  jz      Export mmu_flush_cache_and_disable
; 11/15/12  kedara  Fix end address calc for dcache_inval_region.
; 08/24/12  kedara  Added support for L2 cache (for sparrow)
;                   Add barriers to mmu_relocate_page_table_base
;                   Fixed region based cache APIs
; 03/14/12  dh      Add mmu_relocate_page_table_base, remove boot_cache_mmu_init 
;                   and cache_mmu_re_enable
; 02/03/12  dh      Convert mmu_invalidate_tlb to function
; 10/20/11  dh      Remove l1/l2 page table allocation, use pointers to them instead 
; 09/20/11  dh      use bx to return to allow interwork between arm and thumb
; 09/15/11  dh      Add 7-A.security to the architecture check
; 08/26/11  dh      ported to badger, remove ISB/DSB macro to fix compiler warning
; 05/19/11  kpa     Created
;============================================================================


;============================================================================
;                             MODULE DEFINES
;============================================================================

; Options for DMB/DSB/ISB Barriers, define these manually for RVCT < 3.0
   IF {ARCHITECTURE} != "7-A" :LAND: {ARCHITECTURE} != "7-M" :LAND: {ARCHITECTURE} != "7-A.security" 
SY                      EQU    0xF
UN                      EQU    0x7
ST                      EQU    0xE
UNST                    EQU    0x6
   ENDIF

;============================================================================
; MACRO mdmb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs a data memory barrier, either using the ARMv7 instruction or the
;   legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   mdmb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M" :LOR: {ARCHITECTURE} == "7-A.security"   
      dmb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF050 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F50 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c10, 5  ; Legacy Data Memory Barrier
   ENDIF
   MEND
   
;============================================================================
; MACRO mdsb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs a data synchronization barrier, either using the ARMv7 instruction
;   or the legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   mdsb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M" :LOR: {ARCHITECTURE} == "7-A.security"   
      dsb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF040 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F40 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c10, 4  ; Legacy Data Write Barrier
   ENDIF
   MEND

;============================================================================
; MACRO misb
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Performs an instruction synchronization barrier, either using the ARMv7
;   instruction or the legacy coprocessor instruction.
;
; NOTES
;   For reference see ARM DDI 0406A-03 section A3.8.3.
;============================================================================
   MACRO
   misb
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M" :LOR: {ARCHITECTURE} == "7-A.security"   
      isb                        ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF57FF060 :OR: SY  ; ARMv7 A1 Opcode
      ELSE
         DCI 0xF3BF8F60 :OR: SY  ; ARMv7 T1 Opcode
      ENDIF
      ;mcr    p15, 0, r0, c7, c5, 4   ; Legacy Pre-Fetch Flush
   ENDIF
   MEND

;============================================================================
; MACRO mpli
;
; ARGS
;   $Rn - The register containing the address of the instruction to preload
;
; DESCRIPTION
;   Preload instruction signals to the memory system that instruction memory 
;   accesses from a specified address are likely in the near future.
;
; NOTES
;   The ARMv7 opcode is generated manually for RVDS < 3.0.
;============================================================================
   MACRO
   mpli $Rn
   IF {ARCHITECTURE} = "7-A" :LOR: {ARCHITECTURE} = "7-M" :LOR: {ARCHITECTURE} == "7-A.security"   
      pli $Rn                  ; RVDS >= 3.0 supports ARMv7 instructions
   ELSE
      IF {CONFIG} = 32
         DCI 0xF4D0F000 :OR: ((:RCONST:$Rn) :SHL: 16)  ; ARMv7 A1 Opcode
      ELSE
         ASSERT (:RCONST:$Rn) != 15  ; T1 opcode can't use R15
         DCI 0xF990F000 :OR: ((:RCONST:$Rn) :SHL: 16)  ; ARMv7 T1 Opcode
      ENDIF
   ENDIF
   MEND

;============================================================================
;                             MODULE INCLUDES
;============================================================================
#include "boot_page_table_armv7.h"
; TODO: _FIX_ for 8996. BOOT_MMU_ASM_H asm macro defn

;============================================================================
;                             MODULE IMPORTS
;============================================================================
#ifdef BOOT_USES_L2_DCACHE     
   IMPORT  l2_cache_flush_and_disable
   IMPORT  l2_cache_flush
   IMPORT  l2_ranged_cache_invalidate
   IMPORT  l2_ranged_cache_flush
   IMPORT  l2_ranged_cache_clean
#endif   
;============================================================================
;                             MODULE EXPORTS
;============================================================================
   EXPORT  cache_mmu_disable
   EXPORT  mmu_invalidate_data_cache
   EXPORT  mmu_invalidate_instruction_cache
   EXPORT  mmu_enable_instruction_cache
   EXPORT  mmu_flush_cache
   EXPORT  dcache_clean_all
   EXPORT  dcache_clean_region
   EXPORT  dcache_inval_region
   EXPORT  dcache_flush_region
   EXPORT  cache_set_memory_barrier_location
   EXPORT  memory_barrier
   EXPORT  mmu_invalidate_tlb
   EXPORT  mmu_relocate_page_table_base
   EXPORT  mmu_flush_cache_and_disable
   EXPORT  mmu_set_dacr
   EXPORT  mmu_get_page_table_base
   
;============================================================================
;                             MODULE EQUATES
;============================================================================
C1_REG_DEFAULT        EQU    0xC56870


      
;============================================================================
;
;                         MEMORY BARRIER WORD POINTER
;
; The memory barrier word must exist in non-cached region of memory.
;============================================================================

   AREA  CACHE_MMU_ZI, DATA, NOINIT

; This should be initialized to the actual SMEM memory barrier, otherwise NULL
g_memory_barrier_ptr
   % 4
      
;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================
PRESERVE8   
   AREA    ARM_MMU, CODE, READONLY
   CODE32

;============================================================================
; MACRO mmu_set_default_cfg
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Sets a default value into the mmu control register
;   we do not really need to do this, but we do it to
;   know exactly the state of the processor.
;
; TBD - We should most likely only use this macro at initialization.
;
; TBD - handle default value as or of all the below from equates
;
;  Value  bit  Name    Function
; ---------------------------------------------------
;  0     0 - M bit - MMU disabled
;  0     1 - A bit - Alignment fault disabled
;  0     2 - C bit - Data Cache disabled
;  0     3 - W bit - Write Buffer enabled
;  ----------
;  111  4-6        - SHOULD BE ONES
;  0     7 - B bit - Little Endian
;  ----------
;  0     8 - S bit - MMU Protection disabled
;  0     9 - R bit - ROM Protection disabled
;  0    10 - F bit - SHOULD BE ZERO
;  0    11 - Z bit - Program Flow Protection disabled
;  ----------
;  0    12 - I bit - Level one Instruction Cache disabled
;  0    13 - V bit - Exception Vector Location is Normal
;  0    14 - RR bit- Replacement strategy is Normal/Random (not Round Robin)
;  0    15 - L4 bit- Loads to PC set the T bit
;  ----------
;  1    16 - DT bit- Global Data TCM enabled - SHOULD BE ONE for ARM11JF-S
;  0    17 -       - SHOULD BE ZERO
;  1    18 - IT bit- Global Instruction TCM enabled
;  0    19 -       - SHOULD BE ZERO
;  ----------
;  0    20 -       - SHOULD BE ZERO
;  0    21 - FI bit- Low Latency interrupt features disabled
;  1    22 - U bit - Unaligned data access enabled
;  1    23 - XP bit- Hardware page translation mechanism subpage AP bits disabled
;  ----------
;  0    24 - VE bit- Interrupt vectors are fixed (no VIC)
;  0    25 - EE bit- CPSR E bit set to 0 on taking an exception
;  0   26-27       - Reserved
;  ----------
;  0   28-31       - Reserved
;============================================================================
   MACRO
   mmu_set_default_cfg
   ldr    r0, =C1_REG_DEFAULT 
   mcr    p15, 0, r0, c1, c0, 0
   MEND

;============================================================================
; MACRO mmu_disable_i_and_d_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables the Data Cache
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_disable_i_and_d_cache
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions
   mdsb
   misb
   mrc     p15, 0, r0, c1, c0, 0   ;  Read SCTLR, Control register
   bic     r0, r0, #0x4            ;  C bit - Disable Data Cache
   bic     r0, r0, #0x1000         ;  I bit - Disable Instruction Cache
   mcr     p15, 0, r0, c1, c0, 0   ;  Write SCTLR, Control register
   mdsb
   misb
   MEND
 
;============================================================================
; MACRO mmu_set_page_table_base
;
; ARGS
;   base is as follows:
; 
;  bit  Name    Function
; ---------------------------------------------------
;  0 - C bit - Page table walk is Inner Cacheable? 
;  1 - S bit - Page table walk is Shareable?
;  2 - IMP bit - Implementation defined (Parity enabled for tablewalk)
; 3-4 - RGN bits- Outer cachable attributes for page table walking:
;                 00=Normal memory, Outer Noncacheable
;                 01=Normal memory, Outer Write-Back, write allocate
;                 10=Normal memory, Outer Write-Through, no allocate on write
;                 11=Normal memory, Outer Write-Back, no allocate on write
;  5 - Not Outer Shareable bit. (ARMv7)
;
;  6 - Combines with bit 0 to be IRGN
;
; IRGN [6,0] when multiprocessing extensions are implemented
;      0b00 Normal memory, Inner Non-cacheable
;      0b01 Normal memory, Inner Write-Back Write-Allocate Cacheable
;      0b10 Normal memory, Inner Write-Through Cacheable
;      0b11 Normal memory, Inner Write-Back no Write-Allocate Cacheable
;  
;14-31 - Pointer to level one translation table
;
;
; DESCRIPTION
;  Sets the base address of the mmu page table.
;============================================================================
   MACRO
   mmu_set_page_table_base  $base
   ldr     r0, =$base                ; location of our page table description
   mcr     p15, 0, r0, c2, c0, 0
   MEND


;============================================================================
; MACRO mmu_enable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the mmu
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_enable
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions 
   mdsb
   misb
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   orr     r0, r0, # 0x1              ; Set MMU Enable bit
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   mdsb
   misb
   MEND

;============================================================================
; MACRO mmu_disable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Disables the mmu
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_disable
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions 
   mdsb
   misb
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   bic     r0, r0, #0x1               ; Clear MMU bit
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   mdsb
   misb
   MEND
   
;============================================================================
; MACRO mmu_align_fault_enable
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables address alignment fault checking
;   Does a read modify write, so no other bits in the control register are
;   affected
;============================================================================
   MACRO
   mmu_align_fault_enable
   mrc     p15, 0, r0, c1, c0, 0      ; Read SCTLR, Control Register
   orr     r0, r0, # 0x2              ; Set A bit - Enable alignment fault check
   mcr     p15, 0, r0, c1, c0, 0      ; Write SCTLR, Control Register
   MEND

;============================================================================
; MACRO dcache_line_size
;
; ARGS
;   $reg - cache line size in bytes
;   $tmp - CTR[DMINLINE] (Log2 number of words).
;          used for calculation of the size in bytes 
;
; DESCRIPTION
;   cache_line_size - get the cache line size from the CTR register
;  (available on ARMv7+). Note that the value of $reg and $tmp 
;   will be modified.
;============================================================================   
   MACRO
   dcache_line_size $reg, $tmp
   mrc p15, 0, $tmp, c0, c0, 1 ; read CTR register
   ; read CTR bits 16 through 19 for DMINLINE
   mov   $tmp, $tmp, lsr #16     ; 
   ands  $tmp, $tmp, #0xF      ; cache line size encoding, 4 bits.
   moveq $reg, #0             ; for the unlikely event that DMINLINE is 0
   movne $reg, #4             ; size offset. (DMINLINE is log2 words)
   mov   $reg, $reg, lsl $tmp  ; actual cache line size in $reg. 64 bytes 
                              ; for Krait and 32 bytes for Scorpion.
   MEND

;============================================================================
; MACRO mmu_set_dom_access
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
;   Sets the Access permisions for the 16 domains.
;
;============================================================================
   MACRO
   mmu_set_dom_access $permisions
   mov     r0,   $permisions
   mcr     p15, 0, r0, c3, c0, 0
   MEND

;============================================================================
; MACRO mmu_set_ttb_0
;
; ARGS
;   $boundary_size - 
;   $base - 
;
; DESCRIPTION
;   Sets a value that defines the Translation Table Base boundary that will
;   be used by TTB Registers, and loads Translation Table Base Register 0
;   with the base address of page table.
;   Default value of N after power up is 0 for backward compatible. Any
;   change to the Translation Table Base Register 0 needs to be paired with
;   write to Translation Table Base Control Register.
;============================================================================
   MACRO
   mmu_set_ttb_0  $boundary_size, $base
      ldr   r0, =$boundary_size

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
      mcr   p15, 0, r0, c2, c0, 2
      ldr   r0, =$base
  
      ;TTBR0
      mcr   p15, 0, r0, c2, c0, 0
   MEND

;============================================================================
; MACRO mmu_set_ttb_1
;
; ARGS
;   $base
;
; DESCRIPTION
;   Loads the Translation Table Base Register 1 with the base address of page
;   table.
;============================================================================
   MACRO
   mmu_set_ttb_1  $base
      ldr   r0, =$base
      ldr   r1, =0xFFFFC000
      and   r0, r0, r1     ; make sure [13:0] are 0.
      mcr   p15, 0, r0, c2, c0, 1
   MEND

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
mmu_invalidate_tlb
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions 
   mdsb
   misb
   mov     r0 , #0
   mcr     p15, 0, r0, c8, c7, 0
   mdsb
   misb
   bx lr
   
;============================================================================
; mmu_invalidate_instruction_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire intruction cache.  This command will invalidate
;   any instructions cached.  It also flushes the branch target cache.
;   We need to do this before we enable the mmu
;============================================================================
mmu_invalidate_instruction_cache
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions 
   mdsb
   misb
   ldr     r0, =0x0
   mcr     p15, 0, r0, c8, c5, 0
   mdsb
   misb
   bx      lr

;============================================================================
; mmu_invalidate_data_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Invalidate the entire data cache and the unified cache.  This command will 
;   invalidate any data cached.  It also flushes the branch target cache.
;   We need to do this before we enable the mmu
;============================================================================
mmu_invalidate_data_cache
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions 
   mdsb
   misb
   ldr     r0, =0x0
   mcr     p15, 0, r0, c8, c6, 0
   mdsb
   misb
   bx      lr
   
;============================================================================
; mmu_enable_branch_prediction
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables branch prediction
;============================================================================
mmu_enable_branch_prediction
   mrc    p15, 0, r0, c1, c0, 0     ; Read from CP15 Control Register
   orr    r0, r0, # (1 << 11 )      ; Enable branch prediction
   mcr    p15, 0, r0, c1, c0, 0     ; Write back to CP15 Control Register
   mov    r0, #0                    ; Return success
   bx     lr

;============================================================================
; mmu_enable_instruction_cache
;
; ARGS
;   NONE
;
; DESCRIPTION
;   Enables the level one and level two intruction cache by setting the I1 and I2 bit 
;   in the ACTLR register. The mmu remains disabled
;============================================================================
mmu_enable_instruction_cache
   stmfd  sp!, {r0-r12, lr}
   ; Call memory barrier operations to ensure completion of all cache 
   ; maintenance & branch predictor operations appearing in program 
   ; order, before these instructions 
   mdsb
   misb
   mrc    p15, 0, r0, c1, c0, 0     ; Read from CP15 Control Register
   orr    r0, r0, # (1 << 12 )      ; Enable Instruction Cache
   mcr    p15, 0, r0, c1, c0, 0     ; Write back to CP15 Control Register
   mdsb
   misb
   ldmfd  sp!, {r0-r12, pc}

;============================================================================
; cache_mmu_disable
;
; ARGS
;   
;
;  DESCRIPTION -
;        Disables the MMU and cache.
;           1- flushes out cache and write buffer
;              also disable data and instruction cache
;           2- disables MMU
;           3- invalidates TLB
;
;        The page table is unaffected
;============================================================================
cache_mmu_disable
   stmfd  sp!, {r0-r12, lr}
   ;Flushing cache and disabling should be atomic without any memory load
   ;store operations in between.   
   bl     mmu_flush_cache_and_disable
   mov    r0, #0    
   mmu_disable
   bl mmu_invalidate_tlb   
   ldmfd  sp!, {r0-r12, pc}

   
;============================================================================
; memory_barrier
;
; PROTOTYPE
;   void memory_barrier(void);
;
; ARGS
;
; DESCRIPTION
;   Creates a memory barrier
;
; DEPENDENCIES
;============================================================================
memory_barrier
   mdsb
   bx     r14

;=======================================================================
; cache_set_memory_barrier_location
;
; DESCRIPTION
;   Sets the location of the actual memory barrier in SMEM
;
; DEPENDENCIES
;   cache_set_memory_barrier_location() must be called with a pointer to
;   non-buffered or non-cached memory before any call to memory_barrier(
;   for the instruction and data streams to be syncronized by
;   the memory_barrier() operation.
;=======================================================================
cache_set_memory_barrier_location
   ldr    r1, =g_memory_barrier_ptr
   str    r0, [r1]
   bx     lr

;============================================================================
; dcache_clean_all
;
; DESCRIPTION -
;   Flush the whole D-cache.
;============================================================================  
dcache_clean_all
  mdsb                      ; ensure ordering with previous memory accesses
  mrc  p15, 1, r0, c0, c0, 1    ; read clidr
  ands  r3, r0, #0x7000000    ; extract loc from clidr
  mov  r3, r3, lsr #23          ; left align loc bit field
  beq  finished              ; if loc is 0, then no need to clean
  mov  r10, #0                ; start clean at cache level 0
loop1
  add  r2, r10, r10, lsr #1    ; work out 3x current cache level
  mov  r1, r0, lsr r2          ; extract cache type bits from clidr
  and  r1, r1, #7              ; mask of the bits for current cache only
  cmp  r1, #2                ; see what cache we have at this level
  blt  skip                ; skip if no cache, or just i-cache
  mcr  p15, 2, r10, c0, c0, 0    ; select current cache level in cssr
  misb                      ; isb to sych the new cssr&csidr
  mrc  p15, 1, r1, c0, c0, 0    ; read the new csidr
  and  r2, r1, #7              ; extract the length of the cache lines
  add  r2, r2, #4              ; add 4 (line length offset)
  ldr  r4, =0x3ff
  ands  r4, r4, r1, lsr #3    ; find maximum number on the way size
  clz  r5, r4                ; find bit position of way size increment
  ldr  r7, =0x7fff
  ands  r7, r7, r1, lsr #13    ; extract max number of the index size
loop2
  mov  r9, r4                ; create working copy of max way size
loop3
  orr  r11, r10, r9, lsl r5      ; factor way and cache number into r11
  orr  r11, r11, r7, lsl r2      ; factor index number into r11
  mcr  p15, 0, r11, c7, c14, 2    ; clean & invalidate by set/way
  subs  r9, r9, #1          ; decrement the way
  bge  loop3
  subs  r7, r7, #1          ; decrement the index
  bge  loop2
skip
  add  r10, r10, #2          ; increment cache number
  cmp  r3, r10
  bgt  loop1
finished
  mov  r10, #0                ; swith back to cache level 0
  mcr  p15, 2, r10, c0, c0, 0    ; select current cache level in cssr
  mdsb
  misb
  bx lr

;============================================================================
; dcache_flush_region
;
; PROTOTYPE
;   void dcache_flush_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans and invalidates a memory region in the cache. Please note that the
;   data in the cache would be written back to main memory if dirty and the 
;   region invalidated. So any further access to the data would result in a 
;   cache-miss.   
;============================================================================
dcache_flush_region
  stmfd  sp!, {r0-r12, lr}  
  mov r4, r0                      ; copy addr
  mov r5, r1                      ; copy size

  ; If the size is invalid, then there is no flushing required
  cmp r1, #0
  ble flush_done
  
  add r1, r0, r1                  ; get the end address
  sub r1, r1, #1                  ; Adjust the end address to correct value.

  dcache_line_size r2, r3         ; r2 holds cache line size
  sub r3, r2, #1                  ; r3 holds alignment mask
  
  ; Get the cache line aligned start address
  bic r0, r0, r3                  ; r0 holds cache aligned start address
  
  ; Get the cache line aligned end address and flush the line 
  bic r1, r1, r3                ; r1 holds cache aligned end address
  mcr p15, 0, r1, c7, c14, 1    ; clean & invalidate D / U line 
  
  ; Execute the loop until we have flushed all the lines between start and end	
flush_1oop
  mcr  p15, 0, r0, c7, c14, 1    ; clean & invalidate D / U line
  add  r0, r0, r2
  cmp  r0, r1
    bcs flush_done
  b flush_1oop
  
flush_done
  mdsb                             ; ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  ; L2 cache flush
  mov r0, r4                      ; copy addr to r0
  mov r1, r5                      ; copy size to r1
  bl l2_ranged_cache_flush
#endif

  mdsb
  misb
  ldmfd sp!, {r0-r12, pc}

;============================================================================
; dcache_inval_region
;
; PROTOTYPE
;   void dcache_inval_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Invalidate a memory region in the cache. Please note that the data in 
;   the cache will not be written back to the main memory. However, any 
;   further accesses to data in this region would result in a cache-miss.   
;============================================================================
dcache_inval_region
  stmfd  sp!, {r0-r12, lr}  
  mov r4, r0                      ; copy start addr
  mov r5, r1                      ; copy size	

  ; If the size is invalid, then there is no invalidation required
  cmp r1, #0
  ble inval_done

  add r1, r0, r1                  ; get the end address  
  sub r1, r1, #1                  ; Adjust the end address to correct value.
  mov r6, r1                      ; copy end address
  
  dcache_line_size r2, r3         ; r2 holds cache line size
  sub r3, r2, #1                  ; r3 holds alignment mask

  ; Get the cache line aligned start address and flush the line	
  bic r0, r0, r3                  ; r0 holds cache aligned start address
  cmp r0, r4                      ; compare if start address is already cache line aligned
  mcrne p15, 0, r0, c7, c14, 1 ; clean & invalidate D / U line only if not cache aligned
  
   ; Get the cache line aligned end address and flush the line		
  bic r1, r1, r3                  ; r1 holds cache aligned end address
  cmp r1, r6                      ; compare if end address is already cache line aligned
  mcrne p15, 0, r1, c7, c14, 1 ; clean & invalidate D / U line only if not cache aligned
  mcreq p15, 0, r1, c7, c6, 1  ; invalidate D / U line if cache aligned
  
   ; Execute the loop until we have invalidated all the lines between start and end	
inval_loop
  mcr p15, 0, r0, c7, c6, 1    ; invalidate D / U line
  add r0, r0, r2
  cmp r0, r1
  bcs inval_done
  b inval_loop
  
inval_done	
  mdsb                             ; ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  ; L2 cache inval
  mov r0, r4                      ; copy addr to r0
  mov r1, r5                      ; copy size to r1
  bl l2_ranged_cache_invalidate	
#endif

  mdsb
  misb
  ldmfd sp!, {r0-r12, pc}

;============================================================================
; dcache_clean_region
;
; PROTOTYPE
;   void dcache_clean_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans a memory region in the cache. Please note that this will write 
;   back any data that is dirty but it will not invalidate the cache region. 
;   So any further accesses to data in this region would result in a cache-hit.   
;============================================================================
dcache_clean_region
  stmfd sp!, {r0-r12, lr} 
  mov r4, r0                      ; copy addr
  mov r5, r1                      ; copy size

  ; If the size is invalid, then there is no clean required
  cmp r1, #0
  ble clean_done

  add r1, r0, r1                  ; get the end address  
  sub r1, r1, #1                  ; Adjust the end address to correct value.
  
  dcache_line_size r2, r3         ; r2 holds cache line size
  sub r3, r2, #1                  ; r3 holds alignment mask
  
    
  ; Get the cache line aligned start address
  bic r0, r0, r3                  ; r0 holds cache aligned start address
  
  ; Get the cache line aligned end address and clean the line	
  bic r1, r1, r3                 ; r1 holds cache aligned end address
  mcr p15, 0, r1, c7, c10, 1    ; clean D / U line	
  
  ; Execute the loop until we have cleaned all the lines between start and end
clean_loop
  mcr p15, 0, r0, c7, c10, 1 ; clean D / U line
  add r0, r0, r2
  cmp r0, r1
  bcs clean_done 
  b clean_loop
  
clean_done 
  mdsb                             ; ensure all memory operations are complete

#ifdef BOOT_USES_L2_DCACHE   	
  ; L2 cache clean
  mov r0, r4                      ; copy addr to r0
  mov r1, r5                      ; copy size to r1
  bl l2_ranged_cache_clean	
#endif

  mdsb
  misb
  ldmfd sp!, {r0-r12, pc} 

;============================================================================
; mmu_flush_cache
;
; DESCRIPTION -
;   Flush the entire cache system.
;   The data cache flush is now achieved using atomic clean / invalidates
;   working outwards from L1 cache. This is done using Set/Way based cache
;   maintainance instructions.
;   The instruction cache can still be invalidated back to the point of
;   unification in a single instruction.
;============================================================================  
mmu_flush_cache
  stmfd sp!, {r0-r12, lr} 
  ; Call memory barrier operations to ensure completion of all cache 
  ; maintenance & branch predictor operations appearing in program 
  ; order, before these instructions 
  mdsb
  misb
  bl  dcache_clean_all
  mov r0, #0
  mcr p15, 0, r0, c7, c5, 0; I+BTB cache invalidate
  mdsb
  misb

#ifdef BOOT_USES_L2_DCACHE      
  ; Flush L2 cache
  bl  l2_cache_flush
  mdsb
  misb
#endif

  ldmfd sp!, {r0-r12, pc}

;============================================================================
; mmu_flush_cache_and_disable
;
;
; DESCRIPTION -
;  Flush the entire cache system.
;  The data cache flush is now achieved using atomic clean / invalidates
;  working outwards from L1 cache. This is done using Set/Way based cache
;  maintainance instructions.
;  The instruction cache can still be invalidated back to the point of
;  unification in a single instruction.
;  Immediately after flushing data cache, the caches are disabled.
;
;============================================================================	
mmu_flush_cache_and_disable
  stmfd  sp!, {r0-r12, lr}	
  ; Call memory barrier operations to ensure completion of all cache 
  ; maintenance & branch predictor operations appearing in program 
  ; order, before these instructions 
  mdsb
  misb
  bl dcache_clean_all
  ; need to disable caches here. else the restoring of regs [ldmfd]
  ; caches those addresses and cache does not remain flushed.
  mmu_disable_i_and_d_cache
  
  ; Invalidate cache after disabling it. This ensures no valid cache lines
  ; remain in disabled cache.  
  mov  r0, #0
  mcr  p15, 0, r0, c7, c5, 0		; I+BTB cache invalidate
  mdsb
  misb
  
#ifdef BOOT_USES_L2_DCACHE
  ; Flush L2 cache and disable it
  bl  l2_cache_flush_and_disable
  mdsb
  misb
#endif
     
  ldmfd  sp!, {r0-r12, pc}

;============================================================================
; cache_mmu_init_page_table
;  INPUT       - PAGE_TABLE_BASE ADDRESS     
;  DESCRIPTION -
;        Initializes the ARM MMU.
;                1- sets a default value in the control register
;                2- sets the FSCE PID register to be flat mapping 
;                3- sets the physical address of the page table
;                4- populates the page_table with section entries
;                5- sets the domain access permisions
;                6- flushes out cache and write buffer
;                7- enables the mmu
;============================================================================
cache_mmu_init_page_table
   stmfd    sp!, {r0-r12, lr}
   mov      r5, r0                  ; backing up the base register
   bl       mmu_flush_cache
   mov      r0, #0    
   mcr      p15, 0, r0, c8, c7, 0   ; Invalidate instruction and data TLB
   mcr      p15, 0, r0, c2, c0, 2   ; Use Translation Base Reg 0, 16KB TTB boundary
   mov      r0, r5
   mcr      p15, 0, r0, c2, c0, 0   ; Update the TTB register to point to Page table base 
   mov      r0, #0
   mcr      p15, 0, r0, c13, c0, 0  ; Set FCSE PID Register to flat mapping
   mmu_set_dom_access   #0x00000001 ; set domain 0 for client access   
   mmu_enable                       ; Enable MMU
   ldmfd    sp!, {r0-r12, lr}
   bx lr

;============================================================================


;============================================================================
; mmu_relocate_page_table_base
;
;; ARGS
;   unsigned long *dst_base  - destination page table base
;
; DESCRIPTION -
;  Relocate the base(TTBR0) of page table, all attributes of TTBR0 will be 
;  perserved
;
;============================================================================
mmu_relocate_page_table_base

   stmfd    sp!,{r0-r12,lr}
   mdsb
   misb
   
   mov      r1,r0                   ; move destination page table base to r1
   mrc      p15, 0, r0, c2, c0, 0   ; R0 = TTBR0 
   and      r0, r0, #0x7F           ; KEEP THE ATTRIBUTE BITS
   orr      r0, r0, r1             ; get the new PT BASE with the old ATTR bits
   mcr      p15, 0, r0, c2, c0, 0   ; Update the TTB register to point to Page table base 
   mdsb
   misb
   mcr      p15, 0, r0, c8, c7, 0   ; UTLBIALL
   mcr      p15, 0, r0, c7, c5, 6   ; BPIALL
   
   mdsb
   misb
   ldmfd    sp!,{r0-r12,lr}
   bx lr

   
;============================================================================
; mmu_get_page_table_base
;
;; RETURN
;   unsigned long  -  page table base
;
; DESCRIPTION -
;  Return the base address of page table, all attributes of TTBR0 will be 
;  cleared
;
;============================================================================
mmu_get_page_table_base
   nop
   mrc      p15, 0, r0, c2, c0, 0 ; get TTBR0
   lsr      r0, r0, #14           ; [31:14] is base address
   lsl      r0, r0, #14
   bx lr
   
   
;============================================================================
; mmu_set_dacr
;
; PROTOTYPE
;   void mmu_set_dacr(uint32 dacr_value);
;
; ARGS
;   unsigned int dacr_value   - Value to be written to DACR
;   r0 register shall contain the argument
;
; DESCRIPTION
;   Configure DACR, Domain Access Control Register.
;   
;============================================================================    
mmu_set_dacr
  ; Configure Domain access control register register DACR to have appropriate 
  ; permissions for different domains.
  mcr     p15, 0, r0,  c3, c0, 0         ; Write DACR  
  mdsb
  misb
  bx lr
   
;============================================================================
   END
