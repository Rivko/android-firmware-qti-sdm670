/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat WWAN DB Receiver Types and API functions

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_WWANDBRECV_C_H__
#define __IZAT_MANAGER_WWANDBRECV_C_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define IZAT_BS_LOC_WITH_LAT_LON          0x0;
#define IZAT_BS_LOC_MAR_VALID             0x1;
#define IZAT_BS_LOC_HORIZONTAL_ERR_VALID  0x2;
#define IZAT_BS_LOC_RELIABILITY_VALID     0x4;

struct BSInfo_s {
public:
    // Type of cell in which this Mac Address is observerd atleast
    // once, if available
    // 0: unknown, 1: GSM, 2: WCDMA, 3: CDMA, 4: LTE
    uint8_t  cell_type;

    // Cell RegionID1 Mobile Country Code(MCC), For CDMA Set to 0
    uint32_t cell_id1;

    // Cell RegionID2 Mobile Network Code(MNC), For CDMA set to
    // System ID(SID), For WCDMA set to 0 if not available
    uint32_t cell_id2;

    // Cell RegionID3 GSM: Local Area Code(LAC), WCDMA: Local Area
    // Code(LAC) set to 0 if not available, CDMA: Network ID(NID),
    // LTE: Tracking Area Code(TAC) set to 0 if not available
    uint32_t cell_id3;

    // Cell RegionID4 Cell ID(CID), For CDMA set to Base Station
    // ID(BSID)
    uint32_t cell_id4;

    inline BSInfo_s() :
          cell_type(0)
        , cell_id1(0)
        , cell_id2(0)
        , cell_id3(0)
        , cell_id4(0)    {
    }
};

struct BSLocationData_s {
public:
    uint8_t cellType;
    uint32_t cellRegionID1;
    uint32_t cellRegionID2;
    uint32_t cellRegionID3;
    uint32_t cellRegionID4;

    float latitude;
    float longitude;

    // Use IZAT_BS_LOC.. bit masks
    int valid_bits;

    float horizontal_coverage_radius;
    uint8_t horizontal_confidence;
    uint8_t horizontal_reliability;
    float altitude;
    float altitude_uncertainty;
    uint8_t altitude_confidence;
    uint8_t altitude_reliability;

    inline BSLocationData_s() : cellType(0)
        , cellRegionID1(0)
        , cellRegionID2(0)
        , cellRegionID3(0)
        , cellRegionID4(0)
        , latitude(0)
        , longitude(0)
        , valid_bits(0)
        , horizontal_coverage_radius(0)
        , horizontal_confidence(0)
        , horizontal_reliability(0)
        , altitude(0)
        , altitude_uncertainty(0)
        , altitude_confidence(0)
        , altitude_reliability(0) {
    }
};

struct BSSpecialInfo_s {
public:
    uint8_t cellType;
    uint32_t cellRegionID1;
    uint32_t cellRegionID2;
    uint32_t cellRegionID3;
    uint32_t cellRegionID4;

    // Special Information on this BS
    // 0: BS information not available
    // 1: BS detected as a moving BS
    uint8_t info;

    inline BSSpecialInfo_s() : cellType(0)
        , cellRegionID1(0)
        , cellRegionID2(0)
        , cellRegionID3(0)
        , cellRegionID4(0)
        ,info(0) {
    }
};

typedef void (*wwanDBRecvStatusUpdateCb)(bool isSuccess, const char *errStr,
                                         void *clientData);
typedef void (*wwanDBRecvBsListUpdateCb)(BSInfo_s *bs_list, size_t bs_list_size,
                                         void *clientData);
typedef void (*wwanDBRecvServiceRequestCb)(void *clientData);
typedef void (*wwanDBRecvEnvNotifyCb)(void *clientData);

struct FdclCallbacks {
    wwanDBRecvBsListUpdateCb bsListAvailCb;
    wwanDBRecvStatusUpdateCb statusCb;
    wwanDBRecvServiceRequestCb serviceReqCb;
    wwanDBRecvEnvNotifyCb envNotifyCb;
};
struct FdclData {
    std::vector<BSLocationData_s> bsLocationDataList;
    std::vector<BSSpecialInfo_s> bsSpecialInfoList;
    int32_t daysValid;
};
struct FdclInfo {
    std::vector<BSInfo_s> bsInfoList;
};

/* registers callbacks for WWAN DB updater
   statusCb - callback to receive DB injection status from GTP
            Can not be NULL.
   bsListAvailCb  - callback to receive list of BSs from GTP
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
           is to be fed to the unregisterWWANDBUpdater() call.
*/
void* registerWWANDBUpdater(wwanDBRecvBsListUpdateCb bsListAvailCb,
                            wwanDBRecvStatusUpdateCb statusCb,
                            wwanDBRecvServiceRequestCb serviceReqCb,
                            wwanDBRecvEnvNotifyCb envNotifyCb,
                            void *clientData);

/* unregisters the client callback
   wwanDBUpdaterHandle - the opaque pointer from the return of
                           registerWWANADUpdater()
*/
void unregisterWWANDBUpdater(void *wwanADUpdaterHandle);

/* Send request to obatin list of BSs for which location assitance data is
   needed.
   expire_in_days - the number of days in future in which
                            the associated location of an BS if
                            available, will expire, to be fetched
                            by this request.
   wwanADUpdaterHandle - the opaque pointer from the return of
                           registerWWANADUpdater()
*/
void sendBSListRequest(int expire_in_days, void *wwanADUpdaterHandle);

/* Send location assitance data of BSs.
   bs_loc_list - List of BS location data
   bs_loc_list_size - number of BSs for which location data is available
   bs_spl_list - List of BSs with Special information
   bs_spl_list_size - number of BSs with special information available
   days_valid- number of days for which this location data is valid
   wwanADUpdaterHandle - the opaque pointer from the return of
                           registerWWANADUpdater()
*/
void pushWWANDB(BSLocationData_s *bs_loc_list[], size_t bs_loc_list_size,
                BSSpecialInfo_s *bs_spl_list[], size_t bs_spl_list_size,
                int days_valid, void *wwanADUpdaterHandle);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef __IZAT_MANAGER_WWANDBRECV_C_H__
