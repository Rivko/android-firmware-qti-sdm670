/*========================================================================
Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/inc/avtimer_drv_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/02/12   fz     Created file.

========================================================================== */
/**
@file qurt_elite_hptimer.h

@brief This file contains utilities for AVtimers. One-shot, period and sleep
timers are provided.
*/

#ifndef AVTIMER_DRV_API_H
#define AVTIMER_DRV_API_H

#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


#define PRINT_AVTIMER_DRV_DBG_MSG
#undef PRINT_AVTIMER_DRV_DBG_MSG

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

//The below define is for normal avtimer use case
#define AVTIMER_DRV_OPEN_FLAG_DEFAULT_AVTIMER             0x00000000

//The below define is for 1st A_TIME hw usually which is used for pull-able PLL tweak
#define AVTIMER_DRV_OPEN_FLAG_ATIMER_1                    0x00000001

//The below define is for 2nd A_TIME hw usually which is used for pull-able PLL tweak
#define AVTIMER_DRV_OPEN_FLAG_ATIMER_2                    0x00000002


typedef void* avtimer_drv_handle_t;

/**
 * avtimer/avsync driver IOCtrl call property
 */
typedef enum _avtimer_drv_property_type
{
   AVTIMER_DRV_PROPERTY_UNDEFINED,
   AVTIMER_DRV_PROPERTY_ENABLE_AVSYNC_BT = 0x1,
   AVTIMER_DRV_PROPERTY_DISABLE_AVSYNC_BT,
   AVTIMER_DRV_PROPERTY_ENABLE_AVSYNC_FM,
   AVTIMER_DRV_PROPERTY_DISABLE_AVSYNC_FM,
   AVTIMER_DRV_PROPERTY_SET_DSTC_CAPTURE,
   AVTIMER_DRV_PROPERTY_GET_DSTC_CAPTURED,   //the tick got back is 64 bit
   AVTIMER_DRV_PROPERTY_MAX
}avtimer_drv_property_type;

/**
 * This is for DSTC (display system time tick) capture. It is for AVTIMER_DRV_OPEN_FLAG_PULLABLE_AVTIMER only
 * If video is on display 0 (DSTC0) then audio needs to sync with this video/display,
 * we need to set AVTIMER_DRV_CAPTURE_DSTC0 to capture the DSTC from from this display. 
 * It is usually 27 MHz.
 */
typedef enum _avtimer_drv_capture_dstc_type
{
   AVTIMER_DRV_CAPTURE_DSTC_UNDEFINED,
   AVTIMER_DRV_CAPTURE_DSTC0 = 0x1,
   AVTIMER_DRV_CAPTURE_DSTC1,
   AVTIMER_DRV_CAPTURE_DSTC2,
   AVTIMER_DRV_CAPTURE_DSTC3,
   AVTIMER_DRV_CAPTURE_DSTC_MAX
}avtimer_drv_capture_dstc_type;


/**
* avtimer/avsync driver IOCtl call property info struct.define 
*/
typedef struct 
{
    /** Property call type*/
    avtimer_drv_property_type  prop;       
    /** Data size of property call*/
    int32                  data_size;
    /** Data pointer of property call*/
    void                  *data_ptr;
    /** Reserved*/
    int32                  flags;             
} avtimer_drv_ioctl_prop_info_t;

/**
* avtimer hw open parameter struct.define 
*/
typedef struct _avtimer_open_param
{
   
   char *client_name;
   /** client name, max is 6 chars */
   
   uint32 flag;
   /** To represent which avtimer hardware is being used
      
    Supported Values:
    - #AVTIMER_DRV_OPEN_FLAG_DEFAULT_AVTIMER 
    - #AVTIMER_DRV_OPEN_FLAG_ATIMER_1 
    - #AVTIMER_DRV_OPEN_FLAG_ATIMER_2
     */
} avtimer_open_param_t;


/**
* for geting hw captured A_TIME, local STC, and V_TIME (DSTC) 
*/
typedef struct
{
    int64_t a_time;
    int64_t v_time;
    int64_t l_stc;
} avtimer_drv_captured_time_t;

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/**
Initialize avtimer. Must be called from the premordial thread before 
using the avtimer

@return
 ADSPResult error code.
*/
ADSPResult avtimer_drv_init(uint32 *p_hw_revision);

/**
Deinitialize avtimer. Must be called from the premordial thread 
before exiting the tests 

@return
 ADSPResult error code.
*/
ADSPResult avtimer_drv_deinit(void);

/****************************************************************************
** Enable/Disable avtimer hardware
*****************************************************************************/
/**
Enable avtimer hardware.

@param[in/out] avtimer_drv_handle_t *avtimer_h
@param[in] avtimer_open_param_t* open_param_ptr

@return
 An indication of success or failure.

@dependencies
 Client should call this function before using any avtimer features
*/
ADSPResult avtimer_drv_hw_open(avtimer_drv_handle_t *avtimer_h, avtimer_open_param_t* open_param_ptr);

/**
Disable avtimer hardware.

@param[in] avtimer_drv_handle_t avtimer_h

@return
 An indication of success or failure.

@dependencies
 Client should call this function if client does not use any avtimer feature
*/
ADSPResult avtimer_drv_hw_close(avtimer_drv_handle_t avtimer_h);

/*
Gets the avtimer current time since it was last reset.

@param[in] avtimer_drv_handle_t avtimer_h

@returns the current time since it was last reset in terms of microseconds
*/
uint64_t avtimer_drv_get_time(avtimer_drv_handle_t avtimer_h);

/*
latches both avtimer and qtimer together.

@param[in] avtimer_h - Handle to avtimer
@param[out] avtimer_ts - avtimer timestamp
@param[out] qtimer_ts - qtimer timestamp

@returns ADSPResult
*/
ADSPResult avtimer_drv_latch_avtimer_qtimer(avtimer_drv_handle_t avtimer_h, uint64_t *avtimer_ts, uint64_t *qtimer_ts);

/**
avtimer_drv_ioctl for hw ioctl

@param[in] avtimer_drv_handle_t avtimer_h
@param[in] avtimer_drv_ioctl_prop_info_t *prop_info_ptr

@return
 An indication of success or failure.

@dependencies
 Client should call avtimer_drv_hw_open first 
*/
ADSPResult avtimer_drv_ioctl(avtimer_drv_handle_t avtimer_h, avtimer_drv_ioctl_prop_info_t *prop_info_ptr);


/****************************************************************************
** Timers  -- the below APIs will be finally retired
*****************************************************************************/

/*
Timer types.
*/

/**
Deletes an existing timer.

@param[in] timer_ptr Timer to be deleted.

@return
 An indication of success or failure.

@dependencies
 The timer object must be created and initialized before calling this function.
*/
ADSPResult avtimer_drv_delete(qurt_elite_timer_t *timer_ptr);

/**
Gets the duration of the specified timer.

@param[in] timer_ptr Timer to be be queried.

@return
 Duration of the timer in microseconds.

@dependencies
 The timer object must be created and initialized before calling
 this function.
*/
uint64_t avtimer_drv_get_duration(qurt_elite_timer_t *timer_ptr);

/**
Creates a timer. The caller must allocate the memory for a timer strucure and
pass the pointer to this function. This step is necessary before arming the
timer.

After this step, call the appropriate start functions based on
the type of timer.

@param[in] timer_ptr Pointer to the QuRT_Elite timer object.
@param[in] timer_type One of the following:
                      - QURT_ELITE_TIMER_ONESHOT_DURATION
                      - QURT_ELITE_TIMER_PERIODIC
                      - QURT_ELITE_TIMER_ONESHOT_ABSOLUTE
@param[in] clock_source One of the following:
  - QURT_ELITE_TIMER_AVS \n
  @notel Only QURT_ELITE_TIMER_AVS is currently implemented.
@param[in] signal_ptr Pointer to the signal to be generated when the timer
                      expires.
@return
 An indication of success (0) or failure (nonzero).

@dependencies
 None.
*/

ADSPResult avtimer_drv_create(qurt_elite_timer_t *timer_ptr, qurt_elite_timer_duration_t timer_type, qurt_elite_signal_t *signal_ptr);

/**
Creates a synchronous sleep timer. Control returns to the callee
after the timer expires.

@param[in] llMicrosec Duration the timer will sleep (in 
      microseconds). if llMicrosec is less than HPTIMER_MIN_DURATION,
      then it will round it to HPTIMER_MIN_DURATION.

@return
 An indication of success (0) or failure (nonzero).

@dependencies
 None.
*/
ADSPResult avtimer_drv_sleep(uint64_t llMicrosec);

/**
Restarts the absolute one-shot timer.

@param[in] timer_ptr Pointer to the QuRT_Elite timer object.
@param[in] time Absolute time of the timer.

@return
 An indication of success (0) or failure (nonzero).

@dependencies
 Before calling this function, ensure that the timer was created using the
 qurt_elite_timer_create() function.
*/



ADSPResult avtimer_drv_start_absolute(qurt_elite_timer_t *timer_ptr, uint64_t time);


/**
Restarts a duration-based one-shot timer.

@param[in] timer_ptr Pointer to the QuRT_Elite timer object.
@param[in] duration  Duration of the timer in @latexonly 
      $\mu$@endlatexonly s. If duration is less than
      AVTIMER_DRV_MIN_DURATION, then duration is rounded to
      AVTIMER_DRV_MIN_DURATION.

@return
 An indication of success (0) or failure (nonzero).

@dependencies
 Before calling this function, ensure that the timer was created using the
 qurt_elite_timer_create() function.
*/

ADSPResult avtimer_drv_start_duration(qurt_elite_timer_t *timer_ptr, uint64_t duration);


/**
Starts a periodic timer.

@param[in] timer_ptr Pointer to the QuRT_Elite timer object.
@param[in] duration Duration of the timer in @latexonly $\mu$@endlatexonly s.

@return
 An indication of success (0) or failure (nonzero).

@dependencies
 Before calling this function, ensure that the timer was created using the
 qurt_elite_timer_create() function.
*/


ADSPResult avtimer_drv_start_periodic(qurt_elite_timer_t *timer_ptr, uint64_t duration);


/**
Cancels a one-shot timer.

@param[in] timer_ptr Pointer to the QuRT_Elite timer object.

@return
 An indication of success (0) or failure (nonzero).

@dependencies
 Before calling this function, ensure that the timer was created using the
 qurt_elite_timer_create() function.
*/

ADSPResult avtimer_drv_cancel_oneshot(qurt_elite_timer_t *timer_ptr);

/*
Convert the avtimer tick count to time in us

@returns the time for the hw tick count
*/
uint64_t avtimer_drv_convert_hw_tick_to_time(uint64_t tick_count);
   

#if defined(PRINT_AVTIMER_DRV_DBG_MSG)
void avtimer_server_dump_dbg_msg(void);   
#endif //#if defined(PRINT_AVTIMER_DRV_DBG_MSG)

/** @} */ /* end_addtogroup qurt_elite_hptimer */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_DRV_API_H

