/******************************************************************************
  @file    lmDBMacros.h
  @brief   Implementation of Learning Module Database

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef LMDB_MACROS_H_
#define LMDB_MACROS_H_

#define LMDB_ENABLE_WAL_MODE


#define LMDB_ROOT_PATH "/data/vendor/lm/"
#define LMDB_INMEMORY_FILENAME ":memory:"

#define LMDB_NULLSTRING ""
#define LMDB_SPACE      " "
#define LMDB_COMMA      ","

#define SQLROWID    rowid
#define INTCOL1     intcol1
#define APPNAME     appName
#define RSVNAME     rsvdName
#define PAYLOAD     blob


//column index
enum q_c_tbl_col_sel_index {
    COL0_SEL_INDEX,
};

//column index
enum q_c_tbl_col_bind_index {
    COL0_BIND_INDEX,
    COL1_BIND_INDEX,
};


/************ SQLITE specific defines **************/
#define LMDB_SQL_DB_HDLR        sqlite3
#define LMDB_SQL_STMT           sqlite3_stmt

#define LMDB_SQL_PREFIX_FN(fn)  sqlite3_##fn
#define LMDB_SQL_ERR_MSG_FN     sqlite3_errmsg
#define LMDB_SQL_EXEC_FN        sqlite3_exec
#define LMDB_SQL_OPEN_FN        sqlite3_open_v2
#define LMDB_SQL_CLOSE_FN       sqlite3_close
#define LMDB_SQL_CONFIG_FN      sqlite3_config
#define LMDB_SQL_INIT_FN        sqlite3_initialize
#define LMDB_SQL_SHTDWN_FN      sqlite3_shutdown
#define LMDB_SQL_PREPARE_FN     sqlite3_prepare_v2
#define LMDB_SQL_ROW_MOD_FN     sqlite3_changes

#define LMDB_SQL_OK                 SQLITE_OK
#define LMDB_SQL_DONE               SQLITE_DONE
#define LMDB_SQL_ROW                SQLITE_ROW
#define LMDB_SQL_SMC_NOFREE         SQLITE_STATIC
#define LMDB_SQL_CONFIG_SERIALIZED  SQLITE_CONFIG_SERIALIZED
#define LMDB_SQL_CONFIG_LOG         SQLITE_CONFIG_LOG

#define LMDB_SQL_OPEN_RW            SQLITE_OPEN_READWRITE
#define LMDB_SQL_OPEN_CR            SQLITE_OPEN_CREATE
/***************************************************/

#define ORDER_BY_HDR " ORDER BY "

#define Q_C_TBL_HDR "CREATE TABLE IF NOT EXISTS "
#define Q_C_TBL_COLUMNS (INTCOL1 INTEGER, \
        APPNAME TEXT NOT null, \
        RSVNAME TEXT, \
        PAYLOAD BLOB)

#define Q_TOTAL_ROW_COL_FLD (INTCOL1, \
        APPNAME, \
        RSVNAME, \
        PAYLOAD)
#define Q_TOTAL_ROW_VAL_FLD (?,?,?,?)

#define Q_I_VAL_HDR "INSERT INTO "
#define Q_I_VAL_FLD Q_TOTAL_ROW_COL_FLD VALUES Q_TOTAL_ROW_VAL_FLD

#define Q_S_VAL_HDR "SELECT *, rowid FROM "

#define Q_S_DA_VAL_HDR "SELECT DISTINCT "
#define Q_SC_DA_VAL_HDR "SELECT COUNT(DISTINCT "

#define Q_SC_VAL_HDR "SELECT COUNT(*) FROM "

#define Q_D_VAL_HDR "DELETE FROM "

#define Q_DR_VAL_HDR Q_D_VAL_HDR
#define Q_DR_VAL_CLAUSE WHERE (SQLROWID) = (?)

#define Q_U_VAL_HDR "UPDATE "
#define Q_U_VAL_FLD SET Q_TOTAL_ROW_COL_FLD = Q_TOTAL_ROW_VAL_FLD Q_DR_VAL_CLAUSE

#define LMDB_SQL_CHK(dbtname, rvar, conn, stmt, rval) \
        if (rval != (rvar=stmt)) { \
            LMDB_DEBUG_SQL_ERR("sql:ERROR:Dbt=%s info=%s\n", dbtname, LMDB_SQL_ERR_MSG_FN(conn)); \
        }

#define LMDB_SQL_CHK_RTN_ERR(dbtname, rvar, conn, stmt, rval) \
        if (rval != (rvar=stmt)) { \
            LMDB_DEBUG_SQL_ERR("ERROR:Dbt=%s info=%s\n", dbtname, LMDB_SQL_ERR_MSG_FN(conn)); \
            if (rval != rvar) {\
                return LMDB_SQL_ERROR; \
            }\
        }

#define LMDB_SQL_CHK_OK(dbtname, rvar, conn, stmt) LMDB_SQL_CHK(dbtname, rvar, conn, stmt, LMDB_SQL_OK)
#define LMDB_SQL_EXEC_STMT_CHK_OK(dbtname, rvar, conn, query, ...)  \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Q=%s\n", dbtname, query); \
        LMDB_SQL_CHK_OK(dbtname, rvar, conn, LMDB_SQL_EXEC_FN(conn, query, __VA_ARGS__))

#define LMDB_SQL_CHK_OK_RTN_ERR(dbtname, rvar, conn, stmt) LMDB_SQL_CHK_RTN_ERR(dbtname, rvar, conn, stmt, LMDB_SQL_OK)
#define LMDB_SQL_EXEC_STMT_CHK_OK_OR_RTN_ERR(dbtname, rvar, conn, query, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Q=%s\n", dbtname, query); \
        LMDB_SQL_CHK_OK_RTN_ERR(dbtname, rvar, conn, LMDB_SQL_EXEC_FN(conn, query, __VA_ARGS__))


#define LMDB_SQL_PREPARE_STMT_CHK_OK(dbtname, rvar, conn, query, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Q=%s\n", dbtname, query); \
        LMDB_SQL_CHK_OK(dbtname, rvar, conn, LMDB_SQL_PREPARE_FN(conn, query, __VA_ARGS__))
#define LMDB_SQL_PREPARE_STMT_CHK_OK_RTN_ERR(dbtname, rvar, conn, query, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Q=%s\n", dbtname, query); \
        LMDB_SQL_CHK_OK_RTN_ERR(dbtname, rvar, conn, LMDB_SQL_PREPARE_FN(conn, query, __VA_ARGS__))


#define LMDB_SQL_PRAGMA_JOURNAL_MODE "PRAGMA journal_mode=WAL"
#define LMDB_SQL_BEGIN_IMM_TXN_STMT "BEGIN IMMEDIATE TRANSACTION;"
#define LMDB_SQL_COMMIT_TXN_STMT "COMMIT TRANSACTION;"
#define LMDB_SQL_ROLLBACK_TXN_STMT "ROLLBACK TRANSACTION;"


#define LMDB_SQL_TXN_CHK(dbtname, rvar, conn, stmt, rval) \
        if (rval != (rvar=stmt)) { \
            LMDB_DEBUG_SQL_ERR("sql:ERROR:Dbt=%s info=%s\n", dbtname, LMDB_SQL_ERR_MSG_FN(conn)); \
            LMDB_SQL_EXEC_STMT_CHK_OK(dbtname, rvar, conn, LMDB_SQL_ROLLBACK_TXN_STMT, NULL, NULL, NULL); \
        }
#define LMDB_SQL_TXN_CHK_OK(dbtname, rvar, conn, stmt) LMDB_SQL_TXN_CHK(dbtname, rvar, conn, stmt, LMDB_SQL_OK)
#define LMDB_SQL_EXEC_TXN_CHK_OK(dbtname, rvar, conn, query, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Q=%s\n", dbtname, query); \
        LMDB_SQL_TXN_CHK_OK(dbtname, rvar, conn, LMDB_SQL_EXEC_FN(conn, query, __VA_ARGS__))


#define LMDB_SQL_STEP_PSTMT(dbtname, rvar, stmt, rval) \
        if (rval != (rvar=stmt)) { \
            LMDB_DEBUG_SQL_ERR("sql:ERROR:Dbt=%s info=%d\n", dbtname, rvar); \
        }
#define LMDB_SQL_STEP_PSTMT_NOCHECK(rvar, stmt) (rvar=stmt)

#define LMDB_SQL_STEP_PSTMT_OK(dbtname, rvar, stmt) LMDB_SQL_STEP_PSTMT(dbtname, rvar, stmt, LMDB_SQL_OK)
#define LMDB_SQL_STEP_PSTMT_DONE(dbtname, rvar, stmt) LMDB_SQL_STEP_PSTMT(dbtname, rvar, stmt, LMDB_SQL_DONE)

#define LMDB_SQL_STEP_PSTMT_CHK_OK(dbtname, rvar, operation_datatype, stmt_handle, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Optyp=%s\n", dbtname, #operation_datatype); \
        LMDB_SQL_STEP_PSTMT_OK(dbtname, rvar, LMDB_SQL_PREFIX_FN(operation_datatype)(stmt_handle, ##__VA_ARGS__))
#define LMDB_SQL_STEP_PSTMT_CHK_DONE(dbtname, rvar, operation_datatype, stmt_handle, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Optyp=%s\n", dbtname, #operation_datatype); \
        LMDB_SQL_STEP_PSTMT_DONE(dbtname, rvar, LMDB_SQL_PREFIX_FN(operation_datatype)(stmt_handle, ##__VA_ARGS__))
#define LMDB_SQL_STEP_PSTMT_CHK_ROW(dbtname, rvar, operation_datatype, stmt_handle, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Optyp=%s\n", dbtname, #operation_datatype); \
        LMDB_SQL_STEP_PSTMT_NOCHECK(rvar, LMDB_SQL_PREFIX_FN(operation_datatype)(stmt_handle, ##__VA_ARGS__))


#define LMDB_SQL_STEP_COLFN_NOCHECK(dbtname, rvar, operation_datatype, stmt_handle, ...) \
        LMDB_DEBUG_SQL_DEBUG("sql:Dbt=%s Optyp=%s\n", dbtname, #operation_datatype); \
        LMDB_SQL_STEP_PSTMT_NOCHECK(rvar, LMDB_SQL_PREFIX_FN(operation_datatype)(stmt_handle, ##__VA_ARGS__))


#define STRINGIFY(...)     #__VA_ARGS__
#define LMDB_STRINGIFY(...) STRINGIFY(__VA_ARGS__)


#endif /* LMDB_MACROS_H_ */
