
/**
@file audproc_topo_bg_thread.h

   Header file for audproc topo background thread structures and
   functions.
*/

/*========================================================================

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
6/13/16     rishabht      Created file.

========================================================================== */

#ifndef AUDPROC_TOPO_BG_THREAD_H
#define AUDPROC_TOPO_BG_THREAD_H


/*----------------------------------------------------------------------------
 * Include files
 * -------------------------------------------------------------------------*/

#include "audproc_topohandler.h"
#ifndef TST_TOPO_LAYER
#ifdef AUDPPSVC_TEST
#include "hexagon_sim_timer.h"
#endif // AUDPPSVC_TEST
#include "adsp_media_fmt.h"
#include "adsp_amdb_static.h"
#include "adsp_audproc_api.h"
#include "AudDynaPPSvc.h"
#include "EliteCmnTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#else
#include "tst_topo_layer.h"
#endif

/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/

/** Gets the most significant bit signal position */
#define get_signal_pos(mask)  (Q6_R_cl0_R(mask))

/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct topo_struct;

enum topo_bg_cb_event_id_t {
    TOPO_BG_EVENT_ENABLE_MAIN_TH_CMDQ,
	TOPO_BG_EVENT_ENABLE_MAIN_TH_DATAQ,
    TOPO_BG_ID_MAX = 0x7FFFFFFF
};

typedef struct topo_bg_cb_info_t{
   uint32_t port_id;
   topo_bg_cb_event_id_t event_id;
   void     *payload;
}topo_bg_cb_info_t;

typedef ADSPResult (*topo_bg_th_cb_f)(void *context_ptr, topo_bg_cb_info_t cb_info);

typedef struct topo_bg_th_cb_info_t{
   topo_bg_th_cb_f cb_func;
   void *context_ptr;
}topo_bg_th_cb_info_t;


typedef struct topo_bg_thread_svc_t
{
    qurt_elite_signal_t  *exit_signal_ptr;         // to indicate bg thread to exit
    qurt_elite_signal_t  *process_signal_ptr;      // to indicate bg thread when to start processing
    qurt_elite_signal_t  *done_signal_ptr;         // raised by bg thread to indicate main thread when processing is finished
    qurt_elite_channel_t qurt_elite_channel;
    qurt_elite_channel_t done_signal_channel;
    uint32_t             rcvd_mask;
    uint32_t             wait_mask;
	uint32_t             bg_th_objId;
    qurt_elite_thread_t thread;

    void *state;                                 // pointer to process utility structure to call the module core process function
}topo_bg_thread_svc_t;

typedef struct topo_bg_thread_port_eos_state_t
{
   bool_t eos;
   bool_t eos_state;
}topo_bg_thread_port_eos_state_t;


typedef struct topo_bg_internal_buff_t
{
	AudPP_BufInfo_t             *pInternalBufInfo;
	uint32_t                     valid_data_end_offset;
	uint32_t                     size_in_bytes;

}topo_bg_internal_buff_t;

typedef struct topo_bg_thread_port_info_t
{
    AudPP_BufInfo_t             *pBufInfo;
    topo_bg_internal_buff_t     internal_buf;
    AdspAudioMediaFormatInfo_t  media_format;   // input media format info of back ground thread
    AdspAudioMediaFormatInfo_t  cached_media_fmt;   // input media format info of back ground thread
    uint32_t                    frame_size_in_bytes;                  // process frame size in bytes per channel
    uint32_t                    buff_offset;
    bool_t                      dataQ_pending_event_flag;
    bool_t                      cmdQ_pending_event_flag;
    uint32_t                    bg_thread_port_delay;
    bool_t                      eos_flag;
}topo_bg_thread_port_info_t;


typedef struct topo_bg_thread_struct_t
{
    topo_struct                  *topo_ptr;                     // topo ptr : same instance is used by both main and bg thread
    uint32_t                     ouput_buffer_num_unit_frames;     // number of output buffer unit frames required by bg thread
    uint32_t                     ouput_main_th_buffer_num_unit_frames;     // number of output buffer unit frames required by bg thread
    uint32_t                     topo_algo_delay;                  // combined algo delay of entire topo
    QURT_ELITE_HEAP_ID           heap_id;
    int8_t                       *pPort_mem;
    topo_bg_thread_port_info_t   *pIn_port_info;
    topo_bg_thread_port_info_t   *pOut_port_info;
    uint32_t                     numActiveInPorts;
    uint32_t                     numActiveOutPorts;
    topo_bg_thread_svc_t         th;
    bool_t                       thread_launched;     // 0 : thread launch failed ; 1 : thread launch successful
    uint16_t                     main_th_objId;
    uint32_t                     stack_size;
	topo_bg_th_cb_info_t         *pp_svc_cb_info;
    char                         thread_name[9];      // 8 characters and one NULL character
    bool_t                       allow_partial_processing;
    uint32_t                     num_unit_frame_offset;

}topo_bg_thread_struct_t;




ADSPResult topo_bg_thread_create(topo_struct *topo_ptr, void **handle, QURT_ELITE_HEAP_ID heap_id,
                                     uint32_t numActiveInPorts, uint32_t numActiveOutPorts, 
                                     uint16_t main_thread_objId, topo_bg_th_cb_info_t * cb_info);
ADSPResult topo_bg_thread_destroy(void **ppInstance);

void topo_bg_thread_wait_for_processing_done(void *pInstance);

ADSPResult topo_bg_thread_start_process(void *pInstance, AudPP_BufInfo_t *inbuf_ptr[], AudPP_BufInfo_t *outbuf_ptr[]);

uint32_t topo_bg_thread_buffering_delay_us(void *pInstance);

bool_t topo_is_bg_thread_created(void *pInstance);

bool_t topo_bg_is_bg_thread_processing(void *pInstance);

ADSPResult topo_bg_thread_reset_input_buffers(void *pInstance, uint32_t port_id, AdspAudioMediaFormatInfo_t *new_media_fmt);

ADSPResult topo_bg_thread_reset_output_buffers(void *pInstance, uint32_t port_id, AdspAudioMediaFormatInfo_t *new_media_fmt);

ADSPResult topo_bg_set_media_type(void *pInstance, AdspAudioMediaFormatInfo_t *pNew_media_format,
                                       uint32_t port_id);

bool_t topo_bg_is_any_dataQ_event_pending(void *pInstance, uint32_t port_id);

void topo_bg_set_dataQ_pending_flag(void *pInstance, bool_t flag, uint32_t port_id);

bool_t topo_bg_is_any_cmdQ_event_pending(void *pInstance, uint32_t port_id);

void topo_bg_set_cmdQ_pending_flag(void *pInstance, bool_t flag, uint32_t port_id);

bool_t topo_bg_any_main_th_event_pending(void *pInstance, uint32_t port_id);

ADSPResult topo_bg_thread_start_process_frac_samp(void *pInstance, AudPP_BufInfo_t *inbuf_ptr[], AudPP_BufInfo_t *outbuf_ptr[]);

bool_t topo_bg_is_extra_out_buff_req(void *pHandle, uint32_t port_idx);

bool_t topo_bg_is_partial_proc_req(void *pHandle, uint32_t port_idx);

void topo_bg_fill_out_buff_info(void *pHandle, AudPP_BufInfo_t *outBufInfo,uint32_t port_idx);

void topo_bg_send_eos_flag(void *pInstance, uint32_t port_id, bool_t is_eos);




#endif // #ifndef AUDPROC_TOPO_BG_THREAD_H

