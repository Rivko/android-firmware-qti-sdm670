//------------------------------------------------------------------------------
//
// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//
//------------------------------------------------------------------------------

// export functions
.global TimeSyncMutexLock
.global TimeSyncMutexUnlock

TimeSyncMutexLock:
      MOV W1, #0x1      // Value to write to lock mutex
TRY:  LDXR W2, [X0]     // X0 has address of mutex to lock
      CBNZ W2, TRY      // If already locked, wait
      STXR W2, W1, [X0] // Not locked, attempt to lock it
      CBNZ W2, TRY      // Check if Store-Exclusive failed, repeat if failed
      // Lock acquired
      DMB SY            // Data Memory Barrier, required before accessing protected resource
      RET               // Return

TimeSyncMutexUnlock:
      MOV W1, #0x0      // Value to write to unlock mutex
      DMB SY            // Required before releasing protected resource
      STR W1, [X0]      // Unlock mutex
      RET               // Return