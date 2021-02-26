#ifndef __AFE_HAL_AUIDOIF_COMMON_H__
#define __AFE_HAL_AUIDOIF_COMMON_H__

/** @file HALafe.h
    This module contains Audio Hardware interface Abstraction layer function 
    declarations. This file publicizes enums defined by the HAL. 

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEHalAudioIfCommon.h#1 $


========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

#include "AFEAudioIfCommon.h"
#include "AFEHalDump.h"


void hal_audioif_common_init(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t mux_offset);

void hal_audioif_common_config_mux(audioif_hw_t intf_type, uint32_t mux_index);

#ifdef AUDIOIF_REG_DUMP   
void hal_afe_audioif_mux_dump(uint32_t intf_index, uint32_t dump_seq);
#endif

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AFE_HAL_AUIDOIF_COMMON_H__)
