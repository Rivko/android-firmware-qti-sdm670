/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                              EDIT HISTORY
$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/common/target/SDM670/header/ddr_phy_technology.h#3 $
$DateTime: 2018/02/15 02:48:22 $
$Author: pwbldsvc $

==============================================================================*/

#define VREF_MP_HP                  2
#define PINS_PER_PHY_CONNECTED_CA   6
#define NUM_CA_PHY_BIT             10

#define DSF_LPDDR4Y_SUPPORTED       1

#define CA_PATTERN_NUM              4 

#define COARSE_VREF_MAX_VALUE       0x37
#define COARSE_VREF_MAX_VALUE_WRITE 0x32 //Limit VREF to max number of possible steps.
#define COARSE_CDC_MAX_VALUE        0x18
#define MIN_WIDTH                   0x1
#define COARSE_VREF                 (COARSE_VREF_MAX_VALUE + 1)
#define COARSE_CDC                  (COARSE_CDC_MAX_VALUE + 1)

#define FINE_VREF_MAX_VALUE         0x8 
#define FINE_CDC_MAX_VALUE          0x8
#define FINE_VREF                   (FINE_VREF_MAX_VALUE + 1)
#define FINE_CDC                    (FINE_CDC_MAX_VALUE + 1)
#define FINE_RD_CDC                 0x10
#define PERBIT_CDC_MAX_VALUE        0xF 
#define DBI_WR_PBIT_OFFSET          0

#define COARSE_STEP_IN_PS           60 // 58ps at high voltage
#define FINE_STEP_IN_PS             9  // 9.5ps at high voltage
#define FINE_STEPS_PER_COARSE       6  // changed for periodic training purposes, was 7

#define WRITE_FINE_CDC_MIN          5
#define WRITE_FINE_CDC_MAX          10
#define WRITE_TERMINATED_MR14_VREF  0x19
#define WRITE_UNTERMINATED_MR14_VREF 0x4B
#define MAX_TDQS2DQ_IN_PS           800

#define VREF_RD_MP_STEP_IN_MV 9 
#define VREF_RD_MP_OFFSET_IN_MV 40 
#define VREF_RD_HP_STEP_IN_MV 21 
#define VREF_RD_HP_OFFSET_IN_MV 55 
#define VREF_WR_LP4x_STEP_IN_MV (3.6) // LP4x DRAM side: (0.6% * VDDQ) 
#define VREF_WR_LP4x_OFFSET_IN_MV 90 // 15% first value // only LP4x  need to add also LP4
#define DQS_VREF                    0x8 //0xF
//#define MIN_DTTS_TRACKING_PRFS      0x6

#define LOW_SPEED_HALF_CYC_CONV_THRESHOLD       400  //((COARSE_CDC_MAX_VALUE * COARSE_STEP_IN_PS)/2) //=600ps.
#define LOW_SPEED_NO_HALF_CYC_COARSE_CDC_VALUE  0x10 //0x12 //0x14 //0x10
#define LOW_SPEED_NO_HALF_CYC_FINE_CDC_VALUE    0x0
#define LOW_SPEED_RD_COARSE_CDC_VALUE           0xA


#define F_RANGE_0                   250000
#define F_RANGE_1                   600000
#define F_RANGE_2                   700000
#define F_RANGE_3                   900000
#define F_RANGE_4                   1150000
#define F_RANGE_5                   1500000
#define F_RANGE_6                   1700000
#define F_RANGE_7                   2100000


#define VREF_STEPS                80
#define HISTOGRAM_SIZE            (COARSE_CDC_MAX_VALUE * FINE_STEPS_PER_COARSE)
#define COARSE_L                   0
#define FINE_L                     1
#define DELAY_L                    2
#define CDC_L                      3
//================================================================================================
