#ifndef AOP_INTERRUPT_TABLE_H
#define AOP_INTERRUPT_TABLE_H
/*===========================================================================
                             aop_interrupt_table.h

DESCRIPTION:
  AOP interrupt table for external interrupts. This provides the exhaustive
  list of all the AOP interrupts supported on this target.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include"aop_hwio.h"
//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
typedef enum
{
    APCS_RPMHYPIPCINTERRUPT_0                      =    0  ,
    APCS_RPMHYPIPCINTERRUPT_1                      =    1  ,
    APCS_HLOS_IPC0_IRQ                             =    2  ,
    APCS_HLOS_IPC1_IRQ                             =    3  ,
    APCS_TZ_IPC0_IRQ                               =    4  ,
    APCS_TZ_IPC1_IRQ                               =    5  ,
    LPASS_IPC0_IRQ                                 =    6  ,
    LPASS_IPC1_IRQ                                 =    7  ,
    SSC_IPC0_IRQ                                   =    8  ,
    SSC_IPC1_IRQ                                   =    9  ,
    MSS_IPC0_IRQ                                   =    10 ,
    MSS_IPC1_IRQ                                   =    11 ,
    SPSS_IPC0_IRQ                                  =    12 ,
    SPSS_IPC1_IRQ                                  =    13 ,
    GFX_IPC0_IRQ                                   =    14 ,
    GFX_IPC1_IRQ                                   =    15 ,
    CAMSS_IPC0_IRQ                                 =    16 ,
    CAMSS_IPC1_IRQ                                 =    17 ,
    COMPUTE_DSP_IPC0_IRQ                           =    18 ,
    COMPUTE_DSP_IPC1_IRQ                           =    19 ,
    TBD_20                                         =    20 ,
    TBD_21                                         =    21 ,
    TBD_22                                         =    22 ,
    TBD_23                                         =    23 ,
    TBD_24                                         =    24 ,
    TBD_25                                         =    25 ,
    TBD_26                                         =    26 ,
    TBD_27                                         =    27 ,
    TBD_28                                         =    28 ,
    TBD_29                                         =    29 ,
    TBD_30                                         =    30 ,
    TBD_31                                         =    31 ,
    TOP_RBIF_IRQ                                   =    32 ,
    MX_RBIF_IRQ                                    =    33 ,
    GFX_RBIF_IRQ                                   =    34 ,
    MSS_RBIF_IRQ                                   =    35 ,
    BUS_TIMEOUT_IRQ                                =    36 ,
    VSENSE_RPM_ALARM_IRQ                           =    37 ,
    BIMC_INTERRUPT_3                               =    38 ,
    APCS_SYSWDOGBITEINT                            =    39 ,
    TLMM_SUMMARY_INTR                              =    40 ,
    TLMM_DIR_CONN_INTR                             =    41 ,
    CRC_ERROR_INT                                  =    42 ,
    TO_ERROR_INT                                   =    43 ,
    TBD_44                                         =    44 ,
    TBD_45                                         =    45 ,
    TBD_46                                         =    46 ,
    AOP_PDC_IRQ_OUT_0                              =    47 ,
    AOP_PDC_IRQ_OUT_1                              =    48 ,
    AOP_PDC_IRQ_OUT_2                              =    49 ,
    AOP_PDC_IRQ_OUT_3                              =    50 ,
    AOP_PDC_IRQ_OUT_4                              =    51 ,
    AOP_PDC_IRQ_OUT_5                              =    52 ,
    AOP_PDC_IRQ_OUT_6                              =    53 ,
    AOP_PDC_IRQ_OUT_7                              =    54 ,
    SOC_SLEEP_PDC_AOP_UNLOCK_REQ                   =    55 ,
    AOSSPWRDWNREQ                                  =    56 ,
    TBD_57                                         =    57 ,
    ARC_CX_INTERRUPT                               =    58 ,
    ARC_MX_INTERRUPT                               =    59 ,
    ARC_DDRPHY_INTERRUPT                           =    60 ,
    ARC_SSCX_INTERRUPT                             =    61 ,
    ARC_SSCMX_INTERRUPT                            =    62 ,
    ARC_GFX_INTERRUPT                              =    63 ,
    ARC_MSS_INTERRUPT                              =    64 ,
    ARC_DDR_INTERRUPT                              =    65 ,
    ARC_CXO_INTERRUPT                              =    66 ,
    BCM_IRQ_0                                      =    67 ,
    BCM_IRQ_1                                      =    68 ,
    BCM_IRQ_2                                      =    69 ,
    BCM_IRQ_3                                      =    70 ,
    BCM_IRQ_4                                      =    71 ,
    BCM_IRQ_5                                      =    72 ,
    BCM_IRQ_6                                      =    73 ,
    BCM_IRQ_7                                      =    74 ,
    VOLTAGE_VOTE_INTERRUPT                         =    75 ,
    ENABLE_VOTE_INTERRUPT                          =    76 ,
    MODE_VOTE_INTERRUPT                            =    77 ,
    HEADROOM_VOTE_INTERRUPT                        =    78 ,
    VOLTAGE_CHANGE_INTERRUPT                       =    79 ,
    ENABLE_CHANGE_INTERRUPT                        =    80 ,
    MODE_CHANGE_INTERRUPT                          =    81 ,
    HEADROOM_CHANGE_INTERRUPT                      =    82 ,
    VOLTAGE_SETTLED_INTERRUPT                      =    83 ,
    ENABLE_SETTLED_INTERRUPT                       =    84 ,
    MODE_SETTLED_INTERRUPT                         =    85 ,
    HEADROOM_SETTLED_INTERRUPT                     =    86 ,
    SEQUENCERS_INTERRUPT                           =    87 ,
    VRM_ERROR_INTERRUPT                            =    88 ,
    SOC_SLEEP_PDC_AOP_LOCK_REQ                     =    89 ,
    CX_COLLAPSE_PDC_AOP_UNLOCK_REQ                 =    90 ,
    CX_COLLAPSE_PDC_AOP_LOCK_REQ                   =    91 ,
    AOP_PDC_IRQ_OUT_9                              =    92 ,
    DBG_RSRC_REL_PDC_AOP_LOCK_REQ                  =    93 ,
    DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ                =    94 ,
    PDC_AOP_SUMMARY_IRQ                            =    95 ,
    DDR_AUX_IRQ_0                                  =    96 ,
    DDR_AUX_IRQ_1                                  =    97 ,
    DDR_AUX_IRQ_2                                  =    98 ,
    DDR_AUX_IRQ_3                                  =    99 ,
    DDR_AUX_IRQ_4                                  =    100,
    DDR_AUX_IRQ_5                                  =    101,
    DDR_AUX_IRQ_6                                  =    102,
    DDR_AUX_IRQ_7                                  =    103,
    CPRF_IRQ                                       =    104,
    IPCB_TIMEOUT                                   =    105,
    EE4_RPM_SPMI_PERIPH_IRQ                        =    106,
    AOP_WDOG_BARK_IRQ                              =    107,
    AOP_QTIMER_INTR_0                              =    108,
    AOP_QTIMER_INTR_1                              =    109,
    VOLTAGE_COMP_DONE                              =    110,
    AOP_PDC_IRQ_OUT_10                             =    111,
    AOP_PDC_IRQ_OUT_11                             =    112,
    AOP_PDC_IRQ_OUT_12                             =    113,
    AOP_PDC_IRQ_OUT_13                             =    114,
    AOP_PDC_IRQ_OUT_14                             =    115,
    AOP_PDC_IRQ_OUT_15                             =    116,
    AOP_PDC_IRQ_OUT_16                             =    117,
    AOP_PDC_IRQ_OUT_17                             =    118,
    AOP_PDC_IRQ_OUT_18                             =    119,
    AOP_PDC_IRQ_OUT_19                             =    120,
    AOP_PDC_IRQ_OUT_20                             =    121,
    AOP_PDC_IRQ_OUT_21                             =    122,
    AOP_PDC_IRQ_OUT_22                             =    123,
    AOP_PDC_IRQ_OUT_23                             =    124,
    AOP_PDC_IRQ_OUT_24                             =    125,
    RPMH_IDLE                                      =    126,
    AOP_RSCC_O_RSC_CORE_COMP_IRQ                   =    127,
    NVIC_INT_MAX
} interrupt_number;

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
static unsigned int polarity_registers[4] =
{
    HWIO_AOP_INTR_POLARITY_0_ADDR,
    HWIO_AOP_INTR_POLARITY_1_ADDR,
    HWIO_AOP_INTR_POLARITY_2_ADDR,
    HWIO_AOP_INTR_POLARITY_3_ADDR,
};

static unsigned int edge_registers[4] =
{
    HWIO_AOP_INTR_EDG_LVL_0_ADDR,
    HWIO_AOP_INTR_EDG_LVL_1_ADDR,
    HWIO_AOP_INTR_EDG_LVL_2_ADDR,
    HWIO_AOP_INTR_EDG_LVL_3_ADDR,
};

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================
/** 
 * <!-- aop_interrupt_init -->
 *
 * @brief Configures all the aop interrupts to their default type, and
 *        register system exception ISRs.
 *
 *  This is used to configure all the aop interrupts to their default
 *  type by modifying the POLARITY and EGDE bit for the corresponding
 *  interrupt and register the ISRs to the system exception interrupts such
 *  as APSS non secure watchdog bite interrupt.
 *
 *  @param client : None 
 *  @param context : None
 *
 *  @return void
 */
void aop_interrupt_init(void);

#endif /* AOP_INTERRUPT_TABLE_H */

