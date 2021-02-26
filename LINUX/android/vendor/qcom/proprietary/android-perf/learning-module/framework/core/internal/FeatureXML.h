/******************************************************************************
 *   @file    FeatureXML.h
 *   @brief   XML Parser for Feature XML files
 *
 *   DESCRIPTION
 *      FeatureXML class provides abstraction of XML parsing. It is
 *   self-contained and provides API to read and parse the Feature XML file
 *   for FeatureInfo and Feature Library name.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef FEATUREXML_H
#define FEATUREXML_H

#include <string>
#include <tinyxml2.h>
#include "FeatureInfo.h"
#include "DebugLog.h"

class FeatureXML {
private:
    tinyxml2::XMLDocument mXMLDoc;
    tinyxml2::XMLElement* mXMLRoot;
    std::string getFirstChildText(tinyxml2::XMLNode* elem, std::string name);
    std::string getElementText(tinyxml2::XMLNode* elem);

public:
    FeatureXML(std::string xmlFileName);
    FeatureXML(const char* xmlString);
    FeatureInfo getFeatureInfo();
    std::string getFeatureLibName();
};

#endif /* FEATUREXML_H */
