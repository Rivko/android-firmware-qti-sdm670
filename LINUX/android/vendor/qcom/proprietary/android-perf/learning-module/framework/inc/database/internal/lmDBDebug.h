/******************************************************************************
  @file    lmDBDebug.h
  @brief   Implementation of Learning Module Database

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef LMDB_DEBUG_H
#define LMDB_DEBUG_H

#ifndef LMDB_UTEST
#include <cutils/log.h>
#endif //LMDB_UTEST

//#define LMDB_DEBUG          //debug: enable DBlib logs
//#define LMDB_DATABASE_DEBUG //debug: enable detailed DBlib/sql logs

#define LOG_TAG_LMDB "LMDB"

#ifndef LMDB_UTEST
#define QLOGE(x, ...)      ALOGE("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QLOGW(x, ...)      ALOGW("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#else //LMDB_UTEST
#define QLOGE(x, ...)      printf("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QLOGW(x, ...)      printf("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#endif //LMDB_UTEST

#ifdef LMDB_DEBUG
#ifndef LMDB_UTEST
#define QLOGD(x, ...)      ALOGD("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#ifdef LMDB_DATABASE_DEBUG
#define QLOGI(x, ...)      ALOGI("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QLOGV(x, ...)      ALOGV("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#else //LMDB_DATABASE_DEBUG
#define QLOGI(x, ...)
#define QLOGV(x, ...)
#endif //LMDB_DATABASE_DEBUG
#else //LMDB_UTEST
#define QLOGD(x, ...)      printf("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#ifdef LMDB_DATABASE_DEBUG
#define QLOGI(x, ...)      printf("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#define QLOGV(x, ...)      printf("%s: %s() %d: " x "", LOG_TAG_LMDB, __FUNCTION__ , __LINE__, ##__VA_ARGS__)
#else //LMDB_DATABASE_DEBUG
#define QLOGI(x, ...)
#define QLOGV(x, ...)
#endif //LMDB_DATABASE_DEBUG
#endif //LMDB_UTEST
#else //LMDB_DEBUG
#define QLOGD(x, ...)
#define QLOGI(x, ...)
#define QLOGV(x, ...)
#endif //LMDB_DEBUG

#ifdef LMDB_DATABASE_DEBUG
#define LMDB_DATATABLE_DEBUG
#define BUSY_LIST_DEBUG true
#define QLOGV_DBG(x, ...)               QLOGV(x, __VA_ARGS__)
#else
#define BUSY_LIST_DEBUG false
#define QLOGV_DBG(x, ...)
#endif //LMDB_DATABASE_DEBUG

#define LMDB_DEBUG_SQL_ERR(x, ...)      QLOGE(x, __VA_ARGS__)
#define LMDB_DEBUG_SQL(x, ...)          QLOGV_DBG(x, __VA_ARGS__)
#define LMDB_DEBUG_SQL_DEBUG(x, ...)    QLOGV_DBG(x, __VA_ARGS__)
#define LMDB_DEBUG_SQL_NODEBUG(x, ...)

#define LMDB_DEBUG_LIB(x, ...)          QLOGD(x, __VA_ARGS__)
#define LMDB_DEBUG_UTEST(x, ...)        QLOGD(x, __VA_ARGS__)
#define LMDB_DEBUG_LIB_DETAILED(x, ...) QLOGV_DBG(x, __VA_ARGS__)

#define LMDB_DEBUG_SQL_SHADOW_RD
#define LMDB_DEBUG_SQL_SHADOW_WR
#define LMDB_DEBUG_SQL_SHADOW_DEL
#define LMDB_DEBUG_SQL_SHADOW_COL_LIST


#endif /* LMDB_DEBUG_H */
