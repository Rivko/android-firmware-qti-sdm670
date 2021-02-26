/* This file is automatically generated. */
#ifndef   GEN_CONFIG_HEADER_USED
#define   GEN_CONFIG_HEADER_USED
extern typeof(unsigned int) qurtos_main_priority;



extern typeof(unsigned int) qurtos_main_bitmask;
extern typeof(unsigned int) qurtos_stack_size;
                                                                                    ;

struct objcache_config {
   short memory_blocks;
   short virt_regions;
   short phys_regions;
   short user_processes;
   short user_threads;
   short qurtos_threads;
   short shmem_regions;
};

                                                                       ;

extern typeof(unsigned int) qurtos_reaper_priority;




extern typeof(unsigned int) qurtos_reaper_tcb_partition;




extern typeof(unsigned int) qurtos_reaper_stack_size;





extern typeof(unsigned int) QURTK_max_pi_prio;




extern typeof(unsigned int) qurtos_split_virt_pool;




extern typeof(unsigned int) const qurtos_autostack_total_pages;




extern typeof(unsigned int) qurtos_pager_total_pages;




extern typeof(unsigned char []) qurtos_app_stack;






extern typeof(unsigned int) qurtos_main_stack_size;




extern typeof(unsigned int) qurt_osam_stack_fill_count;





extern typeof(unsigned int) qurt_osam_stack_usage_fill_count;




extern typeof(unsigned int) const qurt_stid_reserve_num;




enum config_heap_type {
   QURTOS_CONFIG_HEAP_STATIC,
   QURTOS_CONFIG_HEAP_DYNAMIC,
   QURTOS_CONFIG_HEAP_ASLR,
   QURTOS_CONFIG_HEAP_APPREGION
};

struct config_heap_info {
   enum config_heap_type type;
   void *vaddr;
   unsigned size;
};
struct config_autostack_info {
    void *vaddr;
    unsigned size;
    unsigned lo_water_mark;
    unsigned hi_water_mark;
    unsigned const_stack_size;
};
                                               ;
                                            ;

extern typeof(struct config_heap_info) qurtos_app_heap_cfg;





extern typeof(struct config_autostack_info) QURTK_autostack_array_cfg;




extern typeof(unsigned int) QURTK_hi_water_mark;





extern typeof(unsigned int) QURTK_lo_water_mark;





extern typeof(unsigned int) QURTK_timetest_port;




extern typeof(unsigned int) QURTK_isdb_flag_imem_addr;





extern typeof(unsigned int) QURTK_etm_to_q6etb;






extern typeof(unsigned int []) qurt_q6etb_dump;




extern typeof(unsigned int) QURTK_tcxo_intmask;




extern typeof(unsigned int) QURTK_dmt_enabled;





extern typeof(unsigned int) QURTK_BQ_shared;
extern typeof(unsigned char []) qurt_tcm_dump_static_loc;
extern typeof(unsigned char *) QURTK_tcm_err_dump;






extern typeof(unsigned int) QURTK_tcm_err_dump_size;





extern typeof(int) QURTK_tcm_dump_type;







extern typeof(unsigned char *) qurt_tcm_dump;





extern typeof(unsigned int) qurt_tcm_dump_size;






extern typeof(unsigned int) qurt_tcm_save_size;
extern typeof(unsigned int) QURTK_ssr_default;







                                                ;
extern typeof(unsigned int) QURTK_config_ccr_default;




                                       ;
extern typeof(unsigned int) QURTK_config_usr_default;




                                       ;
extern typeof(unsigned char) const QURTK_l2cache_size;





extern typeof(unsigned int) const QURTK_l1_icache_size;




extern typeof(unsigned int) const QURTK_l1_dcache_size;
extern typeof(unsigned int) QURTK_l2cache_nwa;
extern typeof(unsigned int) QURTK_l2cache_wb;
extern typeof(unsigned int) QURTK_icache_enable;
extern typeof(unsigned int) QURTK_HI_cache;




extern typeof(unsigned int) QURTK_HD_cache;
extern typeof(unsigned char) QURTK_DC_partition;





extern typeof(unsigned char) QURTK_IC_partition;
                                        ;
                                             ;
extern typeof(unsigned int) const QURTK_l2cfg_tab_len;
extern typeof(unsigned char) QURTK_L2_partition;
extern typeof(unsigned int) const QURTK_chicken_bits_rgdr;





extern typeof(unsigned int) const QURTK_chicken_bits_duck;





extern typeof(unsigned int) const QURTK_chicken_bits_turkey;





extern typeof(unsigned int) const QURTK_chicken_bits_chicken;





extern typeof(unsigned int) QURTK_chicken_bits_rgdr_2nd;




extern typeof(unsigned int) const QURTK_chicken_bits_chicken_2nd;





extern typeof(unsigned int) QURTK_chicken_bits_rgdr_3rd;




extern typeof(unsigned int) const QURTK_chicken_bits_chicken_3rd;






extern typeof(unsigned int [16]) QURTK_supplement_rgdr;





extern typeof(unsigned int [16]) QURTK_supplement_rgdr_revid;





extern typeof(unsigned int [16]) QURTK_supplement_turkey;





extern typeof(unsigned int [16]) QURTK_supplement_turkey_revid;





extern typeof(unsigned int [16]) QURTK_supplement_duck;





extern typeof(unsigned int [16]) QURTK_supplement_duck_revid;





extern typeof(unsigned int [16]) QURTK_supplement_chicken;





extern typeof(unsigned int [16]) QURTK_supplement_chicken_revid;




extern typeof(unsigned int) const QURTK_supplement_rgdr_num;





extern typeof(unsigned int) const QURTK_supplement_turkey_num;





extern typeof(unsigned int) const QURTK_supplement_duck_num;






extern typeof(unsigned int) const QURTK_supplement_chicken_num;





extern typeof(unsigned int) const QURTK_livelock_data;





extern typeof(unsigned int) QURTK_hvx_check_power_lock;





extern typeof(unsigned int) QURTK_hvx_default_mode;
extern typeof(unsigned int) QDSP6_QURT_TIMER_BASE;




extern typeof(unsigned int) QURT_timer_intno;




extern typeof(unsigned int) QURT_timerIST_priority;




extern typeof(unsigned int) QURT_timerIST_tcb_partition;






extern typeof(unsigned int) const QURTK_dbg_cfg_mask;




extern typeof(unsigned int) const QURTK_WB_dbg_cfg_mask;




                                          ;
struct phys_mem_pool_config{
    char name[32];
    struct range {
        unsigned int start;
        unsigned int size;
    } ranges[16];
};





extern typeof(struct phys_mem_pool_config []) pool_configs;






void qurtos_pool_image_fixups(void (*pfn)(const char *,void *,void *))

;




extern typeof(unsigned int) QURTK_tlblock_entries;





extern typeof(int) QURTK_dynamic_tlb_reserve;
extern typeof(unsigned int) config_tlb_first_replaceable;




extern typeof(unsigned int) const QURTK_tlb_debug_level;






extern typeof(typeof(struct { unsigned char a[32] __attribute__((aligned(32))); }) []) QURTK_tlb_debug_space __attribute__((weak));
extern typeof(unsigned int) QURTK_min_image_page;
extern typeof(unsigned long long []) qurtos_mmap_table;





extern typeof(unsigned long long *) qurtos_mmap_unlocked;





struct unlock_after_boot {
   unsigned vpageno;
   unsigned pagecnt;
};



extern typeof(struct unlock_after_boot []) qurtos_unlock_after_boot;
struct boot_mapping {
   unsigned long long template;
   char *start_ptr;
   char *end_ptr;
};


extern typeof(struct boot_mapping []) qurtos_boot_mappings;






extern typeof(int []) qurtos_virt_reserved;





extern typeof(unsigned int) QURTK_l2cache_partition_cfg;





extern typeof(unsigned int) QURTK_bus_prio_cfg;
extern typeof(unsigned int) QURTK_IEL;





extern typeof(unsigned int) QURTK_IAHL;




extern typeof(unsigned int) QURTK_int_nest_allow;
extern typeof(unsigned int [32]) QURTK_l2_type;







extern typeof(unsigned short []) QURTK_isr_data;







extern typeof(char [][64]) QURTK_program_names;
extern typeof(int) QURTK_root_prog_no;





extern typeof(unsigned int) QURTK_int_usrpd_check;





extern typeof(unsigned int) QURTK_stack_framekey_enabled;







extern typeof(typeof(struct { unsigned char a[32] __attribute__((aligned(32))); }) []) qurt_fastint_stack;





extern typeof(unsigned int) qurt_fastint_stack_size;





extern typeof(unsigned int) QURTK_int_max;




extern typeof(typeof(struct { unsigned char a[16] __attribute__((aligned(16))); }) []) QURTK_l2_interrupt_table;





extern typeof(unsigned int) QURTK_int_elem_size;




extern typeof(unsigned char []) qurtos_heap;





                                                       ;



extern typeof(char []) qurtos_xml_passthrough __attribute__((weak));
                                                       ;
                                              ;
                                             ;

extern typeof(struct space * []) g_spaces;
struct QURTK_VTLB_TABLE_DESCRIPTOR {
   unsigned vtlb_version:4;
   unsigned vtlb_table_type:3;
   unsigned next_table_addr_is_virtual:1;
   unsigned long long next_table_addr:36;
   unsigned table_entries:20;
};

struct QURTK_VTLB_BASIC_TABLE {



   unsigned revision;
   unsigned padding;
   unsigned char signature[8];
   struct QURTK_VTLB_TABLE_DESCRIPTOR t0;
   unsigned char asidmap[128];
   struct QURTK_VTLB_TABLE_DESCRIPTOR t1;
};
extern unsigned QURTK_vtlb_ownership[];
extern unsigned short __attribute__((section(".data.ukernel.main"),aligned(32))) QURTK_page_table_v2[];
extern unsigned QURTK_tlb_available_count;
extern unsigned qurt_tlb_idx;
extern unsigned QURTK_tlb_first_replaceable;
extern unsigned QURTK_tlb_last_replaceable;
extern void *QURTK_pagetables[];
extern unsigned short QURTK_vtlb_tree[];
extern unsigned char QURTK_vtlb_asid_map[];
extern unsigned long long QURTK_vtlb_entries[];
extern unsigned QURTK_vtlb_extensions[];
extern unsigned QURTK_vtlb_entry_count;
extern unsigned QURTK_vtlb_tree_count;
extern unsigned QURTK_vtlb_prebiased_entry;
extern unsigned QURTK_vtlb_revision;
extern struct QURTK_VTLB_BASIC_TABLE QURTK_vtlb_basic_table;
extern typeof(int) QURTK_config_MAX_THREADS;





extern typeof(typeof(struct { unsigned char a[320] __attribute__((aligned(32))); }) []) QURTK_thread_contexts_array;
extern typeof(unsigned int) const QURTK_max_mailboxes;




extern typeof(typeof(struct { unsigned char a[32] __attribute__((aligned(8))); }) []) QURTK_mailbox_data;





extern typeof(unsigned long long) const QURTK_mailbox_msg_cycles;





extern typeof(unsigned long long) const QURTK_mailbox_max_cycles;





extern typeof(unsigned int) QURTK_config_debug_buffer_size;




extern typeof(unsigned char []) QURTK_debug_buf;
                                                                                ;





extern typeof(void * [2]) const QURTK_tbb;




extern typeof(void *) const QURTK_L1Dcache_dump;
extern typeof(void *) const QURTK_l2tcm_tag;





typedef struct
{
   unsigned int tbuf_size __attribute__((aligned(8)));
   unsigned int tbuf_mask;
}
QURTK_trace_mask_t;


extern typeof(unsigned int) QURTK_trace_buffer_size;



                                                  ;

extern typeof(QURTK_trace_mask_t) QURTK_config_trace_mask;






typedef enum {
   QURTOS_INIT_STAGE_ZERO,
   QURTOS_INIT_STAGE_GENERIC,
} qurtos_init_stage_t;

typedef void (*qurtos_init_funcptr)(qurtos_init_stage_t);
void qurtos_call_init_funcs(qurtos_init_stage_t stage);

void qurtos_process_reaper_init(unsigned, unsigned, void *, unsigned);
extern typeof(qurtos_init_funcptr []) const qurtos_config_init_funcs;
struct ramfs_scan_info {
   unsigned ppn_start;
   unsigned ppn_end;
   unsigned ppn_step;
};



extern typeof(struct ramfs_scan_info) QURTK_ramfs_scan;
extern typeof(unsigned char []) qurtos_island_heap;






extern typeof(int) QURTK_CONTEXT_SIZE;




extern typeof(int) QURTK_MAX_THREADS_IN_TCM;



                                              ;

extern typeof(unsigned int) QURTK_hthread_startup;




extern typeof(typeof(struct { unsigned char a[320] __attribute__((aligned(32))); }) []) QURTK_thread_contexts_tcm;





extern typeof(unsigned long long []) QURTK_thread_objs_array;
extern typeof(typeof(struct { unsigned char a[16] __attribute__((aligned(16))); }) []) QURTK_futex_objs_array;
extern typeof(unsigned int) qdsp6ss_base_addr;



extern typeof(unsigned int) QURTK_fast_l2vic_port;




extern typeof(unsigned int) QURTK_clade_cfg_base;



extern typeof(unsigned int) QURTK_clade_exc_hi_word;



extern typeof(unsigned int) QURTK_clade_dict_word;



extern typeof(unsigned int []) QURTK_clade_exception_hi;





extern typeof(unsigned int []) QURTK_clade_dict;
extern typeof(unsigned short []) QURTK_int_island;




                                         ;

extern typeof(typeof(struct { unsigned char a[4] __attribute__((aligned(4))); }) []) QURTK_l2_int_idx_table;





extern typeof(typeof(struct { unsigned char a[16] __attribute__((aligned(16))); }) []) QURTK_l2_island_interrupt_table;





extern typeof(unsigned int) QURTK_etm_cfg_base;





extern typeof(unsigned int) QURTK_ssc_atb_lphandshake;





extern typeof(unsigned int) QURTK_etm_atid;





extern typeof(unsigned int) QURTK_etm_atb_size;





extern typeof(unsigned int) QURTK_etm_enable_on_error;





extern typeof(unsigned int) QURTK_island_fail_tlbmiss;





extern typeof(unsigned int) QURTK_int_privilege;




                                                    ;


extern typeof(unsigned int) const QURTK_secure_proc_sid;





extern typeof(typeof(struct { unsigned char a[112] __attribute__((aligned(8))); }) []) QURTK_pcb_data;





extern typeof(unsigned int) QURTK_max_programs;






extern typeof(unsigned int []) QURTK_l2_enable;
extern typeof(const void * []) const QURTK_phys_relocs;





struct genreloc {
   void *ptr __attribute__((aligned(8)));
   int delta;
};


extern typeof(const struct genreloc []) const QURTK_gen_relocs;
int QURTK_srm_reject(unsigned pageno, unsigned pgcnt);
#endif /* GEN_CONFIG_HEADER_USED */
