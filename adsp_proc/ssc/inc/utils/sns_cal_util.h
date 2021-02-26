#pragma once
/*=============================================================================
  @file sns_cal_util.h

  Calibration Utility functions for use by Sensor Developers.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_math_util.h"

/*=============================================================================
  Function Definitions
  ===========================================================================*/

/**
 * Calculates calibrated value of a sample for tri-axial sensors
 * based on following equation.
 *
 *      Sc = C * (Sr - B)
 *
 * Where,
 *      Sc = calibrated sample (3x1 vector (x, y, z))
 *      Sr = raw sample (3x1 vector (x, y, z))
 *      C = compensation matrix (3x3) B = bias (3x1 vector (x, y, z))
 *
 * @param sample_raw
 * @param bias
 * @param compensation_matrix
 * @return vector3 calibrated sample
 *
 */
static inline vector3 sns_apply_calibration_correction_3(vector3 sample_raw,
                                                  vector3 bias,
                                                  matrix3 compensation_matrix)
{
  return matrix3_mul_vector3(compensation_matrix,
                             vector3_sub(sample_raw, bias));
}
