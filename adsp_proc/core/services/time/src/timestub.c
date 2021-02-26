/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using various sources.


EXTERNALIZED FUNCTIONS
  time_get( )
    Retrieves time according to CDMA, HDR, GSM, or slow-clock Time-of-Day

REGIONAL FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2003 - 2018 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/time/src/timestub.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/05/13   abh     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"
#include "qurt.h"
#include "timetick_sclk64.h"
#include "time_genoff.h"
#include "err.h"
#include "HALhwio.h"
#include "msmhwiobase.h"
#include "DALSys.h"

#ifdef TIME_PERIPHERAL_ADSP
#include "timer_HWIO_QTMR_adsp.h"
#elif TIME_PERIPHERAL_CDSP
#include "timer_HWIO_QTMR_cdsp.h"
#elif TIME_PERIPHERAL_SLPI
#include "timer_HWIO_QTMR_slpi.h"
#endif
/*=============================================================================

                           MACROS

=============================================================================*/

#define TIME_MODEM_PROC_ID              0
#define TIME_LPASS_PROC_ID              1
#define TIME_CNSS_PROC_ID               2
#define TIME_SENSOR_PROC_ID             3
#define TIME_LAST_PROC_ID               TIME_SENSOR_PROC_ID
#define TIME_APPS_PROC_ID               (TIME_LAST_PROC_ID + 1)


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
#if !defined(TIME_SENSOR_PD) && !defined(TIME_AUDIO_PD)
static volatile unsigned long qtimer_register_dump[0x13];
#endif
uint64 QTIMER_OFFSET;
/*-----------------------------------------------------------------------------

                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIME_GET

DESCRIPTION
  Returns the current time

DEPENDENCIES
  A valid CDMA time, HDR time, GSM time, or a Time-of-Day, etc.

RETURN VALUE
  Time-stamp returned to ts_val parameter.
  Time source used to determine time is returned

SIDE EFFECTS
  None

=============================================================================*/

time_source_enum_type time_get
(
  /* OUT: The current time */
  time_type                       ts_val
)
{
	unsigned long long time_in_ms = 0;
	unsigned long long ticks = 0;
	
	/* Non Blocking Api to get the ticks */
	ticks = qurt_sysclock_get_hw_ticks();
    /*Get time in micro sec*/
	time_in_ms = qurt_sysclock_timetick_to_us(ticks);
    /* Divided by 1000 to convert to milli sec*/
	time_in_ms = ((time_in_ms*1073742)>>30);
	time_in_ms = time_in_ms *4;
	time_in_ms = time_in_ms/5;
	time_in_ms = time_in_ms<<16;
	ts_val[0] = time_in_ms;
	ts_val[1] = (time_in_ms>>32);

    return TIME_SOURCE_32KHZ;

} /* time_get */

/*=============================================================================

FUNCTION TIME_GET_SUM_GENOFF_TS

DESCRIPTION
  Returns sum generic offset value of system base in timestamp format

DEPENDENCIES
  None

RETURN VALUE
  boolean as return value
  Provides sum generic ts offset value as part of inputs arguments

SIDE EFFECTS
  None

=============================================================================*/
boolean time_get_sum_genoff_ts
(
    time_type        sum_genoff_ts_val
)
{
  qw_set( sum_genoff_ts_val, 0, 0 );	
  return FALSE;
} /*  time_get_sum_genoff_ts */

/*=============================================================================

FUNCTION TIME_GET_UPTIME

DESCRIPTION
  Returns the uptime in timestamp format

DEPENDENCIES
  
RETURN VALUE
  Source of the time

SIDE EFFECTS
  None

=============================================================================*/
time_source_enum_type time_get_uptime
(
  /* OUT: The current time */
  time_type                       ts_val
)
{
  qw_set( ts_val, 0, 0 );
  
  /* Get uptime from sclk_time */
  time_sclk_get( ts_val, (uint64)-1);

  return (time_source_enum_type)(TIME_LPASS_PROC_ID<<4 | TIME_SOURCE_32KHZ);
} /* time_get_uptime */

#if !defined(TIME_SENSOR_PD) && !defined(TIME_AUDIO_PD)
void dump_qtimer_registers(void)
{
	   // Dump registers
	   
       qtimer_register_dump[0x00] = 0xABCDABCD;
       qtimer_register_dump[0x01] = HWIO_QTMR_CNTPCT_LO_IN;
       qtimer_register_dump[0x02] = HWIO_QTMR_CNTPCT_HI_IN;
       qtimer_register_dump[0x03] = HWIO_QTMR_CNTVCT_LO_IN;
       qtimer_register_dump[0x04] = HWIO_QTMR_CNTVCT_HI_IN;
       qtimer_register_dump[0x05] = HWIO_QTMR_CNTFRQ_IN;
       qtimer_register_dump[0x06] = HWIO_QTMR_CNTPL0AC_IN;
       qtimer_register_dump[0x07] = HWIO_QTMR_CNTVOFF_LO_IN;
       qtimer_register_dump[0x08] = HWIO_QTMR_CNTVOFF_HI_IN;
       qtimer_register_dump[0x09] = HWIO_QTMR_CNTP_CVAL_LO_IN;
       qtimer_register_dump[0x0A] = HWIO_QTMR_CNTP_CVAL_HI_IN;
       qtimer_register_dump[0x0B] = HWIO_QTMR_CNTP_TVAL_IN;
       qtimer_register_dump[0x0C] = HWIO_QTMR_CNTP_CTL_IN;
       qtimer_register_dump[0x0D] = HWIO_QTMR_CNTV_CVAL_LO_IN;
       qtimer_register_dump[0x0E] = HWIO_QTMR_CNTV_CVAL_HI_IN;
       qtimer_register_dump[0x0F] = HWIO_QTMR_CNTV_TVAL_IN;
       qtimer_register_dump[0x10] = HWIO_QTMR_CNTV_CTL_IN;
       qtimer_register_dump[0x11] = HWIO_QTMR_VERSION_IN;
       qtimer_register_dump[0x12] = 0xABCDABCD;
	   
}
#endif

/*=============================================================================

FUNCTION TIME_INIT

DESCRIPTION
  Initializes time dependencies

=============================================================================*/
void time_init( void )
{
#if !defined(TIME_SENSOR_PD) && !defined(TIME_AUDIO_PD)
   static DALSYSPropertyVar system_property_var;
   static DALSYS_PROPERTY_HANDLE_DECLARE(qtimer_handle);

   /* Get the propery handle to timetick DAL */
   DALSYS_GetDALPropertyHandleStr("SystemTimer",qtimer_handle);
   DALSYS_GetPropertyValue(qtimer_handle, "QTIMER_OFFSET", 0,&system_property_var);
   
   QTIMER_OFFSET = system_property_var.Val.dwVal;
   
   err_crash_cb_register(dump_qtimer_registers);
#endif
   timetick_sclk64_init();
}
