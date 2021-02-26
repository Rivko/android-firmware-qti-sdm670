/*-------------------------------------------------------------------
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/
#include <sys/stat.h>
#include <libxml/tree.h>
#include "vtest_XmlComdef.h"
#include "vtest_XmlParser.h"
#include "vtest_XmlParserHelper.h"
#include "vtest_Debug.h"
#include "vtest_ITestCase.h"
#include "vtest_TestCaseFactory.h"
#include "vtest_ComDef.h"

#undef LOG_TAG
#define LOG_TAG "VTEST_XML_PARSER"

namespace vtest {

XmlParser::XmlParser()
    :m_pMasterXmlLocation(NULL),
     m_pMasterXmlFullPath(NULL)
{
    m_sessionXml = new char[MAX_STR_LEN];
    m_testId = new char[MAX_STR_LEN];
}

XmlParser::~XmlParser()
{
    delete []m_sessionXml;
    delete []m_testId;
}

OMX_ERRORTYPE XmlParser::ResolveMasterXmlPath(OMX_STRING inputstr, OMX_STRING pMasterXmlLocation)
{
    struct stat s;
    int i = 0;
    OMX_ERRORTYPE res = OMX_ErrorNone;
    char sTemp[MAX_STR_LEN] = "";
    const char* sTemp1 = ".";
    i = stat(inputstr, &s);
    if (stat(inputstr, &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            m_pMasterXmlLocation = strdup(inputstr);
            m_pMasterXmlFullPath = (OMX_STRING)malloc(strlen(inputstr) + strlen("MasterConfg.xml") + 5);
            if (m_pMasterXmlFullPath) {
                SNPRINTF(m_pMasterXmlFullPath, strlen(inputstr) + strlen("MasterConfg.xml") + 5,
                        "%s/MasterConfg.xml", inputstr);
            }
        }
        else if (s.st_mode & S_IFREG) {
            char *cp = NULL;
            VTEST_MSG_LOW("%s", inputstr);
            m_pMasterXmlFullPath = strdup(sTemp);
            cp = strrchr(sTemp, '/');
            if (cp) {
                *cp = 0x0;
                m_pMasterXmlLocation = strdup(sTemp);
            } else {
                m_pMasterXmlLocation = strdup(sTemp1);
            }
        } else {
            res = OMX_ErrorUndefined;
            m_pMasterXmlLocation = m_pMasterXmlFullPath = NULL;
        }
        if (m_pMasterXmlLocation)
            STRLCPY(pMasterXmlLocation, m_pMasterXmlLocation, MAX_STR_LEN);
    } else {
        res = OMX_ErrorUndefined;
    }

    return res;
}


OMX_ERRORTYPE XmlParser::ProcessMasterConfig(VideoStaticProperties* pGlobalVideoProp)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    xmlNodePtr pElem;
    xmlDocPtr pMasterDoc = xmlParseFile(m_pMasterXmlFullPath);

    if (!pMasterDoc) {
        VTEST_MSG_ERROR("Unable to open %s or Invalid MasterConfg.xml", m_pMasterXmlFullPath);
        return status;
    }
    pElem = xmlDocGetRootElement(pMasterDoc);

    memset(pGlobalVideoProp, 0x00, sizeof(VideoStaticProperties));
    pGlobalVideoProp->sCRCRoot[0]   = '\0';
    pGlobalVideoProp->sMISRRoot[0] = '\0';
    pGlobalVideoProp->sInputRoot[0] = '\0';
    pGlobalVideoProp->sOutRoot[0]   = '\0';
    pGlobalVideoProp->sPqRoot[0]    = '\0';
    pGlobalVideoProp->sDecFlowXml[0] = '\0';
    pGlobalVideoProp->sEncFlowXml[0] = '\0';
    pGlobalVideoProp->logMask = 0x3;
    pGlobalVideoProp->bDynamicBufMode = 1;
    pGlobalVideoProp->bSinkType = 0;
    pGlobalVideoProp->nDisplayBufCnt = 2;
    pGlobalVideoProp->nPollTimeout = 20000;
    pGlobalVideoProp->nDefaultWidth = 176;
    pGlobalVideoProp->nDefaultHeight = 144;
    pGlobalVideoProp->nThreadTimeout = 20000;
    pGlobalVideoProp->nMinFrameDelay = 0;
    pGlobalVideoProp->bGenerateCrc = 0;
    pGlobalVideoProp->bGenerateMisr = 0;
    pGlobalVideoProp->bSkipCrcCheck = 0;

    if (pElem == NULL) {
        xmlFreeDoc(pMasterDoc);
        VTEST_MSG_ERROR("Document is Empty!!!");
        return OMX_ErrorBadParameter;
    }

    pElem = pElem->xmlChildrenNode;  //<xml> tag
    while (pElem)
    {

        if (!STRCMP((OMX_STRING)pElem->name, "InputFileRoot")) {
            STRCPY(pGlobalVideoProp->sInputRoot, (OMX_STRING)pElem->children->content);
            VTEST_MSG_LOW("MasterXml: InputFileRoot: %s", pGlobalVideoProp->sInputRoot);
        } else if (!STRCMP((OMX_STRING)pElem->name, "OutputFileRoot")) {
            STRCPY(pGlobalVideoProp->sOutRoot, (OMX_STRING)pElem->children->content);
            VTEST_MSG_LOW("MasterXml: OutputFileRoot: %s", pGlobalVideoProp->sOutRoot);
        } else if (!STRCMP((OMX_STRING)pElem->name, "CrcFileRoot")) {
            STRCPY(pGlobalVideoProp->sCRCRoot, (OMX_STRING)pElem->children->content);
            VTEST_MSG_LOW("MasterXml: CrcFileRoot: %s", pGlobalVideoProp->sCRCRoot);
        } else if (!STRCMP((OMX_STRING)pElem->name, "MisrFileRoot")) {
            STRCPY(pGlobalVideoProp->sMISRRoot, (OMX_STRING)pElem->children->content);
            VTEST_MSG_LOW("MasterXml: MisrFileRoot: %s", pGlobalVideoProp->sMISRRoot);
        } else if (!STRCMP((OMX_STRING)pElem->name, "PQFileRootPath")) {
            STRCPY(pGlobalVideoProp->sPqRoot, (OMX_STRING)pElem->children->content);
            VTEST_MSG_LOW("MasterXml: RoiQpRoot: %s", pGlobalVideoProp->sPqRoot);
        } else if (!STRCMP((OMX_STRING)pElem->name, "LogMask")) {
            sscanf((OMX_STRING)pElem->children->content, "0x%x", &pGlobalVideoProp->logMask );
            VTEST_MSG_LOW("MasterXml: LogMask: 0x%x", pGlobalVideoProp->logMask);
        } else if (!STRCMP((OMX_STRING)pElem->name, "InputDump")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->inputDump);
            VTEST_MSG_LOW("MasterXml: InputDump: %u", pGlobalVideoProp->inputDump);
        } else if (!STRCMP((OMX_STRING)pElem->name, "OutputDump")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->outputDump);
            VTEST_MSG_LOW("MasterXml: OutputDump: %u", pGlobalVideoProp->outputDump);
        } else if (!STRCMP((OMX_STRING)pElem->name, "DynamicBufMode")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->bDynamicBufMode);
            VTEST_MSG_LOW("MasterXml: DynamicBufMode: %u", pGlobalVideoProp->bDynamicBufMode);
        } else if (!STRCMP((OMX_STRING)pElem->name, "SinkType")) {
            pGlobalVideoProp->bSinkType = (SinkType)ParseEnum(g_pSinkType, (OMX_STRING)pElem->children->content);
            VTEST_MSG_LOW("MasterXml: SinkType: %u", pGlobalVideoProp->bSinkType);
        } else if (!STRCMP((OMX_STRING)pElem->name, "DisplayBufCount")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nDisplayBufCnt);
            VTEST_MSG_LOW("MasterXml: DisplayBufCount: %u", pGlobalVideoProp->nDisplayBufCnt);
        } else if (!STRCMP((OMX_STRING)pElem->name, "DefaultWidth")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nDefaultWidth);
            VTEST_MSG_LOW("MasterXml: DefaultWidth: %u", pGlobalVideoProp->nDefaultWidth);
        } else if (!STRCMP((OMX_STRING)pElem->name, "DefaultHeight")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nDefaultHeight);
            VTEST_MSG_LOW("MasterXml: DefaultHeight: %u", pGlobalVideoProp->nDefaultHeight);
        } else if (!STRCMP((OMX_STRING)pElem->name, "ErrorDelay")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nErrorDelay);
            VTEST_MSG_LOW("MasterXml: ErrorDelay: %u", pGlobalVideoProp->nErrorDelay);
        } else if (!STRCMP((OMX_STRING)pElem->name, "MinFrameDelay")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nMinFrameDelay);
            VTEST_MSG_LOW("MasterXml: MinFrameDelay: %u", pGlobalVideoProp->nMinFrameDelay);
        } else if (!STRCMP((OMX_STRING)pElem->name, "PollTimeOut")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nPollTimeout);
            VTEST_MSG_LOW("MasterXml: PollTimeOut: %u", pGlobalVideoProp->nPollTimeout);
        } else if (!STRCMP((OMX_STRING)pElem->name, "ThreadTimeOut")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nThreadTimeout);
            VTEST_MSG_LOW("MasterXml: ThreadTimeOut: %u", pGlobalVideoProp->nThreadTimeout);
        } else if (!STRCMP((OMX_STRING)pElem->name, "GenerateCrc")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->bGenerateCrc);
            VTEST_MSG_LOW("MasterXml: GenerateCrc: %u", pGlobalVideoProp->bGenerateCrc);
        } else if (!STRCMP((OMX_STRING)pElem->name, "GenerateMisr")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->bGenerateMisr);
            VTEST_MSG_LOW("MasterXml: GenerateMisr: %u", pGlobalVideoProp->bGenerateMisr);
        } else if (!STRCMP((OMX_STRING)pElem->name, "SkipCRCCheck")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->bSkipCrcCheck);
            VTEST_MSG_LOW("MasterXml: SkipCRCCheck: %u", pGlobalVideoProp->bSkipCrcCheck);
        } else if (!STRCMP((OMX_STRING)pElem->name, "DecOutPortBufCnt")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nDecOutPortBufCnt);
            VTEST_MSG_LOW("MasterXml: DecOutPortBufCnt: %u", pGlobalVideoProp->nDecOutPortBufCnt);
        } else if (!STRCMP((OMX_STRING)pElem->name, "EncCapPortBufCnt")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nEncCapPortBufCnt);
            VTEST_MSG_LOW("MasterXml: EncCapPortBufCnt: %u", pGlobalVideoProp->nEncCapPortBufCnt);
        } else if (!STRCMP((OMX_STRING)pElem->name, "EncOutPortBufCnt")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nEncOutPortBufCnt);
            VTEST_MSG_LOW("MasterXml: EncOutPortBufCnt: %u", pGlobalVideoProp->nEncOutPortBufCnt);
        } else if (!STRCMP((OMX_STRING)pElem->name, "OffsetDump")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->nOffsetDump);
            VTEST_MSG_LOW("MasterXml: OffsetDump: %u", pGlobalVideoProp->nOffsetDump);
        } else if (!STRCMP((OMX_STRING)pElem->name, "ExtraDataDump")) {
            sscanf((OMX_STRING)pElem->children->content, "%u", &pGlobalVideoProp->bExtraDataDump);
            VTEST_MSG_LOW("MasterXml: ExtraDataDump: %u", pGlobalVideoProp->bExtraDataDump);
        } else if (!STRCMP((OMX_STRING)pElem->name, "FWLogMask")) {
            sscanf((OMX_STRING)pElem->children->content, "0x%x", &pGlobalVideoProp->nFwLog);
            VTEST_MSG_LOW("MasterXml: FWLogMask: 0x%x", pGlobalVideoProp->nFwLog);
        }
        pElem = pElem->next;
    }


    xmlFreeDoc(pMasterDoc);
    return status;
}

OMX_ERRORTYPE XmlParser::ParseSessionXml(OMX_STRING sessionXml, VideoStaticProperties* pGlobalVideoProp, VideoSessionInfo* pSessionInfo)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    xmlDocPtr pSessionDoc;
    xmlNodePtr pXmlNode;
    STRLCPY(m_sessionXml, sessionXml, MAX_STR_LEN);

    vtest::FindTestID(m_sessionXml, m_testId);
    VTEST_MSG_HIGH("SessionXml: %s TestId: %s", m_sessionXml, m_testId);

    pSessionDoc = xmlParseFile(m_sessionXml);
    if (!pSessionDoc) {
        fprintf(pGlobalVideoProp->fResult, "\nVTEST-OMX %s, %s, Unable to open %s\n", OMX_VTEST_VERSION, m_testId, m_sessionXml);
        return OMX_ErrorUndefined;
    }

    pXmlNode = xmlDocGetRootElement(pSessionDoc);
    if (!pXmlNode) {
        xmlFreeDoc(pSessionDoc);
        return OMX_ErrorUndefined;
    }

    if (OMX_ErrorNone != ProcessSessionTag(pGlobalVideoProp, pSessionInfo, pXmlNode->xmlChildrenNode) ) {
        return OMX_ErrorUndefined;
    }

    return status;
}

OMX_ERRORTYPE XmlParser::ProcessSessionTag(VideoStaticProperties* pGlobalVideoProp, VideoSessionInfo* pSessionInfo, xmlNodePtr pXmlNode)
{
    OMX_ERRORTYPE status = OMX_ErrorNone;
    char testName[MAX_STR_LEN];
    OMX_BOOL bFoundSession = OMX_FALSE;
    OMX_U32 numSessions = 0;
    VTEST_MSG_HIGH("At Entry pXmlNode:%p and content: %s", pXmlNode, (OMX_STRING)pXmlNode->name);
    while (pXmlNode && status == OMX_ErrorNone) {
        OMX_STRING tagName = (OMX_STRING)pXmlNode->name;

        //TODO: Pass session number.
        if (!STRCMP(tagName, "TestCaseID")) {
        } else if (!STRCMP(tagName, "EnforceBitExact")) {
        } else if (!STRCMP(tagName, "DecodeSession")) {
            strlcpy(testName, "DECODE", MAX_STR_LEN);
            bFoundSession = OMX_TRUE;
        } else if (!STRCMP(tagName, "EncodeSession")) {
            strlcpy(testName, "ENCODE", MAX_STR_LEN);
            bFoundSession = OMX_TRUE;
        } else if (!STRCMP(tagName, "TranscodeSession")) {
            pGlobalVideoProp->bIsConcurrency = TRUE;
            strlcpy(testName, "TRANSCODE", MAX_STR_LEN);
            bFoundSession = OMX_TRUE;
        } else if (!STRCMP(tagName, "VTSession")) {
            pGlobalVideoProp->bIsConcurrency = TRUE;
            strlcpy(testName, "ENCODE", MAX_STR_LEN);
            bFoundSession = OMX_TRUE;
        }

        if(OMX_TRUE == bFoundSession) {
            VTEST_MSG_MEDIUM("Running test for session:%s",testName);
            STRLCPY(pSessionInfo->SessionType,testName,MAX_STR_LEN);
            pSessionInfo->pXmlNode = pXmlNode->xmlChildrenNode;
            pSessionInfo->bActiveSession = OMX_TRUE;
            numSessions++;
            if(numSessions >= MAX_NUM_SESSIONS) {
                VTEST_MSG_HIGH("Max Number of Sessions exceeded");
                pXmlNode = NULL;
                break;
            }
            pSessionInfo++;
            bFoundSession = OMX_FALSE;
        }

        pXmlNode = pXmlNode->next;
    }

    return status;
}


}

