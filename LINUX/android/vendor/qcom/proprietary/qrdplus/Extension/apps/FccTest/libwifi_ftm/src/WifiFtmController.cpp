/*
 * Copyright (c) 2013, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "WifiFtmController.h"
#include "WifiFtmTest.h"
#include <string.h>

#include <sys/socket.h>

#include <cutils/sockets.h>
#include <cutils/properties.h>

#define LOG_TAG "WifiFtm"
#include <cutils/log.h>

WifiFtmController::WifiFtmController(const char* sock_name) :
        test(0), sock_fd(-1) {

    if ((sock_fd = android_get_control_socket(sock_name)) < 0)
        ALOGE("WifiFtmController: create cntl socket failed");

}

WifiFtmController::~WifiFtmController() {
    if (sock_fd >= 0)
        close(sock_fd);
}

void WifiFtmController::start() {
    if (sock_fd < 0)
        return;

    test = WifiFtmTest::createWifiFtmTest();

    while (true) {
        char buf[4096];
        client_len = sizeof(client);
        int res = recvfrom(sock_fd, buf, sizeof(buf) - 1, 0,
                (struct sockaddr *) &client, &client_len);
        if (res < 0) {
            ALOGE("recvfrom socket failed %s", strerror(errno));
            break;
        }

        buf[res] = '\0';
        if (processMessage(buf) <= 0)
            break;

    }

    ALOGE("start exit");

    delete test;

}

int WifiFtmController::processMessage(const char* msg) {
    ALOGE("%s", msg);
    if (strncmp("START ", msg, 6) == 0) {
        return processStartCmd(msg + 6);
    }
    if (strncmp("STOP ", msg, 5) == 0)
        return processStopCmd(msg + 5);

    if (strncmp("SET ", msg, 4) == 0)
        return processSetCmd(msg + 4);

    if (strncmp("REPORT ", msg, 7) == 0)
        return processReportCmd(msg + 7);

    ALOGE("Ignore unkown command: %s", msg);

    return 1;

}

int WifiFtmController::processStartCmd(const char* msg) {
    if (strncmp("FTM", msg, 3) == 0)
        test->startFtm();

    else if (strncmp("TX", msg, 2) == 0)
        test->startTx();

    else if (strncmp("RX", msg, 2) == 0) {

        test->startRx();
    } else if (strncmp("SCW", msg, 3) == 0)
        test->startSCW();

    return 1;

}

int WifiFtmController::processStopCmd(const char* msg) {
    if (strncmp("FTM", msg, 3) == 0) {
        test->stopFtm();
        return 0;
    }

    if (strncmp("TX", msg, 2) == 0)
        test->stopTx();

    else if (strncmp("RX", msg, 2) == 0)
        test->stopRx();

    else if (strncmp("SCW", msg, 3) == 0)
        test->stopSCW();
    return 1;

}

int WifiFtmController::processSetCmd(const char* msg) {
    char* value = strchr(msg, ' ');
    if (value == NULL)
        return 1; //ignore invalid value
    while (*value == ' ')
        *value++ = '\0';

    if (strncmp("CHANNEL", msg, 7) == 0) {
        test->setChannel(atoi(value));
    }else if (strncmp("CB", msg, 7) == 0) {
        test->setCB(atoi(value));
    }else if (strncmp("TX_POWER", msg, 8) == 0) {
        test->setTxPower(atoi(value));
    } else if (strncmp("TX_RATE", msg, 7) == 0) {
        test->setTxRate(value);
    } else if (strncmp("PWR_MODE", msg, 8) == 0) {
        test->setPowerMode(atoi(value));
    }  else if (strncmp("PACKET_LEN", msg, 10) == 0) {
        test->setPacketLen(atoi(value));
    }  else if (strncmp("PACKET_CNT", msg, 10) == 0) {
        test->setPacketCnt(atoi(value));
    }  else if (strncmp("PACKET_INTERVAL", msg, 15) == 0) {
        test->setPacketInterval(atoi(value));
    }

    return 1;
}

int WifiFtmController::processReportCmd(const char* msg) {
    if (strncmp("RX", msg, 2) == 0) {
        int rx_pkt_count = test->reportRx();
        char buf[32];
        memset(buf, 0, 32);
        sprintf(buf, "%d packets received", rx_pkt_count);
        ALOGD("%s", buf);
        property_set("wifi.ftmd.reply", buf);

    } else if (strncmp("BAND", msg, 4) == 0) {
        int band = test->reportBand();
        char buf[32];
        memset(buf, 0, 32);
        switch (band){
            case 2:
                sprintf(buf, "HT80");
                break;
            case 1:
                sprintf(buf, "5G");
                break;
            default:
                sprintf(buf, "2G");
        }
        ALOGD("BAND %s", buf);

        property_set("wifi.ftmd.reply", buf);

    }
    return 1;

}
