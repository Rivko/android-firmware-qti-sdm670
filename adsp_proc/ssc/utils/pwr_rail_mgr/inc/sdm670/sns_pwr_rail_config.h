#pragma once
/*
 * File : sns_pwr_rail_config.h
 * Description : This file contains the sensors power
 *               rail configuration.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

/**
 *  SNS_VDDIO_PWR_RAIL_ALWAYS_ON governs the power rail ON state.
 *  Due to differences in PMIC design on ADSP vs SLPI targets,
 *  this macro is added to have the sensor power rail always ON
 *  for ADSP targets to achieve Rock Bottom State Current (RBSC).
 *  For SLPI targets the macro is disabled.
 */  
#define SNS_VDDIO_PWR_RAIL_ALWAYS_ON 			0x1

