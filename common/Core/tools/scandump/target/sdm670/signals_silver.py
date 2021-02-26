"""
===============================================================================

 SDM845.py

 GENERAL DESCRIPTION
    Target specific configuration data


 Copyright (c) 2015 - 2016 by QUALCOMM Technologies Incorporated.
 All Rights Reserved.

 QUALCOMM Proprietary

-------------------------------------------------------------------------------

  $Header: //components/rel/core.glue/1.2/tools/scandump/target/sdm670/signals_silver.py#1 $
  $DateTime: 2017/09/18 05:13:28 $
  $Author: pwbldsvc $
  $Change: 14401733 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when       who         what, where, why
 --------   ---         ---------------------------------------------------------
 12/12/2015 AJCheriyan  Created for Nazgul
==============================================================================
"""
import os, re

# ARMv8_registermap filter

# This is the mapping of the registers to the register pointers or renaming registers
def register_formatter(value):
    return str(int(value, 2)).zfill(2)

aarch64_map = {
        'X0' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register00_lo']},
        'X1' : { 'val' : '', 'formatter' : register_formatter,  'len' : '64', 'regs' : [ 'phy_register01_lo']},
        'X2' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register02_lo']},
        'X3' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register03_lo']},
        'X4' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register04_lo']},
        'X5' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register05_lo']},
        'X6' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register06_lo']},
        'X7' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register07_lo']},
        'X8' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register08_lo']},
        'X9' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register09_lo']},
        'X10' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register10_lo']},
        'X11' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register11_lo']},
        'X12' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register12_lo']},
        'X13' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register13_lo']},
        'X14' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register14_lo']},
        'X15' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register15_lo']},
        'X16' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register16_lo']},
        'X17' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register17_lo']},
        'X18' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register18_lo']},
        'X19' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register19_lo']},
        'X20' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register20_lo']},
        'X21' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register21_lo']},
        'X22' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register22_lo']},
        'X23' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register23_lo']},
        'X24' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register24_lo']},
        'X25' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register25_lo']},
        'X26' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register26_lo']},
        'X27' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register27_lo']},
        'X28' : { 'val' : '','formatter' : register_formatter,  'len' : '64', 'regs' : [ 'phy_register28_lo']},
        'X29' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register29_lo']},
        'X30' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register30_lo']},
        'SP_EL0' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register31_lo']},
        'SP_EL1' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register32_lo']},
        'SP_EL2' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register33_lo']},
        'SP_EL3' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register34_lo']},
        'ELR_EL1'  : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register35_lo']},
        'ELR_EL2'  : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register36_lo']},
        'ELR_EL3'  : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'phy_register37_lo']},
        'PC' : { 'val' : '', 'formatter' : register_formatter, 'len' : '64', 'regs' : [ 'current_instr_pc']},
}

aarch32_map = {
        'R0' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R1' : { 'val' : '', 'formatter' : register_formatter,  'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R2' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R3' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R4' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R5' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R6' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R7' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R8' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R9' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R10' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R11' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R12' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_HYP' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_IRQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_IRQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_SVC' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_SVC' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_ABT' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_SVC' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_SVC' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_ABT' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_ABT' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_UND' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_UND' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R8_FIQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R9_FIQ' : { 'val' : '','formatter' : register_formatter,  'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R10_FIQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R11_FIQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R12_FIQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_FIQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_FIQ' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R13_MON' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'R14_MON' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'RES'  : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'RES'  : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'RES'  : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'RES'  : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
        'PC' : { 'val' : '', 'formatter' : register_formatter, 'len' : '32', 'regs' : ['phy_register%s_lo']},
}

# This is the list of signals to be modified when the scancell format changes
input_signals = {
        'CPUID'           : 'coreid_reg',
        # Renaming register set

        # Physical register set
        'phy_register00_lo' : 'inf00_reg',
        'phy_register01_lo' : 'inf01_reg',
        'phy_register02_lo' : 'inf02_reg',
        'phy_register03_lo' : 'inf03_reg',
        'phy_register04_lo' : 'inf04_reg',
        'phy_register05_lo' : 'inf05_reg',
        'phy_register06_lo' : 'inf06_reg',
        'phy_register07_lo' : 'inf07_reg',
        'phy_register08_lo' : 'inf08_reg',
        'phy_register09_lo' : 'inf09_reg',
        'phy_register10_lo' : 'inf10_reg',
        'phy_register11_lo' : 'inf11_reg',
        'phy_register12_lo' : 'inf12_reg',
        'phy_register13_lo' : 'inf13_reg',
        'phy_register14_lo' : 'inf14_reg',
        'phy_register15_lo' : 'inf15_reg',
        'phy_register16_lo' : 'inf16_reg',
        'phy_register17_lo' : 'inf17_reg',
        'phy_register18_lo' : 'inf18_reg',
        'phy_register19_lo' : 'inf19_reg',
        'phy_register20_lo' : 'inf20_reg',
        'phy_register21_lo' : 'inf21_reg',
        'phy_register22_lo' : 'inf22_reg',
        'phy_register23_lo' : 'inf23_reg',
        'phy_register24_lo' : 'inf24_reg',
        'phy_register25_lo' : 'inf25_reg',
        'phy_register26_lo' : 'inf26_reg',
        'phy_register27_lo' : 'inf27_reg',
        'phy_register28_lo' : 'inf28_reg',
        'phy_register29_lo' : 'inf29_reg',
        'phy_register30_lo' : 'inf30_reg',
        'phy_register31_lo' : 'inf31_reg',
        'phy_register32_lo' : 'inf32_reg',
        'phy_register33_lo' : 'inf33_reg',
        'phy_register34_lo' : 'inf34_reg',
        'phy_register35_lo' : 'inf35_reg',
        'phy_register36_lo' : 'inf36_reg',
        'phy_register37_lo' : 'inf37_reg',
        'current_instr_pc'  : 'pc_reg',
}


