/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include "MainLoop.h"
#include "Command.h"
#include "ClientSocket.h"
#include "ProtoSource.h"
#include "AndroidLogger.h"
#include "Logger.h"
#include "settings.h"
#include <thread>
#include <memory>
#include <sstream>

#define TAG "RILQ"

#include <framework/Log.h>

#define CLIENT_PROTO_SOCKET  ClientSocket<ProtoSource>::getUnix("/dev/socket/settingsd")
#define REMOTE_REGISTRY     "remote"
#define UPDATE_CARRIER_CMD  "update_carrier"
using namespace QcSettingsD;

void main_loop_thread();
static std::thread threadp;

void settingsd_client_start(void)
{
    QCRIL_LOG_ESSENTIAL("Starting settingsd client thread");
    threadp = std::thread(main_loop_thread);
    threadp.detach();
    QCRIL_LOG_ESSENTIAL("After starting the settingsd client thread");
}

void update_carrier_callback
(
    bool res,
    QcSettingsD::Command::event_type             evt_type,
    const std::shared_ptr<QcSettingsD::Command>  cmd,
    const CommandParameters        &pars,
    CommandResults                 &results
)
{
    (void) evt_type;
    (void) cmd;
    (void) pars;
    QCRIL_HAL_LOG_DEBUG("Execution for update_carrier command completed with %s", res ? "success" : "failure");
    for ( CommandParameter par: results.getParamList() )
    {
        QCRIL_HAL_LOG_DEBUG("==> %s", par.getString().c_str());
    }
}

void settingsd_client_update_iccid_card
(
    qcril_instance_id_e_type instance_id,
    char (*mcc)[QCRIL_MCC_MNC_MAX_SIZE],
    char (*mnc)[QCRIL_MCC_MNC_MAX_SIZE],
    char (*iccid)[QCRIL_UIM_ICCID_LEN]
)
{
    std::shared_ptr<QcSettingsD::Command> cmd = CommandRegistry::findCommand(REMOTE_REGISTRY, UPDATE_CARRIER_CMD);
    if (cmd)
    {
        std::ostringstream os;
        os << (int) instance_id;
        std::string s_instance_id = os.str();
        CommandParameters params = {std::string(s_instance_id), std::string(*mcc), std::string(*mnc), std::string(*iccid)};
        CommandResults   results;
        cmd->enqueue(params, results, update_carrier_callback);
    }
    else
    {
        QCRIL_HAL_LOG_HIGH("Unable to find command %s in registry %s", UPDATE_CARRIER_CMD, REMOTE_REGISTRY);
    }
}

void settingsd_client_indication_cb
(
    const std::string &id,
    const CommandResults &results
)
{
    QCRIL_HAL_LOG_HIGH("Received indication id %s", id.c_str());
    for (const CommandParameter &p: results.getParamList())
    {
        QCRIL_HAL_LOG_DEBUG("==> %s", p.getString().c_str());
    }
    if (id == "MSG_PROPERTY_UPDATE_COMPLETED")
    {
        if (results.size() > 0)
        {
            exit(0);
        }
    }
}

void main_loop_thread()
{
    QCRIL_LOG_ESSENTIAL("main_loop_thread started");
    QCRIL_HAL_LOG_ENTER("Starting main Loop thread for settings client");
    std::unique_ptr<ClientSocket<ProtoSource>> cps = CLIENT_PROTO_SOCKET;
    if (!cps->connect())
    {
        std::unique_ptr<EventSource> ces = std::move(cps);
        MainLoop::addEventSource(ces);
        CommandEvent::register_for_indication(CommandEvent::indication_cb_t(settingsd_client_indication_cb));
        MainLoop::startLoop();
    }
    else
    {
        QCRIL_HAL_LOG_HIGH("Unable to start settings client");
    }
   QCRIL_HAL_LOG_LEAVE();
}
#endif
