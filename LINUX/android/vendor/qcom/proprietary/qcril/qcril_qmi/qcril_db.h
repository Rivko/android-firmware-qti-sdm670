
#ifndef QCRIL_DB_H
#define QCRIL_DB_H

#include <stdio.h>
#include <sqlite3.h>

#include "qcrili.h"
#include "qcril_log.h"
#include "qcril_qmi_nas.h"

#define QCRIL_MAX_EMERGENCY_NUMBERS_LEN (200)
#define QCRIL_MAX_IMS_ADDRESS_LEN       (81)
#define QCRIL_DB_MAX_OPERATOR_TYPE_LEN  (6)
#define QCRIL_DB_MAX_FILE_NAME_LEN      (500)
#define QCRIL_DB_MAX_STMT_LEN           (400)

#define QCRIL_MBN_SW_MBN_LOADED  "persist.vendor.radio.sw_mbn_loaded"
#define QCRIL_MBN_HW_MBN_LOADED  "persist.vendor.radio.hw_mbn_loaded"
#define QCRIL_DB_MBN_VER_INFO    "persist.vendor.radio.ver_info"
#define QCRIL_DB_MCFG_VER_INFO DATA_PARTITION_ROOT "radio/ver_info.txt"
#define QCRIL_DB_UPGRADE_COMPLETE "ril.vendor.radio.db_upgrade"

typedef enum
{
    QCRIL_DB_RECOVERY_BACKUP = 0,
    QCRIL_DB_RECOVERY_RESTORE,
    QCRIL_DB_RECOVERY_CHECK_AND_RESTORE,
    QCRIL_DB_RECOVERY_MAX
} qcril_db_recovery_type;

/*===========================================================================

  FUNCTION  qcril_db_init

===========================================================================*/
/*!
    @brief
    Initializes qcril db.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_init(void);

/*===========================================================================

  FUNCTION  qcril_db_is_mcc_part_of_emergency_numbers_table

===========================================================================*/
/*!
    @brief
    Checks for mcc existence in db and retireves emergency number from db.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_mcc_part_of_emergency_numbers_table
(
    qmi_ril_custom_emergency_numbers_source_type source,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    char *emergency_num
);

/*===========================================================================

  FUNCTION  qcril_db_is_mcc_part_of_emergency_numbers_table_with_service_state

===========================================================================*/
/*!
    @brief
    Checks for mcc & service state existence in db and retrives
    emergency number from db.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_mcc_part_of_emergency_numbers_table_with_service_state
(
    qmi_ril_custom_emergency_numbers_source_type source,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    char *service_state,
    char *emergency_num
);

/*===========================================================================

  FUNCTION  qcril_db_is_number_mcc_part_of_emergency_numbers_table

===========================================================================*/
/*!
    @brief
    Checks for mcc and number existence in db

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_number_mcc_part_of_emergency_numbers_table
(
    char *emergency_num,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    qmi_ril_custom_emergency_numbers_source_type source
);


/*===========================================================================

  FUNCTION  qcril_db_is_number_mcc_part_of_emergency_numbers_table_with_service_state

===========================================================================*/
/*!
    @brief
    Checks for mcc and number existence based on service state

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_number_mcc_part_of_emergency_numbers_table_with_service_state
(
    char *emergency_num,
    char *mcc,
    qmi_ril_custom_emergency_numbers_source_type source,
    char *service
);

/*===========================================================================

  FUNCTION  qcril_db_is_ims_address_for_mcc_emergency_number_part_of_emergency_numbers_table

===========================================================================*/
/*!
    @brief
    Checks for mcc and emergency number existence in db and retrieves
    corresponding ims_address (if present) from db.

    @return
    TRUE if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_ims_address_for_mcc_emergency_number_part_of_emergency_numbers_table
(
    qmi_ril_custom_emergency_numbers_source_type source,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    char *emergency_num,
    char *ims_address
);


/*===========================================================================

  FUNCTION  qcril_db_query_escv_type

===========================================================================*/
/*!
    @brief
    Query ESCV type nased upon iin or (mcc, mnc)

    @return
    escv type
*/
/*=========================================================================*/
int qcril_db_query_escv_type
(
    char *emergency_num,
    char *iin,
    char *mcc,
    char *mnc,
    char *roam
);

/*===========================================================================

  FUNCTION  qcril_db_query_properties_table

===========================================================================*/
/*!
    @brief
    Query property table

    @return
    None
*/
/*=========================================================================*/
void qcril_db_query_properties_table
(
    char *property_name,
    char *value
);


/*===========================================================================

  FUNCTION  qcril_db_insert_properties_table

===========================================================================*/
/*!
    @brief
    Insert property table

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/

int qcril_db_insert_properties_table
(
    char *property_name,
    char *value
);

/*===========================================================================

  FUNCTION  qcril_db_query_operator_type

===========================================================================*/
/*!
    @brief
    Query operator type based upon (mcc, mnc)

    @output
    string 3gpp or 3gpp2
*/
/*=========================================================================*/
void qcril_db_query_operator_type
(
    char *mcc,
    char *mnc,
    char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN]
);

/*===========================================================================

  FUNCTION  qcril_db_update_operator_type

===========================================================================*/
/*!
    @brief
    Update operator type, mcc and mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_operator_type
(
    char *mcc,
    char *mnc,
    char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN]
);

/*===========================================================================

  FUNCTION  qcril_db_insert_operator_type

===========================================================================*/
/*!
    @brief
    Insert operator type, mcc and mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_operator_type
(
    char *mcc,
    char *mnc,
    char operator_type[QCRIL_DB_MAX_OPERATOR_TYPE_LEN]
);

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file

===========================================================================*/
/*!
    @brief
    Query mbn file based upon query string

    @output
    None
*/
/*=========================================================================*/
void qcril_db_query_mbn_file
(
    char            *query_string,
    sqlite3_callback  call_back,
    unsigned char   *output
);

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file_name_from_hw_type

===========================================================================*/
/*!
    @brief
    Query mbn file name from hw type

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_mbn_file_name_from_hw_type
(
    char   **mbn_file_name,
    char   *device_type
);

/*===========================================================================

  FUNCTION  qcril_db_update_mbn_file

===========================================================================*/
/*!
    @brief
    Update mbn file in db using update statement

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_mbn_file
(
   const char* update_stmt
);

/*===========================================================================

  FUNCTION  qcril_db_update_mbn_file_with_int

===========================================================================*/
/*!
    @brief
    Update mbn file in db by filling values with int

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_mbn_file_with_int
(
    char *update_stmt_fmt,
    char *table_name,
    char *file_name,
    char *type,
    int value
);

/*===========================================================================

  FUNCTION  qcril_db_update_mbn_file_with_str

===========================================================================*/
/*!
    @brief
    Update mbn file in db by filling values with string

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_mbn_file_with_str
(
    char *update_stmt_fmt,
    char *table_name,
    char *file_name,
    char *type,
    char *value
);

/*===========================================================================

  FUNCTION  qcril_db_insert_mbn_file

===========================================================================*/
/*!
    @brief
    Insert mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_mbn_file
(
    char *insert_stmt_fmt,
    char *table_name,
    char *file
);

/*===========================================================================

  FUNCTION  qcril_db_reset_cleanup

===========================================================================*/
/*!
    @brief
    Reset all global vars and release database

    @return
    0 on success
*/
/*=========================================================================*/
int qcril_db_reset_cleanup();

/*===========================================================================

  FUNCTION  qcril_db_update_hw_mbn_file_with_int

===========================================================================*/
/*!
    @brief
    Update hardware mbn file with integer value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_hw_mbn_file_with_int
(
    char *file,
    char *type,
    int   value
);

/*===========================================================================

  FUNCTION  qcril_db_update_hw_mbn_file_with_str

===========================================================================*/
/*!
    @brief
    Update hardware mbn file with string value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_hw_mbn_file_with_str
(
    char *file,
    char *type,
    char *value
);

/*===========================================================================

  FUNCTION  qcril_db_insert_hw_mbn_file

===========================================================================*/
/*!
    @brief
    Insert hardware mbn file to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_hw_mbn_file
(
    char *file
);

/*===========================================================================

  FUNCTION  qcril_db_update_sw_mbn_file_with_int

===========================================================================*/
/*!
    @brief
    Update software mbn file with integer value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_sw_mbn_file_with_int
(
    char *file,
    char *type,
    int   value
);


/*===========================================================================

  FUNCTION  qcril_db_update_sw_mbn_file_with_str

===========================================================================*/
/*!
    @brief
    Update software mbn file with string value

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_sw_mbn_file_with_str
(
    char *file,
    char *type,
    char *value
);


/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_file

===========================================================================*/
/*!
    @brief
    Insert software mbn file to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_sw_mbn_file
(
    char *file
);

/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_iin

===========================================================================*/
/*!
    @brief
    Insert software mbn iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_sw_mbn_iin
(
    char *file,
    char *iin,
    char *volte_info,
    char *mkt_info,
    char *lab_info
);

/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_long_iin

===========================================================================*/
/*!
    @brief
    Insert software mbn long iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_sw_mbn_long_iin
(
    char *file,
    char *long_iin,
    char *volte_info,
    char *mkt_info,
    char *lab_info
);

/*===========================================================================

  FUNCTION  qcril_db_insert_sw_mbn_mcc_mnc

===========================================================================*/
/*!
    @brief
    Insert software mbn mcc_mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_sw_mbn_mcc_mnc
(
    char *file,
    char *mcc,
    char *mnc,
    char *volte_info,
    char *mkt_info,
    char *lab_info
);

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file_name_from_iin

===========================================================================*/
/*!
    @brief
    Query mbn file name from iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_mbn_file_name_from_iin
(
    char   **mbn_file_name,
    char    *iin,
    char    *volte_info,
    char    *mkt_info,
    char    *lab_info
);

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_file_name_from_long_iin

===========================================================================*/
/*!
    @brief
    Query mbn file name from 11-bit iin

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_mbn_file_name_from_long_iin
(
    char   **mbn_file_name,
    char    *long_iin,
    char    *volte_info,
    char    *mkt_info,
    char    *lab_info
);

/*===========================================================================

  FUNCTION  qcril_db_query_sw_mbn_file_with_mcc_mnc

===========================================================================*/
/*!
    @brief
    Query mbn file with mcc mnc

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_sw_mbn_file_with_mcc_mnc
(
    char       **mbn_file_name,
    char        *mcc,
    char        *mnc,
    char        *volte_info,
    char        *mkt_info,
    char        *lab_info
);

/*===========================================================================

  FUNCTION  qcril_db_query_sw_mbn_int_from_file

===========================================================================*/
/*!
    @brief
    Query integer value from software mbn file

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_sw_mbn_int_from_file
(
    char        *mbn_file_name,
    char        *type,
    uint32_t    *value
);

/*===========================================================================

  FUNCTION  qcril_db_query_sw_mbn_file_from_version_type

===========================================================================*/
/*!
    @brief
    Query SW MBN file from version (family version and carrier version)

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_sw_mbn_file_from_version_type
(
    uint32_t    family,
    uint32_t    carrier,
    uint32_t    minor,
    char        **mbn_file_name
);

/*===========================================================================

  FUNCTION  qcril_db_query_sig_config

===========================================================================*/
/*!
    @brief
    Query sig config

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_sig_config
(
    char        *sig_config_type,
    uint16_t    *delta
);

/*===========================================================================

  FUNCTION  qcril_db_insert_new_iccid_into_prov_table

===========================================================================*/
/*!
    @brief
    Insert new iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_new_iccid_into_prov_table
(
    char *iccid,
    int  user_pref
);

/*===========================================================================

  FUNCTION  qcril_db_update_user_pref_prov_table

===========================================================================*/
/*!
    @brief
    Insert new iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_user_pref_prov_table
(
    char *iccid,
    int  user_pref
);

/*===========================================================================

  FUNCTION  qcril_db_query_user_pref_from_prov_table

===========================================================================*/
/*!
    @brief
    Query user preference

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_user_pref_from_prov_table
(
    char        *iccid,
    int         *user_pref
);

/*===========================================================================

  FUNCTION  qcril_db_insert_new_mcc_mnc_into_mbn_imsi_exception_table

===========================================================================*/
/*!
    @brief
    Insert new mcc,mnc entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_new_mcc_mnc_into_mbn_imsi_exception_table
(
    char *mcc,
    char *mnc,
    int  carrier_pref
);

/*===========================================================================

  FUNCTION  qcril_db_update_carrier_pref_mbn_imsi_exception_table

===========================================================================*/
/*!
    @brief
    Update carrier pref in existing mcc,mnc entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_carrier_pref_mbn_imsi_exception_table
(
    char *mcc,
    char *mnc,
    int  carrier_pref
);

/*===========================================================================

  FUNCTION  qcril_db_query_carrier_pref_from_mbn_imsi_exception_table

===========================================================================*/
/*!
    @brief
    Query carrier preference

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_carrier_pref_from_mbn_imsi_exception_table
(
    char        *mcc,
    char        *mnc,
    int         *carrier_pref
);

/*===========================================================================

  FUNCTION  qcril_db_insert_new_mcc_mnc_into_mbn_iccid_exception_table

===========================================================================*/
/*!
    @brief
    Insert new iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_new_mcc_mnc_into_mbn_iccid_exception_table
(
    char *iccid,
    int  carrier_pref
);

/*===========================================================================

  FUNCTION  qcril_db_update_carrier_pref_mbn_iccid_exception_table

===========================================================================*/
/*!
    @brief
    Update carrier pref in existing iccid entry

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_update_carrier_pref_mbn_iccid_exception_table
(
    char *iccid,
    int  carrier_pref
);

/*===========================================================================

  FUNCTION  qcril_db_query_carrier_pref_from_mbn_iccid_exception_table

===========================================================================*/
/*!
    @brief
    Query carrier preference

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_query_carrier_pref_from_mbn_iccid_exception_table
(
    char        *iccid,
    int         *carrier_pref
);

/*===========================================================================

  FUNCTION  qcril_db_table_query_callback_integer

===========================================================================*/
/*!
    @brief
    Generic function for table query with integer output

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_table_query_callback_integer
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
);

/*===========================================================================

  FUNCTION qcril_db_check_if_table_exists

===========================================================================*/
/*!
    @brief
    Check if qcril.db has table or not.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int qcril_db_check_if_table_exists(char *tname);

/*===========================================================================

  FUNCTION qcril_db_update_prov_info_from_tel_db

===========================================================================*/
/*!
    @brief
    database busy handler.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int qcril_db_busy_handler(void* ptr,int count);

/*===========================================================================

  FUNCTION  qcril_db_is_sw_mbn_reevaluation_needed

===========================================================================*/
/*!
    @brief
    Check and update if software mcfg tables needs to be re-evaluated.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_db_is_sw_mbn_reevaluation_needed(void);

/*===========================================================================

  FUNCTION  qcril_db_is_hw_mbn_reevaluation_needed

===========================================================================*/
/*!
    @brief
    Check and update if hardware mcfg tables needs to be re-evaluated.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_db_is_hw_mbn_reevaluation_needed(void);

/*===========================================================================

  FUNCTION  qcril_db_update_cur_modem_version

===========================================================================*/
/*!
    @brief
    Update modem version as current version.

    @return
    none
*/
/*=========================================================================*/
void qcril_db_update_cur_modem_version();

/*===========================================================================

  FUNCTION  qcril_db_sqlite3_exec

===========================================================================*/
/*!
    @brief
    Execute SQL command and retry DB operation after restoring
    the DB from backup, if operation failed.

    @return
    SQLITE_OK on success
*/
/*=========================================================================*/

int qcril_db_sqlite3_exec
(
    const char *sql_stmt,                             /* SQL to be evaluated */
    int        (*callback)(void*,int,char**,char**),  /* Callback function */
    void       *arg                                   /* 1st argument to callback */
);

/*===========================================================================

  FUNCTION  qcril_db_recovery

===========================================================================*/
/*!
    @brief
    Either a backup or restore will be done based on input param

    @return
    SQLITE_OK on success
*/
/*=========================================================================*/
int qcril_db_recovery(qcril_db_recovery_type isRecovery);

int qcril_db_begin(void);
int qcril_db_commit(void);
int qcril_db_abort(void);
#endif /* QCRIL_DB_H */
