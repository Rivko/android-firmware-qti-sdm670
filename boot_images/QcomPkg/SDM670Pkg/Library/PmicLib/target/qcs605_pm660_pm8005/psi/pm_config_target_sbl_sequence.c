 /*! \file  pm_config_target_sbl_sequence.c 
 *   
 *  \brief  File Contains the PMIC Set Mode Driver Implementation 
 *  \details Set Mode Driver implementation is responsible for setting and getting 
 *  all mode settings such as Register values, memory values, etc. 
 *   
 *    PMIC code generation Version: 1.0.0.0 
 *    Date: 2019-01-31 
 *    PMIC PSI Version: QCS605-SBL-20190131-b0x06_v0x08 - Approved 
 *    PBS RAM Version: PME605_RAM_b0x06_v0x05 
 *    PBS RAM Version: PM8005_RAM_b0x06_v0x01 
 *    This file contains code for Target specific settings and modes. 
 *   
 *  &copy; Copyright 2018 Qualcomm Technologies Incorporated, All Rights Reserved 
 */ 
 
/*=========================================================================== 
 
                        EDIT HISTORY FOR MODULE 
 
  This document is created by a code generator, therefore this section will 
  not contain comments describing changes made to the module. 
 
$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PmicLib/target/qcs605_pm660_pm8005/psi/pm_config_target_sbl_sequence.c#8 $  
$DateTime: 2019/02/13 01:11:23 $  $Author: pwbldsvc $ 
 
when       who     what, where, why 
--------   ---     ----------------------------------------------------------  
 
===========================================================================*/ 
 
/*=========================================================================== 
 
                     INCLUDE FILES  
 
===========================================================================*/ 
 
#include "pm_target_information.h" 
#include "pm_config_sbl.h" 
 
/*========================== PSI Sequence LUT =============================*/ 
 
//NOTES ON CREATING PSI SEQUENCE: 
 
//1. When creating PSI sequence(Table A), if configuration do not need conditional check, set cond start/end index to 0 
//2. If Reg configuration require conditional check, indicate the range of conditional check using cond start/end index 
//3. For Reg operation PM_SBL_DELAY case, address field contains delay amount in us 
//4. For Reg Operation PM_SBL_PBS_RAM case, data field contains index to access in PBS RAM Table 
//5. For Reg Operation PM_SBL_PBS_RAM case, address field contains size of PBS RAM 
 
//Table B (Conditional configuration check Table): 
//1. DO NOT use the first entry. Index 0 is used when no conditional configuration is needed (This is just to avoid confusion) 
//2. Single or multiple configuration can be added in Table B 
//3. If multiple configuration is needed, it should be entered in a consecutive order, so that it can be indexed in one range(from Table A) 
 
pm_sbl_config_info_type
pm_sbl_config_info = 
{
 	PSI_SIGNATURE,	// PSI Signature
 	0x01,	// PSI Major Version
 	0x02,	// PSI Minor Version
 	0x01,	// Number of target configurations
 	0x17	// Total number of conditional entries on pm_sbl_cond_seq_type table
 };
pm_sbl_cond_seq_type 
pm_sbl_cond_seq [ ] = 
{	
	 // sid 	data 	mask 	register 	operator	 
	 { 0,	0x2B,	0xFF,	0x1358,	EQUAL}, 	//1
	 { 0,	0x6A,	0xFF,	0x1358,	EQUAL}, 	//2
	 { 4,	0x0F,	0x0F,	0x704C,	EQUAL}, 	//3
	 { 4,	0x00,	0x0F,	0x704C,	EQUAL}, 	//4
	 { 4,	0x01,	0x0F,	0x704C,	EQUAL}, 	//5
	 { 4,	0x02,	0x0F,	0x704C,	EQUAL}, 	//6
	 { 4,	0x03,	0x0F,	0x704C,	EQUAL}, 	//7
	 { 4,	0x09,	0x0F,	0x704C,	EQUAL}, 	//8
	 { 4,	0x0A,	0x0F,	0x704C,	EQUAL}, 	//9
	 { 4,	0xF0,	0xF0,	0x704C,	EQUAL}, 	//10
	 { 4,	0x00,	0xF0,	0x704C,	EQUAL}, 	//11
	 { 4,	0x10,	0xF0,	0x704C,	EQUAL}, 	//12
	 { 4,	0x20,	0xF0,	0x704C,	EQUAL}, 	//13
	 { 4,	0x30,	0xF0,	0x704C,	EQUAL}, 	//14
	 { 4,	0x90,	0xF0,	0x704C,	EQUAL}, 	//15
	 { 4,	0xA0,	0xF0,	0x704C,	EQUAL}, 	//16
	 { 4,	0x0F,	0x0F,	0x704D,	EQUAL}, 	//17
	 { 4,	0x00,	0x0F,	0x704D,	EQUAL}, 	//18
	 { 4,	0x01,	0x0F,	0x704D,	EQUAL}, 	//19
	 { 4,	0x02,	0x0F,	0x704D,	EQUAL}, 	//20
	 { 4,	0x03,	0x0F,	0x704D,	EQUAL}, 	//21
	 { 4,	0x09,	0x0F,	0x704D,	EQUAL}, 	//22
	 { 4,	0x0A,	0x0F,	0x704D,	EQUAL}, 	//23
 };
pm_sbl_seq_type 
pm_sbl_seq [ ] = 
{
	 //Mode - PBS_CONFIG_PME605 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xC0,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0,	     0x0200,	 0xFF,	 PM_SBL_PBS_RAM, 0,	 0}, 	  
	 { 0,	 0x00,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7050,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7051,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x0C,	 0x7054,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7055,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x10,	 0x7058,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7059,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x14,	 0x705C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x705D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x70D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x70DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7240,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7241,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7340,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7341,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7440,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7441,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xFF,	 0x7248,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xFF,	 0x7348,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xFF,	 0x7448,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x7246,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x7346,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x7446,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - GROOT_PON_INIT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0xA5,	 0x2CD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x05,	 0x2CDA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x08D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xED,	 0x0884,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x08D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xAB,	 0x0886,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x087F,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x48,	 0x3C42,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - GROOT_INIT_GENERAL 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x80,	 0x3746,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x3747,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - GROOT_PBUS_LOGGER_RESET_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0xA5,	 0x04D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x04DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - RESET_CONFIG_PME605 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x74,	 0x0880,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300	 
	 { 0,	 0x08,	 0x0842,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300
	 { 0,	 0x80,	 0x0843,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300	 
	 { 0,	 0xF4,	 0x0880,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300	 
	 { 0,	 0x00,	 0x085B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x0096,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //150	 
	 { 0,	 0x08,	 0x085A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x0096,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //150
	 { 0,	 0x80,	 0x085B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - GROOT_INTERRUPT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x80,	 0x0546,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - BUA 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x16,	 0x1C40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0xC640,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0xC644,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x22,	 0xC645,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0xC646,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x1C46,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - CLOCKS_PME605_STANDALONE 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x40,	 0x6044,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x0D,	 0x5058,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x5059,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x505E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x5046,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5048,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x59D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5941,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x59D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x5944,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x514C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x524C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x534C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x03,	 0x544C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x03,	 0x554C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5447,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x5043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5443,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x5444,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x6046,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x5A5B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x5448,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x5548,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - CLOCKS_PME605_COMMON 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0xA5,	 0x59D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x34,	 0x5940,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5346,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x5B43,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x54D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x54E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x55D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x55E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x51D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x51E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x52D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x52E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x53D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x53E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - LDO_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0x07,	 0x4149,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4249,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4449,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4549,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4649,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4849,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4949,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4B49,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - LDO_STEPPER_RATE 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0x82,	 0x4161,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x82,	 0x4261,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x82,	 0x4461,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x82,	 0x4561,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x82,	 0x4661,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - LDO_OCP 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xD4,	 0x4066,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4166,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4566,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4666,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4766,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4966,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4C66,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x5066,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x5166,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x5266,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - RPMH_INIT_PIN_CTRL_CONFIGS 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x01,	 0x0970,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x22,	 0x4247,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x22,	 0x4847,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - LDO_VSET 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0x20,	 0x4040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x4041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xB0,	 0x4140,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x4141,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x18,	 0x4240,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x4241,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xB0,	 0x4440,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x4441,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x70,	 0x4540,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x4541,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x20,	 0x4640,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x4641,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4740,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4741,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4840,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4841,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4940,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4941,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4A40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4A41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4B40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4B41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4C40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4C41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4D40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x4D41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xF0,	 0x4E40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0A,	 0x4E41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xC0,	 0x4F40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0B,	 0x4F41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x10,	 0x5040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0C,	 0x5041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x90,	 0x5140,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0B,	 0x5141,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x90,	 0x5240,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0B,	 0x5241,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - LDO_ULS_VSET 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x42D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x50,	 0x4268,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x4269,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - SMPS_VSET 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xF0,	 0x1440,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x02,	 0x1441,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x20,	 0x1740,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x1741,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x58,	 0x1A40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x02,	 0x1A41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xF8,	 0x1D40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x1D41,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xE8,	 0x2040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x2041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x48,	 0x2340,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x2341,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - SMPS_ULS_VSET 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x78,	 0x1468,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x1469,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xF8,	 0x2068,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x2069,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - Groot_MISC 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0xEA,	 0x094C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x094D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x20,	 0x094E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x03,	 0x094F,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x094B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x58,	 0x0950,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x0951,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x06,	 0x0956,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_1a_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x90,	 0x14A6,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x99,	 0x1596,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x93,	 0x1597,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x84,	 0x14A8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x32,	 0x14A9,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x3F,	 0x1575,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x13,	 0x1574,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1467,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x14E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1445,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x28,	 0x144B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x14DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0C,	 0x1651,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x1450,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x20,	 0x1462,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0A,	 0x1463,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x70,	 0x1464,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x14AC,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x09,	 0x1471,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x10,	 0x1561,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xAA,	 0x1563,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1566,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x70,	 0x1440,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x1441,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_2a_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x90,	 0x17A6,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x60,	 0x1762,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x99,	 0x1896,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x93,	 0x1897,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x84,	 0x17A8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x32,	 0x17A9,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x8A,	 0x1863,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1866,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x3F,	 0x1875,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x13,	 0x1874,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1767,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0B,	 0x1771,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x17E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x3F,	 0x1764,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x01,	 0x17DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_3a_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x90,	 0x1AA6,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x60,	 0x1A62,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x99,	 0x1B96,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x93,	 0x1B97,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x84,	 0x1AA8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x32,	 0x1AA9,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x8A,	 0x1B63,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1B66,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x3F,	 0x1B75,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x13,	 0x1B74,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1A67,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0B,	 0x1A71,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x1AE3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x3F,	 0x1A64,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_OCP 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x4C,	 0x1466,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x4C,	 0x1766,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x4C,	 0x1A66,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - HFS_S4a_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x1ED0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x97,	 0x1E4A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1ED0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x88,	 0x1E4B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xF8,	 0x1D51,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x18,	 0x1D52,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x10,	 0x1D53,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0E,	 0x1D5B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1ED0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xFE,	 0x1E70,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1ED0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x1E72,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1ED0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x1E73,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0E,	 0x1E7E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x80,	 0x1D59,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x02,	 0x1D5A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA7,	 0x1D5C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x1D5D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x30,	 0x1D5E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xC5,	 0x1D44,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x32,	 0x1D7E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x62,	 0x1D63,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x1E45,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x01,	 0x1D54,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1ED0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x65,	 0x1E5A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1ED0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x1E63,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1D45,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1D49,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x1D48,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x82,	 0x1D61,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x80,	 0x1D47,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x01,	 0x1DDA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - HFS_S6a_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x24D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x69,	 0x244A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x24D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x8C,	 0x244B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xEC,	 0x2351,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x15,	 0x2352,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x20,	 0x2353,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x24D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xFE,	 0x2470,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x24D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x2472,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x24D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x2473,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x42,	 0x247E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x80,	 0x2359,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x235A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA6,	 0x235C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x235D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA0,	 0x235E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xC6,	 0x2344,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x30,	 0x237E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x42,	 0x2363,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x2445,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x01,	 0x2354,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x24D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x97,	 0x245A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x24D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x2463,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x2345,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x2349,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x2348,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x82,	 0x2361,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - HFS_S5a_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x3B,	 0x214A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x86,	 0x214B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA0,	 0x2051,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x1E,	 0x2052,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x20,	 0x2053,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xFE,	 0x2170,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x2172,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x03,	 0x2173,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x02,	 0x217E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x80,	 0x2059,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x02,	 0x205A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0E,	 0x205B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA6,	 0x205C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x09,	 0x205D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x30,	 0x205E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xC6,	 0x2044,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x30,	 0x207E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x42,	 0x2063,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x2145,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x54,	 0x215A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x21D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x2163,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x2045,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x2048,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x82,	 0x2061,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - HFS_OCP 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xA5,	 0x1DD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x4C,	 0x1D66,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x20D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x6C,	 0x2066,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xA5,	 0x23D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x4C,	 0x2366,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - SCHG_Config 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0xB8,	 0x1363,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x19D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x81,	 0x1946,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x19D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x1950,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x19D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x1946,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xC3,	 0x1051,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x0C,	 0x107D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x10B0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x18,	 0x14C2,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x0B,	 0x1358,	 0xFF,	 PM_SBL_WRITE,	 1,	 1}, 	  
	 { 0,	 0xCA,	 0x1358,	 0xFF,	 PM_SBL_WRITE,	 2,	 2}, 	  
	 { 0,	 0xA5,	 0x16D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x15,	 0x16C8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x16D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xE7,	 0x1365,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - SFG_Config 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0xA5,	 0x40D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x40DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x41D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x41DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x42D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x03,	 0x42DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x43D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x03,	 0x43DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x44D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x44DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xA5,	 0x45D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x45DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x04,	 0x42A8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x04,	 0x42A9,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x89,	 0x4586,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x8C,	 0x4587,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x60,	 0x4052,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x4583,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x4311,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x4312,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x4313,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - PBS_CONFIG_PM8005 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0xC0,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x02,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 1,	     0x0080,	 0xFF,	 PM_SBL_PBS_RAM, 0,	 0}, 	  
	 { 4,	 0x00,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x08,	 0x7050,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x02,	 0x7051,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x0C,	 0x7054,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x02,	 0x7055,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0xA5,	 0x70D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x70DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x8F,	 0x7248,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x80,	 0x7246,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - WARM_RESET_DATASET_INIT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0xA5,	 0x09D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x0B,	 0x09DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x0945,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0xC0,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - PM8005_PON_INIT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0xA5,	 0x08D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0xE7,	 0x0884,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x80,	 0x2446,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x40,	 0x3C42,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - PM8005_PON_INITa 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0xA5,	 0x08D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0xE7,	 0x0884,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x80,	 0x2446,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x40,	 0x3C42,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - BUS 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0xA5,	 0x04D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x01,	 0x04DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - RESET_CONFIG_PM8005 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0x00,	 0x085B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300	 
	 { 4,	 0x00,	 0x0853,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300	 
	 { 4,	 0x0A,	 0x0852,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300	 
	 { 4,	 0x80,	 0x0853,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	 //300
	 //Mode - PM8005_INTERRUPT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0x80,	 0x0546,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - S4C_FTS_LUT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 5,	 0x0B,	 0x1E80,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x08,	 0x1E81,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x03,	 0x1E82,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x3F,	 0x1E83,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x02,	 0x1E87,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x03,	 0x1E88,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x3F,	 0x1E89,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x41,	 0x1E8C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x20,	 0x1E8D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x45,	 0x1E8E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x1E8F,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x1E90,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x1E91,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_1c_4c_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0B,	 0x14DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0B,	 0x1DDA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0B,	 0x1EDA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0B,	 0x15DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x90,	 0x14A6,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x99,	 0x1596,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x93,	 0x1597,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x99,	 0x1E96,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x93,	 0x1E97,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x40,	 0x1E51,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x1D71,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x09,	 0x1471,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x84,	 0x14A8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x32,	 0x14A9,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0C,	 0x1E62,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xDA,	 0x1563,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x05,	 0x1566,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x3F,	 0x1575,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x3F,	 0x1E75,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x13,	 0x1574,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x06,	 0x1467,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x06,	 0x1445,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x07,	 0x14AC,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x03,	 0x14AD,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x1454,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x1DE3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x14E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0A,	 0x1463,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x7A,	 0x1464,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x28,	 0x144B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_2c_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x18DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x17DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x90,	 0x17A6,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x99,	 0x1896,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x93,	 0x1897,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x84,	 0x17A8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x32,	 0x17A9,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x09,	 0x1771,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x14,	 0x1860,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xDA,	 0x1863,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x05,	 0x1866,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x3F,	 0x1875,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x13,	 0x1874,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x06,	 0x1745,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x07,	 0x17AC,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x02,	 0x17AD,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x17E3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0A,	 0x1763,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x7A,	 0x1764,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x28,	 0x174B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_3c_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x1AD8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x07,	 0x1ADA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x90,	 0x1AA6,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x99,	 0x1B96,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x93,	 0x1B97,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x84,	 0x1AA8,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0A,	 0x1B61,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x8A,	 0x1B63,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x3F,	 0x1B75,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0D,	 0x1A71,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1B66,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x13,	 0x1B74,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x32,	 0x1AA9,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x1AAC,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x01,	 0x1AAD,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x0A,	 0x1A63,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x1AE3,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x3A,	 0x1A64,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x05,	 0x1A67,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x28,	 0x1A4B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - PM8005_FTS_OCP 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x4C,	 0x1466,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x4C,	 0x1766,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x4C,	 0x1A66,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - FTS_PSEUDO_TRIM_PM8005 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 4,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0xCC,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x01,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 3,	 3}, 	  
	 { 5,	 0x04,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 4,	 4}, 	  
	 { 5,	 0x05,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 5,	 5}, 	  
	 { 5,	 0x06,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 6,	 6}, 	  
	 { 5,	 0x07,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 7,	 7}, 	  
	 { 5,	 0x03,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 8,	 8}, 	  
	 { 5,	 0x02,	 0x1562,	 0xFF,	 PM_SBL_WRITE,	 9,	 9}, 	  
	 { 5,	 0x00,	 0x14D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 10,	 10}, 	  
	 { 5,	 0x04,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 11,	 11}, 	  
	 { 5,	 0x05,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 12,	 12}, 	  
	 { 5,	 0x06,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 13,	 13}, 	  
	 { 5,	 0x07,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 14,	 14}, 	  
	 { 5,	 0x03,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 15,	 15}, 	  
	 { 5,	 0x02,	 0x1862,	 0xFF,	 PM_SBL_WRITE,	 16,	 16}, 	  
	 { 5,	 0x00,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 17,	 17}, 	  
	 { 5,	 0x04,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 18,	 18}, 	  
	 { 5,	 0x05,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 19,	 19}, 	  
	 { 5,	 0x06,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 20,	 20}, 	  
	 { 5,	 0x07,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 21,	 21}, 	  
	 { 5,	 0x03,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 22,	 22}, 	  
	 { 5,	 0x02,	 0x1B62,	 0xFF,	 PM_SBL_WRITE,	 23,	 23}, 	  
	 { 5,	 0x00,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x00,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - PM8005_ULS_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x70,	 0x1468,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1469,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xD0,	 0x1768,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1769,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x17D0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0xA5,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x30,	 0x1A68,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x04,	 0x1A69,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 5,	 0x00,	 0x1AD0,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - generateSblBranchAndVersion 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x08,	 0x0150,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x06,	 0x0151,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x08,	 0x0150,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 4,	 0x06,	 0x0151,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
 	 // This line of data is created by PSI Compiler per request from Embedded SW Driver.It is not part of original code.
 	 {0, 0x00, 0x0000, 0xFF, PM_SBL_OPERATION_INVALID, 0, 0} 
 };