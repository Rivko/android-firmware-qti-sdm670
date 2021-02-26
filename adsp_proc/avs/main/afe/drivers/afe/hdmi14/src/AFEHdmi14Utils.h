#ifndef __AFEDALHDMIHELP_H__
#define __AFEDALHDMIHELP_H__

/** @file AFEDalHdmiHelp.h
    This module contains Audio HDMI specific struct, constant, enum and function declarations. 
    Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi14/src/AFEHdmi14Utils.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------
11/18/09          Initial Creation

========================================================================== */

#include "AFEHdmi14Driver_i.h"

ADSPResult afe_hdmi14_init_IEC60958_ch_status_table(hdmi14_port_state_t *p_dev_state);

ADSPResult afe_hdmi14_deinit_IEC60958_ch_status_table(hdmi14_port_state_t *p_dev_state);

void afe_hdmi14_copy_IEC60958_data(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst);

uint32_t afe_hdmi14_get_present_bits(uint32_t ca);


#endif  // !defined(__AFEDALHDMIHELP_H__)
