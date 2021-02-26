#ifndef ELITE_INTF_EXTNS_VOCODER_H
#define ELITE_INTF_EXTNS_VOCODER_H

/* ======================================================================== */
/**
@file Elite_intf_extns_vocoder.h

@brief Interface extension header for vocoders

  This file defines the parameters, events and other behaviors associated
  with vocoders
*/

/* =========================================================================
  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
  ========================================================================== */

/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   07/16/16   sw      (Tech Comm) Merged Doxygen comments from 8953.2.7.
   09/28/15   ka      Initial Version.
   ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#include "Elite_CAPI_V2_types.h"

/** @addtogroup capiv2_ext_vocoder
@{ */

/** Unique identifier of the custom interface extension for vocoders. */
#define INTF_EXTN_VOCODER           (0x00010E68)

/*------------------------------------------------------------------------------
 * Parameter IDs
 *----------------------------------------------------------------------------*/

/** ID of the parameter that indicates whether discontinuous transmission
  is enabled.

  The VSS_ISTREAM_CMD_SET_ENC_DTX_MODE command passes the DTX mode to the
  encoder through this parameter ID. For details on this command, refer to
  @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{voc_param_enc_dtx_t}
  @table{weak__voc__param__enc__dtx__t}
 */
#define VOC_PARAM_ENC_DTX           (0x00010e69)

typedef struct voc_param_enc_dtx_t voc_param_enc_dtx_t;

/** @weakgroup weak_voc_param_enc_dtx_t
@{ */
struct voc_param_enc_dtx_t
{
   uint32_t dtx_mode;    /**< Discontinuous transmission mode. */
};
/** @} *//* end_weakgroup weak_voc_param_enc_dtx_t */


/** ID of the parameter that identifies the encoding rate for the
  encoder.
  The exact interpretation of the value and valid range depends on the
  type of encoder.

  The SET_ENC_RATE command for the appropriate vocoder passes the rate to the
  encoder through this parameter ID (for example,
  VSS_ISTREAM_CMD_VOC_AMR_SET_ENC_RATE and
  VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE). For details on all the SET_ENC_RATE
  commands, refer to @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{voc_param_enc_rate_t}
  @table{weak__voc__param__enc__rate__t} @newpage
 */
#define VOC_PARAM_ENC_RATE          (0x00010e6a)

typedef struct voc_param_enc_rate_t voc_param_enc_rate_t;

/** @weakgroup weak_voc_param_enc_rate_t
@{ */
struct voc_param_enc_rate_t
{
   uint32_t rate;   /**< Encoder rate. */
};
/** @} *//* end_weakgroup weak_voc_param_enc_rate_t */


/** ID of the parameter that identifies the minimum and maximum encoding
  rates to be used.
  The exact interpretation of the values and valid ranges depends on the
  type of encoder.

  The VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE command passes the rates to the
  encoder through this parameter ID. This command may not be supported by
  all encoders. For details on this command, refer to
  @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{voc_param_enc_minmax_rate_t}
  @table{weak__voc__param__enc__minmax__rate__t}
 */
#define VOC_PARAM_ENC_MINMAX_RATE   (0x00010e6b)

typedef struct voc_param_enc_minmax_rate_t voc_param_enc_minmax_rate_t;

/** @weakgroup weak_voc_param_enc_minmax_rate_t
@{ */
struct voc_param_enc_minmax_rate_t
{
   uint32_t min_rate;   /**< Minimum encoding rate. */
   uint32_t max_rate;   /**< Maximum encoding rate. */
};
/** @} *//* end_weakgroup weak_voc_param_enc_minmax_rate_t */


/** ID of the parameter that identifies the encoding rate for modulation.
  The exact interpretation of the value and valid range depends on the
  type of encoder.

  The VSS_ISTREAM_CMD_CDMA_SET_ENC_RATE_MODULATION command passes the rate to
  the encoder through this parameter ID. This command may not be supported by
  all encoders. For details on this command, refer to
  @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{voc_param_enc_rate_mod_t}
  @table{weak__voc__param__enc__rate__mod__t}
 */
#define VOC_PARAM_ENC_RATE_MOD          (0x00010e6c)

typedef struct voc_param_enc_rate_mod_t voc_param_enc_rate_mod_t;

/** @weakgroup weak_voc_param_enc_rate_mod_t
@{ */
struct voc_param_enc_rate_mod_t
{
   uint32_t rate_modulation_param;
   /**< Encoder rate modulation command */
};
/** @} *//* end_weakgroup weak_voc_param_enc_rate_mod_t */


/** ID of the parameter that identifies the rate and bandwidth the encoder
  uses. The exact interpretation of the value and valid range depends on the
  type of encoder.

  The VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE passes the mode to the
  encoder through this parameter ID. For details on this command, refer to
  @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{voc_param_enc_mode_t}
  @table{weak__voc__param__enc__mode__t} @newpage
 */
#define VOC_PARAM_ENC_MODE          (0x00010e6d)

typedef struct voc_param_enc_mode_t voc_param_enc_mode_t;

/** @weakgroup weak_voc_param_enc_mode_t
@{ */
struct voc_param_enc_mode_t
{
   uint32_t rate;        /**< Encoder rate. */
   uint32_t bandwidth;   /**< Encoder bandwidth. */
};
/** @} *//* end_weakgroup weak_voc_param_enc_mode_t */


/** ID of the parameter that identifies when the Channel Aware mode is set.
  The exact interpretation of the value and valid range depends on the type of
  encoder.

  The VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE and
  VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE commands pass the mode
  to the encoder through this parameter ID. These commands may not be supported
  by all encoders. For details on these commands, refer to
  @xrefcond{Q8,80-NF774-31,80-NA610-31}.

  @msgpayload{voc_param_enc_channel_aware_mode_t}
  @table{weak__voc__param__enc__channel__aware__mode__t}
 */
#define VOC_PARAM_ENC_CHANNEL_AWARE_MODE          (0x00010e6e)

typedef struct voc_param_enc_channel_aware_mode_t voc_param_enc_channel_aware_mode_t;

/** @weakgroup weak_voc_param_enc_channel_aware_mode_t
@{ */
struct voc_param_enc_channel_aware_mode_t
{
   uint32_t mode;        /**< Channel Aware mode. */
   uint8_t fec_offset;   /**< Forward error correction offset. */
   uint8_t fer_rate;     /**< Frame error rate threshold. */
};
/** @} *//* end_weakgroup weak_voc_param_enc_channel_aware_mode_t */


/** ID of the parameter that identifies if BeAMR is to be enabled.

  This parameter is associated with the VOICE_MODULE_BEAMR module. For details,
  refer to @xrefcond{Q9,80-NF774-2,80-NA610-2}.

  @msgpayload{voc_param_beamr_t}
  @table{weak__voc__param__beamr__t}
 */
#define VOC_PARAM_DEC_BEAMR             (0x00010e6f)

typedef struct voc_param_beamr_t voc_param_beamr_t;

/** @weakgroup weak_voc_param_beamr_t
@{ */
struct voc_param_beamr_t
{
   uint32_t beamr_enable;
   /**< Specifies whether BeAMR is enabled.

        @values
        - 0 -- Disabled
        - 1 -- Enabled @tablebulletend */
};
/** @} *//* end_weakgroup weak_voc_param_beamr_t */

#define VOC_PARAM_VOC_RESET             (0x00010EB3)
/**< Parameter to reset voice encoder and/or decoder.
     This param id can be used reset vocoder whenever client sends homing sequence to network.
     Since these homing sequence reset far end decoder, client can use this param id to reset 
     near end encoder so that both encoder and decoder are in sync.
     Associated data structure: voc_param_voc_reset_t */

typedef struct voc_param_voc_reset_t voc_param_voc_reset_t;

struct voc_param_voc_reset_t
{
   uint16_t reset_encoder;
   /**< if set encoder will be reset */
   uint16_t reset_decoder;
   /**< if set decoder will be reset */
};

/*------------------------------------------------------------------------------
 * Events
 *----------------------------------------------------------------------------*/

/** Raised by the vocoder when it is initialized or when its internal operating
  mode changed.

  The operating mode represents the actual bandwidth of the signal that the
  encoder is encoding or the decoder is outputting.

  @msgpayload{voc_evt_operating_mode_update_t}
  @table{weak__voc__evt__operating__mode__update__t} @newpage
*/
#define VOC_EVT_OPERATING_MODE_UPDATE  (0x00010e70)

typedef struct voc_evt_operating_mode_update_t voc_evt_operating_mode_update_t;

/** @weakgroup weak_voc_evt_operating_mode_update_t
@{ */
struct voc_evt_operating_mode_update_t
{
   uint32_t direction;
   /**< Direction of the operating mode.

        @values See #voc_operating_mode_direction_t */

   uint32_t mode;
   /**< Operating mode of the vocoder.

        @values See #voc_operating_mode_t */
};
/** @} *//* end_weakgroup weak_voc_evt_operating_mode_update_t */

/** Values for direction of the vocoder operating mode.
 */
typedef enum voc_operating_mode_direction_t
{
   VOC_OPERATING_MODE_DIRECTION_TX = 0,
   /**< Detect a mode change from the Tx path (encoder). The
        #VOC_EVT_OPERATING_MODE_UPDATE event is raised when a change is
        detected. */

   VOC_OPERATING_MODE_DIRECTION_RX
   /**< Detect a mode change from the Rx path (decoder). The
        #VOC_EVT_OPERATING_MODE_UPDATE event is raised when a change is
        detected. */

} /** @cond */voc_operating_mode_direction_t/** @endcond */;

/** Valid operating modes.
 */
typedef enum voc_operating_mode_t
{
   VOC_OPERATING_MODE_NB = 0x00013086,  /**< Vocoder bandwidth mode is 8 kHz. */
   VOC_OPERATING_MODE_WB =0x00013087,   /**< Vocoder bandwidth mode is 16 kHz. */
   VOC_OPERATING_MODE_SWB = 0x00013088, /**< Vocoder bandwidth mode is 32 kHz. */
   VOC_OPERATING_MODE_FB = 0x00013089   /**< Vocoder bandwidth mode is 48 kHz. */
} /** @cond */voc_operating_mode_t/** @endcond */;


/** Decoder raises this event when a packet of zero size is provided or if the
  packet is invalid.

  The decoder overwrites the input packet with an erasure packet and raises
  this event to indicate the size of the overwritten packet.

  @msgpayload{voc_evt_dec_erasure_t}
  @table{weak__voc__evt__dec__pkt__owt__t}
*/
#define VOC_EVT_DEC_PKT_OVERWRITTEN (0x00010e71)

typedef struct voc_evt_dec_pkt_owt_t voc_evt_dec_pkt_owt_t;

/** @weakgroup weak_voc_evt_dec_pkt_owt_t
@{ */
struct voc_evt_dec_pkt_owt_t
{
   uint32_t packet_size;   /**< Size of the overwritten packet. */
};
/** @} *//* end_weakgroup weak_voc_evt_dec_pkt_owt_t */

/** @} *//* end_addtogroup capiv2_ext_vocoder */

#ifdef __cplusplus
}
#endif //__cplusplus
#endif
