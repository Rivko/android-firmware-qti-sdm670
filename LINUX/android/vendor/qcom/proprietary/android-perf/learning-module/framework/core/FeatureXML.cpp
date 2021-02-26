/******************************************************************************
 *   @file    FeatureXML.cpp
 *   @brief   Implementation of XML Parser for Feature XML files
 *
 *   DESCRIPTION
 *      FeatureXML class provides abstraction of XML parsing. It is
 *   self-contained and provides API to read and parse the Feature XML file
 *   for FeatureInfo and Feature Library name. TinyXML2 is used for parsing.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "internal/FeatureXML.h"
#include "AsyncData.h"

#define LOG_TAG_FX "FeatureXML"

using namespace std;
using namespace tinyxml2;

/*
 * Sample XML Text - Please update when making changes:
<Feature>
    <Name>FeatureName</Name>
    <Libname>libfeature.so</Libname>
    <Meters>
        <SamplesRequired>10</SamplesRequired>
        <Meter>
            <Name>MeterName1</Name>
            <Triggers>
                <Trigger>
                    <Primary>true</Primary>
                    <AsyncId>VENDOR_HINT_FIRST_LAUNCH_BOOST</AsyncId>
                    <AsyncType>1</AsyncType>
                </Trigger>
            </Triggers>
            <Samples>5</Samples>
            <Fields>
                <Field>FieldName1</Field>
                <Field>FieldName2</Field>
            </Fields>
            <Configs>
                <Config1Key>Config1Value</Config1Key>
                <Config2Key>Config2Value</Config2Key>
            </Configs>
        </Meter>
        <Meter>
            <Name>MeterName2</Name>
            <Triggers>
                <AsyncId>VENDOR_HINT_FIRST_DRAW</AsyncId>
            </Triggers>
            <Samples>5</Samples>
        </Meter>
    </Meters>
</Feature>
 */

FeatureXML::FeatureXML(string xmlFileName) :
       mXMLRoot(nullptr) {
    int ret_code = mXMLDoc.LoadFile(xmlFileName.c_str());
    if (ret_code != XML_SUCCESS) {
        DEBUGE(LOG_TAG_FX,
                "Unable to load XML file %s : %d, ErrorStr1 = %s,  ErrorStr2 = %s",
                xmlFileName.c_str(), ret_code,
                mXMLDoc.GetErrorStr1(),
                mXMLDoc.GetErrorStr2());
    } else {
        mXMLRoot = mXMLDoc.RootElement();
        if (mXMLRoot == NULL) {
            DEBUGE(LOG_TAG_FX, "Unable to find XMLRoot in %s", xmlFileName.c_str());
        }
    }
}

/* This enables test to specify XML in a string and get it parsed into FeatureInfo */
FeatureXML::FeatureXML(const char* xmlString) :
       mXMLRoot(nullptr) {
    int ret_code = mXMLDoc.Parse(xmlString);
    if (ret_code != XML_SUCCESS) {
        DEBUGE(LOG_TAG_FX,
                "Unable to load XML string %s : %d, ErrorStr1 = %s,  ErrorStr2 = %s",
                xmlString, ret_code, mXMLDoc.GetErrorStr1(),
                mXMLDoc.GetErrorStr2());
    } else {
        mXMLRoot = mXMLDoc.RootElement();
        if (mXMLRoot == NULL) {
            DEBUGE(LOG_TAG_FX, "Unable to find XMLRoot in XML String %s", xmlString);
        }
    }
}

string FeatureXML::getFirstChildText(XMLNode* elem, string name) {
    string childText;
    /* If element is NULL, returns default string (blank) */
    if(elem != NULL) {
        childText = getElementText(elem->FirstChildElement(name.c_str()));
    }
    return childText;
}

string FeatureXML::getElementText(XMLNode* elem) {
    /*
     * This helper function takes an element which contains a text and returns
     * the text as a string
     *          <Name>FeatureName</Name>
     * For element <Name>, it returns "FeatureName"
     * */
    string elemText;
    /* If element is NULL, returns default string (blank) */
    if(elem != NULL) {
        XMLNode* child = elem->FirstChild();
        if(child != NULL) {
            XMLNode* childText = child->ToText();
            if(childText != NULL) {
                elemText = childText->Value();
            }
        }
    }
    return elemText;
}

FeatureInfo FeatureXML::getFeatureInfo() {
    FeatureInfo currFeatureInfo;
    currFeatureInfo.samplesRequired = 0;
    /*
     * If XML file is not loaded/no root is present, return blank FeatureInfo.
     * The caller needs to check FeatureInfo.feature.empty() before any further
     * processing
     */

    /* XML:
     *    <Feature>
     */
    if(mXMLRoot != NULL) {
        /* XML:
         *        <Name>FeatureName</Name>
         */
        currFeatureInfo.name = getFirstChildText(mXMLRoot,"Name");
        /* XML:
         *        <Meters>
         */
        /* Read Each Meter from the xml file */
        XMLElement* metersElem = mXMLRoot->FirstChildElement("Meters");
        if(metersElem != NULL) {
            /* XML:
             *        <SamplesRequired>10</SamplesRequired>
             */
            XMLElement* samplesReqElem = metersElem->FirstChildElement("SamplesRequired");
            string reqSamplesText = getElementText(samplesReqElem);
            DEBUGV(LOG_TAG_FX, "SamplesRequired = %s",reqSamplesText.c_str());
            if(reqSamplesText.size() != 0) {
                if (isdigit(reqSamplesText.at(0))) {
                    currFeatureInfo.samplesRequired = stoi(reqSamplesText);
                }
            }
            /* XML:
            *            <Meter>
            */
            for(XMLElement* currMeter = metersElem->FirstChildElement("Meter"); currMeter != NULL; currMeter = currMeter->NextSiblingElement("Meter")) {
                MeterInfo currMeterInfo;
                currMeterInfo.numberOfSamples = 0;
                /* Read Each Meter's contents from the xml file */
                for(XMLNode* meterChild = currMeter->FirstChildElement(); meterChild != NULL; meterChild = meterChild->NextSiblingElement()) {
                    string meterChildName = meterChild->Value();
                    if(meterChildName == "Name") {
                        /* XML:
                         *    <Name>MeterName1</Name>
                         */
                        /* Read the meter's name */
                        currMeterInfo.name = getElementText(meterChild);
                        DEBUGV(LOG_TAG_FX, "meterName = %s",currMeterInfo.name.c_str());
                    } else if (meterChildName == "Triggers") {
                        /* Read the meter's triggers */
                        for(XMLElement* currTrigger = meterChild->FirstChildElement("Trigger"); currTrigger != NULL; currTrigger = currTrigger->NextSiblingElement("Trigger")) {
                            /* XML:
                             *    <Trigger>
                             */
                            AsyncInfo currAsyncInfo;
                            DEBUGV(LOG_TAG_FX, "Reading Trigger's contents");
                            /* Read Each Trigger's contents from the xml file */
                            for(XMLNode* triggerChild = currTrigger->FirstChildElement(); triggerChild != NULL; triggerChild = triggerChild->NextSiblingElement()) {
                                string triggerChildName = triggerChild->Value();
                                if(triggerChildName == "Primary") {
                                /* XML:
                                 *        <Primary>true</Primary>
                                 */
                                    string primaryText = getElementText(triggerChild);
                                    if(primaryText.size() != 0) {
                                        currAsyncInfo.isPrimary = (primaryText == "true");
                                    }
                                    DEBUGV(LOG_TAG_FX, "isPrimary = %s", currAsyncInfo.isPrimary ? "true" : "false");
                                } else if(triggerChildName == "AsyncId") {
                                /* XML:
                                 *        <AsyncId>VENDOR_HINT_FIRST_LAUNCH_BOOST</AsyncId>
                                 */
                                    string asyncIdName = getElementText(triggerChild);
                                    if(asyncIdName.size() != 0) {
                                        AsyncIdMap enumMap;
                                        currAsyncInfo.hintID = enumMap[asyncIdName];
                                    }
                                    DEBUGV(LOG_TAG_FX, "hintID = 0x%x", currAsyncInfo.hintID);

                                } else if(triggerChildName == "AsyncType") {
                                    /* XML:
                                     *        <AsyncType>1</AsyncType>
                                     */
                                    string asyncType = getElementText(triggerChild);
                                    if(asyncType.size() != 0) {
                                        if (isdigit(asyncType.at(0))) {
                                            currAsyncInfo.hintType = stoi(asyncType);
                                        }
                                    }
                                    DEBUGV(LOG_TAG_FX, "hintType = %d", currAsyncInfo.hintType);
                                }
                            }
                            currMeterInfo.triggersList.insert(currAsyncInfo);
                            /*
                             * Add to feature Async List: unordered_map / unordered_set -> ignores
                             * if already exists
                             */
                            DEBUGV(LOG_TAG_FX,
                                    "Adding asyncId = (0x%x, %d) to FeatureInfo",
                                    currAsyncInfo.hintID,
                                    currAsyncInfo.hintType);
                            currFeatureInfo.asyncIdList[currAsyncInfo.hintID].insert(currAsyncInfo.hintType);
                        }
                    } else if (meterChildName == "Fields") {
                        /* XML:
                         *    <Fields>
                         *        <Field>FieldName1</Field>
                         *        <Field>FieldName2</Field>
                         *    </Fields>
                         */
                        /* Read the meter's fields */
                        for(XMLNode* currField = meterChild->FirstChildElement(); currField != NULL; currField = currField->NextSiblingElement()) {
                            string currFieldName = getElementText(currField);
                            DEBUGV(LOG_TAG_FX, "currFieldName = %s",currFieldName.c_str());
                            currMeterInfo.fieldsList.push_back(currFieldName);
                        }
                    } else if (meterChildName == "Samples") {
                        /* XML:
                         *    <Samples>5</Samples>
                         */
                        /* Read the meter's number of Samples */
                        string numSamplesText = getElementText(meterChild);
                        if(numSamplesText.size() != 0) {
                            if (isdigit(numSamplesText.at(0))) {
                                currMeterInfo.numberOfSamples = stoi(numSamplesText);
                            }
                        }
                        DEBUGV(LOG_TAG_FX, "numberOfSamples = %d",currMeterInfo.numberOfSamples);
                    } else if (meterChildName == "Configs") {
                        /* XML:
                         *    <Configs>
                         *        <Config1Key>Config1Value</Config1Key>
                         *        <Config2Key>Config2Value</Config2Key>
                         *    </Configs>
                         */
                        /* Read the meter's configs */
                        for(XMLNode* currConfig = meterChild->FirstChildElement(); currConfig != NULL; currConfig = currConfig->NextSiblingElement()) {
                            string currConfigKey = currConfig->Value();
                            string currConfigValue = getElementText(currConfig);
                            DEBUGV(LOG_TAG_FX, "currConfig[%s] = %s",currConfigKey.c_str(),currConfigValue.c_str());
                            currMeterInfo.configsList[currConfigKey] = currConfigValue;
                        }
                    }
                }
                currFeatureInfo.meterInfoList.push_back(currMeterInfo);
            }
        }
        /* XML:
         *        <IgnoredApps>
         */
        /* Read Each ignored app from the xml file */
        XMLElement* ignoredAppsElem = mXMLRoot->FirstChildElement("IgnoredApps");
        if(ignoredAppsElem != NULL) {
            /* XML:
             *            <App>
             */
            for(XMLElement* currApp = ignoredAppsElem->FirstChildElement("App"); currApp != NULL; currApp = currApp->NextSiblingElement("App")) {
                std::string appName = getElementText(currApp);
                DEBUGV(LOG_TAG_FX, "appName = %s", appName.c_str());
                currFeatureInfo.ignoredAppsList.push_back(appName);
            }
        }
    }

#ifdef DEBUG_FEATUREINFO
    DEBUGV(LOG_TAG_FX, "currFeatureInfo = %s",currFeatureInfo.toString().c_str());
#endif
    // return by value in C++ 11 calls move constructor if present => no copy
    return currFeatureInfo;
}

string FeatureXML::getFeatureLibName() {
    string currLibName;
    if(mXMLRoot) {
        /* XML:
         *        <Libname>libfeature.so</Libname>
         */
        currLibName = getFirstChildText(mXMLRoot,"Libname");
    } else {
        DEBUGE(LOG_TAG_FX, "Fatal: No XmlRoot in XML File");
    }
    return currLibName;
}
