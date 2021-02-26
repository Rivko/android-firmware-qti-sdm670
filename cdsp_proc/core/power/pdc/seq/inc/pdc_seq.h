#ifndef __PDC_SEQ_H__
#define __PDC_SEQ_H__
/*==============================================================================
  FILE:         pdc_seq.h
  
  OVERVIEW:     This file defines public interface for PDC sequencer.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/seq/inc/pdc_seq.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                                  TYPES
 =============================================================================*/
/* Forward declaration of internal configuration structure that can be 
 * version specific. Here, it can only be used as reference or pointer */
struct pdc_seq_hal_bsp_cfg_s;

/* Forward declaration of low power mode structure for PDC sequencer */
struct pdc_seq_hal_bsp_mode_s;

/* Structure containing information about specific PDC sequencer instance.
 * Depending on the subsystem, there can be more than one PDC, its SW
 * may configure */
typedef struct pdc_seq_s
{
  struct pdc_seq_hal_bsp_cfg_s  *cfg;       /* Static config */
  struct pdc_seq_hal_bsp_mode_s *modes;     /* List of LPMs for this PDC */
  uint16                        mode_count; /* Number of modes in above list */
  uint32                        *cmds;      /* Sequence command values */
  uint16                        cmd_length; /* number of sequence commands */
  uint32                        addr;       /* PDC instance's mapped address */
  uint32                        rsc_addr;   /* RSC's mapped address */
} pdc_seq_t;

/* Return value enumberation for different PDC sequencer API */
typedef enum
{
  PDC_SEQ_SUCCESS           = 0,
  PDC_SEQ_INVALID_HANDLE    = -1,
  PDC_SEQ_INVALID_PARAM     = -2,
  PDC_SEQ_UNSUPPORTED_MODE  = -3,
  PDC_SEQ_NO_MEM            = -4,
  PDC_SEQ_ERROR             = -65536
} pdc_seq_result_t;

/* Generic enumeration that can be used for different PDC sequence mode ID's */
typedef enum
{
  PDC_MODE_CX_MIN       = 0,    /* CX minimization */
  PDC_MODE_CX_COL,              /* CX collapse */
  PDC_MODE_CX_MIN_AOSS,         /* CX minimization and AOSS sleep */
  PDC_MODE_CX_COL_AOSS,         /* CX collapse and AOSS sleep */
  PDC_MODE_CX_COL_AOSS_NOTIMER, /* CX collapse and AOSS sleep without wakeup timer */
}pdc_low_power_mode_type;

/*==============================================================================
                                  FUNCTIONS
 =============================================================================*/
/**
 * pdc_seq_count
 * 
 * @brief Returns the number of PDC associated with input entity. This
 *        function is used before querying for PDC sequencer handles.
 *
 * On a given subsystem, driver will contain only PDCs for entities it 
 * handles. It will not be exhaustive for entire system.
 *
 * Current expectation is that each entity will have only one associated
 * PDC but we are keeping interfce a generic, mainly to be compatible with
 * other sequencers we have and have better flexibility going forward.
 *
 * @param entity: Name of the entity for which we are querying PDC count
 *                (e.g. "aop", "mss", etc)
 *
 * @return Number of PDC sequencers associated with input entity.
 */
uint32 pdc_seq_count(const char *entity);

/**
 * pdc_seq_sys_init
 *
 * @brief This function initializes common PDC sequencer driver as well
 *        as all the PDC instances supported on a given subsystem.
 *
 * This function is provided where it is difficult for primary PDC
 * sequencer client to have it intiailized and enabled before RPMh 
 * communication is up. The intended usage is to plug this funciton
 * in subsystem initialization mechanism.
 *
 * @note If a subsystem calls this function, PDC sequencer clients do
 *       not need to intialize their respective PDC instances separately.
 */
void pdc_seq_sys_init(void);

/**
 * pdc_seq_set_lpm
 *
 * @brief Sets the low power mode (lpm) configuration for the PDC sequencer.
 *        This configuration will be effective when the PDC sequencer kicks
 *        in next time.
 *
 * @param mode_id: low power mode id.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_set_lpm(pdc_low_power_mode_type mode_id);

/**
 * pdc_set_wakeup_time
 *
 * @brief Sets up the wake up time for the given PDC.
 *  
 * @param wakeup: Absolute time at which we want PDC to wake up.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_set_wakeup_time(uint64 wakeup);

#endif /* __PDC_SEQ_H__ */

