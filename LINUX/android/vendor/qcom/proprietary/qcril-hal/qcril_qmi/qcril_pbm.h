/******************************************************************************
#  Copyright (c) 2008-2010, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_pbm.h
  @brief   qcril qmi - PB

  DESCRIPTION

******************************************************************************/


#ifndef QCRIL_PBM_H
#define QCRIL_PBM_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "qcrili.h"
#include "qmi_client.h"
#include "qcril_qmi_client.h"

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

typedef enum
{
  QCRIL_PBM_DEVICE_TYPE_ECC,
  QCRIL_PBM_DEVICE_TYPE_FDN
} qcril_pbm_device_type;

#define NON_STD_OTASP_NUMBER "*228"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void qcril_qmi_pbm_pre_init(void);

void qcril_qmi_pbm_cleanup(void);

void qcril_qmi_pbm_init(void);

int qmi_ril_phone_number_is_emergency(char * number_to_check);

int qmi_ril_phone_number_is_non_std_otasp(const char * number_to_check);

RIL_Errno qcril_qmi_pbm_enable_emergency_number_indications(int enable);

int qmi_ril_set_ecc_property(char* ecc_numbers);

void qcril_qmi_pbm_unsolicited_indication_cb_helper
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
);

RIL_Errno qcril_qmi_pbm_enable_phonebook_ready_indications(int enable);

void qcril_qmi_pbm_get_pb_capacity_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

void qcril_qmi_pbm_get_adn_record_hndlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
);

void qcril_qmi_pbm_update_adn_record_hndlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
);

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_pbm_module_cleanup();
#endif

#ifdef __cplusplus
}
#endif
#endif /* QCRIL_PBM_H */
