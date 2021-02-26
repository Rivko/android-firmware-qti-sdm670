/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qcril_mbn_reader.h
  @brief   Provides interface to read mbn file

  DESCRIPTION

******************************************************************************/

#ifndef QCRIL_MBN_META_H
#define QCRIL_MBN_META_H

#include "qcrili.h"

#define QCRIL_MBN_SW_OEM_VERSION_TYPE  (0x01)
#define QCRIL_MBN_SW_CARRIER_NAME_TYPE (0x03)
#define QCRIL_MBN_SW_IIN_LIST_TYPE     (0x04)
#define QCRIL_MBN_SW_QC_VERSION_TYPE   (0x05)
#define QCRIL_MBN_SW_MCC_MNC_LIST_TYPE (0x06)
#define QCRIL_MBN_SW_CAPABILITY_TYPE   (0x07)
#define QCRIL_MBN_SW_LONG_IIN_LIST_TYPE (0x09)

#define QCRIL_MBN_HW_OEM_VERSION_TYPE  (0x01)
#define QCRIL_MBN_HW_HWID_NAME_TYPE    (0x03)
#define QCRIL_MBN_HW_QC_VERSION_TYPE   (0x05)

#define QCRIL_MBN_SW_MKT_INFO_MASK     (0x02)
#define QCRIL_MBN_SW_VOLTE_INFO_MASK   (0x04)
#define QCRIL_MBN_SW_LAB_INFO_MASK     (0x08)

#ifdef QMI_RIL_UTF
#define QCRIL_MBN_COPY_COMPLETE_FILE "data/misc/radio/copy_complete"
#define QCRIL_MBN_COPY_COMPLETE_DIR  "data/misc/radio"
#else
#define QCRIL_MBN_COPY_COMPLETE_FILE DATA_PARTITION_ROOT "radio/copy_complete"
#define QCRIL_MBN_COPY_COMPLETE_DIR  DATA_PARTITION_ROOT "radio"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

  FUNCTION qcril_file_retrieve_mbn_meta_data

===========================================================================*/
/*!
    @brief
    Retrieves meta data from mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_mbn_meta_data
(
    char           *mbn_file_name,
    unsigned char **meta_data,
    unsigned int   *meta_data_len
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_meta_retrieve_qc_version

===========================================================================*/
/*!
    @brief
    Retrieves qc version from software meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_sw_qc_version
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    int* qc_version
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_meta_retrieve_oem_version

===========================================================================*/
/*!
    @brief
    Retrieves oem version from software meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_sw_oem_version
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    int* oem_version
);

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_sw_capability

===========================================================================*/
/*!
    @brief
    Retrieves capability from software meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_sw_capability
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    uint32_t* capability
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_meta_retrieve_iin_list

===========================================================================*/
/*!
    @brief
    Retrieves iin list from software meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_sw_iin_list
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    int* iin_flag,
    int* iin_list_count,
    int** iin_list
);

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_sw_long_iin_list

===========================================================================*/
/*!
    @brief
    Retrieves long iin list from software meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_sw_long_iin_list
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    int* long_iin_list_len,
    char** long_iin_list
);

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_sw_mcc_mnc_list

===========================================================================*/
/*!
    @brief
    Retrieves mcc mnc list from software meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_sw_mcc_mnc_list
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    int* mcc_mnc_flag,
    size_t* mcc_mnc_list_count,
    size_t* mcc_mnc_list_len,
    uint16_t** mcc_mnc_list
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_meta_retrieve_carrier_name

===========================================================================*/
/*!
    @brief
    Retrieves carrier name from software meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_sw_carrier_name
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    char** carrier_name
);


/*===========================================================================

  FUNCTION qcril_mbn_hw_meta_retrieve_qc_version

===========================================================================*/
/*!
    @brief
    Retrieves qc version from hardware meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_hw_qc_version
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    int* qc_version
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_meta_retrieve_oem_version

===========================================================================*/
/*!
    @brief
    Retrieves oem version from hardware meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_hw_oem_version
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    int* oem_version
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_meta_retrieve_hwid_name

===========================================================================*/
/*!
    @brief
    Retrieves HWID name from hardware meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_hw_name
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    char** hwid_name
);

/*===========================================================================

  FUNCTION qcril_mbn_is_copy_completed

===========================================================================*/
/*!
    @brief
    is mbn file copy completed

    @return
    0 or 1
*/
/*=========================================================================*/
int qcril_mbn_is_copy_completed
(
    void
);

#ifdef __cplusplus
}
#endif

#endif /* QCRIL_MBN_META_H */
