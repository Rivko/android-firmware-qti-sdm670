/** @file GpiLib.c
   
  Gpi Library code to define APIs to client

  Copyright (c) 2016-2017, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/11/17   ts      Included support to de register
 12/15/16   ts      Created

=============================================================================*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

/* I2C Interfaces */
#include <Protocol/EFIGpiProtocol.h>

static QCOM_GPI_PROTOCOL *gpi_if = NULL;

/**
 * Registers a client GPI interface.
 *
 * @param[in]    params                GPI interface registration parameters
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_reg(gpi_iface_params *params)
{
  EFI_STATUS Status = EFI_SUCCESS;

  /* Get the I2C protocol handle */
  Status = gBS->LocateProtocol(&gQcomGpiInitProtocolGuid, NULL, (VOID **) &gpi_if);
  if ((EFI_SUCCESS != Status) || (NULL == gpi_if))
  {
    DEBUG((EFI_D_ERROR, "Failed to Locate GPI protocol 0x%08x\n", Status));

    return GPI_STATUS_ERROR;
  }

  if (gpi_if == NULL)
  {
    DEBUG((EFI_D_ERROR, "GPI protocol handle is invalid\n"));
    return GPI_STATUS_GSI_NOT_READY;
  }

  return (gpi_if->iface_reg(params));
}

/**
 * De-Registers a client GPI interface.
 *
 * @param[in]    params                GPI interface handle
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_dereg(GPI_CLIENT_HANDLE handle)
{
  if (gpi_if == NULL)
  {
    DEBUG((EFI_D_ERROR, "GPI protocol handle is invalid\n"));
    return GPI_STATUS_GSI_NOT_READY;
  }

  return (gpi_if->iface_dereg(handle));
}

/**
 * Queries whether a GPI interface is running in interrupt or polling mode.
 *
 * @param[in]        handle            Handle to GPI Interface
 * @param[in, out]   irq_mode          1 if Interface in IRQ mode, 0 otherwise
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_in_irq_mode(GPI_CLIENT_HANDLE handle, UINT8 *irq_mode)
{
  return GPI_STATUS_SUCCESS;
}

/**
 * Enables/Disables a GPI interface IRQ mode.
 *
 * @param[in]    handle           Handle to the GPI Interface
 * @param[in]    irq_en           TRUE to enable IRQ mode, FALSE for polling
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_set_irq_mode(GPI_CLIENT_HANDLE handle, boolean irq_en)
{
  return GPI_STATUS_SUCCESS;
}

/**
 * Queries the channel current status.
 *
 * @param[in]         handle           Handle to the GPI Interface
 * @param[in]         chan             Virtual Channel ID
 * @param[in, out]    chan_status      Information about channel current status
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_query_chan_status(GPI_CLIENT_HANDLE handle,
                                        GPI_CHAN_TYPE chan,
                                        chan_status_info * chan_status)
{
  if (gpi_if == NULL)
  {
    DEBUG((EFI_D_ERROR, "GPI protocol handle is invalid\n"));
    return GPI_STATUS_GSI_NOT_READY;
  }

  return (gpi_if->query_chan_status(handle, chan, chan_status));
}

/**
 * Polls a GPI interface.
 * The corresponding callback will be invoked with any available results.
 *
 * @param[in]    handle               GPI client handle
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_iface_poll(GPI_CLIENT_HANDLE handle)
{
  if (gpi_if == NULL)
  {
    DEBUG((EFI_D_ERROR, "GPI protocol handle is invalid\n"));
    return GPI_STATUS_GSI_NOT_READY;
  }

  return (gpi_if->iface_poll(handle));
}

/**
 * Issues a channel command.
 *
 * @param[in]      handle              Handle to GPI Interface
 * @param[in]      chan                Virtual Channel ID
 * @param[in]      cmd_type            Command type
 * @param[in]      user_data           Client context data for this command
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_issue_cmd(GPI_CLIENT_HANDLE handle,
                                GPI_CHAN_TYPE chan,
                                GPI_CHAN_CMD cmd_type,
                                void *user_data)
{
  if (gpi_if == NULL)
  {
    DEBUG((EFI_D_ERROR, "GPI protocol handle is invalid\n"));
    return GPI_STATUS_GSI_NOT_READY;
  }

  return (gpi_if->issue_cmd(handle, chan, cmd_type, user_data));
}

/**
 * Process TREs request.
 *
 * @param[in]    tre_request           Request details
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_process_tre(gpi_tre_req *tre_request)
{
  if (gpi_if == NULL)
  {
    DEBUG((EFI_D_ERROR, "GPI protocol handle is invalid\n"));
    return GPI_STATUS_GSI_NOT_READY;
  }

  return (gpi_if->process_tre(tre_request));
}