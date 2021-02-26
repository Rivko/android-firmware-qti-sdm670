#ifndef QURT_SCLK_H
#define QURT_SCLK_H
/**
  @file qurt_sclk.h 
  @brief  The header file that describes the APIs supported by QURT system sclk
   feature.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/




/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 Conversion from microseconds to sleep ticks.
 */
#define QURT_SYSCLOCK_TIMETICK_FROM_US(us) (us * 192ull / 10ul)
#define qurt_sysclock_timetick_from_us(us) QURT_SYSCLOCK_TIMETICK_FROM_US(us)
/*
 Conversion from timer ticks to microseconds at the nominal frequency.
*/
#define QURT_SYSCLOCK_TIMETICK_TO_US(ticks) qurt_timer_timetick_to_us(ticks)

/*
  The maximum microseconds value for Qtimer is 1042499 hours.
*/
#define QURT_SYSCLOCK_MAX_DURATION (1042499uLL * 3600uLL * 1000uLL * 1000uLL)
#define qurt_sysclock_max_duration() QURT_SYSCLOCK_MAX_DURATION
/* 
 The timer clock for Qtimer is 19.2MHz.
*/
#define QURT_SYSCLOCK_MAX_DURATION_TICKS (1042499uLL * 3600uLL * 19200000uLL)
#define qurt_sysclock_max_duration_ticks() QURT_SYSCLOCK_MAX_DURATION_TICKS
/* 
 The sleep timer error margin for Qtimer is 192 ticks ~10us.
*/
#define QURT_SYSCLOCK_ERROR_MARGIN 192 //QURT_TIMER_MIN_DURATION*timer_freq;
#define qurt_sysclock_error_margin() QURT_SYSCLOCK_ERROR_MARGIN
/*=============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/**@ingroup func_qurt_sysclock_register
  @xreflabel{sec:qurt_sysclock_register}
  Register a signal object to receive an event on a system clock alarm or timer.

  The return value is a client identifier value, which is used to associate a registered signal
  with a system clock alarm or timer object.

  @datatypes
  #qurt_anysignal_t

  @param[in]   signal       Signal object set when the system clock event occurs.
  @param[in]   signal_mask  Signal mask, which specifies the signal to set in the signal object when
                            the clock event occurs. 
 
  @return 
  Integer --  System clock client identifier. \n
  #QURT_EFATAL -- Not enough memory to create timer.
    
  @dependencies
  None.
 */

extern int qurt_sysclock_register (qurt_anysignal_t *signal, unsigned int signal_mask);

/**@ingroup func_qurt_sysclock_alarm_create
  Creates a system clock alarm with the specified time value, and starts the alarm.

  @param[in]   id       System clock client ID; indirectly indicating the signal that the 
                        alarm-expired event sets. The signal must already be 
                        registered to receive a system clock event 
                        (Section @xref{sec:qurt_sysclock_register}) -- it is 
                        specified here by the client identifier that is returned by the 
                        register operation.
  @param[in]  ref_count   System clock count when the match value was calculated, which specifies the 
                        system clock time when the match_value parameter is calculated. 
                        This value is obtained using the get hardware ticks operation 
                        (Section @xref{sec:qurt_sysclock_get_hw_ticks}).
  @param[in] match_value  Match value to be programmed in system clock hardware; indicates 
                        the system clock time (in system clock ticks) when the alarm
                        should expire.
 
  @return  
  Integer -- Match value programmed.

  @dependencies
  None.             
 */

extern unsigned long long qurt_sysclock_alarm_create (int id, unsigned long long ref_count, unsigned long long match_value);

/**@ingroup func_qurt_sysclock_timer_create
  Creates a system clock timer with the specified duration, and starts the timer.
    
  @param[in]  id          System clock client ID. Indirectly specifies the signal that the timer-expired 
                          event sets. The signal must already be registered to receive a system clock event 
                          (Section @xref{sec:qurt_sysclock_register}) -- the client 
                          identifier returned by the register operation specifies the signal.
  @param[in]   duration   Timer duration (in system clock ticks). Specifies the interval between the 
                          creation of the system clock timer object and the generation of the corresponding 
                          timer event.
                           
  @return  
  #QURT_EOK -- Timer successfully created.
 
  @dependencies
  None.
 */

extern int qurt_sysclock_timer_create (int id, unsigned long long duration);

/**@ingroup func_qurt_sysclock_get_expiry
  Gets the duration until next timer event.\n
  Returns the number of system ticks that elapse before the next timer event occurs on
  any active timer in the QuRT application system.

  A system tick is defined as one cycle of the Hexagon processor's 19.2 MHz Qtimer clock.
 
  @return 
  Integer -- Number of system ticks until next timer event.

  @dependencies
  None.
 */

extern unsigned long long qurt_sysclock_get_expiry (void);

/**@ingroup func_qurt_sysclock_get_hw_ticks
  @xreflabel{sec:qurt_sysclock_get_hw_ticks}
  Gets the hardware tick count.\n
  Returns the current value of a 64-bit hardware counter. The value wraps around to zero
  when it exceeds the maximum value. 

  @note1hang This operation must be used with care because of the wrap-around behavior.
 
  @return 
  Integer -- Current value of 64-bit hardware counter. 

  @dependencies
  None.
 */
unsigned long long qurt_sysclock_get_hw_ticks (void);


/**@ingroup func_qurt_sysclock_get_hw_ticks_32
  @xreflabel{sec:qurt_sysclock_get_hw_ticks_32}
  Gets the hardware tick count in 32 bits.\n
  Returns the current value of a 32-bit hardware counter. The value wraps around to zero
  when it exceeds the maximum value. 

  @note1hang This operation is implemented as an inline C function, and should be called from a C/C++ program.
             The returned 32 bits are the lower 32 bits of the Qtimer counter.
 
  @return 
  Integer -- Current value of the 32-bit timer counter. 

  @dependencies
  None.
 */
extern int qurt_timer_base __attribute__((section(".data.qurt_timer_base")));
static inline unsigned long qurt_sysclock_get_hw_ticks_32 (void)
{
    #if defined (CONFIG_HEXAGON_ARCH_V61) || __HEXAGON_ARCH__ >= 61
       #ifdef CONFIG_V62_QTIMER_WORKAROUND
          return  (volatile unsigned long)(*((unsigned long *)((unsigned int)qurt_timer_base+0x1000)));  //QTMR_V1_CNTPCT_LO 
       #else
          //Beginning with v61 there is a HW register that can be read directly.
          unsigned long count;
          __asm__ __volatile__ (" %0 = c30 " : "=r"(count));
          return count;
       #endif
    #else
       return  (volatile unsigned long)(*((unsigned long *)((unsigned int)qurt_timer_base+0x1000)));  //QTMR_V1_CNTPCT_LO 
    #endif
}


/**@ingroup func_qurt_sysclock_get_hw_ticks_16
  @xreflabel{sec:qurt_sysclock_get_hw_ticks_16}
  Gets the hardware tick count in 16 bits.\n
  Returns the current value of a 16-bit timer counter. The value wraps around to zero
  when it exceeds the maximum value. 

  @note1hang This operation is implemented as an inline C function, and should be called from a C/C++ program.
             The returned 16 bits are based on the value of the lower 32 bits in Qtimer 
             counter, right shifted by 16 bits.
 
  @return 
  Integer -- Current value of the 16-bit timer counter, calculated from the lower 32 bits in the
             Qtimer counter, right shifted by 16 bits. 

  @dependencies
  None.
 */


static inline unsigned short qurt_sysclock_get_hw_ticks_16 (void)
{
    unsigned long ticks;

    #if defined (CONFIG_HEXAGON_ARCH_V61) || __HEXAGON_ARCH__ >= 61
       #ifdef CONFIG_V62_QTIMER_WORKAROUND
          ticks = (volatile unsigned long)(*((unsigned long *)((unsigned int)qurt_timer_base+0x1000))); //QTMR_V1_CNTPCT_LO 
       #else
          //Beginning with v61 there is a HW register that can be read directly.
          __asm__ __volatile__ (" %0 = c30 " : "=r"(ticks));
       #endif
    #else    
    ticks = (volatile unsigned long)(*((unsigned long *)((unsigned int)qurt_timer_base+0x1000))); //QTMR_V1_CNTPCT_LO 
    #endif
    __asm__ __volatile__ ( "%0 = lsr(%0, #16) \n" :"+r"(ticks));

    return (unsigned short)ticks; 
}


unsigned long long qurt_timer_timetick_to_us(unsigned long long ticks);
#define qurt_sysclock_timetick_to_us(ticks) qurt_timer_timetick_to_us(ticks)

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif /* __cplusplus */

#endif /* QURT_SCLK_H */
