/*
 *
 * Copyright (c) 2013, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#ifndef __ADIERTAC_H__
#define __ADIERTAC_H__

#include "acph.h"

#ifndef _ANDROID_
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#endif

#define ACPH_CAL_DATA_UNIT_LENGTH   4

#define CODEC_INFO_PATH "/proc/asound/card0/codecs/"

struct chipset_id_info {
	char	*name;
	int	id;
};

static struct chipset_id_info codec_chipset[] = {
	{"WCD9341", WCD9341},
	{"WCD9340", WCD9340},
	{"MSM8X53", MSM8X53},
	{"MSM8X52", MSM8X52},
	{"WCD9335", WCD9335},
	{"WSA881X-ANALOG", WSA881X_ANALOG},
	{"WSA881X-SOUNDWIRE", WSA881X_SOUNDWIRE},
	{"WSA881X", WSA881X},
	{"MSM8909", MSM8909},
	{"WCD9330", WCD9330},
	{"WCD9326", WCD9326},
	{"WCD9320", WCD9320},
	{"WCD9310", WCD9310},
	{"WCD9306", WCD9306},
	{"WCD9302", WCD9302},
	{"MSM8X16", MSM8X16},
	{"MSM8X10", MSM8X10},
	{"SDM660-CDC", SDM660_DIGITAL},
	{"SDW-CDC", MSM89XX_SOUNDWIRE},
	{"DRAX-CDC", SDM660_ANALOG}
};

void adie_rtac_init(void);
void adie_rtac_exit(void);
#endif
