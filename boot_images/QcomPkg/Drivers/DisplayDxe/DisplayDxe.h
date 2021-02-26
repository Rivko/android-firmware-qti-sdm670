/*=============================================================================
 
  File: DisplayDxe.h
 
  Display driver for UEFI
  
 
    Copyright (c) 2011-2017 Qualcomm Technologies, Inc.
    All Rights Reserved
    Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __DISPLAYDXE_H__
#define __DISPLAYDXE_H__
  
#include <Uefi.h>
#include "MDPLib.h"
#include "MemRegionInfo.h"

/**********************************************************************************************************************
 * Defines
 **/

#define GRAPHICS_OUTPUT_PROTOCOL_REVISION  0x0001000

#define DISPLAYDXE_MAX_MODES               0x1

#define DISPLAYDXE_DEFAULT_MDP_FORMAT      MDP_PIXEL_FORMAT_ARGB_8888_32BPP

#define DISPLAYDXE_DEFAULT_PIXEL_FORMAT    PixelBlueGreenRedReserved8BitPerColor
#define DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL 4
#define DISPLAYDXE_RED_MASK                0xFF0000
#define DISPLAYDXE_GREEN_MASK              0x00FF00
#define DISPLAYDXE_BLUE_MASK               0x0000FF
#define DISPLAYDXE_ALPHA_MASK              0x000000


/**********************************************************************************************************************
 * Macros
 **/
#define DISPLAYDXE_PHYSICALADDRESS32(_x_) (UINTN)((_x_) & 0xFFFFFFFF)

/**********************************************************************************************************************
 *   Function Prototypes.
 **/


// EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE Handler
EFI_STATUS
DisplayDxe_SetMode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  UINT32                       ModeNumber);

  
// EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE Handler
EFI_STATUS
DisplayDxe_QueryMode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  UINT32                                ModeNumber,
  UINTN                                 *SizeOfInfo,
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info);


// EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT  Handler
EFI_STATUS
DisplayDxe_Blt (
    IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
    IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer,
    IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
    IN  UINTN                                   SourceX,
    IN  UINTN                                   SourceY,
    IN  UINTN                                   DestinationX,
    IN  UINTN                                   DestinationY,
    IN  UINTN                                   Width,
    IN  UINTN                                   Height,
    IN  UINTN                                   Delta
        );

//  Memory map update
EFI_STATUS
DisplayDxeMemmapUpdate (void);

// Register LPM Call back 
EFI_STATUS DisplayPwr_InitLPMSupport(BOOLEAN InitLpm);

/**********************************************************************************************************************
 *   Structures
 **/


/* EFI_DISPLAY_DEVICE_PATH */
typedef struct {
  VENDOR_DEVICE_PATH        DisplayDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  EndDevicePath;
} EFI_DISPLAY_DEVICE_PATH;



/* DisplayDxe_CurrentMode */
typedef struct 
{
    BOOLEAN                               bModeSet;         /* Indicants if a mode has been configured yet */
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE     sProtocolInfo;    /* Information returned from EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE */
    UINT32                                uModeId;          /* indicates the current mode */ 
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  sCurrentModeInfo; /* Structure information about each mode */
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  sModeList[MDP_DISPLAY_MAX][DISPLAYDXE_MAX_MODES];  /* List of all of the supported modes */
    UINT32                                uNumModes[MDP_DISPLAY_MAX];                        /* Total number of supported modes per display */
    UINT32                                uSelectedModeIndex[MDP_DISPLAY_MAX];               /* Selected Mode Index */
    BOOLEAN                               bHDMIDisabled;    /* Is HDMI supported */
    EFI_EVENT                             UIActiveEvent;    /* Event handle for UI_Active */
    EFI_EVENT                             UIIdleEvent;      /* Event handle for UI_Idle */
    UINT32                                uExitFlags;       /* Flags to be used while shutting down */
    ARM_MEMORY_REGION_ATTRIBUTES          eDisplayFBAttrib; /* Display frame buffer memory attributes */
} DisplayDxe_ModeInfo;


#endif // __DISPLAYDXE_H__
