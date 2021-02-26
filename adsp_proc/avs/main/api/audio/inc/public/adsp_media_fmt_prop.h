/*========================================================================*/
/**
@file adsp_media_fmt_prop.h

This file contains the structure definitions for the media format
blocks used in client-ASM communications.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/*========================================================================
Edit History

when       who     what, where, why
--------   ----    -------------------------------------------------------
08\17\16	HB		Create File.
========================================================================== */

#ifndef _ADSP_MEDIA_FMT_PROP_H_
#define _ADSP_MEDIA_FMT_PROP_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define ASM_MEDIA_FMT_DSD                0x00012f3e
#define ASM_MEDIA_FMT_ADPCM              0x00010BE7
#define ASM_MEDIA_FMT_MP3                0x00010BE9
#define ASM_MEDIA_FMT_WMA_V8             0x00010D91
#define ASM_MEDIA_FMT_WMA_V9_V2          0x00010DA8
#define ASM_MEDIA_FMT_WMA_V10PRO_V2      0x00010DA7
#define ASM_MEDIA_FMT_MIDI               0x00010BF1
#define ASM_MEDIA_FMT_DTMF               0x00010C2F

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_ADSP_MEDIA_FMT_PROP_H_*/
