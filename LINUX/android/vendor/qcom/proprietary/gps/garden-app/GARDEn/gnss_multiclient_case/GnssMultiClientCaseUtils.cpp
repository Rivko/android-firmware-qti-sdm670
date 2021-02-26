/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "GARDEN_GMCC_Utils"

#include "LocationAPIClientBase.h"
#include "GnssMultiClientCaseUtils.h"

namespace garden {

GmccUtils* GmccUtils::sInstance = nullptr;

// Utilities
void GmccUtils::getUserInputEnterToContinue()
{
    PRINT("\nPress Enter to continue..");
    std::string userInput;
    std::cin.clear();
    std::getline(std::cin, userInput);
    PRINTLN("");
}

int GmccUtils::getUserInputInt(std::string prompt)
{
    std::string userInput;
    int intValue = -1;
    while(true) {
        PRINT("%s", prompt.c_str());
        std::cin.clear();
        std::getline(std::cin, userInput);
        std::stringstream inputStream(userInput);
        if ((inputStream >> intValue)) {
            break;
        }
        PRINTERROR("Please enter a valid number.");
    }
    PRINTLN("");
    return intValue;
}

double GmccUtils::getUserInputDouble(std::string prompt)
{
    std::string userInput;
    double doubleValue = -1.0;
    while(true) {
        PRINT("%s", prompt.c_str());
        std::cin.clear();
        std::getline(std::cin, userInput);
        std::stringstream inputStream(userInput);
        if ((inputStream >> doubleValue)) {
            break;
        }
        PRINTERROR("Please enter a valid double value.");
    }
    PRINTLN("");
    return doubleValue;
}

uint64_t GmccUtils::getUserInputMask64Bit(std::string prompt)
{
    std::string userInput;
    uint64_t mask = 0;
    while(true) {
        PRINT("%s", prompt.c_str());
        std::cin.clear();
        std::getline(std::cin, userInput);
        std::stringstream inputStream(userInput);
        if ((inputStream >> std::hex >> mask)) {
            break;
        }
        PRINTERROR("Please enter a valid 64 bit mask value.");
    }
    PRINTLN("");
    return mask;
}

std::string GmccUtils::getUserInputString(std::string prompt)
{
    PRINT("%s", prompt.c_str());
    std::string userInput;
    std::cin.clear();
    std::getline(std::cin, userInput);
    std::stringstream inputStream(userInput);
    if (!(inputStream >> userInput)) {
        PRINTERROR("Invalid input !");
    }
    PRINTLN("");
    return userInput;
}

bool GmccUtils::getUserInputYesNo(std::string prompt)
{
    std::string choice = getUserInputString(prompt);
    return (0 == choice.compare("y") ||
            0 == choice.compare("yes") ||
            0 == choice.compare("Y") ||
            0 == choice.compare("YES"));
}

SESSION_MODE
GmccUtils::getUserInputSessionMode(std::string prompt)
{
    PRINTLN("%s", prompt.c_str());
    SESSION_MODE sessionMode = SESSION_MODE_NONE;

    PRINTLN("0. SESSION_MODE_NONE\n"
            "1. SESSION_MODE_ON_FULL\n"
            "2. SESSION_MODE_ON_FIX\n"
            "3. SESSION_MODE_ON_TRIP_COMPLETED");
    int choice = getUserInputInt("Enter choice: ");
    switch(choice) {
    case 0:
        sessionMode = SESSION_MODE_NONE;
        break;
    case 1:
        sessionMode = SESSION_MODE_ON_FULL;
        break;
    case 2:
        sessionMode = SESSION_MODE_ON_FIX;
        break;
    case 3:
        sessionMode = SESSION_MODE_ON_TRIP_COMPLETED;
        break;
    default:
        sessionMode = SESSION_MODE_NONE;
        break;
    }

    return sessionMode;
}

LocationOptions
GmccUtils::getUserInputTrackingOptions(std::string prompt)
{
    PRINTLN("%s\n", prompt.c_str());
    LocationOptions options = {};

    PRINTLN("SUPL Mode: \n"
            "0: Standalone \n"
            "1: MSB \n"
            "2: MSA");
    int choice = getUserInputInt("Enter choice: ");
    switch(choice) {
    case 0:
        options.mode = GNSS_SUPL_MODE_STANDALONE;
        break;
    case 1:
        options.mode = GNSS_SUPL_MODE_MSB;
        break;
    case 2:
        options.mode = GNSS_SUPL_MODE_MSA;
        break;
    default:
        options.mode = GNSS_SUPL_MODE_STANDALONE;
        break;
    }

    options.minInterval = getUserInputInt("Min Interval (ms): ");
    options.minDistance = getUserInputInt("Min Distance (metres): ");

    options.size = sizeof(LocationOptions);

    return options;
}

int GmccUtils::getUserInputLocClientIndex(std::string prompt, uint32_t clientListSize)
{
    PRINTLN("%s", prompt.c_str());
    PRINTLN("Location API Client count: %d", clientListSize);

    // validate
    if (0 == clientListSize) {
        PRINTERROR("There are no Location API Clients created yet !");
        return -1;
    }

    int clientIndex = getUserInputInt("Index of Location API Client to use: ");

    if (clientIndex < 0 || clientIndex >= (int)clientListSize) {
        PRINTERROR("Invalid client index, should be in [0, %d]",
                clientListSize);
        return -1;
    }

    return clientIndex;
}

GnssConfig GmccUtils::getUserInputGnssConfig(std::string prompt)
{
    PRINT("%s", prompt.c_str());

    bool exit_loop = false;
    GnssConfig config = {};

    while(!exit_loop) {
        PRINTLN("Which Gnss Config would you like to update:\n\n"
                "1: GPS Lock \n"
                "2: SUPL Version \n"
                "3: Assistance Server \n"
                "4: LPP Profile \n"
                "5: LPPE Control Plane Mask \n"
                "6: LPPE User Plane Mask \n"
                "7: AGlonass Position Protocol Mask \n"
                "8: Emergency PDN \n"
                "9: SUPL Emergency Services \n"
                "10: SUPL Mode Mask \n"
                "99: Display this menu again \n"
                "0: <-- back ( to submit the updated config ) \n");
        int choice = getUserInputInt("Enter choice: ");

        switch (choice) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            PRINTERROR("Not yet supported !");
            break;
        case 99:
            continue;
        case 0:
            exit_loop = true;
            break;
        default:
            PRINTERROR("Invalid command");
        }
    }

    config.size = sizeof(GnssConfig);
    return config;
}

GnssConfigFlagsMask
GmccUtils::getUserInputGnssConfigFlagsMask(std::string prompt)
{
    PRINTLN("%s\n", prompt.c_str());

    PRINTLN("Here are the bit values for the config mask:\n");
    PRINTLN("GNSS_CONFIG_FLAGS_GPS_LOCK_VALID_BIT                   = (1<<0)   = 1     =    0x1");
    PRINTLN("GNSS_CONFIG_FLAGS_SUPL_VERSION_VALID_BIT               = (1<<1)   = 2     =    0x2");
    PRINTLN("GNSS_CONFIG_FLAGS_SET_ASSISTANCE_DATA_VALID_BIT        = (1<<2)   = 4     =    0x4");
    PRINTLN("GNSS_CONFIG_FLAGS_LPP_PROFILE_VALID_BIT                = (1<<3)   = 8     =    0x8");
    PRINTLN("GNSS_CONFIG_FLAGS_LPPE_CONTROL_PLANE_VALID_BIT         = (1<<4)   = 16    =   0x10");
    PRINTLN("GNSS_CONFIG_FLAGS_LPPE_USER_PLANE_VALID_BIT            = (1<<5)   = 32    =   0x20");
    PRINTLN("GNSS_CONFIG_FLAGS_AGLONASS_POSITION_PROTOCOL_VALID_BIT = (1<<6)   = 64    =   0x40");
    PRINTLN("GNSS_CONFIG_FLAGS_EM_PDN_FOR_EM_SUPL_VALID_BIT         = (1<<7)   = 128   =   0x80");
    PRINTLN("GNSS_CONFIG_FLAGS_SUPL_EM_SERVICES_BIT                 = (1<<8)   = 256   =  0x100");
    PRINTLN("GNSS_CONFIG_FLAGS_SUPL_MODE_BIT                        = (1<<9)   = 512   =  0x200");

    GnssConfigFlagsMask mask =
            (GnssConfigFlagsMask)getUserInputMask64Bit("\nEnter the config mask value: ");

    // print the selected mask
    PRINTLN("\nGnss Config Mask Bits you've selected:\n");
    if (mask & GNSS_CONFIG_FLAGS_GPS_LOCK_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_GPS_LOCK_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_SUPL_VERSION_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_SUPL_VERSION_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_SET_ASSISTANCE_DATA_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_SET_ASSISTANCE_DATA_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_LPP_PROFILE_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_LPP_PROFILE_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_LPPE_CONTROL_PLANE_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_LPPE_CONTROL_PLANE_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_LPPE_USER_PLANE_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_LPPE_USER_PLANE_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_AGLONASS_POSITION_PROTOCOL_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_AGLONASS_POSITION_PROTOCOL_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_EM_PDN_FOR_EM_SUPL_VALID_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_EM_PDN_FOR_EM_SUPL_VALID_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_SUPL_EM_SERVICES_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_SUPL_EM_SERVICES_BIT");
    }
    if (mask & GNSS_CONFIG_FLAGS_SUPL_MODE_BIT) {
        PRINTLN("GNSS_CONFIG_FLAGS_SUPL_MODE_BIT");
    }

    return mask;
}

std::vector<std::string>
GmccUtils::strUtilTokenize(const std::string& str)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(str);
    // Tokenize by whitespace
    for (std::string s; stream >> s; ) {
        tokens.push_back(s);
    }
    return tokens;
}

} // namespace garden
