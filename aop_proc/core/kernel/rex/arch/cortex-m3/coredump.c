
#include "coredump.h"
#include "aop_hwio.h"
#include "aop_ipc.h"

//#include "HALhwio.h"
// #include "rpm_hwio.h"

#include "aop_ulog.h"
//#include "swevent.h"

//#include "hw_version.h"
//#include "Chipinfo.h"
//#include "gcc_reset_hwio.h"
#include "mpm_pshold_hwio.h"

extern RPM_LOG_CONTEXT rpm_ulogContext;

//__attribute__((section("aop_core_dump")))
m3_core_dump_t aop_core_dump =
{
    .cookie = 0xbaff1ed,
    .chip_reset_en = 0,
    .ulog_state = &rpm_ulogContext,
};

void notify_tz(void)
{
    AOP_IPC_SendInterrupt(AOP_IPC_APPS_TZ, AOP_IPC_SecondaryInterrupt);
}

#if 0
void notify_apss(void)
{
    HWIO_OUT(RPM_IPC, (1 << 6) | (1 << 22));
}
#endif

void err_fatal_sw_event(unsigned lr, unsigned ipsr)
{
    uint32 gcc_reset = 0;	
    SWEVENT(AOP_ERR_FATAL, lr, ipsr);
#if 0
    gcc_reset = HWIO_IN(GCC_RESET_STATUS);
    if(gcc_reset != 0)
    {
      SWEVENT(GCC_RESET, gcc_reset);
    }
#endif
}


void reset_chip(void)
{
    if(aop_core_dump.chip_reset_en)
    {
        //SWEVENT(RPM_RESET_CHIP);

        HWIO_OUT(MPM_PS_HOLD, 0);
    }
}

#if 0
void rpm_core_dump_init(void)
{
    rpm_core_dump.hw_version.v = HWIO_IN(TCSR_SOC_HW_VERSION);
}
#endif


#if defined __ARMCC_VERSION__
__asm void abort(void)
{
  IMPORT aop_core_dump
  //IMPORT notify_apss
  IMPORT err_fatal_sw_event
  IMPORT reset_chip
  //IMPORT rbcpr_core_dump

  ; Disable all interrupts except NMI
  cpsid f
      
  ; We are going to use r0-r3 as scratch registers for a while, so save them first.
  push  {r0-r3}

  ; Get the address of the core dump structure.
  ldr   r0, =aop_core_dump

  ; If we have already dumped registers on this run, stop here.
  ldr   r1, [r0, #4] ; aop_core_dump.dumped_at first chunk
  cbnz  r1, hang     ; if nonzero, previous dump performed
  ldr   r1, [r0, #8] ; aop_core_dump.dumped_at second chunk
  cbnz  r1, hang     ; if nonzero, previous dump performed

  ; Read the timetick and store it
  ldr   r1, =timer_addr
  ldr   r1, [r1]     ; r1 now points to "lo" word of timetick
retry
  ldr   r3, [r1, #4] ; read "hi" word
  ldr   r2, [r1]     ; read "lo" word
  str   r2, [r0, #4] ; save "lo" word
  ldr   r2, [r1, #4] ; read "hi" word again
  cmp   r2, r3       ; if "hi" word rolled over while we dumped
  bne   retry        ;   we must retry
  str   r2, [r0, #8] ; save "hi" word

  ; Store r4-r12
  add   r0, r0, #28 ; skip cookie, dump timestamp, and r0-r3
  stmia r0!, {r4-r12}

  ; Go back and get r0-r3
  pop   {r2-r5}
  ldr   r1, =aop_core_dump  ; get a fresh pointer to the core dump [r0 is just past registers, we need that later]
  add   r1, r1, #12         ; move past cookie and timestamp to where r0-r3 should be stored
  stm   r1!, {r2-r5}

  ; Grab the remaining registers.
  mrs   r1, MSP
  mrs   r2, PSP
  mov   r3, lr
  ldr   r4, =abort
  mrs   r5, XPSR
  mrs   r6, PRIMASK
  mrs   r7, FAULTMASK
  mrs   r8, BASEPRI
  mrs   r9, CONTROL
  stmia r0!, {r1-r9}

  ; Grab and save all the auxiliary fault status registers
  ldr   r1, =0xE000ED24
  ldmia r1!, {r2-r8}
  stmia r0!, {r2-r8}
  
  ; Add something useful to the log
  mov   r0, lr
  mrs   r1, IPSR
  push  {r0, lr}            ; push lr so we can pop later and preserve the stack for the debugger. Need to push 8 bytes
  bl    err_fatal_sw_event
  ;bl    rbcpr_core_dump

#ifdef DDR_ABORT_REG_DUMP
  bl    ddr_store_status_regs_error_fatal
#endif

  ;bl    notify_apss
  
  ;bl    reset_chip

  ; Do this last
  pop   {r0, lr}

hang
  b     hang

  ALIGN 4

timer_addr
  ;dcd   HWIO_RPM_F0_QTMR_V1_CNTPCT_LO_ADDR

#elif defined __clang__

__attribute__((used))
static const uint32_t timer_addr = HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_ADDR;

__asm void abort(void)
{
  /* Disable all interrupts except NMI */
  asm("cpsid f");

  /* We are going to use r0-r3 as scratch registers for a while, so save them first. */
  asm("push  {r0-r3}");

  /* Get the address of the core dump structure. */
  asm("ldr   r0, =aop_core_dump");

  /* If we have already dumped registers on this run, stop here. */
  asm("ldr   r1, [r0, #4]"); /* aop_core_dump.dumped_at first chunk */
  asm("cbnz  r1, hang    "); /* if nonzero, previous dump performed */
  asm("ldr   r1, [r0, #8]"); /* aop_core_dump.dumped_at second chunk */
  asm("cbnz  r1, hang    "); /* if nonzero, previous dump performed */

  /* Read the timetick and store it */
  asm("ldr   r1, =timer_addr");
  asm("ldr   r1, [r1]");   /* r1 now points to "lo" word of timetick */

asm("retry:");
  asm("ldr   r3, [r1, #4]"); /* read "hi" word */
  asm("ldr   r2, [r1]    "); /* read "lo" word */
  asm("str   r2, [r0, #4]"); /* save "lo" word */
  asm("ldr   r2, [r1, #4]"); /* read "hi" word again */
  asm("cmp   r2, r3      "); /* if "hi" word rolled over while we dumped */
  asm("bne   retry       "); /*   we must retry */
  asm("str   r2, [r0, #8]"); /* save "hi" word */

  /* Store r4-r12 */
  asm("add   r0, r0, #28");  /* skip cookie, dump timestamp, and r0-r3 */
  asm("stmia r0!, {r4-r12}");

  /* Go back and get r0-r3 */
  asm("pop   {r2-r5}");
  asm("ldr   r1, =aop_core_dump"); /* get a fresh pointer to the core dump [r0 is just past registers, we need that later] */
  asm("add   r1, r1, #12");        /* move past cookie and timestamp to where r0-r3 should be stored */
  asm("stm   r1!, {r2-r5}");

  /* Grab the remaining registers. */
  asm("mrs   r1, MSP");
  asm("mrs   r2, PSP");
  asm("mov   r3, lr");
  asm("ldr   r4, =abort");
  asm("mrs   r5, XPSR");
  asm("mrs   r6, PRIMASK");
  asm("mrs   r7, FAULTMASK");
  asm("mrs   r8, BASEPRI");
  asm("mrs   r9, CONTROL");
  asm("stmia r0!, {r1-r9}");

  /* Grab and save all the auxiliary fault status registers */
  asm("ldr   r1, =0xE000ED24");
  asm("ldmia r1!, {r2-r8}");
  asm("stmia r0!, {r2-r8}");

  /* Add something useful to the log */
  asm("mov   r0, lr");
  asm("mrs   r1, IPSR");
  asm("push  {r0, lr}"); /* push lr so we can pop later and preserve the stack for the debugger. Need to push 8 bytes */
  asm("bl    err_fatal_sw_event");
  // asm("bl    rbcpr_core_dump");

#ifdef DDR_ABORT_REG_DUMP
  asm("bl    ddr_store_status_regs_error_fatal");
#endif

  // asm("bl    notify_apss");

  asm("bl    reset_chip");

  /* Do this last */
  asm("pop   {r0, lr}");

asm("hang:");
  asm("b     hang");
}
#endif /* __clang__ */

