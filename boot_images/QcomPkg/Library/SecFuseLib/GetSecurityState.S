//============================================================================
//
// Call TZ to get security state 
//
// Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved 
//============================================================================

//============================================================================
//
//                       EDIT HISTORY FOR MODULE
//
// when      who     what, where, why
// --------  -----   ----------------------------------------------------------
// 01/24/17  ai      Initial revision.
//============================================================================

//============================================================================
//                             MODULE EXPORTS
//============================================================================

  GCC_ASM_EXPORT(GetSecurityState)

//============================================================================
//                                 MACROS 
//============================================================================

// TZ_INFO_GET_SECURE_STATE
#define SMC_ID   0x02000604

// TZ_INFO_GET_SECURE_STATE_PARAM_ID
#define PARAM_ID 0x00000000

#define FuncPrologue                        \
    stp           x29, x30, [sp, #-0x10]!;  \
    mov           x29,sp

#define FuncEpilogue                        \
    ldp           x29, x30, [sp], #0x10  ;  \
    ret

#define SmcCall(func)                 \
smc_loop_##func:                  ;   \
    smc           #0x0            ;   \
    cmp           w0, #0          ;   \
    b.gt          smc_loop_##func

//============================================================================
//                               FUNCTIONS 
//============================================================================

.text
.align 3

//============================================================================
// UINT64 GetSecurityState (OUT tz_get_secure_state_rsp_s*)
//
// @param      
//
// @retval         0 - Success
// @retval  Negative - Failed. See {SmcErrNoType} 
//============================================================================
GetSecurityState:
    FuncPrologue
    stp x19, x20, [sp, #-0x10]!
    mov x19, x0                   // x19 = &tz_get_secure_state_rsp_s
    ldr x0, =SMC_ID          
    mov x1, #PARAM_ID 
    SmcCall(get_security_state)
    cmp w0, #0
    b.ne Exit                     // Return if smc call failed 
    mov x20, #1
    str x20, [x19], #0x8           // tz_get_secure_state_rsp_s.common_rsp.status = 1, 
                                  //    x19 = &tz_get_secure_state_rsp_s.status_0
    stp x1, x2, [x19]             // tz_get_secure_state_rsp_s.status_0 = x1,
                                  //    tz_get_secure_state_rsp_s.status_1 = x2
Exit:
    ldp x19, x20, [sp], #0x10
    FuncEpilogue

