#ifndef __AFE_HAL_HDMI14_H__
#define __AFE_HAL_HDMI14_H__

/** @file HALafe.h
    This module contains Audio Hardware interface Abstraction layer function 
    declarations. This file publicizes enums defined by the HAL. 

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi14/src/AFEHalHdmi14.h#1 $

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

  /* The following type used as a generic flag to represent enable/disable of a module */
  typedef enum
  {
    DISABLE = 0,
    ENABLE = 1
  } hdmi14_mod_en_state_t;

void hal_hdmi14_init(uint32_t version, uint32_t phy_base, uint32_t virt_base);

void hal_hdmi14_disable_core(void);

void hal_hdmi14_enable_core(void);

void hal_hdmi14_enable_hw_pack(void);

void hal_hdmi14_disable_hw_pack(void);

void hal_hdmi14_set_packet_info(void* ch_status_table_ptr, uint32_t validity_bit);


void hal_hdmi14_set_pkt_ctl_cs_60958_source_hwd(void);

void hal_hdmi14_set_pkt_ctl_channel_enable_layout1(void);
  

void hal_hdmi14_set_pkt_ctl_cs_60958_update(void);

void hal_hdmi14_sample_present_bit_layout1(uint32_t present_bits);

void hal_hdmi14_reset_sample_present_bit(void);

void hal_hdmi14_disable_packet_ctl(void);

void hal_hdmi14_enable_packet_ctl(void);

uint32 hal_hdmi14_get_intf(void);

/**
 * Configures High bit rate (HBR) enable bit, to enable/disable
 * high bit rate for compressed pass-through use case.
 *
 * @param[in] enable flag that indicates to enable or disable HBR
 *
 * @return  None
 *
 * @dependencies
 * None.
 */
void hal_hdmi14_enable_disable_hbr(hdmi14_mod_en_state_t enable);

#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AFE_HAL_HDMI14_H__)
