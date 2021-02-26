#=============================================================================
#
#                                    cfgvars.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
#
#  We no longer assign configuration variables to a section in cust_config_template.c;
#    instead we do it here, where we have much more control over things than is allowed
#    by the C preprocessor.
#
#  For each identifier in the configuration, name it under qurt_vars,
#    followed by zero or more flags on the same line separated with whitespace.
#
#  Flags that are planned are as described here.  Not all of these are meaningful yet:
#
#    BUILD    -- indicates that the identifier is consumed in the build process and does not
#                need to be propagated to the run-time image
#
#    COLDBOOT -- indicates that the identifier is consumed in the cold boot process and can
#                be reclaimed after that
#
#    EXPORT   -- indicates that the identifier is owned by the micro-kernel, but that it
#                needs to be readable by the guest-OS
#
#    QURTOS   -- indicates that the identifier needs to be accessible to the guest-OS
#
#    ISLAND   -- indicates that the identifier needs to be micro-image resident
#
#    TCM      -- indicates that the identifier needs to be TCM resident for functionality reasons
#
#    FAST     -- indicates that the identifier needs to be placed in the most preferential memory
#                for performance reasons
#
#    PAGE     -- indicates that the identifier describes an object which is 4K-page-aligned
#                and which consumes an exact number of pages
#

qurt_vars = '''

eip_*                                   BUILD
config_tlb_first_replaceable
g_spaces
pool_configs                            QURTOS
QDSP6_QURT_TIMER_BASE
qdsp6ss_base_addr                       ISLAND
qurt_fastint_stack
qurt_fastint_stack_size
QURTK_BQ_shared
QURTK_bus_prio_cfg
QURTK_chicken_bits_chicken
QURTK_chicken_bits_chicken_2nd
QURTK_chicken_bits_chicken_3rd
QURTK_chicken_bits_duck
QURTK_chicken_bits_rgdr
QURTK_chicken_bits_rgdr_2nd
QURTK_chicken_bits_rgdr_3rd
QURTK_chicken_bits_turkey
QURTK_clade_cfg_base                    ISLAND
QURTK_config_ccr_default
QURTK_config_debug_buffer_size
QURTK_config_max_island_int             ISLAND
QURTK_config_MAX_THREADS
QURTK_config_trace_mask
QURTK_config_usr_default
QURTK_CONTEXT_SIZE                      ISLAND
QURTK_dbg_cfg_mask
QURTK_DC_partition
QURTK_debug_buf
QURTK_dmt_enabled
QURTK_dynamic_tlb_reserve
QURTK_etm_atid                          ISLAND
QURTK_etm_cfg_base                      ISLAND
QURTK_etm_to_q6etb
QURTK_etm_enable_on_error               ISLAND
QURTK_futex_objs_array                  ISLAND
QURTK_futex_objs                        ISLAND
QURTK_gen_relocs
QURTK_HD_cache
QURTK_HI_cache
QURTK_hthread_startup                   ISLAND
QURTK_hvx_check_power_lock
QURTK_IAHL
QURTK_icache_enable
QURTK_IC_partition
QURTK_IEL
QURTK_int_elem_size
QURTK_int_island                        ISLAND
QURTK_int_max                           ISLAND
QURTK_int_nest_allow
QURTK_int_privilege                     ISLAND
QURTK_int_usrpd_check
QURTK_isdb_flag_imem_addr
QURTK_island_type                       ISLAND
QURTK_island_fail_tlbmiss               ISLAND
QURTK_isr_data
QURTK_l1_icache_size
QURTK_l2cache_nwa
QURTK_l2cache_partition_cfg
QURTK_l2cache_size
QURTK_l2cache_wb
QURTK_l2cfg_tab
QURTK_l2cfg_tab_len
QURTK_l2_enable                         ISLAND
QURTK_l2_interrupt_table
QURTK_l2_int_idx_table                  ISLAND
QURTK_l2_island_interrupt_table         ISLAND
QURTK_L2_partition
QURTK_l2_type
QURTK_livelock_data
QURTK_mailbox_data
QURTK_mailbox_max_cycles
QURTK_mailbox_msg_cycles
QURTK_max_mailboxes
QURTK_max_pi_prio
QURTK_max_programs                      ISLAND
QURTK_max_spcbs
QURTK_MAX_THREADS_IN_TCM                ISLAND
QURTK_min_image_page
QURTK_mprotect
QURTK_pagetables
QURTK_pagetablestruct_v2
QURTK_page_table_v2
QURTK_page_table_v2_0x15
QURTK_pcb_data                          ISLAND
QURTK_phys_relocs
QURTK_program_names
QURTK_ramfs_scan
QURTK_root_prog_no
QURTK_secure_proc_ctrl_data
QURTK_secure_proc_sid                   ISLAND
QURTK_ssr_default
QURTK_stack_framekey_enabled
QURTK_supplement_chicken
QURTK_supplement_chicken_num
QURTK_supplement_chicken_revid
QURTK_supplement_rgdr
QURTK_supplement_rgdr_num
QURTK_supplement_rgdr_revid
QURTK_tbb
QURTK_tcm_dump_static
QURTK_tcxo_intmask
QURTK_thread_contexts
QURTK_thread_contexts_tcm               ISLAND
QURTK_thread_objs_array                 ISLAND
QURTK_thread_objs                       ISLAND
QURTK_timetest_port
QURTK_tlb_available_count
QURTK_tlb_debug_level
QURTK_tlb_first_replaceable
QURTK_tlb_last_replaceable
QURTK_tlblock_entries
QURTK_TLBMISS_SCRATCH_AREA
QURTK_trace_buffer_size
QURTK_WB_dbg_cfg_mask
qurt_osam_stack_fill_count
qurt_osam_stack_usage_fill_count
qurtos_app_heap_cfg
qurtos_app_stack                        QURTOS
qurtos_boot_mappings
qurtos_config_init_funcs
qurtos_heap
qurtos_island_heap                      ISLAND QURTOS
qurtos_island_stack_size
qurtos_main_bitmask
qurtos_main_priority
qurtos_main_stack_size                  QURTOS
qurtos_mmap_table
qurtos_mmap_unlocked
qurtos_pool_image_fixups
qurtos_process_reaper_generic_init      QURTOS
qurtos_reaper_priority
qurtos_reaper_stack_size
qurtos_reaper_tcb_partition
qurtos_split_virt_pool
qurtos_stack_size
qurtos_unlock_after_boot
qurtos_virt_reserved
qurtos_xml_passthrough
qurt_q6etb_dump
qurt_tcm_dump
qurt_tcm_dump_size
qurt_tcm_save_size
QURT_timer_intno
QURT_timerIST_priority
QURT_timerIST_tcb_partition
qurt_tlb_idx
trace_space

'''

class Script(object):
    def __init__(self):
        self.sec_list = []
        self.var_list = []
        for L in qurt_vars.splitlines():
            A = L.split()
            if A:
                self.var_list.append(A)
    def add_section(self, secname, **kw):
        L = []
        exact = kw.pop('EXACT', False)
        name = kw.pop('NAME', None)
        prolog = kw.pop('PROLOG', [])
        epilog = kw.pop('EPILOG', [])
        for A in self.var_list[:]:
            if name != None and A[0] != name:
                break
            for k,v in kw.iteritems():
                if bool(v) != bool(k in A[1:]):
                    break
            else:
                self.var_list.remove(A)
                L.append(A)
        if L or prolog or epilog:
            if exact:
                out_prefixes = ['']
                in_prefixes = ['*.']
            else:
                out_prefixes = ['.text', '.rodata', '.data', '.bss']
                in_prefixes = ['.text.', '.rodata.', '.data.', '.bss.']
            for out_prefix, in_prefix in zip(out_prefixes, in_prefixes):
                self.sec_list.append('  %s%s : {' % (out_prefix, secname,))
                self.sec_list.extend(['    '+s for s in prolog])
                for A in L:
                    self.sec_list.append('    *(%s%s)' % (in_prefix, A[0],))
                self.sec_list.extend(['    '+s for s in epilog])
                self.sec_list.append('  }')
    def tostring(self):
        return '\n'.join(self.sec_list+[])

