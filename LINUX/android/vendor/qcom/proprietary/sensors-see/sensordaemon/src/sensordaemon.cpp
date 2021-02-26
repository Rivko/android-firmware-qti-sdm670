/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 *
 */
#ifndef VERIFY_PRINT_ERROR
#define VERIFY_PRINT_ERROR
#endif

#include <stdio.h>
#include <unistd.h>
#include "verify.h"
#include <diag_lsm.h>
#include "worker.h"
#include "ssc_utils.h"
#include "ssc_utils.h"
#include "sensors_log.h"
#include <unordered_map>
#include <cutils/properties.h>

#define SNS_DIAG_PERIPHERAL_ID          3
#define SNS_DIAG_TX_MODE_STREAMING      0
#define SNS_DIAG_TX_MODE_CIRCULAR_BUFF  2
#define SNS_DIAG_LOW_WM_VALUE           20
#define SNS_DIAG_HIGH_WM_VALUE          70
#define SNS_DIAG_CIRC_BUFF_SHIFT        3

const char SENSORS_DAEMON_PROP_DEBUG[] = "persist.debug.sensors.daemon";

using namespace std;
using namespace sensors_log;

static worker _worker_diag;
static worker _worker_registry;

extern void handle_registry_config();

/*create 2 threads to monitor the buffering mode and QMI error call back for ssr*/
void handle_diag_settings() {
    /* Find diag buffering mode */
    uint32_t settings = 0;
    int err = 0;

    settings = get_sns_settings();
    VERIFY_EPRINTF("settings val:%d", settings);

    boolean diag_init = Diag_LSM_Init(NULL);
    if (!diag_init) {
        VERIFY_EPRINTF("Diag_LSM_Init failed!");
    } else {
        usleep(6000000);
        if((settings & SNS_DIAG_CIRC_BUFF_MODE_FLAG) > SNS_DIAG_CIRC_BUFF_SHIFT)
        {
            VERIFY_EPRINTF("Setting SNS TX MODE to CIRCULAR BUFF!");
            err = diag_configure_peripheral_buffering_tx_mode(SNS_DIAG_PERIPHERAL_ID,
                                                              SNS_DIAG_TX_MODE_CIRCULAR_BUFF,
                                                              SNS_DIAG_LOW_WM_VALUE,
                                                              SNS_DIAG_HIGH_WM_VALUE);
         } else {
            VERIFY_EPRINTF("Setting SNS TX MODE to STREAMING!");
            err = diag_configure_peripheral_buffering_tx_mode(SNS_DIAG_PERIPHERAL_ID,
                                                              SNS_DIAG_TX_MODE_STREAMING,
                                                              SNS_DIAG_LOW_WM_VALUE,
                                                              SNS_DIAG_HIGH_WM_VALUE);
        }
         /*1 is success and 0 failure*/
        if (1 == err) {
            VERIFY_EPRINTF("Set SNS TX MODE success and return val:%d", err);
        } else {
            VERIFY_EPRINTF("Set SNS TX MODE failed and return val:%d", err);
        }
    }
}

/* map debug property value to log_level */
static const unordered_map<char, sensors_log::log_level> log_level_map = {
    { '0', sensors_log::SILENT },
    { '1', sensors_log::INFO },
    { 'e', sensors_log::ERROR },
    { 'E', sensors_log::ERROR },
    { 'i', sensors_log::INFO },
    { 'I', sensors_log::INFO },
    { 'd', sensors_log::DEBUG },
    { 'D', sensors_log::DEBUG },
    { 'v', sensors_log::VERBOSE },
    { 'V', sensors_log::VERBOSE },
};

void set_log_level()
{
    char debug_prop[PROPERTY_VALUE_MAX];
    int len;
    log_level loglevel;
    sensors_log::set_tag("sensors.qti");
    len = property_get(SENSORS_DAEMON_PROP_DEBUG, debug_prop, "i");
    if (len > 0) {
        if (log_level_map.find(debug_prop[0]) != log_level_map.end()) {
            loglevel = log_level_map.at(debug_prop[0]);
             sns_logi("log_level set: %d", loglevel);
             sensors_log::set_level(loglevel);
        }
    }
}

int main() {
    set_log_level();
    sns_logi("sensors daemon starting");

  _worker_registry.add_task(NULL, [] {
      handle_registry_config();
 });

  _worker_diag.add_task(NULL, [] {
      handle_diag_settings();
  });

  VERIFY_EPRINTF("sensors daemon starting");
  sleep(INT_MAX);
  VERIFY_EPRINTF("sensors daemon exiting!");

  return 0;
}
