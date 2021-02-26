
/*========================================================================

@file VoiceProcTx.h
@brief This file provides interfaces to create and control an instance of the
voice proc tx dynamic service

Copyright (c) 2009-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*//*====================================================================== */

/*========================================================================
Edit History

$Header://source/qcom/qct/multimedia2/Audio/voc/svcs/VoiceProcTx/dev/elite/src/VoiceProcTx.cpp

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/11/10   ksa     Added doxygen markup
10/29/09   dc      Created file.

========================================================================== */
#ifndef VOICEPROCTX_H
#define VOICEPROCTX_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "adsp_vpm_api.h"
#include "VoiceCmnUtils.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/****************************************************************************
** Service related types
*****************************************************************************/

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

// 1ms block size in samples
#define VPTX_DMA_BLOCK(SAMPLING_RATE) ((uint32_t)(SAMPLING_RATE)/(uint32_t)(1000))
// #define LOG_VOICEPROC_DATA

// table to get the number of channels for corresponding Tx topology.
const topo_descriptor_struct topo_vptx[] =
{
   { VPM_TX_NONE,               1},
   { VPM_TX_SM_ECNS,            1},
   { VPM_TX_SM_FLUENCEV5,       1},
   { VPM_TX_SM_ECNS_V2,         1},
   { VPM_TX_CUSTOM_SM_ECNS_1,   1},
   { VPM_TX_CUSTOM_SM_ECNS_2,   1},
   { VPM_TX_CUSTOM_SM_ECNS_3,   1},
   { VPM_TX_DM_FLUENCE,         2},
   { VPM_TX_DM_FLUENCEV5,       2},
   { VPM_TX_CUSTOM_DM_ECNS_1,   2},
   { VPM_TX_CUSTOM_DM_ECNS_2,   2},
   { VPM_TX_CUSTOM_DM_ECNS_3,   2},
   { VPM_TX_DM_FLUENCEV5_BROADSIDE, 2},
   { VPM_TX_DM_VPECNS,          2},
   { VPM_TX_QM_FLUENCE_PROV2,   4},
   { VPM_TX_CUSTOM_QM_ECNS_1,   4},
   { VPM_TX_CUSTOM_QM_ECNS_2,   4},
   { VPM_TX_CUSTOM_QM_ECNS_3,   4},
};

typedef struct vptx_vpm_apr_info_t
{
   /*The following is necessary information for direct communication with client
    and for talking with APR*/
   uint32_t    apr_handle;
   /**< This is the apr handle that is required in sending ack. */
   uint16_t    self_addr;
   /**< self's address (16bit) */
   uint16_t    self_port;
   /**< slef's port (16bit) */
   uint16_t    client_addr;
   /**< Client's address (16bit)*/
   uint16_t    client_port;
   /**< Client's port (16bit) */
}vptx_vpm_apr_info_t;


typedef struct vptx_create_params_t
{
   uint16_t near_port_id;         //afe_tx_port_id ID of Tx AFE port to connect to
   uint16_t far_port_id;          //afe_rx_port_id ID of Rx AFE port to connect to for far reference
   uint32_t topology_id;          //topology_id Topology ID to be used
   uint32_t sampling_rate;        //sampling_rate Sampling rate to run the voice proc at
   uint32_t session_num;          //session_num VPM/ADM session number that this instance belongs to
   uint32_t shared_mem_client;    //shared_mem_client Client to use when converting physical addresses to virtual
   uint16_t num_mics;             //number of mics
   uint32_t static_svc_apr_handle;   //APR handle of static svc creating this dynamic svc
} vptx_create_params_t;


typedef struct vptx_create_params_v2_t
{
   uint16_t near_port_id;         //afe_tx_port_id ID of Tx AFE port to connect to
   uint16_t far_port_id;          //afe_rx_port_id ID of Rx AFE port to connect to for far reference
   uint32_t topology_id;          //topology_id Topology ID to be used
   uint32_t sampling_rate;        //sampling_rate Sampling rate to run the voice proc at
   uint32_t session_num;          //session_num VPM/ADM session number that this instance belongs to
   uint32_t shared_mem_client;    //shared_mem_client Client to use when converting physical addresses to virtual
   uint16_t num_mics;             //number of mics for near channel
   uint32_t static_svc_apr_handle;   //APR handle of static svc creating this dynamic svc
   uint16_t num_ref_channels;     // number of channels in reference path
   uint8_t  channel_map_near[8];  // channel mapping information for near port
   uint8_t  channel_map_far[8];   // channel mapping information for reference/far port

} vptx_create_params_v2_t;


/** Creates an instance of the voice proc tx dynamic service for use in the
    voice path


    @param [out] near handle Return handle to the created instance for near data Q
    @param [out] far handle Return handle to the created instance for far dataQ
    @param [out] handle Return handle to the created instance
    @param [in] vptx_create_params_t - create params

    NOTE: Initialization of algorithms is exercised during vptx_create, so the APPI
    function handles must be sent down right now, and not later (using a message for example).
    Otherwise, Vptx sequencing would need to be revisited.
*/
ADSPResult vptx_create (void **near_handle,
                        void **far_handle,
                        vptx_create_params_v2_t* create_param_ptr,
                        vptx_vpm_apr_info_t *apr_info_ptr
                        );

/** Creates an instance of the voice proc tx dynamic service for use in the
    audio path.

    This instance is used when echo cancellation or dual mic is required in the
    audio record path. This instance differs from the voice call instance
    in the following ways:
    1. Contains a resampler to upsample output to 48k
    2. Runs in faster than real time mode
    3. Output is upmixed to stereo

    @param [out] near handle Return handle to the created instance for near data Q
    @param [out] far handle Return handle to the created instance for far dataQ
    @param [in] afe_tx_port_id ID of Tx AFE port to connect to
    @param [in] afe_rx_port_id ID of Rx AFE port to connect to for far reference
    @param [in] topology_id Topology ID to be used
    @param [in] sampling_rate Sampling rate to run the voice proc at
    @param [in] session_num VPM/ADM session number that this instance belongs to
    @param [in] shared_mem_client Client to use when converting physical
    addresses to virtual
*/
ADSPResult vptx_aud_create (void **handle,
                           void **far_handle,
                           uint16_t afe_tx_port_id,
                           uint16_t afe_rx_port_id,
                           uint32_t topology_id,
                           uint32_t sampling_rate,
                           uint32_t session_num,
                           uint32_t shared_mem_client);

/** Creates an instance of the voice proc tx dynamic service for use in the
    audio path

    @param [out] near handle Return handle to the created instance for near data Q
    @param [out] far handle Return handle to the created instance for far dataQ
    @param [out] handle Return handle to the created instance
    @param [in] vptx_create_params_t - create params
*/
ADSPResult vptx_aud_create_v2 (void **near_handle,
                               void **far_handle,
                               vptx_create_params_t* create_param_ptr
                              );


/** Creates an instance of the voice proc tx dynamic service for use in the
    audio path

    @param [out] near handle Return handle to the created instance for near data Q
    @param [out] far handle Return handle to the created instance for far dataQ
    @param [out] handle Return handle to the created instance
    @param [in] vptx_create_params_v2_t - create params
*/
ADSPResult vptx_aud_create_v3 (void **near_handle,
                               void **far_handle,
                               vptx_create_params_v2_t* create_param_ptr
                              );

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//#ifndef VOICEPROCTX_H

