/******************************************************************************
  @file    iopDB.cpp
  @brief   Implementation of Learning Module Database test

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#include "iopDB.h"


#define IOP_DB_NAME "iopexp"

iopDatabase* iopDatabase::iopdb = NULL;

iopDatabase::iopDatabase(unsigned long int flags, const std::string db_name, const std::string pkgNPtr, const std::string fileNPtr): LMDBIopBase(flags, db_name, pkgNPtr, fileNPtr) {
}

iopDatabase::~iopDatabase() {
    if(iopdb) {
        iopdb = NULL;
    }
}

struct temp {
    int x;
};
iopDatabase* iopDatabase::getInstance() {
    int err;
    if (!iopdb) {
        iopdb = new iopDatabase(0, IOP_DB_NAME, PKG_TABLE_NAME, PKG_FILE_TABLE_NAME);
        if (!iopdb) {
            QLOGE("ERROR: getInstance failure\n");
            return NULL;
        }

        err=iopdb->LMDatabaseInit();
        if (err) {
            QLOGE("ERROR: err=%d\n", err);
            delete iopdb;
            return NULL;
        }
        iopdb->test_pkg_table();
        iopdb->test_file_table();

    }
    return iopdb;
}

#define TEST_PKG_ITR_WR 6
#define TEST_PKG_ITR_RD 8
void iopDatabase::test_pkg_table() {
    //std::vector<DR_pkg_details*> tmpVec;
    iopDatabase::vecT<pkg_details> tmpVec;
    LMDBDataRowKey *stKeyPtr, stKey;

    size_t reqCount = TEST_PKG_ITR_WR, retCount = 0;
    tmpVec = allocateDR<pkg_details>(reqCount);

    int itr_cnt = tmpVec.size();
    for (int i = 0; i < itr_cnt; ++i) {
        stKeyPtr = &tmpVec[i]->dataRowKey;
        auto stDataPtr = &tmpVec[i]->dataRowPayload;
        //LMDB_DEBUG_UTEST("stKeyPtr=%p, stDataPtr=%p\n", stKeyPtr, stDataPtr);

        switch (i) {
        case 0:
            stKeyPtr->appNm = "pkg0"; stKeyPtr->keys[KEY_TYPE_INT1_INDEX] = 0;
            stDataPtr->num_launches=11; stDataPtr->last_time_launched=111;break;
        case 1:
            stKeyPtr->appNm = "pkg1"; stKeyPtr->keys[KEY_TYPE_INT1_INDEX] = 1;
            stDataPtr->num_launches=22; stDataPtr->last_time_launched=222;break;
        case 2:
            stKeyPtr->appNm = "pkg2"; stKeyPtr->keys[KEY_TYPE_INT1_INDEX] = 0;
            stDataPtr->num_launches=33; stDataPtr->last_time_launched=333;break;
        case 3:
            stKeyPtr->appNm = "pkg3"; stKeyPtr->keys[KEY_TYPE_INT1_INDEX] = 1;
            stDataPtr->num_launches=44; stDataPtr->last_time_launched=444;break;
        case 4:
            stKeyPtr->appNm = "pkg4"; stKeyPtr->keys[KEY_TYPE_INT1_INDEX] = 0;
            stDataPtr->num_launches=55; stDataPtr->last_time_launched=555;break;
        case 5:
            stKeyPtr->appNm = "pkg5"; stKeyPtr->keys[KEY_TYPE_INT1_INDEX] = 1;
            stDataPtr->num_launches=66; stDataPtr->last_time_launched=666;break;
        }
    }

    writeDeallocateDR<pkg_details>(tmpVec);
    deallocateDR<pkg_details>(tmpVec);  //tmpVec of 0 size


    auto AppList = getTxtColUniqList<pkg_details>(TXTCOL_APPNAME, ORDERBYCOL_APPNAME, ORDERBYDIR_DESCENDING);
    int AppListSz = AppList.size();
    stKey.appNm = "";
    if ((size_t)AppListSz != (reqCount=getCountDR<pkg_details>(stKey))) {
        LMDB_DEBUG_UTEST("pkg_details-ERROR: Issue with getting complete pkg list, AppListSz(%d) != reqCount(%zu)\n", AppListSz, reqCount);
        return;
    }
    itr_cnt = TEST_PKG_ITR_RD;
    for (int i = 0; i < itr_cnt; ++i) {
        stKeyPtr = &stKey;

        switch (i) {
        case 0: case 1: case 2: case 3: case 4: case 5:
            if (i < AppListSz) {
                stKeyPtr->appNm = AppList[i];
                break;
            } else {
                i=5;
                continue;
            }
        case 6:
            stKeyPtr->appNm = "pkg6"; break;
        case 7:
            LMDB_DEBUG_UTEST("pkg_details-All Pkgs:%d\n", AppListSz);
            stKeyPtr->appNm = "";
            stKeyPtr->keys[KEY_TYPE_INT1_INDEX] = 1; break;

        }

        reqCount=getCountDR<pkg_details>(stKey);
        tmpVec=readAllocateDR<pkg_details>(stKey, reqCount);
        auto itr_cnt1 = tmpVec.size();
        for (unsigned long i = 0; i < itr_cnt1; ++i) {
            stKeyPtr = &(tmpVec[i]->dataRowKey);
            auto stDataPtr = &(tmpVec[i]->dataRowPayload);
            LMDB_DEBUG_UTEST("pkg_details pkg_name=%s, nlnchs=%d, ltlnchd=%d\n", stKeyPtr->appNm.c_str(), stDataPtr->num_launches, stDataPtr->last_time_launched);
        }
        if (i == 0 ) {
            stKeyPtr->appNm = "pkg7";
            retCount=writeDeallocateDR<pkg_details>(tmpVec);
            LMDB_DEBUG_UTEST("pkg_details-writeDeallocateDR retCount=%zu\n", retCount);
        } else if (i == 5) {
            retCount=deleteDeallocateDR<pkg_details>(tmpVec);
            LMDB_DEBUG_UTEST("pkg_details-deleteDeallocateDR reqCount=%zu, retCount=%zu\n", reqCount, retCount);
        } else {
            retCount=deallocateDR<pkg_details>(tmpVec);
            LMDB_DEBUG_UTEST("pkg_details-deallocateDR retCount=%zu\n", retCount);
        }
    }
    LMDBDataRowKey stKeyTemp("");
    //reqCount=deleteAllDR<pkg_details>(stKeyTemp);
}

#define TEST_FILE_ITR_WR 6
#define TEST_FILE_ITR_RD 5
void iopDatabase::test_file_table() {
    //std::vector<DR_file_details*> tmpVec;
    iopDatabase::vecT<file_details> tmpVec;
    LMDBDataRowKey *stKeyPtr, stKey;

    size_t reqCount = TEST_FILE_ITR_WR, retCount = 0;
    tmpVec = allocateDR<file_details>(reqCount);

    int itr_cnt = tmpVec.size();
    for (int i = 0; i < itr_cnt; ++i) {
        stKeyPtr = &tmpVec[i]->dataRowKey;
        auto stDataPtr = &tmpVec[i]->dataRowPayload;
        //LMDB_DEBUG_UTEST("stKeyPtr=%p, stDataPtr=%p\n", stKeyPtr, stDataPtr);

        switch (i) {
        case 0:
            stKeyPtr->appNm = "pkg0"; stKeyPtr->rsvdNm = "file0";
            snprintf(stDataPtr->dirName, MAX_DIR_NAME_LENGTH, "%s", "how"); stDataPtr->filesize=1010; break;
        case 1:
            stKeyPtr->appNm = "pkg0"; stKeyPtr->rsvdNm = "file1";
            snprintf(stDataPtr->dirName, MAX_DIR_NAME_LENGTH, "%s", "now"); stDataPtr->filesize=2020; break;
        case 2:
            stKeyPtr->appNm = "pkg0"; stKeyPtr->rsvdNm = "file2";
            snprintf(stDataPtr->dirName, MAX_DIR_NAME_LENGTH, "%s", "wow"); stDataPtr->filesize=3030; break;
        case 3:
            stKeyPtr->appNm = "pkg1"; stKeyPtr->rsvdNm = "file3";
            snprintf(stDataPtr->dirName, MAX_DIR_NAME_LENGTH, "%s", "get"); stDataPtr->filesize=4040; break;
        case 4:
            stKeyPtr->appNm = "pkg2"; stKeyPtr->rsvdNm = "file1";
            snprintf(stDataPtr->dirName, MAX_DIR_NAME_LENGTH, "%s", "set"); stDataPtr->filesize=5050; break;
        case 5:
            stKeyPtr->appNm = "pkg2"; stKeyPtr->rsvdNm = "file1";
            snprintf(stDataPtr->dirName, MAX_DIR_NAME_LENGTH, "%s", "let"); stDataPtr->filesize=6060; break;
        }
    }


    writeDeallocateDR<file_details>(tmpVec);
    deallocateDR<file_details>(tmpVec); //tmpVec of 0 size


    auto AppList = getTxtColUniqList<file_details>(TXTCOL_APPNAME);
    int AppListSz = AppList.size();

    itr_cnt = TEST_FILE_ITR_RD;
    for (int i = 0; i < itr_cnt; ++i) {
        stKeyPtr = &stKey;

        switch (i) {
        case 0: case 1: case 2:
            if (i < AppListSz) {
                stKeyPtr->appNm = AppList[i];
                break;
            } else {
                i=2;
                continue;
            }
        case 3:
            stKeyPtr->appNm = "pkg3"; break;

        case 4:
            LMDB_DEBUG_UTEST("file_details-All Pkgs:%d\n", AppListSz);
            stKeyPtr->appNm = "";
            stKeyPtr->rsvdNm = "file1"; break;
        }
        LMDBDataRowKey stKeyTemp(stKey);
        reqCount=getCountDR<file_details>(stKeyTemp);
        tmpVec=readAllocateDR<file_details>(stKeyTemp, reqCount);

        auto itr_cnt1 = tmpVec.size();
        for (unsigned long i = 0; i < itr_cnt1; ++i) {
            stKeyPtr = &(tmpVec[i]->dataRowKey);
            auto stDataPtr = &(tmpVec[i]->dataRowPayload);
            LMDB_DEBUG_UTEST("file_details pkg_name=%s, file_name=%s, dir_name=%s, filesz=%d\n", stKeyPtr->appNm.c_str(), stKeyPtr->rsvdNm.c_str(), stDataPtr->dirName, stDataPtr->filesize);
        }
        if (i == 2 ) {
            stKeyPtr->appNm = "pkg7";
            retCount=writeDeallocateDR<file_details>(tmpVec);
            LMDB_DEBUG_UTEST("file_details-writeDeallocateDR retCount=%zu\n", retCount);
        } else if (i == 1 ) {
            retCount=deleteDeallocateDR<file_details>(tmpVec);
            LMDB_DEBUG_UTEST("file_details-deleteDeallocateDR reqCount=%zu, retCount=%zu\n", reqCount, retCount);
        } else {
            retCount=deallocateDR<file_details>(tmpVec);
            LMDB_DEBUG_UTEST("file_details-deallocateDR retCount=%zu\n", retCount);
        }
    }
    LMDBDataRowKey stKeyTemp("");
    //reqCount=deleteAllDR<file_details>(stKeyTemp);
}
