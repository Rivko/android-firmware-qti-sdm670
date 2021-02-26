/******************************************************************************
 *   @file    ReferenceFeature.cpp
 *   @brief   Feature Implementation for Reference
 *
 *   DESCRIPTION
 *      Reffeature is a demo feature developed as a developer show-case for
 *   developing features with the LM Framework.
 *      * Reffeature has a metameter that gets triggered at App launch
 *      * It has a Reference meter which logs time of the day when app launched
 *      * After some samples are collected, the data is ready for analysis
 *      * The algo runs offline and takes average time duration between launches
 *      and puts it into the output DB
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "ReferenceFeature.h"
#include "ReferenceAction.h"
#include "ReferenceAlgo.h"
#include "ReferenceMetaMeter.h"

/* Register the feature in FeatureRegistry */
FeatureRegistry<ReferenceFeature> ReferenceFeature::registerFeature(REF_FEATURE_NAME);

ReferenceFeature::ReferenceFeature(FeatureInfo currFeatureInfo) :
        Feature(currFeatureInfo),
        mFeatureInputDb(nullptr) {
    DEBUGV(LOG_TAG_REF, "Constructor %s", REF_FEATURE_NAME);

    /* Always create the DB objects first and delete them last */
    createFeatureInputDb();

    if (mFeatureInputDb) {
        mMetaMeter = new ReferenceMetaMeter(mfeatureInfo, mFeatureInputDb, mFeatureState);
    }

    mAction = &ReferenceAction::getInstance();
    DEBUGV(LOG_TAG_REF, "Constructor %s", REF_FEATURE_NAME);

    if (mFeatureInputDb && mAction) {
        LMDatabase<ReferenceOutput>* outputDb = (LMDatabase<ReferenceOutput>*)mAction->getActionOutputDb();
        if(outputDb) {
            mAlgorithm = new ReferenceAlgo(mFeatureInputDb, outputDb, mFeatureState);
        }
    }
}

void ReferenceFeature::createFeatureInputDb() {
    DEBUGV(LOG_TAG_REF, "createFeatureInputDb %s", REF_FEATURE_NAME);
    lmdbError dbError = LMDB_NO_ERROR;
    if (mFeatureInputDb) {
        return;
    }

    mFeatureInputDb = new LMDatabase<time_t>(0,
            mfeatureInfo.name + ".db", REF_FEATURE_NAME);
    if (mFeatureInputDb) {
        dbError = mFeatureInputDb->LMDatabaseInit();
        if (LMDB_NO_ERROR != dbError) {
            DEBUGE(LOG_TAG_REF,
                    "Error in DB initialization - createFeatureInputDb  %d",
                    (int )dbError);
            delete mFeatureInputDb;
            mFeatureInputDb = nullptr;
            return;
        }
    }
}
