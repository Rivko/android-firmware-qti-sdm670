#ifndef ELITESVC_H
#define ELITESVC_H

/**
@file EliteSvc.h

@brief This file defines service-related structures, IDs.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013-2015 Qualcomm Technologies, Inc.  All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) Added Doxygen condition commands.
09/18/14   sw      (Tech Pubs) Updated Doxygen comment for 2.6.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
02/04/10   DC      Created file.
==========================================================================*/

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup elite_svc_ids
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/* Supported Service IDs
  Only the most significant 16 bits are used, and they can grow incrementally.
  The least significant 16 bits stay as zeros, which is an easy way to
  define service-specific custom messages or parameter IDs.
*/
#define ELITE_CORE_SVCID          0x00010000L
    /**< Core service. */
#define ELITE_EXAMPLE_SVCID       0x00020000L
    /**< Sample service. */
#define ELITE_EXAMPLETST_SVCID    0x00030000L
    /**< Sample test service. */
#define ELITE_VOICE_STREAM_SVCID            0x00040000L
    /**< Voice stream service. */
#define ELITE_VOICE_DEVICE_SVCID            0x00050000L
    /**< Voice device service. */
#define ELITE_VOICE_ENC_SVCID               0x00060000L
    /**< Voice encoder service. */
#define ELITE_VOICE_DEC_SVCID               0x00070000L
    /**< Voice decoder service. */
#define ELITE_VOICEPROCTX_SVCID             0x00080000L
    /**< Voice processor service on the Tx path. */
#define ELITE_VOICEPROCRX_SVCID             0x00090000L
    /**< Voice processor service on the Rx path. */
#define ELITE_VOICEPROCTX_TEST_SVCID        0x000A0000L
    /**< Voice processor test service on the Tx path. */
#define ELITE_VOICEPROCRX_TEST_SVCID        0x000B0000L
    /**< Voice processor test service on the Rx path. */
#define ELITE_STATASM_SVCID       0x000C0000L
    /**< Static audio stream manager service. */
#define ELITE_SINK_SVCID          0x000D0000L
    /**< Sink service. */
#define ELITE_DYNAPP_SVCID        0x000E0000L 
    /**< Dynamic application service, which includes the audio postprocessor. */
#define ELITE_MIXER_SVCID         0x000F0000L
    /**< Audio Matrix Mixer service. */
#define ELITE_POPP_SVCID          0x00100000L
    /**< Not used. */
#define ELITE_COPP_SVCID          0x00110000L
    /**< Not used. */
#define ELITE_PDOPP_SVCID         0x00120000L
    /**< Not used. */
#define ELITE_DYNAPPTST_SVCID     0x00130000L
    /**< Dynamic application test service. */
#define ELITE_STAT_PLAYBACK_SVCID 0x00140000L
    /**< Static playback service. */
#define ELITE_DYN_DECODER_SVCID   0x00150000L
    /**< Dynamic decoder service. */
#define ELITE_DYN_SINK_SVCID      0x00160000L
    /**< Dynamic sink service. */
#define ELITE_DYN_ENCODER_SVCID   0x00170000L
    /**< Dynamic encoder service. */
#define ELITE_MT_MX_SVCID         0x00180000L
    /**< Not used. */
#define ELITE_MT_MX_TEST_DOWNSTREAM_SVCID   0x00190000L
    /**< Not used. */
#define ELITE_MT_MX_TEST_UPSTREAM_SVCID     0x001A0000L
    /**< Not used. */
#define ELITE_MT_MX_TEST_SVCID              0x001B0000L
    /**< Not used. */
#define ELITE_DYN_ENCODER_TST_SVCID         0x001C0000L
    /**< Dynamic encoder test service. */
#define ELITE_AFE_DEVSERVICE_SVCID          0x00200000L
    /**< AFE device service. */
#define ELITE_AFE_DEVPORT0_SVCID            0x00210000L
    /**< AFE device port 0 service. */
#define ELITE_AFE_DEVPORT1_SVCID            0x00220000L
    /**< AFE device port 1 service. */
/** @cond OEM_only */
#define ELITE_AAC_SBR_RIGHT_SVCID           0x00230000L
    /**< Advanced audio codec spectral band replication service. */
/** @endcond */
#define ELITE_AUDPP_TEST_DOWNSTREAM_SVCID   0x00240000L
    /**< Audio postprocessor test service on the downstream path. */
#define ELITE_AUDPP_TEST_UPSTREAM_SVCID     0x00250000L 
    /**< Audio postprocessor test service on the upstream path. */
#define ELITE_AUDPP_TEST_SVCID              0x00260000L
    /**< Audio postprocessor test service. */
#define ELITE_VOICE_MX_SVCID                0x00270000L
    /**< Voice mix service. */
/** @cond OEM_only */
#define ELITE_DYN_MIDI_SVCID                0x00280000L
    /**< Dynamic MIDI service. */
/** @endcond */
#define ELITE_DYN_SWITCH_SVCID              0x00290000L
    /**< Dynamic switch service. */
#define ELITE_DYN_STREAM_ROUTER_SVCID       0x002A0000L
    /**< Dynamic stream router service. */
#define ELITE_STATUSM_SVCID                 0x002B0000L
    /**< Static ultrasound service. */
#define ELITE_STATLSM_SVCID                 0x002C0000L
    /**< Static listen service. */
#define ELITE_PSPD_SVCID                    0x002D0000L
    /**< Per-stream per-device threads spawned by the matrix service. */
/** @cond OEM_only */
#define ELITE_HEAAC_SBR_RIGHT_SVCID         0x002E0000L
    /**< High Efficiency AAC codec spectral band replication
         service. @newpage */
/** @endcond */

/* -----------------------------------------------------------------------
** Type definitions
** ----------------------------------------------------------------------- */

/** Contains the public fields that are shared with clients.
 */
typedef struct
{
   uint32_t                  unSvcId;    /**< ID of the service. */
   qurt_elite_thread_t       threadId;   /**< Handle to the thread to join
                                              when a service is destroyed. */
   qurt_elite_queue_t*       dataQ;      /**< Handle to the data queue of the
                                              service. */
   qurt_elite_queue_t*       cmdQ;       /**< Handle to the command queue of
                                              the service. */
   qurt_elite_queue_t*       gpQ;        /**< Handle to the output data queue
                                              of the encoder service. */
} elite_svc_handle_t;

/** @} */ /* end_addtogroup elite_svc_ids */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITESVC_H

