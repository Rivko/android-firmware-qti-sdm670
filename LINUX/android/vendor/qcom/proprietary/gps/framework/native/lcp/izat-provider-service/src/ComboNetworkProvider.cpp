/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  Combo Network Provider module

  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_ComboNetworkProvider"

#include "ComboNetworkProvider.h"
#include "IOSFramework.h"
#include "IOSFrameworkCleaner.h"
#include "QNP.h"
#include "OsNpProxy.h"
#include "loc_cfg.h"

#include "DataItemId.h"
#include "DataItemConcreteTypes.h"
#include "DataItemsFactory.h"
#include <log_util.h>
#include <IDataItemCore.h>
#include "Wiper.h"

namespace izat_manager
{

ILocationProvider* ComboNetworkProvider::mComboNetworkProvider = NULL;

void ComboNetworkProvider::switchNlpModeMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_NON_ZERO(0, (mCombo->mInEmergencyMode == mLocRequest.emergencyRequest));
        mCombo->mInEmergencyMode = mLocRequest.emergencyRequest;

        LocationRequest locRequest = {ILocationProvider::LocationRequestAction_Stop, 0, false};

        // If we are switching to or from emergency mode, and a provider not
        // required to be running in that  switched to mode is already running
        // then we need to stop it.
        if (COMBO_STATE_TRACKING == mCombo->mProviderState) {
            if (mCombo->mInEmergencyMode) {
                // Both allowed emergency modes include QNP, so we only
                // really care about stopping GNP if GNP is not allowed in the
                // emergency mode
                if ((NLP_MODE_QNP_ONLY == mCombo->mEmergencyNlpMode) && (NULL != mCombo->mOSNP)) {
                    mCombo->mOSNP->setRequest(&locRequest);
                }
            } else {
                switch (mCombo->mNlpMode)
                {
                    // 1. Both allowed emergency modes include QNP, so we need
                    // to stop QNP only if regular mode is GNP_ONLY
                    case NLP_MODE_GNP_ONLY:
                        if (NULL != mCombo->mQNP) {
                            mCombo->mQNP->setRequest(&locRequest);
                        }
                        break;
                    // 2. Emergency mode COMBO includes GNP, so we need to stop GNP
                    // only if regular mode is QNP_ONLY
                    case NLP_MODE_QNP_ONLY:
                        if (NULL != mCombo->mOSNP) {
                            mCombo->mOSNP->setRequest(&locRequest);
                        }
                        break;
                   //.3. Both allowed emergency modes include QNP, so
                   // we need to stop QNP if regular mode is in QNP_PREFERRED, QNP not NULL and
                   // and EULA is not accepted and QNP does not support fix when EULA is disabled,
                   // in which case OSNP will be used
                   case NLP_MODE_QNP_PREFER:
                        if (!(mCombo->useQNPInQNPPreferredMode())) {
                            if (NULL != mCombo->mQNP) {
                                mCombo->mQNP->setRequest(&locRequest);
                            }
                        }
                        break;
                   default:
                        break;
                }
            }
        }

        if (mCombo->mInEmergencyMode) {
            mCombo->mNlpModeInUse = mCombo->mEmergencyNlpMode;
        } else {
            mCombo->mNlpModeInUse = mCombo->mNlpMode;
        }

        LOC_LOGD("Combo NlpModeInUse = %d", mCombo->mNlpModeInUse);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::requestLocationMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_NON_ZERO(1, COMBO_STATE_DISABLED == mCombo->mProviderState);

        if ((LocationRequestAction_SingleShot == mLocRequest.action) ||
            (LocationRequestAction_Start == mLocRequest.action)) {
            // Save current request just in case EULA changes
            mCombo->mCurrentRequest = mLocRequest;

            if (COMBO_STATE_IDLE == mCombo->mProviderState) {
                LOC_LOGD("In IDLE state, start tracking");
                mCombo->startTrackingSession(&mLocRequest);
                mCombo->mProviderState = COMBO_STATE_TRACKING;
            } else {
                LOC_LOGD("In the middle of position request, update tracking session if required.");
                mCombo->updateTrackingSession(&mLocRequest);
            }
        } else if (LocationRequestAction_Stop == mLocRequest.action) {
            mCombo->stopTrackingSession(&mLocRequest);
            mCombo->mProviderState = COMBO_STATE_IDLE;
        } else {
            LOC_LOGE("Unhandled LocationRequest type.");
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::toleranceTimerMsg::proc() const
{
    ENTRY_LOG();

    int result = -1;
    do {
         BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);

        if (mCombo->mAwaitingQuickQnpFix) {
            LOC_LOGD("Timed out waiting for quick QNP location, so start OSNP");
            mCombo->mAwaitingQuickQnpFix = false;
            mCombo->mOSNP->setRequest(&mCombo->mCurrentRequest);
            if (PROVIDER_NONE == mCombo->mPreferredProvider) {
                mCombo->resetToleranceTimer(mCombo->mToleranceTimeFirstInMsec);
                result = 0;
                break;
            }
        } else if ((mCombo->mQNPLocReport.isValid() == false) &&
            (mCombo->mFrameworkLocReport.isValid() == false)) {
            mCombo->mToleranceExpired = true;
            break;
        } else if (PROVIDER_NONE == mCombo->mPreferredProvider) {

            if (mCombo->mQNPLocReport.isValid()) {
                LOC_LOGD("Reporting QNP location, as Framework NLP did not report within %d ms",
                    mCombo->mToleranceTimeFirstInMsec);
                mCombo->reportBestLocation(mCombo->mQNPLocReport);
                mCombo->transitionPreferredProviderToQNP();
            } else if (mCombo->mFrameworkLocReport.isValid()) {
                LOC_LOGD("Reporting Framework NLP location, as QNP did not report within %d ms",
                    mCombo->mToleranceTimeFirstInMsec);
                mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
                mCombo->mPreferredProvider = PROVIDER_OSNP;
            }

            mCombo->increamentAdaptiveCounter();
        } else if (PROVIDER_OSNP == mCombo->mPreferredProvider) {
            if ((mCombo->mQNPLocReport.isValid() == false) &&
                (mCombo->mFrameworkLocReport.isValid())) {
                mCombo->increamentAdaptiveCounter();
            } else if ((mCombo->mQNPLocReport.isValid()) &&
                (mCombo->mFrameworkLocReport.isValid() == false)) {
                mCombo->decreamentAdaptiveCounter();
            }
        } else {
            if ((mCombo->mQNPLocReport.isValid()) &&
                (mCombo->mFrameworkLocReport.isValid() == false)) {
                mCombo->increamentAdaptiveCounter();
            } else if ((mCombo->mQNPLocReport.isValid() == false) &&
                (mCombo->mFrameworkLocReport.isValid())) {
                mCombo->decreamentAdaptiveCounter();
            }
        }

        // reset the next combo state variables for next interval
        mCombo->resetForNextInterval(
            capTouint32(mCombo->mToleranceTimeAfterInMsec + (int64) mCombo->mCurrentTimeBetweenFixInMsec));
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::reportLocationMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);

        if (NLP_MODE_QNP_PREFER == mCombo->mNlpModeInUse  &&
            mCombo->useQNPInQNPPreferredMode() &&
            mLocationReport.mHasPositionSourceProvider &&
            EXTERNAL == mLocationReport.mPositionSourceProvider) {
            result = 2;
            break;
        }

        mCombo->reportBestLocation(mLocationReport);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::injectLocationInIdleMsg::proc() const
{
    if (COMBO_STATE_IDLE == mCombo->mProviderState) {
        if (NULL == mCombo->mWiperObj) {
            mCombo->mWiperObj = Wiper::getInstance(NULL);
        }

        if (mCombo->mWiperObj != NULL) {
            mCombo->mWiperObj->reportLocation(&mLocationReport, NULL);
        }
    }
}

void ComboNetworkProvider::handleQNPLocReportMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);

        if (mCombo->mAwaitingQuickQnpFix) {
            if (mCombo->mQNPLocReport.mHorizontalAccuracy <= mCombo->mQuickFixAccuracyMinInMeters) {
                LOC_LOGD("Reporting quick QNP location as accuracy is %f meters",
                         mCombo->mQNPLocReport.mHorizontalAccuracy);
                mCombo->reportBestLocation(mCombo->mQNPLocReport);
                mCombo->transitionPreferredProviderToQNP();
                mCombo->resetForNextInterval(
                    capTouint32(mCombo->mToleranceTimeAfterInMsec +
                    (int64)mCombo->mCurrentTimeBetweenFixInMsec));
            } else {
                if (mCombo->mInEmergencyMode) {
                    LOC_LOGD("Reporting QNP location in E911 mode");
                    mCombo->reportIntermediateLocation(mCombo->mQNPLocReport);
                }
                LOC_LOGD("Quick QNP location not good enough (%f m), so start OSNP",
                         mCombo->mQNPLocReport.mHorizontalAccuracy);
                mCombo->mAwaitingQuickQnpFix = false;
                mCombo->mOSNP->setRequest(&mCombo->mCurrentRequest);
                if (PROVIDER_NONE == mCombo->mPreferredProvider) {
                    mCombo->resetToleranceTimer(mCombo->mToleranceTimeFirstInMsec);
                } else {
                    mCombo->resetToleranceTimer(
                        capTouint32(mCombo->mToleranceTimeAfterInMsec +
                        (int64)mCombo->mCurrentTimeBetweenFixInMsec));
                }
            }
            result = 0;
            break;
        } else if (PROVIDER_NONE == mCombo->mPreferredProvider) {
            if (mCombo->mFrameworkLocReport.isValid()) {
                mCombo->consolidateFirstFixesAndReport();
            } else if ((mCombo->mToleranceExpired)&&(mCombo->mQNPLocReport.mIsIntermediateFix == false)) {
                LOC_LOGD("Reporting QNP location, as Framework NLP did not report within %d ms",
                    mCombo->mToleranceTimeFirstInMsec);

                mCombo->reportBestLocation(mCombo->mQNPLocReport);
                mCombo->transitionPreferredProviderToQNP();
            } else if (mCombo->mInEmergencyMode) {
                LOC_LOGD("Not waiting for tolerance time to expire, "
                    "Reporting QNP location as we are in an E911 mode");
                mCombo->reportIntermediateLocation(mCombo->mQNPLocReport);
            } else {
                LOC_LOGD("The QNP fix is saved, and we will wait for Framework Nlp fix");
            }
        } else if (PROVIDER_QNP == mCombo->mPreferredProvider) {
            LOC_LOGD("Reporting QNP location");
            mCombo->reportBestLocation(mCombo->mQNPLocReport);

            if (mCombo->mToleranceExpired) {
                mCombo->increamentAdaptiveCounter();
            }
        } else if (mCombo->mInEmergencyMode) {
            LOC_LOGD("Reporting QNP location in E911 mode");
            mCombo->reportIntermediateLocation(mCombo->mQNPLocReport);
        } else {
            LOC_LOGD("Dropping non-preferred QNP location.");
            if (mCombo->mToleranceExpired) {
                mCombo->decreamentAdaptiveCounter();
            }
        }

        mCombo->adjustAdaptiveCounter();

        if (((mCombo->mFrameworkLocReport.isValid()) &&
            (mCombo->mQNPLocReport.isValid()) &&
            (mCombo->mQNPLocReport.mIsIntermediateFix == false)) || mCombo->mToleranceExpired) {

            mCombo->resetForNextInterval(
                capTouint32(mCombo->mToleranceTimeAfterInMsec + (int64)mCombo->mCurrentTimeBetweenFixInMsec));
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::handleFrameworkLocReportMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);

        if (PROVIDER_NONE == mCombo->mPreferredProvider) {
            if (mCombo->mQNPLocReport.isValid()) {
                mCombo->consolidateFirstFixesAndReport();
            } else if (mCombo->mToleranceExpired) {
                LOC_LOGD("Reporting Framework NLP location, \
                    as QNP did not report within %d ms", mCombo->mToleranceTimeFirstInMsec);

                mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
                mCombo->mPreferredProvider = PROVIDER_OSNP;
            } else if (mCombo->mInEmergencyMode) {
                LOC_LOGD("Not waiting for tolerance time to expire, "
                    "Reporting Framework location as we are in an E911 mode");
                mCombo->reportIntermediateLocation(mCombo->mFrameworkLocReport);
            } else if (!mCombo->mEULAAccepted) {
                LOC_LOGD("Reporting Framework NLP location as EULA is not accepted");
                mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
                mCombo->mPreferredProvider = PROVIDER_OSNP;
            } // else the Framework Nlp fix is anyways saved, and we
            // continue to wait for QNP fix
        } else if (PROVIDER_OSNP == mCombo->mPreferredProvider) {
            LOC_LOGD("Reporting Framework NLP location");

            mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
            if (mCombo->mToleranceExpired) {
                mCombo->increamentAdaptiveCounter();
            }
        } else if (mCombo->mInEmergencyMode) {
            LOC_LOGD("Reporting Framework location in E911 mode");
            mCombo->reportIntermediateLocation(mCombo->mFrameworkLocReport);
        } else {
            LOC_LOGD("Dropping non-preferred Framework NLP location.");
            if (mCombo->mToleranceExpired) {
                mCombo->decreamentAdaptiveCounter();
            }
        }

        mCombo->adjustAdaptiveCounter();

        if (((mCombo->mFrameworkLocReport.isValid()) &&
            (mCombo->mQNPLocReport.isValid())) || mCombo->mToleranceExpired) {

            mCombo->resetForNextInterval(
                capTouint32(mCombo->mToleranceTimeAfterInMsec + (int64) mCombo->mCurrentTimeBetweenFixInMsec));
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::subscribeEULAMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, mCombo != NULL);

        std::list<DataItemId> dataItemIdList(1, ENH_DATA_ITEM_ID);
        mCombo->mIzatContext->mSystemStatusOsObsrvr->subscribe(dataItemIdList, mCombo);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

ComboNetworkProvider::handleOsObserverUpdateMsg::
            handleOsObserverUpdateMsg(ComboNetworkProvider *pCombo,
            const std::list<IDataItemCore *> & dataItemList) : mCombo(pCombo)
{
    int result = -1;
    do {
        std::list<IDataItemCore *>::const_iterator it = dataItemList.begin();
        for (; it != dataItemList.end(); it++) {
            IDataItemCore *updatedDataItem = *it;

            IDataItemCore * dataitem = DataItemsFactory::createNewDataItem(updatedDataItem->getId());
            BREAK_IF_ZERO(2, dataitem);
            // Copy the contents of the data item
            dataitem->copy(updatedDataItem);

            mDataItemList.push_back(dataitem);
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::handleOsObserverUpdateMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;

        do {
            std::list<IDataItemCore *>::const_iterator it = mDataItemList.begin();
            for (; it != mDataItemList.end(); it++) {
                IDataItemCore* dataItem = *it;
                switch (dataItem->getId())
                {
                    case ENH_DATA_ITEM_ID:
                    {
                        ENHDataItem *enh = static_cast<ENHDataItem*>(dataItem);
                        mCombo->handleEULAUpdate(enh->mEnabled);
                        result = 0;
                    }
                    break;

                    default:
                    break;
                }
            }
        } while(0);
        EXIT_LOG_WITH_ERROR("%d", result);
}

ComboNetworkProvider::handleOsObserverUpdateMsg::~handleOsObserverUpdateMsg()
{
    std::list<IDataItemCore *>::const_iterator it = mDataItemList.begin();
    for (; it != mDataItemList.end(); it++)
    {
        delete *it;
    }
}

ComboNetworkProvider::ComboNetworkProvider(const struct s_IzatContext *izatContext,
        ILocationProvider* pQNP, ILocationProvider* pOSNP,
        NlpMode mode, NlpMode emergencyNlpMode, unsigned int toleranceTimeFirstInMsec,
        unsigned int toleranceTimeAfterInMsec, unsigned int nlpThreshold,
        unsigned int accuracyMultiple,
        unsigned int nlpComboModeUsesQnpWithNoEulaConsent,
        unsigned int quickFixWaitTimeMaxInMsec,
        unsigned int quickFixAccuracyMinInMeters) :
    LocationProvider(izatContext),mQNP(pQNP), mOSNP(pOSNP),
    mNlpMode(mode), mEmergencyNlpMode(emergencyNlpMode), mNlpModeInUse(mode),
    mProviderState(COMBO_STATE_DISABLED), mAwaitingQuickQnpFix(false),
    mPreferredProvider(PROVIDER_NONE), mAdaptiveCounter(0), mToleranceExpired(false),
    mCurrentTimeBetweenFixInMsec(0), mToleranceTimeFirstInMsec(toleranceTimeFirstInMsec),
    mToleranceTimeAfterInMsec(toleranceTimeAfterInMsec), mNlpThreshold(nlpThreshold),
    mAccuracyMultiple(accuracyMultiple),
    mNlpComboModeUsesQnpWithNoEulaConsent(nlpComboModeUsesQnpWithNoEulaConsent!=0),
    mQuickFixWaitTimeMaxInMsec(quickFixWaitTimeMaxInMsec),
    mQuickFixAccuracyMinInMeters(quickFixAccuracyMinInMeters),
    mToleranceTimer(this),
    mEULAAccepted(false)
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow)subscribeEULAMsg(this));

    if (mQNP != NULL) {
        mQNP->subscribe(this);
    }

    if (mOSNP != NULL) {
        mOSNP->subscribe(this);
    }
}

ComboNetworkProvider::~ComboNetworkProvider()
{
    if (mQNP != NULL) {
        QNP::destroyInstance();
        mQNP = NULL;
    }

    if (mOSNP != NULL) {
        delete mOSNP;
        mOSNP = NULL;
    }
}

ILocationProvider* ComboNetworkProvider::getInstance(const struct s_IzatContext* izatContext)
{
    int result = 0;

    // minimum accpetable values
    const unsigned int toleranceTimeFirstMinInMsec = 100;
    const unsigned int toleranceTimeAfterMinInMsec = 100;
    const unsigned int nlpThresholdMin = 1;
    const unsigned int nlpAccuracyMultipleMin = 1;

    // default values if not specified in the izat.conf.
    const unsigned int toleranceTimeFirstDefaultInMsec = 2000;
    const unsigned int toleranceTimeAfterDefaultInMsec = 20000;
    const unsigned int nlpThresholdDefault = 3;
    const unsigned int accuracyMultipleDefault = 2;
    const NlpMode nlpModeDefault = NLP_MODE_COMBO;
    const NlpMode nlpModeEmergencyDefault = NLP_MODE_QNP_ONLY;
    const unsigned int nlpComboModeUsesQNPWithNoEULAConsentDefault = 1;

    unsigned int toleranceTimeFirstInMsec = toleranceTimeFirstDefaultInMsec;
    unsigned int toleranceTimeAfterInMsec = toleranceTimeAfterDefaultInMsec;
    unsigned int nlpThreshold = nlpThresholdDefault;
    unsigned int accuracyMultiple = accuracyMultipleDefault;
    NlpMode nlpMode = nlpModeDefault;
    NlpMode emergencyNlpMode = nlpModeEmergencyDefault;
    unsigned int nlpComboModeUsesQnpWithNoEulaConsent =
                            nlpComboModeUsesQNPWithNoEULAConsentDefault;
    unsigned int quickFixWaitTimeMaxInMsec = 500;
    unsigned int quickFixAccuracyMinInMeters = 100;

    ILocationProvider* pQNP = NULL;
    ILocationProvider* pOSNP = NULL;

    static loc_param_s_type izat_conf_param_table[] = {
       {"NLP_MODE", &nlpMode, NULL, 'n'},
       {"NLP_MODE_EMERGENCY", &emergencyNlpMode, NULL, 'n'},
       {"NLP_TOLERANCE_TIME_FIRST", &toleranceTimeFirstInMsec, NULL, 'n'},
       {"NLP_TOLERANCE_TIME_AFTER", &toleranceTimeAfterInMsec, NULL, 'n'},
       {"NLP_THRESHOLD", &nlpThreshold, NULL, 'n'},
       {"NLP_ACCURACY_MULTIPLE", &accuracyMultiple, NULL, 'n'},
       {"NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT",
        &nlpComboModeUsesQnpWithNoEulaConsent, NULL, 'n'},
       {"NLP_COMBO_QUICK_FIX_WAIT_TIME_MAX",
        &quickFixWaitTimeMaxInMsec, NULL, 'n'},
       {"NLP_COMBO_QUICK_FIX_ACCURACY_MIN",
        &quickFixAccuracyMinInMeters, NULL, 'n'},
    };

    ENTRY_LOG();

    do {

        // Combo already intialized
        BREAK_IF_NON_ZERO(0, mComboNetworkProvider);
        BREAK_IF_ZERO(1, izatContext);

        // Read the default configurations from the izat.conf first
        UTIL_READ_CONF(mIzatConfFile.c_str(), izat_conf_param_table);

        if (nlpMode > NLP_MODE_QNP_PREFER || nlpMode < NLP_MODE_NONE) {
            nlpMode = NLP_MODE_COMBO;
        }

        // NLP_MODE_EMERGENCY set to GNP_ONLY is an invalid value, reset it to QNP_ONLY
        //if set to GNP_ONLY in izat.conf.
        if (emergencyNlpMode > NLP_MODE_COMBO || emergencyNlpMode < NLP_MODE_NONE ||
            NLP_MODE_GNP_ONLY == emergencyNlpMode) {
            emergencyNlpMode = NLP_MODE_QNP_ONLY;
        }

        // Check for minimum toleranceTimeFirst
        toleranceTimeFirstInMsec =
            (toleranceTimeFirstInMsec < toleranceTimeFirstMinInMsec ?
            toleranceTimeFirstMinInMsec : toleranceTimeFirstInMsec);

        // Check for minimum toleranceTimeAfter
        toleranceTimeAfterInMsec =
            (toleranceTimeAfterInMsec < toleranceTimeAfterMinInMsec ?
            toleranceTimeAfterMinInMsec: toleranceTimeAfterInMsec);

        // Check for minimum threshold set for nlp selection
        nlpThreshold = (nlpThreshold < nlpThresholdMin ?
            nlpThresholdMin: nlpThreshold);

        // Check for minimum accuracy factory to be used to compare against.
        accuracyMultiple = (accuracyMultiple < nlpAccuracyMultipleMin ?
            nlpAccuracyMultipleMin : accuracyMultiple);

        // Decide on the final regular functioning mode
        if (NLP_MODE_QNP_ONLY == nlpMode) {
            pQNP = QNP::getInstance(izatContext);
            if (pQNP == NULL) {
                pOSNP = OsNpProxy::createInstance(izatContext);
                if (pOSNP != NULL) {
                    nlpMode = NLP_MODE_GNP_ONLY;
                } else {
                    nlpMode = NLP_MODE_NONE;
                }
            }
        }
        else if (NLP_MODE_GNP_ONLY == nlpMode) {
            pOSNP = OsNpProxy::createInstance(izatContext);
            if (pOSNP == NULL) {
                pQNP = QNP::getInstance(izatContext);
                if (pQNP != NULL) {
                    nlpMode = NLP_MODE_QNP_ONLY;
                } else {
                    nlpMode = NLP_MODE_NONE;
                }
            }
        }
        else if ((NLP_MODE_COMBO == nlpMode) ||
                 (NLP_MODE_QNP_PREFER == nlpMode) ||
                 (NLP_MODE_NONE == nlpMode)) {
            pQNP = QNP::getInstance(izatContext);
            pOSNP = OsNpProxy::createInstance(izatContext);

            if ((pQNP == NULL) || (pOSNP == NULL)) {
                if (pQNP != NULL) {
                    nlpMode = NLP_MODE_QNP_ONLY;
                } else if (pOSNP != NULL) {
                    nlpMode = NLP_MODE_GNP_ONLY;
                } else {
                    nlpMode = NLP_MODE_NONE;
                }
            } else if (NLP_MODE_NONE == nlpMode) {
                nlpMode = NLP_MODE_COMBO;
            }
        }


        // Decide on the emergency functioning mode
        if (NLP_MODE_QNP_ONLY == emergencyNlpMode) {
            pQNP = QNP::getInstance(izatContext);
            if (NULL == pQNP) {
                LOC_LOGW("Failed to initialize to NLP_MODE_QNP_ONLY for Emergency mode");
                emergencyNlpMode = NLP_MODE_NONE;
            }
        } else if (NLP_MODE_COMBO == emergencyNlpMode) {
            pQNP = QNP::getInstance(izatContext);

            // if OSNP is not already created for regular modes then create it here
            if (NULL == pOSNP) {
                pOSNP = OsNpProxy::createInstance(izatContext);
            }

            if ((NULL == pQNP) || (NULL == pOSNP)) {
                if (NULL == pQNP) {
                    LOC_LOGW("Failed to initialize to NLP_MODE_COMBO for Emergency mode");
                    emergencyNlpMode = NLP_MODE_NONE;
                } else if (NULL == pOSNP) {
                    emergencyNlpMode = NLP_MODE_QNP_ONLY;
                }
            }
        }


        // this should not happen, but just in case
        BREAK_IF_NON_ZERO(3, NLP_MODE_NONE == nlpMode);

        LOC_LOGD ("LocTech-Label :: COMBO :: Combo Mode");
        LOC_LOGD ("LocTech-Value :: NLP_MODE = %d NLP_MODE_EMERGENCY = %d \
            NLP_TOLERANCE_TIME_FIRST (ms) = %d NLP_TOLERANCE_TIME_AFTER (ms)= %d \
            NLP_THRESHOLD = %d NLP_ACCURACY_MULTIPLE = %d NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT = %d",
            nlpMode, emergencyNlpMode, toleranceTimeFirstInMsec, toleranceTimeAfterInMsec,
            nlpThreshold, accuracyMultiple, nlpComboModeUsesQnpWithNoEulaConsent);

        mComboNetworkProvider = new (nothrow) ComboNetworkProvider(izatContext, pQNP, pOSNP,
            nlpMode, emergencyNlpMode, toleranceTimeFirstInMsec, toleranceTimeAfterInMsec,
            nlpThreshold, accuracyMultiple, nlpComboModeUsesQnpWithNoEulaConsent,
            quickFixWaitTimeMaxInMsec, quickFixAccuracyMinInMeters);

        BREAK_IF_ZERO(4, mComboNetworkProvider);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mComboNetworkProvider;
}

int ComboNetworkProvider::destroyInstance()
{
    ENTRY_LOG();

    delete mComboNetworkProvider;
    mComboNetworkProvider = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
    return 0;
}

// ILocationRequest overrides
int ComboNetworkProvider::setRequest(const LocationRequest *request)
{
    ENTRY_LOG();
    int result = 0;

    do {
        BREAK_IF_ZERO(1, request);
        LOC_LOGD ("LocTech-Label :: COMBO :: Position Request In");
        LOC_LOGD ("LocTech-Value :: Action (Single Shot = 0, Start = 1, Stop = 2): %d", request->action);
        LOC_LOGD ("LocTech-Value :: Interval In milliseconds: %u", request->intervalInMsec);
        LOC_LOGD ("LocTech-Value :: Emergency Request: %d", request->emergencyRequest);

        mIzatContext->mMsgTask->sendMsg(new (nothrow) switchNlpModeMsg(this, request));
        mIzatContext->mMsgTask->sendMsg(new (nothrow)requestLocationMsg(this, request));
        result = 0;

    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void ComboNetworkProvider::enable()
{
    ENTRY_LOG();

    if (COMBO_STATE_DISABLED == mProviderState) {
        mProviderState = COMBO_STATE_IDLE;

        if (mQNP != NULL) {
            mQNP->enable();
        }

        if (mOSNP != NULL) {
            mOSNP->enable();
        }
    }

    LOC_LOGD ("LocTech-Label :: COMBO :: ComboNetworkProviderState");
    LOC_LOGD ("LocTech-Value :: Combo State: %d", mProviderState);

    EXIT_LOG_WITH_ERROR("%d", 0);

}

void ComboNetworkProvider::disable()
{
    ENTRY_LOG();

    if ((COMBO_STATE_IDLE == mProviderState) ||
         (COMBO_STATE_TRACKING == mProviderState)) {
        // On-going request / timer must be stopped
        LocationRequest locRequest = {ILocationProvider::LocationRequestAction_Stop, 0, false};
        stopTrackingSession(&locRequest);

        // Just change the state to disabled and exit.
        // If location comes after changing the state to disabled, location will be dropped.
        mProviderState = COMBO_STATE_DISABLED;

        if (mQNP != NULL) {
            mQNP->disable();
        }

        if (mOSNP != NULL) {
            mOSNP->disable();
        }
    }

    LOC_LOGD ("LocTech-Label :: COMBO :: ComboNetworkProviderState");
    LOC_LOGD ("LocTech-Value :: Combo State: %d", mProviderState);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

// ILocationResponse overrides
void ComboNetworkProvider::reportLocation(const LocationReport *report,
    const ILocationProvider *providerSrc)
{
    ENTRY_LOG();

    LOC_LOGD ("LocTech-Label :: COMBO :: Position Report In");

    if (providerSrc == mQNP) {
        LOC_LOGD ("LocTech-Value :: Provider Source: QNP");
    } else if (providerSrc == mOSNP) {
        LOC_LOGD ("LocTech-Value :: Provider Source: OSNP");
    }

    string locationReport;
    report->stringify (locationReport);
    LOC_LOGD ("LocTech-Value :: Location Report: %s", locationReport.c_str());

    mIzatContext->mMsgTask->sendMsg(new (nothrow) injectLocationInIdleMsg(this, report));

    if ((mNlpModeInUse < NLP_MODE_COMBO) || (NLP_MODE_QNP_PREFER == mNlpModeInUse)) {
        // GNP takes ~50 secs to actually stop sending fixes which will not be acceptable in E911 mode and
        // preferred emergency nlp mode does not allow GNP only fixes.
        if ((mInEmergencyMode) && (providerSrc == mOSNP)) {
            LOC_LOGD("Ignore OSNP fix in E911 mode");
            return;
        }
        mIzatContext->mMsgTask->sendMsg(new (nothrow) reportLocationMsg(this, report));
    } else {
        if (providerSrc == mQNP) {
            mQNPLocReport = *report;
            mIzatContext->mMsgTask->sendMsg(new (nothrow) handleQNPLocReportMsg(this));
        } else if (providerSrc == mOSNP) {
            mFrameworkLocReport = *report;
            mIzatContext->mMsgTask->sendMsg(new (nothrow) handleFrameworkLocReportMsg(this));
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::reportError(const LocationError* /*error*/,
        const ILocationProvider* /*providerSrc*/)
{
}

void ComboNetworkProvider::notify(const std::list<IDataItemCore *> & dlist)
{
    ENTRY_LOG();
    mIzatContext->mMsgTask->sendMsg(new (nothrow) handleOsObserverUpdateMsg(this, dlist));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::startTrackingSession(const LocationRequest *request)
{
    ENTRY_LOG();

    mAdaptiveCounter = 0;
    mPreferredProvider = PROVIDER_NONE;

    // record the new interval
    mCurrentTimeBetweenFixInMsec = request->intervalInMsec;

    if (NLP_MODE_COMBO == mNlpModeInUse) {

        // QNP is only used if this is an emergency request
        // or if EULA is accepted
        // or forced via NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT
        if (mInEmergencyMode) {
            mQNP->setRequest(request);
            mOSNP->setRequest(request);
        } else if (mEULAAccepted) {
            mAwaitingQuickQnpFix = true;
            mQNP->setRequest(request);
        } else if (mNlpComboModeUsesQnpWithNoEulaConsent) {
            mQNP->setRequest(request);
            mOSNP->setRequest(request);
        } else {
            mOSNP->setRequest(request);
        }

        if (mAwaitingQuickQnpFix) {
            resetForNextInterval(mQuickFixWaitTimeMaxInMsec);
        } else {
            resetForNextInterval(mToleranceTimeFirstInMsec);
        }

    } else if (NLP_MODE_QNP_PREFER == mNlpModeInUse) {

        // if EULA is accepted or if QNP can support fix when EULA is off,
        // QNP is used, otherwise, OSNP is used
        if (useQNPInQNPPreferredMode()) {
            if (NULL != mQNP) {
                mQNP->setRequest(request);
            }
        } else {
            if (NULL != mOSNP) {
                mOSNP->setRequest(request);
            }
        }
    } else {
        if (NLP_MODE_QNP_ONLY == mNlpModeInUse) {
            mQNPLocReport.reset();
            mQNP->setRequest(request);
        } else if (NLP_MODE_GNP_ONLY == mNlpModeInUse) {
            mFrameworkLocReport.reset();
            mOSNP->setRequest(request);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::updateTrackingSession(const LocationRequest *request)
{
    ENTRY_LOG();

    mCurrentTimeBetweenFixInMsec = request->intervalInMsec;

    if (NLP_MODE_COMBO == mNlpModeInUse) {

        if ((mAwaitingQuickQnpFix) && (!mInEmergencyMode)) {
            mQNP->setRequest(request);
        } else {

            // QNP is only used if this is an emergency request
            // or if EULA is accepted
            // or forced via NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT
            if (mInEmergencyMode) {
                mQNP->setRequest(request);
                mOSNP->setRequest(request);
            } else if (mEULAAccepted) {
                mAwaitingQuickQnpFix = true;
                mQNP->setRequest(request);
                LocationRequest stopRequest =
                        {ILocationProvider::LocationRequestAction_Stop, 0, false};
                mOSNP->setRequest(&stopRequest);
            } else if (mNlpComboModeUsesQnpWithNoEulaConsent) {
                mQNP->setRequest(request);
                mOSNP->setRequest(request);
            } else {
                mOSNP->setRequest(request);
            }

            // 1. If we are waiting on first fix from an ongoing session then we just
            //     continue to do that.
            // 2. If we already have preferred providers then we want to go back to
            //     choosing preferred providers again and setting a tolerance of 5 sec.
            //     This is to avoid too much delay to get first fix to this new client
            //     incase the already selected preferred provider does not return a fix
            //     immediately.
            if (PROVIDER_NONE != mPreferredProvider) {
                mPreferredProvider = PROVIDER_NONE;
                mAdaptiveCounter = 0;
            }

            LOC_LOGD("Reset tolerance timer to get first fix for session update.");
            if (mAwaitingQuickQnpFix) {
                resetForNextInterval(mQuickFixWaitTimeMaxInMsec);
            } else {
                resetForNextInterval(mToleranceTimeFirstInMsec);
            }
        }
    } else if (NLP_MODE_QNP_PREFER == mNlpModeInUse) {
        // if EULA is accepted, QNP is used
        // otherwise, OSNP is used
        if (useQNPInQNPPreferredMode()) {
            if (NULL != mQNP) {
                mQNP->setRequest(request);
            }
        } else {
            if (NULL != mOSNP) {
                mOSNP->setRequest(request);
            }
        }
    } else {
        if (NLP_MODE_QNP_ONLY == mNlpModeInUse) {
            mQNP->setRequest(request);
        } else if (NLP_MODE_GNP_ONLY == mNlpModeInUse) {
            mOSNP->setRequest(request);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}


void ComboNetworkProvider::stopTrackingSession(const LocationRequest *request)
{
    ENTRY_LOG();

    if (NLP_MODE_COMBO == mNlpModeInUse) {
        mQNP->setRequest(request);
        if (!mAwaitingQuickQnpFix) {
            mOSNP->setRequest(request);
        }

        mToleranceTimer.stop();
     } else if (NLP_MODE_QNP_PREFER == mNlpModeInUse) {
        // if EULA is accepted or if QNP can support fix when EULA is off,
        // QNP is used, otherwise, OSNP is used
        if (useQNPInQNPPreferredMode()) {
            if (NULL != mQNP) {
                mQNP->setRequest(request);
            }
        } else {
            if (NULL != mOSNP) {
                mOSNP->setRequest(request);
            }
        }
    } else {
        if (NLP_MODE_QNP_ONLY == mNlpModeInUse) {
            mQNP->setRequest(request);
        } else if (NLP_MODE_GNP_ONLY == mNlpModeInUse) {
            mOSNP->setRequest(request);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::increamentAdaptiveCounter()
{
    ++mAdaptiveCounter;
    if ((uint32_t)mAdaptiveCounter > mNlpThreshold) {
        mAdaptiveCounter = mNlpThreshold;
    }

    LOC_LOGD("+Adaptive Counter = %d", mAdaptiveCounter);
}

void ComboNetworkProvider::decreamentAdaptiveCounter()
{
    int thresholdMin = -1 * mNlpThreshold;

    --mAdaptiveCounter;
    if (mAdaptiveCounter <= thresholdMin) {
        if (PROVIDER_QNP == mPreferredProvider) {
            mPreferredProvider = PROVIDER_OSNP;
        } else {
            transitionPreferredProviderToQNP();
        }

        mAdaptiveCounter = 0;

        if (PROVIDER_OSNP == mPreferredProvider) {
            LOC_LOGD("Preferred change to Framework NLP");
        } else {
            LOC_LOGD("Preferred change to QNP");
        }
    }

    LOC_LOGD("-Adaptive Counter = %d", mAdaptiveCounter);
}

void ComboNetworkProvider::resetForNextInterval(uint32_t interval)
{
    // reset these variables for every tolerance timer interval

    mToleranceExpired = false;

    // reset all position or error reports
    mQNPLocReport.reset();
    mFrameworkLocReport.reset();
    mQNPErrorReport.reset();
    mFrameworkErrorReport.reset();

    mToleranceTimer.stop();
    mToleranceTimer.start(interval, false);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::resetToleranceTimer(uint32_t interval)
{
    mToleranceExpired = false;
    mToleranceTimer.stop();
    mToleranceTimer.start(interval, false);
}
void ComboNetworkProvider::consolidateFirstFixesAndReport()
{
    if (mFrameworkLocReport.isValid() && mQNPLocReport.isValid()) {
        // We may receive intermediate QNP fixes
        if (mQNPLocReport.mIsIntermediateFix)
        {
          LOC_LOGD("Reporting QNP intermediate location, as %f <= %f",
              mQNPLocReport.mHorizontalAccuracy, mFrameworkLocReport.mHorizontalAccuracy);
          reportIntermediateLocation(mQNPLocReport);
          if (mToleranceExpired)
          {
            LOC_LOGD("Reporting Framework NLP location, as %f <= %f",
                mFrameworkLocReport.mHorizontalAccuracy, mQNPLocReport.mHorizontalAccuracy);
             reportBestLocation(mFrameworkLocReport);
          }
          return;
        }
        if (mFrameworkLocReport.mHorizontalAccuracy < mQNPLocReport.mHorizontalAccuracy) {
            mPreferredProvider = PROVIDER_OSNP;

            LOC_LOGD("Reporting Framework NLP location, as %f <= %f",
                mFrameworkLocReport.mHorizontalAccuracy, mQNPLocReport.mHorizontalAccuracy);
            reportBestLocation(mFrameworkLocReport);
        } else {
            transitionPreferredProviderToQNP();

            LOC_LOGD("Reporting QNP location, as %f <= %f",
                mQNPLocReport.mHorizontalAccuracy ,mFrameworkLocReport.mHorizontalAccuracy);
            reportBestLocation(mQNPLocReport);
        }
    }
}

void ComboNetworkProvider::adjustAdaptiveCounter()
{
    if (mFrameworkLocReport.isValid() && mQNPLocReport.isValid()) {
        if (mFrameworkLocReport.mHorizontalAccuracy >=
            mAccuracyMultiple * mQNPLocReport.mHorizontalAccuracy) {

            LOC_LOGD("Framework NLP has much worse accuracy than QNP (%f vs %f)",
                mFrameworkLocReport.mHorizontalAccuracy, mQNPLocReport.mHorizontalAccuracy);

            if (PROVIDER_OSNP == mPreferredProvider) {
                decreamentAdaptiveCounter();
            } else {
                increamentAdaptiveCounter();
            }
        } else if (mQNPLocReport.mHorizontalAccuracy >=
            mAccuracyMultiple * mFrameworkLocReport.mHorizontalAccuracy) {

            LOC_LOGD("QNP has much worse accuracy than Framework NLP (%f vs %f)",
                mQNPLocReport.mHorizontalAccuracy, mFrameworkLocReport.mHorizontalAccuracy);

            if (PROVIDER_OSNP == mPreferredProvider) {
                increamentAdaptiveCounter();
            } else {
                decreamentAdaptiveCounter();
            }
        }
    }
}

void ComboNetworkProvider::reportBestLocation(const LocationReport & report)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(2, report.isValid());

        LOC_LOGD("Best location ready for broadcast");

        broadcastLocation(&report);
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::reportIntermediateLocation(LocationReport report)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(2, report.isValid());

        LOC_LOGD("Intermediate location ready for broadcast");
        report.mIsIntermediateFix = true;

        broadcastLocation(&report);
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::Timer::timeOutCallback()
{
    ENTRY_LOG();
    mClient->mIzatContext->mMsgTask->sendMsg(new (nothrow) toleranceTimerMsg(mClient));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::handleEULAUpdate(bool eulaAccepted)
{
    ENTRY_LOG();
    if (eulaAccepted != mEULAAccepted) {
        LOC_LOGD("EULA changed to: %d", eulaAccepted);
        mEULAAccepted = eulaAccepted;

        if ((false == mInEmergencyMode) &&
            (mNlpModeInUse == NLP_MODE_COMBO) &&
            (mNlpComboModeUsesQnpWithNoEulaConsent == false) &&
            (mProviderState == COMBO_STATE_TRACKING))
        {
            if (mEULAAccepted == true) {
                mQNP->setRequest(&mCurrentRequest);
                LOC_LOGD("QNP is requested after EULA change");
            } else {
                LocationRequest stopRequest = {ILocationProvider::LocationRequestAction_Stop, 0, false};
                mQNP->setRequest(&stopRequest);
                LOC_LOGD("QNP request canceled after EULA change");
            }
        } else if ((mNlpModeInUse == NLP_MODE_QNP_PREFER) &&
                   (mProviderState == COMBO_STATE_TRACKING))
        {
            if (useQNPInQNPPreferredMode()) {
                LocationRequest stopRequest = {ILocationProvider::LocationRequestAction_Stop, 0, false};
                if (NULL != mOSNP) {
                    mOSNP->setRequest(&stopRequest);
                }
                if (NULL != mQNP) {
                    mQNP->setRequest(&mCurrentRequest);
                }
                LOC_LOGD("QNP is requested and OSNP request canceled after EULA change");
            } else {
                LocationRequest stopRequest = {ILocationProvider::LocationRequestAction_Stop, 0, false};
                if (NULL != mQNP) {
                    mQNP->setRequest(&stopRequest);
                }
                if (NULL != mOSNP) {
                    mOSNP->setRequest(&mCurrentRequest);
                }
                LOC_LOGD("QNP request canceled and OSNP is requested after EULA change");
            }
        }
    }
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::transitionPreferredProviderToQNP()
{
    ENTRY_LOG();

    mPreferredProvider = PROVIDER_QNP;
    if (mQNPLocReport.mHorizontalAccuracy <= mQuickFixAccuracyMinInMeters) {
        LOC_LOGD("QNP now preferred with good (%f m) QNP location",
                  mQNPLocReport.mHorizontalAccuracy);
        if (false == mAwaitingQuickQnpFix) {
            LOC_LOGD("Turn off OSNP");
            LocationRequest stopRequest = {ILocationProvider::LocationRequestAction_Stop, 0, false};
            mOSNP->setRequest(&stopRequest);
            mAwaitingQuickQnpFix = true;
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

bool ComboNetworkProvider::useQNPInQNPPreferredMode()
{
    bool useQNP = false;

    if ((NULL != mQNP) &&
        ((true == mEULAAccepted) ||
         (static_cast<QNP *>(mQNP))->isFixSupportedWhenEULAOff() == true)) {
        useQNP = true;
    }

    return useQNP;
}

}
