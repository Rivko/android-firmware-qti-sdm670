/*
@file secdspdemo.h
@brief This file contains all definitions used in secdspdemo
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/apps.tz/2.0.2/securemsm/trustzone/qsapps/secdspdemo/secdspdemo.h#1 $
  $DateTime: 2018/08/09 06:10:01 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __SECDSPDEMO_H__
#define __SECDSPDEMO_H__

#include "stdint.h"

#define MAX_VMIDS     3
#define NUM_ELEMS(x)    (sizeof(x) / sizeof((x)[0]))

typedef enum dsc_features {
    DSP_FACE_3D = 1,
} dsc_features_t;

enum commands {
    DECRYPT_IP = 0,
    REGISTER_BUFFER,
    DEREGISTER_BUFFER,
    LAST_CMD = DEREGISTER_BUFFER,
};

typedef enum buffer_type {
    MODEL = 1,
    AUTH = 2,
    CAMERA = 3,
} buf_type_t;

typedef enum state_type {
    RESET = 0,
    INIT,
    LOADED,
    IDLE,
    EXECUTING,
    TEMPLATE_PROCESS,
    PASS,
    ERROR,
    CLEANUP,
} state_type_t;

typedef struct send_cmd_struct {
    uint32_t cmd_id;
    uint64_t addr;
    uint32_t size;
    uint32_t buf_type;
    uint64_t out_addr;
    uint32_t out_size;
} __attribute__ ((packed)) send_cmd_t;

typedef struct send_cmd_rsp_struct {
    uint32_t status;
} __attribute__ ((packed)) send_cmd_rsp_t;

typedef struct buffer_vm_list {
    buf_type_t buf_type;
    ACVirtualMachineId vm_list[MAX_VMIDS];
    uint32_t nr_vmids;
} buffer_vm_list_t;


#endif // __SECDSPDEMO_H__
