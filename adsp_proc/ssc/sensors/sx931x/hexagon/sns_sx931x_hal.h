#pragma once
/*******************************************************************************
 * Copyright (c) 2017, Semtech
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Semtech nor the
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
 *******************************************************************************/


#include <stdint.h>
#include "sns_sensor.h"
#include "sns_sensor_uid.h"

#include "sns_sx931x_sensor.h"
#include "sns_sx931x_sensor_instance.h"


/* Constants */
#define SX_NULL                          	(0)


/*******ERR Code**************************************/
#define  SUCCESS      						((uint8_t)0)
#define E_NULL_PTR         				((int8_t)-127)
#define E_COMM_RES         				((int8_t)-1)
#define E_OUT_OF_RANGE     				((int8_t)-2)
#define E_ERROR                           ((int8_t)-1)


/* Sensor Specific constants */
#define SLEEP_MODE                    (0x00)
#define NORMAL_MODE            				(0x03)

/************************************************/
/**\name  WORKING MODE DEFINITION       */
/***********************************************/
#define ULTRA_LOW_POWER_MODE          (0x00)
#define ULTRA_HIGH_RESOLUTION_MODE    (0x04)




/*
 *  Address registers
 */
#define SX931x_WHOAMI_REG								0x42
#define SX931x_WHOAMI_VALUE							0x02  /** Who Am I default value */



/** Off to idle time */
#define SX93xx_OFF_TO_IDLE_MS      			100  //ms
#define SX931x_NUM_AXES         				4

#if 1
/*******Interrupt and status*****************/
#define SX931x_IRQSTAT_REG    	0x00
#define SX931x_STAT0_REG    	  0x01
#define SX931x_STAT1_REG    	  0x02
#define SX931x_IRQ_ENABLE_REG	  0x03
#define SX931x_IRQFUNC_REG		  0x04
	
#define SX931x_CPS_CTRL0_REG    0x10
#define SX931x_CPS_CTRL1_REG    0x11
#define SX931x_CPS_CTRL2_REG    0x12
#define SX931x_CPS_CTRL3_REG    0x13
#define SX931x_CPS_CTRL4_REG    0x14
#define SX931x_CPS_CTRL5_REG    0x15
#define SX931x_CPS_CTRL6_REG    0x16
#define SX931x_CPS_CTRL7_REG    0x17
#define SX931x_CPS_CTRL8_REG    0x18
#define SX931x_CPS_CTRL9_REG	  0x19
#define SX931x_CPS_CTRL10_REG	  0x1A
#define SX931x_CPS_CTRL11_REG 	0x1B
#define SX931x_CPS_CTRL12_REG	  0x1C
#define SX931x_CPS_CTRL13_REG	  0x1D
#define SX931x_CPS_CTRL14_REG	  0x1E
#define SX931x_CPS_CTRL15_REG	  0x1F
#define SX931x_CPS_CTRL16_REG	  0x20
#define SX931x_CPS_CTRL17_REG	  0x21
#define SX931x_CPS_CTRL18_REG	  0x22
#define SX931x_CPS_CTRL19_REG	  0x23
#define SX931x_SAR_CTRL0_REG	  0x2A
#define SX931x_SAR_CTRL1_REG	  0x2B
#define SX931x_SAR_CTRL2_REG	  0x2C

#define SX931x_SOFTRESET_REG    0x7F

/*    Sensor Readback */
#define SX931x_CPSRD            0x30

#define SX931x_USEMSB           0x31
#define SX931x_USELSB           0x32

#define SX931x_AVGMSB           0x33
#define SX931x_AVGLSB           0x34

#define SX931x_DIFFMSB          0x35
#define SX931x_DIFFLSB          0x36
#define SX931x_OFFSETMSB		    0x37
#define SX931x_OFFSETLSB		    0x38

#define SX931x_SARMSB			      0x39
#define SX931x_SARLSB			      0x3A

/*      IrqStat 0:Inactive 1:Active     */
#define SX931x_IRQSTAT_RESET_FLAG      0x80
#define SX931x_IRQSTAT_TOUCH_FLAG      0x40
#define SX931x_IRQSTAT_RELEASE_FLAG    0x20
#define SX931x_IRQSTAT_COMPDONE_FLAG   0x10
#define SX931x_IRQSTAT_CONV_FLAG       0x08
#define SX931x_IRQSTAT_CLOSEALL_FLAG   0x04
#define SX931x_IRQSTAT_FARALL_FLAG     0x02
#define SX931x_IRQSTAT_SMARTSAR_FLAG   0x01


/* CpsStat  */
#define SX931x_TCHCMPSTAT_TCHCOMB_FLAG    0x08
#define SX931x_TCHCMPSTAT_TCHSTAT2_FLAG   0x04
#define SX931x_TCHCMPSTAT_TCHSTAT1_FLAG   0x02
#define SX931x_TCHCMPSTAT_TCHSTAT0_FLAG   0x01


/*      SoftReset */
#define SX931x_SOFTRESET_VALUE  0xDE

#endif


typedef struct _REGISTER_SETTING
{
    uint8_t Register;
    uint8_t Value;
} REGISTER_SETTING, *PREGISTER_SETTING;





void sx931x_dump_reg(sns_sensor_instance *this, uint32_t hw_idx);
sns_rc sx931x_device_sw_reset(sns_sync_com_port_service *scp_service,
                              sns_sync_com_port_handle *port_handle);
sns_rc sx931x_reset_device(sns_sync_com_port_service *scp_service,
                           sns_sync_com_port_handle *port_handle,
                           uint32_t hw_id);

void sx931x_send_config_event(sns_sensor_instance *const instance);
sns_rc sx931x_get_who_am_i(sns_sync_com_port_service *scp_service,
                           sns_sync_com_port_handle *port_handle,
                           uint8_t *buffer);

void sx931x_start_sensor_sar_polling_timer(sns_sensor_instance *this);
void sx931x_reconfig_hw(sns_sensor_instance *this, sx931x_sensor_type sensor_type);


void sx931x_set_sar_polling_config(sns_sensor_instance *const this);
/*
* Executes requested self-tests.
*/
void sx931x_run_self_test(sns_sensor_instance *instance);
void sx931x_handle_sar_data_stream_timer_event(sns_sensor_instance *const instance);
void sx931x_handle_sar_data_stream_interrupt_event(sns_sensor_instance *const instance);
void sx931x_register_interrupt(sns_sensor_instance *this);
//void sx932x_update_intr(sns_sensor_instance *const instance);
void sx931x_read_gpio(sns_sensor_instance *instance, uint32_t gpio, bool is_chip_pin);
sns_rc sx931x_interrupt_config(sns_sync_com_port_service *scp_service,
                                        sns_sync_com_port_handle *port_handle,
                                        uint8_t config);







