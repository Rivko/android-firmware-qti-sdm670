#ifndef __GPI_I_H__
#define __GPI_I_H__

/**
 * @file  gpi_i.h
 * @brief Provides defines and types that are internal to GPI. 
 */
/*
===============================================================================

Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
04/21/17   ah      Moved NUM_GPII definitions to target specific files.
02/19/16   ah      File created.

===============================================================================
              Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "gpi_ee.h"

/* *********************************************************************************************** */
/*                                         DEFINITIONS                                             */
/* *********************************************************************************************** */


#define QUPV3_HW_VERSION             0x10000000
#define NUM_RINGS_PER_GPII           2
#define NUM_EVT_RINGS_PER_GPII       1
#define NUM_GPI_EVT_RING_ELEM        MAX_NUM_GPI_RING_ELEM
#define GPI_EVT_Q_SIZE               NUM_GPII*NUM_RINGS_PER_GPII*MAX_NUM_GPI_RING_ELEM
#define GPI_4K_ALIGN_OFFSET          0xFFF
#define MAX_QUP_IRQ_VAL              99999
#define MAX_QUP_IRQ_NAME_LEN         15

#undef FALSE
#undef TRUE
#define FALSE    0
#define TRUE     1

/* GPI DATA FIELDS */
#define GPI_CHAN_STATE__MASK                          0xFF
#define GPI_CHAN_STATE__SHIFT                         0x0

#define GPI_INT_MOD_TIMER__MASK                       0xFFFF0000
#define GPI_INT_MOD_TIMER__SHIFT                      0x10

#define GPI_STATE__MASK                               0xFF000000
#define GPI_STATE__SHIFT                              0x18

#define GPI_EVT_CHID__MASK                            0xFF000000
#define GPI_EVT_CHID__SHIFT                           0x18

#define GPI_EVT_CODE__MASK                            0xFF000000
#define GPI_EVT_CODE__SHIFT                           0x18

#define GPI_EVT_XFER_LEN__MASK                        0xFFFFFF
#define GPI_EVT_XFER_LEN__SHIFT                       0x0

#define GPI_EVT_IMM_DATA_LEN__MASK                    0xF
#define GPI_EVT_IMM_DATA_LEN__SHIFT                   0x0

#define GPI_EVT_IMM_DATA_TRE_IDX__MASK                0xFFFF00
#define GPI_EVT_IMM_DATA_TRE_IDX__SHIFT               0x8

#define GPI_EVT_QUP_COUNT__MASK                       0xFFFFFF
#define GPI_EVT_QUP_COUNT__SHIFT                      0x0

#define GPI_EVT_STATUS__MASK                          0xFFFF
#define GPI_EVT_STATUS__SHIFT                         0x0

#define GPI_EVT_TYPE__MASK                            0xFF0000
#define GPI_EVT_TYPE__SHIFT                           0x10

#define GPI_CMD_TYPE__MASK                            0xFF0000
#define GPI_CMD_TYPE__SHIFT                           0x10

#define GPI_ELEMENT_TYPE__MASK                        0xFF0000
#define GPI_ELEMENT_TYPE__SHIFT                       0x10

/* Local definitions */
#define GPI_LOCAL_EVT_GPII_ID__MASK                   0xF
#define GPI_LOCAL_EVT_GPII_ID__SHIFT                  0x0

#define GPI_LOCAL_EVT_CHID__MASK                     0xF0
#define GPI_LOCAL_EVT_CHID__SHIFT                    0x4

#define GPI_LOCAL_EVT_CHAN_STATE__MASK               0xF00
#define GPI_LOCAL_EVT_CHAN_STATE__SHIFT              0x8

#define GPI_LOCAL_EVT_ERROR__MASK                    0xF000
#define GPI_LOCAL_EVT_ERROR__SHIFT                   0xC

#define GPI_LOCAL_EVT_INT_TYPE__MASK                 0xF0000
#define GPI_LOCAL_EVT_INT_TYPE__SHIFT                0x10

#define GPI_LOCAL_EVT_GSI_INST__MASK                 0xF00000
#define GPI_LOCAL_EVT_GSI_INST__SHIFT                0x14

typedef struct gpi_ctxt gpi_ctxt;

/* *************************************************************** */
/*                          DATA STRUCTURES                        */
/* *************************************************************** */

/* Event Ring */
typedef enum
{
   GPI_EVT_RING_NOT_ALLOC   = 0x0,
   GPI_EVT_RING_ALLOC       = 0x1
}GPI_EVT_RING_STATE;

/* Ring Information */
typedef struct _ring_info_type
{
   U_LONG        base_pa;
   U_LONG        base_va;
   U_LONG        local_rp;
   uint8            num_elem;
   GPI_CHAN_STATE   state;
}ring_info_type;

/* Interrupt Type */
typedef enum
{
   GPI_INT_MSI              = 0x0,
   GPI_INT_IRQ              = 0x1
}GPI_INT_TYPE;

/* IRQ Type */
typedef enum
{
   GPI_IRQ_CH_CTRL          = 1,
   GPI_IRQ_EV_CTRL          = 2,
   GPI_IRQ_GLOB_EE          = 4,
   GPI_IRQ_IEOB             = 8,
   GPI_IRQ_INTER_EE_CH_CTRL = 16,
   GPI_IRQ_INTER_EE_EV_CTRL = 32,
   GPI_IRQ_GENERAL          = 64,

   GPI_IRQ_OTHER            = RESERVED_VALUE
}GPI_IRQ_TYPE;

/* Ring Pointer Type */
typedef enum
{
   RING_RP              = 0,
   RING_WP              = 1,
   EVT_RING_RP          = 2,
   EVT_RING_UPDATED_RP  = 3,
   EVT_RING_WP          = 4,
   ARBITRARY_PTR        = 5
} GPI_RING_PTR_TYPE;

/* Work Queue */
typedef struct _gpi_work_q
{
   gpi_spinlock_type lock;
   gpi_ring_elem *base;
   gpi_ring_elem *volatile wp;
   gpi_ring_elem *volatile rp;
   gpi_ring_elem elem[GPI_EVT_Q_SIZE];
}gpi_work_q;

/* GPI Thread State */
typedef enum
{
   GPI_THREAD_INITIAL = 0,
   GPI_THREAD_WAITING = 1,
   GPI_THREAD_RUNNING = 2,
   GPI_THREAD_STOPPED = 3,
   GPI_THREAD_TERMINATING = 0xDEAD
}GPI_THREAD_STATE;

/* Thread Type */
typedef struct _gpi_thread_type
{
   uint32                     priority;
   GPI_THREAD_STATE volatile  state;
   gpi_work_q                 q;
#ifdef GPI_MT
   generic_thread_type        thread;
#endif
}gpi_thread_type;

/* QUP Instance */
typedef struct _qup_instance
{
   QUP_BLOCK_TYPE  type;
   boolean         active;
   U_LONG       gsi_pa;
   uint32          qup_irq;
}qup_instance;

/* GPII Info */
typedef struct _gpii_info
{
   uint8            id;
   QUP_BLOCK_TYPE   qup_type;
   boolean          registered;
   boolean          initialized;
   gpi_sync_type    gpii_mutex;
   uint8            ee;
   uint8            se;
   uint32           irq;
   uint8            options;
   GPI_PROTOCOL     protocol;
   gpi_ctxt       * gpi;
   uint32           int_modt_val;
   uint32           int_modc_val;
   boolean          irq_mode;
   client_cb        cb;
   ring_info_type   chan_info[NUM_RINGS_PER_GPII];
   void *           transf_ctxt[NUM_RINGS_PER_GPII][MAX_NUM_GPI_RING_ELEM];   
   ring_info_type   evt_ring_info;
#ifdef GPI_ISLAND_MODE_ENABLE
   gpi_ring_elem  * evt_ring_elem;
#else
   gpi_ring_elem    evt_ring_elem[NUM_GPI_EVT_RING_ELEM] __attribute__ ((aligned (NUM_GPI_EVT_RING_ELEM * sizeof(gpi_ring_elem))));
#endif
   GPI_CHAN_CMD     pending_cmd;
   void *           cmd_user_data;
   void           * user_data;
}gpii_info;

/* GPI Execution Mode */
typedef enum
{
   GPI_POLLING  = 0,
   GPI_TASK     = 1,
   GPI_THREAD   = 2
}GPI_EXEC_MODE;

/* GPI context */
typedef struct gpi_ctxt
{
   qup_instance    qup[MAX_NUM_QUP_BLOCKS];
   uint8           num_qup;
   gpii_info       gpii[NUM_GPII];
   uint8           num_gpii;
   uint16          gpii_hw_ready_bmsk;
   uint16          gsi_evt_ring_bmsk;
   boolean         initialized;
   boolean         irq_muxing;
   gpi_sync_type   gpi_mutex;

   /* GPI Execution Mode */
   GPI_EXEC_MODE   exec_mode;
   boolean         env_64bit;

#ifdef GPI_ST
   /* Single threaded execution mode GPI task */
   rex_tcb_type    gpi_task_tcb;
#endif

   /* Thread(s) */
   gpi_thread_type  gpi_thread;

   GPI_DEV_HANDLE  *hGpiDALInterrupt;
   GPI_DEV_HANDLE  *hGpiHWIO;
}gpi_ctxt;

/* *************************************************************** */
/*                           Macros                                */
/* *************************************************************** */

#define GPI_READ_FIELD(_VAR,_FIELD)                                                       \
   ((_VAR & GPI_##_FIELD##__MASK) >> GPI_##_FIELD##__SHIFT)

#define GPI_WRITE_FIELD(_VAR,_FIELD, _VAL)                                                \
   {                                                                                      \
      _VAR &= ~GPI_##_FIELD##__MASK;                                                      \
      _VAR |= (_VAL << GPI_##_FIELD##__SHIFT);                                            \
   }

#define INC_RING_IDX(idx, num_elem)                                                       \
                   (((idx) + 1) % (num_elem))

#define DEC_RING_IDX(idx, num_elem)                                                       \
                   ((((idx) - 1) < 0)? ((num_elem) - 1): ((idx) - 1))

#define ADVANCE_RING_PTR(base, ptr, size)                                                 \
                *ptr = ((*ptr - base)/sizeof(gpi_ring_elem) == (size - 1))?               \
                       base: (*ptr + sizeof(gpi_ring_elem))

#define GET_RING_VIRT_ADDR(ring_info, pa)                                                 \
                   ((ring_info)->base_va + ((pa) - (ring_info)->base_pa))

#define GET_RING_PHYS_ADDR(ring_info, va)                                                 \
                   ((ring_info)->base_pa + ((va) - (ring_info)->base_va))

/* Ring WP or RP index */
#define GET_RING_PTR_INDEX(gpii, chan, ptr_type)                                          \
                   ((get_ring_ptr(gpii, chan, ptr_type) - gpii->chan_info[chan].base_pa) / sizeof(gpi_ring_elem))

/* Arbitrary ring element index */
#define GET_RING_ELEMENT_INDEX(ring_base, elem)                                           \
                   (((elem) - (ring_base))/sizeof(gpi_ring_elem))

#define VALID_RING_PTR(ring, ptr)                                                         \
                   (((ptr) >= (ring)->base) &&                                            \
                    ((ptr) <= ((ring)->base + (ring)->length - sizeof(gpi_ring_elem))))

#define VALID_CHAN_STATE(state)                                                         \
                   (((state) >= 0 && (state) < 5) || ((state) == 15))

#ifndef MIN
#define MIN(_x,_y)		   ((_x) < (_y) ? (_x): (_y))
#endif


/**
 * Main function to initialize GPI EE, target, and gloabl data structure.
 * To be called from the main task before any GPI request is made.
 * 
 * @param[in]    gpi    GPI context.
 *
 * @return   None.
 */
void gpi_init_all(gpi_ctxt *gpi);

/**
 * Initializes GPII HW registers, and IRQ.
 *
 * @param[in]    gpii                  Pointer to GPII context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpii_init(gpii_info *gpii);

/**
 * Main function to initialize GPI common data structure, interrupts,
 * threads and synchronization objects.
 *
 * @param[in]    gpi                   Pointer to GPI context.
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS gpi_common_init(gpi_ctxt *gpi);

/**
 * @brief Set GPII specific parameters all GPI rings
 *
 * @param[in,out ]        gpi_ctxt           GPI context
 */
GPI_RETURN_STATUS set_gpii_params(gpi_ctxt *gpi,
                                  gpii_info *gpii,
                                  gpi_iface_params *params);

/**
 * Issues a GPI command.
 *
 * @param[in]    gpii_id         GPI Interface ID
 * @param[in]    chan            Virtual Channel ID
 * @param[in]    cmd_type        Command type
 *
 * @return       GPI_RETURN_STATUS     Return status.
 */
GPI_RETURN_STATUS issue_cmd(gpii_info *gpii, GPI_CHAN_TYPE chan, GPI_CHAN_CMD cmd_type);

#endif /* __GPI_I_H__ */
