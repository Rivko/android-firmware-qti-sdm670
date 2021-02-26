/***********************************************************************
 * fs_osal.h
 *
 * This is a thin OS abstraction layer.
 * Copyright (C) 2009-2013 QUALCOMM Technologies, Inc.
 *
 * Using this file allows us to compile EFS/HFAT code without any sort
 * of underlying operating system (REX in this case). This situation
 * arises when we are compiling code into qcsbl or oemsbl.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-02-08   wek   Fix type definitions when compiling with out rex.
2013-01-22   nr    Add fucntion to get current task name.
2012-03-20   wek   Cleanup customer header file inclusion.
2012-02-29   rp    Make Tasks free-float (CPU affinity free)
2011-11-03   wek   Remove dereference to task TCB.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-06-12   ebb   Create

===========================================================================*/

#ifndef __FS_OSAL_H__
#define __FS_OSAL_H__

#include "fs_config_i.h"
#include "comdef.h"

/* Invalid Dog-report id; dog.h does not provide a #define for this. */
#define FS_DOG_INVALID_RPT        (-1)

/* REX not defined, stub everything out */
#ifdef FEATURE_EFS_COMPILE_WITHOUT_REX

  /* These two fields must be present because they are getting dereferenced
   * in our code */
  typedef struct
  {
    int     err_num;
    uint32  dog_report_val;
  } fs_os_tcb_type;

  typedef uint32               fs_os_timer_type;
  typedef uint32               fs_os_timer_cnt_type;
  typedef uint32               fs_os_sigs_type;
  typedef uint32               fs_os_crit_sect_type;
  typedef uint32               fs_os_stack_size_type;
  typedef uint32               fs_os_priority_type;
  typedef void                 (*fs_os_task_func_type) (unsigned long);

  /* Needed to maintain the notion of time for unit tests */
  #ifdef FS_UNIT_TEST
    extern uint64 unit_test_time;
  #endif

  fs_os_tcb_type* fs_os_self (void);

  /* Function Stubs */
  static inline fs_os_sigs_type fs_os_clr_sigs (fs_os_tcb_type *tcb,
                                              fs_os_sigs_type sigs)
  {
    (void)tcb; (void)sigs;
    return 0;
  }

  static inline fs_os_sigs_type fs_os_set_sigs (fs_os_tcb_type *tcb,
                                                fs_os_sigs_type sigs)
  {
    (void)tcb; (void)sigs;
    return 0;
  }

  static inline void fs_os_def_timer (fs_os_timer_type *tmr,
                                      fs_os_tcb_type *tcb,
                                      fs_os_sigs_type sigs)
  {
    (void)tmr; (void)tcb; (void)sigs;
  }

  static inline fs_os_timer_cnt_type fs_os_set_timer (fs_os_timer_type *tmr,
                                                      fs_os_timer_cnt_type ms)
  {
    (void)tmr; (void)ms;
    return 0;
  }

  static inline fs_os_timer_cnt_type fs_os_clr_timer (fs_os_timer_type *tmr)
  {
    (void)tmr;
    return 0;
  }

  static inline fs_os_sigs_type fs_os_wait (fs_os_sigs_type sigs)
  {
    (void)sigs;
    return 0;
  }

  static inline fs_os_sigs_type fs_os_timed_wait(fs_os_sigs_type sigs,
                                                 fs_os_timer_type *tmr,
                                                 fs_os_timer_cnt_type cnt)
  {
    (void)sigs; (void)tmr; (void)cnt;

    #ifdef FS_UNIT_TEST
      unit_test_time += cnt;
    #endif
    return 0;
  }

  static inline int fs_os_autodog_enable (int val)
  {
    (void)val;
    return 0;
  }

  static inline int fs_os_autodog_enable_self (void)
  {
    return 0;
  }

  static inline int fs_os_autodog_disable (void)
  {
    return 0;
  }

  static inline void fs_os_init_crit_sect (fs_os_crit_sect_type *cs)
  {
    (void)cs;
  }

  static inline void fs_os_enter_crit_sect (fs_os_crit_sect_type *cs)
  {
    (void)cs;
  }

  static inline void fs_os_leave_crit_sect (fs_os_crit_sect_type *cs)
  {
    (void)cs;
  }

  static inline void fs_os_set_dac (fs_os_tcb_type *tcb, unsigned int dac)
  {
    (void)tcb; (void)dac;
  }

  static inline void fs_os_def_task_ext (fs_os_tcb_type *tcb,
                                        unsigned char *stk,
                                        fs_os_stack_size_type stksz,
                                        fs_os_priority_type pri,
                                        fs_os_task_func_type tsk,
                                        dword p,
                                        char *nme,
                                        boolean s,
                                        int dog_report_val)
  {
    (void)tcb; (void)stk; (void)stksz; (void)tsk;
    (void)pri; (void)p; (void)nme; (void)s; (void) dog_report_val;
  }

  static inline int
  fs_os_get_task_name(fs_os_tcb_type *p_tcb, char *dest, unsigned int size,
                      unsigned int *p_len)
  {
    (void) p_tcb;
    (void) dest;
    (void) size;

    /* Set the len to zero so any potential check on this would be correctly
     * handled. */
    *p_len = 0;
    return 0;
  }

#else /* REX is defined, redirect all calls to the REX API */

  /* Include rex header */
  #include "rex.h"

  /* Data types */
  typedef rex_tcb_type         fs_os_tcb_type;
  typedef rex_timer_type       fs_os_timer_type;
  typedef rex_timer_cnt_type   fs_os_timer_cnt_type;
  typedef rex_sigs_type        fs_os_sigs_type;
  typedef rex_crit_sect_type   fs_os_crit_sect_type;

  /* Functions */
  #define fs_os_clr_sigs(tcb,sigs)       rex_clr_sigs(tcb,sigs)
  #define fs_os_set_sigs(tcb,sigs)       rex_set_sigs(tcb,sigs)
  #define fs_os_def_timer(tmr,tcb,sigs)  rex_def_timer(tmr,tcb,sigs)
  #define fs_os_set_timer(tmr,msecs)     rex_set_timer(tmr,msecs)
  #define fs_os_clr_timer(tmr)           rex_clr_timer(tmr)
  #define fs_os_wait(sigs)               rex_wait(sigs)
  #define fs_os_timed_wait(sigs,tmr,cnt) rex_timed_wait(sigs,tmr,cnt)
  #define fs_os_self()                   rex_self()
  #define fs_os_autodog_enable(val)      rex_autodog_enable(val)
  #define fs_os_autodog_enable_self()    rex_autodog_enable_self()
  #define fs_os_autodog_disable()        rex_autodog_disable()
  #define fs_os_init_crit_sect(cs)       rex_init_crit_sect(cs)
  #define fs_os_enter_crit_sect(cs)      rex_enter_crit_sect(cs)
  #define fs_os_leave_crit_sect(cs)      rex_leave_crit_sect(cs)
  #define fs_os_set_dac(tcb,dac)         rex_set_dac(tcb,dac)

  #define fs_os_get_task_name(tcb,dest_buffer,buffer_size,len) \
                             rex_get_task_name(tcb,dest_buffer,buffer_size,len)
  /* On Q6-processor */
#ifdef FEATURE_QDSP6
  #define fs_os_def_task_ext(tcb,stk,stksz,pri,tsk,p,nme,s,dog_val)   \
            rex_def_task_ext5(REX_ANY_CPU_AFFINITY_MASK, tcb,stk,     \
                              stksz,pri,tsk,p,nme,s,dog_val)
#else
  #define fs_os_def_task_ext(tcb,stk,stksz,pri,tsk,p,nme,s,dog_val)   \
            rex_def_task_ext(tcb,stk,stksz,pri,tsk,p,nme,s)
#endif


#endif /* FEATURE_EFS_COMPILE_WITHOUT_REX */

#endif /* not __FS_OSAL_H__ */
