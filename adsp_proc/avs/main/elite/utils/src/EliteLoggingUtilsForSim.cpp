/*============================================================================

                    E l i t e  L o g g i n g  U t i l s  F o r  S i m

 *//** @file EliteLoggingUtilsForSim.cpp
  This file contains utilities used by logging for sim

@par EXTERNALIZED FUNCTIONS
  (none)

@par INITIALIZATION AND SEQUENCING REQUIREMENTS
  (none)

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
  *//*========================================================================*/

/*============================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/src/EliteLoggingUtilsForSim.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
            rb

============================================================================*/

/*============================================================================
 *                       INCLUDE FILES FOR MODULE
 *==========================================================================*/

#if defined(SIM)

#include "EliteLoggingUtils_i.h"
#include "qurt_elite.h"
#include "adsp_private_api.h"
#include "EliteAprIf.h"

#define ELITE_SIM_MAX_STRING_LEN     256
#define ELITE_SIM_CFG_FILE          "aDSPTest/TestConfig/qdsp6/root/adsp_file_logging.cfg"
/**
 * 152E is at zero position, 0x1586 is at 88th position.
 */
#define ELITE_SIM_LOG_CODE_TO_IND(log_code)  (log_code - QXDM_LOG_CODE_AUD_DEC_IN)

#define ELITE_SIM_NUM_TAGS    19
const char ELITE_SIM_TAGS[ELITE_SIM_NUM_TAGS][ELITE_SIM_MAX_STRING_LEN] =
{
		"LOG_FILE_NAME_PREFIX",
		"0x152E__QXDM_LOG_CODE_AUD_DEC_IN",
		"0x152F__QXDM_LOG_CODE_AUD_POPP_IN",
		"0x1531__QXDM_LOG_CODE_AUD_COPP_IN",
		"0x1534__QXDM_LOG_CODE_AUD_POPREP_IN",
		"0x1532__QXDM_LOG_CODE_AUD_COPREP_IN",
		"0x1530__QXDM_LOG_CODE_AUD_MTMX_RX_IN",
		"0x1533__QXDM_LOG_CODE_AUD_MTMX_TX_IN",
		"0x1535__QXDM_LOG_CODE_AUD_ENC_IN",
		"0x1536__QXDM_LOG_CODE_AUD_ENC_OUT",
		"0x1586__QXDM_LOG_CODE_AFE_RX_TX_OUT",
		"0x17D8__QXDM_LOG_CODE_AFE_ALGO_CALIB",
		"0x1882__QXDM_LOG_CODE_LSM_OUTPUT",
		"0x19AF__QXDM_LOG_CODE_AUD_DEC_OUT",
		"0x19B0__QXDM_LOG_CODE_AUD_COPP_OUT",
		"0x19B1__QXDM_LOG_CODE_AUD_POPP_OUT",
		"TEXT_LOGGING_LEVEL__POW_2_AVS_FATAL_16_TO_LOW_1",
		"TEXT_LOGGING_LEVEL__POW_2_TEST_FMWK_FATAL_16_TO_LOW_1",
		"TEXT_LOGGING_LEVEL__POW_2_SHARED_LIBS_FATAL_16_TO_LOW_1"
};

struct elite_sim_file_log_cfg
{
	char     file_name_prefix[ELITE_SIM_MAX_STRING_LEN];
	bool_t   is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_POPP_OUT)+1];
};

static elite_sim_file_log_cfg sim_log_cfg;

/** Lowest debug message level to be shown in ADSP. This is only valid in SIM mode. */
uint8_t qurt_elite_debugmsg_lowest_prio = DBG_MED_PRIO;
/** Lowest debug message level to be shown in so files. This is only valid in SIM mode. */
uint8_t shared_lib_api_debugmsg_lowest_prio = DBG_MED_PRIO;
/** Lowest debug message level to be shown in ADSP test fmwk. Variable is defined in test framework dbg_util.cpp.  */
extern uint8_t adsp_test_fmwk_debugmsg_lowest_prio;

ADSPResult elite_sim_read_file_log_cfg(const char *file_name, elite_sim_file_log_cfg *log_cfg)
{
	ADSPResult result = ADSP_EOK;
	char format[32];
	char temp[ELITE_SIM_MAX_STRING_LEN];
	uint8_t temp_adsp_logging=qurt_elite_debugmsg_lowest_prio;

	memset(log_cfg, 0, sizeof(elite_sim_file_log_cfg));

	FILE *cfg_file = fopen(file_name, "r");

	if (NULL == cfg_file)
	{
		return result;
	}

	snprintf(format, sizeof(format), "%%%lus", (unsigned long int) (ELITE_SIM_MAX_STRING_LEN-1));

	for (int i = 0; i < ELITE_SIM_NUM_TAGS; i++)
	{
		//reset to start of the file.
		fseek(cfg_file, 0, SEEK_SET);

		while (!feof(cfg_file))
		{
			(void) fscanf(cfg_file,format,temp);

			if (strncmp(temp, ELITE_SIM_TAGS[i], strlen(ELITE_SIM_TAGS[i])))
			{
				continue;
			}
			else
			{
				(void) fscanf(cfg_file,format,temp);
				//match
				switch (i)
				{
				case 0:  //look for string file name prefix
					qurt_elite_strl_cpy(log_cfg->file_name_prefix, temp, ELITE_SIM_MAX_STRING_LEN );
					break;
				case 1:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_DEC_IN)] = (bool_t) atol(temp);
					break;
				case 2:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_POPP_IN)] = (bool_t) atol(temp);
					break;
				case 3:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_COPP_IN)] = (bool_t) atol(temp);
					break;
				case 4:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_POPREP_IN)] = (bool_t) atol(temp);
					break;
				case 5:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_COPREP_IN)] = (bool_t) atol(temp);
					break;
				case 6:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_MTMX_RX_IN)] = (bool_t) atol(temp);
					break;
				case 7:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_MTMX_TX_IN)] = (bool_t) atol(temp);
					break;
				case 8:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_ENC_IN)] = (bool_t) atol(temp);
					break;
				case 9:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_ENC_OUT)] = (bool_t) atol(temp);
					break;
				case 10:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AFE_RX_TX_OUT)] = (bool_t) atol(temp);
					break;
				case 11:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AFE_ALGO_CALIB)] = (bool_t) atol(temp);
					break;
				case 12:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_LSM_OUTPUT)] = (bool_t) atol(temp);
					break;
				case 13:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_DEC_OUT)] = (bool_t) atol(temp);
					break;
				case 14:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_COPP_OUT)] = (bool_t) atol(temp);
					break;
				case 15:
					log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_POPP_OUT)] = (bool_t) atol(temp);
					break;

				case 16: //TEXT_LOGGING_LEVEL__POW_2_AVS_FATAL_16_TO_LOW_1
				{
					uint8_t temp1 = (uint8_t) atol(temp);
					if (temp1 < DBG_LOW_PRIO)
					{
						temp1 = DBG_LOW_PRIO;
					}
					if (temp1 > DBG_FATAL_PRIO)
					{
						temp1 = DBG_FATAL_PRIO;
					}
					temp_adsp_logging = temp1;
				}
				break;
				case 17: //TEXT_LOGGING_LEVEL__POW_2_TEST_FMWK_FATAL_16_TO_LOW_1
				{
					uint8_t temp1 = (uint8_t) atol(temp);
					if (temp1 < DBG_LOW_PRIO)
					{
						temp1 = DBG_LOW_PRIO;
					}
					if (temp1 > DBG_ERROR_PRIO)
					{
						temp1 = DBG_ERROR_PRIO;
					}

					//map from qurt_elite definition to test fmwk definition
					//Qurt elite:					//test framework
					//#define DBG_FATAL_PRIO 16
					//#define DBG_ERROR_PRIO 8		//#define DBG_MSG_LEVEL_ERROR  8
					//#define DBG_HIGH_PRIO  4		//#define DBG_MSG_LEVEL_HIGH   4
					//#define DBG_MED_PRIO   2		//#define DBG_MSG_LEVEL_MEDIUM 2
					//#define DBG_LOW_PRIO   1		//#define DBG_MSG_LEVEL_LOW    1

					adsp_test_fmwk_debugmsg_lowest_prio = temp1;
				}
				break;
				case 18: //TEXT_LOGGING_LEVEL__POW_2_SHARED_LIBS_FATAL_16_TO_LOW_1
				{
					uint8_t temp1 = (uint8_t) atol(temp);
					if (temp1 < DBG_LOW_PRIO)
					{
						temp1 = DBG_LOW_PRIO;
					}
					if (temp1 > DBG_FATAL_PRIO)
					{
						temp1 = DBG_FATAL_PRIO;
					}					
					shared_lib_api_debugmsg_lowest_prio = temp1;
				}
				break;
				default:
					break;
				}
			} /* strnmp */
		} /* while (feof) */
	} /* end of for loop */

	/** note that this log message may not appear based on logs are enabled or not */
	MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"File logging code-enable = %X-%d, %X-%d, %X-%d, %X-%d,",
			QXDM_LOG_CODE_AUD_DEC_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_DEC_IN)],
			QXDM_LOG_CODE_AUD_POPP_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_POPP_IN)],
			QXDM_LOG_CODE_AUD_COPP_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_COPP_IN)],
			QXDM_LOG_CODE_AUD_POPREP_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_POPREP_IN)]);

	MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"File logging code-enable = %X-%d, %X-%d, %X-%d, %X-%d,",
			QXDM_LOG_CODE_AUD_COPREP_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_COPREP_IN)],
			QXDM_LOG_CODE_AUD_MTMX_RX_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_MTMX_RX_IN)],
			QXDM_LOG_CODE_AUD_MTMX_TX_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_MTMX_TX_IN)],
			QXDM_LOG_CODE_AUD_ENC_IN, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_ENC_IN)]);

	MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"File logging code-enable = %X-%d, %X-%d, %X-%d",
			QXDM_LOG_CODE_AUD_ENC_OUT, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_ENC_OUT)],
			QXDM_LOG_CODE_AFE_RX_TX_OUT,log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AFE_RX_TX_OUT)],
			QXDM_LOG_CODE_AFE_ALGO_CALIB,log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AFE_ALGO_CALIB)]);

	MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"File logging code-enable = %X-%d, %X-%d, %X-%d",
				QXDM_LOG_CODE_AUD_DEC_OUT, log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_DEC_OUT)],
				QXDM_LOG_CODE_AUD_COPP_OUT,log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_COPP_OUT)],
				QXDM_LOG_CODE_AUD_POPP_OUT,log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_AUD_POPP_OUT)]);

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"File logging code-enable = %X-%d, prefix = %s",
			QXDM_LOG_CODE_LSM_OUTPUT,log_cfg->is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(QXDM_LOG_CODE_LSM_OUTPUT)],log_cfg->file_name_prefix);

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Min Text logging level (FATAL 16, ERROR 8, HIGH 4, MED 2, LOW 1): ADSP: %u, Test Framework: %u , so files: %u",
			temp_adsp_logging, adsp_test_fmwk_debugmsg_lowest_prio, shared_lib_api_debugmsg_lowest_prio);

	qurt_elite_debugmsg_lowest_prio = temp_adsp_logging;

	fclose(cfg_file);
	return result;
}

ADSPResult elite_sim_init_file_logging()
{
	ADSPResult result = elite_sim_read_file_log_cfg(ELITE_SIM_CFG_FILE, &sim_log_cfg);

	return result;
}

bool_t elite_sim_is_file_log_enabled (elite_qxdm_log_code log_code)
{
	return sim_log_cfg.is_log_code_enabled[ELITE_SIM_LOG_CODE_TO_IND(log_code)];
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
	uint8_t		                 channel_mapping[8];

	if(NULL == log_info_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EliteLoggingUtil : Log info ptr is NULL");
		return ADSP_EFAILED;
	}

	if (elite_sim_is_file_log_enabled(log_info_ptr->qxdm_log_code))
	{
		char temp[512];
		if (ELITE_LOG_DATA_FMT_PCM == log_info_ptr->data_fmt)
		{
			if (log_info_ptr->data_info.pcm_data_fmt.interleaved) //if interleaved, then write as one file.
			{
				/* Provide the default mapping */
				if(NULL == log_info_ptr->data_info.pcm_data_fmt.channel_mapping)
				{
					memset(channel_mapping, 0, 8);
					log_info_ptr->data_info.pcm_data_fmt.channel_mapping = channel_mapping;
					for(uint32_t i=0; i <  log_info_ptr->data_info.pcm_data_fmt.num_channels; i++)
					{
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[i] = i+1;
					}
				}


				snprintf(temp, sizeof(temp), (const char*) "%s_%X_%lX_%lX__b%d_s%lu_c%d__cm_%d_%d_%d_%d_%d_%d_%d_%d.pcm", sim_log_cfg.file_name_prefix,
						log_info_ptr->qxdm_log_code,
						log_info_ptr->session_id,
						log_info_ptr->log_tap_id,
						log_info_ptr->data_info.pcm_data_fmt.bits_per_sample,
						log_info_ptr->data_info.pcm_data_fmt.sampling_rate,
						log_info_ptr->data_info.pcm_data_fmt.num_channels,
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[0],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[1],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[2],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[3],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[4],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[5],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[6],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[7]);
				FILE *log_file = fopen(temp,"ab");
				if (NULL == log_file)
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
					return ADSP_EFAILED;
				}
				fwrite(log_info_ptr->buf_ptr, sizeof(uint8_t), log_info_ptr->buf_size, log_file);
				fclose(log_file);

				snprintf(temp, sizeof(temp), (const char*) "%s.txt", temp);
				FILE *ts_file = fopen(temp,"a");
				if (NULL == ts_file)
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
					return ADSP_EFAILED;
				}
				fprintf(ts_file, (const char*) "[TS: %llu][Buf Size: %lu]\n",log_info_ptr->log_time_stamp, log_info_ptr->buf_size);
				fclose(ts_file);
			}
			else
			{
				uint32_t buf_size_per_ch = log_info_ptr->buf_size/log_info_ptr->data_info.pcm_data_fmt.num_channels;
				uint8_t *buf_ptr = log_info_ptr->buf_ptr;
				for (int ch = 0; ch < log_info_ptr->data_info.pcm_data_fmt.num_channels; ch++)
				{
					snprintf(temp, sizeof(temp), (const char*) "%s_%X_%lX_%lX__b%d_s%lu_c%d__cm_%d_pos_%d.pcm", sim_log_cfg.file_name_prefix,
							log_info_ptr->qxdm_log_code,
							log_info_ptr->session_id,
							log_info_ptr->log_tap_id,
							log_info_ptr->data_info.pcm_data_fmt.bits_per_sample,
							log_info_ptr->data_info.pcm_data_fmt.sampling_rate,
							log_info_ptr->data_info.pcm_data_fmt.num_channels,
							log_info_ptr->data_info.pcm_data_fmt.channel_mapping[ch],ch);
					FILE *log_file = fopen(temp,"ab");
					if (NULL == log_file)
					{
						MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
						return ADSP_EFAILED;
					}
					fwrite(buf_ptr, sizeof(uint8_t), buf_size_per_ch, log_file);
					buf_ptr += buf_size_per_ch;
					fclose(log_file);

					snprintf(temp, sizeof(temp), (const char*) "%s.txt", temp);
					FILE *ts_file = fopen(temp,"a");
					if (NULL == ts_file)
					{
						MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
						return ADSP_EFAILED;
					}
					fprintf(ts_file, (const char*) "[TS: %llu][Buf Size: %lu]\n",log_info_ptr->log_time_stamp, buf_size_per_ch);
					fclose(ts_file);
				}
			}
		}
		else
		{
			snprintf(temp, sizeof(temp), (const char*)"%s_%X_%lX_%lX_%lX.bin", sim_log_cfg.file_name_prefix,
					log_info_ptr->qxdm_log_code,
					log_info_ptr->data_info.media_fmt_id,
					log_info_ptr->session_id,
					log_info_ptr->log_tap_id);
			FILE *log_file = fopen(temp,"ab");
			if (NULL == log_file)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
				return ADSP_EFAILED;
			}
			fwrite(log_info_ptr->buf_ptr, sizeof(uint8_t), log_info_ptr->buf_size, log_file);
			fclose(log_file);

			snprintf(temp, sizeof(temp), (const char*) "%s.txt", temp);
			FILE *ts_file = fopen(temp,"a");
			if (NULL == ts_file)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
				return ADSP_EFAILED;
			}
			fprintf(ts_file, (const char*) "[TS: %llu][Buf Size: %lu]\n",log_info_ptr->log_time_stamp, log_info_ptr->buf_size);
			fclose(ts_file);
		}

	}

	return result;
}


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
	void        *log_pkt_ptr = NULL;

	log_pkt_size =  buf_size;

	/* Allocate the log packet
	 * log_alloc() returns pointer to the allocated log packet
	 * Returns NULL if log code is disabled on the QXDM GUI
	 */

#ifdef DEBUG_LOGGING_UTIL
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " EliteLoggingUtil : Req Pkt Size %u : bytes for  QXDM log code 0x%X", log_pkt_size, log_code);
#endif

	if(elite_sim_is_file_log_enabled(log_code))
	{
		log_pkt_ptr = qurt_elite_memory_malloc(log_pkt_size, QURT_ELITE_HEAP_DEFAULT);
	}

	return log_pkt_ptr;
}

ADSPResult elite_log_buffer(void *log_pkt_ptr, elite_log_info  *log_info_ptr)
{
	ADSPResult                          result = ADSP_EOK;
	uint8_t		                         channel_mapping[8];

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

	if (elite_sim_is_file_log_enabled(log_info_ptr->qxdm_log_code))
	{
		char temp[512];
		switch(log_info_ptr->data_fmt)
		{
		case ELITE_LOG_DATA_FMT_PCM:
		{

			/* Provide the default mapping */
			if(NULL == log_info_ptr->data_info.pcm_data_fmt.channel_mapping)
			{
				memset(channel_mapping, 0, 8);
				log_info_ptr->data_info.pcm_data_fmt.channel_mapping = channel_mapping;
				for(uint32_t i=0; i <  log_info_ptr->data_info.pcm_data_fmt.num_channels; i++)
				{
					log_info_ptr->data_info.pcm_data_fmt.channel_mapping[i] = i+1;
				}
			}

			if(log_info_ptr->data_info.pcm_data_fmt.interleaved) //if interleaved, then write as one file.
			{
				snprintf(temp, sizeof(temp), (const char *)"%s_%X_%lX_%lX__b%d_s%lu_c%d__cm_%d_%d_%d_%d_%d_%d_%d_%d.pcm", sim_log_cfg.file_name_prefix,
						log_info_ptr->qxdm_log_code,
						log_info_ptr->session_id,
						log_info_ptr->log_tap_id,
						log_info_ptr->data_info.pcm_data_fmt.bits_per_sample,
						log_info_ptr->data_info.pcm_data_fmt.sampling_rate,
						log_info_ptr->data_info.pcm_data_fmt.num_channels,
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[0],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[1],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[2],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[3],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[4],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[5],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[6],
						log_info_ptr->data_info.pcm_data_fmt.channel_mapping[7]);
				FILE *log_file = fopen(temp, "ab");
				if(NULL == log_file)
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
					return ADSP_EFAILED;
				}
				fwrite(log_info_ptr->buf_ptr, sizeof(uint8_t), log_info_ptr->buf_size, log_file);
				fclose(log_file);

				snprintf(temp, sizeof(temp), (const char *)"%s.txt", temp);
				FILE *ts_file = fopen(temp, "a");
				if(NULL == ts_file)
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
					return ADSP_EFAILED;
				}
				fprintf(ts_file, (const char *)"[TS: %llu][Buf Size: %lu]\n", log_info_ptr->log_time_stamp, log_info_ptr->buf_size);
				fclose(ts_file);
			}
			else
			{
				uint32_t buf_size_per_ch = log_info_ptr->buf_size / log_info_ptr->data_info.pcm_data_fmt.num_channels;
				uint8_t *buf_ptr = log_info_ptr->buf_ptr;
				for(int ch = 0; ch < log_info_ptr->data_info.pcm_data_fmt.num_channels; ch++)
				{
					snprintf(temp, sizeof(temp), (const char *)"%s_%X_%lX_%lX__b%d_s%lu_c%d__cm_%d_pos_%d.pcm", sim_log_cfg.file_name_prefix,
							log_info_ptr->qxdm_log_code,
							log_info_ptr->session_id,
							log_info_ptr->log_tap_id,
							log_info_ptr->data_info.pcm_data_fmt.bits_per_sample,
							log_info_ptr->data_info.pcm_data_fmt.sampling_rate,
							log_info_ptr->data_info.pcm_data_fmt.num_channels,
							log_info_ptr->data_info.pcm_data_fmt.channel_mapping[ch], ch);
					FILE *log_file = fopen(temp, "ab");
					if(NULL == log_file)
					{
						MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
						return ADSP_EFAILED;
					}
					fwrite(buf_ptr, sizeof(uint8_t), buf_size_per_ch, log_file);
					buf_ptr += buf_size_per_ch;
					fclose(log_file);

					snprintf(temp, sizeof(temp), (const char *)"%s.txt", temp);
					FILE *ts_file = fopen(temp, "a");
					if(NULL == ts_file)
					{
						MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
						return ADSP_EFAILED;
					}
					fprintf(ts_file, (const char *)"[TS: %llu][Buf Size: %lu]\n", log_info_ptr->log_time_stamp, buf_size_per_ch);
					fclose(ts_file);
				}
			}
		}
		break;
		case ELITE_LOG_DATA_FMT_BITSTREAM:
		{
			snprintf(temp, sizeof(temp), (const char *)"%s_%X_%lX_%lX_%lX.bin", sim_log_cfg.file_name_prefix,
					log_info_ptr->qxdm_log_code,
					log_info_ptr->data_info.media_fmt_id,
					log_info_ptr->session_id,
					log_info_ptr->log_tap_id);
			FILE *log_file = fopen(temp, "ab");
			if(NULL == log_file)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
				return ADSP_EFAILED;
			}
			fwrite(log_info_ptr->buf_ptr, sizeof(uint8_t), log_info_ptr->buf_size, log_file);
			fclose(log_file);

			snprintf(temp, sizeof(temp), (const char *)"%s.txt", temp);
			FILE *ts_file = fopen(temp, "a");
			if(NULL == ts_file)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Error logging file");
				return ADSP_EFAILED;
			}
			fprintf(ts_file, (const char *)"[TS: %llu][Buf Size: %lu]\n", log_info_ptr->log_time_stamp, log_info_ptr->buf_size);
			fclose(ts_file);
		}
		break;
		}

		qurt_elite_memory_free(log_pkt_ptr);
		log_pkt_ptr = NULL;
	}

	return result;
}

ADSPResult AVS_SetLogCfgFile(elite_apr_packet_t *pPkt)
{
	ADSPResult result = ADSP_EOK;

	if(elite_apr_if_get_payload_size(pPkt) < sizeof(avcs_cmd_set_log_cfg_file_t))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for SetLogCfgFile");
		return ADSP_ENEEDMORE;
	}

	avcs_cmd_set_log_cfg_file_t *pCmd = (avcs_cmd_set_log_cfg_file_t*)(elite_apr_if_get_payload_ptr(pPkt));
	result = elite_sim_read_file_log_cfg((const char *)pCmd->log_cfg_file_name, &sim_log_cfg);

	return result;
}

#endif  //defined(SIM)

