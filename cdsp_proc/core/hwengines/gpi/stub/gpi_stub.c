/**
 * @file  gpi_stub.c
 * @brief Implements the GSI interface stubs
 */
/*
===============================================================================

                               Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
02/19/16   ah      File created.

===============================================================================
              Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "gpi_i.h"


/**
 * Main function to initialize GPI EE, target, and gloabl data structure.
 * To be called from some entity before any GPI request is made, and ONLY after all GSI HW instances
 * are enabled.
 * 
 * @param[in]    None.
 *
 * @return       None.
 */
void gpi_init(void)
{
   return;
}

/**
 * Registers a client GPI interface.
 *
 * @param[in]    params                GPI interface registration parameters
 *
 * @return       GPI_RETURN_STATUS     GPI_STATUS_SUCCESS for success, GPI_STATUS_ERROR otherwise.
 */
GPI_RETURN_STATUS gpi_iface_reg(gpi_iface_params *params)
{
   return GPI_STATUS_ERROR;
}

/**
 * Queries wheteher a GPI interface is running in interrupt or polling mode.
 *
 * @param[in]        handle            Handle to GPI Interface
 * @param[in, out]   irq_mode          1 if Interface in IRQ mode, 0 otherwise
 *
 * @return       GPI_RETURN_STATUS     GPI_STATUS_SUCCESS for success, GPI_STATUS_ERROR otherwise.
 */
GPI_RETURN_STATUS gpi_iface_in_irq_mode(GPI_CLIENT_HANDLE handle, uint8 *irq_mode)
{
   return GPI_STATUS_ERROR;
}

/**
 * Enables/Disables a GPI interface IRQ mode.
 *
 * @param[in]    handle               Handle to GPI Interface
 * @param[in]    irq_en               TRUE to enable IRQ mode, FALSE for polling
 * 
 * @return       GPI_RETURN_STATUS    GPI_STATUS_SUCCESS for success, GPI_STATUS_ERROR otherwise.
 */
GPI_RETURN_STATUS gpi_iface_set_irq_mode(GPI_CLIENT_HANDLE handle, boolean irq_en)
{
   return GPI_STATUS_ERROR;
}

/**
 * Queries the channel current status.
 *
 * @param[in]        handle              Handle to GPI Interface
 * @param[in]        chan                Virtual Channel ID: 0 (INBOUND_CHAN), 1 (OUTBOUND_CHAN)
 * @param[in, out]   chan_status         Information about channel current status
 *
 * @return         GPI_RETURN_STATUS     GPI_STATUS_SUCCESS for success, GPI_STATUS_ERROR otherwise.
 */
GPI_RETURN_STATUS gpi_query_chan_status(GPI_CLIENT_HANDLE handle,
                                        GPI_CHAN_TYPE chan,
                                        chan_status_info *chan_status)
{
   return GPI_STATUS_ERROR;
}

/**
 * Polls a GPI interface. 
 * The corresponding callback will be invoked with any available results.
 *
 * @param[in]    handle               GPI client handle
 *
 * @return       GPI_RETURN_STATUS     handle that clients use in future interactions with GPI
 */
GPI_RETURN_STATUS gpi_iface_poll(GPI_CLIENT_HANDLE handle)
{
   return GPI_STATUS_ERROR;
}

/**
 * Issues a channel command.
 *
 * @param[in]      handle              Handle to GPI Interface
 * @param[in]      chan                Virtual Channel ID: 0 (INBOUND_CHAN), 1 (OUTBOUND_CHAN)
 * @param[in]      cmd_type            Command type (ALLOCATE_CHAN, START_CHAN, STOP_CHAN...)
 * @param[in]      user_data           Client context data for this command
 *
 * @return       GPI_RETURN_STATUS     GPI_STATUS_SUCCESS for success, GPI_STATUS_ERROR otherwise.
 */
GPI_RETURN_STATUS gpi_issue_cmd(GPI_CLIENT_HANDLE handle,
                                GPI_CHAN_TYPE chan,
                                GPI_CHAN_CMD cmd_type,
                                void *user_data)
{
   return GPI_STATUS_ERROR;
}

/**
 * Process TREs request.
 *
 * @param[in]    tre_request           Request details
 * 
 * @return       GPI_RETURN_STATUS     GPI_STATUS_SUCCESS for success, GPI_STATUS_ERROR otherwise.
 */
GPI_RETURN_STATUS gpi_process_tre(gpi_tre_req *tre_request)
{
   return GPI_STATUS_ERROR;
}

/**
 * De-Registers a client GPI interface.
 *
 * @param[in]    handle            Handle to GPI Interface
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_dereg(GPI_CLIENT_HANDLE handle)
{
   return GPI_STATUS_ERROR;
}

/**
 * gpi_firmware_load()
 * 
 * Load the GSI firmware.
 *
 * Performs sanity checks on the passed firmware buffer and load it into
 * GSI IRAM.
 *
 * @param
 *  fwBuf - void pointer to the GSI firmware segment base address
 *  size  - size in bytes of the firmware buffer
 *
 * @return
 *  success or failure return status
 */
GPI_RETURN_STATUS gpi_firmware_load
  (
    const void * fwBuf,
    uint32 fwBufSize
  )
{
   return GPI_STATUS_ERROR;
}