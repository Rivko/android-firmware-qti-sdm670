;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
; Copyright (c) 2015 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
; MACRO dcache_granule_size
;
; ARGS
;   $reg - cache granule size in bytes
;   $tmp - cache granule size in words used for calculation
;          of the size in bytes
;
; DESCRIPTION
;   cache_granule_size - get the size of the smallest dcache/unified
;   cache (available on ARMv7+). Note that the value of $reg and $tmp
;   will be modified.
;
;============================================================================
  .macro    dcache_granule_size reg, tmp
    mrc     p15, 0, \tmp, c0, c0, 1 ; read CTR
    lsr     \tmp, \tmp, #16
    and     \tmp, \tmp, #0xF        ; \tmp = CTR[DminLine]
    mov     \reg, #4                ; CTR[DminLine] = log2 num words
    lsl     \reg, \reg, \tmp        ; cache writeback granule size in bytes
  .endm

;============================================================================
; void clean_and_inval_dcache_region(uint32 vaddr, uint32 len)
;============================================================================
    .global clean_and_inval_dcache_region
.type clean_and_inval_dcache_region,%function
clean_and_inval_dcache_region:
    stmfd   sp!, {r0, lr}
    dsb     sy                      ; data barrier before flushing

    add     r1, r1, r0              ; get the end address
    dcache_granule_size r2, r3      ; r2 = cache granule in bytes
    sub     r3, r2, #1              ; r3 = (cache granule size - 1)
    bic     r0, r0, r3              ; r0 = start address with cache granule
                                    ;      size bits removed
.Lflush_loop:
    mcr     p15, 0, r0, c7, c14, 1  ; clean & invalidate line
    add     r0, r0, r2              ; r0 = (r0 + cache granule size)
    cmp     r0, r1                  ; r0 > "end of region" ?
    blt     .Lflush_loop
    dsb     sy                      ; ensure all memory operations are complete
    ldmfd   sp!, {r0, pc}

;============================================================================
; dcache_iterate_set_way_loc
;
; DESCRIPTION -
;   Iterates through the set/way and calls function passed in
;============================================================================
    .global dcache_iterate_set_way_loc
.type dcache_iterate_set_way_loc,%function
dcache_iterate_set_way_loc:
    stmfd   sp!, {r0-r12, lr}
    mrc     p15, 1, r11, c0, c0, 1  ; read clidr
    ands    r3, r11, #0x7000000     ; extract loc from clidr
    mov     r3, r3, lsr #23         ; left align loc bit field
    beq     .Literate_finished      ; if loc is 0, then no need to clean
    b       dcache_iterate_set_way

;============================================================================
; dcache_flush_line
;
; DESCRIPTION -
;   Flush a single cache line by set/way
;============================================================================
    .global dcache_flush_line
.type dcache_flush_line,%function
dcache_flush_line:
    mcr     p15, 0, r0, c7, c14, 2 ; clean & invalidate by set/way
    bx      lr
    
;============================================================================
; void dcache_iterate_set_way(void)
;
; Do not call directly.  Call *_loc or *_louis
;============================================================================
    .global dcache_iterate_set_way
.type dcache_iterate_set_way,%function
dcache_iterate_set_way:
    dsb                             ; ensure ordering with previous
                                    ; memory  accesses
    mov     r10, #0                 ; start clean at cache level 0
.Literate_loop1:
    add     r2, r10, r10, lsr #1    ; work out 3x current cache level
    mov     r1, r11, lsr r2         ; extract cache type bits from clidr
    and     r1, r1, #7              ; mask of the bits for current cache only
    cmp     r1, #2                  ; see what cache we have at this level
    blt     .Literate_skip          ; skip if no cache, or just i-cache
    mcr     p15, 2, r10, c0, c0, 0  ; select current cache level in cssr
    ISB                             ; isb to sych the new cssr&csidr
    mrc     p15, 1, r1, c0, c0, 0   ; read the new csidr
    and     r2, r1, #7              ; extract the length of the cache lines
    add     r2, r2, #4              ; add 4 (line length offset)
    ldr     r4, =0x3ff
    ands    r4, r4, r1, lsr #3      ; find maximum number on the way size
    clz     r5, r4                  ; find bit position of way size increment
    ldr     r7, =0x7fff
    ands    r7, r7, r1, lsr #13     ; extract max number of the index size
.Literate_loop2:
    mov     r9, r4                  ; create working copy of max way size
.Literate_loop3:
    orr     r0, r10, r9, lsl r5     ; factor way and cache number into r0
    orr     r0, r0, r7, lsl r2      ; factor index number into r0
    bl      dcache_flush_line       ; flush the cache line
    subs    r9, r9, #1              ; decrement the way
    bge     .Literate_loop3
    subs    r7, r7, #1              ; decrement the index
    bge     .Literate_loop2
.Literate_skip:
    add     r10, r10, #2            ; increment cache number
    cmp     r3, r10
    bgt     .Literate_loop1
.Literate_finished:
    mov     r10, #0                 ; switch back to cache level 0
    mcr     p15, 2, r10, c0, c0, 0  ; select current cache level in cssr
    isb
    dsb
    ldmfd   sp!, {r0-r12, pc}

;============================================================================
; dcache_flush_all
;
; DESCRIPTION -
;   Flush the whole D-cache.
;============================================================================
    .global dcache_flush_all
.type dcache_flush_all,%function
dcache_flush_all:
    push    {lr}
    dsb
    bl      dcache_iterate_set_way_loc
    ldmfd   sp!, {pc}

;============================================================================
; cache_flush
;
; DESCRIPTION -
;  Flush the entire cache system.  The data cache flush is achieved
;  using atomic clean / invalidates working outwards from L1
;  cache. This is done using Set/Way based cache maintainance
;  instructions.  The instruction cache can still be invalidated back
;  to the point of unification in a single instruction.  This also
;  flushes the branch target cache.
;============================================================================
    .global cache_flush
.type cache_flush,%function
cache_flush:
    push    {lr}

    bl      dcache_flush_all

    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0   ; ICIALLU: instruction & branch target
                                    ; invalidate
    ldmfd   sp!, {pc}
    
.end
