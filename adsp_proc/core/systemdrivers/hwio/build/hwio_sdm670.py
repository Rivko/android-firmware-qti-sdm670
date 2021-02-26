#!/usr/bin/env python
#===========================================================================
#
#  @file hwio_sdm670.py
#  @brief HWIO config file for the HWIO generation scripts for SDM670.
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_sdm670.py --flat=..\..\..\api\systemdrivers\hwio\sdm670\ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/hwio/build/hwio_sdm670.py#2 $
#  $DateTime: 2018/05/16 04:01:11 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'sdm670'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  # General bases
  'AOP_SS_MSG_RAM_START_ADDRESS',
  'AGGRE_NOC_AGGRE_NOC',
  'AOSS',
  'CLK_CTL',
  'CONFIG_NOC_WRAPPER',
  'CORE_TOP_CSR',
  'DDR_SS',
  'PERIPH_SS_AHB2PHY_NORTH',
  'PERIPH_SS_PDM_PERPH_WEB',
  'PERIPH_SS_SDC2_SDCC5_TOP',
  'PERIPH_SS_SDC4_SDCC5_TOP',
  'PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER',
  'QDSS_SOC_DBG',
  'QUPV3_0_QUPV3_ID_1',
  'QUPV3_1_QUPV3_ID_1',
  'SECURITY_CONTROL',
  'SPDM_WRAPPER_TOP',
  'SYSTEM_NOC',
  'TLMM',
  'USB3_PRI_USB30_PRIM',
  'PRNG_CFG_PRNG_TOP',
  
  # ADSP/LPASS bases
  'LPASS',

  # CDSP/Turing bases
  'TURING',
  'TURING_L2_CFG',

]

base_resize = {
  'AOP_SS_MSG_RAM_START_ADDRESS':   0x100000,
  'TURING':                         0x3C1000,
  'QUPV3_0_QUPV3_ID_1':             0xC7000,
  'QUPV3_1_QUPV3_ID_1':             0xC7000,
  'PERIPH_SS_AHB2PHY_NORTH':        0x10000,
}

HWIO_BASE_FILES = [
  {
    'filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwiobase.h',
    'bases': bases,
    'map-type': 'qurt',
    'virtual-address-start': 0xE0000000,
    'virtual-address-end': 0xF0000000,
    'resize': base_resize,
    'qurt-memsection-filename': '../../../api/systemdrivers/hwio/' + CHIPSET + '/msmhwioplat.xml',
    'default-cache-policy': 'uncached',
    'nomap-regions' : [{'address' : 0x082d2000, 'size' : 0xe000}], # Alias to VAPSS_NOC
    'devcfg-filename': '../../../settings/systemdrivers/hwio/config/' + CHIPSET + '/HWIOBaseMap.c',
    'check-sizes': True,
    'check-for-overlaps': True,
    
    # We need to manually map the LPASS region because the LPM region
    # which is right in the middle of LPASS needs special cache
    # attributes.  This is handled in the top-level qdsp6.xml file.
    'fixed-virtual-address': { 'LPASS': 0xEE000000 },
    'skip-memsection': ['LPASS'],
	'cache-policy': {'LPASS_L2_CFG': 'uncached'},
	'cache-policy': {'TURING_L2_CFG': 'uncached'},
  }
]

HWIO_EXTRA_BASES = [
    ['TURING_L2_CFG_BASE', 0x085a0000, 0x1000],
]


# ============================================================================
# HWIO_REGISTER_FILES
# ============================================================================

HWIO_REGISTER_FILES = [
  {
    'filename': '../hw/' + CHIPSET + '/msmhwioreg.h.ref',
    'bases': bases,
    'filter-exclude': ['RESERVED', 'DUMMY'],
    'header': '''
#error This file is for reference only and cannot be included.  See "go/hwio" or mail corebsp.sysdrv.hwio for help.
'''
  }
]


# ============================================================================
# HWIO_T32VIEW_FILES
# ============================================================================

HWIO_T32VIEW_FILES = [
  {
    'symbol-filename': '../scripts/' + CHIPSET + '/hwio.cmm',
    'limit-array-size': [ 10, 4 ],
    'per-filename': '../scripts/' + CHIPSET + '/hwioreg',
    'filter-exclude': ['RESERVED', 'DUMMY']
  },
]


# ============================================================================
# Main
#
# Entry point when invoking this directly.
# ============================================================================

if __name__ == "__main__":
  import sys, os
  from subprocess import call

  flat_path = '../../../api/systemdrivers/hwio/{}/ARM_ADDRESS_FILE.FLAT'.format(CHIPSET)
  if os.name == 'nt':
    hwio_path = r'\\ben\corebsp_labdata_0001\sysdrv\hwio\HWIOGen.py'
  else:
    hwio_path = r'/net/ben/vol/eng_asw_labdata_0001/corebsp_labdata_0001/sysdrv/hwio/HWIOGen.py'

  call(['python', hwio_path, '--cfg', __file__, '--flat', flat_path] + sys.argv[1:])

