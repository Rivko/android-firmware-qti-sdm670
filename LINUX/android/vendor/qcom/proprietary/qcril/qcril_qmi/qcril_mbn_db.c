/******************************************************************************

  @file    qcril_mbn_reader.c
  @brief   Provides interface to read mbn file

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#include "qcril_db.h"
#include "qcril_log.h"

int qcril_db_query_sw_mbn_int_from_file
(
    char        *mbn_file_name,
    char        *type,
    uint32_t    *value
);

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
)
{
    QCRIL_LOG_FUNC_ENTRY();
    return qcril_db_query_mbn_file_name_from_hw_type(mbn_file_name, device_type);
    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION qcril_mbn_db_add_sw_mbn_file

===========================================================================*/
/*!
    @brief
    Add software mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_db_add_sw_mbn_file
(
    char   *mbn_file_name
)
{
    return qcril_db_insert_sw_mbn_file(mbn_file_name);
}

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
)
{
    return qcril_db_insert_hw_mbn_file(mbn_file_name);
}

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
)
{
    IxErrnoType ret = E_FAILURE;

    ret = qcril_db_update_hw_mbn_file_with_str(mbn_file_name, "HW_NAME", hw_name);
    if (strstr(hw_name, "LA_SS") || strstr(hw_name, "LA-SS"))
    {
        ret = qcril_db_update_hw_mbn_file_with_str(mbn_file_name, "SHORT_NAME", "LA_SS");
    }
    else if (strstr(hw_name, "LA_SS") || strstr(hw_name, "LA-DS"))
    {
        ret = qcril_db_update_hw_mbn_file_with_str(mbn_file_name, "SHORT_NAME", "LA_DS");
    }

    return ret;
}

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
)
{
    uint8_t  *versions = (uint8_t *)&version;

    *family  = versions[3];
    *oem     = versions[2];
    *carrier = versions[1];
    *minor   = versions[0];

    return E_SUCCESS;
}

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
)
{
    uint8_t family  = 0;
    uint8_t oem     = 0;
    uint8_t carrier = 0;
    uint8_t minor   = 0;

    qcril_mbn_db_split_version(qc_version, &family, &oem, &carrier, &minor);

    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "QC_VERSION_FAMILY", family);
    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "QC_VERSION_OEM", oem);
    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "QC_VERSION_CARRIER", carrier);
    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "QC_VERSION_MINOR", minor);
    return 0;
}

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
)
{
    uint8_t family  = 0;
    uint8_t oem     = 0;
    uint8_t carrier = 0;
    uint8_t minor   = 0;

    qcril_mbn_db_split_version(oem_version, &family, &oem, &carrier, &minor);

    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_FAMILY", family);
    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_OEM", oem);
    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_CARRIER", carrier);
    qcril_db_update_hw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_MINOR", minor);
    return 0;
}

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
)
{
    uint8_t family  = 0;
    uint8_t oem     = 0;
    uint8_t carrier = 0;
    uint8_t minor   = 0;
    IxErrnoType ret = E_FAILURE;

    qcril_mbn_db_split_version(qc_version, &family, &oem, &carrier, &minor);

    do
    {
        ret = qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "QC_VERSION_FAMILY", family);
        if(E_FAILURE == ret)
        {
            break;
        }

        ret = qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "QC_VERSION_OEM", oem);
        if(E_FAILURE == ret)
        {
            break;
        }

        ret = qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "QC_VERSION_CARRIER", carrier);
        if(E_FAILURE == ret)
        {
            break;
        }

        ret = qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "QC_VERSION_MINOR", minor);
        if(E_FAILURE == ret)
        {
            break;
        }
    }while(FALSE);

    return ret;
}

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
)
{
    uint8_t family  = 0;
    uint8_t oem     = 0;
    uint8_t carrier = 0;
    uint8_t minor   = 0;

    qcril_mbn_db_split_version(oem_version, &family, &oem, &carrier, &minor);

    qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_FAMILY", family);
    qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_OEM", oem);
    qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_CARRIER", carrier);
    qcril_db_update_sw_mbn_file_with_int(mbn_file_name, "MCFG_VERSION_MINOR", minor);
    return 0;
}

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
)
{
    char iin[17] = {0};
    int  index   = 0;
    IxErrnoType ret = E_FAILURE;

    if (mbn_file_name)
    {
        if (iin_flag)
        {
            ret = qcril_db_insert_sw_mbn_iin(mbn_file_name, "wild", volte_info, mkt_info, lab_info);
        }
        else if(iin_list && iin_list_len)
        {
            for (index = 0; index < iin_list_len; index++)
            {
                snprintf(iin, 16, "%d", iin_list[index]);
                ret = qcril_db_insert_sw_mbn_iin(mbn_file_name, iin, volte_info, mkt_info, lab_info);
            }
        }
        else
        {
            ret = E_FAILURE;
        }
    }

    return ret;
}

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
)
{
#define LONG_IIN_LENGTH    (11)

    char *iin = NULL;
    char *lastPtr = NULL;
    int  size   = 0;
    IxErrnoType ret = E_FAILURE;

    if (mbn_file_name && long_iin_list)
    {
        iin = strtok_r(long_iin_list, ",", &lastPtr);
        while (iin != NULL && ret)
        {
            size = lastPtr - long_iin_list;
            if (size > long_iin_list_len)
            {
                QCRIL_LOG_ERROR("add long iin fail. iin list longer than iin len");
                ret = E_FAILURE;
                break;
            }
            ret = qcril_db_insert_sw_mbn_long_iin(mbn_file_name, iin, volte_info, mkt_info, lab_info);
            iin = strtok_r(NULL, ",", &lastPtr);
        }
        if (size < long_iin_list_len)
        {
            QCRIL_LOG_ERROR("add long iin fail. iin list shorter than iin len");
            ret = E_FAILURE;
        }
    }

    return ret;
}

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
)
{
    char mcc_str[4] = {0};
    char mnc_str[4] = {0};
    size_t index = 0;
    IxErrnoType ret = E_FAILURE;

    if (mbn_file_name && mcc_mnc_list && mcc_mnc_list_count)
    {
        if (mcc_mnc_flag)
        {
            ret = qcril_db_insert_sw_mbn_iin(mbn_file_name, "wild", volte_info, mkt_info, lab_info);
        }
        else
        {
            if ((mcc_mnc_list_count*sizeof(*mcc_mnc_list)*2) != mcc_mnc_list_len)
            {
                QCRIL_LOG_DEBUG("mcc_mnc_list count doesn't match length, count %u, length %u",
                                mcc_mnc_list_count,
                                mcc_mnc_list_len);
            }
            else {
                // mcc_mnc_list[0]: mcc[0], mcc_mnc_list[1]: mnc[0], mcc_mnc_list[2]: mcc[1], ...
                for (index = 0; index < mcc_mnc_list_count; index++)
                {
                    snprintf(mcc_str, 4, "%u", mcc_mnc_list[index*2]);
                    snprintf(mnc_str, 4, "%u", mcc_mnc_list[index*2+1]);
                    ret = qcril_db_insert_sw_mbn_mcc_mnc(mbn_file_name, mcc_str, mnc_str,
                                                          volte_info, mkt_info, lab_info);
                    if (ret != E_SUCCESS)
                    {
                        break;
                    }
                }
            }
        }
    }

    return ret;
}

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
)
{
    return qcril_db_query_sw_mbn_file_with_mcc_mnc(mbn_file_name, mcc, mnc,
                                                   volte_info, mkt_info, lab_info);
}

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
)
{
    return qcril_db_query_mbn_file_name_from_iin(mbn_file_name, iin, volte_info, mkt_info, lab_info);
}

/*===========================================================================

  FUNCTION qcril_mbn_db_retrieve_sw_mbn_file_for_long_iccid

===========================================================================*/
/*!
    @brief
    Retrieves 11-bit iccid from sw mbn file

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
)
{
    return qcril_db_query_mbn_file_name_from_long_iin(mbn_file_name, long_iin,
                                                      volte_info, mkt_info, lab_info);
}

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
)
{
    uint8_t family_old  = 0;
    uint8_t oem_old     = 0;
    uint8_t carrier_old = 0;
    uint8_t minor_old   = 0;

    uint32_t family_new  = 0;
    uint32_t oem_new     = 0;
    uint32_t carrier_new = 0;
    uint32_t minor_new   = 0;

    int      is_updated = 1;

    if (mbn_file_name)
    {
        QCRIL_LOG_DEBUG("config_version %x", oem_version);
        qcril_mbn_db_split_version(oem_version, &family_old, &oem_old, &carrier_old, &minor_old);

        qcril_db_query_sw_mbn_int_from_file(mbn_file_name, "MCFG_VERSION_FAMILY", &family_new);
        qcril_db_query_sw_mbn_int_from_file(mbn_file_name, "MCFG_VERSION_OEM", &oem_new);
        qcril_db_query_sw_mbn_int_from_file(mbn_file_name, "MCFG_VERSION_CARRIER", &carrier_new);
        qcril_db_query_sw_mbn_int_from_file(mbn_file_name, "MCFG_VERSION_MINOR", &minor_new);

        QCRIL_LOG_DEBUG("family_new %x, family_old %x", family_new, family_old);
        QCRIL_LOG_DEBUG("oem_new    %x, oem_old    %x", oem_new, oem_old);
        QCRIL_LOG_DEBUG("carrier_new %x, carrier_old %x", carrier_new, carrier_old);
        QCRIL_LOG_DEBUG("minor_new %x, minor_old %x", minor_new, minor_old);

        if ((family_old == family_new) &&
            (oem_old == oem_new) &&
            (carrier_new == carrier_old) &&
            (minor_new == minor_old))
        {
            is_updated = 0;
        }
    }
    else
    {
        is_updated = 0;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(is_updated)
    return is_updated;
}
