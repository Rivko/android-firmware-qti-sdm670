/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemsFactory Implementation

  Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <IDataItemCore.h>
#include <DataItemsFactory.h>
#include <DataItemConcreteTypes.h>

using namespace izat_manager;

// Macro to create data item and return the object of type requested.
// In case of cloneDataItem, we also need to copy from DI object passed.
#define CREATE_DI_AND_CAST(DICLASS) \
{ \
    DICLASS *dataItem = new DICLASS(); \
    if (nullptr != dataItem) { \
        if (nullptr != di) { \
            dataItem->copy (di); \
        } \
        diItemReturn = static_cast<T *> (dataItem); \
    } \
} \

template<class T>
T* DataItemsFactory::createDataItem(DataItemId diId, IDataItemCore * di) {
    T* diItemReturn = nullptr;
    DataItemId id = diId;
    if (nullptr != di) {
        // we are cloning DataItem
        id = di->getId();
    }
    switch (id) {
    case AIRPLANEMODE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(AirplaneModeDataItem);
        break;
    case ENH_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(ENHDataItem);
        break;
    case GPSSTATE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(GPSStateDataItem);
        break;
    case NLPSTATUS_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(NLPStatusDataItem);
        break;
    case WIFIHARDWARESTATE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(WifiHardwareStateDataItem);
        break;
    case NETWORKINFO_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(NetworkInfoDataItem);
        break;
    case SERVICESTATUS_DATA_ITEM_ID:
       CREATE_DI_AND_CAST(ServiceStatusDataItem);
        break;
    case RILCELLINFO_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(RilCellInfoDataItem);
        break;
    case RILSERVICEINFO_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(RilServiceInfoDataItem);
        break;
    case MODEL_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(ModelDataItem);
        break;
    case MANUFACTURER_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(ManufacturerDataItem);
        break;
    case ASSISTED_GPS_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(AssistedGpsDataItem);
        break;
    case SCREEN_STATE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(ScreenStateDataItem);
        break;
    case POWER_CONNECTED_STATE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(PowerConnectStateDataItem);
        break;
    case TIMEZONE_CHANGE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(TimeZoneChangeDataItem);
        break;
    case TIME_CHANGE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(TimeChangeDataItem);
        break;
    case WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(WifiSupplicantStatusDataItem);
        break;
    case SHUTDOWN_STATE_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(ShutdownStateDataItem);
        break;
    case TAC_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(TacDataItem);
        break;
    case MCCMNC_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(MccmncDataItem);
        break;
    case BTLE_SCAN_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(BtLeDeviceScanDetailsDataItem);
        break;
    case BT_SCAN_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(BtDeviceScanDetailsDataItem);
        break;
    case BATTERY_LEVEL_DATA_ITEM_ID:
        CREATE_DI_AND_CAST(BatteryLevelDataItem);
        break;
    case INVALID_DATA_ITEM_ID:
    case MAX_DATA_ITEM_ID:
    default:
        break;
    };
    return diItemReturn;
}


//Expose function to invoke via dlsym
extern "C" IDataItemCore* getConcreteDataItem(DataItemId id)
{
    return DataItemsFactory::createNewDataItem(id);
}

IDataItemCore* DataItemsFactory::createNewDataItem(DataItemId id) {
    return createDataItem<IDataItemCore>(id, nullptr);
}

IDataItemSerialization * DataItemsFactory::cloneDataItem(IDataItemCore * di) {
    IDataItemSerialization *diSerializer = nullptr;
    if (nullptr != di) {
        diSerializer =
            createDataItem<IDataItemSerialization>(di->getId(), di);
    }
    return diSerializer;
}
