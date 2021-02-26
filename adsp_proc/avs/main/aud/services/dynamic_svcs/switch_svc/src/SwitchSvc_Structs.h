
/*========================================================================

This file contains structure definitions for Switch Service.

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/switch_svc/src/SwitchSvc_Structs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/04/10   RP      Created file.

========================================================================== */
#ifndef SWITCHSVC_STRUCT_H
#define SWITCHSVC_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

static const uint32_t  SWITCH_CMD_SIG    = 0x80000000;
static const uint32_t  SWITCH_DATA0_SIG  = 0x40000000;
static const uint32_t  SWITCH_DATA1_SIG  = 0x20000000;
static const uint32_t  SWITCH_BUF_SIG    = 0x10000000;
static const uint32_t  PORT_SWITCH_MASK  = 0x60000000;

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static const uint32_t DATA_BUFFER_SIGNAL[2] = {SWITCH_DATA0_SIG, SWITCH_DATA1_SIG};

#define SWITCH_THREAD_STACK_SIZE 1024

// maximum data messages in data queue. Must consider data buffers + EOS messages.
static const uint32_t MAX_INP_DATA_Q_ELEMENTS = 16;

// maximum number of buffers in output buffer queue.
static const uint32_t MAX_SWITCH_SVC_BUF_Q_ELEMENTS = 16;

// maximum number of commands expected ever in command queue.
static const uint32_t MAX_CMD_Q_ELEMENTS = 8;

//maximum number of input data ports in gapless switch
static const uint32_t NUM_PORTS_SWITCH = 2;

// This size matches the decoder svc PCM output buffer sizes
static const uint32_t SWITCH_SVC_DEFAULT_OUT_BUF_SIZE = 840*3*2;


typedef enum
{
   INACTIVE = 0,
   ACTIVE
} enum_port_state;

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
struct switch_svc_t
{
   elite_svc_handle_t                   service_handle[NUM_PORTS_SWITCH];     //handle to give out to others
                                                               //array of two handles, each
                                                               // corresponding to 1 input data
                                                               // queue
   elite_svc_handle_t                   *downstream_svc_ptr;    // down stream peer service handle
   qurt_elite_channel_t                      channel;              //hold MQ's owned by this instance
   qurt_elite_queue_t*                  bufQ;                  // output buffer queue
   uint32_t                             out_bufs_allocated;    //number of output buffers allocated on bufQ
   uint32_t                             current_bitfield;
   uint32_t                             channel_status;
   uint32_t                             current_data_bit;
   enum_port_state                      port_state[NUM_PORTS_SWITCH];
   elite_msg_any_t                      cmd_msg;
   elite_msg_any_t                      inp_data_msg;

   AudioStreamMgr_CallBackHandleType    CbData;

   volatile uint32_t                    *delay_ptr;
   uint16_t                             num_channels;
   uint16_t                             bits_per_sample;
   uint32_t                             sample_rate;

} ;


static inline uint32_t switch_svc_adjust_buf_delay_and_get_client_token(switch_svc_t *pMe, uint32_t buf_size, uint32_t old_buf_delay)
{
   uint32_t sample_rate = pMe->sample_rate;
   uint32_t num_channels = pMe->num_channels;
   uint32_t bytes_per_sample = pMe->bits_per_sample/8;

   uint32_t bytes_p_sec = (sample_rate * num_channels * bytes_per_sample);

   int64_t delay = *pMe->delay_ptr;
   delay -= old_buf_delay; //subtract old delay

   if (bytes_p_sec == 0)
   {
      *pMe->delay_ptr = delay;
      return 0;
   }

   uint32_t cur_buf_delay = ( ( ((int64_t)buf_size*1000000L) + bytes_p_sec - 1) / bytes_p_sec ); //add current delay (round up)
   
   delay +=  cur_buf_delay;

   *pMe->delay_ptr = (uint32_t) delay; //add delay when the buf goes down & also store as client token.

   uint32_t client_token = cur_buf_delay;

   return client_token;

}

static inline uint32_t switch_svc_get_buf_delay_from_client_token(switch_svc_t *pMe, uint32_t *client_token_ptr)
{
   uint32_t delay = *client_token_ptr;

   *client_token_ptr = 0;

   return delay;
}

static inline void switch_svc_subtract_old_buf_delay(switch_svc_t *pMe, uint32_t old_buf_delay)
{
   *pMe->delay_ptr = (uint32_t) ((int64_t)(*pMe->delay_ptr) - (int64_t)old_buf_delay);
}


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef SWITCHSVC_STRUCT_H

