/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sselog.h>
#include <drTsController.h>
#include <stringl.h>
#include "qsee_core.h"


#define MASK_8BIT           0xFF

#define FS_FILENAME "/data/vendor/ts_loopback/data"
#define IRQ_FILENAME "/data/vendor/ts_loopback/0-1-ff/secure_touch"

typedef uint32_t BOOL;
#define true  1
#define false 0

// Used by is_secure_boot_enabled 
#define SECBOOT_FUSE          0
#define SHK_FUSE              1
#define DEBUG_DISABLED_FUSE   2
#define ANTI_ROLLBACK_FUSE    3
#define FEC_ENABLED_FUSE      4
#define RPMB_ENABLED_FUSE     5
#define DEBUG_RE_ENABLED_FUSE 6

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


// Quick and easy atoi
int32_t atoil(const int8_t *in, int32_t len) 
{
  int32_t out = 0;
  int8_t *pt = (int8_t *)in;
  if (len <= 0) return 0;
  int32_t i = 0;

  while(*pt != 0) 
  {
    if (*pt < 0x30 || *pt > 0x39) 
    {
      // out of range, stop decoding
      break;
    }
    else 
    {
      out *= 10;
      out += *pt - 0x30;
      pt++;
    }
    i++;
    if (i >= len) break;
  }
  return out;
}

BOOL is_secure_boot_enabled(void)
{
  qsee_secctrl_secure_status_t status = {0,0};
  BOOL retval = true;

  qsee_get_secure_state(&status);

  if(!CHECK_BIT(status.value[0], SECBOOT_FUSE) && !CHECK_BIT(status.value[0], SHK_FUSE)
        && !CHECK_BIT(status.value[0], DEBUG_DISABLED_FUSE) && !CHECK_BIT(status.value[0], RPMB_ENABLED_FUSE)
        && CHECK_BIT(status.value[0], DEBUG_RE_ENABLED_FUSE))
    retval = true;
  else
    retval = false;

  return retval;
}

/*
* The Open and close function are dummy functions, nothing is done
*/
uint32_t loopback_drTsOpen(const uint32_t width, const uint32_t height)
{
  int32_t rv = 0;
  (void) width;
  (void) height;
  
  ENTER;
  
  qsee_log(QSEE_LOG_MSG_DEBUG, "HLOS-Loopback opening, drtsOpen");
  do {
    if (is_secure_boot_enabled()) 
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "Secure-boot enabled, can not use test driver");
      rv = -1;
      break;
    }
    
    // Clear the file out for the next data
    int fd = open(IRQ_FILENAME, O_WRONLY);
    if (fd != -1)
    {
      int bytes_written =  write(fd, "0", 1); 
      qsee_log(QSEE_LOG_MSG_DEBUG, "bytes_writeen: %d to IRQ file", bytes_written);
      if (bytes_written != 1) {
        // Failed to clear, but file exists, can continue
        qsee_log(QSEE_LOG_MSG_ERROR, "Error trying to write the IRQ flag");
      }
      close(fd);    
    }
    else 
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "Error trying to open the IRQ flag file");
      rv = -1;
    }
  } while(0);

  EXITV((uint32_t)rv);
}

uint32_t loopback_drTsClose(void)
{
  int32_t rv = 0;
  
  ENTER;

  qsee_log(QSEE_LOG_MSG_DEBUG, "HLOS-Loopback closing, drTsClose");

  EXITV((uint32_t)rv);
}

/** Process all available touch events
 */
uint32_t loopback_drTsProcessEvents(void)
{
  int32_t rv = 0;
  int8_t tsData[20];
  int32_t x, y, fin, code;
  int8_t irq_data = 0xff;

  ENTER;
  qsee_log(QSEE_LOG_MSG_DEBUG, "HLOS-Loopback ProcessEvent");
  do {
    int fd = open(FS_FILENAME, O_RDONLY);
    if (fd == -1)
    {
      qsee_log(QSEE_LOG_MSG_ERROR,"FILE OPEN (%s) failed", FS_FILENAME);
      rv = -1;
      break;
    }

    int bytes_read = read(fd, tsData, sizeof(tsData));
    close(fd);

    // Clear the file out for the next data
    fd = open(IRQ_FILENAME, O_RDONLY);
    if (fd != -1)
    {
      read(fd, &irq_data, sizeof(irq_data));
      close(fd);
      if(irq_data == '0'){
        rv = 0;
        qsee_log(QSEE_LOG_MSG_ERROR, "No new IRQ to handle");
        break;
      }

      fd = open(IRQ_FILENAME, O_WRONLY);
      if(fd < 0){
        qsee_log(QSEE_LOG_MSG_ERROR, "Error trying to clear IRQ flag");
        rv = -1;
      } else {
        write(fd, "0", 1);
        close(fd);
      }
    }
    else
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "Error trying to clear IRQ flag");
      rv = -1;
    }

    // Need to have the whole packet, otherwise, don't decode
    if (bytes_read < 15) 
    {
      break;
    }
    // Skip empty packet
    if (tsData[0] == ' ') 
    {
      break;
    }
    
    // Decode the incoming data into x/y/event
    
    // Extract X item
    x = atoil(tsData, 5);
    // Extract Y item
    y = atoil(tsData + 6, 5);
    // Extract Finger #
    fin = atoil(tsData + 12, 2);
    qsee_log(QSEE_LOG_MSG_DEBUG, "HLOS-Loopback (%d, %d, %d, %c)", 
                                                 x, y, fin, tsData[15]);
    // Extract CODE
    switch(tsData[15]) {
      case 'D':
        drNotifyEvent(x, y, TOUCH_EVENT_DOWN, fin);
        break;
      case 'U':
        drNotifyEvent(x, y, TOUCH_EVENT_UP, fin);
        break;
      case 'M':
        drNotifyEvent(x, y, TOUCH_EVENT_MOVE, fin);
        break;
      default:
        qsee_log(QSEE_LOG_MSG_ERROR, "Invalid event code - %x", tsData[15]);
        rv = -1;
        break;
    }
  } while (0);
  EXITV((uint32_t)rv);
}




