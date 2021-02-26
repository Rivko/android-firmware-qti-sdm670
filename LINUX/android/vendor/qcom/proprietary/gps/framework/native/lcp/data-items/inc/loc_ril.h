/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 Receive Telephony / Data connection Info from upper frameworks

 Copyright (c) 2012-2014 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.

 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/

#ifndef RILINFO_H
#define RILINFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LOC_RIL_VERSION 1
#define LOC_RIL_SERVER "Loc-Ril-Server"

typedef enum
{
  LOC_NW_ROAMING = 1,
  LOC_NW_HOME = 2,
  LOC_NW_OOO = 3,
  LOC_NW_MAX = 0x10000000
} LOC_NWstatus;

typedef enum
{
  LOC_RIL_TECH_CDMA = 0x1,
  LOC_RIL_TECH_GSM = 0x2,
  LOC_RIL_TECH_WCDMA = 0x4,
  LOC_RIL_TECH_LTE = 0x8,
  LOC_RIL_TECH_TD_SCDMA = 0x16,
  LOC_RIL_TECH_MAX = 0x10000000
} LOC_RilTechType;

typedef struct
{
  // Integer (0..999)
  uint16_t mcc;

  // Integer (0..999)
  uint16_t mnc;

  // Integer (0..65535)
  uint16_t lac;

  //Integer (0..268435455)
  uint32_t cid;

  // Integer (0..16383) OPTIONAL,
  uint16_t frequency;

  // Integer  (0..511)
  uint16_t primarySynchcode;

} LOC_RilTechWcdmaCinfo;

typedef struct
{
  // Integer (0..999)
  uint16_t mcc;

  // Integer (0..999)
  uint16_t mnc;

  // Integer (0..65535)
  uint16_t lac;

  // Integer (0..65535)
  uint32_t cid;

  // Integer (0..1023)
  uint16_t arfcn;

  // Integer (0..63)
  uint8_t bsic;

  // Integer (0..63)
  uint8_t rxLevel;

} LOC_RilTechGsmCinfo;

typedef struct
{
  // Integer (0..999)
  // Set to 0 if mcc is not available
  uint16_t mcc;

  // Integer (0..32767)
  uint16_t sid;

  // Integer (0..65535)
  uint16_t nid;

  // Integer (0..65535)
  uint32_t bsid;

  // Latitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
  // It is represented in units of 0.25 seconds and ranges from -1296000
  // to 1296000, both values inclusive (corresponding to a range of -90
  // to +90 degrees). Integer.MAX_VALUE is considered invalid value.
  int32_t bslat;

  // Longitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
  // It is represented in units of 0.25 seconds and ranges from -2592000
  // to 2592000, both values inclusive (corresponding to a range of -180
  // to +180 degrees). Integer.MAX_VALUE is considered invalid value.
  int32_t bslon;

  // Integer (-24..24). Short.MAX_VALUE is considered as
  // timezone information not present.
  int16_t local_timezone_offset_from_utc;

  //Boolean
  bool local_timezone_on_daylight_savings;

  // Integer [0, 511] Pilot PN info
  uint16_t pilotPN;

  // Integer [0, 31] CDMA band class
  uint8_t  bandClass;

  // Integer [0, 2047] CDMA frequency
  uint16_t cdmaFrequency;
} LOC_RilTechCdmaCinfo;

typedef enum
{
  // on LTE narrow band mode, but does not have more info
  // on the specific mode, e.g.: M1 or NB1
  NB_MODE_UNKNOWN = 0,
  // on LTE narrow band mode M1 (up to 1Mbps), but does not
  // have more info regarding whether it is Mode A or Mode B
  NB_MODE_M1 = 1,
  // NB_MODE_CE_MODE_A and NB_MODE_CE_MODE_B are the two sub modes
  // that belong to LTE narrow band mode M1
  NB_MODE_CE_MODE_A =2,
  // NB_MODE_CE_MODE_A and NB_MODE_CE_MODE_B are the two sub modes
  // that belong to LTE narrow band mode M1
  NB_MODE_CE_MODE_B = 3,
  // on LTE narrow band mode NB1 (up to 200Kbps)
  NB_MODE_NB1 = 4,
  // Force to 32-bit
  NB_MODE_MAX = 0x10000000
} LOC_RilLteNbMode;

// LTE Narrow band EARFCN offset
typedef enum
{
  // _X is for negative offset
  // for exmaple, NB_EARFCN_OFFSET_10 means: offset -10
  NB_EARFCN_OFFSET_10 = 0,
  NB_EARFCN_OFFSET_9  = 1,
  NB_EARFCN_OFFSET_8  = 2,
  NB_EARFCN_OFFSET_7  = 3,
  NB_EARFCN_OFFSET_6  = 4,
  NB_EARFCN_OFFSET_5  = 5,
  NB_EARFCN_OFFSET_4  = 6,
  NB_EARFCN_OFFSET_3  = 7,
  NB_EARFCN_OFFSET_2  = 8,
  NB_EARFCN_OFFSET_1  = 9,
  NB_EARFCN_OFFSET_0dot5 = 10,
  // positive OFFSET starts here
  NB_EARFCN_OFFSET0 = 11,
  NB_EARFCN_OFFSET1 = 12,
  NB_EARFCN_OFFSET2 = 13,
  NB_EARFCN_OFFSET3 = 14,
  NB_EARFCN_OFFSET4 = 15,
  NB_EARFCN_OFFSET5 = 16,
  NB_EARFCN_OFFSET6 = 17,
  NB_EARFCN_OFFSET7 = 18,
  NB_EARFCN_OFFSET8 = 19,
  NB_EARFCN_OFFSET9 = 20,
  NB_EARFCN_OFFSET10 = 21,
  // Force to 32-bit
  NB_EARFCN_OFFSET_MAX = 0x10000000
} LOC_RilLteNbEarfcnOffsetType;

typedef struct
{
  // Integer (0..999)
  uint16_t mcc;

  // Integer (0..999)
  uint16_t mnc;

  // Integer (0..65535)
  // 0 = tac unavailable
  uint16_t tac;

  // Integer (0..503)
  // SHRT_MAX = pci unavailable
  // Optional value
  uint16_t pci;

  // Integer (0..268435455)
  uint32_t cid;

  // Integer (0..262143): 65535 will be treated as not available
  uint32_t earfcn;

  // optional field
  LOC_RilLteNbMode nb_mode;

  // optional field
  LOC_RilLteNbEarfcnOffsetType nb_earfcn_offset;

} LOC_RilTechLteCinfo;

// The following defines are for valid_mask in RilCellInfo,
// please make sure those defines do not overlap with
// LOC_RIL_TECH_XXX
#define LOC_RIL_CELLINFO_HAS_NW_STATUS 0x00000001
#define LOC_RIL_CELLINFO_HAS_TECH_TYPE 0x00000002
#define LOC_RIL_CELLINFO_HAS_CELL_INFO 0x00000004

// The following defines are for valid mask for Loc_RilTechGsmCinfo
// and Loc_RilTechWcdmaCinfo fields.
#define LOC_RIL_TECH_GW_HAS_MCC 0x00000008
#define LOC_RIL_TECH_GW_HAS_MNC 0x00000010
#define LOC_RIL_TECH_GW_HAS_LAC 0x00000020
#define LOC_RIL_TECH_GW_HAS_CID 0x00000040

// The following defines are valid mask for Loc_RilTechGsmInfo
#define LOC_RIL_TECH_GSM_HAS_ARFCN     0x0000080
#define LOC_RIL_TECH_GSM_HAS_BSIC      0x0000100
#define LOC_RIL_TECH_GSM_HAS_RXLEVEL   0x0000200

// The following defines are valid mask for Loc_RilTechWCDMAInfo
#define LOC_RIL_TECH_WCDMA_HAS_FREQUENCY           0x0000080
#define LOC_RIL_TECH_WCDMA_HAS_PRIMARY_SYNCHCODE   0x0000100

// The following defines are for valid_mask in LOC_RilTechCdmaCinfo
#define LOC_RIL_TECH_CDMA_HAS_MCC              0x00000008
#define LOC_RIL_TECH_CDMA_HAS_SID              0x00000010
#define LOC_RIL_TECH_CDMA_HAS_NID              0x00000020
#define LOC_RIL_TECH_CDMA_HAS_BSID             0x00000040
#define LOC_RIL_TECH_CDMA_HAS_BSLAT            0x00000080
#define LOC_RIL_TECH_CDMA_HAS_BSLONG           0x00000100
#define LOC_RIL_TECH_CDMA_HAS_TIMEZONE         0x00000200
#define LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING  0x00000400
#define LOC_RIL_TECH_CDMA_HAS_PILOTPN          0x00000800
#define LOC_RIL_TECH_CDMA_HAS_BANDCLASS        0x00001000
#define LOC_RIL_TECH_CDMA_HAS_FREQUENCY        0x00002000

// The following defines are for valid_mask in LOC_RilTechLteCinfo
#define LOC_RIL_TECH_LTE_HAS_MCC     0x00000008
#define LOC_RIL_TECH_LTE_HAS_MNC     0x00000010
#define LOC_RIL_TECH_LTE_HAS_TAC     0x00000020
#define LOC_RIL_TECH_LTE_HAS_PCI     0x00000040
#define LOC_RIL_TECH_LTE_HAS_CID     0x00000080
#define LOC_RIL_TECH_LTE_HAS_EARFCN  0x00000100
#define LOC_RIL_TECH_LTE_HAS_NB_MODE            0x00000200
#define LOC_RIL_TECH_LTE_HAS_NB_EARFCN_OFFSET   0x00000400

union LOC_RilCell
{
  LOC_RilTechCdmaCinfo cdmaCinfo;
  LOC_RilTechGsmCinfo gsmCinfo;
  LOC_RilTechWcdmaCinfo wcdmaCinfo;
  LOC_RilTechLteCinfo lteCinfo;
};

typedef struct LOC_RilCellInfo
{
  uint32_t valid_mask;
  LOC_NWstatus nwStatus;
  LOC_RilTechType rtType;
  LOC_RilCell u;
} LOC_RilCellInfo;

#define LOC_RILAIRIF_CDMA  0x01
#define LOC_RILAIRIF_GSM   0x02
#define LOC_RILAIRIF_WCDMA 0x04
#define LOC_RILAIRIF_LTE   0x08
#define LOC_RILAIRIF_EVDO  0x10
#define LOC_RILAIRIF_WIFI  0x20
typedef uint32_t LOC_RilAirIf_mask;
typedef uint32_t LOC_RilAirIf_type;

#define LOC_RIL_SERVICE_INFO_HAS_AIR_IF_TYPE          0x00000001
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE  0x00000002
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC          0x00000004
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC          0x00000008
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME         0x00000010
#define LOC_RIL_CARRIER_NAME_MAX_LEN                  32

typedef struct LOC_RilServiceInfo
{
  uint32_t valid_mask;        // Set to 0 in case of failure
  LOC_RilAirIf_mask airIf_type;        // Air interface types supported by mobile
  LOC_RilAirIf_type carrierAirIf_type; // home carrier air interface type is either:
                                       // LOC_RILAIRIF_GSM, LOC_RILAIRIF_WCDMA or
                                       // LOC_RILAIRIF_CDMA
  uint16_t carrierMcc;        // home carrier MCC
  uint16_t carrierMnc;        // home carrier MNC
  uint16_t carrierNameLen;    // home carrier name length
  char carrierName[LOC_RIL_CARRIER_NAME_MAX_LEN]; // home carrier name
} LOC_RilServiceInfo;

typedef uint32_t LOC_Ril_version;

typedef struct LOC_RilVoiceCallInfo
{
  bool isStateValid;
  bool isInVoiceCall;
  time_t voiceCallStartTime;
  time_t cellUpdateTimeInVoiceCall;
} LOC_RilVoiceCallInfo;

//
// Response message payload
// +--------------------------------------------------------------------+
// | Payload Length in bytes |  Payload      |
// +--------------------------------------------------------------------+
// <<------- 32 bits -------> <-- variable -->
//
typedef struct
{
  uint32_t msgLength;
  // It is caller's responsibility to allocate enough memory to hold the msg paylaod
  uint8_t msgPayload[1];
} LOC_RilRespMsgBuf;

typedef enum
{
  // Request version number for this service
  LOC_RIL_REQUEST_VERSION = 1,

  // Register for cell update, update will be sent when
  // there is update in RilCellInfo changes.
  // Response will be LOC_RIL_RESPONSE_CELL_UPDATE.
  LOC_RIL_REGISTER_CELL_UPDATE = 2,

  // De-register for cell update
  // No response is sent back.
  LOC_RIL_DEREISTER_CELL_UPDATE = 3,

  // Request for one-time RilServiceInfo
  // Response will be LOC_RIL_RESPONSE_SERVICE_INFO.
  LOC_RIL_REQUEST_SERVICE_INFO = 4,

  // Request for voice call update
  LOC_RIL_REGISTER_VOICE_CALL_UPDATE = 5,

  // Deregister for voice call update
  // No reponse is sent back
  LOC_RIL_DEREGISTER_VOICE_CALL_UPDATE = 6,

  // Force it to 32-bit
  LOC_RIL_REQUEST_TYPE_MAX = 0x10000000
} LOC_RilRequestType;

typedef enum
{
  // Response version number for this service.
  // Payload length will be sizeof (uint32_t)
  // Payload will be 32-bit version number
  LOC_RIL_RESPONSE_VERSION = 1,

  // Update when there is a change in RilCellInfo changes.
  // Payload length will be sizeof (LOC_RilCellInfo)
  // Payload will be LOC_RilCellInfo.
  LOC_RIL_RESPONSE_CELL_UPDATE = 2,

  // Update when phone is OOO.
  // Payload length will be 0 and there will be no payload.
  LOC_RIL_RESPONSE_PHONE_OOO = 3,

  // Reponse for RilServiceInfo request
  // Payload length will be sizeof(LOC_RilServiceInfo) and
  // payload will be LOC_RilServiceInfo
  LOC_RIL_RESPONSE_SERVICE_INFO = 4,

  // Force it to 32-bit
  LOC_RIL_RESPONSE_TYPE_MAX = 0x10000000
} LOC_RilResponseType;

#ifdef __cplusplus
}
#endif

#endif // RILINFO_H
