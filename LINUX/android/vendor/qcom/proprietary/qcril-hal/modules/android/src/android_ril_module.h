#pragma once
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <map>
#include <telephony/ril.h>
#include <framework/Module.h>
#include <framework/legacy.h>
#include <modules/android/version.h>

struct factory_info_t;


class android_ril_module: public Module {
    private:
        static struct RIL_Env local_env;
#ifdef RIL_SHLIB
        const RIL_RadioFunctions *qcril_legacy_functions[ QCRIL_MAX_INSTANCE_ID ];
        /*!< Functions for ril to call */
#endif /* RIL_SHLIB */

        void handleRegisterRequest(std::shared_ptr<Message> msg);
        //ResourceList<unsigned, factory_info_t> factories;
        std::map<int, std::shared_ptr<factory_info_t>> factories;
    public:
        android_ril_module();
        bool dispatch_request(qcril_instance_id_e_type instance_id, int req_id, void *data, size_t size, RIL_Token t);
        int is_request_supported(qcril_instance_id_e_type instance_id, int req_id);
        bool is_dispatch_allowed(qcril_instance_id_e_type instance_id, int req_id);
        void ril_init
        (
          qcril_instance_id_e_type instance_id,
          const struct RIL_Env *env,
          int argc,
          char **argv
        );
        RIL_RadioState currentState
        (
          qcril_instance_id_e_type instance_id
        );
        void send_response(qcril_instance_id_e_type instance_id, RIL_Token token_id, RIL_Errno e, void *response, size_t response_len);
        void send_response(std::shared_ptr<ril_response_type> rsp);
        void send_request_ack
        (
            RIL_Token token
        );
        void setup_timed_callback
        (
            qcril_instance_id_e_type instance_id,
            RIL_TimedCallback callback,
            void *param,
            const struct timeval *relativeTime
        );
        ~android_ril_module(){}
};

android_ril_module &get_android_ril_module();
