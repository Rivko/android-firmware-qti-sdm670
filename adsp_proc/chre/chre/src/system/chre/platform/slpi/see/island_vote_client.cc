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

#include "chre/platform/slpi/see/island_vote_client.h"

#include <cinttypes>

#include "chre/platform/assert.h"
#include "chre/platform/fatal_error.h"
#include "chre/platform/log.h"
#include "chre/platform/system_time.h"
#include "chre/util/lock_guard.h"

namespace chre {

IslandVoteClient::IslandVoteClient(const char *clientName) {
#ifdef CHRE_SLPI_UIMG_ENABLED
  mClientHandle = sns_island_aggregator_register_client(clientName);
  if (mClientHandle == nullptr) {
    FATAL_ERROR("Island aggregator client register failed");
  }
#endif // CHRE_SLPI_UIMG_ENABLED
}

IslandVoteClient::~IslandVoteClient() {
#ifdef CHRE_SLPI_UIMG_ENABLED
  sns_island_aggregator_deregister_client(mClientHandle);
#endif // CHRE_SLPI_UIMG_ENABLED
}

bool IslandVoteClient::voteBigImage(bool bigImage) {
#ifdef CHRE_SLPI_UIMG_ENABLED
  bool success = true;

  {
    LockGuard<Mutex> lock(mMutex);
    mLastBigImageRequest = bigImage;

    bool needBigImage = (bigImage || mBigImageRefCount > 0);
    if (needBigImage != mLastBigImageVote) {
      success = voteSnsPowerMode(needBigImage);
      mLastBigImageVote = needBigImage;
    }
  }
  return success;
#else
  return true;
#endif // CHRE_SLPI_UIMG_ENABLED
}

#ifdef CHRE_SLPI_UIMG_ENABLED
void IslandVoteClient::incrementBigImageRefCount() {
  LockGuard<Mutex> lock(mMutex);

  if (mBigImageRefCount++ == 0) {
    mRefCountStart = Milliseconds(SystemTime::getMonotonicTime());
    LOGW("Big image ref count begins");

    if (!mLastBigImageVote) {
      // Do not call voteBigImage() directly as it will override
      // mLastBigImageRequest.
      voteSnsPowerMode(true /* bigImage */);
      mLastBigImageVote = true;
    }
  }
}

void IslandVoteClient::decrementBigImageRefCount() {
  LockGuard<Mutex> lock(mMutex);
  CHRE_ASSERT_LOG(mBigImageRefCount > 0,
                  "Tried to decrement big image ref count when it's 0");

  if (--mBigImageRefCount == 0) {
    uint64_t duration =
        Milliseconds(SystemTime::getMonotonicTime()).getMilliseconds()
        - mRefCountStart.getMilliseconds();
    LOGW("Big image ref count ends: %" PRIu64 " ms", duration);

    // There's no big image activity now, restore the intended uimg power state.
    if (!mLastBigImageRequest) {
      voteSnsPowerMode(false /* bigImage */);
      mLastBigImageVote = false;
    }
  }
}

bool IslandVoteClient::voteSnsPowerMode(bool bigImage) {
  sns_rc rc = bigImage
      ? sns_island_block(mClientHandle) : sns_island_unblock(mClientHandle);

  // TODO: (b/74524281) define success = (rc == SNS_RC_SUCCESS).
  bool success = (rc != SNS_RC_FAILED);
  if (!success) {
    // Note that FATAL_ERROR must not be used here, because this can be called
    // from preFatalError() (not that we should use it here regardless)
    LOGE("Failed to vote for bigImage %d with result %d", bigImage, rc);
  }
  return success;
}
#endif  // CHRE_SLPI_UIMG_ENABLED

//! Explicitly instantiate the IslandVoteClient singleton to reduce code size.
template class Singleton<IslandVoteClient>;

} // namespace chre
