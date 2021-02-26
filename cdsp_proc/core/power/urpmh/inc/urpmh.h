#ifndef URPMH_H
#define URPMH_H
/*=============================================================================
FILE:         urpmh.h

OVERVIEW:     This file provides the public externs and definitions that
are exported by the rpm module for ADSP uImage.

Copyright (c) 2013 - 2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/urpmh/inc/urpmh.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "rpmh_client.h"
#include "rpmh_client_internal.h"

#define SLAVE_ID(address) address >> 0x10  
#define OFFSET(address) address & 0xFFFF 

#define NUM_URPMH_CMD_SETS 6

typedef enum
{
  ISLAND_EMPTY = 0,     //this is used to indicate the cmd_set is not in use	
  ISLAND_ENTER_AMC = 1, //on entering island mode we send AMCs to reduce power
  ISLAND_EXIT_AMC = 2,  //one exiting island mode we restore state
  ISLAND_READ_AMC = 3,  //during island mode we send read commands
  ISLAND_CMD_SET_MAX
} urpmh_cmd_set_type_enum;

typedef enum
{
  URPMH_READ_SUCCESS            = 0,
  URPMH_READ_FAIL_NO_CMD_SET    = 1,
  URPMH_READ_FAIL_NOT_IN_ISLAND = 2,
  URPMH_READ_MAX                = 3,
} urpmh_read_enum;

typedef struct
{
  urpmh_cmd_set_type_enum     type;
  uint32                      tcs_num;
  rpmh_command_set_internal_t cmd_set;
} urpmh_command_set_t;


/**
 * <!-- urpmh_issue_read -->
 *
 * @brief Issue a read AMC  
 *
 * @param address : the address to read
 * @param read_val : the value read at the hardware
 *
 * @return URPMH_READ_SUCCESS if completed the read successfully
 */ 
urpmh_read_enum urpmh_issue_read(uint32 address, uint32 *read_val);

/**
 * <!-- urpmh_island_enter -->
 *
 * @brief Called on island entry. Sends the proper island enter AMCs  
 */ 
void urpmh_island_enter(void);

/**
 * <!-- urpmh_island_exit -->
 *
 * @brief Called on island exit. Sends the proper island exit AMCs  
 */ 
void urpmh_island_exit(void);

#endif /* URPMH_H */

