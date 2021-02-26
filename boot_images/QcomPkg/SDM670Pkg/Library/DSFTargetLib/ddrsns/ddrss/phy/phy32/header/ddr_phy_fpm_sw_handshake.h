/***************************************************************************************************
 Auto-generated sequence C codes from Autoseq 3.1 3/24/2016
 User Name:psangaiy
 Input File:ddr_phy.blk
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

#ifndef _DDR_PHY_FPM_SW_HANDSHAKE_H
#define _DDR_PHY_FPM_SW_HANDSHAKE_H

#ifndef _DDR_PHY_FPM_SW_HANDSHAKE_SEQ_HEADER_
#define _DDR_PHY_FPM_SW_HANDSHAKE_SEQ_HEADER_
//#include "ddr_phy_seq_hwiobase.h"
//#include "ddr_phy_seq_hwioreg.h"
#include "HALhwio.h"
#include "HAL_SNS_DDR.h"
#include "ddr_ss_seq_hwiobase.h"
#include "ddr_ss_seq_hwioreg.h"
#include "target_config.h"



void DDR_PHY_ddr_phy_fpm_sw_hndshake( uint32 _inst_, uint32 period );
void DDR_PHY_poll_1( uint32 _inst_, uint8 poll );
void DDR_PHY_poll_2( uint32 _inst_, uint8 poll );

#endif
#endif
