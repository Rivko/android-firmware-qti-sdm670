/* Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "OmaDrmRightsManager"

#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <utils/Log.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "OmaDrmGlue.h"
#include "OmaDrmRightsManager.h"

#define RIGHTS_AVAILABLE 1
#define RIGHTS_NOT_AVAILABLE 0

#define LENGTH_FILE_PATH 300
#define STRING_LENGTH_INCREMENT 25
#define SECONDS_IN_MINUTE 60l
#define SECONDS_IN_HOUR 60 * 60l
#define SECONDS_IN_DAY  24 * 60 * 60l
#define SECONDS_IN_MONTH 30 * 24 * 60 *60l
#define SECONDS_IN_YEAR 365 * 24 * 60 * 60l

static char base64EncodingTable[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

static int base64ModTable[] = { 0, 2, 1 };

typedef struct OmaDrmRightsManager_String {
    char *ptr;
    size_t length;
    size_t maxLength;
    size_t lengthIncrement;
} OmaDrmRightsManager_String_t;

typedef enum OmaDrmRightsManager_DateScannerState {
    OmaDrmRightsManager_DateScannerState_WantPeriod,
    OmaDrmRightsManager_DateScannerState_WantTime,
    OmaDrmRightsManager_DateScannerState_WantYear,
    OmaDrmRightsManager_DateScannerState_WantMonth,
    OmaDrmRightsManager_DateScannerState_WantDay,
    OmaDrmRightsManager_DateScannerState_WantHour,
    OmaDrmRightsManager_DateScannerState_WantMinute,
    OmaDrmRightsManager_DateScannerState_WantSecond,
    OmaDrmRightsManager_DateScannerState_Stop
} OmaDrmRightsManager_DateScannerState_t;

const char strRightsFilePath[] = "/data/drm/omadrm/rights/";

const char *strRightsXmlMimetype = "application/vnd.oma.drm.rights+xml";
const char *strRightsWBXmlMimetype = "application/vnd.oma.drm.rights+wbxml";

const char *strElementContext = "context";
const char *strElementRights = "rights";
const char *strElementVersion = "version";
const char *strElementAgreement = "agreement";
const char *strElementAsset = "asset";
const char *strElementUid = "uid";
const char *strElementPermission = "permission";
const char *strElementPlay = "play";
const char *strElementExecute = "execute";
const char *strElementPrint = "print";
const char *strElementDisplay = "display";
const char *strElementConstraint = "constraint";
const char *strElementCount = "count";
const char *strElementDateTime = "datetime";
const char *strElementStartDate = "start";
const char *strElementEndDate = "end";
const char *strElementInterval = "interval";
const char *strElementKeyInfo = "keyinfo";
const char *strElementKeyValue = "keyvalue";
const char *strElementKeyInfoClassCase = "KeyInfo";
const char *strElementKeyValueClassCase = "KeyValue";

static const OmaDrmRightsManager_String_t nullString = { NULL, 0, 0,
        STRING_LENGTH_INCREMENT };

char* OmaDrmRightsManager_BuildBase64DecodingTable();
char* OmaDrmRightsManager_BuildRightsFilePath(char* uid);
void OmaDrmRightsManager_ClearData();
int OmaDrmRightsManager_ConsumeRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *permission);
long int OmaDrmRightsManager_DateToSec(char*);
void OmaDrmRightsManager_DumpRights(OmaDrmRightsManager_Rights_t *rights);
void OmaDrmRightsManager_ExtendDrmRights(const char *rightsFileName,
        const OmaDrmRightsManager_Rights_t *rights);
void OmaDrmRightsManager_ExtendDrmRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *newPermission,
        OmaDrmRightsManager_RightsPermission_t *existingPermission);
int OmaDrmRightsManager_HasRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *permission);
void OmaDrmRightsManager_InstallDrmRights(OmaDrmRightsManager_Rights_t *rights);
void OmaDrmRightsManager_InitRights(OmaDrmRightsManager_Rights_t *rights);
void OmaDrmRightsManager_InitRightsHeader(
        OmaDrmRightsManager_RightsHeader_t *header);
void OmaDrmRightsManager_InitRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *permission);
void OmaDrmRightsManager_ParseXML(OmaDrmRightsManager_Rights_t *rights,
        xmlNode * a_node);
void OmaDrmRightsManager_ParseXMLPermission(xmlNode *node,
        OmaDrmRightsManager_RightsPermission_t *permission);
void OmaDrmRightsManager_ParseXMLRights(OmaDrmRightsManager_Rights_t *rights,
        const char *data, const size_t length);
long int OmaDrmRightManager_PeriodToSec(char*);
void OmaDrmRightsManager_RemoveDrmRights(const char *rightsFileName);
int OmaDrmRightsManager_StringAppend(OmaDrmRightsManager_String_t *pString,
        int ch);

char* OmaDrmRightsManager_BuildRightsFilePath(char* uid) {
    char rightsFileName[LENGTH_FILE_PATH];
    memset(rightsFileName, 0x00, LENGTH_FILE_PATH);
    if (uid != NULL && strlen(uid) > 0) {
        sprintf(rightsFileName, "%s%s\0", strRightsFilePath, uid);
    }
    return rightsFileName;
}

int OmaDrmRightsManager_ProcessRightsObject(const char *data,
        const size_t length, const char *mimeType) {
    /*Parse XML rights Object*/
    OmaDrmRightsManager_Rights_t *rights =
            (struct OmaDrmRightsManager_Rights*) malloc(
                    sizeof(struct OmaDrmRightsManager_Rights));
    OmaDrmRightsManager_InitRights(rights);
    if (strncmp(mimeType, strRightsXmlMimetype, strlen(strRightsXmlMimetype))
            == 0) {
        ALOGI("Found a XML based right object");
        OmaDrmRightsManager_ParseXMLRights(rights, data, length);
#ifdef LOG_NDEBUG
        OmaDrmRightsManager_DumpRights(rights);
#endif
    } else if (strncmp(mimeType, strRightsWBXmlMimetype,
            strlen(strRightsWBXmlMimetype)) == 0) {
        ALOGI("Found a WBXML based right object");
    }

    if (strlen(rights->rightsHeader.uid) > 0) {
        OmaDrmRightsManager_InstallDrmRights(rights);
    }

    if (rights != NULL) {
        free(rights);
        rights = NULL;
    }

    return 0;
}

void OmaDrmRightsManager_ParseXMLRights(OmaDrmRightsManager_Rights_t *rights,
        const char *data, const size_t length) {
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    LIBXML_TEST_VERSION
    ALOGI("OmaDrmRightsManager_ProcessRightsObject: content : %s", data);

    doc = xmlReadMemory(data, length, "noname.xml", NULL, 0);
    if (doc == NULL) {
        ALOGE("Failed to parse document, err = %s\n", stderr);
        return 0;
    }
    ALOGV("Parse XML success ...........");

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    OmaDrmRightsManager_ParseXML(rights, root_element);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();

}

void OmaDrmRightsManager_InitRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *permission) {
    permission->type = -1;
    memset(permission->count, '\0', LENGTH_RIGHTS_COUNT);
    memset(permission->startDate, '\0', LENGTH_RIGHTS_STARTDATE);
    memset(permission->endDate, '\0', LENGTH_RIGHTS_ENDDATE);
    memset(permission->interval, '\0', LENGTH_RIGHTS_INTERVAL);
}

void OmaDrmRightsManager_InitRightsHeader(
        OmaDrmRightsManager_RightsHeader_t *header) {
    memset(header->version, '\0', LENGTH_RIGHTS_VERSION);
    memset(header->uid, '\0', LENGTH_RIGHTS_UID);
    memset(header->keyValue, '\0', LENGTH_RIGHTS_KEYVALUE);
}

void OmaDrmRightsManager_InitRights(OmaDrmRightsManager_Rights_t *rights) {
    OmaDrmRightsManager_InitRightsHeader(&(rights->rightsHeader));
    OmaDrmRightsManager_InitRightsPermission(&(rights->permission));
}

void OmaDrmRightsManager_DumpRights(OmaDrmRightsManager_Rights_t *rights) {
    ALOGV("----RIGHTS BEGIN ----");
    ALOGV("Adderess : %lu", rights);
    ALOGV("Version : %s", rights->rightsHeader.version);
    ALOGV("UID : %s", rights->rightsHeader.uid);
    ALOGV("KeyValue : %s", rights->rightsHeader.keyValue);

    ALOGV("Type : %i", rights->permission.type);
    ALOGV("Permission Count : %s", rights->permission.count);
    ALOGV("Permission StartDate : %s", rights->permission.startDate);
    ALOGV("Permission EndDate : %s", rights->permission.endDate);
    ALOGV("Permission Interval : %s", rights->permission.interval);
    ALOGV("----RIGHTS END ----");
}

void OmaDrmRightsManager_ParseXML(OmaDrmRightsManager_Rights_t *rights,
        xmlNode * a_node) {
    xmlNode *cur_node = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            ALOGV("node type: Element, name: %s\n", cur_node->name);
            if (strncmp((char*) cur_node->name, strElementVersion,
                    strlen(strElementVersion)) == 0) {
                char* res = xmlNodeGetContent(cur_node);
                ALOGV("version = %s, len = %d ", res, strlen(res));
                strncpy(rights->rightsHeader.version, res, strlen(res));
            } else if (strncmp((char*) cur_node->name, strElementUid,
                    strlen(strElementUid)) == 0) {
                char* res = xmlNodeGetContent(cur_node);
                ALOGV("uid = %s, len = %d ", res, strlen(res));
                strncpy(rights->rightsHeader.uid, res, strlen(res));
            } else if (strncmp((char*) cur_node->name, strElementKeyValue,
                    strlen(strElementKeyValue)) == 0
                    || strncmp((char*) cur_node->name,
                            strElementKeyValueClassCase,
                            strlen(strElementKeyValueClassCase)) == 0) {
                size_t outputLength;
                char* res = xmlNodeGetContent(cur_node);
                res = OmaDrmRightsManager_Base64Decode(res, strlen(res),
                        &outputLength);
                ALOGV("keyValue = %s, len = %d ", res, outputLength);
                memset(rights->rightsHeader.keyValue, 0x00,
                        LENGTH_RIGHTS_KEYVALUE);
                strncpy(rights->rightsHeader.keyValue, res, outputLength);
            } else if (strncmp((char*) cur_node->name, strElementPlay,
                    strlen(strElementPlay)) == 0) {
                rights->permission.type = PLAY;
            } else if (strncmp((char*) cur_node->name, strElementDisplay,
                    strlen(strElementDisplay)) == 0) {
                rights->permission.type = DISPLAY;
            }
            OmaDrmRightsManager_ParseXMLPermission(cur_node,
                    &(rights->permission));
        }
        OmaDrmRightsManager_ParseXML(rights, cur_node->children);
    }
}

void OmaDrmRightsManager_ParseXMLPermission(xmlNode *node,
        OmaDrmRightsManager_RightsPermission_t *permission) {
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            ALOGV("parsePermission, name: %s\n", cur_node->name);
            if (strncmp((char*) cur_node->name, strElementCount,
                    strlen(strElementCount)) == 0) {
                char* res = xmlNodeGetContent(cur_node);
                ALOGV("count = %s, len = %d ", res, strlen(res));
                strncpy(permission->count, res, strlen(res));
            } else if (strncmp((char*) cur_node->name, strElementStartDate,
                    strlen(strElementStartDate)) == 0) {
                char* date = xmlNodeGetContent(cur_node);
                long int sec = OmaDrmRightsManager_DateToSec(date);
                char* res[LENGTH_RIGHTS_STARTDATE];
                sprintf(res, "%lu", sec);
                ALOGV("startDate = %s, date in sec = %s, sec len = %d ", date,
                        res, strlen(res));
                strncpy(permission->startDate, res, strlen(res));
            } else if (strncmp((char*) cur_node->name, strElementEndDate,
                    strlen(strElementEndDate)) == 0) {
                char* date = xmlNodeGetContent(cur_node);
                long int sec = OmaDrmRightsManager_DateToSec(date);
                char* res[LENGTH_RIGHTS_STARTDATE];
                sprintf(res, "%lu", sec);
                ALOGV("endDate = %s, date in sec = %s, sec len = %d ", date,
                        res, strlen(res));
                strncpy(permission->endDate, res, strlen(res));
            } else if (strncmp((char*) cur_node->name, strElementInterval,
                    strlen(strElementInterval)) == 0) {
                char* date = xmlNodeGetContent(cur_node);
                long int sec = OmaDrmRightManager_PeriodToSec(date);
                char* res[LENGTH_RIGHTS_STARTDATE];
                sprintf(res, "%lu", sec);
                ALOGV("interval = %s, interval in sec = %s, sec len = %d ",
                        date, res, strlen(res));
                strncpy(permission->interval, res, strlen(res));
            }
        }
        OmaDrmRightsManager_ParseXMLPermission(cur_node->children, permission);
    }
}

void OmaDrmRightsManager_InstallDrmRights(OmaDrmRightsManager_Rights_t *rights) {
    ALOGI("Installing drm rights ...");
    char rightsFileName[LENGTH_FILE_PATH];
    FILE * file;
    if (rights != NULL && strlen(rights->rightsHeader.uid) > 0) {
        sprintf(rightsFileName, "%s%s\0", strRightsFilePath,
                rights->rightsHeader.uid);
        ALOGV("rightsFileName : %s", rightsFileName);
        if (OmaDrmGlue_CreateDirectories(rightsFileName, S_IRWXU)) {
            ALOGV("omadrm rights folder Exist");
            if (access(rightsFileName, F_OK) != -1) {
                // Rights file already available, I am extending the rights
                ALOGI("Rights file already Exist! I am extending existing rights");
                OmaDrmRightsManager_ExtendDrmRights(rightsFileName, rights);
            } else {
                // file not exists
                ALOGV("rights file does not Exist, creating new file");
                file = fopen(rightsFileName, "wb");
                if (file != NULL) {
                    fwrite(rights, sizeof(OmaDrmRightsManager_Rights_t), 1,
                            file);
                    fclose(file);
                    ALOGI("Rights install successfully");
                }
            }
        }
    }
}

int OmaDrmRightsManager_ReadDrmRights(const char *rightsFileName,
        OmaDrmRightsManager_Rights_t *rights) {
    FILE * file;
    if (rightsFileName != NULL) {
        // reading file
        ALOGV("-----READ Rights from File -----");
        ALOGV("rights file : %s", rightsFileName);
        file = fopen(rightsFileName, "r");
        if (file == NULL) {
            ALOGI("Unable to open rights file! bail.");
            return 0;
        }
        fread(rights, sizeof(struct OmaDrmRightsManager_Rights), 1, file);
        fclose(file);

#ifdef LOG_NDEBUG
        OmaDrmRightsManager_DumpRights(rights);
#endif

    }
    return 1;
}

void OmaDrmRightsManager_ExtendDrmRights(const char *rightsFileName,
        const OmaDrmRightsManager_Rights_t *rights) {
    OmaDrmRightsManager_Rights_t *existingRights =
            (struct OmaDrmRightsManager_Rights*) malloc(
                    sizeof(struct OmaDrmRightsManager_Rights));
    FILE * file;
    if (rights != NULL && strlen(rights->rightsHeader.uid) > 0) {
        OmaDrmRightsManager_ReadDrmRights(rightsFileName, existingRights);
        OmaDrmRightsManager_RemoveDrmRights(rightsFileName);
        OmaDrmRightsManager_ExtendDrmRightsPermission(&(rights->permission),
                &(existingRights->permission));
        ALOGV("Saving Extended rights file");
        file = fopen(rightsFileName, "wb");
        if (file != NULL) {
            fwrite(rights, sizeof(OmaDrmRightsManager_Rights_t), 1, file);
            fclose(file);
            ALOGI("Rights install successfully!");
#ifdef LOG_NDEBUG
            ALOGV("-----EXTENDED RIGHTS ------");
            OmaDrmRightsManager_DumpRights(rights);
#endif
        }

        if (existingRights != NULL) {
            free(existingRights);
        }
    }
}

void OmaDrmRightsManager_RemoveDrmRights(const char *rightsFileName) {
    int ret;
    if (access(rightsFileName, F_OK) != -1) {
        remove(rightsFileName);
        if (ret == 0) {
            ALOGV("Rights Removed successfully! %s", rightsFileName);
        } else {
            ALOGV("Error: unable to delete the file");
        }
    }
}

void OmaDrmRightsManager_ExtendDrmRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *newPermission,
        OmaDrmRightsManager_RightsPermission_t *existingPermission) {
    if ((newPermission->count != NULL && strlen(newPermission->count) > 0)
            && (existingPermission->count != NULL
                    && strlen(existingPermission->count) > 0)) {
        int count = atoi(newPermission->count)
                + atoi(existingPermission->count);
        sprintf(newPermission->count, "%d", count);
        ALOGV("Count permission extended! new count : %s",
                newPermission->count);
    }
}

int OmaDrmRightsManager_HasRights(const char *uid,
        OmaDrmRightsManager_Rights_t *omaDrmRights) {
    ALOGI("Validating Rights : uid : %s", uid);
    char rightsFileName[LENGTH_FILE_PATH];
    int ret = 0;
    OmaDrmRightsManager_Rights_t *rights = NULL;

    if (omaDrmRights != NULL) {
        rights = omaDrmRights;
    }

    if (uid != NULL && strlen(uid) > 0) {
        sprintf(rightsFileName, "%s%s\0", strRightsFilePath, uid);
        if (OmaDrmRightsManager_ReadDrmRights(rightsFileName, rights)) {
            ret += OmaDrmRightsManager_HasRightsPermission(
                    &(rights->permission));
            // remove rights file form drive if rights does not exist or valid
            if (!ret) {
                ALOGI("Rights not available for : %s", uid);
                OmaDrmRightsManager_RemoveDrmRights(rightsFileName);
            } else {
                ALOGI("Rights available for : %s", rights->rightsHeader.uid);
            }
        } else {
            ALOGI(
                    "Unable to find valid License! May license expired or not installed");
        }
    } else {
        ALOGI("uid should not be empty");
    }
    return ret;
}

int OmaDrmRightsManager_ConsumeRights(const char *uid) {
    ALOGI("Consuming one Rights : uid : %s", uid);
    char rightsFileName[LENGTH_FILE_PATH];
    FILE * file;
    int ret = 0;
    OmaDrmRightsManager_Rights_t *rights =
            (struct OmaDrmRightsManager_Rights*) malloc(
                    sizeof(struct OmaDrmRightsManager_Rights));
    if (uid != NULL && strlen(uid) > 0) {
        sprintf(rightsFileName, "%s%s\0", strRightsFilePath, uid);
        if (OmaDrmRightsManager_ReadDrmRights(rightsFileName, rights)) {
            ret += OmaDrmRightsManager_ConsumeRightsPermission(
                    &(rights->permission));
            if (ret) {
                OmaDrmRightsManager_RemoveDrmRights(rightsFileName);
                file = fopen(rightsFileName, "wb");
                if (file != NULL) {
                    fwrite(rights, sizeof(struct OmaDrmRightsManager_Rights), 1,
                            file);
                    fclose(file);
                    ALOGI("Rights consumed successfully!");
#ifdef LOG_NDEBUG
                    ALOGV("---- AFTER RIGHTS CONSUMED ----");
                    OmaDrmRightsManager_DumpRights(rights);
#endif
                }
            }
        } else {
            ALOGI("Unable to consume rights! file not found");
        }
    } else {
        ALOGI("uid should not be empty");
    }

    if (rights) {
        free(rights);
    }
    return ret;
}

int OmaDrmRightsManager_ConsumeRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *permission) {
    char strCount[LENGTH_RIGHTS_COUNT];
    memset(strCount, '\0', LENGTH_RIGHTS_COUNT);
    if (permission->count != NULL && strlen(permission->count) > 0) {
        int count = atoi(permission->count);
        if (count > 0) {
            count--;
            sprintf(strCount, "%d", count);
            memset(permission->count, '\0', LENGTH_RIGHTS_COUNT);
            strncpy(permission->count, strCount, strlen(strCount));
            return 1;
        }
    }
    return 0;
}

int OmaDrmRightsManager_HasRightsPermission(
        OmaDrmRightsManager_RightsPermission_t *permission) {
    long int currentTime = OmaDrmGlue_CurrentSecond();
    if (permission->count != NULL && strlen(permission->count) > 0) {
        if (atoi(permission->count) > 0) {
            return RIGHTS_AVAILABLE;
        }
    }

    if ((permission->startDate != NULL && strlen(permission->startDate) > 0)
            && (permission->endDate != NULL && strlen(permission->endDate) > 0)) {
        if (atol(permission->startDate) < currentTime
                && atol(permission->endDate) > currentTime) {
            return RIGHTS_AVAILABLE;
        }
    }

    if (permission->interval != NULL && strlen(permission->interval) > 0) {
        if (atoi(permission->interval) > currentTime) {
            return RIGHTS_AVAILABLE;
        }
    }

    return RIGHTS_NOT_AVAILABLE;
}

long int OmaDrmRightsManager_DateToSec(char* date) {
    OmaDrmRightsManager_String_t year = nullString;
    OmaDrmRightsManager_String_t month = nullString;
    OmaDrmRightsManager_String_t day = nullString;
    OmaDrmRightsManager_String_t hour = nullString;
    OmaDrmRightsManager_String_t minute = nullString;
    OmaDrmRightsManager_String_t second = nullString;
    struct tm info;
    OmaDrmRightsManager_DateScannerState_t dateScannerState;
    dateScannerState = OmaDrmRightsManager_DateScannerState_WantYear;

    int i;
    int len = strlen(date);
    for (i = 0;
            i < len
                    && dateScannerState
                            != OmaDrmRightsManager_DateScannerState_Stop; i++) {
        char ch = date[i];
        switch (dateScannerState) {
        case OmaDrmRightsManager_DateScannerState_WantYear:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantMonth;
                break;
            }
            OmaDrmRightsManager_StringAppend(&year, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantMonth:
            if (!isdigit(ch)) {
                dateScannerState = OmaDrmRightsManager_DateScannerState_WantDay;
                break;
            }
            OmaDrmRightsManager_StringAppend(&month, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantDay:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantTime;
                break;
            }
            OmaDrmRightsManager_StringAppend(&day, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantTime:
            dateScannerState = OmaDrmRightsManager_DateScannerState_WantHour;
            break;
        case OmaDrmRightsManager_DateScannerState_WantHour:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantMinute;
                break;
            }
            OmaDrmRightsManager_StringAppend(&hour, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantMinute:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantSecond;
                break;
            }
            OmaDrmRightsManager_StringAppend(&minute, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantSecond:
            if (!isdigit(ch)) {
                dateScannerState = OmaDrmRightsManager_DateScannerState_Stop;
                break;
            }
            OmaDrmRightsManager_StringAppend(&second, ch);
            break;

        default:
            break;
        }
    }

    info.tm_year = atoi(year.ptr) - 1900;
    info.tm_mon = atoi(month.ptr) - 1;
    info.tm_mday = atoi(day.ptr);
    info.tm_hour = atoi(hour.ptr);
    info.tm_min = atoi(minute.ptr);
    info.tm_sec = atoi(second.ptr);
    info.tm_isdst = -1;

    long int ret = mktime(&info);
    ALOGV("Date conversion %s to seconds >> %lu", date, ret);
    return ret;

}

long int OmaDrmRightManager_PeriodToSec(char* period) {
    OmaDrmRightsManager_String_t year = nullString;
    OmaDrmRightsManager_String_t month = nullString;
    OmaDrmRightsManager_String_t day = nullString;
    OmaDrmRightsManager_String_t hour = nullString;
    OmaDrmRightsManager_String_t minute = nullString;
    OmaDrmRightsManager_String_t second = nullString;
    OmaDrmRightsManager_DateScannerState_t dateScannerState;
    dateScannerState = OmaDrmRightsManager_DateScannerState_WantPeriod;
    long int currentTime = OmaDrmGlue_CurrentSecond();

    int i;
    int len = strlen(period);
    for (i = 0;
            i < len
                    && dateScannerState
                            != OmaDrmRightsManager_DateScannerState_Stop; i++) {
        char ch = period[i];
        switch (dateScannerState) {
        case OmaDrmRightsManager_DateScannerState_WantPeriod:
            dateScannerState = OmaDrmRightsManager_DateScannerState_WantYear;
            break;
        case OmaDrmRightsManager_DateScannerState_WantYear:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantMonth;
                break;
            }
            OmaDrmRightsManager_StringAppend(&year, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantMonth:
            if (!isdigit(ch)) {
                dateScannerState = OmaDrmRightsManager_DateScannerState_WantDay;
                break;
            }
            OmaDrmRightsManager_StringAppend(&month, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantDay:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantTime;
                break;
            }
            OmaDrmRightsManager_StringAppend(&day, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantTime:
            dateScannerState = OmaDrmRightsManager_DateScannerState_WantHour;
            break;

        case OmaDrmRightsManager_DateScannerState_WantHour:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantMinute;
                break;
            }
            OmaDrmRightsManager_StringAppend(&hour, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantMinute:
            if (!isdigit(ch)) {
                dateScannerState =
                        OmaDrmRightsManager_DateScannerState_WantSecond;
                break;
            }
            OmaDrmRightsManager_StringAppend(&minute, ch);
            break;

        case OmaDrmRightsManager_DateScannerState_WantSecond:
            if (!isdigit(ch)) {
                dateScannerState = OmaDrmRightsManager_DateScannerState_Stop;
                break;
            }
            OmaDrmRightsManager_StringAppend(&second, ch);
            break;

        default:
            break;
        }
    }
    long int yearInSec = atoi(year.ptr) * SECONDS_IN_YEAR;
    long int monthInSec = atoi(month.ptr) * SECONDS_IN_MONTH;
    long int dayInSec = atoi(day.ptr) * SECONDS_IN_DAY;
    long int hourInSec = atoi(hour.ptr) * SECONDS_IN_HOUR;
    long int minuteInSec = atoi(minute.ptr) * SECONDS_IN_MINUTE;
    long int ret = currentTime + yearInSec + monthInSec + dayInSec + hourInSec
            + minuteInSec + atoi(second.ptr);
    ALOGV("Period conversion %s to seconds >> %lu", period, ret);
    return ret;
}

int OmaDrmRightsManager_StringAppend(OmaDrmRightsManager_String_t *pString,
        int ch) {
    if (pString->length == pString->maxLength) {
        size_t newMaxLength = pString->maxLength + pString->lengthIncrement;
        char *newPtr = realloc(pString->ptr, newMaxLength + 1);
        if (newPtr == NULL) {
            return 0;
        }
        pString->ptr = newPtr;
        pString->maxLength = newMaxLength;
    }
    pString->ptr[pString->length++] = ch;
    pString->ptr[pString->length] = '\0';
    return 1;
}

char* OmaDrmRightsManager_Base64Encode(const unsigned char *data,
        size_t inputLength, size_t *outputLength) {
    int i, j;
    *outputLength = 4 * ((inputLength + 2) / 3);

    char *encodedData = malloc(*outputLength);
    if (encodedData == NULL)
        return NULL;

    for (i = 0, j = 0; i < inputLength;) {

        uint32_t octet_a = i < inputLength ? (unsigned char) data[i++] : 0;
        uint32_t octet_b = i < inputLength ? (unsigned char) data[i++] : 0;
        uint32_t octet_c = i < inputLength ? (unsigned char) data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encodedData[j++] = base64EncodingTable[(triple >> 3 * 6) & 0x3F];
        encodedData[j++] = base64EncodingTable[(triple >> 2 * 6) & 0x3F];
        encodedData[j++] = base64EncodingTable[(triple >> 1 * 6) & 0x3F];
        encodedData[j++] = base64EncodingTable[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < base64ModTable[inputLength % 3]; i++) {
        encodedData[*outputLength - 1 - i] = '=';
    }

    return encodedData;
}

unsigned char *OmaDrmRightsManager_Base64Decode(const char *data,
        size_t inputLength, size_t *outputLength) {
    int i, j;
    char *base64DecodingTable = OmaDrmRightsManager_BuildBase64DecodingTable();

    if (inputLength % 4 != 0)
        return NULL;

    *outputLength = inputLength / 4 * 3;
    if (data[inputLength - 1] == '=')
        (*outputLength)--;
    if (data[inputLength - 2] == '=')
        (*outputLength)--;

    unsigned char *decodedData = malloc(*outputLength);
    if (decodedData == NULL)
        return NULL;

    for (i = 0, j = 0; i < inputLength;) {
        uint32_t sextet_a =
                data[i] == '=' ? 0 & i++ : base64DecodingTable[data[i++]];
        uint32_t sextet_b =
                data[i] == '=' ? 0 & i++ : base64DecodingTable[data[i++]];
        uint32_t sextet_c =
                data[i] == '=' ? 0 & i++ : base64DecodingTable[data[i++]];
        uint32_t sextet_d =
                data[i] == '=' ? 0 & i++ : base64DecodingTable[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6)
                + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < *outputLength)
            decodedData[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *outputLength)
            decodedData[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *outputLength)
            decodedData[j++] = (triple >> 0 * 8) & 0xFF;
    }

    if (base64DecodingTable == NULL) {
        free(base64DecodingTable);
        base64DecodingTable = NULL;
    }

    return decodedData;
}

char* OmaDrmRightsManager_BuildBase64DecodingTable() {
    int i;
    char *base64DecodingTable = (char*) malloc(256);

    for (i = 0; i < 64; i++) {
        base64DecodingTable[(unsigned char) base64EncodingTable[i]] = i;
    }
    return base64DecodingTable;
}
