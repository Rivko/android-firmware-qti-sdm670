#!/usr/bin/env python

CLOCK_FLAGS = {
  'gpll0': ['STUB_HW_RUMI'],
  'core_clk_src': ['SUPPRESSIBLE'],
  'lpaif_spkr_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_pri_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_sec_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_ter_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_quad_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'lpaif_pcmoe_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'ext_mclk0_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'ext_mclk1_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'ext_mclk2_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'int_i2s0_1_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'int_i2s2_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'int_i2s3_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'int_i2s4_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'int_i2s5_6_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'int_mclk0_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'int_mclk1_clk_src': ['DOMAIN_WAIT_FOR_ROOT_OFF'],
  'qdsp6ss_pll': ['SUPPORTS_SLEWING'],
  'scc_main_rcg_clk_src': ['SUPPRESSIBLE'],
  'qdsp6ss_rcg_clk_src': ['SUPPRESSIBLE'],
  'scc_qupv3_se0_clk_src': ['SUPPRESSIBLE','DOMAIN_WAIT_FOR_ROOT_OFF'],
  'scc_qupv3_se1_clk_src': ['SUPPRESSIBLE','DOMAIN_WAIT_FOR_ROOT_OFF'],
  'scc_qupv3_se2_clk_src': ['SUPPRESSIBLE','DOMAIN_WAIT_FOR_ROOT_OFF'],
  'scc_qupv3_se3_clk_src': ['SUPPRESSIBLE','DOMAIN_WAIT_FOR_ROOT_OFF'],
  'scc_qupv3_se4_clk_src': ['SUPPRESSIBLE','DOMAIN_WAIT_FOR_ROOT_OFF'],
  'scc_qupv3_se5_clk_src': ['SUPPRESSIBLE','DOMAIN_WAIT_FOR_ROOT_OFF'],
  'gcc_aggre_noc_north_sf_clk_src' :  ['SUPPRESSIBLE']
}


HAL_CONTROL = {
  'audio_core_gdsc': 'HAL_clk_AudioCorePowerDomainControl'
}

  
DEBUG_MUX_OVERRIDE = {
   'LPASS_AON_Q6': { 'inputs': {'q6_core_clk': 0x1}},
}



DB_OVERRIDE = {
  'LPASS_CC': {
    'cbc': {
      'lpass_audio_core_lpaif_codec_spkr_ebit_clk': {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_pri_ebit_clk':        {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_quad_ebit_clk':       {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_sec_ebit_clk':        {'l_master': ['clockdrivergen_ebit_clk_src'],},
      'lpass_audio_core_lpaif_ter_ebit_clk':        {'l_master': ['clockdrivergen_ebit_clk_src'],},
	  'q6_core_clk': {'l_master':['qdsp6ss_rcg_clk_src']},
    },
    'rcg': {
     'qdsp6ss_rcg_clk_src': {'d_registers': {'cmd_rcgr_name': 'SSC_QDSP6SS_CORE_CMD_RCGR'}},
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
  'chip_aliases': ['napaliq_1.0'],
  'chip_release': {
   'napaliq_1.0': 'napaliq_v1.0_p3q2r97',
 },
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
		'include_domain_clock': ['scc_qupv3_se.*_clk_src'],
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

