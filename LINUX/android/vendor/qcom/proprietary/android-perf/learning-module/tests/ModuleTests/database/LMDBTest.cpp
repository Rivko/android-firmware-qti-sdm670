/******************************************************************************
  @file    LMDBTest.cpp
  @brief   Implementation of Learning Module Database test

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/

#include "LMDBTest.h"

int LMDBTest () {
    //call only once
    LMDBGlobal::LMDBGlobalConfig();

    iopDatabase* tmp = iopDatabase::getInstance();
    delete tmp;

    //call only once
    LMDBGlobal::LMDBGlobalDeConfig();
    return 0;
}
