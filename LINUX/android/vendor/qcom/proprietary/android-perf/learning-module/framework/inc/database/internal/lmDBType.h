/******************************************************************************
  @file    lmDBType.h
  @brief   Implementation of Learning Module Database

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#ifndef LMDB_TYPE_H_
#define LMDB_TYPE_H_
#include <limits>
#include <string>
#include <mutex>
#include "sqlite3.h"

/* sqlite rowid is 64bit signed integer */
using lmdbInt64Key = sqlite3_int64;
using lmdbIntKey = int;

using DRSize = size_t;
using Name =  std::string;
using TblName = Name;
using Mtx = std::mutex;
using DBFlag = unsigned long int;

#endif /* LMDB_TYPE_H_ */
