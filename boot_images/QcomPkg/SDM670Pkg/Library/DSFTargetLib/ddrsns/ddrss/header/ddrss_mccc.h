 /****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================*/

#ifndef __DDRSS_MCCC_H__
#define __DDRSS_MCCC_H__

#include "HAL_SNS_DDR.h"
#include "mccc_seq_hwiobase.h"
#include "mccc_seq_hwioreg.h"
#include "ddrss.h"


#define REG_OFFSET_MCCC_SLV(uint8)      ((uint8 == 0) ? \
                                        (SEQ_DDR_SS_MCCC_CH0_MCCC_SLV_OFFSET) : \
                                        (uint8 == 1) ? \
                                        (SEQ_DDR_SS_MCCC_CH1_MCCC_SLV_OFFSET) : \
                                        (uint8 == 2) ? \
                                        (SEQ_DDR_SS_MCCC_CH2_MCCC_SLV_OFFSET) : \
                                        (uint8 == 3) ? \
                                        (SEQ_DDR_SS_MCCC_CH3_MCCC_SLV_OFFSET) : \
                                        (SEQ_DDR_SS_MCCC_BROADCAST_MCCC_SLV_OFFSET)) 

/**
 * @brief MCCC one-time settings
 * 
 * @param[in] BOOT_DDR_STRUCT *ddr: Pointer to ddr conifiguration struct
 * @param[in] DDR_CHANNEL channel:  Channel selection
 * 
 * @retval None
 */  
void MCCC_Config(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);

#endif // #ifdef _MCCC_H_
