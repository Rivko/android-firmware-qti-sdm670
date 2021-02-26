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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <sys/time.h>

#include "OmaDrmGlue.h"

#define TRUE 1
#define FALSE 0

#define KEY_SIZE 16
#define KEY_SIZE_IN_BITS (KEY_SIZE * 8)

static int isInitialized = FALSE;

static const char strKeyFilename[] = "/data/drm/fwdlock/kek.dat";

static AES_KEY encryptionRoundKeys;
static AES_KEY decryptionRoundKeys;

/**
 * Creates all directories along the fully qualified path of the given file.
 *
 * @param[in] path A reference to the fully qualified path of a file.
 * @param[in] mode The access mode to use for the directories being created.
 *
 * @return A Boolean value indicating whether the operation was successful.
 */
int OmaDrmGlue_CreateDirectories(const char *path, mode_t mode) {
    int result = TRUE;
    size_t partialPathLength = strlen(path);
    char *partialPath = malloc(partialPathLength + 1);
    if (partialPath == NULL) {
        result = FALSE;
    } else {
        size_t i;
        for (i = 0; i < partialPathLength; ++i) {
            if (path[i] == '/' && i > 0) {
                partialPath[i] = '\0';
                if (mkdir(partialPath, mode) != 0 && errno != EEXIST) {
                    result = FALSE;
                    break;
                }
            }
            partialPath[i] = path[i];
        }
        free(partialPath);
    }
    return result;
}

/**
 * Initializes the round keys used for encryption and decryption of session keys. First creates a
 * device-unique key-encryption key if none exists yet.
 */
static void OmaDrmGlue_InitializeRoundKeys() {
    unsigned char keyEncryptionKey[KEY_SIZE];
    int fileDesc = open(strKeyFilename, O_RDONLY);
    if (fileDesc >= 0) {
        if (read(fileDesc, keyEncryptionKey, KEY_SIZE) == KEY_SIZE) {
            isInitialized = TRUE;
        }
        (void) close(fileDesc);
    } else if (errno == ENOENT
            && OmaDrmGlue_GetRandomNumber(keyEncryptionKey, KEY_SIZE)
            && OmaDrmGlue_CreateDirectories(strKeyFilename, S_IRWXU)) {
        fileDesc = open(strKeyFilename, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR);
        if (fileDesc >= 0) {
            if (write(fileDesc, keyEncryptionKey, KEY_SIZE) == KEY_SIZE) {
                isInitialized = TRUE;
            }
            (void) close(fileDesc);
        }
    }
    if (isInitialized) {
        if (AES_set_encrypt_key(keyEncryptionKey, KEY_SIZE_IN_BITS,
                &encryptionRoundKeys) != 0
                || AES_set_decrypt_key(keyEncryptionKey, KEY_SIZE_IN_BITS,
                        &decryptionRoundKeys) != 0) {
            isInitialized = FALSE;
        }
    }
    memset(keyEncryptionKey, 0, KEY_SIZE); // Zero out key data.
}

/**
 * Validates the padding of a decrypted key.
 *
 * @param[in] pData A reference to the buffer containing the decrypted key and padding.
 * @param[in] decryptedKeyLength The length in bytes of the decrypted key.
 *
 * @return A Boolean value indicating whether the padding was valid.
 */
static int OmaDrmGlue_ValidatePadding(const unsigned char *pData,
        size_t decryptedKeyLength) {
    size_t i;
    size_t padding = AES_BLOCK_SIZE - (decryptedKeyLength % AES_BLOCK_SIZE);
    pData += decryptedKeyLength;
    for (i = 0; i < padding; ++i) {
        if ((size_t) * pData != padding) {
            return FALSE;
        }
        ++pData;
    }
    return TRUE;
}

int OmaDrmGlue_GetRandomNumber(void *pBuffer, size_t numBytes) {
    // Generate 'cryptographically secure' random bytes by reading them from "/dev/urandom" (the
    // non-blocking version of "/dev/random").
    ssize_t numBytesRead = 0;
    int fileDesc = open("/dev/urandom", O_RDONLY);
    if (fileDesc >= 0) {
        numBytesRead = read(fileDesc, pBuffer, numBytes);
        (void) close(fileDesc);
    }
    return numBytesRead >= 0 && (size_t) numBytesRead == numBytes;
}

int OmaDrmGlue_InitializeKeyEncryption() {
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, OmaDrmGlue_InitializeRoundKeys);
    return isInitialized;
}

size_t OmaDrmGlue_GetEncryptedKeyLength(size_t plaintextKeyLength) {
    return ((plaintextKeyLength / AES_BLOCK_SIZE) + 2) * AES_BLOCK_SIZE;
}

int OmaDrmGlue_EncryptKey(const void *pPlaintextKey, size_t plaintextKeyLength,
        void *pEncryptedKey, size_t encryptedKeyLength) {
    int result = FALSE;
    assert(
            encryptedKeyLength
                    == OmaDrmGlue_GetEncryptedKeyLength(plaintextKeyLength));
    if (OmaDrmGlue_InitializeKeyEncryption()) {
        unsigned char initVector[AES_BLOCK_SIZE];
        if (OmaDrmGlue_GetRandomNumber(initVector, AES_BLOCK_SIZE)) {
            size_t padding = AES_BLOCK_SIZE
                    - (plaintextKeyLength % AES_BLOCK_SIZE);
            size_t dataLength = encryptedKeyLength - AES_BLOCK_SIZE;
            memcpy(pEncryptedKey, pPlaintextKey, plaintextKeyLength);
            memset((unsigned char *) pEncryptedKey + plaintextKeyLength,
                    (int) padding, padding);
            memcpy((unsigned char *) pEncryptedKey + dataLength, initVector,
                    AES_BLOCK_SIZE);
            AES_cbc_encrypt(pEncryptedKey, pEncryptedKey, dataLength,
                    &encryptionRoundKeys, initVector, AES_ENCRYPT);
            result = TRUE;
        }
    }
    return result;
}

int OmaDrmGlue_DecryptKey(const void *pEncryptedKey, size_t encryptedKeyLength,
        void *pDecryptedKey, size_t decryptedKeyLength) {
    int result = FALSE;
    assert(
            encryptedKeyLength
                    == OmaDrmGlue_GetEncryptedKeyLength(decryptedKeyLength));
    if (OmaDrmGlue_InitializeKeyEncryption()) {
        size_t dataLength = encryptedKeyLength - AES_BLOCK_SIZE;
        unsigned char *pData = malloc(dataLength);
        if (pData != NULL) {
            unsigned char initVector[AES_BLOCK_SIZE];
            memcpy(pData, pEncryptedKey, dataLength);
            memcpy(initVector,
                    (const unsigned char *) pEncryptedKey + dataLength,
                    AES_BLOCK_SIZE);
            AES_cbc_encrypt(pData, pData, dataLength, &decryptionRoundKeys,
                    initVector, AES_DECRYPT);
            memcpy(pDecryptedKey, pData, decryptedKeyLength);
            result = OmaDrmGlue_ValidatePadding(pData, decryptedKeyLength);
            free(pData);
        }
    }
    return result;
}

long int OmaDrmGlue_CurrentSecond() {
    struct timeval now;
    gettimeofday(&now, NULL);
    long int currentTime = now.tv_sec;
    return currentTime;
}
