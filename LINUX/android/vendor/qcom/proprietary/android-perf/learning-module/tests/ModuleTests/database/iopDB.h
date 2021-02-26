/******************************************************************************
  @file    iopDB.h
  @brief   Implementation of Learning Module Database test

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef IOPDB_H_
#define IOPDB_H_

#include "lmDB.h"

//PackageTableInfo
#define PKG_TABLE_NAME      "io_pkg_tbl"

//FileTableInfo
#define PKG_FILE_TABLE_NAME "io_pkg_file_tbl"
#define MAX_DIR_NAME_LENGTH 4
#define MAX_PKG_NAME_LEN    5
#define MAX_FILE_NAME_LEN   6

typedef struct io_file_details
{
    char dirName[MAX_DIR_NAME_LENGTH];
    int filesize;       // size in KB
} file_details;

typedef struct io_pkg_details {
    int last_time_launched;
    int num_launches;
} pkg_details;

using DR_file_details = LMDBDataRow<file_details>;
using DR_pkg_details = LMDBDataRow<pkg_details>;

using LMDBIopBase = LMDatabase<pkg_details, file_details>;

//Singleton class iopDatabase
class iopDatabase: public LMDBIopBase {
public:
    static iopDatabase* getInstance();
    ~iopDatabase();

private:
    //ctor, copy ctor, assignment overloading
    iopDatabase(unsigned long int, const std::string, const std::string, const std::string);
    iopDatabase(iopDatabase const& copy);               //Not to be implemented
    iopDatabase& operator=(iopDatabase const& copy);    //Not to be implemented

    void test_pkg_table();
    void test_file_table();

    static iopDatabase *iopdb;
};


#endif /* IOPDB_H_ */
