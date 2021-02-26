/*******************************************************************************
 * Copyright (c) 2018, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
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

#include "sns_bmm150_sensor.h"
#include "sns_bmm150_sensor_instance.h"
#include "sns_com_port_types.h"
#include "sns_types.h"
#include "sns_mem_util.h"


#if !BMM_CONFIG_ENABLE_REGISTRY
/** TODO Using 8996 Platform config as defaults. This is for
 *  test purpose only. All platform specific information will
 *  be available to the Sensor driver via Registry.
 *  otherwise, please change accordingly
 *  */

#ifdef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
#define SPI_BUS_INSTANCE               0x02
#else
#define SPI_BUS_INSTANCE               0x01
#endif

#define RAIL_1                         "/pmic/client/sensor_vddio"
#define RAIL_2                         "/pmic/client/sensor_vdd"
#define IRQ_NUM                        117
#define NUM_OF_RAILS                   2

#define SPI_BUS_MIN_FREQ_KHZ           7*100  // using a value of 0 will introduce more errors in timing
#define SPI_BUS_MAX_FREQ_KHZ           13*100 // 1.3MHz
#define SPI_SLAVE_CONTROL              0x0


#define SENSOR_I2C_SLAVE_ADDRESS       0x10
#define I2C_BUS_INSTANCE               0x03
#define I2C_BUS_MIN_FREQ_KHZ           400
#define I2C_BUS_MAX_FREQ_KHZ           400



#define BMM_DEFAULT_REG_CFG_RAIL_ON         SNS_RAIL_ON_NPM
#define BMM_DEFAULT_REG_CFG_ISDRI           0
#define BMM_DEFAULT_REG_CFG_HW_ID           1
#define BMM_DEFAULT_REG_CFG_SUPPORT_SYN_STREAM 0

#define bmm_sensor_com_port_spi_config_init_default   { \
  .bus_type          = SNS_BUS_SPI,            \
  .slave_control     = SPI_SLAVE_CONTROL,      \
  .reg_addr_type     = SNS_REG_ADDR_8_BIT,     \
  .min_bus_speed_KHz = SPI_BUS_MIN_FREQ_KHZ,   \
  .max_bus_speed_KHz = SPI_BUS_MAX_FREQ_KHZ,   \
  .bus_instance      = SPI_BUS_INSTANCE        \
  }


#define bmm_sensor_com_port_i2c_config_init_default   { \
  .bus_type          = SNS_BUS_I2C,            \
  .slave_control     = SENSOR_I2C_SLAVE_ADDRESS,   \
  .reg_addr_type     = SNS_REG_ADDR_8_BIT,     \
  .min_bus_speed_KHz = I2C_BUS_MIN_FREQ_KHZ,   \
  .max_bus_speed_KHz = I2C_BUS_MAX_FREQ_KHZ,   \
  .bus_instance      = I2C_BUS_INSTANCE        \
  }

/**
 * Sensor platform resource configuration with Hard Code
 * @param this
 */
void sns_bmm_registry_def_config(sns_sensor *const this)
{
  bmm150_state* sstate = (bmm150_state*) this->state->state;
  sns_com_port_config *com_port_cfg = &sstate->common.com_port_info.com_config;
  BMM_SENSOR_LOG(LOW, this, "sns_bmm_registry_def_config");

  // <general config>
  {
    sstate->is_dri         = BMM_DEFAULT_REG_CFG_ISDRI;
    sstate->hardware_id    = BMM_DEFAULT_REG_CFG_HW_ID;
    sstate->supports_sync_stream = BMM_DEFAULT_REG_CFG_SUPPORT_SYN_STREAM;
    sstate->common.registry_cfg_received = true;
  }

  // <platform configure>
  {
#if BMM_CONFIG_DFT_BUS_SPI
    sns_memscpy(com_port_cfg,
                sizeof(sstate->common.com_port_info.com_config),
                &((sns_com_port_config) bmm_sensor_com_port_spi_config_init_default ),
                sizeof(sns_com_port_config));

#else  // I2C
    sns_memscpy(com_port_cfg,
      sizeof(sstate->common.com_port_info.com_config),
      &((sns_com_port_config) bmm_sensor_com_port_i2c_config_init_default),
      sizeof(sns_com_port_config));
#endif
    sstate->common.registry_rail_on_state  = BMM_DEFAULT_REG_CFG_RAIL_ON;
    strlcpy(sstate->common.rail_config.rails[0].name,
            RAIL_1,
            sizeof(sstate->common.rail_config.rails[0].name));
    strlcpy(sstate->common.rail_config.rails[1].name,
            RAIL_2,
            sizeof(sstate->common.rail_config.rails[1].name));
    sstate->common.rail_config.num_of_rails = 2;

    sstate->common.registry_pf_cfg_received = true;
  }
  // <placement>
  {
    uint8_t i;
    for (i = 0; i < 9; i ++)
    {
      sstate->common.placement[i] = 0;
    }
    sstate->common.registry_placement_received = true;
  }

  // <orient>
  {
    uint8_t i;
    //done in bmm150_common_init
    if(sstate->sensor == BMM150_MAG)
    {
      // initialize axis conversion settings
      for(i = 0; i < TRIAXIS_NUM; i++)
      {
        sstate->common.axis_map[i].opaxis = (triaxis)i;
        sstate->common.axis_map[i].ipaxis = (triaxis)i;
        sstate->common.axis_map[i].invert = false;
      }
    }
    sstate->common.registry_orient_received = true;
  }
  // <fac_cal>
  {
    // <fac_cal/bias>
    {
      float *sensor_fac_cal_bias = sstate->fac_cal_bias;
      if (sstate->sensor == BMM150_MAG)
      {
        sensor_fac_cal_bias[0] = 0.01;
        sensor_fac_cal_bias[1] = 0.01;
        sensor_fac_cal_bias[2] = 0.01;
      }

    }
    // <fac cal/corr_mat>
    {
      if (sstate->sensor == BMM150_MAG)
      {
        uint8_t i;
        for (i = 0; i < 9; i++)
        {
          sstate->fac_cal_corr_mat.data[i] = 0.0;
        }
        sstate->fac_cal_corr_mat.e00 = 1.0;
        sstate->fac_cal_corr_mat.e11 = 1.0;
        sstate->fac_cal_corr_mat.e22 = 1.0;
      }
    }
    // <fac/scale>
    {
      uint8_t i;
      for (i = 0; i < TRIAXIS_NUM; i++)
      {
        sstate->fac_cal_scale[i] = 1.0;
      }
    }
    sstate->registry_fac_cal_received = true;
  }

}
#endif
