/***************************************************************************************************
 Auto-generated sequence C codes from Autoseq 2.4.4 2/2/2016
 User Name:fengding
 Input File:llcc.blk
 ***************************************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 
 All data and information contained in or disclosed by this document are confidential and
 proprietary information of Qualcomm Technologies Incorporated, and all rights therein are expressly
 reserved. By accepting this material, the recipient agrees that this material and the
 information contained therein are held in confidence and in trust and will not be used,
 copied, reproduced in whole or in part, nor its contents revealed in any manner to others
 without the express written permission of Qualcomm Technologies Incorporated.
 
 This technology was exported from the United States in accordance with the Export
 Administration Regulations. Diversion contrary to U. S. law prohibited.
 ***************************************************************************************************/

#ifndef _LLCC_H
#define _LLCC_H

#ifndef _LLCC_SEQ_HEADER_
#define _LLCC_SEQ_HEADER_
#include "llcc_seq_hwiobase.h"
#include "llcc_seq_hwioreg.h"
#include "ddr_ss_seq_hwiobase.h"
#include "ddr_ss_seq_hwioreg.h"
#include "ddr_common.h"
#include "ddrss.h"
//#define DUMMY                                                        0

#define NUM_PCB							6
#define ALL_PCB_MASK					0x3F
#define ACTIVITY_BASED_SLEEP_EN			0

#define REG_OFFSET_LLCC_COMMON(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_COMMON_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_COMMON_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_COMMON_OFFSET))

#define REG_OFFSET_LLCC_FEAC(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_FEAC_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_FEAC_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_FEAC_OFFSET))


#define REG_OFFSET_LLCC_FEWC(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_FEWC_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_FEWC_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_FEWC_OFFSET))

#define REG_OFFSET_LLCC_BERC(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_BERC_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_BERC_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_BERC_OFFSET))


#define REG_OFFSET_LLCC_FERC(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_FERC_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_FERC_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_FERC_OFFSET))

#define REG_OFFSET_LLCC_BIST(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_BIST_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_BIST_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_FERC_OFFSET))

#define REG_OFFSET_LLCC_TBUF(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_TBUF_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_TBUF_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_TBUF_OFFSET))

#define REG_OFFSET_LLCC_DRP(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_DRP_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_DRP_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_DRP_OFFSET))

#define REG_OFFSET_LLCC_TRP(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_TRP_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_TRP_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_TRP_OFFSET))

#define REG_OFFSET_LLCC_AMON(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_AMON_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_AMON_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_AMON_OFFSET))


#define REG_OFFSET_LLCC_BEAC(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_BEAC_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_BEAC_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_BEAC_OFFSET))

#define REG_OFFSET_LLCC_MPU(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_MPU_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_MPU_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_MPU_OFFSET))
										
#define REG_OFFSET_LLCC_PMGR(uint8)   ((uint8 == 0) ? \
                                        SEQ_DDR_SS_LLCC0_LLCC_PMGR_OFFSET : \
                                        (uint8 == 1) ? \
                                        SEQ_DDR_SS_LLCC1_LLCC_PMGR_OFFSET : \
                                        (SEQ_DDR_SS_LLCC_BROADCAST_LLCC_PMGR_OFFSET))										


#define LLCC_CH_INX(DDR_CHANNEL)        ((DDR_CHANNEL == DDR_CH0) ? \
                                         0: \
                                         (DDR_CHANNEL == DDR_CH1) ? \
                                         1: \
                                         (DDR_CHANNEL == DDR_CH2) ? \
                                         2: \
                                         (DDR_CHANNEL == DDR_CH3) ? \
                                         3: \
                                         4)


#define NUMBER_OF_SCIDS                        32


void LLCC_config (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);
void LLCC_init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);
void LLCC_initialize_tag_ram (DDR_CHANNEL channel);
void LLCC_configure_beac_credit (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);
void LLCC_cacheable_mode_override_ctrl (DDR_CHANNEL channel, CACHEABLE_MODE cachable_mode);
void LLCC_ram_init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);
void LLCC_PXI_flush (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16 start_inx, uint16 end_inx, uint16 way_vec);
void LLCC_PXI_TCM_mode_ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint8 act_en);

#endif
#endif
