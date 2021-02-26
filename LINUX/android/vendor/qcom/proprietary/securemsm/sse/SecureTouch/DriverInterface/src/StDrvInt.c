/*
 * Copyright(c) 2013-2016 Qualcomm Technologies, Inc.All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#define MAIN_C

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <dirent.h>
#include <utils/Log.h>
#include "common_log.h"

static char* locations[] = {
#if defined(ST_TARGET_MSM8953)
"/sys/devices/soc/78b7000.i2c/i2c-3/3-004b",
#elif defined(ST_TARGET_THORIUM)
"/sys/devices/soc/78b7000.i2c/i2c-3/3-0038",
"/sys/devices/soc/78b7000.i2c/i2c-3/3-0020",
#elif defined(ST_TARGET_MSM8974)
"/sys/devices/f9924000.i2c/i2c-2/2-004a",
#elif defined(ST_TARGET_APQ8084)
"/sys/devices/f9966000.i2c/i2c-1/1-004a",
"/sys/devices/f9966000.i2c/i2c-1/1-0020",
#elif defined(ST_TARGET_MSM8916)
"/sys/devices/soc.0/78b9000.i2c/i2c-5/5-0020",
"/sys/devices/soc.0/78b9000.i2c/i2c-5/5-0020/input/input2",
#elif defined(ST_TARGET_MSM8994)
"/sys/devices/soc.0/f9924000.i2c/i2c-2/2-0020/input/input0",
"/sys/devices/soc.0/f9924000.i2c/i2c-2/2-004a",
#elif defined(ST_TARGET_MSM8996)
"/sys/devices/soc/75ba000.i2c/i2c-12/12-0020",
"/sys/devices/soc/75ba000.i2c/i2c-12/12-004a",
#elif defined(ST_TARGET_MSM8952)
"/sys/devices/soc.0/78b9000.i2c/i2c-5/5-0020",
#elif defined(ST_TARGET_MSM8998)
"/sys/devices/soc/c179000.i2c/i2c-5/5-0020",
#elif defined(ST_TARGET_SDM660)
"/sys/devices/soc/c178000.i2c/i2c-4/4-0020",
#elif defined(ST_TARGET_SDM845)
"/sys/devices/platform/soc/a98000.i2c/i2c-2/2-0020",
#elif defined(ST_TARGET_SDM710)
"/sys/devices/platform/soc/a84000.i2c/i2c-2/2-0020",
#endif
};

/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "libStDrvInt: "
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 0 //Define to enable LOGD
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MAX_PATH  100

static int fd_control = -1;
static int fd_irq = -1;
static char control_file[MAX_PATH] = {0};
static char irq_file[MAX_PATH] = {0};

static int touch_search_recursive(char const * path, char (*control)[MAX_PATH], char (*irq)[MAX_PATH]) {
  DIR *dirp = NULL;
  struct dirent *dir_info = NULL;
  char device_path[MAX_PATH] = {0};
  size_t path_length = 0;
  int found = 0;
  int rv = 0;

  if (!control || !irq || !path) {
    LOGE("Invalid input");
    return -EINVAL;
  }

  LOGD("Searching %s", path);
  dirp = opendir(path);
  if (!dirp) {
    LOGE("Failed to open %s: %s (%d)", path, strerror(errno), errno);
    return -EINVAL;
  }


  strlcpy(device_path, path, MAX_PATH);
  path_length = strnlen(device_path, MAX_PATH);
  device_path[path_length] = '/';

  while ((dir_info = readdir(dirp)) != NULL) {
    if ((!strncmp(dir_info->d_name, ".", 10)) ||
        (!strncmp(dir_info->d_name, "..", 10)))
      continue;
    if ((dir_info->d_type == DT_REG) && (strcmp(dir_info->d_name, "secure_touch") == 0)) {
      strlcpy(*irq, device_path, MAX_PATH);
      strlcat(*irq, dir_info->d_name, MAX_PATH);
      strlcpy(*control, *irq, MAX_PATH);
      strlcat(*control, "_enable", MAX_PATH);
      LOGD("Secure touch found @ %s: %s - %s", device_path, *control, *irq);
      found = 1;
      break;
    } else if (dir_info->d_type == DT_DIR) {
      char new_path[MAX_PATH] = {0};
      strlcpy(new_path, device_path, MAX_PATH);
      strlcat(new_path, dir_info->d_name, MAX_PATH);
      rv = touch_search_recursive(new_path,control,irq);
      if (rv == 0) {
        found = 1;
        break;
      }
    }
  }
  closedir(dirp);

  if (found) return 0;
  return -ENOENT;
}

static int touch_search_device(char (*control)[MAX_PATH], char (*irq)[MAX_PATH])
{
  int rv = 0;
  size_t i = 0;

  if (!control || !irq) {
    LOGE("Invalid input");
    return -EINVAL;
  }

  for(i = 0; i < ARRAY_LENGTH(locations); i++) {
    LOGD("Searching %s", locations[i]);
    rv = touch_search_recursive(locations[i], control, irq);
    if (rv == 0) break;
  }

  return rv;

}

int32_t stStartSession(uint32_t *touchController)
{
  int32_t rv = 0;
  int i;
  ssize_t writtenBytes = 0;
  char *token;
  const char s[] = "-";
  const char s2[] = "/";
  long int value = 0;
  char str[MAX_PATH] = {0};
  char *stringptr;
  if (fd_control != -1) {
    LOGE("Session already started");
    return EBUSY;
  }
  rv = touch_search_device(&control_file, &irq_file);
  if (rv) {
    LOGE("No secure touch support detected");
    return rv;
  }

  for(i = 0;i<strlen(control_file);++i)
  {
     str[i] = control_file[i];
  }

  token = strtok_r(str,s,&stringptr);
  if(token != NULL)
     token = strtok_r(NULL,s,&stringptr);
  if(token != NULL)
     token = strtok_r(NULL,s2,&stringptr);
  if(token != NULL)
     value = strtol(token,NULL,16);

  if(touchController != NULL)
     *touchController = (uint32_t)value;
  LOGD("Opening control file %s", control_file);
  fd_control = open(control_file, O_WRONLY);
  if (fd_control == -1) {
    LOGE("Error opening %s: %s (%d)", control_file, strerror(errno), errno);
    return errno;
  }
  writtenBytes = pwrite(fd_control,"1",1,0);
  if (writtenBytes <= 0) {
    LOGE("Error writing (1) to %s: %s (%d)", control_file, strerror(errno), errno);
    close(fd_control);
    fd_control = -1;
    return errno;
  }
  return rv;
}

int32_t stTerminateSession(uint32_t force)
{
  int32_t rv = 0;
  ssize_t writtenBytes = 0;
  if (force && (fd_control == -1)) {
    rv = touch_search_device(&control_file, &irq_file);
    if (rv) {
      LOGE("No secure touch support detected");
      return rv;
    }
    fd_control = open(control_file, O_WRONLY);
  }
  if (fd_control == -1) {
    LOGE("No session active");
    return ENODEV;
  }
  writtenBytes = pwrite(fd_control,"0",1,0);
  if (writtenBytes <= 0) {
    LOGE("Error writing (0) to %s: %s (%d)", control_file, strerror(errno), errno);
    return errno;
  }
  close(fd_control);
  fd_control = -1;
  if (fd_irq != -1) {
    close(fd_irq);
    fd_irq = -1;
  }
  return rv;
}

int32_t stWaitForEvent(int32_t abortFd, int32_t timeout)
{
  int32_t rv = 0;
  ssize_t readBytes = 0;
  char c;
  struct pollfd *fds = NULL; /* Used for poll() */
  size_t events = 1; /* Number of FD to poll */

  if (fd_irq == -1) {
    fd_irq = open(irq_file,O_RDONLY);
    if (fd_irq == -1) {
      LOGE("Error opening %s: %s (%d)", irq_file, strerror(errno), errno);
      return errno;
    }
  }

  // read and verify if an interrupt is already pending
  readBytes = pread(fd_irq,&c,1,0);
  if (readBytes <= 0) {
    LOGE("Error reading from %s: %s (%d)", irq_file, strerror(errno), errno);
    return errno;
  }

  if (c == '1') {
    // interrupt
    return 0;
  }

  if (abortFd != -1)
    events = 2;

  fds = (struct pollfd *)calloc(events, sizeof(struct pollfd));
  if (fds == NULL) {
    return -ENOMEM;
  }
  /* IRQ FD, always available */
  fds[0].fd = fd_irq;
  fds[0].events = POLLERR|POLLPRI;
  /* FD for abort requests */
  if (events == 2) {
    fds[1].fd = abortFd;
    fds[1].events = POLLIN;
  }

  rv = poll(fds, events, timeout);
  if (rv < 0) {
    /* Error, return error condition */
    LOGE("Error condition during polling: %s (%d)", strerror(errno), errno);
    rv = errno;
    goto free_fds;
  }
  if (rv == 0) {
    /* timeout */
    rv = -ETIMEDOUT;
    goto free_fds;
  }
  /* Check for external abort */
  if ((events == 2) && (fds[1].revents)) {
    rv = -ECONNABORTED;
    goto free_fds;
  }
  /* Consume data, or error, and return */
  if (fds[0].revents) {
    readBytes = pread(fd_irq,&c,1,0);
    if (readBytes <= 0) {
      LOGE("Error reading from %s: %s (%d)", irq_file, strerror(errno), errno);
      rv = errno;
      goto free_fds;
    }
    if (c == '1') {
      // interrupt
      rv = 0;
      goto free_fds;
    }
    rv = 0;
  }
free_fds:
  if (fds) {
    free (fds);
  }
  return rv;
}
