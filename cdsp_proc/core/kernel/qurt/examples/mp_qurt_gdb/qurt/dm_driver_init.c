/*=============================================================================

                                    dm_driver_init.c

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
#include <assert.h>
#include "debugger_types.h"
#include "dm_config.h"
#include <qurt_ptrace.h>

#define REMOTE_DATA_READY_MASK  0x1
#define REMOTE_UNREGISTER_MASK  0x2
#define REMOTE_INCOMING_EVENT   0x4
#define DBGR_TX_THREAD_PRIO     36
#define DBGR_TX_STACK_SIZE      512

#ifdef SIM_TEST
    #define DEBUG_MONITOR_PROCESSOR_IMAGE_ID            0xF0000101
    #define DEBUG_MONITOR_RX_INTERRUPT_NUMBER           40
    #define DEBUG_MONITOR_TX_INTERRUPT_REGISTER_ADDR    0x28885014
    #define DEBUG_MONITOR_TX_INTERRUPT_MASK             0x1
    #define DEBUG_MONITOR_SH_MEM_BASE_ADDR              0x60000000
    #define DEBUG_MONITOR_SH_MEM_SIZE                   0x00004000
#endif

/** @brief A unique subsystem identifier required for debug services */
#ifndef DEBUG_MONITOR_PROCESSOR_IMAGE_ID
    #define DEBUG_MONITOR_PROCESSOR_IMAGE_ID            0
#endif
unsigned int debug_monitor_processor_image_id = DEBUG_MONITOR_PROCESSOR_IMAGE_ID;

/** @brief Interrupt Number through which the debug monitor is notified of debugger payload arrival */
#ifndef DEBUG_MONITOR_RX_INTERRUPT_NUMBER
    #define DEBUG_MONITOR_RX_INTERRUPT_NUMBER           -1
#endif
unsigned int debug_monitor_rx_interrupt_num = DEBUG_MONITOR_RX_INTERRUPT_NUMBER;

/** @brief Address of HW register to generate an interrupt to notify the remote debugger of a DM response */
#ifndef DEBUG_MONITOR_TX_INTERRUPT_REGISTER_ADDR
    #define DEBUG_MONITOR_TX_INTERRUPT_REGISTER_ADDR    0x0
#endif
unsigned int debug_monitor_tx_interrupt_reg_addr = DEBUG_MONITOR_TX_INTERRUPT_REGISTER_ADDR;

/** @brief Mask of a HW register to generate an interrupt to notify the remote debugger of a DM response */
#ifndef DEBUG_MONITOR_TX_INTERRUPT_MASK
    #define DEBUG_MONITOR_TX_INTERRUPT_MASK             0x0
#endif
unsigned int debug_monitor_tx_interrupt_mask = DEBUG_MONITOR_TX_INTERRUPT_MASK;

/** @brief Base Address of shared memory region reserved for remote debugger to debug monitor communication */
#ifndef DEBUG_MONITOR_SH_MEM_BASE_ADDR
    #define DEBUG_MONITOR_SH_MEM_BASE_ADDR              0x0
#endif
unsigned int debug_monitor_smem_base_addr = DEBUG_MONITOR_SH_MEM_BASE_ADDR;

/** @brief Size of shared memory region reserved for remote debugger to debug monitor communication */
#ifndef DEBUG_MONITOR_SH_MEM_SIZE
    #define DEBUG_MONITOR_SH_MEM_SIZE                   0
#endif
unsigned int debug_monitor_smem_buf_size = DEBUG_MONITOR_SH_MEM_SIZE;

dm_config dm_cfg;
qurt_anysignal_t g_notify_send_sig;
qurt_anysignal_t g_wait_for_irq_poll_event_sig;
unsigned int g_notify_send_sig_mask             = REMOTE_DATA_READY_MASK;
unsigned int g_notify_unregister_sig_mask       = REMOTE_UNREGISTER_MASK;
unsigned int g_wait_for_irq_poll_event_sig_mask = REMOTE_INCOMING_EVENT;

static qurt_thread_t tx_thread_handle;
static char tx_stack[DBGR_TX_STACK_SIZE];

// static qurt_thread_t dbgr_default_thread_handle;
// static char dbgr_default_stack[DBGR_TX_STACK_SIZE];

void debugger_irq_handler(int num);
void dbgr_platform_notify_agent_thr(void *arg);

//static qurt_timer_t g_timer;
//static int use_interrupt = 0;

// qurt_timer_attr_t attr;
// qurt_timer_attr_init(&attr);
// //qurt_timer_attr_set_duration(&attr, 100*1000); //250 ms
// qurt_timer_attr_set_type(&attr, QURT_TIMER_PERIODIC);
// err_code = qurt_timer_create(&g_timer, &attr, p_signal, mask);
//qurt_timer_delete(g_timer);

extern int qurt_debug_monitor_ctrl(int on_off, void *p_cfg_data, size_t cfg_size);

int dbgr_thread_create_helper(qurt_thread_t *p_handle, void *stack, unsigned int stack_size, int prio, char *name, void *pc, void *arg)
{
    qurt_thread_t tid;
    qurt_thread_attr_t attr;
    int ret_val = -1;
    do
    {
        if (NULL == p_handle)
        {
            break;
        }
        qurt_thread_attr_init (&attr);
        qurt_thread_attr_set_name(&attr, (char*)name);
        qurt_thread_attr_set_stack_size (&attr, stack_size);
        qurt_thread_attr_set_stack_addr (&attr, stack);
        qurt_thread_attr_set_priority (&attr, (unsigned short)prio);
        ret_val = qurt_thread_create(p_handle, &attr, pc, (void *)arg);
    } while(0);

    if (-1 == ret_val)
    {
        printf(" failed to create thread \n");
        assert(0);
    }

    return 0;
}

void dbgr_platform_get_smem_info(addr_t *base_addr, size_t *p_size)
{
    *base_addr = debug_monitor_smem_base_addr;
    *p_size = debug_monitor_smem_buf_size;
}

int dm_driver_init(void)
{
    int found = 0, pr_idx, ret_val = -1, err_code;

    do
    {
        dm_cfg.magic = 0xFF00FF00;
        dbgr_platform_get_smem_info(&(dm_cfg.pd_table_base_addr), &(dm_cfg.pd_table_base_size));
        if ((0 == dm_cfg.pd_table_base_addr) || (0 == dm_cfg.pd_table_base_size))
        {
            break;
        }
        /* obtain apps --> hex interrupt num and the config info hexagon --> apps interrupt */
        if ((-1 == debug_monitor_rx_interrupt_num)      || 
            (0 == debug_monitor_tx_interrupt_mask)      || 
            (0 == debug_monitor_tx_interrupt_reg_addr))
        {
            break;
        }

        dm_cfg.wait_for_irq_poll_event_sig = &g_wait_for_irq_poll_event_sig;
        qurt_anysignal_init(dm_cfg.wait_for_irq_poll_event_sig);
        dm_cfg.wait_for_irq_poll_event_sig_mask = g_wait_for_irq_poll_event_sig_mask;
        dm_cfg.notify_send_sig = &g_notify_send_sig;
        qurt_anysignal_init(dm_cfg.notify_send_sig);
        dm_cfg.notify_send_sig_mask = g_notify_send_sig_mask;
        dm_cfg.notify_unregister_sig_mask = g_notify_unregister_sig_mask;

        /* create the thread to accept notifications from the debugger in the event remote data is ready to be sent */
        err_code = dbgr_thread_create_helper(&(tx_thread_handle),
                                                tx_stack,
                                                DBGR_TX_STACK_SIZE,
                                                DBGR_TX_THREAD_PRIO,
                                                "dbgr_tx_loop",
                                                dbgr_platform_notify_agent_thr, NULL);
        if (0 != err_code)
        {
            break;
        }
        err_code = qurt_fastint_register(debug_monitor_rx_interrupt_num, debugger_irq_handler);
        if (0 != err_code)
        {
            break;
        }

        /* start the debugger with the config information */
        ret_val = qurt_debug_monitor_ctrl(1, (void*)&dm_cfg, sizeof(dm_config));
		
		/* Create the dbgr_default thread */
		// err_code = dbgr_thread_create_helper(&(dbgr_default_thread_handle),
                                                // dbgr_default_stack,
                                                // DBGR_TX_STACK_SIZE,
                                                // DBGR_TX_THREAD_PRIO,
                                                // "dbgr_default_thread",
                                                // dbgr_default_thread_main, NULL);
    } while(0);

    return ret_val;
}

int dm_driver_deinit(void)
{
    return qurt_debug_monitor_ctrl(0, NULL, 0);
}

void debugger_irq_handler(int num)
{
    qurt_anysignal_set(dm_cfg.wait_for_irq_poll_event_sig, dm_cfg.wait_for_irq_poll_event_sig_mask);
}

// void dbgr_default_thread_main(void *arg)
// {
	// while(0)
	// {
		// ptrace(PTRACE_SILENT, 0, NULL, NULL);
		
		// dbgr_default_attach();	
	// }
// }

void dbgr_platform_notify_agent_thr(void *arg)
{
    unsigned int curr_sigs;
    unsigned int sigs = (REMOTE_DATA_READY_MASK|REMOTE_UNREGISTER_MASK);
    if (dm_cfg.wait_for_irq_poll_event_sig)
    {
        do
        {
            qurt_anysignal_wait(dm_cfg.notify_send_sig, sigs);
            curr_sigs = qurt_anysignal_get(dm_cfg.notify_send_sig);
            if (REMOTE_DATA_READY_MASK & curr_sigs)
            {
                *((volatile unsigned int *)(debug_monitor_tx_interrupt_reg_addr)) = (debug_monitor_tx_interrupt_mask & ~0x0);
                (void)*((volatile unsigned int *)(debug_monitor_tx_interrupt_reg_addr));            
            }
            if (REMOTE_UNREGISTER_MASK & curr_sigs)
            {
                qurt_fastint_deregister(debug_monitor_rx_interrupt_num);
                /* once deregistered, will no longer wait */
                break;
            }
            qurt_anysignal_clear(dm_cfg.notify_send_sig, sigs);
        } while(1);
    }
}
