/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/message_id.h>
#include <framework/Dispatcher.h>

template <typename T>
class add_message_id
{
    public:
        static inline message_id_t get_class_message_id() {
            static message_id_t id = REG_MSG(T::MESSAGE_NAME);
            return id;
        }
};
