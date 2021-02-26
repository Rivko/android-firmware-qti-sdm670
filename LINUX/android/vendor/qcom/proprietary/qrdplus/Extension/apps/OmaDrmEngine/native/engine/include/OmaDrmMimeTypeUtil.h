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

#ifndef __OMADRMMIMETYPEUTIL_H__
#define __OMADRMMIMETYPEUTIL_H__

#include <utils/String8.h>

namespace android {

class OmaDrmMimeTypeUtil {

public:

    OmaDrmMimeTypeUtil() {}

    virtual ~OmaDrmMimeTypeUtil() {}

/**
 * May convert the mimetype if there is a well known
 * replacement mimetype otherwise the original mimetype
 * is returned.
 *
 * @param mimeType - mimetype in lower case to convert.
 *
 * @return mimetype or null.
 */
static String8 convertMimeType(String8& mimeType);

};
};

#endif /* __OMADRMMIMETYPEUTIL_H__ */
