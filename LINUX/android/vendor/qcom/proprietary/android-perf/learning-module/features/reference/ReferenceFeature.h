/******************************************************************************
 *   @file    ReferenceFeature.h
 *   @brief   Reference Derived Feature class
 *
 *   DESCRIPTION
 *      Reffeature is a demo feature developed as a developer show-case for
 *   developing features with the LM Framework.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef REFERENCEFEATURE_H
#define REFERENCEFEATURE_H

#include "Feature.h"
#include "FeatureRegistry.h"
#include "lmDB.h"
#include "DebugLog.h"
#define LOG_TAG_REF "RefFeature"
#include <ctime>

/*
 * IMPORTANT: This REF_FEATURE_NAME should be same as the First Child <Name> element
 * in FeatureName.xml
 * <Feature>
 *   <Name>Reference</Name>
 * </Feature>
 */

#define REF_FEATURE_NAME "RefFeature"

template class LMDatabase<std::time_t>;

class ReferenceFeature: public Feature {
public:
    ReferenceFeature(FeatureInfo currFeatureInfo);
    ~ReferenceFeature() {
        DEBUGV(LOG_TAG_REF, "Destructor %s", REF_FEATURE_NAME);
        delete mMetaMeter;
        mMetaMeter = nullptr;
        delete mAlgorithm;
        mAlgorithm = nullptr;
        /* Always destruct the DB object last */
        delete mFeatureInputDb;
        mFeatureInputDb = nullptr;
    }

private:
    LMDatabase<std::time_t>* mFeatureInputDb;
    static FeatureRegistry<ReferenceFeature> registerFeature;

    void createFeatureInputDb();
};

#endif /* REFERENCEFEATURE_H */
