/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/nas/NasSetInitialAttachApnRequestMessage.h"
#include "modules/android/version.h"
#include "telephony/ril.h"

#ifndef QCRIL_RIL_VERSION
#error "undefined QCRIL_RIL_VERSION!"
#endif

bool NasSetInitialAttachApnRequestMessage::allocate_and_copy_data(const char *src, char **des)
{
    if (!des)
      return false;

    bool res = true;
    size_t len = src? (strlen(src) + 1) : 0;
    if(len > 0) {
        *des = new char[len];
        if (*des) {
            memcpy(*des, src, len);
        } else {
            res = false;
        }
    } else {
        *des = nullptr;
    }

    return res;
}

NasSetInitialAttachApnRequestMessage::~NasSetInitialAttachApnRequestMessage() {
  if (params != nullptr) {
#if (QCRIL_RIL_VERSION < 15)
    RIL_InitialAttachApn *apninfo = (RIL_InitialAttachApn*)params->data;
#else
    RIL_InitialAttachApn_v15 *apninfo = (RIL_InitialAttachApn_v15*)params->data;
#endif

    if(apninfo) {
        if(apninfo->apn) {
            delete []apninfo->apn;
        }

        if(apninfo->protocol) {
            delete []apninfo->protocol;
        }

        if(apninfo->username) {
            delete []apninfo->username;
        }

        if(apninfo->password) {
            delete []apninfo->password;
        }

#if (QCRIL_RIL_VERSION >= 15)
        if (apninfo->roamingProtocol) {
            delete []apninfo->roamingProtocol;
        }
        if (apninfo->mvnoType) {
            delete []apninfo->mvnoType;
        }
        if (apninfo->mvnoMatchData) {
            delete []apninfo->mvnoMatchData;
        }
#endif
        delete apninfo;
    }

    delete params;
  }
}

void NasSetInitialAttachApnRequestMessage::deepCopy(const ril_request_type &request) {
#if (QCRIL_RIL_VERSION < 15)
  RIL_InitialAttachApn *apninfo = nullptr;
#else
  RIL_InitialAttachApn_v15 *apninfo = nullptr;
#endif
  bool success = false;

  do {
    params = new qcril_request_params_type;
    if (!params)
      break;

    // fill legacy params
    params->instance_id = request.instance_id;
    params->modem_id = QCRIL_DEFAULT_MODEM_ID;
    params->event_id = request.req_id;
    params->t = request.t;
    params->datalen = request.payload_len;
    params->data = nullptr;

#if (QCRIL_RIL_VERSION < 15)
    apninfo = new RIL_InitialAttachApn;
#else
    apninfo = new RIL_InitialAttachApn_v15;
#endif
    if (!apninfo)
      break;
    // reset all pointers as nullptr to avoid unexpcted free when fail
    memset(apninfo, 0, sizeof(*apninfo));

#if (QCRIL_RIL_VERSION < 15)
    RIL_InitialAttachApn* payload = (RIL_InitialAttachApn*)request.payload;
#else
    RIL_InitialAttachApn_v15* payload = (RIL_InitialAttachApn_v15*)request.payload;
#endif
    if (!payload)
      break;

    apninfo->authtype = payload->authtype;
#if (QCRIL_RIL_VERSION >= 15)
    apninfo->supportedTypesBitmask = payload->supportedTypesBitmask;
    apninfo->bearerBitmask = payload->bearerBitmask;
    apninfo->modemCognitive = payload->modemCognitive;
    apninfo->mtu = payload->mtu;
#endif

    if(allocate_and_copy_data(payload->apn, &apninfo->apn) == false)
      break;

    if(allocate_and_copy_data(payload->protocol, &apninfo->protocol) == false)
      break;

    if(allocate_and_copy_data(payload->password, &apninfo->password) == false)
      break;

    if(allocate_and_copy_data(payload->username, &apninfo->username) == false)
      break;

#if (QCRIL_RIL_VERSION >= 15)
    if (allocate_and_copy_data(payload->roamingProtocol, &apninfo->roamingProtocol) == false)
      break;

    if (allocate_and_copy_data(payload->mvnoType, &apninfo->mvnoType) == false)
      break;

    if (allocate_and_copy_data(payload->mvnoMatchData, &apninfo->mvnoMatchData) == false)
      break;
#endif

    success = true;
  } while (false);

  if (!success) { // free the allocated memory
    if (apninfo) {
      if (apninfo->apn)
        delete []apninfo->apn;
      if (apninfo->protocol)
        delete []apninfo->protocol;
      if (apninfo->username)
        delete []apninfo->username;
      if (apninfo->password)
        delete []apninfo->password;
#if (QCRIL_RIL_VERSION >= 15)
      if (apninfo->roamingProtocol)
        delete []apninfo->roamingProtocol;
      if (apninfo->mvnoType)
        delete []apninfo->mvnoType;
      if (apninfo->mvnoMatchData)
        delete []apninfo->mvnoMatchData;
#endif
      delete apninfo;
    }
  } else {
    params->data = (void*)apninfo;
  }
}

string NasSetInitialAttachApnRequestMessage::dump() {
  return mName;
}

