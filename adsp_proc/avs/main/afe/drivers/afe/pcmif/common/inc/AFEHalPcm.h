#ifndef __AFE_HAL_PCM_H__
#define __AFE_HAL_PCM_H__

/** @file HALafe.h
    This module contains Audio Hardware interface Abstraction layer function 
    declarations. This file publicizes enums defined by the HAL. 

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/common/inc/AFEHalPcm.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------
07/30/10    fzhou  Remove doxygen warning
02/02/10    FZ     Add digital mic support
12/10/09    AY     Win32 sim compile support
07/30/09    GY     Backport AFE/HAL unified API changes from dev/lpass branch
11/18/08    RA     Initial Creation

========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

#include "AFEHalDump.h"

/**
 * The following structures is used for I2S interface
 */
typedef struct pcm_config
{
   /** num of Slots max: 4*/
   uint32_t slot_num;              ///< num of slots max: 4
   /** Slot no. for data (0-31)*/
   uint32_t slot[4];                ///< Slot no. for data (0-31)
   /** Currently, only aux or pcm modes are used*/
   uint32_t frame;           ///< Currently, only aux or pcm modes are used
   /** MSM in master/slave mode*/
   uint32_t sync_src;          ///< MSM in master/slave mode
   /** Long sync / short sync pulse*/
   uint32_t sync;                ///< Long sync / short sync pulse
   /** Linear or Non-Linear*/
   uint32_t quant;       ///< Linear or Non-Linear.
   /** ctrl_data_oe for Multiple Masters */
   uint32_t ctrl_data_oe;

} __attribute__((aligned(4))) pcm_config_t;


void hal_pcm_init(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t pcm_offset);

void hal_pcm_reset(uint32_t pcm_index, uint32_t dir);

void hal_pcm_reset_release(uint32_t pcm_index, uint32_t dir);


void hal_pcm_config(uint32_t pcm_index, uint32_t dir, pcm_config_t *config_ptr);

void hal_pcm_enable(uint32_t pcm_index, uint32_t dir);

void hal_pcm_disable(uint32_t pcm_index, uint32_t dir);

uint32_t hal_pcm_get_intf(uint32_t pcm_index, uint32_t dir);

void hal_pcm_dump(uint32_t pcm_index, uint32_t dir, uint32_t dump_seq);

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AFE_HAL_PCM_H__)
