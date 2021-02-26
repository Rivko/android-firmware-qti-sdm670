/*===========================================================================
                              aop_interrupt_table.c

DESCRIPTION:

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "aop_interrupt_table.h"
#include "rex.h"
#include "cortex-m3.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct
{
  interrupt_number intNumber;
  interrupt_config intConfig;
} interruptData;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static interruptData aopInterrupts[128] =
{
   {APCS_RPMHYPIPCINTERRUPT_0,	       RISING_EDGE},
   {APCS_RPMHYPIPCINTERRUPT_1,	       RISING_EDGE},
   {APCS_HLOS_IPC0_IRQ,		       RISING_EDGE},
   {APCS_HLOS_IPC1_IRQ,		       RISING_EDGE},
   {APCS_TZ_IPC0_IRQ,		       RISING_EDGE},
   {APCS_TZ_IPC1_IRQ,		       RISING_EDGE},
   {LPASS_IPC0_IRQ,		       RISING_EDGE},
   {LPASS_IPC1_IRQ,		       RISING_EDGE},
   {SSC_IPC0_IRQ,		       RISING_EDGE},
   {SSC_IPC1_IRQ,		       RISING_EDGE},
   {MSS_IPC0_IRQ,		       RISING_EDGE},
   {MSS_IPC1_IRQ,		       RISING_EDGE},
   {SPSS_IPC0_IRQ,		       RISING_EDGE},
   {SPSS_IPC1_IRQ,		       RISING_EDGE},
   {GFX_IPC0_IRQ,		       RISING_EDGE},
   {GFX_IPC1_IRQ,		       RISING_EDGE},
   {CAMSS_IPC0_IRQ,		       RISING_EDGE},
   {CAMSS_IPC1_IRQ,		       RISING_EDGE},
   {COMPUTE_DSP_IPC0_IRQ,	       RISING_EDGE},
   {COMPUTE_DSP_IPC1_IRQ ,	       RISING_EDGE},
   {TBD_20,			       RISING_EDGE},
   {TBD_21,			       RISING_EDGE},
   {TBD_22,			       RISING_EDGE},
   {TBD_23,			       RISING_EDGE},
   {TBD_24,			       RISING_EDGE},
   {TBD_25,			       RISING_EDGE},
   {TBD_26,			       RISING_EDGE},
   {TBD_27,			       RISING_EDGE},
   {TBD_28,			       RISING_EDGE},
   {TBD_29,			       RISING_EDGE},
   {TBD_30,			       RISING_EDGE},
   {TBD_31,			       RISING_EDGE},			
   {TOP_RBIF_IRQ,		       RISING_EDGE},
   {MX_RBIF_IRQ,		       RISING_EDGE},
   {GFX_RBIF_IRQ,		       RISING_EDGE},
   {MSS_RBIF_IRQ,		       RISING_EDGE},
   {BUS_TIMEOUT_IRQ,		       LEVEL_HIGH},
   {VSENSE_RPM_ALARM_IRQ,	       LEVEL_HIGH},
   {BIMC_INTERRUPT_3,		       LEVEL_HIGH},
   {APCS_SYSWDOGBITEINT,	       LEVEL_HIGH},
   {TLMM_SUMMARY_INTR,		       LEVEL_HIGH},
   {TLMM_DIR_CONN_INTR,		       LEVEL_HIGH},
   {CRC_ERROR_INT,		       LEVEL_HIGH},
   {TO_ERROR_INT,		       LEVEL_HIGH},
   {TBD_44,			       LEVEL_HIGH},
   {TBD_45,			       LEVEL_HIGH},
   {TBD_46,			       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_0,		       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_1,		       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_2,		       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_3,		       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_4,		       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_5,		       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_6,		       LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_7,		       RISING_EDGE},
   {SOC_SLEEP_PDC_AOP_UNLOCK_REQ,      LEVEL_HIGH},  
   {AOSSPWRDWNREQ,		       RISING_EDGE},
   {TBD_57,			       LEVEL_HIGH},
   {ARC_CX_INTERRUPT,		       LEVEL_HIGH},
   {ARC_MX_INTERRUPT,		       LEVEL_HIGH},
   {ARC_DDRPHY_INTERRUPT,	       LEVEL_HIGH},
   {ARC_SSCX_INTERRUPT,		       LEVEL_HIGH},
   {ARC_SSCMX_INTERRUPT,	       LEVEL_HIGH},
   {ARC_GFX_INTERRUPT,		       LEVEL_HIGH},
   {ARC_MSS_INTERRUPT,		       LEVEL_HIGH},
   {ARC_DDR_INTERRUPT,		       LEVEL_HIGH},
   {ARC_CXO_INTERRUPT,		       LEVEL_HIGH},
   {BCM_IRQ_0,			       LEVEL_HIGH},
   {BCM_IRQ_1,			       LEVEL_HIGH},
   {BCM_IRQ_2,			       LEVEL_HIGH},
   {BCM_IRQ_3,			       LEVEL_HIGH},
   {BCM_IRQ_4,			       LEVEL_HIGH},
   {BCM_IRQ_5,			       LEVEL_HIGH},
   {BCM_IRQ_6,			       LEVEL_HIGH},
   {BCM_IRQ_7,			       LEVEL_HIGH},
   {VOLTAGE_VOTE_INTERRUPT,            LEVEL_HIGH},
   {ENABLE_VOTE_INTERRUPT,             LEVEL_HIGH},
   {MODE_VOTE_INTERRUPT,               LEVEL_HIGH},
   {HEADROOM_VOTE_INTERRUPT,           LEVEL_HIGH},
   {VOLTAGE_CHANGE_INTERRUPT,          LEVEL_HIGH},
   {ENABLE_CHANGE_INTERRUPT,           LEVEL_HIGH},
   {MODE_CHANGE_INTERRUPT,             LEVEL_HIGH},
   {HEADROOM_CHANGE_INTERRUPT,         LEVEL_HIGH},
   {VOLTAGE_SETTLED_INTERRUPT,         LEVEL_HIGH},
   {ENABLE_SETTLED_INTERRUPT,          LEVEL_HIGH},
   {MODE_SETTLED_INTERRUPT,            LEVEL_HIGH},
   {HEADROOM_SETTLED_INTERRUPT,        LEVEL_HIGH},
   {SEQUENCERS_INTERRUPT,              LEVEL_HIGH},
   {VRM_ERROR_INTERRUPT,               LEVEL_HIGH},
   {SOC_SLEEP_PDC_AOP_LOCK_REQ,        LEVEL_HIGH},
   {CX_COLLAPSE_PDC_AOP_UNLOCK_REQ,    LEVEL_HIGH},
   {CX_COLLAPSE_PDC_AOP_LOCK_REQ,      LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_9,                 LEVEL_HIGH},
   {DBG_RSRC_REL_PDC_AOP_LOCK_REQ,     LEVEL_HIGH},
   {DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ,   LEVEL_HIGH},
   {PDC_AOP_SUMMARY_IRQ,               RISING_EDGE},
   {DDR_AUX_IRQ_0,                     LEVEL_HIGH}, 
   {DDR_AUX_IRQ_1,                     LEVEL_HIGH},
   {DDR_AUX_IRQ_2,                     LEVEL_HIGH},
   {DDR_AUX_IRQ_3,                     LEVEL_HIGH},
   {DDR_AUX_IRQ_4,                     LEVEL_HIGH},
   {DDR_AUX_IRQ_5,                     LEVEL_HIGH},
   {DDR_AUX_IRQ_6,                     LEVEL_HIGH},
   {DDR_AUX_IRQ_7,                     LEVEL_HIGH},
   {CPRF_IRQ,                          RISING_EDGE},
   {IPCB_TIMEOUT,                      LEVEL_HIGH},
   {EE4_RPM_SPMI_PERIPH_IRQ,           LEVEL_HIGH},
   {AOP_WDOG_BARK_IRQ,                 RISING_EDGE},
   {AOP_QTIMER_INTR_0,                 LEVEL_HIGH},
   {AOP_QTIMER_INTR_1,                 LEVEL_HIGH},
   {VOLTAGE_COMP_DONE,                 LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_10,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_11,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_12,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_13,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_14,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_15,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_16,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_17,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_18,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_19,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_20,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_21,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_22,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_23,                LEVEL_HIGH},
   {AOP_PDC_IRQ_OUT_24,                LEVEL_HIGH},
   {RPMH_IDLE,                         LEVEL_HIGH},
   {AOP_RSCC_O_RSC_CORE_COMP_IRQ,      LEVEL_HIGH},
};

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
extern __asm void apps_non_secure_wd_bite_abort_isr(void) __irq;


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
/** 
 * <!-- config_aop_interrupts -->
 *
 * @brief Configures all the aop interrupts to their default type.
 *
 *  This is used to configure all the aop interrupts to their default
 *  type by modifying the POLARITY and EGDE bit for the corresponding
 *  interrupt.
 *
 *  @param client : None 
 *  @param context : None
 *
 *  @return void
 */
static void configure_aop_interrupts(void)
{
    int i;
    int numberOfAopInterrupts = (sizeof(aopInterrupts)/sizeof(interruptData));
    for (i=0; i < numberOfAopInterrupts; i++)
    {
        rex_configure_interrupt(aopInterrupts[i].intNumber, aopInterrupts[i].intConfig);
        rex_clear_interrupt(aopInterrupts[i].intNumber);
    }
}

/**
 * <!-- register_aop_exception_isrs -->
 *
 * @brief Register ISRs to the system exception interrupts.
 *
 *  This is used to register ISRs to the system exeption interrupt
 *  such as APPS non secure wdog bite interrupt.
 *
 * @param client : None 
 * @param context : None
 *
 * @return void
 */
static void register_aop_exception_isrs(void)
{
    rex_set_interrupt_vector(APCS_SYSWDOGBITEINT, (rex_isr_func_type)apps_non_secure_wd_bite_abort_isr);
    rex_enable_interrupt(APCS_SYSWDOGBITEINT);
}

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
void aop_interrupt_init(void)
{
    configure_aop_interrupts();

    register_aop_exception_isrs();
}

