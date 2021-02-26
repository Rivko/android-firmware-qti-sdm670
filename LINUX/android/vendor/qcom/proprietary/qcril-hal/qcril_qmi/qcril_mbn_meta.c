/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qcril_mbn_reader.c
  @brief   Provides interface to read mbn file

  DESCRIPTION

******************************************************************************/

#include "qcril_file_utils.h"
#include "qcril_log.h"
#include "qcril_mbn_meta.h"


#define QCRIL_MBN_CONFIG_INFO_MAGIC  "MCFG_TRL"

/*===========================================================================

  FUNCTION qcril_mbn_is_copy_completed

===========================================================================*/
/*!
    @brief
    is mbn file copy completed

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_mbn_is_copy_completed
(
    void
)
{
    char   compy_completed_buf[2]   = {0};
#ifndef QMI_RIL_UTF
    static int is_copy_completed    = 0;
#else
    int is_copy_completed           = 0;
#endif
    void        *file_desc          = NULL;

    if (!is_copy_completed)
    {
        file_desc = qcril_file_open_file_for_read(QCRIL_MBN_COPY_COMPLETE_FILE);
        if (file_desc)
        {
            if (1 == qcril_file_read_bytes_from_front(file_desc,
                                                    compy_completed_buf, 1))
            {
                if (!strcmp(compy_completed_buf, "1"))
                {
                    is_copy_completed = 1;
                }
            }

            qcril_file_close_file(file_desc);
        }
    }

    return is_copy_completed;
}

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
)
{
    IxErrnoType  ret                = E_FAILURE;
    void        *file_desc          = NULL;
    int          meta_start_offset  = -1;

#define QCRIL_MBN_META_OFFSET_SIZE   (4)
#define QCRIL_MBN_META_DATA_LEN_SIZE (4)

    QCRIL_LOG_FUNC_ENTRY();
    do {

        if (!mbn_file_name || !meta_data || !meta_data_len)
        {
            break;
        }

        file_desc = qcril_file_open_file_for_read(mbn_file_name);
        if (!file_desc)
        {
            break;
        }

        if (QCRIL_MBN_META_OFFSET_SIZE != qcril_file_read_bytes_till_end(
                                                file_desc,
                                                &meta_start_offset,
                                                QCRIL_MBN_META_OFFSET_SIZE,
                                                QCRIL_MBN_META_OFFSET_SIZE))
        {
            break;
        }

        QCRIL_LOG_INFO("meta_start_offset %d", meta_start_offset);

        if (QCRIL_MBN_META_DATA_LEN_SIZE != qcril_file_read_bytes_till_end(
                                                file_desc,
                                                meta_data_len,
                                                meta_start_offset,
                                                QCRIL_MBN_META_DATA_LEN_SIZE))
        {
            break;
        }

        QCRIL_LOG_INFO("meta_data_len %d", *meta_data_len);
        if ((*meta_data_len < QCRIL_MBN_META_DATA_LEN_SIZE) || (*meta_data_len > ((unsigned int)meta_start_offset)))
        {
            break;
        }

        *meta_data = calloc(1, *meta_data_len);
        if (!(*meta_data))
        {
            break;
        }

        int meta_bytes_read = qcril_file_read_bytes_till_end(
                               file_desc,
                               *meta_data,
                               meta_start_offset - QCRIL_MBN_META_DATA_LEN_SIZE,
                               *meta_data_len - QCRIL_MBN_META_DATA_LEN_SIZE);

        QCRIL_LOG_INFO("meta_bytes_read %d", meta_bytes_read);
        if ( (meta_bytes_read < 0 ) || (*meta_data_len -  QCRIL_MBN_META_DATA_LEN_SIZE) != (unsigned int)meta_bytes_read)
        {
            QCRIL_LOG_INFO("meta_data_len not equal to meta_bytes_read");
            break;
        }

        qcril_file_close_file(file_desc);
        ret = E_SUCCESS;
    } while (0);

    QCRIL_LOG_FUNC_RETURN();
    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_type_value_from_meta_data

===========================================================================*/
/*!
    @brief
    Retrieves TLV from meta data

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_meta_retrieve_type_value_from_meta_data
(
    unsigned char  *meta_data,
    unsigned int    meta_data_len,
    unsigned int    type,
    unsigned char **value,
    unsigned int   *length
)
{
#define QCRIL_MBN_CONFIG_INFO_MAGIC_LEN  (8 + 8)

    unsigned int   index    = 0;
    unsigned int   tlv_len  = 0;
    IxErrnoType    ret      = E_FAILURE;

    do {

        if (!meta_data || !meta_data_len || !value || !length)
        {
            break;
        }

        index += QCRIL_MBN_CONFIG_INFO_MAGIC_LEN;

        while (index < meta_data_len)
        {
            if (meta_data[index] == type)
            {
                break;
            }

            index++;

            /* Since we are reading 2 bytes, need to consider index +1 */
            if (index + 1 >= meta_data_len) break;

            tlv_len = *((int16_t*)&meta_data[index]);
            index += 2 + tlv_len;
        }

        if (index >= meta_data_len)
        {
            break;
        }

        index++;

        if (index + 1 >= meta_data_len) break;

        tlv_len = *((int16_t*)&meta_data[index]);
        *length = tlv_len;
        *value = calloc(1, tlv_len);

        if(*value)
        {
            index += 2;
            if ( (index + tlv_len) >= meta_data_len ) {
                QCRIL_LOG_DEBUG("Trying to read beyond available data");
                free(*value);
                *value = NULL;
                break;
            }
            memcpy (*value, &meta_data[index], tlv_len);
        }
        else
        {
            QCRIL_LOG_DEBUG("Failed to allocate mem for value");
        }

        ret = E_SUCCESS;
    } while (0);

    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_sw_qc_version

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;

    do
    {
        if (!qc_version)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_SW_QC_VERSION_TYPE,
                                                                 &value, &length );
        if (ret != E_SUCCESS)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length: %d", length);
        if (length != 4)
        {
            QCRIL_LOG_DEBUG("%s", "length should be 4, exit");
            ret = E_FAILURE;
        }

        if (value)
        {
            // qc_version 4 bytes
            *qc_version = *((int32_t*)value);
        }
    } while(0);

    if (value) {
        free(value);
    }

    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_sw_oem_version

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    do
    {
        if (!oem_version)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_SW_OEM_VERSION_TYPE,
                                                                 &value, &length );
        if (ret != E_SUCCESS)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length: %d", length);
        if (length != 4)
        {
            QCRIL_LOG_DEBUG("%s", "length should be 4, exit");
            ret = E_FAILURE;
        }

        if (value)
        {
            // oem_version 4 bytes
            *oem_version = *((int32_t*)value);
        }
    } while(0);

    if (value) {
        free(value);
    }

    return ret;
}

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    do
    {
        if (!capability)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_SW_CAPABILITY_TYPE,
                                                                 &value, &length );
        if (ret != E_SUCCESS)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length: %ud", length);
        if (length != 4)
        {
            QCRIL_LOG_DEBUG("%s", "length should be 4, exit");
            ret = E_FAILURE;
            break;
        }

        if (value)
        {
            // capability 4 bytes
            *capability = *((uint32_t*)value);
        }
    } while(0);

    if (value) {
        free(value);
    }

    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_sw_iin_list

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    int iin_size;
    unsigned int index = 0;
    int i;
    do
    {
        if (!iin_flag || ! iin_list_count || !iin_list)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_SW_IIN_LIST_TYPE,
                                                                 &value, &length );
        if ((ret != E_SUCCESS) || !value)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length %d", length);
        // iin_flag 1 byte
        *iin_flag = *((int8_t*)(&value[index]));
        index++;
        if (index >= length)
        {
            ret = E_FAILURE;
            break;
        }

        // iin_size 1 byte
        iin_size = *((int8_t*)(&value[index]));
        QCRIL_LOG_DEBUG("iin_size %d", iin_size);
        *iin_list_count = iin_size;

        if(iin_size <= 0)
        {
            QCRIL_LOG_DEBUG("Empty IIN list(or) invalid size");
            if(*iin_flag == 1) {
                QCRIL_LOG_DEBUG("wild_entry: add to db");
                ret = E_SUCCESS;
                break;
            } else {
                ret = E_FAILURE;
                break;
            }
        }

        *iin_list = malloc(iin_size*sizeof(int32_t));
        if(*iin_list)
        {
            index++;
            if (index >= length)
            {
                ret = E_FAILURE;
                break;
            }

            QCRIL_LOG_DEBUG("iin_size %d", iin_size);
            QCRIL_LOG_DEBUG("index %d", index);

            // iin_list[i] 4 byte each
            for (i = 0; (i < iin_size) && (index < length) ; ++i)
            {
                QCRIL_LOG_DEBUG(" %d", i);
                (*iin_list)[i] = *((int32_t*)&value[index]);
                index += 4;
            }
        }
        else
        {
            QCRIL_LOG_DEBUG("Failed to allocate mem for iin_list");
            ret = E_FAILURE;
        }

    } while(0);

    if (value)
    {
        free(value);
    }

    return ret;
}

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    unsigned int index = 0;
    do
    {
        if (!long_iin_list_len || !long_iin_list)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_SW_LONG_IIN_LIST_TYPE,
                                                                 &value, &length );
        if ((ret != E_SUCCESS) || !value)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length %d", length);
        if (index >= length)
        {
            ret = E_FAILURE;
            break;
        }

        *long_iin_list_len = length;
        *long_iin_list = calloc(1, (length + 1 ) * sizeof(char));
        if (!(*long_iin_list)) {
            ret = E_FAILURE;
            break;
        }
        memcpy(*long_iin_list, value, length);
    } while(0);

    if (value)
    {
        free(value);
    }

    return ret;
}

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
    size_t* mcc_mnc_list_len, // in bytes
    uint16_t** mcc_mnc_list
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    unsigned int mcc_mnc_size;
    size_t index = 0;
    unsigned int i;
    do
    {
        if (!mcc_mnc_flag || ! mcc_mnc_list_count || !mcc_mnc_list)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_SW_MCC_MNC_LIST_TYPE,
                                                                 &value, &length );
        if ((ret != E_SUCCESS) || !value)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length %u", length);
        // mcc_mnc_flag 1 byte
        *mcc_mnc_flag = *((int8_t*)(&value[index]));
        index++;
        if (index >= length)
        {
            ret = E_FAILURE;
            break;
        }

        // mcc_mnc_size 1 byte
        mcc_mnc_size = *((int8_t*)(&value[index]));
        if (mcc_mnc_size <= 0)
        {
            QCRIL_LOG_DEBUG("Non-positive mcc_mnc_count %d", mcc_mnc_size)
            ret = E_FAILURE;
            break;
        }
        index++;
        if (index >= length)
        {
            ret = E_FAILURE;
            break;
        }

        *mcc_mnc_list_count = mcc_mnc_size;
        *mcc_mnc_list = malloc(2*mcc_mnc_size*sizeof(uint16_t));
        *mcc_mnc_list_len = length - 2; // total length - flag(1 byte) - size(1 byte)
        QCRIL_LOG_DEBUG("mcc_mnc_list_count %u", *mcc_mnc_list_count);
        QCRIL_LOG_DEBUG("mcc_mnc_list_len %u", *mcc_mnc_list_len);
        QCRIL_LOG_DEBUG("index %u", index);

        // mcc_mnc_list[0]: mcc[0], mcc_mnc_list[1]: mnc[0], mcc_mnc_list[2]: mcc[1], ...
        if (*mcc_mnc_list)
        {
            for (i = 0; (i < mcc_mnc_size) && (index + 2 < length) ; ++i)
            {
                (*mcc_mnc_list)[i*2] = *((uint16_t*)&value[index]);
                index += 2;
                (*mcc_mnc_list)[i*2+1] = *((uint16_t*)&value[index]);
                index += 2;
            }
        }
        else {
            QCRIL_LOG_ERROR("Empty mcc_mnc_list");
            ret = E_FAILURE;
            break;
        }
    } while(0);

    if (value)
    {
        free(value);
    }

    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_sw_carrier_name

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    do
    {
        if (!carrier_name)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_SW_CARRIER_NAME_TYPE,
                                                                 &value, &length );
        if (ret != E_SUCCESS)
        {
            break;
        }
        if (value)
        {
            *carrier_name = malloc(sizeof(char)*length+1);
            // TODO: add free() function when this function is called
            if(*carrier_name)
            {
                memset(*carrier_name, 0, length+1);
                memcpy(*carrier_name, value, length);
            }
            else
            {
                QCRIL_LOG_DEBUG("Failed to allocate mem for carrier_name");
                ret = E_FAILURE;
            }
            free(value);
        }
    } while(0);


    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_hw_qc_version

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
    int            *qc_version
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;

    do
    {
        if (!qc_version)
        {
            break;
        }
        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_HW_QC_VERSION_TYPE,
                                                                 &value, &length );
        if (ret != E_SUCCESS)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length: %d", length);
        if (length != 4)
        {
            QCRIL_LOG_DEBUG("%s", "length should be 4, exit");
            ret = E_FAILURE;
        }

        if (value) {
            // qc_version 4 bytes
            *qc_version = *((int32_t*)value);
        }
    } while(0);

    if (value)
    {
        free(value);
    }

    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_hw_oem_version

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    do
    {
        if (!oem_version)
        {
            break;
        }

        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_HW_OEM_VERSION_TYPE,
                                                                 &value, &length );
        if (ret != E_SUCCESS)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length: %d", length);
        if (length != 4)
        {
            QCRIL_LOG_DEBUG("%s", "length should be 4, exit");
            ret = E_FAILURE;
        }

        if (value) {
            // oem_version 4 bytes
            *oem_version = *((int32_t*)value);
        }
    } while(0);

    if (value)
    {
        free(value);
    }

    return ret;
}

/*===========================================================================

  FUNCTION qcril_mbn_meta_retrieve_hw_name

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
)
{
    IxErrnoType ret = E_FAILURE;
    unsigned char *value = NULL;
    unsigned int length;
    do
    {
        if (!hwid_name)
        {
            break;
        }

        ret = qcril_mbn_meta_retrieve_type_value_from_meta_data( meta_data, meta_data_len,
                                                                 QCRIL_MBN_HW_HWID_NAME_TYPE,
                                                                 &value, &length );
        if (ret != E_SUCCESS)
        {
            break;
        }

        QCRIL_LOG_DEBUG("length: %u", length);

        if (value)
        {
            *hwid_name = malloc((sizeof(char)*(length+1)));
            if(*hwid_name)
            {
                memset(*hwid_name, 0, length+1);
                memcpy(*hwid_name, value, length);
            }
            else
            {
                QCRIL_LOG_DEBUG("Failed to allocate mem for hwid_name");
                ret = E_FAILURE;
            }
            free(value);
        }
    } while(0);


    return ret;
}
