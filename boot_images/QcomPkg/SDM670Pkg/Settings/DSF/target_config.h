#ifndef TARGET_CONFIG_H
#define TARGET_CONFIG_H

/***************************************************************************************************

 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.

 ***************************************************************************************************/
// DDR System Firmware Version
#define TARGET_DDR_SYSTEM_FIRMWARE_MAJOR_VERSION     32
#define TARGET_DDR_SYSTEM_FIRMWARE_MINOR_VERSION     2

#define TARGET_DDR_SYSTEM_FIRMWARE_MAJOR_VERSION_SHIFT     16
#define TARGET_DDR_SYSTEM_FIRMWARE_MINOR_VERSION_SHIFT      0
#define TARGET_DDR_SYSTEM_FIRMWARE_VERSION   ((TARGET_DDR_SYSTEM_FIRMWARE_MAJOR_VERSION << TARGET_DDR_SYSTEM_FIRMWARE_MAJOR_VERSION_SHIFT) \
                                            | (TARGET_DDR_SYSTEM_FIRMWARE_MINOR_VERSION << TARGET_DDR_SYSTEM_FIRMWARE_MINOR_VERSION_SHIFT))
					    
											

// Define silicon target or emulation target. Some portions of the DDR System Firmware will or will not
// be compiled depending on what this macro is defined as. 
// If TARGET_SILICON is defined as a 0, it implies an emulation build.
// If TARGET_SILICON is defined as a 1, it implies a real-silicon build.
// If TARGET_SILICON is already defined (on the compiler command line), do not override that definition.
//    This will allow users to override this emulation/silicon flag at compile time without changing the DSF.
#ifndef TARGET_SILICON
#define TARGET_SILICON                         1 
#endif // #ifndef TARGET_SILICON

// BIMC and PHY Core Architecture, Major and Minor versions.
#define TARGET_MC_ARCH_VERSION                2
#define TARGET_MC_CORE_MAJOR_VERSION          4
#define TARGET_MC_CORE_MINOR_VERSION          0
#define TARGET_PHY_CORE_MAJOR_VERSION         3
#define TARGET_PHY_CORE_MINOR_VERSION         2
#define TARGET_LLCC_CORE_MAJOR_VERSION        1
#define TARGET_LLCC_CORE_MINOR_VERSION        0
#define TARGET_SHRM_CORE_MAJOR_VERSION        1
#define TARGET_SHRM_CORE_MINOR_VERSION        0

//Enable this macro while simulating DSF in DV environment
#define TARGET_DSF_DV_SIMULATION              0

// Define DDR base address in the chip address map from APPS viewpoint.
//    From the RPM viewpoint, DDR is also at 0x80000000, but this address
//    is remapped to 0x60000000 by setting RPM_PAGE_SELECT to 2.
#define DDR_BASE                         0x80000000

#define DDR_SS_BASE                 0x01100000

// Size of statically allocated area in ZI data area that training functions will
// use in place of local variables to reduce stack growth pressure.
// Used in the function HAL_DDR_Boot_Training().
#define LOCAL_VARS_AREA_SIZE            0x10000 /* 64KB */

// The flat 32-bit system address from APPS viewpoint where PHY training code will write
// training patterns to and read back from. Training addresses currently are hard-coded to assume
// that System Address Bit 10 is the interleave bit, hence the address for channel 1 is 0x00000400.
#define TRAINING_BASE_ADDRESS_OFFSET     0x10000000   
#define TRAINING_INTERLEAVE_ADDRESS_OFFSET  0x00000400
#define MIN_EYE_WIDTH    60
#define MAX_SKEW_RANGE   1200

#define DSF_MIN_SUPPORTED_DDR_TRAINING_FREQ     547200

#define DSF_PERIODIC_TRAINING_EN                     1
#define DSF_MIN_PERIODIC_TRAINING_FREQ          1555200

#define DSF_PXI_TRAINING_EN				             0
#define DSF_PLLPRECAL_TRAINING_LOG_IN                0
#define DSF_CA_VREF_TRAINING_EN                      1
#define DSF_WRLVL_TRAINING_EN                        1
#define DSF_WRLVL_TRAINING_LOG                       0
#define DSF_RCW_TRAINING_EN                          1
#define DSF_RCW_TRAINING_LOG                         0

#define DSF_DCC_TRAINING_DQ_DCC_LOG                  0    
#define DSF_DCC_TRAINING_CA_DCC_LOG                  0 

#define DSF_SMALL_EYE_DETECTION_LOG                  0
#define DSF_TRAINING_LOG_EN                          0   
#define DSF_RD_DQDQS_TRAINING_EN                     1
#define DSF_RD_DQDQS_TRAINING_LOG_IN                 1 & DSF_RD_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN
#define DSF_RD_DQDQS_TRAINING_LOG_HIST_IN            0 & DSF_RD_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN
#define DSF_RD_DQDQS_TRAINING_LOG_BEFORE             0 & DSF_RD_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN
#define DSF_RD_DQDQS_TRAINING_LOG_AFTER              0 & DSF_RD_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN

#define DSF_RD_DQDQS_COARSE_FINE_PLOT_PIVOT          0  
//#define DSF_RD_DQDQS_COARSE_FINE_PLOT_FREQ           1804800

#define DSF_RD_DQDCC_EN                              0                     
#define DSF_RD_HPVREF_TRAINING_EN                    1
#define DSF_RD_HPVREF_TRAINING_LOG_IN                1 & DSF_RD_HPVREF_TRAINING_EN  & DSF_TRAINING_LOG_EN                                  
#define DSF_RD_HPVREF_TRAINING_LOG_HIST_IN	         0 & DSF_RD_HPVREF_TRAINING_EN  & DSF_TRAINING_LOG_EN & DSF_RD_HPVREF_TRAINING_LOG_IN  
#define DSF_RD_HPVREF_TRAINING_LOG_BEFORE            0 & DSF_RD_HPVREF_TRAINING_EN  & DSF_TRAINING_LOG_EN                                  
#define DSF_RD_HPVREF_TRAINING_LOG_AFTER             0 & DSF_RD_HPVREF_TRAINING_EN  & DSF_TRAINING_LOG_EN                                  

#define DSF_RD_MPVREF_TRAINING_EN                    1
#define DSF_RD_MPVREF_TRAINING_LOG_IN                1 & DSF_RD_MPVREF_TRAINING_EN & DSF_TRAINING_LOG_EN                                   
#define DSF_RD_MPVREF_TRAINING_LOG_HIST_IN           0 & DSF_RD_MPVREF_TRAINING_EN & DSF_TRAINING_LOG_EN  & DSF_RD_MPVREF_TRAINING_LOG_IN  
                                                                                                          
#define DSF_WR_DQDQS_TRAINING_TDQS2DQ_CAL_LOG        0                                                                                                          
#define DSF_WR_DQDQS_TRAINING_EN                     1                                                    
#define DSF_WR_DQDQS_TRAINING_LOG_IN                 1  & DSF_WR_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN                                   
#define DSF_WR_DQDQS_TRAINING_LOG_HIST_IN            0  & DSF_WR_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN  & DSF_WR_DQDQS_TRAINING_LOG_IN   
#define DSF_WR_DQDQS_TRAINING_LOG_AFTER              0  & DSF_WR_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN                                   
#define DSF_WR_DQDQS_TRAINING_LOG_BEFORE             0  & DSF_WR_DQDQS_TRAINING_EN & DSF_TRAINING_LOG_EN                                   

#define DSF_WR_DQDQS_COARSE_FINE_PLOT_PIVOT          0  // Warning: The Pivot plot takes ~5 min
//#define DSF_WR_DQDQS_COARSE_FINE_PLOT_FREQ           1804800

#define DSF_WR_DQDQS_TRAINING_TDQS2DQ_CAL_EN         1    //WR dqdqs training for band0.


enum ddr_Training_enable_index
{
    _RD_P       = 0,
    _WR_P       = 1,
  _DCC_Mon  = 0,
  _WRLVL_CF = 0,
  _C_CF     = 0, 
  _C_PB     = 1, 
  _DCC      = 2, 
  _V_FSP1   = 3, 
  _V_FSP0   = 4, 
};
//DDRSS_AHB2PHY0_AHB2PHY_BROADCAST_EN_CFG_LOWER
//BROADCAST MASK0
// ----------------------------------------------------------------------
// |CH_01 CC|     CH1     |      CH0     |
// ----------------------------------------------------------------------
//     { CC}| {DQ1|DQ0|CA}| {DQ1|DQ0| CA}|
// ----------------------------------------------------------------------
//     {  6}|{  5|  4 | 3}| {  2|  1|  0}|

//DDRSS_AHB2PHY1_AHB2PHY_BROADCAST_EN_CFG_LOWER
//BROADCAST MASK1
// ----------------------------------------------------------------------
// |CH_23 CC|     CH3     |      CH2     |
// ----------------------------------------------------------------------
//     { CC}| {DQ1|DQ0|CA}| {DQ1|DQ0| CA}|
// ----------------------------------------------------------------------
//     {  6}|{  5|  4 | 3}| {  2|  1|  0}|

#define PHY_BC_DISABLE         0x0000

#define DQ_DBI_BIT     0xFF
#define DQ_UNCONNECTED 0x1F
#define CA_UNCONNECTED 0x1F

//Channel 0                 
#define CS_CH0_DDRPHY0_CA0      0x0001 
#define CS_CH0_DDRPHY0_DQ0      0x0002 
#define CS_CH0_DDRPHY0_DQ1      0x0004 

//Channel 1                               
#define CS_CH1_DDRPHY0_CA0      0x0008  
#define CS_CH1_DDRPHY0_DQ0      0x0010  
#define CS_CH1_DDRPHY0_DQ1      0x0020  

#define CS_CH01_DDRCC          0x0040  

//Channel 2                 
#define CS_CH2_DDRPHY1_CA0      0x0001 
#define CS_CH2_DDRPHY1_DQ0      0x0002 
#define CS_CH2_DDRPHY1_DQ1      0x0004 

//Channel 3                 
#define CS_CH3_DDRPHY1_CA0      0x0008 
#define CS_CH3_DDRPHY1_DQ0      0x0010 
#define CS_CH3_DDRPHY1_DQ1      0x0020 

#define CS_CH23_DDRCC          0x0040 



//All Channels

#define ALL_PHY0_CAs    (CS_CH0_DDRPHY0_CA0 | CS_CH1_DDRPHY0_CA0)

#define ALL_PHY1_CAs    (CS_CH2_DDRPHY1_CA0 | CS_CH3_DDRPHY1_CA0)                     
                    
#define ALL_PHY0_DQs    (CS_CH0_DDRPHY0_DQ0 | CS_CH0_DDRPHY0_DQ1 | CS_CH1_DDRPHY0_DQ0 | CS_CH1_DDRPHY0_DQ1 )  

#define ALL_PHY1_DQs    (CS_CH2_DDRPHY1_DQ0 | CS_CH2_DDRPHY1_DQ1 | CS_CH3_DDRPHY1_DQ0 | CS_CH3_DDRPHY1_DQ1 )
                    
#define ALL_PHY0_CCs    (CS_CH01_DDRCC )

#define ALL_PHY1_CCs    (CS_CH01_DDRCC )

//#define ALL_CAsDQs  (CS_CH0_DDRPHY0_CA0 | CS_CH1_DDRPHY0_CA0 | \
//                     CS_CH2_DDRPHY1_CA0 | CS_CH3_DDRPHY1_CA0 | \
//                     CS_CH0_DDRPHY0_DQ0 | CS_CH0_DDRPHY0_DQ1 | CS_CH1_DDRPHY0_DQ0 | CS_CH1_DDRPHY0_DQ1 | \
//                     CS_CH2_DDRPHY1_DQ0 | CS_CH2_DDRPHY1_DQ1 | CS_CH3_DDRPHY1_DQ0 | CS_CH3_DDRPHY1_DQ1 )  

#define ALL_PHY0_CAsDQs  (CS_CH0_DDRPHY0_CA0 | CS_CH1_DDRPHY0_CA0 | \
                          CS_CH0_DDRPHY0_DQ0 | CS_CH0_DDRPHY0_DQ1 | CS_CH1_DDRPHY0_DQ0 | CS_CH1_DDRPHY0_DQ1)  

#define ALL_PHY1_CAsDQs  (CS_CH2_DDRPHY1_CA0 | CS_CH3_DDRPHY1_CA0 | \
                          CS_CH2_DDRPHY1_DQ0 | CS_CH2_DDRPHY1_DQ1 | CS_CH3_DDRPHY1_DQ0 | CS_CH3_DDRPHY1_DQ1 )                       

#endif /* TARGET_CONFIG_H */


