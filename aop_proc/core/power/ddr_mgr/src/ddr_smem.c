/*===========================================================================
                              ddr_smem.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddr_smem.h"
#include "ddr_aop_params.h"
#include "page_select.h"
#include "msg_ram_layout.h" // "aop_proc/core/power/utils/inc/msg_ram_layout.h"
#include "aop_ulog.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
#ifndef DDR_SWEVENTS
void ddr_log_msg32(char *str, uint32 data);
void ddr_log_msg8 (char *str, uint8 data1, uint8 data2, uint8 data3, uint8 data4);
#endif
//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================


/*
 * <!-- ddr_get_smem_addr -->
 *
 * @brief This function gets the size and address of the desired table in smem.
 *
 * @param table_id : The Table_id desired
 * @param size     : Variable in which to return the size.
 *
 * @return The address of the desired table or NULL if an error is seen.
*/
void *get_ddr_smem_struct(smem_tables table_id, uint16 *size)
{
  uint32 table_addr = 0x0;
  ddr_smem_info *smem_table = NULL;
  void *ret_val = NULL;
  smem_table_err smem_status;
  *size = 0;

  smem_status = msg_ram_get_smem_addr(DDR_AOP_SMEM_ID, &table_addr);
  if (smem_status != SMEM_TABLE_SUCCESS)
  {
    #ifndef DDR_SWEVENTS
    ddr_log_msg8("Bad retval: msg_ram_get_smem_addr", (DDR_AOP_SMEM_ID >> 8) & 0xFF,
                 DDR_AOP_SMEM_ID & 0xFF, 0, smem_status);
    #endif
    DDR_MGR_SWEVENT(ddr_BADRETVAL_MSG_RAM_ADDR, DDR_AOP_SMEM_ID, smem_status);
    return NULL;
  }

  smem_table = (ddr_smem_info *)set_ddr_access ((uint64_t)table_addr);

  if ((smem_table->major_version == AOP_PARAMS_MAJOR_VERSION) &&
      (smem_table->minor_version == AOP_PARAMS_MINOR_VERSION))
  {
    if (table_id < MAX_NUM_TABLES)
    {
      *size = smem_table->shared_table_header[table_id].size;
      ret_val = (void *) ((uint32) smem_table + (uint32) smem_table->shared_table_header[table_id].offset);
    }
     #ifndef DDR_SWEVENTS
    ddr_log_msg32("SMEM_TABLE returns", (uint32) ret_val);
    #endif
    DDR_MGR_SWEVENT(ddr_BADRETVAL_MSG_RAM_ADDR, DDR_AOP_SMEM_ID, smem_status);
  }
  else
  {
    #ifndef DDR_SWEVENTS
    ddr_log_msg8("SMEM_TABLE Version", smem_table->major_version, smem_table->minor_version, 0, 0);
    #endif
    DDR_MGR_SWEVENT(ddr_SMEM_TABLE_RETURNS, smem_table->major_version, smem_table->minor_version);
  }

  return ret_val;
}


/*
 * <!-- get_ddr_table -->
 *
 * @brief This function gets the SMEM_FREQ_STATE table from SMEM and puts it into a memory buffer
 *
 * @param table_id : The Table_id desired
 * @param entries  : Number of entries in the table
 *
 * @return The address of the desired table or NULL if an error is seen.  Caller must free the structure.
*/
FREQ_STATE *get_ddr_table(smem_tables table_id, uint8 *entries)
{
  void *table_addr;
  uint16 table_size;
  uint16 num_entries = 0;
  FREQ_STATE *freq_state = NULL;

  table_addr = get_ddr_smem_struct(table_id, &table_size);
  if (NULL != table_addr)
  {
    num_entries = table_size / sizeof(FREQ_STATE);
    if ((num_entries > 0)    &&
        (num_entries <= 16)  &&
        (num_entries * sizeof(FREQ_STATE) == table_size))
    {
      freq_state = malloc(table_size);
      if (freq_state != NULL)
      {
        memcpy(freq_state, table_addr, table_size);
      }
    }

    set_page_select (0);
  }

  *entries = num_entries;
  return freq_state;
}
