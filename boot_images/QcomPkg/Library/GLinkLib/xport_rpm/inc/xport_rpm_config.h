#ifndef XPORT_RPM_CONFIG_H
#define XPORT_RPM_CONFIG_H

/**
 * @file xport_rpm_config.h
 *
 * Internal definitions for RPM Glink transport configurations
 */

/*==============================================================================
     Copyright (c) 2014 - 2015 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/xport_rpm/inc/xport_rpm_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/15   bc      Add secure processor support
10/20/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_os_utils.h"
#include "smem_type.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
#define XPORT_RPM_TOC_MAGIC 0x67727430

#define XPORT_RPM_AP2R_ID   0x61703272
#define XPORT_RPM_R2AP_ID   0x72326170
#define XPORT_RPM_MP2R_ID   0x6D703272
#define XPORT_RPM_R2MP_ID   0x72326D70
#define XPORT_RPM_AD2R_ID   0x61643272
#define XPORT_RPM_R2AD_ID   0x72326164
#define XPORT_RPM_SC2R_ID   0x73633272
#define XPORT_RPM_R2SC_ID   0x72327363
#define XPORT_RPM_WC2R_ID   0x77633272
#define XPORT_RPM_R2WC_ID   0x72327763
#define XPORT_RPM_TZ2R_ID   0x747A3272
#define XPORT_RPM_R2TZ_ID   0x7232747A
#define XPORT_RPM_SP2R_ID   0x53503252
#define XPORT_RPM_R2SP_ID   0x52325350

#define XPORT_RPM_TOC_MAGIC_IDX    0
#define XPORT_RPM_TOC_LENGTH_IDX   1
#define XPORT_RPM_TOC_ENTRIES_IDX  2

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
typedef struct _xport_rpm_toc_entry_type
{
  uint32 fifo_id;                   /* FIFO ID                */
  uint32 fifo_offset;               /* FIFO offset in MSG RAM */
  uint32 fifo_size;                 /* FIFO size              */
} xport_rpm_toc_entry_type;

typedef struct _xport_rpm_toc_config_type
{
  uint32 fifo_id;                    /* FIFO ID                        */
  smem_host_type host;               /* Host ID for MSG RAM allocation */
  uint32 fifo_size;                  /* FIFO size                      */
} xport_rpm_toc_config_type;


typedef struct _xport_rpm_config_type
{
  const char* remote_ss;            /* Remote host name        */
  uint32 tx_fifo_id;                /* Tx FIFO ID in RPM ToC   */
  uint32 rx_fifo_id;                /* Rx FIFO ID in RPM ToC   */
  os_ipc_intr_type irq_out;         /* Outgoing interrupt      */
  uint32 irq_in;                    /* Incoming interrupt      */
} xport_rpm_config_type;       

#endif //XPORT_RPM_CONFIG_H

