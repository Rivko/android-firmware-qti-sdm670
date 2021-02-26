/*==========================================================================
Description
   Header function for declarations of the functions defined in main.c

# Copyright (c) 2016 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/


#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdbool.h>

#define MAX_NUM_SOCKS 4
enum {
   BT = 0,
   FM,
   ANT,
   CTRL
} soc_ind;

//Byte written to CTRL_SOCK to cleanup and stop filter
#define STOP_WCNSS_FILTER 0xDD

#define MAX_RING_BUF_SIZE (8*1024)

#define OUT_RING_BUF 0
#define IN_RING_BUF 1
typedef struct {
   unsigned char ring_buf[MAX_RING_BUF_SIZE];
   unsigned char* wPtr;
   int size;
} RING_BUF;

typedef enum {
   DOWN = 0,
   UP
}sock_state;

/*Constants needed for debug feature
  to send special bytes on CMD timeout
*/
#define TIMER_NOT_CREATED 0x00
#define TIMER_CREATED 0x01
#define TIMER_ACTIVE 0x02

void report_soc_failure(void);

void commit_out_ring_buffer();
void commit_in_ring_buffer();
bool is_debug_out_ring_buf_enabled();
bool is_debug_in_ring_buf_enabled();
bool is_debug_force_special_bytes();
bool soc_crash_wait_timer_stop();
void update_ring_buffer (RING_BUF *rb, unsigned char* buf, int len, RING_BUF *swapb,  bool is_in_buf);
bool is_diag_enabled();
static void cleanup_and_suicide();
int init_hci_transport();

#endif//__MAIN_H__
