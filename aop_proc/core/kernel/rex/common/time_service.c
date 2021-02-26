#include "aop_hwio.h"
#include "mpm_ctrl_hwio.h"
#include "HALhwio.h"
#include "CoreVerify.h"
//#include "swevent.h"
#include "aoss_cc_hwio.h"

#define BUSYWAIT_XO_FREQUENCY_IN_KHZ 19200

void time_service_init(void)
{
  HWIO_OUT(MPM_CONTROL_CNTCR,1);
  //allow read/write access to timer registers
  //TODO: make this write more obvious/indicative of whats going on
  HWIO_OUTI(AOP_QTMR_AC_CNTACRn_FG0, 0, 33);

  //enable RPM QTMR
  HWIO_OUTF(AOP_F0_QTMR_V1_CNTP_CTL, EN, 1);
}

static void qtimer_zero(void)
{
  uint32 cc_reset = 0;	
  //read CC register, if !0, log.
  cc_reset = HWIO_IN(AOSS_CC_RESET_STATUS);
  if(cc_reset != 0)
  {
    //SWEVENT(GCC_RESET, gcc_reset);
  }	  
  CORE_VERIFY(0);
}

uint64_t time_service_now(void)
{
    uint32_t first_hi, second_hi, lo;

    do
    {
        first_hi = HWIO_IN(AOP_F0_QTMR_V1_CNTPCT_HI);
        __schedule_barrier();

        lo = HWIO_IN(AOP_F0_QTMR_V1_CNTPCT_LO);
        __schedule_barrier();

        second_hi = HWIO_IN(AOP_F0_QTMR_V1_CNTPCT_HI);
        __schedule_barrier();
    } while(first_hi != second_hi);

    if((!second_hi) && (!lo))
    {
      qtimer_zero();
    }
    return (((uint64_t)second_hi) << 32) | lo;
}

void busywait (uint32 pause_time_us)
{
  uint32 start_count = 0;
  uint64 delay_count = 0;

  start_count = HWIO_IN(AOP_F0_QTMR_V1_CNTPCT_LO);
  delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ) / 1000;
  while ((HWIO_IN(AOP_F0_QTMR_V1_CNTPCT_LO) - start_count) < delay_count);
} 

