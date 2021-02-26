/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S.
  Government. Diversion contrary to U.S. law prohibited. 
==============================================================================*/
#ifndef ELITE_PCM_CH_DEFS_H
#define ELITE_PCM_CH_DEFS_H

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** @addtogroup asm_strm_media_format_pcm_ch_defs
@{ */

#define PCM_CHANNEL_NULL 0
    /**< @xreflabel{hdr:PcmChDefNull}  Not used.

         This speaker position enumeration is maintained for backward
         compatibility. Currently, there are no devices that support this type.
         The client is responsible for ensuring that valid and existing channel
         mapping configurations corresponding to real speaker positions are
         used to ensure proper user experience without audio/data losses. */

/** @xreflabel{hdr:PcmChDefL}  Front left channel. */
#define PCM_CHANNEL_L    1

/** @xreflabel{hdr:PcmChDefR}  Front right channel. */
#define PCM_CHANNEL_R    2

/** @xreflabel{hdr:PcmChDefC}  Front center channel. */
#define PCM_CHANNEL_C    3

/** @xreflabel{hdr:PcmChDefLS}  Left surround channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_LS   4

/** @xreflabel{hdr:PcmChDefRS}  Right surround channel.
  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_RS   5

/** @xreflabel{hdr:PcmChDefLFE}  Low frequency effect channel. */
#define PCM_CHANNEL_LFE  6

/** @xreflabel{hdr:PcmChDefCS}  Center surround channel; Rear center channel. */
#define PCM_CHANNEL_CS   7

/** @xreflabel{hdr:PcmChDefLB}  Left back channel; Rear left channel. */
#define PCM_CHANNEL_LB   8

/** @xreflabel{hdr:PcmChDefRB}  Right back channel; Rear right channel. */
#define PCM_CHANNEL_RB   9 

/** @xreflabel{hdr:PcmChDefTS}  Top surround channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_TS   10

/** @xreflabel{hdr:PcmChDefCVH}  Center vertical height channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_CVH  11

/** @xreflabel{hdr:PcmChDefMS}  Mono surround channel.

  This speaker position enumeration is maintained for backward compatibility.
  Currently, there are no devices that support this type. The client is
  responsible for ensuring that valid and existing channel mapping
  configurations corresponding to real speaker positions are used to ensure
  proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_MS   12

/** @xreflabel{hdr:PcmChDefFLC}  Front left of center. */
#define PCM_CHANNEL_FLC  13

/** @xreflabel{hdr:PcmChDefFRC}  Front right of center. */
#define PCM_CHANNEL_FRC  14

/** @xreflabel{hdr:PcmChDefRLC}  Rear left of center. */
#define PCM_CHANNEL_RLC  15

/** @xreflabel{hdr:PcmChDefRRC}  Rear right of center.
    @newpage */
#define PCM_CHANNEL_RRC  16

/** @} */ /* end_addtogroup asm_strm_media_format_pcm_ch_defs */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ELITE_PCM_CH_DEFS_H
