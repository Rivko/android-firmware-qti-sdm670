#!/usr/bin/env python
#===========================================================================
#
#  @file hwio_sdm855.py
#  @brief HWIO config file for the HWIO generation scripts for SDM855.
#
#  This file can be invoked by calling:
#
#    HWIOGen.py --cfg=hwio_sdm845.py --flat=..\..\..\api\systemdrivers\hwio\sdm845\ARM_ADDRESS_FILE.FLAT
#
#  ===========================================================================
#
#  Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
#
#  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/hwio/build/hwio_sdm855.py#1 $
#  $DateTime: 2017/07/21 22:10:47 $
#  $Author: pwbldsvc $
#
#  ===========================================================================

CHIPSET = 'sdm855'

# ============================================================================
# HWIO_BASE_FILES
# ============================================================================

bases = [
  # General bases
  'AOP_SS_MSG_RAM_START_ADDRESS',
  'SPDM_WRAPPER_TOP',
  'PERIPH_SS_PDM_PERPH_WEB',
  'PERIPH_SS_SDC2_SDCC5_TOP',
  'PERIPH_SS_SDC4_SDCC5_TOP',
  'PERIPH_SS_AHB2PHY_NORTH',
  'PERIPH_SS_TSIF_TSIF_12SEG_WRAPPER',
  'QDSS_SOC_DBG',
  'CLK_CTL',
  'CONFIG_NOC_WRAPPER',
  'CORE_TOP_CSR',
  'SECURITY_CONTROL',
  'SYSTEM_NOC',
  'TLMM',
  'USB3_PRI_USB30_PRIM',
  'TURING',
  'SSC',
  'AGGRE_NOC_AGGRE_NOC',
  'DDR_SS',
  'AOSS',
  'QUPV3_0_QUPV3_ID_1',
  #'QUPV3_1_QUPV3_ID_1',
  #'LPASS_L2_CFG',
  #'TURING_L2_CFG',
  'PRNG_CFG_PRNG_TOP',
  # LPASS bases
  'LPASS',
]

base_resize = {
  'AOP_SS_MSG_RAM_START_ADDRESS':   0x100000,
  'QUPV3_0_QUPV3_ID_1':             0xC7000,
  #'QUPV3_1_QUPV3_ID_1':             0xC7000,
  'LPASS':                          0x3B1000,
  'TURING':                         0x3C1000,
  'PERIPH_SS_AHB2PHY_NORTH':        0x10000,
  'CONFIG_NOC_WRAPPER':             0x28000,
  'AGGRE_NOC_AGGRE_NOC':            0x5C000,
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
	'cache-policy': {'SSC_L2_CFG': 'uncached'},
  }
]

HWIO_EXTRA_BASES = [
    ['LPASS_L2_CFG_BASE',  0x175a0000, 0x1000],
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

