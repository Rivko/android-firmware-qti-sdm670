#!/usr/bin/env python

CLOCK_FLAGS = {
  'gpll0': ['STUB_HW_RUMI'],
  'lpass_qdsp6ss_pll': ['SUPPORTS_SLEWING', 'SUPPRESSIBLE'],
  'aon_clk_src': ['SUPPRESSIBLE'],
  'core_clk_src': ['SUPPRESSIBLE'],
  'lpass_qdsp6ss_core_clk_src': ['SUPPRESSIBLE'],
  'lpaif_spkr_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_pri_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_sec_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_ter_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_quad_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_pcmoe_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'ext_mclk0_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'ext_mclk1_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'ext_mclk2_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
}


HAL_CONTROL = {
}

  
DEBUG_MUX_OVERRIDE = {
  'LPASS_Q6': { 'inputs': {'lpass_qdsp6ss_core_clk': 0x1}},
}



DB_OVERRIDE = {
  'LPASS_CC': {
    'cbc': {
      'lpass_qdsp6ss_core_clk': {'l_master':['lpass_qdsp6ss_core_clk_src']},
      'lpass_audio_core_lpaif_codec_spkr_ebit_clk': {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_pri_ebit_clk':        {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_quad_ebit_clk':       {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_sec_ebit_clk':        {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_ter_ebit_clk':        {'l_master': ['clockdrivergen_ebit_clk_src'],},
    },
    'rcg': {
      'lpass_qdsp6ss_core_clk_src': {'d_registers': {'cmd_rcgr_name': 'LPASS_QDSP6SS_CORE_CMD_RCGR'}},
      'clockdrivergen_ebit_clk_src': {
        "d_freq": {
          "MINSVS_FREQ": "3.072000",
          "LOWSVS_FREQ": "6.144000",
          "SVS_FREQ":    "12.288000",
          "SVS_L1_FREQ": "12.288000",
          "NOM_FREQ":    "24.576000",
          "TURBO_FREQ":  "24.576000"
        },
      },
    }
  },
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
        'include_fmax': ['clockdrivergen_ebit_clk_src'], 
        'env': 'lpass',
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

