/******************************************************************************
 *   @file    FeatureRegistry.h
 *   @brief   Registry Factory pattern for Feature class objects
 *
 *   DESCRIPTION
 *      FeatureRegistry is a template specialization of Registry for Feature
 *   class objects and it takes FeatureInfo as the constructor argument for
 *   Feature constructors.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef FEATUREREGISTRY_H
#define FEATUREREGISTRY_H

#include "Feature.h"
#ifdef LOG_TAG_REG
#undef LOG_TAG_REG
#endif
#define LOG_TAG_REG "FeatureRegistry"
#include "Registry.h"

using FeatureRegistryBase = RegistryBase<Feature, FeatureInfo>;

/******************************************************************************
 * FeatureRegistry provides a factory pattern that enables LM to seamlessly find
 * new Feature classes, instantiate their objects and call their methods with
 * just the name of the feature and using Feature abstract class.
 *
 * This factory pattern ensures no code change is required in LM Core when new
 * Feature classes are added.
 *
 * All Features should
 *      1. Derive from Feature abstract class
 *      2. In header, include a private static member of type FeatureRegistry<FeatureClassName>
 *          private:
 *              static FeatureRegistry<FeatureClassName> registerFeature;
 *      3. In source, instantiate the member with featureName as arg
 *              FeatureRegistry<FeatureClassName> FeatureClassName::registerFeature("FeatureName");
 *
 * Feature1.h:
 * class Feature1 : Feature {
 * .............
 * .............
 * private:
 *      static FeatureRegistry<Feature1> registerFeature;
 * }
 *
 * Feature1.cpp:
 *
 * FeatureRegistry<Feature1> Feature1::registerFeature("Feature1");
 *
 *******************************************************************************/

template<typename TFeature>
using FeatureRegistry = Registry<Feature, FeatureInfo, TFeature>;

extern template FeatureRegistryBase::NameClassMap* FeatureRegistryBase::classList;
extern template std::mutex FeatureRegistryBase::mMutex;

#endif /* FEATUREREGISTRY_H */
