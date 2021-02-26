{# Copyright (c) 2017 Qualcomm Technologies, Inc.
	All Rights Reserved.
	Confidential and Proprietary - Qualcomm Technologies, Inc. #}

/*
 * Created on May 19, 2017
 *
 * @author: amavrin
 *
 *   Note: this file has to contain only explicit basic types, that can not be misinterpreted on other platform
 *   I.g. uintptr_t should not be used - use unsigned long long int - this will cover both 32 and 64 bit targets
 *
 */
// we can't use <stdint.h> as this is not available in the kernel build

typedef struct {
	unsigned long long int mp_va_addr; /* MP data VA adress */
	unsigned long long int mp_offset; /* offset of MP data from .head.text */
	unsigned int mp_size; /* size of MP data */
	unsigned char sha256[32]; /* hash */
} __attribute__((__packed__)) dts_payload_t;

// Instantiate the structure
const dts_payload_t dts_payload = { {{mp_va_addr}}, {{mp_offset}}, {{mp_size}}, {0x{{sha256.replace(':', ',0x')}} } };
