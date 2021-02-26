/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#ifdef __cplusplus
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>
#include <functional>

class Module;
class Dispatcher;

#endif

struct message_id_info;
typedef struct message_id_info *message_id_t;

#ifdef __cplusplus
struct message_id_info
{
    friend class Dispatcher;
    private:
        std::shared_ptr<std::string> m_name;
        std::vector<Module*> module_list;

    public:
        size_t idx = 0;
        message_id_info(): m_name(std::make_shared<std::string>("")) { }


        message_id_info(const std::string &name): m_name(std::make_shared<std::string>(name)) {}
        message_id_info(const message_id_info &other):
            m_name(other.m_name),
            module_list(other.module_list),
            idx(other.idx) {}

        message_id_info(message_id_info &&other):
            m_name(std::move(other.m_name)),
            module_list(std::move(other.module_list)),
            idx(other.idx) {}

        message_id_info &operator=(const message_id_info &other) {
            m_name = other.m_name;
            module_list = other.module_list;
            idx = other.idx;
            return *this;
        }
        const std::string get_name() const { return m_name ? *m_name : "<Invalid>"; };
        int get_idx() const { return idx; }

        ~message_id_info() {
        }
};

template<>
struct std::hash<message_id_t>
{
    size_t operator() (message_id_t id) const {
        size_t ret = 0;
        if (id) {
            std::hash<size_t> h;
            ret = h(id->idx);
        }
        return ret;
    }
};

template<>
struct std::equal_to<message_id_t>
{
    bool operator() (const message_id_t &lhs, const message_id_t &rhs) const {
        bool ret = false;
        std::equal_to<size_t> h;
        if ( lhs && rhs ) {
            ret = h(lhs->idx, rhs->idx);
        }
        return ret;
    }
};
#endif