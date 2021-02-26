/******************************************************************************

  @file    qcril_mbn_reader.h
  @brief   Provides interface to read mbn file

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_MBN_DB_H
#define QCRIL_MBN_DB_H

#include "qcrili.h"

/*===========================================================================

  FUNCTION qcril_mbn_db_retrieve_hw_mbn_file_for_hw_type

===========================================================================*/
/*!
    @brief
    Retrieves hw mbn file from hw type

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_retrieve_hw_mbn_file_for_hw_type
(
    char   **mbn_file_name,
    char   *device_type
);

/*===========================================================================

  FUNCTION qcril_file_retrieve_mbn_file_for_operator

===========================================================================*/
/*!
    @brief
    Retrieves meta data from mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_retrieve_mbn_file_for_operator
(
    char   *mbn_file_name,
    char   *mcc,
    char   *mnc,
    char   *iin
);


/*===========================================================================

  FUNCTION qcril_file_retrieve_mbn_file_for_version

===========================================================================*/
/*!
    @brief
    Retrieves db data from mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_retrieve_mbn_file_for_version
(
    char   *mbn_file_name,
    char   *version
);

/*===========================================================================

  FUNCTION qcril_file_retrieve_mbn_file_for_version

===========================================================================*/
/*!
    @brief
    Retrieves db data from mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_retrieve_version
(
    char   *version,
    char   *mbn_file_name
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_sw_mbn_file

===========================================================================*/
/*!
    @brief
    Retrieves db data from mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_sw_mbn_file
(
    char   *mbn_file_name
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_hw_mbn_file

===========================================================================*/
/*!
    @brief
    Add hardware mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_hw_mbn_file
(
    char   *mbn_file_name
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_hw_mbn_hw_name
===========================================================================*/
/*!
    @brief
    add hw mbn hw_name

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_hw_mbn_hw_name
(
    char   *mbn_file_name,
    char   *hw_name
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_hw_mbn_qc_version

===========================================================================*/
/*!
    @brief
    Add hw mbn qc_version

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_hw_mbn_qc_version
(
    char   *mbn_file_name,
    int     qc_version
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_hw_mbn_oem_version

===========================================================================*/
/*!
    @brief
    Add hw mbn oem_verion

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_hw_mbn_oem_version
(
    char     *mbn_file_name,
    uint32_t  oem_version
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_sw_mbn_qc_version

===========================================================================*/
/*!
    @brief
    Add sw mbn qc_version

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_sw_mbn_qc_version
(
    char   *mbn_file_name,
    int     qc_version
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_sw_mbn_oem_version

===========================================================================*/
/*!
    @brief
    Add sw mbn oem_version

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_sw_mbn_oem_version
(
    char     *mbn_file_name,
    uint32_t  oem_version
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_sw_mbn_iin

===========================================================================*/
/*!
    @brief
    Add sw mbn iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_sw_mbn_iin
(
    char     *mbn_file_name,
    int       iin_flag,
    int      *iin_list,
    int       iin_list_len,
    char     *volte_info,
    char     *mkt_info,
    char     *lab_info
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_sw_mbn_long_iin

===========================================================================*/
/*!
    @brief
    Add sw mbn long iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_sw_mbn_long_iin
(
    char     *mbn_file_name,
    char     *long_iin_list,
    int      long_iin_list_len,
    char     *volte_info,
    char     *mkt_info,
    char     *lab_info
);

/*===========================================================================

  FUNCTION qcril_mbn_db_add_sw_mbn_mcc_mnc_list

===========================================================================*/
/*!
    @brief
    Add sw mbn mcc_mnc_list

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_sw_mbn_mcc_mnc_list
(
    char     *mbn_file_name,
    int       mcc_mnc_flag,
    size_t    mcc_mnc_list_count,
    size_t    mcc_mnc_list_len, // in bytes
    uint16_t *mcc_mnc_list,
    char     *volte_info,
    char     *mkt_info,
    char     *lab_info
);


/*===========================================================================

  FUNCTION qcril_mbn_db_is_sw_version_updated

===========================================================================*/
/*!
    @brief
    Check if mbn sw_version is updated

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_mbn_db_is_sw_version_updated
(
    char      *mbn_file_name,
    uint32_t  oem_version
);

/*===========================================================================

  FUNCTION qcril_mbn_db_retrieve_sw_mbn_file_for_iccid

===========================================================================*/
/*!
    @brief
    Retrieves iccid from sw mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_retrieve_sw_mbn_file_for_iccid
(
    char   **mbn_file_name,
    char    *iin,
    char    *volte_info,
    char    *mkt_info,
    char    *lab_info
);

/*===========================================================================

  FUNCTION qcril_mbn_db_retrieve_sw_mbn_file_for_long_iccid

===========================================================================*/
/*!
    @brief
    Retrieves long iccid from sw mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_retrieve_sw_mbn_file_for_long_iccid
(
    char   **mbn_file_name,
    char    *long_iin,
    char    *volte_info,
    char    *mkt_info,
    char    *lab_info
);

/*===========================================================================

  FUNCTION qcril_mbn_db_query_sw_mbn_file_with_mcc_mnc

===========================================================================*/
/*!
    @brief
    Retrieves mbn file from mcc mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_query_sw_mbn_file_with_mcc_mnc
(
    char   **mbn_file_name,
    char    *mcc,
    char    *mnc,
    char    *volte_info,
    char    *mkt_info,
    char    *lab_info
);

/*===========================================================================

  FUNCTION qcril_mbn_db_split_version

===========================================================================*/
/*!
    @brief
    Split version info for mbn db version

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_split_version
(
    uint32_t version,
    uint8_t  *family,
    uint8_t  *oem,
    uint8_t  *carrier,
    uint8_t  *minor
);

#endif /* QCRIL_MBN_DB_H */
