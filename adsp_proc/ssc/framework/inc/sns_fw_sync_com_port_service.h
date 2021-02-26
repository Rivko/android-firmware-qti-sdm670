#pragma once
/**
 * sns_fw_pwr_sync_com_port_service.h
 *
 * Framework header for Sensors Sync Com Port Service.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/framework/inc/sns_fw_sync_com_port_service.h#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 */

/** Forward Declaration. */
typedef struct sns_sync_com_port_service sns_sync_com_port_service;

/**
 * Initialize the sync com port service; to be used only by the
 * Service Manager.
 */
sns_sync_com_port_service* sns_sync_com_port_service_init(void);
