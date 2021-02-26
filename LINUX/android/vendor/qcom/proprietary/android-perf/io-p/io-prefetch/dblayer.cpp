/******************************************************************************
  @file    dblayer.c
  @brief   Source file to database of files

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

/******************************************************************************
     pkg_file_tbl               pkg_tbl
  |-----------------|      |-----------------|
  |  pkg_name       |      |  pkg_name       |
  |  file_name      |      |-----------------|
  |-----------------|      | pkg_use_time    |
  |                 |      | num_of_launches |
  | file_use_ctr    |      |-----------------|
  | file_time_stamp |
  | file_size       |
  | mark_for_delete |
  | file_modify_time|
  | file_iop_size   |
  | study_finish    |
  | mincore_array   |
  | cache_dropped   |
  | disabled        |
  |-----------------|
******************************************************************************/

/******************************************************************************
      ux_pkg_tbl                 ux_lat_tbl
  |--------------------|        |-----------------|
  |  pkg_name          |        |    pkg_name     |
  |  week_day          |        |-----------------|
  |--------------------|        |   bindApp_dur   |
  | pkg_last_use       |        |   disAct_dur    |
  | num_of_launches    |        |  wakeLock_dur   |
  | num_of_sublaunches |        |    memory_st    |
  | timeslot_1_count   |        |    bindApp_ct   |
  | timeslot_2_count   |        | predict_success |
  | timeslot_3_count   |        |   predict_fail  |
  | timeslot_4_count   |        |    not_game     |
  |--------------------|        |-----------------|
******************************************************************************/

#define ATRACE_TAG ATRACE_TAG_ALWAYS
#include <cutils/trace.h>

#include "dblayer.h"
#include "string.h"
#include <cutils/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "sqlite3.h"
#include "maintain_db.h"

#undef LOG_TAG
#define LOG_TAG           "ANDR-IOP_DB"

#if QC_DEBUG
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#define QLOGE(...)
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#endif

#define _SQLITE_CHECK(stmt, rt) \
    if (rt != stmt) { \
        QLOGE("\n %s:%d: reason:%s", __FILE__, __LINE__, sqlite3_errmsg(db_conn)); \
        QLOGE(" \n in error code \n");\
        sem_post(&mutex);\
        return 0; \
    }

#define _SQLITE_TRANSACTION_CHECK(stmt, rt) \
    if (rt != stmt) { \
        QLOGE("\n %s:%d: reason:%s", __FILE__, __LINE__, sqlite3_errmsg(db_conn)); \
        sqlite3_exec(db_conn, "ROLLBACK TRANSACTION;", NULL, NULL, NULL);\
        sem_post(&mutex);\
        return 0; \
    }

#define _SQLITE_CHECK_DONE(stmt) _SQLITE_CHECK(stmt, SQLITE_DONE)
#define _SQLITE_CHECK_OK(stmt) _SQLITE_CHECK(stmt, SQLITE_OK)
#define _SQLITE_CHECK_TRANSACTION_OK(stmt) _SQLITE_TRANSACTION_CHECK(stmt, SQLITE_OK)

/* database queries */

/* IOP Queries */
#define IO_PREFETCHER_QUERY_CREATE_TBL_FILE_LIST "CREATE TABLE IF NOT EXISTS io_pkg_file_tbl( \
pkg_name TEXT , \
file_name TEXT,\
file_use_counter INTEGER,\
file_time_stamp INTEGER,\
file_size INTEGER,\
mark_for_delete INTEGER,\
file_modify_time INTEGER, \
file_iop_size INTEGER, \
study_finish INTEGER, \
mincore_array blob, \
cache_dropped INTEGER, \
disabled INTEGER, \
PRIMARY KEY(pkg_name,file_name)\
)"

#define IO_PREFETCHER_QUERY_CREATE_TBL_PKG_LIST  "CREATE TABLE IF NOT EXISTS io_pkg_tbl( \
 pkg_name TEXT PRIMARY KEY, \
 pkg_last_use DATETIME, \
 pkg_use_count INTEGER \
 )"

#define IO_PREFETCHER_QUERY_REMOVE_PACKAGE_PKG_TBL   "DELETE from io_pkg_tbl WHERE pkg_name = '%s';\
 DELETE from io_pkg_file_tbl\
 WHERE pkg_name = '%s'"

#define IO_PREFETCHER_QUERY_REMOVE_PACKAGE_FILE_TBL  "DELETE from io_pkg_file_tbl \
 WHERE pkg_name = '%s' AND \
 file_name = '%s'"

#define IO_PREFETCHER_QUERY_MARK_FOR_DELETE      "UPDATE io_pkg_file_tbl \
  SET mark_for_delete = 1\
  WHERE pkg_name = '%s'\
  AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_MARK_FOR_DISABLE      "UPDATE io_pkg_file_tbl \
  SET disabled = 1\
  WHERE pkg_name = '%s'\
  AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_MARK_FOR_RESTUDY      "UPDATE io_pkg_file_tbl \
  SET study_finish = 0\
  WHERE pkg_name = '%s'\
  AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_MARK_CACHE_DROPPED "UPDATE io_pkg_file_tbl \
  SET cache_dropped=%d \
  WHERE pkg_name='%s' AND file_name='%s'"

#define IO_PREFETCHER_QUERY_MARK_PKG_FOR_DELETE      "UPDATE io_pkg_file_tbl \
  SET mark_for_delete = 1\
  WHERE pkg_name = '%s'"

#define IO_PREFETCHER_QUERY_DELETE_MARK_FILE     "DELETE from io_pkg_file_tbl \
WHERE mark_for_delete = 1"

#define IO_PREFETCHER_QUERY_TOTAL_PKG            "SELECT COUNT(pkg_name) \
AS num_of_pkg \
FROM io_pkg_tbl"

#define IO_PREFETCHER_QUERY_TOTAL_FILE "SELECT COUNT(file_name) \
 AS num_of_file \
 FROM io_pkg_file_tbl \
 WHERE pkg_name = '%s'"

#define IO_PREFETCHER_QUERY_GET_FILE_LIST "SELECT * FROM io_pkg_file_tbl \
  WHERE pkg_name = '%s' \
  ORDER BY file_time_stamp ASC"

#define IO_PREFETCHER_QUERY_GET_PKG_LIST "SELECT * FROM io_pkg_tbl ORDER BY pkg_last_use"

#define IO_PREFETCHER_QUERY_GET_FILE "select * from io_pkg_file_tbl where pkg_name='%s'\
  AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_UPDATE_FILE_DETAILS  "UPDATE io_pkg_file_tbl \
 SET \
 file_use_counter = file_use_counter+1\
 ,file_time_stamp = %d\
 ,file_size = %d\
 ,mincore_array = :mincore_array\
 ,study_finish = study_finish+1\
 ,file_iop_size = %d\
 WHERE pkg_name = '%s' AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_UPDATE_FILE_MODIFIED  "UPDATE io_pkg_file_tbl \
 SET \
 file_modify_time = %d\
 ,file_size = %d\
 ,study_finish = 0\
 ,cache_dropped = 1\
 WHERE pkg_name = '%s' AND (file_name = '%s')"

#define IO_PREFETCHER_QUERY_INSER_FILE "INSERT INTO io_pkg_file_tbl (pkg_name,file_name\
 ,file_use_counter\
 ,file_time_stamp\
 ,file_size\
 ,mark_for_delete\
 ,file_modify_time\
 ,file_iop_size\
 ,study_finish\
 ,mincore_array\
 ,cache_dropped\
 ,disabled)\
 VALUES ('%s','%s',1,%d,%d,0,%d,%d,0, :mincore_array,0,0)"

#define IO_PREFETCHER_QUERY_GET_PKG "SELECT * FROM io_pkg_tbl WHERE pkg_name='%s'"

#define IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS  "UPDATE io_pkg_tbl \
 SET pkg_last_use = %lu\
 ,pkg_use_count = pkg_use_count+1 \
 WHERE pkg_name = '%s'"

#define IO_PREFETCHER_QUERY_INSERT_PKG "INSERT INTO io_pkg_tbl (pkg_name,\
  pkg_last_use,\
  pkg_use_count)\
  VALUES ('%s',%lu,1)"

/* UXEngine Queries */

/* UXEngine Latency Table */
#define UX_LATENCY_QUERY_CREATE_TBL_PKG_LIST  "CREATE TABLE IF NOT EXISTS ux_lat_tbl( \
 pkg_name TEXT PRIMARY KEY, \
 bindApp_dur INTEGER, \
 disAct_dur INTEGER, \
 memory_st INTEGER, \
 bindApp_ct INTEGER, \
 predict_success INTEGER, \
 predict_fail INTEGER, \
 not_game INTEGER \
 )"

#define UX_LATENCY_QUERY_REMOVE_PACKAGE_PKG_TBL   "DELETE from ux_lat_tbl WHERE pkg_name = '%s'"

#define UX_LATENCY_QUERY_UPDATE_PKG_DETAILS  "UPDATE ux_lat_tbl \
SET bindApp_dur = %d\
,disAct_dur = %d \
,memory_st = %d \
,bindApp_ct = bindApp_ct+%d \
WHERE pkg_name = '%s'"

#define UX_LATENCY_QUERY_UPDATE_PRED_DETAILS  "UPDATE ux_lat_tbl \
SET predict_success = predict_success+%d \
,predict_fail = predict_fail+%d \
WHERE pkg_name = '%s'"

#define UX_LATENCY_QUERY_UPDATE_GAME_DETAILS  "UPDATE ux_lat_tbl \
SET not_game = %d \
WHERE pkg_name = '%s'"

#define UX_LATENCY_QUERY_INSERT_PKG "INSERT INTO ux_lat_tbl (pkg_name,\
  bindApp_dur,\
  disAct_dur,\
  memory_st,\
  bindApp_ct,\
  predict_success,\
  predict_fail,\
  not_game)\
  VALUES ('%s',%d,%d,%d,1,0,0,1)"

#define UX_LATENCY_QUERY_GET_LAT "SELECT * FROM ux_lat_tbl WHERE pkg_name='%s'"

#define UX_LATENCY_QUERY_GET_LAT_PRED "SELECT pkg_name,predict_success,predict_fail FROM ux_lat_tbl"

#define UX_LATENCY_QUERY_GET_PKG_COUNT "SELECT SUM(pkg_use_count) FROM ux_pkg_tbl WHERE pkg_name='%s'"

#define UX_LATENCY_QUERY_GET_BINDAPP_COUNT "SELECT bindApp_ct FROM ux_lat_tbl WHERE pkg_name='%s'"

#define UX_LATENCY_QUERY_SET_PKG_UPDATE  "UPDATE ux_lat_tbl \
SET bindApp_ct = 0 \
WHERE pkg_name = '%s'"

/* UXEngine Package Table */
#define UX_ENGINE_QUERY_CREATE_TBL_PKG_LIST  "CREATE TABLE IF NOT EXISTS ux_pkg_tbl( \
 pkg_name TEXT, \
 week_day TEXT, \
 pkg_last_use DATETIME, \
 pkg_use_count INTEGER, \
 pkg_sub_use_count INTEGER, \
 timeslot_count_1 INTEGER, \
 timeslot_count_2 INTEGER, \
 timeslot_count_3 INTEGER, \
 timeslot_count_4 INTEGER, \
 PRIMARY KEY (pkg_name, week_day) \
 )"

#define UX_ENGINE_QUERY_REMOVE_PACKAGE_PKG_TBL   "DELETE from ux_pkg_tbl WHERE pkg_name = '%s' AND week_day = '%s'"

#define UX_ENGINE_QUERY_GET_TOP_PKG_LIST "SELECT pkg_name,pkg_last_use,pkg_use_count,timeslot_count_%d,bindApp_dur,disAct_dur,memory_st,not_game FROM ux_pkg_tbl \
JOIN ux_lat_tbl USING (pkg_name) \
WHERE week_day = '%s' \
ORDER BY timeslot_count_%d DESC, bindApp_dur DESC, pkg_last_use ASC"

#define UX_ENGINE_QUERY_TOTAL_PKG            "SELECT COUNT(pkg_name) \
AS num_of_pkg \
FROM ux_pkg_tbl"

#define UX_ENGINE_QUERY_AVERAGE_LAUNCH_COUNT    "SELECT AVG(pkg_use_count) \
AS num_of_pkg \
FROM ux_pkg_tbl \
WHERE week_day = '%s'"

#define UX_ENGINE_QUERY_UPDATE_PKG_DETAILS  "UPDATE ux_pkg_tbl \
SET pkg_last_use = %lu\
,pkg_use_count = pkg_use_count+1 \
,pkg_sub_use_count = pkg_sub_use_count+%d \
,timeslot_count_%d = timeslot_count_%d+1 \
WHERE pkg_name = '%s' AND week_day = '%s' "

#define UX_ENGINE_QUERY_INSERT_PKG "INSERT INTO ux_pkg_tbl (pkg_name,\
  week_day,\
  pkg_last_use,\
  pkg_use_count,\
  pkg_sub_use_count,\
  timeslot_count_1,\
  timeslot_count_2,\
  timeslot_count_3,\
  timeslot_count_4)\
  VALUES ('%s','%s',%lu,%d,%d,%d,%d,%d,%d)"

#define UX_ENGINE_QUERY_RESET_PKG_DETAILS  "UPDATE ux_pkg_tbl \
SET pkg_last_use = %lu\
,pkg_use_count = %d \
,pkg_sub_use_count = %d \
,timeslot_count_1 = %d \
,timeslot_count_2 = %d \
,timeslot_count_3 = %d \
,timeslot_count_4 = %d \
WHERE pkg_name = '%s' AND week_day = '%s' "

#define UX_ENGINE_QUERY_GET_PKG "SELECT pkg_name,week_day FROM ux_pkg_tbl WHERE pkg_name='%s'"

#define UX_ENGINE_QUERY_GET_PKG_LIST "SELECT * FROM ux_pkg_tbl ORDER BY pkg_use_count DESC"

#define UX_ENGINE_QUERY_UNINSTALL_PKG "DELETE from ux_pkg_tbl WHERE pkg_name = '%s';\
DELETE from ux_lat_tbl WHERE pkg_name = '%s'"

sqlite3 *db_conn = NULL;     // database connection object
sem_t mutex;
char    _dbpath[] = "/data/vendor/iop/io-prefetcher.db"; // database path

/* Private API */
int create_database();
static int open_db();
static int close_database();

//Open database connection
static int open_db()
{
    // don't allow opening up more than one database connection

    if( db_conn != NULL )
    {
      QLOGI( "Already Opened database connection");
      return 1;
    }
    // open the database connection
    int rc = sqlite3_open(_dbpath, &db_conn );
    QLOGI("sqlite3_open db_conn = %p rc = %d \n",db_conn,rc);

    if( db_conn == NULL || rc != SQLITE_OK)
    {
        QLOGI("unable to open database at this path=\"%s\" with sql err_msg=%s"
                     " sql ret code=%d", _dbpath, sqlite3_errmsg(db_conn),rc);

      return 0;
    }
    QLOGI( "db=%s has been opened successful", _dbpath);
    return 1;
}

int create_database()
{
    int fd = -1;

    if(db_conn != NULL)
    {
        // As DB is already init no need to do anything
        return 0;
    }

    fd = open(_dbpath, O_CREAT| O_NOFOLLOW,0600);

    if(fd < 0)
    {
        QLOGI("fail to create database");
        return -1;
    }

    close(fd);

    int conn_status = open_db();
    QLOGI("\n open DB %d \n", conn_status);

    if(conn_status != 1)
    {
        ALOGI("fail to open DB");
        return -1;
    }
    else
    {
        if( db_conn == NULL )
        {
          QLOGI("fail to create DB");
            return -1;
        }
        sem_init(&mutex, 0, 1);
        //create table
        QLOGI("executing Query");
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, IO_PREFETCHER_QUERY_CREATE_TBL_FILE_LIST, NULL,NULL,NULL));
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, IO_PREFETCHER_QUERY_CREATE_TBL_PKG_LIST, NULL,NULL,NULL));
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, UX_ENGINE_QUERY_CREATE_TBL_PKG_LIST, NULL,NULL,NULL));
        _SQLITE_CHECK_OK(sqlite3_exec(db_conn, UX_LATENCY_QUERY_CREATE_TBL_PKG_LIST, NULL,NULL,NULL));
        close_database();
    }
    db_maintainer_init();
    return 0;
}

// Close data base connection
static int close_database()
{
  // db already closed?
  if( db_conn == NULL ) return -1;

  int rc = sqlite3_close(db_conn);
  if( rc != SQLITE_OK )
  {
    QLOGI("unable to close database=%s with sql err_msg=%s sql ret code=%d",
                   _dbpath, sqlite3_errmsg(db_conn),rc);
    return -1;
  }
  db_conn = NULL;

  QLOGI( "db=%s has been closed successfully", _dbpath);
  return 0;
}

int clean_db()
{
    //TBD
    // This function is not needed so far
    // may need to revisit in V2
    return 0;
}

//=============================================================================
// Get list of file for given package
int get_file_list(char *pkg_name, file_details *file_info_list, int size)
{
    int count=0;
    char *error=NULL;
    sqlite3_stmt *stmt=NULL;
    char query_str[1024] = IO_PREFETCHER_QUERY_GET_FILE_LIST;
    snprintf(query_str, sizeof(query_str), IO_PREFETCHER_QUERY_GET_FILE_LIST, pkg_name);
    QLOGI("Query = %s", query_str);
    if (NULL == db_conn) open_db();

    sem_wait(&mutex);
    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt)==SQLITE_ROW) {
        //fill field
        const char *pkg_name = (const char *)sqlite3_column_text(stmt, 0);
        const char *file_name = (const char *)sqlite3_column_text(stmt, 1);
        if (pkg_name != NULL) {
            snprintf(file_info_list[count].pkg_name, sizeof(file_info_list[count].pkg_name),
                                               "%s", pkg_name);
        } else {
            // Skip this iteration.
            continue;
        }
        if (file_name != NULL) {
            snprintf(file_info_list[count].file_name, sizeof(file_info_list[count].file_name),
                                               "%s", file_name);
        } else {
            // Skip this iteration.
            continue;
        }
        file_info_list[count].file_use_counter = sqlite3_column_int(stmt, 2);
        file_info_list[count].file_time_stamp = sqlite3_column_int(stmt, 3);
        file_info_list[count].filesize = sqlite3_column_int(stmt, 4);
        file_info_list[count].mark_for_delete = sqlite3_column_int(stmt, 5);
        file_info_list[count].file_modify_time = sqlite3_column_int(stmt, 6);
        file_info_list[count].file_iop_size = sqlite3_column_int(stmt, 7);
        file_info_list[count].study_finish =  sqlite3_column_int(stmt, 8);
        const char *array = (const char *)sqlite3_column_blob(stmt, 9);
        int len = sqlite3_column_bytes(stmt, 9);
        file_info_list[count].cache_dropped = sqlite3_column_int(stmt, 10);
        file_info_list[count].disabled = sqlite3_column_int(stmt, 11);

        if (len > 0 && array != NULL) {
            file_info_list[count].mincore_array = (char *)malloc(len);
            memcpy(file_info_list[count].mincore_array, array, len);
        }
        count++;
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return count;
}

//===========

// Get total number of file
int get_total_file(char * pkg_name)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_TOTAL_FILE;
    int num_files = 0;
    char *error=NULL;
    sqlite3_stmt *stmt=NULL;

    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_TOTAL_FILE,pkg_name);

    if(NULL == db_conn) open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt)==SQLITE_ROW) {
        num_files = sqlite3_column_int(stmt, 0);
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    sem_post(&mutex);

    return num_files;
}

//===========
// Callback function for get_total_pkgs
// Get total number of packages
int get_total_pkgs()
{
    char query_str[1024] = IO_PREFETCHER_QUERY_TOTAL_PKG;
    int num_pkg = 0;
    char *error=NULL;
    sqlite3_stmt *stmt=NULL;

    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt)==SQLITE_ROW) {
        num_pkg = sqlite3_column_int(stmt, 0);
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    sem_post(&mutex);

    return num_pkg;
}
//=============================================================================

// to get the list of packages
int get_pkg_list(pkg_details *pkg_list_ptr,int size)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_GET_PKG_LIST;
    int num_pkg;
    char *error=NULL;
    sqlite3_stmt *stmt=NULL;
    int counter = 0;

    if(NULL == db_conn)
        open_db();


    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt)==SQLITE_ROW && counter < size) {
        //QLOGI("pkg_name = %s \n",col_value[0]);
        const char *pkg_name = (const char *)sqlite3_column_text(stmt, 0);
        if (pkg_name != NULL) {
            strlcpy(pkg_list_ptr[counter].pkg_name, pkg_name, PKG_NAME_LEN);
        } else {
            // Skip this iteration.
            continue;
        }
        pkg_list_ptr[counter].last_time_launched = sqlite3_column_int(stmt, 1);
        pkg_list_ptr[counter].num_launches = sqlite3_column_int(stmt, 2);
        counter++;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    sem_post(&mutex);

    return counter;
}

int iop_query_exec(const char * query_str)
{
    char * error;
    sqlite3_stmt *stmt=NULL;

    QLOGI("queries = %s\n",query_str);
    if(NULL == db_conn) open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    if (sqlite3_step(stmt)!=SQLITE_DONE)
    {
        QLOGI("queries = %s fail to execute\n",query_str);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    sem_post(&mutex);

    return 0;
}

//==========================
// Remove package from databae
int remove_pkg(char *pkg_name)
{
    char pkg_query_str[2048] = IO_PREFETCHER_QUERY_REMOVE_PACKAGE_PKG_TBL;

    snprintf(pkg_query_str,sizeof(pkg_query_str),IO_PREFETCHER_QUERY_REMOVE_PACKAGE_PKG_TBL,pkg_name,pkg_name);

    return  iop_query_exec(pkg_query_str);
}

//==========================
// Remove file from databae
int remove_file(char *pkg_name,char* file_name)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_REMOVE_PACKAGE_FILE_TBL;
    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_REMOVE_PACKAGE_FILE_TBL,pkg_name,file_name);
    return  iop_query_exec(query_str);
}

//=====================
// Mark any file for delete so can be cleanup in next ittr
int mark_for_delete(char * pkg_name,char * file_name)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_MARK_FOR_DELETE;

    if (file_name) {
    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_MARK_FOR_DELETE
                                        ,pkg_name
                                        ,file_name);
    } else {
        snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_MARK_PKG_FOR_DELETE
                                            ,pkg_name);
    }
    return  iop_query_exec(query_str);
}

//=====================
// Mark any file for disable, that iop will ignore load or study it
int mark_for_disable(char * pkg_name,char * file_name)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_MARK_FOR_DISABLE;
    if (file_name) {
        snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_MARK_FOR_DISABLE
                                            ,pkg_name
                                            ,file_name);
     }
     return  iop_query_exec(query_str);
}

//=====================
// Mark need restudy study_finish=0
int mark_for_restudy(char * pkg_name,char * file_name)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_MARK_FOR_RESTUDY;
    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_MARK_FOR_RESTUDY
                                        ,pkg_name
                                        ,file_name);
    return  iop_query_exec(query_str);
}

int mark_cache_dropped(char *pkg_name, char *file_name, int cache_dropped)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_MARK_CACHE_DROPPED;
    snprintf(query_str, sizeof(query_str), IO_PREFETCHER_QUERY_MARK_CACHE_DROPPED
                                         , cache_dropped, pkg_name, file_name);
    return  iop_query_exec(query_str);
}

static int update_file_mtime_and_size(char *pkg_name, char *file_name
                                                    , int file_modify_time, int file_size)
{
    char query_str[1024] = IO_PREFETCHER_QUERY_UPDATE_FILE_MODIFIED;
    snprintf(query_str, sizeof(query_str), IO_PREFETCHER_QUERY_UPDATE_FILE_MODIFIED
                                         , file_modify_time, file_size, pkg_name, file_name);
    return  iop_query_exec(query_str);
}

/*
 * commit all info at one time to speed up db operation
 */
int commit_filelist_info(file_details *file_info_list, int size)
{
    int i;
    iop_query_exec("BEGIN TRANSACTION;");
    for (i=0; i<size; i++) {
        switch (file_info_list[i].cache_dropped) {
            case NEED_MARK_CACHE_NOT_DROPPED:
                mark_cache_dropped(file_info_list[i].pkg_name, file_info_list[i].file_name, 0);
                break;
            case NEED_MARK_CACHE_DROPPED:
                mark_cache_dropped(file_info_list[i].pkg_name, file_info_list[i].file_name, 1);
                break;
            default:
                break;
        }
        switch (file_info_list[i].mark_for_delete) {
            case NEED_MARK_FOR_DELETE_FILE://delete this file from db only
                mark_for_delete(file_info_list[i].pkg_name, file_info_list[i].file_name);
                break;
            case NEED_MARK_FOR_DELETE_PKG://delete all files of this pkg
                mark_for_delete(file_info_list[i].pkg_name, NULL);
                break;
            case NEED_MARK_FOR_UPDATE://need update modifytime and file size
                update_file_mtime_and_size(file_info_list[i].pkg_name, file_info_list[i].file_name,
                        file_info_list[i].file_modify_time, file_info_list[i].filesize);
                break;
            case NEED_MARK_FOR_DISABLE://mark file as disabled
                mark_for_disable(file_info_list[i].pkg_name, file_info_list[i].file_name);
                break;
            default://do nothing
                break;
        }
    }
    iop_query_exec("COMMIT TRANSACTION;");

    return 0;
}

//=====================
// Delete all the marked file for deletion
int delete_mark_files()
{
    return  iop_query_exec(IO_PREFETCHER_QUERY_DELETE_MARK_FILE);
}

// update detail for file with provided attibutes
int update_file_details(char * pkg_name,file_details *file_info[], int size)
{
    int i = 0;
    char *error=NULL;
    sqlite3_stmt *stmt=NULL;
    char trace_buf[1024];
    char query_str[2048] = IO_PREFETCHER_QUERY_UPDATE_FILE_DETAILS;
    iop_query_exec("BEGIN TRANSACTION;");
    for(i=0;i<size;i++) {
        file_details temp_file_info;
        temp_file_info.file_name[0]=0;
        int is_file_present  = get_file(pkg_name, file_info[i]->file_name, &temp_file_info);

        if(is_file_present) {
            //Update
            QLOGI("MINCORE update file %s %s study_finish %d, cache_dropped %d",
                    pkg_name, temp_file_info.file_name,
                    temp_file_info.study_finish, temp_file_info.cache_dropped);
            /*
             * update mincore array data if this file cache is dropped and study not finished
             */
            if (temp_file_info.cache_dropped == 1 && temp_file_info.study_finish <= 2) {
                snprintf(trace_buf, sizeof(trace_buf), "update file %s", file_info[i]->file_name);
                ATRACE_BEGIN(trace_buf);
                snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_UPDATE_FILE_DETAILS
                                                    ,file_info[i]->file_time_stamp
                                                    ,file_info[i]->filesize
                                                    ,file_info[i]->file_iop_size
                                                    ,pkg_name
                                                    ,file_info[i]->file_name);
                QLOGI("\nQuery = %s \n",query_str);
                int pages_in_range = (file_info[i]->filesize + PAGE_SIZE - 1)/PAGE_SIZE;
                sem_wait(&mutex);
                _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                            strlen(query_str), &stmt, (const char **)&error));

                int index = sqlite3_bind_parameter_index(stmt, ":mincore_array");
                if (file_info[i]->mincore_array) {
                    _SQLITE_CHECK_OK(sqlite3_bind_blob(stmt, index,
                                file_info[i]->mincore_array, pages_in_range, SQLITE_STATIC));
                }
                else
                {
                    _SQLITE_CHECK_OK(sqlite3_bind_blob(stmt, index, NULL, 0, SQLITE_STATIC));
                }
                _SQLITE_CHECK_DONE(sqlite3_step(stmt));
                sqlite3_reset(stmt);
                sqlite3_finalize(stmt);
                sem_post(&mutex);
                ATRACE_END();
            }
            else
            {
                if (file_info[i]->cache_dropped) {
                    mark_cache_dropped(pkg_name, file_info[i]->file_name, 0);
                }
            }
        }
        else
        {
            //insert new file to db
            int index;
            int pagesize = sysconf(_SC_PAGESIZE);
            QLOGI("MINCORE insert new file %s %s iopsize %d filesize %d",
                                                  pkg_name, file_info[i]->file_name,
                                                  file_info[i]->file_iop_size, file_info[i]->filesize);
            snprintf(trace_buf, sizeof(trace_buf), "insert new file %s", file_info[i]->file_name);
            ATRACE_BEGIN(trace_buf);

            snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_INSER_FILE
                                                ,pkg_name,file_info[i]->file_name
                                                ,file_info[i]->file_time_stamp
                                                ,file_info[i]->filesize
                                                ,file_info[i]->file_modify_time
                                                ,file_info[i]->file_iop_size
                                                );
            QLOGI("\nQuery = %s \n",query_str);
            sem_wait(&mutex);
            _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                        strlen(query_str), &stmt, (const char**)&error));
            if (file_info[i]->mincore_array != NULL && file_info[i]->file_iop_size >pagesize) {
                index = sqlite3_bind_parameter_index(stmt, ":mincore_array");
                int pages_in_range = (file_info[i]->filesize + PAGE_SIZE - 1)/PAGE_SIZE;
                _SQLITE_CHECK_OK(sqlite3_bind_blob(stmt, index,
                            file_info[i]->mincore_array, pages_in_range, SQLITE_STATIC));
            }
            _SQLITE_CHECK_DONE(sqlite3_step(stmt));
            sqlite3_reset(stmt);
            sqlite3_finalize(stmt);
            sem_post(&mutex);
            ATRACE_END();
        }
    }
    iop_query_exec("COMMIT TRANSACTION;");
     return 0;
}

//Update package deails with provided attributes
int update_pkg_details(pkg_details pkg_info)
{
    char query_str[2048] = IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS;
    pkg_details temp_pkg_info;
    temp_pkg_info.pkg_name[0] = 0;
    int is_file_present  =  get_package(pkg_info.pkg_name, &temp_pkg_info);
    QLOGI("update_pkg_details");

    if(NULL == db_conn)
        open_db();

    if(is_file_present)
    {
        //Update
        snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS
                                            ,pkg_info.last_time_launched
                                            ,pkg_info.pkg_name);
        QLOGI("\nQuery = %s \n",query_str);
        iop_query_exec(query_str);
    }
    else
    {
        //Insert
        snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_INSERT_PKG
                                            ,pkg_info.pkg_name
                                            ,pkg_info.last_time_launched
                                            );

        QLOGI("\nQuery = %s \n",query_str);
        iop_query_exec(query_str);
    }
    return 0;
}

//=============================================================================

int get_file(char *pkg_name, char *file_name, file_details *file_info)
{
    char query_str[2048] = IO_PREFETCHER_QUERY_UPDATE_PKG_DETAILS;
    char * error;
    sqlite3_stmt *stmt=NULL;
    int count = 0;
    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_GET_FILE,pkg_name,file_name);
    QLOGI("queries = %s\n",query_str);
    if(NULL == db_conn) open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt)==SQLITE_ROW) {

        //fill field
        const char *pkg_name = (const char *)sqlite3_column_text(stmt, 0);
        const char *file_name = (const char *)sqlite3_column_text(stmt, 1);

        if (pkg_name != NULL) {
            snprintf(file_info->pkg_name, sizeof(file_info->pkg_name),
                                    "%s", pkg_name);
        } else {
            // Skip this iteration.
            continue;
        }
        if (file_name != NULL) {
            snprintf(file_info->file_name, sizeof(file_info->file_name),
                                    "%s", file_name);
        } else {
            // Skip this iteration.
            continue;
        }
        file_info->file_use_counter = sqlite3_column_int(stmt, 2);
        file_info->file_time_stamp = sqlite3_column_int(stmt, 3);
        file_info->filesize = sqlite3_column_int(stmt, 4);
        file_info->mark_for_delete = sqlite3_column_int(stmt, 5);
        file_info->file_modify_time = sqlite3_column_int(stmt, 6);
        file_info->file_iop_size = sqlite3_column_int(stmt, 7);
        file_info->study_finish = sqlite3_column_int(stmt, 8);
        file_info->cache_dropped = sqlite3_column_int(stmt, 10);
        file_info->disabled = sqlite3_column_int(stmt, 11);

        count++;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);

    sem_post(&mutex);
    return count;
}

//=============================================================================
int get_package(char *pkg_name, pkg_details *pkg_info)
{

    char query_str[1024] = IO_PREFETCHER_QUERY_GET_PKG;
    char * error;
    sqlite3_stmt *stmt=NULL;
    int count = 0;

    snprintf(query_str,sizeof(query_str),IO_PREFETCHER_QUERY_GET_PKG,pkg_name);
    QLOGI("\n get_file %s \n",query_str);
    if(NULL == db_conn) open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt)==SQLITE_ROW) {
        const char *pkg_name = (const char *)sqlite3_column_text(stmt, 0);
        if (pkg_name != NULL) {
            strlcpy(pkg_info->pkg_name, pkg_name, PKG_NAME_LEN);
        } else {
            // Skip this iteration.
            continue;
        }
        pkg_info->last_time_launched = sqlite3_column_int(stmt, 1);
        pkg_info->num_launches = sqlite3_column_int(stmt, 2);
        count++;
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);

    return count;
}

/* UXEngine SQL Query functions */

#undef LOG_TAG
#define LOG_TAG           "UXEngine"

//==========================
// Remove package from databae
int remove_ux_pkg(char *pkg_name, char *week_day)
{

    char pkg_query_str1[2048] = UX_ENGINE_QUERY_REMOVE_PACKAGE_PKG_TBL;
    char pkg_query_str2[2048] = UX_LATENCY_QUERY_REMOVE_PACKAGE_PKG_TBL;
    snprintf(pkg_query_str1,sizeof(pkg_query_str1),UX_ENGINE_QUERY_REMOVE_PACKAGE_PKG_TBL,pkg_name,week_day);
    snprintf(pkg_query_str2,sizeof(pkg_query_str2),UX_LATENCY_QUERY_REMOVE_PACKAGE_PKG_TBL,pkg_name);

    QLOGI("queries = %s\n",pkg_query_str1);
    QLOGI("queries = %s\n",pkg_query_str2);

    iop_query_exec(pkg_query_str1);
    iop_query_exec(pkg_query_str2);

    return 0;
}

int offset = 1;
// get the list of packages most used
int get_top_ux_pkg_list(pkg_ux_top_details *pkg_list, char *week_day, int time_slot, int size, int start)
{
    int counter = 0;
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    char query_str[2048] = UX_ENGINE_QUERY_GET_TOP_PKG_LIST;
    snprintf(query_str,sizeof(query_str),UX_ENGINE_QUERY_GET_TOP_PKG_LIST,time_slot,week_day,time_slot);

    QLOGI("queries = %s\n", query_str);

    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        pkg_ux_top_details tmp_pkg;
        const char *pkg_name = (const char *) sqlite3_column_text(stmt, 0);
        int timeslot_count_select = sqlite3_column_int(stmt,3);
        int bindApp_dur = sqlite3_column_int(stmt,4);
        int disAct_dur = sqlite3_column_int(stmt,5);
        int not_game = sqlite3_column_int(stmt,7);

        if (pkg_name == NULL) {
            continue;
        }

        QLOGI("pkg_name = %s timeslot_cnt = %d bindApp_dur = %d disAct_dur = %d not_game = %d\n", pkg_name,
                 timeslot_count_select, bindApp_dur, disAct_dur, not_game);
        if(disAct_dur != 0 && (counter < size) && not_game)
        {
            if (offset < start) {
                offset++;
                sqlite3_reset(stmt);
                sqlite3_finalize(stmt);
                sem_post(&mutex);
                return 0;
            }

            snprintf(pkg_list[counter].pkg_name, sizeof(pkg_list[counter].pkg_name), "%s", pkg_name);
            pkg_list[counter].last_time_launched = sqlite3_column_int(stmt,1);
            pkg_list[counter].num_launches = sqlite3_column_int(stmt,2);
            pkg_list[counter].timeslot_count_select = timeslot_count_select;
            pkg_list[counter].bindApp_dur = bindApp_dur;
            pkg_list[counter].disAct_dur = disAct_dur;
            pkg_list[counter].memory_st = sqlite3_column_int(stmt,6);

            // Compare to prev_pkg (depth of half preferred_apps)
            /* Bubble sort to half-depth & prioritize higher bindApp
               latencies when the launch count of preferred apps are
               close to each other (< 3)
            */
            int swap = 0;
            do {
                swap = 0;
                for(int i = 1; i <= PREFERRED_APP_COUNT/2; i++) {
                    if (counter - i < 0) {
                       break;
                    }
                    if (pkg_list[counter-i].timeslot_count_select - pkg_list[counter-i+1].timeslot_count_select < 3)
                    {
                        //launch count is close. Treat them as equal priority & compare bindApp
                        if (pkg_list[counter-i+1].bindApp_dur > pkg_list[counter-i].bindApp_dur) {
                            // tmp
                            strlcpy(tmp_pkg.pkg_name, pkg_list[counter-i].pkg_name, PKG_NAME_LEN);
                            tmp_pkg.last_time_launched = pkg_list[counter-i].last_time_launched;
                            tmp_pkg.num_launches = pkg_list[counter-i].num_launches;
                            tmp_pkg.timeslot_count_select = pkg_list[counter-i].timeslot_count_select;
                            tmp_pkg.bindApp_dur = pkg_list[counter-i].bindApp_dur;
                            tmp_pkg.disAct_dur = pkg_list[counter-i].disAct_dur;
                            tmp_pkg.memory_st = pkg_list[counter-i].memory_st;

                            // swap-1
                            strlcpy(pkg_list[counter-i].pkg_name, pkg_list[counter-i+1].pkg_name, PKG_NAME_LEN);
                            pkg_list[counter-i].last_time_launched = pkg_list[counter-i+1].last_time_launched;
                            pkg_list[counter-i].num_launches = pkg_list[counter-i+1].num_launches;
                            pkg_list[counter-i].timeslot_count_select = pkg_list[counter-i+1].timeslot_count_select;
                            pkg_list[counter-i].bindApp_dur = pkg_list[counter-i+1].bindApp_dur;
                            pkg_list[counter-i].disAct_dur = pkg_list[counter-i+1].disAct_dur;
                            pkg_list[counter-i].memory_st = pkg_list[counter-i+1].memory_st;

                            // swap-2
                            strlcpy(pkg_list[counter-i+1].pkg_name, tmp_pkg.pkg_name,PKG_NAME_LEN);
                            pkg_list[counter-i+1].last_time_launched = tmp_pkg.last_time_launched;
                            pkg_list[counter-i+1].num_launches = tmp_pkg.num_launches;
                            pkg_list[counter-i+1].timeslot_count_select = tmp_pkg.timeslot_count_select;
                            pkg_list[counter-i+1].bindApp_dur = tmp_pkg.bindApp_dur;
                            pkg_list[counter-i+1].disAct_dur = tmp_pkg.disAct_dur;
                            pkg_list[counter-i+1].memory_st = tmp_pkg.memory_st;

                            swap = 1;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            } while (swap);
            counter++;
        }
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return counter;
}

// Get total number of packages
int get_total_ux_pkgs(int uniq)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    char query_str[1024] = UX_ENGINE_QUERY_TOTAL_PKG;
    int num_pkgs = 0;

    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        num_pkgs = sqlite3_column_int(stmt, 0);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return num_pkgs;
}

int get_total_pkg_use_count(char *pkg_name)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    int launch_count = 0;
    char query_str[2048] = UX_LATENCY_QUERY_GET_PKG_COUNT;

    snprintf(query_str,sizeof(query_str),UX_LATENCY_QUERY_GET_PKG_COUNT,pkg_name);

    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        launch_count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return launch_count;
}

int get_total_pkg_bindapp_count(char *pkg_name)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    int bindApp_count = 0;
    char query_str[2048] = UX_LATENCY_QUERY_GET_BINDAPP_COUNT;

    snprintf(query_str,sizeof(query_str),UX_LATENCY_QUERY_GET_BINDAPP_COUNT,pkg_name);

    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        bindApp_count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return bindApp_count;
}

int get_ux_lat_pkg(char *pkg_name, pkg_ux_lat_details *pkg_info)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    int counter = 0;
    char query_str[1024] = UX_LATENCY_QUERY_GET_LAT;

    snprintf(query_str,sizeof(query_str),UX_LATENCY_QUERY_GET_LAT,pkg_name);
    QLOGI("\n get_lat_package %s \n",query_str);
    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *pkg_name = (const char *)sqlite3_column_text(stmt, 0);
        if (pkg_name == NULL)
            continue;
        QLOGI("pkg_name = %s\n", pkg_name);
        strlcpy(pkg_info[counter].pkg_name, pkg_name, PKG_NAME_LEN);
        pkg_info[counter].bindApp_dur = sqlite3_column_int(stmt,1);
        pkg_info[counter].disAct_dur = sqlite3_column_int(stmt,2);
        pkg_info[counter].memory_st = sqlite3_column_int(stmt,3);
        pkg_info[counter].bindApp_ct = sqlite3_column_int(stmt,4);
        counter++;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return counter;
}

int get_ux_lat_pred(pkg_ux_lat_pred_details *pkg_info)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    int counter = 0;
    char query_str[1024] = UX_LATENCY_QUERY_GET_LAT_PRED;

    QLOGI("\n get_lat_pred_packages %s \n",query_str);
    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *pkg_name = (const char *)sqlite3_column_text(stmt, 0);
        if (pkg_name == NULL)
            continue;
        QLOGI("pkg_name = %s\n", pkg_name);
        strlcpy(pkg_info[counter].pkg_name, pkg_name, PKG_NAME_LEN);
        pkg_info[counter].pred_success = sqlite3_column_int(stmt,1);
        pkg_info[counter].pred_fail = sqlite3_column_int(stmt,2);
        counter++;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return counter;
}

//Modify database on an app update to ensure data accuracy.
int set_ux_lat_app_upd(char *pkg_name)
{
    char query_str[2048] = UX_LATENCY_QUERY_SET_PKG_UPDATE;
    snprintf(query_str,sizeof(query_str),UX_LATENCY_QUERY_SET_PKG_UPDATE
                                      ,pkg_name);
    QLOGI("set_ux_lat_db_on_app_update query: %s\n", query_str);

    return iop_query_exec(query_str);
}

//Update latency details of package
int update_ux_lat_details(char *pkg_name, int bindApp_dur, int disAct_dur, int memory, int non_empty)
{
    char query_str1[2048] = UX_LATENCY_QUERY_UPDATE_PKG_DETAILS;
    char query_str2[2048] = UX_LATENCY_QUERY_INSERT_PKG;
    pkg_ux_lat_details *pkg_info;
    pkg_info = (pkg_ux_lat_details *) malloc (sizeof(pkg_ux_lat_details));

    if (pkg_info == NULL)
        return 0;

    int is_pkg_present = get_ux_lat_pkg(pkg_name, pkg_info);
    free(pkg_info);

    QLOGI("Updating ux_lat database: is_pkg_present: %d\n", is_pkg_present);

    if (is_pkg_present)
    {
        snprintf(query_str1,sizeof(query_str1),UX_LATENCY_QUERY_UPDATE_PKG_DETAILS
                                            ,bindApp_dur
                                            ,disAct_dur
                                            ,memory
                                            ,non_empty
                                            ,pkg_name);
        QLOGE("\nUpdate Query = %s \n",query_str1);
        iop_query_exec(query_str1);
    } else {
        snprintf(query_str2,sizeof(query_str2),UX_LATENCY_QUERY_INSERT_PKG
                                            ,pkg_name
                                            ,bindApp_dur
                                            ,disAct_dur
                                            ,memory);
        QLOGE("\nInsert Query = %s \n",query_str2);
        iop_query_exec(query_str2);
    }
    return 0;
}

int update_ux_lat_pred_details(char *pkg_name, int succ, int weight)
{
    char query_str[2048] = UX_LATENCY_QUERY_UPDATE_PRED_DETAILS;
    QLOGI("update_lat_pred_details");

    if (succ)
    {
        snprintf(query_str,sizeof(query_str),UX_LATENCY_QUERY_UPDATE_PRED_DETAILS
                                             ,weight
                                             ,0
                                             ,pkg_name);
        QLOGI("\nUpdate Query = %s \n",query_str);
        iop_query_exec(query_str);
    } else {
        snprintf(query_str,sizeof(query_str),UX_LATENCY_QUERY_UPDATE_PRED_DETAILS
                                             ,0
                                             ,weight
                                             ,pkg_name);
        QLOGI("\nUpdate Query = %s \n",query_str);
        iop_query_exec(query_str);
    }
    return 0;
}

int update_ux_lat_game_details(char *pkg_name, int not_game)
{
    char query_str1[2048] = UX_LATENCY_QUERY_UPDATE_GAME_DETAILS;
    snprintf(query_str1,sizeof(query_str1),UX_LATENCY_QUERY_UPDATE_GAME_DETAILS
                                          ,not_game
                                          ,pkg_name);

    QLOGI("\nUpdate Query = %s \n",query_str1);
    iop_query_exec(query_str1);
    return 0;
}

//Update package details with provided attributes
int update_ux_pkg_details(pkg_ux_details pkg_info, char *week_day, int time_slot, int sub)
{
    char query_str1[2048] = UX_ENGINE_QUERY_UPDATE_PKG_DETAILS;
    char query_str2[2048] = UX_ENGINE_QUERY_INSERT_PKG;
    int is_pkg_present  =  get_ux_package(pkg_info.pkg_name);

    QLOGI("Updating_ux_pkg_details database: is_pkg_present: %d\n", is_pkg_present);

    if(is_pkg_present == 3 || (is_pkg_present == 2 && !strncmp(week_day, "true", 4))
       || (is_pkg_present == 1 && !strncmp(week_day, "false", 5)))
    {
        //Update
        snprintf(query_str1,sizeof(query_str1),UX_ENGINE_QUERY_UPDATE_PKG_DETAILS
                                            ,pkg_info.last_time_launched
                                            ,sub
                                            ,time_slot
                                            ,time_slot
                                            ,pkg_info.pkg_name
                                            ,week_day);
        QLOGI("\nUpdate Query = %s \n",query_str1);
        iop_query_exec(query_str1);
    }
    else
    {
        int timeArray[4] = {0,0,0,0};
        switch (time_slot) {
            case 1:
                timeArray[0] = 1;
                break;
            case 2:
                timeArray[1] = 1;
                break;
            case 3:
                timeArray[2] = 1;
                break;
            case 4:
                timeArray[3] = 1;
                break;
            default:
                QLOGI("Database insert: Erroneous time slot. Skip insert\n");
                goto out;
        }

        //Insert
        snprintf(query_str2,sizeof(query_str2),UX_ENGINE_QUERY_INSERT_PKG
                                            ,pkg_info.pkg_name
                                            ,week_day
                                            ,pkg_info.last_time_launched
                                            ,(pkg_info.num_launches+1)
                                            ,0
                                            ,timeArray[0]
                                            ,timeArray[1]
                                            ,timeArray[2]
                                            ,timeArray[3]
                                            );

        QLOGE("\nInsert Query = %s \n",query_str2);
        iop_query_exec(query_str2);
        out:
            return -1;
    }
    return 0;
}

int reset_ux_pkg_details(pkg_ux_details pkg_info)
{
    char query_str[2048] = UX_ENGINE_QUERY_RESET_PKG_DETAILS;
    //Insert
    snprintf(query_str,sizeof(query_str),UX_ENGINE_QUERY_RESET_PKG_DETAILS
                                        ,pkg_info.last_time_launched
                                        ,pkg_info.num_launches
                                        ,pkg_info.num_sub_launches
                                        ,pkg_info.timeslot_count_1
                                        ,pkg_info.timeslot_count_2
                                        ,pkg_info.timeslot_count_3
                                        ,pkg_info.timeslot_count_4
                                        ,pkg_info.pkg_name
                                        ,pkg_info.week_day
                                        );

    QLOGI("\nQuery = %s \n",query_str);
    iop_query_exec(query_str);
    return 0;
}

//=============================================================================
// SQL query for get-package
int get_ux_package(char *pkg_name)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    char query_str[1024] = UX_ENGINE_QUERY_GET_PKG;
    int size = 3;
    int counter = 0;

    snprintf(query_str,sizeof(query_str),UX_ENGINE_QUERY_GET_PKG,pkg_name);
    QLOGI("\n get_ux_package %s \n",query_str);

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if(counter < size)
        {
            const char *tmp_week = (const char *)sqlite3_column_text(stmt, 1);
            if (tmp_week == NULL)
                continue;
            if (!strncmp(tmp_week, "true", 4))
                counter = counter + 2;
            else
                counter++;
        } else {
            break;
        }
    }
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return counter;
}

// to get the list of packages
int get_ux_pkg_list(pkg_ux_details *pkg_list, int size)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    int counter = 0;
    char query_str[1024] = UX_ENGINE_QUERY_GET_PKG_LIST;

    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if(counter < size)
        {
            const char *pkg_name = (const char *)sqlite3_column_text(stmt, 0);
            const char *tmp_week = (const char *)sqlite3_column_text(stmt, 1);
            if (pkg_name == NULL || tmp_week == NULL)
                continue;
            strlcpy(pkg_list[counter].pkg_name, pkg_name, PKG_NAME_LEN);
            strlcpy(pkg_list[counter].week_day, tmp_week, 6);
            pkg_list[counter].last_time_launched = sqlite3_column_int(stmt, 2);
            pkg_list[counter].num_launches = sqlite3_column_int(stmt, 3);
            pkg_list[counter].timeslot_count_1 = sqlite3_column_int(stmt, 4);
            pkg_list[counter].timeslot_count_2 = sqlite3_column_int(stmt, 5);
            pkg_list[counter].timeslot_count_3 = sqlite3_column_int(stmt, 6);
            pkg_list[counter].timeslot_count_4 = sqlite3_column_int(stmt, 7);
            counter++;
        } else {
            break;
        }
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return counter;
}

double get_ux_launch_count_avg(char *week_day)
{
    char *error = NULL;
    sqlite3_stmt *stmt = NULL;
    double avg = 0;
    char query_str[1024] = UX_ENGINE_QUERY_AVERAGE_LAUNCH_COUNT;
    snprintf(query_str,sizeof(query_str),UX_ENGINE_QUERY_AVERAGE_LAUNCH_COUNT,week_day);

    if(NULL == db_conn)
        open_db();

    sem_wait(&mutex);

    _SQLITE_CHECK_OK(sqlite3_prepare_v2(db_conn, query_str,
                strlen(query_str), &stmt, (const char**)&error));

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        avg = sqlite3_column_int(stmt, 0);
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    sem_post(&mutex);
    return avg;
}

int uninstall_pkg(char *pkg_name)
{
    char query_str[1024] = UX_ENGINE_QUERY_UNINSTALL_PKG;
    snprintf(query_str,sizeof(query_str),UX_ENGINE_QUERY_UNINSTALL_PKG,pkg_name,pkg_name);

    iop_query_exec(query_str);
    return 0;
}
