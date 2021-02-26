#ifndef __AFEAANC_H__
#define __AFEAANC_H__
/*========================================================================
  @file AFEAanc.h

  This file contains the declarations that need to be opened to non-AANC 
  driver code

  Copyright (c) 2011 - 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

    $Header: //components/rel/avs.adsp/2.8.5/afe/algos/aanc/fwk/inc/AFEAanc.h#1 $

    when       who     what, where, why
    --------   ---     -------------------------------------------------------
   30/04/12    pn       initial version


  ========================================================================== */
/*==========================================================================
  Include files
  ========================================================================== */
/**
 * Enum Definitions
 */


/**
 * Structure Definitions
 */

/**
 * Function Prototypes
 */

/**
  Creates the AANC handler thread. This thread handles all the AANC commands
  from driver. It also handles the run-time invocation of AANC algo library

  @param[in]  pAfeSvc       AFE static service object instance

  @result
  None

  @dependencies
  None.
*/
void afe_create_aanc_handler_svc (aud_stat_afe_svc_t *pAfeSvc);

/**
  Destory the AANC handler thread

  @param[in]  pAfeSvc       AFE static service object instance

  @result
  None

  @dependencies
  None.
*/
void afe_destroy_aanc_handler_svc (aud_stat_afe_svc_t *pAfeSvc);

/**
  Used for sending APR message to the AANC Cmd Queue
*/
ADSPResult afe_svc_aanc_send_msg(qurt_elite_queue_t *aanc_cmdq_ptr, uint32_t op_code,
                                 afe_dev_port_t *pDevPort, elite_apr_packet_t *pkt_ptr,
                                 uint32_t payload_size, uint32_t payload_start_addr);
#endif

