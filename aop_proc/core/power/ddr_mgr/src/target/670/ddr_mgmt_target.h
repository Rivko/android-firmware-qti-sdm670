#ifndef DDR_MGMT_TARGET_H
#define DDR_MGMT_TARGET_H
/*===========================================================================
                             ddr_mgmt_target.h

DESCRIPTION:
  Contains target-specific defines, includes, enums etc. for ddr_mgr

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

// For testing with DDR team, define ALLOW_SHRM and ARC_TEST.
#define ALLOW_SHRM
//#define ARC_TEST

#define DDR_AUX_SEQ0_INTCLR_MSK (0x01)
#define DDR_AUX_SEQ1_INTCLR_MSK (0x02)
#define DDR_AUX_TCS_DONE_INTCLR_MSK (0x04)

#define BCM_ALC_CLOCK_DOMAIN    10
#define BCM_ACV_CLOCK_DOMAIN    11

#endif // DDR_MGMT_TARGET_H
