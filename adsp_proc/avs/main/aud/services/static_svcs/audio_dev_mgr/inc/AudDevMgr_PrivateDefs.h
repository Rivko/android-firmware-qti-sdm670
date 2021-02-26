/*========================================================================
AudDevMgr_PrivateDefs.h

This file contains ADM commands that are currently not exposed via public APIs.

Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/inc/AudDevMgr_PrivateDefs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/17/12   KR      Created

========================================================================== */
/**
@file AudDevMgr_PrivateDefs.h

@brief This file contains ADM commands that are currently not exposed via public APIs.
*/

#ifndef _AUDDEVMGR_PRIVATEDEFS_H_
#define _AUDDEVMGR_PRIVATEDEFS_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

//Defining internal topology IDs for default COPP topology in either direction
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_RX 0xE0010BE3
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_TX 0xF0010BE3

//Defining internal topology ID for topology containing no modules for uncompressed data
#define AUDPROC_TOPOLOGY_ID_UNCOMPRESSED_NONE                         0xE0010774

/** Definition for the ADM native mode channels bitmask. */
#define ADM_BIT_MASK_NATIVE_MODE                                  (0x000000C0UL)

/** Definition for the ADM native mode channels shift value. */
#define ADM_BIT_SHIFT_NATIVE_MODE_CHANNELS                            7

/** Definition for the ADM native mode bit width shift value. */
#define ADM_BIT_SHIFT_NATIVE_MODE_BIT_WIDTH                           6

/** @} */ /* end_addtogroup adsp_privatedefs */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _AUDDEVMGR_PRIVATEDEFS_H_ */
