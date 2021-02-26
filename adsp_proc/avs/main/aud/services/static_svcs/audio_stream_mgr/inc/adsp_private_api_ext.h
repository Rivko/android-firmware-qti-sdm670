/*========================================================================
adsp_privatedefs.h

This file contains private ADSP APIs

Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/inc/adsp_private_api_ext.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/07/13   RB      Created

========================================================================== */
/**
@file adsp_privatedefs_ext.h

@brief This file contains private ADSP APIs
*/

#ifndef _ADSP_PRIVATEAPI_EXT_H_
#define _ADSP_PRIVATEAPI_EXT_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup adsp_privatedefs
    @{ */

/******************************************************************************* stream commands */

/** End point IDs to go in stream_cmd */
/** Identifies the source as matrix or stream router. */
#define ASM_SOURCE_MATRIX_STRTR  0
/** Identifies the source as client */
#define ASM_SOURCE_CLIENT  1

/** Identifies the sink as matrix or stream router. */
#define ASM_SINK_MATRIX_STRTR  0
/** Identifies the sink as client */
#define ASM_SINK_CLIENT  1

/** Stream types to go in stream_cmd */
/** Identifies as stream type unknown */
#define ASM_STREAM_TYPE_UNKNOWN 0
/** Identifies the primary/main audio PID. */
#define ASM_STREAM_TYPE_PRIMARY  1
/** Identifies the secondary/associated audio/descriptor audio PID */
#define ASM_STREAM_TYPE_SECONDARY  2
/** Identifies the mix of primary and secondary */
#define ASM_STREAM_TYPE_PRIMARY_PLUS_SECONDARY  3
/** Identifies the a bit stream with both primary and secondary */
#define ASM_STREAM_TYPE_PRIMARY_SECONDARY  4

/** event id for IEC 61937 media format event id using for registration between ASM and Dec services*/
#define ASM_STREAM_IEC_61937_MEDIA_FMT_EVENT_ID 0x0001321F
/** @} */ /* end_addtogroup adsp_privatedefs */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_PRIVATEAPI_EXT_H_ */
