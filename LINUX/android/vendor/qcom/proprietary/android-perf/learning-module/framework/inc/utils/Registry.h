/******************************************************************************
 *   @file    Registry.h
 *   @brief   Generic Registry Factory pattern
 *
 *   DESCRIPTION
 *      Registry is a generic registry factory pattern which provides auto
 *   registration and lookup of derived classes based on base class.
 *   This template takes two arguments - TBase, which should be used for the
 *   Base class and TInfo, which should be used as argument for Base class
 *   constructor. TInfo is expected to have a string member called name -
 *   if that is not implemented, template specialization is required for
 *   RegistryBase::get() with appropriate argument.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef REGISTRY_H
#define REGISTRY_H

#include <unordered_map>
#include <string>
#include <mutex>
#include "DebugLog.h"

#ifndef LOG_TAG_REG
#define LOG_TAG_REG "Registry"
#endif

template <typename TBase, typename TInfo>
class RegistryBase {
public:
    using NameClassMap = std::unordered_map<std::string, TBase*(*)(TInfo)>;

    template <typename TArg = TInfo>
    static TBase* get(TArg cInfo) {
        std::string className = cInfo.name;
        std::lock_guard<std::mutex> lock(mMutex);
        auto classMap = getMap();
        if(!classMap) {
            DEBUGE(LOG_TAG_REG,"getMap() returned nullptr");
            return nullptr;
        }
        auto currFeature = classMap->find(className);
        DEBUGV(LOG_TAG_REG,"get %s", className.c_str());
        if(currFeature == classMap->end()) {
            DEBUGV(LOG_TAG_REG,"getMap()->end() %p->end() %s", getMap(), className.c_str());
            return nullptr;
        }
        return currFeature->second(cInfo);
    }

protected:
    static std::mutex mMutex;
    /* TODO: Explore moving to Meyer's singleton */
    static NameClassMap* getMap() {
        if(!classList) {
            classList = new NameClassMap;
        }
        return classList;
    }

private:
    static NameClassMap* classList;
};

template<typename TBase, typename TInfo, typename TDerived> TBase* createClassObj(TInfo info) { return new TDerived(info); }

template<typename TBase, typename TInfo, typename TDerived>
class Registry : RegistryBase <TBase, TInfo> {
public:
    Registry(std::string className) {
        std::lock_guard<std::mutex> lock(RegistryBase<TBase, TInfo>::mMutex);
        DEBUGV(LOG_TAG_REG,"Registering Class %s", className.c_str());
        auto classMap = RegistryBase<TBase, TInfo>::getMap();
        if(classMap) {
            classMap->insert(std::make_pair(className, &createClassObj<TBase,TInfo,TDerived>));
            DEBUGV(LOG_TAG_REG,"classList is %p", classMap);
        } else {
            DEBUGE(LOG_TAG_REG,"getMap() returned nullptr, Class %s not registered", className.c_str());
        }
    }
};

#endif /* REGISTRY_H */
