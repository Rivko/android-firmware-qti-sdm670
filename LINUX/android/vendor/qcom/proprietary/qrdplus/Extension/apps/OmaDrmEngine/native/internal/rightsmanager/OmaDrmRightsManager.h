/* Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OMADRMRIGHTSMANAGER_H__
#define __OMADRMRIGHTSMANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LENGTH_RIGHTS_VERSION 8
#define LENGTH_RIGHTS_UID 256
#define LENGTH_RIGHTS_KEYVALUE 32
#define LENGTH_RIGHTS_COUNT 8
#define LENGTH_RIGHTS_STARTDATE 32
#define LENGTH_RIGHTS_ENDDATE 32
#define LENGTH_RIGHTS_INTERVAL 32

#define KEY_CONTENT_ID "key_content_id"
#define KEY_TYPE  "type"
#define KEY_COUNT "key_count"
#define KEY_SIZE  "size"
#define KEY_START_DATE "key_start_date"
#define KEY_END_DATE "key_end_date"
#define KEY_INTERVAL "key_interval"
#define KEY_MIMETYPE "key_mimetype"

typedef enum OmaDrmAction {
    DEFAULT = 0x00, PLAY, RINGTONE, TRANSFER, OUTPUT, PREVIEW, EXECUTE, DISPLAY
} OmaDrmAction_t;

typedef struct OmaDrmRightsManager_RightsPermission {
    int type;
    char count[LENGTH_RIGHTS_COUNT];
    char startDate[LENGTH_RIGHTS_STARTDATE];
    char endDate[LENGTH_RIGHTS_ENDDATE];
    char interval[LENGTH_RIGHTS_INTERVAL];
} OmaDrmRightsManager_RightsPermission_t;

typedef struct OmaDrmRightsManager_RightsHeader {
    char version[LENGTH_RIGHTS_VERSION];
    char uid[LENGTH_RIGHTS_UID];
    char keyValue[LENGTH_RIGHTS_KEYVALUE];
} OmaDrmRightsManager_RightsHeader_t;

typedef struct OmaDrmRightsManager_Rights {
    OmaDrmRightsManager_RightsHeader_t rightsHeader;
    OmaDrmRightsManager_RightsPermission_t permission;
} OmaDrmRightsManager_Rights_t;

unsigned char *OmaDrmRightsManager_Base64Decode(const char *data,
        size_t inputLength, size_t *outputLength);
char* OmaDrmRightsManager_Base64Encode(const unsigned char *data,
        size_t inputLength, size_t *outputLength);

void OmaDrmRightsManager_InitRights(OmaDrmRightsManager_Rights_t *rights);
int OmaDrmRightsManager_ReadDrmRights(const char *rightsFileName,
        OmaDrmRightsManager_Rights_t *rights);
int OmaDrmRightsManager_ProcessRightsObject(const char *data,
        const size_t length, const char *mimeType);
int OmaDrmRightsManager_HasRights(const char *uid,
        OmaDrmRightsManager_Rights_t *rights);
int OmaDrmRightsManager_ConsumeRights(const char *uid);
char* OmaDrmRightsManager_BuildRightsFilePath(char* uid);

#ifdef __cplusplus
}
#endif

#endif // __OMADRMRIGHTSMANAGER_H__
