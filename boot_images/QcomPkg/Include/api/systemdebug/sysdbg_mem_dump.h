/*=============================================================================
                        sysdbg Memory Dump Utility File 

GENERAL DESCRIPTION     
  This module defines structures for the memory dump driver.
  
	Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdebug/sysdbg_mem_dump.h#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------- 
11/26/2013 ntonupun     Added RTC Logging, updated sysdbgStatsDump structure.
10/02/2013  AJCheriyan  Bumped up CPU Dump version
07/15/2013  AJCheriyan  Some API updates
06/26/2013  AJCheriyan  Fixed order of stats dump fields
03/12/2012	AJCheriyan	First cut for Aragorn. 

===========================================================================*/

#ifndef _SYSDBG_MEMORY_DUMP_H
#define _SYSDBG_MEMORY_DUMP_H

#include "comdef.h"
#include "sysdbg.h"
#include "sysdbg_target_memdump.h"


// Common Macros
#define SYSDBG_ENUM_32BITS(x) x##_INVALID=0x7FFFFFFF

/* Temp hack until API is rewritten */
#define TARGET_CLUSTERS 16
#define TARGET_APPS_CLUSTERS 2 
#define TARGET_CLUSTER_0_SIZE 8
#define TARGET_CLUSTER_1_SIZE 8



typedef enum
{
	/* ETR - Embedded Trace Router */
	TMC_ETR_TYPE=0,
	/* Embedded Trace Buffer / Funnel */	
	TMC_ETB_TYPE,
	MAX_TMC_TYPES,
	SYSDBG_ENUM_32BITS(TMC_TYPES),
}tmc_type;


typedef struct 
{
	uint32 sec:16;
	uint32 cpu:16;
}cpuid_type;



/* External CPU Dump Structure - 64 bit EL */
typedef struct
{
	uint64 x0;
	uint64 x1;
	uint64 x2;
	uint64 x3;
	uint64 x4;
	uint64 x5;
	uint64 x6;
	uint64 x7;
	uint64 x8;
	uint64 x9;
	uint64 x10;
	uint64 x11;
	uint64 x12;
	uint64 x13;
	uint64 x14;
	uint64 x15;
	uint64 x16;
	uint64 x17;
	uint64 x18;
	uint64 x19;
	uint64 x20;
	uint64 x21;
	uint64 x22;
	uint64 x23;
	uint64 x24;
	uint64 x25;
	uint64 x26;
	uint64 x27;
	uint64 x28;
	uint64 x29;
	uint64 x30;
	uint64 pc;
    uint64 currentEL;
    uint64 sp_el3;
    uint64 elr_el3;
    uint64 spsr_el3;
	uint64 sp_el2;
    uint64 elr_el2;
    uint64 spsr_el2;
	uint64 sp_el1;
    uint64 elr_el1;
    uint64 spsr_el1;
    uint64 sp_el0;
    uint64 __reserved1;
    uint64 __reserved2;
    uint64 __reserved3;
    uint64 __reserved4;
}sysdbg_cpu64_ctxt_regs_type;

/* External CPU Dump format - 32 bit EL */
typedef struct
{
	uint64 r0;
	uint64 r1;
	uint64 r2;
	uint64 r3;
	uint64 r4;
	uint64 r5;
	uint64 r6;
	uint64 r7;
	uint64 r8;
	uint64 r9;
	uint64 r10;
	uint64 r11;
	uint64 r12;
	uint64 r13_usr;
	uint64 r14_usr;
	uint64 r13_hyp;
	uint64 r14_irq;
	uint64 r13_irq;
	uint64 r14_svc;
	uint64 r13_svc;
	uint64 r14_abt;
	uint64 r13_abt;
	uint64 r14_und;
	uint64 r13_und;
	uint64 r8_fiq;
	uint64 r9_fiq;
	uint64 r10_fiq;
	uint64 r11_fiq;
	uint64 r12_fiq;
	uint64 r13_fiq;
	uint64 r14_fiq;
	uint64 pc;
    uint64 cpsr;
	uint64 r13_mon;
	uint64 r14_mon;
	uint64 r14_hyp;
	uint64 _reserved;
    uint64 __reserved1;
    uint64 __reserved2;
    uint64 __reserved3;
    uint64 __reserved4;
}sysdbg_cpu32_ctxt_regs_type;

typedef union
{
	sysdbg_cpu32_ctxt_regs_type cpu32_ctxt;
	sysdbg_cpu64_ctxt_regs_type cpu64_ctxt;
}sysdbg_cpu_ctxt_regs_type;

//  Floating Point Context Format
/* FPU / Neon register type */
typedef struct
{
 uint64 lower;
 uint64 upper;
}fp_reg_type;

/* FPU / Neon Registers */
typedef struct
{
 fp_reg_type q0;
 fp_reg_type q1;
 fp_reg_type q2;
 fp_reg_type q3;
 fp_reg_type q4;
 fp_reg_type q5;
 fp_reg_type q6;
 fp_reg_type q7;
 fp_reg_type q8;
 fp_reg_type q9;
 fp_reg_type q10;
 fp_reg_type q11;
 fp_reg_type q12;
 fp_reg_type q13;
 fp_reg_type q14;
 fp_reg_type q15;
 fp_reg_type q16;
 fp_reg_type q17;
 fp_reg_type q18;
 fp_reg_type q19;
 fp_reg_type q20;
 fp_reg_type q21;
 fp_reg_type q22;
 fp_reg_type q23;
 fp_reg_type q24;
 fp_reg_type q25;
 fp_reg_type q26;
 fp_reg_type q27;
 fp_reg_type q28;
 fp_reg_type q29;
 fp_reg_type q30;
 fp_reg_type q31;
}sysdbg_neon_regs_type;

typedef struct
{
 uint32 fpsid;
 uint32 fpscr;
 uint32 fpexc;
 sysdbg_neon_regs_type neon_ctxt;
}sysdbg_neon_ctxt_regs_type;

/* Type for breakpoint / watchpoint info */ 
typedef struct
{
 uint64 value;
 uint64 control;
}dbg_config_type;

/* Debugger Register Context */
typedef struct
{
 uint32 dbgidr;
 uint32 dbgscr;
 dbg_config_type bkpts[16];
 dbg_config_type wtpts[16];
}sysdbg_dbg_ctxt_type;


/* External format for CPU context */
typedef struct
{
    /* Status fields */
	uint32 status[4];
	/* Context for all CPUs */
	sysdbg_cpu_ctxt_regs_type cpu_regs;
	/* Secure Context - Not used  */
	sysdbg_cpu_ctxt_regs_type __reserved3;
    /* Neon Context Dump */
    sysdbg_neon_ctxt_regs_type neon_regs; 
    /* Debug Register Info */
    sysdbg_dbg_ctxt_type dbg_regs;
}SysdbgCPUCtxtType;

/* Main struct to hold cpu context for all cores*/
typedef struct
{
    dump_data_type *CPUDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
}SysdbgCPUDumpType;

/* Format for CPU Context buffer unparsed format */

typedef struct
{
    uint64 regs[0x400];
}cpu_ctxt_buf_type;

/* Format for Generic Dump */
typedef struct
{
    uint32 count;
    uint32 entries[];
}SysdbgGenericDumpType;


/* Stats dump structure */
typedef struct
{
	/* First 3 entries populated by TZ */
	/* Magic number */
	uint32 magic;
	/* Bit mask for L1 validity */
	uint32 l1_valid;
	/* Bit Mask for L2 validity */
	uint32 l2_valid;
	/* Populated by sysdbg */
	// MSM Reset Reason
	uint32 msm_reset_reason;
	// PMIC Power On Reason
	uint32 pon_reason;
	// PMIC Warm Reset Reason
	uint32 wrst_reason;
	// PMIC Power Off Reason
	uint32 poff_reason;

}SysdbgStatsDumpType;


/* Enum for cache type */
typedef enum
{
    L0DATA = 0x0,
    L0INST = 0x1,
    L0UNIF = 0x2,
    L1DATA = 0x3,
    L1INST = 0x4,
    L1UNIF = 0x5,
    L2DATA = 0x6,
    L2INST = 0x7,
    L2UNIF = 0x8,
    L3INST = 0x9,
    L3DATA = 0xA,
    L3UNIF = 0xB,
    LxMAX  = 0xC,
}sysdbg_cache_type;

/* Enum for various cache rams. A one-to-one mapping with the value */
typedef enum
{
    L1ITAGRAM  = 0x0,
    L1IDATARAM = 0x1,
    L1IBTBRAM  = 0x2,
    L1IGHBRAM  = 0x3,
    L1ITLBRAM  = 0x4,
    L1IPRAMRAM = 0x5,
    L1DTAGRAM  = 0x8,
    L1DDATARAM = 0x9,
    L1DTLBRAM  = 0xA,
    RESERV1 = 0xB,
    RESERV2 = 0xC,
    RESERV3 = 0xD,
    RESERV4 = 0xE,
    RESERV5 = 0xF,
    L2TAGRAM  = 0x10,
    L2DATARAM = 0x11,
    L2STAGRAM = 0x12,
    L2PARITYRAM = 0x13,
    L2DIRTYRAM = 0x14,
    L2TLBRAM   = 0x15,
    CACHE_RAM_MAX = 0x7FFFFFFF,
}sysdbgOCRamType;

/* Cache dump structure */
typedef struct 
{
    /* Status fields */
	uint32 status[4];
    /* Tag Size in u32 words */
	uint32 TagSize;
    /* Line Size in u32 words */
	uint32 LineSize;
    /* Number of sets */
	uint32 NumSets;
    /* Number of ways */
	uint32 NumWays;
    uint64 *next;
	uint32 __reserved[4];
	uint32 lines[];
}sysdbgCacheLvlDumpType;

/* Onchip RAM Dump structure */
typedef struct
{
    /* Common header */
    dump_header_type header;
    /* Number of entries in the dump*/
    uint32 NumEntries;
    /* Size of each entry */
    uint32 EntrySize;
    uint64 *next;
    uint32 __reserved[4];
    uint32 lines[];
}sysdbgOCRamDumpType;

/* Collective Cache Dump Structure */
typedef struct
{
    /* All L0 cache dump buffers */
    dump_data_type *L0CacheDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
    /* All L1 I-cache dump buffers */
    dump_data_type *L1ICacheDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
    /* All L1 D-cache dump buffers */
    dump_data_type *L1DCacheDump[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
    /* All L2 cache dump buffers */
    dump_data_type *L2CacheDump[TARGET_CLUSTERS];
    /* At the time of writing, a reserved and unused field */
    dump_data_type *L3CacheDump[TARGET_CLUSTERS];
}sysdbgCacheDumpType;

/* L2 dump structure */
typedef struct
{
	uint32 magic_number;
	uint32 version;
	uint32 TagSize;
	uint32 LineSize;
	uint32 NumLines; /* Ways * Sets */
	uint32 lines[];
}sysdbgL2DumpType;


/* Global struct for ETM Regs */
typedef struct
{
	dump_data_type *ETMCPU[TARGET_CLUSTER_0_SIZE+TARGET_CLUSTER_1_SIZE];
}SysdbgETMDumpType;



typedef struct
{
	dump_data_type *TMCs[MAX_TMC_TYPES];
}SysdbgTMCDumpType;




/*===========================================================================
** 	Function: sysdbgDumpStats
** ==========================================================================
*/
/*!
* 
* @brief
* 	Function that contains stats populated for and by sysdbg
* 	Includes 
* 		- State of caches populated by TZ for sysdbg
* 		- Reset reason as seen by sysdbg. Populated by sysdbg*
*
* @param[in] 
*	Pointer to data that contains stats populated by sysdbg
*  
* @par Dependencies
*	None
* 
* @retval
*   Success / Fail
* 
* @par Side Effects
*	Must be called only after global struct is initialized
*
*/
boolean SysdbgDumpStats(uint32 *data);




/*===========================================================================
** 	Function: SysdbgDumpTableInit
** ==========================================================================
*/
/*!
* 
* @brief
* 	Initializes the dump table region
*
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
boolean SysdbgDumpTableInit(void);



/*===========================================================================
** 	Function: sysdbgMemoryDumpInit
** ==========================================================================
*/
/*!
* 
* @brief
* 	Function that initalizes the dump structure for the various dump types
*
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
boolean SysdbgMemoryDumpInit(uint32);


/*===========================================================================
** 	Function: SysdbgDumpCPUCtxt
** ==========================================================================
*/
/*!
* 
* @brief
* 	Function that copies the data from internal IMEM structure to external
* 	DDR location
*
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	Must be called only after global pointer is initialized
*
*/
boolean SysdbgDumpCPUCtxt(uint32);

/*===========================================================================
**  Function: SysdbgDumpMemories(uint32 subsys)
** ==========================================================================
*/
/*!
* 
* @brief
*   Function that does a raw memory backup from some source to allocated buffer
*   in the destination
*
*
* @param[in] 
*   Subys if needed. 
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   Must be called only after global pointer is initialized
*
*/
boolean SysdbgDumpMemories(uint32);


/*===========================================================================
** 	Function: SysdbgGetCPUCtxtBuffer
** ==========================================================================
*/
/*!
* 
* @brief
* 	Buffer to save the CPU registers to be passed to monitor image
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	Must be called only after global pointer is initialized
*
*/
uint64 *SysdbgGetCPUCtxtBuffer(void);


#endif /* _SYSDBG_MEMORY_DUMP_H */
