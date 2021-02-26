/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief SLIM Provider configuration file.

This file defines additional SLIM configuration interface.

@ingroup slim_APSS
*/
#ifndef __SLIM_PROVIDER_CONF_H_INCLUDED__
#define __SLIM_PROVIDER_CONF_H_INCLUDED__

#include <slim_provider_data.h>

//! @addtogroup slim_APSS
//! @{

/**
@brief Sets SLIM provider configuration parameters

@param[in] e_Setting     Configured provider setting.
@param[in] q_EnabledMask Mask with @ref slim_ProviderEnumType provider fields.
*/
void slim_ProviderConfSetConfig
(
  slim_ProviderSettingEnumType e_Setting,
  uint32_t q_EnabledMask
);

//! @}

#endif /*__SLIM_PROVIDER_CONF_H_INCLUDED__ */
