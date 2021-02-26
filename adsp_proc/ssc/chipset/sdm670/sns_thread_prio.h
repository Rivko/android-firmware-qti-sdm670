#pragma once
/*
 * File : sns_thread_prio.h
 * Description : This file contains the sensors thread
 *               priorities.
 *
 * Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
 
// Number of worker threads assigned to pool 'Q'
#define SNS_PRIO_HIGH_NORMAL_THREAD_LENGTH       0x02

// Number of worker threads assigned to pool 'B'
#define SNS_PRIO_LOW_NORMAL_THREAD_LENGTH        0x02

// Number of worker threads assigned to pool 'P'
#define SNS_PRIO_HIGH_ISLAND_THREAD_LENGTH       0x02

// Number of worker threads assigned to pool 'I'
#define SNS_PRIO_LOW_ISLAND_THREAD_LENGTH        0x02


// Interrupt sensor thread priority
#define SNS_INTERRUPT_THREAD_PRIO 			     0xDF

// Low memory thread priority
#define SNS_LOW_MEM_THREAD_PRIO 			     0xE0

// Timer sensor thread priority
#define SNS_TIMER_THREAD_PRIO 				     0xE1

// Dae sensor thread priority
// One higher than sig sensor
#define DAE_RX_DONE_PRIO                         0xE1

// Signal sensor thread priority
#define SNS_SIG_SENSR_THREAD_PRIO 			     0xE2

// Async-com-port sensor thread priority
#define SNS_ASCP_THREAD_PRIO 				     0xE2

// Client manager sensor thread priority
#define SNS_CM_THREAD_PRIO 					     0xE3

// Remote-proc sensor thread priority
#define SNS_REMOTE_PROC_STATE_THREAD_PRIO 	     0xE3

// Device mode sensor thread priority
#define SNS_DEVICE_MODE_THREAD_PRIO              0xE3

// Worker thread priority for the pools 'P','Q'
#define SNS_THREAD_PRIO_HIGH 				     0xE4

// Worker thread priority for the pools 'I','B'
#define SNS_THREAD_PRIO_LOW 				     0xE5

// Registry sensor priority
#define SNS_REGISTRY_THREAD_PRIO 			     0xE6

// Ext-svc sensor priority
#define SNS_EXT_SVC_THREAD_PRIO 			     0xE7

// Diag service priority
#define SNS_DIAG_THREAD_PRIO		    	     0xE8
