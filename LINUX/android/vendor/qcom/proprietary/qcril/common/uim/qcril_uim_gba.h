#ifndef QCRIL_UIM_GBA_H
#define QCRIL_UIM_GBA_H
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/15   at      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "qcril_uim.h"

/*===========================================================================

                           TYPES

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_oem_get_impi_resp

===========================================================================*/
/*!
    @brief
    Handles the response of OEM get IMPI request.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_oem_get_impi_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_oem_gba_resp

===========================================================================*/
/*!
    @brief
    Handles the response of OEM GBA initialization request

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_oem_gba_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_oem_request_gba_init

===========================================================================*/
void qcril_uim_oem_request_gba_init
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_oem_request_get_impi

===========================================================================*/
void qcril_uim_oem_request_get_impi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

#endif /* QCRIL_UIM_GBA_H */

