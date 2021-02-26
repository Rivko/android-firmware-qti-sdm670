/******************************************************************************
  @file    lmDBStruct.h
  @brief   Implementation of Learning Module Database

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef LMDB_STRUCT_H_
#define LMDB_STRUCT_H_
#include <cstring>
#include "internal/lmDBType.h"

/*limits related to max reqCount to be used while calling DBI APIs*/
#define RESTRICT_MAX_ALLOCATE_DR_ROW_REQCOUNT   100
#define RESTRICT_MAX_TXTCOL_UNIQUELIST_REQCOUNT 1000
#define RESTRICT_MAX_READALLOC_REQCOUNT         10000

#define LMDB_INTKEY_DEFVAL (std::numeric_limits<lmdbIntKey>::max())

/*limits related to DB row*/
#define KEY_TYPE_INT_RSVD               1



class LMDBDataRowHeader {
public:
    lmdbInt64Key rID;   /* rowID: used internally by DB */
    LMDBDataRowHeader() : rID(0) {
    }
};


enum lmdb_drk_index {
    KEY_TYPE_INT_UNUSED_INDEX=-1,
    KEY_TYPE_INT1_INDEX=0
};

class LMDBDataRowKey {
    //LMDBDataRowKey(const LMDBDataRowKey& copy);             //Not to be implemented
    //LMDBDataRowKey& operator=(const LMDBDataRowKey& copy);  //Not to be implemented
public:
    lmdbIntKey keys[KEY_TYPE_INT_RSVD]; //evntID/sampleNo, rsvdID1, rsvdID2
    Name appNm;
    Name rsvdNm;

    LMDBDataRowKey() {
        for(int i = 0; i < KEY_TYPE_INT_RSVD; i++) {
            keys[i] = LMDB_INTKEY_DEFVAL;
        }
    }
    LMDBDataRowKey(const Name& AppName, lmdbIntKey keys_0=LMDB_INTKEY_DEFVAL) : LMDBDataRowKey() {
        appNm = AppName;
        keys[KEY_TYPE_INT1_INDEX] = keys_0;
    }
};


template <class S>
class LMDBDataRow {
    template<class... T>
    friend class LMDatabase;
    LMDBDataRowHeader dataRowHeader;

    LMDBDataRow(const LMDBDataRow& copy);             //Not to be implemented
    LMDBDataRow& operator=(const LMDBDataRow& copy);  //Not to be implemented
public:
    LMDBDataRowKey dataRowKey;
    S dataRowPayload;
    LMDBDataRow() {}
    LMDBDataRow(const Name& AppName): dataRowKey(AppName) {}
};


#endif /* LMDB_STRUCT_H_ */
