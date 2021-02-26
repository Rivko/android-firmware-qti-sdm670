#  ===========================================================================
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================

targ_spec_config = {
    '670' : {
                'arc' : {
                            'no_rm'  : 9,
                            'no_drv' : 30,
                            'vt_base_address'  : 0x0B600000,
                            'rm_enb_base_addr' : 0x0B7E0000,
                            'rm_ol_base_addr'  : 0x0B7E00C0,  
                            'rm_seq_base_addr' : 0x0B7E4500, 
                            'rm' : {
                                    0: 'CX',
                                    1: 'MX',
                                    2: 'EBI',
                                    3: 'LPI_CX',
                                    4: 'LPI_MX',
                                    5: 'GFX',
                                    6: 'MSS',
                                    7: 'DDR_SS',
                                    8: 'XO',
                                   },
                        },
                'drv' : {
                            0 : 'TZ',
                            1 : 'HYP',
                            2 : 'APPS',
                            3 : 'SEC_PROC',
                            4 : 'LPASS',
                            5 : 'SENSOR',
                            6 : 'AOP',
                            7 : 'DEBUG',
                            8 : 'GPU',
                            9 : 'Display',
                            10 : 'CDSP',
                            11 : 'MSS',
                            12 : 'MSS_HW',
                            13 : 'CPRF',
                            14 : 'BCM_CD0',
                            15 : 'BCM_CD1',
                            16 : 'BCM_CD2',
                            17 : 'BCM_CD3',
                            18 : 'BCM_CD4',
                            19 : 'BCM_CD5',
                            20 : 'BCM_CD6',
                            21 : 'BCM_CD7',
                            22 : 'BCM_CD8',
                            23 : 'BCM_CD9',
                            24 : 'BCM_CD10',
                            25 : 'BCM_CD11',
                            26 : 'BCM_CD12',
                            27 : 'BCM_CD13',
                            28 : 'BCM_CD14',
                            29 : 'BCM_CD15',
                        },
            },
}

vrm = {
	'670' :{ 
				'vt_base_address'    : 0x0B800000, 
				'vt_en_base_address' : 0x0B800004,
				'mode_vote'          : 0x0B800008,
				'headroom_vt'        : 0x0B80000C,
				'volt_csr'           : 0x0B9E0000,
				'enb_csr'            : 0x0B9E0004,
				'mode_csr'           : 0x0B9E0008,
				'headroom_csr'       : 0x0B9E000C,
				'volt_wsr'           : 0x0B9E0020,
				'enb_wsr'            : 0x0B9E0024,
				'mode_wsr'           : 0x0B9E0028,
				'headroom_wsr'       : 0x0B9E002C,
				'volt_dsr'           : 0x0B9E0040,
				'enb_dsr'            : 0x0B9E0044,
				'mode_dsr'           : 0x0B9E0048,
				'headroom_dsr'       : 0x0B9E004C,
                'sequence_csr'       : 0x0B9F2000,
                'status_csr'         : 0x0B9F2100,
				'no_regs' : 46,
				'no_drv'  : 14,
                'no_seq'  : 32,
			},
}

cprf = {
	'670' :{ 
				'hw_ver_addr'         : 0x0C200000 ,
				'aoss_setting_addr'   : 0x0C200004 ,
				'cprf_cntrl_addr'     : 0x0C201004 ,
				'cprf_vrm_addr'       : 0x0C201008 ,
				'volt_limits_addr'    : 0x0C20100C ,
				'volt_safe_addr'      : 0x0C20108C ,
				'enable_addr'         : 0x0C201118 ,
				'values_addr'         : 0x0C201230 ,
				'value_1_addr'        : 0x0C201244 ,
				'value_2_addr'        : 0x0C2012D0 ,
				'value_3_addr'        : 0x0C201350 ,
				'value_4_addr'        : 0x0C2013D0 ,
				'value_5_addr'        : 0x0C201450 ,
				'no_cprf': 7,
				'no_corners': 16,

			},
}

pdc_ss_list = {
	'670' :[
	"apps",  "audio", "sensors", "aop", "debug", "gpu", "display", "compute", "modem"
],
}	

pdc_global = {
	'670' :{ 
				'pdc_apps_baseaddr'		: 0x0B2E0020, 
				'pdc_sp_baseaddr'		: 0x0B2E0050,
				'pdc_audio_baseaddr'	: 0x0B2E0060,
				'pdc_sensors_baseaddr'	: 0x0B2E0070,
				'pdc_aop_baseaddr'		: 0x0B2E0080,
				'pdc_debug_baseaddr'	: 0x0B2E0090,
				'pdc_gpu_baseaddr'		: 0x0B2E00A0,
				'pdc_display_baseaddr'	: 0x0B2E00B0,
				'pdc_compute_baseaddr'	: 0x0B2E00C0,
				'pdc_modem_baseaddr'	: 0x0B2E00D0,
				
				
			},
}


pdc_apps={
	'670' :{ 
				'no_drv'							: 3,
				'no_bank'							: 4,
				'no_units'							: 5,
				'no_irq'				   			: 126,
				'irq_enable_bank_baseaddr'			: 0x0B200010, 
				'irq_cfg_baseaddr'					: 0x0B200110, 
				'irq_status_bank_baseaddr'			: 0x0B200900, 
				'pdc_version_drv_baseaddr'			: 0x0B201000,
				'pdc_mode_status_baseaddr'			: 0x0B201030, 
				'enable_pdc_baseaddr'				: 0x0B204500,
				'timer_match_value_lo_baseaddr'		: 0x0B204510,
				'timer_match_value_hi_baseaddr'		: 0x0B204514,
				'seq_busy_drv_baseaddr'				: 0x0B201020,
				'profiling_output_baseaddr'			: 0x0B201200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B201204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B201208,
				
				
			},
}
pdc_sp={
	'670' :{ 
				'no_drv'							: 1,
				'no_bank'							: 1,
				'no_units'							: 5,
				'no_irq'							: 20,
				'irq_enable_bank_baseaddr'			: 0x0B230010, 
				'irq_cfg_baseaddr'					: 0x0B230110, 
				'irq_status_bank_baseaddr'			: 0x0B230900, 
				'pdc_version_drv_baseaddr'			: 0x0B231000,
				'pdc_mode_status_baseaddr'			: 0x0B231030, 
				'enable_pdc_baseaddr'				: 0x0B234500,
				'timer_match_value_lo_baseaddr'		: 0x0B234510,
				'timer_match_value_hi_baseaddr'		: 0x0B234514,
				'seq_busy_drv_baseaddr'				: 0x0B231020,
				'profiling_output_baseaddr'			: 0x0B231200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B231204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B231208,
				
			},
}
pdc_audio={
	'670' :{ 
				'no_drv'							: 1,
				'no_bank'							: 1,
				'no_units'							: 5,
				'no_irq'							: 20,
				'irq_enable_bank_baseaddr'			: 0x0B240010, 
				'irq_cfg_baseaddr'					: 0x0B240110, 
				'irq_status_bank_baseaddr'			: 0x0B240900, 
				'pdc_version_drv_baseaddr'			: 0x0B241000,
				'pdc_mode_status_baseaddr'			: 0x0B241030, 
				'enable_pdc_baseaddr'				: 0x0B244500,
				'timer_match_value_lo_baseaddr'		: 0x0B244510,
				'timer_match_value_hi_baseaddr'		: 0x0B244514,
				'seq_busy_drv_baseaddr'				: 0x0B241020,
				'profiling_output_baseaddr'			: 0x0B241200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B241204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B241208,
				
			},
}
pdc_sensors={
	'670' :{ 
				'no_drv'							: 1,
				'no_bank'							: 3,
				'no_units'							: 5,
				'no_irq'							: 75,
				'irq_enable_bank_baseaddr'			: 0x0B250010, 
				'irq_cfg_baseaddr'					: 0x0B250110, 
				'irq_status_bank_baseaddr'			: 0x0B250900, 
				'pdc_version_drv_baseaddr'			: 0x0B251000,
				'pdc_mode_status_baseaddr'			: 0x0B251030, 
				'enable_pdc_baseaddr'				: 0x0B254500,
				'timer_match_value_lo_baseaddr'		: 0x0B254510,
				'timer_match_value_hi_baseaddr'		: 0x0B254514,
				'seq_busy_drv_baseaddr'				: 0x0B251020,
				'profiling_output_baseaddr'			: 0x0B251200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B251204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B251208,
				
			},
}

pdc_aop={
	'670' :{ 
				'no_drv'							: 1,
				'no_bank'							: 1,
				'no_units'							: 5,
				'no_irq'							: 25,
				'irq_enable_bank_baseaddr'			: 0x0B260010, 
				'irq_cfg_baseaddr'					: 0x0B260110, 
				'irq_status_bank_baseaddr'			: 0x0B260900, 
				'pdc_version_drv_baseaddr'			: 0x0B261000,
				'pdc_mode_status_baseaddr'			: 0x0B261030, 
				'enable_pdc_baseaddr'				: 0x0B264500,
				'timer_match_value_lo_baseaddr'		: 0x0B264510,
				'timer_match_value_hi_baseaddr'		: 0x0B264514,
				'seq_busy_drv_baseaddr'				: 0x0B261020,
				'profiling_output_baseaddr'			: 0x0B261200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B261204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B261208,
				
			},
}
pdc_debug={
	'670' :{ 
				'no_drv'							: 1,
				'no_bank'							: 1,
				'no_irq'							: 1,
				'irq_enable_bank_baseaddr'			: 0x0B270010, 
				'irq_cfg_baseaddr'					: 0x0B270110, 
				'irq_status_bank_baseaddr'			: 0x0B270900, 
				'pdc_version_drv_baseaddr'			: 0x0B271000,
				'pdc_mode_status_baseaddr'			: 0x0B271030, 
				'enable_pdc_baseaddr'				: 0x0B254500,
				'timer_match_value_lo_baseaddr'		: 0x0B274510,
				'timer_match_value_hi_baseaddr'		: 0x0B274514,
				'seq_busy_drv_baseaddr'				: 0x0B271020,
				
			},
}
pdc_gpu={
	'670' :{ 
				'no_drv'							: 1,
				'no_units'							: 5,
				'pdc_version_drv_baseaddr'			: 0x0B281000,
				'pdc_mode_status_baseaddr'			: 0x0B281030, 
				'enable_pdc_baseaddr'				: 0x0B284500,
				'timer_match_value_lo_baseaddr'		: 0x0B284510,
				'timer_match_value_hi_baseaddr'		: 0x0B284514,
				'seq_busy_drv_baseaddr'				: 0x0B281020,
				'profiling_output_baseaddr'			: 0x0B281200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B281204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B281208,
				
			},
}
pdc_display={
	'670' :{ 
				'no_drv'							: 1,
				'no_units'							: 5,
				'pdc_version_drv_baseaddr'			: 0x0B291000,
				'pdc_mode_status_baseaddr'			: 0x0B291030, 
				'enable_pdc_baseaddr'				: 0x0B294500,
				'timer_match_value_lo_baseaddr'		: 0x0B294510,
				'timer_match_value_hi_baseaddr'		: 0x0B294514,
				'seq_busy_drv_baseaddr'				: 0x0B291020,
				'profiling_output_baseaddr'			: 0x0B291200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B291204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B291208,
				
			},
}
pdc_compute={
	'670' :{ 
				'no_drv'							: 1,
				'no_bank'							: 1,
				'no_units'							: 5,
				'no_irq'							: 10,
				'irq_enable_bank_baseaddr'			: 0x0B2A0010, 
				'irq_cfg_baseaddr'					: 0x0B2A0110, 
				'irq_status_bank_baseaddr'			: 0x0B2A0900, 
				'pdc_version_drv_baseaddr'			: 0x0B2A1000,
				'pdc_mode_status_baseaddr'			: 0x0B2A1030, 
				'enable_pdc_baseaddr'				: 0x0B2A4500,
				'timer_match_value_lo_baseaddr'		: 0x0B2A4510,
				'timer_match_value_hi_baseaddr'		: 0x0B2A4514,
				'seq_busy_drv_baseaddr'				: 0x0B2A1020,
				'profiling_output_baseaddr'			: 0x0B2A1200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B2A1204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B2A1208,
				
			},
}
pdc_modem={
	'670' :{ 
				'no_drv'							: 2,
				'no_bank'							: 2,
				'no_units'							: 5,
				'no_irq'							: 45,
				'irq_enable_bank_baseaddr'			: 0x0B2B0010, 
				'irq_cfg_baseaddr'					: 0x0B2B0110, 
				'irq_status_bank_baseaddr'			: 0x0B2B0900, 
				'pdc_version_drv_baseaddr'			: 0x0B2B1000,
				'pdc_mode_status_baseaddr'			: 0x0B2B1030, 
				'enable_pdc_baseaddr'				: 0x0B2B4500,
				'timer_match_value_lo_baseaddr'		: 0x0B2B4510,
				'timer_match_value_hi_baseaddr'		: 0x0B2B4514,
				'seq_busy_drv_baseaddr'				: 0x0B2B1020,
				'profiling_output_baseaddr'			: 0x0B2B1200,
				'profiling_timestamp_lo_baseaddr'	: 0x0B2B1204,
				'profiling_timestamp_hi_baseaddr'	: 0x0B2B1208,
				
			},
}

bcm = { 
	'670' :{ 
				'vt_base_address' :  0x0BA00000, 
				'bcm_status_fe' : 0xBBF0004,
				'bcm_status_be' : 0xBBF0800,
				'bcm_status_be_seq' : 0xBBF0880,
				'no_regs' : 64,
				'no_drv'  : 12,
				'no_vcds' : 16,
			},
}

rail_level = { 
				0x0   :'OFF',
				0x10  :'RET',
				0x30  :'MIN_SVS',
				0x40  :'LOW_SVS',
				0x80  :'SVS',
				0xC0  :'SVS_L1',
				0x100 :'NOM',
				0x140 :'NOM_L1',
				0x150 :'NOM_L2',
				0x180 :'TUR',
				0x1A0 :'TUR_L1',
				0x1C0 :'TUR_L2',
			   }

xo_level = {
				0x0  :'XO_OFF',
				0x20 :'PMIC_BUFFER_OFF',
				0x50 :'SOC_BUFFER_OFF',
				0x80 :'XO_ON',
			}
			
ddrss_level = {
				0x0  :'OFF',
				0x20 :'PHY_COLLAPSED',
				0x80 :'PLL_DISABLED',
				0xA0 :'ACTIVE',
			}