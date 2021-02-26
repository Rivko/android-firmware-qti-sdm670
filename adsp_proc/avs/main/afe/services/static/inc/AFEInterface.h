/*========================================================================
  This file contains AFE interface definies for internal clients

  Copyright (c) 2009-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEInterface.h#3 $
 ====================================================================== */
#ifndef _AFE_INTERFACE_H_
#define _AFE_INTERFACE_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite.h"
#include "EliteSvc.h"
#include "adsp_afe_service_commands.h"
#include "AFEAvtDrift.h"
#include "adsp_core_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
	  Macro Defines
========================================================================== */
/**< data types of afe client.*/
typedef enum 
{
	LINEAR_PCM        = 0, // linear pcm data
	IEC_61937_NON_LINEAR,   //compressed data
	DSD_DOP_NON_LINEAR,    //DSD over PCM data
	GENERIC_COMPRESSED     //general compressed data
}afe_client_data_type;

/**  Possible data paths

  The following are possible reasons for connecting to the AFE.

  Tx port:

  - Tx client -- Anyone who wants to read from the port: 
      AFE_TX_OUT                : Direct data path from Tx device before AFE topo processing  
      AFE_PP_OUT                : Output of AFE topo processing  
      AFE_TX_PP_AUDIO_SENSE_OUT : Data path to LSM directly, output of topo process (For MAD single stage detection) 

  - Rx client -- None; DTMF is added to the Tx data internally (not through
                 the client connect mechanism) for the Advanced Audio
                 Distribution Profile (A2DP) 

  Rx port:

  - Tx client -- who wants Reference data (audio+voice Ref)
      AFE_PP_OUT             : Output of AFE topo process.(After audio+voice mixing,
                               limiter,speaker protection etc.)For example, EC reference client

  - Rx client -- Anyone who wants to write to the port
      AFE_RX_VOC_MIXER_IN       : AFE Mixer for accumulating voice only client data 
      AFE_RX_AUD_MIXER_IN       : AFE Mixer for accumulating audio only client data 
      AFE_RX_IN                 : Direct data path to Rx device without any audio+voice mixer,
                                  limiter or topo processing. To be used by side Tone client

  Pseudo port (treated as an Rx port internally);

 end Possible data paths **/

/**< data paths(tap points) for afe clients from which they can expect or send the data to/from afe.*/
typedef enum
{//only one bit has to be set in below numbers
  AFE_TX_OUT                = 0x0001, /**< Direct data path from Tx device before AFE topo processing (voice only).*/
  AFE_TX_PP_AUDIO_SENSE_OUT = 0x0002, /**< Data path to LSM directly, output of topo process (For MAD single stage detection).*/

  AFE_PP_OUT                = 0x0004, /**< Output of AFE topo processing. This path is for dual purpose
                                                                      For normal AFE topo output (from Tx port)
                                                                      For reference data (from Tx or Rx or Pseudo port).*/

  AFE_RX_VOC_MIXER_IN       = 0x0008, /**< AFE Mixer for accumulating voice only client data.*/
  AFE_RX_AUD_MIXER_IN       = 0x0010, /**< AFE Mixer for accumulating audio only client data.*/
  AFE_RX_IN                 = 0x0020  /**< Direct data path to Rx device without any audio+voice mixer,
                                      limiter or topo processing. To be used by side Tone client .*/
}afe_client_data_path;

/**< types of re-sampler requested by the afe client.
 * IIR resampler is issued under certain configurations
 * Input and output frequency of resampler should be one of these (8k,16k,48k) and two bytes per sample
 * If config fails, by default FIR resampler is issued */
typedef enum
{
	FIR_BASED_SRC=0,      // by default FIR based re-sampler is selected (SRC => sample rate converter)
	IIR_BASED_SRC         // IIR based re-sampler
}afe_client_resampler_type;

/**< types of audio sense clients .*/
typedef enum
{
	AUD_SENSE_DISABLE = 0,     // audio sense is disabled
	AFE_SW_MAD,                // sw mad client
	AFE_MAD_AUDIO,             // mad audio client
	AFE_MAD_BEACON,            // mad beacon client
	AFE_MAD_ULTRASOUND         //mad ultrasound client
}afe_client_audio_sense_type;

/**< information related to listen clients (grouped logically into one structure).*/
typedef struct audio_sense_info
{
  bool_t                       mask_dc_reset;
  /**< When ever this value is NON ZERO all the LSM and SWMAD clients
	         connected to this port will receive data even after they send DC RESET
	     while when this value is ZERO then LSM clients will not receive any data
	          once they send DC RESET till the HWMAD is resetted*/
  afe_client_audio_sense_type  type;
  /** audio sense type whether swmad,mad audio, mad beacon, mad ultrasound */
}audio_sense_info_t;

/**< customized processing, specific to a particular client.*/
typedef struct afe_client_cust_proc
{
  afe_client_resampler_type  resampler_type;
  /**< indicates whether client requested for IIR or FIR based re-sampler.*/
  bool_t                subscribe_to_avt_drift;
  /**< indicates whether the client subscribes (TRUE) to the avt drift or not (FALSE) */
  bool_t                is_pop_suppressor_required;
  /**< indicates whether client requested for pop suppressor (TRUE) or not (FALSE) */
  audio_sense_info_t      audio_sense;
  /**< indicates the audio sense clients information (to be used by listen clients)*/
  bool_t                  is_it_sidetone;
  /**< It is needed to keep track of whether the client is a sidetone client or not,
   * we need to disable processing in case fractional resampling is enabled on this client.
   * As we cannot support strict latency needs for such client as resampling buffering would
   * increase the RTD.
   */
}afe_client_cust_proc_t;

/* Currently this is used only for SVA*/
typedef enum
{
  AFE_PASS_THROUGH = 0, //If enabled this criterion DC reset will not take effect and all clients will still keep getting data
  AFE_MAX_CRITERION
}afe_client_register_criteria_type;

/**< Indicates the direction of AFE port.*/
typedef enum
{
  AFE_PORT_RX_DIR = 0, /** Rx direction. */
  AFE_PORT_TX_DIR      /** Tx direction. */
}afe_port_dir;


/** Direction mask. */
#define AFE_PORT_DIR_MASK      			0x0001 // bit 0

/** Port ID mask. */
#define AFE_PORT_MASK          			0xfffe // bit 1 to 15

/** AFE static service command queue. */
#define AFE_DEVICE_SERVICE_CMDQ_NAME	"devServCmdQ"

/** AFE static service response queue. */
#define AFE_DEVICE_SERVICE_RSPQ_NAME "devServRspQ"


/** Macro to convert port and direction to combined port ID. */
#define AFE_CONNECT(port, port_dir) \
    ((port<<1)&AFE_PORT_MASK)|((port_dir)&AFE_PORT_DIR_MASK)

/** Macro to check if a given combined ID is an ID of a pseudo port. */
#define IS_PSEUDO_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_PSEUDOPORT_RANGE_START <= port_afe_id) && \
         (port_afe_id <= AFE_PORT_ID_PSEUDOPORT_RANGE_END) )

/** Macro to check if a given combined ID is AUDIO IF port. */
#define IS_AUDIO_IF_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START <= port_afe_id) &&  \
         (port_afe_id <= AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END) )   //since port_afe_id is uint, it's always >=0. hence first condition is not necessary.

/** Macro to check if a given combined ID is AUDIO IF TDM port. */
#define IS_AUDIO_IF_TDM_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_TDM_PORT_RANGE_START <= port_afe_id) &&  \
         (port_afe_id <= AFE_PORT_ID_TDM_PORT_RANGE_END) )   //since port_afe_id is uint, it's always >=0. hence first condition is not necessary.

/** Macro to check if a given combined ID is RIVA port. */ 
#define IS_RIVA_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START <= port_afe_id) && \
         (port_afe_id <= AFE_PORT_ID_INTERNAL_BT_FM_RANGE_END) )

/** Macro to check if a given combined ID is SLIMBUS port. */
#define IS_SLIMBUS_PORT_AFE_ID(port_afe_id) ((AFE_PORT_ID_SLIMBUS_RANGE_START <= port_afe_id)&&(port_afe_id <= AFE_PORT_ID_SLIMBUS_RANGE_END))

/** Macro to check if a given combined ID is a real time proxy port. */
#define IS_RT_PROXY_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_RT_PROXY_PORT_RANGE_START <= port_afe_id) && \
         (port_afe_id <= AFE_PORT_ID_RT_PROXY_PORT_RANGE_END) )

/** Macro to check if a given combined ID is SPDIF port */
#define IS_SPDIF_PORT_AFE_ID(port_afe_id) (AFE_PORT_ID_SPDIF_RX == port_afe_id)

/** Macro to check if a given combined ID is HDMI Rx port */
#define IS_HDMI_OUT_PORT_AFE_ID(port_afe_id) (AFE_PORT_ID_MULTICHAN_HDMI_RX == port_afe_id)

/** Macro to check if a given combined ID is an HDMI multistream output port */
#define IS_HDMI_OUTPUT_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START <= port_afe_id) && \
        (port_afe_id <= AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_END) && (!(port_afe_id & AFE_PORT_DIR_MASK)) )

/** Macro to check if a given combined ID is an HDMI over display port Rx port */
#define IS_HDMI_OVER_DP_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_HDMI_OVER_DP_RANGE_START <= port_afe_id) && \
        (port_afe_id <= AFE_PORT_ID_HDMI_OVER_DP_RANGE_END) && (!(port_afe_id & AFE_PORT_DIR_MASK)) )

/** Macro to check if a given combined ID is I2S port. */
#define IS_I2S_PORT_AFE_ID(port_afe_id) ( ((AFE_PORT_ID_PRIMARY_MI2S_RX <= port_afe_id) &&  \
         (port_afe_id <= AFE_PORT_ID_I2S_RX)) || ((AFE_PORT_ID_QUINARY_MI2S_RX <= port_afe_id) && \
          (port_afe_id <= AFE_PORT_ID_QUATERNARY_MI2S_TX_7)) )

/** Macro to check if a given combined ID is PCM port. */
#define IS_PCM_PORT_AFE_ID(port_afe_id) ( ((AFE_PORT_ID_PRIMARY_PCM_RX <= port_afe_id) &&  \
         (port_afe_id <= AFE_PORT_ID_QUATERNARY_PCM_TX)) || ((AFE_PORT_ID_QUINARY_PCM_RX <= port_afe_id) && \
          (port_afe_id <= AFE_PORT_ID_QUINARY_PCM_TX)) )

/** Macro to check if a given combined ID is USB port. */
#define IS_USBA_PORT_AFE_ID(port_afe_id) ( (AFE_PORT_ID_USB_AUDIO_0_RX <= port_afe_id) &&  \
         (port_afe_id <= AFE_PORT_ID_USB_AUDIO_0_TX) )

/** get index for AUDIOIF port */
#define GET_AUDIOIF_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START)

/** get index for RIVA port */
#define GET_RIVA_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START)

/** get index for RT proxy port */
#define GET_RT_PROXY_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_RT_PROXY_PORT_RANGE_START)

/** get index for SLIMBUS port */
#define GET_SLIMBUS_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_SLIMBUS_RANGE_START)

/** get index for USB Audio port */
#define GET_USBA_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_USB_AUDIO_RANGE_START)

/** get index for PSEUDO port */
#define GET_PSEUDO_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_PSEUDOPORT_RANGE_START)

/** get index for HDMI input port */
#define GET_HDMI_INPUT_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_HDMI_INPUT_RANGE_START)

/** get index for HDMI output port */
#define GET_HDMI_OUTPUT_PORT_INDEX(port_afe_id) ((port_afe_id - AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START)/2)  

/** get index for HDMI over display port AFE Rx port */
#define GET_HDMI_OVER_DP_PORT_INDEX(port_afe_id) ((port_afe_id - AFE_PORT_ID_HDMI_OVER_DP_RANGE_START)/2)

/** get index for USB Audio port */
#define GET_USB_AUDIO_PORT_INDEX(port_afe_id) (port_afe_id - AFE_PORT_ID_USB_AUDIO_RANGE_START)


/** Macro to check if the given port id is Tx port	or	Rx
 *  port. Not valid for	pseudo ports */
#define IS_IT_TX_PORT_AFE_ID(port_afe_id) (port_afe_id & AFE_PORT_DIR_MASK)

/** Maximum number of audio channels supported in the AFE. */
#define MAX_AUDIO_CHANNELS 		AFE_PORT_MAX_CHANNEL_CNT

/** Possible port directions. */
typedef enum
{
   RX_DIR = 0 ,  /**< Direction is Rx. */
   TX_DIR        /**< Direction is Tx. */
} direction;

/** @brief Connects to the AFE service. */
typedef struct afe_client
{
  elite_svc_handle_t*   afe_client_handle;  /**< Client's handle. */
  uint32_t              sample_rate;       /**< Sampling rate of the
                                                   data. */
  uint32_t              channel;          /**< Number of channels */
  uint32_t              buf_size;          /**< Buffer size of each buffer
                                                   in samples/channel;
                                                   must be a multiple or
                                                   one sample number per
                                                   interrupt. */
  uint32_t              num_buffers;       /**< Number of buffers
                                                   in BufQ. */
  uint32_t              bytes_per_channel;  /**< 16-, 24-, or 32-bit
                                                   format. */
  uint32_t              bit_width;  		/**< bit width */

  bool_t                interleaved;       /**< Noninterleave (0) or
                                                   interleave (1) flag of the
                                                   data; AFE_NONINTERLEAVED,
                                                   AFE_INTERLEAVED, or
                                                   sample interleaved. */
  afe_client_data_path   data_path;       /**< Tap point in AFE topology, where the client expects/sends the data from/to AFE. */
  afe_client_cust_proc_t cust_proc;        /**<  customized processing required for a particular client. */
  afe_client_data_type   data_type;        /**<  Denotes whether the client's data is Linear or non-linear (compressed).*/
  uint8_t               channel_mapping[8]; /**< Channel mapping information.*/

  volatile uint32_t     *afe_dp_delay_ptr;  /**< Pointer to a memory location where the current total
                                                   delay in microseconds will be written. 
												   Total delay is algorithmic delay + buffer delay + 1ms AFE processing delay.
                                                   Note: This memory should not be written to by anyone else. 
												   It will be updated only by the AFE.*/

  volatile uint32_t     *afe_dev_hw_delay_ptr;  /**< Pointer to a memory location where the device hardware
                                                   delay in microseconds will be written. 
                                                   Note: This memory should not be written to by anyone else. 
												   It will be updated only by the AFE.*/

} afe_client_t;

#define BLOCK_TRANSFER_TIME_10MSEC 10


/**
 * This function returns the AFE service version
 *
 * @param[out] svc_info_ptr pointer to the service info structure
 * @return error code
 */
ADSPResult afe_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);

/**
  Validate the given port id.

  @param[in] port_id - port id needs to be evaluated

  @return
  ADSP_EOK in case of valid port id
  ADSP_EBADPARAM in case of invalid port id

  @dependencies
  None.
 */
ADSPResult afe_port_id_validation(uint16_t port_id);

/**
Based on the samplerate of client/device port and afe port latency mode, this utility would
return number of equivalent samples rounded to approximately 1ms/0.5ms time frame.
 
The client connecting to AFE can use this information to allocate the buffers
This number is the minimum samples and clients are free to chose any
integral multiple of this number.

@param[in] latency_mode - AFE port operating latency mode
@param[in] sample rate - sample rate for which frame size is to be known
@param[in] *num_samples_ptr - pointer to variable which holds frame size (in samples)

@return
ADSP_EOK in case of valid sample rate
ADSP_EUNSUPPORTED in case of invalid sample rate

@dependencies
None
 */

ADSPResult afe_port_get_frame_size(uint32_t latency_mode, uint32_t sample_rate, uint32_t *num_samples_ptr);



#ifdef __cplusplus
}
#endif //__cplusplus
#endif // _AFE_INTERFACE_H_
