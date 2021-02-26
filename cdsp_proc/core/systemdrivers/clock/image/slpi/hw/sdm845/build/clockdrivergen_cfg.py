#!/usr/bin/env python

CLOCK_FLAGS = {
  'gpll0': ['STUB_HW_RUMI'],
  'qdsp6ss_pll': ['SUPPORTS_SLEWING'],
  'scc_main_rcg_clk_src': ['SUPPRESSIBLE'],
  'qdsp6ss_rcg_clk_src': ['SUPPRESSIBLE'],
}

DB_OVERRIDE = {
  'SCC': {
    'cbc': {
	  'q6_core_clk': {'l_master':['qdsp6ss_rcg_clk_src']},
	},
    'rcg': {
       'qdsp6ss_rcg_clk_src': {'d_registers': {'cmd_rcgr_name': 'SSC_QDSP6SS_CORE_CMD_RCGR'}},
    },
  },
}

HAL_CONTROL = {

}

  
DEBUG_MUX_OVERRIDE = {
  'SCC_Q6': { 'inputs': {'q6_core_clk': 0x1}},
}



CLOCKDRIVERGEN_CONFIG = {
  'target': 'sdm845',
  'filter_warning': 'sw',
  'db_override': DB_OVERRIDE,
  'debug_mux_override': DEBUG_MUX_OVERRIDE,

  'generators': [{
    'type': 'dalclock',
    'output_dir': '..',
    'options': {
      'env': 'scc',
      'clock_flags': CLOCK_FLAGS,
      'hal_ctl_structs': HAL_CONTROL,
      'driver_flags': ['LOG_CLOCK_FREQ_CHANGE', 'LOG_SOURCE_STATE_CHANGE'],

      'include_domain_clock': ['scc_qupv3_se.*_clk_src'],
      'npa_controlled_sources': {
        'aoss_cc_ro_clk': None,
      },
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

