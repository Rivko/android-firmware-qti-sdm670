#ifndef __AFE_HAL_DUMP_H__
#define __AFE_HAL_DUMP_H__

/** @file HALafe.h
    This module contains Audio Hardware interface Abstraction layer function 
    declarations. This file publicizes enums defined by the HAL. 

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEHalDump.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#if defined(__cplusplus)
extern "C" {
#endif

#define AUDIOIF_REG_DUMP
#undef AUDIOIF_REG_DUMP

/** 
 * Define a new type for the interfaces supported by audio hardware
 */
enum
{
   LPA_IF_I2S = 0,
   LPA_IF_PCM, 
   LPA_IF_HDMI14,
};

//direction
enum
{
    LPA_IF_SINK = 0 ,
    LPA_IF_SOURCE,
};

// dump sequence
enum
{
    LPA_IF_START = 0 ,
    LPA_IF_STOP,
    LPA_IF_SEQ_MAX,
};


void hal_afe_dump_init(uint32_t version);

void hal_afe_dump_deinit(void);

void hal_afe_clock_dump(uint32_t intf_type, uint32_t intf_index, uint32_t dump_seq);

void hal_afe_audioif_dump(uint32_t intf_type, uint32_t intf_index, uint32_t dir, uint32 channel, uint32_t dump_seq);

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AFE_HAL_DUMP_H__)
