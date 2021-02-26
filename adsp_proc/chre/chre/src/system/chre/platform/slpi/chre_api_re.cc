/*
 * Copyright (C) 2017 The Android Open Source Project
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

#include "ash/debug.h"
#include "chre_api/chre/re.h"
#include "chre/util/macros.h"

DLL_EXPORT void chreLog(enum chreLogLevel level, const char *formatStr, ...) {
  enum ashLogLevel ashLevel;
  va_list args;

  switch (level) {
    case CHRE_LOG_ERROR:
      ashLevel = ASH_LOG_ERROR;
      break;
    case CHRE_LOG_WARN:
      ashLevel = ASH_LOG_WARN;
      break;
    case CHRE_LOG_INFO:
      ashLevel = ASH_LOG_INFO;
      break;
    case CHRE_LOG_DEBUG:
    default:
      ashLevel = ASH_LOG_DEBUG;
  }

  va_start(args, formatStr);
  ashVaLog(ASH_SOURCE_CHRE, ashLevel, formatStr, args);
  va_end(args);
}
