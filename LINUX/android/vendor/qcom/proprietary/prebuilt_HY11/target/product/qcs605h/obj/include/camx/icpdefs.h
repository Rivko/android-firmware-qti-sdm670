//-------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  icpdefs.h
// @brief ICP command & message definitions.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
// 05/18/2016        vma           Removed properties/cmd/msg not currently
//                                 supported
// 08/04/2015        pahlin        Initial version
//------------------------------------------------------------------------

#ifndef _ICP_DEFS_H_
#define _ICP_DEFS_H_

#include "hfidefs.h"

/**
   Level 2 Comment: "System Level Error types"
   Common HFI_ERROR_SYS_X values to be used as system level error/warning
   for events and messages
   */
#define  HFI_ERR_ICP_INSUFFICIENT_RESOURCES             (HFI_ERR_ICP_START + 0x1)  /**< Insufficient system resources */
#define  HFI_ERR_ICP_UNSUPPORTED_RESOURCES              (HFI_ERR_ICP_START + 0x2)  /**< Unsupported resource*/
#define  HFI_ERR_ICP_UNSUPPORT_PROPERTY                 (HFI_ERR_ICP_START + 0x3)  /**< Unsupported property*/
#define  HFI_ERR_ICP_INIT_EXPECTED                      (HFI_ERR_ICP_START + 0x4)  /**< Upon FW start, first command must be SYS_INIT*/
#define  HFI_ERR_ICP_INIT_IGNORED                       (HFI_ERR_ICP_START + 0x5)  /**< After FW started, SYS_INIT will be ignored*/

/**
   Level 2 Comment: "Common System Level Properties"
   The section defines all the properties applicable for the
   system (i.e. will affect the behavior of all the domains
   running on the core).

   @sa
   HFI_CMD_SYS_SET_PROPERTY
   HFI_CMD_SYS_GET_PROPERTY
   */

/**
   Specifies the debug message levels and channel on which the debug
   messages will be communicated by the FW

   Debug messages could either be communicated through debug queue or
   QDSS (Special Hardware dedicated to collating the debug messages)

   Default: Disabled (No debug messages are generated)

   @datatypes
   HFI_DEBUG_CONFIG_TYPE
   */
#define  HFI_PROPERTY_SYS_DEBUG_CONFIG                      (HFI_PROPERTY_ICP_COMMON_START + 0x001)

/**
    Specifies UBWC-related configurations (legacy - applies to both BPS and IPE)

    Default: NA

    @datatypes
    HFI_UBWC_CONFIG_TYPE
*/
#define  HFI_PROPERTY_SYS_UBWC_CONFIG                       (HFI_PROPERTY_ICP_COMMON_START + 0x002)

/**
   System Property to query firmware image version.

   Host should query this system property to get the firmware image version.

   Default: NA

   @datatypes
   HFI_PROPERTY_SYS_IMAGE_VERSION_INFO_TYPE
   */
#define  HFI_PROPERTY_SYS_IMAGE_VERSION                     (HFI_PROPERTY_ICP_COMMON_START + 0x003)

/**
   Advertise what system supports

   Default: NA

   @datatypes
   HFI_SYSTEM_SUPPORTED_TYPE

   @sa
   HFI_MSG_SYS_INIT_DONE
   */
#define  HFI_PROPERTY_SYSTEM_SUPPORTED                      (HFI_PROPERTY_ICP_COMMON_START + 0x004)

/**
   System property to enable/disable IPE/BPS power collapse.

   Default: IPE/BPS power collapse is disabled by default.

   @datatypes
   HFI_ENABLE_TYPE
   */
#define  HFI_PROPERTY_SYSTEM_IPEBPS_PC                      (HFI_PROPERTY_ICP_COMMON_START + 0x005)

/**
    Specifies the frequency in MHz of the IPE core(s).

    Default: 0.

    @datatypes
    HFI_PROPERTY_CONFIG_TYPE
    */
#define  HFI_PROPERTY_SYS_IPE_HW_FREQUENCY                  (HFI_PROPERTY_ICP_COMMON_START + 0x006)

/**
    Specifies the frequency in MHz of the BPS core.

    Default: 0.

    @datatypes
    HFI_PROPERTY_CONFIG_TYPE
    */
#define  HFI_PROPERTY_SYS_BPS_HW_FREQUENCY                  (HFI_PROPERTY_ICP_COMMON_START + 0x007)

/**
   System property to enable/disable hang dump feature.

    Hang dump feature is a debug feature where essential debug information:
        - Last ConfigIo data
        - Last ProcessFrame data including all indirect CDM buffers / striping output
        - HW Regdump (in case of HW timeout - triggered by targetCompletionTime x 2
    will be written to the CDMBuffer given in the IPE/BPS process frame command.
    The content CDMBuffer will be returned in the ProcessFrameDone message so host
    SW can dump the content to file system for offline analysis/replaying.

    The following property controls the mode where FW would operate regarding to
    this feature:
      0 - FW_HANG_DUMP_DISABLED   - FW does not collect any information and write to
                                   the CDMBuffer provided in IPE/BPS process frame command.
      1 - FW_HANG_DUMP_ON_FAILURE - FW will dump the debug information to CDMBuffer when:
                                      a) IPE/BPS process frame command raised an error
                                         sychronously (e.g. due to invalid configuration)
                                         OR
                                      b) IPE/BPS process frame command is accepted but HW
                                         does not complete the frame in time (set by
                                         targetCompletionTime in process frame command), in
                                         which case HW Register Dump of all HW cores will
                                         also be dumped.
                                   After this mode is set, the CDMBufferSize returned by
                                   IPE/BPS ConfigIoResult may increase to guarantee enough
                                   space to hold the debug information. Host SW should
                                   therefore turn on this mode before sending ConfigIo command.
                                   Other than this size impact, there is no performance
                                   degradation if no failure is encountered.
      2 - FW_HANG_DUMP_ON_ALWAYS  - FW always dump the debug information (excluding HW register
                                   dump) for each IPE/BPS process command. This is useful
                                   for debugging IQ related issues where no failures will
                                   be encountered. Performance will degrade slightly.

   Default: Hang dump is disabled by default.

   @datatypes
   HFI_PROPERTY_CONFIG_HANG_DUMP_MODE
*/
#define  HFI_PROPERTY_SYSTEM_HANG_DUMP_MODE                 (HFI_PROPERTY_ICP_COMMON_START + 0x008)

/**
    Specifies UBWC-related configurations (new - allows separate configurations for BPS and IPE)

    Default: NA

    @datatypes
    HFI_UBWC_CONFIG_TYPE_EX
*/
#define  HFI_PROPERTY_SYS_UBWC_CONFIG_EX                    (HFI_PROPERTY_ICP_COMMON_START + 0x009)

/**
   Level 2 Comment: "Capability Types"
   The section defines all the capabilities (HFI_CAPABILITY_X) that could be
   reported at sys init. These are the supported range of parameters for
   a particular FW-Chipset implementation

   @datatypes
   HFI_CAPABILITY_SUPPORTED_TYPE

   @sa
   HFI_PROPERTY_PARAM_CAPABILITY_SUPPORTED
   HFI_MSG_SYS_INIT_DONE
   */
#define  HFI_CAPABILITY_PLACEHOLDER_1           (HFI_COMMON_BASE + 0x1) /**< Place Holder 1 */
#define  HFI_CAPABILITY_PLACEHOLDER_2           (HFI_COMMON_BASE + 0x2) /**< Place Holder 2 */

#pragma pack(push)
#pragma pack(1)

/**
   Payload to report capabilities through
   HFI_PROPERTY_PARAM_CAPABILITY_SUPPORTED
   */
typedef struct HFI_CAPABILITY_SUPPORTED_TYPE
{
    uint32_t eCapabilityType;   /**< Capability type: refer to HFI_CAPABILITY_X */
    uint32_t nMin;              /**< Minimum supported value for the capability */
    uint32_t nMax;              /**< Maximum supported value for the capability */
    uint32_t nStepSize;         /**< Supported steps between min-max */

} HFI_CAPABILITY_SUPPORTED_TYPE;

/**
   Capability array reported through
   HFI_PROPERTY_PARAM_CAPABILITY_SUPPORTED

   @datatypes
   HFI_CAPABILITY_SUPPORTED_TYPE
   */
typedef struct HFI_CAPABILITY_SUPPORTED_INFO_TYPE
{
    uint32_t nNumCapabilities; /**< Number of capabilities listed */
    HFI_CAPABILITY_SUPPORTED_TYPE rgData[1]; /**< Capabilities info array. The
                                               number of elements in the array
                                               is defined by nNumCapabilities. */

} HFI_CAPABILITY_SUPPORTED_INFO_TYPE;

/**
   Level 2 Comment: "Debug Msg types"
   Section describes different debug levels (HFI_DEBUG_MSG_X)
   available for debug messages from FW

   @sa
   HFI_PROPERTY_SYS_DEBUG_CONFIG
   */
#define  HFI_DEBUG_MSG_LOW      0x00000001 /**< Low level messages. */
#define  HFI_DEBUG_MSG_MEDIUM   0x00000002 /**< Medium level messages. */
#define  HFI_DEBUG_MSG_HIGH     0x00000004 /**< High level messages. */
#define  HFI_DEBUG_MSG_ERROR    0x00000008 /**< Error messages. */
#define  HFI_DEBUG_MSG_FATAL    0x00000010 /**< Fatal messages. */
#define  HFI_DEBUG_MSG_PERF     0x00000020 /**< messages containing performance data */
#define  HFI_DEBUG_CFG_WFI      0x01000000 /**< Disable ARM9 WFI in low power mode. */
#define  HFI_DEBUG_CFG_ARM9WD   0x10000000 /**< Disable ARM9 watchdog. */

/**
   Level 2 Comment: "Debug Msg Communication types"
   Section describes different modes (HFI_DEBUG_MODE_X)
   available to communicate the debug messages

   @sa
   HFI_PROPERTY_SYS_DEBUG_CONFIG
   */
#define HFI_DEBUG_MODE_QUEUE     0x00000001 /**< Debug message output through
   the interface debug queue.*/
#define HFI_DEBUG_MODE_QDSS      0x00000002 /**< Debug message output through QDSS.*/

/**
   Payload structure to configure HFI_PROPERTY_SYS_DEBUG_CONFIG

   @sa
   HFI_PROPERTY_SYS_DEBUG_CONFIG
   HFI_DEBUG_MSG_X
   HFI_DEBUG_CFG_X
   */
typedef struct HFI_DEBUG_CONFIG_TYPE
{
    uint32_t nDebugConfig;   /**< The debug message types to output.
                                 This parameter is the result of
                                 HFI_DEBUG_MSG_X values that are OR’d
                                 together to specify the debug message
                                 types to output.  */
    uint32_t nDebugMode;     /**< Specifies the debug modes to enable.
                                 This parameter is the result of
                                 HFI_DEBUG_MODE_X values that are OR’d
                                 together to specify the debug modes to enable. */

} HFI_DEBUG_CONFIG_TYPE;

typedef enum
{
    CAMERA_ICP_DEVICE_A5,
    CAMERA_ICP_DEVICE_CDM,
    CAMERA_ICP_DEVICE_BPS,
    CAMERA_ICP_DEVICE_IPE,
    MAX_CAMERA_ICP_DEVICE

} CAMERA_ICP_DEVICE_TYPE;

typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t bankspread_en         : 1;
            uint32_t highestbankbit_en     : 1;
            uint32_t highestbankbit_lv1_en : 1;
            uint32_t highestbankbit_val    : 5;
            uint32_t reserved              : 1; // renamed as reserved as original field is
                                                // controlled by UBWC-related fields in ImageInfo
            uint32_t un_used               : 3;
            uint32_t ubwc_8channel_en      : 1;
        } fields;
    } mode;
} UbwcFetchModeCfg;

typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t ubwc_en                 : 1;
            uint32_t compress_en             : 1;
            uint32_t ubwc_bankspread_en      : 1;
            uint32_t ubwc_highestbankbit_en  : 1;
            uint32_t ubwc_highestbank_lv1_en : 1;
            uint32_t ubwc_highestbankbit_val : 5;
            uint32_t reserved                : 3; // renamed as reserved as original field is
                                                  // controlled by UBWC-related fields in ImageInfo
            uint32_t un_used                 : 3;
            uint32_t ubwc_8channel_en        : 1;
        } fields;
    } mode;
} UbwcWriteModeCfg;

typedef struct HFI_UBWC_CONFIG_TYPE
{
    UbwcFetchModeCfg ubwcFetchModeCfg;
    UbwcWriteModeCfg ubwcWriteModeCfg;

} HFI_UBWC_CONFIG_TYPE;

typedef struct HFI_UBWC_CONFIG_TYPE_EX
{
    HFI_UBWC_CONFIG_TYPE bps;
    HFI_UBWC_CONFIG_TYPE ipe;

} HFI_UBWC_CONFIG_TYPE_EX;

typedef struct HFI_PROPERTY_CONFIG_TYPE
{
    uint32_t value;
} HFI_PROPERTY_CONFIG_TYPE;

typedef enum
{
    FW_HANG_DUMP_DISABLED = 0,
    FW_HANG_DUMP_ON_FAILURE,
    FW_HANG_DUMP_ON_ALWAYS,
} HangDumpMode;

typedef struct HFI_PROPERTY_CONFIG_HANG_DUMP_MODE
{
    HangDumpMode hangDumpMode;

} HFI_PROPERTY_CONFIG_HANG_DUMP_MODE;

#define GET_BIT_FIELD(name, val)    (((val) & name##_BMSK) >> name##_SHFT)
#define SET_BIT_FIELD(name, val)    (((val) << name##_SHFT) & name##_BMSK)

/**
   FW API VERSION
   Version of the API defined by the header files. Use macros below to
   get bit fields within the apiVersion from HFI_SYSTEM_SUPPORTED_TYPE
   */
#define FW_API_VERSION_MAJOR_VAL          1
#define FW_API_VERSION_MINOR_VAL          0
#define FW_API_VERSION_REVISION_VAL       35

#define FW_API_VERSION_MAJOR_BMSK         0xFF000000
#define FW_API_VERSION_MAJOR_SHFT         24
#define FW_API_VERSION_MINOR_BMSK         0x00FF0000
#define FW_API_VERSION_MINOR_SHFT         16
#define FW_API_VERSION_REVISION_BMSK      0x0000FFFF
#define FW_API_VERSION_REVISION_SHFT      0

#define FW_API_VERSION                   (SET_BIT_FIELD(FW_API_VERSION_MAJOR, FW_API_VERSION_MAJOR_VAL) | \
                                          SET_BIT_FIELD(FW_API_VERSION_MINOR, FW_API_VERSION_MINOR_VAL) | \
                                          SET_BIT_FIELD(FW_API_VERSION_REVISION, FW_API_VERSION_REVISION_VAL))

#define FW_API_VERSION_GET_MAJOR(ver)     GET_BIT_FIELD(ver, FW_API_VERSION_MAJOR)
#define FW_API_VERSION_GET_MINOR(ver)     GET_BIT_FIELD(ver, FW_API_VERSION_MINOR)
#define FW_API_VERSION_GET_REVISION(ver)  GET_BIT_FIELD(ver, FW_API_VERSION_REVISION)

/**
   Number of HWs
   The numbers of HW core available. Use macros below to
   get bit fields within the numberICPHW from HFI_SYSTEM_SUPPORTED_TYPE
   */
#define NUM_BPS_BMSK                    0xFF000000
#define NUM_BPS_SHFT                    24
#define NUM_IPE_BMSK                    0x00FF0000
#define NUM_IPE_SHFT                    16
#define NUM_CDM_BMSK                    0x0000FF00
#define NUM_CDM_SHFT                    8

#define GET_NUM_BPS(numberICPHW)        GET_BIT_FIELD(numberICPHW, NUM_BPS)
#define GET_NUM_IPE(numberICPHW)        GET_BIT_FIELD(numberICPHW, NUM_IPE)
#define GET_NUM_CDM(numberICPHW)        GET_BIT_FIELD(numberICPHW, NUM_CDM)

/**
   Payload structure to report maximum supported features of FW.
   */
typedef struct HFI_SYSTEM_SUPPORTED_TYPE
{
    uint32_t apiVersion;
    uint32_t deviceTitanVersion;
    uint32_t numberICPHW;               /**< numBPSs | numIPEs | numCDMs | reserved (1 byte each)*/
    uint32_t deviceHWVersion[MAX_CAMERA_ICP_DEVICE];
    uint32_t bForce8BppIpe;             /** flag to indicate if 8BPP IPE fuse is blown,
                                             = 1 means IPE supports only 8BPP output
                                             = 0 means IPE supports both 8/10 BPP output  */
    uint32_t reservedPlaceHolder;

} HFI_SYSTEM_SUPPORTED_TYPE;

/**
   Payload structure to report HFI_PROPERTY_PARAM_PROPERTIES_SUPPORTED
   for a session
   */
typedef struct HFI_PROPERTIES_SUPPORTED_TYPE
{
    uint32_t nNumProperties;   /**< The number of properties supported,
                                   which indicates the size of the properties
                                   array that follows.  */
    uint32_t rgProperties[1];  /**< The array of supported property IDs. */

} HFI_PROPERTIES_SUPPORTED_TYPE;


/**
   Payload structure to report HFI_PROPERTY_SYS_IMAGE_VERSION

   @datatypes
   HFI_PROPERTY_SYS_IMAGE_VERSION_INFO_TYPE
   */
typedef struct HFI_PROPERTY_SYS_IMAGE_VERSION_INFO_TYPE
{
    uint32_t nMajor;               /**< Major version number */
    uint32_t nMinor;               /**< Minor version number */
    uint32_t nStringSize;          /**< size(in Bytes) of the array that follows */
    uint8_t  nStrImageVersion[1];  /**< Image version String */
} HFI_PROPERTY_SYS_IMAGE_VERSION_INFO_TYPE;


/**
   Level 2 Comment: "System Command Types"
   The section defines system specific control/data commands (HFI_CMD_X)
   System commands affect behavior of all the sessions

   All the responses from FW are generated asynchronously. The
   responses are generates as messages and are defined as HFI_MSG_X
   */

/**
   This command initializes the controller after firmware image download

   @return
   HFI_MSG_SYS_INIT_DONE

   @datatypes
   HFI_CMD_SYS_INIT_PACKET_TYPE

   */
#define  HFI_CMD_SYS_INIT                    (HFI_CMD_ICP_COMMON_START + 0x001)

/**
   Command to Firmware to prepare for power collapse.

   @dependecies
   HFI_CMD_SYS_INIT

   @return
   HFI_MSG_SYS_PC_PREP_DONE

   @datatypes
   HFI_CMD_SYS_PC_PREP_PACKET_TYPE
   */
#define  HFI_CMD_SYS_PC_PREP                 (HFI_CMD_ICP_COMMON_START + 0x002)

/**
   This command sets SYS property(ies)

   @dependencies
   HFI_CMD_SYS_INIT

   @return
   None (No response is sent)

   @datatypes
   HFI_CMD_SYS_SET_PROPERTY_PACKET_TYPE

   @sa
   HFI_PROPERTY_SYS_X
   */
#define  HFI_CMD_SYS_SET_PROPERTY            (HFI_CMD_ICP_COMMON_START + 0x003)

/**
   This command queries SYS property(ies). Currently no system level
   properties have been defined

   @dependencies
   HFI_CMD_SYS_INIT

   @return
   None (No response message has been defined for this)

   @datatypes
   HFI_CMD_SYS_GET_PROPERTY_PACKET_TYPE

   @sa
   HFI_PROPERTY_SYS_X
   */
#define  HFI_CMD_SYS_GET_PROPERTY            (HFI_CMD_ICP_COMMON_START + 0x004)


/**
   This command pings the Firmware to confirm whether it is alive.

   @return
   HFI_MSG_SYS_PING_ACK

   @datatypes
   HFI_CMD_SYS_PING_PACKET_TYPE
*/
#define  HFI_CMD_SYS_PING                    (HFI_CMD_ICP_COMMON_START + 0x005)

/**
This command pings the Firmware to confirm reset/destroy FW stack.

@return
HFI_MSG_SYS_RESET_ACK

@datatypes
HFI_CMD_SYS_RESET_PACKET_TYPE
*/
#define  HFI_CMD_SYS_RESET                    (HFI_CMD_ICP_COMMON_START + 0x006)

/**
   Level 2 Comment: "Message Types"
   The section defines common control/data messages sent
   by FW in response to a submitted command

   All the responses from FW are generated asynchronously.
   */

/**
   This message is sent in response to the HFI_CMD_SYS_INIT command once the
   Firmware system initialization is complete.

   @return
   NA

   @datatypes
   HFI_MSG_SYS_INIT_DONE_PACKET_TYPE

   @sa
   HFI_CMD_SYS_INIT
   */
#define  HFI_MSG_SYS_INIT_DONE               (HFI_MSG_ICP_COMMON_START + 0x1)

/**
   This message is sent in response to the HFI_CMD_SYS_PC_PREP command.
   This means that the FW preparation for power collapse is complete and host
   may now initiate the power collapse sequence for the core

   @return
   NA

   @datatypes
   HFI_MSG_SYS_PC_PREP_DONE_PACKET_TYPE

   @sa
   HFI_CMD_SYS_PC_PREP
   */
#define  HFI_MSG_SYS_PC_PREP_DONE            (HFI_MSG_ICP_COMMON_START + 0x2)

/**
   This message is sent by the Firmware to provide debug information
   for the Host.

   @return
   NA

   @datatypes
   HFI_MSG_SYS_DEBUG_PACKET_TYPE

   @sa
   HFI_PROPERTY_SYS_DEBUG_CONFIG
   */
#define  HFI_MSG_SYS_DEBUG                   (HFI_MSG_ICP_COMMON_START + 0x3)

/**
   This message indicates that the firmware has gone into the idle state and
   will not continue further processing until it gets another Host command.
   The firmware is can be commanded to prepare for power collapse when
   this message is received.

   @return
   NA

   @datatypes
   HFI_MSG_SYS_IDLE_PACKET_TYPE

   */
#define  HFI_MSG_SYS_IDLE                    (HFI_MSG_ICP_COMMON_START + 0x4)


/**
  This message returns properties queried in HFI_CMD_SYS_GET_PROPERTY

  @return
  NA

  @datatypes
  HFI_MSG_SYS_PROPERTY_INFO_PACKET_TYPE

  @sa
  HFI_CMD_SYS_GET_PROPERTY
  */
#define  HFI_MSG_SYS_PROPERTY_INFO           (HFI_MSG_ICP_COMMON_START + 0x5)

/**
This message is sent in response to the HFI_CMD_SYS_PING command.

@return
NA

@datatypes
HFI_MSG_SYS_PING_ACK_PACKET_TYPE

@sa
HFI_CMD_SYS_PING
*/
#define  HFI_MSG_SYS_PING_ACK                (HFI_MSG_ICP_COMMON_START + 0x6)

/**
This message is sent in response to the HFI_CMD_SYS_RESET command.

@return
NA

@datatypes
HFI_MSG_SYS_RESET_ACK_PACKET_TYPE

@sa
HFI_CMD_SYS_RESET
*/
#define  HFI_MSG_SYS_RESET_ACK                (HFI_MSG_ICP_COMMON_START + 0x7)


/**
   This message is sent to notify the Host of a particular event.
   (Refer to hfidefs.h for PACKET_TYPE and eventID definition)

   @return
   NA

   @datatypes
   HFI_MSG_EVENT_NOTIFY_PACKET_TYPE

   @sa
   HFI_EVENT_X
   */
#define  HFI_MSG_EVENT_NOTIFY                (HFI_MSG_ICP_COMMON_START + 0x8)

/**
   The System Session Initialization command packet definition.

   @sa
   HFI_CMD_SYS_INIT
   */
typedef struct HFI_CMD_SYS_INIT_PACKET_TYPE
{
    uint32_t nSize;          /**< The packet size, in bytes. */
    uint32_t ePacketType;    /**< HFI_CMD_SYS_INIT */

} HFI_CMD_SYS_INIT_PACKET_TYPE;

/**
   The System Power Collapse Prep command packet definition

   @sa
   HFI_CMD_SYS_PC_PREP
   */
typedef struct HFI_CMD_SYS_PC_PREP_PACKET_TYPE
{
    uint32_t nSize;          /**< The packet size, in bytes. */
    uint32_t ePacketType;    /**< HFI_CMD_SYS_PC_PREP*/

} HFI_CMD_SYS_PC_PREP_PACKET_TYPE;


/**
   The System Set Property command packet definition

   @sa
   HFI_CMD_SYS_SET_PROPERTY
   */
typedef struct HFI_CMD_SYS_SET_PROPERTY_PACKET_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< HFI_CMD_SYS_SET_PROPERTY */
    uint32_t nNumProperties;    /**< The number of properties being set.*/
    uint32_t rgPropertyData[1]; /**< An array of property ID + associated
                                    structure pairs. The size of the array
                                    is defined by nNumProperties. */

} HFI_CMD_SYS_SET_PROPERTY_PACKET_TYPE;

/**
   The System Get Property command packet definition

   @sa
   HFI_CMD_SYS_GET_PROPERTY
   */
typedef struct HFI_CMD_SYS_GET_PROPERTY_PACKET_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< HFI_CMD_SYS_GET_PROPERTY */
    uint32_t nNumProperties;    /**< The number of properties being queried. */
    uint32_t rgPropertyData[1]; /**< The array of property IDs being quered. The
                                    size of the array is defined by
                                    nNumProperties. For values refer
                                    HFI_PROPERTY_X. */

} HFI_CMD_SYS_GET_PROPERTY_PACKET_TYPE;

/**
The System Ping command packet definition

@sa
HFI_CMD_SYS_PING
*/
typedef struct HFI_CMD_SYS_PING_PACKET_TYPE
{
    uint32_t nSize;          /**< The packet size, in bytes. */
    uint32_t ePacketType;    /**< HFI_CMD_SYS_PING */
    uint64_t nClientData;    /**< Client data associated with this command.
                             The Firmware shall return this data in the
                             corresponding HFI_MSG_SYS_PING_ACK
                             message packet.*/

} HFI_CMD_SYS_PING_PACKET_TYPE;

/**
The System Reset command packet definition

@sa
HFI_CMD_SYS_RESET
*/
typedef struct HFI_CMD_SYS_RESET_PACKET_TYPE
{
    uint32_t nSize;          /**< The packet size, in bytes. */
    uint32_t ePacketType;    /**< HFI_CMD_SYS_RESET */
    uint64_t nClientData;    /**< Client data associated with this command.
                             The Firmware shall return this data in the
                             corresponding HFI_MSG_SYS_RESET_ACK
                             message packet.*/

} HFI_CMD_SYS_RESET_PACKET_TYPE;


/**
   The System Init Done message packet definition. Many system level properties
   are also returned with this packet.

   See the following section for example

   @sa
   HFI_MSG_SYS_INIT_DONE
   */
typedef struct HFI_MSG_SYS_INIT_DONE_PACKET_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< HFI_MSG_SYS_INIT_DONE */
    uint32_t eErrorType;        /**< The error code associated with this
                                    response. Refer to HFI_ERROR_X for values. */
    uint32_t nNumProperties;    /**< The number of default and/or capability
                                    property info structures being returned, in
                                    association with this response message.*/
    uint32_t rgPropertyData[1]; /**< An array of property ID + associated structure pairs.
                                    The number of elements in the array is defined by
                                    nNumProperties. */
} HFI_MSG_SYS_INIT_DONE_PACKET_TYPE;

/**
   The System Power Collapse Prep Done message packet definition

   @sa
   HFI_MSG_SYS_PC_PREP_DONE
   */
typedef struct HFI_MSG_SYS_PC_PREP_DONE_PACKET_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< HFI_MSG_SYS_PC_PREP_DONE */
    uint32_t eErrorType;        /**< The error code associated with this response.
                                    Refer to HFI_ERROR_X for values. */

} HFI_MSG_SYS_PC_PREP_DONE_PACKET_TYPE;

/** The System Property Info message packet definition
 *
 * nSize          : The packet size, in bytes.
 * ePacketType    : HFI_MSG_SYS_PROPERTY_INFO
 * nNumProperties : The number of property info structures being
 *                  returned.
 * rgPropertyData : An array of property ID + associated
 *                  structure pairs.  The number of elements in
 *                  the array is defined by nNumProperties.
 */
typedef struct HFI_MSG_SYS_PROPERTY_INFO_PACKET_TYPE
{
    uint32_t nSize;
    uint32_t ePacketType;
    uint32_t nNumProperties;
    uint32_t rgPropertyData[1];
} HFI_MSG_SYS_PROPERTY_INFO_PACKET_TYPE;


/**
   The System Idle message packet definition

   @sa
   HFI_MSG_SYS_IDLE
   */
typedef struct HFI_MSG_SYS_IDLE_PACKET_TYPE
{
    uint32_t nSize;          /**< The packet size, in bytes. */
    uint32_t ePacketType;    /**< HFI_MSG_SYS_IDLE */

} HFI_MSG_SYS_IDLE_PACKET_TYPE;


/**
   The System Debug message packet definition

   @sa
   HFI_MSG_SYS_DEBUG
   */
typedef struct HFI_MSG_SYS_DEBUG_PACKET_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< HFI_MSG_SYS_DEBUG */
    uint32_t eMsgType;          /**< The debug message type. Refer HFI_DEBUG_MSG_X for values. */
    uint32_t nMsgSize;          /**< The size of the debug message in bytes. */
    uint32_t nTimeStampHi;      /**< The most significant 32 bits of the 64 bit timestamp field.
                                    The timestamp shall be interpreted as a signed 64-bit value
                                    representing microseconds. */
    uint32_t nTimeStampLo;      /**< The least significant 32 bits of the 64 bit timestamp field.
                                    The timestamp shall be interpreted as a signed 64-bit value
                                    representing microseconds. */
    uint8_t  rgMsgData[1];      /**< Array of bytes containing message string. */

} HFI_MSG_SYS_DEBUG_PACKET_TYPE;


/**
The System Ping Ack message packet definition

@sa
HFI_MSG_SYS_PING_ACK
*/
typedef struct HFI_MSG_SYS_PING_ACK_PACKET_TYPE
{
    uint32_t nSize;          /**< The packet size, in bytes. */
    uint32_t ePacketType;    /**< HFI_MSG_SYS_PING_ACK */
    uint64_t nClientData;    /**< The associated client data that was passed in
                             the associated ping command. */

} HFI_MSG_SYS_PING_ACK_PACKET_TYPE;

/**
The System Reset Ack message packet definition

@sa
HFI_MSG_SYS_RESET_ACK
*/
typedef struct HFI_MSG_SYS_RESET_ACK_PACKET_TYPE
{
    uint32_t nSize;          /**< The packet size, in bytes. */
    uint32_t ePacketType;    /**< HFI_MSG_SYS_RESET_ACK */
    uint64_t nClientData;    /**< The associated client data that was passed in
                             the associated ping command. */

} HFI_MSG_SYS_RESET_ACK_PACKET_TYPE;


#pragma pack(pop)

#endif // _ICP_DEFS_H_
