#pragma once
/**
 * sns_fw_pwr_rail_service.h
 *
 * Framework header for Sensors Power Rail Service.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/framework/inc/sns_fw_pwr_rail_service.h#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 */

/** Forward Declaration. */
typedef struct sns_fw_pwr_rail_service sns_fw_pwr_rail_service;

/**
 * Initialize the power rail service; to be used only by the
 * Service Manager.
 */
sns_fw_pwr_rail_service* sns_pwr_rail_service_init(void);
