/******************************************************************************
 *   @file    BatteryInfo.h
 *   @brief   Helper for Battery status reading
 *
 *   DESCRIPTION
 *      BatteryInfo.h provides helper definitions and types for reading the battery
 *   status and batterylevel from sysfs. Battery state is used to trigger Algo's
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef BATTERYINFO_H
#define BATTERYINFO_H

#include <unordered_map>

#define SYSFS_BATTERY_PATH "/sys/class/power_supply/battery"
#define SYSFS_BATTERY_STATUS_PATH SYSFS_BATTERY_PATH "/status"
#define SYSFS_BATTERY_LEVEL_PATH SYSFS_BATTERY_PATH "/capacity"

enum class BatteryStatus : int {
    NOT_INTERESTED = 0,
    CHARGING,
    FULL,
};

/**
 * BatteryStatusMap is a map of strings and corresponding enum values for BatteryStatus
 * this is used to convert the string read from the sysfs into corresponding enum
 *
 * BatteryStatusMap is a struct instead of class to make all the members public
 */
struct BatteryStatusMap : std::unordered_map<std::string, BatteryStatus>{
    BatteryStatusMap() {
        /*
         * Map == access using subscript operator creates default init'ed value
         * So, for all other strings, we'll get 0 = BatteryStatus::NOT_INTERESTED
         */
        this->operator []("Charging") = BatteryStatus::CHARGING;
        this->operator []("Full") = BatteryStatus::FULL;
    }
    ~BatteryStatusMap() {}
};

#endif /* BATTERYINFO_H */
