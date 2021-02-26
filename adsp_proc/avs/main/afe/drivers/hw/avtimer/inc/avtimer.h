/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/inc/avtimer.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.
========================================================================== */
/**
@file avtimer.h

@brief This file contains avtimer structs that is required for the server and client
*/

#ifndef AVTIMER_H
#define AVTIMER_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "adsp_error_codes.h"
#include "qurt_elite.h"
#include "qurt_signal2.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/**
 * minimum duration in micro secs that is allowed
 */
#define AVTIMER_MIN_DURATION             ((uint64_t)(10))

/**
 * avtimer max duration (in micro secs)= 8.9 years (1<<48 -1) 
 * that is allowed 
 */
//ToDO remove the cosim hack
#define AVTIMER_MAX_DURATION             ((uint64_t)281474976710655ull)
//#define AVTIMER_MAX_DURATION             ((uint64_t)2147450879ull)
//#define AVTIMER_MAX_DURATION             ((uint64_t)4294967295ull)


/**
 * avtimer_t timer types                                                           
 */
typedef uint32_t    avtimer_t;

/**
 * avtimer_duration_t types                                     
 */
typedef uint64_t    avtimer_duration_t;

/**
 * avtimer_time_t types                                         
 */
typedef uint64_t    avtimer_time_t;

/**
 * avtimer_type types                         
 */
typedef enum
{
    AVTIMER_ONESHOT_DURATION=0,
    AVTIMER_ONESHOT_ABSOLUTE,
    AVTIMER_PERIODIC
} avtimer_type_t;

/**
 * avtimer_attr_t types                                                      
 */
typedef struct 
{
    uint32_t    state;
    /**< current state of sw timer */
                    
    avtimer_duration_t   duration; 
    /**< This attribute specifies the duration of new timer. */ 

    avtimer_time_t   expiry; 
    /**< This attribute specifies the absolute expiry of new timer. */ 

    avtimer_duration_t   remaining; 
    /**< This attribute specifies the remaining time of an active timer. */ 

    avtimer_type_t       type;
    /**< This attribute specifies the timer type, only 
        AVTIMER_ONESHOT_ABSOLUTE, AVTIMER_ONESHOT_DURATION and 
        AVTIMER_PERIODIC are currently supported */

}
avtimer_attr_t;

/**
 * avtimer_callback_t types                                                      
 */
typedef struct avtimer_callback {

    qurt_elite_channel_t  *cb_channel_ptr;
    /**< The signal pointer the client is waiting on.*/

    uint32_t mask;
    /**< The signal mask the client is waiting on. */

    qurt_elite_signal_t cb_signal;
    /**< form a qurt_elite_signal via cb_channel_ptr and mask.*/
}
avtimer_callback_t;

/**
 * avtimer_cmd_create_t types                                                      
 */
typedef struct avtimer_cmd_create {

    avtimer_t          timer;
    /**< Timer ID */

    qurt_signal2_t *ack_ptr;
    /**< Signal the client is waiting on for acknowledgement */

    ADSPResult     result;
    /**< Indicate SUCCESS or FAILURE by the avtimer server */

    avtimer_type_t     type;
    /**< Timer type */

    avtimer_duration_t  duration;
    /**< Duration of the timer */

    avtimer_time_t  expiry;
    /**< Expiry of the timer */

    avtimer_callback_t cb;
    /**< Callback type and the relevent information needed for the callback
        from Server to Client to indicate the timer expiry */


}
avtimer_cmd_create_t;

/**
 * avtimer_cmd_delete_t types                                   
 */
typedef struct avtimer_cmd_delete {

    avtimer_t          timer;
    /**< Timer ID */

    qurt_signal2_t *ack_ptr;
    /**< Signal the client is waiting on for acknowledgement */

    ADSPResult     result;
    /**< Indicate SUCCESS or FAILURE by the avtimer server */

}
avtimer_cmd_delete_t;

/**
 * avtimer_cmd_getattr_t types                                                      
 */
typedef struct avtimer_cmd_getattr {

    avtimer_t          timer;
    /**< Timer ID */

    qurt_signal2_t *ack_ptr;
    /**< Signal the client is waiting on for acknowledgement */

    ADSPResult     result;
    /**< Indicate SUCCESS or FAILURE by the avtimer server */

    avtimer_attr_t     *attr_ptr;
    /**< Pointer to the attribute structure timer server has to update */

}
avtimer_cmd_getattr_t;

/**
 * avtimer_cmd_stop_t types                                    
 */
typedef struct avtimer_cmd_stop {

    avtimer_t          timer;
    /**< Timer ID */

    qurt_signal2_t *ack_ptr;
    /**< Signal the client is waiting on for acknowledgement */

    ADSPResult     result;
    /**< Indicate SUCCESS or FAILURE by the avtimer server */
}
avtimer_cmd_stop_t;

/**
 * avtimer_cmd_restart_t types                                  
 */
typedef struct avtimer_cmd_restart {

    avtimer_t          timer;
    /**< Timer ID */

    qurt_signal2_t *ack_ptr;
    /**< Signal the client is waiting on for acknowledgement */

    ADSPResult     result;
    /**< Indicate SUCCESS or FAILURE by the avtimer server */

    avtimer_duration_t  duration;
    /**< Duration of the timer*/

}
avtimer_cmd_restart_t;

/**
 * avtimer_cmd_any_t types                                   
 */
typedef struct avtimer_cmd_any {

    avtimer_t          timer;
    /**< Timer ID */

    qurt_signal2_t *ack_ptr;
    /**< Signal the client is waiting on for acknowledgement */

    ADSPResult     result;
    /**< Indicate SUCCESS or FAILURE by the avtimer server */

}
avtimer_cmd_any_t;



/**
 * avtimer_msg_t types                                                      
 */
typedef union {
    uint64_t msg;
    struct {

        uint32_t msg_type;
        /**< message type to indicate the type of command that follows */

        union {
            /**
             * Create timer command
             */
            avtimer_cmd_create_t   *create_cmd_ptr;
            /**
             * Stop timer command
             */
            avtimer_cmd_stop_t   *stop_cmd_ptr;
            /**
             * Delete timer command
             */
            avtimer_cmd_delete_t   *delete_cmd_ptr;
            /**
             * Get attribute command
             */
            avtimer_cmd_getattr_t  *getattr_cmd_ptr;
            /**
             * restart attribute command
             */
            avtimer_cmd_restart_t *restart_cmd_ptr;
            /**
             * any command attribute command
             */
            avtimer_cmd_any_t *any_cmd_ptr;

        };
    };
}
avtimer_msg_t;

/*****************************************************************************/
/*                                                                           */
/*                      FUNCTION DECLARATIONS                                */
/*                                                                           */
/*****************************************************************************/

/**
 * Creates a timer with specific attributes
 *
 * @param[out] timer_ptr   Timer object
 * @param[in/out] attr_ptr Specifies timer duration and timer type (
 *                     one-shot (abs / duration) or periodic)
 * @param[in] cb_signal_ptr   Signal sent when timer expires
 * @param[in] mask     Signal mask to be sent when timer expires
 * @return             one of ADSPResult error codes
 */
ADSPResult avtimer_timer_create (avtimer_t *timer_ptr, const avtimer_attr_t *attr_ptr,
                 qurt_elite_channel_t *cb_channel_ptr, uint32_t mask);



/**
 * Stop a one-shot timer. Applicable only to one shot timer 
 * only. 
 *
 * @param[in] timer    Timer ID. 
 * @return             one of ADSPResult error codes
 */
ADSPResult avtimer_timer_stop (avtimer_t timer);


/**
 * Restart a one-shot timer with a duration. Applicable only to 
 * one shot timer only 
 *
 * @param[in] timer        Timer ID. 
 * @param[in] duration     timer duration(absolute or duration) 
 */
ADSPResult avtimer_timer_restart (avtimer_t timer, avtimer_duration_t duration);

/**
 * Initialize timer attributes
 *
 * @param[out] attr_ptr  Timer attributes object
 */
void avtimer_attr_init(avtimer_attr_t *attr_ptr);

/**
 * Set expiry
 *
 * @param[out] attr_ptr      Timer attributes object
 * @param[in] time       Absolute expiry time for the timer in 
 *       micro-seconds
 */
void avtimer_attr_setexpiry(avtimer_attr_t *attr_ptr, avtimer_time_t time);


/**
 * Set duration
 *
 * @param[out] attr_ptr      Timer attributes object
 * @param[in] duration   Duration for the timer
 */
void avtimer_attr_setduration(avtimer_attr_t *attr_ptr, avtimer_duration_t duration);


/**
 * Get duration
 *
 * @param[in] attr_ptr      Timer attributes object
 * @param[out] duration_ptr  Duration for the timer
 */
void avtimer_attr_getduration(avtimer_attr_t *attr_ptr, avtimer_duration_t *duration_ptr);

/**
 * Get remaining time to expire
 *
 * @param[in]  attr_ptr      Timer attributes object
 * @param[out] remaining_ptr    Remaining time for the timer
 */
void avtimer_attr_getremaining(avtimer_attr_t *attr_ptr, avtimer_duration_t *remaining_ptr);

/**
 * Set timer type - one shot abs,duration or periodic
 *
 * @param[out] attr_ptr   Timer attributes object
 * @param[in]  type   Timer type
 */
void avtimer_attr_settype(avtimer_attr_t *attr_ptr, avtimer_type_t type);

/**
 * get the timer type from the timer object attribute
 *  
 * @param[in]  attr_ptr  Timer attributes object
 * @param[out] type  Timer type
 */
void avtimer_attr_gettype(avtimer_attr_t *attr_ptr, avtimer_type_t *type_ptr);

/**
 * Get attributes of avtimer
 *
 * @param[in] timer  Timer ID
 * @param[out] attr_ptr  Timer attributes object
 * @return           one of ADSPResult error codes
 */
ADSPResult avtimer_get_attr(avtimer_t timer, avtimer_attr_t *attr_ptr);

/**
 * Deletes timer
 *
 * @param[in] timer  Pointer to timer object
 * @return           one of ADSPResult error codes
 */
ADSPResult avtimer_timer_delete(avtimer_t timer);


/**
Initialize avtimer. Must be called from the premordial thread before 
using the avtimer

@return
 ADSPResult error code.
*/
ADSPResult avtimer_init(void);

/**
Deinitialize avtimer. Must be called from the premordial thread 
before exiting the tests 

@return
 ADSPResult error code.
*/
ADSPResult avtimer_deinit(void);

/** @} */ /* end_addtogroup avtimer */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_H

