/*=============================================================================
            S Y S T E M  D E B U G   M E M O R Y   D U M P  M O D U L E

GENERAL DESCRIPTION     
  This module defines objects for storing the CPU context saved by the debug image.
  
    Copyright 2012 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SystemDebugLib/sysdbg_mem_dump.c#1 $

when       who          what, where, why
--------   ---          --------------------------------------------------
07/28/15    AJCheriyan  Added some valid buffer checks.
11/10/14    AJCheriyan  Added memory buffer get api
10/02/13    AJCheriyan  Updates to CPU Dump for new format
09/11/13    AJCheriyan  More Robust entry to flush cache routine
09/03/13    AJCheriyan  Restore PBL / sysdbg Boot backed registers
07/16/13    AJCheriyan  Some API updates
07/08/13    AJCheriyan  Retain lower 16 bits of reset reason
05/16/13    AJCheriyan  Unlock ETM registers using OSLAR
06/26/12    AJCheriyan  Added L1 and L2 Save Code
02/26/12    AJCheriyan  First cut for Aragorn. 

===========================================================================*/

#include "sysdbg_mem_dump.h"
#include "sysdbg_target.h"
#include "stddef.h"
#include <stdint.h>
#include "msmhwio.h"
#include "comdef.h"
#include "stringl/stringl.h"
#include "sysdbg_system.h"
#include "armv8_srb.h"
#include "LoaderUtils.h"
#include "sysdbg_target_memdump.h"
#include "sysdbg_sysreg.h"

// Main structure containing the register dump
cpu_ctxt_buf_type sysdbg_cpu_ctxt_regs;

// Main struct containing all the cache dump pointer entries
static sysdbgCacheDumpType sysdbgCacheDump;

// Main struct containing the information for the CPU register dump
static SysdbgCPUDumpType SysdbgCPUDump;

/* Main struct containing the information for the ETB dump */
static dump_data_type *SysdbgETBDump;

/* Main struct containing the information for ETM dump */
static SysdbgETMDumpType SysdbgETMDump;

/* Main struct containing the information for TMC dump */
static SysdbgTMCDumpType SysdbgTMCDump;

/* Main struct that contains the sysdbg Stats dump */
static SysdbgStatsDumpType *SysdbgStatsDump;

/* Main struct containing memory dump types */
static dump_data_type *SysdbgRawMemDump;

/*===========================================================================
**  Function: SysdbgGetCPUCtxtBuffer
** ==========================================================================
*/
/*!
* 
* @brief
*   Buffer to save the CPU registers to be passed to monitor image
*
* @param[in] 
*   None
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
uint64 *SysdbgGetCPUCtxtBuffer(void)
{
    /* Buffer to be passed to monitor. */

    return (uint64 *)&sysdbg_cpu_ctxt_regs;
}



/*===========================================================================
**  Function: sysdbgDumpStats
** ==========================================================================
*/
/*!
* 
* @brief
*   Function that contains stats populated for and by sysdbg
*   Includes 
*       - State of caches populated by TZ for sysdbg
*       - Reset reason as seen by sysdbg. Populated by sysdbg*
*
* @param[in] 
*   Pointer to data that contains stats populated by sysdbg
*  
* @par Dependencies
*   None
* 
* @retval
*   Success / Fail
* 
* @par Side Effects
*   Must be called only after global struct is initialized
*
*/
boolean SysdbgDumpStats(uint32 *data)
{
    SysdbgStatsDump = (SysdbgStatsDumpType *)((uint8 *)SysdbgGetSharedImemBase() + 0x758); //set the table pointer again as it's not retained accross reset
    // Copy the data over
    /*const uint32 size = sizeof(SysdbgStatsDumpType) - offsetof(SysdbgStatsDumpType, msm_reset_reason);
    memscpy(&(SysdbgStatsDump->msm_reset_reason), size, data, size); */
    
    return TRUE;
}   



static void *GetDumpTableStart(void)
{
    return (SysdbgGetSharedImemBase() + 0x10);
}

/*===========================================================================
**  Function: SysdbgAddDumpTableEntry(void *base_addr, dump_entry_type *entry)
** ==========================================================================
*/
/*!
* 
* @brief
*  Accepts a pointer of dump_entry_type which maps to a table and creates a new 
*  table entry to point to.
*
*
* @param[in] 
*   None
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
*/
static boolean SysdbgAddDumpTableEntry(void *base_addr, dump_type id, uint64 size, uint64 *args)
{
    uint32 entries_count = 0;
    uint64 buffer = 0;
    dump_entry_type *entry = (dump_entry_type *)base_addr;
    dump_data_type  *dump_data  = NULL;

    if (!entry)
        return FALSE;

    /* The entry has to be a table entry and cannot be another table. Hence, type must be 1*/
    if (entry->type != DUMP_TYPE_TABLE)
        return FALSE;

     /* Find the current number of entries in the table */
     dump_table_type *dump_table = (dump_table_type *)(entry->start_addr);
     entries_count = dump_table->num_entries;
               
     /* Bail if we overflow the table limit */
     if (!(entries_count + 1 < (MSM_MAX_DUMP)))
         return FALSE;
     else
     {
        /* Allocate a buffer in there and check if there is space */
        buffer = (uint64)(SysdbgAllocDumpSpace(size));

        if (!buffer)
            return FALSE;
        
        /* Make the entry point to the buffer */
        dump_data = (dump_data_type *)(SysdbgAllocEntryRegion(sizeof(dump_data_type)));
        dump_data->start_addr = buffer;
        dump_data->len        = size;
        if (args)
        {
            dump_data->arg0       = args[0];
            dump_data->arg1       = args[1];
        }
        
        /* Add the table entry and mark it as data */
        dump_table->entries[entries_count].id         = id;
        dump_table->entries[entries_count].start_addr = (uint64)(dump_data);
        dump_table->entries[entries_count].type       = DUMP_TYPE_DATA;

        /* Increment the count */
        dump_table->num_entries = entries_count + 1;
     }

     return TRUE;
}


/*===========================================================================
**  Function: SysdbgDumpTableInit
** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the dump table region
*
*
* @param[in] 
*   None
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
*/
boolean SysdbgDumpTableInit(void)
{

    uint32 count = 0;
    dump_table_type *table = NULL;
    uint64 args[] = {0, 0};
    uint32 table_sizes[] = { MAX_ENTRIES_PER_TABLE, 8 };

    /* Reset the dump table pointer */
    dump_table_type **DumpTablePtr = (dump_table_type **)(GetDumpTableStart());
    /* Put our top level table in first */
    *DumpTablePtr = (dump_table_type *)(SysdbgGetDebugRegion());

    /* Create a new table for APPS */
    for (count = 0; count <= DUMP_CLIENT_IMC; count++)
    {
        table = (dump_table_type *)(SysdbgAllocTableRegion(sizeof(dump_entry_type) * table_sizes[count]));
        
        if (table)
        {
            (*DumpTablePtr)->entries[count].id = count;
            (*DumpTablePtr)->entries[count].type = DUMP_TYPE_TABLE;
            (*DumpTablePtr)->entries[count].start_addr = (uint64)table;
            (*DumpTablePtr)->num_entries = count + 1;
        }
        else
            return FALSE;
    }
    
    /* Add entries in the dump table for each of the tables 
     * APPS - cores, ETB , TMC, ETM register dump eventually. Currently, just CPU context */
    for (count = 0; count < sysdbg_get_corecount(SUBSYS_APPS); count++)
    {
        args[0] = count; args[1] = 0;
        /* Add entries for the CPUs */
        if (!SysdbgAddDumpTableEntry((void *) &((*DumpTablePtr)->entries[DUMP_CLIENT_APPS]), CPU_REGS_DUMP, 0x200, args))
            return FALSE;
        if (!SysdbgAddDumpTableEntry((void *) &((*DumpTablePtr)->entries[DUMP_CLIENT_APPS]), ETM_DUMP, 0x1000, args))
         return FALSE;
    }

    /* Add ETB and TMC register dumps */
    args[0] = 0; args[1] = 0;
    if (!SysdbgAddDumpTableEntry((void *) &((*DumpTablePtr)->entries[DUMP_CLIENT_APPS]), ETB_DUMP, 0x1000, args))
         return FALSE;
    if (!SysdbgAddDumpTableEntry((void *) &((*DumpTablePtr)->entries[DUMP_CLIENT_APPS]), TMC_DUMP, 0x1000, args))
         return FALSE;


    /* IMC - core register context only */
    for (count = 0; count < sysdbg_get_corecount(SUBSYS_IMC); count++)
    {
        args[0] = count; args[1] = 0;
        /* Add entries for the CPUs */
        if (!SysdbgAddDumpTableEntry((void *) &((*DumpTablePtr)->entries[DUMP_CLIENT_IMC]), CPU_REGS_DUMP, 0x200, args))
            return FALSE;
    }

    /* Add the entry to back up IMC memories.
     *  Back up 768 KB memory which belongs to IMC CODE_RAM1, CODE_RAM2, DATA_RAM2/
    args[0] = 0x13B00000 */
    args[0] = 0x13B00000;
    args[1] = 0;
    if (!SysdbgAddDumpTableEntry((void *) &((*DumpTablePtr)->entries[DUMP_CLIENT_IMC]), RAWMEM_DUMP, 0xC0000, args))
        return FALSE;


    

    return TRUE;

}




/*===========================================================================
**  Function: sysdbgMemoryDumpInit
** ==========================================================================
*/
/*!
* 
* @brief
*   Function that initalizes the dump structure for the various dump types
*
*
* @param[in] 
*   None
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
*/
boolean SysdbgMemoryDumpInit(uint32 subsys)
{

    uint32 i, j;
    boolean initialized = FALSE;

    /* Update any of the internal pointers for the respective subsystem */
    if (!(subsys < SUBSYS_MAX))
        return FALSE;

    /* Check the cookie in OCIMEM that HLOS has setup for us */
    dump_table_type ** DumpTable = (dump_table_type **)(GetDumpTableStart());

    // Check if HLOS has populated the table after we "reset" the value
    if (*DumpTable != (dump_table_type *)SYSDBG_MAGIC_NUMBER)
    {
        for (i = 0; i < (*DumpTable)->num_entries; i++)
        {
            dump_entry_type top_entry = (**DumpTable).entries[i];

            /* Find the table populated for APPS*/
            if ((top_entry.type == DUMP_TYPE_TABLE) && (subsys == top_entry.id))
            {
                /* We have the apps dump table now. */
                dump_table_type *subsys_dump_table = (dump_table_type *)(uintptr_t)top_entry.start_addr;

                /* Parse all the entries within the apps dump table */
                for (j = 0; j < subsys_dump_table->num_entries; j++)
                {
                     dump_entry_type entry = subsys_dump_table->entries[j];

                     /* Find the corresponding entry for each feature for every CPU */
                    if ((entry.start_addr) && (entry.type == DUMP_TYPE_DATA))
                    {
                          /* The LSN of the id field indicates which CPU / block when the feature involves
                          *multiple CPUs or blocks*/
                          dump_data_type *data = (dump_data_type *)(uintptr_t)entry.start_addr;
                          uint32 block = data->arg0;

                            switch (entry.id)
                            {
                                case CPU_L1_ICACHE_DUMP:
                                    sysdbgCacheDump.L1ICacheDump[block] = data; 
                                    break;

                                case CPU_L1_DCACHE_DUMP:
                                    sysdbgCacheDump.L1DCacheDump[block] = data;
                                    break;

                                case CPU_L2_DCACHE_DUMP:
                                    sysdbgCacheDump.L2CacheDump[block] = data;
                                    break;
                          
                                case CPU_REGS_DUMP:
                                    SysdbgCPUDump.CPUDump[block] = data;
                                    break;

                                case ETB_DUMP:
                                    SysdbgETBDump = data;
                                    break;

                                case ETM_DUMP:
                                     SysdbgETMDump.ETMCPU[block] = data;
                                     break;


                                case TMC_DUMP:
                                    if ((block < MAX_TMC_TYPES))
                                        SysdbgTMCDump.TMCs[block] = data;
                                    break;

                                case RAWMEM_DUMP:
                                        SysdbgRawMemDump = data;
                                    break;

                                default:
                                    break;

                        }
                    }
                }
            }
        }
    
    /* The magic number was overwritten by someone. Assume dump table was populated */
    initialized = TRUE;
    }
    return initialized;
}


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
*   Dump name for which buffer is desired, instance of the block
*   e.g. for cpu context dump buffer, instance would be the sequential cpu
*   number
*
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
dump_data_type *sysdbg_get_buffer(subsys_type subsys, dump_type name, uint32 block)
{
    /* Keep a cache of the last set of buffers for the various features */
    static subsys_type last_subsys = SUBSYS_MAX;
    dump_data_type *buffer = NULL;
    boolean success = TRUE;

    if (subsys != last_subsys)
    {
        success = SysdbgMemoryDumpInit(subsys);
        last_subsys = subsys;
    }
        
  
    /* Make sure our dump table is intialized first */
    if (success)
    {
        switch (name)
        {
            case CPU_REGS_DUMP:
                buffer = SysdbgCPUDump.CPUDump[block];
                break;

            case ETB_DUMP:
                buffer = SysdbgETBDump;
                break;

            case ETM_DUMP:
                buffer = SysdbgETMDump.ETMCPU[block];
                break;

            case TMC_DUMP:
                buffer = SysdbgTMCDump.TMCs[block];
                break;
                
            case CPU_L1_ICACHE_DUMP:
                buffer = sysdbgCacheDump.L1ICacheDump[block];
                break;
                        
            case CPU_L1_DCACHE_DUMP:
                buffer = sysdbgCacheDump.L1DCacheDump[block];
                break;
                        
            case CPU_L2_DCACHE_DUMP:
                buffer = sysdbgCacheDump.L2CacheDump[block];
                break;

            case RAWMEM_DUMP:
                buffer = SysdbgRawMemDump;
                break;
            
            default:
                break;
        }
    }
    

    return buffer;
}



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
boolean SysdbgDumpMemories(uint32 subsys)
{
    dump_data_type *buffer = sysdbg_get_buffer(subsys, RAWMEM_DUMP, 0);

    if (buffer)
    {
        if (buffer->magic != SYSDBG_MAGIC_NUMBER)
        {
            /* Now copy the memory over */
            if ((buffer->arg0) && (buffer->start_addr))
            {
                memscpy((void *)buffer->start_addr, buffer->len, (void *)buffer->arg0, buffer->len);

                /* Write out the version and other info out */
                buffer->magic = SYSDBG_MAGIC_NUMBER;
                buffer->version = RAWMEM_DUMP_FORMAT_VERSION;
                memscpy(&buffer->name, sizeof(SYSDBG_NAME), SYSDBG_NAME, sizeof(SYSDBG_NAME));
            }
        }
    }

    return TRUE;
}



/*===========================================================================
**  Function: SysdbgDumpCPUCtxt
** ==========================================================================
*/
/*!
* 
* @brief
*   Function that copies the data from internal IMEM structure to external
*   DDR location
*
*
* @param[in] 
*   None
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
boolean SysdbgDumpCPUCtxt(uint32 subsys)
{
    uint32 cpu = get_current_cpu_seq(), regs_count = 0;
    /* Find the buffer for this cpu's context */
    dump_data_type *buffer = sysdbg_get_buffer(subsys, CPU_REGS_DUMP, cpu);
    cpu_ctxt_buf_type *srb = (cpu_ctxt_buf_type *)SysdbgGetCPUCtxtBuffer();
    const srb_index_type* srb_data = NULL;


    if (buffer)
    {
        if (buffer->magic != SYSDBG_MAGIC_NUMBER)
        {
            SysdbgCPUCtxtType *databuffer = (SysdbgCPUCtxtType *)(buffer->start_addr);

            if (!databuffer)
                return FALSE;
            else
            {
                /* Fetch our translation information */
                if (get_current_cpuname() == HYDRA)
                    srb_data = HydraSRBFormat();
                else
                    srb_data = ARMSRBFormat();

                /* Basic registers copied over. Copying only x2 - x29 because similarities between arm and hydra routines end there */
                regs_count =  offsetof(sysdbg_cpu64_ctxt_regs_type, x30) - offsetof(sysdbg_cpu64_ctxt_regs_type, x2);
                memscpy(&databuffer->cpu_regs.cpu64_ctxt.x2, regs_count, &srb->regs[srb_data->X2], regs_count);
                
                /* Copy other registers */
                databuffer->cpu_regs.cpu64_ctxt.x0       = srb->regs[srb_data->X0];
                databuffer->cpu_regs.cpu64_ctxt.x1       = srb->regs[srb_data->X1];
                databuffer->cpu_regs.cpu64_ctxt.sp_el3   = GET_REGVALUE(srb->regs, srb_data->SP_EL3); 
                databuffer->cpu_regs.cpu64_ctxt.elr_el3  = srb->regs[srb_data->ELR_EL3];
                databuffer->cpu_regs.cpu64_ctxt.spsr_el3 = srb->regs[srb_data->SPSR_EL3];
                databuffer->cpu_regs.cpu64_ctxt.sp_el2   = srb->regs[srb_data->SP_EL2];
                databuffer->cpu_regs.cpu64_ctxt.elr_el2  = srb->regs[srb_data->ELR_EL2];
                databuffer->cpu_regs.cpu64_ctxt.spsr_el2 = srb->regs[srb_data->SPSR_EL2];
                databuffer->cpu_regs.cpu64_ctxt.sp_el1   = srb->regs[srb_data->SP_EL1];
                databuffer->cpu_regs.cpu64_ctxt.elr_el1  = srb->regs[srb_data->ELR_EL1];
                databuffer->cpu_regs.cpu64_ctxt.spsr_el1 = srb->regs[srb_data->SPSR_EL1];
                databuffer->cpu_regs.cpu64_ctxt.sp_el0   = srb->regs[srb_data->SP_EL0];

                /* The PC for hydra comes from the SRB while for ARM it comes from debug ui */
                if (get_current_cpuname() == HYDRA)
                {
                    databuffer->cpu_regs.cpu64_ctxt.pc       = srb->regs[srb_data->PENDFETCHVA];

                /* Copy the floating point registers over */
                // regs_count = offsetof(sysdbg_neon_regs_type, q31) - offsetof(sysdbg_neon_regs_type, q0) + sizeof(fp_reg_type);
                // memscpy(&databuffer->neon_regs.neon_ctxt, regs_count, &srb->regs[srb_data->V0L], regs_count);

                /* Copy floating point status registers */
                    databuffer->neon_regs.fpscr = GET_REGVALUE(srb->regs, srb_data->FPSCR);
                    databuffer->neon_regs.fpexc = srb->regs[srb_data->FPEXC_EL2];

                /* Copy watchpoint and breakpoint registers */
                    for(uint32 i = 0; i < 8; i++)
                    {
                        databuffer->dbg_regs.bkpts[i].value   = srb->regs[srb_data->DBGBVR0_EL1 + i];
                        databuffer->dbg_regs.bkpts[i].control = srb->regs[srb_data->DBGBCR0_EL1 + i];
                    }

                    for (uint32 i = 0; i < 4; i++)
                    {
                        databuffer->dbg_regs.wtpts[i].value   = srb->regs[srb_data->DBGWVR0_EL1 + i];
                        databuffer->dbg_regs.wtpts[i].control = srb->regs[srb_data->DBGWCR0_EL1 + i];
                    }
                }



                /* Write out the version and other info out */
                buffer->magic = SYSDBG_MAGIC_NUMBER;
                buffer->version = CPU_DUMP_FORMAT_VERSION;
                memscpy(&buffer->name, sizeof(SYSDBG_NAME), SYSDBG_NAME, sizeof(SYSDBG_NAME));
            }
        }
    }

    return TRUE;
}
