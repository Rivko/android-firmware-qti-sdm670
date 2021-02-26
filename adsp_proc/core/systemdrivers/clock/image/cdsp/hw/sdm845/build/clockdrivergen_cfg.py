#!/usr/bin/env python

CLOCK_FLAGS = {
  'gpll0': ['STUB_HW_RUMI'],
  'gpll1': ['STUB_HW_RUMI'],
  'turing_qdsp6ss_pll': ['SUPPORTS_SLEWING'],
  'aon_clk_src': ['SUPPRESSIBLE'],
  'vap_tcms_clk_src': ['SUPPRESSIBLE'],
  'turing_qdsp6ss_core_clk_src': ['SUPPRESSIBLE']
}


HAL_CONTROL = {
}


DB_OVERRIDE = {
  'TURING_CC': {
    'cbc': {
      'turing_qdsp6ss_core_clk': {'l_master':['turing_qdsp6ss_core_clk_src']}
    },
	'rcg': {
      'turing_qdsp6ss_core_clk_src': {'d_registers': {'cmd_rcgr_name': 'TURING_QDSP6SS_CORE_CMD_RCGR'}},
    }
  }
}

  
DEBUG_MUX_OVERRIDE = {
  'TURING_CC_Q6': { 'inputs': {'turing_qdsp6ss_core_clk': 0x1}},
}

HAL_CONTROL = {
  'vapss_gdsc': 'HAL_clk_TuringPowerDomainCtrl'
}

CLOCKDRIVERGEN_CONFIG = {
  'target': 'sdm845',
  'filter_warning': 'sw',
  'db_override': DB_OVERRIDE,
  'debug_mux_override': DEBUG_MUX_OVERRIDE,
  'hal_ctl_structs': HAL_CONTROL,

  'generators': [{
      'type': 'dalclock',
      'output_dir': '..',
      'options': {
        'env': 'css',
        'clock_flags': CLOCK_FLAGS,
        'hal_ctl_structs': HAL_CONTROL,
        'driver_flags': ['LOG_CLOCK_FREQ_CHANGE', 'LOG_SOURCE_STATE_CHANGE'],
      },
  }],
}


if __name__ == "__main__":
  import sys, os
  from subprocess import call

  if os.name == 'nt':
    clockdrivergen_path = r'\\ben\corebsp_labdata_0001\sysdrv\clockdrivergen\bin\clockdrivergen.py'
  else:
    clockdrivergen_path = r'/net/ben/vol/eng_asw_labdata_0001/corebsp_labdata_0001/sysdrv/clockdrivergen/bin/clockdrivergen.py'

  call(['python', clockdrivergen_path, '--cfg', __file__] + sys.argv[1:])

