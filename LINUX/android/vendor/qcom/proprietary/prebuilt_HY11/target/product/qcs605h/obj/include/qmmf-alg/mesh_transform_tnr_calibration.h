/****************************************************************************
 * Copyright (c) 2019 Qualcomm Technologies, Inc.                           *
 * All Rights Reserved.                                                     *
 * Confidential and Proprietary - Qualcomm Technologies, Inc.               *
 ****************************************************************************/

/*! @file mesh_transform_tnr_calibration.h
 */

#pragma once

/// @namespace qmmf::qmmf_alg_plugin::mesh_transform_tnr
namespace qmmf {
namespace qmmf_alg_plugin {
namespace mesh_transform_tnr {

/**
 * An enum.
 * mesh_transform tnr supported operation modes.
 */
typedef enum {
  /// Only mesh_transform
  MESH_TRANSFORM = 0,

  /// Mesh Transform and fast tnr mode
  MESH_TRANSFORM_FAST_TNR = 1,

  /// Mesh Transform and quality tnr mode
  MESH_TRANSFORM_QUALITY_TNR = 2,
} mesh_transform_tnr_mode_t;

/**
 * A structure.
 * mesh transform tnr calibration data.
 */
typedef struct {
  mesh_transform_tnr_mode_t mode;  ///  mesh transform tnr operation mode
} mesh_transform_tnr_calibration_t;

}  // namespace mesh_transform_tnr
}  // namespace qmmf_alg_plugin
}  // namespace qmmf