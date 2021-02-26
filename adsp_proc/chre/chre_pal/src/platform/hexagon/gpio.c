/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <gpio.h>


#include "HAP_farf.h"

struct Gpio* gpioRequest(uint32_t number)
{
    return (struct Gpio*) number;
}

void gpioRelease(struct Gpio* __restrict gpio)
{
    (void)gpio;
}

void gpioConfigInput(const struct Gpio* __restrict gpioHandle, int32_t gpioSpeed, enum GpioPullMode pull)
{
}

void gpioConfigOutput(const struct Gpio* __restrict gpioHandle, int32_t gpioSpeed, enum GpioPullMode pull, enum GpioOpenDrainMode output, bool value)
{
}

void gpioConfigAlt(const struct Gpio* __restrict gpioHandle, int32_t gpioSpeed, enum GpioPullMode pull, enum GpioOpenDrainMode output, uint32_t altFunc)
{
}

void gpioConfigAnalog(const struct Gpio* __restrict gpio)
{
}

void gpioSet(const struct Gpio* __restrict gpioHandle, bool value)
{
}

bool gpioGet(const struct Gpio* __restrict gpioHandle)
{
    return false;
}

