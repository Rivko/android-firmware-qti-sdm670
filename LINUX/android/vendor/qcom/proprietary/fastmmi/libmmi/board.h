/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SYSTEM_CORE_MMI_BOARD__
#define __SYSTEM_CORE_MMI_BOARD__

#define OS_ANDROID "android"
#define OS_ANDROID_WEAR "android_wear"
#define OS_ANDROID_BRILLO "android_brillo"

typedef struct {
    char name[64];
    uint soc_id;
} soc_info_t;

/*!
    @ingroup diag_misc
*/
int get_os_type(char *buf, int size);

/*!
    @ingroup diag_misc
*/
int get_chip_name(char *buf, int size);


/*!
    @ingroup diag_misc
*/
int get_subtype_id(char *buf, int size);

/*!
    @ingroup diag_misc
*/
int get_platform_version(char *buf, int size);
#endif
