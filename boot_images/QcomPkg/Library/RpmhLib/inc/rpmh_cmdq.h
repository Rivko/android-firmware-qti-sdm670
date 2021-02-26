#ifndef RPMH_CMDQ_H
#define RPMH_CMDQ_H

/*===========================================================================
                             rpmh_cmdq.h

DESCRIPTION:
  Interface for rpmh command queues. Defines types and functions needed to
  track commands and their associated states and details. 

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include "inq.h"
#include "rpmh_resource_commands.h"
#include "rpmh_client_internal.h"
#include "rpmh_client.h"
#include "rsc_hal.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/*
 * cmdq structure
 */
typedef struct rpmh_cmdq_s rpmh_cmdq_t;

/*
 * command details sub-structure
 */      
typedef struct
{
  uint32 address;      // address for the given resource command. Concatenation of {slaveID offset}
  RSCRequestType type; // request type for the given resource command
} rpmh_cmd_details_t;

/*
 * command structure
 * NOTE: delete_cmd() relies on the ordering. All lists need to be at the top of the structure 
 *       for proper clearing
 */      
typedef struct
{
  rpmh_cmd_details_t      *details;        //list of resource commands
  uint32                  num_rcs;         //number of resources commands in this command
  uint32                  dependency_bmsk; //NOT USED FOR NOW, dependency bmask for TSC
  rpmh_client_t           *client;         //pointer to client who owns this command
  uint32                  req_id;          //command id
  rpmh_set_enum           set;             //set for the command
  boolean                 amc;             //whether or not this command is an AMC
  inq_link_t              link;            //link for the command queue
} rpmh_cmd_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- rpmh_cmdq_create -->
 *
 * @brief Create a new command queue 
 *
 * @param drv_index : The index into the DRV config to create the queue
 *
 * @return A pointer to the cmdq 
 */ 
rpmh_cmdq_t *rpmh_cmdq_create (uint32 drv_index);

/**
 * <!-- rpmh_cmdq_destroy -->
 *
 * @brief Delete the given cmdq 
 *
 * @param self : The cmdq to destroy
 */ 
void rpmh_cmdq_destroy (rpmh_cmdq_t *self);

/**
 * <!-- rpmh_cmdq_send_read -->
 *
 * @brief Send a new command 
 *
 * @param self    : The cmdq associated with the send
 * @param address : The address to read
 * @param client  : The client who initiated the command
 * @param queue   : V1 workaround - if there are commands in flight we queue the read instead of 
 *                  sending it right away
 *
 * @return The command id for the request 
 */ 
uint32 rpmh_cmdq_send_read (rpmh_cmdq_t *self, uint32 address, rpmh_client_t *client, boolean queue);

/**
 * <!-- rpmh_cmdq_send_cmd_set -->
 *
 * @brief Send a new command  set
 *
 * @param self         : The cmdq associated with the send
 * @param client       : The client who initiated the command set
 * @param command_set  : The command set with all the request details
 *
 * @return The command id for the request 
 */ 
uint32 rpmh_cmdq_send_cmd_set (rpmh_cmdq_t *self, rpmh_client_t *client, 
                               rpmh_command_set_internal_t *command_set);

/**
 * <!-- cmdq_finish_cmd -->
 *
 * @brief A command has completed, finish at the queue level 
 *
 * @param self   : The cmdq associated with the command
 * @param cmd    : The command to finish
 * @param drv_id : The RSC+DRV mapping that we are finishing the command for
 * @param tcs_id : The TCS ID that the command finished on. 
 *                 This is a passthrough for the TCS layer to retrieve read data for read commands
 * @param set    : The set associated with the command completion
 */ 
void cmdq_finish_cmd (rpmh_cmdq_t *self, rpmh_cmd_t *cmd, RSCSW_DRV_MAPPING drv_id, uint8 tcs_id, rpmh_set_enum set);

/**
 * <!-- rpmh_cmdq_send_cmd_at_head -->
 *
 * @brief Send a new command from the head of the enqueued queue
 *
 * @param self : The cmdq associated with the send
 *
 * @return TRUE if sent, false if not because the resource was busy or there were no AMCs
 */ 
boolean rpmh_cmdq_send_cmd_at_head (rpmh_cmdq_t *self);

/**
 * <!-- rpmh_cmdq_flush_sleep_wake_amcs -->
 *
 * @brief Send all of the sleep/wake AMCs. Should be called on the way in/out of sleep 
 *
 * @param self   : The cmdq from which to send the sleep/wake AMCs
 * @param drv_id : The RSC+DRV mapping on which to flush
 * @param set    : The set to indicate which to send (sleep or wake)
 */ 
void rpmh_cmdq_flush_sleep_wake_amcs (rpmh_cmdq_t *self, RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set);

/**
 * <!-- cmdq_empty -->
 *
 * @brief Checks if the corresponding cmdq is empty or not 
 *
 * @param self   : cmdq on which we check if any commands are pending
 *
 * @return TRUE if command is empty, FALSE if any commands are in the cmdq
 */ 
boolean cmdq_empty (rpmh_cmdq_t *self);

/**
 * <!-- cmdq_num_in_flight -->
 *
 * @brief Checks how many requests are in flight 
 *
 * @param self : cmdq on which we check if any commands are pending
 *
 * @return number of commands in flight
 */ 
uint32 cmdq_num_in_flight(rpmh_cmdq_t *self);

/**
 * <!-- rpmh_clean_cmd -->
 *
 * @brief Clean the given cmd 
 *
 * @param cmd      : The command to clean
 * @param num_cmds : The number of resource commands in the command
 */ 
void rpmh_clean_cmd (rpmh_cmd_t *cmd, uint32 num_cmds);

#ifdef RPMH_OFFTARGET_TEST
/**
 * <!-- print_cmd_list -->
 *
 * @brief Prints a list of commands
 *
 * @param cmds : list of commands to print
 */
void print_cmd_list (rpmh_cmd_t *cmds);

/**
 * <!-- print_cmdqs -->
 *
 * @brief Prints current cmd queues (queued, in flight, sleep/wake amcs)
 *
 * @param self : list of cmdqs to print
 */
void print_cmdqs (rpmh_cmdq_t **self);
#endif //RPMH_OFFTARGET_TEST

//TODO: void rpmh_cmdq_update_resource_pointers(rpmh_cmdq_t *self);

#endif // RPMH_CMDQ_H

