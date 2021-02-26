#ifndef __SDCC_HC_H
#define __SDCC_HC_H

/**********************************************************************
 * sdcc_hc.h
 *
 * Header file for SD Host Controller Standard interface
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History


when        who     what, where, why
----------  ---     --------------------------------------------------- 
2017-05-12  sb      Add partial enumeration for eMMC 
2017-01-17  jt      Update sdcc_hc_set_clock() definition
2015-03-30  bn      Corrected the descriptor size for 64-bit mode      
2015-03-26  bn      Moved pDescTableAddr allocation to sdcc_hc.c file
2015-01-26  rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-07-09  bn      Initial release

======================================================================*/

#include "sdcc_priv.h"
#include "SdccCommon.h"

/* Max wait time for status polling. In unit of usec */
#define SDHCI_STATUS_POLL_MAX      5000000
/* Max wait time for CMD READY polling. In unit of usec */
#define SDHCI_CMD_POLL_MAX         10000

/* Each ADMA descriptor is 8 bytes (32-bit mode) or 12 bytes (64-bit mode) */
#define ADMA_DESC_SIZE 12
#define ADMA_MAX_DESCRIPTOR_COUNT 128


#define ADMA_FLAGS_DESC_VALID      0x00000001
#define ADMA_FLAGS_DESC_END        0x00000002
#define ADMA_FLAGS_DESC_INT        0x00000004
#define ADMA_FLAGS_OPERATION_NOP   0x00000000
#define ADMA_FLAGS_OPERATION_TRAN  0x00000020
#define ADMA_FLAGS_OPERATION_LINK  0x00000030
#define MAX_DESCRIPTOR_SIZE        (64*1024)

/******************************************************************************
* Name: sdcc_init_sdhci_mode
*
* Description:
*    This function initializes the SDCC controller to using the Host Controller 
*    interface for communication with the memory devices.
*    It first turns on the clock, configures the GPIOs and then turns on the 
*    power supply to the device.  It also initializes the related software data
*    structures.
*
* Arguments:
*    driveno   [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_init_sdhci_mode(UINT32 driveno);

/******************************************************************************
* Name: sdcc_partial_init_sdhci_mode
*
* Description:
*    This function initializes the SDCC controller to using the Host Controller 
*    interface for communication with the memory devices.
*    It first turns on the clock, configures the GPIOs and then turns on the 
*    power supply to the device.  It also initializes the related software data
*    structures.
*
* Arguments:
*    sdcc_pdev       [IN/OUT]: pointer to device structure
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_partial_init_sdhci_mode(sdcc_dev_type *sdcc_pdev);

/******************************************************************************
* Name: sdcc_hc_set_power_mode
*
* Description:
*    This function sets the voltage level supported in the Capabilities registers. 
*    It then turns the bus power on/off as indicated in the bOn passed in parameter

* Arguments:
*    driveno     [IN]: drive number
*    bOn         [IN]: TRUE to turn bus pwer ON. OFF otherwise
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_set_power_mode(UINT32 driveno, BOOLEAN bOn);

/******************************************************************************
 * Name: sdcc_hc_set_clock
 *
 * Description:
 *    This function sets the SCLK frequency that goes to the memory device
 *
 * Arguments:
 *    driveno     [IN]: drive number
 *    nDivider    [IN]: divider to use
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_hc_set_clock (UINT32 driveno, UINT32 nDivider);

/******************************************************************************
* Name: sdcc_hc_config_buswidth
*
* Description:
*    This function configures the MMC bus width in Host Controller interface mode.
*
* Arguments:
*    driveno          [IN] : drive number
*    bus_width        [IN] : MMC bus width
*
* Returns:
*    Error Code
*
******************************************************************************/
SDCC_STATUS sdcc_hc_config_buswidth (UINT32 driveno, SDCC_MMC_BUSWIDTH buswidth);

/******************************************************************************
* Name: sdcc_hc_fifo_mode_rw
*
* Description:
*    This function is to read/write data from the memory device using
*    non-dma in Host Controller interface mode.
*
* Arguments:
*    p_device               [IN]: pointer to device structure
*    p_command              [IN]: pointer to command structure
*    buffer                 [IN/OUT]: pointer of the data buffer to write to or read from
*    transfer_size_in_byte  [IN]: transfer size in unit of bytes
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_fifo_mode_rw (sdcc_dev_type *p_device,
                                         sdcc_cmd_type *p_command,
                                         UINT8 *buffer,
                                         UINT32 transfer_size_in_byte);

/******************************************************************************
* Name: sdcc_hc_set_transfer_mode
*
* Description:
*    This function sets up the data transfer. The Host Driver must set the 
*    Transfer Mode Register before issuing a command which transfers data. 
*    Writes to this reigster are ignored when the DAT Command Inhibit bit 
*    in the Present State register is set.
*
* Arguments:
*    p_device       [IN]: pointer to device structure
*    p_command      [IN]: pointer to command structure
*    block_count    [IN]: transfer size in unit of 512-byte blocks
*
* Returns:
*    Error Code
*
******************************************************************************/
static void sdcc_hc_set_transfer_mode (sdcc_dev_type *p_device, 
                                       sdcc_cmd_type *p_command, 
                                       UINT16 block_count);

/******************************************************************************
* Name: sdcc_hc_prepare_dma_descriptors
*
* Description:
*    This function prepares the 32-bit ADMA2 descriptor per the HC specs.
*
* Arguments:
*    driveno              [IN]: drive number
*    buffer               [IN]: pointer to the data buffer used in the DMA transfer
*    nTransferSizeInByte  [IN]: size of the transfer in unit of byte
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS
sdcc_hc_prepare_dma_descriptors (UINT32 driveno, UINT8 *buffer,
                                 UINT32 nTransferSizeInByte);

/******************************************************************************
 * Name: sdcc_do_transfer_sdhci_mode
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the SD-card using the Host Controller standard interface.
 *
 * Arguments: 
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform 
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_do_transfer_sdhci_mode (sdcc_dev_type *p_device, sdcc_cmd_type *p_command,
                                         UINT8 *buffer, UINT16 transfer_size);

/******************************************************************************
* Name: sdcc_hc_wait_cmd_ready
*
* Description:
*    This function checks the CMD and DATA Inhibit states. 
*    It returns error if the Inhibit conditions continue after the 
*    timeout period.
*
* Arguments:
*    driveno        [IN]: drive number
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS sdcc_hc_wait_cmd_ready (UINT32 driveno);

/******************************************************************************
* Name: sdcc_hc_set_uhs_mode_select
*
* Description:
*    This function sets the UHS-1 mode value.
*
* Arguments:
*    p_device   [IN]: SDCC context data 
*    mode       [IN]: UHS-1 mode 
*
* Returns:
*    Error Code
*
******************************************************************************/
SDCC_STATUS 
sdcc_hc_set_uhs_mode_select (sdcc_dev_type *p_device, SDHCI_UHS_MODE mode);

/******************************************************************************
* Name: sdcc_hc_wait_interrupt_status
*
* Description:
*    This function reads the Normal and Error Interrupt Status registers and 
*    waits until the specified Status bits are set. It returns error if the 
*    expected bits are not set within a timeout period
*
* Arguments:
*    driveno        [IN]: drive number
*    mask           [IN]: Status bits that we are waiting for
*    pnIntStatus    [OUT]: returned copy from reading the Status Regs
*
* Returns:
*    Error Code
*
******************************************************************************/
static SDCC_STATUS
sdcc_hc_wait_interrupt_status(UINT32 driveno, 
	                      UINT32 mask, UINT32 *pnIntStatus);

/******************************************************************************
* Name: sdcc_command_sdhci_mode
*
* Description:
*    This function is to send the command and poll the status using the standard 
*    Host Controller interface to communicate with the memory devices
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device structure
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_command_sdhci_mode (sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd );

/******************************************************************************
* Name: sdcc_sdhci_print_error
*
* Description:
*    This function prints the error from the Error Interrupt Status register
*
* Arguments:
*    driveno        [IN]: drive number
*
* Returns: None
*
******************************************************************************/
static void sdcc_sdhci_print_error (UINT32 driveno);

#endif /* __SDCC_HC_H */

