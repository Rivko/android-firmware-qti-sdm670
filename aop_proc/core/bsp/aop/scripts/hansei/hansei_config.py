#  ===========================================================================
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


dump_file_types = [
    #
    # Core stuff, most important.
    #
    {   # The place all of the code lives
        'display_name'  : "CODERAM",
        'name'          : {'CODERAM.BIN','md_CODERAM.BIN','aop_code_0xb000000--0xb017fff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'CODERAM', 'md_CODERAM','aop_code_0xb000000--0xb017fff'},
        'required'      : True,
        'no_load'       : False,
        'base'          : {
                            'hoya'  : 0x000000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "DATARAM",
        'name'          : {'DATARAM.BIN','md_DATARAM.BIN','aop_data_0xb0e0000--0xb0e7fff.lst'},
        'ext'           : {'.BIN','.LST' },
        'basename'      : {'DATARAM','md_DATARAM', 'aop_data_0xb0e0000--0xb0e7fff'},
        'required'      : True,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x0E0000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM0",
        'name'          : {'MSGRAM0.BIN','md_MSGRAM0.BIN','aop_msg0_0xc300000--0xc3003ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM0', 'md_MSGRAM0','aop_msg0_0xc300000--0xc3003ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1300000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM1",
        'name'          : {'MSGRAM1.BIN','md_MSGRAM1.BIN','aop_msg1_0xc310000--0xc3103ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM1','md_MSGRAM1','aop_msg1_0xc310000--0xc3103ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1310000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM2",
        'name'          : {'MSGRAM2.BIN','md_MSGRAM2.BIN','aop_msg2_0xc320000--0xc3203ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM2','md_MSGRAM2','aop_msg2_0xc320000--0xc3203ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1320000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM3",
        'name'          : {'MSGRAM3.BIN','md_MSGRAM3.BIN','aop_msg3_0xc330000--0xc3303ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM3','md_MSGRAM3','aop_msg3_0xc330000--0xc3303ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1330000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM4",
        'name'          : {'MSGRAM4.BIN','md_MSGRAM4.BIN','aop_msg4_0xc340000--0xc3403ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM4','md_MSGRAM4','aop_msg4_0xc340000--0xc3403ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1340000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM5",
        'name'          : {'MSGRAM5.BIN','md_MSGRAM5.BIN','aop_msg5_0xc350000--0xc3503ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM5','md_MSGRAM5','aop_msg5_0xc350000--0xc3503ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1350000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM6",
        'name'          : {'MSGRAM6.BIN','md_MSGRAM6.BIN','aop_msg6_0xc360000--0xc3603ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM6','md_MSGRAM6','aop_msg6_0xc360000--0xc3603ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1360000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM7",
        'name'          : {'MSGRAM7.BIN','md_MSGRAM7.BIN','aop_msg7_0xc370000--0xc3703ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM7','md_MSGRAM7','aop_msg7_0xc370000--0xc3703ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1370000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM8",
        'name'          : {'MSGRAM8.BIN','md_MSGRAM8.BIN','aop_msg8_0xc380000--0xc3803ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM8','md_MSGRAM8','md_MSGRAM0.BIN','aop_msg8_0xc380000--0xc3803ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1380000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM9",
        'name'          : {'MSGRAM9.BIN','md_MSGRAM9.BIN','aop_msg9_0xc390000--0xc3903ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM9','md_MSGRAM9','aop_msg9_0xc390000--0xc3903ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x1390000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM10",
        'name'          : {'MSGRAM10.BIN','md_MSGRAM10.BIN','aop_msg10_0xc3a0000--0xc3a03ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM10','md_MSGRAM10','aop_msg10_0xc3a0000--0xc3a03ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x13A0000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM11",
        'name'          : {'MSGRAM11.BIN','md_MSGRAM11.BIN','aop_msg11_0xc3b0000--0xc3b03ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM11','md_MSGRAM11','aop_msg11_0xc3b0000--0xc3b03ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x13B0000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM12",
        'name'          : {'MSGRAM12.BIN','md_MSGRAM12.BIN','aop_msg12_0xc3c0000--0xc3c03ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM12','md_MSGRAM12','aop_msg12_0xc3c0000--0xc3c03ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x13C0000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM13",
        'name'          : {'MSGRAM13.BIN','md_MSGRAM13.BIN','aop_msg13_0xc3d0000--0xc3d03ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM13','md_MSGRAM13','aop_msg13_0xc3d0000--0xc3d03ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x13D0000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "MSGRAM14",
        'name'          : {'MSGRAM14.BIN','md_MSGRAM14.BIN','aop_msg14_0xc3e0000--0xc3e03ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM14','md_MSGRAM14','aop_msg14_0xc3e0000--0xc3e03ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x13E0000,
                          },
    },
	{   # The place all of the data lives
        'display_name'  : "MSGRAM15",
        'name'          : {'MSGRAM15.BIN','md_MSGRAM15.BIN','aop_msg15_0xc3f0000--0xc3f03ff.lst'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'MSGRAM15','md_MSGRAM15','aop_msg15_0xc3f0000--0xc3f03ff'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0x13F0000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "RPMH_BINARY",
        'name'          : {'RPMH_BINARY.BIN'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'RPMH_BINARY'},
        'required'      : False,
        'no_load'       : False,
        'base'          : {
                              'hoya'  : 0xB200000,
                          },
    },
    {   # The place all of the data lives
        'display_name'  : "RPMH_MINIDUMP",
        'name'          : {'md_rpmh.BIN'},
        'ext'           : {'.BIN','.LST'},
        'basename'      : {'md_rpmh'},
        'required'      : False,
        'no_load'       : True,
        'path'          : "",
        'size'          : 0x0,
        'update'        : True,
        'base'          : {
                              'hoya'  : 0x80000000,
                          },
    },

    {   # Certain system data is stored in OCIMEM
        'display_name'  : "OCIMEM",
        'name'          : {'OCIMEM.BIN','ocimem_0x14680000--0x146bffff.lst'},
        'ext'           : {'.BIN', '.LST'},
        'basename'      : {'OCIMEM', 'ocimem_0x14680000--0x146bffff'},
        'no_load'       : True,
        'path'          : "",
        'base'          : {
                              'hoya'  :{ '670': 0x14680000, },
                          },
        'data'          : {
                              'dumptable' : {
                                                '670'   : 0x146BF000 + 0x10,
                                            },
                          },
    },
    {   # Heap is copied to DDR before overwritting
        'display_name'  : "DDRCS0_0",
        'name'          : {'DDRCS0_0.BIN','dram_cs0_0x80000000--0xffffffff.lst'},
        'ext'           : {'.BIN', '.LST'},
        'basename'      : {'DDRCS0_0', 'dram_cs0_0x80000000--0xffffffff'},
        'no_load'       : True,
        'path'          : "",
        'size'          : 0x0,
        'update'        : True,
        'base'          : {
                              'hoya'  : {  #Size      : base address
                                            0x80000000 : 0x80000000,
                                         },
                          },
    },
    {   # Heap is copied to DDR before overwritting
        'display_name'  : "DDRCS0_1",
        'name'          : {'DDRCS0_1.BIN','dram_cs0_0x100000000--0x17fffffff.lst','dram_cs0_0x100000000--0x13fffffff.lst'},
        'ext'           : {'.BIN', '.LST'},
        'basename'      : {'DDRCS0_1', 'dram_cs0_0x100000000--0x17fffffff','dram_cs0_0x100000000--0x13fffffff'},
        'no_load'       : True,
        'path'          : "",
        'update'        : True,
        'size'          : 0x0,
        'base'          : {
                              'hoya'  : {  #Size      : base address
                                            0x40000000 : 0x100000000,
                                            0x80000000 : 0x100000000,
                                         },
                          },
    },
    {   # Heap is copied to DDR before overwritting
        'display_name'  : "DDRCS1_0",
        'name'          : {'DDRCS1_0.BIN','dram_cs1_0x140000000--0x1bfffffff.lst'},
        'ext'           : {'.BIN', '.LST'},
        'basename'      : {'DDRCS1_0', 'dram_cs1_0x140000000--0x1bfffffff'},
        'no_load'       : True,
        'path'          : "",
        'update'        : True,
        'size'          : 0x0,
        'base'          : {
                              'hoya'  : {  #Size      : base address
                                            0x80000000 : 0x100000000,
                                         },
                          },
    },
    {   # Heap is copied to DDR before overwritting
        'display_name'  : "DDRCS1_1",
        'name'          : {'DDRCS1_1.BIN','dram_cs1_0x1c0000000--0x1ffffffff.lst'},
        'ext'           : {'.BIN', '.LST'},
        'basename'      : {'DDRCS1_1', 'dram_cs1_0x1c0000000--0x1ffffffff'},
        'no_load'       : True,
        'path'          : "",
        'update'        : True,
        'size'          : 0x0,
        'base'          : {
                              'hoya'  : {  #Size      : base address
                                            0x80000000 : 0x180000000,
                                         },
                          },
    },
#    #{   # The values of RPM registers. Only consulted if there was no core dump.
#    #    'name'     : 'rpm_registers.cmm',
#    #    # FIXME: need some parser here to make this file useful...
#    #},
#
#    #
#    # Generic RPM peripherals, less important
#    #
#    {   # RPM's generic Cortex-M3 hardware.
#        'display_name'  : "RPM_SCS",
#        'name'          : {'rpm_scs.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM_SCS'},
#        'base'          : {
#                              'badger' : 0xE000E000,
#                              'bear'   : 0xE000E000,
#                              'honey'  : 0xE000E000,
#                          },
#    },
#    {   # RPM's Qualcomm-written hardware.
#        'display_name'  : "RPM_DEC",
#        'name'          : {'rpm__dec.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM__DEC'},
#        'base'          : {
#                              'badger' : 0x180000,
#                              'bear'   : 0x080000,
#                              'honey'  : 0x280000,
#                          },
#    },
#    {   # RPM's timer-related hardware (next 3)
#        'display_name'  : "RPM_QTMR_AC",
#        'name'          : {'rpm__qtmr_ac.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM__QTMR_AC'},
#        'base'          : {
#                              'badger' : 0x182000,
#                              'bear'   : 0x082000,
#                              'honey'  : 0x282000,
#                          },
#    },
#    {
#        'display_name'  : "RPM_QTMR_F0",
#        'name'          : {'rpm__qtmr_f0.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM__QTMR_F0'},
#        'base'          : {
#                              'badger' : 0x183000,
#                              'bear'   : 0x083000,
#                              'honey'  : 0x283000,
#                          },
#    },
#    {
#        'display_name'  : "RPM_QTMR_F1",
#        'name'          : {'rpm__qtmr_f1.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM__QTMR_F1'},
#        'base'          : {
#                              'badger' : 0x184000,
#                              'bear'   : 0x084000,
#                              'honey'  : 0x284000,
#                          },
#    },
#    {   # RPM data RAM access control unit.
#        'display_name'  : "RPM_MPU",
#        'name'          : {'rpm__mpu.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM__MPU'},
#        'base'          : {
#                              'badger' : 0x186000,
#                              'bear'   : -1, #does not exist on bear?
#                              'honey'  : 0x289000,
#                          },
#    },
#    {   # RPM code RAM access control unit.
#        'display_name'  : "RPM_APU",
#        'name'          : {'rpm__apu.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM__APU'},
#        'base'          : {
#                              'badger' : 0x187000,
#                              'bear'   : 0x087000,
#                              'honey'  : 0x087000,
#                          },
#    },
#    {   # RPM message RAM access control unit.
#        'display_name'  : "RPM_RPU",
#        'name'          : {'rpm__rpu.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'RPM__RPU'},
#        'base'          : {
#                              'badger' : 0xFC420000,
#                              'bear'   : -1, #does not exist on bear?
#                              'honey'  : -1,
#                          },
#    },
#
#    #
#    # System peripherals, vaguely RPM related.
#    #
#    {   # The bits of the MPM that control low power states.
#        'display_name'  : "RPM_MPM",
#        'name'          : {'mpm.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'MPM'},
#        'base'          : {
#                              'badger' : 0xFC4A0000,
#                              'bear'   : 0x004A0000,
#                              'honey'  : 0x004A0000,
#                          },
#    },
#    {   # The global clock controller.
#        'display_name'  : "RPM_CLK_CTL",
#        'name'          : {'clk_ctl.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'CLK_CTL'},
#        'base'          : {
#                              'badger' : 0xFC400000,
#                              'bear'   : -1, #not captured for bear?
#                              'honey'  : 0x00300000,
#                          },
#    },
#    {   # Some parts of the TCSR.
#        'display_name'  : "RPM_TCSR_REGS",
#        'name'          : {'tcsr_regs.bin'},
#        'ext'           : {'.BIN'},
#        'basename'      : {'TCSR_REGS'},
#        'base'          : {
#                              'badger' : 0xFD4A1300,
#                              'bear'   : 0x01937300,
#                              'honey'  : 0x007A0000,
#                          },
#    },
]

known_targets = {
    '670' : 'hoya',
    '670' : 'hoya',
}

known_families = {
    'hoya'        : 'hoya',
}

default_target_family = {
    'target'        : '670',
    'family'        : 'hoya',
    'family_name'   : 'Hoya',
    'branch'        : 'AOP.HO.1.0',
}

dumptable_version_map = {
    '670'    : '1.4',
    'default' : '1.4',
}

dumptable_config = {
    '1.4' :  {
                'version' : 0x200000,
                'data'    : {
                              'size'        : 0x3C,
                              'version'     : 0x0,
                              'magic'       : 0x4,
                              'magic_value' : 0x0, #need to actually figure this one out
                              'name'        : 0x8,
                              'name_size'   : 0x20,
                              'addr'        : 0x28,
                              'len'         : 0x30,
                              'reserved'    : 0x38,
                            },
                'entry'   : {
                              'size'        : 0x30,
                              'id'          : 0x0,
                              'name'        : 0x4,
                              'name_size'   : 0x20,
                              'type'        : 0x24,
                              'addr'        : 0x28,
                            },
                'table'   : {
                              'size'        : 0x30,
                              'version'     : 0x0,
                              'num_entries' : 0x4,
                              'entries'     : 0x8,
                            },
              },
}

rpmhdump_config = {
	0x11: {
					'blockdump_type': {
										'size' 			:0x1008,
										'count'			:0x0,
										'block_region'	:0x8,
										'block_size'    :0x20,
										'data'			:0x1008,
										'no_data_blocks':0x80,
										},
					'region_type'	: {
										'size'		:0x20,
										'start_addr':0x0,
										'end_addr'	:0x8,
										'data_size'	:0x10,
										'offset'	:0x18,
										},
	},
	0x12: {
					'blockdump_type': {
										'size' 			:0x1408,
										'count'			:0x0,
										'block_region'	:0x8,
										'block_size'    :0x20,
										'data'			:0x1408,
										'no_data_blocks':0xA0,
										},
					'region_type'	: {
										'size'		:0x20,
										'start_addr':0x0,
										'end_addr'	:0x8,
										'data_size'	:0x10,
										'offset'	:0x18,
										},
	},
	0x13: {
					'blockdump_type': {
										'size' 			:0x1F48,
										'count'			:0x0,
										'block_region'	:0x8,
										'block_size'    :0x20,
										'data'			:0x1F48,
										'no_data_blocks':0xFA,
										},
					'region_type'	: {
										'size'		:0x20,
										'start_addr':0x0,
										'end_addr'	:0x8,
										'data_size'	:0x10,
										'offset'	:0x18,
										},
	}

}