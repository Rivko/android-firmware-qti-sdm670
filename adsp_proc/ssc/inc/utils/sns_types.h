#pragma once
/**
 * @file sns_types.h
 *
 * return size of an array
 *
 * Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/inc/utils/sns_types.h#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 */

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

/* Rounds off the size to align to the next 8 byte address */
#ifndef ALIGN_8
#define ALIGN_8(size) ((((size_t)(size))+0x07)&0xFFFFFFF8)
#endif
