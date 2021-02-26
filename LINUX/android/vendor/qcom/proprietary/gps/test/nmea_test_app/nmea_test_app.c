/*
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <limits.h>
#include <errno.h>
#include "loc_api_v02_client.h"
#include "loc_api_sync_req.h"

locClientHandleType handle;
int id1 = 1;
int tty_fd;
// Nmea sentence types mask
typedef uint32_t NmeaSentenceTypesMask;
#define LOC_NMEA_MASK_GGA_V02   ((NmeaSentenceTypesMask)0x00000001) /**<  Enable GGA type  */
#define LOC_NMEA_MASK_RMC_V02   ((NmeaSentenceTypesMask)0x00000002) /**<  Enable RMC type  */
#define LOC_NMEA_MASK_GSV_V02   ((NmeaSentenceTypesMask)0x00000004) /**<  Enable GSV type  */
#define LOC_NMEA_MASK_GSA_V02   ((NmeaSentenceTypesMask)0x00000008) /**<  Enable GSA type  */
#define LOC_NMEA_MASK_VTG_V02   ((NmeaSentenceTypesMask)0x00000010) /**<  Enable VTG type  */
#define LOC_NMEA_MASK_PQXFI_V02 ((NmeaSentenceTypesMask)0x00000020) /**<  Enable PQXFI type  */
#define LOC_NMEA_MASK_PSTIS_V02 ((NmeaSentenceTypesMask)0x00000040) /**<  Enable PSTIS type  */
#define LOC_NMEA_MASK_GLGSV_V02 ((NmeaSentenceTypesMask)0x00000080) /**<  Enable GLGSV type  */
#define LOC_NMEA_MASK_GNGSA_V02 ((NmeaSentenceTypesMask)0x00000100) /**<  Enable GNGSA type  */
#define LOC_NMEA_MASK_GNGNS_V02 ((NmeaSentenceTypesMask)0x00000200) /**<  Enable GNGNS type  */
#define LOC_NMEA_MASK_GARMC_V02 ((NmeaSentenceTypesMask)0x00000400) /**<  Enable GARMC type  */
#define LOC_NMEA_MASK_GAGSV_V02 ((NmeaSentenceTypesMask)0x00000800) /**<  Enable GAGSV type  */
#define LOC_NMEA_MASK_GAGSA_V02 ((NmeaSentenceTypesMask)0x00001000) /**<  Enable GAGSA type  */
#define LOC_NMEA_MASK_GAVTG_V02 ((NmeaSentenceTypesMask)0x00002000) /**<  Enable GAVTG type  */
#define LOC_NMEA_MASK_GAGGA_V02 ((NmeaSentenceTypesMask)0x00004000) /**<  Enable GAGGA type  */
#define LOC_NMEA_MASK_PQGSA_V02 ((NmeaSentenceTypesMask)0x00008000) /**<  Enable PQGSA type  */
#define LOC_NMEA_MASK_PQGSV_V02 ((NmeaSentenceTypesMask)0x00010000) /**<  Enable PQGSV type  */
#define LOC_NMEA_ALL_SUPPORTED_MASK  (LOC_NMEA_MASK_GGA_V02 | LOC_NMEA_MASK_RMC_V02 | \
              LOC_NMEA_MASK_GSV_V02 | LOC_NMEA_MASK_GSA_V02 | LOC_NMEA_MASK_VTG_V02 | \
        LOC_NMEA_MASK_PQXFI_V02 | LOC_NMEA_MASK_PSTIS_V02 | LOC_NMEA_MASK_GLGSV_V02 | \
        LOC_NMEA_MASK_GNGSA_V02 | LOC_NMEA_MASK_GNGNS_V02 | LOC_NMEA_MASK_GARMC_V02 | \
        LOC_NMEA_MASK_GAGSV_V02 | LOC_NMEA_MASK_GAGSA_V02 | LOC_NMEA_MASK_GAVTG_V02 | \
        LOC_NMEA_MASK_GAGGA_V02 | LOC_NMEA_MASK_PQGSA_V02 | LOC_NMEA_MASK_PQGSV_V02 )

#define SERIAL_PORT "/dev/ttyHSL0"
#define NMEA_PORT "/dev/ttyGS0"

/** Structure that holds the command line options given to main **/
typedef struct command_line_options {
    int t;                        // time to stop fix in seconds
    int deleteAidingData;         // delete all aiding data.
    int positionMode;             // Specifies Position mode.
    int interval;                 // Time in milliseconds between fixes.
    int printNmea;                // Print nmea string
    int tracking;                 // start tracking session
    int nmeaTypes;                // nmea subscription mask
} CommandLineOptionsType;

// Default values
static CommandLineOptionsType sOptions = {
    100,                         // Time to stop fix.
    0,                           // By default don't delete aiding data.
    0,                           // Standalone mode.
    1000,                        // 1000 millis between fixes
    0,                           // Print NMEA
    0,                           // 1 millisecond
    LOC_NMEA_ALL_SUPPORTED_MASK, // Subscribe for all
};


void eventIndCb(
    locClientHandleType handle,
    uint32_t eventIndId,
    const locClientEventIndUnionType eventIndPayload,
    void *pClientCookie)
{
    int *pIndex = pClientCookie;
    int n;
    char buffer[201] = {0};

    switch(eventIndId) {
    case QMI_LOC_EVENT_NMEA_IND_V02:
        if(sOptions.printNmea)
            printf("NMEA: %s\n", eventIndPayload.pNmeaReportEvent->nmea);
        strlcpy(buffer, eventIndPayload.pNmeaReportEvent->nmea, sizeof(buffer));
        /* NMEA strings contains "\r\n" at end of the string, remove '\r' and send it */
        char* p = strchr(buffer, '\r');
        if ((NULL != p) && (*(p + 1) == '\n')) {
            *p = '\n';
            *(p + 1) = '\0';
        }
        n = write(tty_fd, buffer, strlen(eventIndPayload.pNmeaReportEvent->nmea) - 1);
        if (n < 0)
           printf("write() of %d bytes failed!\n", n);
        break;
    default:
        printf("Unsupported eventID %d\n", eventIndId);
        break;
    }
}

void  respIndCb(
    locClientHandleType handle,
    uint32_t respIndId,
    const locClientRespIndUnionType respIndPayload,
    uint32_t respIndPayloadSize,
    void *pClientCookie)
{
    int *pIndex = pClientCookie;
    if(sOptions.printNmea)
        printf("respIndCb received ind %d from handle %p \n", respIndId, handle);

    // process the sync call
    // use pDeleteAssistDataInd as a dummy pointer
    loc_sync_process_ind(handle, respIndId,
          (void *)respIndPayload.pDeleteAssistDataInd, respIndPayloadSize);
}
static void errorCb(
    locClientHandleType handle,
    locClientErrorEnumType errorId,
    void *pClientCookie)
{
    int *pIndex = pClientCookie;
    printf("received errorId%d from handle %p"
          "with index %d\n", __func__, __LINE__, errorId,
          handle, *pIndex);
}


locClientCallbacksType globalCallbacks = {
    sizeof(locClientCallbacksType),
    eventIndCb,
    respIndCb,
    errorCb
};
/* ser NMEA types */
int setNMEATypes ()
{
    locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
    locClientReqUnionType req_union;

    qmiLocSetNmeaTypesReqMsgT_v02 setNmeaTypesReqMsg;
    qmiLocSetNmeaTypesIndMsgT_v02 setNmeaTypesIndMsg;

    printf("Set NMEA Types  = 0%x\n", sOptions.nmeaTypes);

    memset(&setNmeaTypesReqMsg, 0, sizeof(setNmeaTypesReqMsg));
    memset(&setNmeaTypesIndMsg, 0, sizeof(setNmeaTypesIndMsg));

    setNmeaTypesReqMsg.nmeaSentenceType = sOptions.nmeaTypes;

    req_union.pSetNmeaTypesReq = &setNmeaTypesReqMsg;

    result = loc_sync_send_req (handle,
                                QMI_LOC_SET_NMEA_TYPES_REQ_V02,
                                req_union,
                                LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                                QMI_LOC_SET_NMEA_TYPES_IND_V02,
                                &setNmeaTypesIndMsg);

    // if success
    if ( result != eLOC_CLIENT_SUCCESS )
    {
        printf("setNMEATypes loc_sync_send_req Error result = %d \n", result);
    }

    return result;
}
/* start a positioning session */
int startFix(int tbf)
{
    locClientStatusEnumType status;
    locClientReqUnionType req_union;

    printf("Start Fix.. \n");
    qmiLocStartReqMsgT_v02 start_msg;
    // clear all fields, validity masks
    memset(&start_msg, 0, sizeof(start_msg));
    start_msg.minInterval_valid = 1;
    start_msg.minInterval = tbf;
    start_msg.horizontalAccuracyLevel_valid = 1;
    start_msg.horizontalAccuracyLevel =  eQMI_LOC_ACCURACY_HIGH_V02;
    start_msg.fixRecurrence_valid = 1;
    start_msg.fixRecurrence = eQMI_LOC_RECURRENCE_PERIODIC_V02;

    start_msg.sessionId = 1;
    req_union.pStartReq = &start_msg;

    status = locClientSendReq(handle, QMI_LOC_START_REQ_V02, req_union);

    return 0;
}
/* stop a positioning session */
int stopFix()
{
    locClientStatusEnumType status;
    locClientReqUnionType req_union;

    qmiLocStopReqMsgT_v02 stop_msg;

    printf("Stop Fix .. \n");

    memset(&stop_msg, 0, sizeof(stop_msg));

    // dummy session id
    stop_msg.sessionId = 1;
    req_union.pStopReq = &stop_msg;
    status = locClientSendReq(handle,
                              QMI_LOC_STOP_REQ_V02,
                              req_union);

    if( eLOC_CLIENT_SUCCESS != status)
    {
        printf(" error = %d\n",status);
    }

    return 0;
}

/*
* 'open_port()' - Open NMEA port.
*/
int open_port(void)
{
    int n; /* File descriptor for the port */

    printf("open_port \n");
    tty_fd = open(NMEA_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (tty_fd == -1)
    {
        /* Could not open the port. */
        printf("open_port: Unable to open %s \n", NMEA_PORT);
    }
    printf ("open_port success \n");
}

int delete9x07AidingData()
{
    locClientReqUnionType req_union;
    locClientStatusEnumType status = eLOC_CLIENT_FAILURE_UNSUPPORTED;

    qmiLocDeleteAssistDataReqMsgT_v02 delete_req;
    qmiLocDeleteAssistDataIndMsgT_v02 delete_resp;

    memset(&delete_req, 0, sizeof(delete_req));
    memset(&delete_resp, 0, sizeof(delete_resp));

    delete_req.deleteAllFlag = true;

    req_union.pDeleteAssistDataReq = &delete_req;

    status = loc_sync_send_req(handle,
                               QMI_LOC_DELETE_ASSIST_DATA_REQ_V02,
                               req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                               QMI_LOC_DELETE_ASSIST_DATA_IND_V02,
                               &delete_resp);

    if (status != eLOC_CLIENT_SUCCESS ||
          eQMI_LOC_SUCCESS_V02 != delete_resp.status)
    {
        printf("response QMI_LOC_DELETE_ASSIST_DATA_REQ_V02 status = %d, \n",status);
    }
    return 0;
}

int deleteAidingData()
{
    static bool isNewApiSupported = true;
    locClientReqUnionType req_union;
    locClientStatusEnumType status = eLOC_CLIENT_FAILURE_UNSUPPORTED;
    printf("deleteAidingData \n");

    // Use the new API first
    qmiLocDeleteGNSSServiceDataReqMsgT_v02 delete_gnss_req;
    qmiLocDeleteGNSSServiceDataIndMsgT_v02 delete_gnss_resp;

    memset(&delete_gnss_req, 0, sizeof(delete_gnss_req));
    memset(&delete_gnss_resp, 0, sizeof(delete_gnss_resp));

    delete_gnss_req.deleteAllFlag = true;
    req_union.pDeleteGNSSServiceDataReq = &delete_gnss_req;

    status = loc_sync_send_req(handle,
                               QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02,
                               req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                               QMI_LOC_DELETE_GNSS_SERVICE_DATA_IND_V02,
                               &delete_gnss_resp);

    if (status != eLOC_CLIENT_SUCCESS ||
        eQMI_LOC_SUCCESS_V02 != delete_gnss_resp.status)
    {
        printf("response QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02 status = %d, \n",status);
    }

    if (eLOC_CLIENT_FAILURE_UNSUPPORTED == status) {
        // If the new API is not supported we fall back on the old one
        printf("%s:%d]: QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02 not supported"
          "We use QMI_LOC_DELETE_ASSIST_DATA_REQ_V02\n",
          __func__, __LINE__);

        qmiLocDeleteAssistDataReqMsgT_v02 delete_req;
        qmiLocDeleteAssistDataIndMsgT_v02 delete_resp;

        memset(&delete_req, 0, sizeof(delete_req));
        memset(&delete_resp, 0, sizeof(delete_resp));

        delete_req.deleteAllFlag = true;

        req_union.pDeleteAssistDataReq = &delete_req;

        status = loc_sync_send_req(handle,
                                   QMI_LOC_DELETE_ASSIST_DATA_REQ_V02,
                                   req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                                   QMI_LOC_DELETE_ASSIST_DATA_IND_V02,
                                   &delete_resp);

        if (status != eLOC_CLIENT_SUCCESS ||
            eQMI_LOC_SUCCESS_V02 != delete_resp.status)
        {
            printf("response QMI_LOC_DELETE_ASSIST_DATA_REQ_V02 status = %d, \n",status);
        }
    }
    return 0;
}

void printHelp(char **arg)
{
    printf("usage: %s -t <time out in sec> -i <tbf msec> -d\n", arg[0]);
    printf("    -t:  Time out, Defaults: %d\n", sOptions.t);
    printf("    -d:  Delete all aiding data, Defaults: %d\n", sOptions.deleteAidingData);
    printf("    -i:  Interval. Time in milliseconds between fixes, Defaults: %d\n",
           sOptions.interval);
    printf("    -m:  nmea types subscription mask\n"
           "         NMEA_MASK_GGA        (0x00000001) Enable GGA type\n"
           "         NMEA_MASK_RMC        (0x00000002) Enable RMC type\n"
           "         NMEA_MASK_GSV        (0x00000004) Enable GSV type\n"
           "         NMEA_MASK_GSA        (0x00000008) Enable GSA type\n"
           "         NMEA_MASK_VTG        (0x00000010) Enable VTG type\n"
           "         NMEA_MASK_PQXFI      (0x00000020) Enable PQXFI type\n"
           "         NMEA_MASK_PSTIS      (0x00000040) Enable PSTIS type\n"
           "         NMEA_MASK_GLGSV      (0x00000080) Enable GLGSV type\n"
           "         NMEA_MASK_GNGSA      (0x00000100) Enable GNGSA type\n"
           "         NMEA_MASK_GNGNS      (0x00000200) Enable GNGNS type\n"
           "         NMEA_MASK_GARMC      (0x00000400) Enable GARMC type\n"
           "         NMEA_MASK_GAGSV      (0x00000800) Enable GAGSV type\n"
           "         NMEA_MASK_GAGSA      (0x00001000) Enable GAGSA type\n"
           "         NMEA_MASK_GAVTG      (0x00002000) Enable GAVTG type\n"
           "         NMEA_MASK_GAGGA      (0x00004000) Enable GAGGA type\n"
           "         NMEA_MASK_PQGSA      (0x00008000) Enable PQGSA type\n"
           "         NMEA_MASK_PQGSV      (0x00010000) Enable PQGSV type\n"
           "         NMEA_MASK_ALL        (0x0001FFFF) Enable ALL types \n" );

    printf("    -p:  Print NMEA strings, Defaults: %d\n", sOptions.printNmea);
    printf("    -h:  print this help\n");
}

int main(int argc, char** argv)
{
    int opt;
    extern char *optarg;
    char *endptr;
    locClientStatusEnumType status;

    while ((opt = getopt (argc, argv, "t:d:i:m:p:z")) != -1) {
        switch (opt) {
        case 't':
            sOptions.t = atoi(optarg);
            printf("Timeout: %d \n", sOptions.t);
            break;
        case 'd':
            sOptions.deleteAidingData = atoi(optarg);
            printf("Delete Aiding Data: %d \n", sOptions.deleteAidingData);
            break;
        case 'i':
            sOptions.interval = atoi(optarg);
            printf("Interval: %d \n", sOptions.interval);
            break;
        case 'm':
            sOptions.nmeaTypes = strtol(optarg, NULL, 16);
            printf("NMEA Types :0x%x \n", sOptions.nmeaTypes);
            break;
        case 'p':
            sOptions.printNmea = atoi(optarg);
            printf("Print NMEA strings :%d \n", sOptions.printNmea);
            break;
        case 'h':
        default:
            printHelp(argv);
            return 0;
        }
    }

    open_port();
    status = locClientOpen(QMI_LOC_EVENT_MASK_NMEA_V02,
                           &globalCallbacks,
                           &handle,
                           (void *)&id1);
    if (eLOC_CLIENT_SUCCESS != status ||
        handle == LOC_CLIENT_INVALID_HANDLE_VALUE )
    {
        printf ("locClientOpen failed, status = %d\n", status);
        return -1;
    }else {
        printf ("locClientOpen success \n");
        setNMEATypes();
        if (sOptions.deleteAidingData)
            deleteAidingData();
        //delete9x07AidingData();
        startFix(sOptions.interval);
        printf("Waiting for %d seconds \n", sOptions.t);
        sleep(sOptions.t);
        printf("Issue stop fix.. \n");
        stopFix();
        printf("locClientClose.. \n");
        locClientClose(&handle);
        printf("closing tty_fd.. \n");
        close(tty_fd);
    }
    return 0;
}
