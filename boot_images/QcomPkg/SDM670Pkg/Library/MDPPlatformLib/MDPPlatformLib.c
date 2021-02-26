/*=============================================================================
 
  File: MDPPlatformLib.c
 
  MDP platform specific functions
  
  Copyright (c) 2011-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "DDIChipInfo.h"
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFITlmm.h>
#include <Protocol/EFIPmicLpg.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFIPmicVreg.h>
#include <Protocol/EFIPmicWled.h>
#include <Protocol/EFIPmicLcdb.h>
#include <Protocol/EFIPmicOledb.h>
#include <Protocol/EFIPmicLab.h>
#include <Protocol/EFIPmicVersion.h>
#include "MDPTypes.h"
#include "MDPPlatformLib.h"
#include "MDPSystem.h"
#include "DisplayUtils.h"
#include "npa.h"
#include "pmapp_npa.h"
#include "DDITlmm.h"
#include "HALDSILib.h"

/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */

#define DP_PLUG_INVERTED_POLARITY         FALSE   // set to TRUE if USB type-c cable is plugged in reverse

#define PCD_PANEL_TYPE_OEM                1

#define DSI_READ_ADDRESS_SIZE             2
#define DSI_READ_READBACK_SIZE            8
#define DSI_WRITE_BACKLIGHT_SIZE          2

#define PLATFORM_PANEL_ID_MAX_COMMANDS    3       // maximum panel ID read commands

#define PANEL_LIST_MAX_COUNT              16      // maximum number of panels supported

// PMIC Device Indices
#define PMIC_DEV_INDEX                    0       // PMIC device (Vreg, LDO, ect)
#define PMIC_PMI_DEV_INDEX                1       // PMIC interface device (LCD / OLED)

/* SDM670 Main Panel CDP specific defines
*/
#define CDP_DISP_RESET_GPIO               75      //GPIO number for LCD0_RESET
#define CDP_DISP_TE_GPIO                  10      //GPIO number for TE
#define CDP_DISP_MODE_SEL                 76      //GPIO for panel mode select 0 = Dual Port, 1 = Single Port (DSC)

#define CDP_DISP_SXR1120_RESET_GPIO       93      //GPIO number for LCD0_RESET for SXR1120
#define CDP_DISP_SXR1120_ENABLE_GPIO      121     //GPIO to enable SXR1120 platform


#define CDP_DISP_LT9611_RESET_GPIO        95      //GPIO to reset LT9611
#define CDP_DISP_LT9611_ENABLE_GPIO       53      //GPIO to enable 3.3V ldo for LT9611

/* Helper macro for supported panels table entry PanelInfoType */
#define PANEL_CREATE_ENTRY(_name_,_id_,_dtlable_,_interface_,_xml_,_cfg_,_timing_,_pll_, _top0_, _top1_, _top2_)     \
                                          {(_name_),                            \
                                           (_id_),                              \
                                           (_dtlable_),                         \
                                           (_interface_),                       \
                                           (int8*)(_xml_),                      \
                                           (NULL == (_xml_)?0:sizeof((_xml_))), \
                                           (_cfg_),                             \
                                           (_timing_),                          \
                                           (_pll_),                             \
                                           {(_top0_), (_top1_), (_top2_)}}

#define PANEL_INFOLIST_LENGTH(_list_)               (sizeof(_list_)/sizeof(_list_[0]))

/* Macro to do round up of (x / y) to nearest integer */
#define DISP_DIV_ROUND_UP(x, y)                     (((x) + (y/2)) / (y))

#define EFICHIPINFO_FAMILY_QCS605_LC_VARIANT        4
#define EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA        6
#define EFICHIPINFO_FAMILY_QCS605_HP_VARIANT        7
#define EFICHIPINFO_FAMILY_QCS605_INTERNAL_DEVICE   0

//cmdline for LT9611 to enable cont_splash
#define CMDLINE_LT9611_CONT_SPLASH_ENABLE           "lt9611.cont_splash_en=1" 
//cmdline for LT9611 to disable cont_splash
#define CMDLINE_LT9611_CONT_SPLASH_DISABLE          "lt9611.cont_splash_en=0"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* Panel backlight DCS command
*/
typedef struct {
  uint8      command[DSI_WRITE_BACKLIGHT_SIZE];                                       // DCS command for backlight control
} PlatformPanelBacklightControl;

/* Panel command ID list for dynamic panel detection
*/

typedef struct {
  uint8      address[DSI_READ_ADDRESS_SIZE];                                          // DCS command for panel ID
  uint8      expectedReadback[DSI_READ_READBACK_SIZE];                                // expected readback
} PlatformPanelIDCommandInfo;

/* Dynamic panel ID detection entry 
 */
typedef struct {
  uint8                         uCmdType;                                             // data type for panel ID DSI read
  uint32                        uTotalRetry;                                          // number of retry if DSI read fails
  PlatformPanelIDCommandInfo    panelIdCommands[PLATFORM_PANEL_ID_MAX_COMMANDS];      // commands for panel ID and expected readback
  uint32                        uLaneRemapOrder;                                      // Lane remap order
  const int8                   *psPanelCfg;                                           // pointer to the panel configuration
  uint32                        uPanelCfgSize;                                        // size of panel configuration data
  MDPPlatformPanelType          eSelectedPanel;                                       // index of the selected panel
  uint32                        uFlags;                                               // flags to set clock config for now, can set other config in future
} PlatformDSIDetectParams;

/* Power configuration 
 */
typedef struct
{
  npa_client_handle             sNPAClient[MDP_DISPLAY_MAX];                          // NPA client handles for power control of panels.
  npa_client_handle             sTouchNPAClient;                          			  // NPA client handles for power control of Touch.
  MDP_PmicModuleControlType     ePMICSecondaryPower[MDP_DISPLAY_MAX];                 // Configuration for PMIC based secondary power source
  uint32                        uResetGpio;
  uint32                        uWledStringsPrimary;                                  // Set of WLED strings needed for this platform
  uint32                        bOledPostInitDone;                                    // OLED post init is done

} Panel_PowerCtrlParams;

/* NPA Client info
*/
typedef struct
{
  char*                         cResourceName;
  char*                         cClientName;
}NPAClientName;


/* Panel support map type
*/
typedef struct
{
  MDPPlatformPanelType     ePanel;          /* Panel ID */  
  int8                    *pPanelXMLConfig; /* Matching XML configuration */
  uint32                   uConfigSize;     /* Size of XML configuration */
  uint32                   uOverrideFlags;  /* Panel config flags from DisplayOverideFlagType */
} PanelMappingType;


/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

/* Platform detection 
*/
static MDP_Status ReadPlatformIDAndChipID(EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *pPlatformInfo, EFIChipInfoIdType *pChipSetId, EFIChipInfoFamilyType *pChiSetFamily);
static MDP_Status DynamicDSIPanelDetection(MDPPlatformParams *pPlatformParams, uint32 *puPanelID, uint32 uOverrideId, PlatformDSIDetectParams *pPanelList, uint32 uPanelCnt);


/* SDM670 Main Panel CDP Functions
*/
static MDP_Status Panel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo* pPlatformInfo);
static MDP_Status Panel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo *pPlatformInfo);
static MDP_Status Panel_CDP_Reset(MDPPlatformPanelResetInfo *pResetInfo);
static MDP_Status Panel_CDP_BacklightLevel(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, BacklightConfigType *pBacklightConfig);
static MDP_Status Panel_CDP_PeripheralPower(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, bool32 bPowerUp);
static MDP_Status Panel_CDP_PmicOledPostInit(MDP_Display_IDType eDisplayId);


/* Main Panel CDP/MTP Functions
*/

/* SDM670 / QCS605 HDMI Panel Functions
*/
static MDP_Status ExternalPanel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo* pPlatformInfo);
static MDP_Status ExternalPanel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams);


/* DP Functions */
static bool32 DP_USBPlugInvertedOrientation (void);

/* General helper functions */
MDP_Status        SetDefaultGPIOState(GpioStateType *pGPIOList);

/*===========================================================================
 Local Configuration Definitions
 ===========================================================================*/
/******************************************************************
 *                   Sharp 5.5" 4k Panel                            *
 *                                                                *
 *                   nt35950 DDIC                                 *
 *                   2160x3840                                    *
 *                   Dual-DSI(DSI0 and DSI1)                      *
 *                   Video mode DSC                               *
 ******************************************************************/
const static int8 Sharp_4k_video_dsc_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>LS055D1SX04</PanelName>"
"<PanelDescription>Sharp Dual DSI Video Mode Panel with DSC (2160x3840 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>2160</HorizontalActive>"
" <HorizontalFrontPorch>30</HorizontalFrontPorch>"
" <HorizontalBackPorch>100</HorizontalBackPorch>"
" <HorizontalSyncPulse>4</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>3840</VerticalActive>"
" <VerticalFrontPorch>8</VerticalFrontPorch>"
" <VerticalBackPorch>7</VerticalBackPorch>"
" <VerticalSyncPulse>1</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>8</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>1</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSIDSCEnable>True</DSIDSCEnable>"
" <DSIDSCMajor>1</DSIDSCMajor>"
" <DSIDSCMinor>1</DSIDSCMinor>"
" <DSIDSCScr>0</DSIDSCScr>"
" <DSIDSCProfileID>4</DSIDSCProfileID>"
" <DSIDSCSliceWidth>1080</DSIDSCSliceWidth>"
" <DSIDSCSliceHeight>32</DSIDSCSliceHeight>"
" <DSILanes>4</DSILanes>"
  " <DSIHsaHseAfterVsVe>False</DSIHsaHseAfterVsVe>\n"
  " <DSILowPowerModeInHFP>False</DSILowPowerModeInHFP>\n"
  " <DSILowPowerModeInHBP>False</DSILowPowerModeInHBP>\n"
  " <DSILowPowerModeInHSA>False</DSILowPowerModeInHSA>\n"
  " <DSILowPowerModeInBLLPEOF>True</DSILowPowerModeInBLLPEOF>\n"
  " <DSILowPowerModeInBLLP>True</DSILowPowerModeInBLLP>\n"
  " <DSIRefreshRate>0x3C0000</DSIRefreshRate>\n"
  " <DSIPhyDCDCMode>True</DSIPhyDCDCMode>\n"
" <DSIControllerMapping>\n"
"  00 01\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" 39 91 09 20 00 20 02 00 03 1c 04 21 00 0f 03 19 01 97\n"
" 39 92 10 f0\n"
" 15 90 03\n"
" 15 03 01\n"
" 39 f0 55 aa 52 08 04\n"
" 15 c0 03\n"
" 39 f0 55 aa 52 08 07\n"
" 15 ef 01\n"
" 39 f0 55 aa 52 08 00\n"
" 15 b4 10\n"  /* DSC */
" 15 35 00\n"
" 39 f0 55 aa 52 08 01\n"
" 39 ff aa 55 a5 80\n"
" 15 6f 01\n"
" 15 f3 10\n"
" 39 ff aa 55 a5 00\n"
" 05 11\n"
" ff 78\n"
" 05 29\n"
" ff 78\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultLow>4C</TLMMGPIODefaultLow>"
"<Group id='Backlight Configuration'>"
" <BacklightType>1</BacklightType>\n"
" <BacklightPmicControlType>2</BacklightPmicControlType>\n"  
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
"</Group>\n";

/******************************************************************
 *                   Sharp 5.5" 4k Panel                            *
 *                                                                *
 *                   nt35950 DDIC                                 *
 *                   2160x3840                                    *
 *                   Dual-DSI(DSI0 and DSI1)                      *
 *                   Command mode DSC                             *
 ******************************************************************/
const static int8 Sharp_4k_cmd_dsc_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>LS055D1SX04</PanelName>"
"<PanelDescription>Sharp Dual DSI Command Mode DSC Panel (2160x3840 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>2160</HorizontalActive>"
" <HorizontalFrontPorch>30</HorizontalFrontPorch>"
" <HorizontalBackPorch>100</HorizontalBackPorch>"
" <HorizontalSyncPulse>4</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>3840</VerticalActive>"
" <VerticalFrontPorch>8</VerticalFrontPorch>"
" <VerticalBackPorch>7</VerticalBackPorch>"
" <VerticalSyncPulse>1</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>2</DSIAutoRefreshFrameNumDiv>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>3840</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>3840</DSITEvSyncInitVal>"
" <DSIDSCEnable>True</DSIDSCEnable>"
" <DSIDSCMajor>1</DSIDSCMajor>"
" <DSIDSCMinor>1</DSIDSCMinor>"
" <DSIDSCScr>0</DSIDSCScr>"
" <DSIDSCProfileID>4</DSIDSCProfileID>"
" <DSICmdModePixelAlignment>1</DSICmdModePixelAlignment>"
" <DSIDSCSliceWidth>1080</DSIDSCSliceWidth>"
" <DSIDSCSliceHeight>32</DSIDSCSliceHeight>"
" <DSIControllerMapping>\n"
"  00 01\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" 39 91 09 20 00 20 02 00 03 1c 04 21 00 0f 03 19 01 97\n"
" 39 92 10 f0\n"
" 15 90 03\n"
" 15 03 01\n"
" 39 f0 55 aa 52 08 04\n"
" 15 c0 03\n"
" 39 f0 55 aa 52 08 07\n"
" 15 ef 01\n"
" 39 f0 55 aa 52 08 00\n"
" 15 b4 01\n"
" 15 35 00\n"
" 39 f0 55 aa 52 08 01\n"
" 39 ff aa 55 a5 80\n"
" 15 6f 01\n"
" 15 f3 10\n"
" 39 ff aa 55 a5 00\n"
" 05 11\n"
" ff 78\n"
" 05 29\n"
" ff 78\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultLow>4C</TLMMGPIODefaultLow>"
"<Group id='Backlight Configuration'>"
" <BacklightType>1</BacklightType>\n"
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
" <BacklightPmicControlType>2</BacklightPmicControlType>\n"  
"</Group>\n";

/******************************************************************
 *           Truly 5.5in 2k  TFT2P2827-E                          *
 *                                                                *
 *                  nt35597 DDIC                                  *
 *                   1440x2560                                    *
 *                   Single DSI                                   *
 *                   DSC Command mode                             *
 ******************************************************************/
const static int8 Truly_TFT2P2827_E_cmd_dsc_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>TFT2P2827-E</PanelName>"
"<PanelDescription>Truly Single DSI Cmd Mode Panel with DSC (1440x2560 24bpp)</PanelDescription>"
"<PanelId>0x00008012</PanelId>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1440</HorizontalActive>"
" <HorizontalFrontPorch>100</HorizontalFrontPorch>"
" <HorizontalBackPorch>32</HorizontalBackPorch>"
" <HorizontalSyncPulse>16</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>2560</VerticalActive>"
" <VerticalFrontPorch>8</VerticalFrontPorch>"
" <VerticalBackPorch>7</VerticalBackPorch>"
" <VerticalSyncPulse>1</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>2</DSIAutoRefreshFrameNumDiv>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>2560</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>2560</DSITEvSyncInitVal>"
" <DSIDSCEnable>True</DSIDSCEnable>"
" <DSIDSCMajor>1</DSIDSCMajor>"
" <DSIDSCMinor>1</DSIDSCMinor>"
" <DSIDSCScr>0</DSIDSCScr>"
" <DSIDSCProfileID>7</DSIDSCProfileID>"
" <DSIDSCSliceWidth>720</DSIDSCSliceWidth>"
" <DSIDSCSliceHeight>16</DSIDSCSliceHeight>"
" <DSIControllerMapping>\n"
"  01\n"
" </DSIControllerMapping>\n"
"</Group>"
"<DSIInitSequence>"
" FF 20\n"
" 15 FF 20\n"
" 15 fb 01\n"
" 15 00 01\n"
" 15 01 55\n"
" 15 02 45\n"
" 15 05 40\n"
" 15 06 19\n"
" 15 07 1E\n"
" 15 0B 73\n"
" 15 0C 73\n"
" 15 0E B0\n"
" 15 0F AE\n"
" 15 11 B8\n"
" 15 13 00\n"
" 15 58 80\n"
" 15 59 01\n"
" 15 5A 00\n"
" 15 5B 01\n"
" 15 5C 80\n"
" 15 5D 81\n"
" 15 5E 00\n"
" 15 5F 01\n"
" 15 72 11\n"
" 15 68 03\n"
" 15 ff 24\n"
" 15 fb 01\n"
" 15 00 1C\n"
" 15 01 0B\n"
" 15 02 0C\n"
" 15 03 01\n"
" 15 04 0F\n"
" 15 05 10\n"
" 15 06 10\n"
" 15 07 10\n"
" 15 08 89\n"
" 15 09 8A\n"
" 15 0A 13\n"
" 15 0B 13\n"
" 15 0C 15\n"
" 15 0D 15\n"
" 15 0E 17\n"
" 15 0F 17\n"
" 15 10 1C\n"
" 15 11 0B\n"
" 15 12 0C\n"
" 15 13 01\n"
" 15 14 0F\n"
" 15 15 10\n"
" 15 16 10\n"
" 15 17 10\n"
" 15 18 89\n"
" 15 19 8A\n"
" 15 1A 13\n"
" 15 1B 13\n"
" 15 1C 15\n"
" 15 1D 15\n"
" 15 1E 17\n"
" 15 1F 17\n"
" 15 20 40\n"
" 15 21 01\n"
" 15 22 00\n"
" 15 23 40\n"
" 15 24 40\n"
" 15 25 6D\n"
" 15 26 40\n"
" 15 27 40\n"
" 15 E0 00\n"
" 15 DC 21\n"
" 15 DD 22\n"
" 15 DE 07\n"
" 15 DF 07\n"
" 15 E3 6D\n"
" 15 E1 07\n"
" 15 E2 07\n"
" 15 29 D8\n"
" 15 2A 2A\n"
" 15 4B 03\n"
" 15 4C 11\n"
" 15 4D 10\n"
" 15 4E 01\n"
" 15 4F 01\n"
" 15 50 10\n"
" 15 51 00\n"
" 15 52 80\n"
" 15 53 00\n"
" 15 56 00\n"
" 15 54 07\n"
" 15 58 07\n"
" 15 55 25\n"
" 15 5B 43\n"
" 15 5C 00\n"
" 15 5F 73\n"
" 15 60 73\n"
" 15 63 22\n"
" 15 64 00\n"
" 15 67 08\n"
" 15 68 04\n"
" 15 72 02\n"
" 15 7A 80\n"
" 15 7B 91\n"
" 15 7C D8\n"
" 15 7D 60\n"
" 15 7F 15\n"
" 15 75 15\n"
" 15 B3 C0\n"
" 15 B4 00\n"
" 15 B5 00\n"
" 15 78 00\n"
" 15 79 00\n"
" 15 80 00\n"
" 15 83 00\n"
" 15 93 0A\n"
" 15 94 0A\n"
" 15 8A 00\n"
" 15 9B FF\n"
" 15 9D B0\n"
" 15 9F 63\n"
" 15 98 10\n"
" 15 EC 00\n"
" 15 FF 10\n"
" 15 fb 01\n"
" 15 ba 03\n"
" 15 e5 01\n"
" 15 b0 03\n"
" 15 ff 28\n"
" 15 7a 02\n"
" 15 fb 01\n"
" 15 ff 10\n"
" 15 fb 01\n"
" 15 c0 03\n"
" 15 bb 10\n"
" 15 ff e0\n"
" 15 fb 01\n"
" 15 6b 3d\n"
" 15 6c 3d\n"
" 15 6d 3d\n"
" 15 6e 3d\n"
" 15 6f 3d\n"
" 15 35 02\n"
" 15 36 72\n"
" 15 37 10\n"
" 15 08 c0\n"
" 15 ff 24\n"
" 15 fb 01\n"
" 15 c6 06\n"
" 15 ff 10\n"
" 15 35 00\n"
" 15 E5 01\n"
" 15 BB 10\n"
" 05 11\n"
" ff 64\n"
" 05 29\n"
" ff 28\n"
" 07 01\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultHigh>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>1</BacklightType>\n"
" <BacklightPmicControlType>2</BacklightPmicControlType>\n"
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
"</Group>\n";


/******************************************************************
 *           Truly 5.5in 2k  TFT2P2827-E                          *
 *                                                                *
 *                  nt35597 DDIC                                  *
 *                   1440x2560                                    *
 *                   Single DSI                                   *
 *                   DSC Video mode                               *
 ******************************************************************/
const static int8 Truly_TFT2P2827_E_video_dsc_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>TFT2P2827-E</PanelName>"
"<PanelDescription>Truly Single DSI Video Mode Panel with DSC (1440x2560 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive units=\"Dot Clocks\">1440</HorizontalActive>"
" <HorizontalFrontPorch units=\"Dot Clocks\">100</HorizontalFrontPorch>"
" <HorizontalBackPorch units=\"Dot Clocks\">32</HorizontalBackPorch>"
" <HorizontalSyncPulse units=\"Dot Clocks\">16</HorizontalSyncPulse>"
" <HorizontalSyncSkew units=\"Dot Clocks\">0</HorizontalSyncSkew>"
" <HorizontalLeftBorder units=\"Dot Clocks\">0</HorizontalLeftBorder>"
" <HorizontalRightBorder units=\"Dot Clocks\">0</HorizontalRightBorder>"
" <VerticalActive units=\"Dot Clocks\">2560</VerticalActive>"
" <VerticalFrontPorch units=\"Lines\">10</VerticalFrontPorch>"
" <VerticalBackPorch units=\"Lines\">8</VerticalBackPorch>"
" <VerticalSyncPulse units=\"Lines\">2</VerticalSyncPulse>"
" <VerticalSyncSkew units=\"Lines\">0</VerticalSyncSkew>"
" <VerticalTopBorder units=\"Lines\">0</VerticalTopBorder>"
" <VerticalBottomBorder units=\"Lines\">0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType units=\"QDI_DisplayConnectType\">8</InterfaceType>"
" <InterfaceColorFormat units=\"QDI_PixelFormatType\">3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId units=\"DSI_Channel_IDType\">1</DSIChannelId>"
" <DSIVirtualId units=\"DSI_Display_VCType\">0</DSIVirtualId>"
" <DSIColorFormat units=\"DSI_ColorFormatType\">36</DSIColorFormat>"
" <DSITrafficMode units=\"DSI_TrafficModeType\">1</DSITrafficMode>"
" <DSIDSCEnable>True</DSIDSCEnable>"
" <DSIDSCMajor>1</DSIDSCMajor>"
" <DSIDSCMinor>1</DSIDSCMinor>"
" <DSIDSCScr>0</DSIDSCScr>"
" <DSIDSCProfileID>7</DSIDSCProfileID>"
" <DSIDSCSliceWidth>720</DSIDSCSliceWidth>"
" <DSIDSCSliceHeight>16</DSIDSCSliceHeight>"
" <DSILanes units=\"integer\">4</DSILanes>"
  " <DSIHsaHseAfterVsVe units='Bool'>False</DSIHsaHseAfterVsVe>\n"
  " <DSILowPowerModeInHFP units='Bool'>False</DSILowPowerModeInHFP>\n"
  " <DSILowPowerModeInHBP units='Bool'>False</DSILowPowerModeInHBP>\n"
  " <DSILowPowerModeInHSA units='Bool'>False</DSILowPowerModeInHSA>\n"
  " <DSILowPowerModeInBLLPEOF units='Bool'>True</DSILowPowerModeInBLLPEOF>\n"
  " <DSILowPowerModeInBLLP units='Bool'>True</DSILowPowerModeInBLLP>\n"
  " <DSIRefreshRate units='integer Q16.16'>0x3C0000</DSIRefreshRate>\n"
  " <DSIPhyDCDCMode units='Bool'>True</DSIPhyDCDCMode>\n"
" <DSIControllerMapping>\n"
"  01\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" FF 20\n"
" 15 FF 20\n"
" 15 FB 01\n"
" 15 00 01\n"
" 15 01 55\n"
" 15 02 45\n"
" 15 05 40\n"
" 15 06 19\n"
" 15 07 1E\n"
" 15 0B 73\n"
" 15 0C 73\n"
" 15 0E B0\n"
" 15 0F AE\n"
" 15 11 B8\n"
" 15 13 00\n"
" 15 58 80\n"
" 15 59 01\n"
" 15 5A 00\n"
" 15 5B 01\n"
" 15 5C 80\n"
" 15 5D 81\n"
" 15 5E 00\n"
" 15 5F 01\n"
" 15 72 11\n"
" 15 68 03\n"
" 15 FF 24\n"
" 15 FB 01\n"
" 15 00 1C\n"
" 15 01 0B\n"
" 15 02 0C\n"
" 15 03 01\n"
" 15 04 0F\n"
" 15 05 10\n"
" 15 06 10\n"
" 15 07 10\n"
" 15 08 89\n"
" 15 09 8A\n"
" 15 0A 13\n"
" 15 0B 13\n"
" 15 0C 15\n"
" 15 0D 15\n"
" 15 0E 17\n"
" 15 0F 17\n"
" 15 10 1C\n"
" 15 11 0B\n"
" 15 12 0C\n"
" 15 13 01\n"
" 15 14 0F\n"
" 15 15 10\n"
" 15 16 10\n"
" 15 17 10\n"
" 15 18 89\n"
" 15 19 8A\n"
" 15 1A 13\n"
" 15 1B 13\n"
" 15 1C 15\n"
" 15 1D 15\n"
" 15 1E 17\n"
" 15 1F 17\n"
" 15 20 40\n"
" 15 21 01\n"
" 15 22 00\n"
" 15 23 40\n"
" 15 24 40\n"
" 15 25 6D\n"
" 15 26 40\n"
" 15 27 40\n"
" 15 E0 00\n"
" 15 DC 21\n"
" 15 DD 22\n"
" 15 DE 07\n"
" 15 DF 07\n"
" 15 E3 6D\n"
" 15 E1 07\n"
" 15 E2 07\n"
" 15 29 D8\n"
" 15 2A 2A\n"
" 15 4B 03\n"
" 15 4C 11\n"
" 15 4D 10\n"
" 15 4E 01\n"
" 15 4F 01\n"
" 15 50 10\n"
" 15 51 00\n"
" 15 52 80\n"
" 15 53 00\n"
" 15 56 00\n"
" 15 54 07\n"
" 15 58 07\n"
" 15 55 25\n"
" 15 5B 43\n"
" 15 5C 00\n"
" 15 5F 73\n"
" 15 60 73\n"
" 15 63 22\n"
" 15 64 00\n"
" 15 67 08\n"
" 15 68 04\n"
" 15 72 02\n"
" 15 7A 80\n"
" 15 7B 91\n"
" 15 7C D8\n"
" 15 7D 60\n"
" 15 7F 15\n"
" 15 75 15\n"
" 15 B3 C0\n"
" 15 B4 00\n"
" 15 B5 00\n"
" 15 78 00\n"
" 15 79 00\n"
" 15 80 00\n"
" 15 83 00\n"
" 15 93 0A\n"
" 15 94 0A\n"
" 15 8A 00\n"
" 15 9B FF\n"
" 15 9D B0\n"
" 15 9F 63\n"
" 15 98 10\n"
" 15 EC 00\n"
" 15 FF 10\n"
" 15 fb 01\n"
" 15 ba 03\n"
" 15 e5 01\n"
" 15 b0 03\n"
" 39 3B 03 0A 0A\n"
" 15 FF 28\n"
" 15 7a 02\n"
" 15 fb 01\n"
" 15 FF 10\n"
" 15 fb 01\n"
" 15 c0 03\n"
" 15 bb 03\n"
" 15 FF e0\n"
" 15 fb 01\n"
" 15 6b 3d\n"
" 15 6c 3d\n"
" 15 6d 3d\n"
" 15 6e 3d\n"
" 15 6f 3d\n"
" 15 35 02\n"
" 15 36 72\n"
" 15 37 10\n"
" 15 08 c0\n"
" 15 FF 10\n"
" 15 E5 01\n"
" 15 BB 03\n"
" 05 11 00\n"
" ff 64\n"
" 05 29\n"
" ff 28\n"
" 07 01\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultHigh units='hex list'>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType units='MDP_Panel_BacklightType'>1</BacklightType>\n"
" <BacklightPmicControlType units='MDP_PmicBacklightControlType'>2</BacklightPmicControlType>\n"  
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
"</Group>\n";

/******************************************************************
 *           Truly 5.5in 2k  TFT2P2827-E                          *
 *                                                                *
 *                  nt35597 DDIC                                  *
 *                   1440x2560                                    *
 *                   Dual-DSI                                     *
 *                   Command mode                                   *
 ******************************************************************/
const static int8 Truly_TFT2P2827_E_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>TFT2P2827-E</PanelName>"
"<PanelDescription>Truly Dual DSI Command Mode Panel (1440x2560 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1440</HorizontalActive>"
" <HorizontalFrontPorch>100</HorizontalFrontPorch>"
" <HorizontalBackPorch>32</HorizontalBackPorch>"
" <HorizontalSyncPulse>16</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>2560</VerticalActive>"
" <VerticalFrontPorch>8</VerticalFrontPorch>"
" <VerticalBackPorch>7</VerticalBackPorch>"
" <VerticalSyncPulse>1</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>2</DSIAutoRefreshFrameNumDiv>"
" <DSIClockHSForceRequest>1</DSIClockHSForceRequest>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>2560</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>2560</DSITEvSyncInitVal>"
" <DSICmdModePixelAlignment>1</DSICmdModePixelAlignment>"
" <DSIControllerMapping>\n"
"  00 01\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" FF 20\n"
" 15 FF 20\n"
" 15 fb 01\n"
" 15 00 01\n"
" 15 01 55\n"
" 15 02 45\n"
" 15 05 40\n"
" 15 06 19\n"
" 15 07 1E\n"
" 15 0B 73\n"
" 15 0C 73\n"
" 15 0E B0\n"
" 15 0F AE\n"
" 15 11 B8\n"
" 15 13 00\n"
" 15 58 80\n"
" 15 59 01\n"
" 15 5A 00\n"
" 15 5B 01\n"
" 15 5C 80\n"
" 15 5D 81\n"
" 15 5E 00\n"
" 15 5F 01\n"
" 15 72 11\n"
" 15 68 03\n"
" 15 ff 24\n"
" 15 fb 01\n"
" 15 00 1C\n"
" 15 01 0B\n"
" 15 02 0C\n"
" 15 03 01\n"
" 15 04 0F\n"
" 15 05 10\n"
" 15 06 10\n"
" 15 07 10\n"
" 15 08 89\n"
" 15 09 8A\n"
" 15 0A 13\n"
" 15 0B 13\n"
" 15 0C 15\n"
" 15 0D 15\n"
" 15 0E 17\n"
" 15 0F 17\n"
" 15 10 1C\n"
" 15 11 0B\n"
" 15 12 0C\n"
" 15 13 01\n"
" 15 14 0F\n"
" 15 15 10\n"
" 15 16 10\n"
" 15 17 10\n"
" 15 18 89\n"
" 15 19 8A\n"
" 15 1A 13\n"
" 15 1B 13\n"
" 15 1C 15\n"
" 15 1D 15\n"
" 15 1E 17\n"
" 15 1F 17\n"
" 15 20 40\n"
" 15 21 01\n"
" 15 22 00\n"
" 15 23 40\n"
" 15 24 40\n"
" 15 25 6D\n"
" 15 26 40\n"
" 15 27 40\n"
" 15 E0 00\n"
" 15 DC 21\n"
" 15 DD 22\n"
" 15 DE 07\n"
" 15 DF 07\n"
" 15 E3 6D\n"
" 15 E1 07\n"
" 15 E2 07\n"
" 15 29 D8\n"
" 15 2A 2A\n"
" 15 4B 03\n"
" 15 4C 11\n"
" 15 4D 10\n"
" 15 4E 01\n"
" 15 4F 01\n"
" 15 50 10\n"
" 15 51 00\n"
" 15 52 80\n"
" 15 53 00\n"
" 15 56 00\n"
" 15 54 07\n"
" 15 58 07\n"
" 15 55 25\n"
" 15 5B 43\n"
" 15 5C 00\n"
" 15 5F 73\n"
" 15 60 73\n"
" 15 63 22\n"
" 15 64 00\n"
" 15 67 08\n"
" 15 68 04\n"
" 15 72 02\n"
" 15 7A 80\n"
" 15 7B 91\n"
" 15 7C D8\n"
" 15 7D 60\n"
" 15 7F 15\n"
" 15 75 15\n"
" 15 B3 C0\n"
" 15 B4 00\n"
" 15 B5 00\n"
" 15 78 00\n"
" 15 79 00\n"
" 15 80 00\n"
" 15 83 00\n"
" 15 93 0A\n"
" 15 94 0A\n"
" 15 8A 00\n"
" 15 9B FF\n"
" 15 9D B0\n"
" 15 9F 63\n"
" 15 98 10\n"
" 15 EC 00\n"
" 15 ff 10\n"
" 15 35 00\n"
" 15 E5 01\n"
" 15 BB 10\n"
" 15 C0 00\n"
" 29 C9 01 01 70 00 0A 06 67 04 C5 12 18\n"
" 15 FB 01\n"
" 05 11 00\n"
" ff ff\n"
" 05 29 00\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultLow>4C</TLMMGPIODefaultLow>"
"<Group id='Backlight Configuration'>"
" <BacklightType>1</BacklightType>\n"
" <BacklightPmicControlType>2</BacklightPmicControlType>\n"  
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
"</Group>\n";


/******************************************************************
 *           Truly 5.5in 2k  TFT2P2827-E                          *
 *                                                                *
 *                  nt35597 DDIC                                  *
 *                   1440x2560                                    *
 *                   Dual-DSI                                     *
 *                   Video mode                                   *
 ******************************************************************/
const static int8 Truly_TFT2P2827_E_video_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>TFT2P2827-E</PanelName>"
"<PanelDescription>Truly Dual DSI Video Mode Panel (1440x2560 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive units=\"Dot Clocks\">1440</HorizontalActive>"
" <HorizontalFrontPorch units=\"Dot Clocks\">100</HorizontalFrontPorch>"
" <HorizontalBackPorch units=\"Dot Clocks\">32</HorizontalBackPorch>"
" <HorizontalSyncPulse units=\"Dot Clocks\">16</HorizontalSyncPulse>"
" <HorizontalSyncSkew units=\"Dot Clocks\">0</HorizontalSyncSkew>"
" <HorizontalLeftBorder units=\"Dot Clocks\">0</HorizontalLeftBorder>"
" <HorizontalRightBorder units=\"Dot Clocks\">0</HorizontalRightBorder>"
" <VerticalActive units=\"Dot Clocks\">2560</VerticalActive>"
" <VerticalFrontPorch units=\"Lines\">8</VerticalFrontPorch>"
" <VerticalBackPorch units=\"Lines\">7</VerticalBackPorch>"
" <VerticalSyncPulse units=\"Lines\">1</VerticalSyncPulse>"
" <VerticalSyncSkew units=\"Lines\">0</VerticalSyncSkew>"
" <VerticalTopBorder units=\"Lines\">0</VerticalTopBorder>"
" <VerticalBottomBorder units=\"Lines\">0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType units=\"QDI_DisplayConnectType\">8</InterfaceType>"
" <InterfaceColorFormat units=\"QDI_PixelFormatType\">3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId units=\"DSI_Channel_IDType\">1</DSIChannelId>"
" <DSIVirtualId units=\"DSI_Display_VCType\">0</DSIVirtualId>"
" <DSIColorFormat units=\"DSI_ColorFormatType\">36</DSIColorFormat>"
" <DSITrafficMode units=\"DSI_TrafficModeType\">1</DSITrafficMode>"
" <DSILanes units=\"integer\">4</DSILanes>"
  " <DSIHsaHseAfterVsVe units='Bool'>False</DSIHsaHseAfterVsVe>\n"
  " <DSILowPowerModeInHFP units='Bool'>False</DSILowPowerModeInHFP>\n"
  " <DSILowPowerModeInHBP units='Bool'>False</DSILowPowerModeInHBP>\n"
  " <DSILowPowerModeInHSA units='Bool'>False</DSILowPowerModeInHSA>\n"
  " <DSILowPowerModeInBLLPEOF units='Bool'>True</DSILowPowerModeInBLLPEOF>\n"
  " <DSILowPowerModeInBLLP units='Bool'>True</DSILowPowerModeInBLLP>\n"
  " <DSIRefreshRate units='integer Q16.16'>0x3C0000</DSIRefreshRate>\n"
  " <DSIDynamicRefreshRates units='integer Q16.16'>0x3B8000 0x3BC000 0x3C0000 0x3C4000 0x3C8000</DSIDynamicRefreshRates>\n"
  " <DSIPhyDCDCMode units='Bool'>True</DSIPhyDCDCMode>\n"
" <DSIControllerMapping>\n"
"  00 01\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" FF 20\n"
" 15 FF 20\n"
" 15 FB 01\n"
" 15 00 01\n"
" 15 01 55\n"
" 15 02 45\n"
" 15 05 40\n"
" 15 06 19\n"
" 15 07 1E\n"
" 15 0B 73\n"
" 15 0C 73\n"
" 15 0E B0\n"
" 15 0F AE\n"
" 15 11 B8\n"
" 15 13 00\n"
" 15 58 80\n"
" 15 59 01\n"
" 15 5A 00\n"
" 15 5B 01\n"
" 15 5C 80\n"
" 15 5D 81\n"
" 15 5E 00\n"
" 15 5F 01\n"
" 15 72 11\n"
" 15 68 03\n"
" 15 FF 24\n"
" 15 FB 01\n"
" 15 00 1C\n"
" 15 01 0B\n"
" 15 02 0C\n"
" 15 03 01\n"
" 15 04 0F\n"
" 15 05 10\n"
" 15 06 10\n"
" 15 07 10\n"
" 15 08 89\n"
" 15 09 8A\n"
" 15 0A 13\n"
" 15 0B 13\n"
" 15 0C 15\n"
" 15 0D 15\n"
" 15 0E 17\n"
" 15 0F 17\n"
" 15 10 1C\n"
" 15 11 0B\n"
" 15 12 0C\n"
" 15 13 01\n"
" 15 14 0F\n"
" 15 15 10\n"
" 15 16 10\n"
" 15 17 10\n"
" 15 18 89\n"
" 15 19 8A\n"
" 15 1A 13\n"
" 15 1B 13\n"
" 15 1C 15\n"
" 15 1D 15\n"
" 15 1E 17\n"
" 15 1F 17\n"
" 15 20 40\n"
" 15 21 01\n"
" 15 22 00\n"
" 15 23 40\n"
" 15 24 40\n"
" 15 25 6D\n"
" 15 26 40\n"
" 15 27 40\n"
" 15 E0 00\n"
" 15 DC 21\n"
" 15 DD 22\n"
" 15 DE 07\n"
" 15 DF 07\n"
" 15 E3 6D\n"
" 15 E1 07\n"
" 15 E2 07\n"
" 15 29 D8\n"
" 15 2A 2A\n"
" 15 4B 03\n"
" 15 4C 11\n"
" 15 4D 10\n"
" 15 4E 01\n"
" 15 4F 01\n"
" 15 50 10\n"
" 15 51 00\n"
" 15 52 80\n"
" 15 53 00\n"
" 15 56 00\n"
" 15 54 07\n"
" 15 58 07\n"
" 15 55 25\n"
" 15 5B 43\n"
" 15 5C 00\n"
" 15 5F 73\n"
" 15 60 73\n"
" 15 63 22\n"
" 15 64 00\n"
" 15 67 08\n"
" 15 68 04\n"
" 15 72 02\n"
" 15 7A 80\n"
" 15 7B 91\n"
" 15 7C D8\n"
" 15 7D 60\n"
" 15 7F 15\n"
" 15 75 15\n"
" 15 B3 C0\n"
" 15 B4 00\n"
" 15 B5 00\n"
" 15 78 00\n"
" 15 79 00\n"
" 15 80 00\n"
" 15 83 00\n"
" 15 93 0A\n"
" 15 94 0A\n"
" 15 8A 00\n"
" 15 9B FF\n"
" 15 9D B0\n"
" 15 9F 63\n"
" 15 98 10\n"
" 15 EC 00\n"
" 15 FF 10\n"
" 39 3B 03 0A 0A \n"
" 15 35 00\n"
" 15 E5 01\n"
" 15 BB 03\n"
" 15 FB 01\n"
" 05 11 00\n"
" ff 78\n"
" 05 29 00\n"
" ff 78\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultLow units='hex list'>4C</TLMMGPIODefaultLow>"
"<Group id='Backlight Configuration'>"
" <BacklightType units='MDP_Panel_BacklightType'>1</BacklightType>\n"
" <BacklightPmicControlType units='MDP_PmicBacklightControlType'>2</BacklightPmicControlType>\n"  
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
"</Group>\n";

/******************************************************************
 *           Sharp 4.95in                                         *
 *                                                                *
 *                   1080x1920                                    *
 *                   Single-DSI                                   *
 *                   Command mode                                 *
 ******************************************************************/
const static int8 Sharp_1080p_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelDescription>Sharp DSI Command Mode Panel (1080x1920 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1080</HorizontalActive>"
" <HorizontalFrontPorch>0</HorizontalFrontPorch>"
" <HorizontalBackPorch>0</HorizontalBackPorch>"
" <HorizontalSyncPulse>0</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>1920</VerticalActive>"
" <VerticalFrontPorch>0</VerticalFrontPorch>"
" <VerticalBackPorch>0</VerticalBackPorch>"
" <VerticalSyncPulse>0</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>1</DSIAutoRefreshFrameNumDiv>"
" <DSIClockHSForceRequest>1</DSIClockHSForceRequest>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>1920</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>1920</DSITEvSyncInitVal>"
" <DSIControllerMapping>\n"
"  00 \n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" 15 bb 10\n"
" 15 b0 03\n"
" 05 11 00\n"
" ff 78\n"
" 15 51 ff\n"
" 15 53 24\n"
" 15 ff 23\n"
" 15 08 05\n"
" 15 46 90\n"
" 15 ff 10\n"
" 15 ff f0\n"
" 15 92 01\n"
" 15 ff 10\n"
" 15 35 00\n"
" 05 29 00\n"
" ff 64\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultHigh units='hex list'>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>1</BacklightType>\n"
" <BacklightPmicControlType>2</BacklightPmicControlType>\n"  
"</Group>\n";


/******************************************************************
 *           Samsung MG2 5.5in 2k AMOLED PANEL                    *
 *                                                                *
 *                   1440x2560                                    *
 *                   Single DSI                                   *
 *                   DSC Cmd mode                                 *
 ******************************************************************/
/****************************************************************
PPS ==> 88 bytes here + 32 bytes zero filled
0A
11 00 00 89 30 80 0A 00 05 A0 
00 20 02 D0 02 D0 02 00 02 68
00 20 03 87 00 0A 00 0C 03 19
02 63 18 00 10 F0 03 0C 20 00
06 0B 0B 33 0E 1C 2A 38 46 54
62 69 70 77 79 7B 7D 7E 01 02
01 00 09 40 09 BE 19 FC 19 FA
19 F8 1A 38 1A 78 1A B6 2A F6
2B 34 2B 74 3B 74 6B F4\n"
****************************************************************/
const static int8 Samsung_wqhd_amoled_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>S6E3HA3X01</PanelName>"
"<PanelDescription>Samsung MG2 Single DSI DSC Command Mode Panel (1440x2560 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1440</HorizontalActive>"
" <HorizontalFrontPorch>136</HorizontalFrontPorch>"
" <HorizontalBackPorch>100</HorizontalBackPorch>"
" <HorizontalSyncPulse>44</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>2560</VerticalActive>"
" <VerticalFrontPorch>120</VerticalFrontPorch>"
" <VerticalBackPorch>124</VerticalBackPorch>"
" <VerticalSyncPulse>80</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>1</DSIAutoRefreshFrameNumDiv>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>2560</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>2560</DSITEvSyncInitVal>"
" <DSIDSCEnable>True</DSIDSCEnable>"
" <DSIDSCMajor>1</DSIDSCMajor>"
" <DSIDSCMinor>1</DSIDSCMinor>"
" <DSIDSCScr>0</DSIDSCScr>"
" <DSIDSCProfileID>7</DSIDSCProfileID>"
" <DSIDSCSliceWidth>720</DSIDSCSliceWidth>"
" <DSIDSCSliceHeight>16</DSIDSCSliceHeight>"
" <DSITEvSyncStartPos>2560</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>2560</DSITEvSyncInitVal>"
" <DSIPacketTransferHS>True</DSIPacketTransferHS>"
" <DSIControllerMapping>\n"
"  00\n"
" </DSIControllerMapping>\n"
"</Group>"
"<DSIInitSequence>"
"29 F0 5A 5A\n"
"07 01\n"
"05 11\n"
"ff 78\n"
"29 FC 5A 5A\n"
"29 B0 35\n"
"29 FE 03\n"
"29 ED 44\n"
"29 2A 00 00 05 9F\n"
"29 2B 00 00 09 FF\n"
"29 BA 01\n"
"29 FC A5 A5\n"
"29 35 00\n"
"29 FC 5A 5A\n"
"29 C5 0D 08 40 19 BA 07 1F 81 25 CE 79 77 73 00 FF 78 8F 00 00\n"
"29 FC A5 A5\n"
"29 CA 01 00 01 00 01 00 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 00 00 00 00 00\n"
"29 B1 10 03\n"
"29 B0 01\n"
"29 B5 BC 0A\n"
"29 F4 BB 1E\n"
"29 F7 03\n"
"29 F7 00\n"
"29 F0 A5 A5\n"
"29 9F A5 A5\n"
"15 53 20\n"
"05 29 00\n"
"29 9F 5A 5A\n"
" ff 64\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultHigh>0x4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>3</BacklightType>\n"
"</Group>\n";

/******************************************************************
 *           Samsung MG2 5.5in 2k AMOLED PANEL                    *
 *                                                                *
 *                   1080X1920                                    *
 *                   Single DSI                                   *
 *                   DSC Cmd mode                                 *
 ******************************************************************/
/****************************************************************
PPS ==> 88 bytes here + 32 bytes zero filled
0A
11 00 00 89 30 80 07 80 04 38
00 20 02 1C 02 1C 02 00 02 0E
00 20 03 15 00 07 00 0C 03 19
03 2E 18 00 10 F0 03 0C 20 00
06 0B 0B 33 0E 1C 2A 38 46 54
62 69 70 77 79 7B 7D 7E 01 02
01 00 09 40 09 BE 19 FC 19 FA
19 F8 1A 38 1A 78 1A B6 2A F6
2B 34 2B 74 3B 74 6B F4\n"
****************************************************************/
const static int8 Samsung_1080p_amoled_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>S6E3HA3X01</PanelName>"
"<PanelDescription>Samsung MG2 Single DSI DSC Command Mode Panel (1080x1920 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1080</HorizontalActive>"
" <HorizontalFrontPorch>136</HorizontalFrontPorch>"
" <HorizontalBackPorch>220</HorizontalBackPorch>"
" <HorizontalSyncPulse>44</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>1920</VerticalActive>"
" <VerticalFrontPorch>760</VerticalFrontPorch>"
" <VerticalBackPorch>124</VerticalBackPorch>"
" <VerticalSyncPulse>80</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>1</DSIAutoRefreshFrameNumDiv>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>2560</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>2560</DSITEvSyncInitVal>"
" <DSIDSCEnable>True</DSIDSCEnable>"
" <DSIDSCMajor>1</DSIDSCMajor>"
" <DSIDSCMinor>1</DSIDSCMinor>"
" <DSIDSCScr>0</DSIDSCScr>"
" <DSIDSCProfileID>7</DSIDSCProfileID>"
" <DSIDSCSliceWidth>540</DSIDSCSliceWidth>"
" <DSIDSCSliceHeight>16</DSIDSCSliceHeight>"
" <DSITEvSyncStartPos>1920</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>1920</DSITEvSyncInitVal>"
" <DSIPacketTransferHS>True</DSIPacketTransferHS>"
" <DSIControllerMapping>\n"
"  00\n"
" </DSIControllerMapping>\n"
"</Group>"
"<DSIInitSequence>"
"29 F0 5A 5A\n"
"07 01\n"
"05 11\n"
"29 E6 01\n"
"29 FC 5A 5A\n"
"29 B0 35\n"
"29 FE 03\n"
"29 FC A5 A5\n"
"FF 78\n"
"29 35 00\n"
"29 ED 44\n"
"29 2A 00 00 04 37\n"
"29 2B 00 00 07 7F\n"
"29 BA 02\n"
"29 FC 5A 5A\n"
"29 C5 0D 08 40 19 BA 07 1F 81 25 CE 79 77 73 00 FF 78 8F 00 00\n"
"29 FC A5 A5\n"
"29 CA 01 00 01 00 01 00 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 00 00 00 00 00\n"
"29 B1 10 03\n"
"29 B0 01\n"
"29 B5 BC 0A\n"
"29 F4 BB 1E\n"
"29 F7 03\n"
"FF 78\n"
"29 F7 00\n"
"29 F0 A5 A5\n"
"FF 78\n"
"29 9F A5 A5\n"
"15 53 20\n"
"05 29 00\n"
"FF 78\n"
"29 9F 5A 5A\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultHigh>0x4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>3</BacklightType>\n"
"</Group>\n";
/******************************************************************
 *           Samsung MG2 5.5in 2k AMOLED PANEL                    *
 *                                                                *
 *                   720x1280                                     *
 *                   Single DSI                                   *
 *                   Cmd mode, no DSC                             *
 ******************************************************************/
const static int8 Samsung_720p_amoled_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelDescription>Samsung MG2 Amoled 720P DSI Command Mode Panel (720x1280 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>720</HorizontalActive>"
" <HorizontalFrontPorch>16</HorizontalFrontPorch>"
" <HorizontalBackPorch>16</HorizontalBackPorch>"
" <HorizontalSyncPulse>8</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>1280</VerticalActive>"
" <VerticalFrontPorch>1400</VerticalFrontPorch>"
" <VerticalBackPorch>124</VerticalBackPorch>"
" <VerticalSyncPulse>80</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>1</DSIAutoRefreshFrameNumDiv>"
" <DSIClockHSForceRequest>1</DSIClockHSForceRequest>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>1280</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>1280</DSITEvSyncInitVal>"
" <DSIPacketTransferHS>True</DSIPacketTransferHS>"
" <DSIControllerMapping>\n"
"  00 \n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" 29 F0 5A 5A\n" 
" 07 00\n"       
" 05 11 00\n"     
" ff 78\n"
" 29 E6 01\n"   
" 29 FC 5A 5A\n"
" 29 B0 35\n"
" 29 FE 03\n"
" 29 FC A5 A5\n"
" 29 35 00\n"    
" 29 ED 44\n"    
" 29 2A 00 00 02 CF\n"  
" 29 2B 00 00 04 FF\n"
" 29 BA 00\n"    
" 29 FC 5A 5A\n"
" 29 C5 0D 08 40 19 BA 07 1F 81 25 CE 79 77 73 00 FF 78 8F 00 00\n"
" 29 FC A5 A5\n"
" 29 CA 01 00 01 00 01 00 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 00 00 00 00 00\n"
" 29 B1 10 03\n"      
" 29 B0 01\n"
" 29 B5 BC 0A\n"     
" 29 F4 BB 1E\n"      
" 29 F7 03\n"     
" 29 F7 00\n"
" 29 F0 A5 A5\n"      
" 29 9F A5 A5\n"     
" 15 53 20\n"
" 05 29 00\n"
" 29 9F 5A 5A\n"    
" ff 64\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultHigh units='hex list'>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>3</BacklightType>\n"
"</Group>\n";

/******************************************************************
 *           Samsung MG1 5.1in 2k AMOLED PANEL                    *
 *                                                                *
 *                   1440X2560                                    *
 *                   dual DSI                                     *
 *                   Cmd mode, no DSC                             *
 ******************************************************************/
const static int8 Samsung_wqhd_amoled_dual_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelDescription>Samsung MG1 Amoled wqxa dual DSI Command Mode Panel (1440x2560 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1440</HorizontalActive>"
" <HorizontalFrontPorch>0</HorizontalFrontPorch>"
" <HorizontalBackPorch>0</HorizontalBackPorch>"
" <HorizontalSyncPulse>0</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>2560</VerticalActive>"
" <VerticalFrontPorch>0</VerticalFrontPorch>"
" <VerticalBackPorch>0</VerticalBackPorch>"
" <VerticalSyncPulse>0</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>1</DSIAutoRefreshFrameNumDiv>"
" <DSIClockHSForceRequest>1</DSIClockHSForceRequest>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>2560</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>2560</DSITEvSyncInitVal>"
" <DSICmdModePixelAlignment>1</DSICmdModePixelAlignment>"
" <DSIPacketTransferHS>True</DSIPacketTransferHS>"
" <DSIControllerMapping>\n"
"  00 01\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" 29 F0 5A 5A\n" 
" 07 00\n"       
" 05 11 00\n"     
" ff 78\n"
" 29 E6 01\n"   
" 29 FC 5A 5A\n"
" 29 B0 35\n"
" 29 FE 03\n"
" 29 FC A5 A5\n"
" 29 35 00\n"    
" 29 ED 44\n"    
"29 2A 00 00 05 9F\n"
"29 2B 00 00 09 FF\n"
" 29 BA 00\n"    
" 29 FC 5A 5A\n"
" 29 C5 0D 08 40 19 BA 07 1F 81 25 CE 79 77 73 00 FF 78 8F 00 00\n"
" 29 FC A5 A5\n"
" 29 CA 01 00 01 00 01 00 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 00 00 00 00 00\n"
" 29 B1 10 03\n"      
" 29 B0 01\n"
" 29 B5 BC 0A\n"     
" 29 F4 BB 1E\n"      
" 29 F7 03\n"     
" 29 F7 00\n"
" 29 F0 A5 A5\n"      
" 29 9F A5 A5\n"     
" 15 53 20\n"
" 05 29 00\n"
" 29 9F 5A 5A\n"    
" ff 64\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultHigh units='hex list'>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>3</BacklightType>\n"
"</Group>\n";


/******************************************************************
 *           Truly FHD (1080 x 1920) Video mode                   *
 *                                                                *
 ******************************************************************/
const static int8 Truly_NT35695b_FHD_video_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>NT35695b</PanelName>"
"<PanelDescription>Truly FHD DSI Video Mode Panel (1080x1920 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive units=\"Dot Clocks\">1080</HorizontalActive>"
" <HorizontalFrontPorch units=\"Dot Clocks\">120</HorizontalFrontPorch>"
" <HorizontalBackPorch units=\"Dot Clocks\">60</HorizontalBackPorch>"
" <HorizontalSyncPulse units=\"Dot Clocks\">12</HorizontalSyncPulse>"
" <VerticalActive units=\"Dot Clocks\">1920</VerticalActive>"
" <VerticalFrontPorch units=\"Lines\">12</VerticalFrontPorch>"
" <VerticalBackPorch units=\"Lines\">2</VerticalBackPorch>"
" <VerticalSyncPulse units=\"Lines\">2</VerticalSyncPulse>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType units=\"QDI_DisplayConnectType\">8</InterfaceType>"
" <InterfaceColorFormat units=\"QDI_PixelFormatType\">3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId units=\"DSI_Channel_IDType\">1</DSIChannelId>"
" <DSIVirtualId units=\"DSI_Display_VCType\">0</DSIVirtualId>"
" <DSIColorFormat units=\"DSI_ColorFormatType\">36</DSIColorFormat>"
" <DSITrafficMode units=\"DSI_TrafficModeType\">2</DSITrafficMode>"
" <DSILanes units=\"integer\">4</DSILanes>"
" <DSILowPowerModeInBLLPEOF units='Bool'>True</DSILowPowerModeInBLLPEOF>\n"
" <DSILowPowerModeInBLLP units='Bool'>True</DSILowPowerModeInBLLP>\n"
" <DSIRefreshRate units='integer Q16.16'>0x3C0000</DSIRefreshRate>\n"
" <DSIInitMasterTime units='integer'>1</DSIInitMasterTime>\n"  
" <DSIControllerMapping>\n"
"  00\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" FF 20\n"
" 15 ff 20\n" 
" ff 10\n"
" 15 fb 01\n"
" 15 00 01\n"
" 15 01 55\n"
" 15 02 45\n"
" 15 03 55\n"
" 15 05 50\n"
" 15 06 a8\n"
" 15 07 ad\n"
" 15 08 0c\n"
" 15 0b aa\n"
" 15 0c aa\n"
" 15 0e b0\n"
" 15 0f b3\n"
" 15 11 28\n"
" 15 12 10\n"
" 15 13 01\n"
" 15 14 4a\n"
" 15 15 12\n"
" 15 16 12\n"
" 15 30 01\n"
" 15 72 11\n"
" 15 58 82\n"
" 15 59 00\n"
" 15 5a 02\n"
" 15 5b 00\n"
" 15 5c 82\n"
" 15 5d 80\n"
" 15 5e 02\n"
" 15 5f 00\n"
" 15 ff 24\n"
" 15 fb 01\n"
" 15 00 01\n"
" 15 01 0b\n"
" 15 02 0c\n"
" 15 03 89\n"
" 15 04 8a\n"
" 15 05 0f\n"
" 15 06 10\n"
" 15 07 10\n"
" 15 08 1c\n"
" 15 09 00\n"
" 15 0a 00\n"
" 15 0b 00\n"
" 15 0c 00\n"
" 15 0d 13\n"
" 15 0e 15\n"
" 15 0f 17\n"
" 15 10 01\n"
" 15 11 0b\n"
" 15 12 0c\n"
" 15 13 89\n"
" 15 14 8a\n"
" 15 15 0f\n"
" 15 16 10\n"
" 15 17 10\n"
" 15 18 1c\n"
" 15 19 00\n"
" 15 1a 00\n"
" 15 1b 00\n"
" 15 1c 00\n"
" 15 1d 13\n"
" 15 1e 15\n"
" 15 1f 17\n"
" 15 20 00\n"
" 15 21 01\n"
" 15 22 00\n"
" 15 23 40\n"
" 15 24 40\n"
" 15 25 6d\n"
" 15 26 40\n"
" 15 27 40\n"
" 15 29 d8\n"
" 15 2a 2a\n"
" 15 4b 03\n"
" 15 4c 11\n"
" 15 4d 10\n"
" 15 4e 01\n"
" 15 4f 01\n"
" 15 50 10\n"
" 15 51 00\n"
" 15 52 80\n"
" 15 53 00\n"
" 15 54 07\n"
" 15 55 25\n"
" 15 56 00\n"
" 15 58 07\n"
" 15 5b 43\n"
" 15 5c 00\n"
" 15 5f 73\n"
" 15 60 73\n"
" 15 63 22\n"
" 15 64 00\n"
" 15 67 08\n"
" 15 68 04\n"
" 15 7a 80\n"
" 15 7b 91\n"
" 15 7c d8\n"
" 15 7d 60\n"
" 15 93 06\n"
" 15 94 06\n"
" 15 8a 00\n"
" 15 9b 0f\n"
" 15 b3 c0\n"
" 15 b4 00\n"
" 15 b5 00\n"
" 15 b6 21\n"
" 15 b7 22\n"
" 15 b8 07\n"
" 15 b9 07\n"
" 15 ba 22\n"
" 15 bd 20\n"
" 15 be 07\n"
" 15 bf 07\n"
" 15 c1 6d\n"
" 15 c4 24\n"
" 15 e3 00\n"
" 15 ec 00\n"
" 15 ff 10\n"
" 15 bb 03\n"
" 05 11 00\n"
" ff 78\n"
" 05 29 00\n"           
" ff 78\n"
"</DSIInitSequence>"
"<DSITermSequence>\n"
" 05 28 00\n"
" FF 14\n"
" 05 10 00\n"
" FF 78\n"
"</DSITermSequence>\n"
"<TLMMGPIODefaultHigh units='hex list'>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType units='MDP_Panel_BacklightType'>1</BacklightType>\n"
" <BacklightPmicControlType units='MDP_PmicBacklightControlType'>2</BacklightPmicControlType>\n"  
"</Group>\n";


/******************************************************************
 *           Truly FHD (1080 x 1920) Command mode                 *
 *                                                                *
 ******************************************************************/
const static int8 Truly_NT35695b_FHD_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>TFT2P2827-E</PanelName>"
"<PanelDescription>Truly FHD DSI Command Mode Panel (1080x1920 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive units=\"Dot Clocks\">1080</HorizontalActive>"
" <HorizontalFrontPorch units=\"Dot Clocks\">120</HorizontalFrontPorch>"
" <HorizontalBackPorch units=\"Dot Clocks\">60</HorizontalBackPorch>"
" <HorizontalSyncPulse units=\"Dot Clocks\">12</HorizontalSyncPulse>"
" <VerticalActive units=\"Dot Clocks\">1920</VerticalActive>"
" <VerticalFrontPorch units=\"Lines\">12</VerticalFrontPorch>"
" <VerticalBackPorch units=\"Lines\">2</VerticalBackPorch>"
" <VerticalSyncPulse units=\"Lines\">2</VerticalSyncPulse>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType units=\"QDI_DisplayConnectType\">9</InterfaceType>"
" <InterfaceColorFormat units=\"QDI_PixelFormatType\">3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId units=\"DSI_Channel_IDType\">2</DSIChannelId>"
" <DSIVirtualId units=\"DSI_Display_VCType\">0</DSIVirtualId>"
" <DSIColorFormat units=\"DSI_ColorFormatType\">36</DSIColorFormat>"
" <DSITrafficMode units=\"DSI_TrafficModeType\">2</DSITrafficMode>"
" <DSILanes units=\"integer\">4</DSILanes>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>1</DSIAutoRefreshFrameNumDiv>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>480</DSITEvSyncStartPos>"
" <DSIRefreshRate units='integer Q16.16'>0x3C0000</DSIRefreshRate>\n"
" <DSIInitMasterTime units='integer'>1</DSIInitMasterTime>\n"  
" <DSIControllerMapping>\n"
"  00\n"
" </DSIControllerMapping>\n" 
"</Group>"
"<DSIInitSequence>"
" FF 20\n"
" 15 ff 20\n" 
" ff 10\n"
" 15 fb 01\n"
" 15 00 01\n"
" 15 01 55\n"
" 15 02 45\n"
" 15 03 55\n"
" 15 05 50\n"
" 15 06 a8\n"
" 15 07 ad\n"
" 15 08 0c\n"
" 15 0b aa\n"
" 15 0c aa\n"
" 15 0e b0\n"
" 15 0f b3\n"
" 15 11 28\n"
" 15 12 10\n"
" 15 13 01\n"
" 15 14 4a\n"
" 15 15 12\n"
" 15 16 12\n"
" 15 30 01\n"
" 15 72 11\n"
" 15 58 82\n"
" 15 59 00\n"
" 15 5a 02\n"
" 15 5b 00\n"
" 15 5c 82\n"
" 15 5d 80\n"
" 15 5e 02\n"
" 15 5f 00\n"
" 15 ff 24\n"
" 15 fb 01\n"
" 15 00 01\n"
" 15 01 0b\n"
" 15 02 0c\n"
" 15 03 89\n"
" 15 04 8a\n"
" 15 05 0f\n"
" 15 06 10\n"
" 15 07 10\n"
" 15 08 1c\n"
" 15 09 00\n"
" 15 0a 00\n"
" 15 0b 00\n"
" 15 0c 00\n"
" 15 0d 13\n"
" 15 0e 15\n"
" 15 0f 17\n"
" 15 10 01\n"
" 15 11 0b\n"
" 15 12 0c\n"
" 15 13 89\n"
" 15 14 8a\n"
" 15 15 0f\n"
" 15 16 10\n"
" 15 17 10\n"
" 15 18 1c\n"
" 15 19 00\n"
" 15 1a 00\n"
" 15 1b 00\n"
" 15 1c 00\n"
" 15 1d 13\n"
" 15 1e 15\n"
" 15 1f 17\n"
" 15 20 00\n"
" 15 21 01\n"
" 15 22 00\n"
" 15 23 40\n"
" 15 24 40\n"
" 15 25 6d\n"
" 15 26 40\n"
" 15 27 40\n"
" 15 29 d8\n"
" 15 2a 2a\n"
" 15 4b 03\n"
" 15 4c 11\n"
" 15 4d 10\n"
" 15 4e 01\n"
" 15 4f 01\n"
" 15 50 10\n"
" 15 51 00\n"
" 15 52 80\n"
" 15 53 00\n"
" 15 54 07\n"
" 15 55 25\n"
" 15 56 00\n"
" 15 58 07\n"
" 15 5b 43\n"
" 15 5c 00\n"
" 15 5f 73\n"
" 15 60 73\n"
" 15 63 22\n"
" 15 64 00\n"
" 15 67 08\n"
" 15 68 04\n"
" 15 7a 80\n"
" 15 7b 91\n"
" 15 7c d8\n"
" 15 7d 60\n"
" 15 93 06\n"
" 15 94 06\n"
" 15 8a 00\n"
" 15 9b 0f\n"
" 15 b3 c0\n"
" 15 b4 00\n"
" 15 b5 00\n"
" 15 b6 21\n"
" 15 b7 22\n"
" 15 b8 07\n"
" 15 b9 07\n"
" 15 ba 22\n"
" 15 bd 20\n"
" 15 be 07\n"
" 15 bf 07\n"
" 15 c1 6d\n"
" 15 c4 24\n"
" 15 e3 00\n"
" 15 ec 00\n"
" 15 ff 10\n"
" 15 bb 10\n"
" 15 35 02\n"
" 05 11 00\n"
" ff 78\n"
" fe 00 00 00\n"
" 05 29 00\n"           
" ff 78\n"
"</DSIInitSequence>"
"<DSITermSequence>\n"
" 05 28 00\n"
" FF 14\n"
" 05 10 00\n"
" FF 78\n"
"</DSITermSequence>\n"
"<TLMMGPIODefaultHigh units='hex list'>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType units='MDP_Panel_BacklightType'>1</BacklightType>\n"
" <BacklightPmicControlType units='MDP_PmicBacklightControlType'>2</BacklightPmicControlType>\n"  
"</Group>\n";

 
/******************************************************************
 *           Truly 5.5in 2k  NT36850                              *
 *                                                                *
 *                   1440x2560                                    *
 *                   Dual-DSI                                     *
 *                   Cmd mode                                     *
 ******************************************************************/
const static int8 Truly_NT36850_WQHD_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>NT36850</PanelName>"
"<PanelDescription>Truly Dual DSI Command Mode Panel (1440x2560 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1440</HorizontalActive>"
" <HorizontalFrontPorch>120</HorizontalFrontPorch>"
" <HorizontalBackPorch>140</HorizontalBackPorch>"
" <HorizontalSyncPulse>20</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>2560</VerticalActive>"
" <VerticalFrontPorch>8</VerticalFrontPorch>"
" <VerticalBackPorch>20</VerticalBackPorch>"
" <VerticalSyncPulse>4</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSIDynamicRefreshRates>0x350000 0x360000 0x370000 0x380000 0x390000 0x3A0000 0x3B0000 0x3C0000</DSIDynamicRefreshRates>\n"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>2</DSIAutoRefreshFrameNumDiv>"
" <DSIClockHSForceRequest>1</DSIClockHSForceRequest>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>2560</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>2560</DSITEvSyncInitVal>"
" <DSICmdModePixelAlignment>1</DSICmdModePixelAlignment>"
" <DSIControllerMapping>\n"
"  00 01\n"
" </DSIControllerMapping>\n"
"</Group>"
"<DSIInitSequence>"
" FD 02\n"
" 15 FF 10\n"
" 15 FB 01\n"
" 15 36 00\n"
" 15 35 00\n"
" 39 44 03 E8\n"
" 15 51 FF\n"
" 15 53 2C\n"
" 15 55 01\n"
" 05 20 00\n"
" FF 0A\n"
" 15 BB 10\n"
" 05 11 00\n"
" FF 78\n"
" 05 29 00\n"
" FF 14\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultLow>4C</TLMMGPIODefaultLow>"
"<Group id='Backlight Configuration'>"
" <BacklightType>1</BacklightType>\n"
" <BacklightPmicControlType>2</BacklightPmicControlType>\n"
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
"</Group>\n";


 /******************************************************************
 *                Truly FHD HX8399                                 *
 *                                                                 *
 *                   1080x2160                                     *
 *                   Single-DSI                                    *
 *                   Video mode                                    *
 *******************************************************************/
const static int8 Truly_HX8399_FHD_video_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>HX8399</PanelName>"
"<PanelDescription>HX8399 video mode dsi truly panel</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1080</HorizontalActive>"
" <HorizontalFrontPorch>42</HorizontalFrontPorch>"
" <HorizontalBackPorch>42</HorizontalBackPorch>"
" <HorizontalSyncPulse>10</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>2160</VerticalActive>"
" <VerticalFrontPorch>10</VerticalFrontPorch>"
" <VerticalBackPorch>15</VerticalBackPorch>"
" <VerticalSyncPulse>3</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>8</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>1</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSILowPowerModeInBLLPEOF>True</DSILowPowerModeInBLLPEOF>"
" <DSILowPowerModeInBLLP>True</DSILowPowerModeInBLLP>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIControllerMapping>\n"
"  00\n"
" </DSIControllerMapping>\n"
"</Group>"
"<DSIInitSequence>"
" 39 B9 FF 83 99\n"
" 39 D2 88\n"
" 39 B1 02 04 72 92 01 32 AA 11 11 52 57\n"
" 39 B2 00 80 80 CC 05 07 5A 11 10 10 00 1E 70 03 D4\n"
" 39 B4 00 FF 59 59 01 AB 00 00 09 00 03 05 00 28 03 0B 0D 21 03 02 00 0C A3 80 59 59 02 AB 00 00 09 00 03 05 00 28 03 0B 0D 02 00 0C A3 01\n"
" 39 D3 00 0C 03 03 00 00 10 10 00 00 03 00 03 00 08 78 08 78 00 00 00 00 00 24 02 05 05 03 00 00 00 05 40\n"
" 39 D5 20 20 19 19 18 18 02 03 00 01 24 24 18 18 18 18 24 24 00 00 00 00 00 00 00 00 2F 2F 30 30 31 31\n"
" 39 D6 24 24 18 18 19 19 01 00 03 02 24 24 18 18 18 18 20 20 40 40 40 40 40 40 40 40 2F 2F 30 30 31 31\n"
" 39 BD 00\n"
" 39 D8 AA AA AA AA AA AA AA AA AA BA AA AA AA BA AA AA\n"
" 39 BD 01\n"
" 39 D8 00 00 00 00 00 00 00 00 82 EA AA AA 82 EA AA AA\n"
" 39 BD 02\n"
" 39 D8 FF FF C0 3F FF FF C0 3F\n"
" 39 BD 00\n"
" 39 E0 01 21 31 2D 66 6F 7B 75 7A 81 86 89 8C 90 95 97 9A A1 A2 AA 9E AD B0 5B 57 63 7A 01 21 31 2D 66 6F 7B 75 7A 81 86 89 8C 90 95 97 9A A1 A2 AA 9E AD B0 5B 57 63 7A\n"
" 39 B6 7E 7E\n"
" 39 CC 08\n"
" 05 11 00\n"
" FF 96\n"
" 05 29 00\n"
" FF 32\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultLow>4C</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>1</BacklightType>\n"
" <BacklightPmicControlType>2</BacklightPmicControlType>\n"
" <DisplayResetInfo>0 10 10000 10000 0</DisplayResetInfo>\n"
"</Group>\n";


/******************************************************************
 *              FHD AMOLED RM67195                                *
 *                                                                *
 *                   1080x1920                                    *
 *                   Single-DSI                                   *
 *                   Cmd mode                                     *
 ******************************************************************/
const static int8 Rm67195_amoled_fhd_cmd_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>rm67195</PanelName>"
"<PanelDescription>rm67195 amoled fhd cmd mode dsi panel (1080x1920 24bpp)</PanelDescription>"
"<Group id=\"Active Timing\">"
" <HorizontalActive>1080</HorizontalActive>"
" <HorizontalFrontPorch>120</HorizontalFrontPorch>"
" <HorizontalBackPorch>60</HorizontalBackPorch>"
" <HorizontalSyncPulse>12</HorizontalSyncPulse>"
" <HorizontalSyncSkew>0</HorizontalSyncSkew>"
" <HorizontalLeftBorder>0</HorizontalLeftBorder>"
" <HorizontalRightBorder>0</HorizontalRightBorder>"
" <VerticalActive>1920</VerticalActive>"
" <VerticalFrontPorch>8</VerticalFrontPorch>"
" <VerticalBackPorch>12</VerticalBackPorch>"
" <VerticalSyncPulse>4</VerticalSyncPulse>"
" <VerticalSyncSkew>0</VerticalSyncSkew>"
" <VerticalTopBorder>0</VerticalTopBorder>"
" <VerticalBottomBorder>0</VerticalBottomBorder>"
" <InvertDataPolarity>False</InvertDataPolarity>"
" <InvertVsyncPolairty>False</InvertVsyncPolairty>"
" <InvertHsyncPolarity>False</InvertHsyncPolarity>"
" <BorderColor>0x0</BorderColor>"
" <PanelOrientation>0x2</PanelOrientation>"
"</Group>"
"<Group id=\"Display Interface\">"
" <InterfaceType>9</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>2</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>0</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIEnableAutoRefresh>True</DSIEnableAutoRefresh>"
" <DSIAutoRefreshFrameNumDiv>1</DSIAutoRefreshFrameNumDiv>"
" <DSITECheckEnable>True</DSITECheckEnable>"
" <DSITEUsingDedicatedTEPin>True</DSITEUsingDedicatedTEPin>"
" <DSITEvSyncStartPos>1920</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>1920</DSITEvSyncInitVal>"
" <DSITEvSyncStartPos>1920</DSITEvSyncStartPos>"
" <DSITEvSyncInitVal>1920</DSITEvSyncInitVal>"
" <DSIPacketTransferHS>True</DSIPacketTransferHS>"
" <DSIControllerMapping>\n"
"  00\n"
" </DSIControllerMapping>\n"
"</Group>"
"<DSIInitSequence>"
"15 fe 0d\n"
"15 0b c0\n"
"15 42 00\n"
"15 18 08\n"
"15 08 41\n"
"15 46 02\n"
"15 1e 04\n"
"15 1e 00\n"
"15 fe 0a\n"
"15 24 17\n"
"15 04 07\n"
"15 1a 0c\n"
"15 0f 44\n"
"15 fe 0b\n"
"15 28 40\n"
"15 29 4f\n"
"15 fe 04\n"
"15 0a d8\n"
"15 0c e6\n"
"15 4e 20\n"
"15 4f 1b\n"
"15 50 2f\n"
"15 51 08\n"
"15 fe 09\n"
"15 00 08\n"
"15 01 08\n"
"15 02 00\n"
"15 03 00\n"
"15 04 10\n"
"15 05 00\n"
"15 06 08\n"
"15 07 08\n"
"15 08 00\n"
"15 12 24\n"
"15 13 49\n"
"15 14 92\n"
"15 15 49\n"
"15 16 92\n"
"15 17 24\n"
"15 18 24\n"
"15 19 49\n"
"15 1a 92\n"
"15 1b 49\n"
"15 1c 92\n"
"15 1d 24\n"
"15 1e 24\n"
"15 1f 49\n"
"15 20 92\n"
"15 21 49\n"
"15 22 92\n"
"15 23 24\n"
"15 9b 07\n"
"15 9c a5\n"
"15 fe 00\n"
"15 c2 08\n"
"15 35 00\n"
"39 44 03 e8\n"
"05 11 00\n"
"05 29 00\n"
"</DSIInitSequence>"
"<DSITermSequence>\n"
" 05 28\n"
" 05 10\n"
"</DSITermSequence>\n"
"<TLMMGPIODefaultHigh>0x34</TLMMGPIODefaultHigh>"
"<Group id='Backlight Configuration'>"
" <BacklightType>3</BacklightType>\n"
"</Group>\n";


/******************************************************************
 *                LT9611 DSI2HDMI                                 *
 *                                                                *
 *                   1920x1080                                    *
 *                   Single-DSI                                   *
 *                   Video mode                                   *
 ******************************************************************/
const static int8 LT9611_DSI2HDMI_FHD_video_xmldata[] = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"<PanelName>LT9611</PanelName>\n"
"<PanelDescription>LT9611 DSI2HDMI (1920x1080 24bpp)</PanelDescription>\n"
"<Group id='Active Timing'>\n"
"  <HorizontalActive>1920</HorizontalActive>\n"
"  <HorizontalFrontPorch>88</HorizontalFrontPorch>\n"
"  <HorizontalBackPorch>148</HorizontalBackPorch>\n"
"  <HorizontalSyncPulse>44</HorizontalSyncPulse>\n"
"  <HorizontalSyncSkew>0</HorizontalSyncSkew>\n"
"  <HorizontalLeftBorder>0</HorizontalLeftBorder>\n"
"  <HorizontalRightBorder>0</HorizontalRightBorder>\n"
"  <VerticalActive>1080</VerticalActive>\n"
"  <VerticalFrontPorch>4</VerticalFrontPorch>\n"
"  <VerticalBackPorch>36</VerticalBackPorch>\n"
"  <VerticalSyncPulse>5</VerticalSyncPulse>\n"
"  <VerticalSyncSkew>0</VerticalSyncSkew>\n"
"  <VerticalTopBorder>0</VerticalTopBorder>\n"
"  <VerticalBottomBorder>0</VerticalBottomBorder>\n"
"</Group>\n"
"<Group id=\"Display Interface\">"
" <InterfaceType>8</InterfaceType>"
" <InterfaceColorFormat>3</InterfaceColorFormat>"
"</Group>"
"<Group id=\"DSI Interface\">"
" <DSIChannelId>1</DSIChannelId>"
" <DSIVirtualId>0</DSIVirtualId>"
" <DSIColorFormat>36</DSIColorFormat>"
" <DSITrafficMode>1</DSITrafficMode>"
" <DSILanes>4</DSILanes>"
" <DSILowPowerModeInBLLPEOF>True</DSILowPowerModeInBLLPEOF>"
" <DSILowPowerModeInBLLP>True</DSILowPowerModeInBLLP>"
" <DSIRefreshRate>0x3C0000</DSIRefreshRate>"
" <DSICmdSwapInterface>False</DSICmdSwapInterface>"
" <DSICmdUsingTrigger>False</DSICmdUsingTrigger>"
" <DSIControllerMapping>\n"
"  00\n"
" </DSIControllerMapping>\n"
"</Group>"
"<DSII2CSlaveAddressList>0x3B</DSII2CSlaveAddressList>\n"
"<DSII2CI2CNumOfSlaves>0x1</DSII2CI2CNumOfSlaves>\n"
"<DSII2CFrequency>0x190</DSII2CFrequency>\n"
"<DSII2CGSBIPort>0xB</DSII2CGSBIPort>\n"
"<DSII2COffsetLength>0x1</DSII2COffsetLength>\n"
"<I2CInitSequence>\n"
"  3b ff 80\n"
"  3b ee 01\n"
"  ff 78\n"
"  3b ff 81\n"
"  3b 01 18\n"
"  3b ff 82\n"
"  3b 1b 69\n"
"  3b 1c 78\n"
"  3b cb 69\n"
"  3b cc 78\n"
"  3b ff 82\n"
"  3b 51 01\n"
"  3b 58 0a\n"
"  3b 59 80\n"
"  3b 9e f7\n"
"  3b ff 80\n"
"  3b 04 f0\n"
"  3b 06 f0\n"
"  3b 0a 80\n"
"  3b 0b 40\n"
"  3b 11 fa\n"
"  3b ff 81\n"
"  3b 06 40\n"
"  3b 0a fe\n"
"  3b 0b bf\n"
"  3b 11 40\n"
"  3b 15 fe\n"
"  3b 16 bf\n"
"  3b 1c 03\n"
"  3b 20 03\n"
"  3b ff 83\n"
"  3b 00 00\n" 
"  3b 0a 00\n" 
"  3b ff 82\n"
"  3b 4f 80\n"
"  3b 50 10\n"
"  3b ff 83\n"
"  3b 02 0a\n"
"  3b 06 0a\n"
"  3b ff 81\n"
"  3b 23 40\n"
"  3b 24 64\n"
"  3b 25 80\n"
"  3b 26 55\n"
"  3b 2c 37\n"
"  3b 2f 01\n"
"  3b 26 55\n"
"  3b 27 66\n"
"  3b 28 88\n"
"  3b 2d 99\n"
"  3b ff 82\n"
"  3b e3 01\n"
"  3b e4 22\n"
"  3b e5 0a\n"
"  3b de 20\n"
"  3b de e0\n"
"  3b ff 80\n"
"  3b 16 f1\n"
"  3b 16 f3\n"
"  3b ff 83\n"
"  3b 0d 04\n"
"  3b 0e 65\n"
"  3b 0f 04\n"
"  3b 10 38\n"
"  3b 11 08\n"
"  3b 12 98\n"
"  3b 13 07\n"
"  3b 14 80\n"
"  3b 15 05\n"
"  3b 16 2c\n"
"  3b 17 04\n"
"  3b 18 29\n"
"  3b 19 58\n"
"  3b 1a 00\n"
"  3b 1b c0\n"
"  3b ff 83\n"
"  3b 0b 01\n"
"  3b 0c 10\n"
"  3b 48 00\n"
"  3b 49 81\n"
"  3b 21 4a\n"
"  3b 24 71\n"
"  3b 25 30\n"
"  3b 2a 01\n"
"  3b 4a 40\n"
"  3b 1d 10\n"
"  3b 2d 38\n"
"  3b 31 08\n"
"  3b 26 37\n"
"  3b ff 80\n"
"  3b 11 5a\n"
"  3b 11 fa\n"
"  3b ff 84\n"
"  3b 43 36\n"
"  3b 44 84\n"
"  3b 47 10\n"
"  3b ff 82\n"
"  3b d6 8c\n"
"  3b d7 04\n"
"  3b ff 81\n"
"  3b 30 6a\n"
"  3b 31 44\n"
"  3b 32 4a\n"
"  3b 33 0b\n"
"  3b 34 00\n"
"  3b 35 00\n"
"  3b 36 00\n"
"  3b 37 44\n"
"  3b 3f 0f\n"
"  3b 40 a0\n"
"  3b 41 a0\n"
"  3b 42 a0\n"
"  3b 43 a0\n"
"  3b 44 0a\n"
"  ff ff\n"
"  ff ff\n"
"  3b ff 81\n"
"  3b 30 ea\n"
"</I2CInitSequence>"
"<I2CTermSequence>"
"  3b ff 81\n"
"  3b 30 6a\n"
"  3b ff 80\n"
"  3b 24 76\n"
"  3b 23 01\n"
"  3b ff 81\n"
"  3b 57 03\n"
"  3b 49 0b\n"
"  3b ff 81\n"
"  3b 51 30\n"
"  3b 02 48\n"
"  3b 23 80\n"
"  3b 30 00\n"
"  3b 00 01\n"
"  3b 01 00\n"
"</I2CTermSequence>"
" <DisplayResetInfo>0 20000 20000 20000 0</DisplayResetInfo>\n";


/*
* Dummy panel configuration, default fallback mode.
*
*/
const static int8 dummy_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"    <PanelName>VirtioDSI</PanelName>"
"    <PanelDescription>DSI Model (640x480 24bpp)</PanelDescription>"
"    <Group id=\"Active Timing\">"
"        <HorizontalActive>640</HorizontalActive>"
"        <HorizontalFrontPorch>13</HorizontalFrontPorch>"
"        <HorizontalBackPorch>14</HorizontalBackPorch>"
"        <HorizontalSyncPulse>15</HorizontalSyncPulse>"
"        <HorizontalSyncSkew>0</HorizontalSyncSkew>"
"        <HorizontalLeftBorder>0</HorizontalLeftBorder>"
"        <HorizontalRightBorder>0</HorizontalRightBorder>"
"        <VerticalActive>480</VerticalActive>"
"        <VerticalFrontPorch>11</VerticalFrontPorch>"
"        <VerticalBackPorch>12</VerticalBackPorch>"
"        <VerticalSyncPulse>13</VerticalSyncPulse>"
"        <VerticalSyncSkew>0</VerticalSyncSkew>"
"        <VerticalTopBorder>0</VerticalTopBorder>"
"        <VerticalBottomBorder>0</VerticalBottomBorder>"
"        <InvertDataPolarity>False</InvertDataPolarity>"
"        <InvertVsyncPolairty>False</InvertVsyncPolairty>"
"        <InvertHsyncPolarity>False</InvertHsyncPolarity>"
"        <BorderColor>0x0</BorderColor>"   
"    </Group>"
"    <Group id=\"Display Interface\">"
"        <InterfaceType>8</InterfaceType>"
"        <InterfaceColorFormat>3</InterfaceColorFormat>"
"    </Group>"
"    <Group id=\"DSI Interface\">"
"        <DSIChannelId>1</DSIChannelId>"     
"        <DSIVirtualId>0</DSIVirtualId>"     
"        <DSIColorFormat>36</DSIColorFormat>"     
"        <DSITrafficMode>0</DSITrafficMode>"
"        <DSILanes>2</DSILanes>"
"        <DSIHsaHseAfterVsVe>False</DSIHsaHseAfterVsVe>"
"        <DSILowPowerModeInHFP>False</DSILowPowerModeInHFP>"
"        <DSILowPowerModeInHBP>False</DSILowPowerModeInHBP>"
"        <DSILowPowerModeInHSA>False</DSILowPowerModeInHSA>"
"        <DSILowPowerModeInBLLPEOF>False</DSILowPowerModeInBLLPEOF>"
"        <DSILowPowerModeInBLLP>False</DSILowPowerModeInBLLP>"
"        <DSIRefreshRate>0x3c0000</DSIRefreshRate>" 
"    </Group>"
"        <DisplayPrimaryFlags>0x2</DisplayPrimaryFlags>"
"    <DSIInitSequence>"
"    32 00 00\n"
"    </DSIInitSequence>";

/***************************************************************************
Dynamic panel detect list:
    -- first entry contains default panel

At SDM845 panel detection base on platform subtype
   -- subtype_0 use Truly 2k panel
       --- dual dsi (without dsc) video and cmd mode
       --- single dsi with DSC video and cmd mode

   -- subtype_1 use Sharp 4k panel
      --- dual dsi DSC with video and cmd mode
***************************************************************************/
/* Default panel for SDM670 : Dual DSI Video mode. */
static PlatformDSIDetectParams panelList_0[] = {    /* subtype == 1 */
    // Panel #1 - Truly Dual DSI Video Mode Panel (1440x2560 24bpp
    {
      0x06,                                                  // uCmdType
      0x05,                                                  // total number of retry on failures
      {
        {{0xDA, 0x00},                                       // address to read ID1
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     // expected readback
        }, 
        {{0xDB, 0x00},                                       // address to read ID2
        {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     // expected readback
        },
        {{0xFE, 0x00},                                       // address to read ID3
        {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}     // expected readback
        }
      },
      0,                                                   // Lane remap order {0, 1, 2, 3}
      Truly_TFT2P2827_E_video_xmldata,                     // psPanelCfg (panel configuration)
      sizeof(Truly_TFT2P2827_E_video_xmldata),             // uPanelCfgSize
      MDPPLATFORM_PANEL_TRULY_WQXGA_DUALDSI_VIDEO,         // eSelectedPanel 
      0                                                    // uFlags
    }
};

/*
* List of supported panels. The ones with XML data have UEFI support and the rest are only supported by kernel.
*
*/
/*
DT info for panels
*/
const PanelInfoType gPanelList[] =
{
  /*Supported Panels*/
  /*4k cmd*/
  PANEL_CREATE_ENTRY("sharp_4k_dsc_cmd",            MDPPLATFORM_PANEL_SHARP_4K_DSC_CMD,            "dsi_sharp_4k_dsc_cmd_display:",         DISP_INTF_DSI, Sharp_4k_cmd_dsc_xmldata,              DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI,                                DISP_MODE_DUAL_DSI,                                   DISP_MODE_DUAL_DSI),
  /*4k video*/
  PANEL_CREATE_ENTRY("sharp_4k_dsc_vid",          MDPPLATFORM_PANEL_SHARP_4K_DSC_VIDEO,          "dsi_sharp_4k_dsc_video_display:",       DISP_INTF_DSI, Sharp_4k_video_dsc_xmldata,            DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI,                                DISP_MODE_DUAL_DSI,                                   DISP_MODE_DUAL_DSI),


  /*Truly 2k NT35597 dsc cmd */
  PANEL_CREATE_ENTRY("truly_wqxga_dsc_cmd",     MDPPLATFORM_PANEL_TRULY_WQXGA_DSC_CMD,     "dsi_nt35597_truly_dsc_cmd_display:",  DISP_INTF_DSI, Truly_TFT2P2827_E_cmd_dsc_xmldata,   DISP_TOPOLOGY_CONFIG_1,  DISP_TIMING_CONFIG_NONE,    PLL_OVERRIDE_PLL1, DISP_MODE_SINGLE_DSI | DISP_MODE_USE_DSI1,         DISP_MODE_SINGLE_DSI | DISP_MODE_USE_DSI1,            DISP_MODE_SINGLE_DSI | DISP_MODE_USE_DSI1 | DISP_MODE_DSC_MERGE),

  /*Truly 2k NT35597 dsc video */
  PANEL_CREATE_ENTRY("truly_wqxga_dsc_vid",     MDPPLATFORM_PANEL_TRULY_WQXGA_DSC_VIDEO,     "dsi_nt35597_truly_dsc_video_display:",  DISP_INTF_DSI, Truly_TFT2P2827_E_video_dsc_xmldata,   DISP_TOPOLOGY_CONFIG_1,  DISP_TIMING_CONFIG_NONE,    PLL_OVERRIDE_PLL1, DISP_MODE_SINGLE_DSI | DISP_MODE_USE_DSI1,         DISP_MODE_SINGLE_DSI | DISP_MODE_USE_DSI1,            DISP_MODE_SINGLE_DSI | DISP_MODE_USE_DSI1 | DISP_MODE_DSC_MERGE),


  /*Truly 2k NT35597 cmd*/
  PANEL_CREATE_ENTRY("truly_wqxga_dual_cmd",   MDPPLATFORM_PANEL_TRULY_WQXGA_DUALDSI_CMD,   "dsi_dual_nt35597_truly_cmd_display:",DISP_INTF_DSI, Truly_TFT2P2827_E_cmd_xmldata,       DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE,    PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI,                                DISP_MODE_DUAL_DSI,                                   DISP_MODE_DUAL_DSI),

  /*Truly 2k NT35597 video*/
  PANEL_CREATE_ENTRY("truly_wqxga_dual_vid",   MDPPLATFORM_PANEL_TRULY_WQXGA_DUALDSI_VIDEO,   "dsi_dual_nt35597_truly_video_display:",DISP_INTF_DSI, Truly_TFT2P2827_E_video_xmldata,       DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE,    PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI,                                DISP_MODE_DUAL_DSI,                                   DISP_MODE_DUAL_DSI),

  /* Sharp 1080p cmd */
  PANEL_CREATE_ENTRY("sharp_1080p_cmd",   MDPPLATFORM_PANEL_SHARP_1080P_CMD,   "dsi_sharp_1080_cmd_display:",DISP_INTF_DSI, Sharp_1080p_cmd_xmldata,       DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE,    PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI,                                DISP_MODE_SINGLE_DSI,                                   DISP_MODE_SINGLE_DSI),

PANEL_CREATE_ENTRY("ss_mg2_wqhd_dsc_cmd",     MDPPLATFORM_PANEL_SAMSUNG_WQHD_AMOLED_DSC_CMD,     "dsi_samsung_wqhd_amoled_cmd_display:",      DISP_INTF_DSI, Samsung_wqhd_amoled_cmd_xmldata,                  DISP_TOPOLOGY_CONFIG_1,  DISP_TIMING_CONFIG_0,    PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI,                                DISP_MODE_DUAL_DSI,                                   DISP_MODE_DUAL_DSI),

PANEL_CREATE_ENTRY("ss_mg2_1080p_dsc_cmd",     MDPPLATFORM_PANEL_SAMSUNG_1080P_AMOLED_DSC_CMD,     "dsi_samsung_wqhd_amoled_cmd_fhd_display:",      DISP_INTF_DSI, Samsung_1080p_amoled_cmd_xmldata,                  DISP_TOPOLOGY_CONFIG_1,  DISP_TIMING_CONFIG_1,    PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI,                                DISP_MODE_DUAL_DSI,                                   DISP_MODE_DUAL_DSI),

PANEL_CREATE_ENTRY("ss_mg2_720p_cmd",     MDPPLATFORM_PANEL_SAMSUNG_720P_AMOLED_CMD,     "dsi_samsung_wqhd_amoled_cmd_hd_display:",      DISP_INTF_DSI, Samsung_720p_amoled_cmd_xmldata,                  DISP_TOPOLOGY_CONFIG_NONE,   DISP_TIMING_CONFIG_2,   PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI,                                DISP_MODE_SINGLE_DSI,                                   DISP_MODE_SINGLE_DSI),

PANEL_CREATE_ENTRY("ss_mg1_wqhd_dual_cmd",     MDPPLATFORM_PANEL_SAMSUNG_WQHD_AMOLED_DUAL_CMD,     "dsi_samsung_wqhd_amoled_dual_cmd_display:",      DISP_INTF_DSI, Samsung_wqhd_amoled_dual_cmd_xmldata,                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE,    PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI,                                DISP_MODE_SINGLE_DSI,                                   DISP_MODE_SINGLE_DSI),

  /* Truly 1080p video */
  PANEL_CREATE_ENTRY("nt35695b_truly_1080p_video", MDPPLATFORM_PANEL_TRULY_1080P_VIDEO,      "dsi_nt35695b_truly_fhd_video_display:", DISP_INTF_DSI, Truly_NT35695b_FHD_video_xmldata, DISP_TOPOLOGY_CONFIG_NONE, DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI),
  /* Truly 1080p cmd */
  PANEL_CREATE_ENTRY("nt35695b_truly_1080p_cmd",   MDPPLATFORM_PANEL_TRULY_1080P_CMD,        "dsi_nt35695b_truly_fhd_cmd_display:",   DISP_INTF_DSI, Truly_NT35695b_FHD_cmd_xmldata,   DISP_TOPOLOGY_CONFIG_NONE, DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI),
  /* FHD Amoled cmd */
  PANEL_CREATE_ENTRY("rm67195_amoled_fhd_cmd",     MDPPLATFORM_PANEL_RM67195_AMOLED_FHD_CMD, "dsi_rm67195_amoled_fhd_cmd_display:",   DISP_INTF_DSI, Rm67195_amoled_fhd_cmd_xmldata,   DISP_TOPOLOGY_CONFIG_NONE, DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI),

  /* Truly NT36850 wqhd cmd */
  PANEL_CREATE_ENTRY("nt36850_truly_wqhd_cmd",     MDPPLATFORM_PANEL_NT36850_TRULY_WQHD_CMD, "dsi_dual_nt36850_truly_cmd_display:",   DISP_INTF_DSI, Truly_NT36850_WQHD_cmd_xmldata,   DISP_TOPOLOGY_CONFIG_NONE, DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI,   DISP_MODE_DUAL_DSI,   DISP_MODE_DUAL_DSI),

  /* Truly HX8399 fhd video */
  PANEL_CREATE_ENTRY("hx8399_truly_fhd_video",     MDPPLATFORM_PANEL_HX8399_TRULY_FHD_VIDEO, "dsi_hx8399_truly_fhd_video_display:",   DISP_INTF_DSI, Truly_HX8399_FHD_video_xmldata,   DISP_TOPOLOGY_CONFIG_NONE, DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI),

  /* LT9611 DSI2HDMI fhd video */
  PANEL_CREATE_ENTRY("lt9611_dsi2hdmi_fhd_video",	 MDPPLATFORM_PANEL_LT9611_DSI2HDMI_FHD_VIDEO, "dsi2hdmi_lt9611_fhd_video_display:",	DISP_INTF_DSI, LT9611_DSI2HDMI_FHD_video_xmldata, DISP_TOPOLOGY_CONFIG_NONE, DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI, DISP_MODE_SINGLE_DSI),


  /*Simulation panels */
  /*sim video*/
  PANEL_CREATE_ENTRY("sim_vid_panel",             MDPPLATFORM_PANEL_SIM_VIDEO_PANEL,             "dsi_sim_vid_display:",                DISP_INTF_DSI, NULL,                                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER,  DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER,     DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER),
  /*sim dualdsi video*/
  PANEL_CREATE_ENTRY("sim_dual_vid_panel",     MDPPLATFORM_PANEL_SIM_DUALDSI_VIDEO_PANEL,     "dsi_dual_sim_vid_display:",           DISP_INTF_DSI, NULL,                                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,  DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,     DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER),
  /*sim CMD*/
  PANEL_CREATE_ENTRY("sim_cmd_panel",               MDPPLATFORM_PANEL_SIM_CMD_PANEL,               "dsi_sim_cmd_display:",                  DISP_INTF_DSI, NULL,                                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER,  DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER,     DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER),
  /*sim dualdsi CMD*/
  PANEL_CREATE_ENTRY("sim_dual_cmd_panel",       MDPPLATFORM_PANEL_SIM_DUALDSI_CMD_PANEL,       "dsi_dual_sim_cmd_display:",             DISP_INTF_DSI, NULL,                                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,  DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,     DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER),

  /* sim single dsc  with dsc compression 3.75 cmd */
  PANEL_CREATE_ENTRY("sim_dsc_375_cmd_panel",               MDPPLATFORM_PANEL_SIM_DSC_375_CMD_PANEL,               "dsi_sim_dsc_375_cmd_display:",                  DISP_INTF_DSI, NULL,                                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER,  DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER,     DISP_MODE_SINGLE_DSI | DISP_MODE_SKIP_BOOTLOADER),

  /* sim dual dsi with dsc compression 3.75 cmd */
  PANEL_CREATE_ENTRY("sim_dual_dsc_375_cmd_panel",       MDPPLATFORM_PANEL_SIM_DUAL_DSC_375_CMD_PANEL,       "dsi_dual_sim_dsc_375_cmd_display:",             DISP_INTF_DSI, NULL,                                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,  DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,     DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER),
  
  /*Skip mode only panels, SW render in UEFI*/
  /* Sharp 1080p cmd, 120Hz */
  PANEL_CREATE_ENTRY("sharp_120hz_1080p_cmd",   MDPPLATFORM_PANEL_SHARP_120HZ_1080P_CMD,   "dsi_dual_sharp_1080_120hz_cmd_display:",DISP_INTF_DSI, Sharp_1080p_cmd_xmldata,       DISP_TOPOLOGY_CONFIG_NONE,   DISP_TIMING_CONFIG_NONE,   PLL_OVERRIDE_NONE, DISP_MODE_SINGLE_DSI,                                DISP_MODE_SINGLE_DSI,                                   DISP_MODE_SINGLE_DSI),
  /* Sharp wqhd cmd and video */
  PANEL_CREATE_ENTRY("sharp_wqhd_dual_cmd",     MDPPLATFORM_PANEL_SHARP_WQHD_DUALDSI_CMD,  "dsi_dual_nt35597_cmd_display:",         DISP_INTF_DSI,                    NULL,       DISP_TOPOLOGY_CONFIG_NONE,   DISP_TIMING_CONFIG_NONE,   PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,    DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,       DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER),
  PANEL_CREATE_ENTRY("sharp_wqhd_dual_vid",     MDPPLATFORM_PANEL_SHARP_WQHD_DUALDSI_VIDEO,"dsi_dual_nt35597_video_display:",       DISP_INTF_DSI,                    NULL,       DISP_TOPOLOGY_CONFIG_NONE,   DISP_TIMING_CONFIG_NONE,   PLL_OVERRIDE_NONE, DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,    DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,       DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER),

  /*End of Table, DO NOT ADD PANEL BELOW THIS*/
  PANEL_CREATE_ENTRY("",                            MDPPLATFORM_PANEL_NONE,                         "",                                      DISP_INTF_NONE,NULL,                                  DISP_TOPOLOGY_CONFIG_NONE,  DISP_TIMING_CONFIG_NONE, PLL_OVERRIDE_NONE, DISP_MODE_NONE,                                    DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER,     DISP_MODE_DUAL_DSI   | DISP_MODE_SKIP_BOOTLOADER),
};
/*===========================================================================
Function Definitions
===========================================================================*/
/**********************************************************************************************
*
* FUNCTION: FindPanelIndex()
*
* DESCRIPTION:
*	The function convert panel id into Index to panel table index
*
***********************************************************************************************/
static MDP_Status FindPanelIndex(MDPPlatformPanelInfo *pPlatformPanel)
{
	MDP_Status eStatus						  = MDP_STATUS_NOT_SUPPORTED;
	UINT32	   uIndex						  = 0;

    for (uIndex = 0; uIndex < PANEL_INFOLIST_LENGTH(gPanelList); uIndex++)
    {
      if (gPanelList[uIndex].ePanel == pPlatformPanel->eSelectedPanel)
      {
        pPlatformPanel->uSelectedPanelIndex = uIndex;
        eStatus	 = MDP_STATUS_OK;
        break;
      }
    }
    return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: PlatformClientInit()
** 
** DESCRIPTION:
**        Initialize NPA client
**
**/
static MDP_Status PlatformClientInit(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo* pPlatformInfo)
{
  MDP_Status    Status = MDP_STATUS_OK;

  NPAClientName aNPAClientName[MDP_DISPLAY_MAX] =
  {
    {PMIC_NPA_GROUP_ID_DISP_PRIM,     "DisplayPrim"},
    {PMIC_NPA_GROUP_ID_DISP_SEC,      "DisplaySec"},
    {PMIC_NPA_GROUP_ID_DISP_EXT_DP,   "DisplayExt"},
  };

  if ((NULL                                  != pPlatformInfo)                           &&
     ((EFICHIPINFO_FAMILY_QCS605             == pPlatformInfo->sEFIChipSetFamily)        &&
     ((EFICHIPINFO_FAMILY_QCS605_LC_VARIANT  == pPlatformInfo->sEFIPlatformType.subtype) ||
      (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA  == pPlatformInfo->sEFIPlatformType.subtype))))
  {
    aNPAClientName[MDP_DISPLAY_PRIMARY].cResourceName = PMIC_NPA_GROUP_ID_DISP_EXT_HDMI;
    aNPAClientName[MDP_DISPLAY_PRIMARY].cClientName   = "DisplayExtHdmi";
  }
  else if (MDP_PMIC_MODULE_CONTROLTYPE_PM_AMOLED == pPowerParams->ePMICSecondaryPower[eDisplayId])
  {
    /* Use PMIC_NPA_GROUP_ID_DISP_PRIM_AMOLED for OLED Panel */
    aNPAClientName[MDP_DISPLAY_PRIMARY].cResourceName = PMIC_NPA_GROUP_ID_DISP_PRIM_AMOLED;
    aNPAClientName[MDP_DISPLAY_PRIMARY].cClientName   = "DisplayPrimAmoled";
  }

  if (eDisplayId >= MDP_DISPLAY_MAX )
  {
    DEBUG ((EFI_D_ERROR, "DisplayDxe: Unsupported Display ID for power init.\n"));
    Status =  MDP_STATUS_FAILED;
  }
  else if (NULL == pPowerParams->sNPAClient[eDisplayId])
  {
    pPowerParams->sNPAClient[eDisplayId] = npa_create_sync_client( aNPAClientName[eDisplayId].cResourceName, aNPAClientName[eDisplayId].cClientName, NPA_CLIENT_REQUIRED); 
    
    if (NULL == pPowerParams->sNPAClient[eDisplayId])
    {
      DEBUG ((EFI_D_ERROR, "DisplayDxe: Failed to retrieve NPA Display Handle for Display ID %x.\n", eDisplayId ));
      Status =  MDP_STATUS_FAILED;
    }
  }      

  return Status;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PowerUp()
** 
** DESCRIPTION:
**        Panel power up sequence for CDP
**
**/
static MDP_Status Panel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo* pPlatformInfo)
{
  MDP_Status                   Status           = MDP_STATUS_OK;
  EFI_TLMM_PROTOCOL           *TLMMProtocol     = NULL;
  EFI_QCOM_PMIC_GPIO_PROTOCOL *PmicGpioProtocol = NULL;

  if (MDP_STATUS_OK != (Status =  PlatformClientInit(eDisplayId, pPowerParams, pPlatformInfo)))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Failed to initialize handle for Primary display NPA node.\n"));
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gEfiTLMMProtocolGuid, NULL, (void **)&TLMMProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate TLMM protocol failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicGpioProtocolGuid, NULL, (VOID **)&PmicGpioProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate PMIC GPIO protocol failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else
  {
    if ((NULL                   != pPlatformInfo) &&
        (EFICHIPINFO_ID_SXR1120 == pPlatformInfo->sEFIChipSetId))
    {
      /* Set SXR1120 platform enable gpio */
      if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(CDP_DISP_SXR1120_ENABLE_GPIO, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_16MA), TLMM_GPIO_ENABLE))
      {
        DEBUG((EFI_D_WARN, "DisplayDxe: Configure GPIO %d for CDP_DISP_SXR1120_ENABLE_GPIO Failed!\n", CDP_DISP_MODE_SEL));
      }

      if (EFI_SUCCESS != TLMMProtocol->GpioOut((UINT32)EFI_GPIO_CFG(CDP_DISP_SXR1120_ENABLE_GPIO, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), GPIO_LOW_VALUE))
      {
        DEBUG((EFI_D_WARN, "DisplayDxe: CDP_DISP_SXR1120_ENABLE_GPIO line Low failed!\n"));
      }
    
    }

    if ((NULL                                    != pPlatformInfo) &&
       ((EFICHIPINFO_FAMILY_QCS605               == pPlatformInfo->sEFIChipSetFamily)        &&
       ((EFICHIPINFO_FAMILY_QCS605_LC_VARIANT    == pPlatformInfo->sEFIPlatformType.subtype) ||
        (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA    == pPlatformInfo->sEFIPlatformType.subtype))))
    {
      /* Configure LT9611 3.3V ldo enable gpio */
      if (EFI_SUCCESS != TLMMProtocol->ConfigGpio(EFI_GPIO_CFG(CDP_DISP_LT9611_ENABLE_GPIO, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), TLMM_GPIO_ENABLE))
      {
        DEBUG ((EFI_D_WARN, "DisplayDxe: Configure CDP_DISP_LT9611_ENABLE_GPIO failed!\n"));
      }

      if (EFI_SUCCESS != TLMMProtocol->GpioOut(EFI_GPIO_CFG(CDP_DISP_LT9611_ENABLE_GPIO, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), GPIO_HIGH_VALUE))
      {
        DEBUG ((EFI_D_WARN, "DisplayDxe: Set CDP_DISP_LT9611_ENABLE_GPIO to high failed\n"));
      }

      MDP_OSAL_DELAYMS(20);
    }

   /*
    * Voting for Display NPA node to be ON
    */
    npa_issue_required_request(pPowerParams->sNPAClient[eDisplayId], PMIC_NPA_MODE_ID_GENERIC_ACTIVE);

    MDP_OSAL_DELAYMS(10);   /* delay 10ms to allow power grid to settle */


    /* Set display mode select (dual DSI or single DSI /w DSC) */
    if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(CDP_DISP_MODE_SEL, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_16MA), TLMM_GPIO_ENABLE))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Configure GPIO %d for DISP_MODE_SEL Failed!\n", CDP_DISP_MODE_SEL));
    }

    /* Default to low, dual DSI */
    if (EFI_SUCCESS != TLMMProtocol->GpioOut((UINT32)EFI_GPIO_CFG(CDP_DISP_MODE_SEL, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), GPIO_LOW_VALUE))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: DISP_MODE_SEL line Low failed!\n"));
    }

    /* Display TE pin */
    if (EFI_SUCCESS != TLMMProtocol->ConfigGpio(EFI_GPIO_CFG(CDP_DISP_TE_GPIO, 1, GPIO_INPUT, GPIO_NO_PULL, GPIO_2MA), TLMM_GPIO_ENABLE))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Configure GPIO %d for TE line failed %d\n", CDP_DISP_TE_GPIO));
    }

  }

  return Status;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PowerDown()
** 
** DESCRIPTION:
**        Panel power down sequence for CDP
**
**/
static MDP_Status Panel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo* pPlatformInfo)
{
  MDP_Status                   Status           = MDP_STATUS_OK;
  EFI_TLMM_PROTOCOL           *TLMMProtocol     = NULL;
  EFI_QCOM_PMIC_GPIO_PROTOCOL *PmicGpioProtocol = NULL;

  if (NULL == pPowerParams->sNPAClient[eDisplayId])
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: NULL Handle for Primary display NPA node.\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gEfiTLMMProtocolGuid, NULL, (void **)&TLMMProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate TLMM protocol failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicGpioProtocolGuid, NULL, (VOID **)&PmicGpioProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate PMIC GPIO protocol failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else
  {
    /* 
     * GPIOs & VRegs
     */
    /* Display RESET_N pin */
    if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(pPowerParams->uResetGpio, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_16MA), TLMM_GPIO_DISABLE))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Failed to disable GPIO %d for Reset_N line\n", pPowerParams->uResetGpio));
    }

    /* Display TE pin */
    if (EFI_SUCCESS != TLMMProtocol->ConfigGpio(EFI_GPIO_CFG(CDP_DISP_TE_GPIO, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), TLMM_GPIO_DISABLE))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Failed to disable GPIO %d for TE pin\n", CDP_DISP_TE_GPIO));
    }

    if ((NULL                                     != pPlatformInfo) &&
        ((EFICHIPINFO_FAMILY_QCS605               == pPlatformInfo->sEFIChipSetFamily)        &&
        ((EFICHIPINFO_FAMILY_QCS605_LC_VARIANT    == pPlatformInfo->sEFIPlatformType.subtype) ||
         (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA    == pPlatformInfo->sEFIPlatformType.subtype))))
    {
      /* Configure LT9611 3.3V ldo enable gpio */
      if (EFI_SUCCESS != TLMMProtocol->ConfigGpio(EFI_GPIO_CFG(CDP_DISP_LT9611_ENABLE_GPIO, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), TLMM_GPIO_DISABLE))
      {
         DEBUG ((EFI_D_ERROR, "DisplayDxe: Failed to disable CDP_DISP_LT9611_ENABLE_GPIO\n"));
      }
    }

    /*
    * Voltage Regulators
    */
    npa_complete_request(pPowerParams->sNPAClient[eDisplayId]);           //Complete the request to power rails
  }

  return Status;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_ResetStateExec()
** 
** DESCRIPTION:
**        toggle reset gpio base on states transition
**
*/ /* -------------------------------------------------------------------- */
static void Panel_ResetStateExec(EFI_TLMM_PROTOCOL *tlmm, MDPPlatformPanelResetInfo *pResetInfo, uint32 state0, uint32 state1, uint32 state2)
{
    if (EFI_SUCCESS != tlmm->GpioOut((UINT32)EFI_GPIO_CFG(pResetInfo->uResetGpio, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA), state0))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Reset_N line FAILED at state-0!\n"));
    }

    MDP_OSAL_DELAYUS(pResetInfo->uPreResetTimeUs);

    if (EFI_SUCCESS != tlmm->GpioOut((UINT32)EFI_GPIO_CFG(pResetInfo->uResetGpio, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA), state1))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Reset_N line FAILED at state-1!\n"));
    }

    MDP_OSAL_DELAYUS(pResetInfo->uResetAssertTimeUs);

    if (EFI_SUCCESS != tlmm->GpioOut((UINT32)EFI_GPIO_CFG(pResetInfo->uResetGpio, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), state2))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Reset_N line FAILED at state-2!\n"));
    }
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_Reset()
** 
** DESCRIPTION:
**        Panel reset sequence for CDP
**
**/
MDP_Status Panel_CDP_Reset(MDPPlatformPanelResetInfo *pResetInfo)
{
  MDP_Status                    Status           = MDP_STATUS_OK;
  EFI_TLMM_PROTOCOL            *TLMMProtocol     = NULL;

  if (EFI_SUCCESS != gBS->LocateProtocol(&gEfiTLMMProtocolGuid, NULL, (void **)&TLMMProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate TLMM protocol failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else
  {
    /* Enable Display Reset pin*/
    if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(pResetInfo->uResetGpio, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA), TLMM_GPIO_ENABLE))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: Configure GPIO %d for Reset_N line Failed!\n", pResetInfo->uResetGpio));
    }

    switch (pResetInfo->uAssertState)
    {
      case MDP_RESET_STATE_EDGE_LOW2HIGH:
         Panel_ResetStateExec(TLMMProtocol, pResetInfo, GPIO_LOW_VALUE, GPIO_HIGH_VALUE, GPIO_HIGH_VALUE);
       break;
      case MDP_RESET_STATE_EDGE_HIGH2LOW:
         Panel_ResetStateExec(TLMMProtocol, pResetInfo, GPIO_HIGH_VALUE, GPIO_LOW_VALUE, GPIO_LOW_VALUE);
       break;
      case MDP_RESET_STATE_LEVEL_HIGH:
         Panel_ResetStateExec(TLMMProtocol, pResetInfo, GPIO_LOW_VALUE, GPIO_HIGH_VALUE, GPIO_LOW_VALUE);
       break;
      case MDP_RESET_STATE_LEVEL_LOW:
      default:
         Panel_ResetStateExec(TLMMProtocol, pResetInfo, GPIO_HIGH_VALUE, GPIO_LOW_VALUE, GPIO_HIGH_VALUE);
       break;
    }

    MDP_OSAL_DELAYUS(pResetInfo->uPostResetTimeUs);

    /*
     * post reset time: Timing space between end of reset to start sending init dcs cmds
     * NOTE:
     *   use DSIInitMasterTime to extend post reset time control if required
     */
  }

  return Status;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PmicOledPostInit()
** 
** DESCRIPTION:
**        Any Oledb initialization needed after panel init sequence is sent & OLEDB is enabled
**
**/
static MDP_Status Panel_CDP_PmicOledPostInit(MDP_Display_IDType eDisplayId)
{ 
  MDP_Status                    Status           = MDP_STATUS_OK;
  EFI_QCOM_PMIC_OLEDB_PROTOCOL *PmicOledProtocol = NULL;
  EFI_QCOM_PMIC_LAB_PROTOCOL   *PmicLABProtocol  = NULL; 
        
  if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicOledbProtocolGuid, NULL, (VOID **)&PmicOledProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate OLED Protocol Failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicLabProtocolGuid, NULL, (VOID **)&PmicLABProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate PMIC LAB Protocol Failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else 
  {
    EFI_PM_LAB_STATUS_TYPE   sLabStatus;
    uint32                   uI;
     
    // LAB config
    
    MDP_OSAL_MEMZERO((void *) &sLabStatus, sizeof(EFI_PM_LAB_STATUS_TYPE));

    /* Check Lab Status in a Loop as the SWIRE signal generation time is not consistent
	   after sending panel init commands */
    for (uI = 0; uI < 30; ++uI)
    {
      if (EFI_SUCCESS != PmicLABProtocol->GetStatus(PMIC_PMI_DEV_INDEX, &sLabStatus))
      {
        DEBUG((EFI_D_ERROR, "DisplayDxe: Error getting LAB status\n"));
        break;
      }
      else if (sLabStatus.LabVregOk)
      {
        break;
      }

      MDP_OSAL_DELAYMS(5);
    }

    if (!sLabStatus.LabVregOk)
    {    
      DEBUG((EFI_D_ERROR, "DisplayDxe: LAB_VREG_OK is not TRUE - Disabling oled module\n"));  

      /* LAB VREG is not ok - Disable modules to avoid short circuit or other issues */
      
      if (EFI_SUCCESS != PmicLABProtocol->ModuleRdy(PMIC_PMI_DEV_INDEX, FALSE))
      {
        DEBUG((EFI_D_WARN, "DisplayDxe: Error disabling LAB \n"));
      } 
      
      MDP_OSAL_DELAYUS(100);
  
      if (EFI_SUCCESS != PmicOledProtocol->OledbSetModuleRdy(PMIC_PMI_DEV_INDEX, FALSE))
      {
        DEBUG((EFI_D_WARN, "DisplayDxe: Error disabling OLEDB\n"));
      }      
    }
    else
    {  
      // OLEDB config
  
      /* Do not listen to SWIRE for pulldown feature */
      if (EFI_SUCCESS != PmicOledProtocol->OledbSwireCtl(PMIC_PMI_DEV_INDEX, EFI_PM_OLEDB_SWIRE_CONTROL__PD_EN, FALSE))
      {
        DEBUG((EFI_D_ERROR, "DisplayDxe: Error configuring Oledb Swire Ctrl \n"));
      }
  
      /* Enable Discharge resistor */
      if (EFI_SUCCESS != PmicOledProtocol->OledbPdCtrl(PMIC_PMI_DEV_INDEX, TRUE))
      {
        DEBUG((EFI_D_ERROR, "DisplayDxe: Error to enable OLEDB\n"));
      } 
    }
  }

  return Status;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PeripheralPower()
** 
** DESCRIPTION:
**        Secondary power sequence for other PMIC modules such as PM WLED / OLED
**
**/
static MDP_Status Panel_CDP_PeripheralPower(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, bool32 bPowerUp)
{
  MDP_Status                      Status              = MDP_STATUS_OK;
  
  switch (pPowerParams->ePMICSecondaryPower[eDisplayId])
  {
    case MDP_PMIC_MODULE_CONTROLTYPE_PM_LCD_WLED:
      {
        // TODO : Add PMIC support check (different PMIC devices are needed for LCD vs. OLED)
        
        EFI_QCOM_PMIC_WLED_PROTOCOL  *PmicWledProtocol = NULL;
        EFI_QCOM_PMIC_LCDB_PROTOCOL  *PmicLcdbProtocol = NULL;    
    
        if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicWledProtocolGuid, NULL, (VOID **)&PmicWledProtocol))
        {
          DEBUG((EFI_D_ERROR, "DisplayDxe: Locate WLED Protocol Failed!\n"));
          Status = MDP_STATUS_NO_RESOURCES;
        }
        else if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicLcdbProtocolGuid, NULL, (VOID **)&PmicLcdbProtocol))
        {
          DEBUG((EFI_D_ERROR, "DisplayDxe: Locate PMIC LCDB Protocol Failed!\n"));
          Status = MDP_STATUS_NO_RESOURCES;
        }
        else if (TRUE == bPowerUp)
        {
          // Power up
    
          /************************************************************************/
          /* Turn On + and - Bias for LCD                                         */
          /************************************************************************/
          //Enable LCDB module
          if (EFI_SUCCESS != PmicLcdbProtocol->LcdbModuleEnable(PMIC_PMI_DEV_INDEX, TRUE))
          {
            DEBUG((EFI_D_ERROR, "DisplayDxe: Error to config LCDB to LCD mode\n"));
          }
    
          /************************************************************************
           *  Enable secondary power source
           ************************************************************************/
    
          //0xD84F             EN_CURRENT_SINK         0xE0       Enable all 3 strings
		  if (pPowerParams->uWledStringsPrimary & EFI_PM_WLED_LED1) 
	  	  {
		    if (EFI_SUCCESS != PmicWledProtocol->EnableCurrentSink(PMIC_PMI_DEV_INDEX, EFI_PM_WLED_LED1))
		    {
			  DEBUG((EFI_D_ERROR, "Wled Protocol EnableCurrentSink led2 failed\n"));
		    }
		  }

		  if (pPowerParams->uWledStringsPrimary & EFI_PM_WLED_LED2) 
		  {
		  	if (EFI_SUCCESS != PmicWledProtocol->EnableCurrentSink(PMIC_PMI_DEV_INDEX, EFI_PM_WLED_LED2))
            {
              DEBUG((EFI_D_ERROR, "Wled Protocol EnableCurrentSink led2 failed\n"));
            }
		  }
		  if (pPowerParams->uWledStringsPrimary & EFI_PM_WLED_LED3) 
		  {
		  	if (EFI_SUCCESS != PmicWledProtocol->EnableCurrentSink(PMIC_PMI_DEV_INDEX, EFI_PM_WLED_LED3))
            {
              DEBUG((EFI_D_ERROR, "Wled Protocol EnableCurrentSink led2 failed\n"));
            }
		  }
          if (pPowerParams->uWledStringsPrimary & EFI_PM_WLED_LED4) 
		  {
		  	if (EFI_SUCCESS != PmicWledProtocol->EnableCurrentSink(PMIC_PMI_DEV_INDEX, EFI_PM_WLED_LED4))
            {
              DEBUG((EFI_D_ERROR, "Wled Protocol EnableCurrentSink led2 failed\n"));
            }
		  }
    
          if (EFI_SUCCESS != PmicWledProtocol->EnableModulator(PMIC_PMI_DEV_INDEX, pPowerParams->uWledStringsPrimary, TRUE))
          {
            DEBUG((EFI_D_ERROR, "Wled Protocol EnableModulator failed\n"));
          }
    
          if (EFI_SUCCESS != PmicWledProtocol->EnableSync(PMIC_PMI_DEV_INDEX, pPowerParams->uWledStringsPrimary, TRUE))
          {
            DEBUG((EFI_D_ERROR, "Wled Protocol EnableSync failed\n"));
          }
    
          //0xD846             MODULE_EN      0x80       Enable the entire module
          if (EFI_SUCCESS != PmicWledProtocol->EnableWled(PMIC_PMI_DEV_INDEX, 0, TRUE))
          {
            DEBUG((EFI_D_ERROR, "Wled Protocol EnableWled failed\n"));
          }
        }
        else
        {
          // Power down
          if (EFI_SUCCESS != PmicLcdbProtocol->LcdbModuleEnable(PMIC_PMI_DEV_INDEX, FALSE))
          {
            DEBUG((EFI_D_ERROR, "DisplayDxe: Error to config LCDB to LCD mode\n"));
          }
  
          if (EFI_SUCCESS != PmicWledProtocol->EnableWled(PMIC_PMI_DEV_INDEX, 0, FALSE))
          {
            DEBUG((EFI_D_ERROR, "Wled Protocol EnableWled failed\n"));
          }
        }
      }
      break;

    case MDP_PMIC_MODULE_CONTROLTYPE_PM_AMOLED:
      {
        if (FALSE == bPowerUp)
        {
          // Power down case - execute PBS sequence
          
          EFI_QCOM_PMIC_OLEDB_PROTOCOL *PmicOledProtocol = NULL;
          
          if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicOledbProtocolGuid, NULL, (VOID **)&PmicOledProtocol))
          {
            DEBUG((EFI_D_ERROR, "DisplayDxe: Locate OLED Protocol Failed!\n"));
            Status = MDP_STATUS_NO_RESOURCES;
          }           
          else if (EFI_SUCCESS != PmicOledProtocol->OledbDispOffPbsTrigger(PMIC_PMI_DEV_INDEX))
          {
            DEBUG((EFI_D_ERROR, "DisplayDxe: Error triggering PBS sequence\n"));
          }
        }
        
        // For Power up, All OLED config will be triggered based on hardware pin signal
        // No configuration is needed        
      }
      break;

    default:
      break;
  }

  return Status;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_BacklightLevel()
**
** DESCRIPTION:
**
**/
static MDP_Status Panel_CDP_BacklightLevel(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, BacklightConfigType *pBacklightConfig)
{
  MDP_Status                    Status = MDP_STATUS_OK;

  if (MDP_PANEL_BACKLIGHTTYPE_PMIC == pBacklightConfig->eBacklightType)
  {
    // Configure the power grid based on the module type
    switch (pBacklightConfig->uBacklightCntrl.eBacklightCtrl)
    {
    case MDP_PMIC_BACKLIGHT_CONTROLTYPE_WLED:
      {
        EFI_QCOM_PMIC_WLED_PROTOCOL  *PmicWledProtocol = NULL;

        if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomPmicWledProtocolGuid, NULL, (VOID **)&PmicWledProtocol))
        {
          DEBUG((EFI_D_ERROR, "DisplayDxe: Locate WLED Protocol Failed!\n"));
          Status = MDP_STATUS_NO_RESOURCES;
        }
        else
        {
          uint32 uWLEDValue = (0xFFF * pBacklightConfig->uLevel) / 100; // Calculate duty cycle based on 12 bit mode

          if (EFI_SUCCESS != PmicWledProtocol->SetLedDutyCycle(PMIC_PMI_DEV_INDEX, pPowerParams->uWledStringsPrimary, uWLEDValue))
          {
            DEBUG((EFI_D_ERROR, "Wled Protocol SetLedDutyCycle failed\n"));
          }
        }
      }
      break;
    default:
      break;
    }
  }
  else if (MDP_PANEL_BACKLIGHTTYPE_DSI == pBacklightConfig->eBacklightType)
  {
    PlatformPanelBacklightControl panelBacklightControl = {
      {0x51, 0x80}, // Using standard MIPI DCS commands to control brightness
    };

    // Calculate brightness based on 8 bit mode
    panelBacklightControl.command[1] = DISP_DIV_ROUND_UP(0xFF * pBacklightConfig->uLevel, 100);

    Status = DSIDriver_Write(0x15,
                 panelBacklightControl.command,
                 sizeof(panelBacklightControl.command));
    if (MDP_STATUS_OK != Status)
    {
      DEBUG((EFI_D_ERROR, "Backlight brightness DCS command send failed\n"));
    }
  }
  else
  {
    // Nothing to do for other configurations
  }

  return Status;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CheckOverride()
** 
** DESCRIPTION:
**  Check if the panel was overridden by ABL (apps boot loader) and if so check 
**  if the overridden panel is supported by this platform.
**
**  This function has 1 of 3 outcomes
**  1. panel is supported by both bootloader & kernel 
**     - Set matching xmlconfig
**  2. panel is only supported in kernel 
**     - Return dummyconfig, since sw rendering was already selected during initialization in this scenario
**  3. panel override is not set or the panel is not supported by bootloader & kernel
**     - Return FALSE to trigger panel detection
**/
static bool32 Panel_CheckOverride(MDP_Display_IDType eDisplayId, MDPPlatformParams *pPlatformParams)
{
  Display_UtilsParamsType* pDisplayOverride = Display_Utils_GetParams();
  bool32                   bRet             = FALSE;

  if ((NULL != pDisplayOverride)                           && 
      (NULL != pDisplayOverride->sPrimary.psDTInfo)        &&
      (MDPPLATFORM_PANEL_NONE < pDisplayOverride->sPrimary.psDTInfo->ePanel))
  {
    // Lookup panel
    int32 i = PANEL_INFOLIST_LENGTH(gPanelList) - 1;

    while (0 <= i)
    {
      if (pDisplayOverride->sPrimary.psDTInfo->ePanel == gPanelList[i].ePanel)
      {
        // Match found
        bRet = TRUE;
        if (NULL != gPanelList[i].pPanelXMLConfig)
        {
          // panel is supported in boot loader
          pPlatformParams->sPlatformPanel.pPanelXMLConfig  = gPanelList[i].pPanelXMLConfig;
          pPlatformParams->sPlatformPanel.uConfigSize      = gPanelList[i].uConfigSize;
          pPlatformParams->sPlatformPanel.eSelectedPanel   = gPanelList[i].ePanel;
          if (pDisplayOverride->uFlags & DISP_MODE_SKIP_BOOTLOADER)
          {
            pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8 *)dummy_xmldata;
            pPlatformParams->sPlatformPanel.uConfigSize     = sizeof(dummy_xmldata);
            pPlatformParams->sPlatformInfo.bSWRender        = TRUE;
            DEBUG((EFI_D_ERROR, "DisplayDxe: Panel override <%a> in skip mode\n", pDisplayOverride->sPrimary.shortName));
          }
          else
          {
            DEBUG((EFI_D_ERROR, "DisplayDxe: Panel override <%a>\n", pDisplayOverride->sPrimary.shortName));
          }
        }
        else
        {
          // panel is supported at kernel but not supported in bootloader
          pPlatformParams->sPlatformPanel.pPanelXMLConfig  = (int8 *)dummy_xmldata;
          pPlatformParams->sPlatformPanel.uConfigSize      = sizeof(dummy_xmldata);
          pPlatformParams->sPlatformPanel.eSelectedPanel   = gPanelList[i].ePanel; /* keep panel id */
          DEBUG((EFI_D_ERROR, "DisplayDxe: overridden panel <%a> is not supported\n", pDisplayOverride->sPrimary.shortName));
        }
        break;
      }
      i--;
    }
  }
  
  return bRet;
}
  

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DynamicDSIPanelDetection()
** 
** DESCRIPTION:
**  Detect DSI panels by doing a DSI read specific to each panels.
**  This function could be used as sample for OEM to detect DSI panels, 
**  it is not a complete implementation of all possible combinations of read
**  commands that could be needed for this detection.
**   
**  Return success only if a DSI panel was correctly detected and the information 
**  is updated in pPlatformParams->sPlatformPanel
**/
static MDP_Status DynamicDSIPanelDetection(MDPPlatformParams *pPlatformParams, uint32 *puPanelID, uint32 uOverrideId, PlatformDSIDetectParams *pPanelList, uint32 uPanelCnt)
{
  MDP_Status             Status                = MDP_STATUS_NOT_SUPPORTED;
  bool32                 bDumpPanelId          = FALSE;
  
  // If override is used, search for the panel that matches the override
  if (uOverrideId)
  {
    uint32      uPanelIndex;
    
    for (uPanelIndex = 0; uPanelIndex < uPanelCnt; uPanelIndex++)
    {
        uint32 uPanelId = (pPanelList[uPanelIndex].panelIdCommands[0].expectedReadback[0] << 16) |
                          (pPanelList[uPanelIndex].panelIdCommands[1].expectedReadback[0] << 8)  |
                          (pPanelList[uPanelIndex].panelIdCommands[2].expectedReadback[0]);

        if (uPanelId == uOverrideId)
        {
          // store matched panel configuration xml data
          pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8*) pPanelList[uPanelIndex].psPanelCfg;
          pPlatformParams->sPlatformPanel.uConfigSize     = pPanelList[uPanelIndex].uPanelCfgSize;
          pPlatformParams->sPlatformPanel.eSelectedPanel  = pPanelList[uPanelIndex].eSelectedPanel;
        
          *puPanelID = uPanelId;
          Status     = MDP_STATUS_OK;
          break;
        }
    }  

    if (MDP_STATUS_OK != Status)
    {
      DEBUG((EFI_D_ERROR, "Display: Panel override failed to find a matching panel ID.\n"));      
    }
    
  }
  else if (MDP_STATUS_OK == DSIDriver_MinimalInit())          // do minimal DSI init
  {
    uint8       panelID[PLATFORM_PANEL_ID_MAX_COMMANDS];
    uint32      uPanelIndex;
    bool32      bMatch         = FALSE;
    uint32      uPrevClkConfig = 0; 

    // go through all possible panels
    for (uPanelIndex = 0; uPanelIndex < uPanelCnt; uPanelIndex++)
    {
      uint8     readback[DSI_READ_READBACK_SIZE];
      uint32    readSize                          = sizeof(readback);
      int       iCommandIndex                     = 0;
      uint32    uClkConfig                        = (MDPPLATFORM_PANEL_DETECT_FLAG_CLOCK_FORCEHS & 
                                                     pPanelList[uPanelIndex].uFlags);

      // Check if there is any change in the clock config and set it accordingly
      if (uPrevClkConfig != uClkConfig)
      {
        if (MDP_STATUS_OK != DSIDriver_ConfigClockLane(uClkConfig))
        {
          DEBUG((EFI_D_ERROR, "Display: DSIDriver_ConfigClockLane failed\n"));
        }
        
        uPrevClkConfig = uClkConfig;
      }

      // Reprogram the DSI lane swap based on remap order
      if (MDP_STATUS_OK != DSIDriver_RemapLane(pPanelList[uPanelIndex].uLaneRemapOrder))
      {
        DEBUG((EFI_D_WARN, "Display: DSIDriver_RemapLane failed\n"));
      }

      // Allow debug option to scan panel registers (used to help generate a uniquie panel ID for detection)
      if (TRUE == bDumpPanelId)
      {
        DsiPanelDumpRegisters();
        // Dump just once
        bDumpPanelId = FALSE;
      }

      // clear the panel ID
      MDP_OSAL_MEMZERO(panelID, PLATFORM_PANEL_ID_MAX_COMMANDS);

      // for each possible panel ID read
      for(iCommandIndex = 0; iCommandIndex<PLATFORM_PANEL_ID_MAX_COMMANDS; iCommandIndex++)
      {
        uint32         uRetryCount = 0;

        // if read command is 0, then stop reading panel ID
        if ((0 == pPanelList[uPanelIndex].panelIdCommands[iCommandIndex].address[0]) &&
            (0 == pPanelList[uPanelIndex].panelIdCommands[iCommandIndex].address[1]) )
        {
          break;
        }
        // DSI read
        bMatch = FALSE;

        uRetryCount = 0;
        do
        {
          // clear the readback buffer
          MDP_OSAL_MEMZERO(&readback[0], readSize);
          readSize = sizeof(readback);
          Status = DSIDriver_Read(pPanelList[uPanelIndex].uCmdType, 
                                  pPanelList[uPanelIndex].panelIdCommands[iCommandIndex].address, 
                                  sizeof(pPanelList[uPanelIndex].panelIdCommands[iCommandIndex].address), 
                                  readback, 
                                  &readSize);
          uRetryCount++;
        } while((uRetryCount < pPanelList[uPanelIndex].uTotalRetry) && 
                ((MDP_STATUS_OK != Status)                                  || 
                 (0 == readSize)));

        if ((uRetryCount <= pPanelList[uPanelIndex].uTotalRetry) &&
            (0 != readSize))
        {
          // Read was successful, now check the data is what we expect
          if (0 == CompareMem(readback, pPanelList[uPanelIndex].panelIdCommands[iCommandIndex].expectedReadback, readSize))
          {
            panelID[iCommandIndex] = readback[0];    // store the first byte of readback as panel ID
            bMatch                 = TRUE;                       // mark one panel ID matched
          }
        }

        // if any panel ID is not matched, then go to detect next panel in the list
        if(FALSE == bMatch)
        {
          break;
        }
      }

      // if all panel IDs are matched for a specific panel, store settings and stop
      if(TRUE == bMatch)
      {
        // store matched panel configuration xml data
        pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8*) pPanelList[uPanelIndex].psPanelCfg;
        pPlatformParams->sPlatformPanel.uConfigSize     = pPanelList[uPanelIndex].uPanelCfgSize;
        pPlatformParams->sPlatformPanel.eSelectedPanel  = pPanelList[uPanelIndex].eSelectedPanel;

        // return the final combined panel ID
        *puPanelID = (panelID[0]<<16) | (panelID[1]<<8) | (panelID[2]);
        Status     = MDP_STATUS_OK;
        DEBUG((EFI_D_INFO, "Detected panel id:%08x\n", *puPanelID));
        break;
      }
      else
      {
        Status = MDP_STATUS_FAILED;
      }
    }

    // Close the DSI context opened in DSIDriver_MinimalInit()
    DSIDriver_Close();
  }
  return Status;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: PlatformConfigRemap()
** 
** DESCRIPTION:
**  Remapping of the platform configuration enumeration based on platform properties
**
**
**/
static MDPPlatformConfigType PlatformConfigRemap(MDPPlatformInfo *pPlatformConfig, MDPPlatformConfigType eConfig)
{
  MDPPlatformConfigType eRemappedConfig;

  // In SW render mode bypass all configuration that manipulate the hardware
  if ((NULL !=  pPlatformConfig) && 
      (TRUE == pPlatformConfig->bSWRender))
  {
    switch (eConfig)
    {
      case MDPPLATFORM_CONFIG_POWERUP:
      case MDPPLATFORM_CONFIG_POWERDOWN:
      case MDPPLATFORM_CONFIG_SETBACKLIGHT:
      case MDPPLATFORM_CONFIG_RESETPANEL:
      case MDPPLATFORM_CONFIG_SETGPIOSTATE:
        
        // In SW render mode don't execute these hardware configurations
        eRemappedConfig = MDPPLATFORM_CONFIG_BYPASS;
        
        break;
     default:
      
        // Default mapping (output = input)
        eRemappedConfig = eConfig;
        
        break;
   }
 }
 else
 {
    // Default mapping (output = input)
    eRemappedConfig = eConfig;
 }

 return eRemappedConfig;
}



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: panel_ConfigGPIO()
** 
** DESCRIPTION:
**   Initialize LCD panel GPIOs and PMICs
**
**/
MDP_Status MDPPlatformConfigure(MDP_Display_IDType eDisplayId, MDPPlatformConfigType eConfig, MDPPlatformParams *pPlatformParams)
{
  MDP_Status               eStatus               = MDP_STATUS_OK;
  uint32                   uPanelID              = 0;
  MDPPlatformConfigType    ePlatformConfig;
  PlatformDSIDetectParams  *pPanelList           = NULL;
  uint32                   uPanelCnt;

  /* Static information, initial once during the first call */
  static bool32                 bInitFlag        = FALSE;
  static MDPPlatformInfo        sPlatformInfo;
  static Panel_PowerCtrlParams  sPanelPowerCtrl;

  if (FALSE == bInitFlag)
  {
    MDP_OSAL_MEMZERO(&sPlatformInfo,    sizeof(MDPPlatformInfo));
    MDP_OSAL_MEMZERO(&sPanelPowerCtrl,  sizeof(Panel_PowerCtrlParams));
    bInitFlag = TRUE;
  }

  // Get remapped platform configuration enum
  ePlatformConfig = PlatformConfigRemap(&sPlatformInfo, eConfig);
      
  // Read the platform ID once
  if (FALSE == sPlatformInfo.bPlatformDetected)
  {
    if (MDP_STATUS_OK == ReadPlatformIDAndChipID(&sPlatformInfo.sEFIPlatformType, &sPlatformInfo.sEFIChipSetId, &sPlatformInfo.sEFIChipSetFamily))
    {
      UINT32 uPanelOverride = 0;
      // Read the display panel ID override, it will be checked later during detection
      if ((MDP_STATUS_OK  == MDP_Display_GetVariable_Integer (PANEL_OVERRIDE_STRINGNAME, &uPanelOverride)) &&
          (uPanelOverride  > 0))
      {
        DEBUG((EFI_D_ERROR, "DisplayDxe: Panel override enabled (ID:0x%x)\n", uPanelOverride));
        sPlatformInfo.uPanelIdOverride = (uint32) uPanelOverride;
      }
    }
  }
  
  switch (ePlatformConfig)
  {
  case MDPPLATFORM_CONFIG_SW_RENDERER:
  
    if (NULL == pPlatformParams)
    {
      eStatus = MDP_STATUS_BAD_PARAM;
    }
    else if (TRUE == sPlatformInfo.bSWRender)
    {
      // SW Render mode is enabled already, just return the status
      pPlatformParams->sPlatformInfo.bSWRender = TRUE;
    }
    else if ((EFI_PLATFORMINFO_TYPE_UNKNOWN  == sPlatformInfo.sEFIPlatformType.platform) ||
             (EFI_PLATFORMINFO_TYPE_VIRTIO   == sPlatformInfo.sEFIPlatformType.platform) ||
             (EFI_PLATFORMINFO_TYPE_RUMI     == sPlatformInfo.sEFIPlatformType.platform) ||
             ((EFICHIPINFO_FAMILY_QCS605                 == sPlatformInfo.sEFIChipSetFamily) &&
              (EFICHIPINFO_FAMILY_QCS605_LC_VARIANT      != sPlatformInfo.sEFIPlatformType.subtype) &&
              (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA      != sPlatformInfo.sEFIPlatformType.subtype) &&
              (EFICHIPINFO_FAMILY_QCS605_HP_VARIANT      != sPlatformInfo.sEFIPlatformType.subtype) &&
              (EFICHIPINFO_FAMILY_QCS605_INTERNAL_DEVICE != sPlatformInfo.sEFIPlatformType.subtype)) ||
             (TRUE == pPlatformParams->sPlatformInfo.bSWRenderOverrride) ||
             (TRUE == PcdGetBool(PcdDisplayForceSwRenderer)))
    {
      // Force SW render mode for emulation and unknown platforms
      pPlatformParams->sPlatformInfo.bSWRender = TRUE;
      // Store that an override has been requested by the caller
      sPlatformInfo.bSWRenderOverrride         = pPlatformParams->sPlatformInfo.bSWRenderOverrride;
      // Cache that SW Rendering is enabled
      sPlatformInfo.bSWRender                  = TRUE;
      DEBUG((EFI_D_ERROR, "DisplayDxe: SW renderer mode enabled!\n"));
    }
    else
    {
      // Force SW render mode off
      sPlatformInfo.bSWRender                  = FALSE;      
      // Report SW render mode is disabled
      pPlatformParams->sPlatformInfo.bSWRender = FALSE;
    }
    break;
  case MDPPLATFORM_CONFIG_GETPANELCONFIG:
    {
      if (NULL == pPlatformParams)
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        // Retrieve panel configuration (could be dependent on the interface)
        switch (eDisplayId)
        {
        case MDP_DISPLAY_PRIMARY:
          
          pPlatformParams->sPlatformPanel.uDefaultVendor   = 0;
          pPlatformParams->sPlatformPanel.uDefaultRevision = 0;
          pPlatformParams->sPlatformPanel.uPanelId         = 0;
          pPlatformParams->sPlatformPanel.uPowerGridId     = sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId];
          pPlatformParams->sPlatformPanel.pPanelXMLConfig  = NULL;

          if (TRUE == Panel_CheckOverride(eDisplayId, pPlatformParams))
          {
            //Use configuration set by panel override
          }
          else if (TRUE == sPlatformInfo.bSWRender)
          {
            //use dummy panel for sw rendering
            pPlatformParams->sPlatformPanel.pPanelXMLConfig  = (int8 *)dummy_xmldata;
            pPlatformParams->sPlatformPanel.uConfigSize      = sizeof(dummy_xmldata);
            pPlatformParams->sPlatformPanel.eSelectedPanel   = MDPPLATFORM_PANEL_NONE;
          }
          else
          {
            // Report the proper information depending on the display.
            switch (sPlatformInfo.sEFIPlatformType.platform)
            {
            case EFI_PLATFORMINFO_TYPE_CDP:
            case EFI_PLATFORMINFO_TYPE_MTP:
              pPanelList = panelList_0;
              uPanelCnt  = PANEL_INFOLIST_LENGTH(panelList_0);
			  
              if ((EFICHIPINFO_FAMILY_QCS605                == sPlatformInfo.sEFIChipSetFamily) &&
                 ((EFICHIPINFO_FAMILY_QCS605_LC_VARIANT     == sPlatformInfo.sEFIPlatformType.subtype) ||
                  (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA     == sPlatformInfo.sEFIPlatformType.subtype)))
              {
                pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8 *)LT9611_DSI2HDMI_FHD_video_xmldata;
                pPlatformParams->sPlatformPanel.uConfigSize     = sizeof(LT9611_DSI2HDMI_FHD_video_xmldata);
                pPlatformParams->sPlatformPanel.eSelectedPanel  = MDPPLATFORM_PANEL_LT9611_DSI2HDMI_FHD_VIDEO;
              }
              else if (MDP_STATUS_OK != DynamicDSIPanelDetection(pPlatformParams, &uPanelID, sPlatformInfo.uPanelIdOverride, pPanelList, uPanelCnt))
              {
                /* not detected, use defualt panel */
                pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8 *)pPanelList->psPanelCfg;
                pPlatformParams->sPlatformPanel.uConfigSize     = pPanelList->uPanelCfgSize;
                pPlatformParams->sPlatformPanel.eSelectedPanel  = pPanelList->eSelectedPanel;
                uPanelID = ((pPanelList->panelIdCommands[1].expectedReadback[0] << 8) |
                             pPanelList->panelIdCommands[2].expectedReadback[0]);
                pPlatformParams->sPlatformPanel.uPanelId        = uPanelID;
                sPlatformInfo.uPrimaryPanelId                   = uPanelID;
              }
              else
              {
                /* detected */
                pPlatformParams->sPlatformPanel.uPanelId = uPanelID;
                sPlatformInfo.uPrimaryPanelId            = uPanelID;
              }
              break;

            case EFI_PLATFORMINFO_TYPE_QRD:
              /* Determine platform by parsing the hardware version major value */
              if (0x12 == ((sPlatformInfo.sEFIPlatformType.version >> 16) & 0xFF))
              {
                 /* QRD670 SKU2 uses Hx8399 panel */
                 pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8 *)Truly_HX8399_FHD_video_xmldata;
                 pPlatformParams->sPlatformPanel.uConfigSize     = sizeof(Truly_HX8399_FHD_video_xmldata);
                 pPlatformParams->sPlatformPanel.eSelectedPanel  = MDPPLATFORM_PANEL_HX8399_TRULY_FHD_VIDEO;
              }
              else
              {
                /* QRD670 uses NT36850 panel */
                pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8 *)Truly_NT36850_WQHD_cmd_xmldata;
                pPlatformParams->sPlatformPanel.uConfigSize     = sizeof(Truly_NT36850_WQHD_cmd_xmldata);
                pPlatformParams->sPlatformPanel.eSelectedPanel  = MDPPLATFORM_PANEL_NT36850_TRULY_WQHD_CMD;
              }
              break;

            case EFI_PLATFORMINFO_TYPE_HDK:
                 pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8 *)Truly_HX8399_FHD_video_xmldata;
                 pPlatformParams->sPlatformPanel.uConfigSize     = sizeof(Truly_HX8399_FHD_video_xmldata);
                 pPlatformParams->sPlatformPanel.eSelectedPanel  = MDPPLATFORM_PANEL_HX8399_TRULY_FHD_VIDEO;
              break;

            default:
              pPlatformParams->sPlatformPanel.pPanelXMLConfig    = (int8 *)dummy_xmldata;
              pPlatformParams->sPlatformPanel.uConfigSize        = sizeof(dummy_xmldata);
              pPlatformParams->sPlatformPanel.eSelectedPanel     = MDPPLATFORM_PANEL_NONE;
              break;
            }
          }
          break;

        case MDP_DISPLAY_EXTERNAL: 
          switch (PcdGet32(PcdExtDisplayType))
          {
            case PCD_EXTERNAL_DISPLAY_DP:
              pPlatformParams->sDPConfig.bInvertPlugPolarity       = DP_USBPlugInvertedOrientation();
              
              // For Nazgul, DP Phy lane to pad connection is the reverse of DP alt mode over usb type-c spec defined mapping
              pPlatformParams->sDPConfig.bReversePhyDataPath       = TRUE;
              break;
          
            default:
              break;
          }
          break;
  
        default:
          break;
        }
      }
    }
    break;
  case MDPPLATFORM_CONFIG_POWERUP:
    {
      switch (eDisplayId)
      {
      case MDP_DISPLAY_PRIMARY:
  
        // Config based on the platform
        switch (sPlatformInfo.sEFIPlatformType.platform)
        {
        case EFI_PLATFORMINFO_TYPE_CDP:
        case EFI_PLATFORMINFO_TYPE_MTP:
        case EFI_PLATFORMINFO_TYPE_QRD:
        case EFI_PLATFORMINFO_TYPE_HDK:

            // Panel power configuration override (set through the environment variable)
            if (sPlatformInfo.uPowerCfgOverride)
            {
              sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = sPlatformInfo.uPowerCfgOverride;
            }
            else
            {
              EFI_QCOM_PMIC_VERSION_PROTOCOL *PmicVersionProtocol = NULL;  
              EFI_PM_DEVICE_INFO_EXT_TYPE     PmicDeviceInfo      = {0xFF};

              // Populate the local power configuration (Force to PM_LCD_WLED for LCD)
              //
              // Note: OLED users configure this as 
              //         MDP_PMIC_MODULE_CONTROLTYPE_PM_AMOLED - if using QCOM PMIC power solution
              //         MDP_PMIC_MODULE_CONTROLTYPE_NONE      - if using their own power solution

              /* LCD Power Configuration */
              sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = MDP_PMIC_MODULE_CONTROLTYPE_PM_LCD_WLED;

              if ((EFICHIPINFO_FAMILY_QCS605             == sPlatformInfo.sEFIChipSetFamily)        &&
                 ((EFICHIPINFO_FAMILY_QCS605_LC_VARIANT  == sPlatformInfo.sEFIPlatformType.subtype) ||
                  (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA  == sPlatformInfo.sEFIPlatformType.subtype)))
              {
                /* Skip secondary power configuration for LT9611 display */
                sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = MDP_PMIC_MODULE_CONTROLTYPE_NONE;
              }
              else if ((EFI_SUCCESS == gBS->LocateProtocol(&gQcomPmicVersionProtocolGuid, NULL, (VOID **)&PmicVersionProtocol)) &&
                       (EFI_SUCCESS == PmicVersionProtocol->GetPmicInfoExt(PMIC_PMI_DEV_INDEX, &PmicDeviceInfo)))
              {
                if ((0 == PmicDeviceInfo.PmicVariantRevision) ||
                   ((2 == PmicDeviceInfo.PmicVariantRevision) && (2 == PmicDeviceInfo.PmicAllLayerRevision)))
                 {
                  /* OLED Power Configuration, PmicVariantRevision is 0x0 for PM660A */
                  sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = MDP_PMIC_MODULE_CONTROLTYPE_PM_AMOLED;
                }
              }
              else
              {
                DEBUG((EFI_D_WARN, "DisplayDxe: Get Pmic Variant Revision Failed\n"));
              }

              // Configure power to MDP_PMIC_MODULE_CONTROLTYPE_NONE here if not using QCOM PMIC power solution
              // sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = MDP_PMIC_MODULE_CONTROLTYPE_NONE;
            }

            // Populate the WLED strings used on the platform
            if ((EFI_PLATFORMINFO_TYPE_QRD == sPlatformInfo.sEFIPlatformType.platform) ||
			    (EFI_PLATFORMINFO_TYPE_HDK == sPlatformInfo.sEFIPlatformType.platform))
            {            
              sPanelPowerCtrl.uWledStringsPrimary = (EFI_PM_WLED_LED1 | EFI_PM_WLED_LED2);
            }
            else
            {
              sPanelPowerCtrl.uWledStringsPrimary = (EFI_PM_WLED_LED2 | EFI_PM_WLED_LED3);
            }

            // Primary Power Sequence
            if (MDP_STATUS_OK != (eStatus = Panel_CDP_PowerUp(eDisplayId, &sPanelPowerCtrl, &sPlatformInfo)))
            {
              DEBUG((EFI_D_WARN, "DisplayDxe: Primary Power Up Sequence Failed (%d)\n", eStatus));
            }
            else if (MDP_STATUS_OK != (eStatus = Panel_CDP_PeripheralPower(eDisplayId, &sPanelPowerCtrl, TRUE)))  // Secondary Power Sequence
            {
              DEBUG((EFI_D_WARN, "DisplayDxe: Secondary Power Up Sequence Failed (%d)\n", eStatus));
            }
            break;

            default:
            break;
          }
          break;
        case MDP_DISPLAY_EXTERNAL:
          break;
        default:
          break;
       }
       break;
  	}
    break;
    case MDPPLATFORM_CONFIG_GETPANELDTINFO:
    {
      if (NULL == pPlatformParams)
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        pPlatformParams->psPanelDTInfo = (PanelInfoType*)&gPanelList;
      }
      break;
  
    }
    case MDPPLATFORM_CONFIG_GETPANELLIST:
      {
        if (NULL == pPlatformParams)
        {
          eStatus = MDP_STATUS_BAD_PARAM;
        }
        else
        {
          uint32 i = 0;
          
          while (MDPPLATFORM_PANEL_NONE != gPanelList[i].ePanel)
          {
            //Copy Panel ID
            pPlatformParams->sPanelList.ePanel[i] = gPanelList[i].ePanel;
            i++; 
          }
  
          pPlatformParams->sPanelList.uCount = i;
        }
      }
      break;
    
  case MDPPLATFORM_CONFIG_GETPANELSUPPORTFLAGS:
    {
      if (NULL == pPlatformParams)
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        //Lookup panel
        int32 i = PANEL_INFOLIST_LENGTH(gPanelList) - 1;
        while (0 <= i)
        {
          if (pPlatformParams->sPanelSupport.ePanel == gPanelList[i].ePanel)
          {
            if ((DISP_TOPOLOGY_CONFIG_NONE == pPlatformParams->sPanelSupport.uModeIndex) ||
                (pPlatformParams->sPanelSupport.uModeIndex > DISPLAY_MODE_MAX))
            {
              pPlatformParams->sPanelSupport.uFlags = gPanelList[i].uModeFlags[0];
            }
            else
            {
              pPlatformParams->sPanelSupport.uFlags = gPanelList[i].uModeFlags[pPlatformParams->sPanelSupport.uModeIndex-1];
            }
            break;
          }
          i--;
        }
      }
    }
    break;
  case MDPPLATFORM_CONFIG_POWERDOWN:
    {
      // Handle power down
      switch (eDisplayId)
      {
      case MDP_DISPLAY_PRIMARY:
  
        // Config based on the platform
        switch (sPlatformInfo.sEFIPlatformType.platform)
        {
        case EFI_PLATFORMINFO_TYPE_CDP:
        case EFI_PLATFORMINFO_TYPE_MTP:
        case EFI_PLATFORMINFO_TYPE_QRD:
        case EFI_PLATFORMINFO_TYPE_HDK:
          if (MDP_STATUS_OK != (eStatus = Panel_CDP_PowerDown(eDisplayId, &sPanelPowerCtrl, &sPlatformInfo)))
          {
            DEBUG((EFI_D_WARN, "DisplayDxe: Primary Power Down Sequence Failed (%d)\n", eStatus));
          }
          else if (MDP_STATUS_OK != (eStatus = Panel_CDP_PeripheralPower(eDisplayId, &sPanelPowerCtrl, FALSE)))  // Secondary Power Sequence
          {
            DEBUG((EFI_D_WARN, "DisplayDxe: Secondary Power Down Sequence Failed (%d)\n", eStatus));
          }
          break;
        default:
          break;
        }
  
        break;
      case MDP_DISPLAY_EXTERNAL:
        // Config based on the platform
        switch (sPlatformInfo.sEFIPlatformType.platform)
        {
        case EFI_PLATFORMINFO_TYPE_CDP:
        case EFI_PLATFORMINFO_TYPE_MTP:
        case EFI_PLATFORMINFO_TYPE_QRD:
        case EFI_PLATFORMINFO_TYPE_HDK:
          eStatus = ExternalPanel_CDP_PowerDown(eDisplayId, &sPanelPowerCtrl);
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
    }
    break;
  case MDPPLATFORM_CONFIG_SETBACKLIGHT:
    {
      if (NULL == pPlatformParams)
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        // Handle backlight level
        switch (eDisplayId)
        {
        case MDP_DISPLAY_PRIMARY:
          switch (sPlatformInfo.sEFIPlatformType.platform)
          {
          case EFI_PLATFORMINFO_TYPE_CDP:
          case EFI_PLATFORMINFO_TYPE_MTP:
          case EFI_PLATFORMINFO_TYPE_QRD:
          case EFI_PLATFORMINFO_TYPE_HDK:
          case EFI_PLATFORMINFO_TYPE_RCM:
              if ((MDP_PMIC_MODULE_CONTROLTYPE_PM_AMOLED == sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId]) &&
                  (FALSE == sPanelPowerCtrl.bOledPostInitDone))
              {
                eStatus = Panel_CDP_PmicOledPostInit(eDisplayId);
                sPanelPowerCtrl.bOledPostInitDone = TRUE;
              }
              eStatus = Panel_CDP_BacklightLevel(eDisplayId, &sPanelPowerCtrl, &pPlatformParams->sBacklightConfig);
            break;
          default:
            break;
          }
          break;
        case MDP_DISPLAY_EXTERNAL:
          eStatus = MDP_STATUS_NOT_SUPPORTED;
          break;
        default:
          break;
        }
      }
    }
    break;
  case MDPPLATFORM_CONFIG_GETPANELID:
    {
    }
    break;
  case MDPPLATFORM_CONFIG_GETPLATFORMINFO:
    {
      if (NULL == pPlatformParams)
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        //
        // Return platform information
        //
        MDP_OSAL_MEMCPY(&pPlatformParams->sPlatformInfo, &sPlatformInfo, sizeof(MDPPlatformInfo));
      }
    }
    break;
  case MDPPLATFORM_CONFIG_RESETPANEL:
    {
      if (pPlatformParams->sPlatformPanelReset.uResetGpio == 0)
      {
        if (EFICHIPINFO_ID_SXR1120 == sPlatformInfo.sEFIChipSetId)
        {
          sPanelPowerCtrl.uResetGpio                      = CDP_DISP_SXR1120_RESET_GPIO;
          pPlatformParams->sPlatformPanelReset.uResetGpio = CDP_DISP_SXR1120_RESET_GPIO;
        }
        else if ((EFICHIPINFO_FAMILY_QCS605                == sPlatformInfo.sEFIChipSetFamily)        &&
                ((EFICHIPINFO_FAMILY_QCS605_LC_VARIANT     == sPlatformInfo.sEFIPlatformType.subtype) ||
                 (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA     == sPlatformInfo.sEFIPlatformType.subtype)))
        {
          sPanelPowerCtrl.uResetGpio                      = CDP_DISP_LT9611_RESET_GPIO;
          pPlatformParams->sPlatformPanelReset.uResetGpio = CDP_DISP_LT9611_RESET_GPIO;
        }
        else
        {
          sPanelPowerCtrl.uResetGpio = CDP_DISP_RESET_GPIO;
          pPlatformParams->sPlatformPanelReset.uResetGpio = CDP_DISP_RESET_GPIO;
        }
      }
      else 
      {
           sPanelPowerCtrl.uResetGpio = pPlatformParams->sPlatformPanelReset.uResetGpio;
      }
      switch (eDisplayId)
      {
      case MDP_DISPLAY_PRIMARY:
  
        // Config based on the platform
        switch (sPlatformInfo.sEFIPlatformType.platform)
        {
        case EFI_PLATFORMINFO_TYPE_CDP:
        case EFI_PLATFORMINFO_TYPE_MTP:
        case EFI_PLATFORMINFO_TYPE_QRD:
        case EFI_PLATFORMINFO_TYPE_HDK:
          eStatus = Panel_CDP_Reset(&pPlatformParams->sPlatformPanelReset);
          break;
        default:
          break;
        }
  
        break;
      case MDP_DISPLAY_EXTERNAL:
        eStatus = MDP_STATUS_NOT_SUPPORTED;
        break;
      default:
        break;
      }
    }
    break;
  
  case MDPPLATFORM_CONFIG_SETGPIOSTATE:
    {
      if (NULL == pPlatformParams)
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        eStatus = SetDefaultGPIOState(&pPlatformParams->sGPIOState);
      }
    }
    break;
 case MDPPLATFORM_CONFIG_GETPANELXMLINFO:
    {
      uint32 uPanelCnt  = sizeof(panelList_0)/sizeof(PlatformDSIDetectParams);
	  eStatus           = MDP_STATUS_OK;

      if ((NULL == pPlatformParams) ||
	  	  (pPlatformParams->sPlatformPanel.eSelectedPanel >= uPanelCnt))
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
	  else
      {
        if((EFICHIPINFO_FAMILY_QCS605                 == sPlatformInfo.sEFIChipSetFamily) &&
           (EFICHIPINFO_FAMILY_QCS605_LC_VARIANT      != sPlatformInfo.sEFIPlatformType.subtype) &&
           (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA      != sPlatformInfo.sEFIPlatformType.subtype) &&
           (EFICHIPINFO_FAMILY_QCS605_HP_VARIANT      != sPlatformInfo.sEFIPlatformType.subtype) &&
           (EFICHIPINFO_FAMILY_QCS605_INTERNAL_DEVICE != sPlatformInfo.sEFIPlatformType.subtype))
        {
          DEBUG((EFI_D_INFO, "DisplayDxe: GETPANELXMLINFO: Platform does not have Display\n"));
          eStatus = MDP_STATUS_NO_RESOURCES;
        }
        else if (sPlatformInfo.bSWRender == TRUE)
        {
          pPlatformParams->sPanelXMLInfo.pPanelXMLConfig = (int8 *)dummy_xmldata;
          pPlatformParams->sPanelXMLInfo.uPanelXMLSize   = sizeof (dummy_xmldata);
        }
        else
        {
          MDPPlatformPanelInfo sPlatformPanelInfo;
          MDP_OSAL_MEMZERO(&sPlatformPanelInfo, sizeof(MDPPlatformPanelInfo));

          sPlatformPanelInfo.eSelectedPanel = panelList_0[pPlatformParams->sPanelXMLInfo.uIndex].eSelectedPanel;
          if (MDP_STATUS_OK != FindPanelIndex(&sPlatformPanelInfo))
          {
			eStatus = MDP_STATUS_BAD_PARAM;
          }
		  else
          {
            pPlatformParams->sPanelXMLInfo.pPanelXMLConfig = (int8*)gPanelList[sPlatformPanelInfo.uSelectedPanelIndex].pPanelXMLConfig;
			pPlatformParams->sPanelXMLInfo.uPanelXMLSize   = gPanelList[sPlatformPanelInfo.uSelectedPanelIndex].uConfigSize;
          }
        }
      }
    }
    break;  
  case  MDPPLATFORM_CONFIG_BYPASS:
    {
        // SW Render Bypass mode
        eStatus = MDP_STATUS_OK;
    }
    break;   
  case MDPPLATFORM_CONFIG_GETPANELCMDSUFFIX:
    {
      if (NULL == pPlatformParams)
      {
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        // Report the proper information depending on the platform.
        switch (sPlatformInfo.sEFIPlatformType.platform)
        {
        case EFI_PLATFORMINFO_TYPE_MTP:
          if ((EFICHIPINFO_FAMILY_QCS605                == sPlatformInfo.sEFIChipSetFamily) &&
             ((EFICHIPINFO_FAMILY_QCS605_LC_VARIANT     == sPlatformInfo.sEFIPlatformType.subtype) ||
              (EFICHIPINFO_FAMILY_QCS605_IPC_CAMERA     == sPlatformInfo.sEFIPlatformType.subtype)))
          {
            uint32 uDisplayFlags = PcdGet32(PcdPrimaryFlags);

            if (MDP_PANEL_FLAG_DISABLE_SEAMLESS_SPLASH & uDisplayFlags)
            {
              pPlatformParams->sPanelCmdLine.pCmdLine     = CMDLINE_LT9611_CONT_SPLASH_DISABLE;
              pPlatformParams->sPanelCmdLine.uCmdLineSize = AsciiStrLen(CMDLINE_LT9611_CONT_SPLASH_DISABLE);
            }
            else
            {
              pPlatformParams->sPanelCmdLine.pCmdLine     = CMDLINE_LT9611_CONT_SPLASH_ENABLE;
              pPlatformParams->sPanelCmdLine.uCmdLineSize = AsciiStrLen(CMDLINE_LT9611_CONT_SPLASH_ENABLE);
            }
          }
          break;
        default:
          eStatus = MDP_STATUS_NOT_SUPPORTED;
          break;
        }
      }
    }
    break;
  default:
    eStatus = MDP_STATUS_BAD_PARAM;
    break;
  }
  
  return eStatus;
}



/*===========================================================================
Private Function Definitions
===========================================================================*/

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: ReadPlatformIDAndChipID()
** 
** DESCRIPTION:
**
**/
static MDP_Status ReadPlatformIDAndChipID(EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *pPlatformInfo, EFIChipInfoIdType *pChipSetId, EFIChipInfoFamilyType *pChipSetFamily)
{
  MDP_Status                           eStatus                = MDP_STATUS_OK;
  EFI_PLATFORMINFO_PROTOCOL            *hPlatformInfoProtocol;
  EFI_CHIPINFO_PROTOCOL                *hChipInfoProtocol;

  if (EFI_SUCCESS == gBS->LocateProtocol(&gEfiPlatformInfoProtocolGuid,
                                         NULL,
                                         (VOID **)&hPlatformInfoProtocol))
  {
    if (EFI_SUCCESS != hPlatformInfoProtocol->GetPlatformInfo(hPlatformInfoProtocol, pPlatformInfo))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: gEfiPlatformInfoProtocolGuid->GetPlatformInfo() Failed.\n"));

      eStatus = MDP_STATUS_FAILED;
    }
  }
  else
  {
    DEBUG((EFI_D_WARN, "DisplayDxe: gEfiPlatformInfoProtocolGuid protocol Failed.\n"));

    eStatus = MDP_STATUS_FAILED;
  }

  if (EFI_PLATFORMINFO_TYPE_UNKNOWN == pPlatformInfo->platform)
  {
    pPlatformInfo->platform = EFI_PLATFORMINFO_TYPE_VIRTIO;
  }

  // Read the chipset ID
  if (EFI_SUCCESS == gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,
                                         NULL,
                                         (VOID **)&hChipInfoProtocol))
  {
    if (EFI_SUCCESS != hChipInfoProtocol->GetChipId(hChipInfoProtocol, pChipSetId))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: gEfiChipInfoProtocolGuid->GetChipId() Failed. \n"));

      eStatus = MDP_STATUS_FAILED;
    }
    if (EFI_SUCCESS != hChipInfoProtocol->GetChipFamily(hChipInfoProtocol, pChipSetFamily))
    {
      DEBUG((EFI_D_WARN, "DisplayDxe: gEfiChipInfoProtocolGuid->GetChipFamily() Failed. \n"));

      eStatus = MDP_STATUS_FAILED;
    }

  }
  else
  {
    DEBUG((EFI_D_WARN, "DisplayDxe: gEfiChipInfoProtocolGuid protocol Failed.\n"));

    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DP_USBPlugInvertedOrientation()
** 
** DESCRIPTION:
**   Get DP plug polarity (TRUE = inverted, FALSE = normal)
**
**/
static bool32 DP_USBPlugInvertedOrientation (void)
{ 
  /* USB driver support for detecting orientation is not available. 
   * Use Hardcoded cable polarity 
   */
  bool32  bPolarity = DP_PLUG_INVERTED_POLARITY;

  // For Klocwork - will never be true when this fn. is called
  if (PCD_EXTERNAL_DISPLAY_DP != PcdGet32(PcdExtDisplayType))
  {
     bPolarity = !bPolarity;
  }

  return bPolarity;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: HDMI_CDP_Powerup()
** 
** DESCRIPTION:
**  
**
**/
static MDP_Status ExternalPanel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo* pPlatformInfo)
{
  MDP_Status                    Status            = MDP_STATUS_OK;
  EFI_TLMM_PROTOCOL            *TLMMProtocol      = NULL;
  bool32                        bInvertedPlug     = DP_USBPlugInvertedOrientation();

  if (MDP_STATUS_OK != (Status =  PlatformClientInit(eDisplayId, pPowerParams, pPlatformInfo)))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Failed to initialize handle for External (HDMI) display NPA node.\n"));
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gEfiTLMMProtocolGuid, NULL, (void **)&TLMMProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate TLMM protocol failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else
  {
    /*
    * Voting for Display NPA node to be ON
    */
    npa_issue_required_request(pPowerParams->sNPAClient[eDisplayId], PMIC_NPA_MODE_ID_GENERIC_ACTIVE );

    switch (PcdGet32(PcdExtDisplayType))
    {
      case PCD_EXTERNAL_DISPLAY_DP:
        npa_issue_required_request(pPowerParams->sNPAClient[eDisplayId], PMIC_NPA_MODE_ID_GENERIC_ACTIVE );

        /* Configure DP HPD gpio */ 
        if (EFI_SUCCESS != TLMMProtocol->ConfigGpio(EFI_GPIO_CFG(34, 1, GPIO_INPUT, GPIO_PULL_DOWN, GPIO_2MA), DAL_TLMM_GPIO_ENABLE))
        {
          DEBUG((EFI_D_WARN, "DP: Failed to configure HPD GPIO \n"));
        }

        /* Configure USB_PHY_PS GPIO */
        if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(38, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), DAL_TLMM_GPIO_ENABLE))
        {
          DEBUG((EFI_D_ERROR, "DP: failed to configure USB_PHY_PS GPIO as output \n"));        
          Status = MDP_STATUS_FAILED;
          goto exit;
        }
        if (EFI_SUCCESS != TLMMProtocol->GpioOut((UINT32)EFI_GPIO_CFG(38, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), GPIO_HIGH_VALUE))
        {
          DEBUG((EFI_D_ERROR, "DP: failed to drive USB_PHY_PS GPIO \n"));  
          Status = MDP_STATUS_FAILED;
          goto exit;
        }
           
        /* Configure gpio for chip select OE_N of USB-DP Mux logic (SBU_SW_OE) */      
        if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(77, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_16MA), DAL_TLMM_GPIO_ENABLE))
        {
          DEBUG((EFI_D_ERROR, "DP: failed to configure mux chip select GPIO as output \n"));      
          Status = MDP_STATUS_FAILED;
          goto exit;
        }
        if (EFI_SUCCESS != TLMMProtocol->GpioOut((UINT32)EFI_GPIO_CFG(77, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_16MA), GPIO_LOW_VALUE))
        {
          DEBUG((EFI_D_ERROR, "DP: failed to drive mux chip select GPIO \n"));
          Status = MDP_STATUS_FAILED;
          goto exit;
        }
           
        /* Configure gpio for selecting plug direction in USB-DP mux logic (SBU_SW_SEL) */
        if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(78, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), DAL_TLMM_GPIO_ENABLE))
        {
          DEBUG((EFI_D_ERROR, "DP: failed to configure Aux Sel GPIO as output \n"));      
          Status = MDP_STATUS_FAILED;
          goto exit;
        }
        if (EFI_SUCCESS != TLMMProtocol->GpioOut((UINT32)EFI_GPIO_CFG(78, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), 
                                                       bInvertedPlug ? GPIO_LOW_VALUE : GPIO_HIGH_VALUE))
        {
          DEBUG((EFI_D_ERROR, "DP: failed to drive Aux sel GPIO h\n"));  
          Status = MDP_STATUS_FAILED;
          goto exit;
        }
        
        break;
  
      default:
        break;
    }
  }

exit:
    
  return Status;
}



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: HDMI_CDP_PowerDown()
** 
** DESCRIPTION:
** 
**
**/
static MDP_Status ExternalPanel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams)
{
  MDP_Status                    Status            = MDP_STATUS_OK;
  EFI_TLMM_PROTOCOL             *TLMMProtocol     = NULL;

  if (NULL == pPowerParams->sNPAClient[eDisplayId])
  {
    DEBUG ((EFI_D_ERROR, "DisplayDxe: NULL Handle for HDMI NPA node.\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gEfiTLMMProtocolGuid, NULL, (void **)&TLMMProtocol))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Locate TLMM protocol failed!\n"));
    Status = MDP_STATUS_NO_RESOURCES;
  }
  else
  {
    switch (PcdGet32(PcdExtDisplayType))
    {
      case PCD_EXTERNAL_DISPLAY_DP:
        break;
 
      default:
        break;
    }
  }

  return Status;
}

/****************************************************************************
*
** FUNCTION: SetDefaultGPIOState()
*/
/*!
* \brief
*   The \b SetDefaultGPIOState function toggles a list of GPIO's based on the requested configuration
**
* \retval MDP_Status
*
****************************************************************************/
MDP_Status SetDefaultGPIOState(GpioStateType *pGPIOList)
{
  MDP_Status          eStatus = MDP_STATUS_OK;

  if ((NULL == pGPIOList) ||
      (NULL == pGPIOList->pGPIOList))
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    // Handle each type of GPIO differently
    switch (pGPIOList->eGPIOType)
    {
      case MDPPLATFORM_GPIOTYPE_TLMM:
         {
           EFI_TLMM_PROTOCOL   *TLMMProtocol;        

           if (EFI_SUCCESS == gBS->LocateProtocol(&gEfiTLMMProtocolGuid, NULL, (void**)&TLMMProtocol))
           {
             uint32 uCount;          

             // Loop the list to configure all GPIOs.
             for (uCount=0;uCount<pGPIOList->uListSize;uCount++)
             {
                // Only try to program GPIOs that are within range, TLMM Macro (EFI_GPIO_CFG) supports up to 0x3FF
                if (pGPIOList->pGPIOList[uCount] < 0x3FF) 
                {
                  uint32 uValue = (pGPIOList->uState>0)?GPIO_HIGH_VALUE:GPIO_LOW_VALUE;
                  
                  // Setup GPIO
                  if (EFI_SUCCESS != TLMMProtocol->ConfigGpio((UINT32)EFI_GPIO_CFG(pGPIOList->pGPIOList[uCount], 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), TLMM_GPIO_ENABLE))
                  {
                    DEBUG((EFI_D_ERROR, "DisplayDxe: Error configuring TLMM GPIO%d\n", uCount));

                  }
                  
                  // Setup Output
                  if (EFI_SUCCESS != TLMMProtocol->GpioOut((UINT32)EFI_GPIO_CFG(pGPIOList->pGPIOList[uCount], 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), uValue))
                  {
                    DEBUG((EFI_D_ERROR, "DisplayDxe: Error configuring TLMM GPIO%d\n", uCount));
                  }              
                }
             }
           }
           else
           {
             eStatus = MDP_STATUS_NO_RESOURCES;
           }
         }
         break;
      default:
          // Others (PMIC, PMI) not supported
         break;
    }

  }
  return eStatus;
}

#ifdef __cplusplus
}
#endif
