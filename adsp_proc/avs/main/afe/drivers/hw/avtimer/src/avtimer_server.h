/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_server.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.
========================================================================== */
/**
@file avtimer_server.h

@brief This file contains avtimer server apis implementation.
*/

#ifndef AVTIMER_SERVER_H
#define AVTIMER_SERVER_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "avtimer_defines.h"
#include "avtimer.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
/**
 * maximum messages that a queue can hold
 */
#define AVTIMER_SERVER_MAX_CMD_Q_ELEMENTS  128
#define AVTIMER_SERVER_CMD_Q_SIZE_IN_BYTES (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AVTIMER_SERVER_MAX_CMD_Q_ELEMENTS))

/**
 * avtimer client node
 */
typedef struct avtimer_client_node avtimer_client_node_t;

struct avtimer_client_node
{

  uint32_t                  state;
  /**< state to validate client pointer */

  avtimer_type_t            type;
  /**< Timer type */

  avtimer_callback_t        cb_data;
  /**< call back data, when timer expires */

  avtimer_duration_t        reload;
  /**< Reload offset when timer expires (0 = no reload at expiry) */

  avtimer_tick_type         start;
  /**< hw timer tick when the timer was started */

  avtimer_tick_type         expiry;
  /**< time tick @ which timer expires at or remaining ticks until expiry */

  avtimer_client_node_t*    next_ptr;
  /**< Pointer to the next timer in the list  */

};


typedef struct avtimer_set_timer {

    avtimer_client_node_t * active_list;
    /**< pointer to a list of active timers*/

    avtimer_client_node_t * inactive_list;
    /**< Todo (can remove it )pointer to a list of inactive timers*/

    avtimer_tick_type next_expiry;
    /**< this is the next expiry that hw avtimer block needs to be programmed*/
}avtimer_set_timer_t;

/**
 * This is functon pointer type to reporgram the next expiry time for both avtimer hw revision 1 and 2 denpending on device config
 *
 * @param set_timer_ptr
 */
typedef void (*avtimer_server_config_next_expiry_event_ftype)(avtimer_set_timer_t *set_timer_ptr);

/**
 * function pointer type to process the timer interrupt for both avtimer hw revision 1 and 2 denpending on device config
 *
 * @param server_inst_ptr pointer to avtimer_server_inst_t
 *                        instance struct
 *
 * @return ADSPResult one of the ADSPResult error codes.
 */
typedef ADSPResult (*avtimer_server_process_interrupt_ftype)(void* server_inst_ptr);


typedef struct
{
   avtimer_server_config_next_expiry_event_ftype avtimer_server_config_next_expiry_event_f;
   /** < function pointer to process the timer interrupt  */
   avtimer_server_process_interrupt_ftype avtimer_server_process_interrupt_f;
   /** < functon pointer to reporgram the next expiry time */
} avtimer_server_func_def_t; 

/**
 * avtimer server instance type
 */
typedef struct avtimer_server_inst_t {
    qurt_elite_thread_t avtimer_server_tid;
    /**< avtimer server thread id */

    qurt_elite_channel_t avtimer_server_channel;
    /**< avtimer server channel,waiting to process messages from queue and interrupt */

    QURT_ELITE_ALIGN(char avtimer_server_cmdq[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AVTIMER_SERVER_CMD_Q_SIZE_IN_BYTES)], 8);
    /**< avtimer server message queue */

    qurt_elite_signal_t avtimer_server_intrpt_signal;
    /**< This signal will be set whenever a interrupt is received  */

    qurt_elite_signal_t avtimer_server_exit_signal;
    /**< This signal will be set avtimer_deinit function is called  */


    uint32_t msgq_mask;
    /**< message queue mask  */

    uint32_t intrpt_sig_mask;
    /**< interrupt signal mask  */

    uint32_t exit_sig_mask;
    /**< exit signal mask  */

    uint32_t wait_mask;
    /**< channel wait mask  */

    avtimer_set_timer_t list_timer;
    /** < active list of timers & next expiry  */

    avtimer_server_func_def_t avtimer_server_funcs_table;
    /** < function pointers to process the timer interrupt and and reporgram the next expiry time */ 
}avtimer_server_inst_t;

/* extern the global instance pointer to be used in avtimer client*/
extern avtimer_server_inst_t *avtimer_server_global_inst_ptr;

/**
 * signal the server that the interrupt is received.
 *
 *
 */
void avtimer_server_send_interrupt_signal(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_SERVER_H

