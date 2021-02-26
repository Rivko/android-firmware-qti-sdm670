/*-------------------------------------------------------------------
Copyright (c) 2013-2014, 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2010 The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The Linux Foundation nor
      the names of its contributors may be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/

#include <stdlib.h>
#include "vtest_Script.h"
#include "vtest_Debug.h"
#include "vtest_ComDef.h"
#include "vtest_XmlComdef.h"
#include "vtest_XmlParser.h"
#include "vtest_ITestCase.h"
#include "vtest_TestCaseFactory.h"
vtest::VideoStaticProperties sGlobalStaticVideoProp;

OMX_ERRORTYPE RunTest(vtest::VideoStaticProperties* pGlobalVideoProp, vtest::VideoSessionInfo* pSessionInfo) {

    OMX_ERRORTYPE result = OMX_ErrorNone;
    static OMX_S32 testNum = 0;
    testNum++;
    vtest::ITestCase *pTest =
        vtest::TestCaseFactory::AllocTest(pSessionInfo->SessionType);
    if (pTest == NULL) {
        VTEST_MSG_CONSOLE("Unable to alloc test: %s", pSessionInfo->SessionType);
        return OMX_ErrorInsufficientResources;
    }
    VTEST_SIMPLE_MSG_CONSOLE("\n\n");
    VTEST_MSG_CONSOLE("Running OMX test %s", pSessionInfo->SessionType);
    result = pTest->Start(testNum,pGlobalVideoProp, pSessionInfo);
    if (result != OMX_ErrorNone) {
        VTEST_MSG_CONSOLE("Error starting test");
    } else {
        result = pTest->Finish();
        if (result != OMX_ErrorNone) {
            VTEST_MSG_CONSOLE("Test failed\n");
            if(pGlobalVideoProp->fResult) {
                fprintf(pGlobalVideoProp->fResult, "\nVTEST-OMX %s, %s, FAIL\n", OMX_VTEST_VERSION, pSessionInfo->TestId);
            } else {
                VTEST_MSG_HIGH("Result file not found");
            }
        } else {
            VTEST_MSG_CONSOLE("Test passed\n");
            if(!strcmp(pSessionInfo->SessionType,"DECODE")) {
                if(pGlobalVideoProp->fResult) {
                    fprintf(pGlobalVideoProp->fResult, "\nVTEST-OMX %s, %s, PASS\n", OMX_VTEST_VERSION, pSessionInfo->TestId);
                } else {
                    VTEST_MSG_HIGH("Result file not found");
                }
            } else if(!strcmp(pSessionInfo->SessionType,"ENCODE")) {
                if(pGlobalVideoProp->fResult) {
                    fprintf(pGlobalVideoProp->fResult, "\nVTEST-OMX %s, %s, EPV Pending \n", OMX_VTEST_VERSION, pSessionInfo->TestId);
                } else {
                    VTEST_MSG_HIGH("Result file not found");
                }
            } else {
                if(pGlobalVideoProp->fResult) {
                    fprintf(pGlobalVideoProp->fResult, "\nVTEST-OMX %s, %s, UNSUPPORTED TEST CASE \n", OMX_VTEST_VERSION, pSessionInfo->TestId);
                } else {
                    VTEST_MSG_HIGH("Result file not found");
                }
            }
        }
    }
    vtest::TestCaseFactory::DestroyTest(pTest);
    return result;
}


int main(int argc, char *argv[]) {
    OMX_ERRORTYPE result = OMX_ErrorNone;
    vtest::XmlParser *pXmlParser = NULL;
    vtest::VideoSessionInfo sSessionInfo[MAX_NUM_SESSIONS];
    vtest::VideoSessionInfo *pSessionInfo = NULL;
    vtest::VideoSessionInfo *pBaseSessionInfo = NULL;
    char resultFile[MAX_STR_LEN];
    char masterXmlLocation[MAX_STR_LEN];

    OMX_Init();

    if (argc != 3) {
        VTEST_MSG_CONSOLE("Usage: %s <MasterConfg.xml path> <input.xml>\n", argv[0]);
        return OMX_ErrorBadParameter;
    }

    pXmlParser = new vtest::XmlParser;
    if(!pXmlParser) {
        VTEST_MSG_CONSOLE("Error while allocating memory for pXmlParser\n");
        result = OMX_ErrorUndefined;
        goto DEINIT;
    }

    //Master XML Parsing
    if (OMX_ErrorNone != pXmlParser->ResolveMasterXmlPath(argv[1], masterXmlLocation)) {
        VTEST_MSG_CONSOLE("Error: Input %s is neither a valid path nor a valid filename\n", argv[1]);
        return OMX_ErrorUndefined;
    }


    if (OMX_ErrorNone != pXmlParser->ProcessMasterConfig(&sGlobalStaticVideoProp)) {
        VTEST_MSG_CONSOLE("Error while processing MasterXml\n");
        return OMX_ErrorUndefined;
    }


    //Also open the Results.Csv file
    SNPRINTF(resultFile, MAX_STR_LEN, "%s/Results.csv", masterXmlLocation);
    sGlobalStaticVideoProp.fResult = fopen(resultFile, "a+");

    if (!sGlobalStaticVideoProp.fResult) {
        VTEST_MSG_CONSOLE("Results.Csv file opening failed");
        return OMX_ErrorUndefined;
    }

    //Session XML Parsing and Running
    memset((char*)&sSessionInfo[0], 0, MAX_NUM_SESSIONS * sizeof(vtest::VideoSessionInfo));
    if (OMX_ErrorNone != pXmlParser->ParseSessionXml((OMX_STRING)argv[2], &sGlobalStaticVideoProp, &sSessionInfo[0])) {
        VTEST_MSG_CONSOLE("Error while processing SessionXml and starting test\n");
        return OMX_ErrorUndefined;
    }

    pSessionInfo = pBaseSessionInfo = &sSessionInfo[0];

    while (pSessionInfo->bActiveSession == OMX_TRUE) {

        if (OMX_ErrorNone != RunTest(&sGlobalStaticVideoProp,pSessionInfo)) {
            VTEST_MSG_CONSOLE("Failed Processing Session: %s\n", pSessionInfo->SessionType);
        }

        pSessionInfo++;
        if(pSessionInfo >= (pBaseSessionInfo + MAX_NUM_SESSIONS )) {
            VTEST_MSG_CONSOLE("Exceeded the number of sessions\n");
            break;
        }
    }


    if (sGlobalStaticVideoProp.fResult) {
        fclose(sGlobalStaticVideoProp.fResult);
        sGlobalStaticVideoProp.fResult = NULL;
    }

    if(pXmlParser) {
        delete pXmlParser;
        pXmlParser = NULL;
    }

DEINIT:
    OMX_Deinit();
    return result;
}
