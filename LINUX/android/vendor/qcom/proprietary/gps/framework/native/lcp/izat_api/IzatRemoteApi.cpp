/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <stdint.h>
#include <IzatRemoteApi.h>
#include "IzatWiFiDBReceiver.h"
#include <mq_client/IPCMessagingProxy.h>
#include <gps_extended_c.h>
#include <IzatTypes.h>
#include <algorithm>
#include <vector>
#include <IzatDefines.h>
#include <loc_pla.h>
#include <loc_nmea.h>
#include <loc_gps.h>
#include <time.h>

using namespace std;
using namespace qc_loc_fw;
using namespace izat_manager;

#define POS_LOC_FLAGS                    "GPS-LOC-FLAGS"
#define POS_LATITUDE                     "LATITUDE"
#define POS_LONGITUDE                    "LONGITUDE"
#define POS_HORIZONTAL_ACCURACY          "HORIZONTAL-ACCURACY"
#define POS_ALTITUDE                     "ALTITUDE"
#define POS_UTC_TIME                     "UTC-TIME-STAMP"
#define POS_VERT_UNCERT                  "VERT-UNCERT"
#define POS_BEARING                      "BEARING"
#define POS_SPEED                        "SPEED"
#define POS_SOURCE                       "POSITION-SOURCE"

#define POS_LOC_EXT_FLAGS                "GPS-LOC-EXT-FLAGS"
#define POS_ALT_MEAN_SEA_LEVEL           "ALT-MEAN-SEA-LEVEL"
#define POS_PDOP                         "PDOP"
#define POS_HDOP                         "HDOP"
#define POS_VDOP                         "VDOP"
#define POS_MAGNETIC_DEVIATION           "MAGNETIC-DEVIATION"
#define POS_VERT_UNCERT                  "VERT-UNCERT"
#define POS_SPEED_UNCERT                 "SPEED-UNCERT"
#define POS_BEARING_UNCERT               "BEARING-UNCERT"
#define POS_HOR_RELIABILITY              "HOR-RELIABILITY"
#define POS_VERT_RELIABILITY             "VERT-RELIABILITY"
#define POS_HOR_ELIP_UNC_MAJOR           "HOR-ELIP-UNC-MAJOR"
#define POS_HOR_ELIP_UNC_MINOR           "HOR-ELIP-UNC-MINOR"
#define POS_HOR_ELIP_UNC_AZIMUTH         "HOR-ELIP-UNC-AZIMUTH"
#define NMEA_TAG                         "NMEA-STRING"

namespace izat_remote_api {

struct OutCard {
    const char* mTo;
    OutPostcard* const mCard;
    inline OutCard(const char* to) :
        mTo(to), mCard(OutPostcard::createInstance()) {}
    inline ~OutCard() { delete mCard; }
};

class IzatNotifierProxy : public IIPCMessagingResponse {
private:
    const char* const mName;
    IPCMessagingProxy* const mIPCProxy;
    vector<IzatNotifier*> mNotifiers;
    inline IzatNotifierProxy(const char* const name, IPCMessagingProxy* ipcProxy) :
        mName(name), mIPCProxy(ipcProxy), mNotifiers() {
    }
    inline ~IzatNotifierProxy() { }
public:
    static IzatNotifierProxy* get(const char* const name) {
        IPCMessagingProxy* ipcProxy = IPCMessagingProxy::getInstance();
        IzatNotifierProxy* notifierProxy = (IzatNotifierProxy*)
            ipcProxy->getResponseObj(name);
        if (notifierProxy == nullptr) {
            notifierProxy = new IzatNotifierProxy(name, ipcProxy);
            ipcProxy->registerResponseObj(name, notifierProxy);
        }
        return notifierProxy;
    }
    static void drop(IzatNotifierProxy* notifierProxy) {
        if (notifierProxy->mNotifiers.size() == 0) {
            notifierProxy->mIPCProxy->unregisterResponseObj(notifierProxy->mName);
            delete notifierProxy;
        }
    }
    inline void addNotifier(IzatNotifier* notifier, const OutCard* subCard) {
        mNotifiers.push_back(notifier);
        if (subCard && mNotifiers.size() == 1) {
            mIPCProxy->sendMsg(subCard->mCard, subCard->mTo);
        }
    }
    inline void removeNotifier(IzatNotifier* notifier) {
        vector<IzatNotifier*>::iterator it =
            find(mNotifiers.begin(), mNotifiers.end(), notifier);
        if (it != mNotifiers.end()) {
            mNotifiers.erase(it);
        }
    }
    inline virtual void handleMsg(InPostcard * const in_card) final {
         for (auto const& notifier : mNotifiers) {
              notifier->handleMsg(in_card);
         }
    }
    inline void sendCard(OutPostcard* card, bool deleteCard = true) {
        if (mIPCProxy && card) {
            mIPCProxy->sendIpcMessage(card->getEncodedBuffer());
            if (deleteCard) {
                delete card;
            }
        }
    }
};

IzatNotifier::IzatNotifier(const char* const name, const OutCard* subCard) :
    mNotifierProxy(IzatNotifierProxy::get(name)) {
}

IzatNotifier::~IzatNotifier() {
    mNotifierProxy->removeNotifier(this);
    IzatNotifierProxy::drop(mNotifierProxy);
}

// a static method outside LocationUpdater class.
static const char* sToTag = "TO";
static const char* sFromTag = "FROM";
static const char* sReqTag = "REQ";
static const char* sInfoTag = "INFO";

LocationUpdaterBase::LocationUpdaterBase(const char* const name,
                                         const OutCard* sSubscriptionCard,
                                         const remoteClientInfo *pClientInfo,
                                         const void* clientData) :
                                             IzatNotifier(name,
                                             sSubscriptionCard),
                                             mClientInfo(pClientInfo),
                                             mClientData(clientData),
                                             mName(name) {
    mNotifierProxy->addNotifier(this, sSubscriptionCard);
}

OutCard* LocationUpdaterBase::getLocSubscriptionCard(const char* name,
                                                     uint32_t streamType)
{

    const char* TO = "IZAT-MANAGER";
    bool intermediateFixAccepted = true;
    OutCard* card = new OutCard(TO);
    if (nullptr != card) {
        card->mCard->init();
        card->mCard->addString(sToTag, TO);
        card->mCard->addString(sFromTag, name);
        card->mCard->addString(sReqTag, "PASSIVE-LOCATION");
        card->mCard->addUInt16("LISTENS-TO", streamType);
        card->mCard->addBool("INTERMEDIATE-FIX-ACCEPTED", intermediateFixAccepted);
        card->mCard->finalize();
    }
    return card;
}

const char LocationUpdater::sName[] = "LOCATION-UPDATE";

const OutCard* LocationUpdater::sSubscriptionCard =
      LocationUpdaterBase::getLocSubscriptionCard(LocationUpdater::sName,
                                                  IZAT_STREAM_DR);

const char RawLocationUpdater::sName[] = "RAW-LOCATION-UPDATE";
const OutCard* RawLocationUpdater::sRawSubscriptionCard =
      LocationUpdaterBase::getLocSubscriptionCard(RawLocationUpdater::sName,
                                                  IZAT_STREAM_GNSS);

void LocationUpdaterBase::handleMsg(InPostcard * const in_msg) {
    const char* info = nullptr;
    const char sname[] = "LOCATION-UPDATE";
    if ((in_msg != nullptr) &&
        (0 == in_msg->getString(sInfoTag, &info)) &&
        (0 == strncmp(sname, info, sizeof(sname)-1)))
    {
        UlpLocation ulpLoc;
        GpsLocationExtended locExtended;
        memset(&ulpLoc, 0, sizeof(ulpLoc));
        memset(&locExtended, 0, sizeof(locExtended));
        const void* blob = NULL;
        size_t length = 0;
        in_msg->getBlob ("ULPLOC", &blob, &length);
        ulpLoc = *(UlpLocation *)blob;
        in_msg->getBlob ("LOCEXTENDED", &blob, &length);
        locExtended = *(GpsLocationExtended *)blob;

        if (mClientInfo) {
            //Send position info to registered clients
            mClientInfo->locCb(&ulpLoc, &locExtended, (void*)mClientData);
            //generate and send NMEA
            if(mClientInfo->nmeaCb){
                std::vector<std::string> nmeaArraystr;
                uint8_t generate_nmea = (ulpLoc.gpsLocation.flags & LOC_GPS_LOCATION_HAS_ACCURACY);
                loc_nmea_generate_pos(ulpLoc, locExtended, generate_nmea, nmeaArraystr);
                UlpNmea nmeaStr;

                for(size_t i = 0; i < nmeaArraystr.size(); i++) {
                    memset(&nmeaStr, 0, sizeof(nmeaStr));
                    nmeaStr.len = nmeaArraystr[i].length();
                    strlcpy(nmeaStr.nmea_str, nmeaArraystr[i].c_str(), sizeof(nmeaStr.nmea_str));
                    mClientInfo->nmeaCb(&nmeaStr, (void*)mClientData);
                }
            }
        }
    }
}

const char SstpUpdater::sName[] = "WAA-LISTENER";
const char* const SstpUpdater::sLatTag = "LATITUDE";
const char* const SstpUpdater::sLonTag = "LONGITUDE";
const char* const SstpUpdater::sUncTag = "HOR-UNC-M";
const char* const SstpUpdater::sUncConfTag = "HOR-UNC-CONFIDENCE";

SstpUpdater::SstpUpdater() : IzatNotifier(sName, nullptr) {
    mNotifierProxy->addNotifier(this, nullptr);
}

void SstpUpdater::stop() {
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "GTP-WAA");
            card->addString(sFromTag, SstpUpdater::sName);
            card->addString(sReqTag, "STOP");
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}

void SstpUpdater::handleMsg(InPostcard * const in_msg) {
    const char* info = nullptr;
    const char* status = nullptr;
    const char SUCCESS[] = "MCC-DETECTION-SUCCESS";
    const char INFO[] = "MCC-AND-SITE-INFO";

    if (nullptr != in_msg &&
        0 == in_msg->getString(sInfoTag, &info) &&
        0 == in_msg->getString("STATUS", &status) &&
        0 == strncmp(INFO, info, sizeof(INFO) - 1)) {
        if (0 == strncmp(status, SUCCESS, sizeof(SUCCESS) - 1)) {
            uint32_t mcc = 0;
            const char* confidence = nullptr;
            in_msg->getUInt32("MCC", mcc);
            in_msg->getString("MCC-CONFIDENCE", &confidence);

            mccUpdate(mcc, confidence);

            const char sstp[] = "SSTP";
            const char* source = nullptr;
            if (0 == in_msg->getString("SOURCE", &source) &&
                0 == strncmp(source, sstp, sizeof(sstp) - 1)) {
                const char* siteName = nullptr;
                double lat = 360.0, lon = 360.0;
                float unc = 0;
                int32_t uncConf = 0;
                in_msg->getString("SITE-NAME", &siteName);
                in_msg->getDouble("LATITUDE-DEG", lat);
                in_msg->getDouble("LONGITUDE-DEG", lon);
                in_msg->getFloat("HOR-UNC-M", unc);
                in_msg->getInt32("HOR-UNC-CONFIDENCE", uncConf);

                siteUpdate(siteName, lat, lon, unc, uncConf);
            }
        } else {
            errReport(status);
        }
    }
}

OutCard* SvInfoUpdaterBase::getSvInfoSubscriptionCard(const char* name,
                                                      uint32_t streamType)
{
    const char* TO = "IZAT-MANAGER";
    bool intermediateFixAccepted = true;
    OutCard* card = new OutCard(TO);
    if (nullptr != card) {
        card->mCard->init();
        card->mCard->addString(sToTag, TO);
        card->mCard->addString(sFromTag, name);
        card->mCard->addString(sReqTag, "PASSIVE-SVINFO");
        card->mCard->addUInt16("LISTENS-TO", streamType);
        card->mCard->finalize();
    }
    return card;
}

const char SvInfoUpdater::sName[] = "SVINFO-UPDATE";
const OutCard* SvInfoUpdater::sSubscriptionCard =
      SvInfoUpdaterBase::getSvInfoSubscriptionCard(SvInfoUpdater::sName,
                                                   IZAT_STREAM_DR_SVINFO);

SvInfoUpdaterBase::SvInfoUpdaterBase(const char* const name,
                                     const OutCard* sSubscriptionCard,
                                     remoteClientInfo *pClientInfo,
                                     const void* clientData) :
                                         IzatNotifier(name,
                                         sSubscriptionCard),
                                         mClientInfo(pClientInfo),
                                         mClientData(clientData),
                                         mName(name) {
    mNotifierProxy->addNotifier(this, sSubscriptionCard);
}

const char RawSvInfoUpdater::sName[] = "RAW-SVINFO-UPDATE";
const OutCard* RawSvInfoUpdater::sRawSubscriptionCard =
      SvInfoUpdaterBase::getSvInfoSubscriptionCard(RawSvInfoUpdater::sName,
                                                   IZAT_STREAM_GNSS_SVINFO);

void SvInfoUpdaterBase::handleMsg(InPostcard * const in_msg) {
    const char* info = nullptr;
    const char sname[] = "SVINFO-UPDATE";
    if ((in_msg != nullptr) &&
        (0 == in_msg->getString(sInfoTag, &info)) &&
        (0 == strncmp(sname, info, sizeof(sname)-1))){

        GnssSvNotification svNotify;
        memset(&svNotify, 0, sizeof(GnssSvNotification));
        const void* blob = NULL;
        size_t length = 0;
        in_msg->getBlob ("GNSSSVNOTIFICATION", &blob, &length);
        svNotify = *(GnssSvNotification *)blob;

        if (mClientInfo) {
            //Send SVInfo to remote clients
            mClientInfo->svReportCb(&svNotify, (void*)mClientData);
            //generate and send NMEA
            if(mClientInfo->nmeaCb){
                std::vector<std::string> nmeaArraystr;
                loc_nmea_generate_sv(svNotify, nmeaArraystr);
                UlpNmea nmeaStr;

                for(size_t i = 0; i < nmeaArraystr.size(); i++) {
                    memset(&nmeaStr, 0, sizeof(nmeaStr));
                    nmeaStr.len = nmeaArraystr[i].length();
                    strlcpy(nmeaStr.nmea_str, nmeaArraystr[i].c_str(), sizeof(nmeaStr.nmea_str));
                    mClientInfo->nmeaCb(&nmeaStr, (void*)mClientData);
                }
            }
        }
    }
}


const char WiFiDBUpdater::sName[] = "FDAL-UPDATER";
const char* const WiFiDBUpdater::sAPListTag = "AP-LIST";
const char* const WiFiDBUpdater::sAPLocationTag = "AP-LOC";
const char* const WiFiDBUpdater::sAPListCountTag = "AP-LIST-COUNT";
const char* const WiFiDBUpdater::sAPLocCountTag = "AP-LOC-COUNT";

WiFiDBUpdater::WiFiDBUpdater() : IzatNotifier(sName, nullptr) {
    mNotifierProxy->addNotifier(this, nullptr);
}

void WiFiDBUpdater::sendAPListReq(int expire_in_days) {
    const char EXPIRE_IN_DAYS[] = "EXPIRE-IN-DAYS";
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "GTP-FDAL");
            card->addString(sFromTag, WiFiDBUpdater::sName);
            card->addString(sReqTag, WiFiDBUpdater::sAPListTag);
            card->addInt32(EXPIRE_IN_DAYS, expire_in_days);
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}

void WiFiDBUpdater::pushWiFiDB(vector<APLocationData>* l_data_ptr,
                               vector<APSpecialInfo>* s_data_ptr,
                               int days_valid) {
    const char DAYS_VALID[] = "DAYS-VALID";
    if (mNotifierProxy) {
        OutPostcard* card = OutPostcard::createInstance();
        if (nullptr != card) {
            card->init();
            card->addString(sToTag, "GTP-FDAL");
            card->addString(sFromTag, WiFiDBUpdater::sName);
            card->addString(sReqTag, WiFiDBUpdater::sAPLocationTag);
            for (vector<APLocationData>::iterator it = l_data_ptr->begin();
                 it != l_data_ptr->end(); ++it) {
                APLocationData* ap_loc = &(*it);
                OutPostcard* locCard = OutPostcard::createInstance();
                locCard->init();
                locCard->addUInt64("MAC-ADDRESS", ap_loc->mac_R48b);
                locCard->addFloat("LATITUDE", ap_loc->latitude);
                locCard->addFloat("LONGITUDE", ap_loc->longitude);
                if (0 != (ap_loc->valid_mask & APLocationData::WIFIDBUPDATER_APLOC_MAR_VALID)) {
                    locCard->addFloat("MAX-ANTENA-RANGE", ap_loc->max_antena_range);
                }
                if (0 != (ap_loc->valid_mask &
                          APLocationData::WIFIDBUPDATER_APLOC_HORIZONTAL_ERR_VALID)) {
                    locCard->addFloat("HORIZONTAL_ERROR", ap_loc->horizontal_error);
                }
                if (0 != (ap_loc->valid_mask &
                          APLocationData::WIFIDBUPDATER_APLOC_RELIABILITY_VALID)) {
                    locCard->addUInt8("RELIABILITY", ap_loc->reliability);
                }
                locCard->finalize();
                card->addCard(WiFiDBUpdater::sAPLocationTag, locCard);
                delete locCard;
            }
            card->addUInt32(WiFiDBUpdater::sAPLocCountTag, l_data_ptr->size());
            int mob_ap_cnt = 0, unk_ap_cnt = 0;
            for (vector<APSpecialInfo>::iterator it = s_data_ptr->begin();
                 it != s_data_ptr->end(); ++it) {
                APSpecialInfo* ap_info = &(*it);
                OutPostcard* locCard = OutPostcard::createInstance();
                locCard->init();
                if (1 == ap_info->info) {
                  locCard->addUInt64("AP-MOB-MAC", ap_info->mac_R48b);
                  locCard->finalize();
                  card->addCard("MOB-AP-LIST", locCard);
                  mob_ap_cnt++;
                } else {
                  locCard->addUInt64("AP-UNKNOWN-MAC", ap_info->mac_R48b);
                  locCard->finalize();
                  card->addCard("UNK-AP-LIST", locCard);
                  unk_ap_cnt++;
                }
                delete locCard;
            }
            card->addUInt32("AP-MOB-COUNT", mob_ap_cnt);
            card->addUInt32("AP-UNKNOWN-COUNT", unk_ap_cnt);

            card->addInt32(DAYS_VALID, days_valid);
            card->finalize();

            mNotifierProxy->sendCard(card);
        }
    }
}

void WiFiDBUpdater::handleMsg(InPostcard * const in_msg) {
    const char* info = nullptr;
    const char* req = nullptr;
    const char* status = nullptr;
    const char* client = nullptr;
    const char STATUS[] = "STATUS";
    const char SUCCESS[] = "SUCCESS";
    const char SERVICE[] = "SERVICE";
    const char REGISTER[] = "REGISTER-EVENT";
    uint32_t ap_count = 0;
    vector<APInfo> ap_list;

    if (nullptr != in_msg) {
        if (0 == in_msg->getString(sInfoTag, &info)) {
            if (0 == strncmp(WiFiDBUpdater::sAPListTag, info,
                             sizeof(WiFiDBUpdater::sAPListTag) - 1) &&
                0 == in_msg->getUInt32(WiFiDBUpdater::sAPListCountTag, ap_count) &&
                0 != ap_count) {
                for (uint32_t ii = 0; ii < ap_count; ++ii) {
                    InPostcard* ap_info_card = 0;
                    if (0 == in_msg->getCard(WiFiDBUpdater::sAPListTag, &ap_info_card, ii) &&
                        nullptr != ap_info_card) {
                        APInfo ap_info;
                        unsigned long long mac;
                        ap_info_card->getUInt64("MAC-ADDRESS", mac);
                        ap_info.mac_R48b = (uint64_t)mac;
                        ap_info_card->getUInt8("CELL-TYPE", ap_info.cellType);
                        ap_info_card->getUInt16("CELL-REG-1", ap_info.cellRegionID1);
                        ap_info_card->getUInt16("CELL-REG-2", ap_info.cellRegionID2);
                        ap_info_card->getUInt16("CELL-REG-3", ap_info.cellRegionID3);
                        ap_info_card->getUInt16("CELL-REG-4", ap_info.cellRegionID4);
                        ap_info_card->getUInt8("SSID-BYTE-CNT", ap_info.ssid_valid_byte_count);
                        ap_info.ssid_valid_byte_count = std::min(ap_info.ssid_valid_byte_count,
                                                                 (uint8_t)AP_SSID_PREFIX_LENGTH);
                        ap_info_card->getArrayInt8("SSID", (int*)&(ap_info.ssid_valid_byte_count),
                                                   (PostcardBase::INT8*)ap_info.ssid);
                        ap_list.push_back(ap_info);
                        delete ap_info_card;
                    }
                }
                apListUpdate(&ap_list);
            } else if (0 == strncmp(STATUS, info, sizeof(STATUS) - 1) &&
                       0 == in_msg->getString(STATUS, &status)) {
                if (0 == strncmp(status, SUCCESS, sizeof(SUCCESS) - 1)) {
                    statusUpdate(true, nullptr);
                } else {
                    statusUpdate(false, status);
                }
            } else if (0 == strncmp(REGISTER, info, sizeof(REGISTER) - 1)) {
                static bool isCallbackNotified = false;
                if (!isCallbackNotified) {
                    notifyCallbackEnv();
                    isCallbackNotified = true;
                }
            }
        }
        if (0 == in_msg->getString(sReqTag, &req) &&
            0 == strncmp(SERVICE, req, sizeof(SERVICE) - 1)) {
            serviceRequest();
        }
    }
}

}
