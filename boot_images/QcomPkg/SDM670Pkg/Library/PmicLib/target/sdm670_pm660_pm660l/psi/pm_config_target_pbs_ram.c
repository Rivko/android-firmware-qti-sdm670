/*! \file  pm_config_target_pbs_ram.c
 *  
 *  \brief  File Contains the PMIC Set Mode Driver Implementation
 *  \details Set Mode Driver implementation is responsible for setting and getting 
 *  all mode settings such as Register values, memory values, etc.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    Date: 2018-07-05
 *    PMIC SBL Version: SDM670-SBL-20180705-b0x19_v0x28 - Approved
 *    PBS RAM Version: PM670_RAM_b0x09_v0x0E
 *    PBS RAM Version: PM670_RAM_b0x19_v0x14
 *    PBS RAM Version: PM670L_RAM_b0x09_v0x0C
 *    PBS RAM Version: PM670L_RAM_b0x19_v0x15
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2018 Qualcomm Technologies Inc., All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PmicLib/target/sdm670_pm660_pm660l/psi/pm_config_target_pbs_ram.c#13 $ 
$DateTime: 2018/08/09 01:23:03 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_target_information.h"
#include "pm_config_sbl.h"

/*========================== PBS RAM LUT =============================*/

// To handle multiple PBS RAM configuration for different rev of the same PMIC or for multiple PMICs,
// a double dimension array of PBS RAM data is used. The data field of the specific command (in pm_config_target_sbl_sequence.c)
// to program PBS RAM will hold the index to the PBS RAM that needs to be used. during programming.
// Example:
// 1.sid; 2.data; 3.base_address; 4.offset;  5.reg_operation; 6.rev_id_operation; 7.rev_id;
// {0, 0x00, 0x0000, 0x000, PM_SBL_PBS_RAM, EQUAL, REV_ID_2_0},  //data = 0:  Use the 1st set of PBS RAM data if PMIC Rev ID = REV_ID_2_0
// {0, 0x01, 0x0000, 0x000, PM_SBL_PBS_RAM, EQUAL, REV_ID_1_0},  //data = 1:  Use the 2nd set of PBS RAM data if PMIC Rev ID = REV_ID_1_0

pm_pbs_ram_data_type
pm_pbs_seq [ ][PBS_RAM_DATA_SIZE] =
{
   // "PM670_RAM_b0x09_v0x0E"
   {
      //data  offset  base_addr  sid
      { 0x52,	0x53,	0x42,	0x50},	// W#0 - 	0x800 RAM signature PBSR" Checksum"
      { 0x09,	0x0E,	0x01,	0x18},	// W#1 - 	0x804 Header offset, Header Version, PBS RAM Revision, PBS RAM Branch
      { 0x2C,	0x08,	0xFF,	0x83},	// W#2 - 	0x808 Start of trigger jump table:
      { 0x6C,	0x08,	0xFF,	0x83},	// W#3 - 	0x80C
      { 0x98,	0x08,	0xFF,	0x83},	// W#4 - 	0x810
      { 0x00,	0x09,	0xFF,	0x83},	// W#5 - 	0x814
      { 0xCC,	0x0F,	0xFF,	0x83},	// W#6 - 	0x818 Fixed Offset = RAM-Base-Addr + 0x18 + 0x00 => SLEEP-SET
      { 0xD4,	0x0F,	0xFF,	0x83},	// W#7 - 	0x81C Fixed Offset = RAM-Base-Addr + 0x18 + 0x04 => PON X Reasons
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#8 - 	0x820 Fixed Offset = RAM-Base-Addr + 0x18 + 0x08 => Empty (No RTRR)/Spare
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#9 - 	0x824 Fixed Offset = RAM-Base-Addr + 0x18 + 0x0C => Empty (No HLOS)/Spare
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#10 - 	0x828 Fixed Offset = RAM-Base-Addr + 0x18 + 0x10 => Empty (Spare registers)/LPG
      { 0xFF,	0x48,	0x71,	0x00},	// W#11 - 	WRITE 0xFF TO PBS_CLIENT0__TRIG_LATCHED_CLR; Clear all latched sequences in PBS PON Client 0
      { 0xFB,	0x08,	0x08,	0x40},	// W#12 - 	READ PON__FSM_STATUS TO RB; Store PON_FSM_STATUS in RB for Older OTP-Revs < v0x05
      { 0xFC,	0x08,	0x08,	0x40},	// W#13 - 	READ PON__FSM_STATUS TO RC; Store PON_FSM_STATUS in RC for Newer OTP-Revs >= v0x05
      { 0x04,	0xC0,	0xFF,	0x90},	// W#14 - 	SKIP 1 Lines if RB NOT EQUAL 0x04 (MASK=0xFF); DVDD_CONFIG = State 4
      { 0x14,	0x00,	0xFF,	0x83},	// W#15 - 	GOTO OTP_DVDD_CONFIG (OTP) & End there
      { 0x01,	0xC0,	0xFF,	0x91},	// W#16 - 	Skip if RB NOT EQUAL 0x01 (PON = State 1)
      { 0x00,	0x57,	0x08,	0x00},	// W#17 - 	PMICPR-475: Disable PMIC_WD only at the beginning of PON or WR
      { 0x14,	0x00,	0xFF,	0x83},	// W#18 - 	GOTO OTP_DVDD_CONFIG (OTP) & End there
      { 0x06,	0xC0,	0xFF,	0x90},	// W#19 - 	Skip if RB NOT EQUAL 0x06 (FAULT = State 6)
      { 0x20,	0x09,	0xFF,	0x83},	// W#20 - 	GOTO CLIENT0_FAULT_SEQUENCE & End there
      { 0x02,	0xC0,	0xFF,	0x90},	// W#21 - 	Skip if RB NOT EQUAL 0x02 (POFF = State 2)
      { 0x88,	0x09,	0xFF,	0x83},	// W#22 - 	GOTO CLIENT0_POFF_SEQUENCE & End there
      { 0x07,	0xC0,	0xFF,	0x90},	// W#23 - 	Skip if RB NOT EQUAL 0x07 (WARM RESET = State 7)
      { 0xE4,	0x0A,	0xFF,	0x83},	// W#24 - 	GOTO CLIENT0_WARM_RESET_SEQUENCE & End there
      { 0x00,	0x91,	0x08,	0x00},	// W#25 - 	Undefined State - NACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#26 - 	End Sequence
      { 0xF3,	0x0A,	0x72,	0x40},	// W#27 - 	Read Client 1 trigger latch status and store to Buffer R3
      { 0x02,	0x30,	0x02,	0x91},	// W#28 - 	Skip if not Bit 1 = NC (WL uses it for Type-C UFP crude sensing WA)
      { 0x02,	0x48,	0x72,	0x00},	// W#29 - 	Clear Latched Trigger - Bit 1
      { 0x70,	0x0C,	0xFF,	0x83},	// W#30 - 	CL1_TRIG1: CLIENT1_TRIG1_SCHG_WA
      { 0x04,	0x30,	0x04,	0x91},	// W#31 - 	Skip if not Bit 2 = EOSR_TIMER ; Client1_SCHG_EOSR
      { 0x04,	0x48,	0x72,	0x00},	// W#32 - 	Clear Latched Trigger - Bit 2
      { 0x38,	0x0C,	0xFF,	0x83},	// W#33 - 	CL1_TRIG2: GOTO_CLIENT1_SCHG_EOSR & End there
      { 0x80,	0x30,	0x80,	0x91},	// W#34 - 	Skip if not Bit 7 = SW Trigger (Sleep/Wake)
      { 0x80,	0x48,	0x72,	0x00},	// W#35 - 	Clear Latched Trigger - Bit 7
      { 0x74,	0x0C,	0xFF,	0x83},	// W#36 - 	CL1_TRIG7: CLIENT1_SYSTEM_SLEEP_WAKE_SEQUENCE
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#37 - 	
      { 0xFC,	0x54,	0x09,	0x40},	// W#38 - 	PBSREG.RC will store MISC_SPARE_5 (used in CL2_TRIG0 & CL2_TRIG7)
      { 0xFB,	0x69,	0x41,	0x40},	// W#39 - 	PBSREG.RB will store the latest value of FG_BATT_INFO.ESR_PULL_DOWN_CFG (used in CL2_TRIG0 & CL2_TRIG7)
      { 0x03,	0x16,	0x73,	0x00},	// W#40 - 	Disable PBS_Client2 All (SEQ_ENDED) Interrupts - won't fire for any of CLIENT2 Sequences, except when re-enabled
      { 0xF4,	0x0A,	0x73,	0x40},	// W#41 - 	1BYTE READ PBS_CLIENT2__TRIG_LATCHED TO R4; Read Client 2 trigger latch status and store to Buffer R4
      { 0x01,	0x40,	0x01,	0x91},	// W#42 - 	Skip if not Bit 0 = DTEST MUX / EOSR_TIMER
      { 0x01,	0x48,	0x73,	0x00},	// W#43 - 	1BYTE WRITE 0x01 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 0
      { 0x70,	0x0D,	0xFF,	0x83},	// W#44 - 	CL2_TRIG0: CLIENT2_EOSR_TIMER_EXPIRED_SEQUENCE
      { 0x02,	0x40,	0x02,	0x91},	// W#45 - 	Skip if not Bit 1 = EUD(SCHG)
      { 0x02,	0x48,	0x73,	0x00},	// W#46 - 	1BYTE WRITE 0x02 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 1
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#47 - 	CL2_TRIG1:EMPTY => Replace with GOTO once defined.
      { 0x04,	0x40,	0x04,	0x91},	// W#48 - 	Skip if not Bit 2 = GPIO09 (WCSS_VCTRL)
      { 0x04,	0x48,	0x73,	0x00},	// W#49 - 	1BYTE WRITE 0x04 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 2
      { 0xF4,	0x0D,	0xFF,	0x83},	// W#50 - 	CL2_TRIG2: CLIENT2_WCSS_VCTRL_SEQUENCE
      { 0x08,	0x40,	0x08,	0x91},	// W#51 - 	Skip if not Bit 3 = GPIO05 (WLAN_RF_VCTRL)
      { 0x08,	0x48,	0x73,	0x00},	// W#52 - 	1BYTE WRITE 0x08 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 3
      { 0x38,	0x0E,	0xFF,	0x83},	// W#53 - 	CL2_TRIG3: CLIENT2_WLAN_RF_VCTRL_SEQUENCE
      { 0x20,	0x40,	0x20,	0x91},	// W#54 - 	Skip if not Bit 5 - Used as Internal Re-Trigger from CLIENT3_SW_TRIGGER
      { 0x20,	0x48,	0x73,	0x00},	// W#55 - 	
      { 0x1C,	0x0F,	0xFF,	0x83},	// W#56 - 	Reschedule using CLIENT2_COMMON_END_RESCHED_EVAL_SEQUENCE
      { 0x40,	0x40,	0x40,	0x91},	// W#57 - 	Skip if not Bit 6 = BUA
      { 0x40,	0x48,	0x73,	0x00},	// W#58 - 	
      { 0x5C,	0x0E,	0xFF,	0x83},	// W#59 - 	CL2_TRIG6: CLIENT2_BUA_SEQUENCE
      { 0x80,	0x40,	0x80,	0x91},	// W#60 - 	Skip if not Bit 7 = SW Trigger
      { 0x80,	0x48,	0x73,	0x00},	// W#61 - 	
      { 0x80,	0x0E,	0xFF,	0x83},	// W#62 - 	CL2_TRIG7: CLIENT2_SW_TRIGGER
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#63 - 	
      { 0xF5,	0x0A,	0x74,	0x40},	// W#64 - 	Read Client 3 trigger latch status and store to Buffer R5
      { 0x04,	0x50,	0x04,	0x91},	// W#65 - 	Skip if NOT Bit 2 = Charger Test MUX
      { 0x04,	0x48,	0x74,	0x00},	// W#66 - 	Clear Latched Trigger - Bit 2
      { 0x3C,	0x0F,	0xFF,	0x83},	// W#67 - 	CL3_TRIG2: Client3_SCHG_PD_WA1_Crude_Sensor
      { 0x80,	0x50,	0x80,	0x91},	// W#68 - 	Skip if NOT Bit 7 = SW Trigger
      { 0x80,	0x48,	0x74,	0x00},	// W#69 - 	1BYTE WRITE 0x80 TO PBS_CLIENT3__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 7
      { 0x94,	0x0F,	0xFF,	0x83},	// W#70 - 	CL3_TRIG7: CLIENT3_SW_TRIGGER
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#71 - 	
      { 0x05,	0xDA,	0x2C,	0x10},	// W#72 - 	PMICPR-827: PM670 (Groot) REF_BYP stays on after fault POFF (due to fix in PMICPR-801)
      { 0x2E,	0x44,	0x60,	0x00},	// W#73 - 	PMICPR-942: CalRC offset compensation (+55ppm offset for RTC when PMIC is OFF)
      { 0xC8,	0x0B,	0xFF,	0x9B},	// W#74 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0xF4,	0x80,	0x08,	0x00},	// W#75 - 	When configuring the KPDPWR_N Stage 1 reset timer, corresponding PON trigger must be disabled during the re-configuration;PMICPR-356
      { 0x0B,	0x00,	0xFF,	0xC1},	// W#76 - 	Wait 300us to REGISTER get updated.
      { 0x42,	0x90,	0x16,	0x00},	// W#77 - 	1BYTE WRITE Force STAT low ([7:6]=b'01) so that SMB goes to suspend
      { 0x02,	0x66,	0x13,	0x00},	// W#78 - 	1BYTE WRITE Revert USB5/1 setting to 500mA
      { 0x38,	0x63,	0x13,	0x00},	// W#79 - 	0x1363 [7] WIPWR_RST_EUD_CFG = 0
      { 0x00,	0x53,	0x10,	0x00},	// W#80 - 	1BYTE WRITE 0x00 TO SCHG_CHGR__CFG; Disable VDISCHG_EN_CFG to reduce off current.
      { 0x01,	0x45,	0x16,	0x00},	// W#81 - 	Enable Charger Shutdown CR- 0000171925
      { 0x24,	0x68,	0x13,	0x00},	// W#82 - 	Charger WA-31. Revert to UFP mode (same as FTRIM setting) to avoid unexpected PON
      { 0x04,	0xDF,	0x14,	0x00},	// W#83 - 	CR176140: Set chicken bit high to avoid lockup at following PON
      { 0x88,	0x48,	0x14,	0x01},	// W#84 - 	1BYTE WRITE 0x88 TO S1_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x17,	0x01},	// W#85 - 	1BYTE WRITE 0x88 TO S2_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x1A,	0x01},	// W#86 - 	1BYTE WRITE 0x88 TO S3_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x1D,	0x01},	// W#87 - 	1BYTE WRITE 0x88 TO S4_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x20,	0x01},	// W#88 - 	1BYTE WRITE 0x88 TO S5_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x23,	0x01},	// W#89 - 	1BYTE WRITE 0x88 TO S6_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x80,	0x48,	0x52,	0x01},	// W#90 - 	PMICPR-711: Re-enable L19A PD in FAULT (since it was disabled in SBL)
      { 0xEA,	0x01,	0xFF,	0xC1},	// W#91 - 	WAIT 15016 us; Wait 15ms for VREGs to discharge completely before allowing the next PON trigger
      { 0xF6,	0xC8,	0x08,	0x40},	// W#92 - 	1BYTE READ PON__FAULT_REASON1 TO R6; Read FAULT_REASON1 and store to Buffer R6
      { 0x40,	0x61,	0x40,	0x91},	// W#93 - 	SKIP 2 Lines if R6 EQUAL 0x40 (MASK=0x40); Skip next 2 lines if Bit-6 (UVLO) = 1
      { 0x00,	0x7F,	0x08,	0x00},	// W#94 - 	1BYTE WRITE 0x00 TO PON__SMPL_CTL; Disable SMPL
      { 0x00,	0x46,	0x40,	0x00},	// W#95 - 	PMICPR-407; This is to fix the issue of REF_BYP stays ON during a fault due to a FG write which ignores all resets.
      { 0x40,	0x91,	0x08,	0x00},	// W#96 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#97 - 	End of Sequence
      { 0x00,	0x7F,	0x08,	0x00},	// W#98 - 	1BYTE WRITE 0x00 TO PON__SMPL_CTL; Disable SMPL
      { 0xF4,	0x80,	0x08,	0x00},	// W#99 - 	PMICPR-1039: Groot: Re-enable KPDPWR_N PON_TRIGGER in POFF-Sequence
      { 0x2E,	0x44,	0x60,	0x00},	// W#100 - 	PMICPR-942: CalRC offset compensation (+55ppm offset for RTC when PMIC is OFF)
      { 0xB4,	0x0B,	0xFF,	0x9B},	// W#101 - 	GOTO_SUB_NOID ASSERT_EXT_RESET; Set PON_RESET_N and PON_OUT low
      { 0xC8,	0x0B,	0xFF,	0x9B},	// W#102 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x04,	0x98,	0x08,	0x10},	// W#103 - 	1BYTE WRITE 0x04 TO PON__RESET_CTL; Assert shutdown1_rb
      { 0x03,	0x00,	0xFF,	0xC1},	// W#104 - 	WAIT 91 us; Wait 3 SLEEP_CLK cycles for reset changes to propagate through clock domains
      { 0x06,	0x98,	0x08,	0x10},	// W#105 - 	1BYTE WRITE 0x06 TO PON__RESET_CTL; De-assert shutdown1_rb
      { 0x00,	0x40,	0xC7,	0x00},	// W#106 - 	Configure GPIO08 to Dig INPUT
      { 0x05,	0x42,	0xC7,	0x00},	// W#107 - 	Set SLB to No pull
      { 0x38,	0x63,	0x13,	0x00},	// W#108 - 	0x1363 [7] WIPWR_RST_EUD_CFG = 0
      { 0x7C,	0x0A,	0x14,	0x89},	// W#109 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S1_CTRL; APC0 (Kryo Silver)
      { 0x7C,	0x0A,	0x20,	0x89},	// W#110 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S5_CTRL; Modem SS Q6
      { 0x34,	0x03,	0x76,	0x29},	// W#111 - 	WAIT 25024 us; Wait 25ms.  Store to Buffer R6.  0x334
      { 0x88,	0x0A,	0xFF,	0x9B},	// W#112 - 	
      { 0x7C,	0x0A,	0x51,	0x88},	// W#113 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LN_BB_CLK1; LNBBCLK1
      { 0x7C,	0x0A,	0x49,	0x89},	// W#114 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO10; PHY-PLL-BB_CLK-USB
      { 0xF4,	0x01,	0x76,	0x29},	// W#115 - 	WAIT 15250 us; Wait 15.25ms.  Store to Buffer R6. 146
      { 0xB4,	0x0A,	0xFF,	0x9B},	// W#116 - 	
      { 0xA3,	0x00,	0x76,	0x29},	// W#117 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6.  0x146
      { 0x88,	0x0A,	0xFF,	0x9B},	// W#118 - 	
      { 0x7C,	0x0A,	0x40,	0x89},	// W#119 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO1; USB (1.2V)
      { 0x7C,	0x0A,	0x4A,	0x89},	// W#120 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO11; USB-Retimer
      { 0x7C,	0x0A,	0x47,	0x89},	// W#121 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO08; EMMC/UFS 1.8V
      { 0xF2,	0x5C,	0x09,	0x40},	// W#122 - 	
      { 0x03,	0x20,	0x03,	0x91},	// W#123 - 	skip disabling S3A if not single phase configuration
      { 0x7C,	0x0A,	0x1A,	0x89},	// W#124 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S3_CTRL; APC1 (Kryo Silver) 0.87V SHW
      { 0xF8,	0x09,	0xFF,	0x83},	// W#125 - 	
      { 0x00,	0x44,	0xC0,	0x00},	// W#126 - 	GPIO_01A: Configure to Drive-Low (strong-PD), when in DIG_OUT Mode
      { 0x01,	0x40,	0xC0,	0x00},	// W#127 - 	GPIO_01A: Configure to DIG_OUT Mode
      { 0x21,	0x00,	0xFF,	0xC1},	// W#128 - 	WAIT 1068 us; Wait 33+2 32kHz clock cycles - for DBU2 to discharge
      { 0x7C,	0x0A,	0xC1,	0x88},	// W#129 - 	o SF1 (ALT_SLEEP_CLK)
      { 0x7C,	0x0A,	0x54,	0x88},	// W#130 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL  WLAN 38.4MHz
      { 0x7C,	0x0A,	0x5A,	0x88},	// W#131 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL  SLEEP_CLK1
      { 0x80,	0x48,	0x52,	0x01},	// W#132 - 	PMICPR-711: Re-enable L19A PD in POFF (since it was disabled in SBL)
      { 0x7C,	0x0A,	0x52,	0x89},	// W#133 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO19; WCN3990 CHAIN0 (Wipwr)
      { 0x7C,	0x0A,	0x45,	0x89},	// W#134 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO06; WCN RF,GPS, Metis (Wipwr)
      { 0x7C,	0x0A,	0x48,	0x89},	// W#135 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO09; WCN_XO (Wipwr)
      { 0x80,	0x48,	0x23,	0x01},	// W#136 - 	PMICPR-1947: Re-enable S6A Strong-PD in POFF (since it was disabled in SBL)
      { 0x7C,	0x0A,	0x23,	0x89},	// W#137 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S6_CTRL; MV_LDO SMPS
      { 0x7C,	0x0A,	0x0B,	0x88},	// W#138 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL VREF_MSM; 1.25V reference for UIM and SD
      { 0x7C,	0x0A,	0x4C,	0x89},	// W#139 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO13; PX3 1.8v
      { 0xF4,	0x01,	0x76,	0x29},	// W#140 - 	WAIT 15250 us; Wait 15.25ms.  Store to Buffer R6. 146
      { 0xB4,	0x0A,	0xFF,	0x9B},	// W#141 - 	
      { 0x7C,	0x0A,	0x42,	0x89},	// W#142 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO03A, WTR 1V
      { 0x80,	0x48,	0x1D,	0x01},	// W#143 - 	PMICPR-1947: Re-enable S4A Strong-PD in POFF (since it was disabled in SBL)
      { 0x7C,	0x0A,	0x1D,	0x89},	// W#144 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S4_CTRL; HV sub
      { 0x7C,	0x0A,	0x50,	0x88},	// W#145 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL XO; OK to Skip LDO_XO?
      { 0xA3,	0x00,	0xFF,	0xC1},	// W#146 - 	WAIT 4972 us; Wait 5ms
      { 0x42,	0x90,	0x16,	0x00},	// W#147 - 	1BYTE WRITE Force STAT low ([7:6]=b'01) so that SMB goes to suspend
      { 0x01,	0x45,	0x16,	0x00},	// W#148 - 	eNABLE cHARGER Shutdown CR- 0000171925
      { 0x00,	0x46,	0x40,	0x00},	// W#149 - 	1BYTE WRITE 0x00 TO FG_BATT_SOC__EN_CTL; disable FG algorithm (CR-0000171925).  This bit emulatoes what the SHDN_N HW signal did on earlier PMIs.
      { 0x05,	0x52,	0x45,	0x00},	// W#150 - 	Put RRADC in Shutdown (not shutdown , just pause RRADC)
      { 0x02,	0x66,	0x13,	0x00},	// W#151 - 	1BYTE WRITE Revert USB5/1 setting to 500mA
      { 0x24,	0x68,	0x13,	0x00},	// W#152 - 	Charger WA-31. Revert to UFP mode (same as FTRIM setting) to avoid unexpected PON
      { 0x04,	0xDF,	0x14,	0x00},	// W#153 - 	CR176140: Set chicken bit high to avoid lockup at following PON
      { 0x7C,	0x0A,	0x24,	0x88},	// W#154 - 	
      { 0x7C,	0x0A,	0x2C,	0x88},	// W#155 - 	
      { 0x46,	0x01,	0xFF,	0xC1},	// W#156 - 	WAIT 10010 us; Wait 10ms for VREGs to discharge completely before allowing the next PON trigger
      { 0x40,	0x91,	0x08,	0x00},	// W#157 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#158 - 	End of Sequence
      { 0x00,	0x46,	0xFF,	0x04},	// W#159 - 	1BYTE WRITE 0x00 TO 0x46; Write 0x00 to 0x46 (enable register in the peripheral)
      { 0x21,	0x00,	0xFF,	0xC1},	// W#160 - 	WAIT 1068 us; Wait 33+2 32kHz clock cycles.  Increased for PM8019 2.0.
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#161 - 	End of subroutine. PC = Stored PC.; Go back to next line of the calling function
      { 0x01,	0x00,	0xFF,	0xC1},	// W#162 - 	Wait 1 CLK Cycle before checking
      { 0xF1,	0x08,	0xC7,	0x40},	// W#163 - 	Read GPIO08 Status (GPIO_VAL) and store to Buffer R1
      { 0x01,	0x10,	0x01,	0x92},	// W#164 - 	We will exit  = '1'
      { 0x01,	0x40,	0xC7,	0x00},	// W#165 - 	Configure GPIO08 to Dig Output
      { 0x00,	0x44,	0xC7,	0x00},	// W#166 - 	Set SLB LOW and continue
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#167 - 	
      { 0x01,	0x00,	0x76,	0x25},	// W#168 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#169 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0x88,	0x0A,	0xFF,	0x83},	// W#170 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#171 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#172 - 	End of Sequence
      { 0x00,	0x40,	0xC7,	0x00},	// W#173 - 	Configure GPIO08 to Dig INPUT
      { 0x00,	0x42,	0xC7,	0x00},	// W#174 - 	Set SLB to PU
      { 0x01,	0x00,	0xFF,	0xC1},	// W#175 - 	Wait 30us before trying again
      { 0xF1,	0x08,	0xC7,	0x40},	// W#176 - 	Read GPIO08 Status (GPIO_VAL) and store to Buffer R1
      { 0x00,	0x10,	0x01,	0x91},	// W#177 - 	Skip the next line if GPIO08_STATUS1 (bit 0) is high
      { 0x05,	0x42,	0xC7,	0x00},	// W#178 - 	Set SLB to No pull
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#179 - 	return to the next linE of the calling routing
      { 0x01,	0x00,	0x76,	0x25},	// W#180 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#181 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0xBC,	0x0A,	0xFF,	0x83},	// W#182 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#183 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#184 - 	End of Sequence; Return
      { 0x00,	0x57,	0x08,	0x00},	// W#185 - 	PMICPR-475: Disable PMIC_WD only at the beginning of WR (here) & PON
      { 0xB4,	0x0B,	0xFF,	0x9B},	// W#186 - 	GOTO_SUB_NOID ASSERT_EXT_RESET; Set PON_RESET_N and PON_OUT low
      { 0x8E,	0x01,	0xFF,	0xC1},	// W#187 - 	WAIT 12.2 ms = 400 SLEEP_CLK cycles before setting PON_OUT high again (to signal Warm Reset to slave PMICs)
      { 0x40,	0x49,	0x09,	0x10},	// W#188 - 	Send PON_OUT high to all PMICs.
      { 0xC8,	0x0B,	0xFF,	0x9B},	// W#189 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0xC6,	0x44,	0x1D,	0x01},	// W#190 - 	Restore this setting, in case warm reset occurred in sleep
      { 0x02,	0x5A,	0x1D,	0x11},	// W#191 - 	Restore this setting, in case warm reset occurred in sleep
      { 0x06,	0x45,	0x1D,	0x01},	// W#192 - 	Change to Auto mode, in case it were in Retention (after Sleep)
      { 0x06,	0x49,	0x1D,	0x01},	// W#193 - 	Change to Auto mode, in case it were FORCE_PFM (after Sleep)
      { 0xF8,	0x40,	0x1D,	0x01},	// W#194 - 	S4A: LB for WL_PON-VSET = 0x07_F8 => 2040 mV
      { 0x07,	0x41,	0x1D,	0x01},	// W#195 - 	S4A: UB for WL_PON-VSET = 0x07_F8 => 2040 mV
      { 0x02,	0x98,	0x08,	0x10},	// W#196 - 	1BYTE WRITE 0x02 TO PON__RESET_CTL; Assert warm_rb
      { 0x03,	0x00,	0xFF,	0xC1},	// W#197 - 	WAIT 91.5 us; Wait 3 SLEEP_CLK cycles for reset changes to propagate through clock domains
      { 0x06,	0x98,	0x08,	0x10},	// W#198 - 	1BYTE WRITE 0x06 TO PON__RESET_CTL; De-assert warm_rb
      { 0x07,	0x45,	0x40,	0x01},	// W#199 - 	: 21
      { 0x07,	0x45,	0x42,	0x01},	// W#200 - 	: 04
      { 0x07,	0x45,	0x47,	0x01},	// W#201 - 	: 18
      { 0x07,	0x45,	0x4A,	0x01},	// W#202 - 	: 19
      { 0x07,	0x45,	0x4C,	0x01},	// W#203 - 	: 10
      { 0x07,	0x45,	0x52,	0x01},	// W#204 - 	: 16
      { 0xF0,	0x5C,	0x09,	0x40},	// W#205 - 	
      { 0x03,	0x00,	0x03,	0x94},	// W#206 - 	
      { 0xF0,	0x40,	0x17,	0x01},	// W#207 - 	S2A: LB for WL_SBL-VSET = 0x02_F0 => 0752 mV
      { 0x02,	0x41,	0x17,	0x01},	// W#208 - 	S2A: UB for WL_SBL-VSET = 0x02_F0 => 0752 mV
      { 0x68,	0x40,	0x1A,	0x01},	// W#209 - 	S3A: LB for WL_PON-VSET = 0x04_68 => 1128 mV
      { 0x04,	0x41,	0x1A,	0x01},	// W#210 - 	S3A: UB for WL_PON-VSET = 0x04_68 => 1128 mV
      { 0x58,	0x0B,	0xFF,	0x83},	// W#211 - 	
      { 0xF0,	0x40,	0x1A,	0x01},	// W#212 - 	S3A_S2A: LB for WL_SBL-VSET = 0x02_F0 => 0752 mV
      { 0x02,	0x41,	0x1A,	0x01},	// W#213 - 	S3A_S2A: UB for WL_SBL-VSET = 0x02_F0 => 0752 mV
      { 0x40,	0x40,	0x14,	0x01},	// W#214 - 	S1A: LB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x03,	0x41,	0x14,	0x01},	// W#215 - 	S1A: UB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x40,	0x40,	0x20,	0x01},	// W#216 - 	S5A: LB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x03,	0x41,	0x20,	0x01},	// W#217 - 	S5A: UB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x48,	0x40,	0x23,	0x01},	// W#218 - 	S6A: LB for WL_PON-VSET = 0x05_48 => 1352 mV
      { 0x05,	0x41,	0x23,	0x01},	// W#219 - 	S6A: UB for WL_PON-VSET = 0x05_48 => 1352 mV
      { 0xB0,	0x40,	0x40,	0x01},	// W#220 - 	LDO01A: LB for WL_PON-VSET = 0x04_B0 => 1200 mV
      { 0x04,	0x41,	0x40,	0x01},	// W#221 - 	LDO01A: UB for WL_PON-VSET = 0x04_B0 => 1200 mV
      { 0x20,	0x40,	0x44,	0x01},	// W#222 - 	LDO05A: LB for WL_PON-VSET = 0x03_20 => 0800 mV
      { 0x03,	0x41,	0x44,	0x01},	// W#223 - 	LDO05A: UB for WL_PON-VSET = 0x03_20 => 0800 mV
      { 0x20,	0x40,	0x52,	0x01},	// W#224 - 	LDO19A: LB for WL_PON-VSET = 0x0C_20 => 3104 mV
      { 0x0C,	0x41,	0x52,	0x01},	// W#225 - 	LDO19A: UB for WL_PON-VSET = 0x0C_20 => 3104 mV
      { 0x42,	0x90,	0x16,	0x00},	// W#226 - 	1BYTE WRITE Force STAT low ([7:6]=b'01) so that SMB goes to suspend
      { 0xC4,	0x80,	0x13,	0x00},	// W#227 - 	REVERT REGISTER TO ftrim SETTINGS upon WARM RESET
      { 0x00,	0x52,	0x11,	0x00},	// W#228 - 	REVERT REGISTER TO ftrim SETTINGS upon WARM RESET
      { 0x00,	0xC2,	0x14,	0x10},	// W#229 - 	REVERT REGISTER TO ftrim SETTINGS upon WARM RESET
      { 0x04,	0xDF,	0x14,	0x00},	// W#230 - 	CR176140: Set chicken bit high to avoid lockup at following PON
      { 0xAA,	0x04,	0xFF,	0xC1},	// W#231 - 	WAIT 36417 us; Wait 1194 cycles of SLEEP_CLK (36.4 ms) for MSM prep time during warm reset
      { 0x9B,	0x19,	0x76,	0x29},	// W#232 - 	2BYTE R6 = 0x199B; (2+0x199b) X 30.5 uS = 200 mS.  Pass this into enable wait pon as a parameter
      { 0x99,	0x00,	0x7D,	0x28},	// W#233 - 	1BYTE OFFSET = 0x99; Set offset to PON_RESET_N_CTL register (0x99)
      { 0x18,	0x10,	0x08,	0x88},	// W#234 - 	GOTO ROM_ENABLE_WAIT_RELATIVE (ROM) PON; enable wait PON subroutine will deassert PON_RESET_N and wait for PS_HOLD to go high
      { 0x40,	0x91,	0x08,	0x00},	// W#235 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#236 - 	End of Warm-Reset Sequence
      { 0x00,	0x43,	0x08,	0x02},	// W#237 - 	1BYTE WRITE 0x00 TO SID:2 0x0800__0x43; Set SPMI_SLAVE_RB to 0 to tolerate glitches during power down
      { 0x00,	0x99,	0x08,	0x00},	// W#238 - 	1BYTE WRITE 0x00 TO PON__PON_RESET_N_CTL; Assert PON_RESET_N low
      { 0x00,	0x49,	0x09,	0x10},	// W#239 - 	1BYTE WRITE 0x00 TO MISC__PON_OUT_CTL; Send PON_OUT low to all PMIC's.  Changing to SEC ACCESS (to updated in next .csv).
      { 0x01,	0x43,	0x08,	0x02},	// W#240 - 	1BYTE WRITE 0x01 TO SID:2 0x0800__0x43; Set SPMI_SLAVE_RB back to 1
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#241 - 	
      { 0x1C,	0x08,	0x70,	0xA1},	// W#242 - 	
      { 0x1C,	0x00,	0x70,	0x21},	// W#243 - 	2BYTE R0 = R0 + 0x1C ([5_Datasets x 2_Rows/Dataset - 2_SRC - 1] = 7 Rows x 0x04 Addr-Offset per Row)
      { 0x1C,	0x08,	0x72,	0xA1},	// W#244 - 	
      { 0x24,	0x00,	0x72,	0x21},	// W#245 - 	2BYTE R2 = R2 + 0x24 ([5_Datasets x 2_Rows/Dataset - 0_DST - 1] = 9 Rows x 0x04 Addr-Offset per Row)
      { 0x08,	0x00,	0x78,	0x28},	// W#246 - 	1BYTE R8 = 0x08 (Loop 8 times = [5_Datasets x 2_Rows/Dataset - 2_Rows_for_CURR])
      { 0xF0,	0xFF,	0xF4,	0xA3},	// W#247 - 	4BYTE R4 = MEMADDR(R0); copy the last reason registers into a buffer
      { 0xF2,	0xFF,	0xF4,	0xA7},	// W#248 - 	4BYTE MEMADDR(R2) = R4; write the reasons to the location pointed to by the paste pointer
      { 0x01,	0x00,	0x78,	0x24},	// W#249 - 	
      { 0x00,	0x81,	0xFF,	0x92},	// W#250 - 	SKIP 3 Lines if R8 EQUAL 0x00 (MASK=0xFF); Loop Until loop count (R8) = 4
      { 0x04,	0x00,	0x70,	0x25},	// W#251 - 	2BYTE R0 = R0 - 0x04; Move read pointer to next (upper) DWORD
      { 0x04,	0x00,	0x72,	0x25},	// W#252 - 	2BYTE R2 = R2 - 0x04; Move write pointer to next (upper) DWORD
      { 0xDC,	0x0B,	0xFF,	0x83},	// W#253 - 	
      { 0xF4,	0xC0,	0x08,	0x40},	// W#254 - 	
      { 0xF5,	0xC2,	0x08,	0x40},	// W#255 - 	
      { 0xF6,	0xC4,	0x08,	0x40},	// W#256 - 	
      { 0xF7,	0xC5,	0x08,	0x40},	// W#257 - 	
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#258 - 	4BYTE MEMADDR(R0) = R4
      { 0x04,	0x00,	0x70,	0x21},	// W#259 - 	2BYTE R0 = R0 + 0x04
      { 0xF4,	0xC7,	0x08,	0x40},	// W#260 - 	
      { 0xF5,	0xC8,	0x08,	0x40},	// W#261 - 	
      { 0xF6,	0xC9,	0x08,	0x40},	// W#262 - 	
      { 0xF7,	0x48,	0x60,	0x40},	// W#263 - 	1BYTE READ RTC_RW__RDATA0 TO R7; Fill the last byte with the RTC value.
      { 0xF8,	0x48,	0x60,	0x40},	// W#264 - 	1BYTE READ RTC_RW__RDATA0 TO R8; Fill the last byte with the RTC value.
      { 0xF8,	0xF7,	0x79,	0x26},	// W#265 - 	1BYTE R9 = R8 - R7
      { 0x00,	0x91,	0xFF,	0x90},	// W#266 - 	SKIP 1 Lines if R9 EQUAL 0x00 (MASK=0xFF); If data didn't match we retry.
      { 0x1C,	0x0C,	0xFF,	0x83},	// W#267 - 	
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#268 - 	4BYTE MEMADDR(R0) = R4; Write data to address stored in R0
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#269 - 	End of subroutine. PC = Stored PC
      { 0x70,	0x0F,	0xFF,	0x9B},	// W#270 - 	Assert CHG Clock Req
      { 0xF1,	0x68,	0x13,	0x40},	// W#271 - 	R1 = 0x1368
      { 0x04,	0x11,	0x04,	0x96},	// W#272 - 	Skip next 5 lines if 0x1368[2]=1
      { 0x04,	0x00,	0xF1,	0x30},	// W#273 - 	Set R1[2]=1 --> SNK_EN_CMD=1
      { 0xF1,	0x68,	0x13,	0x08},	// W#274 - 	0x1368=R1 --> SNK_EN_CMD=1
      { 0xF7,	0x59,	0x13,	0x40},	// W#275 - 	R7=0x1358
      { 0xFD,	0x00,	0xF7,	0x2C},	// W#276 - 	Set R7 bit 1=0
      { 0xF7,	0x59,	0x13,	0x08},	// W#277 - 	Set 0x1359[1]=0 --> Enable Crude Sensor
      { 0x80,	0x0F,	0xFF,	0x9B},	// W#278 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0x42,	0x00,	0xFF,	0xC5},	// W#279 - 	Start 2ms timer
      { 0xFB,	0x00,	0xF1,	0x2C},	// W#280 - 	1BYTE R1 = R1 AND 0xFB ; Set R1[2]=0 --> SNK_EN_CMD=0
      { 0xF1,	0x68,	0x13,	0x08},	// W#281 - 	0x1368=R1 --> SNK_EN_CMD=0
      { 0x80,	0x0F,	0xFF,	0x9B},	// W#282 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#283 - 	
      { 0x42,	0x00,	0xFF,	0xC5},	// W#284 - 	Start 2ms timer
      { 0xF7,	0x50,	0x72,	0x40},	// W#285 - 	1BYTE READ PBS_CLIENT1__SCRATCH1 & store to Buffer R7
      { 0x05,	0x70,	0xFF,	0x91},	// W#286 - 	SKIP 1 Lines if R7 NOT EQUAL 0x04 (MASK=0xFF)
      { 0x10,	0x51,	0x72,	0x00},	// W#287 - 	Using 0x10 = d'16 x 30.5us = 488uSec as Safe-Time before BU
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#288 - 	
      { 0x04,	0x70,	0xFF,	0x90},	// W#289 - 	SKIP 1 Lines if R7 NOT EQUAL 0x04 (MASK=0xFF)
      { 0xA8,	0x0C,	0xFF,	0x83},	// W#290 - 	
      { 0x03,	0x70,	0xFF,	0x90},	// W#291 - 	SKIP 1 Lines if R7 NOT EQUAL 0x03 (MASK=0xFF)
      { 0xB0,	0x0C,	0xFF,	0x83},	// W#292 - 	
      { 0x02,	0x70,	0xFF,	0x90},	// W#293 - 	SKIP 1 Lines if R7 NOT EQUAL 0x02 (MASK=0xFF)
      { 0xB8,	0x0C,	0xFF,	0x83},	// W#294 - 	
      { 0x01,	0x70,	0xFF,	0x90},	// W#295 - 	SKIP 1 Lines if R7 NOT EQUAL 0x01 (MASK=0xFF)
      { 0x00,	0x0D,	0xFF,	0x83},	// W#296 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#297 - 	
      { 0x04,	0x51,	0x72,	0x00},	// W#298 - 	Set SCRATCH2 to 0x04 (to indicate to SW that SOC wake sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#299 - 	End of Sequence; Byte-2
      { 0x03,	0x51,	0x72,	0x00},	// W#300 - 	Set SCRATCH2 to 0x03 (to indicate to SW that SOC sleep sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#301 - 	End of Sequence; Byte-2
      { 0x80,	0x46,	0x0B,	0x00},	// W#302 - 	1BYTE WRITE 0x80 TO VREF_MSM__EN_CTL1; Enable VREF_MSM
      { 0x18,	0x08,	0x76,	0xA1},	// W#303 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0xF0,	0x0C,	0x1D,	0x89},	// W#304 - 	Restore S4 Primary-Mode to the one before entering sleep
      { 0xF0,	0x0C,	0x23,	0x89},	// W#305 - 	Restore S6 (was S5 on SL) Primary-Mode to the one before entering sleep
      { 0xC6,	0x44,	0x23,	0x11},	// W#306 - 	Enable PFM_IBOOST. In SBL, Value = 0xC6 (Changed from S5A to S6A for Warlock)
      { 0xC5,	0x44,	0x1D,	0x11},	// W#307 - 	Enable PFM_IBOOST
      { 0x02,	0x5A,	0x1D,	0x11},	// W#308 - 	In SBL, value = 0x02
      { 0x06,	0x49,	0x1D,	0x01},	// W#309 - 	Restore S4 Secondary-Mode to AUTO
      { 0x00,	0xDF,	0x14,	0x00},	// W#310 - 	Use 200k=0. Charger clock may lock up at this command CR-0000176241
      { 0x04,	0xDF,	0x14,	0x00},	// W#311 - 	Toggle chicken bit high to unlock charger
      { 0xC0,	0x03,	0xFE,	0xC1},	// W#312 - 	Wait 50us = 960 x 19.2MHz clock-cycles
      { 0x00,	0xDF,	0x14,	0x00},	// W#313 - 	Toggle chicken bit low
      { 0x02,	0x51,	0x72,	0x00},	// W#314 - 	Set SCRATCH2 to 0x02 (to indicate to SW that wake sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#315 - 	End of wake sequence
      { 0xF6,	0xFF,	0x74,	0xA2},	// W#316 - 	Load R2 to R5 with data from location pointed to by R6
      { 0xF4,	0x45,	0xFF,	0x0C},	// W#317 - 	ReStore MODE_CTL1 from R4
      { 0x04,	0x00,	0x76,	0x21},	// W#318 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#319 - 	End of subroutine.
      { 0xF7,	0x40,	0x11,	0x40},	// W#320 - 	Load R7 with the value in 0x1140 => SCHG_OTG_CMD_OTG
      { 0x01,	0x00,	0xF7,	0x2C},	// W#321 - 	Redundant: Mask-out other bits in R7 to only preserve Bit-0 = OTG_EN
      { 0xF8,	0x0B,	0x16,	0x40},	// W#322 - 	Load R8 with the value in 0x160B => SCHG_MISC_POWER_PATH_STATUS
      { 0x01,	0x00,	0xF8,	0x2C},	// W#323 - 	Redundant: Mask-out other bits in R8 to only preserve Bit-0 = VALID_INPUT_POWER_SOURCE
      { 0xF7,	0xF8,	0xF7,	0x32},	// W#324 - 	R7 = R7 || R8 = OTG_EN || VALID_INPUT_POWER_SOURCE
      { 0x01,	0x71,	0x01,	0x90},	// W#325 - 	Skip next config (that sets SCHG Clock to 200kHz) if OTG-Enabled or Valid Charger-Input present
      { 0x02,	0xDF,	0x14,	0x00},	// W#326 - 	USE 200K CLK instead of 19.2Mhz - only if OTG-Disabled or Charger-Input absent
      { 0x18,	0x08,	0x76,	0xA1},	// W#327 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0x60,	0x0D,	0x1D,	0x89},	// W#328 - 	Backup S4 Primary-Mode so that it can be restored during Wake-Up
      { 0x60,	0x0D,	0x23,	0x89},	// W#329 - 	Backup S6 (was S5 on SL) Primary-Mode so that it can be restored during Wake-Up
      { 0x00,	0x5A,	0x1D,	0x11},	// W#330 - 	1BYTE WRITE 0x00 TO S4_CTRL__PS_THRES_ERRAMP; Set PS_VRST to 0mV to prevent mode under/overshoots (CR-175088)
      { 0x06,	0x45,	0x1D,	0x01},	// W#331 - 	PMICPR-1046: Set S4 Primary-Mode to AUTO
      { 0x05,	0x44,	0x1D,	0x11},	// W#332 - 	Disable PFM_IBOOST
      { 0x05,	0x49,	0x1D,	0x01},	// W#333 - 	Set S4 Secondary-Mode to FORCE_PFM
      { 0x04,	0x45,	0x23,	0x01},	// W#334 - 	Set S6 (was S5 on SL) to RM
      { 0x05,	0x44,	0x23,	0x11},	// W#335 - 	Disable PFM_IBOOST
      { 0xF8,	0x43,	0x4E,	0x41},	// W#336 - 	Read LDO15A.VSET_VALID_UB (UIM1_LDO Enable & VSET_UB) & Store in R8
      { 0x07,	0x85,	0xFF,	0x92},	// W#337 - 	Skip Disabling VREF_MSM if UIM1 VALID_UB > 0x07 (i.e On @ ~3V)
      { 0xF8,	0x43,	0x50,	0x41},	// W#338 - 	Else Read LDO17A.VSET_VALID_UB (UIM2_LDO Enable & VSET_UB) & Store in R8
      { 0x07,	0x85,	0xFF,	0x90},	// W#339 - 	Skip Disabling VREF_MSM if UIM2 VALID_UB > 0x07 (i.e On @ ~3V)
      { 0x00,	0x46,	0x0B,	0x00},	// W#340 - 	Disable VREF_MSM if None of the UIM_LDOs are On @3V (Can be either On @1.8V or Off)
      { 0x00,	0x57,	0x08,	0x00},	// W#341 - 	Disable PMIC WD - TODO (check if this needs to be conditionally re-enabled in Wake)
      { 0x01,	0x51,	0x72,	0x00},	// W#342 - 	Set SCRATCH2 to 0x01 (to indicate to SW that sleep sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#343 - 	End of the Sleep entry sequence
      { 0xF4,	0x45,	0xFF,	0x44},	// W#344 - 	Store MODE_CTL1 into R4
      { 0xF6,	0xFF,	0x74,	0xA6},	// W#345 - 	Load data into location pointed to by R6
      { 0x04,	0x00,	0x76,	0x21},	// W#346 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#347 - 	End of subroutine.
      { 0x7F,	0x00,	0xFC,	0x2C},	// W#348 - 	Clear EOSR_IN_QUEUE_STS = MISC_SPARE_5<7> [AND RC with 0x7F = 0'b0111_1111]
      { 0x20,	0xC1,	0x20,	0x90},	// W#349 - 	GOTO(AFP_BATT_TEMP_CHECK_SEQUENCE) if AFP_BATT_TEMP_STS = MISC_SPARE_5<5> is Set [Mask = 0x20 = 0'b0010_0000]
      { 0xC4,	0x0D,	0xFF,	0x83},	// W#350 - 	Else, proceed to check FG_ESR_QM_STS if AFP_BATT_TEMP_STS is Clear
      { 0xF4,	0x53,	0x09,	0x40},	// W#351 - 	R4 <= HOT_THRESHOLD [MISC_SPARE_4]
      { 0xF3,	0x52,	0x09,	0x40},	// W#352 - 	R3 <= COLD_THRESHOLD [MISC_SPARE_3]
      { 0xF1,	0x51,	0x41,	0x40},	// W#353 - 	R1 <= Current BATT_TEMP_MSB
      { 0xF0,	0x50,	0x41,	0x40},	// W#354 - 	R0 <= Current BATT_TEMP_LSB
      { 0xFF,	0xFF,	0x71,	0x38},	// W#355 - 	SHIFT R1 MSB Left 5-times to make Orig <2:0> to New <7:5>
      { 0xFF,	0xFF,	0x71,	0x38},	// W#356 - 	
      { 0xFF,	0xFF,	0x71,	0x38},	// W#357 - 	
      { 0xFF,	0xFF,	0x71,	0x38},	// W#358 - 	
      { 0xFF,	0xFF,	0x71,	0x38},	// W#359 - 	
      { 0xFF,	0xFF,	0x70,	0x3C},	// W#360 - 	SHIFT R0 LSB Right 3-times to make Orig <7:3> to New <4:0>
      { 0xFF,	0xFF,	0x70,	0x3C},	// W#361 - 	
      { 0xFF,	0xFF,	0x70,	0x3C},	// W#362 - 	
      { 0xF0,	0xF1,	0xF2,	0x32},	// W#363 - 	R2 = R1 <7:5> OR R0 <4:0> = REAL_TEMP
      { 0xF2,	0xF3,	0x75,	0x26},	// W#364 - 	R5 (DIFF) = R2 (REAL) - R3 (COLD_TH)
      { 0xFF,	0xF6,	0xFF,	0x91},	// W#365 - 	Carry=1 if DIFF < 0 i.e. REAL < COLD_TH => Open BATFET
      { 0xF2,	0xF4,	0x75,	0x26},	// W#366 - 	R5 (DIFF) = R2 (REAL) - R4 (HOT_TH)
      { 0xFF,	0xF6,	0xFF,	0x90},	// W#367 - 	Carry=1 if DIFF < 0 i.e. REAL < HOT_TH => Bat-Temp is Safe. Skip AFP, Proceed to Next Step.
      { 0x01,	0x41,	0x16,	0x10},	// W#368 - 	REAL not in <HOT_TH:COLD_TH> Range => Execute AFP-Shutdown
      { 0x10,	0xC1,	0x10,	0x90},	// W#369 - 	GOTO(FG_ESR_FAILSAFE_TIMER_EVAL) if FG_ESR_QM_STS = MISC_SPARE_5<4> is Set [Mask = 0x10 = 0'b0001_0000]
      { 0x1C,	0x0F,	0xFF,	0x83},	// W#370 - 	Else (FG Not in ESR_QUIET_MODE). Finish-Up using CLIENT2_COMMON_END_RESCHED_EVAL_SEQUENCE
      { 0xF0,	0x55,	0x09,	0x40},	// W#371 - 	Load the Loop counter into R0
      { 0x00,	0x01,	0xFF,	0x92},	// W#372 - 	If Timer is expired - enable ESR_PD and Set Interrupt
      { 0x01,	0x00,	0x70,	0x24},	// W#373 - 	Else Decrement Loop counter
      { 0xF0,	0x55,	0x09,	0x08},	// W#374 - 	Store updated Loop counter back to MISC_SPARE_6
      { 0x1C,	0x0F,	0xFF,	0x83},	// W#375 - 	Reschedule using CLIENT2_COMMON_END_RESCHED_EVAL_SEQUENCE
      { 0x02,	0x15,	0x73,	0x00},	// W#376 - 	Enable PBS_Client2 SEQ_ENDED Interrupt - will fire after this seq exits
      { 0xF4,	0x50,	0x73,	0x40},	// W#377 - 	Read the Scratch1 register to point to the requested sequence
      { 0x02,	0x00,	0xF4,	0x30},	// W#378 - 	R4 is loaded with Flags Bit<1>=1 --> FG_ESR_SEQ [OR R4 with Mask = 0x02 = 0'b0000_0010]
      { 0xEF,	0x00,	0xF4,	0x2C},	// W#379 - 	R4 is loaded with Flags Bit<4>=0 --> EXIT_QUIET [AND R4 with Mask = 0xEF = 0'b1110_1111]
      { 0xF4,	0x0E,	0xFF,	0x83},	// W#380 - 	Goto FG_ESR_QUIET_MODE_EXIT with appropraite Flags configured for Forced FG_ESR_QUIET_MODE_EXIT
      { 0xF8,	0x08,	0xC9,	0x40},	// W#381 - 	Readback GPIO10 (WCSS_VCTRL) status and store to Buffer R8
      { 0x01,	0x81,	0x01,	0x90},	// W#382 - 	If GPIO09 is high
      { 0x1C,	0x0E,	0xFF,	0x83},	// W#383 - 	GPI009 is low
      { 0xF9,	0x4E,	0x09,	0x40},	// W#384 - 	Read MISC register for L5 active voltage and store LSB to Buffer R9
      { 0xFA,	0x4F,	0x09,	0x40},	// W#385 - 	Read MISC register for L5 active voltage and store MSB to Buffer RA
      { 0xF9,	0x40,	0x44,	0x09},	// W#386 - 	Write Buffer R9 value to L5A VSET (LSB) (note: PDM error incorrectly shows 2BYTE)
      { 0xFA,	0x41,	0x44,	0x09},	// W#387 - 	Write Buffer RA value to L5A VSET (MSB) (note: PDM error incorrectly shows 2BYTE)
      { 0x07,	0x45,	0x44,	0x01},	// W#388 - 	Set LDO05 mode to NPM [GDG] There is no HW_EN to follow for this. And no PBS Vote for secondary mode
      { 0x01,	0x5A,	0x09,	0x00},	// W#389 - 	Indicate that the sequence set regulators to active state
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#390 - 	
      { 0xF9,	0x4C,	0x09,	0x40},	// W#391 - 	Read MISC register for L5 retention voltage and store LSB to Buffer R9
      { 0xFA,	0x4D,	0x09,	0x40},	// W#392 - 	Read MISC register for L5 retention voltage and store MSB to Buffer RA
      { 0xF9,	0x40,	0x44,	0x09},	// W#393 - 	Write Buffer R9 value to L5A VSET (LSB)
      { 0xFA,	0x41,	0x44,	0x09},	// W#394 - 	Write Buffer RA value to L5A VSET (MSB)
      { 0x04,	0x45,	0x44,	0x01},	// W#395 - 	Set LDO05 mode to RET [GDG] There is no HW_EN to follow for this. And no PBS Vote for secondary mode
      { 0x00,	0x5A,	0x09,	0x00},	// W#396 - 	Indicate that the sequence set regulators to retention state
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#397 - 	
      { 0xF8,	0x08,	0xC4,	0x40},	// W#398 - 	Readback GPIO05 (WLAN_RF_VCTRL) status and store to Buffer R8
      { 0x01,	0x81,	0x01,	0x90},	// W#399 - 	SKIP If GPIO3 is high
      { 0x50,	0x0E,	0xFF,	0x83},	// W#400 - 	GPI005 is low
      { 0x40,	0x4F,	0x23,	0x01},	// W#401 - 	Set PBS_EN and PBS_MODE to use Secondary Mode [GDG] set MODE Secondary to Auto in MODE_CTL2
      { 0x01,	0x5B,	0x09,	0x00},	// W#402 - 	set state to 0x1 to indicate that active state was executed
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#403 - 	
      { 0x00,	0x4F,	0x23,	0x01},	// W#404 - 	Clear PBS_EN and PBS_MODE and go to primary mode
      { 0x00,	0x5B,	0x09,	0x00},	// W#405 - 	Set state to 0x0 to indicate that retention state was executed
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#406 - 	
      { 0xF2,	0x18,	0x1C,	0x40},	// W#407 - 	Readback BUA interrupts and store status to Buffer R2
      { 0x01,	0x20,	0x01,	0x91},	// W#408 - 	If not BATT_ALARM_INT latched
      { 0x01,	0xDB,	0x4E,	0x11},	// W#409 - 	1BYTE WRITE 0x01 TO LDO015__PERPH_RESET_CTL4; SW Reset for LDO15
      { 0x01,	0xDB,	0x50,	0x11},	// W#410 - 	1BYTE WRITE 0x01 TO LDO17__PERPH_RESET_CTL4; SW Reset for LDO17
      { 0x02,	0x20,	0x02,	0x90},	// W#411 - 	If not UICC1_INT latched
      { 0x01,	0xDB,	0x4E,	0x11},	// W#412 - 	SW Reset for LDO15A (UIM1)
      { 0x04,	0x20,	0x04,	0x90},	// W#413 - 	If not UICC2_INT latched
      { 0x01,	0xDB,	0x50,	0x11},	// W#414 - 	SW Reset for LDO17 (UIM2)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#415 - 	
      { 0xF4,	0x50,	0x73,	0x40},	// W#416 - 	Read the Scratch1 register to point to the requested sequence
      { 0x01,	0x40,	0x01,	0x91},	// W#417 - 	Skip if Bit 0 not set (S6 voltage check)
      { 0xF4,	0x51,	0x73,	0x08},	// W#418 - 	Set Scratch2 to value in Scratch1 to show SW that request was read
      { 0xA0,	0x0E,	0xFF,	0x83},	// W#419 - 	
      { 0x02,	0x40,	0x02,	0x90},	// W#420 - 	Skip if Bit 1 not set (FG_ESR_QUIET_MODE_CTRL_SEQUENCE)
      { 0xEC,	0x0E,	0xFF,	0x83},	// W#421 - 	
      { 0xFF,	0x51,	0x73,	0x00},	// W#422 - 	Only Bits <3:0> supported for CMDs. Set Scratch2 to 0xFF to indicate an error if none of those were set.
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#423 - 	
      { 0xF8,	0x42,	0x20,	0x41},	// W#424 - 	
      { 0xF9,	0x43,	0x20,	0x41},	// W#425 - 	
      { 0xEE,	0x02,	0x7A,	0x29},	// W#426 - 	Load R11:R10 with 0x2ee (750mV)
      { 0xF8,	0xFA,	0x7A,	0x27},	// W#427 - 	
      { 0x00,	0xA6,	0x00,	0x90},	// W#428 - 	If the voltage setting is less than 700mV
      { 0xC4,	0x0E,	0xFF,	0x83},	// W#429 - 	
      { 0xC5,	0x44,	0x20,	0x01},	// W#430 - 	S6_CTRL_PFM_CTL = 0xC5
      { 0x84,	0x4B,	0x21,	0x11},	// W#431 - 	Set per Orlando recommendations
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#432 - 	
      { 0x08,	0x02,	0x7A,	0x29},	// W#433 - 	Load R11:R10 with 0x208 (520mV)
      { 0xF8,	0xFA,	0x7A,	0x27},	// W#434 - 	
      { 0x00,	0xA6,	0x00,	0x90},	// W#435 - 	If the voltage setting is less than 700mV
      { 0xE0,	0x0E,	0xFF,	0x83},	// W#436 - 	
      { 0xC6,	0x44,	0x20,	0x01},	// W#437 - 	S5_CTRL_PFM_CTL = 0xC6
      { 0x83,	0x4B,	0x21,	0x11},	// W#438 - 	Set per Orlando recommendations
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#439 - 	
      { 0xC7,	0x44,	0x20,	0x01},	// W#440 - 	S5_CTRL_PFM_CTL = 0xC7
      { 0x83,	0x4B,	0x21,	0x11},	// W#441 - 	Set per Orlando recommendations
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#442 - 	
      { 0x10,	0x40,	0x10,	0x90},	// W#443 - 	Skip over to FG_ESR_QUIET_MODE_EXIT if Bit-4 of R4 was Clear
      { 0x04,	0x0F,	0xFF,	0x83},	// W#444 - 	
      { 0xEF,	0x00,	0xFC,	0x2C},	// W#445 - 	Clear FG_ESR_QM_STS = MISC_SPARE_5<4> [AND RC with 0xEF = 0'b1110_1111]
      { 0x02,	0x00,	0xFB,	0x30},	// W#446 - 	Set ESR_PD_MODE to CRG_DSC PD --> ESR_PULSES_ALLOWED
      { 0x00,	0x00,	0x70,	0x28},	// W#447 - 	Update FG_ESR_QM_FAILSAFE_TIMER Value to Min 0x00 = 0 secs (Redundant)
      { 0x10,	0x0F,	0xFF,	0x83},	// W#448 - 	
      { 0x10,	0x00,	0xFC,	0x30},	// W#449 - 	Set FG_ESR_QM_STS = MISC_SPARE_5<4> [OR RC with 0x10 = 0'b0001_0000]
      { 0x0C,	0x00,	0xFB,	0x2C},	// W#450 - 	Set ESR_PD_MODE to NO_PULL --> ESR_PULSES_STOPPED
      { 0xFF,	0x00,	0x70,	0x28},	// W#451 - 	Update FG_ESR_QM_FAILSAFE_TIMER Value to Max 0xFF = 255 (=> Min 8.5 minutes)
      { 0xFB,	0x69,	0x41,	0x08},	// W#452 - 	Configure ESR_PD_CFG as per updated value
      { 0xF0,	0x55,	0x09,	0x08},	// W#453 - 	Load FG_ESR_QM_FAILSAFE_TIMER as per updated value into MISC_SPARE_6
      { 0xF4,	0x51,	0x73,	0x08},	// W#454 - 	Set PBS_CLIENT2.SCRATCH2 to ACK to SW the Return-Value of the Calling-Sequence
      { 0x00,	0xC5,	0x30,	0x90},	// W#455 - 	Check MISC_SPARE_5 Bits <5> & <4> (Mask = 0x30 = 0'b0011_0000) for EOSR Requirements on AFP or FG
      { 0x34,	0x0F,	0xFF,	0x83},	// W#456 - 	GOTO End of this Check, if None of AFP & FG need a Rescheduled Trigger
      { 0x80,	0xC1,	0x80,	0x92},	// W#457 - 	Check EOSR_IN_QUEUE_STS = MISC_SPARE_5<7> (Mask = 0x80 = 0'b1000_0000) if any EOSR was initiated before current seq execution
      { 0x80,	0x00,	0xFC,	0x30},	// W#458 - 	EOSR was not initiated before. Set EOSR_IN_QUEUE_STS = MISC_SPARE_5<7> here (Mask 0x80 = 0'b1000_0000)
      { 0xFC,	0x54,	0x09,	0x08},	// W#459 - 	Set EOSR_IN_QUEUE_STS Flag to inform any future CLIENT2_SW_TRIGGER Seq to not Reset/Disturb EOSR-Timer (unless EOSR_OVERRIDE_STS is Set)
      { 0xFF,	0xFF,	0xFF,	0xC5},	// W#460 - 	Fire EOSR-Timer to expire after 2-secs & then --> Trigger CLIENT2_EOSR_TIMER_EXPIRED_SEQUENCE
      { 0xFC,	0x54,	0x09,	0x08},	// W#461 - 	Update the Final Value of RC (modified in certain flows) to MISC_SPARE_5
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#462 - 	End this Sequence here. EOSR-Trigger may or may-not happen later based on current EOSR_IN_QUEUE_STS
      { 0x70,	0x0F,	0xFF,	0x9B},	// W#463 - 	Assert CHG Clock Req
      { 0xF0,	0x0E,	0x13,	0x40},	// W#464 - 	R0 = Read [7] UFP_DFP_MODE
      { 0x00,	0x01,	0x80,	0x94},	// W#465 - 	Skip next 3 lines if [7]=0 i.e. UFP mode
      { 0xF7,	0x59,	0x13,	0x40},	// W#466 - 	R7=0x1358
      { 0x02,	0x00,	0xF7,	0x30},	// W#467 - 	Set R7 bit1=1
      { 0xF7,	0x59,	0x13,	0x08},	// W#468 - 	UFP_DFT_MODE=1 --> in DFP mode --> Disable crude sensor 0x1359[1]=1
      { 0x80,	0x0F,	0xFF,	0x9B},	// W#469 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#470 - 	
      { 0xF1,	0x68,	0x13,	0x40},	// W#471 - 	Check if SCHG is in DRP-Mode
      { 0x00,	0x10,	0x06,	0x90},	// W#472 - 	Skip next 1 line if [2:1] != 0 i.e. EOS if SCHG not in DRP-Mode
      { 0x02,	0x42,	0x72,	0x00},	// W#473 - 	Trigger PBS Client1, Trigger 1(NC)
      { 0x80,	0x0F,	0xFF,	0x9B},	// W#474 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#475 - 	
      { 0xF0,	0xDF,	0x14,	0x40},	// W#476 - 	
      { 0x01,	0x00,	0xF0,	0x30},	// W#477 - 	R0 = R0 || 0x01 i.e. Set Bit-0 to High
      { 0xF0,	0xDF,	0x14,	0x08},	// W#478 - 	Writeback SCHG_DC.CLOCK_CONTROL with updated R0 (with Bit-0 set High)
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#479 - 	
      { 0xF0,	0xDF,	0x14,	0x40},	// W#480 - 	
      { 0x08,	0x01,	0x08,	0x91},	// W#481 - 	Bit-3 = 1 => APPS is awake, leave Bit-0 High
      { 0xFE,	0x00,	0xF0,	0x2C},	// W#482 - 	Bit-3 = 0 => APPS is asleep, clear CLK_REQ. R0 = R0 && 0xFE
      { 0xF0,	0xDF,	0x14,	0x08},	// W#483 - 	Write back SCHG_DC.CLOCK_CONTROL with updated R0 (with Bit-0 cleared low)
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#484 - 	
      { 0xF4,	0x50,	0x74,	0x40},	// W#485 - 	Read the Argument Register to point to the requested sequence
      { 0x01,	0x40,	0x01,	0x90},	// W#486 - 	Skip if Bit 0 not set (AFP BATT_TEMP CTRL)
      { 0xA8,	0x0F,	0xFF,	0x83},	// W#487 - 	
      { 0xFF,	0x51,	0x74,	0x00},	// W#488 - 	Only Bit <0> supported for CMDs. Set Scratch2 to 0xFF to indicate an error if none of those were set.
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#489 - 	
      { 0xFC,	0x54,	0x09,	0x40},	// W#490 - 	PBSREG.RC will store MISC_SPARE_5
      { 0x10,	0x40,	0x10,	0x91},	// W#491 - 	Skip over to AFP_BATT_TEMP_CHECK_DISABLE if Bit-4 of R4 was Clear
      { 0x20,	0x00,	0xFC,	0x30},	// W#492 - 	Set AFP_BATT_TEMP_CHK_STS = MISC_SPARE_5<5> [OR RC with 0x20 = 0'b0010_0000]
      { 0xBC,	0x0F,	0xFF,	0x83},	// W#493 - 	End with AFP_BATT_TEMP_CTRL_DONE
      { 0xDF,	0x00,	0xFC,	0x2C},	// W#494 - 	Clear AFP_BATT_TEMP_CHK_STS = MISC_SPARE_5<5> [AND RC with 0xDF = 0'b1101_1111]
      { 0xFC,	0x54,	0x09,	0x08},	// W#495 - 	Update MISC_SPARE_5 with updated AFP_BATT_TEMP_CHK_STS
      { 0xF4,	0x51,	0x74,	0x08},	// W#496 - 	Set PBS_CLIENT3.SCRATCH2 to value in PBS_CLIENT3.SCRATCH1 to show SW that request was executed
      { 0x20,	0x42,	0x73,	0x00},	// W#497 - 	Initiate PBS_CLIENT2.TRIG5 [CLIENT2_BIT5_CHECK]. Round about way, since Client3 has NO EOSR
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#498 - 	
      { 0x44,	0x44,	0x44,	0x44},	// W#499 - 	; S4: B3: Reserved
      { 0x55,	0x55,	0x55,	0x55},	// W#500 - 	; S5: B3: Reserved
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#501 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#502 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#503 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#504 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#505 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#506 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#507 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#508 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#509 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#510 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0x09,	0x0E,	0xFF,	0xF8},	// W#511 - 	Inserted by compiler.
   },
   // "PM670_RAM_b0x19_v0x14"
   {
      //data  offset  base_addr  sid
      { 0x52,	0x53,	0x42,	0x50},	// W#0 - 	0x800 RAM signature PBSR" Checksum"
      { 0x19,	0x14,	0x01,	0x18},	// W#1 - 	0x804 Header offset, Header Version, PBS RAM Revision, PBS RAM Branch
      { 0x2C,	0x08,	0xFF,	0x83},	// W#2 - 	0x808 Start of trigger jump table:
      { 0x6C,	0x08,	0xFF,	0x83},	// W#3 - 	0x80C
      { 0x98,	0x08,	0xFF,	0x83},	// W#4 - 	0x810
      { 0x00,	0x09,	0xFF,	0x83},	// W#5 - 	0x814
      { 0x84,	0x0F,	0xFF,	0x83},	// W#6 - 	0x818 Fixed Offset = RAM-Base-Addr + 0x18 + 0x00 => SLEEP-SET
      { 0x8C,	0x0F,	0xFF,	0x83},	// W#7 - 	0x81C Fixed Offset = RAM-Base-Addr + 0x18 + 0x04 => PON X Reasons
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#8 - 	0x820 Fixed Offset = RAM-Base-Addr + 0x18 + 0x08 => Empty (No RTRR)/Spare
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#9 - 	0x824 Fixed Offset = RAM-Base-Addr + 0x18 + 0x0C => Empty (No HLOS)/Spare
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#10 - 	0x828 Fixed Offset = RAM-Base-Addr + 0x18 + 0x10 => Empty (Spare registers)/LPG
      { 0xFF,	0x48,	0x71,	0x00},	// W#11 - 	WRITE 0xFF TO PBS_CLIENT0__TRIG_LATCHED_CLR; Clear all latched sequences in PBS PON Client 0
      { 0xFB,	0x08,	0x08,	0x40},	// W#12 - 	READ PON__FSM_STATUS TO RB; Store PON_FSM_STATUS in RB for Older OTP-Revs < v0x05
      { 0xFC,	0x08,	0x08,	0x40},	// W#13 - 	READ PON__FSM_STATUS TO RC; Store PON_FSM_STATUS in RC for Newer OTP-Revs >= v0x05
      { 0x04,	0xC0,	0xFF,	0x90},	// W#14 - 	SKIP 1 Lines if RB NOT EQUAL 0x04 (MASK=0xFF); DVDD_CONFIG = State 4
      { 0x14,	0x00,	0xFF,	0x83},	// W#15 - 	GOTO OTP_DVDD_CONFIG (OTP) & End there
      { 0x01,	0xC0,	0xFF,	0x91},	// W#16 - 	Skip if RB NOT EQUAL 0x01 (PON = State 1)
      { 0x00,	0x57,	0x08,	0x00},	// W#17 - 	PMICPR-475: Disable PMIC_WD only at the beginning of PON or WR
      { 0x14,	0x00,	0xFF,	0x83},	// W#18 - 	GOTO OTP_DVDD_CONFIG (OTP) & End there
      { 0x06,	0xC0,	0xFF,	0x90},	// W#19 - 	Skip if RB NOT EQUAL 0x06 (FAULT = State 6)
      { 0x20,	0x09,	0xFF,	0x83},	// W#20 - 	GOTO CLIENT0_FAULT_SEQUENCE & End there
      { 0x02,	0xC0,	0xFF,	0x90},	// W#21 - 	Skip if RB NOT EQUAL 0x02 (POFF = State 2)
      { 0x88,	0x09,	0xFF,	0x83},	// W#22 - 	GOTO CLIENT0_POFF_SEQUENCE & End there
      { 0x07,	0xC0,	0xFF,	0x90},	// W#23 - 	Skip if RB NOT EQUAL 0x07 (WARM RESET = State 7)
      { 0xE8,	0x0A,	0xFF,	0x83},	// W#24 - 	GOTO CLIENT0_WARM_RESET_SEQUENCE & End there
      { 0x00,	0x91,	0x08,	0x00},	// W#25 - 	Undefined State - NACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#26 - 	End Sequence
      { 0xF3,	0x0A,	0x72,	0x40},	// W#27 - 	Read Client 1 trigger latch status and store to Buffer R3
      { 0x02,	0x30,	0x02,	0x91},	// W#28 - 	Skip if not Bit 1 = NC (WL uses it for Type-C UFP crude sensing WA)
      { 0x02,	0x48,	0x72,	0x00},	// W#29 - 	Clear Latched Trigger - Bit 1
      { 0x28,	0x0C,	0xFF,	0x83},	// W#30 - 	CL1_TRIG1: CLIENT1_TRIG1_SCHG_WA
      { 0x04,	0x30,	0x04,	0x91},	// W#31 - 	Skip if not Bit 2 = EOSR_TIMER ; Client1_SCHG_EOSR
      { 0x04,	0x48,	0x72,	0x00},	// W#32 - 	Clear Latched Trigger - Bit 2
      { 0xF0,	0x0B,	0xFF,	0x83},	// W#33 - 	CL1_TRIG2: GOTO_CLIENT1_SCHG_EOSR & End there
      { 0x80,	0x30,	0x80,	0x91},	// W#34 - 	Skip if not Bit 7 = SW Trigger (Sleep/Wake)
      { 0x80,	0x48,	0x72,	0x00},	// W#35 - 	Clear Latched Trigger - Bit 7
      { 0x2C,	0x0C,	0xFF,	0x83},	// W#36 - 	CL1_TRIG7: CLIENT1_SYSTEM_SLEEP_WAKE_SEQUENCE
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#37 - 	
      { 0xFC,	0x54,	0x09,	0x40},	// W#38 - 	PBSREG.RC will store MISC_SPARE_5 (used in CL2_TRIG0 & CL2_TRIG7)
      { 0xFB,	0x69,	0x41,	0x40},	// W#39 - 	PBSREG.RB will store the latest value of FG_BATT_INFO.ESR_PULL_DOWN_CFG (used in CL2_TRIG0 & CL2_TRIG7)
      { 0x03,	0x16,	0x73,	0x00},	// W#40 - 	Disable PBS_Client2 All (SEQ_ENDED) Interrupts - won't fire for any of CLIENT2 Sequences, except when re-enabled
      { 0xF4,	0x0A,	0x73,	0x40},	// W#41 - 	1BYTE READ PBS_CLIENT2__TRIG_LATCHED TO R4; Read Client 2 trigger latch status and store to Buffer R4
      { 0x01,	0x40,	0x01,	0x91},	// W#42 - 	Skip if not Bit 0 = DTEST MUX / EOSR_TIMER
      { 0x01,	0x48,	0x73,	0x00},	// W#43 - 	1BYTE WRITE 0x01 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 0
      { 0x28,	0x0D,	0xFF,	0x83},	// W#44 - 	CL2_TRIG0: CLIENT2_EOSR_TIMER_EXPIRED_SEQUENCE
      { 0x02,	0x40,	0x02,	0x91},	// W#45 - 	Skip if not Bit 1 = EUD(SCHG)
      { 0x02,	0x48,	0x73,	0x00},	// W#46 - 	1BYTE WRITE 0x02 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 1
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#47 - 	CL2_TRIG1:EMPTY => Replace with GOTO once defined.
      { 0x04,	0x40,	0x04,	0x91},	// W#48 - 	Skip if not Bit 2 = GPIO09 (WCSS_VCTRL)
      { 0x04,	0x48,	0x73,	0x00},	// W#49 - 	1BYTE WRITE 0x04 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 2
      { 0xAC,	0x0D,	0xFF,	0x83},	// W#50 - 	CL2_TRIG2: CLIENT2_WCSS_VCTRL_SEQUENCE
      { 0x08,	0x40,	0x08,	0x91},	// W#51 - 	Skip if not Bit 3 = GPIO05 (WLAN_RF_VCTRL)
      { 0x08,	0x48,	0x73,	0x00},	// W#52 - 	1BYTE WRITE 0x08 TO PBS_CLIENT2__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 3
      { 0xF0,	0x0D,	0xFF,	0x83},	// W#53 - 	CL2_TRIG3: CLIENT2_WLAN_RF_VCTRL_SEQUENCE
      { 0x20,	0x40,	0x20,	0x91},	// W#54 - 	Skip if not Bit 5 - Used as Internal Re-Trigger from CLIENT3_SW_TRIGGER
      { 0x20,	0x48,	0x73,	0x00},	// W#55 - 	
      { 0xD4,	0x0E,	0xFF,	0x83},	// W#56 - 	Reschedule using CLIENT2_COMMON_END_RESCHED_EVAL_SEQUENCE
      { 0x40,	0x40,	0x40,	0x91},	// W#57 - 	Skip if not Bit 6 = BUA
      { 0x40,	0x48,	0x73,	0x00},	// W#58 - 	
      { 0x14,	0x0E,	0xFF,	0x83},	// W#59 - 	CL2_TRIG6: CLIENT2_BUA_SEQUENCE
      { 0x80,	0x40,	0x80,	0x91},	// W#60 - 	Skip if not Bit 7 = SW Trigger
      { 0x80,	0x48,	0x73,	0x00},	// W#61 - 	
      { 0x38,	0x0E,	0xFF,	0x83},	// W#62 - 	CL2_TRIG7: CLIENT2_SW_TRIGGER
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#63 - 	
      { 0xF5,	0x0A,	0x74,	0x40},	// W#64 - 	Read Client 3 trigger latch status and store to Buffer R5
      { 0x04,	0x50,	0x04,	0x91},	// W#65 - 	Skip if NOT Bit 2 = Charger Test MUX
      { 0x04,	0x48,	0x74,	0x00},	// W#66 - 	Clear Latched Trigger - Bit 2
      { 0xF4,	0x0E,	0xFF,	0x83},	// W#67 - 	CL3_TRIG2: Client3_SCHG_PD_WA1_Crude_Sensor
      { 0x80,	0x50,	0x80,	0x91},	// W#68 - 	Skip if NOT Bit 7 = SW Trigger
      { 0x80,	0x48,	0x74,	0x00},	// W#69 - 	1BYTE WRITE 0x80 TO PBS_CLIENT3__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 7
      { 0x4C,	0x0F,	0xFF,	0x83},	// W#70 - 	CL3_TRIG7: CLIENT3_SW_TRIGGER
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#71 - 	
      { 0x05,	0xDA,	0x2C,	0x10},	// W#72 - 	PMICPR-827: PM670 (Groot) REF_BYP stays on after fault POFF (due to fix in PMICPR-801)
      { 0x2E,	0x44,	0x60,	0x00},	// W#73 - 	PMICPR-942: CalRC offset compensation (+55ppm offset for RTC when PMIC is OFF)
      { 0x80,	0x0B,	0xFF,	0x9B},	// W#74 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0xF4,	0x80,	0x08,	0x00},	// W#75 - 	When configuring the KPDPWR_N Stage 1 reset timer, corresponding PON trigger must be disabled during the re-configuration;PMICPR-356
      { 0x0B,	0x00,	0xFF,	0xC1},	// W#76 - 	Wait 300us to REGISTER get updated.
      { 0x42,	0x90,	0x16,	0x00},	// W#77 - 	1BYTE WRITE Force STAT low ([7:6]=b'01) so that SMB goes to suspend
      { 0x02,	0x66,	0x13,	0x00},	// W#78 - 	1BYTE WRITE Revert USB5/1 setting to 500mA
      { 0x38,	0x63,	0x13,	0x00},	// W#79 - 	0x1363 [7] WIPWR_RST_EUD_CFG = 0
      { 0x00,	0x53,	0x10,	0x00},	// W#80 - 	1BYTE WRITE 0x00 TO SCHG_CHGR__CFG; Disable VDISCHG_EN_CFG to reduce off current.
      { 0x01,	0x45,	0x16,	0x00},	// W#81 - 	Enable Charger Shutdown CR- 0000171925
      { 0x24,	0x68,	0x13,	0x00},	// W#82 - 	Charger WA-31. Revert to UFP mode (same as FTRIM setting) to avoid unexpected PON
      { 0x04,	0xDF,	0x14,	0x00},	// W#83 - 	CR176140: Set chicken bit high to avoid lockup at following PON
      { 0x88,	0x48,	0x14,	0x01},	// W#84 - 	1BYTE WRITE 0x88 TO S1_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x17,	0x01},	// W#85 - 	1BYTE WRITE 0x88 TO S2_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x1A,	0x01},	// W#86 - 	1BYTE WRITE 0x88 TO S3_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x1D,	0x01},	// W#87 - 	1BYTE WRITE 0x88 TO S4_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x20,	0x01},	// W#88 - 	1BYTE WRITE 0x88 TO S5_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x23,	0x01},	// W#89 - 	1BYTE WRITE 0x88 TO S6_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x80,	0x48,	0x52,	0x01},	// W#90 - 	PMICPR-711: Re-enable L19A PD in FAULT (since it was disabled in SBL)
      { 0xEA,	0x01,	0xFF,	0xC1},	// W#91 - 	WAIT 15016 us; Wait 15ms for VREGs to discharge completely before allowing the next PON trigger
      { 0xF6,	0xC8,	0x08,	0x40},	// W#92 - 	1BYTE READ PON__FAULT_REASON1 TO R6; Read FAULT_REASON1 and store to Buffer R6
      { 0x40,	0x61,	0x40,	0x91},	// W#93 - 	SKIP 2 Lines if R6 EQUAL 0x40 (MASK=0x40); Skip next 2 lines if Bit-6 (UVLO) = 1
      { 0x00,	0x7F,	0x08,	0x00},	// W#94 - 	1BYTE WRITE 0x00 TO PON__SMPL_CTL; Disable SMPL
      { 0x00,	0x46,	0x40,	0x00},	// W#95 - 	PMICPR-407; This is to fix the issue of REF_BYP stays ON during a fault due to a FG write which ignores all resets.
      { 0x40,	0x91,	0x08,	0x00},	// W#96 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#97 - 	End of Sequence
      { 0x00,	0x7F,	0x08,	0x00},	// W#98 - 	1BYTE WRITE 0x00 TO PON__SMPL_CTL; Disable SMPL
      { 0xF4,	0x80,	0x08,	0x00},	// W#99 - 	PMICPR-1039: Groot: Re-enable KPDPWR_N PON_TRIGGER in POFF-Sequence
      { 0x2E,	0x44,	0x60,	0x00},	// W#100 - 	PMICPR-942: CalRC offset compensation (+55ppm offset for RTC when PMIC is OFF)
      { 0x6C,	0x0B,	0xFF,	0x9B},	// W#101 - 	GOTO_SUB_NOID ASSERT_EXT_RESET; Set PON_RESET_N and PON_OUT low
      { 0x80,	0x0B,	0xFF,	0x9B},	// W#102 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x04,	0x98,	0x08,	0x10},	// W#103 - 	1BYTE WRITE 0x04 TO PON__RESET_CTL; Assert shutdown1_rb
      { 0x03,	0x00,	0xFF,	0xC1},	// W#104 - 	WAIT 91 us; Wait 3 SLEEP_CLK cycles for reset changes to propagate through clock domains
      { 0x06,	0x98,	0x08,	0x10},	// W#105 - 	1BYTE WRITE 0x06 TO PON__RESET_CTL; De-assert shutdown1_rb
      { 0x00,	0x40,	0xC7,	0x00},	// W#106 - 	Configure GPIO08 to Dig INPUT
      { 0x05,	0x42,	0xC7,	0x00},	// W#107 - 	Set SLB to No pull
      { 0x38,	0x63,	0x13,	0x00},	// W#108 - 	0x1363 [7] WIPWR_RST_EUD_CFG = 0
      { 0x80,	0x0A,	0x14,	0x89},	// W#109 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S1_CTRL; APC0 (Kryo Silver)
      { 0x80,	0x0A,	0x20,	0x89},	// W#110 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S5_CTRL; Modem SS Q6
      { 0x34,	0x03,	0x76,	0x29},	// W#111 - 	WAIT 25024 us; Wait 25ms.  Store to Buffer R6.  0x334
      { 0x8C,	0x0A,	0xFF,	0x9B},	// W#112 - 	
      { 0x80,	0x0A,	0x51,	0x88},	// W#113 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LN_BB_CLK1; LNBBCLK1
      { 0x80,	0x0A,	0x49,	0x89},	// W#114 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO10; PHY-PLL-BB_CLK-USB
      { 0xF4,	0x01,	0x76,	0x29},	// W#115 - 	WAIT 15250 us; Wait 15.25ms.  Store to Buffer R6. 146
      { 0xB8,	0x0A,	0xFF,	0x9B},	// W#116 - 	
      { 0xA3,	0x00,	0x76,	0x29},	// W#117 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6.  0x146
      { 0x8C,	0x0A,	0xFF,	0x9B},	// W#118 - 	
      { 0x80,	0x0A,	0x40,	0x89},	// W#119 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO1; USB (1.2V)
      { 0x80,	0x0A,	0x4A,	0x89},	// W#120 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO11; USB-Retimer
      { 0x80,	0x0A,	0x47,	0x89},	// W#121 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO08; EMMC/UFS 1.8V
      { 0xF2,	0x5C,	0x09,	0x40},	// W#122 - 	
      { 0x03,	0x20,	0x03,	0x91},	// W#123 - 	skip disabling S3A if not single phase configuration
      { 0x80,	0x0A,	0x1A,	0x89},	// W#124 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S3_CTRL; APC1 (Kryo Silver) 0.87V SHW
      { 0xF8,	0x09,	0xFF,	0x83},	// W#125 - 	
      { 0x04,	0x42,	0xC0,	0x00},	// W#126 - 	GPIO_01A: Revert Pull-config to PD_10uA (irrespective of DIG-State)
      { 0x00,	0x44,	0xC0,	0x00},	// W#127 - 	GPIO_01A: Configure to Drive-Low (strong-PD), when in DIG_OUT Mode
      { 0x01,	0x40,	0xC0,	0x00},	// W#128 - 	GPIO_01A: Configure to DIG_OUT Mode
      { 0x21,	0x00,	0xFF,	0xC1},	// W#129 - 	WAIT 1068 us; Wait 33+2 32kHz clock cycles - for DBU2 to discharge
      { 0x80,	0x0A,	0xC1,	0x88},	// W#130 - 	o SF1 (ALT_SLEEP_CLK)
      { 0x80,	0x0A,	0x54,	0x88},	// W#131 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL  WLAN 38.4MHz
      { 0x80,	0x0A,	0x5A,	0x88},	// W#132 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL  SLEEP_CLK1
      { 0x80,	0x48,	0x52,	0x01},	// W#133 - 	PMICPR-711: Re-enable L19A PD in POFF (since it was disabled in SBL)
      { 0x80,	0x0A,	0x52,	0x89},	// W#134 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO19; WCN3990 CHAIN0 (Wipwr)
      { 0x80,	0x0A,	0x45,	0x89},	// W#135 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO06; WCN RF,GPS, Metis (Wipwr)
      { 0x80,	0x0A,	0x48,	0x89},	// W#136 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO09; WCN_XO (Wipwr)
      { 0x80,	0x48,	0x23,	0x01},	// W#137 - 	PMICPR-1947: Re-enable S6A Strong-PD in POFF (since it was disabled in SBL)
      { 0x80,	0x0A,	0x23,	0x89},	// W#138 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S6_CTRL; MV_LDO SMPS
      { 0x80,	0x0A,	0x0B,	0x88},	// W#139 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL VREF_MSM; 1.25V reference for UIM and SD
      { 0x80,	0x0A,	0x4C,	0x89},	// W#140 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO13; PX3 1.8v
      { 0xF4,	0x01,	0x76,	0x29},	// W#141 - 	WAIT 15250 us; Wait 15.25ms.  Store to Buffer R6. 146
      { 0xB8,	0x0A,	0xFF,	0x9B},	// W#142 - 	
      { 0x80,	0x0A,	0x42,	0x89},	// W#143 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL LDO03A, WTR 1V
      { 0x80,	0x48,	0x1D,	0x01},	// W#144 - 	PMICPR-1947: Re-enable S4A Strong-PD in POFF (since it was disabled in SBL)
      { 0x80,	0x0A,	0x1D,	0x89},	// W#145 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL S4_CTRL; HV sub
      { 0x80,	0x0A,	0x50,	0x88},	// W#146 - 	GOTO DISABLE_WAIT_FUNCTION_LOCAL XO; OK to Skip LDO_XO?
      { 0xA3,	0x00,	0xFF,	0xC1},	// W#147 - 	WAIT 4972 us; Wait 5ms
      { 0x42,	0x90,	0x16,	0x00},	// W#148 - 	1BYTE WRITE Force STAT low ([7:6]=b'01) so that SMB goes to suspend
      { 0x01,	0x45,	0x16,	0x00},	// W#149 - 	eNABLE cHARGER Shutdown CR- 0000171925
      { 0x00,	0x46,	0x40,	0x00},	// W#150 - 	1BYTE WRITE 0x00 TO FG_BATT_SOC__EN_CTL; disable FG algorithm (CR-0000171925).  This bit emulatoes what the SHDN_N HW signal did on earlier PMIs.
      { 0x05,	0x52,	0x45,	0x00},	// W#151 - 	Put RRADC in Shutdown (not shutdown , just pause RRADC)
      { 0x02,	0x66,	0x13,	0x00},	// W#152 - 	1BYTE WRITE Revert USB5/1 setting to 500mA
      { 0x24,	0x68,	0x13,	0x00},	// W#153 - 	Charger WA-31. Revert to UFP mode (same as FTRIM setting) to avoid unexpected PON
      { 0x04,	0xDF,	0x14,	0x00},	// W#154 - 	CR176140: Set chicken bit high to avoid lockup at following PON
      { 0x80,	0x0A,	0x24,	0x88},	// W#155 - 	
      { 0x80,	0x0A,	0x2C,	0x88},	// W#156 - 	
      { 0x46,	0x01,	0xFF,	0xC1},	// W#157 - 	WAIT 10010 us; Wait 10ms for VREGs to discharge completely before allowing the next PON trigger
      { 0x40,	0x91,	0x08,	0x00},	// W#158 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#159 - 	End of Sequence
      { 0x00,	0x46,	0xFF,	0x04},	// W#160 - 	1BYTE WRITE 0x00 TO 0x46; Write 0x00 to 0x46 (enable register in the peripheral)
      { 0x21,	0x00,	0xFF,	0xC1},	// W#161 - 	WAIT 1068 us; Wait 33+2 32kHz clock cycles.  Increased for PM8019 2.0.
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#162 - 	End of subroutine. PC = Stored PC.; Go back to next line of the calling function
      { 0x01,	0x00,	0xFF,	0xC1},	// W#163 - 	Wait 1 CLK Cycle before checking
      { 0xF1,	0x08,	0xC7,	0x40},	// W#164 - 	Read GPIO08 Status (GPIO_VAL) and store to Buffer R1
      { 0x01,	0x10,	0x01,	0x92},	// W#165 - 	We will exit  = '1'
      { 0x01,	0x40,	0xC7,	0x00},	// W#166 - 	Configure GPIO08 to Dig Output
      { 0x00,	0x44,	0xC7,	0x00},	// W#167 - 	Set SLB LOW and continue
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#168 - 	
      { 0x01,	0x00,	0x76,	0x25},	// W#169 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#170 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0x8C,	0x0A,	0xFF,	0x83},	// W#171 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#172 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#173 - 	End of Sequence
      { 0x00,	0x40,	0xC7,	0x00},	// W#174 - 	Configure GPIO08 to Dig INPUT
      { 0x00,	0x42,	0xC7,	0x00},	// W#175 - 	Set SLB to PU
      { 0x01,	0x00,	0xFF,	0xC1},	// W#176 - 	Wait 30us before trying again
      { 0xF1,	0x08,	0xC7,	0x40},	// W#177 - 	Read GPIO08 Status (GPIO_VAL) and store to Buffer R1
      { 0x00,	0x10,	0x01,	0x91},	// W#178 - 	Skip the next line if GPIO08_STATUS1 (bit 0) is high
      { 0x05,	0x42,	0xC7,	0x00},	// W#179 - 	Set SLB to No pull
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#180 - 	return to the next linE of the calling routing
      { 0x01,	0x00,	0x76,	0x25},	// W#181 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#182 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0xC0,	0x0A,	0xFF,	0x83},	// W#183 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#184 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#185 - 	End of Sequence; Return
      { 0x00,	0x57,	0x08,	0x00},	// W#186 - 	PMICPR-475: Disable PMIC_WD only at the beginning of WR (here) & PON
      { 0x6C,	0x0B,	0xFF,	0x9B},	// W#187 - 	GOTO_SUB_NOID ASSERT_EXT_RESET; Set PON_RESET_N and PON_OUT low
      { 0x8E,	0x01,	0xFF,	0xC1},	// W#188 - 	WAIT 12.2 ms = 400 SLEEP_CLK cycles before setting PON_OUT high again (to signal Warm Reset to slave PMICs)
      { 0x40,	0x49,	0x09,	0x10},	// W#189 - 	Send PON_OUT high to all PMICs.
      { 0x80,	0x0B,	0xFF,	0x9B},	// W#190 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0xC6,	0x44,	0x1D,	0x01},	// W#191 - 	Restore this setting, in case warm reset occurred in sleep
      { 0x02,	0x5A,	0x1D,	0x11},	// W#192 - 	Restore this setting, in case warm reset occurred in sleep
      { 0x06,	0x45,	0x1D,	0x01},	// W#193 - 	Change to Auto mode, in case it were in Retention (after Sleep)
      { 0x06,	0x49,	0x1D,	0x01},	// W#194 - 	Change to Auto mode, in case it were FORCE_PFM (after Sleep)
      { 0xF8,	0x40,	0x1D,	0x01},	// W#195 - 	S4A: LB for WL_PON-VSET = 0x07_F8 => 2040 mV
      { 0x07,	0x41,	0x1D,	0x01},	// W#196 - 	S4A: UB for WL_PON-VSET = 0x07_F8 => 2040 mV
      { 0x02,	0x98,	0x08,	0x10},	// W#197 - 	1BYTE WRITE 0x02 TO PON__RESET_CTL; Assert warm_rb
      { 0x03,	0x00,	0xFF,	0xC1},	// W#198 - 	WAIT 91.5 us; Wait 3 SLEEP_CLK cycles for reset changes to propagate through clock domains
      { 0x06,	0x98,	0x08,	0x10},	// W#199 - 	1BYTE WRITE 0x06 TO PON__RESET_CTL; De-assert warm_rb
      { 0x07,	0x45,	0x40,	0x01},	// W#200 - 	: 21
      { 0x07,	0x45,	0x42,	0x01},	// W#201 - 	: 04
      { 0x07,	0x45,	0x47,	0x01},	// W#202 - 	: 18
      { 0x07,	0x45,	0x4A,	0x01},	// W#203 - 	: 19
      { 0x07,	0x45,	0x4C,	0x01},	// W#204 - 	: 10
      { 0x07,	0x45,	0x52,	0x01},	// W#205 - 	: 16
      { 0xFC,	0x5C,	0x09,	0x40},	// W#206 - 	Read GROOT_OPT_ID & Store in RC (Reqd for WARLOCK_REGULATORS_VSET_RECONFIGS from OTP)
      { 0x24,	0x00,	0xFF,	0x9B},	// W#207 - 	Needs RC Argument. Re-write VSETs relevant to Warlock
      { 0x42,	0x90,	0x16,	0x00},	// W#208 - 	1BYTE WRITE Force STAT low ([7:6]=b'01) so that SMB goes to suspend
      { 0xC4,	0x80,	0x13,	0x00},	// W#209 - 	REVERT REGISTER TO ftrim SETTINGS upon WARM RESET
      { 0x00,	0x52,	0x11,	0x00},	// W#210 - 	REVERT REGISTER TO ftrim SETTINGS upon WARM RESET
      { 0x00,	0xC2,	0x14,	0x10},	// W#211 - 	REVERT REGISTER TO ftrim SETTINGS upon WARM RESET
      { 0x04,	0xDF,	0x14,	0x00},	// W#212 - 	CR176140: Set chicken bit high to avoid lockup at following PON
      { 0xAA,	0x04,	0xFF,	0xC1},	// W#213 - 	WAIT 36417 us; Wait 1194 cycles of SLEEP_CLK (36.4 ms) for MSM prep time during warm reset
      { 0x9B,	0x19,	0x76,	0x29},	// W#214 - 	2BYTE R6 = 0x199B; (2+0x199b) X 30.5 uS = 200 mS.  Pass this into enable wait pon as a parameter
      { 0x99,	0x00,	0x7D,	0x28},	// W#215 - 	1BYTE OFFSET = 0x99; Set offset to PON_RESET_N_CTL register (0x99)
      { 0x18,	0x10,	0x08,	0x88},	// W#216 - 	GOTO ROM_ENABLE_WAIT_RELATIVE (ROM) PON; enable wait PON subroutine will deassert PON_RESET_N and wait for PS_HOLD to go high
      { 0x40,	0x91,	0x08,	0x00},	// W#217 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#218 - 	End of Warm-Reset Sequence
      { 0x00,	0x43,	0x08,	0x02},	// W#219 - 	1BYTE WRITE 0x00 TO SID:2 0x0800__0x43; Set SPMI_SLAVE_RB to 0 to tolerate glitches during power down
      { 0x00,	0x99,	0x08,	0x00},	// W#220 - 	1BYTE WRITE 0x00 TO PON__PON_RESET_N_CTL; Assert PON_RESET_N low
      { 0x00,	0x49,	0x09,	0x10},	// W#221 - 	1BYTE WRITE 0x00 TO MISC__PON_OUT_CTL; Send PON_OUT low to all PMIC's.  Changing to SEC ACCESS (to updated in next .csv).
      { 0x01,	0x43,	0x08,	0x02},	// W#222 - 	1BYTE WRITE 0x01 TO SID:2 0x0800__0x43; Set SPMI_SLAVE_RB back to 1
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#223 - 	
      { 0x1C,	0x08,	0x70,	0xA1},	// W#224 - 	
      { 0x1C,	0x00,	0x70,	0x21},	// W#225 - 	2BYTE R0 = R0 + 0x1C ([5_Datasets x 2_Rows/Dataset - 2_SRC - 1] = 7 Rows x 0x04 Addr-Offset per Row)
      { 0x1C,	0x08,	0x72,	0xA1},	// W#226 - 	
      { 0x24,	0x00,	0x72,	0x21},	// W#227 - 	2BYTE R2 = R2 + 0x24 ([5_Datasets x 2_Rows/Dataset - 0_DST - 1] = 9 Rows x 0x04 Addr-Offset per Row)
      { 0x08,	0x00,	0x78,	0x28},	// W#228 - 	1BYTE R8 = 0x08 (Loop 8 times = [5_Datasets x 2_Rows/Dataset - 2_Rows_for_CURR])
      { 0xF0,	0xFF,	0xF4,	0xA3},	// W#229 - 	4BYTE R4 = MEMADDR(R0); copy the last reason registers into a buffer
      { 0xF2,	0xFF,	0xF4,	0xA7},	// W#230 - 	4BYTE MEMADDR(R2) = R4; write the reasons to the location pointed to by the paste pointer
      { 0x01,	0x00,	0x78,	0x24},	// W#231 - 	
      { 0x00,	0x81,	0xFF,	0x92},	// W#232 - 	SKIP 3 Lines if R8 EQUAL 0x00 (MASK=0xFF); Loop Until loop count (R8) = 4
      { 0x04,	0x00,	0x70,	0x25},	// W#233 - 	2BYTE R0 = R0 - 0x04; Move read pointer to next (upper) DWORD
      { 0x04,	0x00,	0x72,	0x25},	// W#234 - 	2BYTE R2 = R2 - 0x04; Move write pointer to next (upper) DWORD
      { 0x94,	0x0B,	0xFF,	0x83},	// W#235 - 	
      { 0xF4,	0xC0,	0x08,	0x40},	// W#236 - 	
      { 0xF5,	0xC2,	0x08,	0x40},	// W#237 - 	
      { 0xF6,	0xC4,	0x08,	0x40},	// W#238 - 	
      { 0xF7,	0xC5,	0x08,	0x40},	// W#239 - 	
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#240 - 	4BYTE MEMADDR(R0) = R4
      { 0x04,	0x00,	0x70,	0x21},	// W#241 - 	2BYTE R0 = R0 + 0x04
      { 0xF4,	0xC7,	0x08,	0x40},	// W#242 - 	
      { 0xF5,	0xC8,	0x08,	0x40},	// W#243 - 	
      { 0xF6,	0xC9,	0x08,	0x40},	// W#244 - 	
      { 0xF7,	0x48,	0x60,	0x40},	// W#245 - 	1BYTE READ RTC_RW__RDATA0 TO R7; Fill the last byte with the RTC value.
      { 0xF8,	0x48,	0x60,	0x40},	// W#246 - 	1BYTE READ RTC_RW__RDATA0 TO R8; Fill the last byte with the RTC value.
      { 0xF8,	0xF7,	0x79,	0x26},	// W#247 - 	1BYTE R9 = R8 - R7
      { 0x00,	0x91,	0xFF,	0x90},	// W#248 - 	SKIP 1 Lines if R9 EQUAL 0x00 (MASK=0xFF); If data didn't match we retry.
      { 0xD4,	0x0B,	0xFF,	0x83},	// W#249 - 	
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#250 - 	4BYTE MEMADDR(R0) = R4; Write data to address stored in R0
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#251 - 	End of subroutine. PC = Stored PC
      { 0x28,	0x0F,	0xFF,	0x9B},	// W#252 - 	Assert CHG Clock Req
      { 0xF1,	0x68,	0x13,	0x40},	// W#253 - 	R1 = 0x1368
      { 0x04,	0x11,	0x04,	0x96},	// W#254 - 	Skip next 5 lines if 0x1368[2]=1
      { 0x04,	0x00,	0xF1,	0x30},	// W#255 - 	Set R1[2]=1 --> SNK_EN_CMD=1
      { 0xF1,	0x68,	0x13,	0x08},	// W#256 - 	0x1368=R1 --> SNK_EN_CMD=1
      { 0xF7,	0x59,	0x13,	0x40},	// W#257 - 	R7=0x1358
      { 0xFD,	0x00,	0xF7,	0x2C},	// W#258 - 	Set R7 bit 1=0
      { 0xF7,	0x59,	0x13,	0x08},	// W#259 - 	Set 0x1359[1]=0 --> Enable Crude Sensor
      { 0x38,	0x0F,	0xFF,	0x9B},	// W#260 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0x42,	0x00,	0xFF,	0xC5},	// W#261 - 	Start 2ms timer
      { 0xFB,	0x00,	0xF1,	0x2C},	// W#262 - 	1BYTE R1 = R1 AND 0xFB ; Set R1[2]=0 --> SNK_EN_CMD=0
      { 0xF1,	0x68,	0x13,	0x08},	// W#263 - 	0x1368=R1 --> SNK_EN_CMD=0
      { 0x38,	0x0F,	0xFF,	0x9B},	// W#264 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#265 - 	
      { 0x42,	0x00,	0xFF,	0xC5},	// W#266 - 	Start 2ms timer
      { 0xF7,	0x50,	0x72,	0x40},	// W#267 - 	1BYTE READ PBS_CLIENT1__SCRATCH1 & store to Buffer R7
      { 0x05,	0x70,	0xFF,	0x91},	// W#268 - 	SKIP 1 Lines if R7 NOT EQUAL 0x04 (MASK=0xFF)
      { 0x10,	0x51,	0x72,	0x00},	// W#269 - 	Using 0x10 = d'16 x 30.5us = 488uSec as Safe-Time before BU
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#270 - 	
      { 0x04,	0x70,	0xFF,	0x90},	// W#271 - 	SKIP 1 Lines if R7 NOT EQUAL 0x04 (MASK=0xFF)
      { 0x60,	0x0C,	0xFF,	0x83},	// W#272 - 	
      { 0x03,	0x70,	0xFF,	0x90},	// W#273 - 	SKIP 1 Lines if R7 NOT EQUAL 0x03 (MASK=0xFF)
      { 0x68,	0x0C,	0xFF,	0x83},	// W#274 - 	
      { 0x02,	0x70,	0xFF,	0x90},	// W#275 - 	SKIP 1 Lines if R7 NOT EQUAL 0x02 (MASK=0xFF)
      { 0x70,	0x0C,	0xFF,	0x83},	// W#276 - 	
      { 0x01,	0x70,	0xFF,	0x90},	// W#277 - 	SKIP 1 Lines if R7 NOT EQUAL 0x01 (MASK=0xFF)
      { 0xB8,	0x0C,	0xFF,	0x83},	// W#278 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#279 - 	
      { 0x04,	0x51,	0x72,	0x00},	// W#280 - 	Set SCRATCH2 to 0x04 (to indicate to SW that SOC wake sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#281 - 	End of Sequence; Byte-2
      { 0x03,	0x51,	0x72,	0x00},	// W#282 - 	Set SCRATCH2 to 0x03 (to indicate to SW that SOC sleep sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#283 - 	End of Sequence; Byte-2
      { 0x80,	0x46,	0x0B,	0x00},	// W#284 - 	1BYTE WRITE 0x80 TO VREF_MSM__EN_CTL1; Enable VREF_MSM
      { 0x18,	0x08,	0x76,	0xA1},	// W#285 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0xA8,	0x0C,	0x1D,	0x89},	// W#286 - 	Restore S4 Primary-Mode to the one before entering sleep
      { 0xA8,	0x0C,	0x23,	0x89},	// W#287 - 	Restore S6 (was S5 on SL) Primary-Mode to the one before entering sleep
      { 0xC6,	0x44,	0x23,	0x11},	// W#288 - 	Enable PFM_IBOOST. In SBL, Value = 0xC6 (Changed from S5A to S6A for Warlock)
      { 0xC5,	0x44,	0x1D,	0x11},	// W#289 - 	Enable PFM_IBOOST
      { 0x02,	0x5A,	0x1D,	0x11},	// W#290 - 	In SBL, value = 0x02
      { 0x06,	0x49,	0x1D,	0x01},	// W#291 - 	Restore S4 Secondary-Mode to AUTO
      { 0x00,	0xDF,	0x14,	0x00},	// W#292 - 	Use 200k=0. Charger clock may lock up at this command CR-0000176241
      { 0x04,	0xDF,	0x14,	0x00},	// W#293 - 	Toggle chicken bit high to unlock charger
      { 0xC0,	0x03,	0xFE,	0xC1},	// W#294 - 	Wait 50us = 960 x 19.2MHz clock-cycles
      { 0x00,	0xDF,	0x14,	0x00},	// W#295 - 	Toggle chicken bit low
      { 0x02,	0x51,	0x72,	0x00},	// W#296 - 	Set SCRATCH2 to 0x02 (to indicate to SW that wake sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#297 - 	End of wake sequence
      { 0xF6,	0xFF,	0x74,	0xA2},	// W#298 - 	Load R2 to R5 with data from location pointed to by R6
      { 0xF4,	0x45,	0xFF,	0x0C},	// W#299 - 	ReStore MODE_CTL1 from R4
      { 0x04,	0x00,	0x76,	0x21},	// W#300 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#301 - 	End of subroutine.
      { 0xF7,	0x40,	0x11,	0x40},	// W#302 - 	Load R7 with the value in 0x1140 => SCHG_OTG_CMD_OTG
      { 0x01,	0x00,	0xF7,	0x2C},	// W#303 - 	Redundant: Mask-out other bits in R7 to only preserve Bit-0 = OTG_EN
      { 0xF8,	0x0B,	0x16,	0x40},	// W#304 - 	Load R8 with the value in 0x160B => SCHG_MISC_POWER_PATH_STATUS
      { 0x01,	0x00,	0xF8,	0x2C},	// W#305 - 	Redundant: Mask-out other bits in R8 to only preserve Bit-0 = VALID_INPUT_POWER_SOURCE
      { 0xF7,	0xF8,	0xF7,	0x32},	// W#306 - 	R7 = R7 || R8 = OTG_EN || VALID_INPUT_POWER_SOURCE
      { 0x01,	0x71,	0x01,	0x90},	// W#307 - 	Skip next config (that sets SCHG Clock to 200kHz) if OTG-Enabled or Valid Charger-Input present
      { 0x02,	0xDF,	0x14,	0x00},	// W#308 - 	USE 200K CLK instead of 19.2Mhz - only if OTG-Disabled or Charger-Input absent
      { 0x18,	0x08,	0x76,	0xA1},	// W#309 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0x18,	0x0D,	0x1D,	0x89},	// W#310 - 	Backup S4 Primary-Mode so that it can be restored during Wake-Up
      { 0x18,	0x0D,	0x23,	0x89},	// W#311 - 	Backup S6 (was S5 on SL) Primary-Mode so that it can be restored during Wake-Up
      { 0x00,	0x5A,	0x1D,	0x11},	// W#312 - 	1BYTE WRITE 0x00 TO S4_CTRL__PS_THRES_ERRAMP; Set PS_VRST to 0mV to prevent mode under/overshoots (CR-175088)
      { 0x06,	0x45,	0x1D,	0x01},	// W#313 - 	PMICPR-1046: Set S4 Primary-Mode to AUTO
      { 0x05,	0x44,	0x1D,	0x11},	// W#314 - 	Disable PFM_IBOOST
      { 0x05,	0x49,	0x1D,	0x01},	// W#315 - 	Set S4 Secondary-Mode to FORCE_PFM
      { 0x04,	0x45,	0x23,	0x01},	// W#316 - 	Set S6 (was S5 on SL) to RM
      { 0x05,	0x44,	0x23,	0x11},	// W#317 - 	Disable PFM_IBOOST
      { 0xF8,	0x43,	0x4E,	0x41},	// W#318 - 	Read LDO15A.VSET_VALID_UB (UIM1_LDO Enable & VSET_UB) & Store in R8
      { 0x07,	0x85,	0xFF,	0x92},	// W#319 - 	Skip Disabling VREF_MSM if UIM1 VALID_UB > 0x07 (i.e On @ ~3V)
      { 0xF8,	0x43,	0x50,	0x41},	// W#320 - 	Else Read LDO17A.VSET_VALID_UB (UIM2_LDO Enable & VSET_UB) & Store in R8
      { 0x07,	0x85,	0xFF,	0x90},	// W#321 - 	Skip Disabling VREF_MSM if UIM2 VALID_UB > 0x07 (i.e On @ ~3V)
      { 0x00,	0x46,	0x0B,	0x00},	// W#322 - 	Disable VREF_MSM if None of the UIM_LDOs are On @3V (Can be either On @1.8V or Off)
      { 0x00,	0x57,	0x08,	0x00},	// W#323 - 	Disable PMIC WD - TODO (check if this needs to be conditionally re-enabled in Wake)
      { 0x01,	0x51,	0x72,	0x00},	// W#324 - 	Set SCRATCH2 to 0x01 (to indicate to SW that sleep sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#325 - 	End of the Sleep entry sequence
      { 0xF4,	0x45,	0xFF,	0x44},	// W#326 - 	Store MODE_CTL1 into R4
      { 0xF6,	0xFF,	0x74,	0xA6},	// W#327 - 	Load data into location pointed to by R6
      { 0x04,	0x00,	0x76,	0x21},	// W#328 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#329 - 	End of subroutine.
      { 0x7F,	0x00,	0xFC,	0x2C},	// W#330 - 	Clear EOSR_IN_QUEUE_STS = MISC_SPARE_5<7> [AND RC with 0x7F = 0'b0111_1111]
      { 0x20,	0xC1,	0x20,	0x90},	// W#331 - 	GOTO(AFP_BATT_TEMP_CHECK_SEQUENCE) if AFP_BATT_TEMP_STS = MISC_SPARE_5<5> is Set [Mask = 0x20 = 0'b0010_0000]
      { 0x7C,	0x0D,	0xFF,	0x83},	// W#332 - 	Else, proceed to check FG_ESR_QM_STS if AFP_BATT_TEMP_STS is Clear
      { 0xF4,	0x53,	0x09,	0x40},	// W#333 - 	R4 <= HOT_THRESHOLD [MISC_SPARE_4]
      { 0xF3,	0x52,	0x09,	0x40},	// W#334 - 	R3 <= COLD_THRESHOLD [MISC_SPARE_3]
      { 0xF1,	0x51,	0x41,	0x40},	// W#335 - 	R1 <= Current BATT_TEMP_MSB
      { 0xF0,	0x50,	0x41,	0x40},	// W#336 - 	R0 <= Current BATT_TEMP_LSB
      { 0xFF,	0xFF,	0x71,	0x38},	// W#337 - 	SHIFT R1 MSB Left 5-times to make Orig <2:0> to New <7:5>
      { 0xFF,	0xFF,	0x71,	0x38},	// W#338 - 	
      { 0xFF,	0xFF,	0x71,	0x38},	// W#339 - 	
      { 0xFF,	0xFF,	0x71,	0x38},	// W#340 - 	
      { 0xFF,	0xFF,	0x71,	0x38},	// W#341 - 	
      { 0xFF,	0xFF,	0x70,	0x3C},	// W#342 - 	SHIFT R0 LSB Right 3-times to make Orig <7:3> to New <4:0>
      { 0xFF,	0xFF,	0x70,	0x3C},	// W#343 - 	
      { 0xFF,	0xFF,	0x70,	0x3C},	// W#344 - 	
      { 0xF0,	0xF1,	0xF2,	0x32},	// W#345 - 	R2 = R1 <7:5> OR R0 <4:0> = REAL_TEMP
      { 0xF2,	0xF3,	0x75,	0x26},	// W#346 - 	R5 (DIFF) = R2 (REAL) - R3 (COLD_TH)
      { 0xFF,	0xF6,	0xFF,	0x91},	// W#347 - 	Carry=1 if DIFF < 0 i.e. REAL < COLD_TH => Open BATFET
      { 0xF2,	0xF4,	0x75,	0x26},	// W#348 - 	R5 (DIFF) = R2 (REAL) - R4 (HOT_TH)
      { 0xFF,	0xF6,	0xFF,	0x90},	// W#349 - 	Carry=1 if DIFF < 0 i.e. REAL < HOT_TH => Bat-Temp is Safe. Skip AFP, Proceed to Next Step.
      { 0x01,	0x41,	0x16,	0x10},	// W#350 - 	REAL not in <HOT_TH:COLD_TH> Range => Execute AFP-Shutdown
      { 0x10,	0xC1,	0x10,	0x90},	// W#351 - 	GOTO(FG_ESR_FAILSAFE_TIMER_EVAL) if FG_ESR_QM_STS = MISC_SPARE_5<4> is Set [Mask = 0x10 = 0'b0001_0000]
      { 0xD4,	0x0E,	0xFF,	0x83},	// W#352 - 	Else (FG Not in ESR_QUIET_MODE). Finish-Up using CLIENT2_COMMON_END_RESCHED_EVAL_SEQUENCE
      { 0xF0,	0x55,	0x09,	0x40},	// W#353 - 	Load the Loop counter into R0
      { 0x00,	0x01,	0xFF,	0x92},	// W#354 - 	If Timer is expired - enable ESR_PD and Set Interrupt
      { 0x01,	0x00,	0x70,	0x24},	// W#355 - 	Else Decrement Loop counter
      { 0xF0,	0x55,	0x09,	0x08},	// W#356 - 	Store updated Loop counter back to MISC_SPARE_6
      { 0xD4,	0x0E,	0xFF,	0x83},	// W#357 - 	Reschedule using CLIENT2_COMMON_END_RESCHED_EVAL_SEQUENCE
      { 0x02,	0x15,	0x73,	0x00},	// W#358 - 	Enable PBS_Client2 SEQ_ENDED Interrupt - will fire after this seq exits
      { 0xF4,	0x50,	0x73,	0x40},	// W#359 - 	Read the Scratch1 register to point to the requested sequence
      { 0x02,	0x00,	0xF4,	0x30},	// W#360 - 	R4 is loaded with Flags Bit<1>=1 --> FG_ESR_SEQ [OR R4 with Mask = 0x02 = 0'b0000_0010]
      { 0xEF,	0x00,	0xF4,	0x2C},	// W#361 - 	R4 is loaded with Flags Bit<4>=0 --> EXIT_QUIET [AND R4 with Mask = 0xEF = 0'b1110_1111]
      { 0xAC,	0x0E,	0xFF,	0x83},	// W#362 - 	Goto FG_ESR_QUIET_MODE_EXIT with appropraite Flags configured for Forced FG_ESR_QUIET_MODE_EXIT
      { 0xF8,	0x08,	0xC9,	0x40},	// W#363 - 	Readback GPIO10 (WCSS_VCTRL) status and store to Buffer R8
      { 0x01,	0x81,	0x01,	0x90},	// W#364 - 	If GPIO09 is high
      { 0xD4,	0x0D,	0xFF,	0x83},	// W#365 - 	GPI009 is low
      { 0xF9,	0x4E,	0x09,	0x40},	// W#366 - 	Read MISC register for L5 active voltage and store LSB to Buffer R9
      { 0xFA,	0x4F,	0x09,	0x40},	// W#367 - 	Read MISC register for L5 active voltage and store MSB to Buffer RA
      { 0xF9,	0x40,	0x44,	0x09},	// W#368 - 	Write Buffer R9 value to L5A VSET (LSB) (note: PDM error incorrectly shows 2BYTE)
      { 0xFA,	0x41,	0x44,	0x09},	// W#369 - 	Write Buffer RA value to L5A VSET (MSB) (note: PDM error incorrectly shows 2BYTE)
      { 0x07,	0x45,	0x44,	0x01},	// W#370 - 	Set LDO05 mode to NPM [GDG] There is no HW_EN to follow for this. And no PBS Vote for secondary mode
      { 0x01,	0x5A,	0x09,	0x00},	// W#371 - 	Indicate that the sequence set regulators to active state
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#372 - 	
      { 0xF9,	0x4C,	0x09,	0x40},	// W#373 - 	Read MISC register for L5 retention voltage and store LSB to Buffer R9
      { 0xFA,	0x4D,	0x09,	0x40},	// W#374 - 	Read MISC register for L5 retention voltage and store MSB to Buffer RA
      { 0xF9,	0x40,	0x44,	0x09},	// W#375 - 	Write Buffer R9 value to L5A VSET (LSB)
      { 0xFA,	0x41,	0x44,	0x09},	// W#376 - 	Write Buffer RA value to L5A VSET (MSB)
      { 0x04,	0x45,	0x44,	0x01},	// W#377 - 	Set LDO05 mode to RET [GDG] There is no HW_EN to follow for this. And no PBS Vote for secondary mode
      { 0x00,	0x5A,	0x09,	0x00},	// W#378 - 	Indicate that the sequence set regulators to retention state
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#379 - 	
      { 0xF8,	0x08,	0xC4,	0x40},	// W#380 - 	Readback GPIO05 (WLAN_RF_VCTRL) status and store to Buffer R8
      { 0x01,	0x81,	0x01,	0x90},	// W#381 - 	SKIP If GPIO3 is high
      { 0x08,	0x0E,	0xFF,	0x83},	// W#382 - 	GPI005 is low
      { 0x40,	0x4F,	0x23,	0x01},	// W#383 - 	Set PBS_EN and PBS_MODE to use Secondary Mode [GDG] set MODE Secondary to Auto in MODE_CTL2
      { 0x01,	0x5B,	0x09,	0x00},	// W#384 - 	set state to 0x1 to indicate that active state was executed
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#385 - 	
      { 0x00,	0x4F,	0x23,	0x01},	// W#386 - 	Clear PBS_EN and PBS_MODE and go to primary mode
      { 0x00,	0x5B,	0x09,	0x00},	// W#387 - 	Set state to 0x0 to indicate that retention state was executed
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#388 - 	
      { 0xF2,	0x18,	0x1C,	0x40},	// W#389 - 	Readback BUA interrupts and store status to Buffer R2
      { 0x01,	0x20,	0x01,	0x91},	// W#390 - 	If not BATT_ALARM_INT latched
      { 0x01,	0xDB,	0x4E,	0x11},	// W#391 - 	1BYTE WRITE 0x01 TO LDO015__PERPH_RESET_CTL4; SW Reset for LDO15
      { 0x01,	0xDB,	0x50,	0x11},	// W#392 - 	1BYTE WRITE 0x01 TO LDO17__PERPH_RESET_CTL4; SW Reset for LDO17
      { 0x02,	0x20,	0x02,	0x90},	// W#393 - 	If not UICC1_INT latched
      { 0x01,	0xDB,	0x4E,	0x11},	// W#394 - 	SW Reset for LDO15A (UIM1)
      { 0x04,	0x20,	0x04,	0x90},	// W#395 - 	If not UICC2_INT latched
      { 0x01,	0xDB,	0x50,	0x11},	// W#396 - 	SW Reset for LDO17 (UIM2)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#397 - 	
      { 0xF4,	0x50,	0x73,	0x40},	// W#398 - 	Read the Scratch1 register to point to the requested sequence
      { 0x01,	0x40,	0x01,	0x91},	// W#399 - 	Skip if Bit 0 not set (S6 voltage check)
      { 0xF4,	0x51,	0x73,	0x08},	// W#400 - 	Set Scratch2 to value in Scratch1 to show SW that request was read
      { 0x58,	0x0E,	0xFF,	0x83},	// W#401 - 	
      { 0x02,	0x40,	0x02,	0x90},	// W#402 - 	Skip if Bit 1 not set (FG_ESR_QUIET_MODE_CTRL_SEQUENCE)
      { 0xA4,	0x0E,	0xFF,	0x83},	// W#403 - 	
      { 0xFF,	0x51,	0x73,	0x00},	// W#404 - 	Only Bits <3:0> supported for CMDs. Set Scratch2 to 0xFF to indicate an error if none of those were set.
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#405 - 	
      { 0xF8,	0x42,	0x20,	0x41},	// W#406 - 	
      { 0xF9,	0x43,	0x20,	0x41},	// W#407 - 	
      { 0xEE,	0x02,	0x7A,	0x29},	// W#408 - 	Load R11:R10 with 0x2ee (750mV)
      { 0xF8,	0xFA,	0x7A,	0x27},	// W#409 - 	
      { 0x00,	0xA6,	0x00,	0x90},	// W#410 - 	If the voltage setting is less than 700mV
      { 0x7C,	0x0E,	0xFF,	0x83},	// W#411 - 	
      { 0xC5,	0x44,	0x20,	0x01},	// W#412 - 	S6_CTRL_PFM_CTL = 0xC5
      { 0x84,	0x4B,	0x21,	0x11},	// W#413 - 	Set per Orlando recommendations
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#414 - 	
      { 0x08,	0x02,	0x7A,	0x29},	// W#415 - 	Load R11:R10 with 0x208 (520mV)
      { 0xF8,	0xFA,	0x7A,	0x27},	// W#416 - 	
      { 0x00,	0xA6,	0x00,	0x90},	// W#417 - 	If the voltage setting is less than 700mV
      { 0x98,	0x0E,	0xFF,	0x83},	// W#418 - 	
      { 0xC6,	0x44,	0x20,	0x01},	// W#419 - 	S5_CTRL_PFM_CTL = 0xC6
      { 0x83,	0x4B,	0x21,	0x11},	// W#420 - 	Set per Orlando recommendations
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#421 - 	
      { 0xC7,	0x44,	0x20,	0x01},	// W#422 - 	S5_CTRL_PFM_CTL = 0xC7
      { 0x83,	0x4B,	0x21,	0x11},	// W#423 - 	Set per Orlando recommendations
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#424 - 	
      { 0x10,	0x40,	0x10,	0x90},	// W#425 - 	Skip over to FG_ESR_QUIET_MODE_EXIT if Bit-4 of R4 was Clear
      { 0xBC,	0x0E,	0xFF,	0x83},	// W#426 - 	
      { 0xEF,	0x00,	0xFC,	0x2C},	// W#427 - 	Clear FG_ESR_QM_STS = MISC_SPARE_5<4> [AND RC with 0xEF = 0'b1110_1111]
      { 0x02,	0x00,	0xFB,	0x30},	// W#428 - 	Set ESR_PD_MODE to CRG_DSC PD --> ESR_PULSES_ALLOWED
      { 0x00,	0x00,	0x70,	0x28},	// W#429 - 	Update FG_ESR_QM_FAILSAFE_TIMER Value to Min 0x00 = 0 secs (Redundant)
      { 0xC8,	0x0E,	0xFF,	0x83},	// W#430 - 	
      { 0x10,	0x00,	0xFC,	0x30},	// W#431 - 	Set FG_ESR_QM_STS = MISC_SPARE_5<4> [OR RC with 0x10 = 0'b0001_0000]
      { 0x0C,	0x00,	0xFB,	0x2C},	// W#432 - 	Set ESR_PD_MODE to NO_PULL --> ESR_PULSES_STOPPED
      { 0xFF,	0x00,	0x70,	0x28},	// W#433 - 	Update FG_ESR_QM_FAILSAFE_TIMER Value to Max 0xFF = 255 (=> Min 8.5 minutes)
      { 0xFB,	0x69,	0x41,	0x08},	// W#434 - 	Configure ESR_PD_CFG as per updated value
      { 0xF0,	0x55,	0x09,	0x08},	// W#435 - 	Load FG_ESR_QM_FAILSAFE_TIMER as per updated value into MISC_SPARE_6
      { 0xF4,	0x51,	0x73,	0x08},	// W#436 - 	Set PBS_CLIENT2.SCRATCH2 to ACK to SW the Return-Value of the Calling-Sequence
      { 0x00,	0xC5,	0x30,	0x90},	// W#437 - 	Check MISC_SPARE_5 Bits <5> & <4> (Mask = 0x30 = 0'b0011_0000) for EOSR Requirements on AFP or FG
      { 0xEC,	0x0E,	0xFF,	0x83},	// W#438 - 	GOTO End of this Check, if None of AFP & FG need a Rescheduled Trigger
      { 0x80,	0xC1,	0x80,	0x92},	// W#439 - 	Check EOSR_IN_QUEUE_STS = MISC_SPARE_5<7> (Mask = 0x80 = 0'b1000_0000) if any EOSR was initiated before current seq execution
      { 0x80,	0x00,	0xFC,	0x30},	// W#440 - 	EOSR was not initiated before. Set EOSR_IN_QUEUE_STS = MISC_SPARE_5<7> here (Mask 0x80 = 0'b1000_0000)
      { 0xFC,	0x54,	0x09,	0x08},	// W#441 - 	Set EOSR_IN_QUEUE_STS Flag to inform any future CLIENT2_SW_TRIGGER Seq to not Reset/Disturb EOSR-Timer (unless EOSR_OVERRIDE_STS is Set)
      { 0xFF,	0xFF,	0xFF,	0xC5},	// W#442 - 	Fire EOSR-Timer to expire after 2-secs & then --> Trigger CLIENT2_EOSR_TIMER_EXPIRED_SEQUENCE
      { 0xFC,	0x54,	0x09,	0x08},	// W#443 - 	Update the Final Value of RC (modified in certain flows) to MISC_SPARE_5
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#444 - 	End this Sequence here. EOSR-Trigger may or may-not happen later based on current EOSR_IN_QUEUE_STS
      { 0x28,	0x0F,	0xFF,	0x9B},	// W#445 - 	Assert CHG Clock Req
      { 0xF0,	0x0E,	0x13,	0x40},	// W#446 - 	R0 = Read [7] UFP_DFP_MODE
      { 0x00,	0x01,	0x80,	0x94},	// W#447 - 	Skip next 3 lines if [7]=0 i.e. UFP mode
      { 0xF7,	0x59,	0x13,	0x40},	// W#448 - 	R7=0x1358
      { 0x02,	0x00,	0xF7,	0x30},	// W#449 - 	Set R7 bit1=1
      { 0xF7,	0x59,	0x13,	0x08},	// W#450 - 	UFP_DFT_MODE=1 --> in DFP mode --> Disable crude sensor 0x1359[1]=1
      { 0x38,	0x0F,	0xFF,	0x9B},	// W#451 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#452 - 	
      { 0xF1,	0x68,	0x13,	0x40},	// W#453 - 	Check if SCHG is in DRP-Mode
      { 0x00,	0x10,	0x06,	0x90},	// W#454 - 	Skip next 1 line if [2:1] != 0 i.e. EOS if SCHG not in DRP-Mode
      { 0x02,	0x42,	0x72,	0x00},	// W#455 - 	Trigger PBS Client1, Trigger 1(NC)
      { 0x38,	0x0F,	0xFF,	0x9B},	// W#456 - 	Revert PBS CHG Clock Req (SW Req is kept)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#457 - 	
      { 0xF0,	0xDF,	0x14,	0x40},	// W#458 - 	
      { 0x01,	0x00,	0xF0,	0x30},	// W#459 - 	R0 = R0 || 0x01 i.e. Set Bit-0 to High
      { 0xF0,	0xDF,	0x14,	0x08},	// W#460 - 	Writeback SCHG_DC.CLOCK_CONTROL with updated R0 (with Bit-0 set High)
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#461 - 	
      { 0xF0,	0xDF,	0x14,	0x40},	// W#462 - 	
      { 0x08,	0x01,	0x08,	0x91},	// W#463 - 	Bit-3 = 1 => APPS is awake, leave Bit-0 High
      { 0xFE,	0x00,	0xF0,	0x2C},	// W#464 - 	Bit-3 = 0 => APPS is asleep, clear CLK_REQ. R0 = R0 && 0xFE
      { 0xF0,	0xDF,	0x14,	0x08},	// W#465 - 	Write back SCHG_DC.CLOCK_CONTROL with updated R0 (with Bit-0 cleared low)
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#466 - 	
      { 0xF4,	0x50,	0x74,	0x40},	// W#467 - 	Read the Argument Register to point to the requested sequence
      { 0x01,	0x40,	0x01,	0x90},	// W#468 - 	Skip if Bit 0 not set (AFP BATT_TEMP CTRL)
      { 0x60,	0x0F,	0xFF,	0x83},	// W#469 - 	
      { 0xFF,	0x51,	0x74,	0x00},	// W#470 - 	Only Bit <0> supported for CMDs. Set Scratch2 to 0xFF to indicate an error if none of those were set.
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#471 - 	
      { 0xFC,	0x54,	0x09,	0x40},	// W#472 - 	PBSREG.RC will store MISC_SPARE_5
      { 0x10,	0x40,	0x10,	0x91},	// W#473 - 	Skip over to AFP_BATT_TEMP_CHECK_DISABLE if Bit-4 of R4 was Clear
      { 0x20,	0x00,	0xFC,	0x30},	// W#474 - 	Set AFP_BATT_TEMP_CHK_STS = MISC_SPARE_5<5> [OR RC with 0x20 = 0'b0010_0000]
      { 0x74,	0x0F,	0xFF,	0x83},	// W#475 - 	End with AFP_BATT_TEMP_CTRL_DONE
      { 0xDF,	0x00,	0xFC,	0x2C},	// W#476 - 	Clear AFP_BATT_TEMP_CHK_STS = MISC_SPARE_5<5> [AND RC with 0xDF = 0'b1101_1111]
      { 0xFC,	0x54,	0x09,	0x08},	// W#477 - 	Update MISC_SPARE_5 with updated AFP_BATT_TEMP_CHK_STS
      { 0xF4,	0x51,	0x74,	0x08},	// W#478 - 	Set PBS_CLIENT3.SCRATCH2 to value in PBS_CLIENT3.SCRATCH1 to show SW that request was executed
      { 0x20,	0x42,	0x73,	0x00},	// W#479 - 	Initiate PBS_CLIENT2.TRIG5 [CLIENT2_BIT5_CHECK]. Round about way, since Client3 has NO EOSR
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#480 - 	
      { 0x44,	0x44,	0x44,	0x44},	// W#481 - 	; S4: B3: Reserved
      { 0x55,	0x55,	0x55,	0x55},	// W#482 - 	; S5: B3: Reserved
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#483 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#484 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#485 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#486 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#487 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#488 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#489 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#490 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#491 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#492 - 	| B3:RTC_LSB      | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#493 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#494 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#495 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#496 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#497 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#498 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#499 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#500 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#501 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#502 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#503 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#504 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#505 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#506 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#507 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#508 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#509 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#510 - 	Inserted by compiler.
      { 0x19,	0x14,	0xFF,	0xF8},	// W#511 - 	Inserted by compiler.
   },
   // "PM670L_RAM_b0x09_v0x0C"
   {
      //data  offset  base_addr  sid
      { 0x52,	0x53,	0x42,	0x50},	// W#0 - 	0x600 RAM signature PBSR""
      { 0x09,	0x0C,	0x01,	0x18},	// W#1 - 	0x604 Header offset, Header Version, PBS RAM Revision, PBS RAM Branch
      { 0x34,	0x06,	0xFF,	0x83},	// W#2 - 	0x608 start of trigger jump table:
      { 0x70,	0x06,	0xFF,	0x83},	// W#3 - 	0x60C Client-1 Trigger
      { 0xD8,	0x06,	0xFF,	0x83},	// W#4 - 	0x610 Client-2 Trigger
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#5 - 	0x614 Empty (Drax does not have Client-3)
      { 0xE4,	0x0B,	0xFF,	0x83},	// W#6 - 	0x618 Fixed Offset = RAM-Base-Addr + 0x18 + 0x00 => SLEEP-SET
      { 0xF8,	0x0B,	0xFF,	0x83},	// W#7 - 	0x61C Fixed Offset = RAM-Base-Addr + 0x18 + 0x04 => (PON X Reasons
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#8 - 	0x620 Fixed Offset = RAM-Base-Addr + 0x18 + 0x08 => (NO RTRR)
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#9 - 	0x624 Fixed Offset = RAM-Base-Addr + 0x18 + 0x0C => (No HLOS) / Spare
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#10 - 	0x628 Fixed Offset = RAM-Base-Addr + 0x18 + 0x10 => (Spare registers) / LPG
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#11 - 	0x62C Fixed Offset = RAM-Base-Addr + 0x18 + 0x14 => (LPG Dataset)
      { 0x24,	0x0C,	0xFF,	0x83},	// W#12 - 	0x630 Fixed Offset = RAM-Base-Addr + 0x18 + 0x18 => (USER-Space: AMOLED Panel-Specific Implementation)
      { 0xFF,	0x48,	0x71,	0x00},	// W#13 - 	Clear all latched sequences in PBS PON Client 0
      { 0xFB,	0x08,	0x08,	0x40},	// W#14 - 	READ PON__FSM_STATUS TO RB; Store PON_FSM_STATUS in RB for Older OTP-Revs < v0x05
      { 0xFC,	0x08,	0x08,	0x40},	// W#15 - 	READ PON__FSM_STATUS TO RC; Store PON_FSM_STATUS in RC for Newer OTP-Revs >= v0x05
      { 0x04,	0xC0,	0xFF,	0x90},	// W#16 - 	Skip if NOT DVDD_CONFIG (State 4)
      { 0x14,	0x00,	0xFF,	0x83},	// W#17 - 	GOTO OTP_DVDD_CONFIG (OTP); Is there a scenario where this is ever used?
      { 0x01,	0xC0,	0xFF,	0x90},	// W#18 - 	Skip if NOT PON (State 1)
      { 0x14,	0x00,	0xFF,	0x83},	// W#19 - 	GOTO OTP_DVDD_CONFIG (OTP); Is there a scenario where this is ever used?
      { 0x06,	0xC0,	0xFF,	0x90},	// W#20 - 	Skip if NOT FAULT (State 6)
      { 0x4C,	0x07,	0xFF,	0x83},	// W#21 - 	
      { 0x02,	0xC0,	0xFF,	0x90},	// W#22 - 	Skip if NOT POFF (State 2)
      { 0x2C,	0x08,	0xFF,	0x83},	// W#23 - 	
      { 0x07,	0xC0,	0xFF,	0x90},	// W#24 - 	Skip if NOT WARM RESET (State 7)
      { 0x70,	0x07,	0xFF,	0x83},	// W#25 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#26 - 	Undefined State - NACK PON (Note:  None of the Client GOTOs have a 'RETURN'; the ACK is done in each subroutine)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#27 - 	End of Sequence; Byte-2
      { 0xF3,	0x0A,	0x72,	0x40},	// W#28 - 	Read Client 1 trigger latch status and store to Buffer R3
      { 0x01,	0x30,	0x01,	0x91},	// W#29 - 	Skip if NOT Bit 0 (DTEST MUX)
      { 0x01,	0x48,	0x72,	0x00},	// W#30 - 	Clear Latched Trigger - Bit 0
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#31 - 	CL1_TRIG0:EMPTY => Replace with GOTO once defined.
      { 0x02,	0x30,	0x02,	0x91},	// W#32 - 	Skip if NOT Bit 1 (BoB)
      { 0x02,	0x48,	0x72,	0x00},	// W#33 - 	Clear Latched Trigger - Bit 1
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#34 - 	CL1_TRIG1:EMPTY => Replace with GOTO once defined.
      { 0x04,	0x30,	0x04,	0x91},	// W#35 - 	Skip if NOT Bit 2 (BA_N)
      { 0x04,	0x48,	0x72,	0x00},	// W#36 - 	Clear Latched Trigger - Bit 2
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#37 - 	CL1_TRIG2:EMPTY => Replace with GOTO once defined.
      { 0x08,	0x30,	0x08,	0x91},	// W#38 - 	Skip if NOT Bit 3 (LAB_IBB_SCP)
      { 0x08,	0x48,	0x72,	0x00},	// W#39 - 	Clear Latched Trigger - Bit 3
      { 0x88,	0x09,	0xFF,	0x83},	// W#40 - 	CL1_TRIG3: LAB_IBB_SCP
      { 0x10,	0x30,	0x10,	0x91},	// W#41 - 	Skip if NOT Bit 4 (EOSR_Timer)
      { 0x10,	0x48,	0x72,	0x00},	// W#42 - 	Clear Latched Trigger - Bit 4
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#43 - 	CL1_TRIG4:EMPTY => Replace with GOTO once defined.
      { 0x20,	0x30,	0x20,	0x91},	// W#44 - 	Skip if NOT Bit 5 (BCL)
      { 0x20,	0x48,	0x72,	0x00},	// W#45 - 	Clear Latched Trigger - Bit 5
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#46 - 	CL1_TRIG5:EMPTY => Replace with GOTO once defined.
      { 0x40,	0x30,	0x40,	0x91},	// W#47 - 	Skip if NOT Bit 6 (OTS2)
      { 0x40,	0x48,	0x72,	0x00},	// W#48 - 	Clear Latched Trigger - Bit 6
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#49 - 	CL1_TRIG6:EMPTY => Replace with GOTO once defined.
      { 0x80,	0x30,	0x80,	0x91},	// W#50 - 	Bit 7 = SW Trigger (System Sleep/Wake)
      { 0x80,	0x48,	0x72,	0x00},	// W#51 - 	Clear Latched Trigger - Bit 7
      { 0xA0,	0x09,	0xFF,	0x83},	// W#52 - 	CL1_TRIG7: CLIENT1_SYSTEM_SLEEP_WAKE_SEQUENCE
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#53 - 	
      { 0xF4,	0x0A,	0x73,	0x40},	// W#54 - 	Read Client 2 trigger latch status and store to Buffer R4
      { 0x01,	0x40,	0x01,	0x91},	// W#55 - 	Skip if not Bit 0 (WCSS pin control, Was EUD from Groot)
      { 0x01,	0x48,	0x73,	0x00},	// W#56 - 	Clear Latched Trigger - Bit 0
      { 0xD0,	0x0A,	0xFF,	0x83},	// W#57 - 	CL2_TRIG0: WCSS_CTRL_PIN_STATE_CHECK
      { 0x02,	0x40,	0x02,	0x91},	// W#58 - 	Skip if not Bit 1 (uSD switch detect, GPIO08)
      { 0x02,	0x48,	0x73,	0x00},	// W#59 - 	Clear Latched Trigger - Bit 1
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#60 - 	CL2_TRIG1:EMPTY => Replace with GOTO once defined.
      { 0x04,	0x40,	0x04,	0x91},	// W#61 - 	Skip if not Bit 2 (SWIRE decode from AOMLED)
      { 0x04,	0x48,	0x73,	0x00},	// W#62 - 	Clear Latched Trigger - Client2 Bit 2
      { 0x28,	0x0C,	0xFF,	0x83},	// W#63 - 	CL2_TRIG2: USER_SPACE_POINTER1
      { 0x08,	0x40,	0x08,	0x91},	// W#64 - 	Skip if not Bit 3 (SSC_PWR_EN from MSM)
      { 0x08,	0x48,	0x73,	0x00},	// W#65 - 	Clear Latched Trigger - Bit 3
      { 0x0C,	0x0B,	0xFF,	0x83},	// W#66 - 	CL2_TRIG3: MULTICARDTRAY_UICC_PROTECTION_SEQ
      { 0x10,	0x40,	0x10,	0x91},	// W#67 - 	Skip if not Bit 4 (SLB, GPIO10, Trigger from Groot)
      { 0x10,	0x48,	0x73,	0x00},	// W#68 - 	Clear Latched Trigger - Bit 4
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#69 - 	CL2_TRIG4:EMPTY => Replace with GOTO once defined.
      { 0x20,	0x40,	0x20,	0x91},	// W#70 - 	Skip if not Bit 5 (EOSR_Timer)
      { 0x20,	0x48,	0x73,	0x00},	// W#71 - 	Clear Latched Trigger - Bit 5
      { 0x68,	0x0B,	0xFF,	0x83},	// W#72 - 	CL2_TRIG5: LAB_PSRR_EOSR_SEQUENCE
      { 0x40,	0x40,	0x40,	0x94},	// W#73 - 	Skip if not Bit 6 (LAB_BANDING_DETECTION)
      { 0xF1,	0x40,	0x73,	0x40},	// W#74 - 	Read RE trigger config for RMW
      { 0xBF,	0x00,	0xF1,	0x2C},	// W#75 - 	clear bit 6 to disable RE trigger
      { 0xF1,	0x40,	0x73,	0x08},	// W#76 - 	RE disabled for client2 bit 6
      { 0x40,	0x48,	0x73,	0x00},	// W#77 - 	Clear PBS_CLIENT2 Bit6 Trig-Latch after disabling RE on corresponding Bit6 above
      { 0x54,	0x0B,	0xFF,	0x83},	// W#78 - 	CL2_TRIG6: LAB_BANDING_DETECTION
      { 0x80,	0x40,	0x80,	0x91},	// W#79 - 	Skip if not Bit 7 (SW Trigger)
      { 0x80,	0x48,	0x73,	0x00},	// W#80 - 	1BYTE WRITE 0x80 TO PBS_CLIENT3__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 7
      { 0xD0,	0x0B,	0xFF,	0x83},	// W#81 - 	CL2_TRIG7: CLIENT2_SW_TRIGGER
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#82 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#83 - 	
      { 0x28,	0x09,	0xFF,	0x9B},	// W#84 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x88,	0x48,	0x14,	0x01},	// W#85 - 	1BYTE WRITE 0x88 TO S1_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x17,	0x01},	// W#86 - 	1BYTE WRITE 0x88 TO S2_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x1A,	0x01},	// W#87 - 	1BYTE WRITE 0x88 TO S3_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x20,	0x01},	// W#88 - 	1BYTE WRITE 0x88 TO S5_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0xEA,	0x01,	0xFF,	0xC1},	// W#89 - 	WAIT 15016 us; Wait 15ms for VREGs to discharge completely before allowing the next PON trigger
      { 0x40,	0x91,	0x08,	0x00},	// W#90 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#91 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#92 - 	
      { 0x28,	0x09,	0xFF,	0x9B},	// W#93 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x08,	0x08,	0x41,	0x89},	// W#94 - 	
      { 0x08,	0x08,	0x44,	0x89},	// W#95 - 	
      { 0x02,	0x00,	0x70,	0x28},	// W#96 - 	1BYTE R0 = 0x02; R0 = 0x2
      { 0x0C,	0x09,	0xFF,	0x9B},	// W#97 - 	GOTO INIT_RESET PON. Do reset stuff common to warm reset and POFF
      { 0x07,	0x45,	0x40,	0x01},	// W#98 - 	: 23
      { 0x07,	0x45,	0x41,	0x01},	// W#99 - 	: 27
      { 0x07,	0x45,	0x43,	0x01},	// W#100 - 	: 26
      { 0x07,	0x45,	0x44,	0x01},	// W#101 - 	: 28
      { 0x07,	0x45,	0x46,	0x01},	// W#102 - 	: 25
      { 0x78,	0x40,	0x14,	0x01},	// W#103 - 	S01B: LB for WL_PON-VSET = 0x03_78 => 0888 mV
      { 0x03,	0x41,	0x14,	0x01},	// W#104 - 	S01B: UB for WL_PON-VSET = 0x03_78 => 0888 mV
      { 0x40,	0x40,	0x1A,	0x01},	// W#105 - 	S03B_S04B: LB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x03,	0x41,	0x1A,	0x01},	// W#106 - 	S03B_S04B: UB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x58,	0x40,	0x20,	0x01},	// W#107 - 	S05B: LB for WL_PON-VSET = 0x02_58 => 0600 mV
      { 0x02,	0x41,	0x20,	0x01},	// W#108 - 	S05B: UB for WL_PON-VSET = 0x02_58 => 0600 mV
      { 0x70,	0x40,	0x40,	0x01},	// W#109 - 	L01B: LB for WL_PON-VSET = 0x03_70 => 0888 mV
      { 0x03,	0x41,	0x40,	0x01},	// W#110 - 	L01B: UB for WL_PON-VSET = 0x03_70 => 0888 mV
      { 0x90,	0x40,	0x41,	0x01},	// W#111 - 	L02B: LB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x0B,	0x41,	0x41,	0x01},	// W#112 - 	L02B: UB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x90,	0x40,	0x43,	0x01},	// W#113 - 	L04B: LB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x0B,	0x41,	0x43,	0x01},	// W#114 - 	L04B: UB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x90,	0x40,	0x44,	0x01},	// W#115 - 	L05B: LB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x0B,	0x41,	0x44,	0x01},	// W#116 - 	L05B: UB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x10,	0x40,	0x46,	0x01},	// W#117 - 	L07B: LB for WL_PON-VSET = 0x0C_10 => 3088 mV
      { 0x0C,	0x41,	0x46,	0x01},	// W#118 - 	L07B: UB for WL_PON-VSET = 0x0C_10 => 3088 mV
      { 0x38,	0x40,	0x48,	0x01},	// W#119 - 	L09B: LB for WL_PON-VSET = 0x03_38 => 0824 mV
      { 0x03,	0x41,	0x48,	0x01},	// W#120 - 	L09B: UB for WL_PON-VSET = 0x03_38 => 0824 mV
      { 0x70,	0x40,	0x49,	0x01},	// W#121 - 	L10B: LB for WL_PON-VSET = 0x03_70 => 0880 mV
      { 0x03,	0x41,	0x49,	0x01},	// W#122 - 	L10B: UB for WL_PON-VSET = 0x03_70 => 0880 mV
      { 0xE0,	0x40,	0xA0,	0x01},	// W#123 - 	1BYTE WRITE 0xE0 TO BOB_CONFIG__VOUT_LB; Set VBOB to 3.296V since BOB ignores warm_rb
      { 0x0C,	0x41,	0xA0,	0x01},	// W#124 - 	1BYTE WRITE 0x0C TO BOB_CONFIG__VOUT_UB; Set VBOB to 3.296V since BOB ignores warm_rb
      { 0x02,	0x45,	0xA0,	0x01},	// W#125 - 	1BYTE WRITE 0x02 TO BOB_CONFIG__VOUT_MODE_CTRL; Set BOB to auto mode since BOB ignores warm_rb
      { 0x00,	0x36,	0xA0,	0x01},	// W#126 - 	1BYTE WRITE 0x00 TO BOB_CONFIG__EXT_CTRL_FOLLOW; Ignore EXT_CTRL1 & 2. Explicit command since BOB ignores warm_rb
      { 0x18,	0x08,	0xFF,	0x9B},	// W#127 - 	GOTO_SUB_NOID WR_MSM_BOOT_VOLTAGE_SS_NOM; Set CX/MX/APC to SS Nominal (new values
      { 0x40,	0x91,	0x08,	0x00},	// W#128 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#129 - 	End of Sequence; Byte-2
      { 0xD2,	0x66,	0xFF,	0x04},	// W#130 - 	Assert OCP_CTL1.Bit1 <OCP_STATUS_CLR> = 1
      { 0x03,	0x00,	0xFF,	0xC1},	// W#131 - 	WAIT 91 us = 3 cycles of 32k clock
      { 0xD0,	0x66,	0xFF,	0x04},	// W#132 - 	De-Assert OCP_CTL1.Bit1 <OCP_STATUS_CLR> = 0
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#133 - 	
      { 0x40,	0x40,	0x1A,	0x01},	// W#134 - 	1BYTE WRITE 0x40 TO S1_CTRL__VSET_LB; Bump S3+S4 (CX) to 0.832V
      { 0x03,	0x41,	0x1A,	0x01},	// W#135 - 	1BYTE WRITE 0x03 TO S1_CTRL__VSET_UB; Bump S3+S4 (CX) to 0.832V
      { 0x78,	0x40,	0x14,	0x01},	// W#136 - 	1BYTE WRITE 0x78 TO S5_CTRL__VSET_LB; Bump S1 (MX) to 0.888V (was S5 on SL, moved to S1 on Warlock)
      { 0x03,	0x41,	0x14,	0x01},	// W#137 - 	1BYTE WRITE 0x03 TO S5_CTRL__VSET_UB; Bump S1 (MX) to 0.888V (was S5 on SL, moved to S1 on Warlock)
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#138 - 	End of subroutine. PC = Stored PC.; Byte-2
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#139 - 	
      { 0x28,	0x09,	0xFF,	0x9B},	// W#140 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x04,	0x00,	0x70,	0x28},	// W#141 - 	1BYTE R0 = 0x04; R0=0x4. This value will be applied to the PON_RESET_CTL register in PON (bit 2 warm_rb
      { 0x0C,	0x09,	0xFF,	0x9B},	// W#142 - 	GOTO INIT_RESET PON. Do reset stuff common to warm reset and POFF
      { 0x00,	0x40,	0xC9,	0x00},	// W#143 - 	Configure GPIO10 to Dig INPUT
      { 0x04,	0x42,	0xC9,	0x00},	// W#144 - 	Set SLB to 10uA pull-down
      { 0x0C,	0x0E,	0x44,	0x89},	// W#145 - 	LDO5B, SD/MMC Card
      { 0x0C,	0x0E,	0x41,	0x89},	// W#146 - 	LDO2B, VDDPX_2 (SD_MMC) - PMICPR-644
      { 0x0C,	0x0E,	0x43,	0x89},	// W#147 - 	LDO4B, UFS (2.96V)
      { 0x0C,	0x0E,	0x46,	0x89},	// W#148 - 	LDO7B, USB (EUD)
      { 0xA3,	0x00,	0x76,	0x29},	// W#149 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6. 146
      { 0xDC,	0x08,	0xFF,	0x9B},	// W#150 - 	
      { 0x46,	0x01,	0x76,	0x29},	// W#151 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6. 146
      { 0xB0,	0x08,	0xFF,	0x9B},	// W#152 - 	
      { 0x0C,	0x0E,	0x40,	0x89},	// W#153 - 	LDO1B, UFS_PHY
      { 0x0C,	0x0E,	0x20,	0x89},	// W#154 - 	LP-DDR4X VDDQ
      { 0xA3,	0x00,	0x76,	0x29},	// W#155 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6. 146
      { 0xDC,	0x08,	0xFF,	0x9B},	// W#156 - 	
      { 0xF4,	0x01,	0x76,	0x29},	// W#157 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6.  0x146
      { 0xB0,	0x08,	0xFF,	0x9B},	// W#158 - 	
      { 0x0C,	0x0E,	0x48,	0x89},	// W#159 - 	LDO9, LPI_Cx
      { 0x0C,	0x0E,	0x1A,	0x89},	// W#160 - 	CX+GPU+cDSP
      { 0x0C,	0x0E,	0x49,	0x89},	// W#161 - 	LDO10, LPI Mx
      { 0x0C,	0x0E,	0x14,	0x89},	// W#162 - 	S1B, LPDDR4
      { 0x04,	0x42,	0xCA,	0x00},	// W#163 - 	GPIO_11B: Revert Pull-config to PD_10uA (irrespective of DIG-State)
      { 0x00,	0x44,	0xCA,	0x00},	// W#164 - 	GPIO_11B: Configure to Drive-Low (strong-PD), when in DIG_OUT Mode
      { 0x01,	0x40,	0xCA,	0x00},	// W#165 - 	GPIO_11B: Configure to DIG_OUT Mode
      { 0x21,	0x00,	0xFF,	0xC1},	// W#166 - 	WAIT 1068 us; Wait 33+2 32kHz clock cycles - for DBU1 to discharge
      { 0x0C,	0x0E,	0xA0,	0x89},	// W#167 - 	Disable BOB
      { 0x00,	0x46,	0x05,	0x00},	// W#168 - 	1BYTE WRITE 0x00 TO INT__EN_CTL1; Disable INT
      { 0x46,	0x01,	0xFF,	0xC1},	// W#169 - 	WAIT 10010 us; Wait 10ms for VREGs to discharge completely before allowing the next PON trigger
      { 0x40,	0x91,	0x08,	0x00},	// W#170 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#171 - 	
      { 0x01,	0x00,	0xFF,	0xC1},	// W#172 - 	wait 1 CLK CYCLE BEFORE CHECKING
      { 0xF1,	0x08,	0xC9,	0x40},	// W#173 - 	Read GPIO10 Status (GPIO_VAL) and store to Buffer R1
      { 0x01,	0x10,	0x01,	0x92},	// W#174 - 	We will exit  = '1'
      { 0x01,	0x40,	0xC9,	0x00},	// W#175 - 	Configure GPIO10 to Dig Output
      { 0x00,	0x44,	0xC9,	0x00},	// W#176 - 	Set SLB LOW and continue
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#177 - 	
      { 0x01,	0x00,	0x76,	0x25},	// W#178 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#179 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0xB0,	0x08,	0xFF,	0x83},	// W#180 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#181 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#182 - 	End of Sequence; Return
      { 0x00,	0x40,	0xC9,	0x00},	// W#183 - 	Configure GPIO10 to Dig INPUT
      { 0x00,	0x42,	0xC9,	0x00},	// W#184 - 	Set SLB to PU
      { 0x01,	0x00,	0xFF,	0xC1},	// W#185 - 	wait 30us before trying again
      { 0xF1,	0x08,	0xC9,	0x40},	// W#186 - 	Read GPIO10 Status (GPIO_VAL) and store to Buffer R1
      { 0x00,	0x10,	0x01,	0x91},	// W#187 - 	Skip the next line if GPIO10_STATUS1 (bit 0) is high
      { 0x05,	0x42,	0xC9,	0x00},	// W#188 - 	Set SLB to No pull
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#189 - 	return to the next linE of the calling routing
      { 0x01,	0x00,	0x76,	0x25},	// W#190 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#191 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0xE4,	0x08,	0xFF,	0x83},	// W#192 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#193 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#194 - 	End of Sequence; Return
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#195 - 	NOP; //This Subroutine does several things that are common to the Warm Reset and POFF seqeunces.  1) disable SPMI to avoid lockup
      { 0x00,	0x43,	0x08,	0x02},	// W#196 - 	1BYTE WRITE 0x00 TO SID:2 0x0800__0x43; Set SPMI_SLAVE_RB to 0 to tolerate glitches during power down
      { 0xF0,	0x98,	0x08,	0x18},	// W#197 - 	1BYTE WRITE R0 TO SID:0 0x0800__0x98; Assert whatever resets are defined in R0
      { 0x03,	0x00,	0xFF,	0xC1},	// W#198 - 	WAIT 91.5 us; Wait 3 SLEEP_CLK cycles for reset changes to propagate through clock domains
      { 0x06,	0x98,	0x08,	0x10},	// W#199 - 	1BYTE WRITE 0x06 TO SID:0 0x0800__0x98; Deassert Resets
      { 0x01,	0x43,	0x08,	0x02},	// W#200 - 	Set SPMI_SLAVE_RB back to 1
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#201 - 	
      { 0x1C,	0x06,	0x70,	0xA1},	// W#202 - 	
      { 0x1C,	0x00,	0x70,	0x21},	// W#203 - 	2BYTE R0 = R0 + 0x1C ([5_Datasets x 2_Rows/Dataset - 2_SRC - 1] = 7 Rows x 0x04 Addr-Offset per Row)
      { 0x1C,	0x06,	0x72,	0xA1},	// W#204 - 	
      { 0x24,	0x00,	0x72,	0x21},	// W#205 - 	2BYTE R2 = R2 + 0x24 ([5_Datasets x 2_Rows/Dataset - 0_DST - 1] = 9 Rows x 0x04 Addr-Offset per Row)
      { 0x08,	0x00,	0x78,	0x28},	// W#206 - 	1BYTE R8 = 0x08 (Loop 8 times = [5_Datasets x 2_Rows/Dataset - 2_Rows_for_CURR])
      { 0xF0,	0xFF,	0xF4,	0xA3},	// W#207 - 	4BYTE R4 = MEMADDR(R0); copy the last reason registers into a buffer
      { 0xF2,	0xFF,	0xF4,	0xA7},	// W#208 - 	4BYTE MEMADDR(R2) = R4; write the reasons to the location pointed to by the paste pointer
      { 0x01,	0x00,	0x78,	0x24},	// W#209 - 	
      { 0x00,	0x81,	0xFF,	0x92},	// W#210 - 	SKIP 3 Lines if R8 EQUAL 0x00 (MASK=0xFF); Loop Until loop count (R8) = 4
      { 0x04,	0x00,	0x70,	0x25},	// W#211 - 	2BYTE R0 = R0 - 0x04; Move read pointer to next (upper) DWORD
      { 0x04,	0x00,	0x72,	0x25},	// W#212 - 	2BYTE R2 = R2 - 0x04; Move write pointer to next (upper) DWORD
      { 0x3C,	0x09,	0xFF,	0x83},	// W#213 - 	
      { 0xF4,	0xC0,	0x08,	0x40},	// W#214 - 	
      { 0xF5,	0xC2,	0x08,	0x40},	// W#215 - 	
      { 0xF6,	0xC4,	0x08,	0x40},	// W#216 - 	
      { 0xF7,	0xC5,	0x08,	0x40},	// W#217 - 	
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#218 - 	4BYTE MEMADDR(R0) = R4
      { 0x04,	0x00,	0x70,	0x21},	// W#219 - 	2BYTE R0 = R0 + 0x04
      { 0xF4,	0xC7,	0x08,	0x40},	// W#220 - 	
      { 0xF5,	0xC8,	0x08,	0x40},	// W#221 - 	
      { 0xF6,	0xC9,	0x08,	0x40},	// W#222 - 	
      { 0xFF,	0x00,	0x77,	0x28},	// W#223 - 	1BYTE R7 = 0xFF
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#224 - 	4BYTE MEMADDR(R0) = R4; Write data to address stored in R0
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#225 - 	End of subroutine. PC = Stored PC.; Byte-2
      { 0xF0,	0x58,	0xDC,	0x11},	// W#226 - 	PMICPR-1051: Minimize delay b/w IBB & LAB discharge
      { 0x00,	0x46,	0xDC,	0x01},	// W#227 - 	Disable IBB
      { 0x00,	0x46,	0xDE,	0x01},	// W#228 - 	Disable LAB
      { 0x00,	0x47,	0xE0,	0x01},	// W#229 - 	Disable OLEDB pin control
      { 0x00,	0x46,	0xE0,	0x01},	// W#230 - 	Disable OLEDB module
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#231 - 	
      { 0xF7,	0x50,	0x72,	0x40},	// W#232 - 	1BYTE READ PBS_CLIENT1__SCRATCH1 TO R7; Readback PBS_CLIENT1_SCRATCH1 and store to Buffer R7
      { 0x04,	0x70,	0xFF,	0x90},	// W#233 - 	SKIP 1 Lines if R7 NOT EQUAL 0x04 (MASK=0xFF)
      { 0xC8,	0x09,	0xFF,	0x83},	// W#234 - 	
      { 0x03,	0x70,	0xFF,	0x90},	// W#235 - 	SKIP 1 Lines if R7 NOT EQUAL 0x03 (MASK=0xFF)
      { 0xD0,	0x09,	0xFF,	0x83},	// W#236 - 	
      { 0x02,	0x70,	0xFF,	0x90},	// W#237 - 	SKIP 1 Lines if R7 NOT EQUAL 0x02 (MASK=0xFF)
      { 0xDC,	0x09,	0xFF,	0x83},	// W#238 - 	
      { 0x01,	0x70,	0xFF,	0x90},	// W#239 - 	SKIP 1 Lines if R7 NOT EQUAL 0x01 (MASK=0xFF)
      { 0x50,	0x0A,	0xFF,	0x83},	// W#240 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#241 - 	
      { 0x04,	0x51,	0x72,	0x00},	// W#242 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#243 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#244 - 	
      { 0x03,	0x51,	0x72,	0x00},	// W#245 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#246 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#247 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#248 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#249 - 	
      { 0x02,	0x45,	0xA0,	0x01},	// W#250 - 	1BYTE WRITE 0x02 TO BOB_CONFIG__VOUT_MODE_CTRL; Put BOB in Auto Mode
      { 0x01,	0x4A,	0x59,	0x10},	// W#251 - 	1BYTE WRITE 0x01 TO CLK_DIST__PMIC_SLEEP_CTL; Set SLEEP_B to follow HW
      { 0x90,	0xA6,	0x14,	0x11},	// W#252 - 	Re-enable S1 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x14,	0x11},	// W#253 - 	Re-enable S1 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x14,	0x11},	// W#254 - 	Enable S1 VDIP comparator; keep 15mV threshold
      { 0x90,	0xA6,	0x17,	0x11},	// W#255 - 	Re-enable S2 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x17,	0x11},	// W#256 - 	Re-enable S2 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x17,	0x11},	// W#257 - 	Enable S2 VDIP comparator; keep 15mV threshold
      { 0x90,	0xA6,	0x1A,	0x11},	// W#258 - 	Re-enable S3 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x1A,	0x11},	// W#259 - 	Re-enable S3 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x1A,	0x11},	// W#260 - 	Enable S3 VDIP comparator; keep 15mV threshold
      { 0x90,	0xA6,	0x20,	0x11},	// W#261 - 	Re-enable S5 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x20,	0x11},	// W#262 - 	Re-enable S5 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x20,	0x11},	// W#263 - 	Enable S5 VDIP comparator; keep 15mV threshold
      { 0x18,	0x06,	0x76,	0xA1},	// W#264 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0x40,	0x0A,	0x14,	0x89},	// W#265 - 	
      { 0x40,	0x0A,	0x17,	0x89},	// W#266 - 	
      { 0x40,	0x0A,	0x1A,	0x89},	// W#267 - 	
      { 0x40,	0x0A,	0x20,	0x89},	// W#268 - 	
      { 0x87,	0x00,	0xFE,	0xC1},	// W#269 - 	WAIT 7083 ns; Wait 7us (part of CR-175088)
      { 0x02,	0x51,	0x72,	0x00},	// W#270 - 	Set SCRATCH2 to 0x02 (to indicate to SW that wake sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#271 - 	
      { 0xF6,	0xFF,	0x74,	0xA2},	// W#272 - 	Load R2 to R5 with data from location pointed to by R6
      { 0xF4,	0x45,	0xFF,	0x0C},	// W#273 - 	ReStore MODE_CTL1 from R4
      { 0x04,	0x00,	0x76,	0x21},	// W#274 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#275 - 	End of subroutine.
      { 0x18,	0x06,	0x76,	0xA1},	// W#276 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0xC0,	0x0A,	0x14,	0x89},	// W#277 - 	
      { 0xC0,	0x0A,	0x17,	0x89},	// W#278 - 	
      { 0xC0,	0x0A,	0x1A,	0x89},	// W#279 - 	
      { 0xC0,	0x0A,	0x20,	0x89},	// W#280 - 	
      { 0x01,	0x71,	0x14,	0x11},	// W#281 - 	Disable S1 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x14,	0x11},	// W#282 - 	Disable S1 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x14,	0x11},	// W#283 - 	Disable S1 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x14,	0x01},	// W#284 - 	Set S1 to Retention Mode
      { 0x01,	0x71,	0x17,	0x11},	// W#285 - 	Disable S2 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x17,	0x11},	// W#286 - 	Disable S2 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x17,	0x11},	// W#287 - 	Disable S2 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x17,	0x01},	// W#288 - 	Set S2 to Retention Mode
      { 0x01,	0x71,	0x1A,	0x11},	// W#289 - 	Disable S3 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x1A,	0x11},	// W#290 - 	Disable S3 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x1A,	0x11},	// W#291 - 	Disable S3 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x1A,	0x01},	// W#292 - 	Set S3 to Retention Mode
      { 0x01,	0x71,	0x20,	0x11},	// W#293 - 	Disable S5 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x20,	0x11},	// W#294 - 	Disable S5 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x20,	0x11},	// W#295 - 	Disable S5 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x20,	0x01},	// W#296 - 	Set S5 to Retention Mode
      { 0x00,	0x4A,	0x59,	0x10},	// W#297 - 	Force SLEEP_B low
      { 0xF6,	0x8F,	0x09,	0x40},	// W#298 - 	Load R6 with the value in MISC.VSET6_UB [Bit-0 = BoB Sleep-Instruction]
      { 0x00,	0x61,	0x01,	0x90},	// W#299 - 	If Bit-0 = 0 SKIP putting BoB to Bypass (needs to be done for Groot 1.0)
      { 0x00,	0x45,	0xA0,	0x01},	// W#300 - 	Put BOB in Bypass
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#301 - 	
      { 0x01,	0x51,	0x72,	0x00},	// W#302 - 	Set SCRATCH2 to 0x01 (to indicate to SW that sleep sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#303 - 	
      { 0xF4,	0x45,	0xFF,	0x44},	// W#304 - 	Store MODE_CTL1 into R4
      { 0xF6,	0xFF,	0x74,	0xA6},	// W#305 - 	Load data into location pointed to by R6
      { 0x04,	0x00,	0x76,	0x21},	// W#306 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#307 - 	End of subroutine.
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#308 - 	WCSS Debug Placeholder-0
      { 0xF8,	0x08,	0xC8,	0x40},	// W#309 - 	Read GPIO09B Pin STATUS & Load into R8
      { 0x01,	0x81,	0x01,	0x90},	// W#310 - 	If GPIO9B PIN = High => Set-WCSS-to-Active Trigger was detected
      { 0xF0,	0x0A,	0xFF,	0x83},	// W#311 - 	Else If GPIO9 PIN = Low => Set-WCSS-to-Retention Trigger was detected
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#312 - 	WCSS Debug Placeholder-1
      { 0x01,	0x94,	0x09,	0x10},	// W#313 - 	Set WCSS_STATE register with updated value (Bit-0 = 1) => WCSS_ACTIVE
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#314 - 	WCSS Debug Placeholder-2
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#315 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#316 - 	Do nothing. (Buffer to apply separate Tags to appropriate TAGs to this & next line)
      { 0xF8,	0x93,	0x09,	0x40},	// W#317 - 	Read present value of MISC.SSC_STATE & Load into R8
      { 0x01,	0x81,	0x01,	0x90},	// W#318 - 	Check SSC_STATE<0>. If = High => proceed w/o touching S2B.PBS_VOTE
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#319 - 	
      { 0x00,	0x94,	0x09,	0x10},	// W#320 - 	Clear WCSS_STATE register with updated value (Bit-0 = 0) => WCSS_RETENTION
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#321 - 	WCSS Debug Placeholder-4
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#322 - 	
      { 0xF2,	0x1A,	0xC1,	0x40},	// W#323 - 	Read GPIO02.INT_MID_SEL for SD-Card to UICC PairingInfo
      { 0x00,	0x20,	0x03,	0x90},	// W#324 - 	
      { 0x50,	0x0B,	0xFF,	0x83},	// W#325 - 	End the sequence if SD-card is NOT paired with any UICC-Card in the tray
      { 0xF5,	0x11,	0xC1,	0x40},	// W#326 - 	Read GPIO02.INT_SET_TYPE into R5 for Expected-state of SD_CARD_PRESENCE
      { 0xF6,	0x08,	0xC1,	0x40},	// W#327 - 	Read GPIO02.STATUS1 into R6 for current-state of GPIO (High/Low)
      { 0xF5,	0xF6,	0x77,	0x22},	// W#328 - 	Update R7 = R6 + R5, Ignore Carry
      { 0x01,	0x71,	0x01,	0x90},	// W#329 - 	IF   R7<0> == 1'b1 ==> ACTUAL_STATE != SD_PRESENCE_STATE --> Proceed to Debounce
      { 0x50,	0x0B,	0xFF,	0x83},	// W#330 - 	ELSE R7<0> == 1'b0 ==> ACTUAL_STATE == SD_PRESENCE_STATE --> End Sequence
      { 0xFA,	0x8C,	0x09,	0x40},	// W#331 - 	Read MISC.VSET5_LB into RA for Num_Cycles_19M parameter_LSB
      { 0xFB,	0x8D,	0x09,	0x40},	// W#332 - 	Read MISC.VSET5_UB into RB for Num_Cycles_19M parameter_MSB
      { 0xFA,	0xFF,	0xFE,	0xC3},	// W#333 - 	Apply a Debounce of the same Num_Cycles_19M from RB:RA
      { 0x08,	0x48,	0x73,	0x00},	// W#334 - 	Clear Latched Trigger - Bit 3 (Again, in-case it got set again during above debounce-time)
      { 0xF6,	0x08,	0xC1,	0x40},	// W#335 - 	Read GPIO02.STATUS1 into R6 for updated-state of GPIO (High/Low)
      { 0xF5,	0xF6,	0x77,	0x22},	// W#336 - 	Update R7 = R6 + R5, Ignore Carry
      { 0x00,	0x71,	0x01,	0x91},	// W#337 - 	IF   R7<0> == 1'b0 ==> ACTUAL_STATE == SD_PRESENCE_STATE --> End Sequence
      { 0x00,	0x46,	0x41,	0x01},	// W#338 - 	Disable LDO02B
      { 0x00,	0x46,	0x44,	0x01},	// W#339 - 	Disable LDO05B
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#340 - 	
      { 0x01,	0x7D,	0xDE,	0x01},	// W#341 - 	PMICPR-636: Increase LDO Headroom to 200mV
      { 0x60,	0x61,	0xDE,	0x01},	// W#342 - 	PMICPR-636: PWM_CTRL_3 = 8CLK
      { 0x80,	0x01,	0xFE,	0xC1},	// W#343 - 	PMICPR-636: Wait 20us (0x180 = 384 x 19.2M pulses = 20us)
      { 0x78,	0x64,	0xDE,	0x01},	// W#344 - 	Variant-1: Enable Clamp, Full-Curr, 1.8V [PMICPR-1031: Mainlining as per 2017-07-10 update]
      { 0x00,	0x80,	0xFF,	0xC5},	// W#345 - 	Start timer for 1 second
      { 0x0E,	0xE5,	0xDE,	0x11},	// W#346 - 	Set DTEST4 to high_psrr_req output
      { 0xF4,	0x09,	0x09,	0x40},	// W#347 - 	read all DTESTs into R4. Bit 3 is DTEST4
      { 0x00,	0xE5,	0xDE,	0x11},	// W#348 - 	free DTEST4
      { 0x00,	0x41,	0x08,	0x90},	// W#349 - 	if bit 3 (DTEST4) is low continue with cool down
      { 0x54,	0x0B,	0xFF,	0x83},	// W#350 - 	otherwise start the whole process over
      { 0xF1,	0x40,	0x73,	0x40},	// W#351 - 	Read RE trigger config to determine what triggered this sequence
      { 0x40,	0x11,	0x40,	0x92},	// W#352 - 	if bit 6 trigger is set then this sequence was triggered after cool down so if is safe to end PSRR mitigation
      { 0x40,	0x00,	0xF1,	0x30},	// W#353 - 	bit 6 was clear. Set it to enable RE trigger
      { 0xF1,	0x40,	0x73,	0x08},	// W#354 - 	RE enabled for client2 bit 6
      { 0x99,	0x19,	0xFF,	0xC5},	// W#355 - 	if bit 6 trigger was cleared then this sequence was triggered by the Detection sequence and cool down needs to run.
      { 0x00,	0x7D,	0xDE,	0x01},	// W#356 - 	PMICPR-636: Decrease LDO Headroom to 100mV
      { 0x80,	0x01,	0xFE,	0xC1},	// W#357 - 	PMICPR-636: Wait 20us (0x180 = 384 x 19.2M pulses = 20us)
      { 0x74,	0x64,	0xDE,	0x01},	// W#358 - 	Enable Clamp, Full-Curr, 1.6V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#359 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x5C,	0x64,	0xDE,	0x01},	// W#360 - 	Enable Clamp, Half-Curr, 1.4V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#361 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x58,	0x64,	0xDE,	0x01},	// W#362 - 	Enable Clamp, Half-Curr, 1.2V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#363 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x54,	0x64,	0xDE,	0x01},	// W#364 - 	Enable Clamp, Half-Curr, 1.0V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#365 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x50,	0x64,	0xDE,	0x01},	// W#366 - 	Enable Clamp, Half-Curr, 0.8V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#367 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x40,	0x64,	0xDE,	0x01},	// W#368 - 	Disable Clamp
      { 0x80,	0x07,	0xFE,	0xC1},	// W#369 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x20,	0x61,	0xDE,	0x01},	// W#370 - 	PMICPR-636: PWM_CTRL_3 = 2CLK
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#371 - 	
      { 0xF4,	0x50,	0x73,	0x40},	// W#372 - 	Read the PBS_CLIENT2.SCRATCH1 register written by SW
      { 0x01,	0x40,	0x01,	0x90},	// W#373 - 	Skip if Bit 0 not set
      { 0x2C,	0x0C,	0xFF,	0x83},	// W#374 - 	Scratch1 Bit-0 was Set, jump to OLEDB_SW_TRIG_CHANGE_PD_STATE
      { 0xFF,	0x51,	0x73,	0x00},	// W#375 - 	Only bit 0 is assigned. If here then there is an error. Set Scratch2 to 0xFF to indicate an error
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#376 - 	
      { 0x11,	0x11,	0x11,	0x11},	// W#377 - 	; S1, B2,B1 = voltage (not used), B0 = mode
      { 0x22,	0x22,	0x22,	0x22},	// W#378 - 	; S2, B2,B1 = voltage (not used), B0 = mode
      { 0x33,	0x33,	0x33,	0x33},	// W#379 - 	; S3  B2,B1 = voltage (not used), B0 = mode
      { 0x55,	0x55,	0x55,	0x55},	// W#380 - 	; S5 voltage, mode
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#381 - 	Buffer Separation (redundant)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#382 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#383 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#384 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#385 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#386 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#387 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#388 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#389 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#390 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#391 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#392 - 	Buffer Separation (redundant)
      { 0xED,	0x01,	0xA3,	0xFF},	// W#393 - 	Random Unique RAW DWord (tried to spell 'FF_AMOLED' in Hex)
      { 0xE0,	0x0C,	0xFF,	0x83},	// W#394 - 	NOP() or SWIRE_DECODE_SEQUENCE_LGE()
      { 0xB4,	0x0C,	0xFF,	0x83},	// W#395 - 	NOP()
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#396 - 	NOP()
      { 0x28,	0x41,	0xDC,	0x01},	// W#397 - 	Set IBB Voltage to -4.0V
      { 0x08,	0x49,	0xE0,	0x01},	// W#398 - 	Set OLEDB Voltage to 5.8V
      { 0x00,	0x41,	0xDE,	0x01},	// W#399 - 	Set LAB Voltage to 4.6V
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#400 - 	
      { 0xF6,	0x47,	0xDC,	0x41},	// W#401 - 	Load R6 with IBB.PD_CTL
      { 0x3F,	0x00,	0xF6,	0x2C},	// W#402 - 	AND R6 with 0x3F (b'0000_0011) => Clear Bit-7 (<6:3> absent)
      { 0xF6,	0x47,	0xDC,	0x09},	// W#403 - 	IBB.PD_CTL: ENABLE_PD = 0
      { 0xF6,	0xE9,	0xE0,	0x41},	// W#404 - 	PMICPR-714: Read existing value of OLEDB.TEST6 into R6
      { 0x80,	0x00,	0xF6,	0x30},	// W#405 - 	PMICPR-714: OR R6 with 0x80 (b'1000_0000) => Set Bit-7
      { 0xF6,	0xE9,	0xE0,	0x19},	// W#406 - 	PMICPR-714: Copy updated R6 value back to OLEDB.TEST6
      { 0xF6,	0x78,	0xDE,	0x41},	// W#407 - 	Load R6 with LAB.LDO_PD_CTL [PMICPR-971]
      { 0x7F,	0x00,	0xF6,	0x2C},	// W#408 - 	AND R6 with 0x7F (b'0111_1111) => Clear Bit-7
      { 0xF6,	0x78,	0xDE,	0x09},	// W#409 - 	LAB.LDO_PD_CTL: Disable PD [PMICPR-971]
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#410 - 	
      { 0xF6,	0x47,	0xDC,	0x41},	// W#411 - 	Load R6 with IBB.PD_CTL
      { 0x80,	0x00,	0xF6,	0x30},	// W#412 - 	OR R6 with 0x80 (b'1000_0000) => Set Bit-7
      { 0xF6,	0x47,	0xDC,	0x09},	// W#413 - 	IBB.PD_CTL: ENABLE_PD = 1
      { 0xF6,	0x4B,	0xE0,	0x41},	// W#414 - 	Load R6 with OLEDB.PD_CTL
      { 0x01,	0x00,	0xF6,	0x30},	// W#415 - 	OR R6 with 0x01 (b'0000_0001) => Set Bit-0
      { 0xF6,	0x4B,	0xE0,	0x09},	// W#416 - 	OLEDB.PD_CTL: Set PD_EN
      { 0xF6,	0x78,	0xDE,	0x41},	// W#417 - 	Load R6 with LAB.LDO_PD_CTL [PMICPR-971]
      { 0x80,	0x00,	0xF6,	0x30},	// W#418 - 	OR R6 with 0x80 (b'1000_0000) => Set Bit-7
      { 0xF6,	0x78,	0xDE,	0x09},	// W#419 - 	LAB.LDO_PD_CTL: Enable PD [PMICPR-971]
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#420 - 	
      { 0xF6,	0x5D,	0xDC,	0x41},	// W#421 - 	Load R6 with IBB.OUTPUT_SLEW_CTL
      { 0x40,	0x00,	0xF6,	0x30},	// W#422 - 	OR R6 with 0x40 (b'0100_0000) => Set Bit-6
      { 0xF6,	0x5D,	0xDC,	0x09},	// W#423 - 	Set IBB Slew Rate = FAST
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#424 - 	
      { 0xF6,	0x5D,	0xDC,	0x41},	// W#425 - 	Load R6 with IBB.OUTPUT_SLEW_CTL
      { 0xBF,	0x00,	0xF6,	0x2C},	// W#426 - 	AND R6 with 0xBF (b'1011_1111) => Clear Bit-6
      { 0xF6,	0x5D,	0xDC,	0x09},	// W#427 - 	Set IBB Slew Rate = SLOW
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#428 - 	
      { 0x24,	0x5A,	0xE0,	0x01},	// W#429 - 	1. 0xE05A = 0x24 // disable fast precharge
      { 0x1A,	0xE7,	0xE0,	0x11},	// W#430 - 	3. 0xE0E7 = 0x1A // disable slow precharge
      { 0x80,	0x46,	0xE0,	0x01},	// W#431 - 	4. 0xE046 > 0x80 // enable OLEDB module
      { 0x00,	0x4B,	0xE0,	0x01},	// W#432 - 	5. 0xE04B > 0x00 // disable the pulldown
      { 0x00,	0x46,	0xE0,	0x01},	// W#433 - 	6. 0xE046 > 0x00 // disable OLEDB module
      { 0xA4,	0x5A,	0xE0,	0x01},	// W#434 - 	7. 0xE05A = 0xA4 // enable fast precharge
      { 0x9A,	0xE7,	0xE0,	0x11},	// W#435 - 	9. 0xE0E7 = 0x9A // enable slow precharge
      { 0xF4,	0x51,	0x73,	0x40},	// W#436 - 	Read PBS_CLIENT2.SCRATCH2 & Store Value in PBSREG.R4
      { 0x01,	0x00,	0xF4,	0x30},	// W#437 - 	Set Bit-0 to High (leaving other bits intact)
      { 0xF4,	0x51,	0x73,	0x08},	// W#438 - 	Copy PBSREG.R4 to in PBS_CLIENT2.SCRATCH2 effectively just setting Bit-0
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#439 - 	
      { 0x01,	0x0B,	0xDC,	0x01},	// W#440 - 	Write any value into Status4 to tranfer edge count into Status4 register
      { 0xF4,	0x0B,	0xDC,	0x41},	// W#441 - 	Read SWIRE command data into R4. Could be up to d'57 for SS [Mak]
      { 0x00,	0x40,	0xFF,	0x90},	// W#442 - 	Jump to COUNT_LE_41_CHECK if SWIRE_EDGE_COUNT > 0
      { 0x34,	0x0C,	0xFF,	0x83},	// W#443 - 	
      { 0x29,	0x45,	0xFF,	0x93},	// W#444 - 	Jump to COUNT_LE_49_CHECK if SWIRE_EDGE_COUNT > 41
      { 0x37,	0x00,	0x76,	0x28},	// W#445 - 	Load R6 with Constant 0x37 (d'55)
      { 0xF6,	0xF4,	0x74,	0x26},	// W#446 - 	R4 = R6 (d'55) - R4 (SWIRE_EDGE_COUNT {1:41})
      { 0xF4,	0x41,	0xDC,	0x09},	// W#447 - 	Set IBB Voltage
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#448 - 	
      { 0x31,	0x45,	0xFF,	0x96},	// W#449 - 	Jump to COUNT_EQ_50_CHECK if SWIRE_EDGE_COUNT > 49
      { 0x2A,	0x00,	0x74,	0x24},	// W#450 - 	R4 = R4 (SWIRE_EDGE_COUNT {42:49}) - 0x2A (d'42)
      { 0xF4,	0xF4,	0x76,	0x22},	// W#451 - 	R6 = R4 + R4 = 2 x R4
      { 0xF4,	0xF6,	0x76,	0x22},	// W#452 - 	R6 = R4 + R6 = 3 x R4
      { 0x1D,	0x00,	0x74,	0x28},	// W#453 - 	Load R4 with Constant 0x1D (d'29 = d'79 - d'50)
      { 0xF4,	0xF6,	0x74,	0x26},	// W#454 - 	R4 = R4 (d'29) - R6
      { 0xF4,	0x49,	0xE0,	0x09},	// W#455 - 	Set OLEDB Voltage
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#456 - 	
      { 0x32,	0x45,	0xFF,	0x90},	// W#457 - 	Jump to COUNT_EQ_51_CHECK if SWIRE_EDGE_COUNT > 50
      { 0x6C,	0x0C,	0xFF,	0x83},	// W#458 - 	
      { 0x33,	0x45,	0xFF,	0x90},	// W#459 - 	Jump to COUNT_EQ_52_CHECK if SWIRE_EDGE_COUNT > 51
      { 0x44,	0x0C,	0xFF,	0x83},	// W#460 - 	
      { 0x34,	0x45,	0xFF,	0x90},	// W#461 - 	Jump to COUNT_EQ_53_CHECK if SWIRE_EDGE_COUNT > 52
      { 0x94,	0x0C,	0xFF,	0x83},	// W#462 - 	
      { 0x35,	0x45,	0xFF,	0x90},	// W#463 - 	Jump to COUNT_LE_57_CHECK if SWIRE_EDGE_COUNT > 53
      { 0xA4,	0x0C,	0xFF,	0x83},	// W#464 - 	
      { 0x39,	0x45,	0xFF,	0x92},	// W#465 - 	Jump to COUNT_CHECK_DONE if SWIRE_EDGE_COUNT > 57
      { 0x35,	0x00,	0x76,	0x28},	// W#466 - 	Load R6 with Constant 0x35 (d'53)
      { 0xF4,	0xF6,	0x74,	0x26},	// W#467 - 	R4 = R4 (SWIRE_EDGE_COUNT {54:57}) - R6 (d'53)
      { 0xF4,	0x41,	0xDE,	0x09},	// W#468 - 	Set LAB Voltage
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#469 - 	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#470 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#471 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#472 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#473 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#474 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#475 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#476 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#477 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#478 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#479 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#480 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#481 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#482 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#483 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#484 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#485 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#486 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#487 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#488 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#489 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#490 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#491 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#492 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#493 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#494 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#495 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#496 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#497 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#498 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#499 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#500 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#501 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#502 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#503 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#504 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#505 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#506 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#507 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#508 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#509 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#510 - 	Inserted by compiler.
      { 0x09,	0x0C,	0xFF,	0xF8},	// W#511 - 	Inserted by compiler.
   },
   // "PM670L_RAM_b0x19_v0x15"
   {
      //data  offset  base_addr  sid
      { 0x52,	0x53,	0x42,	0x50},	// W#0 - 	0x600 RAM signature PBSR""
      { 0x19,	0x15,	0x01,	0x18},	// W#1 - 	0x604 Header offset, Header Version, PBS RAM Revision, PBS RAM Branch
      { 0x34,	0x06,	0xFF,	0x83},	// W#2 - 	0x608 start of trigger jump table:
      { 0x70,	0x06,	0xFF,	0x83},	// W#3 - 	0x60C Client-1 Trigger
      { 0xD8,	0x06,	0xFF,	0x83},	// W#4 - 	0x610 Client-2 Trigger
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#5 - 	0x614 Empty (Drax does not have Client-3)
      { 0xE4,	0x0B,	0xFF,	0x83},	// W#6 - 	0x618 Fixed Offset = RAM-Base-Addr + 0x18 + 0x00 => SLEEP-SET
      { 0xF8,	0x0B,	0xFF,	0x83},	// W#7 - 	0x61C Fixed Offset = RAM-Base-Addr + 0x18 + 0x04 => (PON X Reasons
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#8 - 	0x620 Fixed Offset = RAM-Base-Addr + 0x18 + 0x08 => (NO RTRR)
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#9 - 	0x624 Fixed Offset = RAM-Base-Addr + 0x18 + 0x0C => (No HLOS) / Spare
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#10 - 	0x628 Fixed Offset = RAM-Base-Addr + 0x18 + 0x10 => (Spare registers) / LPG
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#11 - 	0x62C Fixed Offset = RAM-Base-Addr + 0x18 + 0x14 => (LPG Dataset)
      { 0x24,	0x0C,	0xFF,	0x83},	// W#12 - 	0x630 Fixed Offset = RAM-Base-Addr + 0x18 + 0x18 => (USER-Space: AMOLED Panel-Specific Implementation)
      { 0xFF,	0x48,	0x71,	0x00},	// W#13 - 	Clear all latched sequences in PBS PON Client 0
      { 0xFB,	0x08,	0x08,	0x40},	// W#14 - 	READ PON__FSM_STATUS TO RB; Store PON_FSM_STATUS in RB for Older OTP-Revs < v0x05
      { 0xFC,	0x08,	0x08,	0x40},	// W#15 - 	READ PON__FSM_STATUS TO RC; Store PON_FSM_STATUS in RC for Newer OTP-Revs >= v0x05
      { 0x04,	0xC0,	0xFF,	0x90},	// W#16 - 	Skip if NOT DVDD_CONFIG (State 4)
      { 0x14,	0x00,	0xFF,	0x83},	// W#17 - 	GOTO OTP_DVDD_CONFIG (OTP); Is there a scenario where this is ever used?
      { 0x01,	0xC0,	0xFF,	0x90},	// W#18 - 	Skip if NOT PON (State 1)
      { 0x14,	0x00,	0xFF,	0x83},	// W#19 - 	GOTO OTP_DVDD_CONFIG (OTP); Is there a scenario where this is ever used?
      { 0x06,	0xC0,	0xFF,	0x90},	// W#20 - 	Skip if NOT FAULT (State 6)
      { 0x4C,	0x07,	0xFF,	0x83},	// W#21 - 	
      { 0x02,	0xC0,	0xFF,	0x90},	// W#22 - 	Skip if NOT POFF (State 2)
      { 0x2C,	0x08,	0xFF,	0x83},	// W#23 - 	
      { 0x07,	0xC0,	0xFF,	0x90},	// W#24 - 	Skip if NOT WARM RESET (State 7)
      { 0x70,	0x07,	0xFF,	0x83},	// W#25 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#26 - 	Undefined State - NACK PON (Note:  None of the Client GOTOs have a 'RETURN'; the ACK is done in each subroutine)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#27 - 	End of Sequence; Byte-2
      { 0xF3,	0x0A,	0x72,	0x40},	// W#28 - 	Read Client 1 trigger latch status and store to Buffer R3
      { 0x01,	0x30,	0x01,	0x91},	// W#29 - 	Skip if NOT Bit 0 (DTEST MUX)
      { 0x01,	0x48,	0x72,	0x00},	// W#30 - 	Clear Latched Trigger - Bit 0
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#31 - 	CL1_TRIG0:EMPTY => Replace with GOTO once defined.
      { 0x02,	0x30,	0x02,	0x91},	// W#32 - 	Skip if NOT Bit 1 (BoB)
      { 0x02,	0x48,	0x72,	0x00},	// W#33 - 	Clear Latched Trigger - Bit 1
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#34 - 	CL1_TRIG1:EMPTY => Replace with GOTO once defined.
      { 0x04,	0x30,	0x04,	0x91},	// W#35 - 	Skip if NOT Bit 2 (BA_N)
      { 0x04,	0x48,	0x72,	0x00},	// W#36 - 	Clear Latched Trigger - Bit 2
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#37 - 	CL1_TRIG2:EMPTY => Replace with GOTO once defined.
      { 0x08,	0x30,	0x08,	0x91},	// W#38 - 	Skip if NOT Bit 3 (LAB_IBB_SCP)
      { 0x08,	0x48,	0x72,	0x00},	// W#39 - 	Clear Latched Trigger - Bit 3
      { 0x88,	0x09,	0xFF,	0x83},	// W#40 - 	CL1_TRIG3: LAB_IBB_SCP
      { 0x10,	0x30,	0x10,	0x91},	// W#41 - 	Skip if NOT Bit 4 (EOSR_Timer)
      { 0x10,	0x48,	0x72,	0x00},	// W#42 - 	Clear Latched Trigger - Bit 4
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#43 - 	CL1_TRIG4:EMPTY => Replace with GOTO once defined.
      { 0x20,	0x30,	0x20,	0x91},	// W#44 - 	Skip if NOT Bit 5 (BCL)
      { 0x20,	0x48,	0x72,	0x00},	// W#45 - 	Clear Latched Trigger - Bit 5
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#46 - 	CL1_TRIG5:EMPTY => Replace with GOTO once defined.
      { 0x40,	0x30,	0x40,	0x91},	// W#47 - 	Skip if NOT Bit 6 (OTS2)
      { 0x40,	0x48,	0x72,	0x00},	// W#48 - 	Clear Latched Trigger - Bit 6
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#49 - 	CL1_TRIG6:EMPTY => Replace with GOTO once defined.
      { 0x80,	0x30,	0x80,	0x91},	// W#50 - 	Bit 7 = SW Trigger (System Sleep/Wake)
      { 0x80,	0x48,	0x72,	0x00},	// W#51 - 	Clear Latched Trigger - Bit 7
      { 0xA0,	0x09,	0xFF,	0x83},	// W#52 - 	CL1_TRIG7: CLIENT1_SYSTEM_SLEEP_WAKE_SEQUENCE
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#53 - 	
      { 0xF4,	0x0A,	0x73,	0x40},	// W#54 - 	Read Client 2 trigger latch status and store to Buffer R4
      { 0x01,	0x40,	0x01,	0x91},	// W#55 - 	Skip if not Bit 0 (WCSS pin control, Was EUD from Groot)
      { 0x01,	0x48,	0x73,	0x00},	// W#56 - 	Clear Latched Trigger - Bit 0
      { 0xD0,	0x0A,	0xFF,	0x83},	// W#57 - 	CL2_TRIG0: WCSS_CTRL_PIN_STATE_CHECK
      { 0x02,	0x40,	0x02,	0x91},	// W#58 - 	Skip if not Bit 1 (uSD switch detect, GPIO08)
      { 0x02,	0x48,	0x73,	0x00},	// W#59 - 	Clear Latched Trigger - Bit 1
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#60 - 	CL2_TRIG1:EMPTY => Replace with GOTO once defined.
      { 0x04,	0x40,	0x04,	0x91},	// W#61 - 	Skip if not Bit 2 (SWIRE decode from AOMLED)
      { 0x04,	0x48,	0x73,	0x00},	// W#62 - 	Clear Latched Trigger - Client2 Bit 2
      { 0x28,	0x0C,	0xFF,	0x83},	// W#63 - 	CL2_TRIG2: USER_SPACE_POINTER1
      { 0x08,	0x40,	0x08,	0x91},	// W#64 - 	Skip if not Bit 3 (SSC_PWR_EN from MSM)
      { 0x08,	0x48,	0x73,	0x00},	// W#65 - 	Clear Latched Trigger - Bit 3
      { 0x0C,	0x0B,	0xFF,	0x83},	// W#66 - 	CL2_TRIG3: MULTICARDTRAY_UICC_PROTECTION_SEQ
      { 0x10,	0x40,	0x10,	0x91},	// W#67 - 	Skip if not Bit 4 (SLB, GPIO10, Trigger from Groot)
      { 0x10,	0x48,	0x73,	0x00},	// W#68 - 	Clear Latched Trigger - Bit 4
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#69 - 	CL2_TRIG4:EMPTY => Replace with GOTO once defined.
      { 0x20,	0x40,	0x20,	0x91},	// W#70 - 	Skip if not Bit 5 (EOSR_Timer)
      { 0x20,	0x48,	0x73,	0x00},	// W#71 - 	Clear Latched Trigger - Bit 5
      { 0x68,	0x0B,	0xFF,	0x83},	// W#72 - 	CL2_TRIG5: LAB_PSRR_EOSR_SEQUENCE
      { 0x40,	0x40,	0x40,	0x94},	// W#73 - 	Skip if not Bit 6 (LAB_BANDING_DETECTION)
      { 0xF1,	0x40,	0x73,	0x40},	// W#74 - 	Read RE trigger config for RMW
      { 0xBF,	0x00,	0xF1,	0x2C},	// W#75 - 	clear bit 6 to disable RE trigger
      { 0xF1,	0x40,	0x73,	0x08},	// W#76 - 	RE disabled for client2 bit 6
      { 0x40,	0x48,	0x73,	0x00},	// W#77 - 	Clear PBS_CLIENT2 Bit6 Trig-Latch after disabling RE on corresponding Bit6 above
      { 0x54,	0x0B,	0xFF,	0x83},	// W#78 - 	CL2_TRIG6: LAB_BANDING_DETECTION
      { 0x80,	0x40,	0x80,	0x91},	// W#79 - 	Skip if not Bit 7 (SW Trigger)
      { 0x80,	0x48,	0x73,	0x00},	// W#80 - 	1BYTE WRITE 0x80 TO PBS_CLIENT3__TRIG_LATCHED_CLR; Clear Latched Trigger - Bit 7
      { 0xD0,	0x0B,	0xFF,	0x83},	// W#81 - 	CL2_TRIG7: CLIENT2_SW_TRIGGER
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#82 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#83 - 	
      { 0x28,	0x09,	0xFF,	0x9B},	// W#84 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x88,	0x48,	0x14,	0x01},	// W#85 - 	1BYTE WRITE 0x88 TO S1_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x17,	0x01},	// W#86 - 	1BYTE WRITE 0x88 TO S2_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x1A,	0x01},	// W#87 - 	1BYTE WRITE 0x88 TO S3_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0x88,	0x48,	0x20,	0x01},	// W#88 - 	1BYTE WRITE 0x88 TO S5_CTRL__PD_CTL; Enable strong + leak pulldown
      { 0xEA,	0x01,	0xFF,	0xC1},	// W#89 - 	WAIT 15016 us; Wait 15ms for VREGs to discharge completely before allowing the next PON trigger
      { 0x40,	0x91,	0x08,	0x00},	// W#90 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#91 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#92 - 	
      { 0x28,	0x09,	0xFF,	0x9B},	// W#93 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x08,	0x08,	0x41,	0x89},	// W#94 - 	
      { 0x08,	0x08,	0x44,	0x89},	// W#95 - 	
      { 0x02,	0x00,	0x70,	0x28},	// W#96 - 	1BYTE R0 = 0x02; R0 = 0x2
      { 0x0C,	0x09,	0xFF,	0x9B},	// W#97 - 	GOTO INIT_RESET PON. Do reset stuff common to warm reset and POFF
      { 0x07,	0x45,	0x40,	0x01},	// W#98 - 	: 23
      { 0x07,	0x45,	0x41,	0x01},	// W#99 - 	: 27
      { 0x07,	0x45,	0x43,	0x01},	// W#100 - 	: 26
      { 0x07,	0x45,	0x44,	0x01},	// W#101 - 	: 28
      { 0x07,	0x45,	0x46,	0x01},	// W#102 - 	: 25
      { 0x78,	0x40,	0x14,	0x01},	// W#103 - 	S01B: LB for WL_PON-VSET = 0x03_78 => 0888 mV
      { 0x03,	0x41,	0x14,	0x01},	// W#104 - 	S01B: UB for WL_PON-VSET = 0x03_78 => 0888 mV
      { 0x40,	0x40,	0x1A,	0x01},	// W#105 - 	S03B_S04B: LB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x03,	0x41,	0x1A,	0x01},	// W#106 - 	S03B_S04B: UB for WL_PON-VSET = 0x03_40 => 0832 mV
      { 0x58,	0x40,	0x20,	0x01},	// W#107 - 	S05B: LB for WL_PON-VSET = 0x02_58 => 0600 mV
      { 0x02,	0x41,	0x20,	0x01},	// W#108 - 	S05B: UB for WL_PON-VSET = 0x02_58 => 0600 mV
      { 0x70,	0x40,	0x40,	0x01},	// W#109 - 	L01B: LB for WL_PON-VSET = 0x03_70 => 0888 mV
      { 0x03,	0x41,	0x40,	0x01},	// W#110 - 	L01B: UB for WL_PON-VSET = 0x03_70 => 0888 mV
      { 0x90,	0x40,	0x41,	0x01},	// W#111 - 	L02B: LB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x0B,	0x41,	0x41,	0x01},	// W#112 - 	L02B: UB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x90,	0x40,	0x43,	0x01},	// W#113 - 	L04B: LB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x0B,	0x41,	0x43,	0x01},	// W#114 - 	L04B: UB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x90,	0x40,	0x44,	0x01},	// W#115 - 	L05B: LB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x0B,	0x41,	0x44,	0x01},	// W#116 - 	L05B: UB for WL_PON-VSET = 0x0B_90 => 2960 mV
      { 0x10,	0x40,	0x46,	0x01},	// W#117 - 	L07B: LB for WL_PON-VSET = 0x0C_10 => 3088 mV
      { 0x0C,	0x41,	0x46,	0x01},	// W#118 - 	L07B: UB for WL_PON-VSET = 0x0C_10 => 3088 mV
      { 0x38,	0x40,	0x48,	0x01},	// W#119 - 	L09B: LB for WL_PON-VSET = 0x03_38 => 0824 mV
      { 0x03,	0x41,	0x48,	0x01},	// W#120 - 	L09B: UB for WL_PON-VSET = 0x03_38 => 0824 mV
      { 0x70,	0x40,	0x49,	0x01},	// W#121 - 	L10B: LB for WL_PON-VSET = 0x03_70 => 0880 mV
      { 0x03,	0x41,	0x49,	0x01},	// W#122 - 	L10B: UB for WL_PON-VSET = 0x03_70 => 0880 mV
      { 0xE0,	0x40,	0xA0,	0x01},	// W#123 - 	1BYTE WRITE 0xE0 TO BOB_CONFIG__VOUT_LB; Set VBOB to 3.296V since BOB ignores warm_rb
      { 0x0C,	0x41,	0xA0,	0x01},	// W#124 - 	1BYTE WRITE 0x0C TO BOB_CONFIG__VOUT_UB; Set VBOB to 3.296V since BOB ignores warm_rb
      { 0x02,	0x45,	0xA0,	0x01},	// W#125 - 	1BYTE WRITE 0x02 TO BOB_CONFIG__VOUT_MODE_CTRL; Set BOB to auto mode since BOB ignores warm_rb
      { 0x00,	0x36,	0xA0,	0x01},	// W#126 - 	1BYTE WRITE 0x00 TO BOB_CONFIG__EXT_CTRL_FOLLOW; Ignore EXT_CTRL1 & 2. Explicit command since BOB ignores warm_rb
      { 0x18,	0x08,	0xFF,	0x9B},	// W#127 - 	GOTO_SUB_NOID WR_MSM_BOOT_VOLTAGE_SS_NOM; Set CX/MX/APC to SS Nominal (new values
      { 0x40,	0x91,	0x08,	0x00},	// W#128 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#129 - 	End of Sequence; Byte-2
      { 0xD2,	0x66,	0xFF,	0x04},	// W#130 - 	Assert OCP_CTL1.Bit1 <OCP_STATUS_CLR> = 1
      { 0x03,	0x00,	0xFF,	0xC1},	// W#131 - 	WAIT 91 us = 3 cycles of 32k clock
      { 0xD0,	0x66,	0xFF,	0x04},	// W#132 - 	De-Assert OCP_CTL1.Bit1 <OCP_STATUS_CLR> = 0
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#133 - 	
      { 0x40,	0x40,	0x1A,	0x01},	// W#134 - 	1BYTE WRITE 0x40 TO S1_CTRL__VSET_LB; Bump S3+S4 (CX) to 0.832V
      { 0x03,	0x41,	0x1A,	0x01},	// W#135 - 	1BYTE WRITE 0x03 TO S1_CTRL__VSET_UB; Bump S3+S4 (CX) to 0.832V
      { 0x78,	0x40,	0x14,	0x01},	// W#136 - 	1BYTE WRITE 0x78 TO S5_CTRL__VSET_LB; Bump S1 (MX) to 0.888V (was S5 on SL, moved to S1 on Warlock)
      { 0x03,	0x41,	0x14,	0x01},	// W#137 - 	1BYTE WRITE 0x03 TO S5_CTRL__VSET_UB; Bump S1 (MX) to 0.888V (was S5 on SL, moved to S1 on Warlock)
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#138 - 	End of subroutine. PC = Stored PC.; Byte-2
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#139 - 	
      { 0x28,	0x09,	0xFF,	0x9B},	// W#140 - 	Record PON REASONS into dataset (Circular Buffer with History-Size = 3)
      { 0x04,	0x00,	0x70,	0x28},	// W#141 - 	1BYTE R0 = 0x04; R0=0x4. This value will be applied to the PON_RESET_CTL register in PON (bit 2 warm_rb
      { 0x0C,	0x09,	0xFF,	0x9B},	// W#142 - 	GOTO INIT_RESET PON. Do reset stuff common to warm reset and POFF
      { 0x00,	0x40,	0xC9,	0x00},	// W#143 - 	Configure GPIO10 to Dig INPUT
      { 0x04,	0x42,	0xC9,	0x00},	// W#144 - 	Set SLB to 10uA pull-down
      { 0x0C,	0x0E,	0x44,	0x89},	// W#145 - 	LDO5B, SD/MMC Card
      { 0x0C,	0x0E,	0x41,	0x89},	// W#146 - 	LDO2B, VDDPX_2 (SD_MMC) - PMICPR-644
      { 0x0C,	0x0E,	0x43,	0x89},	// W#147 - 	LDO4B, UFS (2.96V)
      { 0x0C,	0x0E,	0x46,	0x89},	// W#148 - 	LDO7B, USB (EUD)
      { 0xA3,	0x00,	0x76,	0x29},	// W#149 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6. 146
      { 0xDC,	0x08,	0xFF,	0x9B},	// W#150 - 	
      { 0x46,	0x01,	0x76,	0x29},	// W#151 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6. 146
      { 0xB0,	0x08,	0xFF,	0x9B},	// W#152 - 	
      { 0x0C,	0x0E,	0x40,	0x89},	// W#153 - 	LDO1B, UFS_PHY
      { 0x0C,	0x0E,	0x20,	0x89},	// W#154 - 	LP-DDR4X VDDQ
      { 0xA3,	0x00,	0x76,	0x29},	// W#155 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6. 146
      { 0xDC,	0x08,	0xFF,	0x9B},	// W#156 - 	
      { 0xF4,	0x01,	0x76,	0x29},	// W#157 - 	WAIT 10010 us; Wait 10ms.  Store to Buffer R6.  0x146
      { 0xB0,	0x08,	0xFF,	0x9B},	// W#158 - 	
      { 0x0C,	0x0E,	0x48,	0x89},	// W#159 - 	LDO9, LPI_Cx
      { 0x0C,	0x0E,	0x1A,	0x89},	// W#160 - 	CX+GPU+cDSP
      { 0x0C,	0x0E,	0x49,	0x89},	// W#161 - 	LDO10, LPI Mx
      { 0x0C,	0x0E,	0x14,	0x89},	// W#162 - 	S1B, LPDDR4
      { 0x04,	0x42,	0xCA,	0x00},	// W#163 - 	GPIO_11B: Revert Pull-config to PD_10uA (irrespective of DIG-State)
      { 0x00,	0x44,	0xCA,	0x00},	// W#164 - 	GPIO_11B: Configure to Drive-Low (strong-PD), when in DIG_OUT Mode
      { 0x01,	0x40,	0xCA,	0x00},	// W#165 - 	GPIO_11B: Configure to DIG_OUT Mode
      { 0x21,	0x00,	0xFF,	0xC1},	// W#166 - 	WAIT 1068 us; Wait 33+2 32kHz clock cycles - for DBU1 to discharge
      { 0x0C,	0x0E,	0xA0,	0x89},	// W#167 - 	Disable BOB
      { 0x00,	0x46,	0x05,	0x00},	// W#168 - 	1BYTE WRITE 0x00 TO INT__EN_CTL1; Disable INT
      { 0x46,	0x01,	0xFF,	0xC1},	// W#169 - 	WAIT 10010 us; Wait 10ms for VREGs to discharge completely before allowing the next PON trigger
      { 0x40,	0x91,	0x08,	0x00},	// W#170 - 	1BYTE WRITE 0x40 TO PON__PBS_INTERFACE; ACK PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#171 - 	
      { 0x01,	0x00,	0xFF,	0xC1},	// W#172 - 	wait 1 CLK CYCLE BEFORE CHECKING
      { 0xF1,	0x08,	0xC9,	0x40},	// W#173 - 	Read GPIO10 Status (GPIO_VAL) and store to Buffer R1
      { 0x01,	0x10,	0x01,	0x92},	// W#174 - 	We will exit  = '1'
      { 0x01,	0x40,	0xC9,	0x00},	// W#175 - 	Configure GPIO10 to Dig Output
      { 0x00,	0x44,	0xC9,	0x00},	// W#176 - 	Set SLB LOW and continue
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#177 - 	
      { 0x01,	0x00,	0x76,	0x25},	// W#178 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#179 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0xB0,	0x08,	0xFF,	0x83},	// W#180 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#181 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#182 - 	End of Sequence; Return
      { 0x00,	0x40,	0xC9,	0x00},	// W#183 - 	Configure GPIO10 to Dig INPUT
      { 0x00,	0x42,	0xC9,	0x00},	// W#184 - 	Set SLB to PU
      { 0x01,	0x00,	0xFF,	0xC1},	// W#185 - 	wait 30us before trying again
      { 0xF1,	0x08,	0xC9,	0x40},	// W#186 - 	Read GPIO10 Status (GPIO_VAL) and store to Buffer R1
      { 0x00,	0x10,	0x01,	0x91},	// W#187 - 	Skip the next line if GPIO10_STATUS1 (bit 0) is high
      { 0x05,	0x42,	0xC9,	0x00},	// W#188 - 	Set SLB to No pull
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#189 - 	return to the next linE of the calling routing
      { 0x01,	0x00,	0x76,	0x25},	// W#190 - 	Subtract 1 from the loop count (R6)
      { 0x00,	0x06,	0x00,	0x90},	// W#191 - 	If the subi in the previous command resulted in a carry then we have reached the terminal count.
      { 0xE4,	0x08,	0xFF,	0x83},	// W#192 - 	
      { 0x00,	0x91,	0x08,	0x00},	// W#193 - 	Time count expired.  Nack the PON! SID=0, PID=8, Addr = 0x91
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#194 - 	End of Sequence; Return
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#195 - 	NOP; //This Subroutine does several things that are common to the Warm Reset and POFF seqeunces.  1) disable SPMI to avoid lockup
      { 0x00,	0x43,	0x08,	0x02},	// W#196 - 	1BYTE WRITE 0x00 TO SID:2 0x0800__0x43; Set SPMI_SLAVE_RB to 0 to tolerate glitches during power down
      { 0xF0,	0x98,	0x08,	0x18},	// W#197 - 	1BYTE WRITE R0 TO SID:0 0x0800__0x98; Assert whatever resets are defined in R0
      { 0x03,	0x00,	0xFF,	0xC1},	// W#198 - 	WAIT 91.5 us; Wait 3 SLEEP_CLK cycles for reset changes to propagate through clock domains
      { 0x06,	0x98,	0x08,	0x10},	// W#199 - 	1BYTE WRITE 0x06 TO SID:0 0x0800__0x98; Deassert Resets
      { 0x01,	0x43,	0x08,	0x02},	// W#200 - 	Set SPMI_SLAVE_RB back to 1
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#201 - 	
      { 0x1C,	0x06,	0x70,	0xA1},	// W#202 - 	
      { 0x1C,	0x00,	0x70,	0x21},	// W#203 - 	2BYTE R0 = R0 + 0x1C ([5_Datasets x 2_Rows/Dataset - 2_SRC - 1] = 7 Rows x 0x04 Addr-Offset per Row)
      { 0x1C,	0x06,	0x72,	0xA1},	// W#204 - 	
      { 0x24,	0x00,	0x72,	0x21},	// W#205 - 	2BYTE R2 = R2 + 0x24 ([5_Datasets x 2_Rows/Dataset - 0_DST - 1] = 9 Rows x 0x04 Addr-Offset per Row)
      { 0x08,	0x00,	0x78,	0x28},	// W#206 - 	1BYTE R8 = 0x08 (Loop 8 times = [5_Datasets x 2_Rows/Dataset - 2_Rows_for_CURR])
      { 0xF0,	0xFF,	0xF4,	0xA3},	// W#207 - 	4BYTE R4 = MEMADDR(R0); copy the last reason registers into a buffer
      { 0xF2,	0xFF,	0xF4,	0xA7},	// W#208 - 	4BYTE MEMADDR(R2) = R4; write the reasons to the location pointed to by the paste pointer
      { 0x01,	0x00,	0x78,	0x24},	// W#209 - 	
      { 0x00,	0x81,	0xFF,	0x92},	// W#210 - 	SKIP 3 Lines if R8 EQUAL 0x00 (MASK=0xFF); Loop Until loop count (R8) = 4
      { 0x04,	0x00,	0x70,	0x25},	// W#211 - 	2BYTE R0 = R0 - 0x04; Move read pointer to next (upper) DWORD
      { 0x04,	0x00,	0x72,	0x25},	// W#212 - 	2BYTE R2 = R2 - 0x04; Move write pointer to next (upper) DWORD
      { 0x3C,	0x09,	0xFF,	0x83},	// W#213 - 	
      { 0xF4,	0xC0,	0x08,	0x40},	// W#214 - 	
      { 0xF5,	0xC2,	0x08,	0x40},	// W#215 - 	
      { 0xF6,	0xC4,	0x08,	0x40},	// W#216 - 	
      { 0xF7,	0xC5,	0x08,	0x40},	// W#217 - 	
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#218 - 	4BYTE MEMADDR(R0) = R4
      { 0x04,	0x00,	0x70,	0x21},	// W#219 - 	2BYTE R0 = R0 + 0x04
      { 0xF4,	0xC7,	0x08,	0x40},	// W#220 - 	
      { 0xF5,	0xC8,	0x08,	0x40},	// W#221 - 	
      { 0xF6,	0xC9,	0x08,	0x40},	// W#222 - 	
      { 0xFF,	0x00,	0x77,	0x28},	// W#223 - 	1BYTE R7 = 0xFF
      { 0xF0,	0xFF,	0xF4,	0xA7},	// W#224 - 	4BYTE MEMADDR(R0) = R4; Write data to address stored in R0
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#225 - 	End of subroutine. PC = Stored PC.; Byte-2
      { 0xF0,	0x58,	0xDC,	0x11},	// W#226 - 	PMICPR-1051: Minimize delay b/w IBB & LAB discharge
      { 0x00,	0x46,	0xDC,	0x01},	// W#227 - 	Disable IBB
      { 0x00,	0x46,	0xDE,	0x01},	// W#228 - 	Disable LAB
      { 0x00,	0x47,	0xE0,	0x01},	// W#229 - 	Disable OLEDB pin control
      { 0x00,	0x46,	0xE0,	0x01},	// W#230 - 	Disable OLEDB module
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#231 - 	
      { 0xF7,	0x50,	0x72,	0x40},	// W#232 - 	1BYTE READ PBS_CLIENT1__SCRATCH1 TO R7; Readback PBS_CLIENT1_SCRATCH1 and store to Buffer R7
      { 0x04,	0x70,	0xFF,	0x90},	// W#233 - 	SKIP 1 Lines if R7 NOT EQUAL 0x04 (MASK=0xFF)
      { 0xC8,	0x09,	0xFF,	0x83},	// W#234 - 	
      { 0x03,	0x70,	0xFF,	0x90},	// W#235 - 	SKIP 1 Lines if R7 NOT EQUAL 0x03 (MASK=0xFF)
      { 0xD0,	0x09,	0xFF,	0x83},	// W#236 - 	
      { 0x02,	0x70,	0xFF,	0x90},	// W#237 - 	SKIP 1 Lines if R7 NOT EQUAL 0x02 (MASK=0xFF)
      { 0xDC,	0x09,	0xFF,	0x83},	// W#238 - 	
      { 0x01,	0x70,	0xFF,	0x90},	// W#239 - 	SKIP 1 Lines if R7 NOT EQUAL 0x01 (MASK=0xFF)
      { 0x50,	0x0A,	0xFF,	0x83},	// W#240 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#241 - 	
      { 0x04,	0x51,	0x72,	0x00},	// W#242 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#243 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#244 - 	
      { 0x03,	0x51,	0x72,	0x00},	// W#245 - 	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#246 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#247 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#248 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#249 - 	
      { 0x02,	0x45,	0xA0,	0x01},	// W#250 - 	1BYTE WRITE 0x02 TO BOB_CONFIG__VOUT_MODE_CTRL; Put BOB in Auto Mode
      { 0x01,	0x4A,	0x59,	0x10},	// W#251 - 	1BYTE WRITE 0x01 TO CLK_DIST__PMIC_SLEEP_CTL; Set SLEEP_B to follow HW
      { 0x90,	0xA6,	0x14,	0x11},	// W#252 - 	Re-enable S1 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x14,	0x11},	// W#253 - 	Re-enable S1 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x14,	0x11},	// W#254 - 	Enable S1 VDIP comparator; keep 15mV threshold
      { 0x90,	0xA6,	0x17,	0x11},	// W#255 - 	Re-enable S2 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x17,	0x11},	// W#256 - 	Re-enable S2 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x17,	0x11},	// W#257 - 	Enable S2 VDIP comparator; keep 15mV threshold
      { 0x90,	0xA6,	0x1A,	0x11},	// W#258 - 	Re-enable S3 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x1A,	0x11},	// W#259 - 	Re-enable S3 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x1A,	0x11},	// W#260 - 	Enable S3 VDIP comparator; keep 15mV threshold
      { 0x90,	0xA6,	0x20,	0x11},	// W#261 - 	Re-enable S5 PFM_EXIT_DIP_EN with DIP_LAT_BYP Disabled
      { 0x32,	0xA9,	0x20,	0x11},	// W#262 - 	Re-enable S5 PFM_EXIT_CL_EN (Disable CLL LPM entry method)
      { 0x09,	0x71,	0x20,	0x11},	// W#263 - 	Enable S5 VDIP comparator; keep 15mV threshold
      { 0x18,	0x06,	0x76,	0xA1},	// W#264 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0x40,	0x0A,	0x14,	0x89},	// W#265 - 	
      { 0x40,	0x0A,	0x17,	0x89},	// W#266 - 	
      { 0x40,	0x0A,	0x1A,	0x89},	// W#267 - 	
      { 0x40,	0x0A,	0x20,	0x89},	// W#268 - 	
      { 0x87,	0x00,	0xFE,	0xC1},	// W#269 - 	WAIT 7083 ns; Wait 7us (part of CR-175088)
      { 0x02,	0x51,	0x72,	0x00},	// W#270 - 	Set SCRATCH2 to 0x02 (to indicate to SW that wake sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#271 - 	
      { 0xF6,	0xFF,	0x74,	0xA2},	// W#272 - 	Load R2 to R5 with data from location pointed to by R6
      { 0xF4,	0x45,	0xFF,	0x0C},	// W#273 - 	ReStore MODE_CTL1 from R4
      { 0x04,	0x00,	0x76,	0x21},	// W#274 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#275 - 	End of subroutine.
      { 0x18,	0x06,	0x76,	0xA1},	// W#276 - 	Using Location stored in GOTO-Pointer at LOC_SLEEP_BACKUP_DATASET_HEAD
      { 0xC0,	0x0A,	0x14,	0x89},	// W#277 - 	
      { 0xC0,	0x0A,	0x17,	0x89},	// W#278 - 	
      { 0xC0,	0x0A,	0x1A,	0x89},	// W#279 - 	
      { 0xC0,	0x0A,	0x20,	0x89},	// W#280 - 	
      { 0x01,	0x71,	0x14,	0x11},	// W#281 - 	Disable S1 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x14,	0x11},	// W#282 - 	Disable S1 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x14,	0x11},	// W#283 - 	Disable S1 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x14,	0x01},	// W#284 - 	Set S1 to Retention Mode
      { 0x01,	0x71,	0x17,	0x11},	// W#285 - 	Disable S2 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x17,	0x11},	// W#286 - 	Disable S2 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x17,	0x11},	// W#287 - 	Disable S2 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x17,	0x01},	// W#288 - 	Set S2 to Retention Mode
      { 0x01,	0x71,	0x1A,	0x11},	// W#289 - 	Disable S3 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x1A,	0x11},	// W#290 - 	Disable S3 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x1A,	0x11},	// W#291 - 	Disable S3 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x1A,	0x01},	// W#292 - 	Set S3 to Retention Mode
      { 0x01,	0x71,	0x20,	0x11},	// W#293 - 	Disable S5 VDIP comparator; keep 15mV threshold
      { 0x50,	0xA6,	0x20,	0x11},	// W#294 - 	Disable S5 PFM_EXIT_DIP_EN to prevent using HCPFM CL
      { 0x92,	0xA9,	0x20,	0x11},	// W#295 - 	Disable S5 PFM_EXIT_CL_EN to prevent using HCPFM CL
      { 0x04,	0x45,	0x20,	0x01},	// W#296 - 	Set S5 to Retention Mode
      { 0x00,	0x4A,	0x59,	0x10},	// W#297 - 	Force SLEEP_B low
      { 0xF6,	0x8F,	0x09,	0x40},	// W#298 - 	Load R6 with the value in MISC.VSET6_UB [Bit-0 = BoB Sleep-Instruction]
      { 0x00,	0x61,	0x01,	0x90},	// W#299 - 	If Bit-0 = 0 SKIP putting BoB to Bypass (needs to be done for Groot 1.0)
      { 0x00,	0x45,	0xA0,	0x01},	// W#300 - 	Put BOB in Bypass
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#301 - 	
      { 0x01,	0x51,	0x72,	0x00},	// W#302 - 	Set SCRATCH2 to 0x01 (to indicate to SW that sleep sequence just ran)
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#303 - 	
      { 0xF4,	0x45,	0xFF,	0x44},	// W#304 - 	Store MODE_CTL1 into R4
      { 0xF6,	0xFF,	0x74,	0xA6},	// W#305 - 	Load data into location pointed to by R6
      { 0x04,	0x00,	0x76,	0x21},	// W#306 - 	Increment pointer by 4 to point at the next set
      { 0xFF,	0xFF,	0xFF,	0x8F},	// W#307 - 	End of subroutine.
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#308 - 	WCSS Debug Placeholder-0
      { 0xF8,	0x08,	0xC8,	0x40},	// W#309 - 	Read GPIO09B Pin STATUS & Load into R8
      { 0x01,	0x81,	0x01,	0x90},	// W#310 - 	If GPIO9B PIN = High => Set-WCSS-to-Active Trigger was detected
      { 0xF0,	0x0A,	0xFF,	0x83},	// W#311 - 	Else If GPIO9 PIN = Low => Set-WCSS-to-Retention Trigger was detected
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#312 - 	WCSS Debug Placeholder-1
      { 0x01,	0x94,	0x09,	0x10},	// W#313 - 	Set WCSS_STATE register with updated value (Bit-0 = 1) => WCSS_ACTIVE
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#314 - 	WCSS Debug Placeholder-2
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#315 - 	
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#316 - 	Do nothing. (Buffer to apply separate Tags to appropriate TAGs to this & next line)
      { 0xF8,	0x93,	0x09,	0x40},	// W#317 - 	Read present value of MISC.SSC_STATE & Load into R8
      { 0x01,	0x81,	0x01,	0x90},	// W#318 - 	Check SSC_STATE<0>. If = High => proceed w/o touching S2B.PBS_VOTE
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#319 - 	
      { 0x00,	0x94,	0x09,	0x10},	// W#320 - 	Clear WCSS_STATE register with updated value (Bit-0 = 0) => WCSS_RETENTION
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#321 - 	WCSS Debug Placeholder-4
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#322 - 	
      { 0xF2,	0x1A,	0xC1,	0x40},	// W#323 - 	Read GPIO02.INT_MID_SEL for SD-Card to UICC PairingInfo
      { 0x00,	0x20,	0x03,	0x90},	// W#324 - 	
      { 0x50,	0x0B,	0xFF,	0x83},	// W#325 - 	End the sequence if SD-card is NOT paired with any UICC-Card in the tray
      { 0xF5,	0x11,	0xC1,	0x40},	// W#326 - 	Read GPIO02.INT_SET_TYPE into R5 for Expected-state of SD_CARD_PRESENCE
      { 0xF6,	0x08,	0xC1,	0x40},	// W#327 - 	Read GPIO02.STATUS1 into R6 for current-state of GPIO (High/Low)
      { 0xF5,	0xF6,	0x77,	0x22},	// W#328 - 	Update R7 = R6 + R5, Ignore Carry
      { 0x01,	0x71,	0x01,	0x90},	// W#329 - 	IF   R7<0> == 1'b1 ==> ACTUAL_STATE != SD_PRESENCE_STATE --> Proceed to Debounce
      { 0x50,	0x0B,	0xFF,	0x83},	// W#330 - 	ELSE R7<0> == 1'b0 ==> ACTUAL_STATE == SD_PRESENCE_STATE --> End Sequence
      { 0xFA,	0x8C,	0x09,	0x40},	// W#331 - 	Read MISC.VSET5_LB into RA for Num_Cycles_19M parameter_LSB
      { 0xFB,	0x8D,	0x09,	0x40},	// W#332 - 	Read MISC.VSET5_UB into RB for Num_Cycles_19M parameter_MSB
      { 0xFA,	0xFF,	0xFE,	0xC3},	// W#333 - 	Apply a Debounce of the same Num_Cycles_19M from RB:RA
      { 0x08,	0x48,	0x73,	0x00},	// W#334 - 	Clear Latched Trigger - Bit 3 (Again, in-case it got set again during above debounce-time)
      { 0xF6,	0x08,	0xC1,	0x40},	// W#335 - 	Read GPIO02.STATUS1 into R6 for updated-state of GPIO (High/Low)
      { 0xF5,	0xF6,	0x77,	0x22},	// W#336 - 	Update R7 = R6 + R5, Ignore Carry
      { 0x00,	0x71,	0x01,	0x91},	// W#337 - 	IF   R7<0> == 1'b0 ==> ACTUAL_STATE == SD_PRESENCE_STATE --> End Sequence
      { 0x00,	0x46,	0x41,	0x01},	// W#338 - 	Disable LDO02B
      { 0x00,	0x46,	0x44,	0x01},	// W#339 - 	Disable LDO05B
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#340 - 	
      { 0x01,	0x7D,	0xDE,	0x01},	// W#341 - 	PMICPR-636: Increase LDO Headroom to 200mV
      { 0x60,	0x61,	0xDE,	0x01},	// W#342 - 	PMICPR-636: PWM_CTRL_3 = 8CLK
      { 0x80,	0x01,	0xFE,	0xC1},	// W#343 - 	PMICPR-636: Wait 20us (0x180 = 384 x 19.2M pulses = 20us)
      { 0x78,	0x64,	0xDE,	0x01},	// W#344 - 	Variant-1: Enable Clamp, Full-Curr, 1.8V [PMICPR-1031: Mainlining as per 2017-07-10 update]
      { 0x00,	0x80,	0xFF,	0xC5},	// W#345 - 	Start timer for 1 second
      { 0x0E,	0xE5,	0xDE,	0x11},	// W#346 - 	Set DTEST4 to high_psrr_req output
      { 0xF4,	0x09,	0x09,	0x40},	// W#347 - 	read all DTESTs into R4. Bit 3 is DTEST4
      { 0x00,	0xE5,	0xDE,	0x11},	// W#348 - 	free DTEST4
      { 0x00,	0x41,	0x08,	0x90},	// W#349 - 	if bit 3 (DTEST4) is low continue with cool down
      { 0x54,	0x0B,	0xFF,	0x83},	// W#350 - 	otherwise start the whole process over
      { 0xF1,	0x40,	0x73,	0x40},	// W#351 - 	Read RE trigger config to determine what triggered this sequence
      { 0x40,	0x11,	0x40,	0x92},	// W#352 - 	if bit 6 trigger is set then this sequence was triggered after cool down so if is safe to end PSRR mitigation
      { 0x40,	0x00,	0xF1,	0x30},	// W#353 - 	bit 6 was clear. Set it to enable RE trigger
      { 0xF1,	0x40,	0x73,	0x08},	// W#354 - 	RE enabled for client2 bit 6
      { 0x99,	0x19,	0xFF,	0xC5},	// W#355 - 	if bit 6 trigger was cleared then this sequence was triggered by the Detection sequence and cool down needs to run.
      { 0x00,	0x7D,	0xDE,	0x01},	// W#356 - 	PMICPR-636: Decrease LDO Headroom to 100mV
      { 0x80,	0x01,	0xFE,	0xC1},	// W#357 - 	PMICPR-636: Wait 20us (0x180 = 384 x 19.2M pulses = 20us)
      { 0x74,	0x64,	0xDE,	0x01},	// W#358 - 	Enable Clamp, Full-Curr, 1.6V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#359 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x5C,	0x64,	0xDE,	0x01},	// W#360 - 	Enable Clamp, Half-Curr, 1.4V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#361 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x58,	0x64,	0xDE,	0x01},	// W#362 - 	Enable Clamp, Half-Curr, 1.2V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#363 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x54,	0x64,	0xDE,	0x01},	// W#364 - 	Enable Clamp, Half-Curr, 1.0V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#365 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x50,	0x64,	0xDE,	0x01},	// W#366 - 	Enable Clamp, Half-Curr, 0.8V
      { 0x80,	0x07,	0xFE,	0xC1},	// W#367 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x40,	0x64,	0xDE,	0x01},	// W#368 - 	Disable Clamp
      { 0x80,	0x07,	0xFE,	0xC1},	// W#369 - 	0x0780 = d'1920 x 19.2M cycles = 100 us
      { 0x20,	0x61,	0xDE,	0x01},	// W#370 - 	PMICPR-636: PWM_CTRL_3 = 2CLK
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#371 - 	
      { 0xF4,	0x50,	0x73,	0x40},	// W#372 - 	Read the PBS_CLIENT2.SCRATCH1 register written by SW
      { 0x01,	0x40,	0x01,	0x90},	// W#373 - 	Skip if Bit 0 not set
      { 0x2C,	0x0C,	0xFF,	0x83},	// W#374 - 	Scratch1 Bit-0 was Set, jump to OLEDB_SW_TRIG_CHANGE_PD_STATE
      { 0xFF,	0x51,	0x73,	0x00},	// W#375 - 	Only bit 0 is assigned. If here then there is an error. Set Scratch2 to 0xFF to indicate an error
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#376 - 	
      { 0x11,	0x11,	0x11,	0x11},	// W#377 - 	; S1, B2,B1 = voltage (not used), B0 = mode
      { 0x22,	0x22,	0x22,	0x22},	// W#378 - 	; S2, B2,B1 = voltage (not used), B0 = mode
      { 0x33,	0x33,	0x33,	0x33},	// W#379 - 	; S3  B2,B1 = voltage (not used), B0 = mode
      { 0x55,	0x55,	0x55,	0x55},	// W#380 - 	; S5 voltage, mode
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#381 - 	Buffer Separation (redundant)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#382 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#383 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#384 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#385 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#386 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#387 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#388 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#389 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#390 - 	| B3:POFF_REASON  | B2:ON_REASON      | B1:WARM_RESET_REASON1 | B0:PON_REASON1
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#391 - 	| B3:0xFF         | B2:FAULT_REASON2  | B1:FAULT_REASON1      | B0:OFF_REASON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#392 - 	Buffer Separation (redundant)
      { 0xED,	0x01,	0xA3,	0xFF},	// W#393 - 	Random Unique RAW DWord (tried to spell 'FF_AMOLED' in Hex)
      { 0xE0,	0x0C,	0xFF,	0x83},	// W#394 - 	NOP() or SWIRE_DECODE_SEQUENCE_LGE()
      { 0xB4,	0x0C,	0xFF,	0x83},	// W#395 - 	NOP()
      { 0xFF,	0xFF,	0xFF,	0xFB},	// W#396 - 	NOP()
      { 0x28,	0x41,	0xDC,	0x01},	// W#397 - 	Set IBB Voltage to -4.0V
      { 0x08,	0x49,	0xE0,	0x01},	// W#398 - 	Set OLEDB Voltage to 5.8V
      { 0x00,	0x41,	0xDE,	0x01},	// W#399 - 	Set LAB Voltage to 4.6V
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#400 - 	
      { 0xF6,	0x47,	0xDC,	0x41},	// W#401 - 	Load R6 with IBB.PD_CTL
      { 0x3F,	0x00,	0xF6,	0x2C},	// W#402 - 	AND R6 with 0x3F (b'0000_0011) => Clear Bit-7 (<6:3> absent)
      { 0xF6,	0x47,	0xDC,	0x09},	// W#403 - 	IBB.PD_CTL: ENABLE_PD = 0
      { 0xF6,	0xE9,	0xE0,	0x41},	// W#404 - 	PMICPR-714: Read existing value of OLEDB.TEST6 into R6
      { 0x80,	0x00,	0xF6,	0x30},	// W#405 - 	PMICPR-714: OR R6 with 0x80 (b'1000_0000) => Set Bit-7
      { 0xF6,	0xE9,	0xE0,	0x19},	// W#406 - 	PMICPR-714: Copy updated R6 value back to OLEDB.TEST6
      { 0xF6,	0x78,	0xDE,	0x41},	// W#407 - 	Load R6 with LAB.LDO_PD_CTL [PMICPR-971]
      { 0x7F,	0x00,	0xF6,	0x2C},	// W#408 - 	AND R6 with 0x7F (b'0111_1111) => Clear Bit-7
      { 0xF6,	0x78,	0xDE,	0x09},	// W#409 - 	LAB.LDO_PD_CTL: Disable PD [PMICPR-971]
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#410 - 	
      { 0xF6,	0x47,	0xDC,	0x41},	// W#411 - 	Load R6 with IBB.PD_CTL
      { 0x80,	0x00,	0xF6,	0x30},	// W#412 - 	OR R6 with 0x80 (b'1000_0000) => Set Bit-7
      { 0xF6,	0x47,	0xDC,	0x09},	// W#413 - 	IBB.PD_CTL: ENABLE_PD = 1
      { 0xF6,	0x4B,	0xE0,	0x41},	// W#414 - 	Load R6 with OLEDB.PD_CTL
      { 0x01,	0x00,	0xF6,	0x30},	// W#415 - 	OR R6 with 0x01 (b'0000_0001) => Set Bit-0
      { 0xF6,	0x4B,	0xE0,	0x09},	// W#416 - 	OLEDB.PD_CTL: Set PD_EN
      { 0xF6,	0x78,	0xDE,	0x41},	// W#417 - 	Load R6 with LAB.LDO_PD_CTL [PMICPR-971]
      { 0x80,	0x00,	0xF6,	0x30},	// W#418 - 	OR R6 with 0x80 (b'1000_0000) => Set Bit-7
      { 0xF6,	0x78,	0xDE,	0x09},	// W#419 - 	LAB.LDO_PD_CTL: Enable PD [PMICPR-971]
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#420 - 	
      { 0xF6,	0x5D,	0xDC,	0x41},	// W#421 - 	Load R6 with IBB.OUTPUT_SLEW_CTL
      { 0x40,	0x00,	0xF6,	0x30},	// W#422 - 	OR R6 with 0x40 (b'0100_0000) => Set Bit-6
      { 0xF6,	0x5D,	0xDC,	0x09},	// W#423 - 	Set IBB Slew Rate = FAST
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#424 - 	
      { 0xF6,	0x5D,	0xDC,	0x41},	// W#425 - 	Load R6 with IBB.OUTPUT_SLEW_CTL
      { 0xBF,	0x00,	0xF6,	0x2C},	// W#426 - 	AND R6 with 0xBF (b'1011_1111) => Clear Bit-6
      { 0xF6,	0x5D,	0xDC,	0x09},	// W#427 - 	Set IBB Slew Rate = SLOW
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#428 - 	
      { 0x24,	0x5A,	0xE0,	0x01},	// W#429 - 	1. 0xE05A = 0x24 // disable fast precharge
      { 0x1A,	0xE7,	0xE0,	0x11},	// W#430 - 	3. 0xE0E7 = 0x1A // disable slow precharge
      { 0x80,	0x46,	0xE0,	0x01},	// W#431 - 	4. 0xE046 > 0x80 // enable OLEDB module
      { 0x00,	0x4B,	0xE0,	0x01},	// W#432 - 	5. 0xE04B > 0x00 // disable the pulldown
      { 0x00,	0x46,	0xE0,	0x01},	// W#433 - 	6. 0xE046 > 0x00 // disable OLEDB module
      { 0xA4,	0x5A,	0xE0,	0x01},	// W#434 - 	7. 0xE05A = 0xA4 // enable fast precharge
      { 0x9A,	0xE7,	0xE0,	0x11},	// W#435 - 	9. 0xE0E7 = 0x9A // enable slow precharge
      { 0xF4,	0x51,	0x73,	0x40},	// W#436 - 	Read PBS_CLIENT2.SCRATCH2 & Store Value in PBSREG.R4
      { 0x01,	0x00,	0xF4,	0x30},	// W#437 - 	Set Bit-0 to High (leaving other bits intact)
      { 0xF4,	0x51,	0x73,	0x08},	// W#438 - 	Copy PBSREG.R4 to in PBS_CLIENT2.SCRATCH2 effectively just setting Bit-0
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#439 - 	
      { 0x01,	0x0B,	0xDC,	0x01},	// W#440 - 	Write any value into Status4 to tranfer edge count into Status4 register
      { 0xF4,	0x0B,	0xDC,	0x41},	// W#441 - 	Read SWIRE command data into R4. Could be up to d'57 for SS [Mak]
      { 0x00,	0x40,	0xFF,	0x90},	// W#442 - 	Jump to COUNT_LE_41_CHECK if SWIRE_EDGE_COUNT > 0
      { 0x34,	0x0C,	0xFF,	0x83},	// W#443 - 	
      { 0x29,	0x45,	0xFF,	0x93},	// W#444 - 	Jump to COUNT_LE_49_CHECK if SWIRE_EDGE_COUNT > 41
      { 0x37,	0x00,	0x76,	0x28},	// W#445 - 	Load R6 with Constant 0x37 (d'55)
      { 0xF6,	0xF4,	0x74,	0x26},	// W#446 - 	R4 = R6 (d'55) - R4 (SWIRE_EDGE_COUNT {1:41})
      { 0xF4,	0x41,	0xDC,	0x09},	// W#447 - 	Set IBB Voltage
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#448 - 	
      { 0x31,	0x45,	0xFF,	0x96},	// W#449 - 	Jump to COUNT_EQ_50_CHECK if SWIRE_EDGE_COUNT > 49
      { 0x2A,	0x00,	0x74,	0x24},	// W#450 - 	R4 = R4 (SWIRE_EDGE_COUNT {42:49}) - 0x2A (d'42)
      { 0xF4,	0xF4,	0x76,	0x22},	// W#451 - 	R6 = R4 + R4 = 2 x R4
      { 0xF4,	0xF6,	0x76,	0x22},	// W#452 - 	R6 = R4 + R6 = 3 x R4
      { 0x1D,	0x00,	0x74,	0x28},	// W#453 - 	Load R4 with Constant 0x1D (d'29 = d'79 - d'50)
      { 0xF4,	0xF6,	0x74,	0x26},	// W#454 - 	R4 = R4 (d'29) - R6
      { 0xF4,	0x49,	0xE0,	0x09},	// W#455 - 	Set OLEDB Voltage
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#456 - 	
      { 0x32,	0x45,	0xFF,	0x90},	// W#457 - 	Jump to COUNT_EQ_51_CHECK if SWIRE_EDGE_COUNT > 50
      { 0x6C,	0x0C,	0xFF,	0x83},	// W#458 - 	
      { 0x33,	0x45,	0xFF,	0x90},	// W#459 - 	Jump to COUNT_EQ_52_CHECK if SWIRE_EDGE_COUNT > 51
      { 0x44,	0x0C,	0xFF,	0x83},	// W#460 - 	
      { 0x34,	0x45,	0xFF,	0x90},	// W#461 - 	Jump to COUNT_EQ_53_CHECK if SWIRE_EDGE_COUNT > 52
      { 0x94,	0x0C,	0xFF,	0x83},	// W#462 - 	
      { 0x35,	0x45,	0xFF,	0x90},	// W#463 - 	Jump to COUNT_LE_57_CHECK if SWIRE_EDGE_COUNT > 53
      { 0xA4,	0x0C,	0xFF,	0x83},	// W#464 - 	
      { 0x39,	0x45,	0xFF,	0x92},	// W#465 - 	Jump to COUNT_CHECK_DONE if SWIRE_EDGE_COUNT > 57
      { 0x35,	0x00,	0x76,	0x28},	// W#466 - 	Load R6 with Constant 0x35 (d'53)
      { 0xF4,	0xF6,	0x74,	0x26},	// W#467 - 	R4 = R4 (SWIRE_EDGE_COUNT {54:57}) - R6 (d'53)
      { 0xF4,	0x41,	0xDE,	0x09},	// W#468 - 	Set LAB Voltage
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#469 - 	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#470 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#471 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#472 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#473 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#474 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#475 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#476 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#477 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#478 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#479 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#480 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#481 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#482 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#483 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#484 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#485 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#486 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#487 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#488 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#489 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#490 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#491 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#492 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#493 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#494 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#495 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#496 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#497 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#498 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#499 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#500 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#501 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#502 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#503 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#504 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#505 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#506 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#507 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#508 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#509 - 	Inserted by compiler.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#510 - 	Inserted by compiler.
      { 0x19,	0x15,	0xFF,	0xF8},	// W#511 - 	Inserted by compiler.
   },
};
