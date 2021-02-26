/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <byteswap.h>

#include "dtshd_packetizer.h"
#include "audio_parsers.h"

static unsigned char *audio_data;
static unsigned int audio_data_sz;

int get_dtshd_iec61937_info(struct audio_parser_codec_info *audio_codec_info)
{
	uint32_t rc = 0;
	uint32_t transmission_rate = 0;
	uint32_t transmission_channels = 0;

	rc = dtshd_get_iec61937_info(audio_data, audio_data_sz,
			&transmission_rate, &transmission_channels);
	if (!rc) {
		audio_codec_info->codec_type = AUDIO_PARSER_CODEC_DTSHD;

		audio_codec_info->codec_config.dtshd_tr_info.sample_rate
						= transmission_rate;
		audio_codec_info->codec_config.dtshd_tr_info.num_channels
						= transmission_channels;
	}

	return rc;
}

void init_audio_parser_dtshd(unsigned char *audio_stream_data,
		unsigned int audio_stream_sz)
{
	audio_data = audio_stream_data;
	audio_data_sz = audio_stream_sz;
}
