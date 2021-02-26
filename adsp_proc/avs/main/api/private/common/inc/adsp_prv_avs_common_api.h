#ifndef _ADSP_PRV_AVS_COMMON_API_H
#define _ADSP_PRV_AVS_COMMON_API_H

/*========================================================================
@file adsp_prv_avs_common_api.h
This file contains private AVS common API commands and events structures definitions.

Copyright (c) 2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/private/common/inc/adsp_prv_avs_common_api.h#2 $

when           who    what, where, why
--------           ---        -------------------------------------------------------
08/19/16    KR     Created file
========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*    Generic decoder module ID.
 *    This module supports the following parameter IDs:
 *              - #AVS_DECODER_PARAM_ID_DEC_FMT_ID (cannot be set run time)
 *              - #AVS_DECODER_PARAM_ID_DEPACKETIZER_ID (cannot be set run time)
 *              - #AVS_DECODER_PARAM_ID_DEC_MEDIA_FMT (may be set run time)
 *    Opcode -- #AVS_MODULE_ID_DECODER
 *    AFE Command #AFE_PORT_CMD_SET_PARAM_V2 supports this module ID.
 *    #define AVS_MODULE_ID_DECODER         0x00013231
 */

/**
 * Decoder media format #AVS_MODULE_ID_DECODER module.
 * This parameter may be set runtime.
 */
#define AVS_DECODER_PARAM_ID_DEC_MEDIA_FMT 0x00013232

/**
 * Payload of the #AVS_DECODER_PARAM_ID_DEC_MEDIA_FMT parameter.
 * Immediately following this structure is an encoder configuration block of size media_fmt_size bytes.
 * Supported encoder configurations:
 *    #asm_aac_fmt_blk_v2_t
 *    Any open DSP defined structure
 */
typedef struct avs_dec_media_fmt_t avs_dec_media_fmt_t;
#include "adsp_begin_pack.h"
struct avs_dec_media_fmt_t
{
	uint32_t                  media_fmt_size;
	/**
	 * Size of the media format that follows this member @values @ge 0 bytes
	 */
}
#include "adsp_end_pack.h"
;

/**
 * Decoder format ID parameter for the #AVS_MODULE_ID_DECODER module.
 * This parameter cannot be set runtime.
 */
#define AVS_DECODER_PARAM_ID_DEC_FMT_ID 0x00013234

/**
 * Payload of the #AVS_DECODER_PARAM_ID_DEC_FMT_ID parameter.
 */
typedef struct avs_dec_fmt_id_param_t avs_dec_fmt_id_param_t;
#include "adsp_begin_pack.h"
struct avs_dec_fmt_id_param_t
{
	uint32_t                 dec_fmt_id;
	/** Supported values:
	 *            #ASM_MEDIA_FMT_AAC_V2
	 *            #Any OpenDSP supported value (custom).
	 */
}
#include "adsp_end_pack.h"
;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AVS_COMMON_API_H */
