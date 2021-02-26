/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "module/IDataModule.h"
#include "local/DataModule.h"
namespace rildata {

IDataModule* IDataModule::makeModule() {
    return dynamic_cast<IDataModule*>(new DataModule());
}

}// namespace
