#include "HALhwio.h"
#include "coredump.h"
#include "aop_ulog.h"
#include "apss_hang_status_hwio.h"
#include "aop_interrupt_table.h"
#include "rex.h"
#include "cortex-m3.h"

#ifdef AOP_BASE
#undef AOP_BASE
#endif

#define AOP_BASE    0x60000000 /* used to convert virtual address to physical address */

#define GLADIATOR_HANG_STATUS_MASK  (HWIO_APSS_SHARED_GNOC_HANG_CONFIG_IO_COUNTER_STS_BMSK |\
                                     HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ACE_COUNTER_STS_BMSK)

#define GIC_ERR_STATUS_MASK  (HWIO_APSS_SHARED_GIC_ERR_IPC_ERR_INT_BMSK |\
                              HWIO_APSS_SHARED_GIC_ERR_IPC_FAULT_INT_BMSK)

#define APSS_CPU_IPM_SIZE    (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE - APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE)


uint32 glob_wd_bite = 0;

static volatile struct
{
    uint32 APSS_SHARED_FIRST_CORE_HANG;    /* CPU Core hang status */
    uint32 APSS_SHARED_GNOC_HANG_CONFIG;   /* Gladiator hang status */
    uint32 APSS_SHARED_GIC_ERR_IPC;        /* GIC error status */
    uint32 APSS_WDT_TMR1_WDOG_STATUS;      /* APPS non-sec wd status */
} apps_hang_status_regs;


#if 0
void tz_abort_sw_event(void)
{
    SWEVENT(RPM_TZ_HALT_INT_RECEIVED);
}

#endif


#if defined __ARMCC_VERSION__
__asm void abort_isr(void) __irq
{
    IMPORT abort

    b abort             // Use b to not overwrite EXEC_RETURN value to make rpm_parse_faults.cmm work
}

#elif defined __clang__
__asm void abort_isr(void) __irq
{
  asm("b abort"); // Use b to not overwrite EXEC_RETURN value to make rpm_parse_faults.cmm work
}

#endif /* __clang__ */


#if 0
__asm void tz_abort_isr(void) __irq
{
    IMPORT tz_abort_sw_event
    IMPORT abort

    sub sp, sp, #0x8    // Eight byte stack alignment is a requirement of AAPCS
    str lr, [sp]        // Save EXEC_RETURN value

    bl tz_abort_sw_event

    ldr lr, [sp]        // Restore EXEC_RETURN value
    add sp, sp, #0x8

    b abort             // Use b to not overwrite EXEC_RETURN value to make rpm_parse_faults.cmm work
}

void dcc_crc_error_abort_isr(void) __irq
{
    SWEVENT(RPM_DCC_CRC_ERR_INT_RECEIVED);
    abort();
}
#endif

/**
 *  Logs hang status registers using ULog.
 */
static void log_hang_status_registers(void)
{
    uint32 i;

    // Log APSS_ALIAS_n_CORE_HANG_THRESHOLD, APSS_ALIAS_n_CORE_HANG_VALUE, and APSS_ALIAS_n_CORE_HANG_CONFIG registers for Core 0 - 7
    for(i = 0; i < 8; i++)
    {
        uint32 base = HWIO_ADDR(APSS_ALIAS_0_CORE_HANG_THRESHOLD) + i * APSS_CPU_IPM_SIZE;

        for(uint32 j = 0; j < 3; j++)
        {
            uint32 addr = base + (j * sizeof(uint32));
            uint32 val  = in_dword(addr);

            SWEVENT(AOP_LOG_REGISTER, addr - AOP_BASE, val);
        }
    }

    // Log Gladiator hang thresholds
    for(i = 0; i <= HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_MAXn; i++)
    {
        uint32 addr = HWIO_ADDRI(APSS_SHARED_GNOC_HANG_THRESHOLD_n, i);
        uint32 val  = HWIO_INI(APSS_SHARED_GNOC_HANG_THRESHOLD_n, i);

        SWEVENT(AOP_LOG_REGISTER, addr - AOP_BASE, val);
    }

    // Log Gladiator hang counters
    for(i = 0; i <= HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_MAXn; i++)
    {
        uint32 addr = HWIO_ADDRI(APSS_SHARED_GNOC_HANG_COUNTER_n, i);
        uint32 val  = HWIO_INI(APSS_SHARED_GNOC_HANG_COUNTER_n, i);

        SWEVENT(AOP_LOG_REGISTER, addr - AOP_BASE, val);
    }
    if((apps_hang_status_regs.APSS_SHARED_GIC_ERR_IPC & GIC_ERR_STATUS_MASK) != 0)
    {
        //GIC Mem corruption
        SWEVENT(AOP_GIC_ERR, (apps_hang_status_regs.APSS_SHARED_GIC_ERR_IPC & HWIO_APSS_SHARED_GIC_ERR_IPC_ERR_INT_BMSK ? 1 : 0),
                (apps_hang_status_regs.APSS_SHARED_GIC_ERR_IPC & HWIO_APSS_SHARED_GIC_ERR_IPC_FAULT_INT_BMSK ? 1 : 0));
    }
                                                                                 
                                                                                 
}

extern void signal_abort_isr(void);
/** 
 * <!-- process_wd_isr -->
 *
 * @brief This routine is called when the wd_isr fires.
 *
 * When the non-secure WD BITE ISR is called, this function disables
 * the interrupt so we don't see it again, and signals the task space
 * to log the interrupt.  When we receive this interrupt, the task will
 * turn on the path to DDR so that when the SECURE Watchdog fires, we
 * will have DDR on.
 *
 * @param client : None 
 * @param context : None
 *
 * @return void
 */
void process_wd_isr(void)
{
  rex_disable_interrupt(APCS_SYSWDOGBITEINT);
  signal_abort_isr();
}

/** 
 * <!-- log_watchdog_bite -->
 *
 * @brief Log the non secure watchdog bite message.
 *
 * When the non-secure WD BITE ISR is called, the function above signaled
 * task space.  If we receive this log message, then we are confident the
 * tasks are still running.
 *
 * @param client : None 
 * @param context : None
 *
 * @return void
 */
void log_watchdog_bite(void)
{
    /*
     * Log AOP received the interrupt before reading the APPS regsiters
     * in case CPUSS AHB register access path hang and AOP is not able to read the registers 
     */
    SWEVENT(AOP_NON_SECURE_WD_BITE_INT_RECEIVED);
}

/** 
 * <!-- detect_apps_hang -->
 *
 * @brief Reads the registers to see if APPS itself is hung
 *
 * This function reads the status registers to check if an apps core is hung.
 * Note - it requires that the path from AOP->Apps is on.
 *
 * @param client : None 
 * @param context : None
 *
 * @return void
 */
void detect_apps_hang(void)
{
    /*
     * Set chip reset flag before reading status registers because
     * if bus fault happens while reading registers, AOP jumps to abort_isr().
     * Assume bus fault means APPS hanged and need to reset chip.
     */
    aop_core_dump.chip_reset_en = 1;

    /*
     * Read the hang status registers.
     */
    apps_hang_status_regs.APSS_SHARED_FIRST_CORE_HANG  = HWIO_IN(APSS_SHARED_FIRST_CORE_HANG );
    apps_hang_status_regs.APSS_SHARED_GNOC_HANG_CONFIG = HWIO_IN(APSS_SHARED_GNOC_HANG_CONFIG);
    apps_hang_status_regs.APSS_SHARED_GIC_ERR_IPC      = HWIO_IN(APSS_SHARED_GIC_ERR_IPC     );
    apps_hang_status_regs.APSS_WDT_TMR1_WDOG_STATUS    = HWIO_IN(APSS_WDT_TMR1_WDOG_STATUS   );

    SWEVENT(AOP_APPS_HANG_STATUS,
            apps_hang_status_regs.APSS_SHARED_FIRST_CORE_HANG,
            apps_hang_status_regs.APSS_SHARED_GNOC_HANG_CONFIG,
            apps_hang_status_regs.APSS_SHARED_GIC_ERR_IPC,
            apps_hang_status_regs.APSS_WDT_TMR1_WDOG_STATUS);

    /*
     * Check if APPS hanged:
     * - Core hang
     * - Gladiator hang
     * - GIC erros
     */
    if( (apps_hang_status_regs.APSS_SHARED_FIRST_CORE_HANG != 0) ||
        ((apps_hang_status_regs.APSS_SHARED_GNOC_HANG_CONFIG & GLADIATOR_HANG_STATUS_MASK) != 0) ||
        ((apps_hang_status_regs.APSS_SHARED_GIC_ERR_IPC & GIC_ERR_STATUS_MASK) != 0) )
    {
        /*
         * APPS hang detected. Log optional hang status registers to help debugging
         */
        log_hang_status_registers();
    }
    else
    {
        /*
         * It's non-secure wdog bite, not hang. No need to reset chip.
         */
        aop_core_dump.chip_reset_en = 0;
    }
}

#if defined __ARMCC_VERSION__
__asm void apps_non_secure_wd_bite_abort_isr(void) __irq
{
    IMPORT process_wd_isr
    //IMPORT abort
    IMPORT    glob_wd_bite
    ldr r1, =glob_wd_bite
    
    mov [r1], 0x1

    sub sp, sp, #0x8    // Eight byte stack alignment is a requirement of AAPCS
    str lr, [sp]        // Save EXEC_RETURN value

    bl process_wd_isr

    ldr lr, [sp]        // Restore EXEC_RETURN value
    add sp, sp, #0x8

    //b abort           // Use b to not overwrite EXEC_RETURN value to make rpm_parse_faults.cmm work
    bx lr               // Return
}

#elif defined __clang__
__asm void apps_non_secure_wd_bite_abort_isr(void) __irq
{
  asm("ldr r1, =glob_wd_bite");
  asm("ldr r2, =0x1 ");
  asm("str r2, [r1]");
  asm("sub sp, sp, #0x8");    // Eight byte stack alignment is a requirement of AAPCS
  asm("str lr, [sp]");        // Save EXEC_RETURN value

  asm("bl process_wd_isr");

  asm("ldr lr, [sp]");        // Restore EXEC_RETURN value
  asm("add sp, sp, #0x8");

  //asm("b abort");           // Use b to not overwrite EXEC_RETURN value to make rpm_parse_faults.cmm work
  asm("bx lr");               // Return
}
#endif /* __clang__ */

