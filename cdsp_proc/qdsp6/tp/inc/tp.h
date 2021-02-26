/* ========================================================================
   Thread Priority Ranges

   List of Priority Ranges for ADSP threads

   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
   All Rights Reserved. Qualcomm Proprietary and Confidential.
  ========================================================================= */

/* ========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ----------------------------------------------------
   04/01/13   weshmawy     Created file.
   ======================================================================= */

/*
 usage

 _HIGH  is the highest priority for the range
 _LOW   is the lowest priority for the range
 _NAME  string should be added to the name of the thread

*/

/* interrupt to signal conversion */
#define IST_HIGH 1
#define IST_LOW  5
#define IST_NAME "_IST"

/* rtos specific threads -- qurt timers */
#define RTOS_HIGH 1
#define RTOS_LOW  5
#define RTOS_NAME "_RTOS"

/* high priority timers such as AVT */
#define TIMERS_HP_HIGH 2
#define TIMERS_HP_LOW  9
#define TIMERS_HP_NAME "_TIMERS"

/* AVS threads with latency Requirements < 100us */
#define AVS_CRITICAL_PRIO_HIGH 10
#define AVS_CRITICAL_PRIO_LOW  29
#define AVS_CRITICAL_PRIO_NAME "_AVSCRIT"

/* AVS threads with latency Requirements < 1ms */
#define AVS_HIGH_PRIO_HIGH 30
#define AVS_HIGH_PRIO_LOW  49
#define AVS_HIGH_PRIO_NAME "_AVSHIGH"

/* AVS threads with latency Requirements < 5ms */
#define AVS_MEDIUM_PRIO_HIGH 50
#define AVS_MEDIUM_PRIO_LOW  69
#define AVS_MEDIUM_PRIO_NAME "_AVSMED"

/* AVS threads with latency Requirements < 10ms */
#define AVS_LOW_PRIO_HIGH 70
#define AVS_LOW_PRIO_LOW  89
#define AVS_LOW_PRIO_NAME "_AVSLOW"

/* AVS threads with latency Requirements > 10ms */
#define AVS_VERY_LOW_PRIO_HIGH 90
#define AVS_VERY_LOW_PRIO_LOW  109
#define AVS_VERY_LOW_PRIO_NAME "_AVSVLOW"

/* Low Priority TIMERS */
#define TIMERS_LP_HIGH 110
#define TIMERS_LP_LOW  119
#define TIMERS_LP_NAME "_TIMERSLP"

/* Communication threads APR, QMI and others */
#define COMM_THREADS_HIGH 120
#define COMM_THREADS_LOW  129
#define COMM_THREADS_NAME "_COMM"

/* HWIO related threads */
#define HWIO_THREADS_HIGH 130
#define HWIO_THREADS_LOW  139
#define HWIO_THREADS_NAME "_HWIO"

/* WLAN related threads */
#define WLAN_THREADS_HIGH 130
#define WLAN_THREADS_LOW  139
#define WLAN_THREADS_NAME "_WLAN"

/* LOC related threads */
#define LOC_THREADS_HIGH 130
#define LOC_THREADS_LOW  139
#define LOC_THREADS_NAME "_LOC"

/* SENSORS PROCESSING */
#define SENSORS_THREADS_HIGH 140
#define SENSORS_THREADS_LOW  169
#define SENSORS_THREADS_NAME "_SENSORS"

/* DAL WORKLOOP */
#define DAL_WORKLOOPS_HIGH 170
#define DAL_WORKLOOPS_LOW  179
#define DAL_WORKLOOPS_NAME "_DALWRK"

/* ADSPPM */
#define ADSPPM_THREADS_HIGH 180
#define ADSPPM_THREADS_LOW  189
#define ADSPPM_THREADS_NAME "_ADSPPM"

/* DIAG */
#define DIAG_THREADS_HIGH 240
#define DIAG_THREADS_LOW  249
#define DIAG_THREADS_NAME "_DIAG"

/* POWER */
#define POWER_THREADS_HIGH 250
#define POWER_THREADS_LOW 255
#define POWER_THREADS_NAME "_POWER"

/* TOUCH */
#define TOUCH_PRIO_HIGH 70
#define TOUCH_PRIO_LOW  89
#define TOUCH_PRIO_NAME "_TOUCH"











