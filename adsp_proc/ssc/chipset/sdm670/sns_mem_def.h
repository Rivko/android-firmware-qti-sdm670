#pragma once
/*
 * File : sns_mem_def.h
 * Description : This file contains the heap size information
 *
 * Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#ifndef SNS_TARGET_HEXAGON_IOT_670
#define SNS_MAIN_HEAP_ALLOC                    3072
#define SNS_BATCH_HEAP_ALLOC                   1536
#define SNS_EVENT_HEAP_ALLOC                   1024
#define SNS_ISLAND_HEAP_ALLOC                  140
#define SNS_FW_EVENT_SERVICE_BUFFER_ALLOC      64
#else
#define SNS_MAIN_HEAP_ALLOC                    350
#define SNS_BATCH_HEAP_ALLOC                   250
#define SNS_EVENT_HEAP_ALLOC                   10
#define SNS_ISLAND_HEAP_ALLOC                  100
#define SNS_FW_EVENT_SERVICE_BUFFER_ALLOC      32
#endif
