/**
  @file provision.h
  @brief Include file for Chamomile Provision.

  This header file contains the provision interface to provisioning
  libraries for chamomile.
*/

/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/

#pragma once

/** @name Provision interface.

  Interface to provisioning library. Takes pData after provisioning by
  chamomile_provision, forwards it to SA to provide to appropriate STA.
  Response from STA returned in pResponse.

  @param[in] pData Encapsulated data to be provided to STA
  @param[in] dataLen Length of encapsulated data
  @param[out] pResponse Response from STA
  @param[out] responseLen Length of response

  @return int - 0 on Success, STA error codes on failure
 */
typedef int (*provision_t)(uint8_t*, size_t, uint8_t*, size_t*);
