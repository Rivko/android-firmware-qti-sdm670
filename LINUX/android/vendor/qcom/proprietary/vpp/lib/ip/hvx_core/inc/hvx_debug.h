/*!
 * @file vpp_ip_hvx_debug.h
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.

 * @services
 */

#ifndef HVX_DEBUG_H
#define HVX_DEBUG_H

#include "vpp_svc.h"

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

//turn on/off some printf
#define DISABLE_PRINTF 0
#if DISABLE_PRINTF
    #define my_printf(fmt, ...) (void)0
#else
    #define my_printf printf
#endif

int print_vpp_svc_nr_params(vpp_svc_nr_params_t *vpp_svc_nr_params) ;         //for debug
int print_vpp_svc_ie_params(vpp_svc_aie_params_t *vpp_svc_ie_params);         //for debug
int print_vpp_svc_mvp_params(vpp_svc_mvp_params_t *vpp_svc_mvp_params);         //for debug
int print_vpp_svc_params(vpp_svc_params_t *vpp_svc_params);                     //for debug
int print_vpp_svc_buffer_attribute_t(vpp_svc_frame_group_descriptor_t *frame_group_descp) ;//for debug
void print_data( char *str, char*buff, int len);
int print_vpp_svc_frc_params(vpp_svc_frc_params_t *frc_params_p);
#endif
