/*=============================================================================

                                    cust_config_template.c

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
// S(adv,sec,base) sets the section attribute to (adv) for
//  CONFIG_ADVANCED_MEMORY builds, to (sec) for CONFIG_SECTIONS
//  builds, and to (base) for builds with neither flag

#if defined(CONFIG_ADVANCED_MEMORY)
#define S(adv,sec,base) __attribute__((section(adv)))
#elif defined(CONFIG_SECTIONS)
#define S(adv,sec,base) __attribute__((section(sec)))
#else
#define S(adv,sec,base) __attribute__((section(base)))
#endif

#ifdef CONFIG_SECTIONS
#define SECTION(s, a) __attribute__((section(s "." a)))
#else
#define SECTION(s, a)
#endif

#define CONFIG_DEFAULT                                            /* Expands to nothing, but can be a macro argument */
#ifdef GEN_CONFIG_HEADER
#define IDIM(x)
#define CONFIG_SEC_VAR(c,s,t,n,i)    extern typeof(t)       n
#define CONFIG_RW_VAR(c,t,n,i)       extern typeof(t)       n
#define CONFIG_RO_VAR(c,t,n,i)       extern typeof(t) const n
#define CONFIG_RW_SPACE(c,s,t,n,a,i) extern typeof(t)       n
#define CONFIG_ZI_SPACE(c,s,t,n,a)   extern typeof(t)       n
#define SET_CONFIG_VALUE(x,y)
#define DEFINE_MAPPING(v,xmlstr)
#define CONFIG_MAP_VAR(c,t,n,i)      extern typeof(t) const n
#define DEFINE_Q6CFG_MMAP(symbol, tlb_size_kib, mmap_tab_idx)
#else
#define IDIM(x) x
#define CONFIG_SEC_VAR(c,s,t,n,i)    typeof(t)       n   s = i    /* This (section config vars) will be deprecated soon */
#define CONFIG_RW_VAR(c,t,n,i)       typeof(t)       n     = i    /* This (R/W config vars) will be deprecated soon */
#define CONFIG_RO_VAR(c,t,n,i)       typeof(t) const n     = i    /* Preferred way to declare information variables */
#define CONFIG_RW_SPACE(c,s,t,n,a,i) typeof(t)       n a s = i    /* Declare an initialized space-providing variable */
#define CONFIG_ZI_SPACE(c,s,t,n,a)   typeof(t)       n a s        /* Declare an un-initialized space-providing variable */

#define EIP_BUILD_COPY               1
#define EIP_BUILD_XML                2
#define EIP_BUILD_MAP_RANGE          3
#define EIP_BUILD_MAP_VAR            4
#define EIP_BUILD_Q6CFG              5

/**
 * @brief Macro to setup EIP build record for a memory map that covers Q6 
 * configuration area. 'symbol' is the base virtual address of the memory 
 * map. Its value is patched by QuRT Image Builder
 */
#define DEFINE_Q6CFG_MMAP(symbol, tlb_size_kib, mmap_tab_idx) \
    extern unsigned int const symbol;                     \
    const struct tag_##symbol {                           \
        unsigned short len;                               \
        unsigned short type;                              \
        void const * addr;                                \
        unsigned short tlb_size;                          \
        unsigned short idx;                               \
    } eip_build_q6cfg_##symbol = {                        \
        sizeof( struct tag_##symbol ),                    \
        EIP_BUILD_Q6CFG,                                  \
        &symbol,                                          \
        tlb_size_kib,                                     \
        mmap_tab_idx                                      \
    }

#define SET_CONFIG_VALUE(x,y)                      \
   extern typeof(y) __attribute__((weak)) x;       \
   union __attribute__((packed)) padded_##x {      \
      typeof(x) value;                             \
      char pad[(sizeof(x)+3)&~3];                  \
   };                                              \
   __attribute__((section(".data.eip_build.build"))) \
   const struct tag_##x {                          \
      unsigned short len;                          \
      unsigned short type;                         \
      void *addr;                                  \
      unsigned copylen;                            \
      union padded_##x data;                       \
   } eip_build_##x = {                             \
      sizeof(struct tag_##x),                      \
      EIP_BUILD_COPY,                              \
      &x,                                          \
      sizeof(x),                                   \
      {y}                                          \
   };

#define DEFINE_MAPPING(v, xmlstr)                     \
   const struct tag_##v {                             \
      unsigned short len __attribute__((aligned(4))); \
      unsigned short type;                            \
      const void *addr;                               \
      unsigned rangelen;                              \
      char xml[sizeof(xmlstr)];                       \
   } eip_build_##v = {                                \
        sizeof(struct tag_##v),                       \
        EIP_BUILD_MAP_RANGE,                          \
        &v,                                           \
        sizeof(v),                                    \
        xmlstr                                        \
   };

#define DEFINE_MAPVAR(v)                              \
   const struct tag_##v {                             \
      unsigned short len __attribute__((aligned(4))); \
      unsigned short type;                            \
      const void *addr;                               \
      unsigned varlen;                                \
   } eip_build_##v = {                                \
        sizeof(struct tag_##v),                       \
        EIP_BUILD_MAP_VAR,                            \
        &v,                                           \
        sizeof(v)                                     \
   };

#define CONFIG_MAP_VAR(c,t,n,i)         typeof(t) const n = i; DEFINE_MAPVAR(n)

SET_CONFIG_VALUE(QURT_EIP_BUILD_PTR, (struct tag_QURT_EIP_BUILD_PTR *)0);
extern struct tag_QURT_EIP_BUILD_PTR QURT_EIP_BUILD[];
struct tag_QURT_EIP_BUILD_PTR *QURT_EIP_BUILD_PTR = QURT_EIP_BUILD;

const struct tag_sender {
   unsigned short len __attribute__((aligned(4)));
   unsigned short type;
   char data[{:XMLSENDERLENGTH}];
} eip_build_sender = {
   sizeof(struct tag_sender),
   EIP_BUILD_XML,
   {:XMLSENDER}
};

#include <string.h>

void QURTK_late_eip_build(void)
{
   struct tag_QURT_EIP_BUILD_PTR *p;

   p = QURT_EIP_BUILD_PTR;
   if (p) {
      while (p->len || p->type) {
         if (p->type == EIP_BUILD_COPY) {
                if(p->addr)
                memcpy(p->addr, &p->data, p->copylen);
         }
         p = (void *)((unsigned)p + p->len);
         }
      }
   }
#endif

#define CACHEALIGNED __attribute__((aligned(32)))
#define DIVUP(number,divisor) (((number)/(divisor))+(((number)%(divisor))>0))  /* Divide, rounding up */
#define ROUNDUP(number,divisor) (DIVUP((number),(divisor))*(divisor))          /* Round up to multiple */
#define ARRAY_INIT(size) [0 ... (size)-1]
#define ALIGNED_SPACE(size) typeof(struct { unsigned char a[size] __attribute__((aligned(size))); })   /* Suitable for simple declaration of aligned space */

#define THREAD_CONTEXT                             \
   typeof(struct {                                 \
         unsigned char a[{:SIZEOF_TCB}]            \
         __attribute__((aligned({:ALIGNOF_TCB}))); \
   })
#define KERNEL_PCB                                 \
   typeof(struct {                                 \
         unsigned char a[{:SIZEOF_PCB}]            \
         __attribute__((aligned({:ALIGNOF_PCB}))); \
   })
#define KERNEL_SPCB                                   \
   typeof(struct {                                    \
         unsigned char a[{:SIZEOF_SPCB}]              \
         __attribute__((aligned({:ALIGNOF_SPCB})));   \
   })
#define KERNEL_MAILBOX                                 \
   typeof(struct {                                     \
         unsigned char a[{:SIZEOF_MAILBOX}]            \
         __attribute__((aligned({:ALIGNOF_MAILBOX}))); \
   })

#if !defined (ISLAND_ONLY) || defined(GEN_CONFIG_HEADER)

CONFIG_RW_VAR("Priority for main thread",
              unsigned int,
              qurtos_main_priority,
              {:QURTKMAINPRIORITY});
CONFIG_RW_VAR("Bitmask for main thread",
              unsigned int,
              qurtos_main_bitmask,
              {:QURTKMAINBITMASK});

#if {:QURTOSSTACKSIZE} != 0
#define ACTUALQURTOSSTACKSIZE ({:QURTOSSTACKSIZE})
#elif defined(CONFIG_MP)
#define ACTUALQURTOSSTACKSIZE (0x7C0)
#else
#define ACTUALQURTOSSTACKSIZE (0x1C0)
#endif

CONFIG_SEC_VAR("Stack size for guest-OS stack associated with each user thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_stack_size,
              ROUNDUP(ACTUALQURTOSSTACKSIZE, 32));

#if {:QURTOSISLANDSTACKSIZE} != 0
#define ACTUALQURTOSISLANDSTACKSIZE ({:QURTOSISLANDSTACKSIZE})
#else
#define ACTUALQURTOSISLANDSTACKSIZE (ACTUALQURTOSSTACKSIZE)
#endif

SET_CONFIG_VALUE(qurtos_island_stack_size, ROUNDUP(ACTUALQURTOSISLANDSTACKSIZE, 32));

struct objcache_config {
   short memory_blocks;
   short virt_regions;
   short phys_regions;
   short user_processes;
   short user_threads;
   short qurtos_threads;
   short shmem_regions;
};
#define OBJCACHE_DEFAULT (-1)
SET_CONFIG_VALUE(qurtos_objcache_config, ((struct objcache_config){{:OBJCACHECONFIG}}));

CONFIG_SEC_VAR("Priority for reaper thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_reaper_priority,
              {:QURTOS_REAPER_PRIORITY});
CONFIG_SEC_VAR("Tcb partition for reaper thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_reaper_tcb_partition,
              {:QURTOS_REAPER_TCB_PARTITION});
CONFIG_SEC_VAR("Stack size for reaper thread",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_reaper_stack_size,
              {:QURTOSREAPERSTACKSIZE});

CONFIG_RW_VAR("Qurt PIMUTEX priority",
              unsigned int,
              QURTK_max_pi_prio,
              {:MAXPIMUTEXPRIO});

CONFIG_SEC_VAR("Set to non-zero to cause qurtos to have a separate virtual pool for shared memory",
               __attribute__((section(".data.qurtos"))),
               unsigned int,
               qurtos_split_virt_pool,
               {:SPLITVIRTPOOL});
CONFIG_RO_VAR("Set autostack size total memory, this is the size of image pool", 
              unsigned int,
              qurtos_autostack_total_pages,
              {:AUTOSTACKTOTALPAGES});

CONFIG_SEC_VAR("Set pager size total memory, this is the size of image pool",
              __attribute__((section(".data.export"))),
              unsigned int,
              qurtos_pager_total_pages,
              {:PAGERTOTALPAGES});
CONFIG_RW_SPACE("Stack space for main thread",
                __attribute__((section(".data.qurtos"))),
                unsigned char [],
                qurtos_app_stack,
                CACHEALIGNED,
                {ARRAY_INIT(ROUNDUP({:MAINSTACKSIZE},8)) = 0xF8});

CONFIG_RW_VAR("Stack size for main thread",
              unsigned int,
              qurtos_main_stack_size,
              sizeof(qurtos_app_stack));

CONFIG_SEC_VAR("How many bytes at the beginning of each thread stack should be filled with F8F8F8F8 watermark?",
               __attribute__((section(".data.export"))),
              unsigned int,
              qurt_osam_stack_fill_count,
              {:OSAMSTACKFILLCOUNT});

CONFIG_SEC_VAR("How many bytes should be filled with F8F8F8F8 watermark at each step to check stack usage?",
               __attribute__((section(".data.export"))),
              unsigned int,
              qurt_osam_stack_usage_fill_count,
             (ROUNDUP({:OSAMSTACKUSAGEFILLCOUNT},4)));
CONFIG_RO_VAR("", 
              unsigned int,
              qurt_stid_reserve_num,
              {:STIDRESNUM});

enum config_heap_type {
   QURTOS_CONFIG_HEAP_STATIC,   /* Statically allocated */
   QURTOS_CONFIG_HEAP_DYNAMIC,  /* Dynamically allocated */
   QURTOS_CONFIG_HEAP_ASLR,     /* Dynamically allocated at random address */
   QURTOS_CONFIG_HEAP_APPREGION /* Not allocated by QuRT -- left up to application */
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

#if {:QURTOSAPPHEAPTYPE} == 0
#if !defined(GEN_CONFIG_HEADER)
static unsigned char qurtos_app_heap[ROUNDUP({:HEAPSIZE},4)] CACHEALIGNED __attribute__((section(".bss.qurtos")));
#endif
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_STATIC, qurtos_app_heap, sizeof(qurtos_app_heap) }
#elif {:QURTOSAPPHEAPTYPE} == 1
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_DYNAMIC, 0, ROUNDUP({:HEAPSIZE},0x1000) }
#elif {:QURTOSAPPHEAPTYPE} == 2
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_ASLR, 0, ROUNDUP({:HEAPSIZE},0x1000) }
#elif {:QURTOSAPPHEAPTYPE} == 3
#define QURTOSAPPHEAPINIT { QURTOS_CONFIG_HEAP_APPREGION, 0, ROUNDUP({:HEAPSIZE},0x1000) }
#endif

#if {:AUTOSTACKTOTALPAGES} != 0
unsigned int QURTK_autostack_array[{:AUTOSTACKTOTALPAGES}] CACHEALIGNED __attribute__((section(".bss.kernel")));
#define QURTKAUTOSTACKINIT { QURTK_autostack_array, sizeof(QURTK_autostack_array), {:AUTOLO}, {:AUTOHI}, {:AUTOINITSIZE} }
#else
#define QURTKAUTOSTACKINIT { 0, 0, 0, 0, 0 }
#endif

SET_CONFIG_VALUE(qurtos_island_handle_count, {:QURTOSISLANDHANDLES});
SET_CONFIG_VALUE(qurtos_qdi_handle_count, {:QURTOSQDIHANDLES});

CONFIG_SEC_VAR("Guest-OS application heap configuration",
              __attribute__((section(".data.export"))),
              struct config_heap_info,
              qurtos_app_heap_cfg,
              QURTOSAPPHEAPINIT);

CONFIG_RW_VAR("QURTK auto stack configuration",
              struct config_autostack_info,
              QURTK_autostack_array_cfg,
              QURTKAUTOSTACKINIT);

CONFIG_SEC_VAR("Hi watermark for autostack",
                __attribute__((section(".data"))),
                unsigned int,
                QURTK_hi_water_mark, 
                {:AUTOHI});
                
CONFIG_SEC_VAR("Lo watermark for autostack",
                __attribute__((section(".data"))),
                unsigned int,
                QURTK_lo_water_mark,
                {:AUTOLO});

CONFIG_RW_VAR("Timetest port number",
              unsigned int,
              QURTK_timetest_port,
              {:TIMETESTPORT});

CONFIG_SEC_VAR("The address of IMEM that contains the flag for initializing ISDB",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_isdb_flag_imem_addr,
               {:ISDBIMEMADDR});

CONFIG_SEC_VAR("Flag to enable ETM trace to Q6ETB",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_to_q6etb,
               {:ETMTOQ6ETB});

#define Q6ETBDUMP {{:INTIQ6ETBDUMP}}
CONFIG_RW_VAR("Q6ETB dump",
              unsigned int [],
              qurt_q6etb_dump,
              Q6ETBDUMP);

CONFIG_RW_VAR("Interrupts that wake up the processor during TCXO shutdown; Most significant bit is INT 0",
              unsigned int,
              QURTK_tcxo_intmask,
              {:TCXOINTMASK});

CONFIG_SEC_VAR("flag to enable DMT scheduling",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_dmt_enabled,
               {:DMT_ENABLED});

CONFIG_SEC_VAR("flag to enable bus resources sharing",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_BQ_shared,
               {:BQ_SHARED});

// QURTKTCMDUMPTYPE = 0: This is used in the error hndlr when "tcm" tag is not present
// in the config xml.
// QURTKTCMDUMPTYPE = 1: This is used in the error hndlr when "tcm" tag is present
#if {:QURTKTCMDUMPTYPE} == 0
CONFIG_RW_SPACE("Space for tcm tag is not defined in the config xml",
                __attribute__((section(".data.qurtos"))),
                unsigned char [],
                qurt_tcm_dump_static_loc,
                CACHEALIGNED,
                {ARRAY_INIT(ROUNDUP({:TCMSIZE},32)) = 0xFF});

#endif

CONFIG_ZI_SPACE("Space for TCM dump in the error handler",
                CONFIG_DEFAULT,
                unsigned char *,
                QURTK_tcm_err_dump,
                CONFIG_DEFAULT);


CONFIG_ZI_SPACE("Size of TCM dump in the error handler",
                CONFIG_DEFAULT,
                unsigned int,
                QURTK_tcm_err_dump_size,
                CONFIG_DEFAULT);

CONFIG_RW_VAR("Flag variable to know if TCM Dump is defined by user or QuRT",
              int,
              QURTK_tcm_dump_type,
              {:QURTKTCMDUMPTYPE});

// qurt_tcm_dump is a pointer. It will be pointing to qurt_tcm_dump_pool if "pool" is
// defined in tcm tag in the config xml and used for saving tcm going into airplane mode.
// If tcm_dump tag is not present, then it'll point to the qurt_tcm_dump_static_loc
CONFIG_SEC_VAR("Pointer to tcm save location during power collapse",
               __attribute__((section(".data"))),
               unsigned char *,
               qurt_tcm_dump,
               0);

CONFIG_SEC_VAR("Size for TCM dump",
               __attribute__((section(".data.export"))),
              unsigned int,
              qurt_tcm_dump_size,
              (ROUNDUP({:TCMSIZE},32)));

//this is the dynamically set size via modified qurt_power_control API
CONFIG_RW_VAR("Size for TCM save config, can be changed during execution",
              unsigned int,
              qurt_tcm_save_size,
              {:TCMSAVESIZE});

/****************************************************************************
 * Default SSR (Supervisor Status Register) settings                        *
 * Read Q6 Architecture system-level Specification before change            *
 * Incorrect setting of any bit will cause runtime system failure           *
 * bit 0 is LSB                                                             *
 * bit 16:  the privilege mode the SW thread will be running                *
 *          1->user mode (default); 0->supervisor mode                      *
 * bit 22:  BadVa selector.                                                 *
 *          1->BadVA1; 0->BadVA0 (default)                                  *
 * bit 23:  enable/disable reading PCYCLE register                          *
 *          1->enabled (default); 0->disabled                               *
 * bit 24:  enable/disable reading PMU                                      *
 *          1->enabled (default); 0->disabled                               *
 * bit 27-29:  XA coprocessor selection                                     *
 *          4->selection (default); 0->disabled                               *
 *                                                                          *
 *  Default SSR bits from MSB:  0010,0001,1000,0111, 0000,0000,0000,0000    *
 ****************************************************************************/

CONFIG_RW_VAR("Default SSR value",
              unsigned int,
              QURTK_ssr_default,
              0x21870000
#if defined(CONFIG_MP)
              | 0x00080000      /* Enable guest OS mode initially */
#endif //defined(CONFIG_MP)
              );
SET_CONFIG_VALUE(qurtos_ssr_default, 0x218F0000);


/****************************************************************************
 * Default CCR (Cache Control Register) settings                            *
 *                                                                          *
 *   bits 0-1, 3-4, 6-7 are cache CP bits, and set to 0 to use CCCC attr    *
 *                                                                          *
 *   bit 16:  HFI for L1 I cache hardware prefetch                          *
 *          1->honor the USR[HFI] settings(default); 0->disabled            *
 *   bit 17:  HFD for L1 D cache hardware prefetch                               *
 *          1->honor the USR[HFD] settings(default); 0->disabled            *
 *   bit 18:  HFI-L2 for L2 I cache hardware prefetch                       *
 *          1->honor the USR[HFIL2] settings(default); 0->disabled          *
 *   bit 19:  HFD-L2 for L2 D cache hardware prefetch                       *
 *          1->honor the USR[HFDL2] settings(default); 0->disabled          *
 *   bit 20:  SFD for  DCFECTCH/L2FETCH                                     *
 *          1->enalble DCFETCH/L2FETCH based on CCCC and partition(default) *
 *          0->disable DCFETCH and L2FETCH                                  *
 *                                                                          *
 *  Default CCR bits from MSB:  0000,0000,0001,0101, 0000,0000,0000,0000    *
 ****************************************************************************/

CONFIG_SEC_VAR("Default CCR value",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_config_ccr_default,
               {:CACHEATTR});
SET_CONFIG_VALUE(qurtos_ccr_default, {:CACHEATTR});

/****************************************************************************
 * Default USR Cache Prefetch settings                                      *
 *                                                                          *
 *   bit 14-13: HFD.  Two bits for L1 data cache prefetching.               *
 *          00: No prefetching  (default)                                   *
 *          01: Prefetch Loads with post-updating address mode when         *
 *          execution is within a hardware loop.                            *
 *          Prefetch up to 4 cache lines.                                   *
 *          10: Prefetch any hardware-detected striding Load when           *
 *          execution is within a hardware loop.                            *
 *          Prefetch up to 4 cache lines.                                   *
 *          11: Prefetch any hardware-detected striding Load.               *
 *          Prefetch up to 4 cache lines.                                   *
 *   bit 16-15: HFI.  Two bits for L1 instruction cache prefetching.        *
 *          00: No prefetching                                              *
 *          01: prefetching of at most 1 additional cache line (default)    *
 *          10: prefetching of at most 2 additional cache lines             *
 *   bit 17:  This bit is reserved.                      *
 *   bit l9-18: HFI-L2. Two bits for L2 prefetching for instructions.       *
 *          00 = Disable L2 instruction prefetching for this thread.        *
 *          01 = Enable L2 instruction prefetching of 1 line.               *
 *          10 = Enable L2 instruction prefetching of 2 lines (default).    *
 *          11 = Enable L2 instruction prefetching of 4 lines.              *
 *                                                                          *
 *  Default USR bits from MSB:  0000,0000,0000,0101, 0000,0000,0000,0000    *
 ****************************************************************************/

CONFIG_SEC_VAR("Default USR value",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_config_usr_default,
               {:USRATTR});
SET_CONFIG_VALUE(qurtos_usr_default, {:USRATTR});



/****************************************************************************
 * Configure L2 cache size                                                  *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The value in this variable doesn't have the exact size number       *
 *      in Kilobytes. Instead, this variable has the value that is          *
 *      assigned to register - SYSCFG:L2CFG. Please refer to Hexagon        *
 *      Architecture system-level specification for more information        *
 *      about system register SYSCFG and L2 cache size configuration.       *
 *      The valid L2 cache size is 0KB, 64KB, 128KB, 256KB, 512kB(V4M).     *
 *      The default size is set by Qurt to 128KB.                           *
 *      - 0 (sets 0KB L2 cache size)                                        *
 *      - 1 (sets 64KB L2 cache size)                                       *
 *      - 2 (sets 128KB L2 cache size)                                      *
 *      - 3 (sets 256KB L2 cache size)                                      *
 *      - 4 (sets 512KB L2 cache size)                                      *
 *      - 5 (sets 1024KB L2 cache size)                                      *
 *      - 100 (sets entire L2/TCM area as L2 cache)                         *
 ****************************************************************************/
#define kb_0   0
#define kb_64  1
#define kb_128 2
#define kb_256 3
#define kb_512 4
#define kb_1024 5
#define kb_full 100

CONFIG_SEC_VAR("L2 cache size configuration",
               __attribute__((section(".start"))),
               unsigned char,
               const QURTK_l2cache_size,
               {:L2SIZE});

CONFIG_RO_VAR("L1 Icache size configuration",
               unsigned int,
               QURTK_l1_icache_size,
               {:L1ICSIZE});
               
CONFIG_RO_VAR("L1 Dcache size configuration",
               unsigned int,
               QURTK_l1_dcache_size,
               {:L1DCSIZE});

/****************************************************************************
 * L2 cache non-write-allocate setting                                      *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L2 cache NWA is set to false by default if no input from XML.   *
 ****************************************************************************/

#define ENABLED     1
#define DISABLED    0
CONFIG_SEC_VAR("L2 cache non-write-allocate configuration",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_l2cache_nwa,
               {:L2CACHENWA});


/****************************************************************************
 * L2 cache write-back setting                                              *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L2 cache write-back support is only available on V3+ target.    *
 *      By default, L2 cache is write-through.                              *
 ****************************************************************************/

#define ENABLED     1
#define DISABLED    0
CONFIG_SEC_VAR("L2 cache write-back configuration",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_l2cache_wb,
               {:L2CACHEWB});


/****************************************************************************
 * I-cache enabling                                      *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The I-cache is enabled by default.                                  *
 *      This selection is only used for special needs (e.g. sw workaround). *
 ****************************************************************************/

#define ENABLED     1
#define DISABLED    0
CONFIG_SEC_VAR("I Cache Enabling",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_icache_enable,
               {:ICACHEENABLE});

/****************************************************************************
 * L1 cache size setting                                                    *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Enabling Half Icache or Dcache is only available on V3+ target.     *
 *      By default, Full I/D cache is used.                                 *
 ****************************************************************************/

CONFIG_SEC_VAR("L1 cache size setting",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_HI_cache,
               DISABLED);
CONFIG_SEC_VAR("L1 cache size setting",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_HD_cache,
               DISABLED);

/****************************************************************************
 * L1 cache partition settings                                              *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      The L1 data and instruction caches can be independently             *
 *      partitioned into two regions, a main partition and an auxiliary     *
 *      partition. The partitions are controlled by the ICPART and          *
 *      DCPART RW-bits in SYSCFG register. The feature is only              *
 *      available on v3+ targets.                                           *
 *                                                                          *
 *      The actual size of each cache partition also depends on the L1      *
 *      cache size setting. The following values are allowed for            *
 *      QURTK_DC_partition and QURTK_IC_partition:                        *
 *      - 0 (L1 cache is shared by all threads)                             *
 *      - 1 (L1 cache is split into two equal parts. Both main and          *
 *          auxiliary sections use 1/2 the cache)                           *
 *      - 2 (L1 cache is split into two parts. The main section is 3/4      *
 *          of the capacity, while the auxiliary section uses 1/4)          *
 *      - 3 (reserved)                                                      *
 ****************************************************************************/
#define full          0
#define half          1
#define three_quarter 2
#define resv          3

CONFIG_SEC_VAR("L1 data cache partition",
               __attribute__((section(".data"))),
               unsigned char,
               QURTK_DC_partition,
               {:L1DP});

CONFIG_SEC_VAR("L1 instruction cache partition",
               __attribute__((section(".data"))),
               unsigned char,
               QURTK_IC_partition,
               {:L1IP});

/****************************************************************************
 * Static (build-time) value for SYSCFG                                     *
 *                                                                          *
 * NOTES:                                                                   *
 *      Contains build-time configured bits/fields of SYSCFG as indicated   *
 *      by the mask. Only those bits/field indicated by mask are inserted   *
 *      into SYSCFG at boot in crt0.S:configure_adv_syscfg. The following   *
 *      variables are considered deprecated ...                             *
 *          - QURTK_BQ_shared                                               *
 *          - QURTK_dmt_enabled                                             *
 *          - QURTK_l2cache_nwa                                             *
 *          - QURTK_l2cache_wb                                              *
 *          - QURTK_l2cache_size                                            *
 *          - QURTK_DC_partition                                            *
 *          - QURTK_IC_partition                                            *
 *          - QURTK_L2_partition                                            *
 ****************************************************************************/
SET_CONFIG_VALUE(QURTK_syscfg_static, {:SYSCFGSTATIC});
SET_CONFIG_VALUE(QURTK_syscfg_static_mask, {:SYSCFGSTATICMASK});

/****************************************************************************
 * L2 Configuration Registers                                               *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Variables to hold a table of L2 configuration registers with        *
 *      initialization values provided during build-time (XML)              *
 ****************************************************************************/
#define L2CFG_TAB_LEN {:L2CFGTABLEN}
CONFIG_RO_VAR( "Number of entries in L2 configuration register table",
               unsigned int,
               QURTK_l2cfg_tab_len,
               L2CFG_TAB_LEN );

#if L2CFG_TAB_LEN > 0

#define L2CFG_TAB {{:L2CFGTAB}}

typedef struct l2cfg_tab_entry_s {
    unsigned int offset;
    unsigned int value;
} l2cfg_tab_entry_t;

CONFIG_RW_VAR( "L2 configuration register table",
               struct l2cfg_tab_entry_s [],
               QURTK_l2cfg_tab,
               L2CFG_TAB );
#endif

/****************************************************************************
 * L2 cache partition settings                                              *
 *                                                                          *
 * NOTES:                                                                   *
 *      L2 cache is partitioined into two regions, a main partition and     *
 *      an auxiliary. This are controlled by the L2PART in SYSCFG           *
 *      This feature is only available on v4+ targets.                      *
 *                                                                          *
 *      - 0 (L2 cache is fully shared)                                      *
 *      - 1 (L2 cache is split into two equal parts. Both main and          *
 *          auxiliary sections use 1/2 the cache)                           *
 *      - 2 (L2 cache is split into two parts. The main section is 3/4      *
 *          of the capacity, while the auxiliary section uses 1/4)          *
 *      - 3 (L2 cache is split into two parts. The main section is 7/8      *
 *          of the capacity, while the auxiliary section uses 1/8)          *
 ****************************************************************************/

// reuse the definitions on the above from 0 to 2, and add definition for 7/8
#define seven_eighths    3

CONFIG_SEC_VAR("L2 cache partition",
               __attribute__((section(".data"))),
               unsigned char,
               QURTK_L2_partition,
               {:L2CP});


/****************************************************************************
 * Chicken bit settings (for HW bug workaround)                             *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_CHICKEN_BITS                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 ****************************************************************************/
/* Chicken bit settings */

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_rgdr,
               {:QURTKCHICKENBITSRGDR});

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_duck,
               {:QURTKCHICKENBITSDUCK});

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_turkey,
               {:QURTKCHICKENBITSTURKEY});

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_chicken,
               {:QURTKCHICKENBITSCHICKEN});

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_chicken_bits_rgdr_2nd,
               {:QURTKCHICKENBITS2NDRGDR});
CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_chicken_2nd,
               {:QURTKCHICKENBITS2NDCHICKEN});

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_chicken_bits_rgdr_3rd,
               {:QURTKCHICKENBITS3RDRGDR});
CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_chicken_bits_chicken_3rd,
               {:QURTKCHICKENBITS3RDCHICKEN});

#define SUPP_RGDR_INIT {{:QURTKSUPRGDR}}
CONFIG_RW_VAR("Supplement rgdr bits",
              unsigned int [16],
              QURTK_supplement_rgdr,
              SUPP_RGDR_INIT);

#define SUPP_RGDRREV_INIT {{:QURTKSUPREVRGDR}}
CONFIG_RW_VAR("Supplement rgdr revid",
              unsigned int [16],
              QURTK_supplement_rgdr_revid,
              SUPP_RGDRREV_INIT);

#define SUPP_TURKEY_INIT {{:QURTKSUPTURKEY}}
CONFIG_RW_VAR("Supplement turkey bits",
              unsigned int [16],
              QURTK_supplement_turkey,
              SUPP_TURKEY_INIT);

#define SUPP_TURKEYREV_INIT {{:QURTKSUPREVTURKEY}}
CONFIG_RW_VAR("Supplement turkey revid",
              unsigned int [16],
              QURTK_supplement_turkey_revid,
              SUPP_TURKEYREV_INIT);

#define SUPP_DUCK_INIT {{:QURTKSUPDUCK}}
CONFIG_RW_VAR("Supplement duck bits",
              unsigned int [16],
              QURTK_supplement_duck,
              SUPP_DUCK_INIT);

#define SUPP_DUCKREV_INIT {{:QURTKSUPREVDUCK}}
CONFIG_RW_VAR("Supplement duck revid",
              unsigned int [16],
              QURTK_supplement_duck_revid,
              SUPP_DUCKREV_INIT);

#define SUPP_CHICKEN_INIT {{:QURTKSUPCHICKEN}}
CONFIG_RW_VAR("Supplement chicken bits",
              unsigned int [16],
              QURTK_supplement_chicken,
              SUPP_CHICKEN_INIT);

#define SUPP_CHICKENREV_INIT {{:QURTKSUPREVCHICKEN}}
CONFIG_RW_VAR("Supplement chicken revid",
              unsigned int [16],
              QURTK_supplement_chicken_revid,
              SUPP_CHICKENREV_INIT);

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_rgdr_num,
               {:QURTKSUPNORGDR});

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_turkey_num,
               {:QURTKSUPNOTURKEY});

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_duck_num,
               {:QURTKSUPNODUCK});


CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_supplement_chicken_num,
               {:QURTKSUPNOCHICKEN});

CONFIG_SEC_VAR("",
               __attribute__((section(".start"))),
               unsigned int,
               const QURTK_livelock_data,
               {:QURTKLIVELOCKDATA});

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_hvx_check_power_lock,
               {:CHKHVXPWRLOCK});

CONFIG_SEC_VAR("",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_hvx_default_mode,
               {:HVXDEFAULTMODE});


/****************************************************************************
 * Qurt Timer or Qtimer definitions                                        *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_QTIMERS                                                      *
 *                                                                          *
 * NOTES:                                                                   *
 *      Refer to your target HW reference manual for configuration          *
 *      of RGPT timer. Make sure the timer base address is one of           *
 *      the physical memory pools. Interrupt number does not conflict       *
 *      with RTOS reserved interrupts (check sched_config.h).               *
 ****************************************************************************/
CONFIG_SEC_VAR("",
               __attribute__((section(".data.export"))),
              unsigned int,
              QDSP6_QURT_TIMER_BASE,
              {:QDSP6QTIMERBASE});
CONFIG_SEC_VAR("",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURT_timer_intno,
              {:TIMERINTNO});
CONFIG_SEC_VAR("//254",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURT_timerIST_priority,
              {:TIMERISTPRIORITY});
CONFIG_SEC_VAR("",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURT_timerIST_tcb_partition,
              {:TIMERISTTCBPARTITION});


CONFIG_RO_VAR("",
              unsigned int,
              QURTK_dbg_cfg_mask,
              {:QDSP6SSDBGCFGMASK});

CONFIG_RO_VAR("",
              unsigned int,
              QURTK_WB_dbg_cfg_mask,
              {:WBQDSP6SSDBGCFGMASK});

SET_CONFIG_VALUE(qurtos_timer_freq_init,{:TIMERFREQINIT});

/****************************************************************************
 * Qurt Memory Pool Configuration                                          *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_DYNAMIC_MEMORY                                               *
 *                                                                          *
 * NOTES:                                                                   *
 *      QURT can have unlimited number of pools and each pool have a name  *
 *      and it can have up to 16 ranges. No gaps are allowed in ranges, e.g *
 *      if RANGE 2 is not defined in a pool the following range definitions,*
 *      e.g RANGE 3, RANGE 4 etc., are ignored                              *
 ****************************************************************************/
#if defined(CONFIG_DYNAMIC_MEMORY) || defined(GEN_CONFIG_HEADER)
struct phys_mem_pool_config{
    char name[32];
    struct range {
        unsigned int start;
        unsigned int size;
    } ranges[16];
};

#define POOLSTART(name) {name, {
#define POOLREGION(addr,size) {addr, size},
#define POOLEND {0}}},
#define POOL_CONFIGS_INIT {{:PHYSPOOLS}}
CONFIG_SEC_VAR("Memory pool config information",
               __attribute__((section(".data.qurtos"))),
               struct phys_mem_pool_config [],
               pool_configs,
               POOL_CONFIGS_INIT);
#endif /* QURT_DYNAMIC_MEMORY */

void qurtos_pool_image_fixups(void (*pfn)(const char *,void *,void *))
#ifdef GEN_CONFIG_HEADER
;
#else
{:IMAGEPOOLINSERT}
#endif

CONFIG_SEC_VAR("Number of entries to be locked in TLB",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_tlblock_entries,
               {:TLBLOCKNUM});

CONFIG_RW_VAR(" Number of TLB entries to be reserved strictly for dynamic (page-table-based) memory mappings   "
              "   The default for this value is -1, which tells QuRT to synthesize a value based on the        "
              "   deprecated variable config_tlb_first_replaceable.                                            "
              " A minimum reasonable number for this value is 4 times the number of HW execution units         "
              "   on the processor; so 12 for v5, and 16 for v55 or v56.                                       "
              " This value may be set to zero, which disables page tables entirely.                            ",
              int,
              QURTK_dynamic_tlb_reserve,
              {:DYNAMICTLBRESERVE});

CONFIG_RW_VAR("",
              unsigned int,
              config_tlb_first_replaceable,
              {:TLBFIRSTREPL});

CONFIG_RO_VAR("TLB debug level (0 for no debugging)",
              unsigned int,
              QURTK_tlb_debug_level,
              {:TLBDEBUGLEVEL});

#if {:TLBDEBUGLEVEL} || defined(GEN_CONFIG_HEADER)

CONFIG_ZI_SPACE("TLB debug space",
                CONFIG_DEFAULT,
                ALIGNED_SPACE(32) [IDIM(4096)],
                QURTK_tlb_debug_space __attribute__((weak)),
                CONFIG_DEFAULT);

#endif

/* configure QURTK_min_image_page for image alignment */
CONFIG_SEC_VAR("Min_image_page for image alignment",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_min_image_page,
               {:MINIMAGEPAGE});

/*
 * Configure static mappings array to add device memory mappings or shared
 * memory mapping
 *
 * vpn:         virtual page number
 * ppn:         physical page number
 * perm:        permission
 * cached_attr: cacheability
 * pgsize:      page size
 * others:      un-used
 *
 * Note: vpn and ppn has to be number of 4KB
 *
 * Note: please contact Qualcomm Technologies Inc. to verify the change.
 *       Incorrect change will crash the build in run time!
 */

#define R    1
#define W    2
#define RW   3
#define WR   3
#define X    4
#define RX   5
#define XR   5
#define WX   6
#define XW   6
#define RWX  7
#define XWR  7
#define KRN  0 /*< Accessible by kernel only */

#define MAIN  0
#define AUX   1

#define MEMORY_MAP(glob, asid, vpn, umode, busattr, perm, cache_attr, pgsize, cache_part, ppn) \
   ( ((unsigned long long)((1 << 31) | (glob << 30) | ((ppn & 0x800000) << 6) | (busattr << 27) | (asid << 20)|vpn) << 32 ) \
   | ( ((perm << 29) & 0xe0000000) | ((umode & 1) << 28) | (cache_attr << 24) | ((ppn & 0x7FFFFF) << 1) | pgsize))

#define MMAP_TABLE_INIT { {:MEMORY_MAPS} 0 }
CONFIG_SEC_VAR("Empty1 0x008000- 0x070000 size = 0x068000 (416k)",
               __attribute__((section(".data.qurtos"))),
              unsigned long long [],
              qurtos_mmap_table,
              MMAP_TABLE_INIT);

CONFIG_SEC_VAR("Pointer to first unlocked entry in mmap table",
               __attribute__((section(".data.qurtos"))),
               unsigned long long *,
               qurtos_mmap_unlocked,
               &qurtos_mmap_table[{:TLBLOCKNUM}]);

struct unlock_after_boot {
   unsigned vpageno;
   unsigned pagecnt;
};

#define UNLOCK_AFTER_BOOT_INIT {:UNLOCKAFTERBOOT}

CONFIG_SEC_VAR("List of memsections to be unlocked after boot",
               __attribute__((section(".data.qurtos"))),
               struct unlock_after_boot [],
               qurtos_unlock_after_boot,
               UNLOCK_AFTER_BOOT_INIT);

#ifndef GEN_CONFIG_HEADER
{:MAPEXTERNDECLS}
#endif

struct boot_mapping {
   unsigned long long template;
   char *start_ptr;
   char *end_ptr;
};

#define BOOT_MAPPING_INIT {{:BOOTMAPDEFINITIONS}}
CONFIG_SEC_VAR("Symbol-based boot mappings",
               __attribute__((section(".data.qurtos"))),
               struct boot_mapping [],
               qurtos_boot_mappings,
               BOOT_MAPPING_INIT);

#define VIRT_RESERVE_INIT { {:VIRT_RESERVE_RANGES} -1 }
CONFIG_SEC_VAR("Reserved virtual address ranges",
               __attribute__((section(".data.qurtos"))),
               int [],
               qurtos_virt_reserved,
               VIRT_RESERVE_INIT);

CONFIG_SEC_VAR("L2 cache partition data",
               __attribute__((section(".data.qurtos"))),
              unsigned int,
              QURTK_l2cache_partition_cfg,
              {:CACHEL2PARTDATA});

CONFIG_SEC_VAR("Internal Bus Priority data",
               __attribute__((section(".data.qurtos"))),
              unsigned int,
              QURTK_bus_prio_cfg,
              {:INTERNALBUSPRIO});

/****************************************************************************
 * Qurt L1 interrupt Configuration                                          *
 *                                                                          *
 * Dependent Features:                                                      *
 *      N/A                                                                 *
 *                                                                          *
 * NOTES:                                                                   *
 *      Use QURTK_IEL to configure interrupt is                             *
 *      - level triggered (bit value = 0)                                   *
 *      - edge triggered (bit value = 1)                                    *
 *      Use QURTK_IAHL to configure interrupt is                            *
 *      - Active high (or rising edge) (bit value = 1)                      *
 *      - Active low (or falling edge) (bit value = 0)                      *
 *      interrupt 0 is at MSB.                                              *
 ****************************************************************************/

CONFIG_RW_VAR("0xffffffff",
              unsigned int,
              QURTK_IEL,
              0xffffffff);

/* 1 - Active High, 0 - Active Low. All bits are 1. Rising edge or active high */
CONFIG_RW_VAR("0xffffffff",
              unsigned int,
              QURTK_IAHL,
              0xffffffff);

CONFIG_RW_VAR("Bitmask of L1 interrupts allowed for nesting",
              unsigned int,
              QURTK_int_nest_allow,
              {:L1NESTALLOW});

/****************************************************************************
 * Qurt L2 interrupt Configuration                                         *
 *                                                                          *
 * Dependent Features:                                                      *
 *      CONFIG_L2_INTERRUPT_ENABLED                                         *
 *                                                                          *
 * NOTES:                                                                   *
 *      Qurt kernel currently support up to 1024 L2VIC interrupts           *
 ****************************************************************************/

/* QURTK_l2_type[N] defines second level interrupt types, in which each bit
 * sets up the type of a L2 interrupt
 */
#define L2TYPE_INIT {{:QURTKSIRCTYPE}}
CONFIG_RW_VAR("interrupt 0 -> QURTK_l2_type[0]"
              "define L2 interrupt 0's type in LSB of QURTK_l2_type[N]"
              "0->level triggered; 1->edge triggered",
              unsigned int [32],
              QURTK_l2_type,
              L2TYPE_INIT);

#define ISR_INIT {{:ISRDATA}}
CONFIG_RW_VAR("isr_int_no | isr_cb_no",
              unsigned short [],
              QURTK_isr_data,
              ISR_INIT);

/* Program Name length, set to the same value of QURT_PROCESS_ATTR_NAME_MAXLEN */
#define PROGRAM_NAME_LEN 64
#define PROGRAM_NAMES {{:QURTKPROGNAMES}}
CONFIG_SEC_VAR("Program names",
               __attribute__((section(".data.qurtos"))),
              char [][PROGRAM_NAME_LEN],
              QURTK_program_names,
              PROGRAM_NAMES);

/* This variable should eventually be filled in just from the XML;
   until we have a special tag in XML to designate the root process,
   we need to defer the computation of this variable.  Not until
   we run qurt-image-build do we actually have the information to
   figure out which slot corresponds to the root ELF.
   For now, if qurt-image-build.py sees this value as a negative
   number less than -1, it will attempt to fill the variable in
   with the information it has. */
CONFIG_SEC_VAR("Slot in QURTK_program_names which is a placeholder for the guest-OS process",
               __attribute__((section(".data.qurtos"))),
               int,
               QURTK_root_prog_no,
               -2);

CONFIG_SEC_VAR("",
               SECTION(".data", "QURTK.INTERRUPT"),
               unsigned int,
               QURTK_int_usrpd_check,
               {:CHKUSRPDINT});

CONFIG_SEC_VAR("framekey usage",
               __attribute__((section(".data.export"))),
              unsigned int,
              QURTK_stack_framekey_enabled,
              {:STACKFRAMEKEYEN});

/* Define stack memory for fast interrupt callback */

CONFIG_ZI_SPACE("",
                __attribute__((section(".bss.qurtos.qurt_fastint_stack.fast"))),
                ALIGNED_SPACE(32) [IDIM({:QURT_CONFIG_MAX_HTHREADS}*DIVUP({:FASTINTSTACKSIZE},32))],
                qurt_fastint_stack,
                CONFIG_DEFAULT);

CONFIG_SEC_VAR("",
                __attribute__((section(".data.qurtos.qurt_fastint_stack_size.fast"))),
               unsigned int,
               qurt_fastint_stack_size,
               32 * DIVUP({:FASTINTSTACKSIZE},32) );

CONFIG_SEC_VAR("",
               SECTION(".data", "QURTK.INTERRUPT"),
               unsigned int,
               QURTK_int_max,
               {:MAXINT});
CONFIG_ZI_SPACE("",
                __attribute__((section(".bss.ukernel.QURTK_l2_interrupt_table.fast"))),
                ALIGNED_SPACE(16) [IDIM({:MAXINT}+1)],
                QURTK_l2_interrupt_table,
                CONFIG_DEFAULT);

CONFIG_RW_VAR("",
              unsigned int,
              QURTK_int_elem_size,
              sizeof(QURTK_l2_interrupt_table[0]));

CONFIG_RW_SPACE("",
                __attribute__((section(".data.qurtos"))),
                unsigned char [],
                qurtos_heap,
                CACHEALIGNED,
                {ARRAY_INIT(ROUNDUP({:KERNELHEAPSIZE},4)) = 0xFF});
SET_CONFIG_VALUE(QURTOS_HEAP_SIZE, sizeof(qurtos_heap));

#if {:XMLLENGTH} || defined(GEN_CONFIG_HEADER)

CONFIG_RW_SPACE(
   "Pass-through XML directives to qurt-image-build.py",
   __attribute__((section(".data.qurtos_xml_passthrough.build"))),
   char [IDIM({:XMLLENGTH})],
   qurtos_xml_passthrough __attribute__((weak)),
   CONFIG_DEFAULT,
   {:XMLCSTRING}
);

#endif /* {:XMLLENGTH} */

SET_CONFIG_VALUE(qurtos_island_heap_size, ROUNDUP({:KERNELHEAPSIZEISLAND},4));
SET_CONFIG_VALUE(qurtos_max_user_processes, {:MAXUSERPROCESSES});
SET_CONFIG_VALUE(QURTK_MAX_USER_PROCESSES, {:MAXUSERPROCESSES});

CONFIG_ZI_SPACE("Array for space pointers in qurtos",
                __attribute__((section(".bss.qurtos"))),
                struct space * [IDIM(1+{:MAXUSERPROCESSES})],
                g_spaces,
                CONFIG_DEFAULT);



/*
||  Some tricky manipulations here because we want a variable length
||   array of page table pointers which have a specific relationship
||   to some other variables in terms of layout in memory; however,
||   the debugger and OSAM functionality expects the page table
||   pointers to have a global symbol pointing at them.
||  So when we generate the actual cust_config object file, we
||   declare a structure and then use inline assembly to associate
||   global symbols to elements of the structure.
||  But when we generate the config header file, we just pretend
||   that the global symbols are normal C declarations.
*/

struct QURTK_VTLB_TABLE_DESCRIPTOR {
   unsigned vtlb_version:4;
   unsigned vtlb_table_type:3;
   unsigned next_table_addr_is_virtual:1;
   unsigned long long next_table_addr:36;
   unsigned table_entries:20;
};

struct QURTK_VTLB_BASIC_TABLE {
   // This structure is 160 bytes long and consists of fields used by OSAM/debugger
   //  4+4+8+8+128+8 == 160

   unsigned revision;                                              // Revision counter
   unsigned padding;                                               // Unused padding
   unsigned char signature[8];                                     // Table descriptor signature
   struct QURTK_VTLB_TABLE_DESCRIPTOR t0;                          // First table descriptor
   unsigned char asidmap[128];                                     // ASID map
   struct QURTK_VTLB_TABLE_DESCRIPTOR t1;                          // Second table descriptor
};

#ifdef CONFIG_VIRTUAL_TLB

#define VTLB_CLIENT_QURTOS 1

#ifdef CONFIG_SRM
#define VTLB_CLIENT_SRM 1
#else
#define VTLB_CLIENT_SRM 0
#endif

#ifdef CONFIG_SECURE_PROCESS
#define VTLB_CLIENT_CPZ {:MAXSECUREPROCS}
#else
#define VTLB_CLIENT_CPZ 0
#endif

#define VTLB_CLIENT_COUNT (VTLB_CLIENT_QURTOS+VTLB_CLIENT_SRM+VTLB_CLIENT_CPZ)

/*
||  VTLB_CLIENT_COUNT is the total number of VTLB clients.
||   Each VTLB client can only manipulate its own VTLB entries.
||   The guest-OS is one VTLB client.
||   Other possible VTLB clients include SRM (Secure Resource Manager),
||    and user processes which have migrated into CPZ.
||
||  The QURTK_vtlb_ownership[] bitmap contains a bit for each combination
||   of a VTLB entry index and a VTLB client index.  If a bit is '1', then
||   the given client and the given entry are associated with each other.
||   If a bit is '0', then they are not.
*/

SET_CONFIG_VALUE(QURTK_vtlb_client_bitmask, (1u << VTLB_CLIENT_COUNT) - 1);

CONFIG_ZI_SPACE("Ownership bitmask for VTLB entries",
                CONFIG_DEFAULT,
                unsigned [IDIM(DIVUP({:VTLB_ENTRIES}*VTLB_CLIENT_COUNT,32))],
                QURTK_vtlb_ownership,
                CONFIG_DEFAULT);

#endif

#ifdef GEN_CONFIG_HEADER

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

#elif defined(CONFIG_VIRTUAL_TLB)

#define VTLB_TREE_COUNT {:VTLB_TREE}                               // Must be multiple of 4
#define VTLB_ENTRY_COUNT {:VTLB_ENTRIES}

#if (VTLB_TREE_COUNT & 3)
#error VTLB_TREE_COUNT not a multiple of 4
#endif

struct tag_QURTK_VTLB_DATA {
   unsigned state[8] __attribute__((aligned(64)));                 // 8 words of state
   unsigned scratch[8];                                            // 8 words of scratch space
   unsigned short tree[VTLB_TREE_COUNT];                           // Lookup tree
   struct QURTK_VTLB_BASIC_TABLE descriptors;                      // OSAM/debugger descriptors
   unsigned long long entries[VTLB_ENTRY_COUNT];                   // VTLB entries
   unsigned extensions[VTLB_ENTRY_COUNT];                          // VTLB extensions
} QURTK_VTLB_DATA __attribute__((section({:VTLB_SECTION}))) = {
   .state = {VTLB_ENTRY_COUNT, VTLB_TREE_COUNT},
   .descriptors = {.signature={'V','T','L','B',3,2,1,0}}
};

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

void *QURTK_pagetables[1] __attribute__((section(".data.ukernel.main"))) = { 0 };

__asm__(
    ".global QURTK_vtlb_entry_count\n"
    ".set    QURTK_vtlb_entry_count, QURTK_VTLB_DATA+0\n"
    ".size   QURTK_vtlb_entry_count, 4\n"
    ".global QURTK_vtlb_tree_count\n"
    ".set    QURTK_vtlb_tree_count, QURTK_VTLB_DATA+4\n"
    ".size   QURTK_vtlb_tree_count, 4\n"
    ".global QURTK_tlb_first_replaceable\n"
    ".set    QURTK_tlb_first_replaceable, QURTK_VTLB_DATA+16\n"
    ".size   QURTK_tlb_first_replaceable, 4\n"
    ".global QURTK_tlb_available_count\n"
    ".set    QURTK_tlb_available_count, QURTK_VTLB_DATA+20\n"
    ".size   QURTK_tlb_available_count, 4\n"
    ".global qurt_tlb_idx\n"
    ".set    qurt_tlb_idx, QURTK_VTLB_DATA+24\n"
    ".size   qurt_tlb_idx, 4\n"
    ".global QURTK_vtlb_prebiased_entry\n"
    ".set    QURTK_vtlb_prebiased_entry, QURTK_VTLB_DATA+28\n"
    ".size   QURTK_vtlb_prebiased_entry, 4\n"
    ".global QURTK_vtlb_tree\n"
    ".set    QURTK_vtlb_tree, QURTK_VTLB_DATA+64\n"
    ".size   QURTK_vtlb_tree, " STRINGIFY(VTLB_TREE_COUNT) "*2\n"
    ".global QURTK_vtlb_basic_table\n"
    ".set    QURTK_vtlb_basic_table, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2\n"
    ".size   QURTK_vtlb_basic_table, 160\n"
    ".global QURTK_vtlb_revision\n"
    ".set    QURTK_vtlb_revision, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2\n"
    ".size   QURTK_vtlb_revision, 4\n"
    ".global QURTK_vtlb_asid_map\n"
    ".set    QURTK_vtlb_asid_map, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2+24\n"
    ".size   QURTK_vtlb_asid_map, 4\n"
    ".global QURTK_vtlb_main\n"
    ".set    QURTK_vtlb_main, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2+16\n"
    ".size   QURTK_vtlb_main, 8\n"
    ".global QURTK_vtlb_entries\n"
    ".set    QURTK_vtlb_entries, QURTK_vtlb_tree+" STRINGIFY(VTLB_TREE_COUNT) "*2+160\n"
    ".size   QURTK_vtlb_entries, " STRINGIFY(VTLB_ENTRY_COUNT) "*8\n"
    ".global QURTK_vtlb_extensions\n"
    ".set    QURTK_vtlb_extensions, QURTK_vtlb_entries+" STRINGIFY(VTLB_ENTRY_COUNT) "*8\n"
    ".size   QURTK_vtlb_extensions, " STRINGIFY(VTLB_ENTRY_COUNT) "*4\n"
);

#elif defined(CONFIG_KERNEL_PAGETABLES)

struct {
   unsigned long long _save[4];                                    // Offset 0, -64 from QURTK_page_table_v2
   unsigned pad[4];                                                // Offset 32, -32 from QURTK_page_table_v2
   unsigned _QURTK_tlb_available_count;                            // Offset 48, -16 from QURTK_page_table_v2
   unsigned _qurt_tlb_idx;                                         // Offset 52, -12 from QURTK_page_table_v2
   unsigned _QURTK_tlb_first_replaceable;                          // Offset 56, -8 from QURTK_page_table_v2
   unsigned _QURTK_tlb_last_replaceable;                           // Offset 60, -4 from QURTK_page_table_v2
   unsigned short _raw[{:V2PAGE_ENTRIES}];                            // Offset 64, -0 from QURTK_page_table_v2
} QURTK_pagetablestruct_v2 __attribute__((section(".data.ukernel.main"),aligned(32))) = {
   ._raw[1] = {:V2PAGE_BLOCKS}
};

void *QURTK_pagetables[1] __attribute__((section(".data.ukernel.main"))) = { 0 };

__asm__(
    ".global QURTK_TLBMISS_SCRATCH_AREA\n"
    ".set    QURTK_TLBMISS_SCRATCH_AREA, QURTK_pagetablestruct_v2+0\n"
    ".size   QURTK_TLBMISS_SCRATCH_AREA, 32\n"
    ".global QURTK_tlb_available_count\n"
    ".set    QURTK_tlb_available_count, QURTK_pagetablestruct_v2+48\n"
    ".size   QURTK_tlb_available_count, 4\n"
    ".global qurt_tlb_idx\n"
    ".set    qurt_tlb_idx, QURTK_pagetablestruct_v2+52\n"
    ".size   qurt_tlb_idx, 4\n"
    ".global QURTK_tlb_first_replaceable\n"
    ".set    QURTK_tlb_first_replaceable, QURTK_pagetablestruct_v2+56\n"
    ".size   QURTK_tlb_first_replaceable, 4\n"
    ".global QURTK_tlb_last_replaceable\n"
    ".set    QURTK_tlb_last_replaceable, QURTK_pagetablestruct_v2+60\n"
    ".size   QURTK_tlb_last_replaceable, 4\n"
    ".global QURTK_page_table_v2\n"
    ".set    QURTK_page_table_v2, QURTK_pagetablestruct_v2+64\n"
    ".size   QURTK_page_table_v2, {:V2PAGE_ENTRIES}*2\n"
    ".global QURTK_page_table_v2_0x15\n"
    ".set    QURTK_page_table_v2_0x15, QURTK_pagetablestruct_v2+64+0x15\n"
    );

#else

struct {
   unsigned _QURTK_tlb_available_count __attribute__((aligned(32)));
   unsigned _qurt_tlb_idx;
   unsigned _QURTK_tlb_first_replaceable;
   unsigned _QURTK_tlb_last_replaceable;
   void *_QURTK_pagetables[1+{:MAXUSERPROCESSES}];
   unsigned _QURTK_dummy;
} QURTK_pagetablestruct S(".data.ukernel.main",".data.QURTK.PGTLB",".data") = {
   ._QURTK_dummy = 0xFFFFFFFFu
};

__asm__(
    ".global QURTK_tlb_available_count\n"
    ".set    QURTK_tlb_available_count, QURTK_pagetablestruct+0\n"
    ".size   QURTK_tlb_available_count, 4\n"
    ".global qurt_tlb_idx\n"
    ".set    qurt_tlb_idx, QURTK_pagetablestruct+4\n"
    ".size   qurt_tlb_idx, 4\n"
    ".global QURTK_tlb_first_replaceable\n"
    ".set    QURTK_tlb_first_replaceable, QURTK_pagetablestruct+8\n"
    ".size   QURTK_tlb_first_replaceable, 4\n"
    ".global QURTK_tlb_last_replaceable\n"
    ".set    QURTK_tlb_last_replaceable, QURTK_pagetablestruct+12\n"
    ".size   QURTK_tlb_last_replaceable, 4\n"
    ".global QURTK_pagetables\n"
    ".set    QURTK_pagetables, QURTK_pagetablestruct+16\n"
    ".size   QURTK_pagetables, 4*(1+{:MAXUSERPROCESSES})\n"
    );

#endif

/* Threads */
CONFIG_RW_VAR("",
              int,
              QURTK_config_MAX_THREADS,
              {:MAXTHREADS});

/* Regular TCB storage */
CONFIG_ZI_SPACE("Regular TCB storage",
                __attribute__((section(".bss.export.QURTK_thread_contexts.fast"))),
                THREAD_CONTEXT [IDIM({:MAXTHREADS} - {:MAXTHREADSINTCM})],
                QURTK_thread_contexts_array,
                CONFIG_DEFAULT);

#ifndef GEN_CONFIG_HEADER
CONFIG_RW_VAR("",
              typeof(QURTK_thread_contexts_array[0]) *,
              QURTK_thread_contexts,
              QURTK_thread_contexts_array);

#endif

/* Mailbox memory */

CONFIG_RO_VAR("max mailboxes",
              unsigned int,
              QURTK_max_mailboxes,
              {:MAXMAILBOXES});

CONFIG_ZI_SPACE("Mailbox Memory",
                S(".data.ukernel.main",".data.QURTK.SCHEDULER",".bss"),
                KERNEL_MAILBOX [IDIM({:MAXMAILBOXES})],
                QURTK_mailbox_data,
                CONFIG_DEFAULT);

CONFIG_RO_VAR("mailbox bandwidth",
              unsigned long long,
              QURTK_mailbox_msg_cycles,
              10000000/{:MAILBOXBANDW});

#define INITIAL_BALANCE 8000    // 64k
CONFIG_RO_VAR("mailbox bandwidth",
              unsigned long long,
              QURTK_mailbox_max_cycles,
              INITIAL_BALANCE*(10000000/{:MAILBOXBANDW}));

/* Debug Buffer*/
CONFIG_RW_VAR("",
              unsigned int,
              QURTK_config_debug_buffer_size,
              {:DEBUGBUFSIZE});

CONFIG_ZI_SPACE("Array for Debug Buffer",
                CONFIG_DEFAULT,
                unsigned char [IDIM({:DEBUGBUFSIZE})],
                QURTK_debug_buf,
                CONFIG_DEFAULT);


/* Uncached content for the kernel */

#if !defined(GEN_CONFIG_HEADER)

union {
   struct {
      /* START uncached kernel content */

#if defined(CONFIG_TRACEBUFFER)
      struct {
         char space[{:TRACESIZE}] __attribute__((aligned(16)));
      } trace_bufs[{:QURT_CONFIG_MAX_HTHREADS}];
      #define TBB_INIT {                         \
         &QURTK_uncached_content.trace_bufs,     \
         &QURTK_uncached_content.trace_bufs + 1, \
      }
      #define TBSIZE sizeof(QURTK_uncached_content.trace_bufs[0])
#else
      #define TBB_INIT { 0,0 }
      #define TBSIZE 0
#endif

#ifdef CONFIG_SAVE_L1DCACHE
   unsigned int l1dd_buf[64][8][10];
   #define L1DD_INIT QURTK_uncached_content.l1dd_buf 
   #define L1DDSIZE sizeof(QURTK_uncached_content.l1dd_buf)
#else
   #define L1DD_INIT 0
   #define L1DDSIZE 0
#endif /* #ifdef CONFIG_SAVE_L1DCACHE */

#ifdef CONFIG_DEBUG_L2
   unsigned int l2dd_buf[1024][8];
   #define L2DD_INIT QURTK_uncached_content.l2dd_buf 
   #define L2DDSIZE sizeof(QURTK_uncached_content.l2dd_buf)
#else
   #define L2DD_INIT 0
   #define L2DDSIZE 0
#endif /* #ifdef CONFIG_DEBUG_L2 */

      /* END uncached kernel content */
      int :0;                                   /* If there is no uncached content, this should prevent
                                                   a compiler error for empty struct.  A zero-length
                                                   unnamed bitfield should suffice as "dummy content"
                                                   while not adding any size to the actual struct.
                                                   Note that if there is no uncached content, this
                                                   should create a 64-byte variable that gets stripped
                                                   out later in garbage-collection. */
   };
   int ucdummy __attribute__((aligned(64)));    /* This makes sure that QURTK_uncached_content has both
                                                   an address and a size that are multiples of 64.
                                                   This is because we don't want to mix uncached and
                                                   cached content in the same 64-byte block. */
} QURTK_uncached_content;

#endif /* GEN_CONFIG_HEADER */

DEFINE_MAPPING(QURTK_uncached_content, "<section cache_policy=\"6:uncached\"/>");
                                                /* After this, CONFIG_MAP_VAR() can be used
                                                   to define variables which point into the
                                                   uncached area, and which get their virtual
                                                   addresses updated to the alternate mapping. */

CONFIG_MAP_VAR("",
               void * [2],
               QURTK_tbb,
               TBB_INIT);

CONFIG_MAP_VAR("Uncached area for dumping L1 D$ contents",
               void *,
               QURTK_L1Dcache_dump,
               L1DD_INIT);

#ifdef CONFIG_SAVE_L1DCACHE
SET_CONFIG_VALUE(QURTK_dcache_dump_size, L1DDSIZE);
#endif /* #ifdef CONFIG_SAVE_L1DCACHE */

CONFIG_MAP_VAR("Uncached area for dumping L2$ tag contents",
               void *,
               QURTK_l2tcm_tag,
               L2DD_INIT);

#if defined(CONFIG_TRACEBUFFER) || defined(GEN_CONFIG_HEADER)
typedef struct
{
   unsigned int   tbuf_size __attribute__((aligned(8)));
   unsigned int   tbuf_mask;
}
QURTK_trace_mask_t;
#define TRACE_INIT(a,b) { .tbuf_size=(a), .tbuf_mask=(b) }

CONFIG_RW_VAR("",
              unsigned int,
              QURTK_trace_buffer_size,
              TBSIZE);
SET_CONFIG_VALUE(qurtos_trace_buffer_size, TBSIZE);

CONFIG_SEC_VAR("",
               SECTION(".data", "QURTK.SCHEDULER"),
               QURTK_trace_mask_t,
               QURTK_config_trace_mask,
               TRACE_INIT(TBSIZE, {:QURTKDEFAULTTRACEMASK}));
#endif // CONFIG_TRACE_BUFFER

typedef enum {
   QURTOS_INIT_STAGE_ZERO,            // Reserved
   QURTOS_INIT_STAGE_GENERIC,         // Install method handlers on the generic handle
} qurtos_init_stage_t;

typedef void (*qurtos_init_funcptr)(qurtos_init_stage_t);
void qurtos_call_init_funcs(qurtos_init_stage_t stage);

void qurtos_process_reaper_init(unsigned, unsigned, void *, unsigned);

#if {:PROCESS_REAPER_ENABLE}

/*
|| If the process reaper is enabled, we will
||  call qurtos_process_reaper_init() and pass it the
||  configuration parameters during qurtos
||  initialization.
*/

void qurtos_process_reaper_generic_init(qurtos_init_stage_t stage)
{
   static CONFIG_ZI_SPACE("Process reaper stack",
                          __attribute__((section(".bss.user.config"))),
                          unsigned long long [IDIM(DIVUP({:PROCESS_REAPER_STACK_SIZE},8))],
                          process_reaper_stack,
                          CONFIG_DEFAULT);

   if (stage == QURTOS_INIT_STAGE_GENERIC) {
      qurtos_process_reaper_init({:PROCESS_REAPER_PRIORITY},
                                 {:PROCESS_REAPER_TCB_PARTITION},
                                 process_reaper_stack,
                                 sizeof(process_reaper_stack));
   }
}

#endif /* PROCESS_REAPER_ENABLE */

#ifndef GEN_CONFIG_HEADER
#define INIT_FUNC_PROTO(n) void n(qurtos_init_stage_t)
{:CFG_INIT_FUNC_PROTOS}
#endif

#define INIT_FUNCS {:CFG_INIT_FUNCS}

CONFIG_SEC_VAR("List of initialization functions to be called at boot time",
               __attribute__((section(".rodata.qurtos"))),
               qurtos_init_funcptr [],
               const qurtos_config_init_funcs,
               INIT_FUNCS);

#ifndef GEN_CONFIG_HEADER
{:CFGSLOTS}
#endif

struct ramfs_scan_info {
   unsigned ppn_start;
   unsigned ppn_end;
   unsigned ppn_step;
};

#define QURTKRAMFSINIT {{:RAMFSSCAN}}

CONFIG_SEC_VAR("Info about scanning for RAMFS partitions",
               __attribute__((section(".data.ukernel.main"))),
               struct ramfs_scan_info,
               QURTK_ramfs_scan,
               QURTKRAMFSINIT);

#endif // !ISLAND_ONLY == MAIN_ONLY

#if !defined(MAIN_ONLY) || defined(GEN_CONFIG_HEADER)

#if {:KERNELHEAPSIZEISLAND} > 0 || defined(GEN_CONFIG_HEADER)
CONFIG_RW_SPACE("",
                __attribute__((section("{:ISLANDHEAPSECTION}"))),
                unsigned char [],
                qurtos_island_heap,
                CACHEALIGNED,
                {ARRAY_INIT(ROUNDUP({:KERNELHEAPSIZEISLAND},4)) = 0xFF});
#endif
CONFIG_RW_VAR("",
              int,
              QURTK_CONTEXT_SIZE,
              {:THREAD_CONTEXT_TOTALSIZE});

CONFIG_RW_VAR("",
              int,
              QURTK_MAX_THREADS_IN_TCM,
              {:MAXTHREADSINTCM});
SET_CONFIG_VALUE(qurtos_max_threads_in_tcm, {:MAXTHREADSINTCM});

CONFIG_RW_VAR("Bit mask to start HW threads, 0x3f to start all HW threads",
              unsigned int,
              QURTK_hthread_startup,
              {:HTHREADMASK});

CONFIG_ZI_SPACE("TCM TCB storage",
                __attribute__((section(".bss.export.QURTK_thread_contexts_tcm.tcm"))),
                THREAD_CONTEXT [IDIM({:MAXTHREADSINTCM})],
                QURTK_thread_contexts_tcm,
                CONFIG_DEFAULT);

CONFIG_ZI_SPACE("Each id has two words",
                SECTION(".data", "QURTK.CONTEXTS"),
                unsigned long long [IDIM({:MAXTHREADS})],
                QURTK_thread_objs_array,
                CONFIG_DEFAULT);

#ifndef GEN_CONFIG_HEADER
unsigned long long *QURTK_thread_objs = &QURTK_thread_objs_array[0];
#endif

/* Futexes */
CONFIG_RW_SPACE("Futex space",
                SECTION(".data", "QURTK.FUTEX"),
                ALIGNED_SPACE(16) [IDIM({:MAXTHREADS})],
                QURTK_futex_objs_array,
                CONFIG_DEFAULT,
                {{{0}}});

#ifndef GEN_CONFIG_HEADER
CONFIG_RW_VAR("",
              typeof(QURTK_futex_objs_array[0]) *,
              QURTK_futex_objs,
              QURTK_futex_objs_array);
#endif

CONFIG_RW_VAR("",
              unsigned int,
              qdsp6ss_base_addr,
              {:QDSP6SSBASE});
CONFIG_RW_VAR("",
              unsigned int,
              QURTK_fast_l2vic_port,
              {:FASTL2PORT});
#ifndef CONFIG_LOW_FOOTPRINT
CONFIG_RW_VAR("CLADE CFG BASE",
              unsigned int,
              QURTK_clade_cfg_base,
              {:QURTKCLADECFGBASE});
CONFIG_RW_VAR("Clade expcetion high size in word",
              unsigned int,
              QURTK_clade_exc_hi_word,
              {:QURTKCLADE_EXC_HI_WORD});
CONFIG_RW_VAR("Clade dictonary size in word",
              unsigned int,
              QURTK_clade_dict_word,
              {:QURTKCLADE_DICT_WORD});
CONFIG_RW_SPACE("Array to save clade expcetion high in cold boot",
                __attribute__((section(".data.qurtos"))),
                unsigned int [],
                QURTK_clade_exception_hi,
                CACHEALIGNED,
                {ARRAY_INIT({:QURTKCLADE_EXC_HI_WORD}+1) = 0xFFFFFFFF});
CONFIG_RW_SPACE("Array to save clade dictonary in cold boot",
                __attribute__((section(".data.qurtos"))),
                unsigned int [],
                QURTK_clade_dict,
                CACHEALIGNED,
                {ARRAY_INIT({:QURTKCLADE_DICT_WORD}+1) = 0xFFFFFFFF});
#endif

#define L2ISLAND_INIT {{:INTISLANDDATA}}
CONFIG_RW_VAR("island interrupts",
              unsigned short [],
              QURTK_int_island,
              L2ISLAND_INIT);

SET_CONFIG_VALUE(QURTK_max_island_int, {:INTISLANDNUM});

CONFIG_ZI_SPACE("",
                SECTION(".data", "QURTK.INTERRUPT"),
                ALIGNED_SPACE(4) [IDIM({:MAXINT}+1)],
                QURTK_l2_int_idx_table,
                CONFIG_DEFAULT);

CONFIG_ZI_SPACE("",
                __attribute__((section(".bss.ukernel.QURTK_l2_island_interrupt_table.island"))),
                ALIGNED_SPACE(16) [IDIM({:INTISLANDNUM})],
                QURTK_l2_island_interrupt_table,
                CONFIG_DEFAULT);

CONFIG_SEC_VAR("The address of memory-map ETM & ETB config registers",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_cfg_base,
               {:ETMCFGBASEADDR});

CONFIG_SEC_VAR("The address of SSC_SSC_ATB_LOW_POWER_HANDSHAKE register",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_ssc_atb_lphandshake,
               {:SSCATBLPHSK});

CONFIG_SEC_VAR("The value ETM ATID",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_atid,
               {:ETMATIDVAL});

CONFIG_SEC_VAR("The value ETM ATB size, only 32-bit and 64-bit sizes are supported",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_atb_size,
               {:ETMATBSIZE});

CONFIG_SEC_VAR("flag to let ETM remain enabled after an exception",
               __attribute__((section(".data"))),
               unsigned int,
               QURTK_etm_enable_on_error,
               {:ETM_ENABLE_ON_ERROR});

CONFIG_RW_VAR("flag to raise a nonfatal error for island tlbmiss",
               unsigned int,
               QURTK_island_fail_tlbmiss,
               {:RAISE_NONFATAL_ON_ISLAND_ERR});

/* Interrupt Privilege Control */
CONFIG_SEC_VAR("disabled=0, enabled=1",
              SECTION(".data", "QURTK.INTERRUPT"),
              unsigned int,
              QURTK_int_privilege,
              {:INTPRIV});
SET_CONFIG_VALUE(qurtos_int_privilege, (char)({:INTPRIV}!=0));

/* Define secure SID info */
CONFIG_RO_VAR("",
              unsigned int,
              QURTK_secure_proc_sid,
              {:SPDSIDINFO});

/* Kernel PCB memory */
CONFIG_ZI_SPACE("PCB Memory",
                S(".data.ukernel.main",".data.QURTK.SCHEDULER",".bss"),
                KERNEL_PCB [IDIM(1+{:MAXUSERPROCESSES})],
                QURTK_pcb_data,
                CONFIG_DEFAULT);

CONFIG_SEC_VAR("Maximum number of programs",
               __attribute__((section(".data.qurtos"))),
              unsigned int,
              QURTK_max_programs,
              {:MAXPROGRAMS});

#define L2ENABLE_INIT {{:QURTKL2VICENABLE}}
CONFIG_RW_VAR("disable=0, enable=1",
              unsigned int [],
              QURTK_l2_enable,
              L2ENABLE_INIT);

#ifndef GEN_CONFIG_HEADER
{:MPROTECT}   // Filled in by Mprotect class in kernel_xml
#endif

#endif // !MAIN_ONLY == ISLAND_ONLY

#if !defined (ISLAND_ONLY) || defined(GEN_CONFIG_HEADER)

/* This goes at the end because it could conceivably have references to
   anything above, and we don't want to pollute with forward references */

#define PHYS_RELOC_INIT {:PHYSRELOCS}
CONFIG_RO_VAR("Words in memory that should be offset at boot time"
              "  by the physical page number of the base of the image",
              const void * [],
              QURTK_phys_relocs,
              PHYS_RELOC_INIT);

struct genreloc {
   void *ptr __attribute__((aligned(8)));
   int delta;
};

#define GEN_RELOC_INIT {:GENRELOCS}
CONFIG_RO_VAR("Words in memory that should be offset at boot time"
              "  by a general offset -- for instance, we use this"
              "  to shrink the default physpool at boot",
              const struct genreloc [],
              QURTK_gen_relocs,
              GEN_RELOC_INIT);

#endif // !ISLAND_ONLY == MAIN_ONLY

/* Give the declaration of the SRM reject function */
#define SRM_REJECT_DECL int QURTK_srm_reject(unsigned pageno, unsigned pgcnt)
#if defined(GEN_CONFIG_HEADER)
SRM_REJECT_DECL;                                /* Prototype for the real SRM reject function */
#else
SRM_REJECT_DECL {:SRMREJECT}                    /* Implementation of the real SRM reject function */
#endif

#ifndef GEN_CONFIG_HEADER
{:Q6CFG}
#endif
