 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Vishay Capella Microsystems nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "sns_sensor.h"
#include "sns_sensor_uid.h"

#include "sns_cm32181_sensor_instance.h"

/**
 *  Address registers
 */
#define CM32181_REG_CONF			0x00
#define	CM32181_REG_ALS_WH			0x01
#define	CM32181_REG_ALS_WL			0x02
#define	CM32181_REG_PSM				0x03
#define	CM32181_REG_ALS_DATA		0x04
#define	CM32181_REG_INT_FLAG		0x06
#define	CM32181_REG_ID				0x07

/** Register value definition: CONF */
#define CM32181_ALS_SD				0x0001
#define CM32181_ALS_INT_EN			0x0002
#define CM32181_ALS_PERS_MASK		0x0030
#define CM32181_ALS_PERS_1			0x0000
#define CM32181_ALS_PERS_2			0x0010
#define CM32181_ALS_PERS_4			0x0020
#define CM32181_ALS_PERS_8			0x0030
#define CM32181_ALS_IT_MASK			0x03C0
#define CM32181_ALS_IT_25MS			0x0300
#define CM32181_ALS_IT_50MS			0x0200
#define CM32181_ALS_IT_100MS		0x0000
#define CM32181_ALS_IT_200MS		0x0040
#define CM32181_ALS_IT_400MS		0x0080
#define CM32181_ALS_IT_800MS		0x00C0
#define CM32181_ALS_SM_MASK			0x1800
#define CM32181_ALS_SM_X1			0x0000
#define CM32181_ALS_SM_X2			0x0800
#define CM32181_ALS_SM_X1_8			0x1000
#define CM32181_ALS_SM_X1_4			0x1800
#define	CM32181_ALS_IT_SHIFT		6
#define CM32181_CONF_DEFAULT		(CM32181_ALS_PERS_1 | CM32181_ALS_IT_100MS | CM32181_ALS_SM_X2)

/* Register value definition: ALS_WH */
#define CM32181_ALS_WH_DEFAULT		0x0000

/* Register value definition: ALS_WL */
#define CM32181_ALS_WL_DEFAULT		0xFFFF

/* Register value definition: PSM */
#define CM32181_PSM_EN				0x0001
#define CM32181_PSM_MASK			0x0006
#define CM32181_PSM_1				0x0000
#define CM32181_PSM_2				0x0002
#define CM32181_PSM_3				0x0004
#define CM32181_PSM_4				0x0006
#define CM32181_PSM_DEFAULT			0

/* Register Value define : INT_FLAG */
#define CM32181_ALS_IF_H			0x4000
#define CM32181_ALS_IF_L			0x8000

/** Default values loaded in probe function */
#define CM32181_WHOAMI_VALUE		(0x81)  /** Who Am I default value */

/** fifo paramters */
#define CM32181_MAX_FIFO			0 // Max fifo samples

/** Off to idle time */
#define CM32181_OFF_TO_IDLE_MS		5000  //ms

#define CM32181_NUM_AXES			2

#define CM32181_LUX_PER_ADC_CODE	0.036f     /* 0.036 Lux for IT 100ms */

/******************* Function Declarations ***********************************/

/**
 * Resets the Sensor HW.
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] port_handle   handle to synch COM port
 * @param[i] sensor        bit mask for sensors to reset
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc cm32181_reset_device(sns_sync_com_port_service *scp_service,
							sns_sync_com_port_handle *port_handle,
							cm32181_sensor_type sensor);

/**
 * Gets Who-Am-I register for the sensor.
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] state         Instance state
 * @param[o] buffer        who am I value read from HW
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc cm32181_get_who_am_i(sns_sync_com_port_service *scp_service,
							sns_sync_com_port_handle  *port_handle,
							uint8_t *buffer);

/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
void cm32181_start_polling_timer(sns_sensor_instance *this);

void cm32181_set_polling_config(sns_sensor_instance *const this);
							
/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
void cm32181_send_config_event(sns_sensor_instance *const instance);							
							
/**
 * Sends ambient light event.
 *
 * @param[i] instance   Sensor Instance
 */
void cm32181_handle_ambient_light_sample(sns_sensor_instance *const instance);

/**
 * Executes requested self-tests.
 *
 * @param instance   reference to the instance
 *
 * @return none
 */
void cm32181_run_self_test(sns_sensor_instance *instance);

