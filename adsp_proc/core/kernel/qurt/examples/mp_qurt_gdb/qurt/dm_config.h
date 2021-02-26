/*=============================================================================

                                    dm_config.h

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
#ifndef __DM_CONFIG_H__
#define __DM_CONFIG_H__

#include <qurt.h>

struct dm_sm_config
{
    int                 magic;
    addr_t              pd_table_base_addr;
    size_t              pd_table_base_size;
    uint32_t            debug_processor_max_desc_count;
    qurt_anysignal_t    *wait_for_irq_poll_event_sig;
    unsigned int        wait_for_irq_poll_event_sig_mask;
    qurt_anysignal_t    *notify_send_sig;
    unsigned int        notify_send_sig_mask;
    unsigned int        notify_unregister_sig_mask;
};
typedef struct dm_sm_config dm_config;

#endif //__DM_CONFIG_H__
