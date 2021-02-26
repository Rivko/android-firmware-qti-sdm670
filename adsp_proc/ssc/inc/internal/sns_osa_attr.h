#pragma once

/*======================================================================================
  @file sns_osa_attr.h

  @brief
  Definitions for OSA attributes

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/internal/sns_osa_attr.h#2 $
  $DateTime: 2018/07/18 19:10:32 $
  $Change: 16675905 $
  ======================================================================================*/

/*
*****************************************************************************************
                               Includes
*****************************************************************************************
*/
#if defined(SSC_TARGET_HEXAGON)
#define __SIZEOF_ATTR_THREAD    64
#define __SIZEOF_ATTR_LOCK      64
#define __SIZEOF_ATTR_TIMER     64
#define __SIZEOF_ATTR_SEM       64
#define __SIZEOF_LOCK           32
#elif defined(SDC)
#define __SIZEOF_ATTR_THREAD    64
#define __SIZEOF_ATTR_LOCK      64
#define __SIZEOF_ATTR_TIMER     64
#define __SIZEOF_ATTR_SEM       64
#define __SIZEOF_LOCK           64
#elif defined(SSC_TARGET_X86)
#define __SIZEOF_ATTR_THREAD    88
#define __SIZEOF_ATTR_LOCK      64
#define __SIZEOF_ATTR_TIMER     64
#define __SIZEOF_ATTR_SEM       64
#define __SIZEOF_LOCK           40
#endif

/*
*****************************************************************************************
                               Typedefs
*****************************************************************************************
*/
typedef enum
{
  SNS_OSA_MEM_TYPE_NORMAL,
  SNS_OSA_MEM_TYPE_ISLAND,
  SNS_OSA_MEM_TYPE_MAX
} sns_osa_mem_type;


