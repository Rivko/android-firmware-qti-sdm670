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

#include "sns_cm3526_sensor_instance.h"

/**
 *  Address registers
 */
#define CM3526_REG_CONF             0x00
#define CM3526_REG_UVA_DATA         0x07
#define CM3526_REG_DUMMY_DATA       0x08
#define CM3526_REG_UVB_DATA         0x09
#define CM3526_REG_UVCOMP1_DATA     0x0A
#define CM3526_REG_UVCOMP2_DATA     0x0B
#define CM3526_REG_ID               0x0C

/** Register value definition : CONF */
#define CM3526_CONF_SD              0x01
#define CM3526_CONF_UV_AF_AUTO      0x00
#define CM3526_CONF_UV_AF_FORCE     0x02
#define CM3526_CONF_UV_TRIG_NO      0x00
#define CM3526_CONF_UV_TRIG_ONCE    0x04
#define CM3526_CONF_HD              0x08
#define CM3526_CONF_UV_IT_MASK      0x70
#define CM3526_CONF_UV_IT_50MS      0x00
#define CM3526_CONF_UV_IT_100MS     0x10
#define CM3526_CONF_UV_IT_200MS     0x20
#define CM3526_CONF_UV_IT_400MS     0x30
#define CM3526_CONF_UV_IT_800MS     0x40
#define CM3526_CONF_DEFAULT         (CM3526_CONF_UV_AF_AUTO | CM3526_CONF_UV_TRIG_NO | CM3526_CONF_UV_IT_100MS)


/** Default values loaded in probe function */
#define CM3526_WHOAMI_VALUE             (0x26)  /** Who Am I default value */

/** fifo paramters */
#define CM3526_MAX_FIFO                 0 // Max fifo samples

/** Off to idle time */
#define CM3526_OFF_TO_IDLE_MS           5000  //ms

#define CM3526_NUM_AXES           5
// Per model parameters
#define UVIA_RESPONSIVITY			0.00353135f
#define UVIB_RESPONSIVITY			0.0048f
#define UVA_RESPONSIVITY			0.0010496f
#define UVB_RESPONSIVITY			0.0000835f

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
sns_rc cm3526_reset_device(sns_sync_com_port_service *scp_service,
                           sns_sync_com_port_handle *port_handle,
                           cm3526_sensor_type sensor);

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
sns_rc cm3526_get_who_am_i(sns_sync_com_port_service *scp_service,
                           sns_sync_com_port_handle  *port_handle,
                           uint8_t *buffer);

/**
 * Sends UV event.
 *
 * @param[i] instance   Sensor Instance
 */
void cm3526_handle_uv_sample(sns_sensor_instance *const instance);

void cm3526_set_polling_config(sns_sensor_instance *const this);