/*
 *
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#ifndef __ACDBRTAC_H__
#define __ACDBRTAC_H__

void acdb_rtac_init(void);
void acdb_rtac_exit(void);

int32_t acdb_rtac_callback(uint16_t cmd, uint8_t *req_buf_ptr,
                uint32_t req_buf_len, uint8_t *resp_buf_ptr,
                uint32_t resp_buf_len, uint32_t *resp_buf_bytes_filled);

int32_t get_audio_copp_id(uint32_t topo_id, uint32_t app_type,
                          uint32_t acdb_id, uint32_t *copp_id);
int32_t get_audio_popp_id(uint32_t topo_id, uint32_t app_type,
                          uint32_t acdb_id, uint32_t* popp_id);

#endif

