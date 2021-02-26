/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/NasDataCache.h"


NasDataCache::NasDataCache() {
        //set default values to the cache
        std::lock_guard<std::mutex> guardNasDataCache(cacheMutex);
        numCalls = -1;
        //prefDataTech = PREF_DATA_TECH_INVALID;
}
NasDataCache::~NasDataCache() {

}

NasDataCache& NasDataCache::getInstance() {
        static NasDataCache sInstance;
        return sInstance;
}

void NasDataCache::setNumDataCalls(unsigned int nCalls) {
        std::lock_guard<std::mutex> guardNasDataCache(cacheMutex);
        numCalls = nCalls;
}
unsigned int NasDataCache::getNumDataCalls() {
        std::lock_guard<std::mutex> guardNasDataCache(cacheMutex);
        return numCalls;
}
/*
void NasDataCache::setPreferredDataTech(qcril_arb_pref_data_tech_e_type dataTech) {
        std::lock_guard<std::mutex> guardNasDataCache(cacheMutex);
        switch (dataTech) {
        case QCRIL_ARB_PREF_DATA_TECH_UNKNOWN:
                prefDataTech = PREF_DATA_TECH_UNKNOWN;
                break;
        case QCRIL_ARB_PREF_DATA_TECH_CDMA:
                prefDataTech = PREF_DATA_TECH_CDMA;
                break;
        case QCRIL_ARB_PREF_DATA_TECH_EVDO:
                prefDataTech = PREF_DATA_TECH_EVDO;
                break;
        case QCRIL_ARB_PREF_DATA_TECH_GSM:
                prefDataTech = PREF_DATA_TECH_GSM;
                break;
        case QCRIL_ARB_PREF_DATA_TECH_UMTS:
                prefDataTech = PREF_DATA_TECH_UMTS;
                break;
        case QCRIL_ARB_PREF_DATA_TECH_EHRPD:
                prefDataTech = PREF_DATA_TECH_EHRPD;
                break;
        case QCRIL_ARB_PREF_DATA_TECH_LTE:
                prefDataTech = PREF_DATA_TECH_LTE;
                break;
        case QCRIL_ARB_PREF_DATA_TECH_TDSCDMA:
                prefDataTech = PREF_DATA_TECH_TDSCDMA;
                break;
        default:
                prefDataTech = PREF_DATA_TECH_INVALID;
                break;
        }
}
NasDataCache::PreferredDataTechType NasDataCache::getPreferredDataTech() {
        std::lock_guard<std::mutex> guardNasDataCache(cacheMutex);
        return prefDataTech;
}
*/
