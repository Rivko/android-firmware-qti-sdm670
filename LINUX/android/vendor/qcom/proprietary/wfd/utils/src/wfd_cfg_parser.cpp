/*==============================================================================
*        @file wfd_cfg_parser.cpp
*
*  @par DESCRIPTION:
*       Definition of the wfd xml configuration file parser
*
*
*  Copyright (c) 2012-2018 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

/*===============================================================================
                            Header Inclusion
===============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "MMMalloc.h"
#include "MMFile.h"
#include "wfd_cfg_parser.h"
#include <string>
#include <vector>
#include <map>
#include <cutils/properties.h>
#include "WFDUtils.h"
using namespace std;

typedef enum DeviceType: int {
    SOURCE,
    PRIMARY_SINK,
    SECONDARY_SINK,
    UNKNOWN,
}wfdDevType;
wfdDevType wfdDevice;

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "wfd_cfg_parser"

#ifdef __cplusplus
extern "C" {
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
      string getValueForTag(string buffer, string tag);
#pragma clang diagnostic pop
      void dumpcapabilitiesRead();
      void parseHWID();
      void parseHWVer();
      void parseXml(string buffer);

      void parseAudioCodec(string, audioCodecName);
      bool initVideoFormatExtResources();
      bool initAuxStreamInfo();
      void parseAVFormatsExtension(string);
      void parseExtVideoCodecHeader(string);
      void parseExtendedVideoCodec(string,int);
      void parseAuxCodec(string buffer);
      bool initWhiteList(int count);
      void parseWhiteListApp(string buffer);
      uint32 getAuxSupportedBitMap(int mode,int width , int height);
      void parseStandby(string);
      void parseidrRequest(string buffer);
      void parseFrameSkipping(string buffer);
      void parseHDCPSupport(string buffer);
      void parseRTPPortType(string buffer);
      unsigned long long getVideoModes(WFD_Video_modes currMode, long int macroBlocks,
                             int videoModesCount, unsigned int uMaxHRes,
                             unsigned int uMaxVRes);
      void updateVar(const char* key, const cfgKeys cfgKey, const string tagValue);
      int getSupportedHDCPVersion(string version);
      void parseRIRSupport(string buffer);
#ifdef __cplusplus
}
#endif
#define VDEC_LOG_HIGH  1 // MMOSAL Macro for priting HIGH messages

#include "MMDebugMsg.h"

bool bRTPDumpEnable;
int mnHW_ID;
int mnHW_Ver;
typedef pair<int,int> pair_val;
vector<int> cfgItems(TOTAL_CFG_KEYS,-1);
map<string,pair_val> cfgItemsStr;

/* =============================================================================
                         Macro Definitions
* =========================================================================== */

#define MACRO_BLOCK_SIZE 256
#define MACRO_BLOCKS(w,h,fps)  (((w*h + MACRO_BLOCK_SIZE - 1)/MACRO_BLOCK_SIZE)*fps)

#define NUM_CEA_MODES   27
#define NUM_VESA_MODES  34
#define NUM_HH_MODES    12

#define LPCM_MODE_FREQ_1 44100
#define LPCM_MODE_FREQ_2 48000

#define AAC_AC3_MODE_FREQ 48000

#define MAX_SUPPORTED_WIDTH  4096
#define MAX_SUPPORTED_HEIGHT 2160
#define MAX_SUPPORTED_FPS 60
#define MAX_ALLOWED_FPS_FOR_MAX_RES 30

#define AUDIO_2_CHANNELS 2
#define AUDIO_4_CHANNELS 4
#define AUDIO_6_CHANNELS 6
#define AUDIO_8_CHANNELS 8

#define AUDIO_MIN_CHANNELS AUDIO_2_CHANNELS
#define AUDIO_MAX_CHANNELS AUDIO_8_CHANNELS

/* Max length for Name of application.
   It should be same as WFDMMService.h file*/
#define WHITELIST_APP_NAME_LENGTH 100


static readConfigFile *pReadCfgCapabilities;



#ifdef __cplusplus
extern "C" {
#endif



/* =======================================================================
                Function Definitions
* ======================================================================= */


/*==============================================================================

         FUNCTION:         getVideoModes

         DESCRIPTION:
         @brief           All bits set to 1 whose MB count is less than the
                          calculated MB count
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - macroBlocks - calculated from user entered resolution
                                       - ((Vres*Hres*Fps)/(16*16))
                        - mbData - MB values for each resolution - pre calculated
                        - modeCount - number of resolutions supported count

*//*     RETURN VALUE:
*//**       @return
                        - uBitMap - video bitmap


@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

unsigned long long  getVideoModes(WFD_Video_modes currMode, long int macroBlocks,
                        int videoModesCount,unsigned int uMaxHRes,
                        unsigned int uMaxVRes )
{
   int uLoopCounter = 0;
   unsigned long long uBitMap = 0;
   const WFD_MM_VideoCommonParamsType *pVideoParams;

   if(currMode < WFD_VIDEO_NUM_MODES)
   {
       pVideoParams = WFD_MM_VideoParamsTables[currMode];
   }
   else
   {
       pVideoParams = NULL;
   }

   MM_MSG_PRIO5(MM_GENERAL, MM_PRIO_ERROR, "getVideoModes %d, %ld, %ld, %lu, %lu",
                currMode, macroBlocks, videoModesCount, uMaxHRes, uMaxVRes);



   if(pVideoParams && videoModesCount)
   {
      for( uLoopCounter = (videoModesCount-1) ;uLoopCounter >= 0; uLoopCounter-- )
      {
         uBitMap <<= 1;
         /* Check if the valid bit is set and MB value less than user specified value */
         MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_LOW, "getVideoModes supported =%d MBsperSec = %lld",
                pVideoParams[uLoopCounter].bSupported, (pVideoParams[uLoopCounter].nMBsPerFrameMBsPerSec & 0xffffffffLL));

         if(pVideoParams[uLoopCounter].bSupported &&
            (pVideoParams[uLoopCounter].nMBsPerFrameMBsPerSec & 0xffffffffLL)
                                                  <= (uint64)macroBlocks &&
            pVideoParams[uLoopCounter].nHeight    <= uMaxVRes &&
            pVideoParams[uLoopCounter].nWidth     <= uMaxHRes)
         {
            uBitMap |= (unsigned long long)1;
         }
      }
   }
   return uBitMap;
}

/*==============================================================================

         FUNCTION:    getValueForTag

         DESCRIPTION:
         @brief       Get value corresponding to string "tag" in string "buffer"
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer  - buffer to parser for tag
                        - tag - the tag to find in the buffer

*//*     RETURN VALUE:
*//**       @return
                        - tag  - identified tag

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

string  getValueForTag(string buffer, string tag)
{
   string tagStart = "<" + tag + ">";
   string tagEnd = "</" + tag + ">";
   string retString;
   size_t begin, end;

   if ((begin = buffer.find(tagStart)) != string::npos)
   {
      if ((end = buffer.find(tagEnd)) != string::npos)
      {
         begin += tagStart.length();
         size_t len = end - begin;
         char *resStr = (char *)MM_Malloc(static_cast<int>(sizeof(char)*(len+1)));
         if(!resStr)
         {
            MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"getValueForTag: Memory Allocation Failure");
            return "";
         }
         size_t stop = buffer.copy(resStr, len, begin);
         resStr[stop] = '\0';
         retString = string(resStr);
         MM_Free(resStr);
      }
   }
   return retString;
}

/*==============================================================================

         FUNCTION:    parseUIBCSupport

         DESCRIPTION:
         @brief       Parse XML for UIBC Support capabilities
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseUIBCSupport(string uibcBuffer)
{
   string tagValue;
   bool bValid = false, bGeneric = false, bHID = false;

   if(!uibcBuffer.c_str())
   {
      MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"parseUIBCSupport: buffer is null");
      return;
   }
   tagValue = getValueForTag(uibcBuffer, UIBC_VALID_KEY);
   bValid = atoi(tagValue.c_str()) ? true : false;
   if(tagValue.length())
   {
     updateVar(UIBC_VALID_KEY,UIBC_VALID,tagValue);
   }
   if(bValid)
   {
      pReadCfgCapabilities->uibcCapability.port_id =
               ( uint16 ) atoi((getValueForTag(uibcBuffer, "TcpPort")).c_str()) ;

      tagValue = getValueForTag(uibcBuffer,"InputCategory");
      bGeneric = atoi((getValueForTag(uibcBuffer, "Generic")).c_str()) ? true : false;
      bHID = atoi((getValueForTag(uibcBuffer, "HID")).c_str()) ? true : false;
      tagValue = getValueForTag(uibcBuffer,UIBC_M14_KEY);
      if(tagValue.length())
      {
        updateVar(UIBC_M14_KEY,UIBC_M14,tagValue);
      }
      tagValue = getValueForTag(uibcBuffer,UIBC_M4_ENABLE_KEY);
      if(tagValue.length())
      {
        updateVar(UIBC_M4_ENABLE_KEY,UIBC_M4_ENABLE,tagValue);
      }
      if(bGeneric)
      {
         pReadCfgCapabilities->uibcCapability.config.category |= GENERIC;
         tagValue = getValueForTag(uibcBuffer,"GenericInputEvents");

         pReadCfgCapabilities->uibcCapability.config.generic_input_type = 0;

         if(atoi((getValueForTag(tagValue, "Keyboard")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= KEYBOARD;
         }
         if(atoi((getValueForTag(tagValue, "Mouse")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= MOUSE;
         }
         if(atoi((getValueForTag(tagValue, "SingleTouch")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= SINGLETOUCH;
         }
         if(atoi((getValueForTag(tagValue, "MultiTouch")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= MULTITOUCH;
         }
         if(atoi((getValueForTag(tagValue, "JoyStick")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= JOYSTICK;
         }
         if(atoi((getValueForTag(tagValue, "Gesture")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= GESTURE;
         }
         if(atoi((getValueForTag(tagValue, "Camera")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= CAMERA;
         }
         if(atoi((getValueForTag(tagValue, "RemoteControl")).c_str()) ? true : false)
         {
            pReadCfgCapabilities->uibcCapability.config.generic_input_type |= REMOTECONTROL;
         }
         MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_HIGH,"parseUIBCSupport: generic \
            input = %d",pReadCfgCapabilities->uibcCapability.config.generic_input_type);
      }
      if(bHID)
      {
        pReadCfgCapabilities->uibcCapability.config.category |= HIDC;
        string sHIDSup = getValueForTag(uibcBuffer,"HIDInputPaths");
        int nHIDInpPath = 0;
        uint8 uHIDBitset = 0;
        for(int i= 0; i< UIBC_NUM_INPUT_PATHS; i++)
        {
            pReadCfgCapabilities->uibcCapability.config.hid_input_type_path[i]= 0;
            switch(i)
            {
                case 0:
                    nHIDInpPath = INFRARED;
                    uHIDBitset = (uint8)atoi((getValueForTag(sHIDSup,"Infrared")).c_str());
                    break;
                case 1:
                    nHIDInpPath = USB;
                    uHIDBitset = (uint8)atoi((getValueForTag(sHIDSup,"USB")).c_str());
                    break;
                case 2:
                    nHIDInpPath = BT;
                    uHIDBitset = (uint8)atoi((getValueForTag(sHIDSup,"BT")).c_str());
                    break;
                case 3:
                    nHIDInpPath = ZIGBEE;
                    uHIDBitset = (uint8)atoi((getValueForTag(sHIDSup,"Zigbee")).c_str());
                    break;
                case 4:
                    nHIDInpPath = WIFI;
                    uHIDBitset = (uint8)atoi((getValueForTag(sHIDSup,"Wifi")).c_str());
                    break;
                case 5:
                    nHIDInpPath = NOSP;
                    uHIDBitset = (uint8)atoi((getValueForTag(sHIDSup,"NoSP")).c_str());
                    break;
                default:
                    nHIDInpPath = NOSP;
                    uHIDBitset = (uint8)atoi((getValueForTag(sHIDSup,"NoSP")).c_str());
            }
            pReadCfgCapabilities->uibcCapability.config.hid_input_type_path[nHIDInpPath]=uHIDBitset;
        }
      }
      if(!bGeneric && !bHID)
      {
         pReadCfgCapabilities->uibcCapability.port_id = 0;
         pReadCfgCapabilities->uibcCapability.config.category= 0;
         pReadCfgCapabilities->uibcCapability.config.generic_input_type = 0;
         return;
      }
   }
   else
   {
      pReadCfgCapabilities->uibcCapability.port_id = 0;
      pReadCfgCapabilities->uibcCapability.config.category= 0;
      pReadCfgCapabilities->uibcCapability.config.generic_input_type = 0;
      return;
   }
}

/*==============================================================================

         FUNCTION:    parseStandby

         DESCRIPTION:
         @brief       Parse XML for stand by resume capabilities
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag
                        - tag - the tag to find in the buffer

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseStandby(string standByBuffer)
{
   bool valid = false;
   valid = atoi((getValueForTag(standByBuffer, "Valid")).c_str()) ? true : false;
   pReadCfgCapabilities->pCfgCapabilities->standby_resume_support = valid;
}

/*==============================================================================

         FUNCTION:    parseidrRequest

         DESCRIPTION:
         @brief       Parse XML for IDR Request validity and interval
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag
                        - tag - the tag to find in the buffer

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseidrRequest(string idrReqBuffer)
{
   pReadCfgCapabilities->idrRequestCapability.idrReqValid
      = atoi((getValueForTag(idrReqBuffer, "Valid")).c_str()) ? true : false;
   pReadCfgCapabilities->idrRequestCapability.idrIntvl
      = (unsigned long)atoi((getValueForTag(idrReqBuffer, "IDRInterval")).c_str());
}

/*==============================================================================

         FUNCTION:    parseRIRSupport

         DESCRIPTION:
         @brief       Parse XML for Random IR validity and interval
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseRIRSupport(string buffer)
{
    if(!buffer.c_str())
    {
        MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"Invalid param to parseRIRSupport");
        return;
    }
    string tagvalue = getValueForTag(buffer,PERIODIC_IDR_INTERVAL_VALID_KEY);
    if(tagvalue.length())
    {
        updateVar(PERIODIC_IDR_INTERVAL_VALID_KEY,PERIODIC_IDR_INTERVAL_VALID,tagvalue);
    }
    tagvalue = getValueForTag(buffer,PERIODIC_IDR_INTERVAL_KEY);
    if(tagvalue.length())
    {
        updateVar(PERIODIC_IDR_INTERVAL_KEY,PERIODIC_IDR_INTERVAL,tagvalue);
    }
}

/*==============================================================================

         FUNCTION:    parseFrameSkipping

         DESCRIPTION:
         @brief        Parse XML for Frame Skipping validity and interval
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag
                        - tag - the tag to find in the buffer

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseFrameSkipping(string fsBuffer)
{
   pReadCfgCapabilities->frameSkippingCapability.frameSkipValid
      = atoi((getValueForTag(fsBuffer, "Valid")).c_str()) ? true : false;
   pReadCfgCapabilities->frameSkippingCapability.frameSkipIntvl
      = (unsigned short)atoi((getValueForTag(fsBuffer, "FrameInterval")).c_str());
   if(pReadCfgCapabilities->frameSkippingCapability.frameSkipValid)
   {
     for(int index=0; index < pReadCfgCapabilities->pCfgCapabilities->video_capability.num_codec; index++)
     {
         /*Store Frame skipping Support -- bit 1 of frame_rate_control_support */
        ( pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index] ).frame_rate_control_support
                  |= 0X01;
        ( pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index] ).frame_rate_control_support
                  |= static_cast<uint8>((pReadCfgCapabilities->frameSkippingCapability.frameSkipIntvl << 1));
     }
   }
}

/*==============================================================================

         FUNCTION:    parseHDCPSupport

         DESCRIPTION:
         @brief        Parse XML for Content Protection validity
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag
                        - tag - the tag to find in the buffer

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseHDCPSupport(string hdcpBuffer)
{
   bool bValid = false;
   bValid = atoi((getValueForTag(hdcpBuffer, "Valid")).c_str()) ? true : false;
   if(bValid)
   {
      string version((getValueForTag(hdcpBuffer,"Version")).c_str()) ;

      pReadCfgCapabilities->pCfgCapabilities->content_protection_config.content_protection_capability
               = static_cast<uint8>(getSupportedHDCPVersion(version));
      pReadCfgCapabilities->pCfgCapabilities->content_protection_config.content_protection_ake_port
               = ( uint16 ) atoi((getValueForTag(hdcpBuffer, "CPPort")).c_str()) ;
   }
   else
   {
      /* 0 in HDCP version indicates no support for HDCP */
      pReadCfgCapabilities->pCfgCapabilities->content_protection_config.content_protection_capability = 0;
      pReadCfgCapabilities->pCfgCapabilities->content_protection_config.content_protection_ake_port = 0;
   }
}

/*==============================================================================

         FUNCTION:    parseRTPPortType

         DESCRIPTION:
         @brief        Parse XML for RTP port connection type
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseRTPPortType(string rtpBuffer)
{
   pReadCfgCapabilities->pCfgCapabilities->transport_capability_config.eRtpPortType =
      (WFD_rtp_port_type )atoi((getValueForTag(rtpBuffer, "ConnectionType")).c_str()) ;
   MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_HIGH,"RTP Port Connection Type = %d",
   pReadCfgCapabilities->pCfgCapabilities->transport_capability_config.eRtpPortType);
}

/*==============================================================================

         FUNCTION:    parseRTPDumpEnable

         DESCRIPTION:
         @brief        Parse XML for RTP DumpEnable boolean flag
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseRTPDumpEnable(string buffer)
{
    MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"RTPDumpEnable deprecated");
    /*DEPRECATED.

      For enabling RTP encoder/parser dumping- set property
      "persist.sys.enable_RTPDumps" to  1
      */
}

#if 0
/*==============================================================================

         FUNCTION:    parseWFDSessionCodec

         DESCRIPTION:
         @brief        Parse XML for WFD Session Codec information
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - videoBuffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/
void parseWFDSessionCodec(string buffer)
{
    string tagValue;
    tagValue = getValueForTag(buffer, "VideoHeader");
    if(tagValue.length())
        parseVideoHeader(tagValue);

    string sVideoCodec = getValueForTag(buffer, "CBP");
    if(sVideoCodec.length())
    {
        tagValue = getValueForTag(sVideoCodec, "VideoCodec");
    }

    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"PARSE CBP");
    if(tagValue.length())
        parseVideoCodec(tagValue, 0);

    if(pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.num_h264_profiles > 0)
    {
        string sVideoCodec = getValueForTag(buffer, "CHP");
        if(sVideoCodec.length())
        {
            tagValue = getValueForTag(sVideoCodec, "VideoCodec");
        }

        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"PARSE CHP");
        if(tagValue.length())
            parseVideoCodec(tagValue, 1);
    }
}

/*==============================================================================

         FUNCTION:    parseVideoHeader

         DESCRIPTION:
         @brief        Parse XML for Video Header Parameters
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - videoBuffer to parser for tag
                        - tag - the tag to find in the buffer

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseVideoHeader(string vidheadbuffer)
{

   pReadCfgCapabilities->pCfgCapabilities->video_method
      = (WFD_video_type )atoi((getValueForTag(vidheadbuffer, "VideoMethod")).c_str());
   pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.preferred_display_mode_supported
      = (uint8)atoi((getValueForTag(vidheadbuffer, "PreferredDisplaySupport")).c_str());
   pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.num_h264_profiles
      = (uint8)atoi((getValueForTag(vidheadbuffer, "H264Profiles")).c_str());
}

/*==============================================================================

         FUNCTION:    parseVideoCodec

         DESCRIPTION:
         @brief        Parse XML for Video Codec Parameters
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - videoBuffer - buffer to parser for tag
                        - tag - the tag to find in the videoBuffer

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/

void  parseVideoCodec(string videoBuffer, int index)
{
   int      fpsSupported = 0;
   uint8    nativeModes  = 0;
   uint16   uMaxHRes     = 0;
   uint16   uMaxVRes     = 0;
   long int uMacroBlocks = 0;

   uint32   hhBitMap     = 0;
   uint32   vesaBitMap   = 0;
   uint32   ceaBitMap    = 0;

   /* Profile Values : CBP or CHP */
   string  profile((getValueForTag(videoBuffer, "Profile")).c_str());
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).h264_profile = (uint8)atoi((getValueForTag(videoBuffer, "Profile")).c_str());
/*    = profile.compare("CBP")?
            ( profile.compare("CHP") ? H264_SUPPORTED_PROFILE_CBP : H264_SUPPORTED_PROFILE_CHP )
        : BIT0; */

   MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"wfd_cfg_parser : user enterted profile %d for index %d",
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).h264_profile, index);

   /* Level values : 3.1/3.2/4/4.1/4.2*/
   string  level((getValueForTag(videoBuffer, "Level")).c_str());

   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).h264_level
   = level.compare("3.1") ?
      (level.compare("3.2")?
         (level.compare("4")?
            (level.compare("4.1")?
               (level.compare("4.2")? BIT0   // Level 3.1, BIT0 by default
                                    : BIT4 )
            : BIT3 )
         : BIT2 )
         : BIT1 )
     : BIT0;

    MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_HIGH,"user enterted level %d ",
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).h264_level);

   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).max_hres = uMaxHRes
      = (uint16)atoi((getValueForTag(videoBuffer, "HorizontalResolution")).c_str());
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).max_vres
      = uMaxVRes = (uint16)atoi((getValueForTag(videoBuffer, "VerticalResolution")).c_str());
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).decoder_latency
      = (uint8)atoi((getValueForTag(videoBuffer, "Latency")).c_str());
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).min_slice_size
      = (uint16)atoi((getValueForTag(videoBuffer, "MinimumSliceSize")).c_str());
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).slice_enc_params
      = (uint8)atoi((getValueForTag(videoBuffer, "SliceEncodingParams")).c_str());
   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).frame_rate_control_support
      = (uint8)atoi((getValueForTag(videoBuffer, "FrameRateControlSupport")).c_str());

   fpsSupported = (unsigned int )atoi((getValueForTag(videoBuffer, "VideoFps")).c_str());

   MM_MSG_PRIO3(MM_GENERAL,MM_PRIO_HIGH,"wfd_cfg_parser : user enterted reolution %d x %d %d fps",
                 uMaxHRes,uMaxVRes,fpsSupported);

   switch(mnHW_ID)
   {
     /*--1--*/
     case 206:
     /*--2--*/
     case 294:
     case 295:
     /*--3--*/
     case 303:
     case 307:
     /*--4--*/
     case 313:
        if(wfdDevice == SOURCE && (uMaxHRes > 1280 || uMaxVRes > 720))
        {
            uMaxHRes = 1280;
            (pReadCfgCapabilities->pCfgCapabilities->video_config
                    .video_config.h264_codec[index]).max_hres = uMaxHRes;
            uMaxVRes = 720;
            (pReadCfgCapabilities->pCfgCapabilities->video_config
                    .video_config.h264_codec[index]).max_vres = uMaxVRes;
            MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,
             "wfd_cfg_parser : Updated resolution %d x %d ",uMaxHRes,uMaxVRes);
        }
        break;

      case 239:
      case 240:
      case 241:
      case 242:
      case 243: /* Place Holder if something is needed. */
      default:
        break;
   }
   if((uMaxHRes <= MAX_SUPPORTED_WIDTH)&& (uMaxVRes <= MAX_SUPPORTED_HEIGHT)
                                       && (fpsSupported <= MAX_SUPPORTED_FPS))
   {
      uMacroBlocks = MACRO_BLOCKS(uMaxHRes, uMaxVRes, fpsSupported);
   }
   else
   {
      uMacroBlocks = MACRO_BLOCKS(MAX_SUPPORTED_WIDTH, MAX_SUPPORTED_HEIGHT,
                                  MAX_ALLOWED_FPS_FOR_MAX_RES);
   }


   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).supported_cea_mode
      = ceaBitMap = (uint32)getVideoModes(WFD_VIDEO_CEA_MODE, uMacroBlocks, NUM_CEA_MODES, uMaxHRes, uMaxVRes );


   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).supported_vesa_mode
      = vesaBitMap = (uint32)getVideoModes(WFD_VIDEO_VESA_MODE, uMacroBlocks, NUM_VESA_MODES, uMaxHRes, uMaxVRes );


   (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).supported_hh_mode
      = hhBitMap = (uint32)getVideoModes(WFD_VIDEO_HH_MODE, uMacroBlocks, NUM_HH_MODES, uMaxHRes, uMaxVRes );

    nativeModes = vesaBitMap ? 1 : 0;
    nativeModes = hhBitMap ? (1 << 1) : 0 ;

   // Default bit map CEA BIT0, nativeModes = 0;
    if(!nativeModes && !ceaBitMap && !vesaBitMap && !hhBitMap)
    {
      MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR," Defaulting to VGA");
      (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).supported_cea_mode =
                                                                                         H264_MANDATORY_CEA_640x480p60;
      (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).max_hres = 640;
      (pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.h264_codec[index]).max_vres = 480;
       nativeModes = 0;
    }
    pReadCfgCapabilities->pCfgCapabilities->video_config.video_config.native_bitmap = nativeModes;
}
#endif

/*==============================================================================

         FUNCTION:    initVideoFormatExtResources

         DESCRIPTION:
         @brief        Initialize Data Structures related to video_format_extension
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/
bool initVideoFormatExtResources()
{
    int nCount = pReadCfgCapabilities->numAVFormatsExtention;
    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,
                 "initVideoFormatExtResources numAVFormatsExtention=%d", nCount);

    /*Initialize*/
    if(pReadCfgCapabilities->pCfgCapabilities == NULL)
    {
        pReadCfgCapabilities->pCfgCapabilities = (WFD_MM_capability_t *)
            MM_Malloc(sizeof(WFD_MM_capability_t));

        if(pReadCfgCapabilities->pCfgCapabilities == NULL)
        {
            MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,
                "initVideoFormatExtResouces() - Failed to allocate resources");
            return false;
        }
        memset(pReadCfgCapabilities->pCfgCapabilities,0,sizeof(WFD_MM_capability_t));
    }

    memset(pReadCfgCapabilities->pCfgCapabilities->video_capability.codec,0,
        sizeof(WFD_video_codec_capability_t) * WFD_MAX_NUM_VIDEO_CODEC);

    return true;
}


bool initAuxStreamInfo()
{
    /*Sanity check*/
    if(pReadCfgCapabilities->pAuxCfgCapabilities != NULL)
    {
        MM_Free(pReadCfgCapabilities->pAuxCfgCapabilities);
        pReadCfgCapabilities->pAuxCfgCapabilities = NULL;
    }

    /*Initialize*/
    pReadCfgCapabilities->pAuxCfgCapabilities = (WFD_MM_Aux_capability_t *)
        MM_Malloc(sizeof(WFD_MM_Aux_capability_t));

    if(pReadCfgCapabilities->pAuxCfgCapabilities == NULL)
    {
        MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,
            "initAuxStreamInfo() - Failed to allocate resources");
        return false;
    }
    memset(pReadCfgCapabilities->pAuxCfgCapabilities,0,sizeof(WFD_MM_Aux_capability_t));
    return true;
}

/*==============================================================================

         FUNCTION:    parseExtendedVideoCodec

         DESCRIPTION:
         @brief        Parse XML for Extended Video Codec information
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag
                        - index  - to store the information at appropriate index

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/
void parseExtendedVideoCodec(string videoBuffer, int index)
{
   int fpsSupported;
   uint8 nativeModes = 0;
   uint16 uMaxHRes,uMaxVRes;
   long int uMacroBlocks = 0;
   int ceaModeCount = NUM_CEA_MODES;
   int vesaModeCount = NUM_VESA_MODES;
   int hhModeCount = NUM_HH_MODES;
   uint32  hhBitMap = 0;
   uint64  vesaBitMap = 0LL;
   uint32  ceaBitMap = 0;
   uint8 frameRateCtrlSupp = 0;

   if ((pReadCfgCapabilities == NULL) || (pReadCfgCapabilities->pCfgCapabilities == NULL))
   {
       MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,
         "pReadCfgCapabilities or pReadCfgCapabilities->pCfgCapabilities is NULL");
       return;
   }

   /* Store the CODEC name*/
   string codecName = getValueForTag(videoBuffer,"CodecName");
   strlcpy((pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).name,
           codecName.c_str(),sizeof((pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).name ));

   /*Store Profile Value*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).profile
      = (uint8)atoi((getValueForTag(videoBuffer, "Profile")).c_str());

   /*Parse Level based on Codec Name*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).level
      = (uint8)atoi((getValueForTag(videoBuffer,"Level")).c_str());

   /*Store Horizontal Resolution*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_hres
      = uMaxHRes = (uint16)atoi((getValueForTag(videoBuffer, "HorizontalResolution")).c_str());

   /*Store Vertical Resolution*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_vres
      = uMaxVRes = (uint16)atoi((getValueForTag(videoBuffer, "VerticalResolution")).c_str());

    if((uMaxVRes > 1080) || (uMaxHRes > 1920)) /* Check if the wlan is capable of supporting 4k */
    {
        /* If WLAN doesn't support 4K or target spec doesn't allow 4K */
        if(!pReadCfgCapabilities->pCfgCapabilities->link_properties.enable_4k ||
            /*---- 1 ----*/
            mnHW_ID == 317 ||
            /*---- 2 ----*/
            mnHW_ID == 318)
        {
            MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_HIGH, "enable_4k = %d",
                                                       pReadCfgCapabilities->pCfgCapabilities->link_properties.enable_4k);

            /*Store Horizontal Resolution*/
            (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_hres
               = uMaxHRes = 1920;

            /*Store Vertical Resolution*/
            (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_vres
              = uMaxVRes = 1080;
        }
    }

   /*Store decoder latency*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).decoder_latency
       = (uint8)atoi((getValueForTag(videoBuffer, "Latency")).c_str());

   /*Store Minimum Slice size*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).min_slice_size
       = (uint16)atoi((getValueForTag(videoBuffer, "MinimumSliceSize")).c_str());

   /*Store Slice Encoding Params*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).slice_enc_params
       = (uint8)atoi((getValueForTag(videoBuffer, "SliceEncodingParams")).c_str());

   /*Store Frame Rate Control Support -- bit 4 of frame_rate_control_support */
   frameRateCtrlSupp = (uint8)atoi((getValueForTag(videoBuffer, "FrameRateControlSupp")).c_str());
   if(frameRateCtrlSupp == 1)
   {
     (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).frame_rate_control_support |= 0x10;
   }

   fpsSupported = (unsigned int )atoi((getValueForTag(videoBuffer, "VideoFps")).c_str());

   MM_MSG_PRIO4(MM_GENERAL,MM_PRIO_HIGH,"user enterted resolution for codecNum = %d [%d x %d %d fps]",index,uMaxHRes,uMaxVRes,fpsSupported);

   switch(mnHW_ID)
   {
      /*--1--*/
      case 206:
      /*--2--*/
      case 294:
      case 295:
      /*--3--*/
      case 303:
      case 307:
      /*--4--*/
      case 313:
      /*--5--*/
      case 320:
      /*--6--*/
      case 353:
      case 354:
        if(wfdDevice == SOURCE && (uMaxHRes > 1280 || uMaxVRes > 720))
        {
            uMaxHRes = 1280;
            (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_hres = 1280;
            uMaxVRes = 720;
            (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_vres = 720;

            MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"wfd_cfg_parser : Updated resolution %d x %d ",uMaxHRes,uMaxVRes);
            break;
        } // Fallthrough for sink in all cases
      /*--7--*/
      case 293:
      case 304:
      /*--8--*/
      case 278:
      /*--9--*/
      case 336:
      case 337:
      /*-10--*/
      case 338:
      case 351:
      /*-11--*/
      case 349:
      case 350:
        if(mnHW_Ver == 0)
        {
            break; // Higher Spec hence not limiting.
        }
        if(wfdDevice == SOURCE && (uMaxHRes > 1920 || uMaxVRes > 1080))
        {
            uMaxHRes = 1920;
            (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_hres = 1920;
            uMaxVRes = 1080;
            (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_vres = 1080;

            MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"wfd_cfg_parser : Updated resolution %d x %d ",uMaxHRes,uMaxVRes);
            break;
        } // Fallthrough for sink in all cases
      case 317:
      /*--8--*/
      case 318:
      /*--Others--*/
      case 239:
      case 240:
      case 241:
      case 242:
      case 243:
          if(wfdDevice != SOURCE && (uMaxHRes > 1920 || uMaxVRes > 1080))
          {
             uMaxHRes = 1920;
             (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_hres = 1920;
             uMaxVRes = 1080;
             (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_vres = 1080;
             MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"wfd_cfg_parser : Updated resolution %d x %d ",uMaxHRes,uMaxVRes);
             (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).level = 2;//level 4.0
          }
          break;
      default:
         break;
   }

   if((uMaxHRes <= MAX_SUPPORTED_WIDTH)&& (uMaxVRes <= MAX_SUPPORTED_HEIGHT)
                                       && (fpsSupported <= MAX_SUPPORTED_FPS))
   {
      uMacroBlocks = MACRO_BLOCKS(uMaxHRes, uMaxVRes, fpsSupported);
   }
   else
   {
      uMacroBlocks = MACRO_BLOCKS(MAX_SUPPORTED_WIDTH, MAX_SUPPORTED_HEIGHT, MAX_ALLOWED_FPS_FOR_MAX_RES);
   }

   /*Update supported_cea_mode*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).supported_cea_mode
      = ceaBitMap = (uint32)getVideoModes(WFD_VIDEO_CEA_MODE, uMacroBlocks, ceaModeCount, uMaxHRes, uMaxVRes );

   /*Update supported_vesa_mode*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).supported_vesa_mode
      = vesaBitMap = getVideoModes(WFD_VIDEO_VESA_MODE, uMacroBlocks, vesaModeCount, uMaxHRes, uMaxVRes );

   /*Update supported_hh_mode*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).supported_hh_mode
      = hhBitMap = (uint32)getVideoModes(WFD_VIDEO_HH_MODE, uMacroBlocks, hhModeCount, uMaxHRes, uMaxVRes );

   nativeModes = vesaBitMap ? 1 : 0;
   nativeModes = hhBitMap ? (1 << 1) : 0 ;

   // Default bit map CEA BIT0, nativeModes = 0;
   if(!nativeModes && !ceaBitMap && !vesaBitMap && !hhBitMap)
   {
      MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR," Defaulting to VGA");
      (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).supported_cea_mode
                                                                                                = H264_MANDATORY_CEA_640x480p60;
      (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_hres = 640;
      (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).max_vres = 480;
      nativeModes = 0;
   }

   /*Update native_bitmap*/
   pReadCfgCapabilities->pCfgCapabilities->video_capability.native_bitmap = nativeModes;

   /*Store transcoding_mode*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).transcoding_mode
      = atoi((getValueForTag(videoBuffer, "TranscodingMode")).c_str()) ? true : false;

   /*Store non_transcoding_mode*/
   (pReadCfgCapabilities->pCfgCapabilities->video_capability.codec[index]).non_transcoding_mode
      = atoi((getValueForTag(videoBuffer, "NonTranscodingMode")).c_str()) ? true : false;
}

/*==============================================================================

         FUNCTION:    parseExtVideoCodecHeader

         DESCRIPTION:
         @brief        Parse XML for VideoCodecHeader
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/
void parseExtVideoCodecHeader(string buffer)
{
    int nTotalProfiles = -1;
    string tagValue;

    nTotalProfiles = atoi((getValueForTag(buffer, "TotalProfiles")).c_str());
    MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_HIGH,
        "parseAVFormatsExtn: Total profiles supported = %d", nTotalProfiles);

    pReadCfgCapabilities->numAVFormatsExtention = nTotalProfiles;

    if(nTotalProfiles <= 0)
    {
        return;
    }
    /* Now that we know the total number of profiles to be supported, initialize
     * DataStructure to hold the video_foramts_extension information
     */
    MM_MSG_PRIO(MM_GENERAL,MM_PRIO_HIGH,"parseAVForamtsExtn: Allocate Resources");
    initVideoFormatExtResources();

    /* Update the total number of codecs in cap. data structure*/
    pReadCfgCapabilities->pCfgCapabilities->video_capability.num_codec =
        nTotalProfiles;

    /* Update preferred display support*/
    tagValue = getValueForTag(buffer,"PreferredDisplaySupport");
    if(tagValue.length())
    {
        pReadCfgCapabilities->pCfgCapabilities->video_capability.preferred_display_mode_supported =
            atoi(tagValue.c_str());
    }
}

/*==============================================================================

         FUNCTION:    parseAVFormatsExtension

         DESCRIPTION:
         @brief        Parse XML for VideoFormats
*//**

@par     DEPENDENCIES:
                           None
*//*
         PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                           None

@par     SIDE EFFECTS:
                           None

*//*==========================================================================*/
void parseAVFormatsExtension(string buffer)
{
    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"parseAVFormatsExtension");

    string tagValue;
    /*Parse VideoCodecHeader*/
    tagValue = getValueForTag(buffer,"VideoCodecHeader");
    if(tagValue.length())
        parseExtVideoCodecHeader(tagValue);
    int nNumCodecs = 0;

    if(pReadCfgCapabilities->numAVFormatsExtention > 0)
    {
        /*Parse for all the codecs and their supported profiles*/
        char aCodecTag[15];
        for(int index = 1; index <= pReadCfgCapabilities->numAVFormatsExtention; index++)
        {
            snprintf(aCodecTag,15,"%s%d","VideoCodec",index);
            MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_LOW,
                "parseAVFormatsExtension: Parsing codec Tag = %s",aCodecTag);

            tagValue = getValueForTag(buffer,aCodecTag);
            if(tagValue.length())
            {
                parseExtendedVideoCodec(tagValue,index-1);
                nNumCodecs++;
            }
        }
        pReadCfgCapabilities->numAVFormatsExtention = nNumCodecs;
        pReadCfgCapabilities->pCfgCapabilities->video_capability.num_codec = nNumCodecs;
    }
}

/*==============================================================================

         FUNCTION:       parseAuxCodec

         DESCRIPTION:
         @brief          Parse Codec for Auxiliary Stream
*//**

@par     DEPENDENCIES:
                         None
*//*
         PARAMETERS:
*//**       @param       buffer - buffer to parser for tag

*//*     RETURN VALUE:
*//**       @return
                         None

@par     SIDE EFFECTS:
                         None

*//*==========================================================================*/
void parseAuxCodec(string buffer)
{
    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"parseAuxCodec");

    if(!initAuxStreamInfo())
    {
        return;
    }

    if(!pReadCfgCapabilities->pAuxCfgCapabilities)
    {
        return;
    }

    string tagValue;
    /*Parse Aux Codec & other parameter*/
    tagValue = getValueForTag(buffer,"Codec");
    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,"wfd_cfg_parser: parseAuxCodec string = %s",tagValue.c_str());
    pReadCfgCapabilities->pAuxCfgCapabilities->name = 0;
    if(tagValue.length())
    {
        int nTotalCodec = atoi((getValueForTag(buffer, "Count")).c_str());
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,"parseAuxCodec codec= %d",nTotalCodec);
        pReadCfgCapabilities->pAuxCfgCapabilities->count = (uint8)nTotalCodec;
        int auxCodecBM = 0;
        for(int i=0;i < nTotalCodec;i++)
        {
            char codecName[15];
            snprintf(codecName,15,"%s%d","Name",i+1);
            string name = getValueForTag(buffer,codecName);
            if(!strncmp(name.c_str(),"PNG",sizeof(name)))
                auxCodecBM |= 1;
            else if(!strncmp(name.c_str(),"JPEG",sizeof(name)))
                auxCodecBM |= 2;
            else if(!strncmp(name.c_str(),"H.264 CBP",sizeof(name)))
                auxCodecBM |= 4;
        }
        pReadCfgCapabilities->pAuxCfgCapabilities->name = auxCodecBM;
    }

    int value = atoi((getValueForTag(buffer, "MaxOverlay")).c_str());
    pReadCfgCapabilities->pAuxCfgCapabilities->maxOverlay = value;


    int hRes = atoi((getValueForTag(buffer, "HRes")).c_str());
    int vRes = atoi((getValueForTag(buffer, "VRes")).c_str());

    switch(mnHW_ID)
    {
      /* Overriding max overlay resolution support for low end target */
      case 246:
      case 294:
      case 295:
          if(hRes > 640 || vRes > 480)
          {
              hRes = 640;
              vRes = 480;
              MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_HIGH,
                "Overlay resolution %d x %d",hRes,vRes);
          }
          break;
      default:
          break;
    }

    string modeName = getValueForTag(buffer,"Mode");
    int mode = 3; // Assigned as Not Valid Mode
    if(!strncmp(modeName.c_str(),"CEA",modeName.length()))
    {
        mode = 0;
    }
    else if(!strncmp(modeName.c_str(),"VESA",modeName.length()))
    {
        mode = 1;
    }
    else if(!strncmp(modeName.c_str(),"HH",modeName.length()))
    {
        mode = 2;
    }
    uint32 modeValue = 0;
    if(mode >= 0 && mode <= 2)
    {
        modeValue = getAuxSupportedBitMap(mode,hRes,vRes);
        mode = ((modeValue << 2)&0xFC)|mode;
    }
    else
    {
        /* As there is no Supported Mode like CEA/VESA/HH
           resetting codec count to inform */
           pReadCfgCapabilities->pAuxCfgCapabilities->count = 0;
    }
    pReadCfgCapabilities->pAuxCfgCapabilities->supportedBitMap = (uint8)mode;

    MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_HIGH,"parseAuxCodec name[0] = %d , maxoverlay = %d , mode = %x",
                                pReadCfgCapabilities->pAuxCfgCapabilities->name,
                                pReadCfgCapabilities->pAuxCfgCapabilities->maxOverlay,
                                pReadCfgCapabilities->pAuxCfgCapabilities->supportedBitMap);
}

/*==============================================================================

         FUNCTION:       parseCfgforWhiteList

         DESCRIPTION:
         @brief          Read and parse white list config file.
*//**

@par     DEPENDENCIES:
                         None
*//*
         PARAMETERS:
*//**       @param       filename    - File from where we need to read.
                         readCfgCaps - Capability structure to hold these values.

*//*     RETURN VALUE:
*//**       @return
                         None

@par     SIDE EFFECTS:
                         None

*//*==========================================================================*/
void parseCfgforWhiteList(const char *filename, readConfigFile *readCfgCaps)
{
    ssize_t bytesRead = 0;
    unsigned long length;
    char *buffer = NULL;
    MM_HANDLE pFileHandle;

    if(readCfgCaps)
    {
       pReadCfgCapabilities = readCfgCaps;
    }
    else
    {
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforWhiteList:readCfgCaps is NULL ");
       return;

    }
    if(!MM_File_Create(filename, MM_FILE_CREATE_R, &pFileHandle))
    {
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_LOW,"parseCfgforWhiteList:open config file successful");
    }
    else
    {
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforWhiteList:open config file failed");
       pFileHandle = NULL;
       return;
    }

    MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_END);
    MM_File_GetSize(pFileHandle, &length);
    MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_BEG);

    buffer = (char *) MM_Malloc(static_cast<int>(sizeof(char)*(length+1)));
    if(!buffer)
    {
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforWhiteList:Memory Allocation Failure");
       MM_File_Release(pFileHandle);
       pFileHandle = NULL;
       return;
    }

    MM_File_Read(pFileHandle, buffer, length, &bytesRead);

    if(length == (unsigned long) bytesRead)
    {
       string tagValue;
       buffer[length] = '\0';
       /*Parse WFD White List Application*/
       tagValue = getValueForTag(buffer, "WhiteListApp");
       if(tagValue.length() > 0)
       {
           parseWhiteListApp(tagValue);
       }
    }
    else
    {
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforWhiteList:File read failed");
    }
    if(MM_File_Release(pFileHandle))
    {
       MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforWhiteList:File release failed");
    }
    pFileHandle = NULL;
    MM_Free(buffer);
    buffer = NULL;
}

/*==============================================================================

         FUNCTION:       parseWhiteListApp

         DESCRIPTION:
         @brief          Parse App name which are allowed.
*//**

@par     DEPENDENCIES:
                         None
*//*
         PARAMETERS:
*//**       @param       buffer    - data which has to be parsed for Application
                                     name.

*//*     RETURN VALUE:
*//**       @return
                         None

@par     SIDE EFFECTS:
                         None

*//*==========================================================================*/
void parseWhiteListApp(string buffer)
{
    string tagValue;
    /*Get the name of Allowed Apps*/
    tagValue = getValueForTag(buffer,"Count");
    int nTotalApp = 0;
    bool err = false;
    if(tagValue.length() > 0)
    {
        nTotalApp = atoi(tagValue.c_str());
    }
    if(nTotalApp > 0)
    {
        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,"parseWhiteListApp Count = %d",nTotalApp);
        if(!initWhiteList(nTotalApp))
        {
            MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseWhiteListApp - Initialization failed.");
            err = true;
        }
        else
        {
            for(int i=0;i < nTotalApp;i++)
            {
                char appName[15];
                snprintf(appName,15,"%s%d","Name",i+1);
                string name = getValueForTag(buffer,appName);
                strlcpy(pReadCfgCapabilities->whiteListApp.name[i],name.c_str(),WHITELIST_APP_NAME_LENGTH);
            }
            pReadCfgCapabilities->whiteListApp.count = nTotalApp;
        }
    }
    else
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"parseWhiteListApp - No Apps");
        err = true;
    }

    if(err)
    {
        pReadCfgCapabilities->whiteListApp.count = 0;
    }
}

/*==============================================================================

         FUNCTION:       initWhiteList

         DESCRIPTION:
         @brief          Initialize data structure to store Application Name.
*//**

@par     DEPENDENCIES:
                         None
*//*
         PARAMETERS:
*//**       @param       count    - Number of data for which allocation has
                                    to be done.

*//*     RETURN VALUE:
*//**       @return
                         None

@par     SIDE EFFECTS:
                         None

*//*==========================================================================*/
bool initWhiteList(int count)
{
    /*Initialize*/
    pReadCfgCapabilities->whiteListApp.name =
                     (char**)MM_Malloc(sizeof(char *) * count);
    if(pReadCfgCapabilities->whiteListApp.name != NULL)
    {
        for(int i = 0; i < count; i++)
        {
            pReadCfgCapabilities->whiteListApp.name[i] =
                    (char*)MM_Malloc((sizeof(char))* WHITELIST_APP_NAME_LENGTH);
            if(pReadCfgCapabilities->whiteListApp.name[i] == NULL)
            {
                MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_ERROR,
                "initWhiteList - Exit failure - Count = %d",i);
                return false;
            }
            else
                memset(pReadCfgCapabilities->whiteListApp.name[i],0,
                                      (sizeof(char))*WHITELIST_APP_NAME_LENGTH);
        }
        return true;
    }
    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,
        "initWhiteList - Exit - failure");
    return false;
}

uint32 getAuxSupportedBitMap(int mode,int width , int height)
{
    uint32 map = 0;
    if(mode == 0) //CEA
    {
        switch(width)
        {
            case 640:
                if(height == 480)
                    map = 0;
                break;
            case 720:
                if(height == 480)
                    map = 1;
                else if(height == 576)
                    map = 3;
                break;
            case 1280:
                if(height == 720)
                    map = 5;
                break;
            case 1920:
                if(height == 1080)
                    map = 7;
                break;
            default:
                map = 0;
                break;
        }
    }
    else if(mode == 1)//VESA
    {
        switch(width)
        {
            case 800:
                if(height == 600)
                    map = 0;
                break;
            case 1024:
                if(height == 768)
                    map = 2;
                break;
            case 1152:
                if(height == 864)
                    map = 4;
                break;
            case 1280:
                if(height == 768)
                    map = 6;
                else if(height == 800)
                    map = 8;
                else if(height == 1024)
                    map = 14;
                break;
            case 1360:
                if(height == 768)
                    map = 10;
                break;
            case 1366:
                if(height == 768)
                    map = 12;
                break;
            case 1400:
                if(height == 1050)
                    map = 16;
                break;
            case 1440:
                if(height == 900)
                    map = 18;
                break;
            case 1600:
                if(height == 900)
                    map = 20;
                else if(height == 1200)
                    map = 22;
                break;
            case 1680:
                if(height == 1024)
                    map = 24;
                else if(height == 1050)
                    map = 26;
                break;
            case 1920:
                if(height == 1200)
                    map = 28;
                break;
            default:
                map = 0;
                break;
        }
    }
    else if(mode == 2) //HH
    {
        switch(height)
        {
            case 360:
                if(width == 640)
                    map = 6;
                break;
            case 480:
                if(width == 800)
                    map = 0;
                else if(width == 848)
                    map = 10;
                else if(width == 854)
                    map = 2;
                else if(width == 864)
                    map = 4;
            case 540:
                if(width == 960)
                    map = 8;
                break;
            default:
                map = 0;
                break;
        }
    }
    return map;
}
/*==============================================================================

      FUNCTION:    parseAudioCodec

      DESCRIPTION:
      @brief        Parse XML for Audio Codec Entries
*//**

@par     DEPENDENCIES:
                        None
*//*
      PARAMETERS:
*//**       @param      - buffer - buffer to parser for tag
                     - tag - the tag to find in the buffer

*//*     RETURN VALUE:
*//**       @return
                        None

@par     SIDE EFFECTS:
                        None

*//*==========================================================================*/

void  parseAudioCodec(string audioBuffer, audioCodecName type)
{
   switch(type)
   {
      case LPCM:
      {
         unsigned long samplingFreq;
         string name = getValueForTag(audioBuffer, "Name");
         bool valid = atoi((getValueForTag(audioBuffer, "Valid")).c_str()) ? true : false;
         int channels = atoi((getValueForTag(audioBuffer, "Channels")).c_str());
         samplingFreq = (unsigned long)atoi((getValueForTag(audioBuffer, "SamplingFreq")).c_str());

         if( valid && ( samplingFreq == LPCM_MODE_FREQ_1 || samplingFreq == LPCM_MODE_FREQ_2 ) && ( channels == AUDIO_MIN_CHANNELS ) )
         {
            if( LPCM_MODE_FREQ_1 == samplingFreq )
            {
               pReadCfgCapabilities->pCfgCapabilities->audio_config.lpcm_codec.supported_modes_bitmap = LPCM_441_16_2 ;
               MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"Currently this LPCM Freq is not supported");
            }
            else if(LPCM_MODE_FREQ_2 == samplingFreq )
            {
               pReadCfgCapabilities->pCfgCapabilities->audio_config.lpcm_codec.supported_modes_bitmap = LPCM_48_16_2 ;// BIT1 only ,44.1 is not supoprted right now
            }
            pReadCfgCapabilities->pCfgCapabilities->audio_config.lpcm_codec.decoder_latency = (uint8)atoi((getValueForTag(audioBuffer, "Latency")).c_str());
         }
         else
         {
            // Debug Msg - Invalid data entered
            MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"Invalid data entered for LPCM");
         }
      }
      break;
      case AAC:
      {
         unsigned long samplingFreq;
         string name = getValueForTag(audioBuffer, "Name");
         bool valid = atoi((getValueForTag(audioBuffer, "Valid")).c_str()) ? true : false;
         int channels = atoi((getValueForTag(audioBuffer, "Channels")).c_str());
         samplingFreq = (unsigned long)atoi((getValueForTag(audioBuffer, "SamplingFreq")).c_str());

         if( valid && ( samplingFreq == AAC_AC3_MODE_FREQ ) && ( ( channels >= AUDIO_MIN_CHANNELS )
                         && ( channels <= AUDIO_MAX_CHANNELS )
                         && ( ( channels%2 ) == 0 ) ) )
         {
            switch(channels)
            {
             case AUDIO_MIN_CHANNELS:
             case AUDIO_4_CHANNELS:
                  pReadCfgCapabilities->pCfgCapabilities->audio_config.aac_codec.supported_modes_bitmap = AAC_48_16_2;
             break;
             case AUDIO_6_CHANNELS:
                  pReadCfgCapabilities->pCfgCapabilities->audio_config.aac_codec.supported_modes_bitmap = (AAC_48_16_2 | AAC_48_16_6);
             break;
             case AUDIO_MAX_CHANNELS:
                  pReadCfgCapabilities->pCfgCapabilities->audio_config.aac_codec.supported_modes_bitmap = (AAC_48_16_2 | AAC_48_16_6 | AAC_48_16_8);
             break;
            }
            pReadCfgCapabilities->pCfgCapabilities->audio_config.aac_codec.decoder_latency =
            (uint8)atoi((getValueForTag(audioBuffer, "Latency")).c_str());
         }
         else
         {
            // Invalid AAC Values
            MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"Invalid data entered for AAC");
         }
      }
      break;
      case AC3:
      {
         unsigned long samplingFreq;
         string name = getValueForTag(audioBuffer, "Name");
         bool valid = atoi((getValueForTag(audioBuffer, "Valid")).c_str()) ? true : false;
         int channels = atoi((getValueForTag(audioBuffer, "Channels")).c_str());
         samplingFreq = (unsigned long)atoi((getValueForTag(audioBuffer, "SamplingFreq")).c_str());

         if( valid && ( samplingFreq == AAC_AC3_MODE_FREQ ) && ( ( channels >= AUDIO_MIN_CHANNELS )
         && ( channels <= AUDIO_MAX_CHANNELS )
         && ( ( channels%2 ) == 0 ) ) )
         {
            switch(channels)
            {
               case AUDIO_MIN_CHANNELS:
                  pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.supported_modes_bitmap = DOLBY_DIGITAL_48_16_2_AC3;
               break;
               case AUDIO_4_CHANNELS:
                  pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.supported_modes_bitmap = (DOLBY_DIGITAL_48_16_2_AC3 | DOLBY_DIGITAL_48_16_4_AC3);
               break;
               case AUDIO_6_CHANNELS:
                  pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.supported_modes_bitmap = (DOLBY_DIGITAL_48_16_2_AC3 | DOLBY_DIGITAL_48_16_4_AC3 | DOLBY_DIGITAL_48_16_6_AC3);
               break;
               case AUDIO_8_CHANNELS:
                  pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.supported_modes_bitmap = (DOLBY_DIGITAL_48_16_2_AC3 | DOLBY_DIGITAL_48_16_4_AC3 | DOLBY_DIGITAL_48_16_6_AC3 | DOLBY_DIGITAL_48_16_8_EAC3);
               break;
            }
            pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.decoder_latency =
            (uint8)atoi((getValueForTag(audioBuffer, "Latency")).c_str());
         }
         else
         {
            // Invalid AC3 Values
            MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"Invalid data entered for AC3");
         }
      }
      break;
      default:
         MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"Invalid Audio type");
      break;
   }
   if(pReadCfgCapabilities->pCfgCapabilities->audio_config.lpcm_codec.supported_modes_bitmap ||
      pReadCfgCapabilities->pCfgCapabilities->audio_config.aac_codec.supported_modes_bitmap ||
      pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.supported_modes_bitmap)
   {
      // AC3 is of priority check if it is set and assign the audio method to it
      if(pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.supported_modes_bitmap)
         pReadCfgCapabilities->pCfgCapabilities->audio_method = WFD_AUDIO_DOLBY_DIGITAL;
      // AAC is 2nd priority
      else if(pReadCfgCapabilities->pCfgCapabilities->audio_config.aac_codec.supported_modes_bitmap)
         pReadCfgCapabilities->pCfgCapabilities->audio_method = WFD_AUDIO_AAC;
      // LPCM last option
      else
         pReadCfgCapabilities->pCfgCapabilities->audio_method = WFD_AUDIO_LPCM;
   }
   else
   {
      //Default LPCM BIT0
      pReadCfgCapabilities->pCfgCapabilities->audio_method = WFD_AUDIO_LPCM;
      pReadCfgCapabilities->pCfgCapabilities->audio_config.lpcm_codec.supported_modes_bitmap = LPCM_48_16_2;
   }
}

/*==============================================================================

   FUNCTION:    parseHWID

   DESCRIPTION:
   @brief        Extracts target ID information
                 to properly update properties.

   DEPENDENCIES: None

   PARAMETERS:
       @param -  None

   RETURN VALUE:
       @return - None

   SIDE EFFECTS: None

*//*==========================================================================*/

void parseHWID()
{
    mnHW_ID = -1;
    int result = -1;
    char buffer[10];
    FILE *device = NULL;
    device = fopen("/sys/devices/soc0/soc_id", "r");
    if(device)
    {
      /* 4 = 3 (MAX_SOC_ID_LENGTH) + 1 */
      result = fread(buffer, 1, 4, device);
      fclose(device);
    }
    else
    {
      device = fopen("/sys/devices/system/soc/soc0/id", "r");
      if(device)
      {
         result = fread(buffer, 1, 4, device);
         fclose(device);
      }
    }
    if(result > 0)
    {
       mnHW_ID = atoi(buffer);
    }
    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,"Got HW_ID = %d",mnHW_ID);
}

/*==============================================================================

   FUNCTION:    parseHWVer

   DESCRIPTION:
   @brief        Extracts version of the HW

   DEPENDENCIES: Should be called only after parseHWID()

   PARAMETERS:
       @param -  None

   RETURN VALUE:
       @return - None

   SIDE EFFECTS: None

*//*==========================================================================*/

void parseHWVer()
{
    mnHW_Ver = -1;
    char value[PROPERTY_VALUE_MAX] = {0};
    switch(mnHW_ID)
    {
        case 336:
        case 337:
            property_get("media.sdm670.version", value, "0");
            if(!(strncmp(value, "1", PROPERTY_VALUE_MAX)))
            {
                mnHW_Ver = 1; //Lower Spec
            }
            else if(!(strncmp(value, "0", PROPERTY_VALUE_MAX)))
            {
                mnHW_Ver = 0; //Higher Spec
            }
            break;
        default:
            break;
    }
    MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,"Got HW_Ver = %d",mnHW_Ver);
}

/*==============================================================================

   FUNCTION:    parseXml

   DESCRIPTION:
   @brief        Top level XML Configuration parsing
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
*//**       @param - cfgBuffer - xml data

*//*     RETURN VALUE:
*//**       @return
                     None

@par     SIDE EFFECTS:
                     None

*//*==========================================================================*/
void  parseXml(string cfgBuffer)
{
   cfgItems.clear();
   cfgItems.assign(TOTAL_CFG_KEYS,-1);
   cfgItemsStr.clear();
   string tagValue;
   parseHWID();
   parseHWVer();

   tagValue = getValueForTag(cfgBuffer, "AudioLPCM");
   if (tagValue.length())
      parseAudioCodec(tagValue, LPCM);

   tagValue = getValueForTag(cfgBuffer, "AudioAAC");
   if (tagValue.length())
      parseAudioCodec(tagValue, AAC);

   tagValue = getValueForTag(cfgBuffer, "AudioAC3");
   if (tagValue.length())
      parseAudioCodec(tagValue, AC3);

#if 0
   /*Parse WFD Mirroring codec information*/
   tagValue = getValueForTag(cfgBuffer, "WFDSessionCodec");
   MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"searching WFDSessionCodec");
   if(tagValue.length())
   {
      MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"parseWFDSessionCodec");
      parseWFDSessionCodec(tagValue);
   }
#endif

   /*Parse WFD AV Formats Extension*/
   tagValue = getValueForTag(cfgBuffer, "VideoFormats");
   if(tagValue.length())
   {
      parseAVFormatsExtension(tagValue);
   }
   else
   {
      /*wfd_video_formats_extension is not supported or this is source*/
      MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"No support for VideoFormats");
      pReadCfgCapabilities->pCfgCapabilities->video_capability.num_codec = 0;
      pReadCfgCapabilities->numAVFormatsExtention = 0;
   }

   /*Parse WFD Auxiliary Codec*/
   tagValue = getValueForTag(cfgBuffer, "AuxStreamInfo");
   if(tagValue.length())
   {
      parseAuxCodec(tagValue);
   }
   else
   {
      /*wfd2_aux_stream_formats is not supported or this is source*/
      MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"No support for Auxillary Stream");
      pReadCfgCapabilities->pAuxCfgCapabilities = NULL;
   }

   tagValue = getValueForTag(cfgBuffer, "StandbyResumeCapability");
   if (tagValue.length())
      parseStandby(tagValue);

   tagValue = getValueForTag(cfgBuffer, "IDRRequest");
   if (tagValue.length())
      parseidrRequest(tagValue);

   tagValue = getValueForTag(cfgBuffer, "FrameSkipping");
   if (tagValue.length())
      parseFrameSkipping(tagValue);

   tagValue = getValueForTag(cfgBuffer, "ContentProtection");
   if (tagValue.length())
      parseHDCPSupport(tagValue);

   tagValue = getValueForTag(cfgBuffer, "RTPPortType");
   if (tagValue.length())
      parseRTPPortType(tagValue);

   tagValue = getValueForTag(cfgBuffer, "RTPDumpEnable");
   if (tagValue.length())
      parseRTPDumpEnable(tagValue);

   tagValue = getValueForTag(cfgBuffer,"UIBC");
   if (tagValue.length())
      parseUIBCSupport(tagValue);

   tagValue = getValueForTag(cfgBuffer,"PeriodicIDRSettings");
   if(tagValue.length())
   {
       parseRIRSupport(tagValue);
   }

//Parse for configurable items here itself in order to cache for later use

   tagValue = getValueForTag(cfgBuffer,DISABLE_AVSYNC_MODE_KEY);
   if(tagValue.length())
   {
     updateVar(DISABLE_AVSYNC_MODE_KEY,DISABLE_AVSYNC_MODE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,ENABLE_AUDIO_TRACK_LATENCY_MODE_KEY);
   if(tagValue.length())
   {
     updateVar(ENABLE_AUDIO_TRACK_LATENCY_MODE_KEY,ENABLE_AUDIO_TRACK_LATENCY_MODE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,AUDIO_AVSYNC_DROP_WINDOW_KEY);
   if(tagValue.length())
   {
     updateVar(AUDIO_AVSYNC_DROP_WINDOW_KEY,AUDIO_AVSYNC_DROP_WINDOW,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,VIDEO_AVSYNC_DROP_WINDOW_KEY);
   if(tagValue.length())
   {
     updateVar(VIDEO_AVSYNC_DROP_WINDOW_KEY,VIDEO_AVSYNC_DROP_WINDOW,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,AUDIO_AV_SYNC_DEL_KEY);
   if(tagValue.length())
   {
     updateVar(AUDIO_AV_SYNC_DEL_KEY,AUDIO_AV_SYNC_DEL,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,AUDIO_IN_SUSPEND_KEY);
   if(tagValue.length())
   {
     updateVar(AUDIO_IN_SUSPEND_KEY,AUDIO_IN_SUSPEND,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,CYCLIC_IR_KEY);
   if(tagValue.length())
   {
     updateVar(CYCLIC_IR_KEY,CYCLIC_IR,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,CYCLIC_IR_NUM_MACRO_BLK_KEY);
   if(tagValue.length())
   {
     updateVar(CYCLIC_IR_NUM_MACRO_BLK_KEY,CYCLIC_IR_NUM_MACRO_BLK,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,DISABLE_NALU_FILLER_KEY);
   if(tagValue.length())
   {
     updateVar(DISABLE_NALU_FILLER_KEY,DISABLE_NALU_FILLER,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,DYN_BIT_ADAP_KEY);
   if(tagValue.length())
   {
     updateVar(DYN_BIT_ADAP_KEY,DYN_BIT_ADAP,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,ENCRYPT_AUDIO_DECISION_KEY);
   if(tagValue.length())
   {
     updateVar(ENCRYPT_AUDIO_DECISION_KEY,ENCRYPT_AUDIO_DECISION,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,ENCRYPT_NON_SECURE_KEY);
   if(tagValue.length())
   {
     updateVar(ENCRYPT_NON_SECURE_KEY,ENCRYPT_NON_SECURE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,HDCP_ENFORCED_KEY);
   if(tagValue.length())
   {
     updateVar(HDCP_ENFORCED_KEY,HDCP_ENFORCED,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,VIDEO_PKTLOSS_FRAME_DROP_MODE_KEY);
   if(tagValue.length())
   {
     updateVar(VIDEO_PKTLOSS_FRAME_DROP_MODE_KEY,VIDEO_PKTLOSS_FRAME_DROP_MODE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,MAX_FPS_SUPPORTED_KEY);
   if(tagValue.length())
   {
     if(((mnHW_ID == 206)|| (mnHW_ID>=239 && mnHW_ID <= 243)))
     {
        /* For Lower End Target */
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"Setting MAX_FPS_SUPPORTED to 30");
        tagValue = string("30");
     }
     updateVar(MAX_FPS_SUPPORTED_KEY,MAX_FPS_SUPPORTED,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,PERF_LEVEL_PERF_MODE_KEY);
   if(tagValue.length())
   {
     updateVar(PERF_LEVEL_PERF_MODE_KEY,PERF_LEVEL_PERF_MODE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,MIN_QP_VALUE_KEY);
   if(tagValue.length())
   {
     updateVar(MIN_QP_VALUE_KEY,MIN_QP_VALUE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,MAX_QP_VALUE_KEY);
   if(tagValue.length())
   {
     updateVar(MAX_QP_VALUE_KEY,MAX_QP_VALUE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,PERF_LEVEL_TURBO_MODE_KEY);
   if(tagValue.length())
   {
     updateVar(PERF_LEVEL_TURBO_MODE_KEY,PERF_LEVEL_TURBO_MODE,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,BLANK_FRAME_SUPPORT_KEY);
   if(tagValue.length())
   {
     updateVar(BLANK_FRAME_SUPPORT_KEY,BLANK_FRAME_SUPPORT,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer,STREAMING_FEATURE_SUPPORT_KEY);
   if(tagValue.length())
    {
      updateVar(STREAMING_FEATURE_SUPPORT_KEY,STREAMING_FEATURE_SUPPORT,tagValue);
    }

   tagValue = getValueForTag(cfgBuffer,DS_SCREEN_TIMEOUT_KEY);
   if(tagValue.length())
   {
     updateVar(DS_SCREEN_TIMEOUT_KEY,DS_SCREEN_TIMEOUT,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer, DS_REQUIRES_HDCP_KEY);
   if(tagValue.length())
   {
     updateVar(DS_REQUIRES_HDCP_KEY,DS_REQUIRES_HDCP,tagValue);
   }

   tagValue = getValueForTag(cfgBuffer, RTCP_KEY);

   if(tagValue.length())
   {
      {
         string str = getValueForTag(tagValue, RTCP_RR_NOTIFICATION_ENABLED_KEY);

         if(str.length() > 0)
         {
            MM_MSG_SPRINTF_PRIO_1(MM_GENERAL, MM_PRIO_HIGH,
                "RTCP enabled value '%s' (source config)", str.c_str());
            updateVar(RTCP_RR_NOTIFICATION_ENABLED_KEY, RTCP_RR_NOTIFICATION_ENABLED, str);
         }
      }

      {
         string str = getValueForTag(tagValue, RTCP_RR_ENABLED_KEY);

         if(str.length() > 0)
         {
            MM_MSG_SPRINTF_PRIO_1(MM_GENERAL, MM_PRIO_HIGH,
                "RTCP enabled value '%s' (sink config)", str.c_str());
            updateVar(RTCP_RR_ENABLED_KEY, RTCP_RR_ENABLED, str);
         }
      }

      {
         string str = getValueForTag(tagValue, RTCP_RR_INTERVAL_MS_KEY);

         if(str.length() > 0)
         {
            MM_MSG_SPRINTF_PRIO_1(MM_GENERAL, MM_PRIO_HIGH,
                "RTCP interval '%s' ms (sink config)", str.c_str());
            updateVar(RTCP_RR_INTERVAL_MS_KEY, RTCP_RR_INTERVAL_MS, str);
         }
      }
   }

   tagValue = getValueForTag(cfgBuffer,CURSOR_ENABLE_KEY);
   if(tagValue.length())
   {
     updateVar(CURSOR_ENABLE_KEY,CURSOR_ENABLE,tagValue);
   }

}

/*==============================================================================

   FUNCTION:    parseCfg

   DESCRIPTION:
   @brief      Parse XML File presence, size and call the actual parsing routine
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
*//**       @param  - filename - xfg file name
                    - readCfgCaps - capabilities structure

*//*     RETURN VALUE:
*//**       @return
                     None

@par     SIDE EFFECTS:
                     None

*//*==========================================================================*/
void  parseCfg(const char *filename, readConfigFile *readCfgCaps)
{
   char *buffer;
   ssize_t bytesRead = 0;
   unsigned long length;
   MM_HANDLE pFileHandle;

   if(readCfgCaps)
   {
      pReadCfgCapabilities = readCfgCaps;
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parserCfg:readCfgCaps is NULL ");
      return;
   }

   if(!filename)
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"filename  is NULL");
      return;
   }

   if(!MM_File_Create(filename, MM_FILE_CREATE_R, &pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"parserCfg:open config file successful");
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parserCfg:open config file failed");
      pFileHandle = NULL;
      return;
   }

   if(filename && strstr(filename,"sink"))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH,"parserCfg: parsing sink config file");
      wfdDevice = PRIMARY_SINK;
   }

   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_END);
   MM_File_GetSize(pFileHandle, &length);
   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_BEG);

   buffer = (char *) MM_Malloc(static_cast<int>(sizeof(char)*(length+1)));
   if(!buffer)
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parserCfg:Memory Allocation Failure");
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      return;
   }

   MM_File_Read(pFileHandle, buffer, length, &bytesRead);

   if(length == (unsigned long) bytesRead)
   {
      buffer[length] = '\0';
      string xmlbuffer(buffer,length);
      parseXml(xmlbuffer);
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parserCfg:File read failed");
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      MM_Free(buffer);
      buffer = NULL;
      return;
   }

#if( VDEC_LOG_HIGH )
   dumpcapabilitiesRead();
#endif

   if(MM_File_Release(pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parserCfg:File release failed");
   }

   MM_Free(buffer);
   return;
}

/*==============================================================================

   FUNCTION:    ParseCfgForBitrate

   DESCRIPTION:
   @brief      Parse min and max bitrates from cfg file mentioned.
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
*//**       @param  - mode - CEA VESA or HH
                    - bit  - bit which is set for the mode
                    - filename - cfg file name
                    - minBitrate - minimum Bitrate
                    - maxBitrate - maximum Bitrate


*//*     RETURN VALUE:
*//**       @return
                     None

@par     SIDE EFFECTS:
                     None

*//*==========================================================================*/
void ParseCfgForBitrate(int mode, int bit, char *pFilename, int *minBitrate, int *maxBitrate)
{
    #define SEARCHKEYLEN 6

    if(NULL == minBitrate || NULL == maxBitrate || bit > 31 || !pFilename)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "ParseBitrateTable: Invalid args");
        return;
    }
    *minBitrate = 0;
    *maxBitrate = 0;
    if(mode != WFD_MM_VESA_MODE && mode != WFD_MM_CEA_MODE
       && mode != WFD_MM_HH_MODE && mode != WFD_MM_AAC_MODE
       && mode != WFD_MM_DOLBY_DIGITAL_MODE)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "ParseBitrateTable: Invalid mode");
        return;
    }


    MM_HANDLE hFile = NULL;
    unsigned long nSize = 0;
    char  *pFileBuf = NULL;
    char aModeString[SEARCHKEYLEN];
    char aBitString[SEARCHKEYLEN];
    snprintf(aBitString, SEARCHKEYLEN, "%s%d","BIT",bit);

    if(MM_File_Create(pFilename,MM_FILE_CREATE_R, &hFile) != 0)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "ParseBitrateTable: Unable to open file");
        hFile = NULL;
        return;
    }

    MM_File_GetSize(hFile, &nSize);

    if(!nSize || nSize > 128*1024)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "ParseBitrateTable: File of 0 size or too large");
        MM_File_Release(hFile);
        hFile = NULL;
        return;
    }

    pFileBuf = (char*)MM_Malloc(static_cast<int>(nSize + 4));

    if(!pFileBuf)
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, "ParseBitrateTable: Malloc fail");
        MM_File_Release(hFile);
        hFile = NULL;
        return;
    }

    ssize_t bytesRead = 0;
    MM_File_Read(hFile, pFileBuf, nSize, &bytesRead);

    pFileBuf[nSize] = 0;

    if(mode == WFD_MM_CEA_MODE)
    {
        strlcpy(aModeString, "CEA", SEARCHKEYLEN);
    }
    else if(mode == WFD_MM_VESA_MODE)
    {
       strlcpy(aModeString, "VESA", SEARCHKEYLEN);
    }
    else if(mode == WFD_MM_HH_MODE)
    {
       strlcpy(aModeString, "HH", SEARCHKEYLEN);
    }
    else if(mode == WFD_MM_AAC_MODE)
    {
       strlcpy(aModeString, "AAC", SEARCHKEYLEN);
    }
    else if(mode == WFD_MM_DOLBY_DIGITAL_MODE)
    {
       strlcpy(aModeString, "AC3", SEARCHKEYLEN);
    }
    string sMode = getValueForTag(pFileBuf, aModeString);


    if(sMode.length())
    {
        string sBitrates = getValueForTag(sMode, aBitString);

        if(sBitrates.length())
        {
            *minBitrate = atoi((getValueForTag(sBitrates, "MinBitrate")).c_str());
            *maxBitrate = atoi((getValueForTag(sBitrates, "MaxBitrate")).c_str());
        }
    }

    MM_File_Release(hFile);
    hFile = NULL;
    if(pFileBuf != NULL)
    {
      MM_Free(pFileBuf);
    }
    return;

}

/*==============================================================================

   FUNCTION:    PargeCfgForIntValueForKey

   DESCRIPTION:
   @brief      Parse XML File for value of a given key
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
*//**       @param  - filename - cfg file name
                    - pKey - capabilities structure
                    - pVal

*//*     RETURN VALUE:
*//**       @return
                     None

@par     SIDE EFFECTS:
                     None

*//*==========================================================================*/
int PargeCfgForIntValueForKey(char *filename, char *pKey, int *pVal)
{
   ssize_t bytesRead = 0;
   unsigned long length;
   char *buffer;
   MM_HANDLE pFileHandle;

   if(!filename || !pKey || !pVal)
   {
      return -1;
   }

   if(!MM_File_Create(filename, MM_FILE_CREATE_R, &pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_LOW,"PargeCfgForIntValueForKey:open config file successful");
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"PargeCfgForIntValueForKey:open config file failed");
      pFileHandle = NULL;
      return -1;
   }

   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_END);
   MM_File_GetSize(pFileHandle, &length);
   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_BEG);

   buffer = (char *) MM_Malloc(static_cast<int>(sizeof(char)*(length+1)));
   if(!buffer)
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"PargeCfgForIntValueForKey:Memory Allocation Failure");
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      return -1;
   }

   MM_File_Read(pFileHandle, buffer, length, &bytesRead);

   if(length == (unsigned long) bytesRead)
   {
      string tagValue;
      buffer[length] = '\0';
      string xmlbuffer(buffer,length);
      tagValue = getValueForTag(buffer, pKey);

      if(!tagValue.length())
      {
         MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"PargeCfgForIntValueForKey:Key not present");
         MM_Free(buffer);
         buffer = NULL;
         MM_File_Release(pFileHandle);
         pFileHandle = NULL;
         return -1;
      }

      *pVal =  atoi(tagValue.c_str());
      MM_Free(buffer);
      buffer = NULL;
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      return 0;
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"PargeCfgForIntValueForKey:File read failed");
      MM_Free(buffer);
      buffer = NULL;
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      return -1;
   }
   if(!MM_File_Release(pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"PargeCfgForIntValueForKey:File release failed");
   }
   MM_Free(buffer);
   buffer = NULL;
   return -1;

}

/*=========================================================

   FUNCTION:    updateVar

   DESCRIPTION:
   @brief     Helper method to update variables holding config values

*//**
@par     DEPENDENCIES: None

*//*
   PARAMETERS:
*//**
               @param[in] key - a key for adding to cfgItemsStr map

               @param[in] cfgKey - index of config item in cfgItems vector
                        should be as per cfgKeys enum in wfd_cfg_parser.h

               @param[in] keyValue - string buffer containing value for the
               config item which has a value of key

*//*     RETURN VALUE:
*//**       @return none

*//*=====================================================*/

void updateVar(const char* key, const cfgKeys cfgKey, const string keyValue)
{
  if(keyValue.length() && key)
  {
    pair<map<string,pair_val>::iterator,bool> ret;
    cfgItems.at(cfgKey)= atoi(keyValue.c_str());
    ret = cfgItemsStr.insert(pair<string,pair_val>(make_pair(string(key),make_pair(cfgKey,cfgItems.at(cfgKey)))));
    if(ret.second == false)
    {
        MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"updateVar:: Element %s already exists with %d",(ret.first->first).c_str(),ret.first->second.second);
    }
    else
    {
        MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"updateVar:: Element %s newly inserted with %d",(ret.first->first).c_str(),ret.first->second.second);
    }
  }
  else
  {
    MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"updateVar:: Invalid parameters");
  }
}

/*=========================================================

   FUNCTION:    getCfgItem

   DESCRIPTION:
   @brief     Used to get the value of a configurable
                item from config file

*//**
@par     DEPENDENCIES: None

*//*
   PARAMETERS:
*//**       @param[in] key - a string key for querying
            @param[out] pVal returns the value of the config
                                  item in this variable

*//*     RETURN VALUE:
*//**       @return returns 0 on success else -1


*//*=====================================================*/

int getCfgItem(const char* key,int *pVal)
{
    if(key && pVal)
    {
        if(cfgItems.size() && cfgItemsStr.size())
        {
            if(cfgItemsStr.count(string(key))> 0 )
            {//Key is present in map
                *pVal = cfgItemsStr[(string(key))].second;
                MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"getCfgItem:: Value of %s is %d",key,*pVal);
                return 0;
            }
            else
            {
                MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_ERROR,"getCfgItem:: %s not in config file",key);
            }
        }
        else
        {
            MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"getCfgItem:: Failed to get Config item!");
        }
    }
    else
    {
        MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"getCfgItem:: Invalid param to getConfigItem!!!");
    }
    return -1;
}

/*=========================================================

   FUNCTION:    resetCfgItems

   DESCRIPTION:
   @brief     Used to reset the values of configurable
                 item of config file

*//**
@par     DEPENDENCIES: None

*//*
   PARAMETERS:
*//**       @param none

*//*     RETURN VALUE:
*//**       @return none


*//*=====================================================*/

void resetCfgItems()
{
    cfgItems.clear();
    cfgItemsStr.clear();
    wfdDevice = SOURCE;
    MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_LOW,"Resetting configurable items cfgItems to %d and cfgItemsStr to %d",cfgItems.size(),cfgItemsStr.size());
}


/*=========================================================

   FUNCTION:    getCfgItems

   DESCRIPTION:
   @brief     Used to reset the values of configurable
                 item of config file

*//**
@par     DEPENDENCIES: None

*//*
   PARAMETERS:
*//**       @param none

*//*     RETURN VALUE:
*//**       @return none


*//*=====================================================*/

int getCfgItems(int** configItems, size_t* len)
{
    if(!len || !configItems)
    {
        MM_MSG_PRIO1(MM_GENERAL,MM_PRIO_ERROR,"Invalid args to %s!",
        __FUNCTION__);
        return -1;
    }
    *len = cfgItems.size();
    *configItems = new int[*len];//Caller to take care of deletion
    if(!(*configItems))
    {
        MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"configItems memory allocation failed");
        return -1;
    }
    for(unsigned int i=0;i< *len;i++) {
        (*configItems)[i] = cfgItems.at(i);
        MM_MSG_PRIO2(MM_GENERAL,MM_PRIO_HIGH,"configItems[%d] = %d",i,
            (*configItems)[i]);
    }
    return 0;

}

/*==============================================================================

   FUNCTION:    parseCfgforUIBC

   DESCRIPTION:
   @brief      Parse XML File presence, size and call the actual parsing routine
               for UIBC capabilities from XML
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
*//**       @param  - filename - xfg file name
                    - readCfgCaps - capabilities structure

*//*     RETURN VALUE:
*//**       @return
                     None

@par     SIDE EFFECTS:
                     None

*//*==========================================================================*/
void  parseCfgforUIBC(const char *filename, readConfigFile *readCfgCaps)
{
   ssize_t bytesRead = 0;
   unsigned long length;
   char *buffer;
   MM_HANDLE pFileHandle;

   if(readCfgCaps)
   {
      pReadCfgCapabilities = readCfgCaps;
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforUIBC:readCfgCaps is NULL ");
      return;

   }
   if(!MM_File_Create(filename, MM_FILE_CREATE_R, &pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_LOW,"parseCfgforUIBC:open config file successful");
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforUIBC:open config file failed");
      pFileHandle = NULL;
      return;
   }

   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_END);
   MM_File_GetSize(pFileHandle, &length);
   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_BEG);

   buffer = (char *) MM_Malloc(static_cast<int>(sizeof(char)*(length+1)));
   if(!buffer)
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforUIBC:Memory Allocation Failure");
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      return;
   }

   MM_File_Read(pFileHandle, buffer, length, &bytesRead);

   if(length == (unsigned long) bytesRead)
   {
      string tagValue;
      buffer[length] = '\0';
      string xmlbuffer(buffer,length);
      tagValue = getValueForTag(buffer, "UIBC");
      parseUIBCSupport(tagValue);
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforUIBC:File read failed");
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      MM_Free(buffer);
      buffer = NULL;
      return;
   }
   if(MM_File_Release(pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforUIBC:File release failed");
   }
   MM_Free(buffer);
   return;
}

/*==============================================================================

   FUNCTION:    parseCfgforExtCodec

   DESCRIPTION:
   @brief      Parse XML File presence, size and call the actual parsing routine
               for Extended Codecs from XML
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
*//**       @param  - filename - xfg file name
                    - readCfgCaps - capabilities structure

*//*     RETURN VALUE:
*//**       @return
                     None

@par     SIDE EFFECTS:
                     None

*//*==========================================================================*/
void  parseCfgforExtCodec(const char *filename, readConfigFile *readCfgCaps)
{
   ssize_t bytesRead = 0;
   unsigned long length;
   char *buffer;
   MM_HANDLE pFileHandle;

   if(readCfgCaps)
   {
      pReadCfgCapabilities = readCfgCaps;
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforExtCodec:readCfgCaps is NULL ");
      return;

   }
   if(!MM_File_Create(filename, MM_FILE_CREATE_R, &pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_LOW,"parseCfgforExtCodec:open config file successful");
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforExtCodec:open config file failed");
      pFileHandle = NULL;
      return;
   }

   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_END);
   MM_File_GetSize(pFileHandle, &length);
   MM_File_Seek(pFileHandle, 0, MM_FILE_SEEK_BEG);

   buffer = (char *) MM_Malloc(static_cast<int>(sizeof(char)*(length+1)));
   if(!buffer)
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforExtCodec:Memory Allocation Failure");
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      return;
   }

   MM_File_Read(pFileHandle, buffer, length, &bytesRead);

   if(length == (unsigned long) bytesRead)
   {
      buffer[length] = '\0';
      string tagValue;
      string xmlbuffer(buffer);

      /*Parse WFD AV Formats Extension*/
      tagValue = getValueForTag(buffer, "VideoFormats");
      if(tagValue.length())
      {
         parseAVFormatsExtension(tagValue);
      }
      else
      {
         /*wfd_video_formats_extension is not supported or this is source*/
         MM_MSG_PRIO(MM_GENERAL,MM_PRIO_ERROR,"No support for VideoFormats");
         pReadCfgCapabilities->pCfgCapabilities->video_capability.num_codec = 0;
         pReadCfgCapabilities->numAVFormatsExtention = 0;
      }
   }
   else
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforExtCodec:File read failed");
      MM_File_Release(pFileHandle);
      pFileHandle = NULL;
      MM_Free(buffer);
      buffer = NULL;
      return;
   }
   if(MM_File_Release(pFileHandle))
   {
      MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR,"parseCfgforExtCodec:File release failed");
   }
   MM_Free(buffer);
   return;
}

/*==============================================================================

   FUNCTION:    dumpcapabilitiesRead

   DESCRIPTION:
   @brief      Dump the capabilities read from the xml cfg file
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
                     None
*//*     RETURN VALUE:
*//**       @return
                     None

@par     SIDE EFFECTS:
                     None

*//*==========================================================================*/

void  dumpcapabilitiesRead()
{

   MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "************************************************");
   MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, " Dumping Capabilities Read from XML Config File");
   MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, "************************************************");
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Audio Method  = %d",  pReadCfgCapabilities->pCfgCapabilities->audio_method);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "   AAC Bitmap = %u",  pReadCfgCapabilities->pCfgCapabilities->audio_config.aac_codec.supported_modes_bitmap);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "   AC3 Bitmap = %d",  pReadCfgCapabilities->pCfgCapabilities->audio_config.dolby_digital_codec.supported_modes_bitmap);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "  LPCM Bitmap = %d",  pReadCfgCapabilities->pCfgCapabilities->audio_config.lpcm_codec.supported_modes_bitmap);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, " Num H264 Profiles = %d", pReadCfgCapabilities->pCfgCapabilities->video_capability.num_codec);

   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, " Stand By Resume = %d",  pReadCfgCapabilities->pCfgCapabilities->standby_resume_support);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, " Preferred Display Mode Supported = %d", pReadCfgCapabilities->pCfgCapabilities->video_capability.preferred_display_mode_supported);

   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH,"Dumping Extended Video Codec information: total %d", pReadCfgCapabilities->numAVFormatsExtention);
   pReadCfgCapabilities->pCfgCapabilities->video_capability.dump(true);

   if(pReadCfgCapabilities->pAuxCfgCapabilities)
   {
      MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Aux Stream Codec = %d",
                                pReadCfgCapabilities->pAuxCfgCapabilities->name);
      MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Aux Stream MaxOverlay = %d",
                                pReadCfgCapabilities->pAuxCfgCapabilities->maxOverlay);
      MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Aux Stream supportedBM = %d",
                                pReadCfgCapabilities->pAuxCfgCapabilities->supportedBitMap);
   }

   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "IDR Request Valid = %d",pReadCfgCapabilities->idrRequestCapability.idrReqValid);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "IDR Request Intvl = %d",pReadCfgCapabilities->idrRequestCapability.idrIntvl);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Frame Skip Valid  = %d",pReadCfgCapabilities->frameSkippingCapability.frameSkipValid);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Frame Skip Intvl  = %d",pReadCfgCapabilities->frameSkippingCapability.frameSkipIntvl);

   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Content Protection Version = %d",pReadCfgCapabilities->pCfgCapabilities->content_protection_config.content_protection_capability);
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "Content Protection Port = %d",pReadCfgCapabilities->pCfgCapabilities->content_protection_config.content_protection_ake_port);

   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "RTP Port Type (0-UDP,1-TCP) = %d",pReadCfgCapabilities->pCfgCapabilities->transport_capability_config.eRtpPortType);

   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, "RTP Dump Enable = %d",bRTPDumpEnable);

   MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_HIGH, "UIBC Generic = %d, Generic Input = %d, Port ID: = %d",pReadCfgCapabilities->uibcCapability.config.category,
                                                                                                  pReadCfgCapabilities->uibcCapability.config.generic_input_type,
                                                                                                  pReadCfgCapabilities->uibcCapability.port_id);
}

/*==============================================================================

   FUNCTION:    getSupportedHDCPVersion

   DESCRIPTION:
   @brief      gets the supported version from the version string
*//**

@par     DEPENDENCIES:
                     None
*//*
   PARAMETERS:
                     string
*//*     RETURN VALUE:
*//**       @return
                     int

@par     SIDE EFFECTS:
                     none

*//*==========================================================================*/

int getSupportedHDCPVersion(string version)
{
    int nMinorVersion = WFD_HDCP_VERSION_2_1;  // fall back to version 2.1

    nMinorVersion = strstr(version.c_str(), "WFD_HDCP_2_") ?
                      (atoi((char *)&version.c_str()[strlen("WFD_HDCP_2_")]) + 1) :
                      WFD_HDCP_VERSION_2_1;

   if ( ((WFD_HDCP_version_t)nMinorVersion >= WFD_HDCP_VERSION_2_0) &&
        ((WFD_HDCP_version_t)nMinorVersion <= WFD_HDCP_VERSION_MAX))
   {

        MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_LOW, "getSupportedHDCPVersion() WFD_HDCP_2_%d",
                     (nMinorVersion - 1));
        return nMinorVersion;
   }
   nMinorVersion = WFD_HDCP_VERSION_2_1; // fall back to version 2.1
   MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_LOW, "getSupportedHDCPVersion() fall back to WFD_HDCP_2_%d",
                (nMinorVersion - 1));
   return nMinorVersion;
}

#ifdef __cplusplus
}
#endif


