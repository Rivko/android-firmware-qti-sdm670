;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                     Boot A53 Assembly File
;
; GENERAL DESCRIPTION
;	This file contains the assembly instructions specialized A53 functions 
;
; Copyright 2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 01/14/14   ck      Initial release for Bear family
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the symbols defined in this module
    EXPORT  boot_enable_aarch64
    EXPORT  boot_generate_warm_reset
    EXPORT  boot_configure_vbar

;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    PRESERVE8
    AREA    BOOT_A53_ASM, CODE, READONLY
    CODE32
        
;======================================================================
;
; void boot_enable_aarch64(void)
;
; Description:
;   Switches processor to AARCH64 mode
; 
; Parameters:
;   None
;
; Return Value:
;   None 
;
;======================================================================
boot_enable_aarch64
    mrc p15, 0, r0, c12, c0, 2  ; read RMR
    orr r0, r0, #0x1            ; bit0 for A64 
    dsb
    mcr p15, 0, r0, c12, c0, 2
    isb
    dsb
    bx lr

	
;======================================================================
;
; void boot_generate_warm_reset(uint32)
;
; Description:
;   Generates a warm reset.  This function does not return.
; 
; Parameters:
;   Value to be carried in R0 across reset.
;
; Return Value:
;   None 
;
;======================================================================
boot_generate_warm_reset
    mrc p15, 0, r1, c12, c0, 2
    orr r1, r1, #2              ; bit 1 for RR
    dsb 
    mcr p15, 0, r1, c12, c0, 2
    isb
    dsb
    isb
    wfi
generate_warm_reset_loop
    b generate_warm_reset_loop


;======================================================================
;
; void boot_configure_vbar(uint32 vbar_addr)
;
; Description:
;   Configures VBAR to a different base address based on exit point.
; 
; Parameters:
;   R0 contains the address to which VBAR will be configured to.
;
; Return Value:
;   None 
;
;======================================================================
boot_configure_vbar
    mcr p15, 0, r0, c12, c0, 0  ; Write VBAR Register
    bx  lr

	
END