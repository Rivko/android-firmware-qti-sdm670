/*===========================================================================
                           usf_proximity.cpp

DESCRIPTION: Implementation of the Synchronized Proximity daemon.


INITIALIZATION AND SEQUENCING REQUIREMENTS:
  If not started through java app then make sure to have
  correct /data/usf/proximity/usf_proximity.cfg file linked to the wanted cfg file
  placed in /data/usf/proximity/cfg/.

Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "usf_proximity"

/*----------------------------------------------------------------------------
  Include files
----------------------------------------------------------------------------*/
#include "usf_echo_calculator.h"
#include "proximity_echo_client.h"
#include "usf_log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
static const char PIPE_NAME[] = "/data/usf/proximity/cmd";
static const time_t TIMEOUT_FIFO_CREATE_SECONDS = 10; // after boot anim done

/*----------------------------------------------------------------------------
  Types
----------------------------------------------------------------------------*/
enum Command {
  CMD_INVALID,
  CMD_START,
  CMD_STOP
};

/*==============================================================================
  FUNCTION:  echoThreadRoutine
==============================================================================*/
/**
  Thread on which the echo client is created and run.
*/
static void *echoThreadRoutine(void *unused) {
  (void) unused;

  LOGI("%s: starting", __FUNCTION__);
  ProximityEchoClient client;
  echo_calculator_start(&client); // note: blocks

  // Unblocking echo_calculator_start causes it to call "exit", so
  // following logic won't likely be called.
  LOGI("%s: exiting", __FUNCTION__);
  return NULL;
}

/*==============================================================================
  FUNCTION:  parseCommand
==============================================================================*/
/**
  Parse the input buffer to see if it matches a command; case insensitive.
*/
static Command parseCommand(const char *buf, size_t bufLen) {
  struct TableEntry {
    const char *name;
    Command command;
  };
  static const TableEntry COMMAND_TABLE[] = {
      { "start", CMD_START },
      { "stop", CMD_STOP }
  };

  for (size_t i=0; i < sizeof(COMMAND_TABLE)/sizeof(*COMMAND_TABLE); ++i) {
    const char *name = COMMAND_TABLE[i].name;
    if (bufLen-1 == strlen(name) && 0 == strncasecmp(buf, name, bufLen-1)) {
      return COMMAND_TABLE[i].command;
    }
  }
  return CMD_INVALID;
}

/*==============================================================================
  FUNCTION:  isStillBooting
==============================================================================*/
/**
  Returns true if the phone is still booting.
*/
static bool isStillBooting() {
  char prop_val[PROPERTY_VALUE_MAX];
  property_get("init.svc.bootanim", prop_val, "unknown");
  return 0 != strncmp(prop_val, "stopped", strlen("stopped"));
}

/*==============================================================================
  FUNCTION:  stopSelfIfProximityNotEnabled
==============================================================================*/
/**
  If proximity not enabled, tell init service to stop this daemon; won't be
  automatically restarted.
*/
static void stopSelfIfProximityNotEnabled() {
  char prop_val[PROPERTY_VALUE_MAX];
  property_get("ro.qc.sdk.us.proximity", prop_val, "0");
  if (prop_val[0] != '1') {
    LOGI("%s: proximity not supported, stopping service", __FUNCTION__);
    int retval = property_set("ctl.stop", "usf_proximity");
    if (retval == -1) {
      LOGE("%s: error setting property", __FUNCTION__);
    }
  }
}

/*==============================================================================
  FUNCTION:  createFifo
==============================================================================*/
/**
  Create the FIFO (named pipe).

  Handles various conditions, such as:
  - Returns success immediately if FIFO can be created or is already created
  - If unsuccessful, waits until boot finishes to try again
  - If unsuccessful after boot finishes plus a timeout period, checks if
    proximity is enabled and disables self if so. This path is expected on
    platforms without proximity support.
*/
static bool createFifo() {

  bool isBooting = true; // assume booting
  __time_t timeout_sec; // use within loop after boot finishes
  while(true) {
    int retval = mkfifo(PIPE_NAME, 0660);
    if (retval == 0) {
      LOGI("%s: success", __FUNCTION__);
      return true;
    }

    // check error was because it already exists, which is OK
    if (errno == EEXIST) {
      LOGI("%s: fifo already exists, might have data from prior run", __FUNCTION__);
      return true;
    }

    // couldn't create fifo, still booting?
    // target directory created when bootanim is stopped
    // mark the first time we detect boot has stopped for timeouts
    if (isBooting) {
      if (isStillBooting()) {
        sleep(1);
        continue;
      } else {
        isBooting = false;
        struct timespec startTime;
        retval = clock_gettime(CLOCK_MONOTONIC, &startTime);
        if (retval != 0) {
          LOGE("%s: couldn't get start time, error: %d", __FUNCTION__, errno);
          return false;
        }
        timeout_sec = startTime.tv_sec + TIMEOUT_FIFO_CREATE_SECONDS;
        LOGI("%s: end of boot detected, %d more seconds to create fifo",
          __FUNCTION__, TIMEOUT_FIFO_CREATE_SECONDS);
      }
    }

    // fall-through: not booting, check timeout
    struct timespec currentTime;
    retval = clock_gettime(CLOCK_MONOTONIC, &currentTime);
    if (retval != 0) {
      LOGE("%s: couldn't get current time, error: %d", __FUNCTION__, errno);
      return false;
    }
    if (currentTime.tv_sec > timeout_sec) {
      LOGE("%s: timed out waiting for fifo directory to exist", __FUNCTION__);
      stopSelfIfProximityNotEnabled();
      return false;
    } else {
      sleep(1);
    }
  }
}

/*==============================================================================
  FUNCTION:  main
==============================================================================*/
/**
  Main function of the Proximity daemon.
  Creates a pipe if one doesn't exist, waits for the "start" command, then
  launches a thread to start the echo task. On "stop", stops that task.
*/
int main (void)
{
  LOGI("%s: main entry",
       __FUNCTION__);

  if (!createFifo()) {
    return -1;
  }

  // main loop
  pthread_t echoThread;
  bool running = false;
  int fd = -1;
  while (true) {
    if (fd == -1) {
      fd = open(PIPE_NAME, O_RDONLY);
      if (-1 == fd) {
        LOGE("%s: open error: %d", __FUNCTION__, errno);
        return -1;
      }
    }
    const int BUF_SIZE = 128;
    char buf[BUF_SIZE] = {0}; // clear buffer
    size_t readBytes = read(fd, buf, BUF_SIZE-1); // blocks; -1 ensures null-terminate
    if (-1 == (int) readBytes) {
      LOGE("%s: read error: %d", __FUNCTION__, errno);
      return -1;
    } else if (0 == readBytes) {
      LOGI("%s: pipe closed, re-opening", __FUNCTION__);
      fd = -1;
      continue;
    }

    Command command = parseCommand(buf, readBytes);
    switch (command) {
    case CMD_START:
      LOGI("%s: got start command, running = %d", __FUNCTION__, running);
      if (!running) {
        running = true;
        int retval = pthread_create(&echoThread, NULL, echoThreadRoutine, NULL);
        if (retval != 0) {
          LOGE("%s: error from pthread_create: %d", __FUNCTION__, retval);
          return -1;
        }
      }
      break;
    case CMD_STOP:
      LOGI("%s: got stop command, running = %d", __FUNCTION__, running);
      if (running) {
        running = false;

        // note: stop currently exits the process; join likely won't get
        // called
        echo_calculator_stop();
        int retval = pthread_join(echoThread, NULL);
        if (retval != 0) {
          LOGE("%s: error from pthread_join: %s", __FUNCTION__, errno);
        }
      }
      break;
    default:
      LOGW("%s: ignoring command from pipe: '%s'", __FUNCTION__, buf);
      continue;
    }
  }
}
