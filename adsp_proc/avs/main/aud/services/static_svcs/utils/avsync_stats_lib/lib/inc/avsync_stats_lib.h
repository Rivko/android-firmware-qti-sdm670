/*========================================================================*/
/**
@file avsync_stats_lib.h

@brief AV Sync Statistics Library public header.

*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "avsync_stats" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/avsync_stats_lib/lib/inc/avsync_stats_lib.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
8/8/12     RP      Created file.
========================================================================== */

#ifndef _AVSYNC_STATS_H
#define _AVSYNC_STATS_H

enum e_param_t
{
  STAT_WINDOW_START,
  STAT_WINDOW_END
};

/*get the size of AV Sync instance*/
uint32_t avsync_stats_getsize();

/*Initialize all instance parameters*/
ADSPResult avsync_stats_init(void *inst_ptr);

/*Initialize only the instantaneous parameters*/
ADSPResult avsync_stats_reinit(void *inst_ptr);

/*used to update the stats window start and end*/
ADSPResult avsync_stats_set_param(void *inst_ptr, e_param_t param_type, void *param_val);

/*Update the statistics for every frame*/
ADSPResult avsync_stats_update(void *inst_ptr, int64_t difference, uint64_t duration);

/*For every query, update the memory region pointed to by param_val with the statistics*/
ADSPResult avsync_stats_query(void *inst_ptr, uint32_t param_id, void *param_val);

/*commit the statistics with abs time once buffer is sent down to device. */
ADSPResult avsync_stats_commit(void *inst_ptr, uint64_t abs_time);

#endif /*_AVSYNC_STATS_H*/
