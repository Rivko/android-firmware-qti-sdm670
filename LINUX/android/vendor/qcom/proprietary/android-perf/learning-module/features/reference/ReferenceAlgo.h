/******************************************************************************
 *   @file    ReferenceAlgo.h
 *   @brief   Reference Derived Algorithm class
 *
 *   DESCRIPTION
 *      Reference algo class containing a reference algorithm.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef REFERENCEALGO_H
#define REFERENCEALGO_H

#include <string>
#include "ReferenceAction.h"
#include "Algorithm.h"
#include "lmDB.h"

class ReferenceAlgo: public Algorithm {
private:
    LMDatabase<std::time_t>* mReferenceInputDb;
    LMDatabase<ReferenceOutput>* mActionOutputDb;
    void writeToOutputDb(std::string appName, double time);

public:
    ReferenceAlgo(LMDatabase<time_t>* referenceInputDb,
        LMDatabase<ReferenceOutput>* referenceActionOutputDb,
        FeatureState& featureState) :
        Algorithm(featureState),
        mReferenceInputDb(referenceInputDb),
        mActionOutputDb(referenceActionOutputDb) {}

    ~ReferenceAlgo() {}
    void runAlgo(const std::atomic_bool& interrupt);
};

#endif /* REFERENCEALGO_H */
