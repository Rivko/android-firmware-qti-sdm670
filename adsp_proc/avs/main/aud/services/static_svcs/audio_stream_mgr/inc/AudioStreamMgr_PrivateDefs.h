/*========================================================================
AudioStreamMgr_PrivateDefs.h

This file contains ASM commands that are currently not exposed via public APIs.

Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/inc/AudioStreamMgr_PrivateDefs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/25/11   RP      Created

========================================================================== */
/**
@file AudioStreamMgr_PrivateDefs.h

@brief This file contains ASM commands that are currently not exposed via public APIs.
*/
#include "adsp_private_api_ext.h"

#ifndef _AUDIOSTREAMMGR_PRIVATEDEFS_H_
#define _AUDIOSTREAMMGR_PRIVATEDEFS_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

   /*Important: ASM now supports 15 concurrent audio sessions
    * For backward compatibility purposes the macro in the public api is not changed*/
#define ASM_MAX_SUPPORTED_SESSION_ID                              (15)
#define ASM_MAX_NUM_SUPPORTED_SESSIONS                            (15)

/** Number of streams supported per session == max stream id.*/
#define ASM_MAX_SUPPORTED_STREAM_ID                               (8)

#define ASM_MAX_SUPPORTED_NUM_CHANNELS                            (8)
#define ASM_MAX_SUPPORTED_SAMPLING_RATE_HZ                        (384000)
#define ASM_MAX_SUPPORTED_BITWIDTH                                (24)

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _AUDIOSTREAMMGR_PRIVATEDEFS_H_ */
