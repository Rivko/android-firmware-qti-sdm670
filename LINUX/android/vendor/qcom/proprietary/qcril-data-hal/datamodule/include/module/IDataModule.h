/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Module.h"

namespace rildata {
/**
    IDataModule is the public interface to create Data Module
*/
class IDataModule : public Module {
 public:
  static IDataModule* makeModule();
  virtual void init() = 0;
  virtual ~IDataModule() {}
  IDataModule(const IDataModule&) =delete;
  IDataModule& operator=(const IDataModule&) =delete;
  // void setImsDataModuleInterface(
  //     std::shared_ptr<ImsDataModuleInterface> imsDataModuleInterface);
protected:
  IDataModule() = default;

};

} //namespace

