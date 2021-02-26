/*========================================================================

This file contains declarations for raw compressed passthrough lib

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/06/16   YW      Created file.

========================================================================== */

#ifndef _RAWPASSTHRU_H_
#define _RAWPASSTHRU_H_

#ifndef RAW_PASSTHRU_UNIT_TEST
#include "qurt_elite_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/* =======================================================================
Constant Definitions
========================================================================== */
   //error codes
#define RAW_PASSTHRU_NO_ERROR                  0
#define RAW_PASSTHRU_ERROR                     1


/* =======================================================================
 Type  declarations
========================================================================== */
/* =======================================================================
Function declarations
========================================================================== */
   uint32_t raw_passthru(uint8_t *input_buffer_ptr,
         uint32_t input_buffer_size,
         uint32_t *bytes_consumed,
         uint8_t *output_buffer_ptr,
         uint32_t output_buffer_size,
         uint32_t *output_size);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*ifndef _RAWPASSTHRU_H_*/
