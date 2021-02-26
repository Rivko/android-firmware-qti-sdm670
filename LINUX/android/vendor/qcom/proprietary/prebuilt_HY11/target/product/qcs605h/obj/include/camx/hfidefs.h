//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  hfidefs.h
// @brief HFI (Host-Firmware Interface) command & message definitions.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
//
// 08/04/2015        pahlin        Initial version
//------------------------------------------------------------------------

#ifndef _HFI_DEFS_H_
#define _HFI_DEFS_H_

#include "icpcomdef.h"

/**
    Level 2 Comment: "Constants and Utility Macros"
    The section defines all the constants and macros that are shared between
    FW and host. These utility macros shall be used by both entities to stay
    in sync
    */

/**
    Defines the offset in word for command ID in the HFI command packet
    */
#define HFI_CMD_TYPE_OFFSET_IN_UWORD32    0x1

/**
   Level 2 Comment: "Base offset definitions"
   Following base act as the common starting points for all
   enumerations.
   */

#define  HFI_COMMON_BASE                  (0)                     /**< Base offset for common constants */

/**
   Level 2 Comment: "Domain Base offsets"
   The HFI_DOMAIN_BASE_X defines all possible domain base offsets to be
   used while defining constants for a particular domain
   */
#define  HFI_DOMAIN_SHFT                  (24)
#define  HFI_DOMAIN_BMSK                  (0x7 << HFI_DOMAIN_SHFT) /**< These 3 bits defines the domain. */
#define  HFI_DOMAIN_BASE_ICP              (0x0 << HFI_DOMAIN_SHFT) /**< ICP base. */
#define  HFI_DOMAIN_BASE_IPE_BPS          (0x1 << HFI_DOMAIN_SHFT) /**< IPE/BPS base. */
#define  HFI_DOMAIN_BASE_CDM              (0x2 << HFI_DOMAIN_SHFT) /**< CDM base. */
#define  HFI_DOMAIN_BASE_DBG              (0x3 << HFI_DOMAIN_SHFT) /**< DBG base. */

/**
   Command base offset for commands.
   This offset shall be used as a base offset for defining and command value
   */
#define  HFI_CMD_START_OFFSET             (0x00010000)

/**
    Command base offset for messages.
    This offset shall be used as a base offset for defining and message value
    */
#define  HFI_MSG_START_OFFSET             (0x00020000)

/**
    Level 2 Comment: "System Level Error types"
    Common HFI_ERROR_SYS_X values to be used as system level error/warning
    for events and messages
    */
#define  HFI_ERR_NONE                                   (HFI_COMMON_BASE)          /**< Status: No error */
#define  HFI_ERR_SYS_FATAL                              (HFI_COMMON_BASE + 0x1)    /**< Fatal system error */
#define  HFI_ERR_SYS_VERSION_MISMATCH                   (HFI_COMMON_BASE + 0x2)    /**< Interface version mismatch */
#define  HFI_ERR_SYS_UNSUPPORTED_DOMAIN                 (HFI_COMMON_BASE + 0x3)    /**< Unsupported domain. eventData2 specifies the domain type */
#define  HFI_ERR_SYS_UNSUPPORT_CMD                      (HFI_COMMON_BASE + 0x4)    /**< Command is not supported*/
#define  HFI_ERR_SYS_CMDFAILED                          (HFI_COMMON_BASE + 0x5)    /**< Command is not supported*/
#define  HFI_ERR_SYS_CMDSIZE                            (HFI_COMMON_BASE + 0x6)    /**< command size err*/

#define  HFI_ERR_ICP_START                                 (HFI_COMMON_BASE       + 0x64) /**< Start Range for ICP errors*/
#define  HFI_ERR_IPE_BPS_START                             (HFI_ERR_ICP_START     + 0x64) /**< Start Range for IPE/BPS errors*/
#define  HFI_ERR_CDM_START                                 (HFI_ERR_IPE_BPS_START + 0x64) /**< Start Range for CDM errors*/
#define  HFI_ERR_DBG_START                                 (HFI_ERR_CDM_START     + 0x64) /**< Start Range for DBG errors*/


/**
    Level 2 Comment: "base offset definition"
    The constants defined in the section shall be used as a base offset
    to define any constants for common API
    */

/**
    Base offset to define any system level property
    */
#define  HFI_PROPERTY_ICP_COMMON_START                               (HFI_DOMAIN_BASE_ICP + 0x0000)

/**
    Base offset to define any common system command
    */
#define  HFI_CMD_ICP_COMMON_START             (HFI_DOMAIN_BASE_ICP + HFI_CMD_START_OFFSET + 0x0000)

/**
    Base offset to define any system test command
    */
#define  HFI_CMD_ICP_TEST_START                                 (HFI_CMD_ICP_COMMON_START + 0x0800)

/**
    End of the system command space
    */
#define  HFI_CMD_ICP_END                                        (HFI_CMD_ICP_COMMON_START + 0x0FFF)

/**
    Base offset to define any common IPE/BPS command
    */
#define  HFI_CMD_IPE_BPS_COMMON_START     (HFI_DOMAIN_BASE_IPE_BPS + HFI_CMD_START_OFFSET + 0x0000)

/**
    Base offset to define any IPE/BPS test command
    */
#define  HFI_CMD_IPE_BPS_TEST_START                         (HFI_CMD_IPE_BPS_COMMON_START + 0x0800)

/**
    End of the IPE/BPS command space
    */
#define  HFI_CMD_IPE_BPS_END                                (HFI_CMD_IPE_BPS_COMMON_START + 0x0FFF)

/**
    Base offset to define any common CDM command
    */
#define  HFI_CMD_CDM_COMMON_START             (HFI_DOMAIN_BASE_CDM + HFI_CMD_START_OFFSET + 0x0000)

/**
    Base offset to define any CDM test command
    */
#define  HFI_CMD_CDM_TEST_START                                 (HFI_CMD_CDM_COMMON_START + 0x0800)

/**
    End of the CDM command space
    */
#define  HFI_CMD_CDM_END                                        (HFI_CMD_CDM_COMMON_START + 0x0FFF)

/**
    Base offset to define any common DBG command
*/
#define  HFI_CMD_DBG_COMMON_START             (HFI_DOMAIN_BASE_DBG + HFI_CMD_START_OFFSET + 0x0000)

/**
    Base offset to define any DBG test command
*/
#define  HFI_CMD_DBG_TEST_START                                 (HFI_CMD_DBG_COMMON_START + 0x0800)

/**
    End of the DBG command space
*/
#define  HFI_CMD_DBG_END                                        (HFI_CMD_DBG_COMMON_START + 0x0FFF)

/**
    Base offset to define any system message
    */
#define  HFI_MSG_ICP_COMMON_START             (HFI_DOMAIN_BASE_ICP + HFI_MSG_START_OFFSET + 0x0000)

/**
    Base offset to define any system test message
    */
#define  HFI_MSG_ICP_TEST_START                                 (HFI_MSG_ICP_COMMON_START + 0x0800)

/**
    End of the system message space
    */
#define  HFI_MSG_ICP_END                                        (HFI_MSG_ICP_COMMON_START + 0x0FFF)

/**
    Base offset to define any IPE/BPS message
    */
#define  HFI_MSG_IPE_BPS_COMMON_START      (HFI_DOMAIN_BASE_IPE_BPS + HFI_MSG_START_OFFSET + 0x0000)

/**
    Base offset to define any IPE/BPS test message
    */
#define  HFI_MSG_IPE_BPS_TEST_START                         (HFI_MSG_IPE_BPS_COMMON_START + 0x0800)

/**
    End of the IPE/BPS message space
    */
#define  HFI_MSG_IPE_BPS_END                                (HFI_MSG_IPE_BPS_COMMON_START + 0x0FFF)

/**
    Base offset to define any CDM message
    */
#define  HFI_MSG_CDM_COMMON_START             (HFI_DOMAIN_BASE_CDM + HFI_MSG_START_OFFSET + 0x0000)

/**
    Base offset to define any CDM test message
    */
#define  HFI_MSG_CDM_TEST_START                                 (HFI_MSG_CDM_COMMON_START + 0x0800)

/**
    End of the CDM message space
    */
#define  HFI_MSG_CDM_END                                        (HFI_MSG_CDM_COMMON_START + 0x0FFF)

/**
    Base offset to define any DBG message
*/
#define  HFI_MSG_DBG_COMMON_START             (HFI_DOMAIN_BASE_DBG + HFI_MSG_START_OFFSET + 0x0000)

/**
    Base offset to define any DBG test message
*/
#define  HFI_MSG_DBG_TEST_START                                 (HFI_MSG_DBG_COMMON_START + 0x0800)

/**
    End of the DBG message space
*/
#define  HFI_MSG_DBG_END                                        (HFI_MSG_DBG_COMMON_START + 0x0FFF)

/**
   Level 2 Comment: "Event types"
   All the common event types are defined in this section
   */
#define  HFI_EVENT_SYS_ERROR                (HFI_COMMON_BASE + 0x1) /**< A system level error was encountered. */
#define  HFI_EVENT_ICP_ERROR                (HFI_COMMON_BASE + 0x2) /**< An ICP level error was encountered. */
#define  HFI_EVENT_IPE_BPS_ERROR            (HFI_COMMON_BASE + 0x3) /**< An IPE/BPS level error was encountered. */
#define  HFI_EVENT_CDM_ERROR                (HFI_COMMON_BASE + 0x4) /**< A CDM level error was encountered. */
#define  HFI_EVENT_DBG_ERROR                (HFI_COMMON_BASE + 0x5) /**< A DBG level error was encountered. */

#pragma pack(push)
#pragma pack(1)

/**
   Payload structure to be used for enable/disable properties.
   Applicable to properties that do not need any other parameter
   to qualify

   */
typedef struct HFI_ENABLE_TYPE
{
    uint32_t bEnable; /**< Set to TRUE (1) if enabled;
                       set to FALSE (0) if disabled. */
    uint32_t coreinfo; //To select IPE or BPS for IFPC

} HFI_ENABLE_TYPE;

/**
   The Event Notify message packet definition.

   @sa
   HFI_MSG_EVENT_NOTIFY (in icpdefs.h)
   */
typedef struct HFI_MSG_EVENT_NOTIFY_PACKET_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< HFI_MSG_EVENT_NOTIFY */
    uint32_t nSessionID;        /**< The ID value associated with this session. */
    uint32_t nEventID;          /**< Valid only for session event IDs. */
    uint32_t eventData1;        /**< Event data corresponding to the event ID. */
    uint32_t eventData2;        /**< Event data corresponding to the event ID. */
    uint32_t rgExtEventData[1]; /**< The info array, interpreted based on eventData1/eventData2 */

} HFI_MSG_EVENT_NOTIFY_PACKET_TYPE;

#pragma pack(pop)

#endif // _HFI_DEFS_H_

