#pragma once
/**
 * @file sns_ak0991x_sensor.h
 *
 * AK0991X Sensor implementation.
 *
 * Copyright (c) 2016-2017 Asahi Kasei Microdevices
 * All Rights Reserved.
 * Confidential and Proprietary - Asahi Kasei Microdevices
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include "sns_ak0991x_lite.h"
#include "sns_sensor.h"
#include "sns_data_stream.h"
#include "sns_sensor_uid.h"
#include "sns_pwr_rail_service.h"
#include "sns_suid_util.h"

#include "sns_ak0991x_hal.h"

#include "sns_ak0991x_sensor_instance.h"
#include "sns_math_util.h"
#include "sns_diag_service.h"
#include "sns_registry_util.h"

#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
#include "sns_device_mode.pb.h"
#endif //AK0991X_ENABLE_DEVICE_MODE_SENSOR

#define MAG_SUID1 \
  {  \
    .sensor_uid =  \
    {  \
      0x0f, 0x1e, 0x04, 0xec, 0x30, 0xcf, 0x4d, 0xa5,  \
      0xbc, 0x79, 0xd3, 0x09, 0x4d, 0x60, 0xd5, 0xeb  \
    }  \
  }

#ifdef AK0991X_ENABLE_DUAL_SENSOR
#define MAG_SUID2 \
  {  \
    .sensor_uid =  \
    {  \
      0xaf, 0xf9, 0x3c, 0x5a, 0xc8, 0xc5, 0x45, 0x70, \
      0xb4, 0xbe, 0xc1, 0xa5, 0x7b, 0x8d, 0x5f, 0xec  \
    }  \
  }
#endif

//#ifdef AK0991X_ENABLE_REGISTRY_ACCESS
#define AK0991X_STR                     "ak0991x_"
#define AK0991X_PLATFORM_CONFIG_STR     "_platform.config"
#define AK0991X_PLATFORM_PLACEMENT_STR  "_platform.placement"
#define AK0991X_PLATFORM_ORIENT_STR     "_platform.orient"
#define AK0991X_PLATFORM_FACCAL_STR     "_platform.mag.fac_cal"
#define AK0991X_MAG_CONFIG_STR          ".mag.config"
#define AK0991X_REG_CONFIG_STR          ".mag.config_2"
//#endif // AK0991X_ENABLE_REGISTRY_ACCESS

#if 0
/** TODO Using 8996 Platform config as defaults. This is for
 *  test purpose only. All platform specific information will
 *  be available to the Sensor driver via Registry. */
#define RAIL_1                     "/pmic/client/sensor_vddio"
#define RAIL_2                     "/pmic/client/sensor_vdd"
#define IRQ_NUM                    119
#define NUM_OF_RAILS               2
#define I2C_BUS_FREQ               400
#define I2C_SLAVE_ADDRESS          0x0C
#ifdef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
#ifdef USE_RUMI_SE
#define I2C_BUS_INSTANCE           0x06
#else
#define I2C_BUS_INSTANCE           0x01
#endif /* USE_RUMI_SE */
#define SPI_BUS_INSTANCE           0x02
#else
#define I2C_BUS_INSTANCE           0x03
#define SPI_BUS_INSTANCE           0x01
#endif
#define SPI_BUS_MIN_FREQ_KHZ       0        // 0MHz
#define SPI_BUS_MAX_FREQ_KHZ       33 * 100 // 3MHz
#define SPI_SLAVE_CONTROL          0x0
#endif // AK0991X_USE_DEFAULTS

/** Forward Declaration of Magnetic Sensor API */
extern sns_sensor_api ak0991x_mag_sensor_api;

/**
 * AK0991X ODR definitions
 */
#define AK0991X_ODR_0              0.0
#define AK0991X_ODR_1              1.0
#define AK0991X_ODR_10             10.0
#define AK0991X_ODR_20             20.0
#define AK0991X_ODR_50             50.0
#define AK0991X_ODR_100            100.0
#define AK0991X_ODR_200            200.0

/**
 * Magnetometer ranges
 */
#define AK09918_MIN_RANGE          -4912 /* Minimum -4912uT */
#define AK09918_MAX_RANGE          4912  /* Maximum  4912uT */
#define AK09917_MIN_RANGE          -4912 /* Minimum -4912uT */
#define AK09917_MAX_RANGE          4912  /* Maximum  4912uT */
#define AK09916_MIN_RANGE          -4912 /* Minimum -4912uT */
#define AK09916_MAX_RANGE          4912  /* Maximum  4912uT */
#define AK09915_MIN_RANGE          -4912 /* Minimum -4912uT */
#define AK09915_MAX_RANGE          4912  /* Maximum  4912uT */
#define AK09913_MIN_RANGE          -4912 /* Minimum -4912uT */
#define AK09913_MAX_RANGE          4912  /* Maximum  4912uT */
#define AK09912_MIN_RANGE          -4912 /* Minimum -4912uT */
#define AK09912_MAX_RANGE          4912  /* Maximum  4912uT */
#define AK09911_MIN_RANGE          -4912 /* Minimum -4912uT */
#define AK09911_MAX_RANGE          4912  /* Maximum  4912uT */

/**
 * Magnetometer resolution
 */
#define AK09918_RESOLUTION         (0.15f) /* uT/LSB */
#define AK09917_RESOLUTION         (0.15f) /* uT/LSB */
#define AK09916_RESOLUTION         (0.15f) /* uT/LSB */
#define AK09915_RESOLUTION         (0.15f) /* uT/LSB */
#define AK09913_RESOLUTION         (0.15f) /* uT/LSB */
#define AK09912_RESOLUTION         (0.15f) /* uT/LSB */
#define AK09911_RESOLUTION         (0.6f)  /* uT/LSB */

/* Power consumption limits */
#define AK09918_LO_PWR             1    /* unit of uA */
#define AK09918_HI_PWR             1100 /* unit of uA @ 100Hz */
#define AK09917_LO_PWR             1    /* unit of uA */
#define AK09917_HI_PWR             900  /* unit of uA @ 100Hz low power (TBD copy from AK09915)*/
#define AK09916_LO_PWR             1    /* unit of uA */
#define AK09916_HI_PWR             1100 /* unit of uA @ 100Hz */
#define AK09915_LO_PWR             3    /* unit of uA */
#define AK09915_HI_PWR             900  /* unit of uA @ 100Hz low power */
#define AK09913_LO_PWR             3    /* unit of uA */
#define AK09913_HI_PWR             1500 /* unit of uA @ 100Hz */
#define AK09912_LO_PWR             3    /* unit of uA */
#define AK09912_HI_PWR             1000 /* unit of uA @ 100Hz */
#define AK09911_LO_PWR             3    /* unit of uA */
#define AK09911_HI_PWR             2400 /* unit of uA @ 100Hz */

/** Supported opertating modes */
#define AK0991X_NORMAL             "NORMAL"
#define AK0991X_LOW_POWER          "LOW_POWER"
#define AK0991X_LOW_NOISE          "LOW_NOISE"

#define AK0991X_NUM_OF_ATTRIBUTES  (21)

/** Power rail timeout States for the AK0991X Sensors.*/

#define AK0991X_POWER_RAIL_OFF_TIMEOUT_NS 1000000000ULL /* 1 second */
typedef enum
{
  AK0991X_POWER_RAIL_PENDING_NONE,
  AK0991X_POWER_RAIL_PENDING_INIT,
  AK0991X_POWER_RAIL_PENDING_SET_CLIENT_REQ,
  AK0991X_POWER_RAIL_PENDING_OFF,
} ak0991x_power_rail_pending_state;

#ifdef AK0991X_ENABLE_REGISTRY_ACCESS
/** Registry items supported as part of physical sensor
 *  configuraton registry group
 */
typedef struct ak0991x_registry_phy_sensor_cfg
{
  bool    use_fifo;
  uint8_t nsf;
  uint8_t sdr;
} ak0991x_registry_phy_sensor_cfg;
#endif



/** Interrupt Sensor State. */

typedef struct ak0991x_state
{
  sns_data_stream       *reg_data_stream;
  sns_data_stream       *fw_stream;
  sns_data_stream       *timer_stream;

  // Registry, IRQ, Timer, ASCP, DAE, device_mode
  SNS_SUID_LOOKUP_DATA(6) suid_lookup_data;

  ak0991x_com_port_info com_port_info;
  sns_interrupt_req      irq_config;

  sns_pwr_rail_service  *pwr_rail_service;
  sns_rail_config       rail_config;
  sns_power_rail_state  registry_rail_on_state;

  bool hw_is_present;
  bool sensor_client_present;
  bool remove_timer_stream;

  ak0991x_power_rail_pending_state power_rail_pend_state;

  // parameters which are determined when the connected device is specified.
  akm_device_type device_select; // store the current connected device
  float sstvt_adj[AK0991X_NUM_SENSITIVITY];

  // sensor configuration
  bool is_dri;
  bool supports_sync_stream;
  bool use_fifo;
  uint8_t nsf;
  uint8_t sdr;
  uint8_t resolution_idx;
  int64_t hardware_id;
#ifdef AK0991X_ENABLE_DUAL_SENSOR
  uint32_t registration_idx;
#endif

#ifdef AK0991X_ENABLE_REGISTRY_ACCESS
  // registry sensor config
  bool registry_cfg_received;
  sns_registry_phy_sensor_cfg registry_cfg;
  // registry sensor reg config
  bool registry_reg_cfg_received;
  ak0991x_registry_phy_sensor_cfg registry_reg_cfg;
  // registry sensor platform config
  bool registry_pf_cfg_received;
  sns_registry_phy_sensor_pf_cfg registry_pf_cfg;
  // axis conversion
  bool registry_orient_received;
#endif

  triaxis_conversion axis_map[TRIAXIS_NUM];

  // factory calibration
  ak0991x_cal_param cal_parameter[MAX_DEVICE_MODE_SUPPORTED];
  uint32_t                fac_cal_version;

  // placement
  bool                    registry_placement_received;
  float                   placement[12];

  // debug
  uint16_t who_am_i;
  sns_diag_service *diag_service;
  sns_sync_com_port_service *scp_service;
  size_t   encoded_event_len;
} ak0991x_state;

/** Functions shared by all AK0991X Sensors */
/**
 * notify_event() Sensor API common between all AK0991X Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc ak0991x_sensor_notify_event(sns_sensor *const this
);

/**
 * set_client_request() Sensor API common between all AK0991X
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance *ak0991x_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove
);

/**
 * Initializes Magnetic Sensor attributes.
 *
 * @param this   Sensor reference
 * @param[i]     device ID
 *
 * @return none
 */
void ak0991x_mag_init_attributes(sns_sensor *const this,
                                 akm_device_type device_select
);


sns_rc ak0991x_mag_init(sns_sensor *const this);
#ifdef AK0991X_ENABLE_DEINIT
sns_rc ak0991x_mag_deinit(sns_sensor *const this);
#endif

sns_rc ak0991x_mag_match_odr(float desired_sample_rate,
                             float *chosen_sample_rate,
                             ak0991x_mag_odr *chosen_reg_value,
                             akm_device_type device_select);
void ak0991x_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance);

void ak0991x_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance);
