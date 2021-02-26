
/*========================================================================

*//** @file voice_mcs_hist.cpp

Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for calling MCS api to update histogram.
VDS margins can be plotted as histogram every 100ms using these api's.
Post processing of the logged histogram data can be performed offline.

Histogram structure's general definition is as follows,

PROF_HIST_PACKET_DEFINE(hist_log_packet, , log_id,

                        (hist, id, nbins, min_value, span, bin_type),

                        ...repeat above tuple for more histograms...)

Here, span = max_value -min_value
and bin_type is data type of the bin counters, ranging from uint8 to uint32

Following is the description for various mcs api's

1. prof_hist_update(prof_hist_param_t * hist, data)
   - This api is used to update histogram with new data

2. prof_hist_clear(prof_hist_param_t * hist)
   - This api is used to clear structure when data is logged

3. boolean prof_hist_log_now( prof_hist_log_packet_handle_t * handle)
  - This api is used to immediately log data by diag, returns TRUE for success

4. boolean prof_hist_log_periodic(prof_hist_log_packet_handle_t * handle)
  - This api is used to log data by diag periodically, default is 100ms. It
   returns TRUE only when logging is successful based on the period.

5. void prof_hist_set_min_log_interval(uint16 interval)
  - This api can be used to update minimum logging period in ms, default is 100ms

*//*====================================================================== */

/*========================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  06/23/2017 ro      Created file
  ========================================================================== */


/* =======================================================================
   INCLUDE FILES FOR MODULE
   ========================================================================== */
#include "voice_delivery.h"
#include "prof_hist.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 ** Data structures
 ** ----------------------------------------------------------------------- */
/*
Following macro defines structure for profiling histograms for VPRX and VENC
margins.

  handle: vds_hist_packet_handle

  log_id: 0x18EA

  vds_vprx_hist              0 to 5000 usec, over 50 bins.

  vds_venc_hist              0 to 5000 usec, over 50 bins.

Note-> nbins/span values are rounded off to multiple of 4/16 below.

*/

PROF_HIST_PACKET_DEFINE(vds_hist_packet, , 0x18EA,
  (vds_vprx_hist,     0x0001, 48, 0,  4992,      1BYTE_SAT),
  (vds_venc_hist,     0x0002, 48, 0,  4992,      1BYTE_SAT))

/* Following function is used to update as well as log the histogram
data for VDS margins.
*/

void vds_update_histogram( uint32_t client_token, uint32_t time)
{
  /* Update histogram. */
  if ( (client_token & 0xffffL) == VDS_CLIENT_VPRX_DELIVERY )
  {
     prof_hist_update(&vds_vprx_hist, time);
  }
  else if ( (client_token & 0xffffL) == VDS_CLIENT_VENC_DELIVERY )
  {
	 prof_hist_update(&vds_venc_hist, time);
     /* Log histogram periodically every venc delivery */
     if ( prof_hist_log_periodic(&vds_hist_packet_handle) )
     {
       /* Clear histograms. */
       prof_hist_clear(&vds_vprx_hist);
       prof_hist_clear(&vds_venc_hist);
     }
  }
  return;
}
