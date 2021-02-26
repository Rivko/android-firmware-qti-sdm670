/** 
  @file  EFIDmov.h
  @brief Data Mover (DM) Protocol for UEFI.
         DMOV is also used for the term Data Mover.
*/
/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/26/12   vishalo Merge in Techpubs Doxygen change
 11/02/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/25/11   rl      Created

=============================================================================*/
#ifndef __EFI_DMOV_H__
#define __EFI_DMOV_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_dmov_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_DMOV_PROTOCOL_VERSION 0x0000000000010000
/** @} */ /* end_addtogroup efi_dmov_constants */

/*  Protocol GUID definition */
/** @ingroup efi_dmov_protocol */
#define EFI_DMOV_PROTOCOL_GUID \
    { 0x43FC9E5F, 0xA4AD, 0x4F90, { 0xB2, 0xEE, 0xA3, 0x34, 0x26, 0x88, 0xD9, 0x87 } }

/** @addtogroup efi_dmov_constants 
@{ */

/* DMOV channel operation mode */
/** DM channel Interrupt mode. */
#define EFI_DMOV_CHAN_MODE_INTERRUPT            0
/** DM channel Polling mode. */
#define EFI_DMOV_CHAN_MODE_POLLING              1

/* DMOV command address mode */
/** DM command address Single mode. */
#define EFI_DMOV_CMD_ADDR_MODE_SINGLE           0
/** DM command address Scatter/Gather mode. */
#define EFI_DMOV_CMD_ADDR_MODE_SCATTER_GATHER   1
/** DM command address Box mode. */
#define EFI_DMOV_CMD_ADDR_MODE_BOX              3

/* DMOV command endianness control */
/** DM command endianness control: swap bytes. */
#define EFI_DMOV_CMD_END_CTL_SWAP_BYTES         1
/** DM command endianness control: swap half words. */
#define EFI_DMOV_CMD_END_CTL_SWAP_HALF_WORDS    2
/** DM command endianness control: swap words. */
#define EFI_DMOV_CMD_END_CTL_SWAP_WORDS         4

/** Maximum number of Flush state registers. */
#define EFI_DMOV_MAX_NUM_FLUSH_STATE_REGS       6
/** @} */ /* end_addtogroup efi_dmov_constants */

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/** @cond */
/**
  External reference to the DMOV Protocol GUID.
*/
extern EFI_GUID gEfiDmovProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/** @cond */
/** Protocol declaration. */
typedef struct _EFI_DMOV_PROTOCOL EFI_DMOV_PROTOCOL;
/** @endcond */

/** @addtogroup efi_dmov_data_types 
@{ */
/** Options for a transfer command, such as destination CRCI, endian control, 
  etc. */
typedef UINT32 EFI_DMOV_CMD_FIRST_WORD;

/** @cond */
/** Type definition for the #_EFI_DMOV_CMD_TYPE enumeration. */
typedef enum _EFI_DMOV_CMD_TYPE EFI_DMOV_CMD_TYPE;
/** @endcond */
/** 
  Command specification; DM command type. 
*/
enum _EFI_DMOV_CMD_TYPE
{
  EFI_DMOV_CMD_LIST = 0,     /**< Command list. */
  EFI_DMOV_CMD_PTR_LIST = 1, /**< Command pointer list. */
  EFI_DMOV_MPU_IN = 2,       /**< Memory protection unit input. */
  EFI_DMOV_MPU_OUT = 3,      /**< Memory protection unit output. */

  /** @cond */
  EFI_DMOV_CMD_SPEC_FORCE_32_BIT = 0x7FFFFFFF
  /** @endcond */
};

/** @cond */
/** Type definition for the #_EFI_DMOV_CMD_OPTIONS structure. */
typedef struct _EFI_DMOV_CMD_OPTIONS EFI_DMOV_CMD_OPTIONS;
/** @endcond */
/** 
  DM command options. 
*/
struct _EFI_DMOV_CMD_OPTIONS
{
  UINT32 dwAddrMode;           /**< Address mode. */
  UINT32 dwSrcCRCI;            /**< Client rate control interface source. */
  UINT32 dwDstCRCI;            /**< Client rate control interface destination. */
  UINT32 dwSrcEndianness;      /**< Source endianness. */
  UINT32 dwDstEndianness;      /**< Destination endianness. */
  UINT32 bBlockThisChannel;    /**< Block this channel. */
  UINT32 bBlockOtherChannel;   /**< Block the other channel. */
  UINT32 bUnblockOtherChannel; /**< Unblock the other channel. */
  UINT32 bLastCommand;         /**< Last command. */
};

/** @cond */
/** Type definition for the #_EFI_DMOV_SINGLE_CMD structure. */
typedef struct _EFI_DMOV_SINGLE_CMD EFI_DMOV_SINGLE_CMD;
/** @endcond */
/** 
  DM single command. For the length, bits 0 to 15 specify the length of 
  the transfer. The maximum transfer size is 65535 bytes.
*/
struct _EFI_DMOV_SINGLE_CMD
{
  EFI_DMOV_CMD_FIRST_WORD CmdOptions; /**< Command options; see 
                                           #EFI_DMOV_CMD_FIRST_WORD for 
                                           details. */
  UINT32           dwSrcAddr;         /**< Source address. */
  UINT32           dwDstAddr;         /**< Destination address. */
  UINT32           dwLength;          /**< Data transfer length. @newpagetable */
};
/** @} */ /* end_addtogroup efi_dmov_data_types */
/** @addtogroup efi_dmov_constants 
@{ */
/** Number of single command words; used for the #_EFI_DMOV_SINGLE_CMD 
  structure. */
#define  EFI_DMOV_NUM_SINGLE_CMD_WORDS  4
/** @} */ /* end_addtogroup efi_dmov_constants */

/** @addtogroup efi_dmov_data_types 
@{ */
/** @cond */
/** Type definition for the #_EFI_DMOV_SG_CMD structure. */
typedef struct _EFI_DMOV_SG_CMD EFI_DMOV_SG_CMD; 
/** @endcond */
/** 
  DM scatter/gather command. Each descriptor list entry is represented 
  by two UINT32, one for the address of the segment and one for its length. 
  The maximum length to transfer is 65535 bytes per descriptor.
 */
struct _EFI_DMOV_SG_CMD 
{
  EFI_DMOV_CMD_FIRST_WORD CmdOptions;  /**< Command options; see 
                                            #EFI_DMOV_CMD_FIRST_WORD for 
                                            details. */
  UINT32            dwSrcDescListAddr; /**< Source descriptor list address. */
  UINT32            dwDstDescListAddr; /**< Destination descriptor list 
                                            address. */
  UINT32            dwReserved;        /**< Reserved. */
};
/** @} */ /* end_addtogroup efi_dmov_data_types */
/** @addtogroup efi_dmov_constants 
@{ */
/** Number of scatter/gather command words; used for the #_EFI_DMOV_SG_CMD 
  structure. */
#define EFI_DMOV_NUM_SG_CMD_WORDS  4
/** @} */ /* end_addtogroup efi_dmov_constants */

/** @addtogroup efi_dmov_data_types 
@{ */
/** @cond */
/** Type definition for the #_EFI_DMOV_BOX_CMD structure. */
typedef struct _EFI_DMOV_BOX_CMD EFI_DMOV_BOX_CMD;
/** @endcond */
/** 
  DM Box mode command. 
*/
struct _EFI_DMOV_BOX_CMD
{
  EFI_DMOV_CMD_FIRST_WORD CmdOptions;
  /**< Command options; see #EFI_DMOV_CMD_FIRST_WORD for details. */
  UINT32            dwFristSrcRowAddr;
  /**< Source row address. */
  UINT32            dwFristDstRowAddr;
  /**< Destination row address. */
  UINT32            dwRowLength;
  /**< Row length. 
       Bits 0 to 15 are for the destination; bits 16 to 31 are for the source. */
  UINT32            dwNumRows;
  /**< Number of rows. 
       Bits 0 to 15 are for the destination; bits 16 to 31 are for the source. */
  UINT32            dwRowOffset;
  /**< Row offset. 
       Bits 0 to 15 are for the destination; bits 16 to 31 are for the source. 
       @newpagetable */
};
/** @} */ /* end_addtogroup efi_dmov_data_types */
/** @addtogroup efi_dmov_constants 
@{ */
/** Number of box command words; used for the #_EFI_DMOV_BOX_CMD structure. */
#define EFI_DMOV_NUM_BOX_CMD_WORDS  6
/** @} */ /* end_addtogroup efi_dmov_constants */

/** @addtogroup efi_dmov_data_types 
@{ */
/** @cond */
/** Type definition for the #_EFI_DMOV_TRANSFER_REQUEST structure. */
typedef struct _EFI_DMOV_TRANSFER_REQUEST EFI_DMOV_TRANSFER_REQUEST;
/** @endcond */
/** 
  DM transfer request. These are options for the whole transfer.
*/
struct _EFI_DMOV_TRANSFER_REQUEST
{
  UINT32                 dwDevice;    /**< Device. */
  UINT32                 dwChan;      /**< Channel. */
  UINT32                 dwPriority;  /**< Priority. */
  UINT32                 pUserData;   /**< Data. */
  UINT32                 dwNumCmds;   /**< Number of commands. */
  EFI_DMOV_CMD_TYPE      CommandType; /**< Command type; see #_EFI_DMOV_CMD_TYPE 
                                           for details. */
  /** @cond */
  EFI_EVENT              hEvent;           /*<-- Notify event ( for future use ) */
  /** @endcond */
};

/** @cond */
/** Type definition for the #_EFI_DMOV_RESULT_STATE enumeration. */
typedef enum _EFI_DMOV_RESULT_STATE EFI_DMOV_RESULT_STATE;
/** @endcond */
/** 
  DM transfer result information. The information is part of the result 
  returned to the DM user.
*/
enum _EFI_DMOV_RESULT_STATE
{
  EFI_DMOV_TRANSFER_WAITING_TO_BE_ISSUED,
  /**< Transfer was queued in the DM and is waiting to be issued. */
  EFI_DMOV_TRANSFER_IN_PROGRESS,
  /**< Transfer was issued and is waiting for its completion. */
  EFI_DMOV_TRANSFER_SUCCESSFULL,
  /**< Transfer was successful. */
  EFI_DMOV_TRANSFER_ERROR,
  /**< Transfer resulted in an error. */
  EFI_DMOV_TRANSFER_FLUSH,
  /**< Transfer caused a flush. */
  EFI_DMOV_TRANSFER_NON_EXISTENT,
  /**< DM is not aware of this transfer. */

  /** @cond */
  EFI_DMOV_CHAN_STATE_FORCE_32_BIT = 0x7fffffff
  /** @endcond */
};

/** @cond */
/** Type definition for the #_EFI_DMOV_CHAN_ERROR enumeration.  */
typedef enum _EFI_DMOV_CHAN_ERROR EFI_DMOV_CHAN_ERROR;
/** @endcond */
/** 
  DM channel error.
*/
enum _EFI_DMOV_CHAN_ERROR
{
  EFI_DMOV_CHAN_NO_ERROR = 0,      /**< No error. */
  EFI_DMOV_CHAN_INVALID_STATE = 1, /**< Invalid state. */
  EFI_DMOV_CHAN_CPH_BUS_ERROR = 2, /**< Command phase bus error. */
  EFI_DMOV_CHAN_DPH_BUS_ERROR = 3, /**< Data phase bus error. */
  EFI_DMOV_CHAN_MPU_ERROR = 4,     /**< MPU error. */
  EFI_DMOV_CHAN_LEN_ERROR = 5,     /**< Length error. */
  EFI_DMOV_CHAN_CMD_ERROR = 6,     /**< Command error. @newpage */

  /** @cond */
  EFI_DMOV_CHAN_ERROR_FORCE_32_BIT = 0x7FFFFFFF
  /** @endcond */
};

/** @cond */
/** Type definition for the #_EFI_DMOV_RESULT structure. */
typedef struct _EFI_DMOV_RESULT  EFI_DMOV_RESULT;
/** @endcond */
/** 
  DM transfer result returned to the DM user.
*/
struct _EFI_DMOV_RESULT
{
  EFI_DMOV_RESULT_STATE eState;                /**< State of the transfer; see 
                                                    #_EFI_DMOV_RESULT_STATE for 
                                                    details. */
  UINT32                dwResultValue;         /**< Result value. */
  BOOLEAN               bMoreResultsAvailable; /**< Whether more results are 
                                                    available. */
  UINT32                FlushState[6];         /**< Flush state. */
  EFI_DMOV_CHAN_ERROR   eChanError;            /**< Channel error; see 
                                                    #_EFI_DMOV_CHAN_ERROR for 
                                                    details. */
  UINT32                pUserData;             /**< User data. */
};

/** @cond */
/** Type definition for the #_EFI_DMOV_REQUEST_STATE enumeration. */
typedef enum _EFI_DMOV_REQUEST_STATE EFI_DMOV_REQUEST_STATE;
/** @endcond */
/** 
  State of the DM transfer request issued by the user.
*/
enum _EFI_DMOV_REQUEST_STATE
{
  EFI_DMOV_REQUEST_INITIAL,
  /**< Initial request. */
  EFI_DMOV_REQUEST_OK,
  /**< Request is acceptable. */
  EFI_DMOV_REQUEST_CMD_LIST_NOT_ALIGNED,
  /**< Command list is not aligned. */
  EFI_DMOV_REQUEST_BOOT_SHOULD_BE_SYNCHRONOUS,
  /**< Boot must be synchronous. */
  EFI_DMOV_REQUEST_INVALID_DEVICE,
  /**< Device is invalid. */
  EFI_DMOV_REQUEST_INVALID_INTERRUPT_MODE,
  /**< Interrupt mode is invalid. */
  EFI_DMOV_REQUEST_INVALID_NUM_CMDS,
  /**< Number of commands is invalid. */
  EFI_DMOV_REQUEST_INVALID_DEVICE_STATE,
  /**< Device state is invalid. */
  EFI_DMOV_REQUEST_INVALID_CHAN_NUMBER,
  /**< Channel number is invalid. */
  EFI_DMOV_REQUEST_INVALID_CHAN_STATE,
  /**< Channel state is invalid. */
  EFI_DMOV_MAX_REQUESTS_ISSUED,
  /**< Maximum number of requests has been issued. */

  /** @cond */
  EFI_DMOV_REQUEST_STATE_FORCE_32_BIT = 0x7fffffff
  /** @endcond */
};
/** @} */ /* end_addtogroup efi_dmov_data_types */

/** @addtogroup efi_dmov_constants 
@{ */
/** Invalid token. */
#define EFI_DMOV_INVALID_TOKEN 0xffffffff
/** @} */ /* end_addtogroup efi_dmov_constants */

/** @addtogroup efi_dmov_data_types 
@{ */
/** @cond */
/** Type definition for the #_EFI_DMOV_REQ_STATUS structure. */
typedef struct _EFI_DMOV_REQ_STATUS EFI_DMOV_REQ_STATUS;
/** @endcond */
/** 
  DM transfer request issued by the user.
*/
struct _EFI_DMOV_REQ_STATUS
{
  EFI_DMOV_REQUEST_STATE eState;
  /**< Status of the transfer; see #_EFI_DMOV_REQUEST_STATE for details. */
  UINT32                 dwToken;
  /**< Token to be used for communications in reference to the 
    transfer. @newpagetable */
};


/** @cond */
/** Type definition for the #_EFI_DMOV_DEVICE_ID enumeration. */
typedef enum _EFI_DMOV_DEVICE_ID EFI_DMOV_DEVICE_ID;
/** @endcond */
/** 
  Device identification. For UEFI, the only device that is supported is 
  the application data mover.
*/
enum _EFI_DMOV_DEVICE_ID
{
/** @cond */
  EFI_DMOV_DEVICE_ADM0 = 0,
/** @endcond */
  EFI_DMOV_DEVICE_ADM1,     /**< Application data mover 1. */
/** @cond */
  EFI_DMOV_DEVICE_ID_MAX
/** @endcond */
};
/** @} */ /* end_addtogroup efi_dmov_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_DMOV_OPEN */
/** @ingroup efi_dmov_open
  @par Summary
  Opens the DM device for use.

  @param[in]     This             Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]     deviceID         DM instance to open; 
                                  see #_EFI_DMOV_DEVICE_ID for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.

  @sideeffects
  Application data mover (ADM) clocks are turned on if this is the first client 
  to open the DM driver.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_DMOV_OPEN)(
  IN EFI_DMOV_PROTOCOL *This,
  IN EFI_DMOV_DEVICE_ID deviceID
  );

/* EFI_DMOV_CLOSE */
/** @ingroup efi_dmov_close
  @par Summary
  Closes the DM device. Every Open request must have a matching a Close 
  request. This ensures that the DM clocks are turned off when clients 
  are not using the DM device.

  @param[in]     This             Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]     deviceID         DM instance to close; 
                                  see #_EFI_DMOV_DEVICE_ID for details.
  
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
  
  @sideeffects
  ADM clocks are turned off if this is the last client to close the DM driver.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_DMOV_CLOSE)(
  IN EFI_DMOV_PROTOCOL *This,
  IN EFI_DMOV_DEVICE_ID deviceID
  );

/* EFI_DMOV_TRANSFER */
/** @ingroup efi_dmov_transfer
  @par Summary
  Queues a DM transfer request.
  
  @param[in]  This             Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]  deviceID         DM instance to which to submit the transfer; 
                               see #_EFI_DMOV_DEVICE_ID for details.
  @param[in]  pRequest         Pointer to the #_EFI_DMOV_TRANSFER_REQUEST 
                               structure with the options to be used for this 
                               transfer.
  @param[in]  pCommandList     Pointer to the list of pointers or list of
  	                           DM commands to be executed for this
                               transfer. This pointer must be a physical
                               address.
  @param[out] pRequestStatus   Returns the status (see #_EFI_DMOV_REQ_STATUS)
                               as to whether this transfer was queued 
                               successfully. A token is also returned to be 
                               used in future communication with the DM to 
                               reference this transfer.
  
  @par Description
  The user must ensure that the correct options have been chosen to control 
  the transfer. Only synchronous transfers are permitted. The client 
  must allocate memory for the command structures that are supported. The 
  DM driver modifies the command list to make it ready for the hardware 
  by conforming to the DM command specifications. 
  @par  
  An error is returned if the DM driver finds any of the transfer options
  invalid or if the queuing did not complete successfully. If this occurs,
  the specific error is returned in the #_EFI_DMOV_REQ_STATUS structure.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.

  @dependencies
  The user must have successfully opened the DM device.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_DMOV_TRANSFER)(
  IN EFI_DMOV_PROTOCOL                *This,
  IN  EFI_DMOV_DEVICE_ID              deviceID,
  IN CONST EFI_DMOV_TRANSFER_REQUEST* pRequest,
  IN UINT32*                          pCommandList,
  OUT EFI_DMOV_REQ_STATUS*            pRequestStatus
  );

/* EFI_DMOV_GET_TRANSFER_RESULT */
/** @ingroup efi_dmov_get_transfer_result
  @par Summary
  Gets the results of a DM transfer.

  @param[in]     This             Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]     deviceID         DM instance from which to retrieve the result; 
                                  see #_EFI_DMOV_DEVICE_ID for details.
  @param[in]     dwToken          Token to reference the specific transfer for 
                                  which the result is being retrieved.
  @param[out]    pResult          Pointer to the transfer result structure; 
                                  see #_EFI_DMOV_RESULT for details.
  
  @par Description
  The DM driver checks the mode of the channel for which the client is
  seeking a transfer result. See #_EFI_DMOV_RESULT_STATE for information
  on results returned by this function.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
  
  @dependencies
  The user must have successfully opened the DM device.
  
  @sideeffects
  A call to this function removes a result from the queue. Subsequent
  calls with the same token return an error.
*/
typedef
EFI_STATUS
(EFIAPI* EFI_DMOV_GET_TRANSFER_RESULT)(
  IN EFI_DMOV_PROTOCOL  *This, 
  IN EFI_DMOV_DEVICE_ID deviceID,          
  IN UINT32             dwToken,
  OUT EFI_DMOV_RESULT   *pResult
  );

/* EFI_DMOV_SET_CHAN_OPERATION_MODE */
/** @ingroup efi_dmov_set_chan_op_mode
  @par Summary
  Sets the channel operating mode to Interrupt or Polling.

  @param[in]     This         Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]     deviceID     DM instance to which the channel belongs; 
                              see #_EFI_DMOV_DEVICE_ID for details.
  @param[in]     dwChan       Number of the channel to set.
  @param[in]     dwChanOpMode Operating mode to which to set the channel.

  @par Description
  The selected mode is in effect until this function is called the next
  time for the same channel, or until the device is reset. If the channel
  does not have any pending transfers, it reconfigures the channel's
  operating mode; otherwise, an error is returned. If the requested operating
  mode is not supported by the DM driver, an error is returned.
  
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.

  @dependencies
  The user must have successfully opened the DM device.

  @sideeffects
  The channel mode setting affects all clients using the specified channel.
*/
typedef
EFI_STATUS
(EFIAPI* EFI_DMOV_SET_CHAN_OPERATION_MODE)(
  IN EFI_DMOV_PROTOCOL  *This,
  IN EFI_DMOV_DEVICE_ID deviceID,
  IN UINT32             dwChan,
  IN UINT32             dwChanOpMode
  );

/* EFI_DMOV_FLUSH_CHANNEL */
/** @ingroup efi_dmov_flush_chan
  @par Summary
  Flushes a specified DM channel.

  @param[in] This               Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in] deviceID           DM instance to which the channel belongs; 
                                see #_EFI_DMOV_DEVICE_ID for details.
  @param[in] dwChan             Channel to be flushed.
  @param[in] bFlushQueues       Indicates whether to flush all transfers that
  	                            have not been written to the DM device (that
  								are in the queue).
  @param[in] bDiscardCurrentCmd Indicates whether to discard the data in the
  	                            DM channel write buffer.

  @par Description
  If bFlushQueues is TRUE, transfers that are pending in the DM driver
  software queues are dequeued, user callbacks are invoked with the
  result set to DMOV_TRANSFER_FLUSH, and EFI_DMOV_RESULT::FlushState[] values 
  are set to DMOV_FLUSH_STATE_INVALID. If bFlushQueues is FALSE, only the
  current top pointer is flushed.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.

  @dependencies
  The user must have successfully opened the DM device.
*/
typedef
EFI_STATUS
(EFIAPI* EFI_DMOV_FLUSH_CHANNEL)(
  IN EFI_DMOV_PROTOCOL  *This,
  IN EFI_DMOV_DEVICE_ID deviceID,
  IN UINT32             dwChan,
  IN UINT32             bFlushQueues,
  IN UINT32             bDiscardCurrentCmd
  );

/* EFI_DMOV_FLUSH_DOMAIN */
/** @ingroup efi_dmov_flush_domain
  @par Summary
  Flushes all channels in a specified domain.

  @param[in] This               Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in] deviceID           DM instance; see #_EFI_DMOV_DEVICE_ID for 
                                details.
  @param[in] dwDomain           Security domain of the channels to be flushed.

  @par Description
  This function flushes all channels associated with the specified security 
  domain. The domain must not be controlled by the local processor. The 
  function is intended to clear all channels of a host that is being reset.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.

  @dependencies
  The user must have successfully opened the DM device.
*/
typedef
EFI_STATUS
(EFIAPI* EFI_DMOV_FLUSH_DOMAIN)(
  IN EFI_DMOV_PROTOCOL  *This,
  IN EFI_DMOV_DEVICE_ID deviceID,
  IN UINT32             dwDomain 
  );

/* EFI_DMOV_PREPARE_SG_DESC_LIST */
/** @ingroup efi_dmov_prepare_sg_desc_list
  @par Summary
  Updates scatter/gather descriptor list entries according to
  the DM specification (setting the last descriptor bits).

  @param[in]  This               Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]  deviceID           DM instance; see #_EFI_DMOV_DEVICE_ID for 
                                 details.
  @param[in]  dwNumDescriptors   Number of descriptors in the list.
  @param[out] pDescList          Pointer to the user scatter/gather segment
  	                             list. This pointer must be a user space 
                                 pointer.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS
(EFIAPI *EFI_DMOV_PREPARE_SG_DESC_LIST)(
  IN EFI_DMOV_PROTOCOL   *This,
  IN  EFI_DMOV_DEVICE_ID deviceID,
  IN UINT32              dwNumDescriptors,
  OUT UINT32             *pDescList
  );

/* EFI_DMOV_PREPARE_COMMAND */
/** @ingroup efi_dmov_prepare_command
  @par Summary
  Sets the necessary command options in a given command.

  @param[in]  This            Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]  deviceID        DM instance; see #_EFI_DMOV_DEVICE_ID for 
                              details.
  @param[in]  pCommandOptions Pointer to the #_EFI_DMOV_CMD_OPTIONS structure 
                              that specifies the command options.
  @param[out] pCommand        Pointer to the command.

  @par Description
  Given a pointer to a DM command and the corresponding command options
  (through the pCommandOptions parameter), this function modifies the
  first word of the DM command such that the specified command options are
  included in the appropriate fields of the first command word.
  @par
  For a scatter/gather command, this function supports only one item in the
  index list. If a user needs to make a command with multiple items in the
  scatter/gather command index list, this function is not to be used.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS
(EFIAPI* EFI_DMOV_PREPARE_COMMAND)(
  IN EFI_DMOV_PROTOCOL     *This,
  IN  EFI_DMOV_DEVICE_ID   deviceID,
  IN EFI_DMOV_CMD_OPTIONS* pCommandOptions,
  OUT UINT32               * pCommand
  );

/* EFI_DMOV_PREPARE_COMMAND_LIST_PTR */
/** @ingroup efi_dmov_prepare_command_list_ptr
  @par Summary
  Sets the proper fields in a command list pointer entry (Last Pointer (LP),
  Pointer Type (PT), and ADDR) in a list of DM commands.

  @note1hang This function expects pCommandList to be a physical address. 

  @param[in]  This            Pointer to the EFI_DMOV_PROTOCOL instance.
  @param[in]  deviceID        DM instance; see #_EFI_DMOV_DEVICE_ID for 
                              details.
  @param[in]  pCommandList    Pointer to the command list (physical address).
  @param[in]  bLastPtr        Returns TRUE if the pointer passed is the 
                              last in the list of pointers; otherwise FALSE.
  @param[in]  pCommandListPtr Pointer to the command list pointer buffer.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
 typedef
 EFI_STATUS
 (EFIAPI* EFI_DMOV_PREPARE_COMMAND_LIST_PTR)(
  IN EFI_DMOV_PROTOCOL   *This,
  IN  EFI_DMOV_DEVICE_ID deviceID,
  IN UINT32              *pCommandList,
  IN UINT32              bLastPtr,
  IN UINT32              *pCommandListPtr
  );


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_dmov_protocol
  @par Summary
  Data Mover (DM) Protocol interface.

  @par Parameters
  @inputprotoparams{dmov_proto_params.tex} 
*/
struct _EFI_DMOV_PROTOCOL
{
  UINT64                             Revision;
  EFI_DMOV_OPEN                      Open;
  EFI_DMOV_CLOSE                     Close;
  EFI_DMOV_TRANSFER                  Transfer;
  EFI_DMOV_GET_TRANSFER_RESULT       GetTransferResult;
  EFI_DMOV_SET_CHAN_OPERATION_MODE   SetChanOperationMode;
  EFI_DMOV_FLUSH_CHANNEL             FlushChannel;
  EFI_DMOV_FLUSH_DOMAIN              FlushDomain;
  EFI_DMOV_PREPARE_SG_DESC_LIST      PrepareSGDescList;
  EFI_DMOV_PREPARE_COMMAND           PrepareCommand;
  EFI_DMOV_PREPARE_COMMAND_LIST_PTR  PrepareCommandListPtr;
};

#endif /* __EFI_DMOV_H__ */
