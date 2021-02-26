/*==============================================================================
@file CoreMsgRam.h

Common API for Message RAM Accesses

 Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
                  All Rights Reserved.
          QUALCOMM Proprietary and Confidential.

==============================================================================*/
#ifndef COREMSGRAM_H
#define COREMSGRAM_H

typedef enum
{
  MSG_RAM_SET_SUCCESS,
  MSG_RAM_SET_NOT_INITIALIZED,
} msg_ram_err;

/**
 * <!-- msg_ram_init_base -->
 *
 * @brief Initialize the msg ram base address to use 
 *
 * @param base_addr : The msg_ram address we are saving in message ram
 */ 
void msg_ram_init_base(uint32 *msg_ram_addr);

/**
 * <!-- msg_ram_set_smem_addr -->
 *
 * @brief Write an smem address to message ram for a given smem id 
 *
 * @param smem_id : The smem ID correpsonding to the address we are saving
 * @param smem_addr : The smem address we are saving in message ram
 */ 
msg_ram_err msg_ram_set_smem_address(uint32 smem_id, uint32 smem_addr);

#endif /* COREMSGRAM_H */
