/******************************************************************************
 *   @file    MeterRegistry.h
 *   @brief   Registry Factory pattern for Meter class objects
 *
 *   DESCRIPTION
 *      MeterRegistry is a template specialization of Registry for Meter class
 *   objects and it takes MeterInfo as the constructor argument for Meter
 *   constructors.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef METERREGISTRY_H
#define METERREGISTRY_H

#include "Meter.h"
#ifdef LOG_TAG_REG
#undef LOG_TAG_REG
#endif
#define LOG_TAG_REG "MeterRegistry"
#include "Registry.h"

using MeterRegistryBase = RegistryBase<Meter, MeterInfo>;

/******************************************************************************
 * MeterRegistry provides a factory pattern that enables LM to seamlessly find
 * new Meter classes, instantiate their objects and call their methods with
 * just the name of the meter and using Meter abstract class.
 *
 * This factory pattern ensures no code change is required in LM Core when new
 * Meter classes are added.
 *
 * All Meters should
 *      1. Derive from Meter abstract class
 *      2. In header, include a private static member of type MeterRegistry<MeterClassName>
 *          private:
 *              static MeterRegistry<MeterClassName> registerMeter;
 *      3. In source, instantiate the member with meterName as arg
 *              MeterRegistry<MeterClassName> MeterClassName::registerMeter("MeterName");
 *
 * Meter1.h:
 * class Meter1 : Meter {
 * .............
 * .............
 * private:
 *      static MeterRegistry<Meter1> registerMeter;
 * }
 *
 * Meter1.cpp:
 *
 * MeterRegistry<Meter1> Meter1::registerMeter("Meter1");
 *
 *******************************************************************************/

template<typename TMeter>
using MeterRegistry = Registry<Meter, MeterInfo, TMeter>;

extern template MeterRegistryBase::NameClassMap* MeterRegistryBase::classList;
extern template std::mutex MeterRegistryBase::mMutex;

#endif /* METERREGISTRY_H */
