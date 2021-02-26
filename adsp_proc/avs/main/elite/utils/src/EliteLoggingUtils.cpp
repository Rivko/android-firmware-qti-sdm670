/*============================================================================

                    E l i t e  L o g g i n g  U t i l s

*//** @file EliteLoggingUtils.cpp
  This file contains utilities used by multiple vocproc modules.

@par EXTERNALIZED FUNCTIONS
  (none)

@par INITIALIZATION AND SEQUENCING REQUIREMENTS
  (none)

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*========================================================================*/

/*============================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/src/EliteLoggingUtils.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------


============================================================================*/

/*============================================================================
 *                       INCLUDE FILES FOR MODULE
 *==========================================================================*/

#if defined(__qdsp6__) && !defined(SIM)

#include "EliteLoggingUtils_i.h"
#include <stringl/stringl.h>
#include "EliteAprIf.h"

/**
  Allocate the log packet for logging PCM data through QXDM

  @param[in]  buf_Size      Size of data to be logged in BYTES
  @param[in]  log_code      QXDM log code
  @param[in]  log_code      data type, PCM or bitstream
  @return
  Void pointer to the log packet, if log allocation is
  successful

  @dependencies
  None.
 */

void * log_alloc_buffer(uint32_t buf_size, elite_qxdm_log_code log_code,
                        elite_log_data_format data_fmt)
{
    uint32_t    log_pkt_size;
    uint32_t    log_header_size;
    void        *log_pkt_ptr = NULL;

    /* First calculate the log header size */
    if(ELITE_LOG_DATA_FMT_PCM == data_fmt)
    {
        /* -1, as the payload is declared as 1 byte in the pkt struct*/
        log_header_size = (sizeof(log_pkt_pcm_data) - 1);
    }
    else
    {
        log_header_size = (sizeof(log_pkt_bitstream_data) - 1);
    }

    /* Caclulate total packet size including the data payload */
    log_pkt_size = (log_header_size + buf_size);

    /* Allocate the log packet
     * log_alloc() returns pointer to the allocated log packet
     * Returns NULL if log code is disabled on the QXDM GUI
     */

#ifdef DEBUG_LOGGING_UTIL
     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " EliteLoggingUtil : Req Pkt Size %u : bytes for  QXDM log code 0x%X", log_pkt_size, log_code);
#endif

     log_pkt_ptr = log_alloc(log_code, log_pkt_size);

    return log_pkt_ptr;
}

/**
  Configure the audio log header with stream parameters and
  commit the packet for logging.

  param[in]  log_info_ptr  : Pointer to struct containing common
  information for populating the log header for both PCM and BS
  data

  @result
  ADSPResult

  @dependencies
  None.
 */

ADSPResult elite_allocbuf_and_log_data_pkt(elite_log_info * log_info_ptr)
{
    ADSPResult                  result = ADSP_EOK;
    void                        *log_pkt_ptr;
    uint32_t                    rem_log_buf_size, log_buf_size;
    uint8_t                         *curr_buf_ptr;
    audiolog_header_cmn     *log_header_cmn;
    log_pkt_pcm_data            *pcm_data_pkt_ptr;
    uint32_t                        total_log_buf_size;
    bool_t                      is_first_seg = TRUE;
    uint32_t                        num_channels = 0;
    uint8_t                     interleaved;

    if(NULL == log_info_ptr)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteLoggingUtil : Log info ptr is NULL");
        return ADSP_EFAILED;
    }

    curr_buf_ptr = log_info_ptr->buf_ptr;
    rem_log_buf_size = log_info_ptr->buf_size;

    total_log_buf_size = log_info_ptr->buf_size;

    /*  If the buffer size to log is > ELITE_MAX_LOG_PKT_SIZE, logging per iteraton
        is restricted to this size.
    */
    while(rem_log_buf_size > 0)
    {
        log_buf_size = s32_min_s32_s32(ELITE_MAX_LOG_PKT_SIZE, rem_log_buf_size);

        log_pkt_ptr = log_alloc_buffer(log_buf_size, log_info_ptr->qxdm_log_code, log_info_ptr->data_fmt);

        if(NULL == log_pkt_ptr)
        {

#ifdef DEBUG_LOGGING_UTIL
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " EliteLoggingUtil : Log buffer allocation failed for QXDM log code 0x%X, bufsize : %u bytes", log_info_ptr->qxdm_log_code, log_buf_size);
#endif
            return ADSP_EFAILED;
        }

#ifdef DEBUG_LOGGING_UTIL
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " EliteLoggingUtil : Log buffer alloc SUCCESS for QXDM log code 0x%X, bufsize : %u bytes", log_info_ptr->qxdm_log_code, log_buf_size);
#endif

        log_info_ptr->buf_ptr = curr_buf_ptr;
        log_info_ptr->buf_size = log_buf_size;

        pcm_data_pkt_ptr = (log_pkt_pcm_data *)log_pkt_ptr;
        log_header_cmn = &(pcm_data_pkt_ptr->log_header_pcm.cmn_struct);

        /* For PCM format, with deinterleaved data if log buffer size is more than
            max value, it is segmented and each segment is logged individually.
        */
        interleaved = log_info_ptr->data_info.pcm_data_fmt.interleaved;

        if( (ELITE_MAX_LOG_PKT_SIZE == log_buf_size) &&
             (total_log_buf_size > ELITE_MAX_LOG_PKT_SIZE) &&
             (TRUE == is_first_seg) &&
             (ELITE_LOG_DATA_FMT_PCM == log_info_ptr->data_fmt) &&
             (!interleaved)
            )
        {
            is_first_seg = FALSE;
        }

        if(FALSE == is_first_seg)
        {
            num_channels = log_info_ptr->data_info.pcm_data_fmt.num_channels;
            log_header_cmn->log_seg_number = ELITE_SEG_PKT;
            log_header_cmn->fragment_offset = (total_log_buf_size / (num_channels) );
        }

        result = elite_log_buffer(log_pkt_ptr, log_info_ptr);

        if(ADSP_EOK != result)
        {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " EliteLoggingUtil : Error occured while populating log packet for QXDM log code 0x%X", log_info_ptr->qxdm_log_code);
            /* Freeing up the allocated log buffer */
            log_free(log_pkt_ptr);
            return ADSP_EFAILED;
        }

        /* Increment the read/write pointers to buffer to be logged */

        rem_log_buf_size -= log_buf_size;

        if(FALSE == is_first_seg)
        {
            curr_buf_ptr += (log_buf_size / num_channels);
        }
        else
        {
            curr_buf_ptr += log_buf_size;
        }

    } /* while loop */

    return result;
}


ADSPResult elite_log_buffer(void *log_pkt_ptr, elite_log_info  *log_info_ptr)
{
    ADSPResult                          result = ADSP_EOK;
    audiolog_header_cmn                 *log_header_cmn;
    audiolog_pcm_data_format            *pcm_data_fmt;
    audiolog_bitstream_data_format      *bs_data_fmt;
    pcm_data_info                       *pcm_data_info_ptr;
    uint32_t                            i;
    uint8_t                             *log_dst_ptr = NULL;
    log_pkt_pcm_data                            *pcm_data_pkt_ptr;
    log_pkt_bitstream_data                  *bs_data_pkt_ptr;
    bool_t                                      is_seg_pkt = FALSE;
    uint32_t                                        num_channels = 0;
    uint32_t                                        num_bytes_per_channel = 0;
    uint32_t                                        total_num_bytes_per_channel = 0;


    if( (NULL == log_info_ptr) || (NULL == log_pkt_ptr) )
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteLoggingUtil : Log pkt ptr or log info ptr is NULL");
        return ADSP_EFAILED;
    }

    /* Check if the data format is either PCM or BITSTREAM */
    if( (log_info_ptr->data_fmt != ELITE_LOG_DATA_FMT_PCM) &&
         (log_info_ptr->data_fmt != ELITE_LOG_DATA_FMT_BITSTREAM) )
    {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "EliteLoggingUtil : Invalid data format : %u for QXDM log code 0x%X", log_info_ptr->data_fmt, log_info_ptr->qxdm_log_code);
        return ADSP_EFAILED;
    }

    pcm_data_pkt_ptr = (log_pkt_pcm_data *)log_pkt_ptr;

    /* Common for both PCM and BS data */
    log_header_cmn = &(pcm_data_pkt_ptr->log_header_pcm.cmn_struct);

    if(ELITE_SEG_PKT == log_header_cmn->log_seg_number)
    {
        is_seg_pkt = TRUE;

        total_num_bytes_per_channel = log_header_cmn->fragment_offset;
    }

    /* Fill in the common header for PCM and bitstream data */

    /*************** AUDIO_LOG_HEADER ***************/
    log_header_cmn->tag = AUDIOLOG_CONTAINER_LOG_HEADER;

    if(ELITE_LOG_DATA_FMT_PCM == log_info_ptr->data_fmt)
    {
        log_header_cmn->size = sizeof(audiolog_header_pcm_data);
    }
    else /* bitstream format */
    {
        log_header_cmn->size = sizeof(audiolog_header_bitstream_data);
    }

    /* Session ID is any random but unique value corresponding to a QXDM log code.
        For same QXDM log code and same tapping point, parser generates a new file
        with change in session ID. In general, session ID should be made function
        of stream/device parameters, so that a new log files gets generated if any
        of the parameter changes.
    */

    log_header_cmn->log_session_id = log_info_ptr->session_id;


    /* For now log_seq_number is un-used and set to 0.
        log_seq_number is supposed to be used as an identifier for
        all those chunks. Currently, parser does not support so.
        fragment_offset is used for representing the offset of a partial packet
        in the original buffer in BYTES.
    */
    log_header_cmn->log_seg_number = 0;

    /* Size (in BYTES) of actual buffer to be logged */
    log_header_cmn->segment_size = log_info_ptr->buf_size;

    /* Un-used for now, set to 0*/
    log_header_cmn->fragment_offset = 0;

    /*************** AUDIO_LOG_USER_SESSION ***************/
    log_header_cmn->user_session_info.tag = AUDIOLOG_CONTAINER_USER_SESSION;
    log_header_cmn->user_session_info.size = sizeof(audiolog_user_session);
    /* User session ID is un-used parameter */
    log_header_cmn->user_session_info.user_session_id = 0;
    log_header_cmn->user_session_info.time_stamp = log_info_ptr->log_time_stamp;

    switch(log_info_ptr->data_fmt)
    {
    case ELITE_LOG_DATA_FMT_PCM:
        {
            /*************** AUDIOLOG_PCM_DATA_FORAMT ***************/

            pcm_data_fmt = (audiolog_pcm_data_format *)(&(pcm_data_pkt_ptr->log_header_pcm.pcm_data_fmt));

            pcm_data_info_ptr = (pcm_data_info *)(&log_info_ptr->data_info.pcm_data_fmt);

            pcm_data_fmt->tag = AUDIOLOG_CONTAINER_PCM_DATA_FORMAT;
            pcm_data_fmt->size = sizeof(audiolog_pcm_data_format);
            pcm_data_fmt->log_tap_id = log_info_ptr->log_tap_id;
            pcm_data_fmt->sampling_rate = pcm_data_info_ptr->sampling_rate;
            pcm_data_fmt->num_channels = pcm_data_info_ptr->num_channels;
            pcm_data_fmt->pcm_width = pcm_data_info_ptr->bits_per_sample;
            pcm_data_fmt->interleaved = pcm_data_info_ptr->interleaved;

            if(NULL != pcm_data_info_ptr->channel_mapping)
            {
                for(i=0; i <  pcm_data_info_ptr->num_channels; i++)
                {
                    pcm_data_fmt->channel_mapping[i] = pcm_data_info_ptr->channel_mapping[i];
                }
            }
            else /* Provide the default mapping */
            {
                for(i=0; i <  pcm_data_info_ptr->num_channels; i++)
                {
                    pcm_data_fmt->channel_mapping[i] = i+1;
                }
            }

            /* Set the remaining channels elements as un-used */
            for(i =  pcm_data_info_ptr->num_channels; i < ELITE_MAX_NUM_CH; i++)
            {
                pcm_data_fmt->channel_mapping[i] = 0;
            }

            log_dst_ptr = ((log_pkt_pcm_data *)log_pkt_ptr)->payload;
        }
        break;
    case ELITE_LOG_DATA_FMT_BITSTREAM:
        {
            /********** AUDIOLOG_BITSTREAM_DATA_FORAMT *************/

            bs_data_pkt_ptr = (log_pkt_bitstream_data *)log_pkt_ptr;

            bs_data_fmt = (audiolog_bitstream_data_format *)(&(bs_data_pkt_ptr->log_header_bs.bs_data_fmt));

            bs_data_fmt->tag = AUDIOLOG_CONTAINER_BS_DATA_FORMAT;
            bs_data_fmt->size = sizeof(audiolog_bitstream_data_format);
            bs_data_fmt->log_tap_id = log_info_ptr->log_tap_id;
            bs_data_fmt->media_fmt_id = log_info_ptr->data_info.media_fmt_id;

            log_dst_ptr = ((log_pkt_bitstream_data *)log_pkt_ptr)->payload;
        }
        break;
    }

    /* Populate the log packet payload with the buffer to be logged */

    if(FALSE == is_seg_pkt)
    {
        memscpy(log_dst_ptr, log_info_ptr->buf_size, log_info_ptr->buf_ptr, log_info_ptr->buf_size);
    }
    else /* True */
    {
        pcm_data_info_ptr = (pcm_data_info *)(&log_info_ptr->data_info.pcm_data_fmt);

        num_channels = pcm_data_info_ptr->num_channels;
        num_bytes_per_channel = (log_info_ptr->buf_size / num_channels);

        for(i=0; i < num_channels; i++)
        {
            memscpy(log_dst_ptr, num_bytes_per_channel, log_info_ptr->buf_ptr, num_bytes_per_channel);
            log_dst_ptr += num_bytes_per_channel;
            log_info_ptr->buf_ptr += total_num_bytes_per_channel;
        }
    }

    /* Call the API to commit the log packet */
    log_commit(log_pkt_ptr);

    return result;
}

ADSPResult AVS_SetLogCfgFile(elite_apr_packet_t *pPkt)
{
	ADSPResult result = ADSP_EOK;
	return result;
}

#endif  //defined(__qdsp6__) && !defined(SIM)

