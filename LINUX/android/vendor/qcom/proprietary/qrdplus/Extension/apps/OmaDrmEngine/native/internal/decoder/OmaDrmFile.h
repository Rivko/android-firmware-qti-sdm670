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

#ifndef __OMADRMFILE_H__
#define __OMADRMFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include "OmaDrmDcfFile.h"
#include "OmaDrmRightsManager.h"

/**
 * Attaches to an open Forward Lock file. The file position is assumed to be at the beginning of the
 * file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 *
 * @return A status code.
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int OmaDrmFile_attach(int fileDesc);

/**
 * Reads the specified number of bytes from an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 * @param[out] pBuffer A reference to the buffer that should receive the read data.
 * @param[in] numBytes The number of bytes to read.
 *
 * @return The number of bytes read.
 * @retval -1 Failure.
 */
ssize_t OmaDrmFile_read(int fileDesc, void *pBuffer, size_t numBytes);

/**
 * Updates the file position within an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 * @param[in] offset The offset with which to update the file position.
 * @param[in] whence One of SEEK_SET, SEEK_CUR, and SEEK_END.
 *
 * @return The new file position.
 * @retval ((off64_t)-1) Failure.
 */
off64_t OmaDrmFile_lseek(int fileDesc, off64_t offset, int whence);

/**
 * Detaches from an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 *
 * @return A status code.
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int OmaDrmFile_detach(int fileDesc);

/**
 * Closes an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 *
 * @return A status code.
 * @retval 0 Success.
 * @retval -1 Failure.
 */
int OmaDrmFile_close(int fileDesc);

/**
 * Checks the data integrity of an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 *
 * @return A Boolean value indicating whether the integrity check was successful.
 */
int OmaDrmFile_CheckDataIntegrity(int fileDesc);

/**
 * Checks the header integrity of an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 *
 * @return A Boolean value indicating whether the integrity check was successful.
 */
int OmaDrmFile_CheckHeaderIntegrity(int fileDesc);

/**
 * Checks both the data and header integrity of an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 *
 * @return A Boolean value indicating whether the integrity check was successful.
 */
int OmaDrmFile_CheckIntegrity(int fileDesc);

/**
 * Returns the content type of an open Forward Lock file.
 *
 * @param[in] fileDesc The file descriptor of an open Forward Lock file.
 *
 * @return
 *   A reference to the content type. The reference remains valid as long as the file is kept open.
 */
const char *OmaDrmFile_GetContentType(int fileDesc);

int OmaDrmFile_CheckAndGetDrmRights(int fileDesc,
        OmaDrmRightsManager_Rights_t *rights);

int OmaDrmFile_ConsumeDrmRights(int fileDesc);

int OmaDrmFile_IsForwardLockFile(int fileDesc);

OmaDrmFile_DCF_Info_t* OmaDrmFile_GetDcfInfo(int fileDesc);

int OmaDrmFile_DecodeIfNeeded(int fileDesc);

int OmaDrmFile_SetPlaybackStatus(int fileDesc, int playbackStatus);

int OmaDrmFile_CopyTempDcf(int fileDesc, char* path);

#ifdef __cplusplus
}
#endif

#endif // __OMADRMFILE_H__
