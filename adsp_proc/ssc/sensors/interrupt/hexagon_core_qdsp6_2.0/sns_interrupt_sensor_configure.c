/**
 * sns_interrupt_sensor_configure.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sns_interrupt.pb.h"
#include "sns_interrupt_sensor_configure.h"
#include "sns_island.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"
#include "sns_types.h"
#include "stdint.h"
#include "uGPIOInt.h"

/**
 * Set flag to register sensors interrupts for island mode operation
 */
uint32_t sns_interrupt_sensor_get_island_flag(void)
{
  return UGPIOINTF_ISLAND;
}
/**
 * Set flag to register level triggered interrupts
 */
uint32_t sns_interrupt_sensor_get_level_trigger_flag(void)
{
  return UGPIOINTF_DSR;
}
/**
 * Get flag to use the hardware timestamping unit
 */
uint32_t sns_interrupt_sensor_get_hw_timestamp_flag(void)
{
  return UGPIOINTF_TIMESTAMP_EN;
}

SNS_SECTION(".text.sns") sns_rc send_interrupt_done(sns_sensor_instance *const this,
                           sns_request const *client_request)
{
  int32_t ret = 0;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_interrupt_req *req = (sns_interrupt_req*)client_request->request;
  ret = uGPIOInt_InterruptDone(req->interrupt_num);
  if(ret != UGPIOINT_SUCCESS)
  {
    SNS_INST_PRINTF(ERROR, this, "InterruptDone failed %d", ret);
    rv = SNS_RC_FAILED;
  }
  return rv;
}

/**
 * Attempt to read the timestamp that was captured by the interrupt's HW
 * timestamping unit.
 *
 * @param[i] this        pointer to the sensor instance, for logging purposes
 * @param[i] gpio        interrupt pin to read the timestamp from
 * @param[o] timestamp   where the timestamp is placed
 *
 * @return sns_rc        SNS_RC_SUCCESS if the timestamp was successfully read,
 *                       SNS_RC_FAILED otherwise
 */
SNS_SECTION(".text.sns") sns_rc read_timestamp(sns_sensor_instance *const this,
    uint32_t gpio, sns_time* timestamp, sns_interrupt_timestamp_status* ts_status)
{
  int32_t ret;
  sns_rc rv = SNS_RC_SUCCESS;
  uGPIOIntTimestampStatus gpio_ts_status = UGPIOINT_TSS_INVALID;
  UNUSED_VAR(this);

  ret = uGPIOInt_TimestampRead(gpio, &gpio_ts_status, timestamp);
  // If uGPIOInt_TimestampRead returns an error code, that means that we hit a
  // rare timing error related to PDC and island mode. Try running
  // uGPIOInt_TimestampRead again which should allow us to gracefully handle
  // the timing problem. The second time we call it, it should go through.
  if ( ret != UGPIOINT_SUCCESS )
  {
    ret = uGPIOInt_TimestampRead(gpio, &gpio_ts_status, timestamp);
  }

  // UGPIOINT_TSS_VALID and UGPIOINT_TSS_VALID_OVERFLOW are both acceptable statuses
  if ((ret != UGPIOINT_SUCCESS) || ((gpio_ts_status != UGPIOINT_TSS_VALID) && (gpio_ts_status != UGPIOINT_TSS_VALID_OVERFLOW))  )
  {
    //SNS_INST_PRINTF(ERROR, this, "read_timestamp failed %d, ts_status %u", ret, gpio_ts_status);
    rv = SNS_RC_FAILED;
  }

  *ts_status = (sns_interrupt_timestamp_status) gpio_ts_status;
  return rv;
}
