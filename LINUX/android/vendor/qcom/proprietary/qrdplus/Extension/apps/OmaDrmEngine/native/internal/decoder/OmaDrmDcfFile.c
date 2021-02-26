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

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <sys/stat.h>
#include <utils/Log.h>

#include "OmaDrmGlue.h"
#include "OmaDrmDcfFile.h"
#include "OmaDrmRightsManager.h"

#define TRUE 1
#define FALSE 0
#define DCF_BUFFER_SIZE 500
#define KEY_SIZE AES_BLOCK_SIZE
#define KEY_SIZE_IN_BITS (KEY_SIZE * 8)
#define INVALID_OFFSET ((off64_t)-1)
#define DRM_KEY_LEN                 16
#define DRM_MAX_MALLOC_LEN          (110 * 1024)  /* 50K */
#define DRM_ONE_AES_BLOCK_LEN       16
#define DRM_TWO_AES_BLOCK_LEN       32

const char strDcfFilePath[] = "/data/drm/omadrm/dcf/";
const char strTempFileName[] = "__com.oma.drm_omadrmengine_temp.dm";
const char strTempFilePath[] = "/data/drm/omadrm/__com.oma.drm_omadrmengine_temp.dm";

static int32_t OmaDrmDcfFile_ParseUintVar(uint8_t * buffer, uint8_t * len);

void OmaDrmDcfFile_DecryptDataDiscardPaddingByte(uint8_t *decryptedBuf,
        int32_t *decryptedBufLen);

int32_t OmaDrmDcfFile_AESDecryptDBuffer(uint8_t * Buffer,
        int32_t * BufferLen, AES_KEY *key);

static int32_t OmaDrmDcfFile_ParseUintVar(uint8_t * buffer, uint8_t * len) {
    int32_t i;
    int32_t byteLen;
    int32_t sum;

    if (NULL == buffer)
        return DRM_UINT_VAR_ERR;

    byteLen = 0;
    while ((buffer[byteLen] & UINT_VAR_FLAG) > 0 && byteLen < MAX_UINT_VAR_BYTE) /* UINT_VAR_FLAG == 0x80 */
        byteLen++;

    if (byteLen >= MAX_UINT_VAR_BYTE) /* MAX_UINT_VAR_BYTE == 5 */
        return DRM_UINT_VAR_ERR; /* The var int is too large, and that is impossible */

    *len = (uint8_t)(byteLen + 1);
    sum = buffer[byteLen];
    for (i = byteLen - 1; i >= 0; i--)
        sum += ((buffer[i] & UINT_VAR_DATA) << 7 * (byteLen - i)); /* UINT_VAR_DATA == 0x7F */

    return sum;
}

int OmaDrmDcfFile_ParserDcf(int fileDesc, OmaDrmFile_DCF_Info_t *pDcfInfo) {
    ALOGD("Coming into OmaDrmDcfFile_ParserDcf, fd =  %d", fileDesc);

    uint8_t *buffer;
    uint8_t dataLen;
    uint8_t *pStart, *pEnd;
    uint8_t *pHeader, *pData;
    uint8_t varLen;
    uint8_t *data = (uint8_t*) malloc(sizeof(uint8_t) * DCF_BUFFER_SIZE); // read 4KB data from file

    dataLen = read(fileDesc, data, DCF_BUFFER_SIZE);

    pDcfInfo->dataOffset = 0;
    pDcfInfo->fileDesc = -1;
    pDcfInfo->filePos = 0;
    pDcfInfo->tempFileDesc = -1;

    // Open a temp file for SD expiry usage
    int fd = open(strTempFilePath, O_RDONLY);
    if (fd > -1) {
        pDcfInfo->tempFileDesc = fd;
    } else {
        ALOGE("Unable to open Temp media file : %s ", strerror(errno));
    }



    if (dataLen <= 0) {
        ALOGE("got invalid data, %s", strerror(errno));
        return FALSE;
    }

    buffer = data; // Repoint to the buffer and data will be use for free memory

    /* 1. Parse the version, content-type and content-url */
    pDcfInfo->version = *(buffer++);
    if (0x01 != pDcfInfo->version) { /* Because it is OMA DRM v1.0, the vension must be 1 */
        ALOGE("Invalid drm version");
        return FALSE;
    }

    pDcfInfo->contentTypeLen = *(buffer++);
    if (pDcfInfo->contentTypeLen >= MAX_CONTENT_TYPE_LEN) {
        ALOGE("ContentTypeLen is > MAX_CONTENT_TYPE_LEN");
        return FALSE;
    }

    pDcfInfo->contentIdLen = *(buffer++);
    if (pDcfInfo->contentIdLen >= MAX_CONTENT_ID_LEN) {
        ALOGE("ContentIdLen is > MAX_CONTENT_ID_LEN");
        return FALSE;
    }

    strlcpy((char *) pDcfInfo->contentType, (char *) buffer,
            pDcfInfo->contentTypeLen + 1);
    pDcfInfo->contentType[MAX_CONTENT_TYPE_LEN - 1] = 0;
    buffer += pDcfInfo->contentTypeLen;
    strlcpy((char *) pDcfInfo->contentId, (char *) buffer,
            pDcfInfo->contentIdLen + 1);
    pDcfInfo->contentId[MAX_CONTENT_ID_LEN - 1] = 0;
    buffer += pDcfInfo->contentIdLen;

    /* 2. Get the headers length and data length */
    pDcfInfo->headersLen = OmaDrmDcfFile_ParseUintVar(buffer, &varLen);
    if (DRM_UINT_VAR_ERR == pDcfInfo->headersLen) {
        ALOGE("HeadersLen == DRM_UINT_VAR_ERR");
        return FALSE;
    }
    buffer += varLen;
    pDcfInfo->decryptedDataLen = -1;
    pDcfInfo->encryptedDataLen = OmaDrmDcfFile_ParseUintVar(buffer, &varLen);
    if (DRM_UINT_VAR_ERR == pDcfInfo->encryptedDataLen) {
        ALOGE("EncryptedDataLen == DRM_UINT_VAR_ERR");
        return FALSE;
    }
    buffer += varLen;
    pHeader = buffer;
    buffer += pDcfInfo->headersLen;
    pData = buffer;

    /* 3. Parse the headers */
    pStart = pHeader;
    while (pStart < pData) {
        pEnd = pStart;
        while ('\r' != *pEnd && pEnd < pData)
            pEnd++;

        if (0
                == strncmp((char *) pStart, HEADER_ENCRYPTION_METHOD,
                        HEADER_ENCRYPTION_METHOD_LEN)) {
            if ((pEnd - pStart - HEADER_ENCRYPTION_METHOD_LEN)
                    >= MAX_ENCRYPTION_METHOD_LEN) {
                ALOGE("MAX_ENCRYPTION_METHOD_LEN");
                return FALSE;
            }
            strlcpy((char *) pDcfInfo->encryptionMethod,
                    (char *) (pStart + HEADER_ENCRYPTION_METHOD_LEN),
                    (pEnd - pStart - HEADER_ENCRYPTION_METHOD_LEN) + 1);
            pDcfInfo->encryptionMethod[MAX_ENCRYPTION_METHOD_LEN - 1] = 0;
        } else if (0
                == strncmp((char *) pStart, HEADER_RIGHTS_ISSUER,
                        HEADER_RIGHTS_ISSUER_LEN)) {
            if ((pEnd - pStart - HEADER_RIGHTS_ISSUER_LEN)
                    >= MAX_RIGHTS_ISSUER_LEN) {
                ALOGE("MAX_RIGHTS_ISSUER_LEN");
                return FALSE;
            }
            strlcpy((char *) pDcfInfo->rightsIssuer,
                    (char *) (pStart + HEADER_RIGHTS_ISSUER_LEN),
                    (pEnd - pStart - HEADER_RIGHTS_ISSUER_LEN) + 1);
            pDcfInfo->rightsIssuer[MAX_RIGHTS_ISSUER_LEN - 1] = 0;
        } else if (0
                == strncmp((char *) pStart, HEADER_CONTENT_NAME,
                        HEADER_CONTENT_NAME_LEN)) {
            if ((pEnd - pStart - HEADER_CONTENT_NAME_LEN)
                    >= MAX_CONTENT_NAME_LEN) {
                ALOGE("MAX_CONTENT_NAME_LEN");
                return FALSE;
            }
            strlcpy((char *) pDcfInfo->contentName,
                    (char *) (pStart + HEADER_CONTENT_NAME_LEN),
                    (pEnd - pStart - HEADER_CONTENT_NAME_LEN) + 1);
            pDcfInfo->contentName[MAX_CONTENT_NAME_LEN - 1] = 0;
        } else if (0
                == strncmp((char *) pStart, HEADER_CONTENT_DESCRIPTION,
                        HEADER_CONTENT_DESCRIPTION_LEN)) {
            if ((pEnd - pStart - HEADER_CONTENT_DESCRIPTION_LEN)
                    >= MAX_CONTENT_DESCRIPTION_LEN) {
                ALOGE("MAX_CONTENT_DESCRIPTION_LEN");
                return FALSE;
            }
            strlcpy((char *) pDcfInfo->contentDescription,
                    (char *) (pStart + HEADER_CONTENT_DESCRIPTION_LEN),
                    (pEnd - pStart - HEADER_CONTENT_DESCRIPTION_LEN) + 1);
            pDcfInfo->contentDescription[MAX_CONTENT_DESCRIPTION_LEN - 1] = 0;
        } else if (0
                == strncmp((char *) pStart, HEADER_CONTENT_VENDOR,
                        HEADER_CONTENT_VENDOR_LEN)) {
            if ((pEnd - pStart - HEADER_CONTENT_VENDOR_LEN)
                    >= MAX_CONTENT_VENDOR_LEN) {
                ALOGE("MAX_CONTENT_VENDOR_LEN");
                return FALSE;
            }
            strlcpy((char *) pDcfInfo->contentVendor,
                    (char *) (pStart + HEADER_CONTENT_VENDOR_LEN),
                    (pEnd - pStart - HEADER_CONTENT_VENDOR_LEN) + 1);
            pDcfInfo->contentVendor[MAX_CONTENT_VENDOR_LEN - 1] = 0;
        } else if (0
                == strncmp((char *) pStart, HEADER_ICON_URI,
                        HEADER_ICON_URI_LEN)) {
            if ((pEnd - pStart - HEADER_ICON_URI_LEN) >= MAX_ICON_URI_LEN) {
                ALOGE("MAX_ICON_URI_LEN");
                return FALSE;
            }
            strlcpy((char *) pDcfInfo->iconUri,
                    (char *) (pStart + HEADER_ICON_URI_LEN),
                    (pEnd - pStart - HEADER_ICON_URI_LEN) + 1);
            pDcfInfo->iconUri[MAX_ICON_URI_LEN - 1] = 0;
        }

        if ('\n' == *(pEnd + 1)) {
            pStart = pEnd + 2; /* Two bytes: a '\r' and a '\n' */
        } else {
            pStart = pEnd + 1;
        }
    }

    pDcfInfo->dataOffset = pData - data;
    pDcfInfo->fileDesc = -1;
    pDcfInfo->filePos = 0;

    if (data != NULL) {
        free(data);
    }

    ALOGD("OmaDrmDcfFile_ParserDcf complete");

#ifdef LOG_TAG
    OmaDrmDcfFile_DumpDcfInfo(pDcfInfo);
#endif

    return TRUE;
}

void OmaDrmDcfFile_DumpDcfInfo(OmaDrmFile_DCF_Info_t *info) {
    ALOGD("---- DCF INFO BEGIN ----");
    ALOGD("info.address = %lu", info);
    ALOGD("info.Version = %d", info->version);
    ALOGD("info.ContentTypeLen = %d", info->contentTypeLen);
    ALOGD("info.ContentIdLen = %d", info->contentIdLen);
    ALOGD("info.ContentType = %s", info->contentType);
    ALOGD("info.ContentId = %s", info->contentId);
    ALOGD("info.HeadersLen = %d", info->headersLen);
    ALOGD("info.EncryptedDataLen = %d", info->encryptedDataLen);
    ALOGD("info.Encryption_Method = %s", info->encryptionMethod);
    ALOGD("info.Rights_Issuer = %s", info->rightsIssuer);
    ALOGD("info.Content_Name = %s", info->contentName);
    ALOGD("info.ContentDescription = %s", info->contentDescription);
    ALOGD("info.ContentVendor = %s", info->contentVendor);
    ALOGD("info.Icon_URI = %s", info->iconUri);
    ALOGD("info.dataOffset = %lu", info->dataOffset);
    ALOGD("info.fileDesc = %d", info->fileDesc);
    ALOGD("info.tempFileDesc = %d", info->tempFileDesc);
    ALOGD("---- DCF INFO END ----");
}

ssize_t OmaDrmDcfFile_read(int fileDesc, void *pBuffer, size_t numBytes,
        OmaDrmFile_DCF_Info_t *info) {
    ssize_t numBytesRead = -1;
    if (info->fileDesc > -1) {
        numBytesRead = read(info->fileDesc, pBuffer, numBytes);
    } else if (info->tempFileDesc > -1) {
        numBytesRead = read(info->tempFileDesc, pBuffer, numBytes);
    }
    return numBytesRead;
}

off64_t OmaDrmDcfFile_lseek(int fileDesc, off64_t offset, int whence,
        OmaDrmFile_DCF_Info_t *info) {
    off64_t newFilePos = INVALID_OFFSET;

    if (info->fileDesc != -1) {
        // File was decrypted successfully
        newFilePos = lseek64(info->fileDesc, offset, whence);
        if (newFilePos == INVALID_OFFSET) {
            newFilePos = lseek64(info->fileDesc, 0, whence);
        }
    } else if (info->tempFileDesc > -1) {
        newFilePos = lseek64(info->tempFileDesc, offset, whence);
        if (newFilePos == INVALID_OFFSET) {
            newFilePos = lseek64(info->tempFileDesc, 0, whence);
        }
    }
    return newFilePos;
}

int OmaDrmDcfFile_DecryptData(int fileDesc, OmaDrmFile_DCF_Info_t *info) {
    ALOGI("DCF decrypt in!");

    int32_t offset  = 0;
    uint8_t keyValue[DRM_KEY_LEN];
    uint8_t buf[DRM_TWO_AES_BLOCK_LEN];
    int32_t readBytes = 0;
    int32_t bufLen, piece, i, copyBytes = 0, leftBytes;
    int32_t aesStart, mediaStart, mediaBufOff;
    AES_KEY key;
    FILE* decFile;

    char* rightsFilePath;
    OmaDrmRightsManager_Rights_t rights;

    if (info != NULL && strlen(info->contentId) > 0) {
        rightsFilePath = OmaDrmRightsManager_BuildRightsFilePath(
                info->contentId);
    }

    if (rightsFilePath != NULL) {
        OmaDrmRightsManager_InitRights(&rights);
        OmaDrmRightsManager_ReadDrmRights(rightsFilePath, &rights);
    } else {
        ALOGE("Rights not available");
        return 0;
    }

    if (offset == info->encryptedDataLen){
        ALOGE("Invalid content lenth found");
        return 0;
    }

    memset (keyValue, 0x0, DRM_KEY_LEN);
    if (!memcmp (rights.rightsHeader.keyValue, keyValue, DRM_KEY_LEN)) {
        ALOGE ("cann't get content key");
        return 0;
    }

    // Save into Temp file
    char tempFilePath[255];
    memset(tempFilePath, 0x00, 255);
    sprintf(tempFilePath, "%s%d", strDcfFilePath, fileDesc);
    if (OmaDrmGlue_CreateDirectories(tempFilePath, S_IRWXU)) {
        if (access(tempFilePath, F_OK) != -1) {
            remove(tempFilePath);
        }
        decFile = fopen(tempFilePath, "w");
        if (decFile == NULL) {
            ALOGE("Unable to create decrypt file : %s ", strerror(errno));
            return 0;
        } else {
            if (decFile != NULL){
                fclose(decFile);
            }
            decFile = fopen(tempFilePath, "a");
        }
    }

    memcpy (keyValue, rights.rightsHeader.keyValue, DRM_KEY_LEN);

    readBytes = info->encryptedDataLen - offset;
    aesStart = info->dataOffset + (offset / DRM_ONE_AES_BLOCK_LEN * DRM_ONE_AES_BLOCK_LEN);
    piece = (offset + readBytes - 1) / DRM_ONE_AES_BLOCK_LEN - offset / DRM_ONE_AES_BLOCK_LEN + 2;
    mediaStart = offset % DRM_ONE_AES_BLOCK_LEN;
    leftBytes = readBytes;
    mediaBufOff = 0;

    AES_set_decrypt_key(keyValue, DRM_KEY_LEN * 8, &key);

    // ALOGV("readBytes = %d, aesStart = %d, piece %d, seek = %d", readBytes, aesStart, piece, (aesStart + i * DRM_ONE_AES_BLOCK_LEN));

    for (i = 0; i < piece - 2; i++) {
        lseek(fileDesc, (aesStart + i * DRM_ONE_AES_BLOCK_LEN), SEEK_SET);
        read(fileDesc, buf, DRM_TWO_AES_BLOCK_LEN);

        bufLen = DRM_TWO_AES_BLOCK_LEN;

        if (OmaDrmDcfFile_AESDecryptDBuffer(buf, &bufLen, &key) < 0){
            ALOGE ("Invalid media data!");
            return 0;
        }

        if (i == piece - 3) {
            OmaDrmDcfFile_DecryptDataDiscardPaddingByte(buf, &bufLen);
        }

        if (0 != i) {
            mediaStart = 0;
        }

        if (bufLen - mediaStart <= leftBytes) {
            copyBytes = bufLen - mediaStart;
        } else {
            copyBytes = leftBytes;
        }

        if (decFile != NULL) {
            fwrite(buf + mediaStart, 1, copyBytes, decFile);
        }

        leftBytes -= copyBytes;
        mediaBufOff += copyBytes;
    }

    if (decFile != NULL) {
        fclose(decFile);
    }

    int fd = open(tempFilePath, O_RDONLY);
    if (fd > -1) {
        info->fileDesc = fd;
        info->filePath = tempFilePath;
    } else {
        ALOGE("Unable open decrypted file : %s ", strerror(errno));
    }

    ALOGI("DCF decrypt out!");
    return 1;
}

int OmaDrmDcfFile_Release(int fileDesc, OmaDrmFile_DCF_Info_t *info) {
    if (info->fileDesc > -1) {
        close(info->fileDesc);
        remove(info->filePath);
    }

    if (info->tempFileDesc > -1) {
        close(info->tempFileDesc);
    }

    return 0;
}

void OmaDrmDcfFile_DecryptDataDiscardPaddingByte(uint8_t *decryptedBuf,
        int32_t *decryptedBufLen) {
    int32_t tmpLen = *decryptedBufLen;
    int32_t i;

    if (NULL == decryptedBuf || *decryptedBufLen < 0)
        return;

    /* Check whether the last several bytes are padding or not */
    for (i = 1; i < decryptedBuf[tmpLen - 1]; i++) {
        if (decryptedBuf[tmpLen - 1 - i] != decryptedBuf[tmpLen - 1])
            break; /* Not the padding bytes */
    }
    if (i == decryptedBuf[tmpLen - 1]) /* They are padding bytes */
        *decryptedBufLen = tmpLen - i;
    return;
}


int32_t OmaDrmDcfFile_AESDecryptDBuffer(uint8_t * Buffer, int32_t * BufferLen, AES_KEY *key) {
    uint8_t dbuf[3 * DRM_ONE_AES_BLOCK_LEN], buf[DRM_ONE_AES_BLOCK_LEN];
    uint64_t i, len, wlen = DRM_ONE_AES_BLOCK_LEN, curLen, restLen;
    uint8_t *pTarget, *pTargetHead;

    pTargetHead = Buffer;
    pTarget = Buffer;
    curLen = 0;
    restLen = *BufferLen;

    if (restLen > 2 * DRM_ONE_AES_BLOCK_LEN) {
        len = 2 * DRM_ONE_AES_BLOCK_LEN;
    } else {
        len = restLen;
    }
    memcpy(dbuf, Buffer, (size_t)len);
    restLen -= len;
    Buffer += len;

    if (len < 2 * DRM_ONE_AES_BLOCK_LEN) { /* The original file is less than one block in length */
        len -= DRM_ONE_AES_BLOCK_LEN;
        /* Decrypt from position len to position len + DRM_ONE_AES_BLOCK_LEN */
        AES_decrypt((dbuf + len), (dbuf + len), key);

        /* Undo the CBC chaining */
        for (i = 0; i < len; ++i)
            dbuf[i] ^= dbuf[i + DRM_ONE_AES_BLOCK_LEN];

        /* Output the decrypted bytes */
        memcpy(pTarget, dbuf, (size_t)len);
        pTarget += len;
    } else {
        uint8_t *b1 = dbuf, *b2 = b1 + DRM_ONE_AES_BLOCK_LEN, *b3 = b2 + DRM_ONE_AES_BLOCK_LEN, *bt;

        for (;;) { /* While some ciphertext remains, prepare to decrypt block b2 */
            /* Read in the next block to see if ciphertext stealing is needed */
            b3 = Buffer;
            if (restLen > DRM_ONE_AES_BLOCK_LEN) {
                len = DRM_ONE_AES_BLOCK_LEN;
            } else {
                len = restLen;
            }
            restLen -= len;
            Buffer += len;

            /* Decrypt the b2 block */
            AES_decrypt((uint8_t *)b2, buf, key);

            if (len == 0 || len == DRM_ONE_AES_BLOCK_LEN) { /* No ciphertext stealing */
                /* Unchain CBC using the previous ciphertext block in b1 */
                for (i = 0; i < DRM_ONE_AES_BLOCK_LEN; ++i)
                    buf[i] ^= b1[i];
            } else { /* Partial last block - use ciphertext stealing */
                wlen = len;
                /* Produce last 'len' bytes of plaintext by xoring with */
                /* The lowest 'len' bytes of next block b3 - C[N-1] */
                for (i = 0; i < len; ++i)
                    buf[i] ^= b3[i];

                /* Reconstruct the C[N-1] block in b3 by adding in the */
                /* Last (DRM_ONE_AES_BLOCK_LEN - len) bytes of C[N-2] in b2 */
                for (i = len; i < DRM_ONE_AES_BLOCK_LEN; ++i)
                    b3[i] = buf[i];

                /* Decrypt the C[N-1] block in b3 */
                AES_decrypt((uint8_t *)b3, (uint8_t *)b3, key);

                /* Produce the last but one plaintext block by xoring with */
                /* The last but two ciphertext block */
                for (i = 0; i < DRM_ONE_AES_BLOCK_LEN; ++i)
                    b3[i] ^= b1[i];

                /* Write decrypted plaintext blocks */
                memcpy(pTarget, b3, DRM_ONE_AES_BLOCK_LEN);
                pTarget += DRM_ONE_AES_BLOCK_LEN;
            }

            /* Write the decrypted plaintext block */
            memcpy(pTarget, buf, (size_t)wlen);
            pTarget += wlen;

            if (len != DRM_ONE_AES_BLOCK_LEN) {
                *BufferLen = pTarget - pTargetHead;
                return 0;
            }

            /* Advance the buffer pointers */
            bt = b1, b1 = b2, b2 = b3, b3 = bt;
        }
    }
    return 0;
}


int OmaDrmDcfFile_CopyTempDcf(int inFile, char* path) {
    if ((strlen(path) == strlen(strTempFileName)) &&
            (strncmp((char*) path, (char*) strTempFileName, strlen(strTempFileName)) == 0)) {
        char buffer[DCF_BUFFER_SIZE];
        if (access(strTempFilePath, F_OK) != -1) {
            ALOGI("DCF temp file already exist");
            return 1;
        }

        lseek64(inFile, 0 , SEEK_SET);
        if (OmaDrmGlue_CreateDirectories(strTempFilePath, S_IRWXU)) {
            FILE* outFile = fopen(strTempFilePath, "w");
            int len = 0;
            if (outFile != NULL) {
                while ((len = read(inFile, buffer, DCF_BUFFER_SIZE)) > 0){
                    fwrite(buffer, sizeof(char), len, outFile);
                }
                fclose(outFile);
                ALOGI("DCF temp file created");
                return 1;
            } else {
                ALOGE("Unable to create temp dcf! %s", strerror(errno));
            }
        }
        ALOGW("DCF temp file was not found!");
    }
    return 0;
}
