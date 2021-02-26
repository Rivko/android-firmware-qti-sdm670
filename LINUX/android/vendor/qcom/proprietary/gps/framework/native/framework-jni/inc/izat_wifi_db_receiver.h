/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat WiFi DB Receiver Types and API functions

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_WIFIDBRECV_C_H__
#define __IZAT_MANAGER_WIFIDBRECV_C_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define IZAT_AP_LOC_WITH_LAT_LON          0x0;
#define IZAT_AP_LOC_MAR_VALID             0x1;
#define IZAT_AP_LOC_HORIZONTAL_ERR_VALID  0x2;
#define IZAT_AP_LOC_RELIABILITY_VALID     0x4;

#define IZAT_AP_SSID_PREFIX_LENGTH        8

struct APInfo_s {
public:
    // Full 48 bits of the Mac Address in reverse order represented
    // in 64 bit format
    uint64_t mac_R48b;

    // Type of cell in which this Mac Address is observerd atleast
    // once, if available
    // 0: unknown, 1: GSM, 2: WCDMA, 3: CDMA, 4: LTE
    uint8_t  cell_type;

    // Cell RegionID1 Mobile Country Code(MCC), For CDMA Set to 0
    uint16_t cell_id1;

    // Cell RegionID2 Mobile Network Code(MNC), For CDMA set to
    // System ID(SID), For WCDMA set to 0 if not available
    uint16_t cell_id2;

    // Cell RegionID3 GSM: Local Area Code(LAC), WCDMA: Local Area
    // Code(LAC) set to 0 if not available, CDMA: Network ID(NID),
    // LTE: Tracking Area Code(TAC) set to 0 if not available
    uint16_t cell_id3;

    // Cell RegionID4 Cell ID(CID), For CDMA set to Base Station
    // ID(BSID)
    uint16_t cell_id4;

    // SSID information. First 8 bytes of SSID if available
    char ssid[IZAT_AP_SSID_PREFIX_LENGTH];

    uint8_t ssid_valid_byte_count;

    inline APInfo_s() : mac_R48b(0)
        , cell_type(0)
        , cell_id1(0)
        , cell_id2(0)
        , cell_id3(0)
        , cell_id4(0)
        , ssid { 0}
        , ssid_valid_byte_count(0)    {
    }
};

struct APLocationData_s {
public:
    // Full 48 bits of the Mac Address in reverse order represented
    // in 64 bit format
    uint64_t mac_R48b;

    float latitude;
    float longitude;
    float max_antena_range;
    float horizontal_error;
    // 0-4: VERY_LOW, LOW, MEDIUM, HIGH, VERY_HIGH
    uint8_t reliability;

    // Use IZAT_AP_LOC.. bit masks
    int valid_bits;

    inline APLocationData_s() : mac_R48b(0)
        , latitude(0)
        , longitude(0)
        , max_antena_range(0)
        , horizontal_error(0)
        , reliability(0)
        , valid_bits(0) {
    }
};

struct APSpecialInfo_s {
public:
    // Full 48 bits of the Mac Address in reverse order represented
    // in 64 bit format
    uint64_t mac_R48b;

    // Special Information on this AP
    // 0: AP information not available
    // 1: AP detected as a moving AP
    uint8_t info;

    inline APSpecialInfo_s() : mac_R48b(0), info(0) {
    }
};

typedef void (*wifiDBRecvStatusUpdateCb)(bool isSuccess, const char *errStr,
                                         void *clientData);
typedef void (*wifiDBRecvApListUpdateCb)(APInfo_s *ap_list[], size_t ap_list_size,
                                         void *clientData);
typedef void (*wifiDBRecvServiceRequestCb)(void *clientData);
typedef void (*wifiDBRecvEnvNotifyCb)(void *clientData);


/* registers callbacks for Wifi DB updater
   statusCb - callback to receive DB injection status from GTP
            Can not be NULL.
   apListAvailCb  - callback to receive list of APs from GTP
            Can not be NULL.
   serviceReqCb  - callback to receive service request from GTP.
            Can not be NULL.
   envNotifyCb  - callback environement notification.
            Can be NULL.
   clientData - an opaque data pointer from client. This pointer will be
                provided back when the callbacaks are called.
                This can be used by client to store application context
                or statemachine etc.
                Can be NULL.
   return: an opaque pointer that serves as a request handle. This handle
           is to be fed to the unregisterWiFiDBUpdater() call.
*/
void* registerWiFiDBUpdater(wifiDBRecvApListUpdateCb apListAvailCb,
                            wifiDBRecvStatusUpdateCb statusCb,
                            wifiDBRecvServiceRequestCb serviceReqCb,
                            wifiDBRecvEnvNotifyCb envNotifyCb,
                            void *clientData);

/* unregisters the client callback
   wifiDBUpdaterHandle - the opaque pointer from the return of
                           registerWiFiADUpdater()
*/
void unregisterWiFiDBUpdater(void *wifiADUpdaterHandle);

/* Send request to obatin list of APs for which location assitance data is
   needed.
   expire_in_days - the number of days in future in which
                            the associated location of an AP if
                            available, will expire, to be fetched
                            by this request.
   wifiADUpdaterHandle - the opaque pointer from the return of
                           registerWiFiADUpdater()
*/
void sendAPListRequest(int expire_in_days, void *wifiADUpdaterHandle);

/* Send location assitance data of APs.
   ap_loc_list - List of AP location data
   ap_loc_list_size - number of APs for which location data is available
   ap_spl_list - List of APs with Special information
   ap_spl_list_size - number of APs with special information available
   days_valid- number of days for which this location data is valid
   wifiADUpdaterHandle - the opaque pointer from the return of
                           registerWiFiADUpdater()
*/
void pushWiFiDB(APLocationData_s *ap_loc_list[], size_t ap_loc_list_size,
                APSpecialInfo_s *ap_spl_list[], size_t ap_spl_list_size,
                int days_valid, void *wifiADUpdaterHandle);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef __IZAT_MANAGER_WIFIDBRECV_C_H__
