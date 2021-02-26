#ifndef RPMH_CLIENT_INTERNAL_H
#define RPMH_CLIENT_INTERNAL_H

/*===========================================================================
                             rpmh_client_internal.h

DESCRIPTION:
  Interface for client handle functionality and for event (barrier)
  functionality. Defines the types and functions needed to track client details 
  and events for finishing commands. 

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
#include "rpmh_common.h"
//#include "../common/inq.h"
#include "inq.h"
#include "rpmh_events.h"
#include "rpmh_client.h"
#include "rpmh_resource_commands.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define NUM_CLIENT_CMDS 0x10
#define CMD_EMPTY 0xFFFFFFFF

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

/* 
 * Client queue structure 
 */

typedef struct
{
  inq_t *handles;      //allocated clients
  inq_t *free_handles; //free pool of clients
}rpmh_clientq_t;

/* 
 * wait types used for RPM barriers. 
 */
typedef enum
{
  RPMH_BARRIER_SINGLE = 0, //wait for just the req_id 
  RPMH_BARRIER_ALL = 1,        //wait for everything <= req_id 
  RPMH_CALLBACK_SINGLE,    
  RPMH_CALLBACK_ALL,
  RPMH_BARRIER_MAX
} rpmh_barrier_enum;

/* 
 * Structure used to track a wait event 
 */
typedef struct
{
    uint32            req_id;       //command id for the wait event
    rpmh_event_t      *event;       //OS event
    rpmh_barrier_enum barrier_type; //type of event
    inq_link_t        link;         //link for the event queue
} active_events_t;

/*
 * Internal command set type to hide dependency bmsk for now
 */
typedef struct
{
  rpmh_set_enum set;                 // The set associated with these commands
  uint32 num_commands;               // The number of commands being sent
  uint32 dependency_bmsk;            // Dependency bitmask for the command set
  rpmh_command_t commands[IMAGE_TCS_SIZE]; // An array of commands to be sent
} rpmh_command_set_internal_t;

/*
 * Structure used to track a command in progress
 */
typedef struct
{
    uint32         req_id;     //command id in progress
    uint32         data;       //used to return data for read commands
    boolean        data_valid; //used to return data for read commands
    inq_link_t     link;       //link for the commands in progress queue
} cmds_in_progress_t;

/*
 * Explicit command set structure
 */
typedef struct
{
  uint32 cmd_sets_in_use;
  uint32 cmd_sets_total;
  rpmh_command_set_internal_t *cmd_sets;
} rpmh_explicit_cmd_set_t;

/*
 * Internal client handle structure
 */
typedef struct
{
  RSCSW_DRV_MAPPING drv_id;                 //RSC+DRV id for the client
  const char *name;                         //client name passed to us by the client
  inq_t             *cmds_in_progress;      //list of commands in progress
  inq_t             *free_cmds_in_progress; //list of free commands in progress
  inq_t             *barriers;              //list of events
  inq_t             *free_barriers;         //list of free events
  uint32 req_id;                            //next request id (always increasing)
  uint32 oldest_req_id;                     //oldest request id in progress
  uint32 num_reqs_in_progress;              //number of requests in progress
  rpmh_completion_cb completion_cb;         //callback for when the given client finishes a request
  rpmh_explicit_cmd_set_t  *explicit_cmds;  //for explicit clients
  inq_link_t link;                          //link for the client queue
} rpmh_client_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- rpmh_clientq_create -->
 *
 * @brief Create a client queue 
 *
 * @return A pointer to the client queue
 */ 
rpmh_clientq_t *rpmh_clientq_create (void);

/**
 * <!-- rpmh_client_command_add -->
 *
 * @brief Add a new command in progres to the given handle 
 *
 * @param handle : The handle on which to add a new command
 */ 
void rpmh_client_command_add (rpmh_client_t *handle);

/**
 * <!-- rpmh_client_command_finish -->
 *
 * @brief A command has finished, clean it up at the client level
 *
 * @param handle : The handle associated with the finished command
 * @param req_id : The command id that has finished
 */ 
void rpmh_client_command_finish (rpmh_client_t *handle, uint32 req_id);


/**
 * <!-- rpmh_client_save_read_data -->
 *
 * @brief Used to store data from a read command
 *
 * @param handle : The handle that initiated the read request
 * @param req_id : The read command id
 * @param read   : The data from the read
 */ 
void rpmh_client_save_read_data (rpmh_client_t *handle, uint32 req_id, uint32 data);

/**
 * <!-- rpmh_client_handle_add -->
 *
 * @brief Add a client handle to the given queue 
 *
 * @param self : The queue to add to
 *
 * @return A pointer to the new client handle
 */ 
rpmh_client_t *rpmh_client_handle_add (rpmh_clientq_t *self);

/**
 * <!-- rpmh_client_handle_remove -->
 *
 * @brief Remove the client handle from the given queue 
 *
 * @param self   : The queue to remove from
 * @param handle : The handle to remove
 *
 * @return A pointer to the new client handle
 */ 
void rpmh_client_handle_remove (rpmh_clientq_t *self, rpmh_client_t *handle);

/**
 * <!-- rpmh_client_wait_on_cmd -->
 *
 * @brief Wait until the given command is finished
 *
 * @param handle       : The handle associated with the command
 * @param req_id       : The command id we are waiting for
 * @param barrier_type : The type of wait (all vs single)
 * @param polling      : TRUE for polling mode, FALSE for event based handling
 */ 
void rpmh_client_wait_on_cmd (rpmh_client_t *handle, uint32 req_id, rpmh_barrier_enum barrier_type, boolean churn);

/**
 * <!-- rpmh_client_command_wait_on_read -->
 *
 * @brief Wait until a given read command is finished
 *
 * @param handle  : The handle associated with the read command
 * @param req_id  : The read command id we are waiting for
 * @param polling : TRUE for polling mode, FALSE for event based handling
 */ 
uint32 rpmh_client_wait_on_read (rpmh_client_t *handle, uint32 req_id, boolean churn);

/**
 * <!-- rpmh_client_register_req_notification -->
 *
 * @brief Register to callback to the given client when the given req_id has completed
 *
 * @param handle       : The handle associated with request
 * @param req_id       : The command id we are waiting for
 * @param barrier_type : The type of wait (all vs single)
 */ 
void rpmh_client_register_req_notification (rpmh_client_t *handle, uint32 req_id, rpmh_barrier_enum barrier_type);

/**
 * <!-- rpmh_finish_amc -->
 *  @brief Used to finish a command. Usually triggered by an interrupt in HW. 
 *         Exposed for ease of offtarget testing
 *
 *  @param drv_id : The RSCSW_DRV_MAPPINGon which to finish a command 
 */
void rpmh_finish_amc (RSCSW_DRV_MAPPING drv_id); 

/**
 * <!-- rpmh_finsh_sleep_wake_amc -->
 *
 * @brief Handle a finished sleep or wake AMC. 
 *
 * @param drv_id : The RSC+DRV mapping for the AMC
 * @param set    : The set associated with the finished AMC
 */ 
void rpmh_finish_sleep_wake_amc (RSCSW_DRV_MAPPING drv_id, rpmh_set_enum set);

/**
 * <!-- rpmh_find_resource_command -->
 *
 * @brief Lookup the resource command for the corresponding address
 *
 * @param address : The address of the corresponding resource command to find
 *
 * @return a pointer to the resource command
 */ 
rpmh_resource_command_t *rpmh_find_resource_command (uint32 address);

#ifdef RPMH_OFFTARGET_TEST
/**
 * <!-- print_clients -->
 *
 * @brief Prints current client list and associated client details
 */
void print_clients (rpmh_clientq_t *self);
#endif //RPMH_OFFTARGET_TEST

#endif /* RPMH_CLIENT_INTERNAL_H */

