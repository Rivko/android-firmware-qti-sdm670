/**
  @file DBIDump.h
 
  File contains registers used in Sec and MemoryMap.

  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/04/16  ly       Update DBI Dump Table to version 200000 and add more entries
 12/03/14  nk       Changed dump table address to static address 
 06/19/14  shireesh Increased Size of CPU reg to 1KB, Modified code to get DDR memory base at run time.
 05/15/14  shireesh revoved support for SDI dump version 0 and Added support for SDI dump ver 1
 01/15/14  shireesh moved DBI memory region to 64K
 02/11/13   niting  Updated DBI memory region.
 01/30/13   vk      Move dump structures from sec
 01/23/13   nk      Initial Verison.

 =============================================================================*/

/*
Context         Start Address:       Size:
Cores 0..7      0x10000 + i*400      0x400 
ETB             0x12000              0x10000  
EndEntry        0x22000              0x400 
 
Table Base Address:
DbiDumpTable            MemBase + 0x22400
AppDumpTable            MemBase + 0x22400 + sizeOf(DbiDumpTable)
DumpDataTypeTable[10]   MemBase + 0x22400 + 2*sizeOf(DbiDumpTable) 

                                
*/

#ifndef _DBIDUMP_H_
#define _DBIDUMP_H_

#define DBI_SHARED_IMEM_COOKIE_OFFSET             0x010 
#define NUMBER_OF_CORES                           0x8
#define SDI_DUMP_CORE_AP_REG_SIZE                 0x400
#define SDI_DUMP_ETB_DUMP_SIZE                    0x10000
#define SDI_RPM_CODE_RAM_DUMP_SIZE                0x28000
#define SDI_PMIC_DUMP_SIZE                        0x10000
/* SCAN DUMP used to be one entry for entire gold cluster */
#define SDI_SCAN_DUMP_SIZE                        0x40000
/* New SCAN DUMP is per core */ 
#define SDI_SCAN_DUMP_PER_CORE_SIZE               0x10000

/* Current supported Dump Table Version is 1.4 */
#define DBI_DUMP_TABLE_VERSION                    0x200000 

/* 8 CPU reg dump entries; 8 scan dump entries; ETB; RPM; PMIC */
#define NUM_OF_APPS_DUMP_ENTRY 19

#define CPU_REG_DUMP_START_OFFSET       0x0
#define CPU_REG_DUMP_END_OFFSET         CPU_REG_DUMP_START_OFFSET + SDI_DUMP_CORE_AP_REG_SIZE * NUMBER_OF_CORES  

//ETB Dump location offset from DDR base
#define CPU_ETB_DUMP_START_OFFSET      CPU_REG_DUMP_END_OFFSET
#define CPU_ETB_DUMP_END_OFFSET        CPU_REG_DUMP_END_OFFSET + SDI_DUMP_ETB_DUMP_SIZE

//RPM code ram dump
#define RPM_CODE_RAM_DUMP_START_OFFSET CPU_ETB_DUMP_END_OFFSET
#define RPM_CODE_RAM_DUMP_END_OFFSET   RPM_CODE_RAM_DUMP_START_OFFSET + SDI_RPM_CODE_RAM_DUMP_SIZE

#define PMIC_DUMP_START_OFFSET         RPM_CODE_RAM_DUMP_END_OFFSET
#define PMIC_DUMP_END_OFFSET           PMIC_DUMP_START_OFFSET +  SDI_PMIC_DUMP_SIZE

#define SCAN_DUMP_START_OFFSET         PMIC_DUMP_END_OFFSET
#define SCAN_DUMP_END_OFFSET           SCAN_DUMP_START_OFFSET +  SDI_SCAN_DUMP_PER_CORE_SIZE * NUMBER_OF_CORES

/*
 * DBI DUMP REGION:
 * -DUMP DATA
 *  0-7 CPU REG DUMP Entries (0-7)
 *  8 ETB (0xF0)
 *  9 RPM (0xEA)
 *  10 PMIC (0xE4)
 *  11-18 SCAN DUMP Entries (130-137)
 * -DUMPTABLE <-- Start right after DUMP DATA. So at SCAN_DUMP_END_OFFSET. 
 * -APPSTABLE
 * -DUMPTABLENTRYTABLE
 *
 * Note: Location of DUMPTABLE is saved to shared IMEM for SDI to use.
 */
#define DBI_DUMP_TABLE_OFFSET          SCAN_DUMP_END_OFFSET

/* Generic header for every kind of dump supported */
/* HLOS Dump Structure */
/* Dump Type */
typedef enum
{
  MSM_CPU_REGS_DUMP   = 0x00,
  MSM_CPU_REGS_DUMP2  = 0x10,
  CPU_L1_INST_TLB_DUMP  = 0x20,
  CPU_L1_INST_TLB_DUMP2 = 0x30,
  CPU_L1_DATA_TLB_DUMP  = 0x40,
  CPU_L1_DATA_TLB_DUMP2 = 0x50,
  CPU_L1_ICACHE_DUMP  = 0x60,
  CPU_L1_ICACHE_DUMP2 = 0x70,
  CPU_L1_DCACHE_DUMP  = 0x80,
  CPU_L1_DCACHE_DUMP2 = 0x90,
  MSM_ETM_DUMP = 0xA0,
  _RESERVED2   = 0xB0,
  CPU_L2_DCACHE_DUMP  = 0xC0,
  CPU_L2_ICACHE_DUMP = CPU_L2_DCACHE_DUMP,
  CPU_L3_CACHE_DUMP = 0xD0,
  CPU_L3_ICACHE_DUMP = CPU_L3_CACHE_DUMP,
  CPU_L3_DCACHE_DUMP = CPU_L3_ICACHE_DUMP,
  CPU_REG_DUMP_END_CHECK = 0x100CC001,
  _RESERVED1 = 0xE0, // Reserved for OCMEM
  MSM_PMIC_DUMP = 0xE4,
  MSM_DUMP_DATA_DBGUI_REG = 0xE5,
  MSM_DATA_DCC_REG_DUMP = 0xE6,
  MSM_DATA_DCC_SRAM_DUMP = 0xE7,
  MSM_GENERIC_DUMP = 0xE8,
  MSM_DUMP_DATA_VSENSE = 0xE9,
  MSM_DATA_RPM_CODE_RAM_DUMP = 0xEA,
  MSM_DATA_SCANDUMP = 0xEB, //This is old SCANDUMP for entire Gold Cluster
  MSM_ETB_DUMP = 0xF0, 
  MSM_TMC_DUMP = 0x100,
  MSM_L2_TLB_DUMP = 0x120,
  MSM_SCANDUMP_CORE_DUMP = 0x130, //This is the new SCANDUMP per core. 0x130, 0x131, .....
  MSM_MAX_DUMP = 0x300,
  MAX_DUMP_TYPES = 0x7FFFFFFF,
}DUMP_TYPE;


typedef struct
{
  /* Format version of the saved structure */
  UINT32 version;
  /* Magic number */
  UINT32 magic;
  /* Name of the dump. Currently unused */
  CHAR8 name[32];
}DUMP_HEADER_TYPE; 
 

/* Dump data type */
typedef struct
{
  DUMP_HEADER_TYPE header;
  UINT64 start_addr;
  UINT64 len;
}DUMP_DATA_TYPE; 

/* Enums for the various clients in the dump table */
typedef enum
{
  MSM_DUMP_CLIENT_APPS = 0,
  MSM_DUMP_CLIENT_RPM  = 1,
  MSM_DUMP_CLIENT_MPSS = 2, 
  MSM_DUMP_CLIENT_ADSP = 3, 
  MSM_DUMP_CLIENT_WCSS = 4,
  MSM_DUMP_CLIENT_MAX  = 5,
}MSM_DUMP_CLIENTS;

/* Enum for the type of dump entry provided by HLOS */
typedef enum
{
  MSM_DUMP_TYPE_DATA = 0x0,
  MSM_DUMP_TYPE_TABLE = 0x1,
  MSM_DUMP_TYPE_MAX = 0x2
}MSM_DUMP_ENTY_TYPE;


/* Dump entry in the table */
typedef struct
{
    UINT32 id;
    CHAR8 name[32];
    UINT32 type;
    UINT64 start_addr;
}DUMP_ENTRY_TYPE;

/* Dump Table */
typedef struct 
{
  UINT32 Version;
  UINT32 NumEntries;
  DUMP_ENTRY_TYPE Entries[MSM_MAX_DUMP>>4];
}DUMP_TABLE_TYPE;

typedef struct
{
  DUMP_DATA_TYPE DumpDataType[NUM_OF_APPS_DUMP_ENTRY];
}DUMP_DATA_TYPE_TABLE;

#endif /* _DBIDUMP_H_ */

