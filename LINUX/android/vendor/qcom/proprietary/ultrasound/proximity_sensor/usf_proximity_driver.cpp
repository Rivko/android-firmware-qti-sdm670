/*===========================================================================
                           usf_proximity_driver.cpp

 DESCRIPTION: Implements the USF Proximity Driver

 Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#define LOG_TAG "usf_proximity_driver"

/*-----------------------------------------------------------------------------
 Includes
 -----------------------------------------------------------------------------*/
#include "usf_proximity_driver.h"
#include "check_call.h"
#include "mutex_lock.h"
#include <ProximityExports.h>
#include <cutils/properties.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <pthread.h>
#include <log/log.h>

/*-----------------------------------------------------------------------------
 Macros and constants
 -----------------------------------------------------------------------------*/
static const char SOCK_PATH[] = "/data/usf/proximity/data_socket";
static const char CMD_PIPE_PATH[] = "/data/usf/proximity/cmd";
static const char CMD_START[] = "start";
static const char CMD_STOP[] = "stop";

const int UsfProximityDriver::FAR_DISTANCE_MARKER = -1;
int UsfProximityDriver::sSensorHandle = 0;

static const int PROXIMITY_VALUE_FAR = -1; // special flag showing we sent "far"
static const int PROXIMITY_VALUE_NEAR = 10; // in mm
static const int PROXIMITY_VALUE_INIT = -2;
static const int PAYLOAD_LENGTH_IN_WORDS = 3;
static const int CONNECTION_RETRY_COUNT = 10000;
static const int CONNECTION_RETRY_TIMEOUT = 1000;
static const int RESTART_ON_NTH_CONNECTION_RETRY = 50;

/*-----------------------------------------------------------------------------
 Functions
 -----------------------------------------------------------------------------*/

/*============================================================================
 FUNCTION:  nullKeepAliveCallback
 ============================================================================*/
/**
 Null object for the keep alive callback
 */
static void nullKeepAliveCallback(void *unusedArg, int unusedValue) {
    (void) unusedArg;
    (void) unusedValue;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::UsfProximityDriver
 ============================================================================*/
/**
 C'tor for USF Proximity Driver. Initializes pthread and event primitives.
 */
UsfProximityDriver::UsfProximityDriver(UnSocketReceiver *socketArg) :
        socket(socketArg), shutdownRequested(false), threadRunning(false), lastProximityValue(
                PROXIMITY_VALUE_INIT), hasUnreadProximityData(false), thread(0), keepAliveCallback(
                nullKeepAliveCallback), keepAliveCallbackArg(0), flushRequested(false) {
    CHECK_CALL(pthread_mutex_init(&mutex, NULL));
    CHECK_CALL(pthread_cond_init(&cond, NULL));
    CHECK_CALL(wakeSocketThreadEvent = eventfd(0, EFD_NONBLOCK));
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::~UsfProximityDriver
 ============================================================================*/
/**
 D'tor for USF Proximity Driver. Stops driver, releases resources.
 */
UsfProximityDriver::~UsfProximityDriver() {
    stop();
    CHECK_CALL(pthread_mutex_destroy(&mutex));
    CHECK_CALL(pthread_cond_destroy(&cond));
    CHECK_CALL(close(wakeSocketThreadEvent));
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::socketReaderThreadHook
 ============================================================================*/
/**
 Static method acts as class method hook for phtread_create call.
 */
void *UsfProximityDriver::socketReaderThreadHook(void *pThis) {
    ((UsfProximityDriver*) pThis)->socketReaderThread();
    return 0;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::allowChangeCfgLink
 ============================================================================*/
/**
 Check the property that indicates if changing the cfg symbolic link is allowed
 */
bool UsfProximityDriver::allowChangeCfgLink() {
    char forceUsingCfg[PROPERTY_VALUE_MAX];
    const char* forceUsingCfgProperty = "persist.sys.usf.force_using_cfg";

    property_get(forceUsingCfgProperty, forceUsingCfg, "0");
    if (0 != strncmp("0", forceUsingCfg, PROPERTY_VALUE_MAX)){
        ALOGD("%s: not changing cfg, property %s = [%s]",
              __FUNCTION__, forceUsingCfgProperty, forceUsingCfg);
        return false;
    }
    ALOGD("%s: changing cfg, property %s = [%s]",
          __FUNCTION__, forceUsingCfgProperty, forceUsingCfg);
    return true;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::changeCfgLink
 ============================================================================*/
/**
 Change the cfg symbolic link
 */
int UsfProximityDriver::changeCfgLink() {
    const unsigned int cfgNameMaxLen = 512;
    char cfgDir[cfgNameMaxLen];
    char* platform = NULL;
    char newCfgFile[cfgNameMaxLen];
    const char* cfgDirSymLink = "/data/usf/proximity/cfg";
    const char* proximitySymLink = "/data/usf/proximity/usf_proximity.cfg";
    unsigned int count = 0;
    const char* sensorHandleCfgPrefix = NULL;

    if (!allowChangeCfgLink()) {
        return 0;
    }

    // find the target name from the cfg directory name
    count = readlink(cfgDirSymLink, cfgDir, cfgNameMaxLen);
    if (count >= strlen(cfgDirSymLink) + strlen("_")) {
        cfgDir[count] = '\0';
        platform = &cfgDir[strlen(cfgDirSymLink) + strlen("_")];
    } else {
        ALOGW("%s: couldn't read cfg directory symbolic link", __FUNCTION__);
        return -1;
    }

    if (SENSOR_HANDLE == sSensorHandle) {
        sensorHandleCfgPrefix = "usf_proximity";
    } else if (SENSOR_POCKET_HANDLE == sSensorHandle) {
        sensorHandleCfgPrefix = "usf_pocket";
    } else {
      ALOGW("%s: unsupported sensor handle %d", __FUNCTION__, sSensorHandle);
      return -1;
    }

    snprintf(newCfgFile, cfgNameMaxLen, "%s/%s_%s.cfg",
             cfgDirSymLink, sensorHandleCfgPrefix, platform);
    ALOGD("%s: newCfgFile: %s", __FUNCTION__, newCfgFile);

    remove(proximitySymLink);
    return symlink(newCfgFile, proximitySymLink);
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::startDaemon
 ============================================================================*/
/**
 Starts the daemon
 */
void UsfProximityDriver::startDaemon() {
    ALOGD("%s: handle %d", __FUNCTION__, sSensorHandle);
    char state[PROPERTY_VALUE_MAX];
    property_get("init.svc.usf_proximity", state, "[none]");
    ALOGD("%s: current state: %s", __FUNCTION__, state);

    if(changeCfgLink()) {
        ALOGW("%s: error creating soft link", __FUNCTION__);
    }

    property_set("ctl.start", "usf_proximity");
    int fd;
    CHECK_CALL(fd = open(CMD_PIPE_PATH, O_WRONLY));
    if (fd != -1) {
      CHECK_CALL(write(fd, CMD_START, sizeof(CMD_START)));
      CHECK_CALL(close(fd));
    }
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::stopDaemon
 ============================================================================*/
/**
 Stops the daemon
 */
void UsfProximityDriver::stopDaemon() {
    ALOGD("%s: handle %d", __FUNCTION__, sSensorHandle);
    int fd;
    CHECK_CALL(fd = open(CMD_PIPE_PATH, O_WRONLY));
    if (fd != -1) {
      CHECK_CALL(write(fd, CMD_STOP, sizeof(CMD_STOP)));
      CHECK_CALL(close(fd));
    }
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::getProperty
 ============================================================================*/
/**
 Gets usf_proximity daemon state
 */
void UsfProximityDriver::getDaemonState(char *state) {
    property_get("init.svc.usf_proximity", state, "[none]");
    return;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::doUsleep
 ============================================================================*/
/**
 Performs the usleep call; virtual to be overridden in unit tests
 */
void UsfProximityDriver::doUsleep(useconds_t useconds) {
    ::usleep(useconds);
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::startDaemonAndConnectToSocket
 ============================================================================*/
/**
  Attempts to connect to the socket. Will periodically try to restart daemon
  during the connection. Returns if shutdown requested in the middle.
 */
int UsfProximityDriver::startDaemonAndConnectToSocket() {
    ALOGD("%s: connecting", __FUNCTION__);
    startDaemon();
    socket->open(SOCK_PATH);
    int lastConnectRetval = 0;
    for (int retryCount = 1 ; retryCount < CONNECTION_RETRY_COUNT && !shutdownRequested; retryCount++) {
        if (retryCount % RESTART_ON_NTH_CONNECTION_RETRY == 0) {
            startDaemon();
        }
        lastConnectRetval = socket->connect();
        if (0 == lastConnectRetval) {
            ALOGD("%s: socket connected in %d attempts", __FUNCTION__,
                    retryCount);
            return 0;
        } else {
            // bail without retry if got EACCES
            if (errno == EACCES) {
                CHECK_CALL(lastConnectRetval);
                return -1;
            }
            doUsleep(CONNECTION_RETRY_TIMEOUT);
        }
    }
    if (shutdownRequested) {
        ALOGD("%s: canceling connect, shutdown requested", __FUNCTION__);
    } else {
        ALOGE("%s: failed to connect, giving up", __FUNCTION__);
        CHECK_CALL(lastConnectRetval);
    }
    return -1;
}

void UsfProximityDriver::tearDownThread() {
    threadRunning = false;
    socket->close();
    stopDaemon();
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::socketReaderThread
 ============================================================================*/
/**
 Socket reader thread:
 - Start the USF Proximity daemon.
 - Connect to the data socket.
 - Main loop:
 -- Wait for socket data or shutdown request.
 -- On socket data, parse data notify blocked polling call.
 - On shutdown request:
 -- Close socket.
 -- Stop daemon.
 */
void UsfProximityDriver::socketReaderThread() {
    while (!shutdownRequested) {
        blockOnSocketOrWakeEvent(socket->getFd());
        if (shutdownRequested) {
            ALOGD("%s: shutdown requested, breaking", __FUNCTION__);
            break;
        }
        int32_t payload[PAYLOAD_LENGTH_IN_WORDS];
        int payloadSize = sizeof(int32_t) * PAYLOAD_LENGTH_IN_WORDS;
        int recvBytes;
        CHECK_CALL(recvBytes = socket->receivePacket(payload, payloadSize));
        ALOGD("%s: received %d bytes: [%d %d %d]", __FUNCTION__, recvBytes,
                payload[0], payload[1], payload[2]);
        ALOGD("%s: handle %d", __FUNCTION__, sSensorHandle);
        if (recvBytes == payloadSize) {
            MutexLock lock(mutex);
            // for SENSOR_HANDLE and SENSOR_POCKET_HANDLE

            switch (payload[0]) {
            case QC_US_PROXIMITY_LIB_RESULT_DETECTED:
                // distance reporting is not reliable and may cause bad proximity reports,
                // so we report a default distance
                onNewProximityValue(PROXIMITY_VALUE_NEAR);
                break;
            case QC_US_PROXIMITY_LIB_RESULT_IDLE:
                onNewProximityValue(PROXIMITY_VALUE_FAR);
                break;
            case QC_US_PROXIMITY_LIB_RESULT_NOT_READY:
                // do nothing; we know daemon is alive
                break;
            case QC_US_PROXIMITY_KEEP_ALIVE:
                keepAliveCallback(keepAliveCallbackArg, payload[1]);
                break;
            default:
                ALOGW("%s: unhandled event from socket, result: %d",
                        __FUNCTION__, payload[0]);
                break;
            }
        } else {
            ALOGW("%s: Broken pipe, trying to reconnect..", __FUNCTION__);
            if (0 != startDaemonAndConnectToSocket()) {
                // connectToSocket will log reason for failure.
                // Can fail if daemon never opens socket or shutdown request.
                break;
            }
        }
    }
    tearDownThread();
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::onNewProximityValue
 ============================================================================*/
/**
 Helper function to handle updated proximity values from daemon
 */
void UsfProximityDriver::onNewProximityValue(int newProximityValue) {
    ALOGD("%s", __FUNCTION__);
    if (lastProximityValue != newProximityValue) {
        hasUnreadProximityData = true;
        lastProximityValue = newProximityValue;
        pthread_cond_broadcast(&cond);
    }

}

/*============================================================================
 FUNCTION:  UsfProximityDriver::start
 ============================================================================*/
/**
 Starts the socket reader thread which starts the daemon.
 */
int UsfProximityDriver::start(int sensorHandle) {
    ALOGD("%s", __FUNCTION__);
    shutdownRequested = false;
    lastProximityValue = PROXIMITY_VALUE_INIT;
    sSensorHandle = sensorHandle;

    // If prior run was ended with a call to stop when the thread wasn't
    // blocked, the wake event wasn't cleared.
    // Read from it here to clear it.
    // An "EAGAIN" error is expected and shouldn't emit an error log.

    uint64_t sig;
    int retval = read(wakeSocketThreadEvent, &sig, sizeof(sig));
    if (retval == -1 && errno == EAGAIN) {
        // this is fine & expected
    } else {
        CHECK_CALL(retval);
    }

    if (0 != startDaemonAndConnectToSocket()) {
        return -1;
    }
    threadRunning = true;
    CHECK_CALL(pthread_create(&thread, NULL, socketReaderThreadHook, this));
    return 0;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::stop
 ============================================================================*/
/**
 Shut down the socket reader thread, which will stop the daemon.
 */
int UsfProximityDriver::stop() {
    ALOGD("%s: client requesting exit", __FUNCTION__);
    shutdownRequested = true;
    int64_t wake = 1;
    CHECK_CALL(write(wakeSocketThreadEvent, &wake, sizeof(wake)));
    if (threadRunning) {
        CHECK_CALL(pthread_join(thread, NULL));
    }
    return 0;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::poll
 ============================================================================*/
/**
 Block until new data from socket is returned. This is usually called from a
 separate thread context than the rest of the calls to this class.
 */
int UsfProximityDriver::poll(int *distance) {
    ALOGD("%s", __FUNCTION__);

    MutexLock lock(mutex);
    while (shutdownRequested || (!hasUnreadProximityData && !flushRequested)) {
        pthread_cond_wait(&cond, &mutex);
    }
    if (flushRequested) {
        flushRequested = false;
        return POLL_RETVAL_FLUSH;
    }
    hasUnreadProximityData = false;
    *distance = lastProximityValue;
    return POLL_RETVAL_DATA;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::blockOnSocketOrWakeEvent
 ============================================================================*/
/**
 Helper function to manage waiting on socket or eventFd.
 */
int UsfProximityDriver::blockOnSocketOrWakeEvent(int sockFd) {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sockFd, &readSet);
    FD_SET(wakeSocketThreadEvent, &readSet);
    int maxFd = sockFd > wakeSocketThreadEvent ? sockFd : wakeSocketThreadEvent;
    int numSignalsSet;
    ALOGD("%s: entering pselect", __FUNCTION__);
    CHECK_CALL(
            numSignalsSet = pselect(maxFd + 1, &readSet, NULL, NULL, NULL, NULL));
    ALOGD("%s: pselect %d readSet:%s%s", __FUNCTION__, numSignalsSet,
            FD_ISSET(sockFd, &readSet) ? " socket" : "",
            FD_ISSET(wakeSocketThreadEvent, &readSet) ? " wake" : "");
    if (FD_ISSET(wakeSocketThreadEvent, &readSet)) {
        uint64_t sig;
        CHECK_CALL(read(wakeSocketThreadEvent, &sig, sizeof(sig)));
    }
    return numSignalsSet;
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::registerKeepAliveCallback
 ============================================================================*/
/**
 Registers for a callback to be triggered when "keep alive" packets are
 received.
 */
void UsfProximityDriver::registerKeepAliveCallback(void (callback)(void *, int),
        void *arg) {
    if (callback == NULL) {
        callback = nullKeepAliveCallback;
    } else {
        keepAliveCallback = callback;
        keepAliveCallbackArg = arg;
    }
}

/*============================================================================
 FUNCTION:  UsfProximityDriver::flush
 ============================================================================*/
/**
 Set the flushRequested flag so poll would return with flush complete message.
 */
int UsfProximityDriver::flush() {
    MutexLock lock(mutex);
    flushRequested = true;
    pthread_cond_broadcast(&cond);
    return 0;
}
