#pragma once
/*=============================================================================
  @file sns_thread_prio.h

  This file contains the sensors thread priorities.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

// Number of worker threads assigned to pool 'P'
#define SNS_PRIO_HIGH_ISLAND_THREAD_LENGTH       0x02

// Number of worker threads assigned to pool 'I'
#define SNS_PRIO_LOW_ISLAND_THREAD_LENGTH        0x03


// Interrupt sensor thread priority
#define SNS_INTERRUPT_THREAD_PRIO               0x1e

// Low memory thread priority
#define SNS_LOW_MEM_THREAD_PRIO                 0x20

// Timer sensor thread priority
#define SNS_TIMER_THREAD_PRIO                   0x21

// Dae sensor thread priority
// One higher than sig sensor
#define DAE_RX_DONE_PRIO                        0x21

// Signal sensor thread priority
#define SNS_SIG_SENSR_THREAD_PRIO               0x22

// Async-com-port sensor thread priority
#define SNS_ASCP_THREAD_PRIO                    0x22

// Client manager sensor thread priority
#define SNS_CM_THREAD_PRIO                      0x75

// Remote-proc sensor thread priority
#define SNS_REMOTE_PROC_STATE_THREAD_PRIO       0x78

// Device mode sensor thread priority
#define SNS_DEVICE_MODE_THREAD_PRIO             0xD8

// Worker thread priority for the pools 'P','Q'
#define SNS_THREAD_PRIO_HIGH                    0x80

// Worker thread priority for the pools 'I','B'
#define SNS_THREAD_PRIO_LOW                     0x82

// Registry sensor priority
#define SNS_REGISTRY_THREAD_PRIO                0x89

// Ext-svc sensor priority
#define SNS_EXT_SVC_THREAD_PRIO                 0x8A

// Diag service priority
#define SNS_DIAG_THREAD_PRIO                    0x90
