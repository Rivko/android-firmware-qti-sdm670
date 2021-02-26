/*=============================================================================
 
  File: MDPPlatformLib.c
 
  MDP platform specific functions
  
  Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include <PlatformInfo.h>
#include <Library/PcdLib.h>
#include <api/pmic/pm/pm_ibb.h>
#include <api/pmic/pm/pm_lab.h>
#include <api/pmic/pm/pm_wled.h>
#include <api/pmic/pm/pm_ldo.h>
#include <api/pmic/pm/pm_boot.h>
#include <api/pmic/pm/pm_lcdb.h>
#include <api/dal/DALStdErr.h>
#include "MDPTypes.h"
#include "MDPPlatformLib.h"
#include "MDPSystem.h"
#include "DDITlmm.h"
#include "pmapp_npa.h"
#include "npa.h"
#include "boot_extern_platforminfo_interface.h"

/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */

#define PCD_PANEL_TYPE_OEM                1

#define DSI_READ_ADDRESS_SIZE             2
#define DSI_READ_READBACK_SIZE            8

#define PLATFORM_PANEL_ID_MAX_COMMANDS    3       // maximum panel ID read commands

// PMIC Device Indices
#define PMIC_DEV_INDEX                    0       // PMIC device (Vreg, LDO, ect)
#define PMIC_PMI_DEV_INDEX                1       // PMIC interface device (IBB/LAB, GPIO,  WLED)

#define PMIC_IBBLAB_READYTIME             25      // Maximum of milliseconds to wait for IBB/LAB to stabilize (Can be up to 8.8ms +/-16%)

/* 8998 Main Panel CDP specific defines
*/
#define CDP_DISP_RESET_GPIO               75      //GPIO number for LCD0_RESET
#define CDP_DISP_TE_GPIO                  10      //GPIO number for TE
#define CDP_DISP_MODE_SEL                 76      //GPIO for panel mode select 0 = Dual Port, 1 = Single Port (DSC)

#define CDP_DISP_SXR1120_RESET_GPIO       93      //GPIO number for LCD0_RESET
#define CDP_DISP_SXR1120_ENABLE_GPIO      121     //GPIO to enable SXR1120 platform

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* Panel command ID list for dynamic panel detection
*/

typedef struct {
  uint8      address[DSI_READ_ADDRESS_SIZE];                                        // DCS command for panel ID
  uint8      expectedReadback[DSI_READ_READBACK_SIZE];                              // expected readback
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
  MDP_PmicModuleControlType     ePMICSecondaryPower[MDP_DISPLAY_MAX];                 // Configuration for PMIC based secondary power source
  uint32                        uWledStringsPrimary;                                  // Set of WLED strings needed for this platform
  uint32                        uResetGpio;
} Panel_PowerCtrlParams;

/* NPA Client info
*/
typedef struct
{
  char*                         cResourceName;
  char*                         cClientName;
}NPAClientName;

/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

/* Platform detection 
*/
static MDP_Status ReadPlatformIDAndChipID(EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *pPlatformInfo, EFIChipInfoIdType *pChipSetId, EFIChipInfoFamilyType *pChiSetFamily);


/* 8998 Main Panel CDP Functions
*/
static MDP_Status Panel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo *pPlatformInfo); //uint8 ePanelLDO);
static MDP_Status Panel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo *pPlatformInfo);
static MDP_Status Panel_CDP_Reset(MDPPlatformPanelResetInfo *pResetInfo);
static MDP_Status Panel_CDP_BacklightLevel(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, BacklightConfigType *pBacklightConfig);
static MDP_Status Panel_CDP_PeripheralPower(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, bool32 bPowerUp);

/* Main Panel CDP/MTP Functions
*/

/* 8996 HDMI Panel Functions
*/

/* General helper functions */
MDP_Status        SetDefaultGPIOState(GpioStateType *pGPIOList);


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
" 15 72 31\n"
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
" ff ff\n"
" 05 29 00\n"
" ff ff\n"
"</DSIInitSequence>"
"<TLMMGPIODefaultLow units='hex list'>4C</TLMMGPIODefaultLow>"
"<Group id='Backlight Configuration'>"
" <BacklightType units='MDP_Panel_BacklightType'>1</BacklightType>\n"
" <BacklightPmicControlType units='MDP_PmicBacklightControlType'>2</BacklightPmicControlType>\n"  
" <DisplayResetInfo>0 10 1000 10000 0</DisplayResetInfo>\n"
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
" <DisplayResetInfo>0 10 10000 10000 0</DisplayResetInfo>\n"
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

/*
* Dummy panel configuration, default fallback mode.
*
*/
const static int8 dummy_xmldata[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
"    <PanelName>VirtioDSI</PanelName>"
"    <PanelDescription>DSI Model (640x480 24bpp)</PanelDescription>"
"    <Group id=\"Active Timing\">"
"        <HorizontalActive units=\"Dot Clocks\">640</HorizontalActive>"
"        <HorizontalFrontPorch units=\"Dot Clocks\">13</HorizontalFrontPorch>"
"        <HorizontalBackPorch units=\"Dot Clocks\">14</HorizontalBackPorch>"
"        <HorizontalSyncPulse units=\"Dot Clocks\">15</HorizontalSyncPulse>"
"        <HorizontalSyncSkew units=\"Dot Clocks\">0</HorizontalSyncSkew>"
"        <HorizontalLeftBorder units=\"Dot Clocks\">0</HorizontalLeftBorder>"
"        <HorizontalRightBorder units=\"Dot Clocks\">0</HorizontalRightBorder>"
"        <VerticalActive units=\"Dot Clocks\">480</VerticalActive>"
"        <VerticalFrontPorch units=\"Lines\">11</VerticalFrontPorch>"
"        <VerticalBackPorch units=\"Lines\">12</VerticalBackPorch>"
"        <VerticalSyncPulse units=\"Lines\">13</VerticalSyncPulse>"
"        <VerticalSyncSkew units=\"Lines\">0</VerticalSyncSkew>"
"        <VerticalTopBorder units=\"Lines\">0</VerticalTopBorder>"
"        <VerticalBottomBorder units=\"Lines\">0</VerticalBottomBorder>"
"        <InvertDataPolarity>False</InvertDataPolarity>"
"        <InvertVsyncPolairty>False</InvertVsyncPolairty>"
"        <InvertHsyncPolarity>False</InvertHsyncPolarity>"
"        <BorderColor>0x0</BorderColor>"   
"    </Group>"
"    <Group id=\"Display Interface\">"
"        <InterfaceType units=\"QDI_DisplayConnectType\">8</InterfaceType>"
"        <InterfaceColorFormat units=\"QDI_PixelFormatType\">3</InterfaceColorFormat>"
"    </Group>"
"    <Group id=\"DSI Interface\">"
"        <DSIChannelId units=\"DSI_Channel_IDType\">1</DSIChannelId>"     
"        <DSIVirtualId units=\"DSI_Display_VCType\">0</DSIVirtualId>"     
"        <DSIColorFormat units=\"DSI_ColorFormatType\">36</DSIColorFormat>"     
"        <DSITrafficMode units=\"DSI_TrafficModeType\">0</DSITrafficMode>"
"        <DSILanes units=\"integer\">2</DSILanes>"
"        <DSIHsaHseAfterVsVe units=\"Bool\">False</DSIHsaHseAfterVsVe>"
"        <DSILowPowerModeInHFP units=\"Bool\">False</DSILowPowerModeInHFP>"
"        <DSILowPowerModeInHBP units=\"Bool\">False</DSILowPowerModeInHBP>"
"        <DSILowPowerModeInHSA units=\"Bool\">False</DSILowPowerModeInHSA>"
"        <DSILowPowerModeInBLLPEOF units=\"Bool\">False</DSILowPowerModeInBLLPEOF>"
"        <DSILowPowerModeInBLLP units=\"Bool\">False</DSILowPowerModeInBLLP>"
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

/*===========================================================================
Function Definitions
===========================================================================*/

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: PlatformClientInit()
** 
** DESCRIPTION:
**        Initialize NPA client
**
*//* -------------------------------------------------------------------- */
static MDP_Status PlatformClientInit(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams)
{
  MDP_Status    eStatus = MDP_STATUS_OK;

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PowerUp()
** 
** DESCRIPTION:
**        Panel power up sequence for CDP
**
*/ /* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo *pPlatformInfo)// uint8 ePanelLDO)
{
  MDP_Status        eStatus        = MDP_STATUS_OK;
  uint8             ePanelLDO      = PM_LDO_11;

  if (MDP_STATUS_OK != (eStatus =  PlatformClientInit(eDisplayId, pPowerParams)))
  {
    MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Failed to initialize handle for Primary display NPA node.\n");
  }
  else
  {
    DALGpioSignalType uGPIOCfg = 0;


	if (EFICHIPINFO_ID_SXR1120 == pPlatformInfo->sEFIChipSetId)
    {
      ePanelLDO = PM_LDO_13;
	  
	  uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(CDP_DISP_SXR1120_ENABLE_GPIO, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA, DAL_GPIO_LOW_VALUE);
	  Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);
    }

    /* LDOs 1A & 1B for DSI PHY/PLL are already turned on by system driver.*/

    /*Turn on LDO14 for V_LCD_VDDI_1P8 which is used by panel */
    if (PM_ERR_FLAG__SUCCESS != pm_ldo_volt_level(PMIC_DEV_INDEX, ePanelLDO, 1800000))
    {
	  MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayDxe: Error configuring LDO11 to 1.8V\n");
    }

    if (PM_ERR_FLAG__SUCCESS != pm_ldo_sw_enable(PMIC_DEV_INDEX, ePanelLDO, PM_ON))
    {
	  MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayDxe: Error enabling LDO11\n");
    }

    MDP_OSAL_DELAYMS(10);   /* delay 10ms to allow power grid to settle */

    /* only support dual dsi control case */
    uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(CDP_DISP_MODE_SEL, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA, DAL_GPIO_LOW_VALUE);

    Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);

    uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG(CDP_DISP_TE_GPIO, 1, DAL_GPIO_INPUT, DAL_GPIO_NO_PULL, DAL_GPIO_LOW_VALUE);
    Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);
      
  }

  return eStatus;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PowerDown()
** 
** DESCRIPTION:
**        Panel power down sequence for CDP
**
*/ /* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, MDPPlatformInfo *pPlatformInfo) //uint8 ePanelLDO, uint32 ResetGPIO)
{
  MDP_Status          eStatus      = MDP_STATUS_OK;
  DALGpioSignalType   uGPIOCfg     = 0;

  if (pPlatformInfo->sEFIChipSetId != EFICHIPINFO_ID_SXR1120) /* SXR1120: LDO13 also supplies DDR, hence dont turn off */
  {
    /*Turn OFF LDO11 for V_LCD_VDDI_1P8 which is used by panel */
    if (PM_ERR_FLAG__SUCCESS != pm_ldo_sw_enable(PMIC_DEV_INDEX, PM_LDO_11, PM_OFF))
    {
	  MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayDxe: Error enabling LDO11\n");
    }
  }

  uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(((EFICHIPINFO_ID_SXR1120 == pPlatformInfo->sEFIChipSetId) ? CDP_DISP_SXR1120_RESET_GPIO : CDP_DISP_RESET_GPIO), 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA, DAL_GPIO_LOW_VALUE);
  eStatus = Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_ResetStateExec()
** 
** DESCRIPTION:
**        toggle reset gpio base on states transition
**
*/ /* -------------------------------------------------------------------- */
static void Panel_ResetStateExec(MDPPlatformPanelResetInfo *pResetInfo, uint32 state0, uint32 state1, uint32 state2)
{
  DALGpioSignalType   uGPIOCfg    = 0;

  uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(pResetInfo->uResetGpio, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA, state0);
  Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);

   // Pull reset pin high with same time as assert time
   MDP_OSAL_DELAYUS(pResetInfo->uPreResetTimeUs);

  uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(pResetInfo->uResetGpio, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA, state1);
  Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);

  // Wait 10us, Reset_N(RESX) Need to be at least 10us to really trigger the (as per the panel spec)
  MDP_OSAL_DELAYUS(pResetInfo->uResetAssertTimeUs);
    
  uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(pResetInfo->uResetGpio, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA, state2);
  Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_Reset()
** 
** DESCRIPTION:
**        Panel reset sequence for CDP
**
*/ /* -------------------------------------------------------------------- */
MDP_Status Panel_CDP_Reset(MDPPlatformPanelResetInfo *pResetInfo)
{
  MDP_Status          eStatus     = MDP_STATUS_OK;

    switch (pResetInfo->uAssertState)
    {
      case MDP_RESET_STATE_EDGE_LOW2HIGH:
         Panel_ResetStateExec(pResetInfo, DAL_GPIO_LOW_VALUE, DAL_GPIO_HIGH_VALUE, DAL_GPIO_HIGH_VALUE);
       break;
      case MDP_RESET_STATE_EDGE_HIGH2LOW:
         Panel_ResetStateExec(pResetInfo, DAL_GPIO_HIGH_VALUE, DAL_GPIO_LOW_VALUE, DAL_GPIO_LOW_VALUE);
       break;
      case MDP_RESET_STATE_LEVEL_HIGH:
         Panel_ResetStateExec(pResetInfo, DAL_GPIO_LOW_VALUE, DAL_GPIO_HIGH_VALUE, DAL_GPIO_LOW_VALUE);
       break;
      case MDP_RESET_STATE_LEVEL_LOW:
      default:
         Panel_ResetStateExec(pResetInfo, DAL_GPIO_HIGH_VALUE, DAL_GPIO_LOW_VALUE, DAL_GPIO_HIGH_VALUE);
       break;
    }

  MDP_OSAL_DELAYUS(pResetInfo->uPostResetTimeUs);

   /* 
    * post reset time: Timing space between end of reset to start sending init dcs cmds
    * NOTE:
    *      use DSIInitMasterTime to extend post reset time control if required
    */

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PeripheralPower()
** 
** DESCRIPTION:
**        Secondary power sequence for other PMIC modules such as IBB/LAB
**
*/ /* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_PeripheralPower(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, bool32 bPowerUp)
{
  MDP_Status                    eStatus = MDP_STATUS_OK;
  pm_err_flag_type				eErr = PM_ERR_FLAG__SUCCESS;

  if (TRUE == bPowerUp)
  {
    // ********** Brightness Power-up Sequence **********
    // Power up
    /************************************************************************/
    /* LCDB                                                                 */
    /************************************************************************/
    
    if (PM_ERR_FLAG__SUCCESS != (eErr = pm_lcdb_set_module_rdy(PMIC_PMI_DEV_INDEX, TRUE)))
    {
      MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Error to config LCDB to ready - %d\n", eErr);
    }
    if (PM_ERR_FLAG__SUCCESS != (eErr = pm_lcdb_module_enable(PMIC_PMI_DEV_INDEX, TRUE)))
    {
      MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Error to enable LCDB module - %d\n", eErr);
    }
    
    /************************************************************************/
    /*  WLED                                                                */
    /************************************************************************/

    //0xD84F             EN_CURRENT_SINK         0xE0       Enable all 3 strings
    if (PM_ERR_FLAG__SUCCESS != pm_wled_enable_current_sink(PMIC_PMI_DEV_INDEX, pPowerParams->uWledStringsPrimary))
    {
      MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Wled EnableCurrentSink failed\n");
    }

    if (PM_ERR_FLAG__SUCCESS != pm_wled_enable_modulator(PMIC_PMI_DEV_INDEX, pPowerParams->uWledStringsPrimary, TRUE))
    {
      MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Wled EnableModulator failed\n");
    }

    if (PM_ERR_FLAG__SUCCESS != pm_wled_en_sync(PMIC_PMI_DEV_INDEX, pPowerParams->uWledStringsPrimary, TRUE))
    {
      MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Wled EnableSync failed\n");
    }

    //0xD846             MODULE_EN      0x80       Enable the entire module
    if (PM_ERR_FLAG__SUCCESS != pm_wled_enable_module(PMIC_PMI_DEV_INDEX, 0, TRUE))
    {
      MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Wled EnableWled failed\n");
    }
  }
  else
  {
    // ********** Brightness Power-down Sequence **********
    if (PM_ERR_FLAG__SUCCESS != pm_lcdb_module_enable(PMIC_PMI_DEV_INDEX, FALSE))
    {
      MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Error to disable LCDB\n");
    }

  }
  
  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_BacklightLevel()
**
** DESCRIPTION:
**
*/ /* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_BacklightLevel(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, BacklightConfigType *pBacklightConfig)
{
  MDP_Status                    eStatus = MDP_STATUS_OK;


  if (MDP_PANEL_BACKLIGHTTYPE_PMIC == pBacklightConfig->eBacklightType)
  {
    // Configure the power grid based on the module type
    switch (pBacklightConfig->uBacklightCntrl.eBacklightCtrl)
    {
    case MDP_PMIC_BACKLIGHT_CONTROLTYPE_WLED:
      {

        uint32 uWLEDValue = (0xFFF * pBacklightConfig->uLevel) / 100; // Calculate duty cycle based on 12 bit mode

        if (PM_ERR_FLAG__SUCCESS != pm_wled_set_led_duty_cycle(PMIC_PMI_DEV_INDEX, pPowerParams->uWledStringsPrimary, uWLEDValue))
        {
          MDP_Log_Message(MDP_LOGLEVEL_ERROR, "Wled pm_wled_set_led_duty_cycle failed\n");
        }
      }
      break;
    default:
      break;
    }
  }
  else
  {
    // Nothing to do for other configurations
  }

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: panel_ConfigGPIO()

** 
** DESCRIPTION:
**   Initialize LCD panel GPIOs and PMICs
**
*/ /* -------------------------------------------------------------------- */
MDP_Status MDPPlatformConfigure(MDP_Display_IDType eDisplayId, MDPPlatformConfigType eConfig, MDPPlatformParams *pPlatformParams)
{
  MDP_Status               eStatus    = MDP_STATUS_OK;
  PlatformDSIDetectParams *pPanelList = NULL;
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


  // Read the platform ID once
  if (FALSE == sPlatformInfo.bPlatformDetected)
  {
    if (MDP_STATUS_OK == ReadPlatformIDAndChipID(&sPlatformInfo.sEFIPlatformType, &sPlatformInfo.sEFIChipSetId, &sPlatformInfo.sEFIChipSetFamily))
    {
      UINT32 uPanelOverride = 0;

      // Read the display panel ID override
      if ((MDP_STATUS_OK  == MDP_Display_GetVariable_Integer (PANEL_OVERRIDE_STRINGNAME, &uPanelOverride)) &&
          (uPanelOverride  > 0))
      {
        MDP_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayLib: Panel override enabled (ID:0x%x)\n", uPanelOverride);
        sPlatformInfo.uPanelIdOverride = (uint32) uPanelOverride;
      }
    }
  }

  switch (eConfig)
  {
  case MDPPLATFORM_CONFIG_SW_RENDERER:

    if ((EFI_PLATFORMINFO_TYPE_UNKNOWN == sPlatformInfo.sEFIPlatformType.platform) ||
        (EFI_PLATFORMINFO_TYPE_VIRTIO  == sPlatformInfo.sEFIPlatformType.platform) ||
        (EFI_PLATFORMINFO_TYPE_RUMI    == sPlatformInfo.sEFIPlatformType.platform) ||
        (TRUE == PcdGetBool(PcdDisplayForceSwRenderer)))
    {
      // Treat unknown platforms as a SW model only
      pPlatformParams->sPlatformInfo.bSWRender = TRUE;
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib: SW renderer only\n");
    }
    else
    {
      pPlatformParams->sPlatformInfo.bSWRender = FALSE;
    }
    break;
  case MDPPLATFORM_CONFIG_GETPANELCONFIG:
    {
      pPlatformParams->sPlatformPanel.uDefaultVendor   = 0;
      pPlatformParams->sPlatformPanel.uDefaultRevision = 0;
      pPlatformParams->sPlatformPanel.uPanelId         = 0;

      // Retrieve panel configuration (could be dependent on the interface)
      switch (eDisplayId)
      {
      case MDP_DISPLAY_PRIMARY:
        // Report the proper information depending on the display.
        switch (sPlatformInfo.sEFIPlatformType.platform)
        {
          case EFI_PLATFORMINFO_TYPE_CDP:
          case EFI_PLATFORMINFO_TYPE_MTP:
            pPanelList = panelList_0;
            uPanelCnt  = sizeof(panelList_0)/sizeof(PlatformDSIDetectParams);
 
            /* use default panel */
            pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8 *)pPanelList->psPanelCfg;
            pPlatformParams->sPlatformPanel.uConfigSize     = pPanelList->uPanelCfgSize;
            pPlatformParams->sPlatformPanel.eSelectedPanel  = pPanelList->eSelectedPanel;
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
          default:
            MDP_Log_Message(MDP_LOGLEVEL_WARN, "Ramdump:DisplayLib: Platform id=%d NOT supported\n", sPlatformInfo.sEFIPlatformType.platform);
            break;
        }
        break;
      case MDP_DISPLAY_EXTERNAL:
          pPlatformParams->sPlatformPanel.pPanelXMLConfig = (int8*)dummy_xmldata;
          pPlatformParams->sPlatformPanel.uConfigSize     = sizeof(dummy_xmldata);
          break;
      default:
        break;
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
   		  // Panel power configuration override (set through the environment variable)
   	  	  if (sPlatformInfo.uPowerCfgOverride)
   		  {
   		    sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = sPlatformInfo.uPowerCfgOverride;
   		  }
   		  else
   		  {
   		    // Populate the local power configuration (Force to IBB/LAB for LCD)
   		    //
   		    // Note: OLED users configure this as MDP_PMIC_MODULE_CONTROLTYPE_PM_AMOLED
   		    //		  An incorrect configuration can damage the panel.
   		    //
   		    sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = MDP_PMIC_MODULE_CONTROLTYPE_PM_LCD_WLED;   // LCD Power Configuration (IBB/LAB)
   																										// sPanelPowerCtrl.ePMICSecondaryPower[eDisplayId] = MDP_PMIC_MODULE_CONTROLTYPE_IBB_LAB_OLED;	 // OLED Power Configuration (IBB/LAB)
          }
   		
   		  // CDP/MTP platforms use strings 2 & 3 while QRD uses 1 & 2
   		  // Since there is no dynamic platform detection in ramdump mode, we only enable string 2 so 
   		  // as to avoid damaging the hardware
   		  sPanelPowerCtrl.uWledStringsPrimary = PM_WLED_LED2;
   		
         /*
          * Ramdump mode Primary Power Sequence:
          * MDPPLATFORM_CONFIG_POWERDOWN is not be called at both enter and exit ramdump mode
          * Therefore panel power down is called first before power up panel to obey panel
          * power up sequence
          */
          if (MDP_STATUS_OK != (eStatus = Panel_CDP_PowerDown(eDisplayId, &sPanelPowerCtrl, &sPlatformInfo)))
          {
            MDP_Log_Message(MDP_LOGLEVEL_ERROR, "Ramdump: Panel_PowerDown: Primary Power Down Sequence Failed (%d)\n", eStatus);
          }
          else if (MDP_STATUS_OK != (eStatus = Panel_CDP_PeripheralPower(eDisplayId, &sPanelPowerCtrl, FALSE)))  // Secondary Power Sequence
          {
            MDP_Log_Message(MDP_LOGLEVEL_ERROR, "Ramdump: Panel_PowerDown: Secondary Power Down Sequence Failed (%d)\n", eStatus);
          }

          MDP_OSAL_DELAYMS(10);   /* delay 10ms to allow power grid to settle */

   		  // Primary Power Sequence
   		  if (MDP_STATUS_OK != (eStatus = Panel_CDP_PowerUp(eDisplayId, &sPanelPowerCtrl, &sPlatformInfo)))
   	 	  {
   		    MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib: Primary Power Up Sequence Failed (%d)\n", eStatus);
   		  }
   		  else if (MDP_STATUS_OK != (eStatus = Panel_CDP_PeripheralPower(eDisplayId, &sPanelPowerCtrl, TRUE)))  // Secondary Power Sequence
   		  {
   		    MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib: Secondary Power Up Sequence Failed (%d)\n", eStatus);
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

  case MDPPLATFORM_CONFIG_POWERDOWN:
    {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "Ramdump:DisplayLib: Primary Power Down NOT supported\n");

    }
    break;
  case MDPPLATFORM_CONFIG_SETBACKLIGHT:
    {
      // Handle backlight level
      switch (eDisplayId)
      {
      case MDP_DISPLAY_PRIMARY:
        switch (sPlatformInfo.sEFIPlatformType.platform)
        {
        case EFI_PLATFORMINFO_TYPE_CDP:
          eStatus = Panel_CDP_BacklightLevel(eDisplayId, &sPanelPowerCtrl, &pPlatformParams->sBacklightConfig);
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
  case MDPPLATFORM_CONFIG_GETPANELID:
    {
    }
    break;
  case MDPPLATFORM_CONFIG_GETPLATFORMINFO:
    {
      //
      // Return platform information
      //
      MDP_OSAL_MEMCPY(&pPlatformParams->sPlatformInfo, &sPlatformInfo, sizeof(MDPPlatformInfo));
    }
    break;
  case MDPPLATFORM_CONFIG_RESETPANEL:
    {
      if (pPlatformParams->sPlatformPanelReset.uResetGpio == 0)
      {
           sPanelPowerCtrl.uResetGpio = (EFICHIPINFO_ID_SXR1120 == sPlatformInfo.sEFIChipSetId ? CDP_DISP_SXR1120_RESET_GPIO : CDP_DISP_RESET_GPIO);
           pPlatformParams->sPlatformPanelReset.uResetGpio = (EFICHIPINFO_ID_SXR1120 == sPlatformInfo.sEFIChipSetId ? CDP_DISP_SXR1120_RESET_GPIO : CDP_DISP_RESET_GPIO);
      }
      else 
      {
           sPanelPowerCtrl.uResetGpio = pPlatformParams->sPlatformPanelReset.uResetGpio;
      }
	  
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
      eStatus = SetDefaultGPIOState(&pPlatformParams->sGPIOState);
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
*/ /* -------------------------------------------------------------------- */
static MDP_Status ReadPlatformIDAndChipID(EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *pPlatformInfo, EFIChipInfoIdType *pChipSetId, EFIChipInfoFamilyType *pChipSetFamily)
{
  DalPlatformInfoPlatformInfoType      dalPlatformInfo;
  DALResult                            eDalStatus = DAL_SUCCESS;
  MDP_Status                           eStatus    = MDP_STATUS_OK;
  MDP_OSAL_MEMZERO(&dalPlatformInfo,    sizeof(DalPlatformInfoPlatformInfoType));
  
  eDalStatus = boot_DalPlatformInfo_PlatformInfo(&dalPlatformInfo);
  if (eDalStatus != DAL_SUCCESS)
  {
     MDP_Log_Message(MDP_LOGLEVEL_ERROR, "Ramdump:Display:ReadPlatformIDAndChipId: Failed\n");

     eStatus    = MDP_STATUS_FAILED;
  }
  else 
  {
    pPlatformInfo->platform = (EFI_PLATFORMINFO_PLATFORM_TYPE)dalPlatformInfo.platform;
    pPlatformInfo->version  = dalPlatformInfo.version;
    pPlatformInfo->subtype  = dalPlatformInfo.subtype;
    pPlatformInfo->fusion   = dalPlatformInfo.fusion;

    eDalStatus = boot_DalPlatformInfo_ChipId(pChipSetId);
    eDalStatus = boot_DalPlatformInfo_ChipFamily(pChipSetFamily);
  }

  return eStatus;
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
      uint32              uCount;

      // Loop the list to configure all GPIOs.
      for (uCount = 0;uCount < pGPIOList->uListSize;uCount++)
      {
        // Only try to program GPIOs that are within range, TLMM Macro (EFI_GPIO_CFG) supports up to 0x3FF
        if (pGPIOList->pGPIOList[uCount] < 0x3FF)
        {

          DALGpioSignalType   uGPIOCfg = 0;
          uint32              uValue  = (pGPIOList->uState > 0) ? DAL_GPIO_HIGH_VALUE : DAL_GPIO_LOW_VALUE;

          uGPIOCfg = (DALGpioSignalType)DAL_GPIO_CFG_OUT(pGPIOList->pGPIOList[uCount], 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, uValue);
          Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, &uGPIOCfg, 1);
        }
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
