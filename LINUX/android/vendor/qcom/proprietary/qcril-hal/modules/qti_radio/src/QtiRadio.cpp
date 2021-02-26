/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include <framework/QcrilInitMessage.h>
#include "QtiRadio.h"
#include <framework/Log.h>
#include "qcril_qmi_qti_radio_service.h"

static load_module<QtiRadio> sQtiRadio;

QtiRadio *getQtiRadio() {
    return &(sQtiRadio.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
QtiRadio::QtiRadio() {
  mName = "QtiRadio";

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage, QtiRadio::handleQcrilInit)
  };
}

/* Follow RAII.
*/
QtiRadio::~QtiRadio() {}

/*
 * Module specific initialization that does not belong to RAII .
 */
void QtiRadio::init() {
  Module::init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void QtiRadio::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  Log::getInstance().d("[" + mName + "]: get_instance_id = "
        + std::to_string(msg->get_instance_id()));
  /* Init qti_radio hidl services.*/
  vendor::qti::hardware::radio::qtiradio::V1_0::implementation::
        qcril_qmi_qti_radio_service_init(msg->get_instance_id());
}

