/*==============================================================================
@file CoreMsgRam.c

This file provides access functions to AOP message RAM

 Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
                  All Rights Reserved.
          QUALCOMM Proprietary and Confidential.

==============================================================================*/
#include "CoreVerify.h"
#include "DALSys.h"
#include "CoreMsgRam.h"

#define SMEM_TABLE_MAX_SIZE     0xA

typedef struct
{
  uint32 smem_id;
  uint32 addr;
} smem_addr_t;

typedef struct
{
  uint32      initialized;	           //filled in by XBL once the table is initialized
  uint32      num_addrs;                   //size of the smem lookup table
  smem_addr_t table[SMEM_TABLE_MAX_SIZE];  //table of smem addresses
} aop_smem_table_t;

boolean smem_table_initialized = FALSE;
aop_smem_table_t *smem_table;

void msg_ram_init_base(uint32 *msg_ram_addr)
{
  smem_table = (aop_smem_table_t *)msg_ram_addr;
  smem_table->num_addrs = 0;
  smem_table->initialized = 1; 
  smem_table_initialized = TRUE;
}

/**
 * <!-- msg_ram_set_smem_addr -->
 *
 * @brief Write an smem address to message ram for a given smem id 
 *
 * @param smem_id : The smem ID correpsonding to the address we are saving
 * @param smem_addr : The smem address we are saving in message ram
 */ 
msg_ram_err msg_ram_set_smem_address(uint32 smem_id, uint32 smem_addr)
{
  uint32 smem_index = 0;
  if(!smem_table_initialized)
  {
    return MSG_RAM_SET_NOT_INITIALIZED;
  }

  CORE_VERIFY(smem_table->initialized == 1); 

  smem_index = smem_table->num_addrs;

  CORE_VERIFY(smem_index < SMEM_TABLE_MAX_SIZE);

  smem_table->table[smem_index].addr = smem_addr;
  smem_table->table[smem_index].smem_id = smem_id;
  smem_table->num_addrs++;

  return MSG_RAM_SET_SUCCESS;
}

