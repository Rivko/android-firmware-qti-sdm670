/******************************************************************************
  @file  XmlParser.h
  @brief  xmlparser module to parse xml files and notify registered clients

  xmlparser module to parse xml files and notify registered clients

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef __XMLPARSE_H__
#define __XMLPARSE_H__

#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;

#define MAX_CALLBACKS 6

typedef void  (*ParseElem)(xmlNodePtr, void *);

struct AppsListNode {
    ParseElem mParseElem;
    string mXmlChildToParse;
    void *mData;
    bool mRegistered;
};

class AppsListXmlParser {
private:
    AppsListNode mParserClients[MAX_CALLBACKS];
    string mXmlRoot;
public:
    AppsListXmlParser();
    ~AppsListXmlParser();
    int Register(const string xmlRoot, const string xmlChildToParse,
                 void (*parseElem)(xmlNodePtr, void *), void *data);
    bool DeRegister(int idx);
    int Parse(const string xmlFName);
private:
    bool IsRoot(const xmlChar* root);
    short int FindRootsChildInRegisteredClients(const xmlChar* child);
};
#endif
