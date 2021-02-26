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

#ifndef __OMADRMDCFFILE_H__
#define __OMADRMDCFFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include "OmaDrmRightsManager.h"

#define MAX_ENCRYPTION_METHOD_LEN                            64
#define MAX_RIGHTS_ISSUER_LEN                                256
#define MAX_CONTENT_NAME_LEN                                 64
#define MAX_CONTENT_DESCRIPTION_LEN                          256
#define MAX_CONTENT_VENDOR_LEN                               256
#define MAX_ICON_URI_LEN                                     256
#define MAX_CONTENT_TYPE_LEN                                 64
#define MAX_CONTENT_ID_LEN                                   256

#define HEADER_ENCRYPTION_METHOD                             "Encryption-Method: "
#define HEADER_RIGHTS_ISSUER                                 "Rights-Issuer: "
#define HEADER_CONTENT_NAME                                  "Content-Name: "
#define HEADER_CONTENT_DESCRIPTION                           "Content-Description: "
#define HEADER_CONTENT_VENDOR                                "Content-Vendor: "
#define HEADER_ICON_URI                                      "Icon-Uri: "

#define HEADER_ENCRYPTION_METHOD_LEN                         19
#define HEADER_RIGHTS_ISSUER_LEN                             15
#define HEADER_CONTENT_NAME_LEN                              14
#define HEADER_CONTENT_DESCRIPTION_LEN                       21
#define HEADER_CONTENT_VENDOR_LEN                            16
#define HEADER_ICON_URI_LEN                                  10

#define UINT_VAR_FLAG                                        0x80
#define UINT_VAR_DATA                                        0x7F
#define MAX_UINT_VAR_BYTE                                    5
#define DRM_UINT_VAR_ERR                                     -1

typedef struct OmaDrmFile_DCF_Info {
    uint8_t version;
    uint8_t contentTypeLen; /**< Length of the ContentType field */
    uint8_t contentIdLen; /**< Length of the ContentURI field */
    uint8_t unUsed;
    uint8_t contentType[MAX_CONTENT_TYPE_LEN]; /**< The MIME media type of the plaintext data */
    uint8_t contentId[MAX_CONTENT_ID_LEN]; /**< The unique identifier of this content object */
    int32_t headersLen; /**< Length of the Headers field */
    int32_t encryptedDataLen; /**< Length of the encrypted data field */
    int32_t decryptedDataLen; /**< Length of the decrypted data field */
    uint8_t encryptionMethod[MAX_ENCRYPTION_METHOD_LEN]; /**< Encryption method */
    uint8_t rightsIssuer[MAX_RIGHTS_ISSUER_LEN]; /**< Rights issuer */
    uint8_t contentName[MAX_CONTENT_NAME_LEN]; /**< Content name */
    uint8_t contentDescription[MAX_CONTENT_DESCRIPTION_LEN]; /**< Content description */
    uint8_t contentVendor[MAX_CONTENT_VENDOR_LEN]; /**< Content vendor */
    uint8_t iconUri[MAX_ICON_URI_LEN]; /**< Icon URI */
    uint8_t dataOffset;
    int fileDesc;
    int tempFileDesc;
    ssize_t filePos;
    char* filePath;
} OmaDrmFile_DCF_Info_t;

int OmaDrmDcfFile_ParserDcf(int fileDesc, OmaDrmFile_DCF_Info_t *pDcfInfo);

ssize_t OmaDrmDcfFile_read(int fileDesc, void *pBuffer, size_t numBytes,
        OmaDrmFile_DCF_Info_t *info);

off64_t OmaDrmDcfFile_lseek(int fileDesc, off64_t offset, int whence,
        OmaDrmFile_DCF_Info_t *info);

int OmaDrmDcfFile_Release(int fileDesc, OmaDrmFile_DCF_Info_t *info);

void OmaDrmDcfFile_DumpDcfInfo(OmaDrmFile_DCF_Info_t *info);

int OmaDrmDcfFile_DecryptData(int fileDesc, OmaDrmFile_DCF_Info_t *info);

int OmaDrmDcfFile_CopyTempDcf(int inFile, char* path);

#ifdef __cplusplus
}
#endif

#endif // __OMADRMDCFFILE_H__
