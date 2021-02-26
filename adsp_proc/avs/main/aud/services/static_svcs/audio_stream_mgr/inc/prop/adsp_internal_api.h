/*========================================================================
adsp_internal_api.h

This file contains internal private ADSP APIs

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/18/11   HB      Created file for proprietary information.

========================================================================== */
/**
@file adsp_privatedefs.h

@brief This file contains private ADSP APIs
*/

#ifndef _ADSP_INTERNAL_API_H_
#define _ADSP_INTERNAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup AudioStreamMgr_PrivateDefs
    @{ */

/** Topology ID for creating the DS1 in the POPP.
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_DS1AP                         0x00010DDD

/**
 * Topology ID for creating the DTS Headphone X postmix in the POPP.
 * This topology id is added for conformance testing.
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_DTS_HPX_POSTMIX               0x00012f11

/**
 * Topology ID for creating the DTS Headphone X pipeline in the POPP.
 * This topology id is added for conformance testing.
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_DTS_HPX_PIPELINE              0x00012f12

/** @} */ /* end_addtogroup AudioStreamMgr_PrivateDefs */

/* Setparam IDs for Dolby Heaac Decoder*/

/** @cond OEM_only */
/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/* The following HEAAC parameters make use of the generic
 *  structure asm_heaac_generic_param_t
 */
/** ID of the Dolby Pulse High Efficiency AAC (HEAAC) Number of Output Channels
    parameter in the #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to #ASM_MEDIA_FMT_AAC_V2 bitstream decoding. The command is to be sent
    after opening a write or read/write stream and before actual decoding
    starts.

    @msgpayload
    Specifies the flag to enable 5.1 output configuration.
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values
    - 0 -- 5.1 output mode disabled (Default)
    - 6 -- 5.1 output mode enabled
*/
#define ASM_PARAM_ID_HEAAC_NUM_OUTPUT_CHANNELS                    0x00012F20


/** ID of the Dolby Pulse HEAAC Error Conceal parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter specifies whether to enable error concealment.

    This parameter is used only for internal testing and ITAF verification.
    The command is to be sent after opening a write or read/write stream and
    before actual decoding starts.

    Do not send this command during decoding; such a change might introduce a
    blank frame in the middle of playback.

    @msgpayload
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values
    - 0 -- Disable concealment (Default)
    - 1 -- Enable concealment
*/
#define ASM_PARAM_ID_HEAAC_ERROR_CONCEAL                        0x00012F19


/** ID of the Dolby Pulse HEAAC 2-to-1 Resampling parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter specifies whether to enable 2-to-1 resampling.

    This parameter is used only for internal testing and ITAF verification.
    The command is to be sent after opening a write or read/write stream and
    before actual decoding starts.

    Do not send this command during decoding; such a change might introduce
    discontinuities in the middle of playback.

    @msgpayload
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values
    - 0 -- Disable 2-to-1 resampling (Default)
    - 1 -- Enable 2-to-1 resampling
*/
#define ASM_PARAM_ID_HEAAC_2TO1_RESAMPLING                        0x00012F1A


/** ID of the Dolby Pulse HEAAC Replay Gain
    parameter in the #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter specifies whether the Replay Gain value is applied.

    This parameter is used only for internal testing and ITAF verification.
    The command is to be sent after opening a write or read/write stream and
    before actual decoding starts.

    Do not send this command during decoding; such a change might introduce
    discontinuities in the output.

    @msgpayload
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values
    - Range from -72 to 55 (-18.0dB to 13.75dB in steps of 0.25dB)
    - Default value -- 0
*/
#define ASM_PARAM_ID_HEAAC_REPLAY_GAIN                            0x00012F1B


/** ID of the Dolby Pulse HEAAC default Program Reference Level (PRL)
    parameter in the #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter specifies the Default PRL value to be applied.

    This parameter is used only for internal testing and ITAF verification.
    The command is to be sent after opening a write or read/write stream and
    before actual decoding starts.

    Do not send this command during decoding; such a change might introduce
    discontinuities in the middle of playback.

    @msgpayload
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values
    - Range of 0 to 127 (0dB to -31.75dB in steps of 0.25dB)
    - Default value -- 124 @newpage
*/
#define ASM_PARAM_ID_HEAAC_DEFAULT_PRL                            0x00012F1C


/** ID of the Dolby Pulse HEAAC DRC Type and Mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter specifies whether Dynamic Range Control and Leveling is
    enabled and how it is configured.

    This parameter is used only for internal testing and ITAF verification.
    The command is to be sent after opening a write or read/write stream and
    before actual decoding starts.

    The lower 16 bits indicate the DRC mode and the higher 16 bits indicate
    the DRC type.

    @msgpayload
    Specifies a value that is applied to the DRC type and mode.
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values{for Type}
    - 0 -- None
    - 1 -- MPEG-DRC
    - 2 -- DVB-compression
    - 3 -- Prefer DRC
    - 4 -- Prefer compression
    - 5 -- Portable mode

    @values{for Mode} Application of DRC/compression and consideration of PRL
    on the PCM output of the decoder.
    - 0 -- None
    - 1 -- DRC/compression
    - 2 -- PRL
    - 3 -- DRC/compression and PRL

    @values{for Default}
    - 0 -- Type
    - 2 -- Default mode @newpage
*/
#define ASM_PARAM_ID_HEAAC_DRC_TYPE_MODE                        0x00012F1D


/** ID of the Dolby Pulse HEAAC External Boost parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This postprocessing parameter specifies whether to apply external boost
    on the decoded PCM samples.

    This parameter is used only for internal testing and ITAF verification.
    The command is to be sent after opening a write or read/write stream and
    before actual decoding starts.

    @msgpayload
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values
    - 0 -- Do not apply (Default)
    - 1 -- Apply external boost
*/
#define ASM_PARAM_ID_HEAAC_EXT_BOOST                            0x00012F1E


/** ID of the Dolby Pulse HEAAC External Metadata parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This is a post-processing parameter specifies whether or not to apply
    the external metadata on the decoded PCM samples.

    This parameter is used only for internal testing and ITAF verification.
    The command is to be sent after opening a write or read/write stream and
    before actual decoding starts.

    @msgpayload
    For the payload format, see asm_heaac_generic_param_t.
    @par
    @values
    - 0 -- Do not apply (Default)
    - 1 -- Apply external metadata
*/
#define ASM_PARAM_ID_HEAAC_EXT_METADATA                            0x00012F1F


/* Structure for HEAAC Generic Parameter. */
typedef struct asm_heaac_generic_param_t asm_heaac_generic_param_t;

#include "adsp_begin_pack.h"

/** Payload of the generic HEAAC parameters in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_heaac_generic_param_t
{
    uint32_t                  generic_parameter;
    /**< Generic parameter field used by the following HEAAC parameter IDs:
         - #ASM_PARAM_ID_HEAAC_NUM_OUTPUT_CHANNELS
         - #ASM_PARAM_ID_HEAAC_ERROR_CONCEAL
         - #ASM_PARAM_ID_HEAAC_2TO1_RESAMPLING
         - #ASM_PARAM_ID_HEAAC_REPLAY_GAIN
         - #ASM_PARAM_ID_HEAAC_DEFAULT_PRL
         - #ASM_PARAM_ID_HEAAC_DRC_TYPE_MODE
         - #ASM_PARAM_ID_HEAAC_EXT_BOOST
         - #ASM_PARAM_ID_HEAAC_EXT_METADATA @tablebulletend @newpagetable */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @} */ /* end_addtogroup adsp_privatedefs */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_INTERNAL_API_H_ */
