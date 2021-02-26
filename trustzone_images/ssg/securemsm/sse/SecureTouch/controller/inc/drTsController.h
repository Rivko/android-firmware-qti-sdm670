/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

#define TOUCH_EVENT_NONE  0
#define TOUCH_EVENT_DOWN  1
#define TOUCH_EVENT_MOVE  2
#define TOUCH_EVENT_UP    4

/** Open a connection with the controller.
 * If the width and height of the panel are not provided (i.e. they are passed
 * as (0,0), the default values are used.
 */
uint32_t drTsOpen(const uint32_t width, const uint32_t height);

/** Close the connection with the controller.
 */
uint32_t drTsClose(void);

/** Process all available touch events
 */
uint32_t drTsProcessEvents(void);

/** Notify the upper layer of a touch event.
 * This function has to be called from the controller library, it is
 * implemented by the main driver.
 */
uint32_t drNotifyEvent(const uint32_t x, const uint32_t y, const uint32_t code, const uint8_t finger);

/** Functions representing a controller
 * */
struct drTsFunc {
  uint32_t (*drTsOpen)(const uint32_t width, const uint32_t height);
  uint32_t (*drTsClose)(void);
  uint32_t (*drTsProcessEvents)(void);
};
