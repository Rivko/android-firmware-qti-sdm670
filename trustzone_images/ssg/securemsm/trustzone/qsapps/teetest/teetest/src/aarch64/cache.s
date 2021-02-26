;============================================================================
;                             MODULE IMPORTS
;============================================================================
#include     "util.inc"

;============================================================================
;                             MODULE EXPORTS
;============================================================================

;============================================================================
;
;                        MACROS AND FUNCTIONS
;
;============================================================================
    ;.section .armv8_lib_code.CACHE, "ax", %progbits
    ;.p2align 3

;============================================================================
; void iterate_set_way_louis(void (*fn)(uint64 set_and_way_info))
; Note: iterate_set_way may return
;============================================================================
    .global iterate_set_way_louis
.type iterate_set_way_louis,%function
iterate_set_way_louis:
    mrs     x1, CLIDR_EL1           ; read clidr
    mov     x2, #CLIDR_LOUIS_MSK
    ands    x2, x2, x1                  ; extract louis from clidr
    beq     1f                     ; if louis is 0, then no need to clean
    lsr     x2, x2, #(CLIDR_LOUIS_SHFT-1) ; x2 = (LoUIS * 2)
    b       iterate_set_way
1:  ret

;============================================================================
; void iterate_set_way_loc(void (*fn)(uint64 set_and_way_info))
; Note: iterate_set_way may return
;============================================================================
.type iterate_set_way_loc,%function
iterate_set_way_loc:
    mrs     x1, CLIDR_EL1           ; read clidr
    mov     x2, #CLIDR_LOC_MSK
    ands    x2, x2, x1                  ; extract loc from clidr
    beq     2f                     ; if (LoC == 0), then no need to iterate
    lsr     x2, x2, #(CLIDR_LOC_SHFT-1) ; x2 = (LoC * 2)
    b       iterate_set_way
2:  ret

;============================================================================
; void iterate_set_way_cluster_loc(void (*fn)(uint64 set_and_way_info))
; Note: iterate_set_way may return
;============================================================================
.type iterate_set_way_cluster_loc,%function
iterate_set_way_cluster_loc:
    mrs     x1, CLIDR_EL1          ; read clidr
    mov     x2, #CLIDR_LOUIS_MSK
    ands    x2, x2, x1             ; extract louis from clidr
    beq     3f                     ; if louis is 0, then no need to clean
    add     x2, x2, #0x200000      ; including coherency cache
    lsr     x2, x2, #(CLIDR_LOUIS_SHFT-1) ; x2 = (LoUIS * 2)
    b       iterate_set_way
3:  ret

;============================================================================
; static void iterate_set_way(void (*fn)(uint64 set_and_way_info),
;                             el3_reg_t CLIDR_EL1,
;                             el3_reg_t last_level_times_two)
;============================================================================
.type iterate_set_way,%function
iterate_set_way:
    dsb     sy                      ; ensure ordering with previous
                                    ; memory  accesses
    mov     x6, x0                  ; x6 = function pointer arg
    mov     x12, x30                ; Save LR
    mov     x10, #0                 ; start at L1 cache
.Llevel_loop:
    add     x8, x10, x10, lsr #1    ; x8 = (3 * level)
    lsr     x3, x1, x8              ; extract cache type bits from clidr
    and     x3, x3, #CLIDR_CTYPE_MSK    ;   for current cache level
    cmp     x3, #CLIDR_CTYPE_D_ONLY ; see what cache we have at this level
    blt     .Lnext_level            ;   values below have no d_cache
    msr     CSSELR_EL1, x10         ; select current cache level in csselr
    isb                             ; isb to sych cssrelr & ccsidr
    mrs     x3, CCSIDR_EL1          ; x3 = ccsidr
    and     x8, x3, #CCSIDR_LINE_SZ_BMSK
                                    ; x8 = log2(line length) - 4
    add     x8, x8, #CCSIDR_LINE_SZ_OFFST
                                    ; x8 = log2(line length), offset of set
    ubfx    x4, x3, #CCSIDR_NUM_WAYS_SHFT, #CCSIDR_NUM_WAYS_BITS
                                    ; x4 = number of ways from ccsidr
    clz     w5, w4                  ; find bit position of way size increment
    ubfx    x7, x3, #CCSIDR_NUM_SETS_SHFT, #CCSIDR_NUM_SETS_BITS
                                    ; x7 = set index (from ccsidr)
.Lset_loop:
    mov     x9, x4                  ; x9 = way index
.Lway_loop:
    lsl     x0, x9, x5              ; x0 = way index
    orr     x0, x0, x10             ; x0 |= cache level
    lsl     x3, x7, x8              ; x3 = shifted set index
    orr     x0, x0, x3              ; x0 |= shifted set index
    blr     x6                      ; Jump to function for given cache line
    subs    x9, x9, #1              ; decrement the way index
    bge     .Lway_loop
    subs    x7, x7, #1              ; decrement the set index
    bge     .Lset_loop
.Lnext_level:
    add     x10, x10, #2            ; cache level += 2
    cmp     x2, x10                 ; (last_level * 2) (x2) >=
                                    ; (cache  level * 2) (x10)
    dsb     sy
    bgt     .Llevel_loop
.Literate_finished:
    mov     x10, #0
    msr     CSSELR_EL1, x10         ; select level 0 in csselr
    isb
    mov     x30, x12                ; Restore LR
    ret	                            ; return to original caller



;============================================================================
; static void clean_dcache_line(uint64 set_and_way_info)
;
; DESCRIPTION -
;   Flush a single cache line by set/way
;============================================================================
.type clean_dcache_line,%function
clean_dcache_line:
    dc      csw, x0
    ret

;============================================================================
; static void clean_and_inval_dcache_line(uint64 set_and_way_info)
;============================================================================
.type clean_and_inval_dcache_line,%function
clean_and_inval_dcache_line:
    dc      cisw, x0
    ret

;============================================================================
; static void inval_dcache_line(uint64 set_and_way_info)
;============================================================================
.type inval_dcache_line,%function
inval_dcache_line:
    dc      isw, x0
    ret

;============================================================================
; void clean_cluster_dcache(void)
; Note: iterate_set_way_cluster_loc returns
;============================================================================
    .global clean_cluster_dcache
.type clean_cluster_dcache, %function
clean_cluster_dcache:
    ldr     x0, =clean_dcache_line
    b       iterate_set_way_cluster_loc

;============================================================================
; void clean_all_dcache(void)
; Note: iterate_set_way_loc returns
;============================================================================
    .global clean_all_dcache
.type clean_all_dcache, %function
clean_all_dcache:
    ldr     x0, =clean_dcache_line
    b       iterate_set_way_loc

;============================================================================
; void clean_and_inval_all_dcache(void)
; Note: iterate_set_way_loc returns
;============================================================================
    .global clean_and_inval_all_dcache
.type clean_and_inval_all_dcache, %function
clean_and_inval_all_dcache:
    ldr     x0, =clean_and_inval_dcache_line
    b       iterate_set_way_loc

;============================================================================
; void clean_and_inval_all_dcache_louis(void)
; Note: iterate_set_way_louis returns
;============================================================================
    .global clean_and_inval_all_dcache_louis
.type clean_and_inval_all_dcache_louis, %function
clean_and_inval_all_dcache_louis:
    ldr     x0, =clean_and_inval_dcache_line
    b       iterate_set_way_louis

;============================================================================
; void inval_all_dcache(void)
; Note: iterate_set_way_loc returns
;============================================================================
    .global inval_all_dcache
.type inval_all_dcache, %function
inval_all_dcache:
    ldr     x0, =inval_dcache_line
    b       iterate_set_way_loc

;============================================================================
; void clean_and_inval_l1_l2_cache
;============================================================================
    .global  clean_and_inval_l1_l2_cache
.type clean_and_inval_l1_l2_cache, %function
clean_and_inval_l1_l2_cache:
    ; Flush L1 cache
    mov      x0, #0x0
    sys      #1,c15,c14,#0,x0
    isb
    dsb      sy
    ; Flush L2 cache
    mov      x0, #0x2
    sys      #1,c15,c14,#0,x0
    isb
    dsb      sy
    ret

;============================================================================
; void enable_el3_icache(void)
;============================================================================
    .global enable_el3_icache
.type enable_el3_icache,%function
enable_el3_icache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL3
    orr     x0, x0, #SCTLR_EL3_I_BIT
    msr     SCTLR_EL3, x0
    isb
    ret

;============================================================================
; void enable_el3_dcache(void)
;============================================================================
    .global enable_el3_dcache
.type enable_el3_dcache,%function
enable_el3_dcache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL3
    orr     x0, x0, #SCTLR_EL3_C_BIT
    msr     SCTLR_EL3, x0
    isb
    ret

;============================================================================
; void disable_el3_icache(void)
;============================================================================
    .global disable_el3_icache
.type disable_el3_icache,%function
disable_el3_icache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL3
    mov     x1, #SCTLR_EL3_I_BIT
    bic     x0, x0, x1
    msr     SCTLR_EL3, x0
    isb
    ret

;============================================================================
; void disable_el3_dcache(void)
;============================================================================
    .global disable_el3_dcache
.type disable_el3_dcache,%function
disable_el3_dcache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL3
    mov     x1, #SCTLR_EL3_C_BIT
    bic     x0, x0, x1
    msr     SCTLR_EL3, x0
    isb
    ret

;============================================================================
; void disable_el1_dcache(void)
;============================================================================
    .global disable_el1_dcache
.type disable_el1_dcache, %function
disable_el1_dcache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL1
    mov     x1, #SCTLR_EL1_C_BIT
    bic     x0, x0, x1
    msr     SCTLR_EL1, x0
    isb
    ret

;============================================================================
; boolean is_el3_icache_enabled(void)
;============================================================================
    .global is_el3_icache_enabled
.type is_el3_icache_enabled,%function
is_el3_icache_enabled:
    isb
    mrs     x0, SCTLR_EL3
    ands    x0, x0, #SCTLR_EL3_I_BIT
    lsr     x0, x0, #SCTLR_EL3_I_SHFT
    dsb     sy
    isb
    ret

;============================================================================
; boolean is_el3_dcache_enabled(void)
;============================================================================
    .global is_el3_dcache_enabled
.type is_el3_dcache_enabled,%function
is_el3_dcache_enabled:
    isb
    mrs     x0, SCTLR_EL3
    ands    x0, x0, #SCTLR_EL3_C_BIT
    lsr     x0, x0, #SCTLR_EL3_C_SHFT
    dsb     sy
    isb
    ret

;==========================================================================
; void tzbsp_cache_flush(void)
;==========================================================================
    .global tzbsp_cache_flush
.type tzbsp_cache_flush,%function
tzbsp_cache_flush:
    PushTwo x29, x30
    mrs     x1, DAIF                ; Read DAIF
    msr     DAIFSet, #DAIFSET_I_BIT ; Mask IRQ
    PushTwo x0, x1
    bl      clean_and_inval_all_dcache
    ic      iallu
    dsb     sy
    isb
    PopTwo  x1, x0
    msr     DAIF, x1                ; Restore DAIF
    PopTwo  x30, x29
    ret

;==========================================================================
; void tzbsp_cluster_dcache_clean_all(void)
;==========================================================================
    .global tzbsp_cluster_dcache_clean_all
.type tzbsp_cluster_dcache_clean_all,%function
tzbsp_cluster_dcache_clean_all:
    PushTwo x29, x30
    mrs     x1, DAIF                ; Read DAIF
    msr     DAIFSet, #DAIFSET_I_BIT ; Mask IRQ
    PushTwo x0, x1
    bl      clean_cluster_dcache
    PopTwo  x1, x0
    msr     DAIF, x1                ; Restore DAIF
    PopTwo  x30, x29
    ret

;==========================================================================
; void tzbsp_dcache_clean_all(void)
;==========================================================================
    .global tzbsp_dcache_clean_all
.type tzbsp_dcache_clean_all,%function
tzbsp_dcache_clean_all:
    PushTwo x29, x30
    mrs     x1, DAIF                ; Read DAIF
    msr     DAIFSet, #DAIFSET_I_BIT ; Mask IRQ
    PushTwo x0, x1
    bl      clean_all_dcache
    PopTwo  x1, x0
    msr     DAIF, x1                ; Restore DAIF
    PopTwo  x30, x29
    ret

;==========================================================================
; void tzbsp_dcache_inval_all(void)
;==========================================================================
    .global tzbsp_dcache_inval_all
.type tzbsp_dcache_inval_all,%function
tzbsp_dcache_inval_all:
    PushTwo x29, x30
    mrs     x1, DAIF                ; Read DAIF
    msr     DAIFSet, #DAIFSET_I_BIT ; Mask IRQ
    PushTwo x0, x1
    bl      inval_all_dcache
    PopTwo  x1, x0
    msr     DAIF, x1                ; Restore DAIF
    PopTwo  x30, x29
    ret

;==========================================================================
; void tzbsp_dcache_flush_all(void)
;==========================================================================
    .global tzbsp_dcache_flush_all
.type tzbsp_dcache_flush_all,%function
tzbsp_dcache_flush_all:
    PushTwo x29, x30
    mrs     x1, DAIF                ; Read DAIF
    msr     DAIFSet, #DAIFSET_I_BIT ; Mask IRQ
    PushTwo x0, x1
    bl      clean_and_inval_all_dcache
    PopTwo  x1, x0
    msr     DAIF, x1                ; Restore DAIF
    PopTwo  x30, x29
    ret

;==========================================================================
; void tzbsp_dcache_flush_all_louis(void)
;==========================================================================
    .global tzbsp_dcache_flush_all_louis
.type tzbsp_dcache_flush_all_louis,%function
tzbsp_dcache_flush_all_louis:
    PushTwo x29, x30
    mrs     x1, DAIF                ; Read DAIF
    msr     DAIFSet, #DAIFSET_I_BIT ; Mask IRQ
    PushTwo x0, x1
    bl      clean_and_inval_all_dcache_louis
    PopTwo  x1, x0
    msr     DAIF, x1                ; Restore DAIF
    PopTwo  x30, x29
    ret


;==========================================================================
; void tzbsp_icache_inv(void)
;==========================================================================
    .global tzbsp_icache_inv
.type tzbsp_icache_inv,%function
tzbsp_icache_inv:
    mrs     x0, DAIF                ; Read DAIF
    msr     DAIFSet, #DAIFSET_I_BIT ; Mask IRQ
    ic      iallu
    dsb     sy
    isb
    msr     DAIF, x0                ; Restore DAIF
    ret

;============================================================================
; void tzbsp_enable_icache(void)
;============================================================================
    .global tzbsp_enable_icache
.type tzbsp_enable_icache,%function
tzbsp_enable_icache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL1
    orr     x0, x0, #SCTLR_EL1_I_BIT
    msr     SCTLR_EL1, x0
    isb
    ret

;============================================================================
; void tzbsp_enable_dcache(void)
;============================================================================
    .global tzbsp_enable_dcache
.type tzbsp_enable_dcache,%function
tzbsp_enable_dcache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL1
    orr     x0, x0, #SCTLR_EL1_C_BIT
    msr     SCTLR_EL1, x0
    isb
    ret

;============================================================================
; void tzbsp_disable_icache(void)
;============================================================================
    .global tzbsp_disable_icache
.type tzbsp_disable_icache,%function
tzbsp_disable_icache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL1
    mov     x1, #SCTLR_EL1_I_BIT
    bic     x0, x0, x1
    msr     SCTLR_EL1, x0
    isb
    ret

;============================================================================
; void tzbsp_disable_dcache(void)
;============================================================================
    .global tzbsp_disable_dcache
.type tzbsp_disable_dcache,%function
tzbsp_disable_dcache:
    dsb     sy
    isb
    mrs     x0, SCTLR_EL1
    mov     x1, #SCTLR_EL1_C_BIT
    bic     x0, x0, x1
    msr     SCTLR_EL1, x0
    isb
    ret

;============================================================================
; boolean tzbsp_is_icache_enabled(void)
;============================================================================
    .global tzbsp_is_icache_enabled
.type tzbsp_is_icache_enabled,%function
tzbsp_is_icache_enabled:
    mrs     x0, SCTLR_EL1
    ubfx    x0, x0, #SCTLR_EL1_I_SHFT, #0x1
    dsb     sy
    isb
    ret

;============================================================================
; boolean tzbsp_is_dcache_enabled(void)
;============================================================================
    .global tzbsp_is_dcache_enabled
.type tzbsp_is_dcache_enabled,%function
tzbsp_is_dcache_enabled:
    mrs     x0, SCTLR_EL1
    ubfx    x0, x0, #SCTLR_EL1_C_SHFT, #0x1
    dsb     sy
    isb
    ret


;============================================================================
; void tzbsp_enable_branch_prediction(void)
; TODO: Remove if Hydra does not support BP maintenance
;============================================================================
    .global tzbsp_enable_branch_prediction
.type tzbsp_enable_branch_prediction,%function
tzbsp_enable_branch_prediction:
   ret

;============================================================================
; void tzbsp_bp_invalidate_all(void.L)
; TODO: Remove if Hydra does not support BP maintenance
;============================================================================
    .global tzbsp_bp_invalidate_all
.type tzbsp_bp_invalidate_all,%function
tzbsp_bp_invalidate_all:
   ret


    .end
