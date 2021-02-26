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
#define LOG_TAG "OmaDrmConv"

#include <unistd.h>
#include <utils/Log.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>

#include "OmaDrmConv.h"
#include "OmaDrmGlue.h"
#include "OmaDrmRightsManager.h"

#define TRUE 1
#define FALSE 0

#define INVALID_OFFSET ((off64_t)-1)

#define MAX_NUM_SESSIONS 32

#define OUTPUT_BUFFER_SIZE_INCREMENT 1024
#define READ_BUFFER_SIZE 1024

#define MAX_BOUNDARY_LENGTH 70
#define MAX_DELIMITER_LENGTH (MAX_BOUNDARY_LENGTH + 4)

#define STRING_LENGTH_INCREMENT 25
#define LENGTH_MIMETYPE 40

#define KEY_SIZE AES_BLOCK_SIZE
#define KEY_SIZE_IN_BITS (KEY_SIZE * 8)

#define SHA1_HASH_SIZE 20

#define FWD_LOCK_VERSION 0
#define FWD_LOCK_SUBFORMAT 0
#define USAGE_RESTRICTION_FLAGS 0
#define CONTENT_TYPE_LENGTH_POS 7
#define TOP_HEADER_SIZE 8

/**
 * Data type for the right xml capture state.
 */
typedef enum OmaDrmConv_RightsXmlState {
    OmaDrmConv_RightsXmlState_Invalid,
    OmaDrmConv_RightsXmlState_CaptureStart,
    OmaDrmConv_RightsXmlState_CaptureContinue,
    OmaDrmConv_RightsXmlState_CaptureEnd,
} OmaDrmConv_RightsXmlState_t;

/**
 * Data type for the parser states of the converter.
 */
typedef enum OmaDrmConv_ParserState {
    OmaDrmConv_ParserState_WantsOpenDelimiter,
    OmaDrmConv_ParserState_WantsMimeHeaders,
    OmaDrmConv_ParserState_WantsBinaryEncodedData,
    OmaDrmConv_ParserState_WantsBase64EncodedData,
    OmaDrmConv_ParserState_Done
} OmaDrmConv_ParserState_t;

/**
 * Data type for the scanner states of the converter.
 */
typedef enum OmaDrmConv_ScannerState {
    OmaDrmConv_ScannerState_WantsFirstDash,
    OmaDrmConv_ScannerState_WantsSecondDash,
    OmaDrmConv_ScannerState_WantsCR,
    OmaDrmConv_ScannerState_WantsLF,
    OmaDrmConv_ScannerState_WantsBoundary,
    OmaDrmConv_ScannerState_WantsBoundaryEnd,
    OmaDrmConv_ScannerState_WantsMimeHeaderNameStart,
    OmaDrmConv_ScannerState_WantsMimeHeaderName,
    OmaDrmConv_ScannerState_WantsMimeHeaderNameEnd,
    OmaDrmConv_ScannerState_WantsContentTypeStart,
    OmaDrmConv_ScannerState_WantsContentType,
    OmaDrmConv_ScannerState_WantsContentTransferEncodingStart,
    OmaDrmConv_ScannerState_Wants_A_OR_I,
    OmaDrmConv_ScannerState_Wants_N,
    OmaDrmConv_ScannerState_Wants_A,
    OmaDrmConv_ScannerState_Wants_R,
    OmaDrmConv_ScannerState_Wants_Y,
    OmaDrmConv_ScannerState_Wants_S,
    OmaDrmConv_ScannerState_Wants_E,
    OmaDrmConv_ScannerState_Wants_6,
    OmaDrmConv_ScannerState_Wants_4,
    OmaDrmConv_ScannerState_Wants_B,
    OmaDrmConv_ScannerState_Wants_I,
    OmaDrmConv_ScannerState_Wants_T,
    OmaDrmConv_ScannerState_WantsContentTransferEncodingEnd,
    OmaDrmConv_ScannerState_WantsMimeHeaderValueEnd,
    OmaDrmConv_ScannerState_WantsMimeHeadersEnd,
    OmaDrmConv_ScannerState_WantsByte1,
    OmaDrmConv_ScannerState_WantsByte1_AfterCRLF,
    OmaDrmConv_ScannerState_WantsByte2,
    OmaDrmConv_ScannerState_WantsByte3,
    OmaDrmConv_ScannerState_WantsByte4,
    OmaDrmConv_ScannerState_WantsPadding,
    OmaDrmConv_ScannerState_WantsWhitespace,
    OmaDrmConv_ScannerState_WantsWhitespace_AfterCRLF,
    OmaDrmConv_ScannerState_WantsDelimiter,
    OmaDrmConv_ScannerState_WantsContentIdStart,
    OmaDrmConv_ScannerState_WantsContentId
} OmaDrmConv_ScannerState_t;

/**
 * Data type for the content transfer encoding.
 */
typedef enum OmaDrmConv_ContentTransferEncoding {
    OmaDrmConv_ContentTransferEncoding_Undefined,
    OmaDrmConv_ContentTransferEncoding_Binary,
    OmaDrmConv_ContentTransferEncoding_Base64
} OmaDrmConv_ContentTransferEncoding_t;

/**
 * Data type for a dynamically growing string.
 */
typedef struct OmaDrmConv_String {
    char *ptr;
    size_t length;
    size_t maxLength;
    size_t lengthIncrement;
} OmaDrmConv_String_t;

/**
 * Data type for the per-file state information needed by the converter.
 */
typedef struct OmaDrmConv_Session {
    OmaDrmConv_ParserState_t parserState;
    OmaDrmConv_ScannerState_t scannerState;
    OmaDrmConv_ScannerState_t savedScannerState;
    off64_t numCharsConsumed;
    char delimiter[MAX_DELIMITER_LENGTH];
    size_t delimiterLength;
    size_t delimiterMatchPos;
    OmaDrmConv_String_t mimeHeaderName;
    OmaDrmConv_String_t contentType;
    OmaDrmConv_ContentTransferEncoding_t contentTransferEncoding;
    unsigned char sessionKey[KEY_SIZE];
    void *pEncryptedSessionKey;
    size_t encryptedSessionKeyLength;
    AES_KEY encryptionRoundKeys;
    HMAC_CTX signingContext;
    unsigned char topHeader[TOP_HEADER_SIZE];
    unsigned char counter[AES_BLOCK_SIZE];
    unsigned char keyStream[AES_BLOCK_SIZE];
    int keyStreamIndex;
    unsigned char ch;
    size_t outputBufferSize;
    size_t dataOffset;
    size_t numDataBytes;
    OmaDrmConv_String_t contentId;
    OmaDrmConv_String_t rightsXml;
    OmaDrmConv_RightsXmlState_t rightsXmlStatus;
    char rightsXmlMimetype[LENGTH_MIMETYPE];

} OmaDrmConv_Session_t;

static OmaDrmConv_Session_t *sessionPtrs[MAX_NUM_SESSIONS] = { NULL };

static pthread_mutex_t sessionAcquisitionMutex = PTHREAD_MUTEX_INITIALIZER;

static const OmaDrmConv_String_t nullString = { NULL, 0, 0,
        STRING_LENGTH_INCREMENT };

static const unsigned char topHeaderTemplate[] = { 'F', 'W', 'L', 'K',
        FWD_LOCK_VERSION, FWD_LOCK_SUBFORMAT, USAGE_RESTRICTION_FLAGS };

static const char strContent[] = "content-";
static const char strType[] = "type";
static const char strTransferEncoding[] = "transfer-encoding";
static const char strId[] = "id";
static const char strTextPlain[] = "text/plain";
static const char strDefaultCid[] = "cid:fwlk";
static const char strApplicationVndOmaDrmRightsXml[] =
        "application/vnd.oma.drm.rights+xml";
static const char strApplicationVndOmaDrmContent[] =
        "application/vnd.oma.drm.content";

static const size_t strlenContent = sizeof strContent - 1;
static const size_t strlenTextPlain = sizeof strTextPlain - 1;
static const size_t strlenDefaultCid = sizeof strDefaultCid - 1;

static const signed char base64Values[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1,
        -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1,
        -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51 };

/**
 * Acquires an unused converter session.
 *
 * @return A session ID.
 */
static int OmaDrmConv_AcquireSession() {
    int sessionId = -1;
    int i;
    pthread_mutex_lock(&sessionAcquisitionMutex);
    for (i = 0; i < MAX_NUM_SESSIONS; ++i) {
        if (sessionPtrs[i] == NULL) {
            sessionPtrs[i] = malloc(sizeof *sessionPtrs[i]);
            if (sessionPtrs[i] != NULL) {
                sessionId = i;
            }
            break;
        }
    }
    pthread_mutex_unlock(&sessionAcquisitionMutex);
    return sessionId;
}

/**
 * Checks whether a session ID is in range and currently in use.
 *
 * @param[in] sessionID A session ID.
 *
 * @return A Boolean value indicating whether the session ID is in range and currently in use.
 */
static int OmaDrmConv_IsValidSession(int sessionId) {
    return 0 <= sessionId && sessionId < MAX_NUM_SESSIONS
            && sessionPtrs[sessionId] != NULL;
}

/**
 * Releases a converter session.
 *
 * @param[in] sessionID A session ID.
 */
static void OmaDrmConv_ReleaseSession(int sessionId) {
    pthread_mutex_lock(&sessionAcquisitionMutex);
    assert(OmaDrmConv_IsValidSession(sessionId));
    memset(sessionPtrs[sessionId], 0, sizeof *sessionPtrs[sessionId]); // Zero out key data.
    free(sessionPtrs[sessionId]);
    sessionPtrs[sessionId] = NULL;
    pthread_mutex_unlock(&sessionAcquisitionMutex);
}

/**
 * Derives cryptographically independent keys for encryption and signing from the session key.
 *
 * @param[in,out] pSession A reference to a converter session.
 *
 * @return A status code.
 */
static int OmaDrmConv_DeriveKeys(OmaDrmConv_Session_t *pSession) {
    OmaDrmConv_Status_t status;
    struct OmaDrmConv_DeriveKeys_Data {
        AES_KEY sessionRoundKeys;
        unsigned char value[KEY_SIZE];
        unsigned char key[KEY_SIZE];
    };
    const size_t kSize = sizeof(struct OmaDrmConv_DeriveKeys_Data);
    struct OmaDrmConv_DeriveKeys_Data *pData = malloc(kSize);
    if (pData == NULL) {
        status = OmaDrmConv_Status_OutOfMemory;
    } else {
        if (AES_set_encrypt_key(pSession->sessionKey, KEY_SIZE_IN_BITS,
                &pData->sessionRoundKeys) != 0) {
            status = OmaDrmConv_Status_ProgramError;
        } else {
            // Encrypt the 16-byte value {0, 0, ..., 0} to produce the encryption key.
            memset(pData->value, 0, KEY_SIZE);
            AES_encrypt(pData->value, pData->key, &pData->sessionRoundKeys);
            if (AES_set_encrypt_key(pData->key, KEY_SIZE_IN_BITS,
                    &pSession->encryptionRoundKeys) != 0) {
                status = OmaDrmConv_Status_ProgramError;
            } else {
                // Encrypt the 16-byte value {1, 0, ..., 0} to produce the signing key.
                ++pData->value[0];
                AES_encrypt(pData->value, pData->key, &pData->sessionRoundKeys);
                HMAC_CTX_init(&pSession->signingContext);
                HMAC_Init_ex(&pSession->signingContext, pData->key, KEY_SIZE,
                        EVP_sha1(), NULL);
                status = OmaDrmConv_Status_OK;
            }
        }
        memset(pData, 0, kSize); // Zero out key data.
        free(pData);
    }
    return status;
}

/**
 * Checks whether a given character is valid in a boundary. Allows some non-standard characters that
 * are invalid according to RFC 2046 but nevertheless used by one vendor's DRM packager. Note that
 * the boundary may contain leading and internal spaces.
 *
 * @param[in] ch The character to check.
 *
 * @return A Boolean value indicating whether the given character is valid in a boundary.
 */
static int OmaDrmConv_IsBoundaryChar(int ch) {
    return isalnum(ch) || ch == '\'' || ch == '(' || ch == ')' || ch == '+'
            || ch == '_' || ch == ',' || ch == '-' || ch == '.' || ch == '/'
            || ch == ':' || ch == '=' || ch == '?' || ch == ' ' || ch == '%'
            || ch == '[' || ch == '&' || ch == '*' || ch == '^';
}

/**
 * Checks whether a given character should be considered whitespace, using a narrower definition
 * than the standard-library isspace() function.
 *
 * @param[in] ch The character to check.
 *
 * @return A Boolean value indicating whether the given character should be considered whitespace.
 */
static int OmaDrmConv_IsWhitespace(int ch) {
    return ch == ' ' || ch == '\t';
}

/**
 * Removes trailing spaces from the delimiter.
 *
 * @param[in,out] pSession A reference to a converter session.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_RightTrimDelimiter(
        OmaDrmConv_Session_t *pSession) {
    while (pSession->delimiterLength > 4
            && pSession->delimiter[pSession->delimiterLength - 1] == ' ') {
        --pSession->delimiterLength;
    }
    if (pSession->delimiterLength > 4) {
        return OmaDrmConv_Status_OK;
    }
    return OmaDrmConv_Status_SyntaxError;
}

/**
 * Matches the open delimiter.
 *
 * @param[in,out] pSession A reference to a converter session.
 * @param[in] ch A character.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_MatchOpenDelimiter(
        OmaDrmConv_Session_t *pSession, int ch) {
    OmaDrmConv_Status_t status = OmaDrmConv_Status_OK;
    switch (pSession->scannerState) {
    case OmaDrmConv_ScannerState_WantsFirstDash:
        if (ch == '-') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsSecondDash;
        } else if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        }
        break;
    case OmaDrmConv_ScannerState_WantsSecondDash:
        if (ch == '-') {
            // The delimiter starts with "\r\n--" (the open delimiter may omit the initial "\r\n").
            // The rest is the user-defined boundary that should come next.
            pSession->delimiter[0] = '\r';
            pSession->delimiter[1] = '\n';
            pSession->delimiter[2] = '-';
            pSession->delimiter[3] = '-';
            pSession->delimiterLength = 4;
            pSession->scannerState = OmaDrmConv_ScannerState_WantsBoundary;
            if (pSession->rightsXmlStatus
                    == OmaDrmConv_RightsXmlState_CaptureStart
                    || pSession->rightsXmlStatus
                            == OmaDrmConv_RightsXmlState_CaptureContinue) {
                pSession->rightsXmlStatus =
                        OmaDrmConv_RightsXmlState_CaptureEnd;
            }
        } else if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        }
        break;
    case OmaDrmConv_ScannerState_WantsCR:
        if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        }
        break;
    case OmaDrmConv_ScannerState_WantsLF:
        if (ch == '\n') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsFirstDash;
        } else if (ch != '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        }
        break;
    case OmaDrmConv_ScannerState_WantsBoundary:
        if (OmaDrmConv_IsBoundaryChar(ch)) {
            // The boundary may contain leading and internal spaces, so trailing spaces will also be
            // matched here. These will be removed later.
            if (pSession->delimiterLength < MAX_DELIMITER_LENGTH) {
                pSession->delimiter[pSession->delimiterLength++] = ch;
            } else if (ch != ' ') {
                status = OmaDrmConv_Status_SyntaxError;
            }
        } else if (ch == '\r') {
            status = OmaDrmConv_RightTrimDelimiter(pSession);
            if (status == OmaDrmConv_Status_OK) {
                pSession->scannerState =
                        OmaDrmConv_ScannerState_WantsBoundaryEnd;
            }
        } else if (ch == '\t') {
            status = OmaDrmConv_RightTrimDelimiter(pSession);
            if (status == OmaDrmConv_Status_OK) {
                pSession->scannerState =
                        OmaDrmConv_ScannerState_WantsWhitespace;
            }
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsWhitespace:
        if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsBoundaryEnd;
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsBoundaryEnd:
        if (ch == '\n') {
            pSession->parserState = OmaDrmConv_ParserState_WantsMimeHeaders;
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsMimeHeaderNameStart;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    default:
        status = OmaDrmConv_Status_ProgramError;
        break;
    }
    return status;
}

/**
 * Checks whether a given character is valid in a MIME header name.
 *
 * @param[in] ch The character to check.
 *
 * @return A Boolean value indicating whether the given character is valid in a MIME header name.
 */
static int OmaDrmConv_IsMimeHeaderNameChar(int ch) {
    return isgraph(ch) && ch != ':';
}

/**
 * Checks whether a given character is valid in a MIME header value.
 *
 * @param[in] ch The character to check.
 *
 * @return A Boolean value indicating whether the given character is valid in a MIME header value.
 */
static int OmaDrmConv_IsMimeHeaderValueChar(int ch) {
    return isgraph(ch) && ch != ';';
}

/**
 * Appends a character to the specified dynamically growing string.
 *
 * @param[in,out] pString A reference to a dynamically growing string.
 * @param[in] ch The character to append.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_StringAppend(OmaDrmConv_String_t *pString,
        int ch) {
    if (pString->length == pString->maxLength) {
        size_t newMaxLength = pString->maxLength + pString->lengthIncrement;
        char *newPtr = realloc(pString->ptr, newMaxLength + 1);
        if (newPtr == NULL) {
            return OmaDrmConv_Status_OutOfMemory;
        }
        pString->ptr = newPtr;
        pString->maxLength = newMaxLength;
    }
    pString->ptr[pString->length++] = ch;
    pString->ptr[pString->length] = '\0';
    return OmaDrmConv_Status_OK;
}

/**
 * Attempts to recognize the MIME header name and changes the scanner state accordingly.
 *
 * @param[in,out] pSession A reference to a converter session.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_RecognizeMimeHeaderName(
        OmaDrmConv_Session_t *pSession) {
    OmaDrmConv_Status_t status = OmaDrmConv_Status_OK;
    if (strncmp(pSession->mimeHeaderName.ptr, strContent, strlenContent) == 0) {
        if (strcmp(pSession->mimeHeaderName.ptr + strlenContent, strType)
                == 0) {
            if (pSession->contentType.ptr == NULL) {
                pSession->scannerState =
                        OmaDrmConv_ScannerState_WantsContentTypeStart;
            } else {
                status = OmaDrmConv_Status_SyntaxError;
            }
        } else if (strcmp(pSession->mimeHeaderName.ptr + strlenContent,
                strTransferEncoding) == 0) {
            if (pSession->contentTransferEncoding
                    == OmaDrmConv_ContentTransferEncoding_Undefined) {
                pSession->scannerState =
                        OmaDrmConv_ScannerState_WantsContentTransferEncodingStart;
            } else {
                status = OmaDrmConv_Status_SyntaxError;
            }
        } else if (strcmp(pSession->mimeHeaderName.ptr + strlenContent, strId)
                == 0) {
            // Added to support CD
            if (pSession->contentId.ptr == NULL) {
                pSession->scannerState =
                        OmaDrmConv_ScannerState_WantsContentIdStart;
            } else {
                status = OmaDrmConv_Status_SyntaxError;
            }
        } else {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        }
    } else {
        pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
    }
    return status;
}

/**
 * Applies defaults to missing MIME header values.
 *
 * @param[in,out] pSession A reference to a converter session.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_ApplyDefaults(
        OmaDrmConv_Session_t *pSession) {
    if (pSession->contentType.ptr == NULL) {
        // Content type is missing: default to "text/plain".
        pSession->contentType.ptr = malloc(sizeof strTextPlain);
        if (pSession->contentType.ptr == NULL) {
            return OmaDrmConv_Status_OutOfMemory;
        }
        memcpy(pSession->contentType.ptr, strTextPlain, sizeof strTextPlain);
        pSession->contentType.length = strlenTextPlain;
        pSession->contentType.maxLength = strlenTextPlain;
    }
    if (pSession->contentTransferEncoding
            == OmaDrmConv_ContentTransferEncoding_Undefined) {
        // Content transfer encoding is missing: default to binary.
        pSession->contentTransferEncoding =
                OmaDrmConv_ContentTransferEncoding_Binary;
    }

    if (pSession->contentId.ptr == NULL) {
        // Content id is missing: default to "cid:fwlk".
        int len = (sizeof strDefaultCid);
        pSession->contentId.ptr = malloc(len + 1);
        if (pSession->contentId.ptr == NULL) {
            return OmaDrmConv_Status_OutOfMemory;
        }
        memcpy(pSession->contentId.ptr + 1, strDefaultCid, len);
        pSession->contentId.length = len + 1;
        pSession->contentId.maxLength = pSession->contentId.length;
        pSession->contentId.ptr[0] = pSession->contentId.length;
    }
    return OmaDrmConv_Status_OK;
}

/**
 * Verifies that the content type is supported.
 *
 * @param[in,out] pSession A reference to a converter session.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_VerifyContentType(
        OmaDrmConv_Session_t *pSession) {
    OmaDrmConv_Status_t status;
    if (pSession->contentType.ptr == NULL) {
        status = OmaDrmConv_Status_ProgramError;
    } else if (strcmp(pSession->contentType.ptr,
            strApplicationVndOmaDrmRightsXml) == 0
            || strcmp(pSession->contentType.ptr, strApplicationVndOmaDrmContent)
                    == 0) {
        status = OmaDrmConv_Status_UnsupportedFileFormat;
    } else {
        status = OmaDrmConv_Status_OK;
    }
    return status;
}

/**
 * Writes the header of the output file.
 *
 * @param[in,out] pSession A reference to a converter session.
 * @param[out] pOutput The output from the conversion process.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_WriteHeader(
        OmaDrmConv_Session_t *pSession, OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status;
    if (pSession->contentType.length > UCHAR_MAX) {
        status = OmaDrmConv_Status_SyntaxError;
    } else {
        pSession->outputBufferSize = OUTPUT_BUFFER_SIZE_INCREMENT;
        pOutput->fromConvertData.pBuffer = malloc(pSession->outputBufferSize);
        if (pOutput->fromConvertData.pBuffer == NULL) {
            status = OmaDrmConv_Status_OutOfMemory;
        } else {
            size_t cidPos = TOP_HEADER_SIZE + pSession->contentType.length;
            size_t encryptedSessionKeyPos = cidPos + pSession->contentId.length;
            size_t dataSignaturePos = encryptedSessionKeyPos
                    + pSession->encryptedSessionKeyLength;
            size_t headerSignaturePos = dataSignaturePos + SHA1_HASH_SIZE;
            pSession->dataOffset = headerSignaturePos + SHA1_HASH_SIZE;
            memcpy(pSession->topHeader, topHeaderTemplate,
                    sizeof topHeaderTemplate);
            pSession->topHeader[CONTENT_TYPE_LENGTH_POS] =
                    (unsigned char) pSession->contentType.length;
            memcpy(pOutput->fromConvertData.pBuffer, pSession->topHeader,
                    TOP_HEADER_SIZE);
            memcpy((char *) pOutput->fromConvertData.pBuffer + TOP_HEADER_SIZE,
                    pSession->contentType.ptr, pSession->contentType.length);

            // add cid on encoded file header
            pSession->contentId.ptr[0] = pSession->contentId.length;
            memcpy((char *) pOutput->fromConvertData.pBuffer + cidPos,
                    pSession->contentId.ptr, pSession->contentId.length);

            memcpy(
                    (char *) pOutput->fromConvertData.pBuffer
                            + encryptedSessionKeyPos,
                    pSession->pEncryptedSessionKey,
                    pSession->encryptedSessionKeyLength);

            // Set the signatures to all zeros for now; they will have to be updated later.
            memset((char *) pOutput->fromConvertData.pBuffer + dataSignaturePos,
                    0, SHA1_HASH_SIZE);
            memset(
                    (char *) pOutput->fromConvertData.pBuffer
                            + headerSignaturePos, 0, SHA1_HASH_SIZE);

            pOutput->fromConvertData.numBytes = pSession->dataOffset;
            status = OmaDrmConv_Status_OK;
        }
    }
    return status;
}

/**
 * Matches the MIME headers.
 *
 * @param[in,out] pSession A reference to a converter session.
 * @param[in] ch A character.
 * @param[out] pOutput The output from the conversion process.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_MatchMimeHeaders(
        OmaDrmConv_Session_t *pSession, int ch, OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status = OmaDrmConv_Status_OK;
    switch (pSession->scannerState) {
    case OmaDrmConv_ScannerState_WantsMimeHeaderNameStart:
        if (OmaDrmConv_IsMimeHeaderNameChar(ch)) {
            pSession->mimeHeaderName.length = 0;
            status = OmaDrmConv_StringAppend(&pSession->mimeHeaderName,
                    tolower(ch));
            if (status == OmaDrmConv_Status_OK) {
                pSession->scannerState =
                        OmaDrmConv_ScannerState_WantsMimeHeaderName;
            }
        } else if (ch == '\r') {
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsMimeHeadersEnd;
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsMimeHeaderName:
        if (OmaDrmConv_IsMimeHeaderNameChar(ch)) {
            status = OmaDrmConv_StringAppend(&pSession->mimeHeaderName,
                    tolower(ch));
        } else if (ch == ':') {
            status = OmaDrmConv_RecognizeMimeHeaderName(pSession);
        } else if (OmaDrmConv_IsWhitespace(ch)) {
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsMimeHeaderNameEnd;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsMimeHeaderNameEnd:
        if (ch == ':') {
            status = OmaDrmConv_RecognizeMimeHeaderName(pSession);
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsContentTypeStart:
        if (OmaDrmConv_IsMimeHeaderValueChar(ch)) {
            status = OmaDrmConv_StringAppend(&pSession->contentType,
                    tolower(ch));
            if (status == OmaDrmConv_Status_OK) {
                pSession->scannerState =
                        OmaDrmConv_ScannerState_WantsContentType;
            }
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsContentType:
        if (OmaDrmConv_IsMimeHeaderValueChar(ch)) {
            status = OmaDrmConv_StringAppend(&pSession->contentType,
                    tolower(ch));
        } else if (ch == ';') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        } else if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (OmaDrmConv_IsWhitespace(ch)) {
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsMimeHeaderValueEnd;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsContentTransferEncodingStart:
        if (ch == 'b' || ch == 'B') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_A_OR_I;
        } else if (ch == '7' || ch == '8') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_B;
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_A_OR_I:
        if (ch == 'i' || ch == 'I') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_N;
        } else if (ch == 'a' || ch == 'A') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_S;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_N:
        if (ch == 'n' || ch == 'N') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_A;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_A:
        if (ch == 'a' || ch == 'A') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_R;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_R:
        if (ch == 'r' || ch == 'R') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_Y;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_Y:
        if (ch == 'y' || ch == 'Y') {
            pSession->contentTransferEncoding =
                    OmaDrmConv_ContentTransferEncoding_Binary;
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsContentTransferEncodingEnd;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_S:
        if (ch == 's' || ch == 'S') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_E;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_E:
        if (ch == 'e' || ch == 'E') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_6;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_6:
        if (ch == '6') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_4;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_4:
        if (ch == '4') {
            pSession->contentTransferEncoding =
                    OmaDrmConv_ContentTransferEncoding_Base64;
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsContentTransferEncodingEnd;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_B:
        if (ch == 'b' || ch == 'B') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_I;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_I:
        if (ch == 'i' || ch == 'I') {
            pSession->scannerState = OmaDrmConv_ScannerState_Wants_T;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_Wants_T:
        if (ch == 't' || ch == 'T') {
            pSession->contentTransferEncoding =
                    OmaDrmConv_ContentTransferEncoding_Binary;
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsContentTransferEncodingEnd;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_WantsContentTransferEncodingEnd:
        if (ch == ';') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        } else if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (OmaDrmConv_IsWhitespace(ch)) {
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsMimeHeaderValueEnd;
        } else {
            status = OmaDrmConv_Status_UnsupportedContentTransferEncoding;
        }
        break;
    case OmaDrmConv_ScannerState_WantsMimeHeaderValueEnd:
        if (ch == ';') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        } else if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsCR:
        if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        }
        break;
    case OmaDrmConv_ScannerState_WantsLF:
        if (ch == '\n') {
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsMimeHeaderNameStart;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsMimeHeadersEnd:
        if (ch == '\n') {
            status = OmaDrmConv_ApplyDefaults(pSession);
            if (status == OmaDrmConv_Status_OK) {
                status = OmaDrmConv_VerifyContentType(pSession);
                if (status != OmaDrmConv_Status_OK) {
                    if (strcmp(pSession->contentType.ptr,
                            strApplicationVndOmaDrmRightsXml) == 0) {
                        ALOGV(
                                "Found a Right Object, it may be CD file. Contenttype = %s",
                                pSession->contentType.ptr);
                        // Skip right xml header form CD file, then encrypt the CD content as FL
                        // Now, we will search next Contenttype field in this DRM messgage (file)
                        // Reseting following values in case of right+xml in the file header
                        strncpy(pSession->rightsXmlMimetype,
                                pSession->contentType.ptr,
                                pSession->contentType.length);
                        pSession->parserState =
                                OmaDrmConv_ParserState_WantsOpenDelimiter;
                        pSession->scannerState =
                                OmaDrmConv_ScannerState_WantsFirstDash;
                        free(pSession->mimeHeaderName.ptr);
                        free(pSession->contentType.ptr);
                        free(pSession->contentId.ptr);
                        free(pSession->rightsXml.ptr);
                        pSession->contentType = nullString;
                        pSession->contentId = nullString;
                        pSession->mimeHeaderName = nullString;
                        pSession->rightsXml = nullString;
                        pSession->contentTransferEncoding =
                                OmaDrmConv_ContentTransferEncoding_Undefined;

                        if (pSession->rightsXmlStatus
                                == OmaDrmConv_RightsXmlState_Invalid) {
                            pSession->rightsXmlStatus =
                                    OmaDrmConv_RightsXmlState_CaptureStart;
                        }
                        return status = OmaDrmConv_Status_OK;
                    }
                }
            }
            if (status == OmaDrmConv_Status_OK) {
                status = OmaDrmConv_WriteHeader(pSession, pOutput);
            }
            if (status == OmaDrmConv_Status_OK) {
                if (pSession->contentTransferEncoding
                        == OmaDrmConv_ContentTransferEncoding_Binary) {
                    pSession->parserState =
                            OmaDrmConv_ParserState_WantsBinaryEncodedData;
                } else {
                    pSession->parserState =
                            OmaDrmConv_ParserState_WantsBase64EncodedData;
                }
                pSession->scannerState = OmaDrmConv_ScannerState_WantsByte1;
            }
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsContentIdStart:
        // Added to support CD
        if (ch == '<' || ch == '>') {
            // skip both angular brackets from contentId
            break;
        }
        if (OmaDrmConv_IsMimeHeaderValueChar(ch)) {
            status = OmaDrmConv_StringAppend(&pSession->contentId, 0);
            status = OmaDrmConv_StringAppend(&pSession->contentId,
                    tolower('c'));
            status = OmaDrmConv_StringAppend(&pSession->contentId,
                    tolower('i'));
            status = OmaDrmConv_StringAppend(&pSession->contentId,
                    tolower('d'));
            status = OmaDrmConv_StringAppend(&pSession->contentId,
                    tolower(':'));
            status = OmaDrmConv_StringAppend(&pSession->contentId, tolower(ch));
            if (status == OmaDrmConv_Status_OK) {
                pSession->scannerState = OmaDrmConv_ScannerState_WantsContentId;
            }
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsContentId:
        // Added to support CD
        if (ch == '<' || ch == '>') {
            // skip both angular brackets from contentId
            break;
        }
        if (OmaDrmConv_IsMimeHeaderValueChar(ch)) {
            status = OmaDrmConv_StringAppend(&pSession->contentId, tolower(ch));
        } else if (ch == ';') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsCR;
        } else if (ch == '\r') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (OmaDrmConv_IsWhitespace(ch)) {
            pSession->scannerState =
                    OmaDrmConv_ScannerState_WantsMimeHeaderValueEnd;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    default:
        status = OmaDrmConv_Status_ProgramError;
        break;
    }
    return status;
}

/**
 * Increments the counter, treated as a 16-byte little-endian number, by one.
 *
 * @param[in,out] pSession A reference to a converter session.
 */
static void OmaDrmConv_IncrementCounter(OmaDrmConv_Session_t *pSession) {
    size_t i = 0;
    while ((++pSession->counter[i] == 0) && (++i < AES_BLOCK_SIZE))
        ;
}

/**
 * Encrypts the given character and writes it to the output buffer.
 *
 * @param[in,out] pSession A reference to a converter session.
 * @param[in] ch The character to encrypt and write.
 * @param[in,out] pOutput The output from the conversion process.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_WriteEncryptedChar(
        OmaDrmConv_Session_t *pSession, unsigned char ch,
        OmaDrmConv_Output_t *pOutput) {
    if (pOutput->fromConvertData.numBytes == pSession->outputBufferSize) {
        void *pBuffer;
        pSession->outputBufferSize += OUTPUT_BUFFER_SIZE_INCREMENT;
        pBuffer = realloc(pOutput->fromConvertData.pBuffer,
                pSession->outputBufferSize);
        if (pBuffer == NULL) {
            return OmaDrmConv_Status_OutOfMemory;
        }
        pOutput->fromConvertData.pBuffer = pBuffer;
    }
    if (++pSession->keyStreamIndex == AES_BLOCK_SIZE) {
        OmaDrmConv_IncrementCounter(pSession);
        pSession->keyStreamIndex = 0;
    }
    if (pSession->keyStreamIndex == 0) {
        AES_encrypt(pSession->counter, pSession->keyStream,
                &pSession->encryptionRoundKeys);
    }
    ch ^= pSession->keyStream[pSession->keyStreamIndex];
    ((unsigned char *) pOutput->fromConvertData.pBuffer)[pOutput->fromConvertData.numBytes++] =
            ch;
    ++pSession->numDataBytes;
    return OmaDrmConv_Status_OK;
}

/**
 * Matches binary-encoded content data and encrypts it, while looking out for the close delimiter.
 *
 * @param[in,out] pSession A reference to a converter session.
 * @param[in] ch A character.
 * @param[in,out] pOutput The output from the conversion process.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_MatchBinaryEncodedData(
        OmaDrmConv_Session_t *pSession, int ch, OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status = OmaDrmConv_Status_OK;
    switch (pSession->scannerState) {
    case OmaDrmConv_ScannerState_WantsByte1:
        if (ch != pSession->delimiter[pSession->delimiterMatchPos]) {
            // The partial match of the delimiter turned out to be spurious. Flush the matched bytes
            // to the output buffer and start over.
            size_t i;
            for (i = 0; i < pSession->delimiterMatchPos; ++i) {
                status = OmaDrmConv_WriteEncryptedChar(pSession,
                        pSession->delimiter[i], pOutput);
                if (status != OmaDrmConv_Status_OK) {
                    return status;
                }
            }
            pSession->delimiterMatchPos = 0;
        }
        if (ch != pSession->delimiter[pSession->delimiterMatchPos]) {
            // The current character isn't part of the delimiter. Write it to the output buffer.
            status = OmaDrmConv_WriteEncryptedChar(pSession, ch, pOutput);
        } else if (++pSession->delimiterMatchPos == pSession->delimiterLength) {
            // The entire delimiter has been matched. The only valid characters now are the "--"
            // that complete the close delimiter (no more message parts are expected).
            pSession->scannerState = OmaDrmConv_ScannerState_WantsFirstDash;
        }
        break;
    case OmaDrmConv_ScannerState_WantsFirstDash:
        if (ch == '-') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsSecondDash;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsSecondDash:
        if (ch == '-') {
            pSession->parserState = OmaDrmConv_ParserState_Done;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    default:
        status = OmaDrmConv_Status_ProgramError;
        break;
    }
    return status;
}

/**
 * Checks whether a given character is valid in base64-encoded data.
 *
 * @param[in] ch The character to check.
 *
 * @return A Boolean value indicating whether the given character is valid in base64-encoded data.
 */
static int OmaDrmConv_IsBase64Char(int ch) {
    return 0 <= ch && ch <= 'z' && base64Values[ch] >= 0;
}

/**
 * Matches base64-encoded content data and encrypts it, while looking out for the close delimiter.
 *
 * @param[in,out] pSession A reference to a converter session.
 * @param[in] ch A character.
 * @param[in,out] pOutput The output from the conversion process.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_MatchBase64EncodedData(
        OmaDrmConv_Session_t *pSession, int ch, OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status = OmaDrmConv_Status_OK;
    switch (pSession->scannerState) {
    case OmaDrmConv_ScannerState_WantsByte1:
    case OmaDrmConv_ScannerState_WantsByte1_AfterCRLF:
        if (OmaDrmConv_IsBase64Char(ch)) {
            pSession->ch = base64Values[ch] << 2;
            pSession->scannerState = OmaDrmConv_ScannerState_WantsByte2;
        } else if (ch == '\r') {
            pSession->savedScannerState =
                    OmaDrmConv_ScannerState_WantsByte1_AfterCRLF;
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (ch == '-') {
            if (pSession->scannerState
                    == OmaDrmConv_ScannerState_WantsByte1_AfterCRLF) {
                pSession->delimiterMatchPos = 3;
                pSession->scannerState = OmaDrmConv_ScannerState_WantsDelimiter;
            } else {
                status = OmaDrmConv_Status_SyntaxError;
            }
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsByte2:
        if (OmaDrmConv_IsBase64Char(ch)) {
            pSession->ch |= base64Values[ch] >> 4;
            status = OmaDrmConv_WriteEncryptedChar(pSession, pSession->ch,
                    pOutput);
            if (status == OmaDrmConv_Status_OK) {
                pSession->ch = base64Values[ch] << 4;
                pSession->scannerState = OmaDrmConv_ScannerState_WantsByte3;
            }
        } else if (ch == '\r') {
            pSession->savedScannerState = pSession->scannerState;
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsByte3:
        if (OmaDrmConv_IsBase64Char(ch)) {
            pSession->ch |= base64Values[ch] >> 2;
            status = OmaDrmConv_WriteEncryptedChar(pSession, pSession->ch,
                    pOutput);
            if (status == OmaDrmConv_Status_OK) {
                pSession->ch = base64Values[ch] << 6;
                pSession->scannerState = OmaDrmConv_ScannerState_WantsByte4;
            }
        } else if (ch == '\r') {
            pSession->savedScannerState = pSession->scannerState;
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (ch == '=') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsPadding;
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsByte4:
        if (OmaDrmConv_IsBase64Char(ch)) {
            pSession->ch |= base64Values[ch];
            status = OmaDrmConv_WriteEncryptedChar(pSession, pSession->ch,
                    pOutput);
            if (status == OmaDrmConv_Status_OK) {
                pSession->scannerState = OmaDrmConv_ScannerState_WantsByte1;
            }
        } else if (ch == '\r') {
            pSession->savedScannerState = pSession->scannerState;
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (ch == '=') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsWhitespace;
        } else if (!OmaDrmConv_IsWhitespace(ch)) {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsLF:
        if (ch == '\n') {
            pSession->scannerState = pSession->savedScannerState;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsPadding:
        if (ch == '=') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsWhitespace;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsWhitespace:
    case OmaDrmConv_ScannerState_WantsWhitespace_AfterCRLF:
        if (ch == '\r') {
            pSession->savedScannerState =
                    OmaDrmConv_ScannerState_WantsWhitespace_AfterCRLF;
            pSession->scannerState = OmaDrmConv_ScannerState_WantsLF;
        } else if (ch == '-') {
            if (pSession->scannerState
                    == OmaDrmConv_ScannerState_WantsWhitespace_AfterCRLF) {
                pSession->delimiterMatchPos = 3;
                pSession->scannerState = OmaDrmConv_ScannerState_WantsDelimiter;
            } else {
                status = OmaDrmConv_Status_SyntaxError;
            }
        } else if (OmaDrmConv_IsWhitespace(ch)) {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsWhitespace;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsDelimiter:
        if (ch != pSession->delimiter[pSession->delimiterMatchPos]) {
            status = OmaDrmConv_Status_SyntaxError;
        } else if (++pSession->delimiterMatchPos == pSession->delimiterLength) {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsFirstDash;
        }
        break;
    case OmaDrmConv_ScannerState_WantsFirstDash:
        if (ch == '-') {
            pSession->scannerState = OmaDrmConv_ScannerState_WantsSecondDash;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    case OmaDrmConv_ScannerState_WantsSecondDash:
        if (ch == '-') {
            pSession->parserState = OmaDrmConv_ParserState_Done;
        } else {
            status = OmaDrmConv_Status_SyntaxError;
        }
        break;
    default:
        status = OmaDrmConv_Status_ProgramError;
        break;
    }
    return status;
}

/**
 * Pushes a single character into the converter's state machine.
 *
 * @param[in,out] pSession A reference to a converter session.
 * @param[in] ch A character.
 * @param[in,out] pOutput The output from the conversion process.
 *
 * @return A status code.
 */
static OmaDrmConv_Status_t OmaDrmConv_PushChar(OmaDrmConv_Session_t *pSession,
        int ch, OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status;
    ++pSession->numCharsConsumed;
    switch (pSession->parserState) {
    case OmaDrmConv_ParserState_WantsOpenDelimiter:
        status = OmaDrmConv_MatchOpenDelimiter(pSession, ch);
        if (pSession->rightsXmlStatus == OmaDrmConv_RightsXmlState_CaptureStart
                || pSession->rightsXmlStatus
                        == OmaDrmConv_RightsXmlState_CaptureContinue) {
            OmaDrmConv_StringAppend(&pSession->rightsXml, tolower(ch));
        }
        if (pSession->rightsXmlStatus == OmaDrmConv_RightsXmlState_CaptureEnd) {
            pSession->rightsXml.ptr[pSession->rightsXml.length - 1] = '\0';
        }
        break;
    case OmaDrmConv_ParserState_WantsMimeHeaders:
        status = OmaDrmConv_MatchMimeHeaders(pSession, ch, pOutput);
        break;
    case OmaDrmConv_ParserState_WantsBinaryEncodedData:
        status = OmaDrmConv_MatchBinaryEncodedData(pSession, ch, pOutput);
        break;
    case OmaDrmConv_ParserState_WantsBase64EncodedData:
        if (ch == '\n'
                && pSession->scannerState != OmaDrmConv_ScannerState_WantsLF) {
            // Repair base64-encoded data that doesn't have carriage returns in its line breaks.
            status = OmaDrmConv_MatchBase64EncodedData(pSession, '\r', pOutput);
            if (status != OmaDrmConv_Status_OK) {
                break;
            }
        }
        status = OmaDrmConv_MatchBase64EncodedData(pSession, ch, pOutput);
        break;
    case OmaDrmConv_ParserState_Done:
        status = OmaDrmConv_Status_OK;
        break;
    default:
        status = OmaDrmConv_Status_ProgramError;
        break;
    }
    return status;
}

OmaDrmConv_Status_t OmaDrmConv_OpenSession(int *pSessionId,
        OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status;
    if (pSessionId == NULL || pOutput == NULL) {
        status = OmaDrmConv_Status_InvalidArgument;
    } else {
        *pSessionId = OmaDrmConv_AcquireSession();
        if (*pSessionId < 0) {
            status = OmaDrmConv_Status_TooManySessions;
        } else {
            OmaDrmConv_Session_t *pSession = sessionPtrs[*pSessionId];
            pSession->encryptedSessionKeyLength =
                    OmaDrmGlue_GetEncryptedKeyLength(KEY_SIZE);
            if (pSession->encryptedSessionKeyLength < AES_BLOCK_SIZE) {
                // The encrypted session key is used as the CTR-mode nonce, so it must be at least
                // the size of a single AES block.
                status = OmaDrmConv_Status_ProgramError;
            } else {
                pSession->pEncryptedSessionKey = malloc(
                        pSession->encryptedSessionKeyLength);
                if (pSession->pEncryptedSessionKey == NULL) {
                    status = OmaDrmConv_Status_OutOfMemory;
                } else {
                    if (!OmaDrmGlue_GetRandomNumber(pSession->sessionKey,
                            KEY_SIZE)) {
                        status = OmaDrmConv_Status_RandomNumberGenerationFailed;
                    } else if (!OmaDrmGlue_EncryptKey(pSession->sessionKey,
                            KEY_SIZE, pSession->pEncryptedSessionKey,
                            pSession->encryptedSessionKeyLength)) {
                        status = OmaDrmConv_Status_KeyEncryptionFailed;
                    } else {
                        status = OmaDrmConv_DeriveKeys(pSession);
                    }
                    if (status == OmaDrmConv_Status_OK) {
                        memset(pSession->sessionKey, 0, KEY_SIZE); // Zero out key data.
                        memcpy(pSession->counter,
                                pSession->pEncryptedSessionKey, AES_BLOCK_SIZE);
                        pSession->parserState =
                                OmaDrmConv_ParserState_WantsOpenDelimiter;
                        pSession->scannerState =
                                OmaDrmConv_ScannerState_WantsFirstDash;
                        pSession->rightsXmlStatus =
                                OmaDrmConv_RightsXmlState_Invalid;
                        pSession->numCharsConsumed = 0;
                        pSession->delimiterMatchPos = 0;
                        pSession->mimeHeaderName = nullString;
                        pSession->contentType = nullString;
                        pSession->contentTransferEncoding =
                                OmaDrmConv_ContentTransferEncoding_Undefined;
                        pSession->keyStreamIndex = -1;
                        pOutput->fromConvertData.pBuffer = NULL;
                        pOutput->fromConvertData.errorPos = INVALID_OFFSET;
                        pSession->contentId = nullString;
                        pSession->rightsXml = nullString;
                        memset(pSession->rightsXmlMimetype, '\0',
                                LENGTH_MIMETYPE);
                    } else {
                        free(pSession->pEncryptedSessionKey);
                    }
                }
            }
            if (status != OmaDrmConv_Status_OK) {
                OmaDrmConv_ReleaseSession(*pSessionId);
                *pSessionId = -1;
            }
        }
    }
    return status;
}

OmaDrmConv_Status_t OmaDrmConv_ConvertData(int sessionId, const void *pBuffer,
        size_t numBytes, OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status;
    if (!OmaDrmConv_IsValidSession(sessionId) || pBuffer == NULL
            || pOutput == NULL) {
        status = OmaDrmConv_Status_InvalidArgument;
    } else {
        size_t i;
        OmaDrmConv_Session_t *pSession = sessionPtrs[sessionId];
        pSession->dataOffset = 0;
        pSession->numDataBytes = 0;
        pOutput->fromConvertData.numBytes = 0;
        status = OmaDrmConv_Status_OK;

        for (i = 0; i < numBytes; ++i) {
            //usleep(50 * 1000);
            status = OmaDrmConv_PushChar(pSession, ((char *) pBuffer)[i],
                    pOutput);
            if (status != OmaDrmConv_Status_OK) {
                break;
            }
        }
        if (status == OmaDrmConv_Status_OK) {
            // Update the data signature.
            HMAC_Update(&pSession->signingContext,
                    &((unsigned char *) pOutput->fromConvertData.pBuffer)[pSession->dataOffset],
                    pSession->numDataBytes);
        } else if (status == OmaDrmConv_Status_SyntaxError) {
            pOutput->fromConvertData.errorPos = pSession->numCharsConsumed;
        }
    }
    return status;
}

OmaDrmConv_Status_t OmaDrmConv_CloseSession(int sessionId,
        OmaDrmConv_Output_t *pOutput) {
    OmaDrmConv_Status_t status;
    if (!OmaDrmConv_IsValidSession(sessionId) || pOutput == NULL) {
        status = OmaDrmConv_Status_InvalidArgument;
    } else {
        OmaDrmConv_Session_t *pSession = sessionPtrs[sessionId];
        free(pOutput->fromConvertData.pBuffer);
        if (pSession->parserState != OmaDrmConv_ParserState_Done) {
            pOutput->fromCloseSession.errorPos = pSession->numCharsConsumed;
            status = OmaDrmConv_Status_SyntaxError;
        } else {
            // install Rights if availavle
            if (pSession->rightsXml.ptr != NULL) {
                ALOGV(
                        "RIGHTS XML:\n ---------------------------- \n\n %s \n\n ---------------------------- \n",
                        pSession->rightsXml.ptr);
                OmaDrmRightsManager_ProcessRightsObject(pSession->rightsXml.ptr,
                        pSession->rightsXml.length,
                        pSession->rightsXmlMimetype);
            }
            // Finalize the data signature.
            unsigned int signatureSize = SHA1_HASH_SIZE;
            HMAC_Final(&pSession->signingContext,
                    pOutput->fromCloseSession.signatures, &signatureSize);
            if (signatureSize != SHA1_HASH_SIZE) {
                status = OmaDrmConv_Status_ProgramError;
            } else {
                // Calculate the header signature, which is a signature of the rest of the header
                // including the data signature.
                HMAC_Init_ex(&pSession->signingContext, NULL, KEY_SIZE, NULL,
                        NULL);
                HMAC_Update(&pSession->signingContext, pSession->topHeader,
                        TOP_HEADER_SIZE);
                HMAC_Update(&pSession->signingContext,
                        (unsigned char *) pSession->contentType.ptr,
                        pSession->contentType.length);
                HMAC_Update(&pSession->signingContext,
                        (unsigned char *) pSession->contentId.ptr,
                        pSession->contentId.length);
                HMAC_Update(&pSession->signingContext,
                        pSession->pEncryptedSessionKey,
                        pSession->encryptedSessionKeyLength);
                HMAC_Update(&pSession->signingContext,
                        pOutput->fromCloseSession.signatures, SHA1_HASH_SIZE);
                HMAC_Final(&pSession->signingContext,
                        &pOutput->fromCloseSession.signatures[SHA1_HASH_SIZE],
                        &signatureSize);
                if (signatureSize != SHA1_HASH_SIZE) {
                    status = OmaDrmConv_Status_ProgramError;
                } else {
                    pOutput->fromCloseSession.fileOffset = TOP_HEADER_SIZE
                            + pSession->contentType.length
                            + pSession->contentId.length
                            + pSession->encryptedSessionKeyLength;
                    status = OmaDrmConv_Status_OK;
                }
            }
            pOutput->fromCloseSession.errorPos = INVALID_OFFSET;
        }
        free(pSession->mimeHeaderName.ptr);
        free(pSession->contentType.ptr);
        free(pSession->contentId.ptr);
        free(pSession->rightsXml.ptr);
        free(pSession->pEncryptedSessionKey);
        HMAC_CTX_cleanup(&pSession->signingContext);
        OmaDrmConv_ReleaseSession(sessionId);
    }
    return status;
}

OmaDrmConv_Status_t OmaDrmConv_ConvertOpenFile(int inputFileDesc,
        OmaDrmConv_ReadFunc_t *fpReadFunc, int outputFileDesc,
        OmaDrmConv_WriteFunc_t *fpWriteFunc,
        OmaDrmConv_LSeekFunc_t *fpLSeekFunc, off64_t *pErrorPos) {
    OmaDrmConv_Status_t status;
    if (pErrorPos != NULL) {
        *pErrorPos = INVALID_OFFSET;
    }
    if (fpReadFunc == NULL || fpWriteFunc == NULL || fpLSeekFunc == NULL
            || inputFileDesc < 0 || outputFileDesc < 0) {
        status = OmaDrmConv_Status_InvalidArgument;
    } else {
        char *pReadBuffer = malloc(READ_BUFFER_SIZE);
        if (pReadBuffer == NULL) {
            status = OmaDrmConv_Status_OutOfMemory;
        } else {
            int sessionId;
            OmaDrmConv_Output_t output;
            status = OmaDrmConv_OpenSession(&sessionId, &output);
            if (status == OmaDrmConv_Status_OK) {
                ssize_t numBytesRead;
                OmaDrmConv_Status_t closeStatus;
                while ((numBytesRead = fpReadFunc(inputFileDesc, pReadBuffer,
                        READ_BUFFER_SIZE)) > 0) {
                    status = OmaDrmConv_ConvertData(sessionId, pReadBuffer,
                            (size_t) numBytesRead, &output);
                    if (status == OmaDrmConv_Status_OK) {
                        if (output.fromConvertData.pBuffer != NULL
                                && output.fromConvertData.numBytes > 0) {
                            ssize_t numBytesWritten = fpWriteFunc(
                                    outputFileDesc,
                                    output.fromConvertData.pBuffer,
                                    output.fromConvertData.numBytes);
                            if (numBytesWritten
                                    != (ssize_t) output.fromConvertData.numBytes) {
                                status = OmaDrmConv_Status_FileWriteError;
                                break;
                            }
                        }
                    } else {
                        if (status == OmaDrmConv_Status_SyntaxError
                                && pErrorPos != NULL) {
                            *pErrorPos = output.fromConvertData.errorPos;
                        }
                        break;
                    }
                } // end while
                if (numBytesRead < 0) {
                    status = OmaDrmConv_Status_FileReadError;
                }
                closeStatus = OmaDrmConv_CloseSession(sessionId, &output);
                if (status == OmaDrmConv_Status_OK) {
                    if (closeStatus != OmaDrmConv_Status_OK) {
                        if (closeStatus == OmaDrmConv_Status_SyntaxError
                                && pErrorPos != NULL) {
                            *pErrorPos = output.fromCloseSession.errorPos;
                        }
                        status = closeStatus;
                    } else if (fpLSeekFunc(outputFileDesc,
                            output.fromCloseSession.fileOffset, SEEK_SET) < 0) {
                        status = OmaDrmConv_Status_FileSeekError;
                    } else if (fpWriteFunc(outputFileDesc,
                            output.fromCloseSession.signatures,
                            FWD_LOCK_SIGNATURES_SIZE) != FWD_LOCK_SIGNATURES_SIZE) {
                        status = OmaDrmConv_Status_FileWriteError;
                    }
                }
            }
            free(pReadBuffer);
        }
    }
    return status;
}
