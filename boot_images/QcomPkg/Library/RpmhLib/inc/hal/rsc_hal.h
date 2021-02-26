/*
==============================================================================

FILE:         rsc_hal.h

DESCRIPTION:  This file contains description about RSC Hal. Rsc HAL is mainly responsible for 
              managing TCSs/AMCs associcated with RSC hw block on each subsystem


==============================================================================

                             Edit History



Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================


#ifndef _HAL_RSC
#define _HAL_RSC

#include "comdef.h"
#include "rsc_hal_hwio.h"
#include "ULogFront.h"
#include "rpmh_common.h"


/**
* Error Defines
**/

#define HAL_STATUS_SUCCESS                      (0)
#define HAL_INVALID_DRV_MAP_ID                  (-1)
#define HAL_INVALID_TCS_ID                      (-2)
#define HAL_INVALID_CMD_ID                      (-3)
#define HAL_INVALID_THRESHOLD                   (-4)
#define HAL_INVALID_INPUT_PTR                   (-5)
#define HAL_INVALID_CTXT_NOT_INITIALIZED        (-6)
#define HAL_INVALID_OP_FOR_DRV                  (-7)
#define HAL_INVALID_DRV_ID                      (-8)
#define HAL_TCS_NOT_IDLE                        (-9)

/**
* RPMhRSCRequestType defines types of request RPMh driver could make to RPMh HW.
* WriteFirenForget:         This is a write request with fire-n-forget flag. For a Fire-n-forget request,
*                           RSC Hw ensures that the request has been sent on ePCB. It does NOT guarantee completion.
* WriteCompletion:          This is a write request with completion flag. For a completion request,
*                           RSC Hw ensures completion and updates corrosponding interrupt.
* Read:                     This is a simple read request on RPMh HW block and primarily used for debugging
**/

typedef enum {
    WriteFirenForget=0,
    WriteCompletion,
    ReadRequest
} RSCRequestType;

/**
* RscHalMessageBody defines various parameters of a command to be transferred to RPMh HW through TCS/AMC
* CmdId:        It is the command id in a given TCS. This would vary based on NumCommandsPerTCS defined in HW
* SlaveId:      This is a unique identifier for different blocks on RPMh HW.
*    a. PDC                  1
*    b. PDC Sequence Memory  2
*    c. ARC                  3
*    d. VRM                  4
*    e. BCM                  5
*    f. DDR_AUX              6
* Offset:       Offset on block mentioned as SlaveId
* RequestType:  Mentioned Above 
* Data:         Data to be sent    
* DataSize:     Size of data to be sent. Max could be 8 bytes. Hardcode it to 8
*
**/

typedef struct RSCHALMESSAGEBODY{
    uint8              CmdId;
    uint8              SlaveId;
    uint16             Offset;
    RSCRequestType     RequestType;
    uint8              DataSize;
    uint32             Data;
}RscHalMessageBody;

/**
* RSCHALMESSAGE is the container for TCS/AMC commands
* RscSwDrvMapId:        Helps RSC Hal to identify Drv--e.g. TZ, HLOS, MPSS etc--and hence baseaddress
* TcsId:                TCS/AMC Id to be populated
* TotalCommands:        Total number of commands passed in one shot
* Start:                Pointer to very first command
*/

typedef struct RSCHALMESSAGE{
    RSCSW_DRV_MAPPING  RscSwDrvMapId;
    uint8              TcsId;
    uint32             TotalCommands;
    RscHalMessageBody *Start;
}RscHalMessage;


/**
* This is a config structure maintained internally by RSC HAL
* RscSwDrvMapId:    Uniqure DRV Identifier shared between RPMh driver and RSC HAL
* BaseAddress:      Base Address of RSC HW registers to access DRV specific information
* DrvId:            Most of the RSC addresses are RSC base address + drvid*1000. For most cases, this should be 0, but can be different
* DrvInitialzed:    TRUE/FALSE
* NumTCS:           Number of TCSs supported by DRV
* NumCmdsPerTCS:    Number of commands supported by each TCS/AMC 
**/

typedef struct {
    RSCSW_DRV_MAPPING RscSwDrvMapId;
    uint32            BaseAddress;
    uint32            DrvId;
    boolean           DrvInitialized;
    uint32            NumTcs;
    uint32            NumCmdsPerTcs;
} DrvConfig;

/**
* Internal Context Structure for RSC HAL
**/

typedef struct RscHalCtxt {
    boolean           HalInitialized;
    uint32            NumDrvs;
    DrvConfig         DrvConfigArray[RSC_DRV_MAX];
} RscHalCtxt; 


/**
* Function Name: RSCHalRegisterDrv 
* Description:   It is the first call made by RPMh driver to register/init a new DRV
* Input:         RSCSW_DRV_MAPPING DrvId
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalRegisterDrv (RSCSW_DRV_MAPPING );

/**
* Function Name: RscHalVersionID 
* Description:   It returns RSC HW version Id
* Input:         RSCSW_DRV_MAPPING Id
*                uint32 *version: It returns RSC HW version Id
*                RSC HW Revision ID format:
*                23:16 Major Version
*                15:8  Minor Version
*                7:0   Step Version
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalVersionID (RSCSW_DRV_MAPPING drvMapId, uint32 *version);

/**
* Function Name: RscHalReadConfig 
* Description:   It returns DRV-specific information such as Number of TCSs supported and Number of commands per TCS
* Input:         RSCSW_DRV_MAPPING Id
*                uint32 *NumTCS: Returns number of TCS supported 
*                uint32 *NumCmdsPerTCS: Returns number of commands per TCS
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalReadConfig (RSCSW_DRV_MAPPING DrvId, uint32 *NumTCS, uint32 *NumCmdsPerTCS);

/**
* Function Name: RSCHalIsePCBTimedOut 
* Description:   It returns if ePCB has timedout. ePCB can only time out if PDC is hung which is an error fatal state
* Input:         RSCSW_DRV_MAPPING DrvId
*                bool *ePCBStatus to return TRUE/FALSE
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalIsePCBTimedOut (RSCSW_DRV_MAPPING DrvId, boolean *ePCBStatus);

/**
* Function Name: HalRscClearePCBTimedOut 
* Description:   Clear ePCBTimedout IRQ. This can only be done by DRV0
* Input:         None
* Return:        None
**/

int32 RscHalClearePCBTimedOut (RSCSW_DRV_MAPPING drvMapId);

/**
* Function Name:    RscHalToggleePCBTimeOut
* Description:      RscHalToggleePCBTimeOut enables ePCB time out interrupt
*                   Only DRV0 can perform this operation (Other DRVs are read only)
* Input:            RSCSW_DRV_MAPPING   DrvId
*                   Status to be ENABLE/DISABLE
* Return:           TRUE if success ow error code 
**/

int32 RscHalToggleePCBTimeOut (RSCSW_DRV_MAPPING DrvId, boolean status);

/**
* Function Name:    RscHalUpdateePCBTimeOutThreshold
* Description:      RscHalUpdateePCBTimeOutThreshold updates threshold value for ePCB time out.
*                   The unit is rsc_clk clock cycle which driven by CXO; Only DRV0 can perform this operation 
*                   (Other DRVs read only)
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   Threshold value
* Return:           TRUE if success ow error code 
**/

int32 RscHalUpdateePCBTimeOutThreshold (RSCSW_DRV_MAPPING DrvId, uint16 threshold);

/**
* Function Name:    RscHalTriggerTCS 
* Description:      RscHalTriggerTCS triggers controller in AMC mode 
* Input:            RSCSW_DRV_MAPPING 
*                   TCSId to be triggered
* Return:           TRUE if success ow error code 
**/

int32 RscHalTriggerTCS (RSCSW_DRV_MAPPING DRVId, uint8 TCSId);

/**
* Function Name:    RscHalConvertTCStoAMC 
* Description:      RscHalConvertTCStoAMC converts a TCS into AMC. 
*                   RPMh driver should confirm if TCS is free before converting it to AMC 
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId to be triggered
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalConvertTCStoAMC (RSCSW_DRV_MAPPING DRVId, uint8 TCSId);

/**
* Function Name:    RscHalConvertAMCtoTCS 
* Description:      RscHalConvertAMCtoTCS converts a AMC back into TCS. 
*                   RPMh driver should confirm if AMC is free before converting it to TCS 
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId to be triggered
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalConvertAMCtoTCS (RSCSW_DRV_MAPPING DRVId, uint8 TCSId);

/**
* Function Name:    RscHalIsTCSIdle
* Description:      RscHalIsTCSIdle returns if a TCS is Idle. This is a polling call
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId 
*                   bool * to get the status, TRUE/FALSE
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalIsTCSIdle (RSCSW_DRV_MAPPING DRVId,uint8 TCSId, boolean *IsTCSIdle);

/**
* Function Name:    RscHalToggleTCSCmd
* Description:      RscHalToggleTCSCmd can be used to enable/disable specific commands in a TCS
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId
*                   CmdIdMask: 32-bit CommandMask to identify commands to be enabled/disabled.One bit per command
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalToggleTCSCmd (RSCSW_DRV_MAPPING DRVId, uint8 TCSId, uint32 CmdIdMask);

/**
* Function Name:    RscHalSetupTCS
* Description:      RscHalSetupTCS is used to setup commands on any given TCS
* Input:            RscHalMessage pointer
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalSetupTCS (RscHalMessage *);

/**
* Function Name:    RscHalEnableAMCFinishedIRQ
* Description:      RscHalEnableAMCFinishedIRQ is used to enable completion/finish interrupt on a  TCS/AMC.
*                   Interrupt is generated once per TCS/AMC—e.g. if there are 3 AMCs triggered and all three have completion interrupt enabled, 
*                   there would be 3 interrupts
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalEnableAMCFinishedIRQ (RSCSW_DRV_MAPPING DRVId, uint32 TCSId);

/**
* Function Name:    RscHalCheckTCSCmdTriggerStatus 
* Description:      RscHalCheckTCSCmdTriggerStatus is used to check if a command in TCS was successfully triggered.
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId
*                   Command Id
*                   bool *Status to get TRUE/FALSE 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckTCSCmdTriggerStatus (RSCSW_DRV_MAPPING DRVId, uint8 TCSId, uint8 CmdId, boolean *Status);

/**
* Function Name:    RscHalCheckTCSCmdIssueStatus   
* Description:      RscHalCheckTCSCmdIssueStatus is used to check if a command in TCS was successfully Issued.
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId
*                   Command Id
*                   bool *Status to get TRUE/FALSE 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckTCSCmdIssueStatus (RSCSW_DRV_MAPPING DRVId, uint8 TCSId, uint8 CmdId, boolean *Status);

/**
* Function Name:    RscHalCheckTCSCmdCompletionStatus
* Description:      RscHalCheckTCSCmdCompletionStatus is used to check if a command in TCS was successfully completed.
*                   This can not be done for fire-n-forget requests and can only be done for completion requests.
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId
*                   Command Id
*                   bool *Status to get TRUE/FALSE 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckTCSCmdCompletionStatus (RSCSW_DRV_MAPPING DRVId, uint8 TCSId, uint8 CmdId, boolean *Status);

/**
* Function Name:    RscHalReadResponseData 
* Description:      RscHalReadResponseData is used to read response data from RPMh HW for a read command sent by RPMh driver
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId 
*                   Command Id
*                   uint32 * to be filled with returned data from RPMh HW
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalReadResponseData (RSCSW_DRV_MAPPING DRVId, uint8 TCSId, uint8 CmdId, uint32 *DataReturned);

/**
* Function Name:    RscHalCheckAMCFinishedIRQ 
* Description:      RscHalCheckAMCFinishedIRQ will return TCS/AMC Completion IRQ status
*                   Status will provide 32 bit value for all the completed TCSs. 1-bit per TCS
* Input:            RSCSW_DRV_MAPPING
*                   uint *Status will return completed TCSs
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckAMCFinishedIRQ (RSCSW_DRV_MAPPING DRVId, uint32 *Status);

/**
* Function Name:    RscHalClearAMCFinishedIRQ 
* Description:      RscHalClearAMCFinishedIRQ will clear TCS/AMC Completion IRQ status
*                   The IRQ is cleared per AMC.So, IRQ will stay pending if only one AMC is cleared and interrupt is pending for multiple AMCs. 
*                   Because of this, SW does not need to handle all completed AMCs in a single IRQ handler invocation
* Input:            RSCSW_DRV_MAPPING
*                   TCS Id
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/


int32 RscHalClearAMCFinishedIRQ (RSCSW_DRV_MAPPING drvMapId,uint32 tcsId);


/**
* Function Name:    RscHalConfigureCmdCompletion 
* Description:      RscHalConfigureCmdCompletion will make TCS/AMC HW wait untill a specific command is "completed"
* Input:            RSCSW_DRV_MAPPING  DrvId
*                   TCS Id
*                   CmdMaskId, 32 bit value, to be completed before moving to next command
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/


int32 RscHalConfigureCmdCompletion (RSCSW_DRV_MAPPING drvMapId,uint8 tcsId, uint32 cmdMaskId);

int32 RscHalClearTcsControl(RSCSW_DRV_MAPPING drvMapId, uint8 tcsId);

int32 RscHalClearTcsCmdEnable(RSCSW_DRV_MAPPING drvMapId, uint8 tcsId);
#endif

