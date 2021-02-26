#pragma once
/*============================================================================
  @file sns_log_codes.h

  @brief
  Diag log codes for Sensors.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/


#if defined(SSC_TARGET_HEXAGON)

  #include "log_codes.h"

#elif defined(SSC_TARGET_X86)
  #define LOG_1X_BASE_C                 (0x1000)
  #define LOG_CONVERTED_SENSOR_DATA_C   ((0x4C1) + LOG_1X_BASE_C)
  #define LOG_SNS_QMD_RESULT_C          ((0x9CA) + LOG_1X_BASE_C)
  #define LOG_SNS_SENSOR1_REQUEST_C     ((0x9C8) + LOG_1X_BASE_C)
  #define LOG_SNS_SENSOR1_RESPONSE_C    ((0x9CB) + LOG_1X_BASE_C)
  #define LOG_SNS_SENSOR1_INDICATION_C  ((0x9C9) + LOG_1X_BASE_C)
  #define LOG_SNS_SMR_REQUEST_C         ((0x9C5) + LOG_1X_BASE_C)
  #define LOG_SNS_SMR_RESPONSE_C        ((0x9C6) + LOG_1X_BASE_C)
  #define LOG_SNS_SMR_INDICATION_C      ((0x9C7) + LOG_1X_BASE_C)
  #define LOG_SNS_DRV_MD_IRQ_C          ((0x9CC) + LOG_1X_BASE_C)
  #define LOG_SENSORS_SDP_INTERNAL      ((0x9CD) + LOG_1X_BASE_C)

#endif
