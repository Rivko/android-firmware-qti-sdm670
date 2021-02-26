#ifndef _SYSDBG_H
#define _SYSDBG_H

/*=============================================================================
                        SYSDBG MAIN Header File 

GENERAL DESCRIPTION     
  This module defines functions for intializing SYSDBG functionalities.
  
	Copyright 2014  - 2015 by QUALCOMM Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#include "comdef.h"

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---          -------------------------------------------------- 
11/10/2014 amcheriy Updated enums and adding api to get buffer
01/14/2014 	davidai	Ported SDI to Bear

===========================================================================*/

/* Format versions for various dumps supported by SystemDebug version */
#define CACHE_DUMP_FORMAT_VERSION 0x11
#define CPU_DUMP_FORMAT_VERSION	 0x13
#define RAWMEM_DUMP_FORMAT_VERSION 0x10

/* Other common fields populated by the code
 * Magic number and name fields in the dumps */
#define DUMP_MAGIC_NUMBER 0x42445953
#define SYSDBG_NAME "sysdbg"
#define MAX_ENTRIES_PER_TABLE   128

/* CPU specific enums */
typedef enum
{
    ARMv8_CPU  ,
    HYDRA_CPU  ,
    MAX_CPUS, 
    UNKNOWN_CPU = 0x7FFFFFFF,
}cpu_type;

/* Name for the various sub-systems */
typedef enum
{
    SUBSYS_APPS = 0,
    SUBSYS_IMC  = 1,
    SUBSYS_MAX  = 2,
    SUBSYS_UNKNOWN = 0x7FFFFFFF,
}subsys_type;


/* All the formats for the various system debug memory dumps */
/* Generic header for every kind of dump supported */

/* Enum for the type of dump entry provided by HLOS */
typedef enum
{
    DUMP_TYPE_DATA = 0x0,
    DUMP_TYPE_TABLE = 0x1,
}msm_dump_entry_type;

/* HLOS Dump Structure */
/* Dump Type */
typedef enum
{
	CPU_REGS_DUMP   = 0x00,
    CPU_L1_INST_TLB_DUMP  = 0x20,
    CPU_L1_DATA_TLB_DUMP  = 0x40,
    CPU_L1_ICACHE_DUMP  = 0x60,
    CPU_L1_DCACHE_DUMP  = 0x80,
    CPU_L2_DCACHE_DUMP  = 0xC0,
    CPU_L2_ICACHE_DUMP = CPU_L2_DCACHE_DUMP,
    CPU_L3_CACHE_DUMP = 0xD0,
    CPU_L3_ICACHE_DUMP = CPU_L3_CACHE_DUMP,
    CPU_L3_DCACHE_DUMP = CPU_L3_ICACHE_DUMP,
	ETB_DUMP = 0xF0, 
	ETM_DUMP = 0xA0,
    _RESERVED2   = 0xB0,
    PMIC_DUMP = 0xE4,
	_RESERVED1 = 0xE0, // Reserved for OCMEM
    DCC_REGDUMP  = 0xE6,
    DCC_SRAMDUMP = 0xE7,
    GENERIC_DUMP = 0xE8,
    TMC_DUMP = 0x100,
    RAWMEM_DUMP  = 0x101,
    MSM_MAX_DUMP = 0x300,
	MAX_DUMP_TYPES = 0x7FFFFFFF,
}dump_type;


/* Enums for the various clients in the dump table */
typedef enum
{
    DUMP_CLIENT_APPS = 0,
    DUMP_CLIENT_IMC  = 1,
    DUMP_CLIENT_MPSS = 2, 
    DUMP_CLIENT_ADSP = 3, 
    DUMP_CLIENT_WCSS = 4,
    DUMP_CLIENT_MAX  = 5,
}msm_dump_clients;

typedef struct
{
    /* Format version of the saved structure */
    uint32 version;
    /* Magic number */
    uint32 magic;
    /* Name of the dump. Currently unused */
    char name[32];
}dump_header_type;

/* Dump data type */
typedef struct
{
    /* Version of the format in which data is dumped */
    uint32 version;
    uint32 magic;
    char name[32];
    uint64 start_addr;
    uint64 len;
    /* Version of this structure */
    uint32 structfmt_version;
    uint64 arg0;
    uint64 arg1;
}dump_data_type;

/* Dump entry in the table */
typedef struct
{
	dump_type id;
    char name[32];
    uint32 type;
	uint64 start_addr;
}dump_entry_type;

/* Dump Table */
typedef struct 
{
	uint32 version;
	uint32 num_entries;
	dump_entry_type entries[MAX_ENTRIES_PER_TABLE];
}dump_table_type;




/*===========================================================================
**  Function :  SYSTEM_DEBUG_MAIN
** ==========================================================================
*/
/*!
* 
* @brief
* 	Main SYSDBG
* 
* @param[in] 
*  	uint32 pass_count
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*
*/
void system_debug_main(void);

/*===========================================================================
**  Function : SYSDBG_RESET_CHECK
** ==========================================================================
*/
/*!
* 
* @brief
*   API used to check for whether we're in cold boot or WDT reset boot
*
* @param[in] 
*	
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   
*
*
*/

boolean sysdbg_reset_check(void);

/*===========================================================================
**  Function : SYSDBG_GET_BUFFER 
** ==========================================================================
*/
/*!
* 
* @brief
*   API used to get the buffer pointing to a request dump entry
*
* @param[in] 
*	Dump name for which buffer is desired
*  
* @par Dependencies
*   HLOS Dump table must be populated. If not, it will return NULL
* 
* @retval
*   Pointer to buffer where data can be saved
* 
* @par Side Effects
*   None
*
*
*/
dump_data_type *sysdbg_get_buffer(subsys_type subsys, dump_type name , uint32 block);

/*===========================================================================
**  Function : SYSDBG_SRB_WRITE_HYDRA 
** ==========================================================================
*/
/*!
* 
* @brief
*
* @param[in] 
*  
* @par Dependencies
* 
* @retval
* 
* @par Side Effects
*   None
*
*
*/
void sysdbg_srb_write_hydra(void);



#endif /* _SYSDBG_H */
