#ifndef QDSS_REPLICATOR_CONFIG_H
#define QDSS_REPLICATOR_CONFIG_H

/*=============================================================================

FILE:         qdss_replicator_config.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2012 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/settings/debugtrace/lpa_replicator/config/adsp/sdm670/qdss_lpa_replicator_config.h#1 $
==============================================================================*/

/*****************************************************************************
                              EDIT HISTORY

 09/07/17   rs      File Created with LPA replicator address for warlock.

*****************************************************************************/
#if defined (ENABLE_LPA_REPLICATOR_QDSS) || defined (ENABLE_LPA_REPLICATOR_SWITCH)
#define QDSS_SWAO_REPLIC_CXATBREPLICATOR_64WP_REG_BASE_PHYS      (0x06B0A000) 
#endif

#endif //QDSS_REPLICATOR_CONFIG_H
