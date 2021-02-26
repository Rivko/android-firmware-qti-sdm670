/******************************************************************************
  @file    lmDB.cpp
  @brief   Implementation of Learning Module Database

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/


#include "lmDB.h"

//sets global sql config, must be called before any other sql fn
int LMDBGlobal::LMDBGlobalConfig() {
    //sql config may not be thread safe, so no other thread should call
    //sql interface when config is running
    if (LMDB_SQL_OK !=LMDB_SQL_CONFIG_FN(LMDB_SQL_CONFIG_SERIALIZED)) {
        QLOGE("ERROR:sql config failed\n");
        return LMDB_CONFIG_ERROR;
    }
    //route sql log to adb log
    /*bool lmVerboseLog = true;
    LMDB_SQL_CONFIG_FN(LMDB_SQL_CONFIG_LOG,&lmLogCB,lmVerboseLog ? (void*)1 : NULL);*/

    if (LMDB_SQL_OK != LMDB_SQL_INIT_FN()) {
        QLOGE("ERROR:sql initialize failed\n");
        return LMDB_CONFIG_ERROR;
    }

    return LMDB_NO_ERROR;
}

int LMDBGlobal::LMDBGlobalDeConfig() {
    //sql shutdown may not be threadsafe. Also,
    //All openDB connections must be closed and all
    //sql resources must be deallocated before calling shutdown
    if (LMDB_SQL_OK != LMDB_SQL_SHTDWN_FN()) {
        QLOGE("ERROR:LMDBGlobalDeConfig failed\n");
        return LMDB_CONFIG_ERROR;
    }
    return LMDB_NO_ERROR;
}



