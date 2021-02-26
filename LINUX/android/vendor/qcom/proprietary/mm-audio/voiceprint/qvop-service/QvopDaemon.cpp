/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include <stdio.h>
#include <sys/stat.h>
#include <cutils/properties.h>

#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <android/log.h>

#include "logfile.h"
#include "QtiVoicePrintService.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

static char const* const QVOP_DIRECTORY = "/data/misc/qvop";
static char const* const QVOP_LOG_FILE = "/data/misc/qvop/qvop-log.txt";


using namespace android;

/**
 * @brief daemon for starting the voiceprint service
 * @details VoicePrint daemon that starts the voice print service at startup.
 *
 * @param argc
 * @param argv
 *
 * @return
 */
int main(int argc, char* argv[]) {
    uint8_t logLevel = QVOP_LOG_INFO;
    logfile_init(true);
    //logfile_print_i("%s: App build date: %s %s", QVOP_FN, __DATE__, __TIME__);

    // clear the log file
    logfile_print_e("%s: voiceprint clearing logfile %s", QVOP_FN, QVOP_LOG_FILE);
    FILE* fp = fopen(QVOP_LOG_FILE, "wb");
    if (fp) {
        fclose(fp);
    }

    char buf[PROPERTY_VALUE_MAX] = { };
    bool enable = property_get("persist.vendor.qvop", buf, "false") >= 0 &&
                strncmp(buf, "true", sizeof (buf)) == 0;
    if (!enable) {
        // voiceprint is not enabled, suspend
        logfile_print_i("%s: voiceprint not enabled, suspending", QVOP_FN);
        pause();
        return 0;
    }
    memset(buf, 0, sizeof(buf));

    //adjust log level
    if (property_get("persist.vendor.qvop.tag.QVOP", buf, "3") >= 0) {
        logLevel = atoi(buf);
    }
    logfile_set(logLevel);

    logfile_print_i("%s: voiceprint enabled, starting service at log level %d", QVOP_FN, logLevel);
    signal(SIGPIPE, SIG_IGN);

    //Instantiate the service that supports HIDL interface
    sp<QtiVoicePrintService> aosp = QtiVoicePrintService::getInstance();
    if (aosp != NULL) {
        aosp->registerAsService();
        joinRpcThreadpool();
    }
    else {
        logfile_print_e("%s:Can't create instance of QtiVoicePrintService, nullptr", QVOP_FN);
    }


    logfile_print_i("%s: exit", QVOP_FN);

    return 0;
}
