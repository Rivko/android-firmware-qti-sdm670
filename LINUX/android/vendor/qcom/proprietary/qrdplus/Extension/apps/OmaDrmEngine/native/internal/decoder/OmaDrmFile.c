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
#define LOG_TAG "OmaDrmFile"

#include <utils/Log.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>

#include "OmaDrmFile.h"
#include "OmaDrmGlue.h"
#include "OmaDrmDcfFile.h"
#include "OmaDrmRightsManager.h"

#define TRUE 1
#define FALSE 0

#define INVALID_OFFSET ((off64_t)-1)

#define INVALID_BLOCK_INDEX ((uint64_t)-1)

#define MAX_NUM_SESSIONS 128

#define KEY_SIZE AES_BLOCK_SIZE
#define KEY_SIZE_IN_BITS (KEY_SIZE * 8)

#define SHA1_HASH_SIZE 20
#define SHA1_BLOCK_SIZE 64

#define FWD_LOCK_VERSION 0
#define FWD_LOCK_SUBFORMAT 0
#define USAGE_RESTRICTION_FLAGS 0
#define CONTENT_TYPE_LENGTH_POS 7
#define TOP_HEADER_SIZE 8

#define SIG_CALC_BUFFER_SIZE (16 * SHA1_BLOCK_SIZE)

/**
 * Data type for the per-file state information needed by the decoder.
 */
typedef struct OmaDrmFile_Session {
    int playbackStatus;
    int isFwlkFile;
    int fileDesc;
    unsigned char topHeader[TOP_HEADER_SIZE];
    char *pContentType;
    size_t contentTypeLength;
    char *pContentId;
    size_t contentIdLength;
    void *pEncryptedSessionKey;
    size_t encryptedSessionKeyLength;
    unsigned char dataSignature[SHA1_HASH_SIZE];
    unsigned char headerSignature[SHA1_HASH_SIZE];
    off64_t dataOffset;
    off64_t filePos;
    AES_KEY encryptionRoundKeys;
    HMAC_CTX signingContext;
    unsigned char keyStream[AES_BLOCK_SIZE];
    uint64_t blockIndex;
    OmaDrmFile_DCF_Info_t *pDcfInfo;
} OmaDrmFile_Session_t;

static OmaDrmFile_Session_t *sessionPtrs[MAX_NUM_SESSIONS] = { NULL };

static pthread_mutex_t sessionAcquisitionMutex = PTHREAD_MUTEX_INITIALIZER;

static const unsigned char topHeaderTemplate[] = { 'F', 'W', 'L', 'K',
        FWD_LOCK_VERSION, FWD_LOCK_SUBFORMAT, USAGE_RESTRICTION_FLAGS };

static const char strDefaultCid[] = "cid:fwlk";
static const size_t strlenDefaultCid = sizeof strDefaultCid - 1;

static int OmaDrmFile_isFwlkFile(int fileDesc);
static int OmaDrmFile_dcf_attach(int fileDesc);
static int OmaDrmFile_fwlk_attach(int fileDesc);
static char* OmaDrmFile_GetUid(char *contentId);
/**
 * Acquires an unused file session for the given file descriptor.
 *
 * @param[in] fileDesc A file descriptor.
 *
 * @return A session ID.
 */
static int OmaDrmFile_AcquireSession(int fileDesc) {
    int sessionId = -1;
    if (fileDesc < 0) {
        errno = EBADF;
    } else {
        int i;
        pthread_mutex_lock(&sessionAcquisitionMutex);
        for (i = 0; i < MAX_NUM_SESSIONS; ++i) {
            int candidateSessionId = (fileDesc + i) % MAX_NUM_SESSIONS;
            if (sessionPtrs[candidateSessionId] == NULL) {
                sessionPtrs[candidateSessionId] = malloc(sizeof **sessionPtrs);
                if (sessionPtrs[candidateSessionId] != NULL) {
                    sessionPtrs[candidateSessionId]->playbackStatus = -1;
                    sessionPtrs[candidateSessionId]->isFwlkFile = FALSE; // init Not a Fwlk file
                    sessionPtrs[candidateSessionId]->fileDesc = fileDesc;
                    sessionPtrs[candidateSessionId]->pContentType = NULL;
                    sessionPtrs[candidateSessionId]->pEncryptedSessionKey =
                            NULL;
                    sessionPtrs[candidateSessionId]->pContentId = NULL;
                    sessionPtrs[candidateSessionId]->pDcfInfo = NULL;
                    sessionId = candidateSessionId;
                }
                break;
            }
        }
        pthread_mutex_unlock(&sessionAcquisitionMutex);
        if (i == MAX_NUM_SESSIONS) {
            ALOGE("Too many sessions opened at the same time");
            errno = ENFILE;
        }
    }
    return sessionId;
}

/**
 * Finds the file session associated with the given file descriptor.
 *
 * @param[in] fileDesc A file descriptor.
 *
 * @return A session ID.
 */
static int OmaDrmFile_FindSession(int fileDesc) {
    int sessionId = -1;
    if (fileDesc < 0) {
        errno = EBADF;
    } else {
        int i;
        pthread_mutex_lock(&sessionAcquisitionMutex);
        for (i = 0; i < MAX_NUM_SESSIONS; ++i) {
            int candidateSessionId = (fileDesc + i) % MAX_NUM_SESSIONS;
            if (sessionPtrs[candidateSessionId] != NULL
                    && sessionPtrs[candidateSessionId]->fileDesc == fileDesc) {
                sessionId = candidateSessionId;
                break;
            }
        }
        pthread_mutex_unlock(&sessionAcquisitionMutex);
        if (i == MAX_NUM_SESSIONS) {
            errno = EBADF;
        }
    }
    return sessionId;
}

/**
 * Releases a file session.
 *
 * @param[in] sessionID A session ID.
 */
static void OmaDrmFile_ReleaseSession(int sessionId) {
    pthread_mutex_lock(&sessionAcquisitionMutex);
    assert(
            0 <= sessionId && sessionId < MAX_NUM_SESSIONS
                    && sessionPtrs[sessionId] != NULL);
    free(sessionPtrs[sessionId]->pContentType);
    if (sessionPtrs[sessionId]->isFwlkFile) {
        free(sessionPtrs[sessionId]->pEncryptedSessionKey);
    }
    if (sessionPtrs[sessionId]->pDcfInfo != NULL) {
        OmaDrmDcfFile_Release(sessionPtrs[sessionId]->fileDesc, sessionPtrs[sessionId]->pDcfInfo);
        free(sessionPtrs[sessionId]->pDcfInfo);
        sessionPtrs[sessionId]->pDcfInfo = NULL;
        ALOGV("DCF releases");
    }
    free(sessionPtrs[sessionId]->pContentId);
    memset(sessionPtrs[sessionId], 0, sizeof *sessionPtrs[sessionId]); // Zero out key data.
    free(sessionPtrs[sessionId]);
    sessionPtrs[sessionId] = NULL;
    pthread_mutex_unlock(&sessionAcquisitionMutex);
}

/**
 * Derives keys for encryption and signing from the encrypted session key.
 *
 * @param[in,out] pSession A reference to a file session.
 *
 * @return A Boolean value indicating whether key derivation was successful.
 */
static int OmaDrmFile_DeriveKeys(OmaDrmFile_Session_t * pSession) {
    int result;
    struct OmaDrmFile_DeriveKeys_Data {
        AES_KEY sessionRoundKeys;
        unsigned char value[KEY_SIZE];
        unsigned char key[KEY_SIZE];
    };

    const size_t kSize = sizeof(struct OmaDrmFile_DeriveKeys_Data);
    struct OmaDrmFile_DeriveKeys_Data *pData = malloc(kSize);
    if (pData == NULL) {
        result = FALSE;
    } else {
        result = OmaDrmGlue_DecryptKey(pSession->pEncryptedSessionKey,
                pSession->encryptedSessionKeyLength, pData->key, KEY_SIZE);
        if (result) {
            if (AES_set_encrypt_key(pData->key, KEY_SIZE_IN_BITS,
                    &pData->sessionRoundKeys) != 0) {
                result = FALSE;
            } else {
                // Encrypt the 16-byte value {0, 0, ..., 0} to produce the encryption key.
                memset(pData->value, 0, KEY_SIZE);
                AES_encrypt(pData->value, pData->key, &pData->sessionRoundKeys);
                if (AES_set_encrypt_key(pData->key, KEY_SIZE_IN_BITS,
                        &pSession->encryptionRoundKeys) != 0) {
                    result = FALSE;
                } else {
                    // Encrypt the 16-byte value {1, 0, ..., 0} to produce the signing key.
                    ++pData->value[0];
                    AES_encrypt(pData->value, pData->key,
                            &pData->sessionRoundKeys);
                    HMAC_CTX_init(&pSession->signingContext);
                    HMAC_Init_ex(&pSession->signingContext, pData->key,
                            KEY_SIZE, EVP_sha1(), NULL);
                }
            }
        }
        if (!result) {
            errno = ENOSYS;
        }
        memset(pData, 0, kSize); // Zero out key data.
        free(pData);
    }
    return result;
}

/**
 * Calculates the counter, treated as a 16-byte little-endian number, used to generate the keystream
 * for the given block.
 *
 * @param[in] pNonce A reference to the nonce.
 * @param[in] blockIndex The index number of the block.
 * @param[out] pCounter A reference to the counter.
 */
static void OmaDrmFile_CalculateCounter(const unsigned char *pNonce,
        uint64_t blockIndex, unsigned char *pCounter) {
    unsigned char carry = 0;
    size_t i = 0;
    for (; i < sizeof blockIndex; ++i) {
        unsigned char part = pNonce[i]
                + (unsigned char) (blockIndex >> (i * CHAR_BIT));
        pCounter[i] = part + carry;
        carry = (part < pNonce[i] || pCounter[i] < part) ? 1 : 0;
    }
    for (; i < AES_BLOCK_SIZE; ++i) {
        pCounter[i] = pNonce[i] + carry;
        carry = (pCounter[i] < pNonce[i]) ? 1 : 0;
    }
}

/**
 * Decrypts the byte at the current file position using AES-128-CTR. In CTR (or "counter") mode,
 * encryption and decryption are performed using the same algorithm.
 *
 * @param[in,out] pSession A reference to a file session.
 * @param[in] pByte The byte to decrypt.
 */
void OmaDrmFile_DecryptByte(OmaDrmFile_Session_t * pSession,
        unsigned char *pByte) {
    uint64_t blockIndex = pSession->filePos / AES_BLOCK_SIZE;
    uint64_t blockOffset = pSession->filePos % AES_BLOCK_SIZE;
    if (blockIndex != pSession->blockIndex) {
        // The first 16 bytes of the encrypted session key is used as the nonce.
        unsigned char counter[AES_BLOCK_SIZE];
        OmaDrmFile_CalculateCounter(pSession->pEncryptedSessionKey, blockIndex,
                counter);
        AES_encrypt(counter, pSession->keyStream,
                &pSession->encryptionRoundKeys);
        pSession->blockIndex = blockIndex;
    }
    *pByte ^= pSession->keyStream[blockOffset];
}

int OmaDrmFile_attach(int fileDesc) {
    if (OmaDrmFile_isFwlkFile(fileDesc)) {
        // called incase of FL/CD
        return OmaDrmFile_fwlk_attach(fileDesc);
    } else {
        // called insace of SD
        return OmaDrmFile_dcf_attach(fileDesc);
    }
}

static int OmaDrmFile_isFwlkFile(int fileDesc) {
    int result = FALSE;
    char fwlkHeader[TOP_HEADER_SIZE];
    if (read(fileDesc, fwlkHeader, TOP_HEADER_SIZE) == TOP_HEADER_SIZE
            && memcmp(fwlkHeader, topHeaderTemplate, sizeof topHeaderTemplate)
                    == 0) {
        result = TRUE;
    }
    lseek64(fileDesc, 0, SEEK_SET);
    return result;
}

int OmaDrmFile_IsForwardLockFile(int fileDesc) {
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId >= 0) {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        if (pSession->isFwlkFile) {
            return TRUE;
        }
    }
    return FALSE;
}

static int OmaDrmFile_dcf_attach(int fileDesc) {
    int sessionId = OmaDrmFile_AcquireSession(fileDesc);
    if (sessionId >= 0) {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        int isSuccess = FALSE;
        if (pSession->pDcfInfo == NULL) {
            pSession->pDcfInfo = (struct OmaDrmFile_DCF_Info*) malloc(
                    sizeof(struct OmaDrmFile_DCF_Info));
        }
        if (OmaDrmDcfFile_ParserDcf(fileDesc, pSession->pDcfInfo)) {
            if (pSession->pDcfInfo != NULL) {
                if (pSession->pDcfInfo->version == 0x01) {
                    // valid SD File
                    pSession->contentTypeLength =
                            pSession->pDcfInfo->contentTypeLen;
                    pSession->pContentType = (char*) malloc(
                            pSession->contentTypeLength + 1);
                    memset(pSession->pContentType, 0x00,
                            pSession->contentTypeLength + 1);
                    strncpy(pSession->pContentType,
                            pSession->pDcfInfo->contentType,
                            pSession->contentTypeLength);

                    pSession->contentIdLength =
                            pSession->pDcfInfo->contentIdLen;
                    pSession->pContentId = (char*) malloc(
                            pSession->contentIdLength + 1);
                    memset(pSession->pContentId, 0x00,
                            pSession->contentIdLength + 1);
                    strncpy(pSession->pContentId, pSession->pDcfInfo->contentId,
                            pSession->contentIdLength);
                    isSuccess = TRUE;
                }
            }
        }
        if (isSuccess) {
            ALOGI("Found a SD DRM file");
            pSession->isFwlkFile = FALSE;
            pSession->dataOffset = 0;
            pSession->filePos = 0;
            pSession->blockIndex = INVALID_BLOCK_INDEX;
        } else {
            OmaDrmFile_ReleaseSession(sessionId);
            sessionId = -1;
        }
    }
    return (sessionId >= 0) ? 0 : -1;
}

static int OmaDrmFile_fwlk_attach(int fileDesc) {
    int sessionId = OmaDrmFile_AcquireSession(fileDesc);
    if (sessionId >= 0) {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        int isSuccess = FALSE;
        if (read(fileDesc, pSession->topHeader, TOP_HEADER_SIZE)
                == TOP_HEADER_SIZE
                && memcmp(pSession->topHeader, topHeaderTemplate,
                        sizeof topHeaderTemplate) == 0) {
            pSession->contentTypeLength =
                    pSession->topHeader[CONTENT_TYPE_LENGTH_POS];
            assert(pSession->contentTypeLength <= UCHAR_MAX); // Untaint scalar for code checkers.
            pSession->pContentType = malloc(pSession->contentTypeLength + 1);
            if (pSession->pContentType != NULL
                    && read(fileDesc, pSession->pContentType,
                            pSession->contentTypeLength)
                            == (ssize_t) pSession->contentTypeLength) {
                pSession->pContentType[pSession->contentTypeLength] = '\0';

                // read content id(cid) If available
                char contentIdLength[1];
                if (read(fileDesc, contentIdLength, 1) == 1) {
                    pSession->contentIdLength = (int) contentIdLength[0];
                    assert(pSession->contentIdLength <= UCHAR_MAX);
                    pSession->pContentId = malloc(
                            pSession->contentIdLength + 1);
                    pSession->pContentId[0] = pSession->contentIdLength;
                    if (pSession->pContentId != NULL
                            && read(fileDesc, pSession->pContentId + 1,
                                    pSession->contentIdLength - 1)
                                    == (ssize_t) pSession->contentIdLength
                                            - 1) {
                        pSession->pContentId[pSession->contentIdLength] = '\0';
                    }
                }

                pSession->encryptedSessionKeyLength =
                        OmaDrmGlue_GetEncryptedKeyLength(KEY_SIZE);
                pSession->pEncryptedSessionKey = malloc(
                        pSession->encryptedSessionKeyLength);
                if (pSession->pEncryptedSessionKey != NULL&&
                read(fileDesc, pSession->pEncryptedSessionKey,
                        pSession->encryptedSessionKeyLength) ==
                (ssize_t)pSession->encryptedSessionKeyLength &&
                read(fileDesc, pSession->dataSignature, SHA1_HASH_SIZE) ==
                SHA1_HASH_SIZE &&
                read(fileDesc, pSession->headerSignature, SHA1_HASH_SIZE) ==
                SHA1_HASH_SIZE) {isSuccess = OmaDrmFile_DeriveKeys(pSession);
            }
        }
    }
        if (isSuccess) {
            ALOGI("Found a FL DRM file");
            pSession->isFwlkFile = TRUE;
            pSession->dataOffset = pSession->contentTypeLength
                    + pSession->contentIdLength
                    + pSession->encryptedSessionKeyLength + TOP_HEADER_SIZE
                    + 2 * SHA1_HASH_SIZE;
            pSession->filePos = 0;
            pSession->blockIndex = INVALID_BLOCK_INDEX;
        } else {
            OmaDrmFile_ReleaseSession(sessionId);
            sessionId = -1;
        }
    }
    return (sessionId >= 0) ? 0 : -1;
}

ssize_t OmaDrmFile_read(int fileDesc, void *pBuffer, size_t numBytes) {
    ssize_t numBytesRead;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        numBytesRead = -1;
    } else {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];

        if (!pSession->isFwlkFile) {
            return OmaDrmDcfFile_read(fileDesc, pBuffer, numBytes,
                    pSession->pDcfInfo);
        }

        ssize_t i;
        numBytesRead = read(pSession->fileDesc, pBuffer, numBytes);
        for (i = 0; i < numBytesRead; ++i) {
            OmaDrmFile_DecryptByte(pSession, &((unsigned char *) pBuffer)[i]);
            ++pSession->filePos;
        }
    }
    return numBytesRead;
}

off64_t OmaDrmFile_lseek(int fileDesc, off64_t offset, int whence) {
    off64_t newFilePos;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        newFilePos = INVALID_OFFSET;
    } else {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        if (!pSession->isFwlkFile) {
            return OmaDrmDcfFile_lseek(fileDesc, offset, whence,
                    pSession->pDcfInfo);
        }
        switch (whence) {
        case SEEK_SET:
            newFilePos = lseek64(pSession->fileDesc,
                    pSession->dataOffset + offset, whence);
            break;
        case SEEK_CUR:
        case SEEK_END:
            newFilePos = lseek64(pSession->fileDesc, offset, whence);
            break;
        default:
            errno = EINVAL;
            newFilePos = INVALID_OFFSET;
            break;
        }
        if (newFilePos != INVALID_OFFSET) {
            if (newFilePos < pSession->dataOffset) {
                // The new file position is illegal for an internal Forward Lock file. Restore the
                // original file position.
                (void) lseek64(pSession->fileDesc,
                        pSession->dataOffset + pSession->filePos, SEEK_SET);
                errno = EINVAL;
                newFilePos = INVALID_OFFSET;
            } else {
                // The return value should be the file position that lseek64() would have returned
                // for the embedded content file.
                pSession->filePos = newFilePos - pSession->dataOffset;
                newFilePos = pSession->filePos;
            }
        }
    }
    return newFilePos;
}

int OmaDrmFile_detach(int fileDesc) {
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        return -1;
    }

    OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
    if (!pSession->isFwlkFile) {
        // Incase of SD/DCF file
        OmaDrmDcfFile_Release(pSession->fileDesc, pSession->pDcfInfo);
        OmaDrmFile_ReleaseSession(sessionId);
        return 0;
    }

    HMAC_CTX_cleanup(&sessionPtrs[sessionId]->signingContext);
    OmaDrmFile_ReleaseSession(sessionId);
    return 0;
}

int OmaDrmFile_close(int fileDesc) {
    return (OmaDrmFile_detach(fileDesc) == 0) ? close(fileDesc) : -1;
}

int OmaDrmFile_CheckDataIntegrity(int fileDesc) {
    int result;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        result = FALSE;
    } else {
        struct OmaDrmFile_CheckDataIntegrity_Data {
            unsigned char signature[SHA1_HASH_SIZE];
            unsigned char buffer[SIG_CALC_BUFFER_SIZE];
        }*pData = malloc(sizeof *pData);
        if (pData == NULL) {
            result = FALSE;
        } else {
            OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
            if (lseek64(pSession->fileDesc, pSession->dataOffset, SEEK_SET)
                    != pSession->dataOffset) {
                result = FALSE;
            } else {
                ssize_t numBytesRead;
                unsigned int signatureSize = SHA1_HASH_SIZE;
                while ((numBytesRead = read(pSession->fileDesc, pData->buffer,
                        SIG_CALC_BUFFER_SIZE)) > 0) {
                    HMAC_Update(&pSession->signingContext, pData->buffer,
                            (size_t) numBytesRead);
                }
                if (numBytesRead < 0) {
                    result = FALSE;
                } else {
                    HMAC_Final(&pSession->signingContext, pData->signature,
                            &signatureSize);
                    assert(signatureSize == SHA1_HASH_SIZE);
                    result = memcmp(pData->signature, pSession->dataSignature,
                            SHA1_HASH_SIZE) == 0;
                }
                HMAC_Init_ex(&pSession->signingContext, NULL, KEY_SIZE, NULL,
                        NULL);
                (void) lseek64(pSession->fileDesc,
                        pSession->dataOffset + pSession->filePos, SEEK_SET);
            }
            free(pData);
        }
    }
    return result;
}

int OmaDrmFile_CheckHeaderIntegrity(int fileDesc) {
    int result = FALSE;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        result = FALSE;
    } else {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        if (!pSession->isFwlkFile) {
            // Do not check Header integrity if It is DRM SD file
            // SD file will handle differently than FL, see, pSession->pDcfInfo
            return TRUE;
        }
        unsigned char signature[SHA1_HASH_SIZE];
        unsigned int signatureSize = SHA1_HASH_SIZE;
        HMAC_Update(&pSession->signingContext, pSession->topHeader,
                TOP_HEADER_SIZE);
        HMAC_Update(&pSession->signingContext,
                (unsigned char *) pSession->pContentType,
                pSession->contentTypeLength);
        HMAC_Update(&pSession->signingContext,
                (unsigned char *) pSession->pContentId,
                pSession->contentIdLength);
        HMAC_Update(&pSession->signingContext, pSession->pEncryptedSessionKey,
                pSession->encryptedSessionKeyLength);
        HMAC_Update(&pSession->signingContext, pSession->dataSignature,
                SHA1_HASH_SIZE);
        HMAC_Final(&pSession->signingContext, signature, &signatureSize);
        assert(signatureSize == SHA1_HASH_SIZE);
        result = memcmp(signature, pSession->headerSignature, SHA1_HASH_SIZE)
                == 0;
        HMAC_Init_ex(&pSession->signingContext, NULL, KEY_SIZE, NULL, NULL);

        pSession->pContentId = OmaDrmFile_GetUid(pSession->pContentId);
    }
    return result;
}

int OmaDrmFile_CheckIntegrity(int fileDesc) {
    return OmaDrmFile_CheckHeaderIntegrity(fileDesc)
            && OmaDrmFile_CheckDataIntegrity(fileDesc);
}

const char *OmaDrmFile_GetContentType(int fileDesc) {
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        return NULL;
    }

    OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];

    return pSession->pContentType;
}

static char* OmaDrmFile_GetUid(char *contentId) {
    int length = contentId[0];
    char *uid = (char*) malloc(length + 1);
    memset(uid, 0x00, length + 1);
    strncpy(uid, contentId + 1, length);
    free(contentId);
    return uid;
}

int OmaDrmFile_CheckAndGetDrmRights(int fileDesc,
        OmaDrmRightsManager_Rights_t *rights) {
    int result = FALSE;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        result = FALSE;
    } else {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        ALOGV("CheckAndGetDrmRights uid : %s", pSession->pContentId);
        if (strncmp(pSession->pContentId, strDefaultCid, strlenDefaultCid)
                == 0) {
            // When FWDLOCK file, License available
            ALOGI(
                    "Detected Forward Lock DRM file, unlimited license available");
            strncpy(rights->rightsHeader.uid, strDefaultCid, strlenDefaultCid);
            result = TRUE;
        } else {
            // Check license when CD or SD DRM file
            result = OmaDrmRightsManager_HasRights(pSession->pContentId,
                    rights);
        }
    }
    return result;
}

int OmaDrmFile_ConsumeDrmRights(int fileDesc) {
    int result = FALSE;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        result = FALSE;
    } else {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        ALOGV("ConsumeDrmRights uid : %s", pSession->pContentId);
        if (pSession->playbackStatus == 1) {    // 0-> Playback::START
                                                // 1-> Playback::STOP
                                                // 2-> Playback::PAUSE
                                                // 3-> Playback::RESUME
            if (strncmp(pSession->pContentId, strDefaultCid, strlenDefaultCid)
                == 0) {
                // When FWDLOCK file, License available
                ALOGI("Forward Lock DRM file can be open unlimited time");
                result = TRUE;
            } else {
                // Consume license when CD or SD DRM file
                result = OmaDrmRightsManager_ConsumeRights(pSession->pContentId);
            }
        }
    }
    return result;
}

OmaDrmFile_DCF_Info_t* OmaDrmFile_GetDcfInfo(int fileDesc) {
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId >= 0) {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        return pSession->pDcfInfo;
    }
    return NULL;
}


int OmaDrmFile_DecodeIfNeeded(int fileDesc) {
    // Data will decode only if the file is SD(dcf)
    int result = FALSE;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        result = FALSE;
    } else {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        if (!pSession->isFwlkFile) {
            ALOGI("Decypting data. Found a SD file");
            result = OmaDrmDcfFile_DecryptData(fileDesc, pSession->pDcfInfo);
        }
    }
    return result;
}

int OmaDrmFile_SetPlaybackStatus(int fileDesc, int playbackStatus) {
    // Data will decode only if the file is SD(dcf)
    int result = FALSE;
    int sessionId = OmaDrmFile_FindSession(fileDesc);
    if (sessionId < 0) {
        result = FALSE;
    } else {
        OmaDrmFile_Session_t *pSession = sessionPtrs[sessionId];
        pSession->playbackStatus = playbackStatus;
        result = TRUE;
    }
    return result;
}


int OmaDrmFile_CopyTempDcf(int fileDesc, char* path) {
    return OmaDrmDcfFile_CopyTempDcf(fileDesc, path);
}