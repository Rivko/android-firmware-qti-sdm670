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
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "OmaDrmEngine"
#define LOG_NDEBUG 0

#include "SessionMap.h"
#include "OmaDrmEngine.h"
#include <utils/Log.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "drm_framework_common.h"
#include <fcntl.h>
#include <limits.h>
#include <DrmRights.h>
#include <DrmConstraints.h>
#include <DrmMetadata.h>
#include <DrmInfo.h>
#include <DrmInfoEvent.h>
#include <DrmInfoStatus.h>
#include <DrmInfoRequest.h>
#include <DrmSupportInfo.h>
#include <DrmConvertedStatus.h>
#include <utils/String8.h>
#include <utils/Vector.h>
#include <utils/KeyedVector.h>

#include "OmaDrmConv.h"
#include "OmaDrmDcfFile.h"
#include "OmaDrmFile.h"
#include "OmaDrmGlue.h"
#include "OmaDrmRightsManager.h"
#include "OmaDrmMimeTypeUtil.h"

using namespace android;

const String8 mimeInternalFLString = String8("application/x-android-drm-fl");
const String8 mimeDRMMessage = String8("application/vnd.oma.drm.message");
const String8 mimeDRMContent = String8("application/vnd.oma.drm.content");
const String8 mimeDRMRightsXML = String8("application/vnd.oma.drm.rights+xml");
const String8 mimeDRMRightsWBXML = String8(
        "application/vnd.oma.drm.rights+wbxml");

const String8 extFL = String8(".fl");
const String8 extDM = String8(".dm");
const String8 extDCF = String8(".dcf");
const String8 extDR = String8(".dr");
const String8 extDRC = String8(".drc");

typedef enum OmaDrmEngine_Action {
    OmaDrmEngine_Action_SetInfoListener = 1,
} OmaDrmEngine_Action_t;

// This extern "C" is mandatory to be managed by TPlugInManager
extern "C" IDrmEngine* create() {
    return new OmaDrmEngine();
}

// This extern "C" is mandatory to be managed by TPlugInManager
extern "C" void destroy(IDrmEngine* plugIn) {
    delete plugIn;
}

OmaDrmEngine::OmaDrmEngine() {
    ALOGI(
            "OmaDrmEngine Construction - init (A OMA DRM V1 FL, CD and SD plugin)");
}

OmaDrmEngine::~OmaDrmEngine() {
    ALOGI("OmaDrmEngine Destruction");

    int size = decodeSessionMap.getSize();

    for (int i = 0; i < size; i++) {
        DecodeSession *session = (DecodeSession*) decodeSessionMap.getValueAt(
                i);
        OmaDrmFile_detach(session->fileDesc);
        ::close(session->fileDesc);
    }

    size = convertSessionMap.getSize();
    for (int i = 0; i < size; i++) {
        ConvertSession *convSession =
                (ConvertSession*) convertSessionMap.getValueAt(i);
        OmaDrmConv_CloseSession(convSession->uniqueId, &(convSession->output));
    }
}

int OmaDrmEngine::getConvertedStatus(OmaDrmConv_Status_t status) {
    int retStatus = DrmConvertedStatus::STATUS_ERROR;

    // ALOGI("OmaDrmEngine::getConvertedStatus");

    switch (status) {
    case OmaDrmConv_Status_OK:
        retStatus = DrmConvertedStatus::STATUS_OK;
        break;
    case OmaDrmConv_Status_SyntaxError:
    case OmaDrmConv_Status_InvalidArgument:
    case OmaDrmConv_Status_UnsupportedFileFormat:
    case OmaDrmConv_Status_UnsupportedContentTransferEncoding:
        ALOGE("OmaDrmEngine getConvertedStatus: file conversion Error %d. "
                "Returning STATUS_INPUTDATA_ERROR", status);
        retStatus = DrmConvertedStatus::STATUS_INPUTDATA_ERROR;
        break;
    default:
        ALOGE("OmaDrmEngine getConvertedStatus: file conversion Error %d. "
                "Returning STATUS_ERROR", status);
        retStatus = DrmConvertedStatus::STATUS_ERROR;
        break;
    }

    return retStatus;
}

DrmConstraints* OmaDrmEngine::onGetConstraints(int uniqueId,
        const String8* path, int action) {
    DrmConstraints* drmConstraints = NULL;

    ALOGI("OmaDrmEngine::onGetConstraints");

    if (NULL != path
            && (RightsStatus::RIGHTS_VALID
                    == onCheckRightsStatus(uniqueId, *path, action))) {
        // Return the empty constraints to show no error condition.
        drmConstraints = new DrmConstraints();
    }

    return drmConstraints;
}

DrmMetadata* OmaDrmEngine::onGetMetadata(int uniqueId, const String8* path) {
    DrmMetadata* drmMetadata = NULL;

    ALOGI("OmaDrmEngine::onGetMetadata");

    if (NULL != path) {
        // Returns empty metadata to show no error condition.
        drmMetadata = new DrmMetadata();
    }

    return drmMetadata;
}

android::status_t OmaDrmEngine::onInitialize(int uniqueId) {
    ALOGI("OmaDrmEngine::onInitialize");

    if (OmaDrmGlue_InitializeKeyEncryption()) {
        ALOGI(
                "OmaDrmEngine::onInitialize -- OmaDrmGlue_InitializeKeyEncryption succeeded");
    } else {
        ALOGE(
                "OmaDrmEngine::onInitialize -- OmaDrmGlue_InitializeKeyEncryption failed:"
                        "errno = %d", errno);
    }

    return DRM_NO_ERROR;
}

android::status_t OmaDrmEngine::onSetOnInfoListener(int uniqueId,
        const IDrmEngine::OnInfoListener* infoListener) {
    ALOGI("OmaDrmEngine::onSetOnInfoListener");
    infoListenerClient = (IDrmEngine::OnInfoListener*) infoListener;
    if (infoListenerClient != NULL) {
        ALOGV("Found infoListenerClient");
    }

    return DRM_NO_ERROR;
}

android::status_t OmaDrmEngine::onTerminate(int uniqueId) {
    ALOGI("OmaDrmEngine::onTerminate");

    return DRM_NO_ERROR;
}

// make sure that lower-case letters are used.
const String8 OmaDrmEngine::FileSuffixes[] = { extFL, extDM, extDCF, extDR,
        extDRC, };

// make sure that lower-case letters are used.
const String8 OmaDrmEngine::MimeTypes[] = { mimeInternalFLString,
        mimeDRMMessage, mimeDRMContent, mimeDRMRightsXML, mimeDRMRightsWBXML, };

const String8 OmaDrmEngine::Description("A OMA DRM V1 FL, CD and SD plugin");

void OmaDrmEngine::AddSupportedMimeTypes(DrmSupportInfo *info) {
    for (size_t i = 0, n = sizeof(MimeTypes) / sizeof(MimeTypes[0]); i < n;
            ++i) {
        info->addMimeType(MimeTypes[i]);
    }
}

void OmaDrmEngine::AddSupportedFileSuffixes(DrmSupportInfo *info) {
    for (size_t i = 0, n = sizeof(FileSuffixes) / sizeof(FileSuffixes[0]);
            i < n; ++i) {
        info->addFileSuffix(FileSuffixes[i]);
    }
}

bool OmaDrmEngine::IsMimeTypeSupported(const String8& mime) {
    String8 tmp(mime);
    tmp.toLower();
    for (size_t i = 0, n = sizeof(MimeTypes) / sizeof(MimeTypes[0]); i < n;
            ++i) {
        if (tmp == MimeTypes[i]) {
            return true;
        }
    }
    return false;
}

bool OmaDrmEngine::IsFileSuffixSupported(const String8& suffix) {
    String8 tmp(suffix);
    tmp.toLower();
    for (size_t i = 0, n = sizeof(FileSuffixes) / sizeof(FileSuffixes[0]);
            i < n; ++i) {
        if (tmp == FileSuffixes[i]) {
            return true;
        }
    }
    return false;
}

DrmSupportInfo* OmaDrmEngine::onGetSupportInfo(int uniqueId) {
    DrmSupportInfo* pSupportInfo = new DrmSupportInfo();

    ALOGI("OmaDrmEngine::onGetSupportInfo");

    // fill all Forward Lock mimetypes and extensions
    if (NULL != pSupportInfo) {
        AddSupportedMimeTypes(pSupportInfo);
        AddSupportedFileSuffixes(pSupportInfo);
        pSupportInfo->setDescription(Description);
    }

    return pSupportInfo;
}

bool OmaDrmEngine::onCanHandle(int uniqueId, const String8& path) {
    bool result = false;

    ALOGI("OmaDrmEngine::onCanHandle, path = %s", path.string());

    String8 extString = path.getPathExtension();
    return IsFileSuffixSupported(extString);
}

DrmInfoStatus* OmaDrmEngine::onProcessDrmInfo(int uniqueId,
        const DrmInfo* drmInfo) {
    DrmInfoStatus *drmInfoStatus = NULL;

    // Nothing to process

    drmInfoStatus = new DrmInfoStatus((int) DrmInfoStatus::STATUS_OK, 0, NULL,
            String8(""));

    ALOGI("OmaDrmEngine::onProcessDrmInfo");

    return drmInfoStatus;
}

status_t OmaDrmEngine::onSaveRights(int uniqueId, const DrmRights& drmRights,
        const String8& rightsPath, const String8& contentPath) {
    ALOGI("OmaDrmEngine::onSaveRights");

    DrmBuffer buff = drmRights.getData();
    OmaDrmRightsManager_ProcessRightsObject(buff.data, buff.length,
            "application/vnd.oma.drm.rights+xml");
    return DRM_NO_ERROR;
}

DrmInfo* OmaDrmEngine::onAcquireDrmInfo(int uniqueId,
        const DrmInfoRequest* drmInfoRequest) {
    DrmInfo* drmInfo = NULL;

    // Nothing to be done for Forward Lock file
    ALOGI("OmaDrmEngine::onAcquireDrmInfo");

    return drmInfo;
}

int OmaDrmEngine::onCheckRightsStatus(int uniqueId, const String8& path,
        int action) {
    int result = RightsStatus::RIGHTS_INVALID;

    ALOGI("OmaDrmEngine::onCheckRightsStatus");

    // Only Transfer action is not allowed for forward Lock files.
    if (onCanHandle(uniqueId, path)) {
        switch (action) {
        case Action::DEFAULT:
        case Action::PLAY:
        case Action::RINGTONE:
        case Action::OUTPUT:
        case Action::PREVIEW:
        case Action::EXECUTE:
        case Action::DISPLAY:
            result = RightsStatus::RIGHTS_VALID;
            break;

        case Action::TRANSFER:
        default:
            result = RightsStatus::RIGHTS_INVALID;
            break;
        }
    }

    return result;
}

status_t OmaDrmEngine::onConsumeRights(int uniqueId,
        DecryptHandle* decryptHandle, int action, bool reserve) {
    // No rights consumption
    ALOGI("OmaDrmEngine::onConsumeRights");
    return DRM_NO_ERROR;
}

bool OmaDrmEngine::onValidateAction(int uniqueId, const String8& path,
        int action, const ActionDescription& description) {
    ALOGI("OmaDrmEngine::onValidateAction");

    // For the forwardlock engine checkRights and ValidateAction are the same.
    return (onCheckRightsStatus(uniqueId, path, action)
            == RightsStatus::RIGHTS_VALID);
}

String8 OmaDrmEngine::onGetOriginalMimeType(int uniqueId, const String8& path,
        int fd) {
    ALOGI("OmaDrmEngine::onGetOriginalMimeType");
    String8 mimeString = String8("");
    int fileDesc = dup(fd);

    // Capture infoListenerClientId for License validation
    int omadrmAction = checkForAction(fileDesc, path);
    ALOGV("omadrmAction : %d", omadrmAction);
    if (omadrmAction == OmaDrmEngine_Action_SetInfoListener) {
        ALOGV("Found infoListenerClientId = %d", uniqueId);
        ALOGI("Info Listener Client registered successfully!");
        infoListenerClientId = uniqueId;
        OmaDrmFile_close(fileDesc);
        return String8("audio/mpeg");
    }

    if (getStaticFileMimetypes(path, mimeString)) {
        ALOGI("Found DRM compatable static mimetype %s , %s", path.string(),
                mimeString.string());
        OmaDrmFile_close(fileDesc);
        return mimeString;
    }

    if (-1 < fileDesc) {
        if (OmaDrmFile_attach(fileDesc) < 0) {
            close(fileDesc);
            return mimeString;
        }
        const char* pMimeType = OmaDrmFile_GetContentType(fileDesc);

        if (NULL != pMimeType) {
            String8 contentType = String8(pMimeType);
            contentType.toLower();
            mimeString = OmaDrmMimeTypeUtil::convertMimeType(contentType);
        }

        OmaDrmFile_close(fileDesc);
    }

    return mimeString;
}

int OmaDrmEngine::onGetDrmObjectType(int uniqueId, const String8& path,
        const String8& mimeType) {
    String8 mimeStr = String8(mimeType);

    ALOGI("OmaDrmEngine::onGetDrmObjectType");

    /* Checks whether
     * 1. path and mime type both are not empty strings (meaning unavailable) else content is unknown
     * 2. if one of them is empty string and if other is known then its a DRM Content Object.
     * 3. if both of them are available, then both may be of known type
     *    (regardless of the relation between them to make it compatible with other DRM Engines)
     */
    if (((0 == path.length()) || onCanHandle(uniqueId, path))
            && ((0 == mimeType.length()) || IsMimeTypeSupported(mimeType))
            && (mimeType != path)) {
        return DrmObjectType::CONTENT;
    }

    return DrmObjectType::UNKNOWN;
}

status_t OmaDrmEngine::onRemoveRights(int uniqueId, const String8& path) {
    // No Rights to remove
    ALOGI("OmaDrmEngine::onRemoveRights");
    return DRM_NO_ERROR;
}

status_t OmaDrmEngine::onRemoveAllRights(int uniqueId) {
    // No rights to remove
    ALOGI("OmaDrmEngine::onRemoveAllRights");
    return DRM_NO_ERROR;
}

#ifdef USE_64BIT_DRM_API
status_t OmaDrmEngine::onSetPlaybackStatus(int uniqueId, DecryptHandle* decryptHandle,
        int playbackStatus, int64_t position) {
#else
status_t OmaDrmEngine::onSetPlaybackStatus(int uniqueId,
        DecryptHandle* decryptHandle, int playbackStatus, int position) {
#endif
    // Not used
    ALOGI("OmaDrmEngine::onSetPlaybackStatus : %d", playbackStatus);

    if (NULL != decryptHandle
            && decodeSessionMap.isCreated(decryptHandle->decryptId)) {
        DecodeSession* session = decodeSessionMap.getValue(
                decryptHandle->decryptId);
        if (NULL != session && session->fileDesc > -1) {
            OmaDrmFile_SetPlaybackStatus(session->fileDesc, playbackStatus);
            if (decryptHandle->status >= RightsStatus::RIGHTS_INVALID &&
                    playbackStatus >= Playback::START) {
                // Check rights before showing expiry dialog
                ALOGI("Confirming rights has been expired ----");
                OmaDrmRightsManager_Rights_t *rights =
                        (struct OmaDrmRightsManager_Rights*) malloc(
                                sizeof(struct OmaDrmRightsManager_Rights));
                int hasRights = checkAndGetDrmRights(session->fileDesc, rights);
                if (!hasRights) {
                    showExpiredDialog(session->fileDesc);
                } else {
                    ALOGI("---- Rights available!");
                }
                if (rights != NULL) {
                    free(rights);
                }
            }
        }
    }
    return DRM_NO_ERROR;
}

status_t OmaDrmEngine::onOpenConvertSession(int uniqueId, int convertId) {
    status_t result = DRM_ERROR_UNKNOWN;
    ALOGI("OmaDrmEngine::onOpenConvertSession");
    if (!convertSessionMap.isCreated(convertId)) {
        ConvertSession *newSession = new ConvertSession();
        if (OmaDrmConv_Status_OK
                == OmaDrmConv_OpenSession(&(newSession->uniqueId),
                        &(newSession->output))) {
            convertSessionMap.addValue(convertId, newSession);
            result = DRM_NO_ERROR;
        } else {
            ALOGE(
                    "OmaDrmEngine::onOpenConvertSession -- OmaDrmConv_OpenSession failed.");
            delete newSession;
        }
    }
    return result;
}

DrmConvertedStatus* OmaDrmEngine::onConvertData(int uniqueId, int convertId,
        const DrmBuffer* inputData) {
    OmaDrmConv_Status_t retStatus = OmaDrmConv_Status_InvalidArgument;
    DrmBuffer *convResult = new DrmBuffer(NULL, 0);
    int offset = -1;

    if (NULL != inputData && convertSessionMap.isCreated(convertId)) {
        ConvertSession *convSession = convertSessionMap.getValue(convertId);

        if (NULL != convSession) {
            retStatus = OmaDrmConv_ConvertData(convSession->uniqueId,
                    inputData->data, inputData->length, &(convSession->output));

            if (OmaDrmConv_Status_OK == retStatus) {
                // return bytes from conversion if available
                if (convSession->output.fromConvertData.numBytes > 0) {
                    convResult->data =
                            new char[convSession->output.fromConvertData.numBytes];

                    if (NULL != convResult->data) {
                        convResult->length =
                                convSession->output.fromConvertData.numBytes;
                        memcpy(convResult->data,
                                (char *) convSession->output.fromConvertData.pBuffer,
                                convResult->length);
                    }
                }
            } else {
                offset = convSession->output.fromConvertData.errorPos;
            }
        }
    }
    return new DrmConvertedStatus(getConvertedStatus(retStatus), convResult,
            offset);
}

DrmConvertedStatus* OmaDrmEngine::onCloseConvertSession(int uniqueId,
        int convertId) {
    OmaDrmConv_Status_t retStatus = OmaDrmConv_Status_InvalidArgument;
    DrmBuffer *convResult = new DrmBuffer(NULL, 0);
    int offset = -1;

    ALOGI("OmaDrmEngine::onCloseConvertSession");

    if (convertSessionMap.isCreated(convertId)) {
        ConvertSession *convSession = convertSessionMap.getValue(convertId);

        if (NULL != convSession) {
            retStatus = OmaDrmConv_CloseSession(convSession->uniqueId,
                    &(convSession->output));

            if (OmaDrmConv_Status_OK == retStatus) {
                offset = convSession->output.fromCloseSession.fileOffset;
                convResult->data = new char[FWD_LOCK_SIGNATURES_SIZE];

                if (NULL != convResult->data) {
                    convResult->length = FWD_LOCK_SIGNATURES_SIZE;
                    memcpy(convResult->data,
                            (char *) convSession->output.fromCloseSession.signatures,
                            convResult->length);
                }
            }
        }
        convertSessionMap.removeValue(convertId);
    }
    return new DrmConvertedStatus(getConvertedStatus(retStatus), convResult,
            offset);
}

#ifdef USE_64BIT_DRM_API
status_t OmaDrmEngine::onOpenDecryptSession(int uniqueId,
        DecryptHandle* decryptHandle,
        int fd,
        off64_t offset,
        off64_t length) {
#else
status_t OmaDrmEngine::onOpenDecryptSession(int uniqueId,
        DecryptHandle* decryptHandle, int fd, int offset, int length) {
#endif
    status_t result = DRM_ERROR_CANNOT_HANDLE;
    int fileDesc = -1;

    ALOGI("OmaDrmEngine::onOpenDecryptSession! fd = %d", fd);

    if ((-1 < fd) && (NULL != decryptHandle)
            && (!decodeSessionMap.isCreated(decryptHandle->decryptId))) {
        fileDesc = dup(fd);
    } else {
        ALOGE("OmaDrmEngine::onOpenDecryptSession parameter error");
        return result;
    }

    if (-1 < fileDesc && -1 < ::lseek(fileDesc, offset, SEEK_SET)
            && -1 < OmaDrmFile_attach(fileDesc)) {
        // check for file integrity. This must be done to protect the content mangling.
        int retVal = OmaDrmFile_CheckHeaderIntegrity(fileDesc);

        DecodeSession* decodeSession = new DecodeSession(fileDesc);

        // Check rights befor open decode session
        OmaDrmRightsManager_Rights_t *rights =
                (struct OmaDrmRightsManager_Rights*) malloc(
                        sizeof(struct OmaDrmRightsManager_Rights));
        OmaDrmRightsManager_InitRights(rights);
        int hasRights = checkAndGetDrmRights(fileDesc, rights);
        // if (!hasRights) {
        //     showExpiredDialog(fileDesc);
        // }

        if (retVal && NULL != decodeSession) {
            OmaDrmFile_DecodeIfNeeded(fileDesc);
            decodeSessionMap.addValue(decryptHandle->decryptId, decodeSession);
            const char *pmime = OmaDrmFile_GetContentType(fileDesc);
            String8 contentType = String8(pmime == NULL ? "" : pmime);
            contentType.toLower();

            decryptHandle->mimeType = OmaDrmMimeTypeUtil::convertMimeType(
                    contentType);
            if (hasRights) {
                decryptHandle->status = RightsStatus::RIGHTS_VALID;
            } else {
                decryptHandle->status = RightsStatus::RIGHTS_INVALID;
                if(!OmaDrmFile_IsForwardLockFile(fileDesc)){
                    decryptHandle->mimeType = String8("audio/mpeg");
                }
            }
            decryptHandle->decryptApiType = DecryptApiType::CONTAINER_BASED;
            decryptHandle->decryptInfo = NULL;

            // calculate Drm file size
            // long length = (long) OmaDrmFile_lseek(fileDesc, 0, SEEK_END); // find file size
            // OmaDrmFile_lseek(fileDesc, 0, SEEK_SET); // rewind the file

            // decryptHandle->decryptInfo = new DecryptInfo();
            // decryptHandle->decryptInfo->decryptBufferLength = length;

            decryptHandle->extendedData.add(String8(KEY_MIMETYPE),
                    decryptHandle->mimeType);
            fillExtendedDataRights(&(decryptHandle->extendedData), rights);

            result = DRM_NO_ERROR;
        } else {
            ALOGI(
                    "OmaDrmEngine::onOpenDecryptSession Integrity Check failed for the fd or DRM rights not available");
            OmaDrmFile_detach(fileDesc);
            delete decodeSession;
        }
        if (rights != NULL) {
            free(rights);
        }
    }

    if (DRM_NO_ERROR != result && -1 < fileDesc) {
        ::close(fileDesc);
    }

    ALOGI("OmaDrmEngine::onOpenDecryptSession Exit. result = %d", result);

    return result;
}

status_t OmaDrmEngine::onOpenDecryptSession(int uniqueId,
        DecryptHandle* decryptHandle, const char* uri) {
    status_t result = DRM_ERROR_CANNOT_HANDLE;
    const char fileTag[] = "file://";

    ALOGI("OmaDrmEngine::onOpenDecryptSession! uri = %s", uri);

    if (NULL != decryptHandle && NULL != uri && strlen(uri) > sizeof(fileTag)) {
        String8 uriTag = String8(uri);
        uriTag.toLower();

        if (0 == strncmp(uriTag.string(), fileTag, sizeof(fileTag) - 1)) {
            const char *filePath = strchr(uri + sizeof(fileTag) - 1, '/');
            if (NULL != filePath && onCanHandle(uniqueId, String8(filePath))) {
                int fd = open(filePath, O_RDONLY);

                if (-1 < fd) {
                    // offset is always 0 and length is not used. so any positive size.
                    result = onOpenDecryptSession(uniqueId, decryptHandle, fd,
                            0, 1);

                    // fd is duplicated already if success. closing the file
                    close(fd);
                }
            }
        }
    }

    return result;
}

status_t OmaDrmEngine::onCloseDecryptSession(int uniqueId,
        DecryptHandle* decryptHandle) {
    status_t result = DRM_ERROR_UNKNOWN;
    ALOGI("OmaDrmEngine::onCloseDecryptSession");

    if (NULL != decryptHandle
            && decodeSessionMap.isCreated(decryptHandle->decryptId)) {
        DecodeSession* session = decodeSessionMap.getValue(
                decryptHandle->decryptId);
        if (NULL != session && session->fileDesc > -1) {

            if (decryptHandle->status == RightsStatus::RIGHTS_VALID) {
                // Consume right before close session
                consumeDrmRights(session->fileDesc);
            }

            OmaDrmFile_detach(session->fileDesc);
            ::close(session->fileDesc);
            decodeSessionMap.removeValue(decryptHandle->decryptId);
            result = DRM_NO_ERROR;
        }
    }

    if (NULL != decryptHandle) {
        if (NULL != decryptHandle->decryptInfo) {
            delete decryptHandle->decryptInfo;
            decryptHandle->decryptInfo = NULL;
        }

        decryptHandle->copyControlVector.clear();
        decryptHandle->extendedData.clear();

        delete decryptHandle;
        decryptHandle = NULL;
    }

    ALOGI("OmaDrmEngine::onCloseDecryptSession Exit");
    return result;
}

status_t OmaDrmEngine::onInitializeDecryptUnit(int uniqueId,
        DecryptHandle* decryptHandle, int decryptUnitId,
        const DrmBuffer* headerInfo) {
    ALOGE(
            "OmaDrmEngine::onInitializeDecryptUnit is not supported for this DRM scheme");
    return DRM_ERROR_UNKNOWN;
}

status_t OmaDrmEngine::onDecrypt(int uniqueId, DecryptHandle* decryptHandle,
        int decryptUnitId, const DrmBuffer* encBuffer, DrmBuffer** decBuffer,
        DrmBuffer* IV) {
    ALOGE("OmaDrmEngine::onDecrypt is not supported for this DRM scheme");
    return DRM_ERROR_UNKNOWN;
}

status_t OmaDrmEngine::onDecrypt(int uniqueId, DecryptHandle* decryptHandle,
        int decryptUnitId, const DrmBuffer* encBuffer, DrmBuffer** decBuffer) {
    ALOGE("OmaDrmEngine::onDecrypt is not supported for this DRM scheme");
    return DRM_ERROR_UNKNOWN;
}

status_t OmaDrmEngine::onFinalizeDecryptUnit(int uniqueId,
        DecryptHandle* decryptHandle, int decryptUnitId) {
    ALOGE(
            "OmaDrmEngine::onFinalizeDecryptUnit is not supported for this DRM scheme");
    return DRM_ERROR_UNKNOWN;
}

ssize_t OmaDrmEngine::onRead(int uniqueId, DecryptHandle* decryptHandle,
        void* buffer, int numBytes) {
    ssize_t size = -1;

    if (NULL != decryptHandle
            && decodeSessionMap.isCreated(decryptHandle->decryptId)
            && NULL != buffer && numBytes > -1) {
        DecodeSession* session = decodeSessionMap.getValue(
                decryptHandle->decryptId);
        if (NULL != session && session->fileDesc > -1) {
            size = OmaDrmFile_read(session->fileDesc, buffer, numBytes);

            if (0 > size) {
                session->offset = ((off_t) - 1);
            } else {
                session->offset += size;
            }
        }
    }

    return size;
}

#ifdef USE_64BIT_DRM_API
off64_t OmaDrmEngine::onLseek(int uniqueId, DecryptHandle* decryptHandle,
        off64_t offset, int whence) {
#else
off_t OmaDrmEngine::onLseek(int uniqueId, DecryptHandle* decryptHandle,
        off_t offset, int whence) {
#endif
    off_t offval = -1;

    if (NULL != decryptHandle
            && decodeSessionMap.isCreated(decryptHandle->decryptId)) {
        DecodeSession* session = decodeSessionMap.getValue(
                decryptHandle->decryptId);
        if (NULL != session && session->fileDesc > -1) {
            offval = OmaDrmFile_lseek(session->fileDesc, offset, whence);
            session->offset = offval;
        }
    }

    return offval;
}

#ifdef USE_64BIT_DRM_API
ssize_t OmaDrmEngine::onPread(int uniqueId,
        DecryptHandle* decryptHandle,
        void* buffer,
        ssize_t numBytes,
        off64_t offset) {
#else
ssize_t OmaDrmEngine::onPread(int uniqueId, DecryptHandle* decryptHandle,
        void* buffer, ssize_t numBytes, off_t offset) {
#endif
    ssize_t bytesRead = -1;

    DecodeSession* decoderSession = NULL;

    if ((NULL != decryptHandle)
            && (NULL
                    != (decoderSession = decodeSessionMap.getValue(
                            decryptHandle->decryptId))) && (NULL != buffer)
            && (numBytes > -1) && (offset > -1)) {
        if (offset != decoderSession->offset) {
            decoderSession->offset = onLseek(uniqueId, decryptHandle, offset,
                    SEEK_SET);
        }

        if (((off_t) - 1) != decoderSession->offset) {
            bytesRead = onRead(uniqueId, decryptHandle, buffer, numBytes);
            if (bytesRead < 0) {
                ALOGE("OmaDrmEngine::onPread error reading");
            }
        }
    } else {
        ALOGE("OmaDrmEngine::onPread decryptId not found");
    }

    return bytesRead;
}

int OmaDrmEngine::checkAndGetDrmRights(int fileDesc,
        OmaDrmRightsManager_Rights_t *rights) {
    int ret = OmaDrmFile_CheckAndGetDrmRights(fileDesc, rights);
    return ret;
}

int OmaDrmEngine::consumeDrmRights(int fileDesc) {
    return OmaDrmFile_ConsumeDrmRights(fileDesc);
}

int OmaDrmEngine::fillExtendedDataRights(
        KeyedVector<String8, String8> *extendedData,
        OmaDrmRightsManager_Rights_t *rights) {
    extendedData->add(String8(KEY_CONTENT_ID),
            String8(rights->rightsHeader.uid));
    // Set size here
    extendedData->add(String8(KEY_SIZE), String8("-8888888"));
    fillExtendedDataRightsPermission(extendedData, &(rights->permission));
    return 0;
}

int OmaDrmEngine::fillExtendedDataRightsPermission(
        KeyedVector<String8, String8> *extendedData,
        OmaDrmRightsManager_RightsPermission_t *permission) {
    if (permission->type > -1) {
        String8 key = String8(KEY_TYPE);
        char typeStr[2];
        memset(typeStr, 0x00, 2);
        sprintf(typeStr, "%d", (int) permission->type);
        extendedData->add(key, String8(typeStr));
    }
    if (permission->count != NULL && strlen(permission->count) > 0) {
        String8 key = String8(KEY_COUNT);
        extendedData->add(key, String8(permission->count));
    }
    if (permission->startDate != NULL && strlen(permission->startDate) > 0) {
        String8 key = String8(KEY_START_DATE);
        extendedData->add(key, String8(permission->startDate));
    }
    if (permission->endDate != NULL && strlen(permission->endDate) > 0) {
        String8 key = String8(KEY_END_DATE);
        extendedData->add(key, String8(permission->endDate));
    }
    if (permission->interval != NULL && strlen(permission->interval) > 0) {
        String8 key = String8(KEY_INTERVAL);
        extendedData->add(key, String8(permission->interval));
    }
    return 0;
}

int OmaDrmEngine::getStaticFileMimetypes(const String8 &path,
        String8 &mimeString) {
    String8 ext = path.getPathExtension();
    if (ext == extDR) {
        mimeString = mimeDRMRightsXML;
        return 1;
    } else if (ext == extDRC) {
        mimeString = mimeDRMRightsWBXML;
        return 1;
    }
    return 0;
}

int OmaDrmEngine::showExpiredDialog(int fileDesc) {
    String8 message = String8("");
    // When License expired
    if (infoListenerClient != NULL) {
        ALOGI("I have Info Listener Client");
        if (infoListenerClientId > 0) {
            ALOGI("Found a valid info listener client ID %d",
                    infoListenerClientId);
        } else {
            ALOGE(
                    "Invalid Info Listener Client ID, Can't show right expiry dialog, Device Reboot needed!");
        }
    } else {
        ALOGE("No Info Listener Client found, Device Reboot needed!");
    }

    OmaDrmFile_DCF_Info_t *dcfInfo =
            (OmaDrmFile_DCF_Info_t *) OmaDrmFile_GetDcfInfo(fileDesc);
    if (dcfInfo == NULL) {
        ALOGV("DCF info not found");
    }
    if (infoListenerClientId > 0 && infoListenerClient != NULL) {
        ALOGV(
                "License expired, Notifying OmaDrmEngine client to show proper dialog");

        if (!OmaDrmFile_IsForwardLockFile(fileDesc)) {
            if (dcfInfo != NULL) {
                ALOGV("Rights Issuer : %s", dcfInfo->rightsIssuer);
                message = String8((char*) dcfInfo->rightsIssuer);
            }

        }

        // The rights have been removed. (Ref : DrmInfoEvent.java)
        // public static final int TYPE_RIGHTS_REMOVED = 6;
        DrmInfoEvent *info = new DrmInfoEvent(infoListenerClientId, 6,
                message);
        infoListenerClient->onInfo(*info);
    }
    return 1;
}

int OmaDrmEngine::checkForAction(int fd, String8 path) {
    int ret = 0;
    String8 leaf = path.getPathLeaf();
    ALOGV("checkForAction: %s, leaf = %s", path.string(), leaf.string());
    if (fd > -1) {
        // Found temp file for SD/DCF palyback, in case of invalid license
        if (OmaDrmFile_CopyTempDcf(fd, (char*) leaf.string())) {
            ret = OmaDrmEngine_Action_SetInfoListener;
        }
    }
    return ret;
}