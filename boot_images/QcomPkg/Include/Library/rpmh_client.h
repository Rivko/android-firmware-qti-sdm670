#ifndef RPMH_CLIENT_H
#define RPMH_CLIENT_H

/*===========================================================================
                             rpmh_client.h

DESCRIPTION:
  RPMh Command Services APIs

  This is the main interface for issuing commands to RPMh. It will be used by
  resource owners who need to issue commands to RPMh HW. It interacts with 
  RSC hardware underneath to facilitate RPMh commands to the proper RPMh HW 
  block. The driver stores each command locally and synchronizes the desired 
  state with the state at RPMh.

  Commands can be issued either as fire-and-forget or as completion. In order
  to confirm a command has been applied at the HW, commands must be issued as
  completion, AND an rpmh_barrier API must be called (which blocks until the
  command(s) have finished).

  This interface also provides sleep interaction to prepare for entering and 
  exiting sleep. This is necessary to configure the RSC TSC contents as desired
  by clients.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//==========================================================================
//                   Includes and Public Data Declarations
//==========================================================================

//--------------------------------------------------------------------------
// Include Files 
//--------------------------------------------------------------------------
#include "rpmh_common.h"
#include "rpmh_image_config.h"

//--------------------------------------------------------------------------
// Constant /Define Declarations 
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Type Declarations 
//--------------------------------------------------------------------------

/*************************************//**
 * RPMh Command Structures
 *
 * These structures are used along with the RPMh APIs to issue commands to RPMh blocks.
 ****************************************/

/* 
 * Client callback for completion 
 */
typedef void ( *rpmh_completion_cb )( uint32 req_id );

/**
 * Individual command
 */
typedef struct
{
  uint32 address;     // address for the given resource command. Concatenation of {slaveID, offset}
  uint32 data;        // data to be sent 
  boolean completion; // TRUE for a completion request, FALSE for a fire and forget request
} rpmh_command_t;

/**
 * Command set structure used to send multiple commands in parallel
 */
typedef struct
{
  rpmh_set_enum set;                 // The set associated with these commands
  uint32 num_commands;               // The number of commands being sent
  uint32 reserved;                   // reserved
  rpmh_command_t commands[IMAGE_TCS_SIZE]; // An array of commands to be sent
} rpmh_command_set_t;

/** 
 * opaque client handle (one per client) used to 
 *      differentiate between RSCs
 *      help parallelize between clients
 */
typedef struct rpmh_client_t *rpmh_client_handle;

//--------------------------------------------------------------------------
// Forward Declarations 
//--------------------------------------------------------------------------

//==========================================================================
//                   Funciton Definitions
//==========================================================================


/*************************************//**
 * RPMh APIs
 *
 * These APIs are used to faciliate RPMh communication. They provide functionality to send commands
 * to RPMh hardware blocks, and to understand when those commands have finished. There is additional
 * functionality provided to help coordinate sleep enter/exit.
 ****************************************/

/**
 * <!-- rpmh_client_init -->
 *
 * @brief This should be called once by the OS to initialize the RPMh driver. 
 *        This must be done before any clients use other RPMh APIs 
 */
void rpmh_client_init (void);

void rpmh_exit(void);

/**
 * <!-- rpmh_create_handle -->
 *
 * @brief Create a client handle. 
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating BCMs should have a different handle then the client aggregating VRMs 
 *
 * @param drv_id      : The RSC+DRV mapping for the client
 * @param client_name : A unique string to identify the client
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle (RSCSW_DRV_MAPPING drv_id, const char *client_name);

/**
 * <!-- rpmh_create_handle_explicit -->
 *
 * @brief Create an explicit client handle. 
 *
 *  Explicit client means for sleep/wake requests, the client will take
 *  care of telling the RPMh driver exactly what is required via rpmh_issue_command_set. The RPMh
 *  driver will not fill in any wake/sleep values based on active requests.
 *
 *  Separate client handles should be created for issuing requests 
 *  to different RSCSW_DRV_MAPPING, and for clients handling different blocks. For example, the 
 *  driver aggregating bus resource commands should have a different handle then the client 
 *  aggregating pmic resource commands.
 *
 * @param drv_id          : The RSC+DRV mapping for the client
 * @param client_name     : A unique string to identify the client
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
rpmh_client_handle rpmh_create_handle_explicit (RSCSW_DRV_MAPPING drv_id, const char *client_name);

/**
 * <!-- rpmh_explicit_client_invalidate -->
 *
 * @brief Invalidate requests for the given explicit client 
 *
 * @param drv_id : The explicit client on which to invalidate votes
 *
 * @return An opaque pointer to the handle, to be used for future RPMh driver APIs
 */
void rpmh_invalidate_explicit_cmds(rpmh_client_handle client_handle);

/**
 * <!-- rpmh_client_completion_callback -->
 *
 * @brief Each client is allowed a single callback for completion requests. 
 *        This API registers that callback 
 *
 * @param client_handle : The client associated with the callback being registered
 * @param callback      : The callback function
 */
void rpmh_client_register_completion_callback(rpmh_client_handle client_handle, rpmh_completion_cb callback);

/**
 * <!-- rpmh_delete_handle -->
 *
 * @breif Delete a client handle.
 *
 * @param handle : The handle to delete
 */ 
void rpmh_delete_handle (rpmh_client_handle handle);

/**
 * <!-- rpmh_issue_command -->
 *
 * @brief Issue a single command
 *
 * @param handle     : The client handle associated with the command
 * @param set        : The set associated with the command
 * @param completion : TRUE for completion requests, FALSE for fire and forget
 * @param address    : Concatenation of slaveID and offset
 * @param data       : Data to be sent
 *
 * @return ID for this command on this handle. Used in barrier APIs to track completion
 */
uint32 rpmh_issue_command (rpmh_client_handle handle, rpmh_set_enum set, boolean completion, 
                           uint32 address, uint32 data);

/**
 * <!-- rpmh_issue_command_set -->
 *
 * @brief Issue a command set (multiple commands in parallel). If none of the resource commands
 *          are dirty, the command set is not sent. However if at least one resource command is
 *          dirty, the command set will be sent. The not dirty commands will finish quickly as
 *          the HW aggregation won't update, so the dirty commands will be the long pole.
 *
 * @param handle      : The client handle associated with the command set
 * @param command_set : The structure containing all the details of the command set request
 *
 * @return ID for command set on this handle. Used in barrier APIs to track completion
 */ 
uint32 rpmh_issue_command_set (rpmh_client_handle handle, rpmh_command_set_t *command_set);

/**
 * <!-- rpmh_invalidate_wake_command -->
 *
 * @brief Remove the wake command for a given address (resource command). 
 *
 * @param handle  : The client handle associated with the invalidate
 * @param address : Concatenation of slaveID and offset
 */
void rpmh_invalidate_wake_command (rpmh_client_handle client_handle, uint32 address);

/**
 * <!-- rpmh_invalidate_sleep_command -->
 * @breif Remove the sleep command for a given address (resource command).
 *
 * @param handle  : The client handle associated with the invalidate
 * @param address : Concatenation of slaveID and offset
 */
void rpmh_invalidate_sleep_command (rpmh_client_handle client_handle, uint32 address);

/**
 * <!-- rpmh_barrier_all -->
 * @brief Event based blocking call to check if the given req_id, 
 *        and every req_id prior has finished for the given handle. 
 *
 * @param handle : The client handle associated with the req_id
 * @param req_id : blocks until this ID and evertyhing prior (for the given handle) has finished
 */
void rpmh_barrier_all (rpmh_client_handle handle, uint32 req_id);

/**
 * <!-- rpmh_barrier_single -->
 *
 * @brief Event based blocking call to check if the given req_id has finished for the given handle.  
 *
 * @param handle : The client handle associated with the req_id
 * @param req_id : blocks until this ID (for the given handle) has finished 
 */
void rpmh_barrier_single (rpmh_client_handle handle, uint32 req_id);

/**
 * <!-- rpmh_is_drv_id_valid -->
 *
 * @breif Checks whether or not the given RSC+DRV is valid for the context
 *
 * @param drv_id : The RSC+DRV mapping to check
 *
 * @return whether or not the given ID is supported in the given context
 */
boolean rpmh_is_drv_id_valid (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- rpmh_tcs_num_cmds -->
 *
 * @brief Return the # commands per TCS for this DRV
 *
 * @param drv_id : RSC+DRV id for num cmds check
 *
 * @return number of commands in the TCSs for this DRV
 */
uint32 rpmh_tcs_num_cmds (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- rpmh_notify_completion_all -->
 *
 * @brief Trigger the client's callback with the corresponding req_id when all commands up to and 
 *   including the given req_id have completed
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_notify_completion_all (rpmh_client_handle client_handle, uint32 req_id);

/**
 * <!-- rpmh_notify_completion_single -->
 *
 * @brief Trigger the client's callback with the corresponding req_id when this request has finished
 *
 * @param handle : The handle the command is associated with
 * @param req_id : The command ID to wait on
 */ 
void rpmh_notify_completion_single (rpmh_client_handle client_handle, uint32 req_id);

/*************************************//**
 * Sleep Only APIs
 *
 * Used for single thread mode when entering and exiting sleep
 ****************************************/

/**
 * <!-- rpmh_churn_all -->
 *
 * @brief Polling based blocking call to check if the given req_id, 
 *        and every req_id prior has finished for the given handle.  
 *
 * @param handle : The client handle associated with the req_id
 * @param req_id : blocks until this ID and evertyhing prior (for the given handle) has finished 
 */
void rpmh_churn_all (rpmh_client_handle client_handle, uint32 req_id);

/**
 * <!-- rpmh_churn_single -->
 * @brief Polling based blocking call to check if the given req_id finished for the given handle. 
 *
 * @param handle : The client handle associated with the req_id
 * @param req_id : blocks until this ID (for the given handle) has finished 
 */
void rpmh_churn_single (rpmh_client_handle client_handle, uint32 req_id);

/**
 * <!-- rpmh_enter_sleep -->
 *
 * @breif API to configure RSCSW_DRV for entering sleep. 
 *        Configures RSC TCSs, sends sleep AMCs if needed  
 *
 * @param drv_id : The RSC+DRV mapping to configure for sleep
 */
void rpmh_enter_sleep (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- rpmh_exit_sleep -->
 *
 * @brief API to configure RSCSW_DRV for exiting sleep. 
 *        Sends wake AMCs if needed, cleans up driver context sleep 
 *
 * @param drv_id : The RSC+DRV mapping to configure for sleep exit
 */
void rpmh_exit_sleep (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- rpmh_get_wake_latency -->
 *
 * @brief Retrieve the wake latency for the given DRV ID. For now this is a constant value
 *        roughly equivalent to the worst case time for the wake TCS to complete
 *
 * @param drv_id : The DRV associated with the wake set
 *
 * @return Wake TCS latency in 19.2Mhz ticks
 */ 
uint32 rpmh_get_wake_latency(RSCSW_DRV_MAPPING drv_id);

#ifdef RPMH_OFFTARGET_TEST
/*************************************//**
 * OFFTARGET TEST APIs
 *
 * Used for printing in an offtarget test environment
 ****************************************/

/**
 * <!-- rpmh_offtarget_test_print_drvs -->
 *
 * @brief Prints DRV config and current TCS config for each supported DRV
 */
void rpmh_offtarget_test_print_drvs ();

/**
 * <!-- rpmh_offtarget_test_print_clients -->
 *
 * @brief Prints current client list and associated client details
 */
void rpmh_offtarget_test_print_clients ();

/**
 * <!-- rpmh_offtarget_test_print_resource_commands -->
 *
 * @brief Prints current resource command list and associated details
 */
void rpmh_offtarget_test_print_resource_commands ();

/**
 * <!-- rpmh_offtarget_test_print_drv -->
 *
 * @brief Prints DRV config and current TCS config for the given drv
 *
 * @param drv_id : The RSCSW_DRV_MAPPING to print config
 */
void rpmh_offtarget_test_print_drv (RSCSW_DRV_MAPPING drv_id);

/**
 * <!-- rpmh_offtarget_test_print_cmdqs -->
 *
 * @brief Prints current command queues (queued, in flight, sleep/wake amcs)
 */
void rpmh_offtarget_test_print_cmdqs ();

/**
 * <!-- rpmh_get_core_verify_failures -->
 * @brief Used to track the # of core_verify failures for offtarget testing.  
 *
 * @return The # of core_verify failures so far in this run 
 */
uint32 rpmh_get_core_verify_failures ();

#endif //RPMH_OFFTARGET_TEST

#ifdef __cplusplus
}
#endif //__cplusplus


#endif /* RPMH_CLIENT_H */

