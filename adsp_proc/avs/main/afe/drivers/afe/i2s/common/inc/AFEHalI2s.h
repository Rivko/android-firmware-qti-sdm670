#ifndef __AFE_HAL_I2S_H__
#define __AFE_HAL_I2S_H__

/** @file HALafe.h
    This module contains Audio Hardware interface Abstraction layer function 
    declarations. This file publicizes enums defined by the HAL. 

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/inc/AFEHalI2s.h#1 $

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
  typedef struct i2s_config
  {

    /** Word select source signal (master/slave)*/
    bool_t  ws_master;            ///< Word select source signal (master/slave). 
    /** in bits (16/24/32) */
    int32 bit_width;                       ///< in bits (16/24/32).
    /** SD0, SD1, SD2, and SD3 configuration, 8660 only)*/
    uint32_t  channel_mode;       ///< SD0, SD1, SD2, and SD3 configuration, 8660 only
    /** stereo or mono mode */
    bool_t    stereo_mode;    ///< stereo or mono mode
  } __attribute__((aligned(4))) i2s_config_t;


  void hal_i2s_init(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t i2s_offset, uint32_t int_i2s_offset);

  void hal_i2s_config(uint32_t i2s_index, uint32_t dir, i2s_config_t *config_ptr);

  void hal_i2s_enable(uint32_t i2s_index, uint32_t dir);

  void hal_i2s_disable(uint32_t i2s_index, uint32_t dir);

  uint32_t hal_i2s_get_intf(uint32_t i2s_index, uint32_t dir);

  void hal_i2s_dump(uint32_t intf_index, uint32_t dir, uint32_t dump_seq);

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AFE_HAL_I2S_H__)
