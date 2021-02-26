/******************************************************************************
  @file  XmlParser.cpp`
  @brief  xmlparser module to parse xml files and notify registered clients

  xmlparser module to parse xml files and notify registered clients

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "XmlParser.h"

#define LOG_TAG "ANDR-PERF-XMLPARSER"
#include <cutils/log.h>
#include <cutils/properties.h>

#if QTI_DEBUG
#  define QLOGE(...)    ALOGE(__VA_ARGS__)
#  define QLOGD(...)    ALOGD(__VA_ARGS__)
#  define QLOGW(...)    ALOGW(__VA_ARGS__)
#  define QLOGI(...)    ALOGI(__VA_ARGS__)
#  define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#  define QLOGE(...)
#  define QLOGD(...)
#  define QLOGW(...)
#  define QLOGI(...)
#  define QLOGV(...)
#endif

using namespace std;

AppsListXmlParser::AppsListXmlParser() {
    memset(mParserClients, 0x00, MAX_CALLBACKS*sizeof(AppsListNode));
}

AppsListXmlParser::~AppsListXmlParser() {
}

int AppsListXmlParser::Register(const string xmlRoot,
                                const string xmlChildToParse,
                                void (*parseElem)(xmlNodePtr, void *),
                                void *data) {
    int ret = -1;
    if (NULL == parseElem) {
        QLOGE("No parsing function specified for XML");
        return -1;
    }

    for (int i=0; i<MAX_CALLBACKS; i++) {
        if (!mParserClients[i].mRegistered) {
            mParserClients[i].mParseElem = parseElem;
            mXmlRoot = xmlRoot;
            mParserClients[i].mXmlChildToParse = xmlChildToParse;
            mParserClients[i].mData = data;
            mParserClients[i].mRegistered = true;
            ret = i;
            break;
        }
    }
    return ret;
}

bool AppsListXmlParser::DeRegister(int idx){
    if (idx <0 && idx >= MAX_CALLBACKS) {
        QLOGE("Could do not de-register the callback");
        return false;
    }
    mParserClients[idx].mParseElem = NULL;
    mParserClients[idx].mXmlChildToParse = "";
    mParserClients[idx].mRegistered = false;
    return true;
}

int AppsListXmlParser::Parse(const string fName) {
    xmlDocPtr doc;
    xmlNodePtr currNode;
    short int idx = -1;

    if (access(fName.c_str(), F_OK) < 0) {
        QLOGE("Could not access the XML file at %s", fName.c_str());
        return -1;
    }

    doc = xmlReadFile(fName.c_str(), "UTF-8", XML_PARSE_RECOVER);
    if(!doc) {
        QLOGE("XML Document not parsed successfully");
        return -1;
    }

    currNode = xmlDocGetRootElement(doc);
    if(!currNode) {
        QLOGE("Empty document");
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return -1;
    }

    // Confirm the root-element of the tree
    if(!IsRoot(currNode->name)) {
        QLOGE("Document of the wrong type, root node != root");
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return -1;
    }

    currNode = currNode->xmlChildrenNode;

    for(; currNode != NULL; currNode=currNode->next) {
        if(currNode->type != XML_ELEMENT_NODE)
            continue;

        QLOGI("Parsing the whitelist of %s",currNode->name);

        xmlNodePtr node = currNode;

        idx = FindRootsChildInRegisteredClients(currNode->name);
        if(idx >= 0) {
            node = node->children;
            while(node != NULL) {
                //Call the function pointer to populate
                mParserClients[idx].mParseElem(node, mParserClients[idx].mData);
                QLOGI("parsed the node moving on to next");
                node = node->next;
            }
        }
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}

bool AppsListXmlParser::IsRoot(const xmlChar* root) {
    bool ret = false;
    if (!xmlStrcmp(root, BAD_CAST mXmlRoot.c_str())) {
            ret = true;
    }
    return ret;
}

short int AppsListXmlParser::FindRootsChildInRegisteredClients(const xmlChar* child) {
    short int ret = -1;
    for (int i=0; i<MAX_CALLBACKS; i++) {
        //if checks for mParseElem
        if (!xmlStrcmp(child, BAD_CAST mParserClients[i].mXmlChildToParse.c_str())) {
            ret = i;
        }
    }
    return ret;
}
