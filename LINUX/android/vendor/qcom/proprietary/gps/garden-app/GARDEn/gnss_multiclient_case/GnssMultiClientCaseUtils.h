/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#ifndef GNSS_MULTI_CLIENT_CASE_UTILS_H
#define GNSS_MULTI_CLIENT_CASE_UTILS_H

#include <string>
#include <sstream>
#include <iostream>

#include <log_util.h>
#include <LocationAPI.h>
#include <gps_extended_c.h>

namespace garden {

#define PRINT(fmt, ...) \
    fprintf(stdout, fmt, ##__VA_ARGS__); \
    LOC_LOGv(fmt, ##__VA_ARGS__)

#define PRINTLN(fmt, ...) \
    fprintf(stdout, fmt "\n", ##__VA_ARGS__); \
    LOC_LOGv(fmt, ##__VA_ARGS__)

#define PRINTERROR(fmt, ...) \
    fprintf(stdout, "\n[ERROR] " fmt "\n\n", ##__VA_ARGS__); \
    LOC_LOGe(fmt, ##__VA_ARGS__)

/* Conditional prints : to dynamically enable/disable logging */
#define CONDPRINT(fmt, ...) \
    if (GnssMultiClientCase::condPrintEnabled) { \
        fprintf(stdout, fmt, ##__VA_ARGS__); \
        LOC_LOGv(fmt, ##__VA_ARGS__); \
    }
#define CONDPRINTLN(fmt, ...) \
    if (GnssMultiClientCase::condPrintEnabled) { \
        fprintf(stdout, fmt "\n", ##__VA_ARGS__); \
        LOC_LOGv(fmt, ##__VA_ARGS__); \
    }

/* UTILITY CLASS */
class GmccUtils {

private:
    static GmccUtils* sInstance;
    GmccUtils() {};

public:
    static GmccUtils& get() {
        if (nullptr == sInstance) {
            sInstance = new GmccUtils();
        }
        return *sInstance;
    }

public:

    // User input utils
    void getUserInputEnterToContinue();
    int getUserInputInt(std::string prompt);
    double getUserInputDouble(std::string prompt);
    uint64_t getUserInputMask64Bit(std::string prompt);
    std::string getUserInputString(std::string prompt);
    bool getUserInputYesNo(std::string prompt);
    SESSION_MODE getUserInputSessionMode(std::string prompt);
    LocationOptions getUserInputTrackingOptions(std::string prompt);
    int getUserInputLocClientIndex(std::string prompt, uint32_t clientListSize);
    GnssConfig getUserInputGnssConfig(std::string prompt);
    GnssConfigFlagsMask getUserInputGnssConfigFlagsMask(std::string prompt);

    // String utils
    std::vector<std::string> strUtilTokenize(const std::string& str);

};

} // namespace garden
#endif // GNSS_MULTI_CLIENT_CASE_UTILS_H
