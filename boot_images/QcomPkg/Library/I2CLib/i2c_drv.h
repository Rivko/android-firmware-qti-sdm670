/** 
    @file  i2c_drv.h
    @brief I2C driver interface
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __I2C_DRV_H__
#define __I2C_DRV_H__

#include "i2c_api.h"

#define I2C_NOISE_REJECT_LEVEL_DEFAULT      50
#define I2C_PACKING_DEFAULT                 3

#define I2C_STANDARD_MODE_SPEC_T_LOW_NS     4700    /* 4700 ns */
#define I2C_FAST_MODE_SPEC_T_LOW_NS         1300    /* 1300 ns */
#define I2C_FAST_MODE_PLUS_SPEC_T_LOW_NS    500     /* 500  ns */

#define I2C_STANDARD_MODE_SPEC_T_HIGH_NS    4000    /* 4000 ns */
#define I2C_FAST_MODE_SPEC_T_HIGH_NS        600     /* 600  ns */
#define I2C_FAST_MODE_PLUS_SPEC_T_HIGH_NS   260     /* 260  ns */

#define I2C_SLAVE_CONFIG_1_T_LOW_MASK               0x000000ff
#define I2C_SLAVE_CONFIG_1_T_LOW_SHFT               0

#define I2C_SLAVE_CONFIG_1_T_HIGH_MASK              0x0000ff00
#define I2C_SLAVE_CONFIG_1_T_HIGH_SHFT              8

#define I2C_SLAVE_CONFIG_1_NOISE_REJECT_LEVEL_MASK  0x00ff0000
#define I2C_SLAVE_CONFIG_1_NOISE_REJECT_LEVEL_SHFT  16

#define I2C_SLAVE_CONFIG_1_GET(x, CONFIG) ((x & I2C_SLAVE_CONFIG_1_##CONFIG##_MASK) >> (I2C_SLAVE_CONFIG_1_##CONFIG##_SHFT))
#define I2C_SLAVE_CONFIG_2_GET(x, CONFIG) ((x & I2C_SLAVE_CONFIG_2_##CONFIG##_MASK) >> (I2C_SLAVE_CONFIG_2_##CONFIG##_SHFT))

#define I2C_DRV_HW_INTERRUPT_REGISTERED     0x00000001
#define I2C_DRV_BUS_INTERFACE_INITIALIZED   0x00000002

// transfer state
typedef enum transfer_state     transfer_state;

// all about controller
typedef struct hw_ctxt          hw_ctxt;

// all about transfer
typedef struct transfer_ctxt    transfer_ctxt;

// all about client
typedef struct client_ctxt      client_ctxt;

enum transfer_state
{
    TRANSFER_INVALID = 0,
    TRANSFER_IN_PROGRESS,
    TRANSFER_QUEUED,
    TRANSFER_OUTPUT_DONE,
    TRANSFER_INPUT_DONE,
    TRANSFER_DONE,
    TRANSFER_CANCELED,
    TRANSFER_TIMED_OUT,
};

struct hw_ctxt
{
    // handle to device config
    void            *core_config;
    // handle to logs
    void            *core_debug;
    // handle to bus interface
    void            *core_iface;
    // for critical sections
    void            *core_lock;
    // for signaling
    void            *core_signal;

    // store head and tail of the client requests in queue for async transfers
    void            *queue_lock;
    client_ctxt     *client_ctxt_head;
    client_ctxt     *client_ctxt_tail;

    // reference counts 
    uint32          core_ref_count;
    uint32          power_ref_count;

    // core states
    uint32          core_state;

    // next controller
    hw_ctxt         *next;
};

struct transfer_ctxt
{
    // client parameters
    i2c_slave_config    *config;
    i2c_descriptor      *desc;
    callback_func       callback;
    void                *ctxt;
    uint32              delay;
    uint16              num_descs;

    // track the transfer
    uint32              transferred;
    uint32              transfer_count;
    transfer_state      transfer_state;
    i2c_status          transfer_status;

    // sticky timestamp
    uint32              start_bit_timestamp;
    uint32              stop_bit_timestamp;

    // signal transfer completion
    void                *signal;

    // bus iface io context
    void                *io_ctxt;
};

// the handle that is returned to the client in open
struct client_ctxt
{
    hw_ctxt             *h_ctxt;
    transfer_ctxt       t_ctxt;
    client_ctxt         *next;
};

#endif
