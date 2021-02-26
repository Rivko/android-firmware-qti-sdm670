/*========================================================================*/
/**
@file stream_dev_drift_calc.h

@brief Stream to device drift calculation library header.

*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "avsync_stats" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/stream_dev_drift_calc/lib/inc/stream_dev_drift_calc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/31/13    DG      Created file.
========================================================================== */

#ifndef _STREAM_DEV_DRIFT_CALC_H
#define _STREAM_DEV_DRIFT_CALC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifdef TST_STREAM_DEV_DRIFT_CALC
#include "tst_stream_dev_drift_calc.h"
#else
#include "qurt_elite_types.h"
#endif

/**
 * Declaration for the structure which is use to hold the stream to device drift calculation object.
 * There should be one object per stream-device pair.
 */
typedef struct sd_dc_t sd_dc_t;

/**
  Creates a new object of type sd_dc_t. This function allocates memory for the object and initializes
  it.

  @return
  Pointer to the object if successful, NULL otherwise.

  @dependencies
  None.
*/
sd_dc_t *sd_drift_calc_create(void);

/**
  Destroys an object of type sd_dc_t and deallocates the memory associated with it. Once this function
  is called the memory pointed to by the instance pointer becomes invalid.

  @param[in] obj_ptr  Instance pointer.

  @dependencies
  None.
*/
void sd_drift_calc_destroy(sd_dc_t *obj_ptr);

/**
  Provides a pointer to a location of memory that is updated with the drift value by the object.
  This pointer is updated whenever sd_drift_calc_update_drift is called. The drift value is in
  microseconds. It is positive if the input clock (derived from the stream) is faster than the
  output clock (derived from the device drift pointer). This location will be valid as long as the object
  is in existence.

  @param[in] obj_ptr  Instance pointer.

  @return
  Pointer to the memory location that is updated with the drift value.

  @dependencies
  None.
*/
volatile const int32_t *sd_drift_calc_get_drift_pointer(sd_dc_t *obj_ptr);

/**
  Sets the device drift pointer. The format of this pointer is the same as the drift pointer exposed
  in AFEAvtDrift.h. This drift information is used to get the output drift with respect to the AV Timer.

  @param[in] obj_ptr  Instance pointer.
  @param[in] dev_drift_ptr  Pointer to the memory location that holds the device to AV timer drift
                         information. The format is the same as that mentioned in AFEAvtDrift.h.

  @dependencies
  None.
*/
void sd_drift_calc_set_device_drift_pointer(sd_dc_t *obj_ptr, const volatile uint64_t *dev_drift_ptr);

/**
  Updates the drift value. This function must be called whenever a new input buffer arrives. It must
  be called only once per buffer.

  @param[in] obj_ptr  Instance pointer.
  @param[in] ts  Timestamp of the input buffer. The timestamp must be made using the AV Timer.
  @param[in] duration_in_us The duration of the data in this input buffer in microseconds.

  @dependencies
  None.
*/
void sd_drift_calc_update_drift(sd_dc_t *obj_ptr, uint64_t ts, uint32_t duration_in_us);

/**
  Updates the drift value provided by client. This function must be called whenever
  adjust session clock command is issued.

  @param[in] obj_ptr    Instance pointer.
  @param[in] drift_val  Signed drift value

  @dependencies
  None.
*/
void sd_drift_update_given_drift(sd_dc_t* obj_ptr, int32_t drift_val);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _STREAM_DEV_DRIFT_CALC_H */
