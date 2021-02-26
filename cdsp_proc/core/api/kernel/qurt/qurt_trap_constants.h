#ifndef QURT_TRAP_CONSTANTS_H
#define QURT_TRAP_CONSTANTS_H
/**
  @file qurt_trap_constants.h
  @brief Trap constants for trap table      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

//#TRAPS_START   marks start of trap definitions
 //1-16 are reserved for mini traps
#define QURT_TRAP_RESERVED1                             1
#define QURT_TRAP_GET_PCYCLES                           2
#define QURT_TRAP_GET_TID                               3
#define QURT_TRAP_GETPID                                4
#define QURT_TLB_COUNT_FREE                             5
#define QURT_TLB_LOOKUP                                 6
#define QURT_TRAP_RESERVED2                             7
#define QURT_TRAP_SAFE_WRITE                            8
#define QURT_TRAP_SAFE_READ                             9
#define QURT_TRAP_SAFE_FINISH                           10
#define QURT_TRAP_PROBE_ADDRESS                         11
#define QURT_TRAP_DISPATCH                              12

//17-31 are reserved for fastints
#define QURT_TRAP_FUTEX_WAKE                            17
#define QURT_TRAP_WRITEC                                18
#define QURT_TRAP_RAISE_INTERRUPT                       19
#define QURT_TRAP_FATAL_EXIT                            20
#define QURT_TRAP_THREAD_CONFIG                         21
#define QURT_TRAP_RAISE_L1_INTERRUPT                    22
#define QURT_TRAP_THREAD_ID                             23

//These are all normal + cancellable traps
#define QURT_TRAP_FUTEX_WAIT                            32
#define QURT_TRAP_FUTEX_WAIT_INT                        33
#define QURT_TRAP_ACK_INT                               34
#define QURT_TRAP_THREAD_WAIT_IDLE                      35
#define QURT_TRAP_THREAD_WAIT_ACTIVE                    36
#define QURT_TRAP_VID_GET                               37
#define QURT_TRAP_APCR_CONTROL                          38
#define QURT_TRAP_EXIT                                  39
#define QURT_TRAP_ISLAND_REGISTER_EXCEP                 40
#define QURT_TRAP_ISLAND_EXIT                           41
#define QURT_TRAP_FUTEX_WAIT_CANCELLABLE                42
#define QURT_TRAP_FUTEX_WAIT_INT_CANCELLABLE            43
#define QURT_TRAP_REGISTER_FATAL_NOTIFIER               44
#define QURT_TRAP_FUTEX_VALIDATE                        45
#define QURT_TRAP_REGISTER_FASTINT                      46
#define QURT_TRAP_DEREGISTER_FASTINT                    47
#define QURT_TRAP_ISR_REGISTER                          48
#define QURT_TRAP_ISR_DEREGISTER                        49
#define QURT_TRAP_REGISTER_INT                          50
#define QURT_TRAP_DEREGISTER_INT                        51
#define QURT_TRAP_ENABLE_INT                            52
#define QURT_TRAP_DISABLE_INT                           53
#define QURT_TRAP_INT_STATUS                            54
#define QURT_TRAP_CLEAR_INT                             55
#define QURT_TRAP_INT_GETCONFIG                         56
#define QURT_TRAP_INT_SETCONFIG                         57
#define QURT_TRAP_INT_REG_BITMASK                       58
#define QURT_TRAP_ISR_SUBCALL                           59
#define QURT_TRAP_ISR_RETURN                            60
#define QURT_TRAP_ISR_CB_CHECKIN                        61
#define QURT_TRAP_PLACEHOLDER2                          62
#define QURT_TRAP_YIELD                                 63
#define QURT_TRAP_THREAD_RESUME                         64
#define QURT_TRAP_PLACEHOLDER                           65
#define QURT_TRAP_PIFUTEX_WAIT                          66
#define QURT_TRAP_PIFUTEX_WAKE                          67
#define QURT_TRAP_PRIO_ORIG_GET                         68
#define QURT_TRAP_PRIO_SET                              69
#define QURT_TRAP_PRIO_GET                              70
#define QURT_TRAP_MASK_PRIOS_ABOVE                      71
#define QURT_TRAP_PROFILE_ENABLE                        72
#define QURT_TRAP_CPUTIME                               73
#define QURT_TRAP_PMU_CTRL                              74
#define QURT_TRAP_GENERIC_NON_CSI                       75
#define QURT_TRAP_POWER_CONTROL                         76
#define QURT_TRAP_POWER_CONTROL_STATS                   77
#define QURT_TRAP_POWER_CONTROL_STATS_TICKS             78
#define QURT_TRAP_IPEND_CLEAR                           79
#define QURT_TRAP_IPEND_GET                             80
#define QURT_TRAP_DISABLE_PD_INT                        81
#define QURT_TRAP_TCM_CONTROL                           82
#define QURT_TRAP_REGISTER_PAGER                        83
#define QURT_TRAP_GET_PAGER_PHYSADDR                    84
#define QURT_TRAP_HVX_CONTROL                           85
#define QURT_TRAP_CACHE_OPT                             86
#define QURT_TRAP_L2FETCH_DISABLE                       87
#define QURT_TLB_CREATE_MAPPING                         88
#define QURT_TLB_REMOVE_MAPPING                         89
#define QURT_TLB_QUERY                                  90
#define QURT_TLB_WRITE                                  91
#define QURT_TLB_READ                                   92
#define QURT_TLB_FLUSH                                  93
#define QURT_TRAP_PLACEHOLDER3                          94
#define QURT_TRAP_GET_TRACE_MARKER                      95
#define QURT_TRAP_HAS_PREEMPT_TRACE                     96
#define QURT_TRAP_CONFIG_ETM                            97
#define QURT_TRAP_CONFIG_LIVELOCK                       98
#define QURT_TRAP_FUTEX_WAIT_HASH                       99
#define QURT_TRAP_FUTEX_WAKE_HASH                       100
#define QURT_TRAP_FUTEX_WAIT64                          101
#define QURT_TRAP_FUTEX_WAIT64_HASH                     102
#define QURT_TRAP_OSTHREAD                              103
#define QURT_TRAP_PTRACE                                104
#define QURT_TRAP_SECURE_PTRACE                         105
#define QURT_TRAP_SYSTEM_JUMP                           106
#define QURT_TRAP_OSPAGETABLE                           107
#define QURT_TRAP_ISLAND_SET_INT                        108
#define QURT_TRAP_ISLAND_ENTER                          109
#define QURT_TRAP_CACHE_L1KILL                          110
#define QURT_TRAP_GENERIC_PRIV_NON_CSI                  111
#define QURT_TRAP_FUTEX_WAIT_HASH_CANCELLABLE           112
#define QURT_TRAP_SECURE_PROC_CTRL                      113
#define QURT_TRAP_SECURE_PROC_TRAP                      114
#define QURT_TRAP_MAILBOX                               115
#define QURT_TRAP_GENERIC_PRIV_NON_I                    116
#define QURT_TRAP_STM_GET_ATTR                          117
#define QURT_TRAP_ISLAND_EXIT_STATUS                    118
#define QURT_TRAP_VTLB                                  119
#define QURT_TRAP_PKTCOUNT                              120
#define QURT_TRAP_SRM                                   121
#define QURT_TRAP_CONFIG_STM_TRACE                      122
#define QURT_TRAP_CFG_OPS                               123
//trap1 handler
#define QURT_TRAP1_FREEZE_HELPER                        1
#define QURT_TRAP1_QDI_HANDLER                          2
#define QURT_TRAP1_UNDEFINED                            3

//#TRAPS_END  This marks the end of trap number definitions. No traps should be defined after this point

//other definitions
#define QURT_TRAP_MINI_END                              16
#define QURT_TRAP_END                                   123
#define QURT_TRAP_FASTPATH                              255
#define QURT_TRAP1_END                                  3

#endif /* QURT_TRAP_CONSTANTS_H */
