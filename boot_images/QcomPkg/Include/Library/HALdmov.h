#ifndef HALDMOV_H
#define HALDMOV_H

/*
===============================================================================

                                D A T A   M O V E R 
              H A R D W A R E   A B S T R A C T I O N   L A Y E R

 FILE:         HALdmov.h

 DESCRIPTION:  
   This is the hardware abstraction layer interface for DMOV (data mover block)

===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/07/10   mk      Added ADM_1
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

/* ----------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------- */

#include "HALcomdef.h"


#include "BSPdmov.h"

/*-----------------------------------------------------------------------------
** Constants
**---------------------------------------------------------------------------*/

/* Matches BSP */
#define HAL_DMOV_DEV_ADM               0
#define HAL_DMOV_DEV_MDM               1
#define HAL_DMOV_DEV_ADM_1             2

#define HAL_DMOV_SINGLE_CMD_SIZE       4
#define HAL_DMOV_SG_CMD_SIZE           4
#define HAL_DMOV_BOX_CMD_SIZE          6
#define HAL_DMOV_MPU_CMD_SIZE          2


#define HAL_DMOV_MAX_NUM_DEVICES  BSP_DMOV_MAX_NUM_DEVICES

/* FLUSH_STATE register values reported when transfers are dequeued by 
 * the DMOV device driver before issuing to the Data Mover device */
#define HAL_DMOV_FLUSH_STATE_INVALID   0xdeeeeeef	

/*-----------------------------------------------------------------------------
** Types
**---------------------------------------------------------------------------*/

typedef enum
{
  HAL_DMOV_CHAN_MODE_INTERRUPT = 0,
  HAL_DMOV_CHAN_MODE_POLLING = 1,

  HAL_DMOV_CHAN_MODE_FORCE_32_BIT = 0x7FFFFFFF  /* Force enum to be 32-bit */
} HAL_dmov_ChanOpMode;

/* Channel result */
typedef enum
{
  HAL_DMOV_FLUSH_RESULT,
  HAL_DMOV_ERROR_RESULT,
  HAL_DMOV_TPD_RESULT,
  HAL_DMOV_INVALID_RESULT,

  HAL_DMOV_CHAN_RESULT_FORCE_32_BIT = 0x7FFFFFFF  /* Force enum to be 32-bit */
} HAL_dmov_ResultInfo;

/* Channel error */
typedef enum
{
  HAL_DMOV_NO_ERROR,
  HAL_DMOV_CPH_BUS_ERROR = 8,
  HAL_DMOV_DPH_BUS_ERROR = 9,
  HAL_DMOV_MPU_ERROR = 10,
  HAL_DMOV_LEN_ERROR = 11,
  HAL_DMOV_CMD_ERROR = 20,

  HAL_DMOV_NUM_CHAN_ERRORS,

  HAL_DMOV_ERROR_FORCE_32_BIT = 0x7FFFFFFF  /* Force enum to be 32-bit */
} HAL_dmov_ErrorType;

typedef enum
{
  HAL_DMOV_CMD_OPTION_LC,
  HAL_DMOV_CMD_OPTION_SRC_CRCI_CTL,
  HAL_DMOV_CMD_OPTION_DST_CRCI_CTL,
  HAL_DMOV_CMD_OPTION_SRC_END_CTL,
  HAL_DMOV_CMD_OPTION_DST_END_CTL,
  HAL_DMOV_CMD_OPTION_ADDR_MODE,
  HAL_DMOV_CMD_OPTION_BLOCK_THIS_CHAN,
  HAL_DMOV_CMD_OPTION_BLOCK_OTHER_CHAN,
  HAL_DMOV_CMD_OPTION_UNBLOCK_OTHER_CHAN,

  HAL_DMOV_CMD_OPTION_32_BIT = 0x7FFFFFFF  /* Force enum to be 32-bit */
} HAL_dmov_CmdOptions;

/* Security level */
typedef enum
{
  HAL_DMOV_SECURE_BOOT = 0,  /* ARM9 Boot level security  */
  HAL_DMOV_NON_SECURE_BOOT,  /* ARM11 Boot level security */
  HAL_DMOV_SECURE_AMSS,           /* ARM9 AMSS level security  */
  HAL_DMOV_NON_SECURE_AMSS,       /* ARM11 AMSS level security */

  HAL_DMOV_NON_SECURE_FORCE_32_BIT = 0x7FFFFFFF  /* Force enum to be 32-bit */
} HAL_dmov_SecurityLevel;

/* ----------------------------------------------------------------------------
** Interface 
** ------------------------------------------------------------------------- */

void HAL_dmov_Init                           ( uint32 nDevice, char ** ppszVersion );
void HAL_dmov_Reset                          ( uint32 nDevice ); 
void HAL_dmov_Save                           ( uint32 nDevice );
void HAL_dmov_Restore                        ( uint32 nDevice );

boolean HAL_dmov_DeviceExists                ( uint32 nDevice );
HAL_dmov_SecurityLevel HAL_dmov_GetSecurityLevel ( uint32 nDevice );
uint32 HAL_dmov_GetChanConfig                ( uint32 nDevice, uint32 nChan );
void HAL_dmov_SetChanConfig                  ( uint32 nDevice, uint32 nChan, uint32 nRegValue );
void HAL_dmov_DisableChan                    ( uint32 nDevice, uint32 nChan );
void HAL_dmov_EnableChan                     ( uint32 nDevice, uint32 nChan );
uint32 HAL_dmov_GetNumChannels               ( uint32 nDevice );
uint32 HAL_dmov_GetNumSecDomains             ( uint32 nDevice );
uint32 HAL_dmov_GetNumFlushRegisters         ( uint32 nDevice );
HAL_dmov_ChanOpMode HAL_dmov_GetChanMode     ( uint32 nDevice, uint32 nChan );
void HAL_dmov_SetChanMode                    ( uint32 nDevice, uint32 nChan, HAL_dmov_ChanOpMode eOpMode );
void HAL_dmov_SetChanForceFlushResult        ( uint32 nDevice, uint32 nChan, boolean bForceFlushRslt );
uint32 HAL_dmov_GetChanSecDomain             ( uint32 nDevice, uint32 nChan);
boolean HAL_dmov_SecDomainHasAccess          ( uint32 nDevice, uint32 nDomain );
boolean HAL_dmov_ChanIsAccessible            ( uint32 nDevice, uint32 nChan );
void HAL_dmov_SetCmdWordOptions              ( uint32 * pnCmd, HAL_dmov_CmdOptions eOption, uint32 nValue );
void HAL_dmov_PrepareSGDescList              ( uint32 nUserDescListAddr, uint32 nNumDesc );
void HAL_dmov_PrepareScatterGatherModeCmd    ( uint32 * pnSGCmdWord );
void HAL_dmov_BuildCmdListPtr                ( uint32 * pnCmdList, uint32 * pnCmd, uint32 nCmdType, boolean  bLastPtr );
void HAL_dmov_BuildCmd                       ( uint32 * dwCmd, uint32 * pnCommandListPtr );
boolean HAL_dmov_ChanCmdQIsReady             ( uint32 nDevice, uint32 nChan );
void HAL_dmov_IssueCmd                       ( uint32 nDevice, uint32 nChan, uint32 nCmd );
boolean HAL_dmov_ChanResultQIsEmpty          ( uint32 nDevice, uint32 nChan );
uint32 HAL_dmov_GetChanResult                ( uint32 nDevice, uint32 nChan );
HAL_dmov_ResultInfo HAL_dmov_CheckChanResult ( uint32 nResult );
HAL_dmov_ErrorType HAL_dmov_GetChanError     ( uint32 nDevice, uint32 nChan );
uint32 HAL_dmov_GetDomainPendingInterrupts   ( uint32 nDevice, uint32 nDomain );
void HAL_dmov_FlushChan                      ( uint32 nDevice, uint32 nChan, boolean bDiscard );
boolean HAL_dmov_ChanFlushIsComplete         ( uint32 nDevice, uint32 nChan );
uint32 HAL_dmov_GetChanFlushState            ( uint32 nDevice, uint32 nChan, uint32 nFlushState );
boolean HAL_dmov_FlushChanStateIsIdle        ( uint32 nDevice, uint32 nChan );
void HAL_dmov_SetChanSecDomain               ( uint32 nDevice, uint32 nChan, uint32 nSecDomainVal );

/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

===============================================================================

HAL_dmov_DeviceExists

Description:
  Verifies whether a data mover device exists on the current processor.

Parameters:
  nDevice - Data mover device instance

Return:
  TRUE if the device exists
  FALSE otherwise

Dependencies:
  None

===============================================================================

HAL_dmov_GetSecurityLevel

Description:
  Returns the security level under which the data mover device is running.

Parameters:
  nDevice - Data mover device instance

Dependencies:
  None

Return:
  HAL_DMOV_SECURE_SUPERVISOR, boot level security
  HAL_DMOV_SECURE_USER, secure ARM9
  HAL_DMOV_NON_SECURE, non-secure ARM11
 
===============================================================================
 
HAL_dmov_Reset

Description:
  Resets the data mover device hardware.

Parameters:
  nDevice - Data mover device instance

Dependencies:
  None

Return:
  None

=============================================================================== 

HAL_dmov_Save

Description:
  Saves the current data mover device configuration.

Parameters:
  nDevice - Data mover device instance

Dependencies:
  None

Return:
  None
 
===============================================================================

HAL_dmov_Restore

Description:
  Restores the last data mover device configuration.

Parameters:
  nDevice - Data mover device instance

Dependencies:
  None

Return:
  None
 
===============================================================================
 
HAL_dmov_GetChanConfig

Description:
  Provides the current configuration of the specified channel.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  Valid only if the data mover device is running on a secure processor.

Return:
  Configuration value of the channel
 
===============================================================================
 
HAL_dmov_SetChanConfig

Description:
  Configures the specified channel.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number
  nRegValue - The configuration value

Dependencies:
  Valid only if the data mover device is running on a secure processor.

Return:
  None
 
===============================================================================
 
HAL_dmov_DisableChan

Description:
  Disables the channel hardware by preventing the command pointer FIFO from
  accepting any new command pointers from hosts. The writes to the command 
  pointer register will be ignored.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  Valid only if the data mover device is running on a secure processor.

Return:
  None
 
===============================================================================
 
HAL_dmov_EnableChan

Description:
  Enables the Data Mover channel by clearing the command pointer FIFO halt
  bit

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  Valid only if the data mover device is running on a secure processor.
  Channel must first be initialized with a call to dmov_chan_config().
  All channel errors must be cleared.

Return:
  None
 
===============================================================================
 
HAL_dmov_GetNumChannels

Description:
  Provides the number of channels available on a specific data mover device.

Parameters:
  nDevice - Data mover device instance

Dependencies:
  None

Return:
  Number of channels present on the dmov device
 
===============================================================================
 
HAL_dmov_GetNumSecDomains

Description:
  Provides the number of security domains controlling the data mover device.

Parameters:
  nDevice - Data mover device instance

Dependencies:
  None

Return:
  Number of security domains controlling the dmov device
 
===============================================================================
 
HAL_dmov_GetNumFlushRegisters

Description:
  Provides the number of Hn_FLUSH_STATE*_SDc registers.

Parameters:
  nDevice - Data mover device instance

Dependencies:
  None

Return:
  The number of flush state registers in the device
 
===============================================================================
 
HAL_dmov_GetChanMode

Description:
  Checks whether the channel mode of operation is IRQ or polling.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  None

Return:
  HAL_DMOV_CHAN_MODE_INTERRUPT, if interrupt mode
  HAL_DMOV_CHAN_MODE_POLLING, if polling mode
 
===============================================================================
 
HAL_dmov_SetChanMode

Description:
  Configures the channel for polling or interrupt mode.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number
  eOpMode - Specifies the mode of operation

Dependencies:
  Driver should check for any pending transfers before calling this function

Return:
  None
 
===============================================================================
 
HAL_dmov_SetChanForceFlushResult

Description:
  Configures the channel force flush result parameter. When set (1) it will
  generate a result for each top-level pointer that is flushed due to a 
  host initiated flush.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number
  bForceFlushRslt - TRUE sets the force flush result field of the channel
                    configuration register to 1, sets it to 0 if FALSE

Dependencies:
  None

Return:
  None
 
===============================================================================
 
HAL_dmov_GetChanSecDomain

Description:
  Provides the security domain controlling a specific channel.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  None

Return:
  The security domain controlling this channel
 
===============================================================================
 
HAL_dmov_SecDomainHasAccess

Description:
  Checks whether a data mover device is controlled by a specific domain.

Parameters:
  nDevice - Data mover device instance
  nDomain - The dmov security domain

Dependencies:
  None

Return:
  TRUE if the security domain has access to the dmov device channels.
  FALSE otherwise
 
===============================================================================
 
HAL_dmov_ChanIsAccessible

Description:
  Checks whether the channel is controlled by a security domain that has 
  access to the data mover device.                                                                       

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  None

Return:
  TRUE, if the channel is controlled locally
  FALSE otherwise
 
===============================================================================
 
HAL_dmov_SetCmdWordOptions

Description:
  Helper function to set the options of a dmov transfer command.

Parameters:
  pnCmd - Pointer to the command
  eOption - The option to set
  nValue - Value of the option

Dependencies:
  None

Return:
  None
 
===============================================================================
 
HAL_dmov_PrepareSGDescList

Description:
  Modifies the user's list to make it compatible to the DMOV specification.

Parameters:
  pnUserDescListEntry - pointer to the user S/G segments list
  nNumDesc - number of descriptors in the list

Dependencies:
  None

Return:
  None
 
===============================================================================
 
HAL_dmov_PrepareScatterGatherModeCmd

Description:
  Builds a scatter/gather mode dmov command based on input from the client.

Parameters:
  pnSGCmdWord - pointer to the first S/G cmd word
  nSrcDescListAddr - address of the source descriptor list
  nDstDescListAddr - address of the destination descriptor list

Dependencies:
  None

Return:
  Number of words (uint32) the user should increment while building the 
  cmd list.
 
===============================================================================
 
HAL_dmov_BuildCmdListPtr

Description:
  Prepares the command list pointer to a list of dmov commands

Parameters:
  pnCmdList - pointer to the command list pointer buffer
  pnCmd - pointer to the list of data mover commands
  nCmdType - the type of programming structure pointed to by this entry
           0: Command list
           1: Reserved_programming
           2: Inbound MPU configuration
           3: Outbound MPU configuration
  boolean - TRUE if this is the last pointer in the list, FALSE otherwise
Dependencies:
  None

Return:
  None
 
===============================================================================
 
HAL_dmov_BuildCmd

Description:
  Builds the DMOV command to be issued to CHn_CMD_PTR_SDc

Parameters:
  pnCmd - Address of the Command
  pnCommandListPtr - Address of the Command List Pointer

Dependencies:
  None

Return:
  None
 
===============================================================================
 
HAL_dmov_ChanCmdQIsReady

Description:
  Checks whether a command pointer may be enqueued to the command pointer 
  FIFO.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  None

Return:
  TRUE if the channel can accept a new command pointer
  FALSE otherwise                                                                       
 
===============================================================================
 
HAL_dmov_IssueCmd

Description:
  Issues a data mover command.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number
  nCmd - Command to issue

Dependencies:
  The user should make sure the channel is in a running state.

Return:
  None
 
===============================================================================
 
HAL_dmov_ChanResultQIsEmpty

Description:
  Checks the channel status and determines whether at least one valid 
  result may be dequeued from the result FIFO

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  None

Return:
  TRUE if no result is ready to be dequeued
  FALSE otherwise                                                                       
 
===============================================================================
 
HAL_dmov_GetChanResult

Description:
  Provides the user with the result produced on a specific channel.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  Reading this register pops one item from the result FIFO.

Return:
  Value of the channel result
 
===============================================================================
 
HAL_dmov_CheckChanResult

Description:
  Checks the result value and returns info on whether it was caused by an 
  error or successful completion.

Parameters:
  nResult - Channel result value

Dependencies:
  None

Return:
  HAL_dmov_ResultInfo, Information about the cause of the result
 
===============================================================================
 
HAL_dmov_GetChanError

Description:
  Provides the type of channel error that occured.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  None

Return:
  HAL_dmov_ErrorType, The error type
 
===============================================================================
 
HAL_dmov_GetDomainPendingInterrupts

Description:
  Provides the interrupt status of the channels belonging to the N’th 
  security domain.
  IRQ bits for channels not controlled by this domain are always cleared.

Parameters:
  nDevice - Data mover device instance
  nDomain - dmov security domain

Dependencies:
  None

Return:
  The IRQ status value. Bit location corresponds to the channel number.
 
===============================================================================
 
HAL_dmov_FlushChan

Description:
  Issue a flush command to the channel to interrupt the command executing.
  if a flush with discard is requested then any data already read into the
  data buffer will be discarded.
  If a graceful flush is requested then all inbound data received or 
  requested prior to terminating the top-level command will be written.

Parameters:
  nDevice - The dmov device instance (DEV0 or DEV1 for ADM/MDM)
  nChan - Channel number
  bDiscard - if TRUE then it specifies a flush with discard, otherwise it's
             a graceful flush

Dependencies:
  None

Return:
  None
 
===============================================================================
 
HAL_dmov_ChanFlushIsComplete

Description:
  Indicates that the flush operation is complete and the flush state is now
  valid.

Parameters:
  nDevice - The dmov device instance (DEV0 or DEV1 for ADM/MDM)
  nChan - Channel number

Dependencies:
  None

Return:
  TRUE if channel flush operation is complete
  FALSE otherwise
 
===============================================================================
 
HAL_dmov_GetChanFlushState

Description:
  Gets the state of the channel at the time of the flush.
  Useful in providing info about the state of the channel, the error that
  caused the flush...

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number
  nFlushState - Flush state to read

Dependencies:
  None

Return:
  Value of the flush state
 
===============================================================================
 
HAL_dmov_FlushChanStateIsIdle

Description:
  Checks the channel fulsh state and specifies whether the channel was idle
  at the time the flush was requested.

Parameters:
  nDevice - Data mover device instance
  nChan - Channel number

Dependencies:
  None

Return:
  TRUE if the channel was idle
  FALSE otherwise                                                                       
 
===============================================================================

  HAL_dmov_SetChanSecDomain                                                  
                                                                           
  Description: 
    Configures the security domain register.
                                                                           
  Parameter:                                                               
  nDevice - Data mover device instance
  nChan - Channel number
    nSecDomainVal - the register value to be written
                                                                            
  Return Value:                                                            
  None
                                                                           
  Dependencies:                                                            
  None
                                                                            
===============================================================================
 
HAL_dmov_Init

Description:
  This function initializes the specific data mover device Hardware. 

Parameters:
   **ppszVersion - Ptr to the ptr to point the HAL DMOV Version String 
   nDevice  - Unique index of the data mover device to be initialized 

Dependencies:
  There should be no pending DMOV transfers at the time this function is 
  called.

Return:
  None
 
========================================================================= 
*/

#endif /* #ifndef HALDMOV_H */


