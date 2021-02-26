/******************************************************************************
#  Copyright (c) 2013, 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************

  @file    qcril_db.c
  @brief   Provides interface to communicate with qcril db tables

  DESCRIPTION
    Initialize sqlite db
    Create qcril db tables
    Provides interface to query db tables

******************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#include "modules/nas/qcril_db.h"
#include "qcril_file_utils.h"
#include "qcril_other.h"
#include <cutils/properties.h>
#include <fcntl.h>
#define __USE_GNU /* Required for non-standard scandirat function */
#include <dirent.h>
#include <sys/mman.h>

#ifdef QMI_RIL_UTF
#define QCRIL_DATABASE_NAME "./qcril.db"
#define QCRIL_BACKUP_DATABASE_NAME "./qcril_backup.db"
#define UPGRADE_SCRIPTS_PATH   "./upgrade"
#define QCRIL_PREBUILT_DB_NAME "./qcril_prebuilt.db"
#else
#define QCRIL_DATABASE_NAME DATA_PARTITION_ROOT "radio/qcril.db"
#define QCRIL_BACKUP_DATABASE_NAME DATA_PARTITION_ROOT "radio/qcril_backup.db"
#define QCRIL_PREBUILT_DB_NAME DATA_PARTITION_ROOT "radio/qcril_prebuilt.db"
#define UPGRADE_SCRIPTS_PATH   "/vendor/radio/qcril_database/upgrade"
#endif

#define MAX_RETRY               (10)
#define QCRIL_DB_SLEEP_TIME     (500000)  // in micro seconds.
#define QCRIL_DATA_DIR_CHECK_INTERVAL     (1) // in seconds
#define QCRIL_DATA_DIR_CHECK_TIME_LIMIT   (12) // in seconds

#define QCRIL_DB_PROPERTY_VALUE_MAX   (200)
#define QCRIL_MBN_HW_STR_MAX_SIZE     (750)

#define QCRIL_DB_MIGRATION_PROPERTY_NAME "dbmigration"

#define TEL_DB_ICCID_COL_NAME            "icc_id"
#define TEL_DB_SUBSTATE_COL_NAME         "sub_state"
#define TEL_DB_SIMINFO_TABLE_NAME        "siminfo"

#define IIN_11_LEN              (12)
#define IIN_7_LEN               (8)
#define IIN_6_LEN               (7)

/* QCRIL DB handle */
sqlite3* qcril_db_handle     = NULL;

/* Telephony DB handle */
sqlite3* qcril_tel_db_handle = NULL;

char    *qcril_db     = "qcril.db";
char    *qcril_tel_db = "telephony.db";

typedef enum
{
    QCRIL_DB_TABLE_FIRST = 0,
    QCRIL_DB_TABLE_OPERATOR_TYPE = QCRIL_DB_TABLE_FIRST,
    QCRIL_DB_SW_MBN_FILE_TYPE,
    QCRIL_DB_SW_MBN_IIN_TYPE,
    QCRIL_DB_SW_MBN_MCC_MNC_TYPE,
    QCRIL_DB_HW_MBN_FILE_TYPE,
    QCRIL_DB_SIG_CONFIG_TYPE,
    QCRIL_DB_MANUAL_PROV_TYPE,
    QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE,
    QCRIL_DB_MBN_ICCID_EXCEPTION_TYPE,
    QCRIL_DB_TABLE_MAX
} qcril_db_table_type;

typedef struct qcril_db_table_info {
    const char *table_name;
    const char *create_stmt;
    uint8_t drop_during_bootup;
} qcril_db_table_info;

typedef struct qcril_db_escv_in_out {
    char *mnc;
    int escv_type;
} qcril_db_escv_in_out;


#define QCRIL_PROPERTIES_TABLE_NAME "qcril_properties_table"

/* Statement to create qcril db tables */
#define QCRIL_CREATE_EMRGENCY_TABLE    \
            "create table if not exists %s" \
            "(MCC TEXT, NUMBER TEXT, IMS_ADDRESS TEXT, SERVICE TEXT, PRIMARY KEY(MCC,NUMBER))"

/* Statement to create qcril db escv iin table */
#define QCRIL_DB_CREATE_ESCV_IIN_TABLE  \
            "create table if not exists %s" \
            "(IIN TEXT, NUMBER TEXT, ESCV INTEGER, ROAM TEXT, PRIMARY KEY(IIN,NUMBER,ROAM))"

/* Statement to create qcril db escv nw table */
#define QCRIL_DB_CREATE_ESCV_NW_TABLE  \
            "create table if not exists %s"\
            "(MCC TEXT, MNC TEXT, NUMBER TEXT, ESCV INTEGER, PRIMARY KEY(MCC,NUMBER, ESCV))"

/* Statement to create qcril db properties table */
#define QCRIL_DB_CREATE_PROPERTIES_TABLE  \
            "create table if not exists %s"\
            "(PROPERTY TEXT,VALUE TEXT, PRIMARY KEY(PROPERTY))"

/* Statement to create qcril db operator type table */
#define QCRIL_DB_CREATE_OPERATOR_TYPE_TABLE  \
            "create table if not exists %s"\
            "(MCC TEXT, MNC TEXT, TYPE TEXT, PRIMARY KEY(MCC,MNC))"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_SW_MBN_FILES_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT PRIMARY KEY, MCFG_VERSION_FAMILY INTEGER, "\
            "MCFG_VERSION_OEM INTEGER, MCFG_VERSION_CARRIER INTEGER, "\
            "MCFG_VERSION_MINOR INTEGER, CARRIER_NAME TEXT, "\
            "QC_VERSION_FAMILY INTEGER, QC_VERSION_OEM INTEGER, "\
            "QC_VERSION_CARRIER INTEGER, QC_VERSION_MINOR INTEGER, "\
            "CONFIG_ID TEXT)"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_SW_MBN_MCC_MNC_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT , MCC TEXT, MNC TEXT, "\
            "VOLTE_INFO TEXT, MKT_INFO TEXT, LAB_INFO TEXT, "\
            "PRIMARY KEY(FILE, MCC, MNC))"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_SW_MBN_IIN_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT , MCFG_IIN TEXT, MCFG_LONG_IIN TEXT, "\
            "VOLTE_INFO TEXT, MKT_INFO TEXT, LAB_INFO TEXT, "\
            "PRIMARY KEY(FILE, MCFG_IIN, MCFG_LONG_IIN))"

/* Statement to create qcril db mbn file table */
#define QCRIL_DB_CREATE_HW_MBN_FILES_TABLE  \
            "create table if not exists %s "\
            "(FILE TEXT PRIMARY KEY, MCFG_VERSION_FAMILY INTEGER, "\
            "MCFG_VERSION_OEM INTEGER, MCFG_VERSION_CARRIER INTEGER, "\
            "MCFG_VERSION_MINOR INTEGER, HW_NAME TEXT, "\
            "QC_VERSION_FAMILY INTEGER, QC_VERSION_OEM INTEGER, "\
            "QC_VERSION_CARRIER INTEGER, QC_VERSION_MINOR INTEGER, "\
            "SHORT_NAME TEXT, CONFIG_ID TEXT)"


/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_SIG_CONFIG_TABLE  \
            "create table if not exists %s "\
            "(SIG_CONFIG_TYPE TEXT, DELTA TEXT, PRIMARY KEY(SIG_CONFIG_TYPE))"

/* Statement to create qcril db tables */
#define QCRIL_DB_CREATE_EMERGENCY_MCC_MNC_TABLE    \
            "create table if not exists %s" \
            "(MCC TEXT, MNC TEXT, NUMBER TEXT, IMS_ADDRESS TEXT, SERVICE TEXT, PRIMARY KEY(MCC,NUMBER,MNC))"


/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_PROV_TABLE  \
            "create table if not exists %s "\
            "(ICCID TEXT, USER_PREF INTEGER, PRIMARY KEY(ICCID))"

/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_MBN_IMSI_EXCEPTION_TABLE  \
            "create table if not exists %s "\
            "(MCC TEXT, MNC TEXT, CARRIER_PREF INTEGER, PRIMARY KEY(MCC,MNC))"

/* Statement to create qcril db sig config table */
#define QCRIL_DB_CREATE_MBN_ICCID_EXCEPTION_TABLE  \
            "create table if not exists %s "\
            "(ICCID TEXT, CARRIER_PREF INTEGER, PRIMARY KEY(ICCID))"


/* Table containing qcril db emergency table names */
static qcril_db_table_info qcril_db_emergency_number_tables[QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX] =
{
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC]      =
                 {  "qcril_emergency_source_mcc_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC]    =
                 { "qcril_emergency_source_voice_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_HARD_MCC] =
                 {"qcril_emergency_source_hard_mcc_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_NW]       =
                 { "qcril_emergency_source_nw_table", QCRIL_CREATE_EMRGENCY_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_IIN] =
                 { "qcril_emergency_source_escv_iin_table", QCRIL_DB_CREATE_ESCV_IIN_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_NW]  =
                 { "qcril_emergency_source_escv_nw_table", QCRIL_DB_CREATE_ESCV_NW_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC]  =
                 { "qcril_emergency_source_mcc_mnc_table", QCRIL_DB_CREATE_EMERGENCY_MCC_MNC_TABLE, 0 },
    [QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC]  =
                 { "qcril_emergency_source_voice_mcc_mnc_table", QCRIL_DB_CREATE_EMERGENCY_MCC_MNC_TABLE, 0 },
};

/* Table containing qcril db table names */
static qcril_db_table_info qcril_db_tables[QCRIL_DB_TABLE_MAX] =
{
    [QCRIL_DB_TABLE_OPERATOR_TYPE]  =
                 { "qcril_operator_type_table", QCRIL_DB_CREATE_OPERATOR_TYPE_TABLE, 0},
    [QCRIL_DB_SW_MBN_FILE_TYPE]  =
                 { "qcril_sw_mbn_file_type_table", QCRIL_DB_CREATE_SW_MBN_FILES_TABLE, 1},
    [QCRIL_DB_SW_MBN_IIN_TYPE]  =
                 { "qcril_sw_mbn_iin_table", QCRIL_DB_CREATE_SW_MBN_IIN_TABLE, 1},
    [QCRIL_DB_SW_MBN_MCC_MNC_TYPE]  =
                 { "qcril_sw_mbn_mcc_mnc_table", QCRIL_DB_CREATE_SW_MBN_MCC_MNC_TABLE, 1},
    [QCRIL_DB_HW_MBN_FILE_TYPE]  =
                 { "qcril_hw_mbn_file_type_table", QCRIL_DB_CREATE_HW_MBN_FILES_TABLE, 1},
    [QCRIL_DB_SIG_CONFIG_TYPE]  =
                 { "qcril_sig_config_table", QCRIL_DB_CREATE_SIG_CONFIG_TABLE, 0},
    [QCRIL_DB_MANUAL_PROV_TYPE]  =
                 { "qcril_manual_prov_table", QCRIL_DB_CREATE_PROV_TABLE, 0},
    [QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE]  =
                 { "qcril_mbn_imsi_exception_table", QCRIL_DB_CREATE_MBN_IMSI_EXCEPTION_TABLE, 0},
    [QCRIL_DB_MBN_ICCID_EXCEPTION_TYPE]  =
                 { "qcril_mbn_iccid_exception_table", QCRIL_DB_CREATE_MBN_ICCID_EXCEPTION_TABLE, 0},
};

#define RESERVED_TO_STORE_LENGTH 4

/* Query statement to query emergency number */
static char* qcril_db_query_number_and_mcc_stmt =
                      "select NUMBER from %s where MCC='%s' and NUMBER='%s'";

/* Query statement to query emergency number*/
static char* qcril_db_query_number_and_mcc_and_service_stmt =
                      "select NUMBER from %s where MCC='%s' and NUMBER='%s' and SERVICE='%s'";

/* Query statement to query emergency number and mcc */
static char* qcril_db_query_emergency_number_stmt =
                      "select NUMBER from %s where MCC='%s'";

/* Query statement to query emergency number */
static char* qcril_db_query_number_from_mcc_and_service_stmt =
                      "select NUMBER from %s where MCC='%s' and SERVICE='%s'";

/* Query statement to query ims_address from mcc and emergency number */
static char* qcril_db_query_ims_address_from_mcc_number_stmt =
                      "select IMS_ADDRESS from %s where MCC='%s' and NUMBER='%s'";

/* Emergency numbers retrieved */
static char qcril_db_emergency_numbers[QCRIL_MAX_EMERGENCY_NUMBERS_LEN] = {0};

/* Query statement to query emergency number escv type using iin */
static char* qcril_db_query_escv_iin_stmt =
                      "select ESCV from %s where IIN='%s' and NUMBER='%s' and ROAM='%s'";

/* Query statement to query emergency number escv type using mcc and mnc */
static char* qcril_db_query_escv_nw_stmt   =
                      "select ESCV, MNC from %s where MCC='%s' and NUMBER='%s'";

/* Query statement to query emergency number escv type using iin */
static char* qcril_db_query_properties_stmt =
                      "select VALUE from %s where PROPERTY='%s'";

/* Query statement to insert property_name and value */
static char* qcril_db_insert_properties_stmt =
                      "insert into %s values('%s', '%s')";

/* Query statement to query operator type using mcc and mnc */
static char* qcril_db_query_operator_type_stmt =
                      "select TYPE from %s where MCC='%s' and MNC='%s'";

/* Insert statement to insert operator type, mcc and mnc */
static char* qcril_db_insert_operator_type_stmt =
                      "insert into %s values('%s', '%s', '%s')";

/* Update statement to update operator type, mcc and mnc */
static char* qcril_db_update_operator_type_stmt =
                      "update %s set TYPE='%s' where MCC='%s' and MNC='%s'";

/* Insert statement to insert mbn file row*/
static char* qcril_db_insert_sw_mbn_file_stmt =
                      "insert into %s(FILE) values('%s')";

/* update statement to update string column to mbn file row*/
static char* qcril_db_update_sw_mbn_file_str_stmt =
                      "update %s set %s='%s' where FILE='%s'";

/* update statement to update int column to mbn file row*/
static char* qcril_db_update_sw_mbn_file_int_stmt =
                      "update %s set %s='%d' where FILE='%s'";

/* Query statement to query from mbn file table*/
#if 0
static char* qcril_db_query_sw_mbn_file_stmt =
                      "select %s from %s where %s='%s'";
#endif

/* Insert statement to insert mbn file row*/
static char* qcril_db_insert_hw_mbn_file_stmt =
                      "insert into %s(FILE) values('%s')";

/* Insert statement to insert mbn file row*/
static char* qcril_db_update_hw_mbn_file_str_stmt =
                      "update %s set %s='%s' where FILE='%s'";

/* Insert statement to insert mbn file row*/
static char* qcril_db_update_hw_mbn_file_int_stmt =
                      "update %s set %s='%d' where FILE='%s'";

/* Query statement to query from mbn file table based upon two arguments */
// static char* qcril_db_query_mbn_file_stmt_two_args =
// 		      "select %s from %s where %s='%s' and %s='%s'";

/* Query statement to query from mbn file table based upon three int arguments */
static char* qcril_db_query_mbn_file_stmt_three_int_args =
                      "select %s from %s where %s='%d' and %s='%d' and %s='%d'";

/* Query statement to query from mbn file table based upon three arguments */
static char* qcril_db_query_mbn_file_stmt_three_args =
                      "select %s from %s where %s='%s' and %s='%s' and %s='%s'";

/* Query statement to query from mbn file table based upon four arguments */
static char* qcril_db_query_mbn_file_stmt_four_args =
                      "select %s from %s where %s='%s' and %s='%s' and %s='%s' and %s='%s'";

/* Query statement to query from mbn file table based upon five arguments */
static char* qcril_db_query_mbn_file_stmt_five_args =
                      "select %s from %s where %s='%s' and %s='%s' and %s='%s' and %s='%s' and %s='%s'";

/* Query statement to query from mbn file table*/
static char* qcril_db_query_hw_mbn_file_stmt =
                      "select %s from %s where %s='%s'";

/* Insert statement to insert mbn file row*/
static char* qcril_db_insert_sw_mbn_iin_stmt =
                      "insert into %s(FILE, MCFG_IIN, VOLTE_INFO, MKT_INFO, LAB_INFO) "\
                      "values('%s', '%s', '%s', '%s', '%s')";

/* Insert statement to insert mbn file row*/
static char* qcril_db_insert_sw_mbn_long_iin_stmt =
                      "insert into %s(FILE, MCFG_LONG_IIN, VOLTE_INFO, MKT_INFO, LAB_INFO) "\
                      "values('%s', '%s', '%s', '%s', '%s')";

/* Insert statement to insert mbn file row*/
static char* qcril_db_insert_sw_mbn_mcc_mnc_stmt =
                      "insert into %s(FILE, MCC, MNC, VOLTE_INFO, MKT_INFO, LAB_INFO) "\
                      "values('%s', '%s', '%s', '%s', '%s', '%s')";

/* Query statement from mbn file iin row*/
static char* qcril_db_query_sw_mbn_str_stmt =
                      "select %s from %s where %s='%s'";

/* Query statement from sig config*/
static char* qcril_db_query_sig_config_stmt =
                      "select DELTA from %s where SIG_CONFIG_TYPE='%s'";

/* Query user preference from manual prov table statement*/
static char* qcril_db_query_user_pref_stmt =
                      "select USER_PREF from %s where ICCID='%s'";

/* Query statement to insert new entry in manual prov table */
static char* qcril_db_insert_new_iccid_stmt =
                      "insert into %s values('%s', '%d')";

/* update user preference statement */
static char* qcril_db_update_user_pref_str_stmt =
                      "update %s set USER_PREF='%d' where ICCID='%s'";

/* Query statement to query emergency number */
#if 0
static char* qcril_db_query_emergency_matching_number_mcc_mnc_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s' and NUMBER='%s'";
#endif

/* Query statement to query emergency present or not */
static char* qcril_db_query_number_and_mcc_mnc_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s' and NUMBER='%s'";

/* Query statement to query emergency number and mcc */
static char* qcril_db_query_emergency_matching_mcc_mnc_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s'";

/* Query statement to query emergency number */
static char* qcril_db_query_number_from_mcc_mnc_and_service_stmt =
                      "select NUMBER from %s where MCC='%s' and MNC='%s' and SERVICE='%s'";

/* Check if table exists statement */
static char* qcril_db_check_table_str_stmt =
                      "select COUNT(*) from sqlite_master where type = '%s' and name = '%s'";

/* Insert statement to insert mbn imsi exception row*/
static char* qcril_db_insert_mbn_imsi_exception_stmt =
                      "insert into %s(MCC, MNC, CARRIER_PREF) "\
                      "values('%s', '%s', '%d')";

/* Update carrier pref for mbn imsi exception */
static char* qcril_db_update_mbn_imsi_exception_stmt =
                      "update %s set CARRIER_PREF='%d' where MCC='%s' and MNC='%s'";

/* Query statement to query carrier pref for mbn imsi exception */
static char* qcril_db_query_carrier_pref_from_mbn_imsi_exception_stmt =
                      "select CARRIER_PREF from %s where MCC='%s' and MNC='%s'";

/* Insert statement to insert mbn iccid exception row*/
static char* qcril_db_insert_mbn_iccid_exception_stmt =
                      "insert into %s(ICCID, CARRIER_PREF) "\
                      "values('%s', '%d')";

/* Update carrier pref for mbn iccid exception */
static char* qcril_db_update_mbn_iccid_exception_stmt =
                      "update %s set CARRIER_PREF='%d' where ICCID='%s'";

/* Query statement to query carrier pref for mbn iccid exception */
static char* qcril_db_query_carrier_pref_from_mbn_iccid_exception_stmt =
                      "select CARRIER_PREF from %s where ICCID='%s'";

/* Statement to perform Integrity check */
static char* qcril_db_check_integrity_stmt = "PRAGMA main.integrity_check";

static const char *qcril_db_begin_txn_stmt = "BEGIN";
static const char *qcril_db_commit_txn_stmt = "COMMIT";
static const char *qcril_db_abort_txn_stmt = "ROLLBACK";

/* Emergency numbers retrieved */
static int qcril_db_emergency_numbers_escv_type = 0;
static int qcril_db_query_result = 0;

/*===========================================================================

  FUNCTION qcril_db_busy_handler

===========================================================================*/
/*!
    @brief
    database busy handler

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int qcril_db_busy_handler(void *ptr, int count)
{
    int ret = 0;
    QCRIL_LOG_INFO("db - %s count - %d", (char*)ptr, count);

    if ( count < MAX_RETRY )
    {
        usleep(QCRIL_DB_SLEEP_TIME);
        ret = 1;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*===========================================================================

  FUNCTION qcril_db_atel_database_busy_handler

===========================================================================*/
/*!
    @brief
    database busy handler

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int qcril_db_atel_database_busy_handler(void *ptr, int count)
{
    int ret = 0;
    QCRIL_LOG_INFO("db - %s count - %d", (char*)ptr, count);

    if ( count < MAX_RETRY )
    {
        usleep(QCRIL_DB_SLEEP_TIME);
        ret = 1;
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

int qcril_db_begin(void)
{
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK !=
            (res = qcril_db_sqlite3_exec(
                     qcril_db_begin_txn_stmt, NULL, NULL)))
    {
        QCRIL_LOG_ERROR("stmt %s", qcril_db_begin_txn_stmt);
        QCRIL_LOG_ERROR("Could not start transaction: %d", res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

int qcril_db_commit(void)
{
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK !=
            (res = qcril_db_sqlite3_exec(
                     qcril_db_commit_txn_stmt, NULL, NULL)))
    {
        QCRIL_LOG_ERROR("stmt %s", qcril_db_commit_txn_stmt);
        QCRIL_LOG_ERROR("Could not commit transaction: %d", res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

int qcril_db_abort(void)
{
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK !=
            (res = qcril_db_sqlite3_exec(
                     qcril_db_abort_txn_stmt, NULL, NULL)))
    {
        QCRIL_LOG_ERROR("stmt %s", qcril_db_abort_txn_stmt);
        QCRIL_LOG_ERROR("Could not abort transaction: %d", res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
int qcril_db_check_if_table_exists(char *tname)
{
    char    stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     ret = 0;
    int     res = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !tname )
        {
            QCRIL_LOG_WARN("Invalid table name - null");
            break;
        }

        snprintf( stmt,
                  sizeof(stmt),
                  qcril_db_check_table_str_stmt,
                  "table",
                  tname );

        /* Query database for if table exist.if ret = 0,
        ** table doesn't exist. if ret != 0 table exist.*/
        if (SQLITE_OK !=
                 (res = qcril_db_sqlite3_exec(
                          stmt, qcril_db_table_query_callback_integer, &ret)))
        {
            QCRIL_LOG_ERROR("stmt %s", stmt);
            QCRIL_LOG_ERROR("Could not check if table exist %d", res);
        }
        else
        {
            res = ( ret == 0 ) ? FALSE : TRUE;
        }

    }while(FALSE);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_retrieve_emergency_num_callback

===========================================================================*/
/*!
    @brief
    Retireves emergency number from db output.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_retrieve_emergency_num_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int     tmp_len = 0;
    char   *ptr;
    uint32_t len;

    QCRIL_NOTUSED(azColName);

    if (data)
    {
        QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
        len     = *((uint32_t*)data);
        ptr     = (char*)data + (RESERVED_TO_STORE_LENGTH + len);
        if (argc == 1 && (len < QCRIL_MAX_EMERGENCY_NUMBERS_LEN) && argv[0])
        {
            if (len != 0)
            {
                tmp_len = snprintf(ptr,
                            (QCRIL_MAX_EMERGENCY_NUMBERS_LEN - len), "%s", ",");
                len = len + tmp_len;
                ptr = ptr + tmp_len;
            }

            tmp_len = snprintf(ptr,
                         (QCRIL_MAX_EMERGENCY_NUMBERS_LEN - len), "%s", argv[0]);

            len = len + tmp_len;
            *((int*)data) = len;
        }
    }

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_check_num_and_mcc_callback

===========================================================================*/
/*!
    @brief
    checks existence of emergency number and mcc in a table.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_check_num_and_mcc_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{

    QCRIL_NOTUSED(azColName);

    if ((argc > 0) && strlen(argv[0]) > 0 && data)
    {
        *(int*)data = 1;
    }

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_check_escv_callback

===========================================================================*/
/*!
    @brief
    checks existence of escv row in a table.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_check_escv_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = -1;
    qcril_db_escv_in_out *result = data;
    int escv = 0;

    QCRIL_NOTUSED(azColName);

    if (result)
    {
        escv = atoi(argv[0]);
        if (argc == 1)
        {
            QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
            if (escv >= 0)
            {
                result->escv_type  = escv;
                ret = 0;
            }
        }
        else if (argc == 2)
        {
            QCRIL_LOG_INFO("argc %d argv[0] %s argv[1] %s",
                                 argc, argv[0] ? argv[0] : "null",
                                 argv[1] ? argv[1] : "null");
            if (escv >= 0)
            {
                if ((result->mnc) && (argv[1]))
                {
                    if (!strcmp(result->mnc, argv[1]))
                    {
                        result->escv_type  = escv;
                    }
                }
                else
                {
                    result->escv_type  = escv;
                }
                ret = 0;
             }
         }
    }

    return ret;
}

/*===========================================================================

  FUNCTION  qcril_db_retrieve_ims_address_from_mcc_emergency_num_callback

===========================================================================*/
/*!
    @brief
    Retireves ims_address from mcc and emergency number from db output.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_retrieve_ims_address_from_mcc_emergency_num_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    char   *ptr;
    int     len;

    QCRIL_NOTUSED(azColName);

    if (data)
    {
        QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
        ptr = (char*)data;
        *ptr = 0;

        if (argc == 1 && argv[0])
        {
            len = strlen(argv[0]);
            if (len > 0 && len < QCRIL_MAX_IMS_ADDRESS_LEN)
            {
                snprintf(ptr,
                         QCRIL_MAX_IMS_ADDRESS_LEN,
                         "%s",
                         argv[0]);
            }
        }
    }

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_query_property_callback

===========================================================================*/
/*!
    @brief
    update property.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_property_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;
    int len = 0;

    QCRIL_NOTUSED(azColName);
    if ( data )
    {
        if ( argc == 1 && argv[0] )
        {
            len = strlen(argv[0]);
            strlcpy(data,argv[0],len+1);
        }
    }

    return ret;
}

/*===========================================================================

  FUNCTION qcril_db_check_prebuilt_and_wait

============================================================================*/
/*!
    @brief
    Mostly, this function will return "qcril.db" string directly at first check.
    Two exception here:
    1) For the very first boot, it will copy prebuilt db file to "qcril.db".
    2) For disk encryption, it will copy prebuilt db file to "qcril.db"

    @return
    E_SUCCESS: qcril.db exists, or succceed to copy from prebuilt db file
    E_FAILURE: no qcril.db file. RIL needs to create it by itself

    TODO: need to add "version check" support, which is very useful for OTA
    update
*/
/*==========================================================================*/
static int qcril_db_check_prebuilt_and_wait
(
    void
)
{
    int retries = 0;
    int ret = E_SUCCESS;

    QCRIL_LOG_FUNC_ENTRY();

    // first check if qcril.db is available for R & W
    do
    {
        if ( !access( QCRIL_DATABASE_NAME, R_OK | W_OK ) )
        {
            QCRIL_LOG_INFO("%s already available", QCRIL_DATABASE_NAME);
            return E_SUCCESS;
        }
        else if(qmi_ril_get_process_instance_id() == QCRIL_DEFAULT_INSTANCE_ID)
        {
            /* Primary RIL instance, so allow to create DB */
            break;
        }

        //RIL2/3 will wait here for DB to be created.
        retries++;
        QCRIL_LOG_INFO("Waiting for DB creation. Retry: %d",retries);
        usleep(QCRIL_DB_SLEEP_TIME);
    } while(retries <= MAX_RETRY);

    if(qmi_ril_get_process_instance_id() == QCRIL_DEFAULT_INSTANCE_ID)
    {
        // Expected db file does not exist in data partition
        // Check if prebuilt db file is available
        if ( access( QCRIL_PREBUILT_DB_NAME, R_OK ) )
        {
            QCRIL_LOG_INFO("%s is not available to RIL, reason: %s",
                QCRIL_PREBUILT_DB_NAME, strerror(errno));
            ret = E_FAILURE;
        }
        else
        {
            QCRIL_LOG_INFO("Prebuilt db available. Copy to %s",QCRIL_DATABASE_NAME);

            //when copying db reset db_upgrade property so that upgrade will
            //happen again after device exit encryption
            QCRIL_LOG_INFO("Setting value of ADB prop(%s) to: %d",
                    QCRIL_DB_UPGRADE_COMPLETE,0);
            qmi_ril_set_property_value_to_integer(QCRIL_DB_UPGRADE_COMPLETE, 0);

            // do copy work
            ret = qcril_file_copy_file(QCRIL_PREBUILT_DB_NAME, QCRIL_DATABASE_NAME);
            if (ret)
            {
                QCRIL_LOG_ERROR("Failed to copy %s to %s",
                    QCRIL_PREBUILT_DB_NAME, QCRIL_DATABASE_NAME);
                ret = E_FAILURE;
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return ret;
}

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
void qcril_db_update_cur_modem_version
(
    void
)
{
    char cur_ver_info[PROPERTY_VALUE_MAX]     = {0};
    char prev_ver_info[PROPERTY_VALUE_MAX]    = {0};

    qcril_file_read_string_from_file(QCRIL_DB_MCFG_VER_INFO,
                                     cur_ver_info, sizeof(cur_ver_info) - 1,
                                     sizeof(cur_ver_info) - 1);
    qmi_ril_get_property_value_from_string(QCRIL_DB_MBN_VER_INFO,
                                            prev_ver_info, "");
    if (strcmp(prev_ver_info, cur_ver_info))
    {
        QCRIL_LOG_DEBUG("updating cur version");
        qmi_ril_set_property_value_to_string(QCRIL_DB_MBN_VER_INFO,
                                             cur_ver_info);
    }

    return;
}

/*===========================================================================

  FUNCTION  qcril_db_is_modem_image_updated

===========================================================================*/
/*!
    @brief
    Check if modem image is udated.

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
boolean qcril_db_is_modem_image_updated
(
    void
)
{
    boolean is_ver_updated    = FALSE;
    char cur_ver_info[PROPERTY_VALUE_MAX]     = {0};
    char prev_ver_info[PROPERTY_VALUE_MAX]    = {0};

    qcril_file_read_string_from_file(QCRIL_DB_MCFG_VER_INFO,
                                     cur_ver_info, sizeof(cur_ver_info) - 1,
                                     sizeof(cur_ver_info) - 1);
    qmi_ril_get_property_value_from_string(QCRIL_DB_MBN_VER_INFO,
                                            prev_ver_info, "");
    if (strcmp(prev_ver_info, cur_ver_info))
    {
        QCRIL_LOG_DEBUG("version info updated");
        is_ver_updated = TRUE;
    }

    QCRIL_LOG_DEBUG("prev_ver_info: %s, cur_ver_info: %s", prev_ver_info, cur_ver_info);
    return is_ver_updated;
}

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
boolean qcril_db_is_sw_mbn_reevaluation_needed
(
    void
)
{
    int     is_sw_db_loaded   = 0;
    boolean is_ver_updated    = FALSE;

    qmi_ril_get_property_value_from_integer(QCRIL_MBN_SW_MBN_LOADED,
                                            &is_sw_db_loaded, 0);
    QCRIL_LOG_DEBUG("is sw db loaded = %d", is_sw_db_loaded);

    if (is_sw_db_loaded)
    {
        is_ver_updated = qcril_db_is_modem_image_updated();
        if (is_ver_updated)
        {
            qmi_ril_set_property_value_to_integer(QCRIL_MBN_SW_MBN_LOADED, 0);
        }
    }

    return (!is_sw_db_loaded || is_ver_updated);
}

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
boolean qcril_db_is_hw_mbn_reevaluation_needed
(
    void
)
{
    int     is_hw_db_loaded   = 0;
    boolean is_ver_updated    = FALSE;

    qmi_ril_get_property_value_from_integer(QCRIL_MBN_HW_MBN_LOADED,
                                            &is_hw_db_loaded, 0);
    QCRIL_LOG_DEBUG("is hw db loaded = %d", is_hw_db_loaded);

    if (is_hw_db_loaded)
    {
        is_ver_updated = qcril_db_is_modem_image_updated();
        if (is_ver_updated)
        {
            qmi_ril_set_property_value_to_integer(QCRIL_MBN_HW_MBN_LOADED, 0);
        }
    }

    return (!is_hw_db_loaded || is_ver_updated);
}

/*===========================================================================

  FUNCTION  qcril_db_evaluate_drop

===========================================================================*/
/*!
    @brief
    Check and update if table needs to be dropped.

    @return
    none
*/
/*=========================================================================*/
void qcril_db_evaluate_drop
(
    void
)
{
    if (!qcril_db_is_hw_mbn_reevaluation_needed())
    {
        qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].drop_during_bootup = 0;
    }

    if (!qcril_db_is_sw_mbn_reevaluation_needed())
    {
        qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].drop_during_bootup = 0;
        qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].drop_during_bootup = 0;
        qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].drop_during_bootup = 0;
    }
}

/*===========================================================================

  FUNCTION  qcril_db_upgrade_get_version

===========================================================================*/
/*!
    @brief
    Get the current version of the db from the properties table

    @return the version or -1 if not found
    None
*/
/*=========================================================================*/
int qcril_db_upgrade_get_version(uint8_t vendor)
{
    const char get_version_sql[] =
        "SELECT VALUE from " QCRIL_PROPERTIES_TABLE_NAME " where PROPERTY = 'qcrildb_version'";
    int version = -1;
    int rc = -1;
    sqlite3 *db_handle = NULL;
    sqlite3_stmt *stmt = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (vendor)
        {
            rc = sqlite3_open_v2("file:" QCRIL_PREBUILT_DB_NAME "?immutable=1", &db_handle,
                                 SQLITE_OPEN_READONLY | SQLITE_OPEN_URI, NULL);
            if (SQLITE_OK != rc)
            {
                QCRIL_LOG_ERROR("Failed to open " QCRIL_PREBUILT_DB_NAME ": %d\n", rc);
                break;
            }
        }
        else
        {
            db_handle = qcril_db_handle;
        }

        rc = sqlite3_prepare_v2(db_handle,
                get_version_sql,
                sizeof(get_version_sql),
                &stmt,
                NULL);

        if (rc != SQLITE_OK)
        {
            QCRIL_LOG_ERROR("Unable to prepare query %s", get_version_sql);
            break;
        }

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_ROW)
        {
            break;
        }

        version = sqlite3_column_int(stmt, 0);

        rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW)
        {
            QCRIL_LOG_WARN("Unexpected multiple results for query %s. Using the first only", get_version_sql);
        }
        sqlite3_finalize(stmt);

    } while (0);

    if (vendor) {
        // db_handle could be NULL if the vendor DB failed to open.
        // However, calling sqlite3_close() with a NULL pointer
        // argument is a harmless no-op.
        sqlite3_close(db_handle);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(version);
    return version;
}

int qcril_upgrade_file_sort(const struct dirent **a, const struct dirent **b)
{
    int va = -1, vb = -1;
    char *namea = NULL, *nameb = NULL;
    int ret = 0;
    sscanf(a[0]->d_name, "%d%ms", &va, &namea);
    sscanf(b[0]->d_name, "%d%ms", &vb, &nameb);
    ret = vb - va;
    if (!ret)
    {
        ret = strcmp(namea, nameb);
    }
    return ret;
}
int qcril_file_filter(const struct dirent *de)
{
    int ret = 0;
    if (de->d_type == DT_REG) {
        ret = 1;
    }
    return ret;
}
/*===========================================================================

  FUNCTION  qcril_db_upgrade

===========================================================================*/
/*!
    @brief
    Perform an upgrade by applying the necessary upgrade scripts

    @return
    None
*/
/*=========================================================================*/
void qcril_db_upgrade(int cur_version, int new_version)
{
    int rc;
    int num_files;
    struct dirent **file_names = NULL;
    int dirfd = -1;
    char error[256];

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_NOTUSED(cur_version);
    QCRIL_NOTUSED(new_version);
    memset(error, 0, sizeof(error));

    do {
        dirfd = open(UPGRADE_SCRIPTS_PATH, O_RDONLY|O_DIRECTORY);
        if (dirfd < 0)
        {
            strerror_r(errno, error, sizeof(error));
            QCRIL_LOG_ERROR("Unable to open %s as a folder: %s",
                    UPGRADE_SCRIPTS_PATH, error);
            break;
        }

        num_files = scandirat(dirfd, ".",
                &file_names,
                qcril_file_filter,
                qcril_upgrade_file_sort);

        if (num_files < 0)
        {
            QCRIL_LOG_ERROR("Unable to retrieve files from " UPGRADE_SCRIPTS_PATH);
            break;
        }

        for( int i = 0; i < num_files; i++)
        {
            size_t size = 0;
            void *address;
            char *errmsg = NULL;

            address = qcril_file_open_mmap_at_for_read(
                    dirfd,
                    file_names[i]->d_name,
                    &size);
            if (address == MAP_FAILED)
            {
                QCRIL_LOG_ERROR("Ignoring file %s", file_names[i]->d_name);
                continue;
            }

            rc = sqlite3_exec(
                    qcril_db_handle,
                    (const char *)address,
                    NULL,
                    NULL,
                    &errmsg);
            if (rc != SQLITE_OK)
            {
                QCRIL_LOG_ERROR("Error executing upgrade step %s: %s",
                        file_names[i]->d_name,
                        errmsg);
                break;
            }
            QCRIL_LOG_INFO("Successful upgrade step %s",file_names[i]->d_name);
        }
    } while(0);
    free(file_names);

    QCRIL_LOG_FUNC_RETURN();
}

int qcril_db_open()
{
    int retry_count = 0;
    int res = SQLITE_ERROR;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        /* open qcril DB */
        if (SQLITE_OK !=
                 (res = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                         &qcril_db_handle,
                          SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)))
        {
            QCRIL_LOG_ERROR("Failed to open qcril db %d\n", res);
            usleep(QCRIL_DB_SLEEP_TIME);
            retry_count++;
        }
        else
        {
            QCRIL_LOG_DEBUG("qcril db open successful\n");
            break;
        }
        QCRIL_LOG_DEBUG("db open retry - %d", retry_count);
    } while(retry_count < MAX_RETRY);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_check_and_upgrade

===========================================================================*/
/*!
    @brief
    Compare the current db version with the one in the vendor partition
    and upgrade if necessary

    @return
    None
*/
/*=========================================================================*/
void qcril_db_check_and_upgrade
(
)
{
    const char presql[] = "PRAGMA main.locking_mode = EXCLUSIVE;";
    const char postsql[] = "PRAGMA main.locking_mode = NORMAL;";
    int vnd_version = -1;
    int local_version = -1;
    int rc = -1;
    char *error = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        rc = sqlite3_exec(qcril_db_handle, presql, NULL, NULL, &error);
        if (rc != SQLITE_OK)
        {
            QCRIL_LOG_ERROR("Unable to set locking mode to EXCLUSIVE for " QCRIL_DATABASE_NAME ": %s", error);
            sqlite3_free(error);
            break;
        }
        vnd_version = qcril_db_upgrade_get_version(TRUE);
        local_version = qcril_db_upgrade_get_version(FALSE);

        QCRIL_LOG_DEBUG("Vendor version: %d. Local version: %d",
                vnd_version,
                local_version);
        if (vnd_version < 0) {
            QCRIL_LOG_ERROR("Unable to find vendor version. Not upgrading");
            break;
        }

        if (local_version < 0) local_version = 0;

        if (local_version < vnd_version)
        {
            QCRIL_LOG_INFO("Performing upgrade");
            qcril_db_upgrade(local_version, vnd_version);
        }
        else if (local_version > vnd_version)
        {
            QCRIL_LOG_INFO("Downgrading by removing the existing db");
            sqlite3_close(qcril_db_handle);
            unlink(QCRIL_DATABASE_NAME);
            rc = qcril_db_open();
            if (rc != SQLITE_OK) {
                QCRIL_LOG_ERROR("Unable to reopen database after unlinking");
            }
        }
    } while(0);

    error = NULL;
    rc = sqlite3_exec(qcril_db_handle, postsql, NULL, NULL, &error);
    if (rc != SQLITE_OK)
    {
        QCRIL_LOG_ERROR("Unable to set locking mode to NORMAL for " QCRIL_DATABASE_NAME ": %s", error);
        sqlite3_free(error);
    }

    QCRIL_LOG_FUNC_RETURN();
} /* qcril_db_check_and_upgrade */

/*===========================================================================

  FUNCTION  qcril_db_init

===========================================================================*/
/*!
    @brief
    Initialize qcril db and tables.

    @return
    0 if function is successful.
*/
/*=========================================================================*/

int qcril_db_init
(
    void
)
{
    int     res     = SQLITE_OK;
    char   *zErrMsg = NULL;

    char    create_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     i;
    char   *drop_stmt_fmt = "drop table if exists '%s'";
    char    drop_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     retry_count = 0;
    int     db_upgraded = 0;
    int     db_upgraded_initial = 0;

    QCRIL_LOG_FUNC_ENTRY();

    res = sqlite3_threadsafe();
    QCRIL_LOG_INFO("Sqlite threadsafe: %d\n", res);

    qcril_db_evaluate_drop();
    qcril_db_check_prebuilt_and_wait();


    /* initialize sqlite in serialized mode */
    if(SQLITE_OK != (res = sqlite3_config(SQLITE_CONFIG_SERIALIZED)))
    {
        QCRIL_LOG_ERROR("Failed to configure sqlite3 in serialized mode: %d\n", res);
    }

    /* initialize sqlite engine */
    if (SQLITE_OK != (res = sqlite3_initialize()))
    {
        QCRIL_LOG_ERROR("Failed to initialize sqlite3: %d\n", res);
    }
    else
    {
        /* open qcril DB */
        if (SQLITE_OK !=
                 (res = qcril_db_open()))
        {
            QCRIL_LOG_ERROR("Failed to open qcril db %d\n", res);
        }
        else
        {
            /* for allowing multiple RILDs to access database,
            ** set busy handler. This will call qcril_db_busy_handler
            ** and will retry till qcril_db_busy_handler returns 0.*/
            QCRIL_LOG_INFO("register qcril db busy handler");
            sqlite3_busy_handler(qcril_db_handle, qcril_db_busy_handler, qcril_db);

            if ( qmi_ril_get_process_instance_id() == QCRIL_DEFAULT_INSTANCE_ID )
            {
                qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);
                qmi_ril_get_property_value_from_integer(QCRIL_DB_UPGRADE_COMPLETE,
                                        &db_upgraded_initial, 0);
                QCRIL_LOG_INFO("Initial value of ADB prop(%s): %d",
                            QCRIL_DB_UPGRADE_COMPLETE,db_upgraded_initial);

                if(!db_upgraded_initial)
                {
                    qcril_db_check_and_upgrade();
                    /* Initialize provisioning table */

                    for (i = QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC;
                         i < (QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX); i++)
                    {
                        if (qcril_db_emergency_number_tables[i].table_name &&
                             qcril_db_emergency_number_tables[i].create_stmt)
                        {
                            snprintf(create_stmt, sizeof(create_stmt),
                                     qcril_db_emergency_number_tables[i].create_stmt,
                                     qcril_db_emergency_number_tables[i].table_name);

                            /* create qcril DB tables */
                            if (SQLITE_OK !=
                                     (res = sqlite3_exec(qcril_db_handle,
                                             create_stmt, NULL, NULL, &zErrMsg)))
                            {
                                if (zErrMsg)
                                {
                                    QCRIL_LOG_ERROR("stmt %s", create_stmt);
                                    QCRIL_LOG_ERROR("Could not create table %d %s",
                                                     res, zErrMsg);
                                    sqlite3_free(zErrMsg);
                                }
                            }
                        }
                        memset(create_stmt,0,sizeof(create_stmt));
                    }

                    for (i = QCRIL_DB_TABLE_FIRST;
                         i < QCRIL_DB_TABLE_MAX; i++)
                    {
                        if (qcril_db_tables[i].table_name &&
                             qcril_db_tables[i].create_stmt)
                        {
#ifndef QMI_RIL_UTF
                            if (qcril_db_tables[i].drop_during_bootup)
                            {
                                QCRIL_LOG_DEBUG("dropping %d", i);
                                snprintf(drop_stmt, sizeof(drop_stmt),
                                         drop_stmt_fmt,
                                         qcril_db_tables[i].table_name);

                                if (SQLITE_OK !=
                                (res = sqlite3_exec(qcril_db_handle,
                                        drop_stmt, NULL, NULL, &zErrMsg)))
                                {
                                    if (zErrMsg)
                                    {
                                        QCRIL_LOG_ERROR("stmt %s", drop_stmt);
                                        QCRIL_LOG_ERROR("Could not drop table %d %s",
                                                         res, zErrMsg);
                                        sqlite3_free(zErrMsg);
                                    }
                                }
                            }
#endif

                            snprintf(create_stmt, sizeof(create_stmt),
                                     qcril_db_tables[i].create_stmt,
                                     qcril_db_tables[i].table_name);

                            /* create qcril DB tables */
                            if (SQLITE_OK !=
                                     (res = sqlite3_exec(qcril_db_handle,
                                             create_stmt, NULL, NULL, &zErrMsg)))
                            {
                                if (zErrMsg)
                                {
                                    QCRIL_LOG_ERROR("stmt %s", create_stmt);
                                    QCRIL_LOG_ERROR("Could not create table %d %s",
                                                     res, zErrMsg);
                                    sqlite3_free(zErrMsg);
                                }
                            }
                        }

                        memset(create_stmt,0,sizeof(create_stmt));
                        memset(drop_stmt, 0, sizeof(drop_stmt));
                    }

                    qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);

                    QCRIL_LOG_INFO("Setting value of ADB prop(%s) to: %d",
                                                  QCRIL_DB_UPGRADE_COMPLETE,1);
                    qmi_ril_set_property_value_to_integer(QCRIL_DB_UPGRADE_COMPLETE, 1);
                }
            }
            else
            {
                /* RIL[1,2] will wait for RIL0 to upgrade qcril database. */
                do
                {
                    qmi_ril_get_property_value_from_integer(QCRIL_DB_UPGRADE_COMPLETE,
                                            &db_upgraded, 0);
                    QCRIL_LOG_INFO("Value of ADB prop(%s): %d",
                            QCRIL_DB_UPGRADE_COMPLETE,db_upgraded);
                    if(db_upgraded == 1)
                    {
                        QCRIL_LOG_INFO("DB Operations complete");
                        break;
                    }
                    sleep(1);
                    retry_count++;
                    QCRIL_LOG_INFO("retry count - %d DB Operations done - %d",
                                                        retry_count, db_upgraded);
                }while (retry_count <= 2*MAX_RETRY );
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_is_mcc_part_of_emergency_numbers_table

===========================================================================*/
/*!
    @brief
    Checks for mcc existence in db and retireves emergency number from db.

    @return
    1 if function is successful.
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
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;
    int     ret     = SQLITE_OK;
    char    emergency_numbers[QCRIL_MAX_EMERGENCY_NUMBERS_LEN + RESERVED_TO_STORE_LENGTH] = {0};

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("Source %d MCC %s MNC %s", source, mcc? mcc: "null", mnc? mnc: "null");
    if (!qcril_db_handle)
        return res;

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc &&
         qcril_db_emergency_number_tables[source].table_name   &&
         emergency_num)
    {
        if ( is_mcc_valid && is_mnc_valid && mcc && mnc &&
             (source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC ||
              source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC) )
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_emergency_matching_mcc_mnc_stmt,
                     qcril_db_emergency_number_tables[source].table_name, mcc, mnc);
        }
        else
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_emergency_number_stmt,
                     qcril_db_emergency_number_tables[source].table_name, mcc);
        }

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                  qcril_db_retrieve_emergency_num_callback,
                                  emergency_numbers)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            if ( *((int*)emergency_numbers) > 0 )
            {
                res = TRUE;
                strlcpy(emergency_num, emergency_numbers + RESERVED_TO_STORE_LENGTH,
                         QCRIL_MAX_EMERGENCY_NUMBERS_LEN);
                QCRIL_LOG_INFO("Emergency numbers %s", emergency_num);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_is_mcc_part_of_emergency_numbers_table_with_service_state

===========================================================================*/
/*!
    @brief
    Checks for mcc & service state existence in db and retireves
    emergency number from db.

    @return
    1 if function is successful.
*/
/*=========================================================================*/
int qcril_db_is_mcc_part_of_emergency_numbers_table_with_service_state
(
    qmi_ril_custom_emergency_numbers_source_type source,
    int  is_mcc_valid,
    char *mcc,
    int  is_mnc_valid,
    char *mnc,
    const char *service,
    char *emergency_num
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;
    int     ret     = SQLITE_OK;
    char    emergency_numbers[QCRIL_MAX_EMERGENCY_NUMBERS_LEN + RESERVED_TO_STORE_LENGTH] = {0};

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("Source %d MCC %s MNC %s", source, mcc? mcc: "null", mnc? mnc: "null");
    if (!qcril_db_handle)
        return res;

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc &&
         service && qcril_db_emergency_number_tables[source].table_name
         && emergency_num)
    {
        if ( is_mnc_valid && mnc &&
             (source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC ||
              source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC) )
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_from_mcc_mnc_and_service_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     mnc,
                     service);
        }
        else
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_from_mcc_and_service_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     service);
        }

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                  qcril_db_retrieve_emergency_num_callback,
                                  emergency_numbers)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            if ( *((int*)emergency_numbers) > 0 )
            {
                res = TRUE;
                strlcpy(emergency_num, emergency_numbers + RESERVED_TO_STORE_LENGTH,
                         QCRIL_MAX_EMERGENCY_NUMBERS_LEN);
                QCRIL_LOG_INFO("Emergency numbers %s", emergency_num);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_number_from_emergency_table

===========================================================================*/
/*!
    @brief
    Checks whether number present in table as per query

    @return
    0 if function is successful.
*/
/*=========================================================================*/
int qcril_db_query_number_from_emergency_table
(
    char *query,
    int  *is_num_present
)
{
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();
    if (!qcril_db_handle)
        return ret;

    if ( query && is_num_present )
    {
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                         qcril_db_check_num_and_mcc_callback,
                                         is_num_present)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

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
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc && emergency_num &&
         qcril_db_emergency_number_tables[source].table_name)
    {
        QCRIL_LOG_INFO("Source %d MCC %s MNC %s emergency num %s", source, mcc, mnc? mnc: "null",emergency_num);
        memset(qcril_db_emergency_numbers, 0, sizeof(qcril_db_emergency_numbers));

        if ( is_mnc_valid && mnc &&
             (source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC ||
              source == QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC) )
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_and_mcc_mnc_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     mnc,
                     emergency_num);
        }
        else
        {
            snprintf(query, sizeof(query),
                     qcril_db_query_number_and_mcc_stmt,
                     qcril_db_emergency_number_tables[source].table_name,
                     mcc,
                     emergency_num);
        }

        qcril_db_query_number_from_emergency_table(query,&res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
    const char *service
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         mcc && emergency_num &&
         qcril_db_emergency_number_tables[source].table_name)
    {
        QCRIL_LOG_INFO("Source %d MCC %s emergency num %s", source, mcc, emergency_num);

        snprintf(query, sizeof(query),
                 qcril_db_query_number_and_mcc_and_service_stmt,
                 qcril_db_emergency_number_tables[source].table_name,
                 mcc,
                 emergency_num,
                 service);

        qcril_db_query_number_from_emergency_table(query,&res);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}



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
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = FALSE;
    int     ret     = SQLITE_OK;
    QCRIL_NOTUSED(is_mnc_valid);
    QCRIL_NOTUSED(mnc);

    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO("Source %d MCC %s emergency_num %s",
                   source,
                   mcc? mcc: "null",
                   emergency_num? emergency_num: "null");
    if (!qcril_db_handle)
        return res;

    if ((source < QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX) &&
         is_mcc_valid && mcc && emergency_num && ims_address &&
         qcril_db_emergency_number_tables[source].table_name)
    {

        snprintf(query, sizeof(query),
                 qcril_db_query_ims_address_from_mcc_number_stmt,
                 qcril_db_emergency_number_tables[source].table_name,
                 mcc,
                 emergency_num);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                  qcril_db_retrieve_ims_address_from_mcc_emergency_num_callback,
                                  ims_address)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            QCRIL_LOG_INFO("ims_address %s", ims_address);
            if(strlen(ims_address))
            {
                res = TRUE;
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = 0;
    int     ret     = SQLITE_OK;
    qcril_db_escv_in_out result;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&result, 0, sizeof(result));
    QCRIL_LOG_INFO(" emergency_num %s iin %s mcc %s mnc %s roam %s",
                     emergency_num? emergency_num : "null",
                     iin? iin : "null",
                     mcc? mcc : "null",
                     mnc? mnc : "null",
                     roam? roam : "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!emergency_num )
        {
            break;
        }

        if ( iin && roam )
        {
            snprintf(query, sizeof(query),
               qcril_db_query_escv_iin_stmt,
               qcril_db_emergency_number_tables[QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_IIN].table_name,
               iin,
               emergency_num,
               roam);
            result.escv_type = -1;
        }
        else if ( mcc )
        {
            snprintf(query, sizeof(query),
               qcril_db_query_escv_nw_stmt,
               qcril_db_emergency_number_tables[QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_NW].table_name,
               mcc,
               emergency_num);

            if (mnc)
            {
                result.mnc = mnc;
            }
        }
        else
        {
            break;
        }

        QCRIL_LOG_INFO(" Query %s", query);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(query,
                                         qcril_db_check_escv_callback,
                                         &result)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }
        else
        {
            res = result.escv_type;
        }
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}


/*===========================================================================

  FUNCTION  qcril_db_query_properties_table

===========================================================================*/
/*!
    @brief
    Query property table.
    Caller of this function should pass sufficient buffer (value)
    for storing the information retrieved from database

    @return
    None
*/
/*=========================================================================*/
void qcril_db_query_properties_table
(
    const char *property_name,
    char *value
)
{
    int     ret     = SQLITE_OK;
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        // if null pointer
        if ( !value || !property_name )
        {
            break;
        }

        /* Initialize it to default value */
        *value = '\0';

        snprintf(query,
                 sizeof(query),
                 qcril_db_query_properties_stmt,
                 QCRIL_PROPERTIES_TABLE_NAME,
                 property_name
                 );

        QCRIL_LOG_INFO(" Query %s", query);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                               query, qcril_db_query_property_callback, value)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
            break;
        }

        QCRIL_LOG_INFO("value - %s", value);

    } while (FALSE);

    QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION  qcril_db_insert_properties_table

===========================================================================*/
/*!
    @brief
    Insert property table.

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_properties_table
(
    char *property_name,
    char *value
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    char    create_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    int     table_exists = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" property_name: %s, value: %s",
                     property_name? property_name : "null",
                     value? value : "null");

    do
    {
        // db not ready
        if (!qcril_db_handle) {
            break;
        }
        // if null pointer
        if ( !value || !property_name )
        {
            break;
        }

        table_exists =
            qcril_db_check_if_table_exists(QCRIL_PROPERTIES_TABLE_NAME);

        if ( !table_exists )
        {
            // create rat tlv table.
            snprintf(create_stmt,
                     sizeof(create_stmt),
                     QCRIL_DB_CREATE_PROPERTIES_TABLE,
                     QCRIL_PROPERTIES_TABLE_NAME);

            if (SQLITE_OK != (res = qcril_db_sqlite3_exec(
                                      create_stmt, NULL, NULL)))
            {
                QCRIL_LOG_ERROR("stmt %s", create_stmt);
                QCRIL_LOG_ERROR("Could not create table %d", res);
                break;
            }
        }


        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_properties_stmt,
                 QCRIL_PROPERTIES_TABLE_NAME,
                 property_name,
                 value
                 );

        QCRIL_LOG_INFO("insert_stmt %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_operator_type_callback

===========================================================================*/
/*!
    @brief
    retrieve operator type.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_operator_type_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            strlcpy(data, argv[0], QCRIL_DB_MAX_OPERATOR_TYPE_LEN);
        }
    }

    return ret;
}

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
)
{
    char    query[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s, mnc: %s",
                     mcc? mcc : "null",
                     mnc? mnc : "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!(mcc && mnc && operator_type))
        {
            break;
        }

        snprintf(query, sizeof(query),
           qcril_db_query_operator_type_stmt,
           qcril_db_tables[QCRIL_DB_TABLE_OPERATOR_TYPE].table_name,
           mcc,
           mnc);

        QCRIL_LOG_INFO(" Query: %s", query);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                        query,
                                        qcril_db_query_operator_type_callback,
                                        operator_type)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }

    } while (0);

    return;
}

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
)
{
    char    update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s, mnc: %s, operator type %s",
                     mcc? mcc : "null",
                     mnc? mnc : "null",
                     operator_type? operator_type: "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!(mcc && mnc && operator_type))
        {
            break;
        }

        snprintf(update_stmt, sizeof(update_stmt),
                 qcril_db_update_operator_type_stmt,
                 qcril_db_tables[QCRIL_DB_TABLE_OPERATOR_TYPE].table_name,
                 operator_type, mcc, mnc);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" update stmt: %s", update_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);

        res = E_SUCCESS;

    } while (0);

    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s, mnc: %s, operator type %s",
                     mcc? mcc : "null",
                     mnc? mnc : "null",
                     operator_type? operator_type: "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!(mcc && mnc && operator_type))
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_operator_type_stmt,
                 qcril_db_tables[QCRIL_DB_TABLE_OPERATOR_TYPE].table_name,
                 mcc, mnc, operator_type);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                    insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
        res = E_SUCCESS;

    } while (0);

    return res;
}

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
)
{
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!query_string || !call_back || !output)
        {
            break;
        }

        QCRIL_LOG_INFO(" Query: %s", query_string);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                   query_string, call_back, output)))
        {
            QCRIL_LOG_ERROR("Could not query %d", ret);
        }

    } while (0);

    return;
}

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
)
{
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %d",
                     type? type : "null", value);
    do {
        if (!(type && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_int(qcril_db_update_hw_mbn_file_int_stmt,
                                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

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
)
{
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %s",
                     type? type : "null",
                     value? value : "null");
    do {

        if (!(type && value && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_str(qcril_db_update_hw_mbn_file_str_stmt,
                                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

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
)
{
    int     res     = E_FAILURE;
    char *new_file = NULL;
    char *short_name = NULL;
    char *type = "SHORT_NAME";
    int slash_start_pos = 0;
    int slash_end_pos = 0;
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    do {

        if (!file)
        {
            break;
        }


        res = qcril_db_insert_mbn_file(qcril_db_insert_hw_mbn_file_stmt,
                             qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                             file);
        if (res)
        {
            break;
        }
        // Find the last '/'
        slash_end_pos = strrchr(file, '/') - file + 1;
        new_file = malloc(slash_end_pos*sizeof(char));
        if(NULL != new_file)
        {
            memset(new_file, 0, slash_end_pos);
            memcpy(new_file, file, slash_end_pos - 1);
            QCRIL_LOG_INFO("new_file: %s", new_file);

            // Find the second to the last '/' and get the folder name before *.mbn
            slash_start_pos = strrchr(new_file, '/') - new_file + 1;
            short_name = malloc((slash_end_pos - slash_start_pos)*sizeof(char));
            if(NULL != short_name)
            {
                memset(short_name, 0, slash_end_pos - slash_start_pos);
                memcpy(short_name, file + slash_start_pos, slash_end_pos - slash_start_pos - 1);
                QCRIL_LOG_INFO("short_name: %s", short_name);
            }
        }

        res = qcril_db_update_mbn_file_with_str(qcril_db_update_hw_mbn_file_str_stmt,
                                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                                 file, type, short_name);
        if(NULL != new_file)
        {
            free(new_file);
        }
        if(NULL != short_name)
        {
            free(short_name);
        }
    } while (0);

    QCRIL_LOG_FUNC_RETURN();
    return res;
}

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
)
{
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %d",
                     type? type : "null",
                     value);
    do {

        if (!(type && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_int(qcril_db_update_sw_mbn_file_int_stmt,
                                 qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

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
)
{
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, value: %s",
                     type? type : "null",
                     value? value : "null");
    do {

        if (!(type && value && file))
        {
            break;
        }

        res = qcril_db_update_mbn_file_with_str(qcril_db_update_sw_mbn_file_str_stmt,
                                 qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                                 file, type, value);

    } while (0);

    return res;
}

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
)
{
    int     res     = E_FAILURE;
    QCRIL_LOG_FUNC_ENTRY();
    QCRIL_LOG_INFO(" file: %s", file? file: "null")
    do {

        if (!file)
        {
            break;
        }

        res = qcril_db_insert_mbn_file(qcril_db_insert_sw_mbn_file_stmt,
                             qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                             file);

    } while (0);

    QCRIL_LOG_FUNC_RETURN();
    return res;
}

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
)
{
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!update_stmt)
        {
            break;
        }
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  update_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not update %d", ret);
            break;
        }
        res = E_SUCCESS;
    }
    while (0);

    return res;
}

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
    const char *table_name,
    char *file_name,
    char *type,
    int value
)
{
    char    update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, file_name : %s",
                     type? type : "null",
                     file_name? file_name : "null");
    do {

        if (!(type && file_name && update_stmt_fmt && table_name))
        {
            break;
        }

        snprintf(update_stmt, sizeof(update_stmt),
                 update_stmt_fmt,
                 table_name,
                 type, value, file_name);

        QCRIL_LOG_INFO(" update stmt: %s", update_stmt);

        res = qcril_db_update_mbn_file(update_stmt);

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
    const char *table_name,
    char *file_name,
    char *type,
    char *value
)
{
    char    update_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" type: %s, file_name : %s",
                     type? type : "null",
                     file_name? file_name : "null");
    do {

        if (!(type && value && file_name && update_stmt_fmt && table_name))
        {
            break;
        }

        snprintf(update_stmt, sizeof(update_stmt),
                 update_stmt_fmt,
                 table_name,
                 type, value, file_name);

        QCRIL_LOG_INFO(" update stmt: %s", update_stmt);

        res = qcril_db_update_mbn_file(update_stmt);

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
    const char *table_name,
    char *file
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null")
    do {

        if (!qcril_db_handle)
        {
            break;
        }
        if (!file || !insert_stmt_fmt || !table_name)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 insert_stmt_fmt,
                 table_name,
                 file);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_mbn_int_callback

===========================================================================*/
/*!
    @brief
    Call back for querying integer value from mbn

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_mbn_int_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            *(uint32_t*)data = atoi(argv[0]);
            QCRIL_LOG_DEBUG(" data %x", *(uint32_t*)data);
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}
/*===========================================================================

  FUNCTION  qcril_db_query_mbn_string_callback

===========================================================================*/
/*!
    @brief
    Call back for querying string value from mbn

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_mbn_string_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;
    int len = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            len = strlen(argv[0]);
            if (len >= QCRIL_MBN_HW_STR_MAX_SIZE)
            {
                QCRIL_LOG_ERROR("Unexpected length");
            }
            else
            {
                strlcpy(data, argv[0], QCRIL_MBN_HW_STR_MAX_SIZE);
            }
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}

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
    const char   *device_type
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        QCRIL_LOG_ERROR("device_type: %s", device_type? device_type: "null")

        if (!mbn_file_name || !device_type)
        {
            res = -1;
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_hw_mbn_file_stmt,
                 "FILE",
                 qcril_db_tables[QCRIL_DB_HW_MBN_FILE_TYPE].table_name,
                 "SHORT_NAME",
                 device_type);

        *mbn_file_name = malloc(QCRIL_MBN_HW_STR_MAX_SIZE*sizeof(char));
        *mbn_file_name[0] = 0;
        qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                (unsigned char *)*mbn_file_name);

        QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION qcril_db_query_sw_mbn_file_with_mcc_mnc

===========================================================================*/
/*!
    @brief
    Query mbn file with mcc and mnc

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
    const char        *lab_info
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res = 0;

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        QCRIL_LOG_INFO("mcc %s", mcc? mcc: "null")
        QCRIL_LOG_INFO("mnc %s", mnc? mnc: "null")
        QCRIL_LOG_INFO("volte_info %s", volte_info? volte_info: "null");
        QCRIL_LOG_INFO("mkt_info %s", mkt_info? mkt_info: "null");
        QCRIL_LOG_INFO("lab_info %s", lab_info? lab_info: "null");

        if (!mbn_file_name || !mcc || !mnc || !volte_info || !mkt_info || !lab_info)
        {
            res = -1;
            break;
        }

        *mbn_file_name = malloc(QCRIL_MBN_HW_STR_MAX_SIZE * sizeof(char));

        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;

            if (strlen(volte_info) || strlen(mkt_info) || strlen(lab_info))
            {
                // Query base on mcc + mnc + volte_info + mkt_info
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_five_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].table_name,
                         "MCC", mcc, "MNC", mnc, "VOLTE_INFO", volte_info,
                         "MKT_INFO", mkt_info, "LAB_INFO", lab_info);

                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            // Query base on mcc + mnc only
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_four_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].table_name,
                         "MCC", mcc, "MNC", mnc, "MKT_INFO", mkt_info, "LAB_INFO", lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
  char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
  int res = 0;

  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    if (!mbn_file_name || !type || !value)
    {
        res = -1;
        break;
    }

    snprintf(query_stmt, sizeof(query_stmt),
             qcril_db_query_sw_mbn_str_stmt,
             type,
             qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
             "FILE",
             mbn_file_name);

    qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_int_callback,
                            (unsigned char *)value);

    QCRIL_LOG_INFO("value %d", *value);
  } while (0);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
  return res;
}

int qcril_db_query_sw_mbn_file_from_version_type
(
    uint32_t    family,
    uint32_t    carrier,
    uint32_t    minor,
    char        **mbn_file_name
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        if (!mbn_file_name)
        {
            res = -1;
            break;
        }

        snprintf(query_stmt, QCRIL_DB_MAX_STMT_LEN,
                 qcril_db_query_mbn_file_stmt_three_int_args,
                 "FILE",
                 qcril_db_tables[QCRIL_DB_SW_MBN_FILE_TYPE].table_name,
                 "MCFG_VERSION_FAMILY",
                 family,
                 "MCFG_VERSION_CARRIER",
                 carrier,
                 "MCFG_VERSION_MINOR",
                 minor);
        *mbn_file_name = malloc(QCRIL_MBN_HW_STR_MAX_SIZE*sizeof(char));
        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;
            qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                    (unsigned char *)*mbn_file_name);
        }
        else
        {
            res = -1;
            QCRIL_LOG_ERROR("failed to allocate memory for mbn file name");
        }
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
    const char    *lab_info
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    char    iin_6_digit[IIN_6_LEN] = {0};
    char    iin_7_digit[IIN_7_LEN] = {0};

    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        QCRIL_LOG_INFO("iin %s", iin? iin: "null")
        QCRIL_LOG_INFO("volte_info %s", volte_info? volte_info: "null");
        QCRIL_LOG_INFO("mkt_info %s", mkt_info? mkt_info: "null");
        QCRIL_LOG_INFO("lab_info %s", lab_info? lab_info: "null");

        if (!mbn_file_name || !iin || !volte_info || !mkt_info || !lab_info)
        {
            res = -1;
            break;
        }

        *mbn_file_name = malloc(QCRIL_MBN_HW_STR_MAX_SIZE * sizeof(char));

        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;
            strlcpy(iin_7_digit, iin, sizeof(iin_7_digit));
            strlcpy(iin_6_digit, iin, sizeof(iin_6_digit));

            if (strlen(volte_info) || strlen(mkt_info) || strlen(lab_info))
            {
                // Query base on 7-digit_iin + volte_info + mkt_info
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_four_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_IIN",
                         iin_7_digit,
                         "VOLTE_INFO",
                         volte_info,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            // Query base on 7-digit_iin only
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_three_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_IIN",
                         iin_7_digit,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            if (!strlen(*mbn_file_name))
            {
                if (strlen(volte_info) || strlen(mkt_info))
                {
                // Query base on 6-digit_iin + volte_info + mkt_info
                    snprintf(query_stmt, sizeof(query_stmt),
                             qcril_db_query_mbn_file_stmt_four_args,
                             "FILE",
                             qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                             "MCFG_IIN",
                             iin_6_digit,
                             "VOLTE_INFO",
                             volte_info,
                             "MKT_INFO",
                             mkt_info,
                             "LAB_INFO",
                             lab_info);
                    qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                            (unsigned char *)*mbn_file_name);
                }

                // Query base on 6-digit_iin only
                if (!strlen(*mbn_file_name))
                {
                    snprintf(query_stmt, sizeof(query_stmt),
                             qcril_db_query_mbn_file_stmt_three_args,
                             "FILE",
                             qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                             "MCFG_IIN",
                             iin_6_digit,
                             "MKT_INFO",
                             mkt_info,
                             "LAB_INFO",
                             lab_info);
                    qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                            (unsigned char *)*mbn_file_name);
                }
            }

            // Query based on wild
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_sw_mbn_str_stmt,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_IIN",
                         "wild");
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
    const char    *lab_info
)
{
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    char    iin_11_digit[IIN_11_LEN] = {0};

    int res = 0;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        QCRIL_LOG_INFO("long_iin %s", long_iin? long_iin: "null");
        QCRIL_LOG_INFO("volte_info %s", volte_info? volte_info: "null");
        QCRIL_LOG_INFO("mkt_info %s", mkt_info? mkt_info: "null");
        QCRIL_LOG_INFO("lab_info %s", lab_info? lab_info: "null");

        if (!mbn_file_name || !long_iin || !volte_info || !mkt_info || !lab_info)
        {
            res = -1;
            break;
        }
        *mbn_file_name = malloc(QCRIL_MBN_HW_STR_MAX_SIZE * sizeof(char));

        if (*mbn_file_name)
        {
            *mbn_file_name[0] = 0;
            strlcpy(iin_11_digit, long_iin, sizeof(iin_11_digit));

            if (strlen(volte_info) || strlen(mkt_info) || strlen(lab_info))
            {
                // Query base on long_iin + volte_info + mkt_info + lab_info
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_four_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_LONG_IIN",
                         iin_11_digit,
                         "VOLTE_INFO",
                         volte_info,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);
            }

            // Query base on long_iin only (mkt_info and lab_info will always have value in DB)
            if (!strlen(*mbn_file_name))
            {
                snprintf(query_stmt, sizeof(query_stmt),
                         qcril_db_query_mbn_file_stmt_three_args,
                         "FILE",
                         qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                         "MCFG_LONG_IIN",
                         iin_11_digit,
                         "MKT_INFO",
                         mkt_info,
                         "LAB_INFO",
                         lab_info);
                qcril_db_query_mbn_file(query_stmt, qcril_db_query_mbn_string_callback,
                                        (unsigned char *)*mbn_file_name);

            }
            QCRIL_LOG_INFO("mbn_file_name %s", *mbn_file_name);
        }

    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}
/*===========================================================================

  FUNCTION  qcril_db_insert_using_stmt

===========================================================================*/
/*!
    @brief
    Insert value into DB using insert statement

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
int qcril_db_insert_using_stmt(const char* insert_stmt)
{
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();
    do
    {
        if (!insert_stmt)
        {
            QCRIL_LOG_ERROR("Invalid insert_stmt");
            break;
        }

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);
        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                  insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        res = E_SUCCESS;
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    QCRIL_LOG_INFO(" iin: %s", iin? iin: "null");
    QCRIL_LOG_INFO(" volte_info: %s", volte_info? volte_info: "null");
    QCRIL_LOG_INFO(" mkt_info: %s", mkt_info? mkt_info: "null");
    QCRIL_LOG_INFO(" lab_info: %s", lab_info? lab_info: "null");
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!file || !iin || !volte_info || !mkt_info || !lab_info)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_sw_mbn_iin_stmt,
                 qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                 file, iin, volte_info, mkt_info, lab_info);


        res = qcril_db_insert_using_stmt(insert_stmt);
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null");
    QCRIL_LOG_INFO(" long iin: %s", long_iin? long_iin: "null");
    QCRIL_LOG_INFO(" volte_info: %s", volte_info? volte_info: "null");
    QCRIL_LOG_INFO(" mkt_info: %s", mkt_info? mkt_info: "null");
    QCRIL_LOG_INFO(" lab_info: %s", lab_info? lab_info: "null");
    do {

        if (!file || !long_iin || !volte_info || !mkt_info || !lab_info)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_sw_mbn_long_iin_stmt,
                 qcril_db_tables[QCRIL_DB_SW_MBN_IIN_TYPE].table_name,
                 file, long_iin, volte_info, mkt_info, lab_info);

    } while (0);

    res = qcril_db_insert_using_stmt(insert_stmt);
    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" file: %s", file? file: "null")
    QCRIL_LOG_INFO(" mcc: %s", mcc? mcc: "null")
    QCRIL_LOG_INFO(" mnc: %s", mnc? mnc: "null")
    QCRIL_LOG_INFO(" volte_info: %s", volte_info? volte_info: "null");
    QCRIL_LOG_INFO(" mkt_info: %s", mkt_info? mkt_info: "null");
    QCRIL_LOG_INFO(" lab_info: %s", lab_info? lab_info: "null");
    do {

        if (!file || !mcc || !mnc || !volte_info || !mkt_info || !lab_info)
        {
            break;
        }

        snprintf(insert_stmt, sizeof(insert_stmt),
                 qcril_db_insert_sw_mbn_mcc_mnc_stmt,
                 qcril_db_tables[QCRIL_DB_SW_MBN_MCC_MNC_TYPE].table_name,
                 file, mcc, mnc, volte_info, mkt_info, lab_info);

        res = qcril_db_insert_using_stmt(insert_stmt);
    } while (0);

    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_query_sig_config_callback

===========================================================================*/
/*!
    @brief
    Call back for querying sig config

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_query_sig_config_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);
    if (data)
    {
        if (argc == 1 && argv[0])
        {
            *(uint16_t*)data = atoi(argv[0]);
            QCRIL_LOG_DEBUG(" data %d", *(uint16_t*)data);
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}

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
    const char  *sig_config_type,
    uint16_t    *delta
)
{
    int     res     = 0;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            res = -1;
            break;
        }
        if (!sig_config_type || !delta)
        {
            res = -1;
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_sig_config_stmt,
                 qcril_db_tables[QCRIL_DB_SIG_CONFIG_TYPE].table_name,
                 sig_config_type);

        QCRIL_LOG_INFO(" Query: %s", query_stmt);

        if (SQLITE_OK != (res = qcril_db_sqlite3_exec(
                                             query_stmt,
                                             qcril_db_query_sig_config_callback,
                                             delta)))
        {
            QCRIL_LOG_ERROR("Could not query %d", res);
        }

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

/***************** All manual provisioning related functions start *********/

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s user_pref - %d", iccid? iccid: "null", user_pref)
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_new_iccid_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 iccid,
                 user_pref);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                   insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
        res = E_SUCCESS;

    } while (0);

    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s user_pref - %d", iccid? iccid: "null", user_pref)
    do {
        if ( !qcril_db_handle )
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_update_user_pref_str_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 user_pref,
                 iccid);

        qcril_db_recovery(QCRIL_DB_RECOVERY_CHECK_AND_RESTORE);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = qcril_db_sqlite3_exec(
                                     insert_stmt, NULL, NULL)))
        {
            QCRIL_LOG_ERROR("Could not insert %d", ret);
            break;
        }

        qcril_db_recovery(QCRIL_DB_RECOVERY_BACKUP);
        res = E_SUCCESS;

    } while (0);

    return res;
}

/*===========================================================================

  FUNCTION  qcril_db_table_query_callback_integer

===========================================================================*/
/*!
    @brief
    Call back for querying user preference

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
)
{
    int ret = 0;

    QCRIL_NOTUSED(azColName);

    if (data)
    {
        if (argc == 1 && argv[0])
        {
            *(int*)data = atoi(argv[0]);
            QCRIL_LOG_DEBUG(" data %d", *(int*)data);
        }
        else
        {
            QCRIL_LOG_DEBUG(" argc: %d, argv[0]: %p", argc, argv[0]);
        }
    }

    return ret;
}

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
)
{
    int     res     = E_FAILURE;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!iccid || !user_pref)
        {
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_user_pref_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 iccid);

        QCRIL_LOG_INFO(" Query: %s", query_stmt);

        if(SQLITE_OK != (res = qcril_db_sqlite3_exec(
                                             query_stmt,
                                             qcril_db_table_query_callback_integer,
                                             user_pref)))
        {
            QCRIL_LOG_ERROR("Could not insert after restore %d", res);
            break;
        }
        res = E_SUCCESS;

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s mnc: %s carrier_pref - %d",
                            mcc? mcc: "null",
                            mnc? mnc: "null", carrier_pref);
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !mcc || !mnc )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_mbn_imsi_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE].table_name,
                 mcc, mnc, carrier_pref);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }

            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" mcc: %s mnc: %s carrier_pref - %d",
                            mcc? mcc: "null",
                            mnc? mnc: "null", carrier_pref);
    do {
        if ( !qcril_db_handle )
        {
            break;
        }
        if ( !mcc || !mnc )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_update_mbn_imsi_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 carrier_pref, mcc, mnc);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

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
)
{
    int     res     = E_FAILURE;
    char   *zErrMsg = NULL;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!mcc || !mnc || !carrier_pref)
        {
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_carrier_pref_from_mbn_imsi_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE].table_name,
                 mcc, mnc);

        QCRIL_LOG_INFO(" Query: %s", query_stmt);

        if (SQLITE_OK != (res = sqlite3_exec(qcril_db_handle,
                                             query_stmt,
                                             qcril_db_table_query_callback_integer,
                                             carrier_pref,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not query %d %s", res, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }
        res = E_SUCCESS;

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s carrier_pref - %d",
                            iccid? iccid: "null", carrier_pref);
    do {
        if (!qcril_db_handle)
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_insert_mbn_iccid_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_IMSI_EXCEPTION_TYPE].table_name,
                 iccid, carrier_pref);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }

            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

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
)
{
    char    insert_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};
    int     res     = E_FAILURE;
    int     ret     = SQLITE_OK;
    char   *zErrMsg = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_LOG_INFO(" iccid: %s carrier_pref - %d",
                            iccid? iccid: "null", carrier_pref);
    do {
        if ( !qcril_db_handle )
        {
            break;
        }
        if ( !iccid )
        {
            break;
        }

        snprintf(insert_stmt,
                 sizeof(insert_stmt),
                 qcril_db_update_mbn_iccid_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MANUAL_PROV_TYPE].table_name,
                 carrier_pref, iccid);

        QCRIL_LOG_INFO(" insert stmt: %s", insert_stmt);

        if (SQLITE_OK != (ret = sqlite3_exec(qcril_db_handle,
                                             insert_stmt,
                                             NULL,
                                             NULL,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not insert %d %s", ret, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }

        res = E_SUCCESS;

    } while (0);

    return res;
}

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
)
{
    int     res     = E_FAILURE;
    char   *zErrMsg = NULL;
    char    query_stmt[QCRIL_DB_MAX_STMT_LEN] = {0};

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        if (!qcril_db_handle)
        {
            break;
        }
        if (!iccid || !carrier_pref)
        {
            break;
        }

        snprintf(query_stmt, sizeof(query_stmt),
                 qcril_db_query_carrier_pref_from_mbn_iccid_exception_stmt,
                 qcril_db_tables[QCRIL_DB_MBN_ICCID_EXCEPTION_TYPE].table_name,
                 iccid);

        QCRIL_LOG_INFO(" Query: %s", query_stmt);

        if (SQLITE_OK != (res = sqlite3_exec(qcril_db_handle,
                                             query_stmt,
                                             qcril_db_table_query_callback_integer,
                                             carrier_pref,
                                             &zErrMsg)))
        {
            if (zErrMsg)
            {
                QCRIL_LOG_ERROR("Could not query %d %s", res, zErrMsg);
                sqlite3_free(zErrMsg);
            }
            break;
        }
        res = E_SUCCESS;

    } while (0);


    QCRIL_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}


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

int qcril_db_reset_cleanup()
{
  int ret = SQLITE_ERROR;

  if (qcril_db_handle)
    ret = sqlite3_close(qcril_db_handle);
  sqlite3_shutdown();
  qcril_db_emergency_numbers_escv_type = 0;
  qcril_db_query_result = 0;


  if (ret != SQLITE_OK)
  {
    return -1;
  }

  return 0;

}

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
)
{
  char *zErrMsg = NULL;
  int rc = SQLITE_OK;

  QCRIL_LOG_FUNC_ENTRY();

  if (SQLITE_OK != (rc = sqlite3_exec(qcril_db_handle,
                                       sql_stmt, callback,
                                       arg, &zErrMsg)))
  {
    if (zErrMsg)
    {
      QCRIL_LOG_ERROR("sql_stmt %s", sql_stmt);
      QCRIL_LOG_ERROR("Operation failed %d %s", rc, zErrMsg);
      sqlite3_free(zErrMsg);
    }

    if(rc == SQLITE_CORRUPT)
    {
      qcril_db_recovery(QCRIL_DB_RECOVERY_RESTORE);
      if (SQLITE_OK != (rc = sqlite3_exec(qcril_db_handle,
                                           sql_stmt, callback,
                                           arg, &zErrMsg)))
      {
        if (zErrMsg)
        {
          QCRIL_LOG_ERROR("sql_stmt %s", sql_stmt);
          QCRIL_LOG_ERROR("Operation failed after restore %d %s", rc, zErrMsg);
          sqlite3_free(zErrMsg);
        }
      }
    }
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
  return rc;
}

/*===========================================================================

  FUNCTION  qcril_db_check_integrity_callback

===========================================================================*/
/*!
    @brief
    checks integrity of db callback.

    @return
    0 if function is successful.
*/
/*=========================================================================*/
static int qcril_db_check_integrity_callback
(
    void   *data,
    int     argc,
    char  **argv,
    char  **azColName
)
{
    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED(azColName);

    QCRIL_LOG_INFO("argc %d argv[0] %s", argc, argv[0] ? argv[0] : "null");
    if ((argc > 0) && argv[0] && strlen(argv[0]) > 0 && data)
    {
        if((argc == 1) && (strncmp(argv[0], "ok", strlen(argv[0])) == 0))
        {
            /* DB passed integrity check */
            *(int*)data = TRUE;
        }
        else
        {
            *(int*)data = FALSE;
        }
    }
    QCRIL_LOG_FUNC_RETURN();

    return 0;
}

/*===========================================================================

  FUNCTION  qcril_db_check_integrity

===========================================================================*/
/*!
    @brief
    Check and return if DB passed Inegrity check or not.

    @return
    TRUE(1) if Inegrity check is pass
    FALSE(0) else
*/
/*=========================================================================*/

int qcril_db_check_integrity()
{
    char *zErrMsg = NULL;
    int rc = SQLITE_OK;
    int integrity_check = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    if (SQLITE_OK != (rc = sqlite3_exec(qcril_db_handle,
                                         qcril_db_check_integrity_stmt,
                                         qcril_db_check_integrity_callback,
                                         &integrity_check, &zErrMsg)))
    {
      if (zErrMsg)
      {
        QCRIL_LOG_ERROR("sql_stmt %s", qcril_db_check_integrity_stmt);
        QCRIL_LOG_ERROR("Operation failed %d %s", rc, zErrMsg);
        sqlite3_free(zErrMsg);
      }
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(integrity_check);
    return integrity_check;

}

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

int qcril_db_recovery(qcril_db_recovery_type isRecovery)
{
  int rc = SQLITE_OK;
  int valid_integrity = TRUE;
  sqlite3_backup *qcril_backup_handle = NULL;
  sqlite3 *qcril_src_handle = NULL;
  sqlite3 *qcril_dst_handle = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    switch (isRecovery)
    {
      case QCRIL_DB_RECOVERY_BACKUP:
        QCRIL_LOG_DEBUG("Backup QCRIL database");
        /* First check if the current database is not corrupt */
        valid_integrity = qcril_db_check_integrity();
        QCRIL_LOG_DEBUG("Integrity of current DB: %d", valid_integrity);
        if(TRUE != valid_integrity)
        {
            /* Do not perform backup */
            rc = SQLITE_CORRUPT;
            QCRIL_LOG_DEBUG("Skip backing up of corrupt DB");
            break;
        }
        rc = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                             &qcril_src_handle,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        rc = sqlite3_open_v2(QCRIL_BACKUP_DATABASE_NAME,
                             &qcril_dst_handle,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        break;

      case QCRIL_DB_RECOVERY_RESTORE:
        QCRIL_LOG_DEBUG("Restore QCRIL database");
        rc = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                             &qcril_dst_handle,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        rc = sqlite3_open_v2(QCRIL_BACKUP_DATABASE_NAME,
                             &qcril_src_handle,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        break;

      case QCRIL_DB_RECOVERY_CHECK_AND_RESTORE:
        QCRIL_LOG_DEBUG("Check Integrity and Restore QCRIL database");
        valid_integrity = qcril_db_check_integrity();
        QCRIL_LOG_DEBUG("Integrity of current DB: %d", valid_integrity);
        if(TRUE != valid_integrity)
        {
          /* Perform restore */
          rc = sqlite3_open_v2(QCRIL_DATABASE_NAME,
                               &qcril_dst_handle,
                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
          if(rc != SQLITE_OK)
          {
            QCRIL_LOG_DEBUG("Failed to open qcril.db: %d", rc);
          }
          else
          {
            rc = sqlite3_open_v2(QCRIL_BACKUP_DATABASE_NAME,
                                 &qcril_src_handle,
                                 SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
          }
        }
        break;

      default:
        QCRIL_LOG_DEBUG("Invalid otion for recovery: %d", isRecovery);
        rc = SQLITE_ERROR;
        break;
    }

    if(rc != SQLITE_OK || !qcril_src_handle || !qcril_dst_handle) break;

    qcril_backup_handle = sqlite3_backup_init(qcril_dst_handle, "main",
                                              qcril_src_handle, "main");
    if( qcril_backup_handle )
    {
      (void)sqlite3_backup_step(qcril_backup_handle, -1);
      (void)sqlite3_backup_finish(qcril_backup_handle);
    }
    rc = sqlite3_errcode(qcril_dst_handle);
  } while(FALSE);

  if(TRUE != valid_integrity && isRecovery == QCRIL_DB_RECOVERY_BACKUP)
  {
    /* Backup was requested but current db is corrupt. So instead RECOVER the DB */
    QCRIL_LOG_DEBUG("Trigger Restore instead of Backup");
    qcril_db_recovery(QCRIL_DB_RECOVERY_RESTORE);
  }

  /* Close QCRIL database */
  if(qcril_src_handle)
  {
    (void)sqlite3_close(qcril_src_handle);
  }
  if(qcril_dst_handle)
  {
    (void)sqlite3_close(qcril_dst_handle);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(rc);
  return rc;
}
