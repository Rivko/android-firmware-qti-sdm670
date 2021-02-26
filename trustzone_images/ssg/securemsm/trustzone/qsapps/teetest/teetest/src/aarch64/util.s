#include "util.inc"

;============================================================================
; uint32 get_current_el(void)
;============================================================================
    .global get_current_el
.type get_current_el, %function
get_current_el:
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    and     x0, x0, #3
    ret
