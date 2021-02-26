/***********************************************************************
 * fs_profile.h
 *
 * The EFS profiling module.
 * Copyright (C) 2012-2013 QUALCOMM Technologies, Inc.
 *
 * This file provides api to help profile the efs api usage and parameters.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/EfsLib/src/fs_profile.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-05-06   nr    Add support to enable and disable profiling.
2013-03-28   nr    Add support to profile efs_initialize.
2013-01-17   wek   Featurize the profile module so it can be turned off.
2012-12-26   nr    Implement support of Extensive profiling.
2012-11-28   nr    Create

===========================================================================*/

#ifndef __FS_PROFILE_H__
#define __FS_PROFILE_H__

#include "comdef.h"

/* Defines the max number profile id supported. */
#define FS_PROFILE_MAX_SUPPORTED_ID 32767

/* The profile ID's are dived into Levels 0, 1 and 2
 * LEVEL 0: These are important api's and are profiled with basic, advanced and
 *           extended profiling.
 * Level 1: These api's are are not as important as level 0 and are profiled
 *          with basic and extended profiling.
 * Level 2: These api's are are not as important as level 0 or 1 and are only
 *          profiled with extended profiling.
 */

/* Defines the max id that is profiled in level zero profiling. */
#define FS_PROFILE_LEVEL_0_MAX_ID 10

/* Defines the max id that is profiled in level one profiling. */
#define FS_PROFILE_LEVEL_1_MAX_ID 127

/* Invalid ID to allow easy extended encoding of profile id in extended
 * profiling */
#define FS_PROFILE_INVALID_ID (FS_PROFILE_LEVEL_1_MAX_ID + 1)

/* Defines the max id that is profiled in level two profiling. */
#define FS_PROFILE_LEVEL_2_MAX_ID FS_PROFILE_MAX_SUPPORTED_ID



enum fs_profile_id
{
  /****************************************************************************
  * List of Important ID's to Profile.
  * These are profiled in level 1 profiling.
  ****************************************************************************/
  FS_PROFILE_ID_OPEN,
  FS_PROFILE_ID_READ,
  FS_PROFILE_ID_WRITE,
  FS_PROFILE_ID_CLOSE,
  FS_PROFILE_ID_PUT,
  FS_PROFILE_ID_GET,
  FS_PROFILE_ID_MKDIR,
  FS_PROFILE_ID_SYNC,

  FS_PROFILE_ID_LEVEL_1_MAX, /* Must be  <= FS_PROFILE_LEVEL_1_MAX_ID + 1 */

  /***************************************************************************/
  FS_PROFILE_ID_INVALID = FS_PROFILE_INVALID_ID,

  /****************************************************************************
  * List of secondary id's. These are profiled in level 2 profiling.
  ****************************************************************************/
  FS_PROFILE_ID_SYNC_NO_WAIT,
  FS_PROFILE_ID_EFS_INITIALIZE,

  FS_PROFILE_ID_LEVEL_2_MAX, /* Must be  <= FS_PROFILE_MAX_SUPPORTED_ID + 1 */

  /***************************************************************************/

};

#ifndef FEATURE_EFS_DISABLE_PROFILE

#include "fs_timetick.h"

union fs_profile_time
{
  fs_timetick start_timetick; /* The start time tick of the api */
  fs_time_us time_in_us; /* Net timeticks of api converted to microseconds. */
};

struct fs_profile_handle
{
  union fs_profile_time time; /* Variables to measure time. */
  enum fs_profile_id profile_id; /* Profile id of the api. */
  void *extensive_handle; /* Handle for extensive profiling module. */
  uint8 in_use;
};

/* Initialize the profiling module */
void fs_profile_init (void);

/* Open and set the handle ready to use for that api. */
void fs_profile_open (struct fs_profile_handle *handle,
                      enum fs_profile_id profile_id);

/* Start profiling and begin counting time. */
void fs_profile_start (struct fs_profile_handle *handle);

/* Stop profiling and end time measurement. */
void fs_profile_stop (struct fs_profile_handle *handle);

/* Print integer to the profile buffer. */
void fs_profile_log_int (struct fs_profile_handle *handle, uint32 value);

/* Print string to the profile buffer. */
void fs_profile_log_string (struct fs_profile_handle *handle,
                            const char *string);

/* Close the profile handle. */
void fs_profile_close (struct fs_profile_handle *handle);

/* Flush the profile logs (if any) to the Filesystem form memory. */
void fs_profile_flush_logs (void);

/* Enable and disable EFS profiling. */
void fs_profile_enable (void);
void fs_profile_disable (void);

#else /* FEATURE_EFS_DISABLE_PROFILE is not defined */

struct fs_profile_handle
{
  uint8 dummy;
};

static inline void fs_profile_init (void)
{
}

static inline void fs_profile_open (struct fs_profile_handle *handle,
                                    enum fs_profile_id profile_id)
{
  (void) handle; (void) profile_id;
}

static inline void fs_profile_start (struct fs_profile_handle *handle)
{
  (void) handle;
}

static inline void fs_profile_stop (struct fs_profile_handle *handle)
{
  (void) handle;
}

static inline void fs_profile_log_int (struct fs_profile_handle *handle,
                                       uint32 value)
{
  (void) handle; (void) value;
}

static inline void fs_profile_log_string (struct fs_profile_handle *handle,
                                          const char *string)
{
  (void) handle; (void) string;
}

static inline void fs_profile_close (struct fs_profile_handle *handle)
{
  (void) handle;
}

static inline void fs_profile_flush_logs (void)
{
}

static inline void fs_profile_enable (void)
{
}

static inline void fs_profile_disable (void)
{
}

#endif /* FEATURE_EFS_DISABLE_PROFILE */

#endif /* not __FS_PROFILE_H__ */
