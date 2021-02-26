/******************************************************************************
 *   @file    ReferenceMetaMeter.h
 *   @brief   MetaMeter derived class for Reference
 *
 *   DESCRIPTION
 *      Reference Metameter class containing only constructor and mandatory
 *   database write function.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef REFERENCEMETAMETER_H
#define REFERENCEMETAMETER_H

#include "MetaMeter.h"
#include "FeatureInfo.h"
#include "lmDB.h"

#define TIME_METER_SAMPLES_PER_RUN 1

class ReferenceMetaMeter: public MetaMeter {
private:
    LMDatabase<std::time_t>* mReferenceInputDb;
    /* Used to check required number of samples */
    unsigned int mRefMeterSamples;

public:
    ReferenceMetaMeter(const FeatureInfo& featureInfo,
            LMDatabase<std::time_t>* referenceInputDb,
            FeatureState& featureState) :
            MetaMeter(featureInfo, featureState),
            mReferenceInputDb(referenceInputDb),
            mRefMeterSamples(TIME_METER_SAMPLES_PER_RUN) {}

    ~ReferenceMetaMeter() {}
    bool writeToDataBase(std::vector<Meter*> meterList);
};
#endif /* REFERENCEMETAMETER_H */
