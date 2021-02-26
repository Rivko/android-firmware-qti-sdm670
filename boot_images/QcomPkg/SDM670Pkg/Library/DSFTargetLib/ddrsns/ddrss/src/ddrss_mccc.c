 /****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================*/

#include "ddrss_mccc.h"

void MCCC_Config(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
    uint8 ch = 0;
    uint8 num_ch = ddr->shrm_ddr_ptr->misc.num_channel;
    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
    
    // MCCC MSTR config
    DDRSS_set_config(ddr, SEQ_DDR_SS_MCCC_MCCC_MSTR_OFFSET,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[MCCC_MSTR_CFG_IDX][0]));

    // Enable MCCC broadcast for MCCC SLV config
    ch = DDRSS_Broadcast_MCCC_Config(channel, NO_RDATA);

    do { 
        if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
            // MCCC SLV config
            DDRSS_set_config(ddr, REG_OFFSET_MCCC_SLV(ch),
                (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[MCCC_SLV_CFG_IDX][0]));
        }
        // Increase channel index to loop all channels selected in non-broadcast case
        ch++;
    } while (ch < num_ch);

    // Disable MCCC broadcast
    DDRSS_Broadcast_MCCC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
    
    // Enable MCCC HW clock gating as the last step in MCCC one-time settings
    HWIO_OUTXF (SEQ_DDR_SS_MCCC_MCCC_MSTR_OFFSET, MCCC_MSTR_CLOCK_GATE_CFG, MCCC_HW_CLKGATE_EN, 0x1);
}
  

