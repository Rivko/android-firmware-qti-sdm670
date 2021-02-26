#  ===========================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import logging
logger = logging.getLogger(__name__)

import os
import itertools
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from dwarf import Array as darray
from hansei_utils import *

def dump(debug_data): #dump_path, memory, debug_info):
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    dump_path = debug_data['dump_path']

    debug_data['printer'].p("Dumping DDR stats...")

    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['aop_core_dump'].vartype
    aop_core_dump = decode_object('aop_core_dump', address, dump_type, memory, debug_info)


    if (check_member('ddr_stats', aop_core_dump)):
        #ddr_stats in in core dump
        ddr = aop_core_dump.ddr_stats
        ddr = cast(ddr, 'ddr_stats_type', memory, debug_info)
    else:
        try:
            freq_switch_address = debug_info.variables['ddr_stats'].die
        except:
            ddr_file_name = os.path.join(dump_path, 'ddr_stats.txt')
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_STATS DOES NOT EXIST !!! \n", file=ddr_file)
            return
        try:
            #not in core dump, but elf is close enough to provide a correct pointer
            ddr_address = debug_info.variables['ddr_stats'].address # FIXME: get right address by magic
            ddr = decode_object('ddr_stats', ddr_address, None, memory, debug_info, die=freq_switch_address)
        except:
            #not correct in elf, find absolute address (very hacky)
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_STATS DOES NOT EXIST !!! \n", file=ddr_file)
            return
################################################################################################################################			
	#import pdb; pdb.set_trace()		
	if (check_member('ddr_status', aop_core_dump)):
        #ddr_status in in core dump
		ddr_status = aop_core_dump.ddr_status
		ddr_status = cast(ddr_status, 'ddr_state', memory, debug_info)###################################
	else:
		try:
			freq_switch_address = debug_info.variables['ddr_status'].die###################################
		except:
			ddr_file_name = os.path.join(dump_path, 'ddr_stats.txt')
			with open(ddr_file_name, 'w') as ddr_file:
				print("\n !!! DDR_STATUS DOES NOT EXIST !!! \n", file=ddr_file)
			return
		try:
			#not in core dump, but elf is close enough to provide a correct pointer
			ddr_status_address = debug_info.variables['ddr_status'].address # FIXME: get right address by magic
			ddr_status = decode_object('ddr_status', ddr_status_address, None, memory, debug_info, die=freq_switch_address)
		except:
			#not correct in elf, find absolute address (very hacky)
			with open(ddr_file_name, 'w') as ddr_file:
				print("\n !!! DDR_STATUS DOES NOT EXIST !!! \n", file=ddr_file)
			return		

################################################################################################################################			
    if (check_member('global_ddr_cfg ', aop_core_dump)):
        #global_ddr_cfg  in in core dump
        ddr_cfg = aop_core_dump.global_ddr_cfg 
        ddr_cfg = cast(ddr_cfg, 'DDRCfgType', memory, debug_info)
    else:
        try:
            ddr_cfg_address = debug_info.variables['global_ddr_cfg'].die
        except:
            ddr_file_name = os.path.join(dump_path, 'ddr_stats.txt')
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_CFG DOES NOT EXIST !!! \n", file=ddr_file)
            return
        try:
            #not in core dump, but elf is close enough to provide a correct pointer
            ddr_gbl_address = debug_info.variables['global_ddr_cfg'].address # FIXME: get right address by magic
            ddr_cfg = decode_object('global_ddr_cfg', ddr_gbl_address, None, memory, debug_info, die=ddr_cfg_address)
        except:
            #not correct in elf, find absolute address (very hacky)
            with open(ddr_file_name, 'w') as ddr_file:
                print("\n !!! DDR_cfg DOES NOT EXIST !!! \n", file=ddr_file)
            return

    #import pdb; pdb.set_trace()
    #save_logger_level = update_logger(logging.DEBUG, logging)
    dump_ddr(dump_path, ddr_cfg, ddr, ddr_status, memory, debug_info)
    #update_logger(save_logger_level, logging)

def dump_ddr(dump_path, ddr_cfg, ddr, ddr_status, memory, debug_info):
	ddr_file_name = os.path.join(dump_path, 'ddr_stats.txt')
	with open(ddr_file_name, 'w') as ddr_file:
		print("\n\n######################################################################################################################################################\n ~~ DDR and Shub Clock Plan ~~", file=ddr_file)
		print("######################################################################################################################################################\n ", file=ddr_file)
		try:
			print('\tNumber of MC states:         %d' % ddr_cfg.nNumMCStates, file=ddr_file)
			print("\n---------------------------------------------------------------------------------------------------------------------------------------------------------", file=ddr_file)
			print("\n| Clk_Idx | freq_khz | Clk_Period | Freq_enable | Mode |  VDD_CX  |  VDD_MX  |   VDDA   | ddr_auto_cfg | max_up_idx | min_down_idx | double_freq_change |", file=ddr_file)
			print("\n---------------------------------------------------------------------------------------------------------------------------------------------------------", file=ddr_file)
			for x in range(0, ddr_cfg.nNumMCStates):
				print("|{0:>9d}|{1:>10d}|{2:>12d}|{3:>13d}|{4:>6s}|{5:>10s}|{6:>10s}|{7:>10s}|{8:>14s}|{9:>12d}|{10:>14d}|{11:>20d}|"  .format(\
                                                                ddr_cfg.pMCCfg[x].clk_idx,\
                                                                ddr_cfg.pMCCfg[x].freq_khz,\
                                                                ddr_cfg.pMCCfg[x].clk_period,\
                                                                ddr_cfg.pMCCfg[x].freq_enable,\
                                                                ddr_cfg.pMCCfg[x].mode,\
                                                                ddr_cfg.pMCCfg[x].vddcx,\
                                                                ddr_cfg.pMCCfg[x].vddmx,\
                                                                ddr_cfg.pMCCfg[x].vdda,\
                                                                str(ddr_cfg.pMCCfg[x].ddr_automode_cfg),\
                                                                ddr_cfg.pMCCfg[x].max_up_idx,\
                                                                ddr_cfg.pMCCfg[x].min_down_idx,\
                                                                ddr_cfg.pMCCfg[x].double_freq_change,\
                                                                ), file=ddr_file)
																
			print('', file=ddr_file)

		except:
			print('\t\tERROR Reading MC freq plan  ', file=ddr_file)
			
		try:
			print('\n\tNumber of Shub states:        %d' % ddr_cfg.nNumSHUBStates, file=ddr_file)
			print("\n---------------------------------------------------------------------------------------------------------------------------------------------------------", file=ddr_file)
			print("\n| Clk_Idx | freq_khz | Clk_Period | Freq_enable | Mode |  VDD_CX  |  VDD_MX  |   VDDA   | ddr_auto_cfg | max_up_idx | min_down_idx | double_freq_change |", file=ddr_file)
			print("\n---------------------------------------------------------------------------------------------------------------------------------------------------------", file=ddr_file)
			for y in range(0, ddr_cfg.nNumSHUBStates):
				print("|{0:>9d}|{1:>10d}|{2:>12d}|{3:>13d}|{4:>6s}|{5:>10s}|{6:>10s}|{7:>10s}|{8:>14s}|{9:>12d}|{10:>14d}|{11:>20d}|"  .format(\
                                                                ddr_cfg.pSHUBCfg[y].clk_idx,\
                                                                ddr_cfg.pSHUBCfg[y].freq_khz,\
                                                                ddr_cfg.pSHUBCfg[y].clk_period,\
                                                                ddr_cfg.pSHUBCfg[y].freq_enable,\
                                                                ddr_cfg.pSHUBCfg[y].mode,\
                                                                ddr_cfg.pSHUBCfg[y].vddcx,\
                                                                ddr_cfg.pSHUBCfg[y].vddmx,\
                                                                ddr_cfg.pSHUBCfg[y].vdda,\
                                                                str(ddr_cfg.pSHUBCfg[y].ddr_automode_cfg),\
                                                                ddr_cfg.pSHUBCfg[y].max_up_idx,\
                                                                ddr_cfg.pSHUBCfg[y].min_down_idx,\
                                                                ddr_cfg.pSHUBCfg[y].double_freq_change,\
                                                                ), file=ddr_file)
																
			print('', file=ddr_file)
		except:
			print('\t\tERROR READING Shub Freq Plan', file=ddr_file)
	
		print("\n\n######################################################################################################################################################\n ~~ DDR Stats Dump ~~", file=ddr_file)
		print("######################################################################################################################################################\n ", file=ddr_file)
		try:
			#import pdb;pdb.set_trace()
			print("\t{0:32s}{1:2s}"  .format(	"mc_count:",hex(ddr[0].mc_count)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"shub_count:",hex(ddr[0].shub_count)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"restore_count:",hex(ddr[0].restore_count)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"collapse_count:",hex(ddr[0].collapse_count)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"mc_cp_history:",hex(ddr[0].mc_cp_history)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"shub_cp_history:",hex(ddr[0].shub_cp_history)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"mc_begin:",hex(ddr[0].mc_begin)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"shub_begin:",hex(ddr[0].shub_begin)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"previous_mc_begin:",hex(ddr[0].previous_mc_begin)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"previous_shub_begin:",hex(ddr[0].previous_shub_begin)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"powerstate_collapse_begin:",hex(ddr[0].powerstate_collapse_begin)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"powerstate_restore_begin:",hex(ddr[0].powerstate_restore_begin)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"mc_max:",hex(ddr[0].mc_max)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"mc_max_time:",hex(ddr[0].mc_max_time)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"shub_max:",hex(ddr[0].shub_max)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"shub_max_time:",hex(ddr[0].shub_max_time)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"powerstate_collapse_max:",hex(ddr[0].powerstate_collapse_max)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"powerstate_collapse_max_time:",hex(ddr[0].powerstate_collapse_max_time)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"powerstate_restore_max:",hex(ddr[0].powerstate_restore_max)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"powerstate_restore_max_time:",hex(ddr[0].powerstate_restore_max_time)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"flush_count:",hex(ddr[0].flush_count)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"pad:",hex(ddr[0].pad)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"flush_begin:",hex(ddr[0].flush_begin)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"flush_max:",hex(ddr[0].flush_max)	), file=ddr_file)
			print("\t{0:32s}{1:2s}"  .format(	"flush_max_time:",hex(ddr[0].flush_max_time)	), file=ddr_file)
			print('', file=ddr_file)

		except:
			print('\t\tERROR READING ddr_stats[0]', file=ddr_file)
			
			
			
			
		print("\n\n\n######################################################################################################################################################\n ~~ DDR Status Dump ~~", file=ddr_file)
		print("######################################################################################################################################################\n ", file=ddr_file)
		try:
			print("\t{0:32s}{1:10s}"  .format(	"ddr_rm_pending:",hex(ddr_status.ddr_rm_pending)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"bcm_mc_pending:",hex(ddr_status.bcm_mc_pending)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"bcm_shub_pending:",hex(ddr_status.bcm_shub_pending)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"bcm_alc_pending:",(ddr_status.bcm_alc_pending)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"bcm_acv_pending:",(ddr_status.bcm_acv_pending)	), file=ddr_file)

			print("\t{0:32s}{1:10s}"  .format(	"ddr_rm_vote_on:",hex(ddr_status.ddr_rm_vote_on)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"fal_in_progress:",hex(ddr_status.fal_in_progress)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_dest_mc_lvl:",hex(ddr_status.ddr_dest_mc_lvl)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_dest_shub_lvl:",hex(ddr_status.ddr_dest_shub_lvl)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_next_mc_lvl:",hex(ddr_status.ddr_next_mc_lvl)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_next_shub_lvl:",hex(ddr_status.ddr_next_shub_lvl)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_mc_lvl_double:",hex(ddr_status.ddr_mc_lvl_double)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"current_mc_cp:",hex(ddr_status.current_mc_cp)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"current_shub_cp:",hex(ddr_status.current_shub_cp)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_arc_level:",hex(ddr_status.ddr_arc_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_bcm_mc_level:",hex(ddr_status.ddr_bcm_mc_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_bcm_shub_level:",hex(ddr_status.ddr_bcm_shub_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_bcm_acv_level:",hex(ddr_status.ddr_bcm_acv_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_bcm_alc_level:",hex(ddr_status.ddr_bcm_alc_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_reg_mc_level:",hex(ddr_status.ddr_reg_mc_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_reg_shub_level:",hex(ddr_status.ddr_reg_shub_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_flush_complete:",hex(ddr_status.ddr_flush_complete)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_pcb_mask:",hex(ddr_status.ddr_pcb_mask)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_lb_activations:",hex(ddr_status.ddr_lb_activations)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_xo_vote:",hex(ddr_status.ddr_xo_vote)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_xo_on:",hex(ddr_status.ddr_xo_on)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_dcc_vote:",hex(ddr_status.ddr_dcc_vote)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_dcc_on:",hex(ddr_status.ddr_dcc_on)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"dcc_on_request:",hex(ddr_status.dcc_on_request)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"memtest_needed:",hex(ddr_status.memtest_needed)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_rm_vote:",hex(ddr_status.ddr_rm_vote)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_cx_vote:",hex(ddr_status.ddr_cx_vote)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_mx_vote:",hex(ddr_status.ddr_mx_vote)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_vdda_vote:",hex(ddr_status.ddr_vdda_vote)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_pmic_mode_vote:",hex(ddr_status.ddr_pmic_mode_vote)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_shrm_level:",hex(ddr_status.ddr_shrm_level)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_shrm_on:",hex(ddr_status.ddr_shrm_on)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"tcs_0_pending:",hex(ddr_status.tcs_0_pending)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"tcs_1_pending:",hex(ddr_status.tcs_1_pending)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"tcs_2_pending:",hex(ddr_status.tcs_2_pending)	), file=ddr_file)
			print("\t{0:32s}{1:10s}"  .format(	"ddr_mc_floor:",hex(ddr_status.ddr_mc_floor)	), file=ddr_file)
			
			print('', file=ddr_file)


		except:
			print('\t\tERROR READING ddr_status', file=ddr_file)
