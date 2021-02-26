/******************************************************************************
 *   @file    FeatureInfo.h
 *   @brief   FeatureInfo is internal representation of Feature XML
 *
 *   DESCRIPTION
 *      FeatureInfo defines structs to hold the data represented by the Feature
 *   XML file. Each field of Feature XML file is parsed and filled into the
 *   FeatureInfo object and passed to features during construction.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef FEATUREINFO_H
#define FEATUREINFO_H

#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include "AsyncData.h"

/**
 * Contains information about a specific Async Trigger.
 * This is more of structure to hold meter information.
 * AsyncInfo is a struct instead of class to make all the members public
 */
struct AsyncInfo {
    bool isPrimary;
    /* Types are from mp-ctl.h */
    type_hintid hintID;
    type_hinttype hintType;
    /* Default Constructor */
    AsyncInfo() {
        isPrimary = true;
        hintID = 0;
        hintType = DEFAULT_HINTTYPE;
    }
    /* Provide == overload for AsyncInfo to enable easy comparison */
    bool operator==(const AsyncInfo &rhs) const {
        return std::tie(hintID, hintType, isPrimary) ==
                std::tie(rhs.hintID, rhs.hintType, rhs.isPrimary);
    }
    /* Provide < overload for AsyncInfo to use std::set */
    bool operator<(const AsyncInfo &rhs) const {
        return std::tie(hintID, hintType, isPrimary) <
                std::tie(rhs.hintID, rhs.hintType, rhs.isPrimary);
    }
    AsyncInfo(bool p, type_hintid id, type_hinttype type) {
        isPrimary = p;
        hintID = id;
        hintType = type;
    }
    ~AsyncInfo(){}

    std::string toString() {
        std::string retvalue;
        retvalue += "AsyncInfo: hintID = "+ std::to_string(hintID) + "\n";
        retvalue += "hintType: "+ std::to_string(hintType) + "\n";
        retvalue += "isPrimary = "+ std::to_string(isPrimary) + "\n";
        return retvalue;
    }
};

/**
 * Contains information about a specific Meter.
 * This is more of structure to hold meter information.
 * MeterInfo is a struct instead of class to make all the members public
 */
struct MeterInfo {
    std::string name; /**< Name of Meter */
    /* This is a set and not unordered_set as we don't want to implement inefficient hash function and result in collisions */
    std::set<AsyncInfo> triggersList; /**< List of Async triggers. This is made per feature to handle Multiple Async's per feature. */
    std::vector<std::string> fieldsList; /**< List of fields that this Meter can collect data. This list can vary across feature to feature for a given Meter */
    std::unordered_map<std::string, std::string> configsList; /**< Map of key value pairs passed as config to the Meter */
    int numberOfSamples; /**< Number of sample to be collected. */

    std::string toString() {
        std::string retvalue;
        retvalue += "MeterInfo: meterName = "+ name + "\n";
        for(auto currTrigger : triggersList) {
            retvalue += currTrigger.toString();
        }
        for(std::string currField : fieldsList) {
            retvalue += "Field: "+ currField + "\n";
        }
        retvalue += "Configs:\n";
        for(auto currConfig : configsList) {
            retvalue += currConfig.first+ " = "+ currConfig.second  + "\n";
        }
        retvalue += "numberOfSamples = "+ std::to_string(numberOfSamples) + "\n";
        return retvalue;
    }
};

/**
 * Contains information about a specific Feature.
 * This is more of structure to hold meter information.
 *
 * Sample Representation of FeatureInfo
 *
 *  FeatureInfo preKillFinfo =
 *  {
 *      "PreKill",
 *      {
 *          VENDOR_HINT_FIRST_LAUNCH_BOOST,
 *          VENDOR_HINT_FIRST_DRAW,
 *      },
 *      10,
 *      {
 *          { "Memory", VENDOR_HINT_FIRST_LAUNCH_BOOST, {"Free:", "Cached:"}, {}, 5 },
 *          { "Memory",VENDOR_HINT_FIRST_DRAW, {"Free:"}, {}, 5},
 *      },
 *  };
 *
 * FeatureInfo is a struct instead of class to make all the members public
 */
struct FeatureInfo {
    std::string name; /**< Name of the feature */
    std::unordered_map<type_hintid, std::unordered_set<type_hinttype>> asyncIdList; /** Hashmap of hintids and hinttypes of interest */
    int samplesRequired;
    std::vector<MeterInfo> meterInfoList;     /**< List of meter information */
    std::vector<std::string> ignoredAppsList; /**< List of ignored apps */

    std::string toString() {
        std::string retvalue;
        retvalue += "FeatureInfo: featureName = "+ name + "\n";
        retvalue += "requiredSamples = "+ std::to_string(samplesRequired) + "\n";
        for(auto currAsyncId : asyncIdList) {
            retvalue += "AsyncId: "+ std::to_string(currAsyncId.first) + "\n";
            for(auto currAsyncType : currAsyncId.second) {
                retvalue += "AsyncType: "+ std::to_string(currAsyncType) + "\n";
            }
        }
        for(MeterInfo currMeterInfo : meterInfoList) {
            retvalue += currMeterInfo.toString();
        }
        return retvalue;
    }
};

#endif /* FEATUREINFO_H */
