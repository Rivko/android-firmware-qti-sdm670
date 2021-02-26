/*-------------------------------------------------------------------
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/

#ifndef _VTEST_XML_PARSER_H
#define _VTEST_XML_PARSER_H

#include <libxml/tree.h>
#include "vtest_XmlComdef.h"

namespace vtest {

class XmlParser {

public:
    XmlParser();
    ~XmlParser();
    OMX_ERRORTYPE ResolveMasterXmlPath(OMX_STRING inputstr, OMX_STRING pMasterXmlLocation);
    OMX_ERRORTYPE ProcessMasterConfig(VideoStaticProperties* pGlobalVideoProp);
    OMX_ERRORTYPE ParseSessionXml(OMX_STRING sessionXml, VideoStaticProperties* pGlobalVideoProp, VideoSessionInfo* pSessionInfo);

private:

    OMX_STRING m_pMasterXmlLocation;
    OMX_STRING m_pMasterXmlFullPath;
    OMX_STRING m_sessionXml;
    OMX_STRING m_testId;

    OMX_ERRORTYPE ProcessSessionTag(VideoStaticProperties* pGlobalVideoProp, VideoSessionInfo* pSessionInfo, xmlNodePtr pXmlNode);

};

}


#endif // #ifndef _VTEST_XML_PARSER_H
