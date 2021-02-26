/*==========================================================================
Description
  Wcnss_ibs has the constants for SIBS (Software In-band sleep) protocol

# Copyright (c) 2013, 2016 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc..

===========================================================================*/

#ifndef _WCNSS_IBS_H_
#define _WCNSS_IBS_H_

/* HCI_IBS protocol messages */
#define IBS_WAKE_ACK        0xFC
#define IBS_WAKE_IND        0xFD
#define IBS_SLEEP_IND       0xFE
#define MAX_RING_BUF_SIZE (8*1024)

#ifdef __cplusplus
extern "C"
{
#endif
extern pthread_mutex_t signal_mutex;
int wcnss_wake_assert(void);
void wcnss_ibs_cleanup();
void wcnss_ibs_init(int fd);
void wcnss_tx_done(uint8_t tx_done);
void wcnss_device_can_sleep(void);
void wcnss_stop_idle_timeout_timer(void);
void wcnss_vote_off_clk(void);
void wcnss_vote_on_clk(void);

void wcnss_wakelock_init(char *lock_name);
void wcnss_wakelock_destroy(void);
void wcnss_acquire_wakelock();
void wcnss_release_wakelock();

#ifdef __cplusplus
}
#endif

#endif /* _WCNSS_IBS_H_ */
