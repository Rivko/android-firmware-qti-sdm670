/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_HidlIGnss"

#include "GnssMultiClientHidlIGnss.h"
#include "GnssMultiClientCaseUtils.h"

#include <android/hardware/gnss/1.1/IGnssConfiguration.h>
#include <android/hardware/gnss/1.0/types.h>

using IGnss_V1_0 = android::hardware::gnss::V1_0::IGnss;
using IGnss_V1_1 = android::hardware::gnss::V1_1::IGnss;
using android::hardware::gnss::V1_1::IGnssConfiguration;
using android::hardware::gnss::V1_0::GnssConstellationType;

namespace garden {

#define getUserInputEnterToContinue GmccUtils::get().getUserInputEnterToContinue
#define getUserInputInt GmccUtils::get().getUserInputInt
#define getUserInputDouble GmccUtils::get().getUserInputDouble
#define getUserInputMask64Bit GmccUtils::get().getUserInputMask64Bit
#define getUserInputString GmccUtils::get().getUserInputString
#define getUserInputYesNo GmccUtils::get().getUserInputYesNo
#define getUserInputSessionMode GmccUtils::get().getUserInputSessionMode
#define getUserInputTrackingOptions GmccUtils::get().getUserInputTrackingOptions
#define getUserInputLocClientIndex GmccUtils::get().getUserInputLocClientIndex
#define getUserInputGnssConfig GmccUtils::get().getUserInputGnssConfig
#define getUserInputGnssConfigFlagsMask GmccUtils::get().getUserInputGnssConfigFlagsMask
#define strUtilTokenize GmccUtils::get().strUtilTokenize


/* GnssMultiClientHidlIGnss static elements */
GnssMultiClientHidlIGnss* GnssMultiClientHidlIGnss::sInstance = nullptr;

/* GnssMultiClientHidlIGnss Public APIs */
GARDEN_RESULT GnssMultiClientHidlIGnss::menuTest()
{
    bool exit_loop = false;
    GARDEN_RESULT gardenResult = GARDEN_RESULT_INVALID;

    while(!exit_loop) {
        PRINTLN("\n\n"
                "1: Create HIDL client for IGnss 1.1 \n"
                "1001: IGnss1.0 -> start \n"
                "1002: IGnss1.0 -> stop \n"
                "1101: IGnss 1.1 -> setPositionMode_1_1 \n"
                "1102: IGnss 1.1 -> injectBestLocation \n"
                "1103: IGnssMeasurement 1.1 -> setCallback_1_1 \n"
                "1104: IGnssConfiguration 1.1 -> setBlacklist \n"
                "99: Display this menu again \n"
                "0: <-- back\n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
            gardenResult = createHidlClient();
            break;
        case 1001:
            gardenResult = IGnss_1_0_start();
            break;
        case 1002:
            gardenResult = IGnss_1_0_stop();
            break;
        case 1101:
            gardenResult = IGnss_1_1_setPositionMode_1_1();
            break;
        case 1102:
            gardenResult = IGnss_1_1_injectBestLocation();
            break;
        case 1103:
            gardenResult = IGnssMeasurement_1_1_setCallback_1_1();
            break;
        case 1104:
            gardenResult = IGnssConfiguration_1_1_setBlacklist();
            break;
        case 99:
            continue;
        case 0:
            gardenResult = GARDEN_RESULT_ABORT;
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }

        if (0 != choice) {
            PRINTLN("\nExecution Result: %d", gardenResult);
            getUserInputEnterToContinue();
        }
    }

    return gardenResult;
}

/* Callbacks registered with HIDL framework */
void GnssDeathRecipient::serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) {
    PRINTERROR("IGnss service died");
    GnssMultiClientHidlIGnss::get().mGnssIface = nullptr;
}

/* GnssMultiClientHidlIGnss TEST CASES */
GARDEN_RESULT GnssMultiClientHidlIGnss::createHidlClient()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    // Get IGNSS service
    mGnssIface = IGnss::getService();
    if (mGnssIface != nullptr) {
        PRINTLN("IGnss::getService() success.");

        // Link to IGNSS service Death
        mGnssDeathRecipient = new GnssDeathRecipient();
        android::hardware::Return<bool> linked = mGnssIface->linkToDeath(mGnssDeathRecipient, 0);
        if (linked.isOk() && linked) {
            PRINTLN("mGnssIface->linkToDeath() success.");

            // Get Extension : IGnssConfiguration 1.1
            auto gnssConfig = mGnssIface->getExtensionGnssConfiguration_1_1();
            if (gnssConfig.isOk()) {
                PRINTLN("mGnssIface->getExtensionGnssConfiguration_1_1() success.");
                mGnssConfigurationIface = gnssConfig;

                // Get Extension : IGnssMeasurement 1.1
                auto gnssMeasurement = mGnssIface->getExtensionGnssMeasurement_1_1();
                if (gnssMeasurement.isOk()) {
                    PRINTLN("mGnssIface->getExtensionGnssMeasurement_1_1() success.");
                    mGnssMeasurementIface = gnssMeasurement;

                    gardenResult = GARDEN_RESULT_PASSED;
                } else {
                    PRINTERROR("mGnssIface->getExtensionGnssMeasurement_1_1() failed.");
                }
            } else {
                PRINTERROR("mGnssIface->getExtensionGnssConfiguration_1_1() failed.");
            }
        } else {
            PRINTERROR("mGnssIface->linkToDeath() failed, error: %s", linked.description().c_str());
        }
    } else {
        PRINTERROR("IGnss::getService() call failed.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlIGnss::IGnss_1_0_start()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mGnssIface != nullptr) {

        auto result = mGnssIface->start();

        if (result.isOk() && result) {

            PRINTLN("mGnssIface->start success.");
            gardenResult = GARDEN_RESULT_PASSED;

        } else {
            PRINTERROR("mGnssIface->start failed.");
        }
    } else {
        PRINTERROR("HIDL client not created.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlIGnss::IGnss_1_0_stop()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mGnssIface != nullptr) {

        auto result = mGnssIface->stop();

        if (result.isOk() && result) {

            PRINTLN("mGnssIface->stop success.");
            gardenResult = GARDEN_RESULT_PASSED;

        } else {
            PRINTERROR("mGnssIface->stop failed.");
        }
    } else {
        PRINTERROR("HIDL client not created.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlIGnss::IGnss_1_1_setPositionMode_1_1()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mGnssIface != nullptr) {

        PRINTLN("Please specify position mode options:");
        uint32_t mode = getUserInputInt("Position Mode ( Standalone 0 / MSB 1 / MSA 2 ) : ");
        uint32_t recurrence = getUserInputInt("Recurrence ( Periodic 0 / Single 1 ) : ");
        uint32_t tbf = getUserInputInt("Min Interval (millis) : ");
        uint32_t accuracy = getUserInputInt("Preferred Accuracy (metres) : ");
        uint32_t time = getUserInputInt("Preferred Time (for first fix) (millis) : ");
        bool lowPowerMode = getUserInputYesNo("Use Low Power Mode (y/n) : ");

        auto result = mGnssIface->setPositionMode_1_1(
                static_cast<IGnss_V1_0::GnssPositionMode>(mode),
                static_cast<IGnss_V1_0::GnssPositionRecurrence>(recurrence),
                tbf, accuracy, time, lowPowerMode);

        if (result.isOk() && result) {

            PRINTLN("mGnssIface->setPositionMode_1_1 success.");
            gardenResult = GARDEN_RESULT_PASSED;

        } else {
            PRINTERROR("mGnssIface->setPositionMode_1_1 failed.");
        }
    } else {
        PRINTERROR("HIDL client not created.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlIGnss::IGnss_1_1_injectBestLocation()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    GnssLocation location;
    PRINTLN("Please specify values to inject:");
    location.gnssLocationFlags = static_cast<uint16_t>(0x11);
    location.latitudeDegrees = getUserInputDouble("Latitude (degrees): ");
    location.longitudeDegrees = getUserInputDouble("Longitude (degrees): ");
    location.horizontalAccuracyMeters = static_cast<float>(
            getUserInputInt("Horizontal Accuracy (metres): "));

    if (mGnssIface != nullptr) {

        auto result = mGnssIface->injectBestLocation(location);
        if (result.isOk() && result) {
            PRINTLN("mGnssIface->injectBestLocation success.");
            gardenResult = GARDEN_RESULT_PASSED;
        }

    } else {
        PRINTERROR("HIDL client not created.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlIGnss::IGnssMeasurement_1_1_setCallback_1_1()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mGnssIface != nullptr) {
        if (mGnssMeasurementIface != nullptr) {

            bool enableFullTracking = getUserInputYesNo("Enable Full Tracking (y/n): ");
            auto result = mGnssMeasurementIface->setCallback_1_1(mHidlIGnssCb, enableFullTracking);

            if (result.isOk() &&
                    IGnssMeasurement_V1_0::GnssMeasurementStatus::SUCCESS == result) {

                PRINTLN("mGnssMeasurementIface->setCallback_1_1 success.");
                gardenResult = GARDEN_RESULT_PASSED;

            } else {
                PRINTERROR("mGnssMeasurementIface->setCallback_1_1 failed.");
            }
        } else {
            PRINTERROR("mGnssMeasurementIface null");
        }
    } else {
        PRINTERROR("HIDL client not created.");
    }

    return gardenResult;
}

GARDEN_RESULT GnssMultiClientHidlIGnss::IGnssConfiguration_1_1_setBlacklist()
{
    GARDEN_RESULT gardenResult = GARDEN_RESULT_FAILED;

    if (mGnssIface != nullptr) {
        if (mGnssConfigurationIface != nullptr) {

            std::vector<IGnssConfiguration::BlacklistedSource> tempVector;

            // Populate the blacklist vector
            while(true) {
                if (!getUserInputYesNo("Add a blacklist source (y/n)? ")) {
                    break;
                }
                PRINTLN("GnssConstellationType: GPS 1 / SBAS 2 / GLONASS 3 / QZSS 4 / BEIDOU 5 / GALILEO 6");
                PRINTLN("SV ID: GPS (1-32) / SBAS (33-54) / GLONASS (65-96) / QZSS (193-200) / BEIDOU (201-235) / GALILEO (301-)\n");
                IGnssConfiguration::BlacklistedSource source;
                source.constellation = (GnssConstellationType)getUserInputInt("Constellation Type: ");
                source.svid = (uint16_t)getUserInputInt("SV ID: ");
                tempVector.push_back(source);
            }

            hidl_vec<IGnssConfiguration::BlacklistedSource> blacklist(tempVector);
            PRINTLN("blacklist vector size %d", (int)blacklist.size());

            auto result = mGnssConfigurationIface->setBlacklist(blacklist);

            if (result.isOk() && result) {

                PRINTLN("mGnssConfigurationIface->setBlacklist success.");
                gardenResult = GARDEN_RESULT_PASSED;

            } else {
                PRINTERROR("mGnssConfigurationIface->setBlacklist failed.");
            }
        } else {
            PRINTERROR("mGnssConfigurationIface null");
        }
    } else {
        PRINTERROR("HIDL client not created.");
    }

    return gardenResult;
}

} // namespace garden
