/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2011, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GARDEN_TYPE_H
#define GARDEN_TYPE_H

#include <string.h>
#include <errno.h>
#include <gps_extended.h>
#include <LocTimer.h>
#include <MsgTask.h>


/** Structure that holds the command line options given to main **/
typedef struct command_line_options {
    LocGpsPositionRecurrence r; // recurrence type
    int l; // Number of sessions to loop through.
    int t; // time to stop fix in seconds
    int s; // Stacks to test.
    int remoteAPI; // Remote api.
    int ulpTestCaseNumber; // Run specified ULP test case number
    int deleteAidingDataMask; // Specifies Hexadecimal mask for deleting aiding data.
    int positionMode; // Specifies Position mode.
    int interval; // Time in milliseconds between fixes.
    int accuracy; // Accuracy in meters
    int responseTime; // Requested time to first fix in milliseconds
    LocGpsLocation location; // Only latitude, longiture and accuracy are used in this structure.
    int networkInitiatedResponse[256]; // To store the response pattern
    int niCount; // Number of back to back Ni tests
    int niResPatCount; // Number of elements in the response pattern
    int rilInterface; // Ril Interface
    char gpsConfPath[256]; // Path to config file
    int printNmea; // Print nmea string
    int satelliteDetails; // Print detailed info on satellites in view.
    int measurements; // Print measurements info
    int fixThresholdInSecs; // User specified time to first fix threshold in seconds
    int zppTestCaseNumber; // Run specified ULP test case number
    int stopOnMinSvs; // part of conditions that cause test to stop. Minimum number of SVs
    float stopOnMinSnr; // part of conditions that casue test to stop. Minimum number of SNR
    int tracking;    // start tracking session
    int trackUsingFlpInSecs; // start tracking using HW_FLP,for user specified time in secs.
    int keepAlive;
    char geofenceInput[256]; // Path to geofence input file
    char flpInput[256];      // Path to flp input file
    char multiClientInput[256]; //Path to multi client input file
    int useLocationClientApi; // use LocationClient Api
} CommandLineOptionsType;

#endif // GARDEN_TYPE_H
