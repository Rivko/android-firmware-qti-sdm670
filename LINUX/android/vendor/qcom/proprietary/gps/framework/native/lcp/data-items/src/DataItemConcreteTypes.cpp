/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemConcreteTypes Implementation

  Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <string>
#include <string.h>
#include <comdef.h>
#include <DataItemId.h>
#include <IDataItemCore.h>
#include <DataItemDefines.h>
#include <DataItemConcreteTypes.h>
#include <loc_ril.h>
#include <postcard.h>
#include <log_util.h>
#include <loc_pla.h>

using namespace std;
using namespace izat_manager;
using namespace qc_loc_fw;
using loc_core::IDataItemCore;

// macro for stringifier
#define DATA_ITEM_STRINGIFY(T) \
void T :: stringify (string & valueStr)

#define STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(T,ID) \
if(getId() != ID) { result = 1; break; } \
T * d = static_cast<T *>(this);

// macro for copier
#define DATA_ITEM_COPY(T) \
int32_t T::copy (IDataItemCore * src, bool * dataItemCopied)

#define COPIER_ERROR_CHECK_AND_DOWN_CAST(T,ID) \
   if(src == NULL) { result = 1; break; } \
   if(getId() != src->getId()) { result = 2; break; } \
   if(getId() != ID) { result = 3; break; } \
   T * s = static_cast<T *>(this); \
   T * d = static_cast<T *>(src);

#define SET_COPIED(B) \
    if (NULL != B) { *B = true;}

// macro for serializer
#define DATA_ITEM_SERIALIZE(T) \
int32 T::serialize (OutPostcard * oc)

#define BREAK_AND_DELETE_IF_NON_ZERO(ERR,X,OBJ) \
if(0!=(X)) {result = (ERR); delete (OBJ); break;}

#define ADD_INNER_CARD() \
    if (ic != NULL) { \
        BREAK_AND_DELETE_IF_NON_ZERO(100, ic->finalize(), ic); \
        BREAK_AND_DELETE_IF_NON_ZERO(101, oc->addCard(getCardName().c_str(), ic), ic); \
        delete ic; ic = NULL; \
    } \

#define CREATE_INNER_CARD(T,ID) \
    if(oc==NULL) { result = 1; break; } \
    if(getId() != ID) { result = 2; break; } \
    if(hasCardName()) { \
        ic = OutPostcard::createInstance(); \
        BREAK_IF_ZERO(3, ic); \
        BREAK_AND_DELETE_IF_NON_ZERO(4, ic->init(), ic); \
        card = ic; \
    } \
    T * d = static_cast<T *>(this);

// macro for deserializer
#define DATA_ITEM_DESERIALIZE(T) \
int32 T::deSerialize \
(InPostcard *)

/*
    This macro to be used when we start filling the methods
    defined using DATA_ITEM_DESERIALIZE macro
#define CHECK_AND_ADD_NEW_CARD() \
    if(ic == NULL) { result = 1; break; } \
    if(hasCardName() ) { \
        card = OutPostcard::createInstance(); \
        if(card == NULL) { result = 2; break; } \
        BREAK_IF_NON_ZERO(3,oc->addCard(getCardName().c_str(), card)); \
    }

*/

// Ctors
AirplaneModeDataItem :: AirplaneModeDataItem
 (bool mode)
:
AirplaneModeDataItemBase(mode),
mCardName(AIRPLANEMODE_CARD) {}

ENHDataItem :: ENHDataItem
 (bool enabled)
:
ENHDataItemBase(enabled),
mCardName(ENH_CARD) {}

GPSStateDataItem :: GPSStateDataItem
 (bool enabled)
:
GPSStateDataItemBase(enabled),
mCardName(GPSSTATE_CARD) {}

NLPStatusDataItem :: NLPStatusDataItem
 (bool enabled)
:
NLPStatusDataItemBase(enabled),
mCardName(NLPSTATUS_CARD) {}

WifiHardwareStateDataItem :: WifiHardwareStateDataItem
 (bool enabled)
:
WifiHardwareStateDataItemBase(enabled),
mCardName(WIFIHARDWARESTATE_CARD) {}

ScreenStateDataItem :: ScreenStateDataItem
 (bool state)
:
ScreenStateDataItemBase(state),
mCardName(SCREENSTATE_CARD) {}

PowerConnectStateDataItem :: PowerConnectStateDataItem
 (bool state)
:
PowerConnectStateDataItemBase(state),
mCardName(POWERCONNECTSTATE_CARD) {}

BatteryLevelDataItem :: BatteryLevelDataItem
 (uint8_t batteryPct)
:
BatteryLevelDataItemBase(batteryPct),
mCardName(BATTERYLEVEL_CARD) {}


TimeZoneChangeDataItem :: TimeZoneChangeDataItem
 (
    int64 currTimeMillis,
    int32 rawOffset,
    int32 dstOffset
)
:
TimeZoneChangeDataItemBase(currTimeMillis, rawOffset, dstOffset),
mCardName(TIMEZONECHANGE_CARD) {}

TimeChangeDataItem :: TimeChangeDataItem
 (
    int64 currTimeMillis,
    int32 rawOffset,
    int32 dstOffset
)
:
TimeChangeDataItemBase(currTimeMillis, rawOffset, dstOffset),
mCardName(TIMECHANGE_CARD)
{}

ShutdownStateDataItem :: ShutdownStateDataItem
 (bool state)
:
ShutdownStateDataItemBase(state),
mCardName(SHUTDOWN_CARD) {}

AssistedGpsDataItem :: AssistedGpsDataItem
 (bool enabled)
:
AssistedGpsDataItemBase(enabled),
mCardName(ASSISTEDGPS_CARD) {}

NetworkInfoDataItem :: NetworkInfoDataItem
 (
    int32 type,
    std :: string typeName,
    std :: string subTypeName,
    bool available,
    bool connected,
    bool roaming
)
:
NetworkInfoDataItemBase(getNormalizedType(type), type, typeName,
                        subTypeName, available, connected, roaming),
mCardName(NETWORKINFO_CARD) {}

ServiceStatusDataItem :: ServiceStatusDataItem
 (int32 serviceState)
:
ServiceStatusDataItemBase(serviceState),
mCardName(SERVICESTATUS_CARD) {}

ModelDataItem :: ModelDataItem
 (const std :: string & name)
:
ModelDataItemBase(name),
mCardName(MODEL_CARD) {}

ManufacturerDataItem :: ManufacturerDataItem
 (const std :: string & name)
:
ManufacturerDataItemBase(name),
mCardName(MANUFACTURER_CARD) {}

RilServiceInfoDataItem :: RilServiceInfoDataItem
 (LOC_RilServiceInfo * serviceInfo)
:
RilServiceInfoDataItemBase(),
mCardName(RILSERVICEINFO_CARD),
mServiceInfo (serviceInfo),
mDestroy (false)
{
    if (mServiceInfo == NULL) {
        mServiceInfo = new  (std :: nothrow) LOC_RilServiceInfo;
        if (mServiceInfo) {
            mDestroy = true;
            // Initialize to default values
            mServiceInfo->valid_mask = RTLLSERVICEINFO_DEFAULT_VALID_MASK;
            mServiceInfo->airIf_type = RTLLSERVICEINFO_DEFAULT_AIRIF_TYPE;
            mServiceInfo->carrierAirIf_type = RTLLSERVICEINFO_DEFAULT_CARRIER_AIRIF_TYPE;
            mServiceInfo->carrierMcc = RTLLSERVICEINFO_DEFAULT_CARRIER_MCC;
            mServiceInfo->carrierMnc = RTLLSERVICEINFO_DEFAULT_CARRIER_MNC;
            mServiceInfo->carrierNameLen = RTLLSERVICEINFO_DEFAULT_CARRIER_NAMELEN;
            strlcpy (mServiceInfo->carrierName, RTLLSERVICEINFO_DEFAULT_CARRIER_NAME, LOC_RIL_CARRIER_NAME_MAX_LEN);
        }
    }
    mData = mServiceInfo;
}

RilCellInfoDataItem :: RilCellInfoDataItem
 (LOC_RilCellInfo * cellInfo)
:
RilCellInfoDataItemBase(),
mCellInfo (cellInfo),
mCardName(RILCELLINFO_CARD),
mDestroy (false)
{
    if (mCellInfo == NULL) {
        mCellInfo = new  (std :: nothrow) LOC_RilCellInfo;
        if (mCellInfo) {
            mDestroy = true;
            // Initialize to default values
            mCellInfo->valid_mask = RILLCELLINFO_DEFAULT_VALID_MASK;
            mCellInfo->nwStatus = static_cast<LOC_NWstatus> (RILLCELLINFO_DEFAULT_NETWORK_STATUS);
            mCellInfo->rtType = static_cast<LOC_RilTechType> (RILLCELLINFO_DEFAULT_RIL_TECH_TYPE);
            mCellInfo->u.cdmaCinfo.mcc = RILLCELLINFO_DEFAULT_CDMA_CINFO_MCC;
            mCellInfo->u.cdmaCinfo.sid = RILLCELLINFO_DEFAULT_CDMA_CINFO_SID;
            mCellInfo->u.cdmaCinfo.nid = RILLCELLINFO_DEFAULT_CDMA_CINFO_NID;
            mCellInfo->u.cdmaCinfo.bsid = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSID;
            mCellInfo->u.cdmaCinfo.bslat = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSLAT;
            mCellInfo->u.cdmaCinfo.bslon = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSLON;
            mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc =
                RILLCELLINFO_DEFAULT_CDMA_CINFO_LOCAL_TIME_ZONE_OFFSET_FROM_UTC;
            mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings =
                RILLCELLINFO_DEFAULT_CDMA_CINFO_LOCAL_TIME_ZONE_ON_DAYLIGHT_SAVINGS;
        }
    }
    mData = mCellInfo;
}

bool RilServiceInfoDataItem :: operator==(const RilServiceInfoDataItemBase& other) const {
    LOC_RilServiceInfo* data = reinterpret_cast<LOC_RilServiceInfo*>(other.mData);
    return (data == mServiceInfo ||
            (nullptr != data && nullptr != mServiceInfo &&
             data->valid_mask == mServiceInfo->valid_mask &&
             data->airIf_type == mServiceInfo->airIf_type &&
             data->carrierAirIf_type == mServiceInfo->carrierAirIf_type &&
             data->carrierMcc == mServiceInfo->carrierMcc &&
             data->carrierMnc == mServiceInfo->carrierMnc &&
             data->carrierNameLen == mServiceInfo->carrierNameLen &&
             strncmp(data->carrierName, mServiceInfo->carrierName,
                     std::min(data->carrierNameLen,
                              (decltype(data->carrierNameLen))LOC_RIL_CARRIER_NAME_MAX_LEN))));
}

bool RilCellInfoDataItem :: operator==(const RilCellInfoDataItemBase& other) const {
    LOC_RilCellInfo* data = reinterpret_cast<LOC_RilCellInfo*>(other.mData);
    if (data == mCellInfo) return true;
    bool equal = (nullptr != data && nullptr != mCellInfo &&
                  data->valid_mask == mCellInfo->valid_mask &&
                  data->nwStatus == mCellInfo->nwStatus &&
                  data->rtType == mCellInfo->rtType);
    if (equal) {
        switch (data->rtType) {
        case LOC_RIL_TECH_CDMA:
            return (data->u.cdmaCinfo.mcc == mCellInfo->u.cdmaCinfo.mcc &&
                    data->u.cdmaCinfo.sid == mCellInfo->u.cdmaCinfo.sid &&
                    data->u.cdmaCinfo.nid == mCellInfo->u.cdmaCinfo.nid &&
                    data->u.cdmaCinfo.bsid == mCellInfo->u.cdmaCinfo.bsid &&
                    data->u.cdmaCinfo.bslat == mCellInfo->u.cdmaCinfo.bslat &&
                    data->u.cdmaCinfo.bslon == mCellInfo->u.cdmaCinfo.bslon &&
                    data->u.cdmaCinfo.local_timezone_offset_from_utc ==
                            mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc &&
                    data->u.cdmaCinfo.local_timezone_on_daylight_savings ==
                            mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings);
        case LOC_RIL_TECH_GSM:
            return (data->u.gsmCinfo.mcc == mCellInfo->u.gsmCinfo.mcc &&
                    data->u.gsmCinfo.mnc == mCellInfo->u.gsmCinfo.mnc &&
                    data->u.gsmCinfo.lac == mCellInfo->u.gsmCinfo.lac &&
                    data->u.gsmCinfo.cid == mCellInfo->u.gsmCinfo.cid);
        case LOC_RIL_TECH_WCDMA:
            return (data->u.wcdmaCinfo.mcc == mCellInfo->u.wcdmaCinfo.mcc &&
                    data->u.wcdmaCinfo.mnc == mCellInfo->u.wcdmaCinfo.mnc &&
                    data->u.wcdmaCinfo.lac == mCellInfo->u.wcdmaCinfo.lac &&
                    data->u.wcdmaCinfo.cid == mCellInfo->u.wcdmaCinfo.cid);
        case LOC_RIL_TECH_LTE:
            return (data->u.lteCinfo.mcc == mCellInfo->u.lteCinfo.mcc &&
                    data->u.lteCinfo.mnc == mCellInfo->u.lteCinfo.mnc &&
                    data->u.lteCinfo.tac == mCellInfo->u.lteCinfo.tac &&
                    data->u.lteCinfo.pci == mCellInfo->u.lteCinfo.pci &&
                    data->u.lteCinfo.cid == mCellInfo->u.lteCinfo.cid);
        }
    }
    return equal;
}

void RilServiceInfoDataItem :: setPeerData(RilServiceInfoDataItemBase& peer) const {
    if (nullptr == peer.mData) {
        peer.mData = malloc(sizeof(*mServiceInfo));
        if (nullptr != peer.mData) {
            *(decltype(mServiceInfo))peer.mData = *mServiceInfo;
        }
    }
}

void RilCellInfoDataItem :: setPeerData(RilCellInfoDataItemBase& peer) const {
    if (nullptr == peer.mData) {
        peer.mData = malloc(sizeof(*mCellInfo));
        if (nullptr != peer.mData) {
            *(decltype(mCellInfo))peer.mData = *mCellInfo;
        }
    }
}

WifiSupplicantStatusDataItem :: WifiSupplicantStatusDataItem()
:
WifiSupplicantStatusDataItemBase(),
mCardName(WIFI_SUPPLICANT_STATUS_CARD)
{}

TacDataItem :: TacDataItem
 (const std :: string & name)
:
TacDataItemBase(name),
mCardName(TAC_CARD) {}

MccmncDataItem :: MccmncDataItem
 (const std :: string & name)
:
MccmncDataItemBase(name),
mCardName(MCCMNC_CARD) {}

BtLeDeviceScanDetailsDataItem :: BtLeDeviceScanDetailsDataItem()
:
BtLeDeviceScanDetailsDataItemBase(),
mCardName(BTLESCANDETAILS_CARD) {}

BtDeviceScanDetailsDataItem :: BtDeviceScanDetailsDataItem()
:
BtDeviceScanDetailsDataItemBase(),
mCardName(BTSCANDETAILS_CARD) {}

// Dtors
AirplaneModeDataItem :: ~AirplaneModeDataItem () {}
ENHDataItem :: ~ENHDataItem () {}
GPSStateDataItem :: ~GPSStateDataItem () {}
NLPStatusDataItem :: ~NLPStatusDataItem () {}
WifiHardwareStateDataItem :: ~WifiHardwareStateDataItem () {}
ScreenStateDataItem :: ~ScreenStateDataItem () {}
PowerConnectStateDataItem :: ~PowerConnectStateDataItem () {}
TimeZoneChangeDataItem :: ~TimeZoneChangeDataItem () {}
TimeChangeDataItem :: ~TimeChangeDataItem () {}
ShutdownStateDataItem :: ~ShutdownStateDataItem () {}
AssistedGpsDataItem :: ~AssistedGpsDataItem () {}
NetworkInfoDataItem :: ~NetworkInfoDataItem () {}
ServiceStatusDataItem :: ~ServiceStatusDataItem () {}
ModelDataItem :: ~ModelDataItem () {}
ManufacturerDataItem :: ~ManufacturerDataItem () {}
RilServiceInfoDataItem :: ~RilServiceInfoDataItem () {
    if (  (mDestroy) &&  (mServiceInfo) ) { delete mServiceInfo; }
    mData = nullptr;
}
RilCellInfoDataItem :: ~RilCellInfoDataItem () {
    if (  (mDestroy) &&  (mCellInfo) ) { delete mCellInfo; }
    mData = nullptr;
}
WifiSupplicantStatusDataItem :: ~WifiSupplicantStatusDataItem () {}
TacDataItem :: ~TacDataItem () {}
MccmncDataItem :: ~MccmncDataItem () {}
BtLeDeviceScanDetailsDataItem :: ~BtLeDeviceScanDetailsDataItem () {}
BtDeviceScanDetailsDataItem :: ~BtDeviceScanDetailsDataItem () {}
BatteryLevelDataItem :: ~BatteryLevelDataItem () {}

// stringify
DATA_ITEM_STRINGIFY(AirplaneModeDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(AirplaneModeDataItem, AIRPLANEMODE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = AIRPLANEMODE_FIELD_MODE;
        valueStr += ": ";
        valueStr += (d->mMode) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
}

DATA_ITEM_STRINGIFY(ENHDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(ENHDataItem, ENH_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = ENH_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(GPSStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(GPSStateDataItem, GPSSTATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = GPSSTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(NLPStatusDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(NLPStatusDataItem,NLPSTATUS_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = NLPSTATUS_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(WifiHardwareStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(WifiHardwareStateDataItem,WIFIHARDWARESTATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = WIFIHARDWARESTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ScreenStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(ScreenStateDataItem,SCREEN_STATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = SCREENSTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mState) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(PowerConnectStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(PowerConnectStateDataItem,POWER_CONNECTED_STATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = POWERCONNECTSTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mState) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}

DATA_ITEM_STRINGIFY(BatteryLevelDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(BatteryLevelDataItem,BATTERY_LEVEL_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += BATTERYLEVEL_FIELD_BATTERY_PCT;
        valueStr += ": ";
        char state [12];
        snprintf (state, 12, "%d", d->mBatteryPct);
        valueStr += string (state);
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
}

DATA_ITEM_STRINGIFY(TimeZoneChangeDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(TimeZoneChangeDataItem,TIMEZONE_CHANGE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = TIMEZONECHANGE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += "true";
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(TimeChangeDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(TimeChangeDataItem,TIME_CHANGE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = TIMECHANGE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += "true";
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ShutdownStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(ShutdownStateDataItem,SHUTDOWN_STATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = SHUTDOWN_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mState) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(AssistedGpsDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(AssistedGpsDataItem,ASSISTED_GPS_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = ASSISTEDGPS_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(NetworkInfoDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(NetworkInfoDataItem,NETWORKINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = NETWORKINFO_CARD;
        valueStr += "::";
        valueStr += NETWORKINFO_FIELD_TYPE;
        valueStr += "s_MASK: ";
        char type [12];
        snprintf (type, 12, "%x", mAllTypes);
        valueStr += string (type);
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_TYPENAME;
        valueStr += ": ";
        valueStr += d->mTypeName;
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_SUBTYPENAME;
        valueStr += ": ";
        valueStr += d->mSubTypeName;
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_AVAILABLE;
        valueStr += ": ";
        valueStr += (d->mAvailable) ? ("true") : ("false");
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_CONNECTED;
        valueStr += ": ";
        valueStr += (d->mConnected) ? ("true") : ("false");
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_ROAMING;
        valueStr += ": ";
        valueStr += (d->mRoaming) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ServiceStatusDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(ServiceStatusDataItem,SERVICESTATUS_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += SERVICESTATUS_FIELD_STATE;
        valueStr += ": ";
        char state [12];
        snprintf (state, 12, "%d", d->mServiceState);
        valueStr += string (state);
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ModelDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(ModelDataItem,MODEL_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += MODEL_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mModel;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ManufacturerDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(ManufacturerDataItem,MANUFACTURER_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += MANUFACTURER_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mManufacturer;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(RilServiceInfoDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(RilServiceInfoDataItem,RILSERVICEINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += RILSERVICEINFO_CARD;
        if (d->mServiceInfo->valid_mask) {
            valueStr += ":";

            valueStr += RILSERVICEINFO_FIELD_ARIF_TYPE_MASK;
            valueStr += ": ";
            char t[10];
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->airIf_type);
            valueStr += t;


            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE;
            valueStr += "(CDMA-1, GSM -2, WCDMA-4, LTA-8, EVDO-16, WIFI-32):";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierAirIf_type);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_MCC;
            valueStr += ": ";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierMcc);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_MNC;
            valueStr += ": ";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierMcc);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_NAME;
            valueStr += ": ";
            valueStr += d->mServiceInfo->carrierName;
        } else {
            valueStr += "Invalid";
        }
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(RilCellInfoDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(RilCellInfoDataItem,RILCELLINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += RILCELLINFO_CARD;
        if(d->mCellInfo->valid_mask) {
            if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_NW_STATUS) ==
                LOC_RIL_CELLINFO_HAS_NW_STATUS) {
                valueStr += ": ";
                valueStr += RILCELLINFO_FIELD_NETWORK_STATUS;
                valueStr += ": ";
                if (d->mCellInfo->nwStatus == LOC_NW_ROAMING) {
                    valueStr += "ROAMING";
                } else if (d->mCellInfo->nwStatus == LOC_NW_HOME) {
                    valueStr += "ROAMING";
                } else {
                    valueStr += "OOO";
                }
            }
            if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_CELL_INFO) ==
                LOC_RIL_CELLINFO_HAS_CELL_INFO) {
                if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_TECH_TYPE) ==
                    LOC_RIL_CELLINFO_HAS_TECH_TYPE) {

                        valueStr += ", ";
                        valueStr += RILCELLINFO_FIELD_RIL_TECH_TYPE;
                        valueStr += ": ";

                    switch(d->mCellInfo->rtType) {
                        case LOC_RIL_TECH_CDMA:
                        valueStr += "CDMA";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_MCC) == LOC_RIL_TECH_CDMA_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_SID) == LOC_RIL_TECH_CDMA_HAS_SID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_SID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.sid);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_NID) == LOC_RIL_TECH_CDMA_HAS_NID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_NID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.nid);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSID) == LOC_RIL_TECH_CDMA_HAS_BSID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bsid);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLAT) == LOC_RIL_TECH_CDMA_HAS_BSLAT) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSLAT;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bslat);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLONG) == LOC_RIL_TECH_CDMA_HAS_BSLONG) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSLON;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bslon);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_TIMEZONE) == LOC_RIL_TECH_CDMA_HAS_TIMEZONE) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_UTC_TIME_OFFSET;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) == LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_DAYLIGHT_TIMEZONE;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_GSM:
                        valueStr += "GSM";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) == LOC_RIL_TECH_GW_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) == LOC_RIL_TECH_GW_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.mnc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) == LOC_RIL_TECH_GW_HAS_LAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_LAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.lac);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) == LOC_RIL_TECH_GW_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.cid);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_WCDMA:
                        valueStr += "WCDMA";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) == LOC_RIL_TECH_GW_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) == LOC_RIL_TECH_GW_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.mnc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) == LOC_RIL_TECH_GW_HAS_LAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_LAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.lac);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) == LOC_RIL_TECH_GW_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.cid);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_LTE:
                        valueStr += "LTE";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MCC) == LOC_RIL_TECH_LTE_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MNC) == LOC_RIL_TECH_LTE_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.mnc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_TAC) == LOC_RIL_TECH_LTE_HAS_TAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_TAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.tac);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_PCI) == LOC_RIL_TECH_LTE_HAS_PCI) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_PCI;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.pci);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_CID) == LOC_RIL_TECH_LTE_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.cid);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_TD_SCDMA:
                        case LOC_RIL_TECH_MAX:
                        default:
                        break;
                    }; // end switch
                }
            }
        }
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}

DATA_ITEM_STRINGIFY(WifiSupplicantStatusDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(WifiSupplicantStatusDataItem, WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID);
        valueStr += "Attach state: ";
        char t[50];
        memset (t, '\0', 50);
        snprintf (t, 50, "%d", d->mState);
        valueStr += t;

        valueStr += ", Mac address valid: ";
        valueStr += (d->mApMacAddressValid) ? ("true") : ("false");

        valueStr += ", AP MAC address: ";
        memset (t, '\0', 50);
        snprintf
        (
            t,
            50,
            "[%02x:%02x:%02x:%02x:%02x:%02x]",
            d->mApMacAddress[0],
            d->mApMacAddress[1],
            d->mApMacAddress[2],
            d->mApMacAddress[3],
            d->mApMacAddress[4],
            d->mApMacAddress[5]
        );
        valueStr += t;

        valueStr += ", Wifi-Ap SSID Valid: ";
        valueStr += (d->mWifiApSsidValid) ? ("true") : ("false");

        valueStr += ", SSID: ";
        valueStr += d->mWifiApSsid;

    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
}
DATA_ITEM_STRINGIFY(TacDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(TacDataItem,TAC_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += TAC_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mValue;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(MccmncDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(MccmncDataItem,MCCMNC_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += MCCMNC_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mValue;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}

DATA_ITEM_STRINGIFY(BtLeDeviceScanDetailsDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(BtLeDeviceScanDetailsDataItem,BTLE_SCAN_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += BTLESCANDETAILS_FIELD_VALID;
        valueStr += ": ";
        valueStr += d->mValidSrnData;
        valueStr += ", ";

        valueStr += BTLESCANDETAILS_FIELD_RSSI;
        valueStr += ": ";
        valueStr += d->mApSrnRssi;
        valueStr += ", ";

        char t[10];
        memset (t, '\0', 10);
        valueStr += BTLESCANDETAILS_FIELD_MAC;
        valueStr += ": ";
        snprintf
        (
            t,
            10,
            "[%02x:%02x:%02x:%02x:%02x:%02x]",
            d->mApSrnMacAddress[0],
            d->mApSrnMacAddress[1],
            d->mApSrnMacAddress[2],
            d->mApSrnMacAddress[3],
            d->mApSrnMacAddress[4],
            d->mApSrnMacAddress[5]
        );
        valueStr += t;
        valueStr += ", ";

        valueStr += BTLESCANDETAILS_FIELD_SCANREQ;
        valueStr += ": ";
        valueStr += d->mApSrnTimestamp;
        valueStr += ", ";

        valueStr += BTLESCANDETAILS_FIELD_SCANSTART;
        valueStr += ": ";
        valueStr += d->mRequestTimestamp;
        valueStr += ", ";

        valueStr += BTLESCANDETAILS_FIELD_SCANRECV;
        valueStr += ": ";
        valueStr += d->mReceiveTimestamp;
        valueStr += ", ";

        valueStr += BTLESCANDETAILS_FIELD_SCANERROR;
        valueStr += ": ";
        valueStr += d->mErrorCause;
        valueStr += ", ";
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
}

DATA_ITEM_STRINGIFY(BtDeviceScanDetailsDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        STRINGIFY_ERROR_CHECK_AND_DOWN_CAST(BtDeviceScanDetailsDataItem,BT_SCAN_DATA_ITEM_ID);
        valueStr.clear ();

        valueStr += BTSCANDETAILS_FIELD_VALID;
        valueStr += ": ";
        valueStr += d->mValidSrnData;
        valueStr += ", ";

        valueStr += BTSCANDETAILS_FIELD_RSSI;
        valueStr += ": ";
        valueStr += d->mApSrnRssi;
        valueStr += ", ";

        char t[10];
        memset (t, '\0', 10);
        valueStr += BTSCANDETAILS_FIELD_MAC;
        valueStr += ": ";
        snprintf
        (
            t,
            10,
            "[%02x:%02x:%02x:%02x:%02x:%02x]",
            d->mApSrnMacAddress[0],
            d->mApSrnMacAddress[1],
            d->mApSrnMacAddress[2],
            d->mApSrnMacAddress[3],
            d->mApSrnMacAddress[4],
            d->mApSrnMacAddress[5]
        );
        valueStr += t;
        valueStr += ", ";

        valueStr += BTSCANDETAILS_FIELD_SCANREQ;
        valueStr += ": ";
        valueStr += d->mApSrnTimestamp;
        valueStr += ", ";

        valueStr += BTSCANDETAILS_FIELD_SCANSTART;
        valueStr += ": ";
        valueStr += d->mRequestTimestamp;
        valueStr += ", ";

        valueStr += BTSCANDETAILS_FIELD_SCANRECV;
        valueStr += ": ";
        valueStr += d->mReceiveTimestamp;
        valueStr += ", ";

        valueStr += BTSCANDETAILS_FIELD_SCANERROR;
        valueStr += ": ";
        valueStr += d->mErrorCause;
        valueStr += ", ";
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
}

// copy
DATA_ITEM_COPY(AirplaneModeDataItem) {
   int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(AirplaneModeDataItem, AIRPLANEMODE_DATA_ITEM_ID);
        if(s->mMode == d->mMode) { result = 0; break; }
         s->mMode = d->mMode;
         SET_COPIED(dataItemCopied);
         result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ENHDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(ENHDataItem, ENH_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result =0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(GPSStateDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(GPSStateDataItem, GPSSTATE_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
 }
DATA_ITEM_COPY(NLPStatusDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(NLPStatusDataItem,NLPSTATUS_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
         s->mEnabled = d->mEnabled;
         SET_COPIED(dataItemCopied);
         result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(WifiHardwareStateDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(WifiHardwareStateDataItem,WIFIHARDWARESTATE_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ScreenStateDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(ScreenStateDataItem,SCREEN_STATE_DATA_ITEM_ID);
        if(s->mState == d->mState) { result = 0; break; }
        s->mState = d->mState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(PowerConnectStateDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(PowerConnectStateDataItem,POWER_CONNECTED_STATE_DATA_ITEM_ID);
        if(s->mState == d->mState) { result = 0; break; }
        s->mState = d->mState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}

DATA_ITEM_COPY(BatteryLevelDataItem) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(BatteryLevelDataItem,BATTERY_LEVEL_DATA_ITEM_ID);
        if(s->mBatteryPct == d->mBatteryPct) { result = 0; break; }
        s->mBatteryPct = d->mBatteryPct;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}

DATA_ITEM_COPY(TimeZoneChangeDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(TimeZoneChangeDataItem,TIMEZONE_CHANGE_DATA_ITEM_ID);
        if(s->mCurrTimeMillis == d->mCurrTimeMillis &&
           s->mRawOffsetTZ == d->mRawOffsetTZ &&
           s->mDstOffsetTZ == d->mDstOffsetTZ) {
            result = 0;
            break;
        }
        s->mCurrTimeMillis = d->mCurrTimeMillis;
        s->mRawOffsetTZ = d->mRawOffsetTZ;
        s->mDstOffsetTZ = d->mDstOffsetTZ;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(TimeChangeDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(TimeChangeDataItem,TIME_CHANGE_DATA_ITEM_ID);
        if(s->mCurrTimeMillis == d->mCurrTimeMillis &&
           s->mRawOffsetTZ == d->mRawOffsetTZ &&
           s->mDstOffsetTZ == d->mDstOffsetTZ) {
            result = 0;
            break;
        }
        s->mCurrTimeMillis = d->mCurrTimeMillis;
        s->mRawOffsetTZ = d->mRawOffsetTZ;
        s->mDstOffsetTZ = d->mDstOffsetTZ;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ShutdownStateDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(ShutdownStateDataItem,SHUTDOWN_STATE_DATA_ITEM_ID);
        if(s->mState == d->mState) { result = 0; break; }
        s->mState = d->mState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(AssistedGpsDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(AssistedGpsDataItem,ASSISTED_GPS_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(NetworkInfoDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(NetworkInfoDataItem,NETWORKINFO_DATA_ITEM_ID);
        NetworkType type = ((NetworkInfoDataItemBase*)d)->getType();
        if( (s->mAllTypes == d->mAllTypes) &&
            (s->getType() == type) && (0 == s->mTypeName.compare(d->mTypeName)) &&
            (0 == s->mSubTypeName.compare(d->mSubTypeName)) &&
            (s->mAvailable == d->mAvailable) &&
            (s->mConnected == d->mConnected) &&
            (s->mRoaming == d->mRoaming) ) {
            result = 0;
            break;
        }

        s->mAllTypes = (d->mAllTypes == 0 && d->mConnected) ? typeToAllTypes(type) : d->mAllTypes;
        if(s->getType() != type) { s->setType(type);}
        if(0 != s->mTypeName.compare(d->mTypeName)) { s->mTypeName = d->mTypeName;}
        if(0 != s->mSubTypeName.compare(d->mSubTypeName)) {s->mSubTypeName = d->mSubTypeName;}
        if(s->mAvailable != d->mAvailable) {s->mAvailable = d->mAvailable;}
        if(s->mConnected != d->mConnected) {s->mConnected = d->mConnected;}
        if(s->mRoaming != d->mRoaming) {s->mRoaming = d->mRoaming;}

        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ServiceStatusDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(ServiceStatusDataItem,SERVICESTATUS_DATA_ITEM_ID);
        if(s->mServiceState == d->mServiceState) { result = 0; break; }
        s->mServiceState = d->mServiceState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG("%d",result);
    return result;
}
DATA_ITEM_COPY(ModelDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(ModelDataItem,MODEL_DATA_ITEM_ID);
        if(0 == s->mModel.compare(d->mModel)) { result = 0; break; }
        s->mModel = d->mModel;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_COPY(ManufacturerDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(ManufacturerDataItem,MANUFACTURER_DATA_ITEM_ID);
        if(0 == s->mManufacturer.compare(d->mManufacturer)) { result = 0; break; }
        s->mManufacturer = d->mManufacturer;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}
DATA_ITEM_COPY(RilServiceInfoDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(RilServiceInfoDataItem,RILSERVICEINFO_DATA_ITEM_ID);
        if(*s == *d) {
            result =0;
            break;
        }
        s->mServiceInfo->valid_mask = d->mServiceInfo->valid_mask;
        s->mServiceInfo->airIf_type = d->mServiceInfo->airIf_type;
        s->mServiceInfo->carrierAirIf_type = d->mServiceInfo->carrierAirIf_type;
        s->mServiceInfo->carrierMcc = d->mServiceInfo->carrierMcc;
        s->mServiceInfo->carrierMnc = d->mServiceInfo->carrierMnc;
        s->mServiceInfo->carrierNameLen = d->mServiceInfo->carrierNameLen;
        strlcpy(s->mServiceInfo->carrierName, d->mServiceInfo->carrierName, LOC_RIL_CARRIER_NAME_MAX_LEN);
        s->mData = s->mServiceInfo;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
DATA_ITEM_COPY(RilCellInfoDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(RilCellInfoDataItem,RILCELLINFO_DATA_ITEM_ID);
        if(*s == *d) {
            result =0;
            break;
        }
        memcpy(static_cast<void*>(s->mCellInfo), static_cast<void *>(d->mCellInfo), sizeof(LOC_RilCellInfo));
        s->mData = s->mCellInfo;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

DATA_ITEM_COPY(WifiSupplicantStatusDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(WifiSupplicantStatusDataItem, WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID);
        if( (s->mState == d->mState) &&
            (s->mApMacAddressValid == d->mApMacAddressValid) &&
            (s->mWifiApSsidValid == d->mWifiApSsidValid)) {

            // compare mac address
            if (memcmp(s->mApMacAddress, d->mApMacAddress, sizeof(s->mApMacAddress)) == 0) {

                // compare ssid
                if (s->mWifiApSsid.compare(d->mWifiApSsid) == 0) {
                    result = 0;
                    break;
                }
            }
        }

        if (s->mState != d->mState) { s->mState = d->mState;}
        if (s->mApMacAddressValid != d->mApMacAddressValid) {s->mApMacAddressValid = d->mApMacAddressValid;}
        if (s->mWifiApSsidValid != d->mWifiApSsidValid) {s->mWifiApSsidValid = d->mWifiApSsidValid;}
        if (memcmp(s->mApMacAddress, d->mApMacAddress, sizeof(s->mApMacAddress)) != 0) {
            memcpy(static_cast<void*>(s->mApMacAddress), static_cast<void *>(d->mApMacAddress), sizeof(s->mApMacAddress));
        }
        if (s->mWifiApSsid.compare(d->mWifiApSsid) != 0) {
            s->mWifiApSsid = d->mWifiApSsid;
        }

        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

DATA_ITEM_COPY(TacDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(TacDataItem,TAC_DATA_ITEM_ID);
        if(0 == s->mValue.compare(d->mValue)) { result = 0; break; }
        s->mValue = d->mValue;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}

DATA_ITEM_COPY(MccmncDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(MccmncDataItem,MCCMNC_DATA_ITEM_ID);
        if(0 == s->mValue.compare(d->mValue)) { result = 0; break; }
        s->mValue= d->mValue;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}

DATA_ITEM_COPY(BtLeDeviceScanDetailsDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(BtLeDeviceScanDetailsDataItem,BTLE_SCAN_DATA_ITEM_ID);

        if (s->mValidSrnData != d->mValidSrnData) { s->mValidSrnData = d->mValidSrnData;}
        if (s->mApSrnRssi != d->mApSrnRssi) { s->mApSrnRssi = d->mApSrnRssi;}
        if (memcmp(s->mApSrnMacAddress, d->mApSrnMacAddress, sizeof(s->mApSrnMacAddress)) != 0) {
            memcpy(static_cast<void*>(s->mApSrnMacAddress), static_cast<void *>(d->mApSrnMacAddress), sizeof(s->mApSrnMacAddress));
        }
        if (s->mApSrnTimestamp != d->mApSrnTimestamp) {s->mApSrnTimestamp = d->mApSrnTimestamp;}
        if (s->mRequestTimestamp != d->mRequestTimestamp) {s->mRequestTimestamp = d->mRequestTimestamp;}
        if (s->mReceiveTimestamp != d->mReceiveTimestamp) {s->mReceiveTimestamp = d->mReceiveTimestamp;}
        if (s->mErrorCause != d->mErrorCause) {s->mErrorCause = d->mErrorCause;}
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}

DATA_ITEM_COPY(BtDeviceScanDetailsDataItem) {
    int32_t result = -1;
    ENTRY_LOG();
    do {
        COPIER_ERROR_CHECK_AND_DOWN_CAST(BtDeviceScanDetailsDataItem,BT_SCAN_DATA_ITEM_ID);

        if (s->mValidSrnData != d->mValidSrnData) { s->mValidSrnData = d->mValidSrnData;}
        if (s->mApSrnRssi != d->mApSrnRssi) { s->mApSrnRssi = d->mApSrnRssi;}
        if (memcmp(s->mApSrnMacAddress, d->mApSrnMacAddress, sizeof(s->mApSrnMacAddress)) != 0) {
            memcpy(static_cast<void*>(s->mApSrnMacAddress), static_cast<void *>(d->mApSrnMacAddress), sizeof(s->mApSrnMacAddress));
        }
        if (s->mApSrnTimestamp != d->mApSrnTimestamp) {s->mApSrnTimestamp = d->mApSrnTimestamp;}
        if (s->mRequestTimestamp != d->mRequestTimestamp) {s->mRequestTimestamp = d->mRequestTimestamp;}
        if (s->mReceiveTimestamp != d->mReceiveTimestamp) {s->mReceiveTimestamp = d->mReceiveTimestamp;}
        if (s->mErrorCause != d->mErrorCause) {s->mErrorCause = d->mErrorCause;}
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}

// serialize
DATA_ITEM_SERIALIZE(AirplaneModeDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(AirplaneModeDataItem, AIRPLANEMODE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(AIRPLANEMODE_FIELD_MODE, d->mMode), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(ENHDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ENHDataItem, ENH_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(ENH_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(GPSStateDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(GPSStateDataItem, GPSSTATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(GPSSTATE_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(NLPStatusDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(NLPStatusDataItem, NLPSTATUS_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(NLPSTATUS_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(WifiHardwareStateDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(WifiHardwareStateDataItem, WIFIHARDWARESTATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(WIFIHARDWARESTATE_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(ScreenStateDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ScreenStateDataItem, SCREEN_STATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(SCREENSTATE_FIELD_ENABLED, d->mState), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(PowerConnectStateDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(PowerConnectStateDataItem, POWER_CONNECTED_STATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(POWERCONNECTSTATE_FIELD_ENABLED, d->mState), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(BatteryLevelDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(BatteryLevelDataItem, BATTERY_LEVEL_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addUInt8(BATTERYLEVEL_FIELD_BATTERY_PCT, d->mBatteryPct), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(TimeZoneChangeDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(TimeZoneChangeDataItem, TIMEZONE_CHANGE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(TIMEZONECHANGE_FIELD_ENABLED, true), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addInt64(TIMECHANGE_FIELD_CURRENT_TIME_MILLIS, d->mCurrTimeMillis), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addInt64(TIMECHANGE_FIELD_RAW_OFFSET_TZ, d->mRawOffsetTZ), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addInt64(TIMECHANGE_FIELD_DST_OFFSET_TZ, d->mDstOffsetTZ), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(TimeChangeDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(TimeChangeDataItem, TIME_CHANGE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(TIMECHANGE_FIELD_ENABLED, true), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addInt64(TIMECHANGE_FIELD_CURRENT_TIME_MILLIS, d->mCurrTimeMillis), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addInt64(TIMECHANGE_FIELD_RAW_OFFSET_TZ, d->mRawOffsetTZ), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addInt64(TIMECHANGE_FIELD_DST_OFFSET_TZ, d->mDstOffsetTZ), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(ShutdownStateDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ShutdownStateDataItem, SHUTDOWN_STATE_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(SHUTDOWN_FIELD_ENABLED, d->mState), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(AssistedGpsDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(AssistedGpsDataItem, ASSISTED_GPS_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(ASSISTEDGPS_FIELD_ENABLED, d->mEnabled), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

DATA_ITEM_SERIALIZE(NetworkInfoDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        uint32_t type = (mAllTypes & (1<<TYPE_WIFI)) ? 100 : 201;
        CREATE_INNER_CARD(NetworkInfoDataItem, NETWORKINFO_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                6, oc->addBool(NETWORKINFO_FIELD_CONNECTED, d->mConnected), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addInt32(NETWORKINFO_FIELD_TYPE, type), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addString(NETWORKINFO_FIELD_TYPENAME, d->mTypeName.c_str()), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addString(NETWORKINFO_FIELD_SUBTYPENAME, d->mSubTypeName.c_str()), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                10, card->addBool(NETWORKINFO_FIELD_AVAILABLE, d->mAvailable), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                11, card->addBool(NETWORKINFO_FIELD_CONNECTED, d->mConnected), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                12, card->addBool(NETWORKINFO_FIELD_ROAMING, d->mRoaming), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(ServiceStatusDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ServiceStatusDataItem, SERVICESTATUS_DATA_ITEM_ID);
        if (d->mServiceState == LOC_NW_ROAMING) {
            BREAK_AND_DELETE_IF_NON_ZERO(
                    5, card->addString(SERVICESTATUS_FIELD_STATE, "ROAMING"), ic);
        } else if (d->mServiceState == LOC_NW_HOME) {
            BREAK_AND_DELETE_IF_NON_ZERO(
                    6, card->addString(SERVICESTATUS_FIELD_STATE, "HOME"), ic);
        } else {
            BREAK_AND_DELETE_IF_NON_ZERO(
                    7, card->addString(SERVICESTATUS_FIELD_STATE, "OOO"), ic);
        }
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(ModelDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ModelDataItem, MODEL_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addString(MODEL_FIELD_NAME, d->mModel.c_str()), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(ManufacturerDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(ManufacturerDataItem, MANUFACTURER_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addString(MANUFACTURER_FIELD_NAME, d->mManufacturer.c_str()), ic);
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(RilServiceInfoDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(RilServiceInfoDataItem, RILSERVICEINFO_DATA_ITEM_ID);
        // check if the data we received is valid
        if(d->mServiceInfo->valid_mask) {
            if((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_AIR_IF_TYPE) ==
                LOC_RIL_SERVICE_INFO_HAS_AIR_IF_TYPE) {
                BREAK_AND_DELETE_IF_NON_ZERO(
                        6, card->addUInt32(
                                RILSERVICEINFO_FIELD_ARIF_TYPE_MASK, d->mServiceInfo->airIf_type), ic);
            }
            if((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE) ==
                LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE) {
                if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_GSM) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            7, card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "GSM"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_CDMA) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            8, card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "CDMA"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_WCDMA) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            9, card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "WCDMA"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_LTE) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            10, card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "LTE"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_EVDO) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            11, card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "EVDO"), ic);
                } else if (d->mServiceInfo->carrierAirIf_type == LOC_RILAIRIF_WIFI) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            12, card->addString(RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE, "WIFI"), ic);
                }
            }
            if((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC) ==
                LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC) {
                BREAK_AND_DELETE_IF_NON_ZERO(
                        13, card->addUInt16(RILSERVICEINFO_FIELD_CARRIER_MCC, d->mServiceInfo->carrierMcc), ic);
            }
            if((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC) ==
                LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC) {
                BREAK_AND_DELETE_IF_NON_ZERO(
                        14, card->addUInt16(RILSERVICEINFO_FIELD_CARRIER_MNC, d->mServiceInfo->carrierMnc), ic);
            }
            if((d->mServiceInfo->valid_mask & LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME) ==
                LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME) {
                BREAK_AND_DELETE_IF_NON_ZERO(
                        15, card->addString(RILSERVICEINFO_FIELD_CARRIER_NAME, d->mServiceInfo->carrierName), ic);
            }
        }
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
DATA_ITEM_SERIALIZE(RilCellInfoDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(RilCellInfoDataItem, RILCELLINFO_DATA_ITEM_ID);
        // check if the data we received is valid
        if(d->mCellInfo->valid_mask) {
            if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_NW_STATUS) ==
                LOC_RIL_CELLINFO_HAS_NW_STATUS) {
                if (d->mCellInfo->nwStatus == LOC_NW_ROAMING) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            6, card->addString(RILCELLINFO_FIELD_NETWORK_STATUS, "ROAMING"), ic);
                } else if (d->mCellInfo->nwStatus == LOC_NW_HOME) {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            7, card->addString(RILCELLINFO_FIELD_NETWORK_STATUS, "HOME"), ic);
                } else {
                    BREAK_AND_DELETE_IF_NON_ZERO(
                            8, card->addString(RILCELLINFO_FIELD_NETWORK_STATUS, "OOO"), ic);
                }
            }
            if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_CELL_INFO) ==
                LOC_RIL_CELLINFO_HAS_CELL_INFO) {
                if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_TECH_TYPE) ==
                    LOC_RIL_CELLINFO_HAS_TECH_TYPE) {
                    switch(d->mCellInfo->rtType) {
                        case LOC_RIL_TECH_CDMA:
                        BREAK_AND_DELETE_IF_NON_ZERO(
                                9, card->addString(RILCELLINFO_FIELD_RIL_TECH_TYPE, "CDMA"), ic);
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_MCC) == LOC_RIL_TECH_CDMA_HAS_MCC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    10, card->addUInt16(RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.cdmaCinfo.mcc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_SID) == LOC_RIL_TECH_CDMA_HAS_SID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    11, card->addUInt16(RILLCELLINFO_FIELD_SID, d->mCellInfo->u.cdmaCinfo.sid), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_NID) == LOC_RIL_TECH_CDMA_HAS_NID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    12, card->addUInt16(RILLCELLINFO_FIELD_NID, d->mCellInfo->u.cdmaCinfo.nid), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSID) == LOC_RIL_TECH_CDMA_HAS_BSID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    13, card->addUInt32(RILLCELLINFO_FIELD_BSID, d->mCellInfo->u.cdmaCinfo.bsid), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLAT) == LOC_RIL_TECH_CDMA_HAS_BSLAT) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    14, card->addInt32(RILLCELLINFO_FIELD_BSLAT,d->mCellInfo->u.cdmaCinfo.bslat), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLONG) == LOC_RIL_TECH_CDMA_HAS_BSLONG) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    15, card->addInt32(RILLCELLINFO_FIELD_BSLON, d->mCellInfo->u.cdmaCinfo.bslon), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_TIMEZONE) == LOC_RIL_TECH_CDMA_HAS_TIMEZONE) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    16, card->addInt32(RILLCELLINFO_FIELD_UTC_TIME_OFFSET,
                                            d->mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) == LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    17, card->addBool(RILLCELLINFO_FIELD_DAYLIGHT_TIMEZONE,
                                            d->mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings), ic);
                        }
                        break;
                        case LOC_RIL_TECH_GSM:
                        BREAK_AND_DELETE_IF_NON_ZERO(
                                18, card->addString(RILCELLINFO_FIELD_RIL_TECH_TYPE, "GSM"), ic);
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) == LOC_RIL_TECH_GW_HAS_MCC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    19, card->addUInt16(RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.gsmCinfo.mcc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) == LOC_RIL_TECH_GW_HAS_MNC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    20, card->addUInt16(RILLCELLINFO_FIELD_MNC, d->mCellInfo->u.gsmCinfo.mnc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) == LOC_RIL_TECH_GW_HAS_LAC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    21, card->addUInt16(RILLCELLINFO_FIELD_LAC, d->mCellInfo->u.gsmCinfo.lac), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) == LOC_RIL_TECH_GW_HAS_CID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    22, card->addUInt32(RILLCELLINFO_FIELD_CID, d->mCellInfo->u.gsmCinfo.cid), ic);
                        }
                        break;
                        case LOC_RIL_TECH_WCDMA:
                        BREAK_AND_DELETE_IF_NON_ZERO(
                                23, card->addString(RILCELLINFO_FIELD_RIL_TECH_TYPE, "WCDMA"), ic);
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) == LOC_RIL_TECH_GW_HAS_MCC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    24, card->addUInt16(RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.wcdmaCinfo.mcc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) == LOC_RIL_TECH_GW_HAS_MNC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    25, card->addUInt16(RILLCELLINFO_FIELD_MNC, d->mCellInfo->u.wcdmaCinfo.mnc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) == LOC_RIL_TECH_GW_HAS_LAC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    26, card->addUInt16(RILLCELLINFO_FIELD_LAC, d->mCellInfo->u.wcdmaCinfo.lac), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) == LOC_RIL_TECH_GW_HAS_CID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    27, card->addUInt32(RILLCELLINFO_FIELD_CID, d->mCellInfo->u.wcdmaCinfo.cid), ic);
                        }
                        break;
                        case LOC_RIL_TECH_LTE:
                        BREAK_AND_DELETE_IF_NON_ZERO(
                                28, card->addString(RILCELLINFO_FIELD_RIL_TECH_TYPE, "LTE"), ic);
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MCC) == LOC_RIL_TECH_LTE_HAS_MCC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    29, card->addUInt16(RILLCELLINFO_FIELD_MCC, d->mCellInfo->u.lteCinfo.mcc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MNC) == LOC_RIL_TECH_LTE_HAS_MNC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    30, card->addUInt16(RILLCELLINFO_FIELD_MNC, d->mCellInfo->u.lteCinfo.mnc), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_TAC) == LOC_RIL_TECH_LTE_HAS_TAC) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    31, card->addUInt16(RILLCELLINFO_FIELD_TAC, d->mCellInfo->u.lteCinfo.tac), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_PCI) == LOC_RIL_TECH_LTE_HAS_PCI) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    32, card->addUInt16(RILLCELLINFO_FIELD_PCI, d->mCellInfo->u.lteCinfo.pci), ic);
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_CID) == LOC_RIL_TECH_LTE_HAS_CID) {
                            BREAK_AND_DELETE_IF_NON_ZERO(
                                    33, card->addUInt32(RILLCELLINFO_FIELD_CID, d->mCellInfo->u.lteCinfo.cid), ic);
                        }
                        break;
                        case LOC_RIL_TECH_TD_SCDMA:
                        case LOC_RIL_TECH_MAX:
                        default:
                        break;
                    }; // end switch
                }
            }
        }
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

DATA_ITEM_SERIALIZE(WifiSupplicantStatusDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(WifiSupplicantStatusDataItem, WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID);
        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addInt32(WIFI_SUPPLICANT_FIELD_STATE, d->mState), ic);
        // TODO: serialize remaining fields if required
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(TacDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(TacDataItem, TAC_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addString(TAC_FIELD_NAME, d->mValue.c_str()));
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(MccmncDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(MccmncDataItem, MCCMNC_DATA_ITEM_ID);
        BREAK_IF_NON_ZERO(5, card->addString(MCCMNC_FIELD_NAME, d->mValue.c_str()));
        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(BtLeDeviceScanDetailsDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(BtLeDeviceScanDetailsDataItem, BTLE_SCAN_DATA_ITEM_ID);

        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(BTLESCANDETAILS_FIELD_VALID, d->mValidSrnData), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                6, card->addInt32(BTLESCANDETAILS_FIELD_RSSI, d->mApSrnRssi), ic);
        const void* blob = (const void*) d->mApSrnMacAddress;
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addBlob(BTLESCANDETAILS_FIELD_MAC, blob, SRN_MAC_ADDRESS_LENGTH), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addInt64(BTLESCANDETAILS_FIELD_SCANREQ, d->mApSrnTimestamp), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addInt64(BTLESCANDETAILS_FIELD_SCANSTART, d->mRequestTimestamp), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                10, card->addInt64(BTLESCANDETAILS_FIELD_SCANRECV, d->mReceiveTimestamp), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                11, card->addInt32(BTLESCANDETAILS_FIELD_SCANERROR, d->mErrorCause), ic);

        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_SERIALIZE(BtDeviceScanDetailsDataItem) {
    int32 result = 0;
    OutPostcard * card = oc;
    OutPostcard * ic = NULL;
    ENTRY_LOG();
    do {
        CREATE_INNER_CARD(BtDeviceScanDetailsDataItem, BT_SCAN_DATA_ITEM_ID);

        BREAK_AND_DELETE_IF_NON_ZERO(
                5, card->addBool(BTSCANDETAILS_FIELD_VALID, d->mValidSrnData), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                6, card->addInt32(BTSCANDETAILS_FIELD_RSSI, d->mApSrnRssi), ic);
        const void* blob = (const void*) d->mApSrnMacAddress;
        BREAK_AND_DELETE_IF_NON_ZERO(
                7, card->addBlob(BTSCANDETAILS_FIELD_MAC, blob, SRN_MAC_ADDRESS_LENGTH), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                8, card->addInt64(BTSCANDETAILS_FIELD_SCANREQ, d->mApSrnTimestamp), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                9, card->addInt64(BTSCANDETAILS_FIELD_SCANSTART, d->mRequestTimestamp), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                10, card->addInt64(BTSCANDETAILS_FIELD_SCANRECV, d->mReceiveTimestamp), ic);
        BREAK_AND_DELETE_IF_NON_ZERO(
                11, card->addInt32(BTSCANDETAILS_FIELD_SCANERROR, d->mErrorCause), ic);

        ADD_INNER_CARD();
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

// deSerialize TODO
DATA_ITEM_DESERIALIZE(AirplaneModeDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ENHDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(GPSStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(NLPStatusDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(WifiHardwareStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ScreenStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(PowerConnectStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(BatteryLevelDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(TimeZoneChangeDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(TimeChangeDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ShutdownStateDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(AssistedGpsDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(NetworkInfoDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ServiceStatusDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ModelDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ManufacturerDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(RilCellInfoDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(RilServiceInfoDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(WifiSupplicantStatusDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(TacDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(MccmncDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(BtLeDeviceScanDetailsDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(BtDeviceScanDetailsDataItem) {
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}

