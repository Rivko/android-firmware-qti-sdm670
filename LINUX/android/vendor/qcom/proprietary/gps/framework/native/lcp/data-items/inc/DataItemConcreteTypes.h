/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemConcreteTypes

  Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMCONCRETETYPES_H__
#define __IZAT_MANAGER_DATAITEMCONCRETETYPES_H__

#include <comdef.h>
#include <string>
#include <IDataItemSerializer.h>
#include <DataItemConcreteTypeDefaultValues.h>
#include <DataItemConcreteTypeFieldNames.h>
#include <DataItemConcreteTypesBase.h>

namespace qc_loc_fw {
    class InPostcard;
    class OutPostcard;
}
using loc_core::IDataItemCore;

struct LOC_RilServiceInfo;
struct LOC_RilCellInfo;

// Add Concrete DataItem definitions here.
namespace izat_manager
{
using namespace std;
using namespace loc_core;
using namespace ::qc_loc_fw;

class AirplaneModeDataItem : public IDataItemSerialization,
                             public AirplaneModeDataItemBase {
public:
    AirplaneModeDataItem(bool mode = AIRPLANEMODE_DEFAULT_MODE);
    virtual ~AirplaneModeDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class ENHDataItem : public IDataItemSerialization,
                    public ENHDataItemBase {
public:
    ENHDataItem(bool enabled = ENH_DEFAULT_ENABLED);
    virtual ~ENHDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class GPSStateDataItem : public IDataItemSerialization,
                         public GPSStateDataItemBase {
public:
    GPSStateDataItem(bool enabled = GPSSTATE_DEFAULT_ENABLED);
    virtual ~GPSStateDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class NLPStatusDataItem : public IDataItemSerialization,
                          public NLPStatusDataItemBase {
public:
    NLPStatusDataItem(bool enabled = NLPSTATUS_DEFAULT_ENABLED);
    virtual ~NLPStatusDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class WifiHardwareStateDataItem : public IDataItemSerialization,
                                  public WifiHardwareStateDataItemBase {
public:
    WifiHardwareStateDataItem(bool enabled = WIFIHARDWARESTATE_DEFAULT_ENABLED);
    virtual ~WifiHardwareStateDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class ScreenStateDataItem : public IDataItemSerialization,
                            public ScreenStateDataItemBase {
public:
    ScreenStateDataItem(bool enabled = SCREEN_STATE_DEFAULT_ENABLED);
    virtual ~ScreenStateDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class PowerConnectStateDataItem : public IDataItemSerialization,
                                  public PowerConnectStateDataItemBase {
public:
    PowerConnectStateDataItem(bool enabled = POWER_CONNECT_STATE_DEFAULT_ENABLED);
    virtual ~PowerConnectStateDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class TimeZoneChangeDataItem : public IDataItemSerialization,
                               public TimeZoneChangeDataItemBase {
public:
    TimeZoneChangeDataItem(
        int64_t currTimeMillis = TIME_DEFAULT_CURRTIME,
        int32_t rawOffset = TIMEZONE_DEFAULT_RAWOFFSET,
        int32_t dstOffset = TIMEZONE_DEFAULT_DSTOFFSET
    );
    virtual ~TimeZoneChangeDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class TimeChangeDataItem : public IDataItemSerialization,
                           public TimeChangeDataItemBase {
public:
    TimeChangeDataItem(
        int64_t currTimeMillis = TIME_DEFAULT_CURRTIME,
        int32_t rawOffset = TIMEZONE_DEFAULT_RAWOFFSET,
        int32_t dstOffset = TIMEZONE_DEFAULT_DSTOFFSET
    );
    virtual ~TimeChangeDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class ShutdownStateDataItem : public IDataItemSerialization,
                              public ShutdownStateDataItemBase {
public:
    ShutdownStateDataItem(bool state = SHUTDOWN_DEFAULT_STATE);
    virtual ~ShutdownStateDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class AssistedGpsDataItem : public IDataItemSerialization,
                            public AssistedGpsDataItemBase {
public:
    AssistedGpsDataItem(bool enabled = ASSISTED_GPS_DEFAULT_ENABLED);
    virtual ~AssistedGpsDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class NetworkInfoDataItem : public IDataItemSerialization,
                            public NetworkInfoDataItemBase {
public:
    NetworkInfoDataItem(
        int32_t type = NETWORKINFO_DEFAULT_TYPE,
        std::string typeName = NETWORKINFO_DEFAULT_TYPENAME,
        std::string subTypeName = NETWORKINFO_DEFAULT_SUBTYPENAME,
        bool available = NETWORKINFO_DEFAULT_AVAILABLE,
        bool connected = NETWORKINFO_DEFAULT_CONNECTED,
        bool roaming = NETWORKINFO_DEFAULT_ROAMING
    );
    virtual ~NetworkInfoDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
    inline virtual NetworkType getType(void) const override {
        return getNormalizedType(mType);
    }
    inline static NetworkType getNormalizedType(int32_t type) {
        NetworkType typeout = TYPE_UNKNOWN;
        switch (type) {
            case 100:
                typeout = TYPE_WIFI;
                break;
            case 101:
                typeout = TYPE_ETHERNET;
                break;
            case 102:
                typeout = TYPE_BLUETOOTH;
                break;
            case 201:
                typeout = TYPE_MOBILE;
                break;
            case 202:
                typeout = TYPE_DUN;
                break;
            case 203:
                typeout = TYPE_HIPRI;
                break;
            case 204:
                typeout = TYPE_MMS;
                break;
            case 205:
                typeout = TYPE_SUPL;
                break;
            case 220:
                typeout = TYPE_WIMAX;
                break;
            case 300:
            default:
                typeout = TYPE_UNKNOWN;
                break;
        }
        return typeout;
    }
protected:
    string mCardName;
private:
    inline void setType(NetworkType type) {
        switch (type) {
            case TYPE_WIFI:
                mType = 100;
                break;
            case TYPE_ETHERNET:
                mType = 101;
                break;
            case TYPE_BLUETOOTH:
                mType = 102;
                break;
            case TYPE_MOBILE:
                mType = 201;
                break;
            case TYPE_DUN:
                mType = 202;
                break;
            case TYPE_HIPRI:
                mType = 203;
                break;
            case TYPE_MMS:
                mType = 204;
                break;
            case TYPE_SUPL:
                mType = 205;
                break;
            case TYPE_WIMAX:
                mType = 220;
                break;
            default:
                mType = 300;
                break;
        }
    }
};

class ServiceStatusDataItem : public IDataItemSerialization,
                              public ServiceStatusDataItemBase {
public:
    ServiceStatusDataItem(int32_t serviceState = SERVICESTATUS_DEFAULT_STATE);
    virtual ~ServiceStatusDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class ModelDataItem : public IDataItemSerialization,
                      public ModelDataItemBase {
public:
    ModelDataItem(const std::string & state = MODEL_DEFAULT_NAME);
    virtual ~ModelDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class ManufacturerDataItem : public IDataItemSerialization,
                             public ManufacturerDataItemBase {
public:
    ManufacturerDataItem(const std::string & state = MANUFACTURER_DEFAULT_NAME);
    virtual ~ManufacturerDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class RilServiceInfoDataItem : public IDataItemSerialization,
                               public RilServiceInfoDataItemBase {
public:
    RilServiceInfoDataItem(struct LOC_RilServiceInfo * serviceInfo = NULL);
    virtual ~RilServiceInfoDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
    virtual bool operator==(const RilServiceInfoDataItemBase& other) const override;
    virtual void setPeerData(RilServiceInfoDataItemBase& peer) const override;
// Data members
    struct LOC_RilServiceInfo * mServiceInfo;
protected:
    string mCardName;
private:
    bool mDestroy;
};

class RilCellInfoDataItem : public IDataItemSerialization,
                            public RilCellInfoDataItemBase {
public:
    RilCellInfoDataItem(struct LOC_RilCellInfo * cellInfo = NULL);
    virtual ~RilCellInfoDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
    virtual bool operator==(const RilCellInfoDataItemBase& other) const override;
    virtual void setPeerData(RilCellInfoDataItemBase& peer) const override;
// Data members
    struct LOC_RilCellInfo * mCellInfo;
protected:
    string mCardName;
private:
    bool mDestroy;
};

class WifiSupplicantStatusDataItem : public IDataItemSerialization,
                                     public WifiSupplicantStatusDataItemBase {
public:
    WifiSupplicantStatusDataItem();
    virtual ~WifiSupplicantStatusDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class TacDataItem : public IDataItemSerialization,
                    public TacDataItemBase {
public:
    TacDataItem(const std::string & state = TAC_DEFAULT_NAME);
    virtual ~TacDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class MccmncDataItem : public IDataItemSerialization,
                       public MccmncDataItemBase {
public:
    MccmncDataItem(const std::string & state = MCCMNC_DEFAULT_NAME);
    virtual ~MccmncDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class BtLeDeviceScanDetailsDataItem : public IDataItemSerialization,
                                      public BtLeDeviceScanDetailsDataItemBase {
public:
    BtLeDeviceScanDetailsDataItem();
    virtual ~BtLeDeviceScanDetailsDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class BtDeviceScanDetailsDataItem : public IDataItemSerialization,
                                    public BtDeviceScanDetailsDataItemBase {
public:
    BtDeviceScanDetailsDataItem();
    virtual ~BtDeviceScanDetailsDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

class BatteryLevelDataItem : public IDataItemSerialization,
                             public BatteryLevelDataItemBase {

public:
    BatteryLevelDataItem(uint8_t battPct = BATTERY_PCT_DEFAULT);
    virtual ~BatteryLevelDataItem();
    virtual void stringify (string & valueStr);
    virtual int32_t copy (IDataItemCore * src, bool *dataItemCopied = NULL);
    virtual int32 serialize (OutPostcard * out);
    virtual int32 deSerialize (InPostcard * in);
    inline virtual const string & getCardName () const {
        return mCardName;
    }
    inline virtual bool hasCardName () const {
        return !mCardName.empty ();
    }
protected:
    string mCardName;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMCONCRETETYPES_H__
