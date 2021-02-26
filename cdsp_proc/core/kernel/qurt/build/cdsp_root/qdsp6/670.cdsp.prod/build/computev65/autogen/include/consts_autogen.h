#ifndef CONSTS_AUTOGEN_H
#define CONSTS_AUTOGEN_H
/*-----------------------------------
 * Offsets to various fields in TCB 
 *-----------------------------------*/
#define CONTEXT_hthread 21
#define CONTEXT_prio 20
#define CONTEXT_valid 22
#define CONTEXT_guest_info 16
#define CONTEXT_ssr 28
#define CONTEXT_elr 24
#define CONTEXT_ssrelr 24
#define CONTEXT_ugp 52
#define CONTEXT_gp 48
#define CONTEXT_ugpgp 48
#define CONTEXT_r3130 32
#define CONTEXT_r31 36
#define CONTEXT_r30 32
#define CONTEXT_r2928 40
#define CONTEXT_r29 44
#define CONTEXT_r28 40
#define CONTEXT_r2322 56
#define CONTEXT_r2120 64
#define CONTEXT_r1918 72
#define CONTEXT_r1716 80
#define CONTEXT_r1514 88
#define CONTEXT_r1312 96
#define CONTEXT_r1110 104
#define CONTEXT_r0908 112
#define CONTEXT_r0706 120
#define CONTEXT_r0504 128
#define CONTEXT_r0302 136
#define CONTEXT_r0100 144
#define CONTEXT_r00 144
#define CONTEXT_r01 148
#define CONTEXT_lc0sa0 152
#define CONTEXT_lc1sa1 160
#define CONTEXT_m1m0 168
#define CONTEXT_sr_preds 176
#define CONTEXT_usr 180
#define CONTEXT_preds 176
#define CONTEXT_totalcycles 280
#define CONTEXT_oncpu_start 272
#define CONTEXT_waiting 8
#define CONTEXT_futex_ptr 8
#define CONTEXT_pending 12
#define CONTEXT_error 240
#define CONTEXT_tid 232
#define CONTEXT_cs1cs0 184
#define CONTEXT_ccr 220
#define CONTEXT_ugpgp 48
#define CONTEXT_continuation 236
#define CONTEXT_r2726 192
#define CONTEXT_r2524 200
#define CONTEXT_thread_id 208
#define CONTEXT_interrupt_register_mask 212
#define CONTEXT_interrupt_received 216
#define CONTEXT_cfg 224
#define CONTEXT_hvx 264
#define CONTEXT_stid 226
#define CONTEXT_prio_orig 227
#define CONTEXT_frmlimitkey 248
#define CONTEXT_framelimit 248
#define CONTEXT_framekey 252
#define CONTEXT_pcb 256
#define CONTEXT_spd_notify 260
#define CONTEXT_pktcount 312


/*---------------------------------------------------------
 * TCB Size aligned to 32 bytes
 *---------------------------------------------------------*/
#define THREAD_CONTEXT_TOTALSIZE (320)


/*---------------------------------------------------------
 * Offsets to various fields in QURT error data structure
 *---------------------------------------------------------*/
#define ERR_OFFSET_status 0
#define ERR_OFFSET_error_param 4
#define ERR_OFFSET_globregs 8
#define ERR_OFFSET_locregs 104
#define ERR_OFFSET_GLOBAL_syscfg 0
#define ERR_OFFSET_GLOBAL_pcyclelo 8
#define ERR_OFFSET_GLOBAL_evb 16
#define ERR_OFFSET_GLOBAL_iad 24
#define ERR_OFFSET_GLOBAL_iel 32
#define ERR_OFFSET_GLOBAL_rev 40
#define ERR_OFFSET_GLOBAL_context 72
#define ERR_OFFSET_GLOBAL_imask0 48
#define ERR_OFFSET_GLOBAL_imask1 52
#define ERR_OFFSET_GLOBAL_imask2 56
#define ERR_OFFSET_GLOBAL_imask3 60
#define ERR_OFFSET_GLOBAL_imask4 64
#define ERR_OFFSET_GLOBAL_imask5 68
#define ERR_OFFSET_GLOBAL_chicken 76
#define ERR_OFFSET_GLOBAL_rgdr 80
#define ERR_OFFSET_GLOBAL_timerlo 88
#define ERR_OFFSET_LOCAL_SIZE 224
#define ERR_OFFSET_LOCAL_r0 0
#define ERR_OFFSET_LOCAL_r2 8
#define ERR_OFFSET_LOCAL_r4 16
#define ERR_OFFSET_LOCAL_r6 24
#define ERR_OFFSET_LOCAL_r8 32
#define ERR_OFFSET_LOCAL_r10 40
#define ERR_OFFSET_LOCAL_r12 48
#define ERR_OFFSET_LOCAL_r14 56
#define ERR_OFFSET_LOCAL_r16 64
#define ERR_OFFSET_LOCAL_r18 72
#define ERR_OFFSET_LOCAL_r20 80
#define ERR_OFFSET_LOCAL_r22 88
#define ERR_OFFSET_LOCAL_r24 96
#define ERR_OFFSET_LOCAL_r26 104
#define ERR_OFFSET_LOCAL_r28 112
#define ERR_OFFSET_LOCAL_r30 120
#define ERR_OFFSET_LOCAL_elr 128
#define ERR_OFFSET_LOCAL_gp 136
#define ERR_OFFSET_LOCAL_sa0 144
#define ERR_OFFSET_LOCAL_sa1 152
#define ERR_OFFSET_LOCAL_m0 160
#define ERR_OFFSET_LOCAL_preds 168
#define ERR_OFFSET_LOCAL_badva 176
#define ERR_OFFSET_LOCAL_cs0 184
#define ERR_OFFSET_LOCAL_cs1 188
#define ERR_OFFSET_LOCAL_ccr 192
#define ERR_OFFSET_LOCAL_g0 200
#define ERR_OFFSET_LOCAL_g1 204
#define ERR_OFFSET_LOCAL_g2 208
#define ERR_OFFSET_LOCAL_g3 212
#define ERR_OFFSET_LOCAL_framekey 220
#define ERR_OFFSET_LOCAL_framelimit 216
#define QURT_ERR_SIZE 1128
#define QURTK_WARM_CONTEXT_SIZE_IN_CACHE_LINE 3


/*---------------------------------------------------------
 * Offsets to various fields in UTCB
 *---------------------------------------------------------*/
#define OFFSET_UTCB_table0_ptr 16
#define OFFSET_UTCB_outertable_ptr 24
#define OFFSET_UTCB_localhandle_max 32
#define OFFSET_UTCB_thread_name 48
#define OFFSET_UTCB_thread_stack_size 72
#define OFFSET_UTCB_thread_stack_addr 76
#define OFFSET_UTCB_entrypoint 4
#define OFFSET_UTCB_arg 8




/*---------------------------------------------------------
 * Offsets to various fields in PCB struct
 *---------------------------------------------------------*/
#define OFFSET_PCB_asid 0
#define OFFSET_PCB_gmchain 80
#define OFFSET_PCB_int_enable 4
#define OFFSET_PCB_isr_cb_list 8
#define OFFSET_PCB_name 12
#define OFFSET_PCB_state 16
#define OFFSET_PCB_srm_handle 40
#define OFFSET_PCB_pid 88
#define OFFSET_PCB_refcnt 92
#define OFFSET_PCB_ssr_default 104
#define OFFSET_PCB_cfg 32




/*---------------------------------------------------------
 * Offsets to various fields in SPD Ctrl struct
 *---------------------------------------------------------*/
#define OFFSET_SPDCTL_assistant 48
#define OFFSET_SPDCTL_tmp_stack 36
#define OFFSET_SPDCTL_mem_count 92
#define OFFSET_SPDCTL_spd_mem 116


#define SIZEOF_UGP_PREALLOC 104
#define ALIGNOF_UGP_PREALLOC 8
#define SIZEOF_PCB 112
#define ALIGNOF_PCB 8
#define SIZEOF_MAILBOX 32
#define ALIGNOF_MAILBOX 8
#define SIZEOF_TCB 320
#define ALIGNOF_TCB 32
/*
|| Size of the qurtos_thread_info structure
*/
#define QURTOS_THREAD_INFO_SIZE 112

#define THREAD_INFO_pInvokeSP     100
#define THREAD_INFO_client_handle 64

#define THREAD_INFO_framelimit 40

/*---------------------------------------------------------
 * Max. no. of HW threads supported by QuRT
 *---------------------------------------------------------*/
#define QURT_CONFIG_MAX_HTHREADS (4)

/*---------------------------------------------------------
 * No. of TLB entries supported by QuRT
 *---------------------------------------------------------*/
#define QURT_CONFIG_TLB_SIZE (128)

/*---------------------------------------------------------
 * Constants for QDI trap and a fixup that can disable it
 *---------------------------------------------------------*/
#define QDI_TRAP_ENCODING          0x5480C008
#define QDI_FIXUP_ENCODING         0x78DFFFE0

/*---------------------------------------------------------
 * Constants which encode a single or double RTE instruction
 *---------------------------------------------------------*/
#define QURTK_ENCODING_ASM_RTE     0x57E0C000
#define QURTK_ENCODING_ASM_RTE_RTE 0x57E0C00057E0C000
#define QURTK_ENCODING_C_RTE       0x57E0C000u
#define QURTK_ENCODING_C_RTE_RTE   0x57E0C00057E0C000ull

/*---------------------------------------------------------
 * QURT version string 
 *---------------------------------------------------------*/
#define QURTK_VERSION "QURT Kernel ver.: 03.00.29.05.12.05.18"


#endif /* CONSTS_AUTOGEN_H */
