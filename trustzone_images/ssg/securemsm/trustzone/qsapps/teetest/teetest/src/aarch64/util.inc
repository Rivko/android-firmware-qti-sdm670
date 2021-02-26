; from securemsm/trustzone/armlib/v8/inc/util.inc

  #include     "sys_fields.inc"

; ----------------
; MACRO: CurCPUNum
; ----------------
    .macro    CurCPUNum rx, ry
        mrs     \rx, MPIDR_EL1
        ubfx    \ry, \rx, #MPIDR_EL1_AFF0_SHFT, #MPIDR_EL1_AFF_LENGTH
        ubfx    \rx, \rx, #MPIDR_EL1_AFF1_SHFT, #MPIDR_EL1_AFF_LENGTH
        add     \rx, \ry, \rx, LSL #TZBSP_CLUSTER_SIZE_SHFT
    .endm

; -----------------
; MACRO: RestoreSCR
; -----------------
    .macro RestoreSCR rx, ry
        CurCPUNum \ry, \rx
        ldr     \rx, =saved_scr         ; cached SCR base address.
        lsl     \ry, \ry, #2                 ; CPU to byte offset
        ldr     \rx, [\rx, \ry]         ; Read SCR for this CPU.
        msr     SCR_EL3, \rx
    .endm


; -------------------
; MACRO: SetSecure
; -------------------
   .macro SetSecure rx, ry
        mrs     \rx, SCR_EL3
        ; Set up SCR for Secure world.
        ; IRQ taken by the OS directly in the secure world
        ; FIQ taken by the OS directly in the secure world
        ; SError taken by the OS directly in the secure world 
        ; Secure EL1 have access to secure system timer (CNTPS)
        mov     \ry, #SCR_EL3_DEFAULT
        msr     SCR_EL3, \ry
        isb
    .endm

; -------------------
; MACRO: SetScrEl3
; -------------------
    .macro    SetScrEl3 rx, imm
        mov     \rx, \imm
        msr     SCR_EL3, \rx
        isb
    .endm

; -------------------
; MACRO: GetScrEl3
; -------------------
    .macro    GetScrEl3 rx
        mrs     \rx, SCR_EL3
        isb
    .endm

; -------------------------------
; MACRO: MonCallReturnToNonSecure
; -------------------------------
; All monitor calls return to non-secure mode.
;
; Arguments:
; rx: Used as a work register, set to zero.
    .macro    MonCallReturnToNonSecure rx
        SetNonSecure    \rx
        mov             \rx, #0     ; Do not leak anything in this reg.
        eret                        ; Return to the normal world.
    .endm

; -------------------
; MACRO: PushTwo
; -------------------
    .macro    PushTwo x0, x1
        stp     \x1, \x0, [sp, #-0x10]!
    .endm

; -------------------
; MACRO: PopTwo
; -------------------
    .macro    PopTwo x0, x1
        ldp     \x0, \x1, [sp], #0x10
    .endm

; ----------------
; MACRO: PushAArch32NPRegs
; ----------------
    .macro    PushAArch32NPRegs
        PushTwo x5,  x6
        PushTwo x7,  x8
        PushTwo x9,  x10
        PushTwo x11, x12
        PushTwo x13, x14
        PushTwo x15, x16
        PushTwo x17, x18
        PushTwo x19, x20
        PushTwo x21, x22
        PushTwo x23, x24
        PushTwo x25, x26
        PushTwo x27, x28
        PushTwo x29, x30
    .endm

; ----------------
; MACRO: PopAArch32NPRegs
; ----------------
    .macro    PopAArch32NPRegs
        PopTwo x30, x29
        PopTwo x28, x27
        PopTwo x26, x25
        PopTwo x24, x23
        PopTwo x22, x21
        PopTwo x20, x19
        PopTwo x18, x17
        PopTwo x16, x15
        PopTwo x14, x13
        PopTwo x12, x11
        PopTwo x10, x9
        PopTwo x8,  x7
        PopTwo x6,  x5
    .endm

; ----------------
; MACRO: PushAArch64CSRegs
; ----------------
; Push AArch64 Callee-saved registers
    .macro    PushAArch64CSRegs
        PushTwo x19, x20
        PushTwo x21, x22
        PushTwo x23, x24
        PushTwo x25, x26
        PushTwo x27, x28
        PushTwo x29, x30
    .endm

; ----------------
; MACRO: PopAArch64CSRegs
; ----------------
; Pop AArch64 Callee-saved registers
    .macro    PopAArch64CSRegs
        PopTwo x30, x29
        PopTwo x28, x27
        PopTwo x26, x25
        PopTwo x24, x23
        PopTwo x22, x21
        PopTwo x20, x19
    .endm

; ----------------
; MACRO: PushAArch64PSRegs
; ----------------
; Push AArch64 Param and Scratch Registers.  The order of registers
; is important due to assumptions of context structure on the stack.
    .macro    PushAArch64PSRegs
        PushTwo x19, x18
        PushTwo x17, x16
        PushTwo x15, x14
        PushTwo x13, x12
        PushTwo x11, x10
        PushTwo x9,  x8
        PushTwo x7,  x6
        PushTwo x5,  x4
        PushTwo x3,  x2
        PushTwo x1,  x0
    .endm

; ----------------
; MACRO: PopAArch64PSRegs
; ----------------
    .macro    PopAArch64PSRegs
        PopTwo  x0,  x1
        PopTwo  x2,  x3
        PopTwo  x4,  x5
        PopTwo  x6,  x7
        PopTwo  x8,  x9
        PopTwo  x10, x11
        PopTwo  x12, x13
        PopTwo  x14, x15
        PopTwo  x16, x17
        PopTwo  x18, x19
    .endm

; ----------------
; MACRO: WipeParamRegs
; ----------------
; Wipes the parameter registers for AArch64
    .macro    WipeParamRegs
        mov     x0,  #0
        mov     x1,  #0
        mov     x2,  #0
        mov     x3,  #0
        mov     x4,  #0
        mov     x5,  #0
        mov     x6,  #0
        mov     x7,  #0
        mov     x8,  #0
    .endm

; ----------------
; MACRO: WipeScratchRegs
; ----------------
; Wipes the scratch registers for AArch64
    .macro    WipeScratchRegs
        mov     x9,  #0
        mov     x10, #0
        mov     x11, #0
        mov     x12, #0
        mov     x13, #0
        mov     x14, #0
        mov     x15, #0
        mov     x16, #0
        mov     x17, #0
        mov     x18, #0
;        mov     x31, #0  ; <- compiler does not like
    .endm

; ----------------
; MACRO: WipeAllGPRegs
; ----------------
; Wipes all AArch64 GP registers
    .macro    WipeAllGPRegs
        WipeParamRegs
        WipeScratchRegs
        mov     x19, #0
        mov     x20, #0
        mov     x21, #0
        mov     x22, #0
        mov     x23, #0
        mov     x24, #0
        mov     x25, #0
        mov     x26, #0
        mov     x27, #0
        mov     x28, #0
        mov     x29, #0
        mov     x30, #0
;        mov     x31, #0  ; <- compiler does not like
    .endm


; ----------------
; MACRO: CorruptMonRegs
; ----------------
; Use in testing to set all the registers to something monitor-specific
    .macro    CorruptMonRegs
        mov     x0,  #0xA
        mov     x1,  #0xB
        mov     x2,  #0xC
        mov     x3,  #0xD
        mov     x4,  #0xE
        mov     x5,  #0xF
        mov     x6,  #0xA
        mov     x7,  #0xB
        mov     x8,  #0xC
        mov     x9,  #0xD
        mov     x10, #0xE
        mov     x11, #0xF
        mov     x12, #0xA
        mov     x13, #0xB
        mov     x14, #0xC
        mov     x15, #0xD
        mov     x16, #0xE
        mov     x17, #0xF
        mov     x18, #0xA
        mov     x19, #0xB
        mov     x20, #0xC
        mov     x21, #0xD
        mov     x22, #0xE
        mov     x23, #0xF
        mov     x24, #0xA
        mov     x25, #0xB
        mov     x26, #0xC
        mov     x27, #0xD
        mov     x28, #0xE
        mov     x29, #0xF
        mov     x30, #0xA
    .endm

; -----------------------------------------------------------------------------
; MACRO: SpinlockObtain
; -----------------------------------------------------------------------------
; Multicore safe spinlock implementation. Waits until the monitor lock
; is cleared and the lock is successfully claimed.
  .macro  SpinlockObtain lock_addr, cpu, cpuwork, hw_work
    CurCPUNum \cpu, \cpuwork
    add     \cpu, \cpu,  #0x1             ; (cpu num + 1) = TZ is locked
42:   ldaxr   \hw_work, [\lock_addr]  ; Load the current lock status.
    cbnz    \hw_work, 42b          ; Is TZ locked?
    stlxr   \hw_work, \cpu, [\lock_addr] ; Not locked: try to claim the lock.
    cbnz    \hw_work, 42b          ; Not locked: Did we get the lock?
    dmb     sy                      ; Ensure other observers see lock claim.
  .endm

; -----------------------------------------------------------------------------
; MACRO: SpinlockRelease
; -----------------------------------------------------------------------------
; Releases a lock acquired by WaitForAccess
  .macro  SpinlockRelease lock_addr, zero
    ; Ensure all reads/writes are visible to other observers before
    ; releasing the lock.
    dmb     sy
    mov     \zero, #0            ; Zero equals unlocked.
    str     \zero, [\lock_addr]  ; Clear the lock
    dmb     sy
  .endm
