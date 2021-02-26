/*
 * Copyright (c) 2014-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*
 * 2014-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 * Copyright (c) 2006 Atheros Communications Inc.
 * All rights reserved.
 *
 *
// The software source and binaries included in this development package are
// licensed, not sold. You, or your company, received the package under one
// or more license agreements. The rights granted to you are specifically
// listed in these license agreement(s). All other rights remain with
// Atheros
// Communications, Inc., its subsidiaries, or the respective owner including
// those listed on the included copyright notices.  Distribution of any
// portion of this package must be in strict compliance with the license
// agreement(s) terms.
// </copyright>
//
// <summary>
// Wifi driver for AR6002
// </summary>
//
 *
 */
/*
#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <net/if.h>
#include <linux/wireless.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <stdio.h>
#include <time.h>
*/
#include <ctype.h>
#include "art_utf_common.h"

#include "libtcmd.h"
#include "testcmd.h"

#include "cmdAllParms.h"
#include "cmdOpcodes.h"

#include "tcmdHostInternal.h"
#include "maskRateCommon.h"

#include "genTxBinCmdTlv.h"
#include "parseRxBinCmdTlv.h"
#include "CliTlvCmd_if.h"

#include "resp_api.h"
#include "testcmd6174.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef USE_TLV2
//#include "dkCmdIds.h"
#include "parseBinCmdStream.h"
#include "tlv2Inc.h"
#include "cmdRspParmsDict.h"
#include "rate2RateBitIndex.h"
#endif /* USE_TLV2 */

#define ALOGE printf
#include "hex_dump.h"

#ifdef ANDROID
#include <cutils/properties.h>
#endif

#define BIT(n) (1 << (n))

#define DEFAULT_TX_BSSID       "50:55:55:55:55:05"
#define DEFAULT_TX_TXSTATION   "20:22:22:22:22:02"
#define DEFAULT_TX_RXSTATION   "ff:ff:ff:ff:ff:ff"

#define DEFAULT_RX_BSSID       "01:00:00:c0:ff:ee"
#define DEFAULT_RX_ADDR        "01:00:00:c0:ff:ee"

void testcmd_error(int code, const char *fmt, ...);


_CMD_PARM gCmd;

char g_ifname[IFNAMSIZ] = "wlan0";

const RATE_STR bgRateStrTbl[G_RATE_NUM] = {
	{ "1   Mb" },
	{ "2   Mb" },
	{ "5.5 Mb" },
	{ "11  Mb" },
	{ "6   Mb" },
	{ "9   Mb" },
	{ "12  Mb" },
	{ "18  Mb" },
	{ "24  Mb" },
	{ "36  Mb" },
	{ "48  Mb" },
	{ "54  Mb" },
	{ "HT20 MCS0 6.5   Mb" },
	{ "HT20 MCS1 13    Mb" },
	{ "HT20 MCS2 19.5  Mb" },
	{ "HT20 MCS3 26    Mb" },
	{ "HT20 MCS4 39    Mb" },
	{ "HT20 MCS5 52    Mb" },
	{ "HT20 MCS6 58.5  Mb" },
	{ "HT20 MCS7 65    Mb" },
	{ "HT20 MCS8 13    Mb" },
	{ "HT20 MCS9 26    Mb" },
	{ "HT20 MCS10 39   Mb" },
	{ "HT20 MCS11 52   Mb" },
	{ "HT20 MCS12 78   Mb" },
	{ "HT20 MCS13 104  Mb" },
	{ "HT20 MCS14 117  Mb" },
	{ "HT20 MCS15 130  Mb" },
	{ "HT20 MCS16 19.5 Mb" },
	{ "HT20 MCS17 39   Mb" },
	{ "HT20 MCS18 58.5 Mb" },
	{ "HT20 MCS19 78   Mb" },
	{ "HT20 MCS20 117  Mb" },
	{ "HT20 MCS21 156  Mb" },
	{ "HT20 MCS22 175.5  Mb" },
	{ "HT20 MCS23 195    Mb" },
	{ "HT40 MCS0 13.5    Mb" },
	{ "HT40 MCS1 27.0    Mb" },
	{ "HT40 MCS2 40.5    Mb" },
	{ "HT40 MCS3 54      Mb" },
	{ "HT40 MCS4 81      Mb" },
	{ "HT40 MCS5 108     Mb" },
	{ "HT40 MCS6 121.5   Mb" },
	{ "HT40 MCS7 135     Mb" },
	{ "HT40 MCS8 27      Mb" },
	{ "HT40 MCS9 54      Mb" },
	{ "HT40 MCS10 81     Mb" },
	{ "HT40 MCS11 108    Mb" },
	{ "HT40 MCS12 162    Mb" },
	{ "HT40 MCS13 216    Mb" },
	{ "HT40 MCS14 243    Mb" },
	{ "HT40 MCS15 270    Mb" },
	{ "HT40 MCS16 40.5   Mb" },
	{ "HT40 MCS17 81     Mb" },
	{ "HT40 MCS18 121.5  Mb" },
	{ "HT40 MCS19 162    Mb" },
	{ "HT40 MCS20 243    Mb" },
	{ "HT40 MCS21 324    Mb" },
	{ "HT40 MCS22 364.5  Mb" },
	{ "HT40 MCS23 405    Mb" },
	{ "VHT20 MCS0 S1 6.5 Mb" }, //60
	{ "VHT20 MCS1 S1 13 Mb" },
	{ "VHT20 MCS2 S1 19.5 Mb" },
	{ "VHT20 MCS3 S1 26 Mb" }, //63
	{ "VHT20 MCS4 S1 39 Mb" },
	{ "VHT20 MCS5 S1 52 Mb" },
	{ "VHT20 MCS6 S1 58.5 Mb" },
	{ "VHT20 MCS7 S1 65 Mb" },
	{ "VHT20 MCS8 S1 78 Mb" },
	{ "VHT20 MCS9 S1 86 Mb" },
	{ "VHT20 MCS0 S2 13 Mb" },
	{ "VHT20 MCS1 S2 26 Mb" },
	{ "VHT20 MCS2 S2 39 Mb" },
	{ "VHT20 MCS3 S2 52 Mb" },
	{ "VHT20 MCS4 S2 78 Mb" },
	{ "VHT20 MCS5 S2 104 Mb" },
	{ "VHT20 MCS6 S2 117 Mb" },
	{ "VHT20 MCS7 S2 130 Mb" },
	{ "VHT20 MCS8 S2 156 Mb" },
	{ "VHT20 MCS9 S2 173" },
	{ "VHT20 MCS0 S3 19.5 Mb" },
	{ "VHT20 MCS1 S3 39 Mb" },
	{ "VHT20 MCS2 S3 58.5 Mb" },
	{ "VHT20 MCS3 S3 78 Mb" },
	{ "VHT20 MCS4 S3 117 Mb" },
	{ "VHT20 MCS5 S3 156 Mb" },
	{ "VHT20 MCS6 S3 175.5 Mb" },
	{ "VHT20 MCS7 S3 195 Mb" },
	{ "VHT20 MCS8 S3 234 Mb" },
	{ "VHT20 MCS9 S3 260 Mb" },
	{ "VHT40 MCS0 S1 13.5 Mb" },
	{ "VHT40 MCS1 S1 27 Mb" },
	{ "VHT40 MCS2 S1 40.5 Mb" },
	{ "VHT40 MCS3 S1 54 Mb" },
	{ "VHT40 MCS4 S1 81 Mb" },
	{ "VHT40 MCS5 S1 108 Mb" },
	{ "VHT40 MCS6 S1 121.5 Mb" },
	{ "VHT40 MCS7 S1 135 Mb" },
	{ "VHT40 MCS8 S1 162 Mb" },
	{ "VHT40 MCS9 S1 180 Mb" },
	{ "VHT40 MCS0 S2 27 Mb" },
	{ "VHT40 MCS1 S2 54 Mb" },
	{ "VHT40 MCS2 S2 81 Mb" },
	{ "VHT40 MCS3 S2 108 Mb" },
	{ "VHT40 MCS4 S2 162 Mb" },
	{ "VHT40 MCS5 S2 216 Mb" },
	{ "VHT40 MCS6 S2 243 Mb" },
	{ "VHT40 MCS7 S2 270 Mb" },
	{ "VHT40 MCS8 S2 324 Mb" },
	{ "VHT40 MCS9 S2 360 Mb" },
	{ "VHT40 MCS0 S3 40.5 Mb" },
	{ "VHT40 MCS1 S3 81 Mb" },
	{ "VHT40 MCS2 S3 121.5 Mb" },
	{ "VHT40 MCS3 S3 162 Mb" },
	{ "VHT40 MCS4 S3 243 Mb" },
	{ "VHT40 MCS5 S3 324 Mb" },
	{ "VHT40 MCS6 S3 364.5 Mb" },
	{ "VHT40 MCS7 S3 405 Mb" },
	{ "VHT40 MCS8 S3 486 Mb" },
	{ "VHT40 MCS9 S3 540 Mb" },
	{ "VHT80 MCS0 S1 29.3 Mb" },
	{ "VHT80 MCS1 S1 58.5 Mb" },
	{ "VHT80 MCS2 S1 87.8 Mb" },
	{ "VHT80 MCS3 S1 117 Mb" },
	{ "VHT80 MCS4 S1 175.5 Mb" },
	{ "VHT80 MCS5 S1 234 Mb" },
	{ "VHT80 MCS6 S1 263.3 Mb" },
	{ "VHT80 MCS7 S1 292.5 Mb" },
	{ "VHT80 MCS8 S1 351 Mb" },
	{ "VHT80 MCS9 S1 390 Mb" },
	{ "VHT80 MCS0 S2 58.5 Mb" },
	{ "VHT80 MCS1 S2 117 Mb" },
	{ "VHT80 MCS2 S2 175.5 Mb" },
	{ "VHT80 MCS3 S2 234 Mb" },
	{ "VHT80 MCS4 S2 351 Mb" },
	{ "VHT80 MCS5 S2 468 Mb" },
	{ "VHT80 MCS6 S2 526.5 Mb" },
	{ "VHT80 MCS7 S2 585 Mb" },
	{ "VHT80 MCS8 S2 702 Mb" },
	{ "VHT80 MCS9 S2 780 Mb" },
	{ "VHT80 MCS0 S3 87.8 Mb" },
	{ "VHT80 MCS1 S3 175.5 Mb" },
	{ "VHT80 MCS2 S3 263.3 Mb" },
	{ "VHT80 MCS3 S3 351 Mb" },
	{ "VHT80 MCS4 S3 526.5 Mb" },
	{ "VHT80 MCS5 S3 702 Mb" },
	{ "VHT80 MCS6 S3 789.8 Mb" },
	{ "VHT80 MCS7 S3 877.5 Mb" },
	{ "VHT80 MCS8 S3 1053 Mb" },
	{ "VHT80 MCS9 S3 1170 Mb" },
	/* below portion(CCK short preamble rates) should always be placed to the end */
	{ "2(S)   Mb" },
	{ "5.5(S) Mb" },
	{ "11(S)  Mb" },
	/* Below rates are for 4x4 11ac */
	{ "VHT20 MCS0 S4 26 Mb" }, //153
	{ "VHT20 MCS1 S4 52 Mb" },
	{ "VHT20 MCS2 S4 78 Mb" },
	{ "VHT20 MCS3 S4 104 Mb" },
	{ "VHT20 MCS4 S4 156 Mb" },
	{ "VHT20 MCS5 S4 208 Mb" },
	{ "VHT20 MCS6 S4 234 Mb" },
	{ "VHT20 MCS7 S4 260 Mb" },
	{ "VHT20 MCS8 S4 312 Mb" },
	{ "VHT20 MCS9 S4 NA " },
	{ "VHT40 MCS0 S4 54 Mb" }, //163
	{ "VHT40 MCS1 S4 108 Mb" },
	{ "VHT40 MCS2 S4 162 Mb" },
	{ "VHT40 MCS3 S4 216 Mb" },
	{ "VHT40 MCS4 S4 324 Mb" },
	{ "VHT40 MCS5 S4 432 Mb" },
	{ "VHT40 MCS6 S4 486 Mb" },
	{ "VHT40 MCS7 S4 540 Mb" },
	{ "VHT40 MCS8 S4 648 Mb" },
	{ "VHT40 MCS9 S4 720 Mb" },
	{ "VHT80 MCS0 S4 117 Mb" }, //173
	{ "VHT80 MCS1 S4 234 Mb" },
	{ "VHT80 MCS2 S4 351 Mb" },
	{ "VHT80 MCS3 S4 468 Mb" },
	{ "VHT80 MCS4 S4 702 Mb" },
	{ "VHT80 MCS5 S4 936 Mb" },
	{ "VHT80 MCS6 S4 1053 Mb" },
	{ "VHT80 MCS7 S4 1170 Mb" },
	{ "VHT80 MCS8 S4 1404 Mb" },
	{ "VHT80 MCS9 S4 1560 Mb" }, // 182

	{ "VHT160 MCS0 S1  Mb" },  // 183
	{ "VHT160 MCS1 S1  Mb" },
	{ "VHT160 MCS2 S1  Mb" },
	{ "VHT160 MCS3 S1  Mb" },
	{ "VHT160 MCS4 S1  Mb" },
	{ "VHT160 MCS5 S1  Mb" },
	{ "VHT160 MCS6 S1  Mb" },
	{ "VHT160 MCS7 S1  Mb" },
	{ "VHT160 MCS8 S1  Mb" },
	{ "VHT160 MCS9 S1  Mb" },

	{ "VHT160 MCS0 S2  Mb" },
	{ "VHT160 MCS1 S2  Mb" },
	{ "VHT160 MCS2 S2  Mb" },
	{ "VHT160 MCS3 S2  Mb" },
	{ "VHT160 MCS4 S2  Mb" },
	{ "VHT160 MCS5 S2  Mb" },
	{ "VHT160 MCS6 S2  Mb" },
	{ "VHT160 MCS7 S2  Mb" },
	{ "VHT160 MCS8 S2  Mb" },
	{ "VHT160 MCS9 S2  Mb" }, // 202
};

int g_tx_init = false;
/* TLV2.0 flag */
int tlv2_enabled = FALSE;
bool tlv2_rx_vht_rates = FALSE;
/*------------------------------------------------------------------*/
/*
 * Input an Ethernet address and convert to binary.
 */
static int ath_ether_aton(const char *orig, uint8_t *eth)
{
	int mac[6];
	if (sscanf(orig, "%02x:%02x:%02X:%02X:%02X:%02X",
				&mac[0], &mac[1], &mac[2],
				&mac[3], &mac[4], &mac[5])==6) {
		int i;
#ifdef TCMD_LIB_DEBUG
		if (*(orig+12+5) !=0) {
			fprintf(stderr, "%s: trailing junk '%s'!\n", __func__, orig);
			testcmd_error(-1, "trailing junk");
			return -1;
		}
#endif
		for (i=0; i<6; ++i)
			eth[i] = mac[i] & 0xff;
		return 0;
	}
	else {
		testcmd_error(-1, "Invalid mac address format");
	}

	return -1;
}

static inline int is_zero_ether_addr(const uint8_t *a)
{
	return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}

static inline int is_broadcast_ether_addr(const uint8_t *a)
{
	return (a[0] & a[1] & a[2] & a[3] & a[4] & a[5]) == 0xff;
}

uint32_t channel_to_freq(uint32_t channel)
{
	if (channel >= 1 && channel <= 13)
		return 2407 + 5 * channel;
	else if (channel == 14)
		return 2484 + 5;
	else
		return 5000 + 5 * channel;
}

uint32_t freq_to_channel(unsigned int freq)
{
	if (freq >= 2412 && freq <= 2472)
		return (freq - 2407) / 5;
	else if (freq == 2484)
		return 14;
	else if (freq >= 5180 && freq <= 5240)
		return (freq - 5000) / 5;
	else
		return 0;
}

static void getIfName(char *ifname)
{
	memcpy(ifname, g_ifname, IFNAMSIZ);
}

static int isResponseNeeded(void *buf)
{
	int respNeeded = 0;
	TCMD_ID tcmd = TCMD_CONT_RX_ID;
	uint32_t mode = 0;

	tcmd = * ((uint32_t *) buf);
	mode = * ((uint32_t *) buf + 1);

	/// Insert commands which need response
	switch (tcmd)
	{
		case TC_CMD_TLV_ID:
			respNeeded = 1;
			break;
		case TCMD_CONT_RX_ID:
			switch (mode)
			{
				case TCMD_CONT_RX_REPORT:
				case TCMD_CONT_RX_GETMAC:
					respNeeded = 1;
					break;
			}
			break;
		case TC_CMDS_ID:
			switch (mode)
			{
				case TC_CMDS_READTHERMAL:
				case TC_CMDS_EFUSEDUMP:
				case TC_CMDS_EFUSEWRITE:
				case TC_CMDS_OTPSTREAMWRITE:
				case TC_CMDS_OTPDUMP:
					respNeeded = 1; //TC_CMDS_EFUSEDUMP, TC_CMDS_EFUSEWRITE, TC_CMDS_OTPSTREAMWRITE, TC_CMDS_OTPDUMP, TC_CMDS_READTHERMAL
					break;
			}
			break;
		default:
			break;
	}

	if (respNeeded)
	{
		ALOGE("cmdID %d response needed\n", tcmd);
	}
	else
	{
		ALOGE("cmdID %d response not needed\n", tcmd);
	}

	return respNeeded;
}

void parseGenericResponse(void)
{
	int ret = TRUE;
	uint8_t *data;
	uint32_t status,len;

	ret = getParams((uint8_t*)"status", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&status,data,len);
		ALOGE("Response status %d\n",status);
	}
	else
		printf("In parseResponse status not found\n");
}

/* need to verify */
void parseTLVSubmitReport(void)
{
	int ret = TRUE;
	uint8_t *data;
	uint32_t value,len,i,k;
	uint32_t totalpkt = 0;
	int32_t rssi;
	uint8_t rateCntBuf[25];
	uint32_t rateMask,rateMaskCnt[2],shortGuard,dataRate,index=0;
	uint32_t rate11ACMaskCnt[4];
	uint32_t bit0 = 0x00000001;

	ret = getParams((uint8_t*)"totalpkt", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&totalpkt,data,len);
		printf("totalpkt %d\n",totalpkt);
	}

	ret = getParams((uint8_t*)"rssiInDBm", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&rssi,data,len);

		if(totalpkt)
			printf("rssiInDBm %d\n",rssi/(int32_t)totalpkt);
		else
			printf("rssiInDBm %d\n",rssi);
	}

	ret = getParams((uint8_t*)"crcErrPkt", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&value,data,len);
		printf("crcErrPkt %d\n",value);
	}

	ret = getParams((uint8_t*)"secErrPkt", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&value,data,len);
		printf("secErrPkt %d\n",value);
	}

	for(i=0;i<2;i++)
	{
		memset(rateCntBuf,0,sizeof(rateCntBuf));
		snprintf((char*)rateCntBuf,25,"%s%d","rcMask",i);
		ret = getParams((uint8_t*)rateCntBuf,&data,&len);

		if ( ret == TRUE )
		{
			memcpy(&rateMaskCnt[i],data,len);
			//printf("%s %x\n",rateCntBuf,rateMaskCnt[i]);
		}
		else
		{
			rateMaskCnt[i] = 0;
		}
	}

	for(i=0;i<4;i++)
	{
		memset(rateCntBuf,0,sizeof(rateCntBuf));
		snprintf((char*)rateCntBuf,25,"%s%d","rcMask11ac",i);
		ret = getParams((uint8_t*)rateCntBuf,&data,&len);

		if ( ret == TRUE )
		{
			memcpy(&rate11ACMaskCnt[i],data,len);
			//printf("%s %x\n",rateCntBuf,rate11ACMaskCnt[i]);
		}
		else
		{
			rate11ACMaskCnt[i] = 0;
		}
	}

	ret = getParams((uint8_t*)"shortGuard", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&shortGuard,data,len);

		if ( shortGuard )
			printf("<<<<SGI set >>>>\n");
	}

	//11n and below rates..
	for (k=0;k<2;k++)
	{
		rateMask = rateMaskCnt[k];
		i=0; bit0=0x00000001;
		while (rateMask)
		{
			if (bit0 & rateMask)
			{
				rateMask &= ~bit0;
				dataRate = Mask2Rate[k][i];

				if (dataRate == ATH_RATE_2M || dataRate == ATH_RATE_5_5M || dataRate == ATH_RATE_11M){ /* CCK rates */
					if (!(i&1)) /* even number indicates short preamble, see Mask2Rate array for details */
						dataRate += (TCMD_MAX_RATES - 4);/* use last three rates in the rate table for shortPreamble */
				}

				//printf("RateCnt bit set %d dataRate %d %s\n",(k*32+i),dataRate,bgRateStrTbl[dataRate]);
				printf("%s .. ", bgRateStrTbl[dataRate]);

				index = (k*32+i)/2;

				memset(rateCntBuf,0,sizeof(rateCntBuf));
				snprintf((char*)rateCntBuf,25,"%s%d","rateCnt",index);
				ret = getParams((uint8_t*)rateCntBuf,&data,&len);

				if ( ret == TRUE )
				{
					memcpy(&value,data,len);

					if (i && !(i&1))
						printf("rateCnt %d\n",(value & 0xFFFF));
					else {
						if (i && (i&1)) {
							printf("rateCnt %d\n",(value >>16) & 0xFFFF);
						}
						else{
							printf("rateCnt %d\n",(value & 0xFFFF));
						}
					}
				}
				else {
					printf("rateCnt ..%d\n",0);
				}
			}

			bit0 = bit0 << 1;i++;
		}
	}

	//11ac rates..
	for (k=0;k<4;k++)
	{
		rateMask = rate11ACMaskCnt[k];
		i=0; bit0=0x00000001;
		while (rateMask)
		{
			if (bit0 & rateMask)
			{
				rateMask &= ~bit0;
				index = k + 2;
				dataRate = Mask2Rate[index][i];
				//printf("11ac RateCnt bit set %d dataRate %d %s\n",(k*32+i),dataRate,bgRateStrTbl[dataRate]);
				printf("%s .. ",bgRateStrTbl[dataRate]);

				index = (k*32+i)/2;

				memset(rateCntBuf,0,sizeof(rateCntBuf));
				snprintf((char*)rateCntBuf,25,"%s%d","rateCnt11ac",index);
				ret = getParams((uint8_t*)rateCntBuf,&data,&len);

				if ( ret == TRUE )
				{
					memcpy(&value,data,len);

					if (i && !(i&1))
						printf("rateCnt %d\n",(value & 0xFFFF));
					else {
						if (i && (i&1)) {
							printf("rateCnt %d\n",(value >>16) & 0xFFFF);
						}
						else{
							printf("rateCnt %d\n",(value & 0xFFFF));
						}
					}
				}
			}

			bit0 = bit0 << 1;i++;
		}
	}
}

/* need to verify */
void parseResNart(void)
{
	int ret = TRUE;
	uint8_t *data;
	uint32_t status, commandId, len;
	uint32_t i;

	ret = getParams((uint8_t*)"commandId", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&commandId,data,len);
		printf("Response commandId %d, len %d\n",commandId, len);
	}
	else
		printf("In parseResNart commandId not found\n");

	ret = getParams((uint8_t*)"status", &data, &len);

	if ( ret == TRUE )
	{
		memcpy(&status,data,len);
		printf("Response status %d\n",status);
	}
	else
		printf("In parseResNart status not found\n");

	memset(gCmd.nart_rsp_buf, 0, sizeof(gCmd.nart_rsp_buf));
	ret = getParams((uint8_t*)"data", &data, &len);

	if ( ret == TRUE )
	{
		if (len > sizeof(gCmd.nart_rsp_buf)) {
			printf("%s: original len as %d and shrink to %u\n", __func__, len, (uint32_t)sizeof(gCmd.nart_rsp_buf));
			len = sizeof(gCmd.nart_rsp_buf);
		}
		memcpy(gCmd.nart_rsp_buf,data,len);
		printf("Response buf len %d\n",len);
		for (i = 0; i <len; i++) {
			printf("0x%2x ",gCmd.nart_rsp_buf[i]);
			if (((i+1)%16) == 0) printf("\n");
		}
		printf("\n");
	}
	else
		printf("In parseResNart buf not found\n");

}

/* need to verify */
void parseResTxStatus(void)
{
	int ret = TRUE;
	uint8_t *data;
	uint32_t pdadc, paCfg, len;
	int32_t  gainIdx, dacGain;
	uint32_t thermCal;
	//    uint8_t  buf[512];
	//    uint32_t i;
	ret = getParams((uint8_t*)"thermCal", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&thermCal,data,len);
		printf("Response thermCal0 %d, lthermCal1 %d, len %d\n", thermCal&0xff, (thermCal)>>8, len);
	}
	else
		printf("In parseResTxStatus thermal not found\n");

	ret = getParams((uint8_t*)"pdadc", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&pdadc,data,len);
		printf("Response pdadc0 %d, pdadc1 %d len %d\n", pdadc&0xff, (pdadc)>>8 , len);
	}
	else
		printf("In parseResTxStatus pdadc not found\n");

	ret = getParams((uint8_t*)"paCfg", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&paCfg,data,len);
		printf("Response paCfg %d, len %d\n", paCfg, len);
	}
	else
		printf("In parseResTxStatus paCfg not found\n");

	ret = getParams((uint8_t*)"gainIdx", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&gainIdx,data,len);
		printf("Response gainIdx %d, len %d\n", gainIdx, len);
	}
	else
		printf("In parseResTxStatus gainIdx not found\n");

	ret = getParams((uint8_t*)"dacGain", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&dacGain,data,len);
		printf("Response dacGain %d, len %d\n", dacGain, len);
	}
	else
		printf("In parseResTxStatus dacGain not found\n");

}

void parseResRxStatus(void)
{
	int ret = TRUE;
	uint8_t *data = NULL;
	uint32_t len = 0;
	uint32_t totalPackets = 0, goodPackets = 0;
	uint32_t otherError = 0, crcPackets = 0;
	int32_t rssi = 0;
	int32_t evm0 = 0;

	ret = getParams((uint8_t*)"totalPackets", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&totalPackets,data,len);
		printf("Response totalPackets %d\n", totalPackets);
	}
	else
		printf("In %s totalPackets not found\n", __func__);

	ret = getParams((uint8_t*)"goodPackets", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&goodPackets,data,len);
		printf("Response goodPackets %d\n", goodPackets);
	}
	else
		printf("In %s goodPackets not found\n", __func__);

	ret = getParams((uint8_t*)"otherError", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&otherError,data,len);
		printf("Response otherError %d\n", otherError);
	}
	else
		printf("In %s otherError not found\n", __func__);

	ret = getParams((uint8_t*)"crcPackets", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&crcPackets,data,len);
		printf("Response crcPackets %d\n", crcPackets);
	}
	else
		printf("In %s crcPackets not found\n", __func__);

	ret = getParams((uint8_t*)"rssi", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&rssi,data,len);
		printf("Response rssi %d\n", rssi);
	}
	else
		printf("In %s rssi not found\n", __func__);

	ret = getParams((uint8_t*)"evm0", &data, &len);
	if ( ret == TRUE )
	{
		memcpy(&evm0,data,len);
		printf("Response evm0 %d\n", evm0);
	}
	else
		printf("In %s evm0 not found\n", __func__);

#if 0 /* 20140311 Matt */
	gCmd.rxPkt = totalPackets;
#else
	gCmd.rxPkt = totalPackets - crcPackets; /* Rx packet should be a good packet */
#endif /* 20140311 Matt */
	gCmd.rxRssi = rssi;
	gCmd.rxCrcError = crcPackets;
	gCmd.rxSecError = 0;

	ALOGE("====RX report====");
	ALOGE("gCmd.rxPkt      = %d", gCmd.rxPkt);
	ALOGE("gCmd.rxRssi     = %d", gCmd.rxRssi);
	ALOGE("gCmd.rxCrcError = %d", gCmd.rxCrcError);
	ALOGE("gCmd.rxSecError = %d", gCmd.rxSecError);
}

#ifdef USE_TLV2
void parseTlv2Response(void *buf, int len)
{
	uint8_t *reply = (uint8_t*)buf;
	uint8_t *pPayload = NULL;
	uint16_t payloadLen = 0;

	ALOGE("\n %s() TLV2 length got %d\n", __func__, len);
	print_hex_dump(buf, len);

	cmdStreamParsingAndProcessing((uint8_t*)&reply[0], len,
					&pPayload, &payloadLen);

	tlv2_enabled = FALSE;
	return;
}
#endif /* USE_TLV2 */

void parseResponse(void *buf, int len)
{
	uint8_t *reply = (uint8_t*)buf;
	uint8_t responseOpCode = _OP_GENERIC_RSP;
	int ret = FALSE;

	addParameterToCommandWithRawdata(buf, len, 1);

	ALOGE("%s() TLV length got %d\n", __func__, len);
	print_hex_dump(buf, len);

	ret = initResponse((uint8_t*)&reply[0], len, &responseOpCode);

	if ( ret == FALSE )
	{
		printf("Error in init Response\n");
		return;
	}

	ALOGE("response code got %d\n", responseOpCode);

	if ( responseOpCode == _OP_SUBMIT_REPORT_RSP )
		parseTLVSubmitReport();
	else if ( responseOpCode == _OP_GENERIC_RSP )
		parseGenericResponse();
	else if ( responseOpCode == _OP_GENERIC_NART_RSP )
		parseResNart();
	else if ( responseOpCode == _OP_TX_STATUS )
		parseResTxStatus();
	else if ( responseOpCode == _OP_RX_STATUS )
		parseResRxStatus();
	else {
		testcmd_error(-1, "Nothing to do with this response\n");
	}

	return;
}

static int doCommand(uint8_t *rCmdStream, uint32_t cmdStreamLen)
{
	int resp = 0;
	char ifname[IFNAMSIZ];

	getIfName(ifname);

	gCmd.errCode = 0;
	gCmd.errString[0] = '\0';
	ALOGE("%s() ifname %s cmdStreamLen %d\n", __func__, ifname,
		cmdStreamLen);

	if (g_tx_init == false) {

#ifdef USE_TLV2
		if (tlv2_enabled == TRUE) {
			if (tcmd_tx_init(ifname, parseTlv2Response)) {
				testcmd_error(-1, "fail to tx_init");
				return -1;
			}
		}
		else
#endif /* USE_TLV2 */
		{
			if (tcmd_tx_init(ifname, parseResponse)) {
				testcmd_error(-1, "fail to tx_init");
				return -1;
			}
		}
		g_tx_init = true;
	}

	resp = isResponseNeeded( (void*)rCmdStream);

	if (tcmd_tx(rCmdStream, cmdStreamLen, resp))
	{
		testcmd_error(-1, "fail to tcmd_tx");
		return -1;
	}

	return 0;
}

int getOffsetFromBitmap(int bitmap) {
	int i;
	for (i=0; i<32; i++) {
		if (bitmap == BIT(i))
			return i;
	}
	return -1;
}

int getMcsDataRate(int wlanMode)
{
	int dataRate;
	int rateOffset = 0;

#define INDEX_OF_MCS0			15
	switch (wlanMode)
	{
		case TCMD_WLAN_MODE_HT20 :
			rateOffset = ATH_RATE_6_5M;
			break;
		case TCMD_WLAN_MODE_HT40PLUS :
		case TCMD_WLAN_MODE_HT40MINUS :
			rateOffset = ATH_RATE_HT40_13_5M;
			break;
		case TCMD_WLAN_MODE_VHT20 :
			if (gCmd.chain == 1 || gCmd.chain == 2)
				rateOffset = ATH_RATE_VHT20_NSS1_MCS0;
			else
				rateOffset = ATH_RATE_VHT20_NSS2_MCS0;
			break;
		case TCMD_WLAN_MODE_VHT40PLUS :
		case TCMD_WLAN_MODE_VHT40MINUS :
			if (gCmd.chain == 1 || gCmd.chain == 2)
				rateOffset = ATH_RATE_VHT40_NSS1_MCS0;
			else
				rateOffset = ATH_RATE_VHT40_NSS2_MCS0;
			break;
		case TCMD_WLAN_MODE_VHT80_0 :
		case TCMD_WLAN_MODE_VHT80_1 :
		case TCMD_WLAN_MODE_VHT80_2 :
		case TCMD_WLAN_MODE_VHT80_3 :
			if (gCmd.chain == 1 || gCmd.chain == 2)
				rateOffset = ATH_RATE_VHT80_NSS1_MCS0;
			else
				rateOffset = ATH_RATE_VHT80_NSS2_MCS0;
			break;
		default :
			ALOGE("Wrong wlanMode %d value", wlanMode);
			break;
	}

	dataRate = rateOffset + gCmd.rate - INDEX_OF_MCS0;

	return dataRate;
}

int getMcsWlanMode(void)
{
	int wlanMode = 0;

	if (gCmd.wmode != TCMD_WLAN_MODE_MAX)
		return gCmd.wmode;

	switch (gCmd.bandWidth)
	{
		case 0:
			wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 1 :
			wlanMode = TCMD_WLAN_MODE_HT20;
			break;
		case 2 :
			wlanMode = TCMD_WLAN_MODE_HT40PLUS;
			break;
		case 3 :
			wlanMode = TCMD_WLAN_MODE_VHT80_0;
			break;
		case 4 :
			wlanMode = TCMD_WLAN_MODE_VHT20;
			break;
		case 5 :
			wlanMode = TCMD_WLAN_MODE_VHT40PLUS;
			break;
		default :
			ALOGE("Wrong bandwidth %d value", gCmd.bandWidth);
			break;
	}
	return wlanMode;
}

void getDataRateAndWlanMode(uint32_t *dataRate, uint32_t *wlanMode)
{
	switch (gCmd.rate)
	{
		// CCK
		case 1 : // 1Mbps
			*dataRate = ATH_RATE_1M;
			*wlanMode = TCMD_WLAN_MODE_CCK;
			break;
		case 2 : // 2Mbps
			*dataRate = ATH_RATE_2M;
			if (!gCmd.longpreamble)
				*dataRate += (TCMD_MAX_RATES - 4);/* use last three rates in the rate table for shortPreamble */
			*wlanMode = TCMD_WLAN_MODE_CCK;
			break;
		case 3 : // 5.5Mbps
			*dataRate = ATH_RATE_5_5M;
			if (!gCmd.longpreamble)
				*dataRate += (TCMD_MAX_RATES - 4);/* use last three rates in the rate table for shortPreamble */
			*wlanMode = TCMD_WLAN_MODE_CCK;
			break;
		case 6 : // 11Mbps
			*dataRate = ATH_RATE_11M;
			if (!gCmd.longpreamble)
				*dataRate += (TCMD_MAX_RATES - 4);/* use last three rates in the rate table for shortPreamble */
			*wlanMode = TCMD_WLAN_MODE_CCK;
			break;

		// NOHT
		case 4 : // 6Mbps
			*dataRate = ATH_RATE_6M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 5 : // 9Mbps
			*dataRate = ATH_RATE_9M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 7 : // 12Mbps
			*dataRate = ATH_RATE_12M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 8 : // 18Mbps
			*dataRate = ATH_RATE_18M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 10 : // 24Mbps
			*dataRate = ATH_RATE_24M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 12 : // 36Mbps
			*dataRate = ATH_RATE_36M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 13 : // 48Mbps
			*dataRate = ATH_RATE_48M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;
		case 14 : // 54Mbps
			*dataRate = ATH_RATE_54M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
			break;

		// MCS 0~9
		case 15 : // MCS0
		case 16 : // MCS1
		case 17 : // MCS2
		case 18 : // MCS3
		case 19 : // MCS4
		case 20 : // MCS5
		case 21 : // MCS6
		case 22 : // MCS7
		case 23 : // MCS8
		case 24 : // MCS9
		case 25 : // MCS10
		case 26 : // MCS11
		case 27 : // MCS12
		case 28 : // MCS13
		case 29 : // MCS14
		case 30 : // MCS15
			*wlanMode = getMcsWlanMode();
			*dataRate = getMcsDataRate(*wlanMode);
			break;
		default:
			//  0: not used
			//  9: 22Mbps (not supported)
			// 11: 33Mbps (not supported)
#if 1 /* Use 54Mbps for default since Agilent N4010 doesn't support Rate Set command for Rx start */
			*dataRate = ATH_RATE_54M;
			*wlanMode = TCMD_WLAN_MODE_NOHT;
#endif
			break;
	}

	return;
}

int findCenterFreq(int wlanMode)
{
	int freq = 0;
	switch (wlanMode)
	{
		case TCMD_WLAN_MODE_NOHT:
		case TCMD_WLAN_MODE_HT20:
		case TCMD_WLAN_MODE_CCK:
		case TCMD_WLAN_MODE_VHT20:
			freq = gCmd.freq;
			ALOGE("%s() no need to switch to bonding channel", __func__);
			break;
		case TCMD_WLAN_MODE_HT40PLUS:
		case TCMD_WLAN_MODE_HT40MINUS:
		case TCMD_WLAN_MODE_VHT40PLUS:
		case TCMD_WLAN_MODE_VHT40MINUS:
			if (gCmd.freq == 5180 || gCmd.freq == 5200) {
				freq = 5190;
			} else if (gCmd.freq == 5220 || gCmd.freq == 5240) {
				freq = 5230;
			} else if (gCmd.freq == 5260 || gCmd.freq == 5280) {
				freq = 5270;
			} else if (gCmd.freq == 5300 || gCmd.freq == 5320) {
				freq = 5310;
			} else if (gCmd.freq == 5500 || gCmd.freq == 5520) {
				freq = 5510;
			} else if (gCmd.freq == 5540 || gCmd.freq == 5560) {
				freq = 5550;
			} else if (gCmd.freq == 5580 || gCmd.freq == 5600) {
				freq = 5590;
			} else if (gCmd.freq == 5620 || gCmd.freq == 5640) {
				freq = 5630;
			} else if (gCmd.freq == 5660 || gCmd.freq == 5680) {
				freq = 5670;
			} else if (gCmd.freq == 5700 || gCmd.freq == 5720) {
				freq = 5710;
			} else if (gCmd.freq == 5745 || gCmd.freq == 5765) {
				freq = 5755;
			} else if (gCmd.freq == 5785 || gCmd.freq == 5805) {
				freq = 5795;
			} else {
				ALOGE("%s() freq %d doesn't in wlan mode %d.", __func__, gCmd.freq, wlanMode);
				freq = gCmd.freq;
				break;
			}
			ALOGE("%s() changed freq from %d to %d", __func__, gCmd.freq, freq);
			break;
		case TCMD_WLAN_MODE_VHT80_0:
		case TCMD_WLAN_MODE_VHT80_1:
		case TCMD_WLAN_MODE_VHT80_2:
		case TCMD_WLAN_MODE_VHT80_3:
			if (gCmd.freq == 5180 || gCmd.freq == 5200 || gCmd.freq == 5220 || gCmd.freq == 5240) {
				freq = 5210;
			} else if (gCmd.freq == 5260 || gCmd.freq == 5280 || gCmd.freq == 5300 || gCmd.freq == 5320) {
				freq = 5290;
			} else if (gCmd.freq == 5500 || gCmd.freq == 5520 || gCmd.freq == 5540 || gCmd.freq == 5560) {
				freq = 5530;
			} else if (gCmd.freq == 5580 || gCmd.freq == 5600 || gCmd.freq == 5620 || gCmd.freq == 5640) {
				freq = 5610;
			} else if (gCmd.freq == 5660 || gCmd.freq == 5680 || gCmd.freq == 5700 || gCmd.freq == 5720) {
				freq = 5690;
			} else if (gCmd.freq == 5745 || gCmd.freq == 5765 || gCmd.freq == 5785 || gCmd.freq == 5805) {
				freq = 5775;
			} else {
				ALOGE("%s() freq %d doesn't in wlan mode %d.", __func__, gCmd.freq, wlanMode);
				freq = gCmd.freq;
				break;
			}
			ALOGE("%s() changed freq from %d to %d", __func__, gCmd.freq, freq);
			break;
		default:
			freq = gCmd.freq;
			ALOGE("%s() wlanMode %d is not supported!!", __func__, wlanMode);
			break;
	}
	return freq;
}

void rateIndexToArrayMapping(uint32_t rateIndex, uint32_t *rowIndex, uint32_t *bitmask, int *is11AcRate )
{
	*is11AcRate = FALSE;

	if ((rateIndex == ATH_RATE_CCK_2M_S) || (rateIndex==ATH_RATE_CCK_5_5M_S)
	     || (rateIndex == ATH_RATE_CCK_11M_S) ||
		((rateIndex >= ATH_RATE_HT20_MCS24) &&
		 (rateIndex <= ATH_RATE_HT20_MCS31)) ||
		((rateIndex >= ATH_RATE_HT40_MCS24) &&
		 (rateIndex <= ATH_RATE_HT40_MCS31)))
	{
		*is11AcRate = FALSE;
	}
	else if (rateIndex >= 60)
		*is11AcRate = TRUE;
#ifdef TCMD_LIB_DEBUG
	printf("%d %s Is11AcRate %d\n", rateIndex, bgRateStrTbl[rateIndex], *is11AcRate);
#endif
	ALOGE("%s() %d %s Is11AcRate %d\n", __func__, rateIndex, bgRateStrTbl[rateIndex], *is11AcRate);
	switch(rateIndex)
	{
		case 0: // 1 Mb
			*rowIndex = 0;
			*bitmask = 0x00000001; // set Bit 0
			break;
		case 1: // 2 Mb Long CCK
			*rowIndex = 0;
			*bitmask = 0x00000002; // set Bit 1
			break;
		case 150: // 2 Mb Short CCK
			*rowIndex = 0;
			*bitmask = 0x00000004; // set Bit 2 (SHORT CCK)
			break;
		case 2: // 5.5 Mb Long CCK
			*rowIndex = 0;
			*bitmask = 0x00000008; // set Bit 3
			break;
		case 151: // 5.5 Mb Short CCK
			*rowIndex = 0;
			*bitmask = 0x00000010; // set Bit 4 (SHORT CCK)
			break;
		case 3: // 11 Mb Long CCK
			*rowIndex = 0;
			*bitmask = 0x00000020; // set Bit 5
			break;
		case 152: // 11 Mb Short CCK
			*rowIndex = 0;
			*bitmask = 0x00000040; // set Bit 6 (SHORT CCK)
			break;

		case 4: // 6 Mb
			*rowIndex = 0;
			*bitmask = 0x00000100;
			break;
		case 5: // 9 Mb
			*rowIndex = 0;
			*bitmask = 0x00000200;
			break;
		case 6: // 12 Mb
			*rowIndex = 0;
			*bitmask = 0x00000400;
			break;
		case 7: // 18 Mb
			*rowIndex = 0;
			*bitmask = 0x00000800;
			break;
		case 8: // 24 Mb
			*rowIndex = 0;
			*bitmask = 0x00001000;
			break;
		case 9: // 36 Mb
			*rowIndex = 0;
			*bitmask = 0x00002000;
			break;
		case 10: // 48 Mb
			*rowIndex = 0;
			*bitmask = 0x00004000;
			break;
		case 11: // 54 Mb
			*rowIndex = 0;
			*bitmask = 0x00008000;
			break;

		case 12: // HT20 MCS0
			*rowIndex = 0;
			*bitmask = 0x00010000;
			break;
		case 13: // HT20 MCS1
			*rowIndex = 0;
			*bitmask = 0x00020000;
			break;
		case 14: // HT20 MCS2
			*rowIndex = 0;
			*bitmask = 0x00040000;
			break;
		case 15: // HT20 MCS3
			*rowIndex = 0;
			*bitmask = 0x00080000;
			break;
		case 16: // HT20 MCS4
			*rowIndex = 0;
			*bitmask = 0x00100000;
			break;
		case 17: // HT20 MCS5
			*rowIndex = 0;
			*bitmask = 0x00200000;
			break;
		case 18: // HT20 MCS6
			*rowIndex = 0;
			*bitmask = 0x00400000;
			break;
		case 19: // HT20 MCS7
			*rowIndex = 0;
			*bitmask = 0x00800000;
			break;

		case 20: // HT20 MCS8
			*rowIndex = 1;
			*bitmask = 0x00000001;
			break;
		case 21: // HT20 MCS9
			*rowIndex = 1;
			*bitmask = 0x00000002;
			break;
		case 22: // HT20 MCS10
			*rowIndex = 1;
			*bitmask = 0x00000004;
			break;
		case 23: // HT20 MCS11
			*rowIndex = 1;
			*bitmask = 0x00000008;
			break;
		case 24: // HT20 MCS12
			*rowIndex = 1;
			*bitmask = 0x00000010;
			break;
		case 25: // HT20 MCS13
			*rowIndex = 1;
			*bitmask = 0x00000020;
			break;
		case 26: // HT20 MCS14
			*rowIndex = 1;
			*bitmask = 0x00000040;
			break;
		case 27: // HT20 MCS15
			*rowIndex = 1;
			*bitmask = 0x00000080;
			break;

		case 28: // HT20 MCS16
			*rowIndex = 1;
			*bitmask = 0x00010000;
			break;
		case 29: // HT20 MCS17
			*rowIndex = 1;
			*bitmask = 0x00020000;
			break;
		case 30: // HT20 MCS18
			*rowIndex = 1;
			*bitmask = 0x00040000;
			break;
		case 31: // HT20 MCS19
			*rowIndex = 1;
			*bitmask = 0x00080000;
			break;
		case 32: // HT20 MCS20
			*rowIndex = 1;
			*bitmask = 0x00100000;
			break;
		case 33: // HT20 MCS21
			*rowIndex = 1;
			*bitmask = 0x00200000;
			break;
		case 34: // HT20 MCS22
			*rowIndex = 1;
			*bitmask = 0x00400000;
			break;
		case 35: // HT20 MCS23
			*rowIndex = 1;
			*bitmask = 0x00800000;
			break;

		case 36: // HT40 MCS0
			*rowIndex = 0;
			*bitmask = 0x01000000;
			break;
		case 37: // HT40 MCS1
			*rowIndex = 0;
			*bitmask = 0x02000000;
			break;
		case 38: // HT40 MCS2
			*rowIndex = 0;
			*bitmask = 0x04000000;
			break;
		case 39: // HT40 MCS3
			*rowIndex = 0;
			*bitmask = 0x08000000;
			break;
		case 40: // HT40 MCS4
			*rowIndex = 0;
			*bitmask = 0x10000000;
			break;
		case 41: // HT40 MCS5
			*rowIndex = 0;
			*bitmask = 0x20000000;
			break;
		case 42: // HT40 MCS6
			*rowIndex = 0;
			*bitmask = 0x40000000;
			break;
		case 43: // HT40 MCS7 135
			*rowIndex = 0;
			*bitmask = 0x80000000;
			break;

		case 44: // HT40 MCS8
			*rowIndex = 1;
			*bitmask = 0x00000100;
			break;
		case 45: // HT40 MCS9
			*rowIndex = 1;
			*bitmask = 0x00000200;
			break;
		case 46: // HT40 MCS10
			*rowIndex = 1;
			*bitmask = 0x00000400;
			break;
		case 47: // HT40 MCS11
			*rowIndex = 1;
			*bitmask = 0x00000800;
			break;
		case 48: // HT40 MCS12
			*rowIndex = 1;
			*bitmask = 0x00001000;
			break;
		case 49: // HT40 MCS13
			*rowIndex = 1;
			*bitmask = 0x00002000;
			break;
		case 50: // HT40 MCS14
			*rowIndex = 1;
			*bitmask = 0x00004000;
			break;
		case 51: // HT40 MCS15
			*rowIndex = 1;
			*bitmask = 0x00008000;
			break;

		case 52: // HT40 MCS16
			*rowIndex = 1;
			*bitmask = 0x01000000;
			break;
		case 53: // HT40 MCS17
			*rowIndex = 1;
			*bitmask = 0x02000000;
			break;
		case 54: // HT40 MCS18
			*rowIndex = 1;
			*bitmask = 0x04000000;
			break;
		case 55: // HT40 MCS19
			*rowIndex = 1;
			*bitmask = 0x08000000;
			break;
		case 56: // HT40 MCS20
			*rowIndex = 1;
			*bitmask = 0x10000000;
			break;
		case 57: // HT40 MCS21
			*rowIndex = 1;
			*bitmask = 0x20000000;
			break;
		case 58: // HT40 MCS22
			*rowIndex = 1;
			*bitmask = 0x40000000;
			break;
		case 59: // HT40 MCS23
			*rowIndex = 1;
			*bitmask = 0x80000000;
			break;

		case 60: // VHT20 MCS0 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000001;
			break;
		case 61: // VHT20 MCS1 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000002;
			break;
		case 62: // VHT20 MCS2 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000004;
			break;
		case 63: // VHT20 MCS3 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000008;
			break;
		case 64: // VHT20 MCS4 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000010;
			break;
		case 65: // VHT20 MCS5 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000020;
			break;
		case 66: // VHT20 MCS6 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000040;
			break;
		case 67: // VHT20 MCS7 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000080;
			break;
		case 68: // VHT20 MCS8 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00000100;
			break;
		case 69: // VHT20 MCS9 S1
			*rowIndex = 0;
			*bitmask = 0x00000200;
			break;

		case 70: // VHT20 MCS0 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000010;
			break;
		case 71: // VHT20 MCS1 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000020;
			break;
		case 72: // VHT20 MCS2 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000040;
			break;
		case 73: // VHT20 MCS3 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000080;
			break;
		case 74: // VHT20 MCS4 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000100;
			break;
		case 75: // VHT20 MCS5 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000200;
			break;
		case 76: // VHT20 MCS6 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000400;
			break;
		case 77: // VHT20 MCS7 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000800;
			break;
		case 78: // VHT20 MCS8 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00001000;
			break;
		case 79: // VHT20 MCS9 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00002000;
			break;

		case 80: // VHT20 MCS0 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000100;
			break;
		case 81: // VHT20 MCS1 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000200;
			break;
		case 82: // VHT20 MCS2 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000400;
			break;
		case 83: // VHT20 MCS3 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000800;
			break;
		case 84: // VHT20 MCS4 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00001000;
			break;
		case 85: // VHT20 MCS5 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00002000;
			break;
		case 86: // VHT20 MCS6 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00004000;
			break;
		case 87: // VHT20 MCS7 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00008000;
			break;
		case 88: // VHT20 MCS8 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00010000;
			break;
		case 89: // VHT20 MCS9 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00020000;
			break;

		case 90: // VHT40 MCS0 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00001000;
			break;
		case 91: // VHT40 MCS1 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00002000;
			break;
		case 92: // VHT40 MCS2 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00004000;
			break;
		case 93: // VHT40 MCS3 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00008000;
			break;
		case 94: // VHT40 MCS4 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00010000;
			break;
		case 95: // VHT40 MCS5 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00020000;
			break;
		case 96: // VHT40 MCS6 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00040000;
			break;
		case 97: // VHT40 MCS7 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00080000;
			break;
		case 98: // VHT40 MCS8 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00100000;
			break;
		case 99: // VHT40 MCS9 S1
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x00200000;
			break;

		case 100: // VHT40 MCS0 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00010000;
			break;
		case 101: // VHT40 MCS1 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00020000;
			break;
		case 102: // VHT40 MCS2 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00040000;
			break;
		case 103: // VHT40 MCS3 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00080000;
			break;
		case 104: // VHT40 MCS4 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00100000;
			break;
		case 105: // VHT40 MCS5 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00200000;
			break;
		case 106: // VHT40 MCS6 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00400000;
			break;
		case 107: // VHT40 MCS7 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00800000;
			break;
		case 108: // VHT40 MCS8 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x01000000;
			break;
		case 109: // VHT40 MCS9 S2
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x02000000;
			break;

		case 110: // VHT40 MCS0 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00100000;
			break;
		case 111: // VHT40 MCS1 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00200000;
			break;
		case 112: // VHT40 MCS2 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00400000;
			break;
		case 113: // VHT40 MCS3 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00800000;
			break;
		case 114: // VHT40 MCS4 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x01000000;
			break;
		case 115: // VHT40 MCS5 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x02000000;
			break;
		case 116: // VHT40 MCS6 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x04000000;
			break;
		case 117: // VHT40 MCS7 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x08000000;
			break;
		case 118: // VHT40 MCS8 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x10000000;
			break;
		case 119: // VHT40 MCS9 S3
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x20000000;
			break;

		case 120: // VHT80 MCS0 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x01000000;
			break;
		case 121: // VHT80 MCS1 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x02000000;
			break;
		case 122: // VHT80 MCS2 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x04000000;
			break;
		case 123: // VHT80 MCS3 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x08000000;
			break;
		case 124: // VHT80 MCS4 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x10000000;
			break;
		case 125: // VHT80 MCS5 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x20000000;
			break;
		case 126: // VHT80 MCS6 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x40000000;
			break;
		case 127: // VHT80 MCS7 S0
			*rowIndex = 2;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 0;
			*bitmask = 0x80000000;
			break;
		case 128: // VHT80 MCS8 S0
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000001;
			break;
		case 129: // VHT80 MCS9 S0
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x00000002;
			break;

		case 130: // VHT80 MCS0 S1
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x10000000;
			break;
		case 131: // VHT80 MCS1 S1
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x20000000;
			break;
		case 132: // VHT80 MCS2 S1
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x40000000;
			break;
		case 133: // VHT80 MCS3 S1
			*rowIndex = 3;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 1;
			*bitmask = 0x80000000;
			break;
		case 134: // VHT80 MCS4 S1
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000001;
			break;
		case 135: // VHT80 MCS5 S1
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000002;
			break;
		case 136: // VHT80 MCS6 S1
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000004;
			break;
		case 137: // VHT80 MCS7 S1
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000008;
			break;
		case 138: // VHT80 MCS8 S1
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000010;
			break;
		case 139: // VHT80 MCS9 S1
			*rowIndex = 4;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 2;
			*bitmask = 0x00000020;
			break;

		case 140: // VHT80 MCS0 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000001;
			break;
		case 141: // VHT80 MCS1 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000002;
			break;
		case 142: // VHT80 MCS2 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000004;
			break;
		case 143: // VHT80 MCS3 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000008;
			break;
		case 144: // VHT80 MCS4 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000010;
			break;
		case 145: // VHT80 MCS5 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000020;
			break;
		case 146: // VHT80 MCS6 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000040;
			break;
		case 147: // VHT80 MCS7 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000080;
			break;
		case 148: // VHT80 MCS8 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000100;
			break;
		case 149: // VHT80 MCS9 S3
			*rowIndex = 5;
			if (tlv2_rx_vht_rates == TRUE)
				*rowIndex = 3;
			*bitmask = 0x00000200;
			break;
		case 153: // VHT20 MCS0 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000001; // set Bit
			break;
		case 154: // VHT20 MCS1 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000002; // set Bit
			break;
		case 155: // VHT20 MCS2 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000004; // set Bit
			break;
		case 156: // VHT20 MCS3 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000008; // set Bit
			break;
		case 157: // VHT20 MCS4 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000010; // set Bit
			break;
		case 158: // VHT20 MCS5 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000020; // set Bit
			break;
		case 159: // VHT20 MCS6 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000040; // set Bit
			break;
		case 160: // VHT20 MCS7 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000080; // set Bit
			break;
		case 161: // VHT20 MCS8 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000100; // set Bit
			break;
		case 162: // VHT20 MCS9 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000200; // set Bit
			break;
		case 163: // VHT40 MCS0 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000400; // set Bit
			break;
		case 164: // VHT40 MCS1 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00000800; // set Bit
			break;
		case 165: // VHT40 MCS2 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00001000; // set Bit
			break;
		case 166: // VHT40 MCS3 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00002000; // set Bit
			break;
		case 167: // VHT40 MCS4 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00004000; // set Bit
			break;
		case 168: // VHT40 MCS5 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00008000; // set Bit
			break;
		case 169: // VHT40 MCS6 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00010000; // set Bit
			break;
		case 170: // VHT40 MCS7 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00020000; // set Bit
			break;
		case 171: // VHT40 MCS8 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00040000; // set Bit
			break;
		case 172: // VHT40 MCS9 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00080000; // set Bit
			break;
		case 173: // VHT80 MCS0 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00100000; // set Bit
			break;
		case 174: // VHT80 MCS1 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00200000; // set Bit
			break;
		case 175: // VHT80 MCS2 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00400000; // set Bit
			break;
		case 176: // VHT80 MCS3 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x00800000; // set Bit
			break;
		case 177: // VHT80 MCS4 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x01000000; // set Bit
			break;
		case 178: // VHT80 MCS5 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x02000000; // set Bit
			break;
		case 179: // VHT80 MCS6 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x04000000; // set Bit
			break;
		case 180: // VHT80 MCS7 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x08000000; // set Bit
			break;
		case 181: // VHT80 MCS8 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x10000000; // set Bit
			break;
		case 182: // VHT80 MCS9 S4
			*rowIndex = 0;         // row Index 0
			*bitmask = 0x20000000; // set Bit
			break;
		// cascade
		case 183: // VHT160 MCS0 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000001; // set Bit
			break;
		case 184: // VHT160 MCS1 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000002; // set Bit
			break;
		case 185: // VHT160 MCS2 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000004; // set Bit
			break;
		case 186: // VHT160 MCS3 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000008; // set Bit
			break;
		case 187: // VHT160 MCS4 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000010; // set Bit
			break;
		case 188: // VHT160 MCS5 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000020; // set Bit
			break;
		case 189: // VHT160 MCS6 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000040; // set Bit
			break;
		case 190: // VHT160 MCS7 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000080; // set Bit
			break;
		case 191: // VHT160 MCS8 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000100; // set Bit
			break;
		case 192: // VHT160 MCS9 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x000000200; // set Bit
			break;
		case 193: // VHT160 MCS0 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000400; // set Bit
			break;
		case 194: // VHT160 MCS1 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00000800; // set Bit
			break;
		case 195: // VHT160 MCS2 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00001000; // set Bit
			break;
		case 196: // VHT160 MCS3 S1
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00002000; // set Bit
			break;
		case 197: // VHT160 MCS4 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00004000; // set Bit
			break;
		case 198: // VHT160 MCS5 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00008000; // set Bit
			break;
		case 199: // VHT160 MCS6 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00010000; // set Bit
			break;
		case 200: // VHT160 MCS7 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00020000; // set Bit
			break;
		case 201: // VHT160 MCS8 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00040000; // set Bit
			break;
		case 202: // VHT160 MCS9 S2
			*rowIndex = 7;                // row Index 0
			*bitmask = 0x00080000; // set Bit
			break;
	}
}

//
// PUBLIC FUNCTIONS
//
void qca6174Check(void)
{
	if (gCmd.freq == 0 ) {
		gCmd.freq = 2412;
	}
	qca6174TxStop();
	qca6174RxPacketStop();
}

void testcmd_error(int code, const char *fmt, ...)
{
	gCmd.errCode = code;
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(gCmd.errString, sizeof(gCmd.errString)-1, fmt, ap);
	va_end(ap);

	ALOGE("%s", gCmd.errString);
}

int qca6174ApiInit(void)
{
	memset(&gCmd, 0, sizeof(gCmd));

	gCmd.txPwr = 0;
	gCmd.pktLen0 = 1500;
	gCmd.aifs = 1;

//	optarg = NULL;
//	optind = opterr = 1;
//	optopt = '?';
	return 0;
}

void qca6174ApiCleanup(void)
{
	qca6174Check();
	memset(&gCmd, 0, sizeof(gCmd));
}

void qca6174ChannelSet(int channel)
{
	gCmd.freq = channel_to_freq(channel);
}

void qca6174FreqSet(uint32_t freq)
{
	gCmd.freq = freq;
}

void qca6174Freq2Set(uint32_t freq)
{
	gCmd.freq2 = freq;
}

void qca6174ShortGuardSet(int enable)
{
	gCmd.shortguard = enable ? 1 : 0;
}

void qca6174RateSet(int rate)
{
	gCmd.rate = rate;
}

void qca6174TxPowerSet(char *val)
{
	gCmd.txPwr = val ? strtod(val, NULL) : 0;
}

void qca6174TxPcmSet(int txpwr)
{
	gCmd.tpcm = txpwr;
}

void qca6174TxPacketSizeSet(int size)
{
	gCmd.pktLen0 = size;
}

void qca6174SetPaCfg(int val)
{
	gCmd.paConfig = val;
}

void qca6174SetDacGain(int val)
{
	gCmd.dacGain = val;
}

void qca6174SetGainIdx(int val)
{
	gCmd.gainIdx = val;
}

void qca6174SetNumPkt(int val)
{
	gCmd.numPkt = val;
}

void qca6174SetAgg(int val)
{
	gCmd.agg = val;
}

void qca6174SetStbc(int val)
{
	gCmd.stbc = val;
}

void qca6174SetLdpc(int val)
{
	gCmd.ldpc = val;
}

void qca6174SetWlanMode(int val)
{
	gCmd.wmode = val;
}

void qca6174SetLPreamble()
{
	gCmd.longpreamble = true;
}

void qca6174init()
{
	gCmd.wmode = TCMD_WLAN_MODE_MAX;
}

void qca6174SetPhyid(int val)
{
	gCmd.phyid = val;
}

#ifdef NOT_USE_CLITLVCMD_IF_MODULE
int qca6174TxCommand(const char *txtype)
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	uint32_t miscFlags = PROCESS_RATE_IN_ORDER_MASK | TX_STATUS_PER_RATE_MASK;
	uint32_t txMode, value;
	uint8_t mac[ATH_MAC_LEN];
	uint32_t rowIndex = 0, rateBitMask = 0;
	int is11ACRate = FALSE;
	uint32_t dataRate = 0, wlanMode = 0;


	getDataRateAndWlanMode(&dataRate, &wlanMode);

	if (gCmd.freq == 0) {
		gCmd.freq = 2412;
	}

	createCommand(_OP_TX);
	ALOGE("%s() opcode _OP_TX", __func__);

	if (gCmd.broadcast != 1){
		addParameterToCommand("broadcast",(uint8_t *)&gCmd.broadcast, sizeof(gCmd.broadcast));
		ALOGE("\n%s() adding command(broadcast, %d)\n", __func__, gCmd.broadcast);
	}


	if (is_zero_ether_addr(gCmd.bssid)) {
		ath_ether_aton(DEFAULT_TX_BSSID, mac);
		addParameterToCommand("bssid",(uint8_t *)mac, sizeof(mac));
		ALOGE("\n%s() adding command(bssid, %x:%x:%x:%x:%x:%x)\n", __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	else {
		addParameterToCommand("bssid",(uint8_t *)gCmd.bssid, sizeof(gCmd.bssid));
		ALOGE("\n%s() adding command(bssid, %x:%x:%x:%x:%x:%x)\n", __func__, gCmd.bssid[0], gCmd.bssid[1], gCmd.bssid[2], gCmd.bssid[3], gCmd.bssid[4], gCmd.bssid[5]);
	}

	if (is_zero_ether_addr(gCmd.txStation)) {
		ath_ether_aton(DEFAULT_TX_TXSTATION, mac);
		addParameterToCommand("txStation",(uint8_t *)mac,sizeof(mac));
		ALOGE("\n%s() adding command(txStation, %x:%x:%x:%x:%x:%x)\n", __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	else{
		addParameterToCommand("txStation",(uint8_t *)gCmd.txStation, sizeof(gCmd.txStation));
		ALOGE("\n%s() adding command(txStation, %x:%x:%x:%x:%x:%x)\n", __func__, gCmd.txStation[0], gCmd.txStation[1], gCmd.txStation[2], gCmd.txStation[3], gCmd.txStation[4], gCmd.txStation[5]);

	}

	if (is_zero_ether_addr(gCmd.rxStation)) {
		ath_ether_aton(DEFAULT_TX_RXSTATION, mac);
		addParameterToCommand("rxStation",(uint8_t *)mac, sizeof(mac));
		ALOGE("\n%s() adding command(rxStation, %x:%x:%x:%x:%x:%x)\n", __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	}
	else{
		addParameterToCommand("rxStation",(uint8_t *)gCmd.rxStation, sizeof(gCmd.rxStation));
		ALOGE("\n%s() adding command(rxStation, %x:%x:%x:%x:%x:%x)\n", __func__, gCmd.rxStation[0], gCmd.rxStation[1], gCmd.rxStation[2], gCmd.rxStation[3], gCmd.rxStation[4], gCmd.rxStation[5]);
	}

//	addParameterToCommand((uint8_t *)"tpcm",(uint8_t *)&gCmd.tpcm);
	addParameterToCommand("flags",(uint8_t *)&miscFlags, sizeof(miscFlags));
      ALOGE("\n%s() adding command(flags, %d)\n", __func__, miscFlags);

	if (wlanMode != TCMD_WLAN_MODE_HT20){
		addParameterToCommand("wlanMode",(uint8_t *)&wlanMode, sizeof(wlanMode));
		ALOGE("\n%s() adding command(wlanMode, %d)\n", __func__, wlanMode);
	}

	if (!strcmp(txtype, "sine")) {
		txMode = TCMD_CONT_TX_SINE;
	} else if (!strcmp(txtype, "frame")) {
		txMode = TCMD_CONT_TX_FRAME;
	} else if (!strcmp(txtype, "tx99")) {
		txMode = TCMD_CONT_TX_TX99;
	} else if (!strcmp(txtype, "tx100")) {
		txMode = TCMD_CONT_TX_TX100;
	} else if (!strcmp(txtype, "cwtone")) {
		txMode = TCMD_CONT_TX_CWTONE;
	} else if (!strcmp(txtype, "clpcpkt")) {
		txMode = TCMD_CONT_TX_CLPCPKT;
	} else if (!strcmp(txtype, "off")) {
		txMode = TCMD_CONT_TX_OFF;
	} else {
		ALOGE("%s():%d Error wrong txMode %s\n", __func__, __LINE__, txtype);
	}

	addParameterToCommand("txMode",(uint8_t *)&txMode, sizeof(txMode));
	ALOGE("\n%s() adding command(txMode, %d)\n", __func__, txMode);

	value = PN9_PATTERN;
	addParameterToCommand("txPattern",(uint8_t *)&value, sizeof(value));
	ALOGE("\n%s() adding command(txPattern, %d)\n", __func__, value);

	if (gCmd.aifs){
		addParameterToCommand("aifsn",(uint8_t *)&gCmd.aifs, sizeof(gCmd.aifs));
		ALOGE("\n%s() adding command(aifsn, %d)\n", __func__, gCmd.aifs);
	}

	rateIndexToArrayMapping(dataRate, &rowIndex, &rateBitMask, &is11ACRate);
	value = rowIndex*32 + getOffsetFromBitmap(rateBitMask);
#ifdef TCMD_LIB_DEBUG
	printf("dataRate %d rateIndex %d \n", dataRate, value);
#endif
	addParameterToCommand("rateBitIndex0",(uint8_t *)&value, sizeof(value));
      printf("dataRate %d rateIndex %d \n", dataRate, value);

	if (gCmd.shortguard) {
		value = 1;
		addParameterToCommand("shortGuard",(uint8_t *)&value, sizeof(value));
		ALOGE("\n%s() adding command(shortGuard, %d)\n", __func__, value);
	}

	if (!gCmd.pktLen0)
		gCmd.pktLen0 = 1500;
	addParameterToCommand("pktLen0",(uint8_t *)&gCmd.pktLen0, sizeof(gCmd.pktLen0));
	ALOGE("\n%s() adding command(pktLen0, %d)\n", __func__, gCmd.pktLen0);
	addParameterToCommand("txChain0",(uint8_t *)&gCmd.chain, sizeof(gCmd.chain));
	ALOGE("\n%s() adding command(txChain0, %d)\n", __func__, gCmd.chain);

	value = 1;
	addParameterToCommand("ir",(uint8_t *)&value, sizeof(value));
	ALOGE("\n%s() adding command(ir, %d)\n", __func__, value);

	value = gCmd.txPwr * 2;
	addParameterToCommand("txPower0",(uint8_t *)&value, sizeof(value));
	ALOGE("\n%s() adding command(txPower0, %d)\n", __func__, value);

	value = 0;
	addParameterToCommand("paConfig",(uint8_t *)&value, sizeof(value));
	ALOGE("\n%s() adding command(paConfig, %d)\n", __func__, value);

	commandComplete(&rCmdStream, &cmdStreamLen);
	print_hex_dump(rCmdStream, cmdStreamLen);

	doCommand(rCmdStream, cmdStreamLen);

	if (gCmd.errCode == 0) {
		gCmd.isTxStart = 1;

	}
	else {
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
	}

	return (gCmd.errCode == 0) ? 0 : -1;
}
#else

#ifdef USE_TLV2
void handleTXSTATUSRSP (void *parms)
{
	CMD_TXSTATUSRSP_PARMS *pParms = (CMD_TXSTATUSRSP_PARMS *)parms;

	if (pParms->status != 0)
	{
		printf("TXSTATUS returns error %d\n", pParms->status);
		return;
	}
	printf("numOfReports = %u\n", pParms->numOfReports);
	printf("totalPackets = %u\n", pParms->totalPackets);
	printf("goodPackets = %u\n", pParms->goodPackets);
	printf("underruns = %u\n", pParms->underruns);
	printf("otherError = %u\n", pParms->otherError);
	printf("rateBit = %u\n", pParms->rateBitIndex);
	printf("startTime = %u\n", pParms->startTime);
	printf("endTime = %u\n", pParms->endTime);
	printf("byteCount = %u\n", pParms->byteCount);
	printf("dontCount = %u\n", pParms->dontCount);
	printf("rssi = %d\n", pParms->rssi);
	printf("rssic = %d, %d, %d, %d\n", pParms->rssic[0],
		pParms->rssic[1], pParms->rssic[2], pParms->rssic[3] );
	printf("rssie = %d, %d, %d, %d\n", pParms->rssie[0],
		pParms->rssie[1], pParms->rssie[2], pParms->rssie[3]);
}

void handleBASICRSP (void *parms)
{
	CMD_BASICRSP_PARMS *pParms = (CMD_BASICRSP_PARMS *)parms;
	printf("Response cmdId: %d\n", pParms->cmdId);
	printf("Response status %d", pParms->status);
}

void handleRXSTATUSRSP (void *parms)
{
	CMD_RXSTATUSRSP_PARMS *pParms = (CMD_RXSTATUSRSP_PARMS *)parms;

	if (pParms->numOfReports == 0)
	{
		printf("RXSTATUS returns no report\n");
		return;
	}
	printf("numOfReports = %u\n", pParms->numOfReports);
	printf("totalPackets = %u\n", pParms->totalPackets);
	printf("goodPackets = %u\n", pParms->goodPackets);
	printf("crcPackets = %u\n", pParms->crcPackets);
	printf("decrypErrors = %u\n", pParms->decrypErrors);
	printf("otherError = %u\n", pParms->otherError);
	printf("rateBit = %u\n", pParms->rateBit);
	printf("startTime = %u\n", pParms->startTime);
	printf("endTime = %u\n", pParms->endTime);
	printf("byteCount = %u\n", pParms->byteCount);
	printf("dontCount = %u\n", pParms->dontCount);
	printf("rssi = %d\n", pParms->rssi);
	printf("rssic = %d, %d, %d, %d\n", pParms->rssic[0], pParms->rssic[1],
		pParms->rssic[2], pParms->rssic[3] );
	printf("rssie = %d, %d, %d, %d\n", pParms->rssie[0], pParms->rssie[1],
		pParms->rssie[2], pParms->rssie[3]);
	printf("badrssi = %d\n", pParms->badrssi);
	printf("badrssic = %d, %d, %d, %d\n", pParms->badrssic[0],
		pParms->badrssic[1], pParms->badrssic[2], pParms->badrssic[3] );
	printf("badrssie = %d, %d, %d, %d\n", pParms->badrssie[0],
		pParms->badrssie[1], pParms->badrssie[2], pParms->badrssie[3]);
	printf("evm = %d, %d, %d, %d\n", pParms->evm[0],
		pParms->evm[1], pParms->evm[2], pParms->evm[3]);
	printf("badevm = %d, %d, %d, %d\n", pParms->badevm[0],
		pParms->badevm[1], pParms->badevm[2], pParms->badevm[3] );
}

void handleRXRSP (void *parms)
{
	CMD_RXRSP_PARMS *pParms = (CMD_RXRSP_PARMS *)parms;
	uint32_t i, k;
	uint32_t rateMask, dataRate, index;
	uint32_t bit0;

	printf("status %d\n", pParms->status);

	if (pParms->rxMode != TCMD_CONT_RX_REPORT)
	{
		return;
	}

	printf("totalpkt %d\n", pParms->totalPackets);
	printf("rssiInDBm %d\n", (pParms->totalPackets ?
		 (pParms->rssiInDBm/(A_INT32)pParms->totalPackets) :
		 pParms->rssiInDBm));
	printf("crcErrPkt %d\n", pParms->crcPackets);
	printf("secErrPkt %d\n", pParms->secErrPkt);
	printf("rcMask 4x4 %d\n", pParms->rateMask11AC[4]);

	//11n and below rates..
	for (k=0;k<2;k++) {
		rateMask = pParms->rateMask[k];
		i=0; bit0=0x00000001;
		while (rateMask) {
			if (bit0 & rateMask) {
				rateMask &= ~bit0;
				dataRate = Mask2Rate[k][i];
				if (dataRate == ATH_RATE_2M ||
					dataRate == ATH_RATE_5_5M ||
					/* CCK rates */
					dataRate == ATH_RATE_11M) {
					/* even number indicates short preamble,
					 * see Mask2Rate array for details
					 */
					if (!(i&1)) {
						/* use last three rates in the
						 * rate table for shortPreamble
						 */
						//dataRate +=
						//(TCMD_MAX_RATES_11AC_3x3 - 4);
						dataRate +=(TCMD_MAX_RATES - 4);
					}
				}
				printf("%s .. ",bgRateStrTbl[dataRate]);
				index = (k*32+i)/2;
				if ((i & 1) == 0 ) {
					printf("rateCnt %d\n",
					(pParms->rateCnt[index] & 0xFFFF));
				}
				else {
					printf("rateCnt %d\n",
						(pParms->rateCnt[index] >>16)
						& 0xFFFF);
				}
			}
			bit0 = bit0 << 1;i++;
		}
	}

	//11ac rates..
	for (k=0;k<4;k++) {
		rateMask = pParms->rateMask11AC[k];
		i=0; bit0=0x00000001;
		while (rateMask) {
			if (bit0 & rateMask) {
				rateMask &= ~bit0;
				index = k + 2;
				dataRate = Mask2Rate[index][i];
				printf("%s .. ",bgRateStrTbl[dataRate]);

				index = (k*32+i)/2;
				if (i && !(i&1)) {
					printf("rateCnt %d\n",
					(pParms->rateCnt11ac[index] & 0xFFFF));
				}
				else {
					printf("rateCnt %d\n",
					(pParms->rateCnt11ac[index] >>16) &
					0xFFFF);
				}
			}
			bit0 = bit0 << 1;i++;
		}
	}

	//4x4 11ac rates..
	rateMask = pParms->rateMask11AC[4];
	i=0; bit0=0x00000001;
	while (rateMask) {
		if (bit0 & rateMask) {
			rateMask &= ~bit0;
			index = 6; //4x4 11ac
			dataRate = Mask2Rate[index][i];
			printf("%s .. ",bgRateStrTbl[dataRate]);

			index = i >> 1;
			if ((i&1) == 0) {
				printf("rateCnt %d\n",
				(pParms->rateCnt11ac[54+index] & 0xFFFF));
			}
			else {
				printf("rateCnt %d\n",
				(pParms->rateCnt11ac[54+index] >>16) & 0xFFFF);
			}
		}
		bit0 = bit0 << 1;i++;
	}

#if 0
	// needed late
	//Cascade BW 160 rates.
	rateMask = pParms->rateMask11AC[5];
	i=0; bit0=0x00000001;
	while (rateMask) {
		if (bit0 & rateMask) {
			rateMask &= ~bit0;
			index = 7; //Cascade
			dataRate = Mask2Rate[index][i];
			printf("%s .. ",bgRateStrTbl[dataRate]);

			index = i >> 1;
			if ((i&1) == 0) {
				/* TBD: need to increase the buffer
				 * to support Cascade
				 */
                                printf("rateCnt %d\n",
				(pParms->rateCnt11ac[54+index] & 0xFFFF));
			}
			else {
				/* TBD: need to increase the buffer
				 * to support Cascade
				 */
				printf("rateCnt %d\n",
				(pParms->rateCnt11ac[54+index] >>16) & 0xFFFF);
			}
		}
		bit0 = bit0 << 1;i++;
	}
#endif
}

#endif /* USE_TLV2 */

void qca6174SetDBS(char *dbs_val)
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	TESTFLOW_CMD_STREAM_V2 *pCmdStream;
	int dbs_value = PHY_RF_MODE_NON_DBS_PHY0;

	/*Bind to TLV2 parser*/
	addTLV2p0BinCmdParser();
	addTLV2p0Encoder();

	registerBASICRSPHandler(handleBASICRSP);

	tlv2CreateCmdHeader(CMD_SETPHYRFMODE);

	if (strncmp(dbs_val, "dbs", 3) == 0) {
		dbs_value = PHY_RF_MODE_DBS_PHY0_PHY1;
	} else if (!strncmp(dbs_val, "phya", 4)) {
		dbs_value = PHY_RF_MODE_NON_DBS_PHY0;
	} else if (!strncmp(dbs_val, "phyb", 4)) {
		dbs_value = PHY_RF_MODE_NON_DBS_PHY1;
	} else if (!strncmp(dbs_val, "sbs", 4)) {
		dbs_value = PHY_RF_MODE_SBS_PHY0_PHY1;
	} else {
		//dbs_value = PHY_RF_MODE_MAX;
		ALOGE("\n %s() Error PHY_RF_MODE_MAX", __func__);
	}

	tlv2AddParms(2, PARM_PHYRFMODE, dbs_value);

	if (tlv2_enabled == TRUE) {
		ALOGE("\n %s() TLV2 CMD_SETPHYRFMODE Phy RF mode %d\n",
			__func__, dbs_value);
		/*Complete TLV2 stream by adding length, CRC, etc */
		pCmdStream = (TESTFLOW_CMD_STREAM_V2 *) tlv2CompleteCmdRsp();
		if (!pCmdStream) {
			ALOGE("\n %s() TLV2 pCmdStream is NULL\n", __func__);
			return;
		}
		cmdStreamLen = (sizeof(TESTFLOW_CMD_STREAM_HEADER_V2) +
				pCmdStream->cmdStreamHeader.length);
		rCmdStream = (A_UINT8 *) pCmdStream;
	}
	else {
		ALOGE("\n %s() CMD_SETPHYRFMODE not supported, needs TLV2\n",
		__func__);
		return;

	}

	print_hex_dump(rCmdStream, cmdStreamLen);
	doCommand(rCmdStream, cmdStreamLen);

	if (gCmd.errCode == 0) {
		ALOGE("\n%s() success = %d\n", __func__, gCmd.errCode);
	} else {
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
	}

}

void qca6174CmdSARIndex(char *val)
{
	gCmd.sar_index = (int)strtol(val, NULL, 0);
}

void qca6174CmdSARChain(unsigned int val)
{
	gCmd.sar_chain = val;
}

void qca6174CmdSARCCK2gLimit(char *val)
{
	gCmd.sar_cck2glimit = (int)strtol(val, NULL, 0);
}

void qca6174CmdSAROFDM2gLimit(char *val)
{
	gCmd.sar_ofdm2glimit = (int)strtol(val, NULL, 0);
}

void qca6174CmdSAROFDM5gLimit(char *val)
{
	gCmd.sar_ofdm5glimit = (int)strtol(val, NULL, 0);
}

void qca6174CmdFlagDPDEnable()
{
	gCmd.enable_dpd_flag = PAPRD_ENA_MASK;
}

void handleSARRSPOp(void *parms)
{
	CMD_SARRSP_PARMS *pSARRSPParms = (CMD_SARRSP_PARMS *)parms;

	printf("SARRSPOp: status %u\n", pSARRSPParms->status);
	printf("SARRSPOp: CCK2gLimit %u\n", pSARRSPParms->CCK2gLimit);
	printf("SARRSPOp: Ofdm2gLimit %u\n", pSARRSPParms->Ofdm2gLimit);
	printf("SARRSPOp: Ofdm5gLimit %u\n", pSARRSPParms->Ofdm5gLimit);
}

void qca6174CmdSAR(uint32_t sar_command_id)
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	TESTFLOW_CMD_STREAM_V2 *pCmdStream;

	/*Bind to TLV2 parser*/
	addTLV2p0BinCmdParser();
	addTLV2p0Encoder();

	registerSARRSPHandler(handleSARRSPOp);

	tlv2CreateCmdHeader(CMD_SAR);

	if (sar_command_id == 1) {
		tlv2AddParms(2, PARM_CMDID, 1);
		tlv2AddParms(2, PARM_CHAIN, gCmd.sar_chain);
		tlv2AddParms(2, PARM_CCK2GLIMIT, gCmd.sar_cck2glimit);
		tlv2AddParms(2, PARM_OFDM2GLIMIT, gCmd.sar_ofdm2glimit);
		tlv2AddParms(2, PARM_OFDM5GLIMIT, gCmd.sar_ofdm5glimit);
		ALOGE("\n %s() TLV2 CMD_SAR chain %u, sar_cck2glimit %u, "
			"sar_ofdm2glimit %u, sar_ofdm5glimit %u \n",
			__func__, gCmd.sar_chain, gCmd.sar_cck2glimit,
			gCmd.sar_ofdm2glimit, gCmd.sar_ofdm5glimit);
	}
	else {
		tlv2AddParms(2, PARM_CMDID, sar_command_id);
		tlv2AddParms(2, PARM_INDEX8, gCmd.sar_index);
		tlv2AddParms(2, PARM_CHAIN, gCmd.sar_chain);
		ALOGE("\n %s() TLV2 CMD_SAR cmd-id %u, index8 %u, chain %u \n",
			__func__, sar_command_id, gCmd.sar_index, gCmd.sar_chain);
	}

	if (tlv2_enabled == TRUE) {
		ALOGE("\n %s() TLV2 CMD_SAR cmd-id %u\n",
			__func__, sar_command_id);
		/*Complete TLV2 stream by adding length, CRC, etc */
		pCmdStream = (TESTFLOW_CMD_STREAM_V2 *) tlv2CompleteCmdRsp();
		if (!pCmdStream) {
			ALOGE("\n %s() TLV2 pCmdStream is NULL\n", __func__);
			return;
		}
		cmdStreamLen = (sizeof(TESTFLOW_CMD_STREAM_HEADER_V2) +
				pCmdStream->cmdStreamHeader.length);
		rCmdStream = (A_UINT8 *) pCmdStream;
	}
	else {
		ALOGE("\n %s() CMD_SAR not supported, needs TLV2\n",
		__func__);
		return;
	}

	print_hex_dump(rCmdStream, cmdStreamLen);
	doCommand(rCmdStream, cmdStreamLen);
	if (gCmd.errCode == 0) {
		ALOGE("\n%s() success = %d\n", __func__, gCmd.errCode);
	} else {
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
	}
}


/* SETREGDMN Command and Response */
void handleSETREGDMNRSP(void *parms)
{
	UNUSED(parms);
	printf("Received SETREGDMN Response\n");
}


void qca6174CmdSETREGDMN(char *val)
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	TESTFLOW_CMD_STREAM_V2 *pCmdStream;
	int temp[2] = {0};
	uint16_t reg_domain[2] = {0};

	if (tlv2_enabled == FALSE) {
		ALOGE("\n %s() CMD_SETREGDMN not supported, needs TLV2\n",
		__func__);
		return;
	}

	/*Bind to TLV2 parser*/
	addTLV2p0BinCmdParser();
	addTLV2p0Encoder();

	registerSETREGDMNRSPHandler(handleSETREGDMNRSP);
	tlv2CreateCmdHeader(CMD_SETREGDMN);

	if (val) {
		sscanf(val, "%4x:%4x", &temp[0], &temp[1]);
		reg_domain[0] = (uint16_t) temp[0];
		reg_domain[1] = (uint16_t) temp[1];
	}
	else {
		ALOGE("\n %s() error: In CMD_SETREGDMN, REGDMN param is NULL\n",
		__func__);
		return;
	}

	tlv2AddParms(4, PARM_REGDMN, 2, 0, reg_domain);
	ALOGE("\n %s() TLV2 CMD_SETREGDMN reg_domain-1 %x reg_domain-2 %x\n",
		__func__, reg_domain[0], reg_domain[1]);
	/*Complete TLV2 stream by adding length, CRC, etc */
	pCmdStream = (TESTFLOW_CMD_STREAM_V2 *) tlv2CompleteCmdRsp();
	if (!pCmdStream) {
		ALOGE("\n %s() TLV2 pCmdStream is NULL\n", __func__);
		return;
	}
	cmdStreamLen = (sizeof(TESTFLOW_CMD_STREAM_HEADER_V2) +
			pCmdStream->cmdStreamHeader.length);
	rCmdStream = (A_UINT8 *) pCmdStream;

	print_hex_dump(rCmdStream, cmdStreamLen);
	doCommand(rCmdStream, cmdStreamLen);
	if (gCmd.errCode == 0) {
		ALOGE("\n%s() success = %d\n", __func__, gCmd.errCode);
	} else {
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
	}
}


int qca6174TxCommand(const char *txtype)
{
	TX_DATA_START_PARAMS *Params;
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	uint32_t rowIndex = 0, rateBitMask = 0;
	int is11ACRate = FALSE;
	uint32_t dataRate = 0, wlanMode = 0;
	int16_t tx_power = 0;
	int isPowerChanged = 0;
	int i;

	Params = malloc(sizeof(TX_DATA_START_PARAMS));
	if (Params == NULL)
		return -1;

	if (gCmd.freq == 0) {
		gCmd.freq = 2412;
	}

	if (!strcmp(txtype, "sine")) {
		Params->mode = TCMD_CONT_TX_SINE;
	} else if (!strcmp(txtype, "frame")) {
		Params->mode = TCMD_CONT_TX_FRAME;
	} else if (!strcmp(txtype, "tx99")) {
		Params->mode = TCMD_CONT_TX_TX99;
	} else if (!strcmp(txtype, "tx100")) {
		Params->mode = TCMD_CONT_TX_TX100;
	} else if (!strcmp(txtype, "cwtone")) {
		Params->mode = TCMD_CONT_TX_CWTONE;
	} else if (!strcmp(txtype, "clpcpkt")) {
		Params->mode = TCMD_CONT_TX_CLPCPKT;
	} else if (!strcmp(txtype, "off")) {
		Params->mode = TCMD_CONT_TX_OFF;
	} else {
		ALOGE("%s():%d Error wrong txMode %s\n",
				__func__, __LINE__, txtype);
		return -1;
	}

	Params->antenna = 0;
	Params->enANI = gCmd.ani;
	Params->scramblerOff = 0;
	Params->aifsn = gCmd.aifs;
	Params->txPattern = ZEROES_PATTERN;
	Params->shortGuard = gCmd.shortguard;
	Params->numPackets = gCmd.numPkt;
	/* Use chain 3 for default since Agilent N4010 doesn't support 2 chains */
        ALOGE("-------------------------%s():%d gCmd.chain = %d\n", __func__, __LINE__, gCmd.chain);
	if (gCmd.chain) {
		Params->txChain = gCmd.chain;
	} else
		Params->txChain = 1;

	Params->miscFlags = PROCESS_RATE_IN_ORDER_MASK | TX_STATUS_PER_RATE_MASK;
	if (gCmd.stbc)
		Params->miscFlags |= DESC_STBC_ENA_MASK;

	if (gCmd.ldpc)
		Params->miscFlags |= DESC_LDPC_ENA_MASK;

	/* don't set TX_STATUS_PER_RATE_MASK flag for scpc cal.
	 * otherwise, firmware will do scpc cal and won't send response first.
	 */
	if (gCmd.scpc_cal)
		Params->miscFlags &= ~TX_STATUS_PER_RATE_MASK;

	if (is_zero_ether_addr(gCmd.bssid))
		ath_ether_aton(DEFAULT_TX_BSSID, Params->bssid);
	else
		memcpy(Params->bssid, gCmd.bssid, ATH_MAC_LEN);

	Params->bandwidth = gCmd.bandWidth;

	if (is_zero_ether_addr(gCmd.txStation))
		ath_ether_aton(DEFAULT_TX_TXSTATION, Params->txStation);
	else
		memcpy(Params->txStation, gCmd.txStation, ATH_MAC_LEN);

	if (is_zero_ether_addr(gCmd.rxStation)) {
		ath_ether_aton(DEFAULT_TX_RXSTATION, Params->rxStation);
		Params->broadcast = 1;
	}
	else {
		memcpy(Params->rxStation, gCmd.rxStation, ATH_MAC_LEN);
		/* if destination MAC address is present then set broadcast
			to off */
		Params->broadcast = 0;
	}

	if (!gCmd.tpcm)
		Params->tpcm = TPC_TX_PWR;
	else
		Params->tpcm = gCmd.tpcm;

	Params->agg = gCmd.agg;

	getDataRateAndWlanMode(&dataRate, &wlanMode);
	if (gCmd.wmode != TCMD_WLAN_MODE_MAX)
		Params->wlanMode = gCmd.wmode;
	else
		Params->wlanMode = wlanMode;

	Params->freq = gCmd.freq;
	rateIndexToArrayMapping(dataRate, &rowIndex, &rateBitMask, &is11ACRate);
	for (i=0; i<RATE_POWER_MAX_INDEX; i++)
		Params->rateMaskBitPosition[i] = 0xff;
	Params->rateMaskBitPosition[0] = (rowIndex << 5) + getOffsetFromBitmap(rateBitMask);
#ifdef TCMD_LIB_DEBUG
	printf("dataRate %d rateIndex %d \n", dataRate, Params->rateMaskBitPosition[0]);
#endif
	for (i=0; i<RATE_POWER_MAX_INDEX; i++)
		Params->txPower[i] = 0xff;
	printf("dataRate %d rateIndex %d \n", dataRate,	Params->rateMaskBitPosition[0]);
#ifdef USE_TLV2
	uint32_t rMask[3], rMask11AC[6];

	memset(rMask, '\0', sizeof(rMask));
	memset(rMask11AC, '\0', sizeof(rMask11AC));
	if ( gCmd.rate == MASK_RATE_MAX /* 219 */) // Sweep all rates
	{
		rMask[0]     = 0xFFFFFF00;
		rMask[1]     = 0xFFFFFFFF;
		rMask11AC[0] = 0xFF3FF1FF;
		rMask11AC[1] = 0xF3FF1FF3;
		rMask11AC[2] = 0x3FF3FF3F;
		rMask11AC[3] = 0x3BF;
		rMask11AC[4] = 0x3FFFFFFF;
		rMask11AC[5] = 0x000FFFFF;
	}
	else {
		if (is11ACRate == FALSE ) {
			rMask[rowIndex] = rateBitMask;
			if ( rowIndex != 0 ) {
				/* NOTE: This is required as on the firmware
				 * default rateMask0 is set to rate 11
				 */
				rMask[0] = 0;
			}
		}
		else {
			if ( dataRate <= 152 ) {
				rMask11AC[rowIndex] = rateBitMask;
			}
			else if ( dataRate <= ATH_RATE_VHT80_NSS4_MCS9 ) {
				rMask11AC[4] = rateBitMask;
			}
			else {
				rMask11AC[5] = rateBitMask;
			}
			/* NOTE: This is required as on the firmware
			 * default rateMask0 is set to rate 11
			 */
			rMask[0] = 0;
		}
	}
#endif //USE_TLV2

#if 0 /* SS HW and QCA HW required */
	if (gCmd.txPwr) {
		if ((Params->freq >= 2412 && Params->freq <= 2472) || Params->freq == 2484) {
			switch (wlanMode) {
				case TCMD_WLAN_MODE_NOHT:
					tx_power = gCmd.txPwr - 2;
					isPowerChanged = 1;
					break;
				case TCMD_WLAN_MODE_HT20:
				case TCMD_WLAN_MODE_HT40PLUS:
				case TCMD_WLAN_MODE_HT40MINUS:
					break;
				case TCMD_WLAN_MODE_CCK:
					tx_power = gCmd.txPwr - 3;
					isPowerChanged = 1;
					break;
				case TCMD_WLAN_MODE_VHT20:
				case TCMD_WLAN_MODE_VHT40PLUS:
				case TCMD_WLAN_MODE_VHT40MINUS:
				case TCMD_WLAN_MODE_VHT80_0:
				case TCMD_WLAN_MODE_VHT80_1:
				case TCMD_WLAN_MODE_VHT80_2:
				case TCMD_WLAN_MODE_VHT80_3:
					break;
				default:
					break;
			}
		}
	} else {
		if ((Params->freq >= 2412 && Params->freq <= 2472) || Params->freq == 2484) {
			switch (wlanMode) {
				case TCMD_WLAN_MODE_NOHT:
					tx_power = 14;
					isPowerChanged = 1;
					break;
				case TCMD_WLAN_MODE_HT20:
				case TCMD_WLAN_MODE_HT40PLUS:
				case TCMD_WLAN_MODE_HT40MINUS:
					tx_power = 12;
					isPowerChanged = 1;
					break;
				case TCMD_WLAN_MODE_CCK:
					tx_power = 17;
					isPowerChanged = 1;
					break;
				case TCMD_WLAN_MODE_VHT20:
				case TCMD_WLAN_MODE_VHT40PLUS:
				case TCMD_WLAN_MODE_VHT40MINUS:
				case TCMD_WLAN_MODE_VHT80_0:
				case TCMD_WLAN_MODE_VHT80_1:
				case TCMD_WLAN_MODE_VHT80_2:
				case TCMD_WLAN_MODE_VHT80_3:
					ALOGE("%s ERROR: not available! freq: %d, wlanMode %d", __func__, Params->freq, wlanMode);
					break;
				default:
					break;
			}
		} else {
			switch (wlanMode) {
				case TCMD_WLAN_MODE_NOHT:
					tx_power = 10;
					isPowerChanged = 1;
					break;
				case TCMD_WLAN_MODE_HT20:
				case TCMD_WLAN_MODE_HT40PLUS:
				case TCMD_WLAN_MODE_HT40MINUS:
					tx_power = 9;
					isPowerChanged = 1;
					break;
				case TCMD_WLAN_MODE_CCK:
					ALOGE("%s ERROR: not available! freq: %d, wlanMode %d", __func__, Params->freq, wlanMode);
					break;
				case TCMD_WLAN_MODE_VHT20:
				case TCMD_WLAN_MODE_VHT40PLUS:
				case TCMD_WLAN_MODE_VHT40MINUS:
				case TCMD_WLAN_MODE_VHT80_0:
				case TCMD_WLAN_MODE_VHT80_1:
				case TCMD_WLAN_MODE_VHT80_2:
				case TCMD_WLAN_MODE_VHT80_3:
					tx_power = 9;
					isPowerChanged = 1;
					break;
				default:
					break;
			}
		}
	}
#endif

	if (tx_power < 0)
		tx_power = 0;
	if (gCmd.txPwr < 0)
		gCmd.txPwr = 0;

	if (isPowerChanged) {
		Params->txPower[0] = tx_power * 2;
	} else {
		Params->txPower[0] = gCmd.txPwr * 2;
	}
	 ALOGE("%s - input pwr %.2f output pwr %d", __func__,
		gCmd.txPwr, Params->txPower[0]);

	for (i=0; i<RATE_POWER_MAX_INDEX; i++)
		Params->pktLength[i] = 0x0;

	if (!gCmd.pktLen0)
		gCmd.pktLen0 = 1500;

	Params->pktLength[0] = gCmd.pktLen0;

	Params->ir = 1;
	Params->dutycycle = 0;
	Params->paConfig = gCmd.paConfig;
	Params->gainIdx = gCmd.gainIdx;
	Params->dacGain = gCmd.dacGain;

#ifdef USE_TLV2
	TESTFLOW_CMD_STREAM_V2 *pCmdStream;

	/*Bind to TLV2 parser*/
	addTLV2p0BinCmdParser();
	addTLV2p0Encoder();

	/*Create TLV2 command header*/
	if (Params->mode == TCMD_CONT_TX_OFF) {
		ALOGE("\n %s() TLV2 CMD_TXSTATUS %d", __func__, Params->mode);
		registerTXSTATUSRSPHandler(handleTXSTATUSRSP);
		// stop Tx and need report
		tlv2CreateCmdHeader(CMD_TXSTATUS);

		if (gCmd.phyid)
			tlv2AddParms(6, PARM_PHYID, gCmd.phyid, PARM_STOPTX,
					1, PARM_NEEDREPORT, 1);
		else
			tlv2AddParms(6, PARM_PHYID, 0, PARM_STOPTX,
					1, PARM_NEEDREPORT, 1);
	} else {
		ALOGE("\n %s() TLV2 CMD_TX %d", __func__, Params->mode);

		registerBASICRSPHandler(handleBASICRSP);
		tlv2CreateCmdHeader(CMD_TX);
		if (gCmd.phyid)
			tlv2AddParms(4, PARM_PHYID, gCmd.phyid,
					PARM_TXMODE, Params->mode);
		else
			tlv2AddParms(4, PARM_PHYID, 0,
					PARM_TXMODE, Params->mode);

		if (gCmd.freq)
			tlv2AddParms(2, PARM_FREQ, gCmd.freq);

		if (gCmd.freq2)
			tlv2AddParms(2, PARM_FREQ2, gCmd.freq2);

		tlv2AddParms(2, PARM_WLANMODE, Params->wlanMode);
		if (gCmd.chain)
			tlv2AddParms(2, PARM_CHAINMASK, Params->txChain);
		else
			tlv2AddParms(2, PARM_CHAINMASK, 3);
		tlv2AddParms(2, PARM_BANDWIDTH, Params->bandwidth);
		/* Enable DPD Flag */
		if (gCmd.enable_dpd_flag) {
			Params->miscFlags |= gCmd.enable_dpd_flag;
			ALOGE("\n %s() TLV2 CMD_TX DPD flag is Enabled! "
				"PARM_FLAGS %u", __func__, Params->miscFlags);
		}
		else {
			ALOGE("\n %s() TLV2 CMD_TX DPD flag is not Enabled "
				"PARM_FLAGS %u", __func__, Params->miscFlags);
		}
		tlv2AddParms(2, PARM_FLAGS, Params->miscFlags);
		if (Params->enANI)
			tlv2AddParms(2, PARM_ENANI, Params->enANI);
		if (Params->aifsn)
			tlv2AddParms(2, PARM_AIFSN, Params->aifsn);
		tlv2AddParms(2, PARM_BROADCAST, Params->broadcast);
		tlv2AddParms(2, PARM_AGG, Params->agg);
		tlv2AddParms(2, PARM_SHORTGUARD, Params->shortGuard);
		tlv2AddParms(2, PARM_TXNUMPACKETS, Params->numPackets);
		tlv2AddParms(2, PARM_TPCM, Params->tpcm);
		if (Params->txPower[0] == 0)
			tlv2AddParms(2, PARM_TXPOWER, 0x1c);
		else
			tlv2AddParms(2, PARM_TXPOWER, Params->txPower[0]);
		tlv2AddParms(2, PARM_PKTSZ, gCmd.pktLen0);
		//tlv2AddParms(2, PARM_ANTENNA, Params->antenna);
		//tlv2AddParms(2, PARM_SCRAMBLEROFF, Params->scramblerOff);
		//tlv2AddParms(2, PARM_TXPATTERN, Params->txPattern);
		//tlv2AddParms(2, PARM_IR, Params->ir);
		//tlv2AddParms(2, PARM_DUTYCYCLE, Params->dutycycle);
		if (Params->paConfig)
			tlv2AddParms(2, PARM_PACONFIG, Params->paConfig);
		if (Params->tpcm == TPC_FORCED_GAINIDX) {
			tlv2AddParms(2, PARM_GAINIDX, Params->gainIdx);
			tlv2AddParms(2, PARM_DACGAIN, Params->dacGain);
		}
		tlv2AddParms(2, PARM_RATEBITINDEX,
				Params->rateMaskBitPosition[0]);
		tlv2AddParms(4, PARM_RATEMASK, 3, 0, rMask);
		tlv2AddParms(4, PARM_RATEMASK11AC, 5, 0, rMask11AC);
		tlv2AddParms(2, PARM_RATEMASKAC160, rMask11AC[5]);
		tlv2AddParms(4, PARM_BSSID, ATH_MAC_LEN, 0,
				(uint8_t *)&Params->bssid);
		tlv2AddParms(4, PARM_TXSTATION, ATH_MAC_LEN, 0,
				(uint8_t *)&Params->txStation);
		tlv2AddParms(4, PARM_RXSTATION, ATH_MAC_LEN, 0,
				(uint8_t *)&Params->rxStation);
	}

	if (tlv2_enabled == TRUE) {
		ALOGE("\n %s() TLV2 CMD_TX/CMD_TXSTATUS %d\n",
				__func__, Params->mode);
		/*Complete TLV2 stream by adding length, CRC, etc */
		pCmdStream = (TESTFLOW_CMD_STREAM_V2 *) tlv2CompleteCmdRsp();
		if (!pCmdStream) {
			ALOGE("\n %s() TLV2 pCmdStream is NULL\n", __func__);
			return -1;
		}
		cmdStreamLen = (sizeof(TESTFLOW_CMD_STREAM_HEADER_V2) +
				pCmdStream->cmdStreamHeader.length);

		rCmdStream = (A_UINT8 *) pCmdStream;
	}
	else
#endif /* USE_TLV2 */
	{
		createCommand(_OP_TX);
		ALOGE("\n %s() opcode _OP_TX\n", __func__);

		addTxParameters(Params);
		commandComplete(&rCmdStream, &cmdStreamLen);
	}

	print_hex_dump(rCmdStream, cmdStreamLen);

	doCommand(rCmdStream, cmdStreamLen);

	if (gCmd.errCode == 0) {
		gCmd.isTxStart = 1;
		ALOGE("\n%s() Success = %d\n", __func__, gCmd.errCode);

	}
	else {
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
	}

	return (gCmd.errCode == 0) ? 0 : -1;
}
#endif

int qca6174TxSineStart(void)
{
	return qca6174TxCommand("sine");
}

int qca6174Tx99Start(void)
{
	return qca6174TxCommand("tx99");
}

int qca6174TxFrameStart(void)
{
	return qca6174TxCommand("frame");
}

int qca6174Tx100Start(void)
{
    return qca6174TxCommand("tx100");
}

int qca6174TxCWStart(void)
{
    return qca6174TxCommand("cwtone");
}

int qca6174TxCLPCPktStart(void)
{
    return qca6174TxCommand("clpcpkt");
}

int qca6174TxStop(void)
{
	int ret;
	if (1 || gCmd.isTxStart) {
		ret = qca6174TxCommand("off");
		if (!ret)
			gCmd.isTxStart = 0;
		return ret;
	}
	return 0;
}

void addRateMaskToCommand(int dataRate)
{
	uint32_t rowIndex = 0, rateBitMask = 0;
	int is11ACRate = FALSE;
	char rateMask[25];
	uint32_t value = 0;

	/* let user input index of rateTable instead of string parse */
	if ( dataRate > 154 ) {
		ALOGE("%s() Invalid Index\n", __func__);
		return;
	}

	if ( dataRate == 153 ) // Sweep all rates
	{
#if 0
		memset(rateMask,0,sizeof(rateMask));
		snprintf((char*)rateMask,25,"%s%d","rateMask",0);
		rateBitMask = 0xFFFFFF00;
		addParameterToCommand((uint8_t *)rateMask,(uint8_t *)&rateBitMask);

		memset(rateMask,0,sizeof(rateMask));
		snprintf((char*)rateMask,25,"%s%d","rateMask",1);
		rateBitMask = 0xFFFFFFFF;
		addParameterToCommand((uint8_t *)rateMask,(uint8_t *)&rateBitMask);

		memset(rateMask,0,sizeof(rateMask));
		snprintf((char*)rateMask,25,"%s%d","rateMask11AC",0);
		rateBitMask = 0xFF3FF1FF;
		addParameterToCommand((uint8_t *)rateMask,(uint8_t *)&rateBitMask);

		memset(rateMask,0,sizeof(rateMask));
		snprintf((char*)rateMask,25,"%s%d","rateMask11AC",1);
		rateBitMask = 0xF3FF1FF3;
		addParameterToCommand((uint8_t *)rateMask,(uint8_t *)&rateBitMask);

		memset(rateMask,0,sizeof(rateMask));
		snprintf((char*)rateMask,25,"%s%d","rateMask11AC",2);
		rateBitMask = 0x3FF3FF3F;
		addParameterToCommand((uint8_t *)rateMask,(uint8_t *)&rateBitMask);

		memset(rateMask,0,sizeof(rateMask));
		snprintf((char*)rateMask,25,"%s%d","rateMask11AC",3);
		rateBitMask = 0x3BF;
		addParameterToCommand((uint8_t *)rateMask,(uint8_t *)&rateBitMask);
#else
		ALOGE("%s() Sweep all rates is not verified.\n", __func__);
		return;
#endif
	}
	else {
		// Map ratemask to the bitmask value..
		rateIndexToArrayMapping (dataRate, &rowIndex, &rateBitMask, &is11ACRate);

		printf("rowIndex %d bitMask %8x\n",rowIndex,rateBitMask);

		memset(rateMask,0,sizeof(rateMask));

		snprintf(rateMask,25,"%s%d","rateMask",rowIndex);
		addParameterToCommand(rateMask,(uint8_t *)&rateBitMask, sizeof(rateBitMask));

		if ( rowIndex != 0 ) {
			//NOTE: This is required as on the firmware default rateMask0 is set to rate 11
			memset(rateMask,0,sizeof(rateMask));
			snprintf(rateMask,25,"%s%d","rateMask",0);
			value = 0;
			addParameterToCommand(rateMask,(uint8_t *)&value, sizeof(value));
		}
	}
}

#ifdef NOT_USE_CLITLVCMD_IF_MODULE
int qca6174RxPacketStart(void)
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	uint32_t miscFlags = PROCESS_RATE_IN_ORDER_MASK | RX_STATUS_PER_RATE_MASK;
	uint8_t mac[ATH_MAC_LEN];
	uint32_t dataRate = 0, wlanMode = 0;
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 300000;  /* 300 usec */

	gCmd.rxPkt = gCmd.rxRssi = gCmd.rxCrcError = gCmd.rxSecError = 0;

	getDataRateAndWlanMode(&dataRate, &wlanMode);
	if (gCmd.freq == 0) {
		gCmd.freq = 2412;
	}

	createCommand(_OP_RX);
	ALOGE("%s() opcode _OP_RX", __func__);

	if (gCmd.freq != 2412)
		addParameterToCommand("channel",(uint8_t *)&gCmd.freq, sizeof(gCmd.freq));

	if (gCmd.antenna != 0)
		addParameterToCommand("antenna",(uint8_t *)&gCmd.antenna, sizeof(gCmd.antenna));

	if (gCmd.ani != 0)
		addParameterToCommand("enANI",(uint8_t *)&gCmd.ani, sizeof(gCmd.ani));

	addParameterToCommand("wlanMode",(uint8_t *)&wlanMode, sizeof(wlanMode));
	addParameterToCommand("rxChain",(uint8_t *)&gCmd.chain, sizeof(gCmd.chain));

	if (is_zero_ether_addr(gCmd.bssid)) {
		ath_ether_aton(DEFAULT_RX_BSSID, mac);
		addParameterToCommand("bssid",(uint8_t *)mac, sizeof(mac));
	}
	else
		addParameterToCommand("bssid",(uint8_t *)gCmd.bssid, sizeof(gCmd.bssid));

	if (is_zero_ether_addr(gCmd.addr)) {
		ath_ether_aton(DEFAULT_RX_ADDR, mac);
		addParameterToCommand("addr",(uint8_t *)mac, sizeof(mac));
	}
	else
		addParameterToCommand("addr",(uint8_t *)gCmd.addr, sizeof(gCmd.addr));

	addParameterToCommand("expectedPkts",(uint8_t *)&gCmd.expectedPkts, sizeof(gCmd.expectedPkts));
	addParameterToCommand("flags",(uint8_t *)&miscFlags, sizeof(miscFlags));

	addRateMaskToCommand(dataRate);

	commandComplete(&rCmdStream, &cmdStreamLen);
	print_hex_dump(rCmdStream, cmdStreamLen);

	doCommand(rCmdStream, cmdStreamLen);

	if (gCmd.errCode == 0) {
		gCmd.isRxStart = 1;
	} else {
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
	}
	nanosleep (&ts, NULL); /* delay here */
	return (gCmd.errCode == 0) ? 0 : -1;
}
#else
int qca6174RxPacketStart(char *rx_type)
{
	RX_DATA_START_PARAMS *Params;
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	uint32_t rowIndex = 0, rateBitMask = 0;
	int is11ACRate = FALSE;
	uint32_t dataRate = 0, wlanMode = 0;
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 300000;  /* 300 usec */
	int i, rx_value = 0;
	uint32_t misc_flags = 0;

	if (gCmd.freq == 0) {
		gCmd.freq = 2412;
	}

	Params = malloc(sizeof(RX_DATA_START_PARAMS));
	if (Params == NULL)
		return -1;

	gCmd.rxPkt = gCmd.rxRssi = gCmd.rxCrcError = gCmd.rxSecError = 0;

	getDataRateAndWlanMode(&dataRate, &wlanMode);
	if (gCmd.wmode != TCMD_WLAN_MODE_MAX)
		Params->wlanMode = gCmd.wmode;
	else
		Params->wlanMode = wlanMode;

	Params->freq = gCmd.freq;
	Params->antenna = 0;
	Params->enANI = gCmd.ani;
	Params->promiscuous = 1;
	ALOGE("\n%s() freq = %d\n", __func__, Params->freq);
	ALOGE("\n%s() antenna = %d\n", __func__, Params->antenna);
	ALOGE("\n%s() enANI = %d\n", __func__, Params->enANI);
	ALOGE("\n%s() wlanMode = %d\n", __func__, Params->wlanMode);

	/* Use chain 3 for default since Agilent N4010 doesn't support 2 chains */
	if (gCmd.chain) {
		Params->rxChain = gCmd.chain;
	} else
		Params->rxChain = 3;
	Params->broadcast = 1;

	ALOGE("\n%s() broadcast = %d\n", __func__, Params->broadcast);
	ALOGE("\n%s() rxChain = %d\n", __func__, Params->rxChain);

	if (is_zero_ether_addr(gCmd.bssid))
		ath_ether_aton(DEFAULT_RX_BSSID, Params->bssid);
	else
		memcpy(Params->bssid, gCmd.bssid, ATH_MAC_LEN);
	ALOGE("\n%s() bssid = ", __func__);
      print_hex_dump(Params->bssid,6);
	Params->bandwidth = gCmd.bandWidth;
	ALOGE("\n%s() bandwidth = %d\n", __func__, Params->bandwidth);
	if (is_zero_ether_addr(gCmd.addr))
		ath_ether_aton(DEFAULT_RX_ADDR, Params->rxStation);
	else
		memcpy(Params->rxStation, gCmd.addr, ATH_MAC_LEN);

      ALOGE("\n%s() rxStation = ", __func__);
      print_hex_dump(Params->rxStation,6);

	Params->numPackets = 0;

#ifdef USE_TLV2
	if (tlv2_enabled == TRUE) {
		tlv2_rx_vht_rates = TRUE;
	}
#endif /* USE_TLV2 */
	for (i=0; i<RATE_MASK_ROW_MAX; i++)
		Params->rateMask[i] = 0;
	// Map ratemask to the bitmask value..
	rateIndexToArrayMapping(dataRate, &rowIndex, &rateBitMask, &is11ACRate);
	Params->rateMask[rowIndex] = rateBitMask;
	if ( rowIndex != 0 ) {
		//NOTE: This is required as on the firmware default rateMask0 is set to rate 11
		Params->rateMask[0] = 0;
	}

#ifdef USE_TLV2
	TESTFLOW_CMD_STREAM_V2 *pCmdStream;
	uint32_t rMask[3], rMask11AC[6];

	memset(rMask, '\0', sizeof(rMask));
	memset(rMask11AC, '\0', sizeof(rMask11AC));
	if ( gCmd.rate == MASK_RATE_MAX /* 219 */) // Sweep all rates
	{
		rMask[0]     = 0xFFFFFF00;
		rMask[1]     = 0xFFFFFFFF;
		rMask11AC[0] = 0xFF3FF1FF;
		rMask11AC[1] = 0xF3FF1FF3;
		rMask11AC[2] = 0x3FF3FF3F;
		rMask11AC[3] = 0x3BF;
		rMask11AC[4] = 0x3FFFFFFF;
		rMask11AC[5] = 0x000FFFFF;
	}
	else {
		if (is11ACRate == FALSE ) {
			rMask[rowIndex] = rateBitMask;
			if ( rowIndex != 0 ) {
				/* NOTE: This is required as on the firmware
				 * default rateMask0 is set to rate 11
				 */
				rMask[0] = 0;
			}
		}
		else {
			if ( dataRate <= 152 ) {
				rMask11AC[rowIndex] = rateBitMask;
			}
			else if ( dataRate <= ATH_RATE_VHT80_NSS4_MCS9 ) {
				rMask11AC[4] = rateBitMask;
			}
			else {
				rMask11AC[5] = rateBitMask;
			}
			/* NOTE: This is required as on the firmware
			 * default rateMask0 is set to rate 11
			 */
			rMask[0] = 0;
		}
	}

	/*Bind to TLV2 parser*/
	addTLV2p0BinCmdParser();
	addTLV2p0Encoder();
	//registerBASICRSPHandler(handleBASICRSP);
	registerRXRSPHandler(handleRXRSP);

	tlv2CreateCmdHeader(CMD_RX);
	misc_flags = (PROCESS_RATE_IN_ORDER_MASK |
			RX_STATUS_PER_RATE_MASK);

	if (strncmp(rx_type, "promis", 6) == 0) {
		rx_value = TCMD_CONT_RX_PROMIS;
	} else if (!strncmp(rx_type, "filter", 6)) {
		rx_value = TCMD_CONT_RX_FILTER;
	} else if (!strncmp(rx_type, "report", 6)) {
		rx_value = TCMD_CONT_RX_REPORT;
	} else {
		ALOGE("\n %s() Wrong Rx Type \n", __func__);
		return -1;
	}

	if (gCmd.phyid)
		tlv2AddParms(4, PARM_PHYID, gCmd.phyid,
				PARM_RXMODE, rx_value);
	else
		tlv2AddParms(4, PARM_PHYID, 0, PARM_RXMODE, rx_value);

	if (gCmd.freq)
		tlv2AddParms(2, PARM_FREQ, Params->freq);

	if (gCmd.freq2)
		tlv2AddParms(2, PARM_FREQ2, gCmd.freq2);

	tlv2AddParms(2, PARM_WLANMODE, Params->wlanMode);
	tlv2AddParms(2, PARM_CHAINMASK, Params->rxChain);
	tlv2AddParms(2, PARM_FLAGS, misc_flags);
	//tlv2AddParms(2, PARM_BANDWIDTH, Params->bandwidth);
	//tlv2AddParms(2, PARM_ANTENNA, Params->antenna);
	//tlv2AddParms(2, PARM_ENANI, Params->enANI);
	//tlv2AddParms(2, PARM_BROADCAST, Params->broadcast);
	//tlv2AddParms(2, PARM_EXPECTEDPKTS, Params->numPackets);
	tlv2AddParms(4, PARM_RATEMASK, 3, 0, rMask);
	tlv2AddParms(4, PARM_RATEMASK11AC, 5, 0, rMask11AC);
	tlv2AddParms(2, PARM_RATEMASKAC160, rMask11AC[5]);
	tlv2AddParms(4, PARM_BSSID, ATH_MAC_LEN, 0,
			(uint8_t *)&Params->bssid);
	tlv2AddParms(4, PARM_STAADDR, ATH_MAC_LEN, 0,
			(uint8_t *)&Params->rxStation);
	tlv2AddParms(4, PARM_BTADDR, ATH_MAC_LEN, 0,
			(uint8_t *)&gCmd.bt_addr);

	if (tlv2_enabled == TRUE) {
		ALOGE("\n %s() TLV2 CMD_RX RX mode %d \n", __func__, rx_value);
		/*Complete TLV2 stream by adding length, CRC, etc */
		pCmdStream = (TESTFLOW_CMD_STREAM_V2 *) tlv2CompleteCmdRsp();
		if (!pCmdStream) {
			ALOGE("\n %s() TLV2 pCmdStream is NULL\n", __func__);
			return -1;
		}
		cmdStreamLen = (sizeof(TESTFLOW_CMD_STREAM_HEADER_V2) +
				pCmdStream->cmdStreamHeader.length);

		rCmdStream = (A_UINT8 *) pCmdStream;
	}
	else
#endif /* USE_TLV2 */
	{
		createCommand(_OP_RX);
		ALOGE("\n %s() opcode _OP_RX \n", __func__);

		addRxParameters(Params);

		commandComplete(&rCmdStream, &cmdStreamLen);
	}

	print_hex_dump(rCmdStream, cmdStreamLen);

	doCommand(rCmdStream, cmdStreamLen);

#ifdef USE_TLV2
	tlv2_rx_vht_rates = FALSE;
#endif /* USE_TLV2 */

	if (gCmd.errCode == 0) {
		gCmd.isRxStart = 1;
		ALOGE("\n%s() success = %d\n", __func__, gCmd.errCode);
	} else {
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
	}
	nanosleep (&ts, NULL); /* delay here */
	return (gCmd.errCode == 0) ? 0 : -1;
}
#endif

int qca6174RxPacketStop(void)
{
      ALOGE("\n%s() isRxStart = %d\n", __func__, gCmd.isRxStart);
	if (1 || gCmd.isRxStart)
	{
		uint8_t *rCmdStream = NULL;
		uint32_t cmdStreamLen = 0;
		uint32_t value;
#ifdef USE_TLV2
		TESTFLOW_CMD_STREAM_V2 *pCmdStream;

		/*Bind to TLV2 parser*/
		addTLV2p0BinCmdParser();
		addTLV2p0Encoder();
		//registerBASICRSPHandler(handleBASICRSP);
		registerRXSTATUSRSPHandler(handleRXSTATUSRSP);

		if (gCmd.freq == 0) {
			gCmd.freq = 2412;
		}

		// stop Rx and need report
		tlv2CreateCmdHeader(CMD_RXSTATUS);
		if (gCmd.phyid)
			tlv2AddParms(6, PARM_PHYID, gCmd.phyid, PARM_STOPRX,
					1, PARM_FREQ, gCmd.freq);
		else
			tlv2AddParms(6, PARM_PHYID, 0, PARM_STOPRX,
					1, PARM_FREQ, gCmd.freq);

		if (tlv2_enabled == TRUE) {
			ALOGE("\n %s() TLV2 CMD_RXSTATUS \n", __func__);
			/*Complete TLV2 stream by adding length, CRC, etc */
			pCmdStream =
				(TESTFLOW_CMD_STREAM_V2 *) tlv2CompleteCmdRsp();
			if (!pCmdStream) {
				ALOGE("\n %s() TLV2 pCmdStream is NULL\n",
					__func__);
				return -1;
			}
			cmdStreamLen = (sizeof(TESTFLOW_CMD_STREAM_HEADER_V2) +
					pCmdStream->cmdStreamHeader.length);

			rCmdStream = (A_UINT8 *) pCmdStream;
		}
		else
#endif /* USE_TLV2 */
		{
			createCommand(_OP_GENERIC_NART_CMD);
			ALOGE("\n %s() opcode _OP_GENERIC_NART_CMD \n",
				__func__);

			/* What is meaning of that value? */
			value = 0xb7;
			addParameterToCommand("commandId", (uint8_t *)&value,
						sizeof(value));

			value = 1;
			addParameterToCommand("param1", (uint8_t *)&value,
						sizeof(value));

			commandComplete(&rCmdStream, &cmdStreamLen);
		}

		print_hex_dump(rCmdStream, cmdStreamLen);

		doCommand(rCmdStream, cmdStreamLen);

		if (gCmd.errCode == 0) {
			gCmd.isRxStart = 0;
			ALOGE("\n%s() Success = %d\n", __func__, gCmd.errCode);
		} else {
			ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);
		}
		return (gCmd.errCode == 0) ? 0 : -1;
	}

	return 0;
}

void qca6174SetLongPreamble(int enable)
{
	gCmd.longpreamble = enable;
}

void qca6174SetAifsNum(int slot)
{
	gCmd.aifs = slot;
}

void qca6174SetAntenna(int antenna)
{
//	gCmd.antenna = antenna;
	gCmd.chain = antenna;
ALOGE("\n-----------------%s() gCmd.chain = %d\n", __func__, gCmd.chain);
}

void qca6174SetAni(int ani)
{
	gCmd.ani = ani;
}

void qca6174SetBssid(char *mac)
{
	if (ath_ether_aton(mac, gCmd.bssid) != 0) {
		ALOGE("Invalid bssid address format! \n");
	}
}

void qca6174SetTxStation(char *mac)
{
	if (ath_ether_aton(mac, gCmd.txStation) != 0) {
		ALOGE("Invalid TX address format! \n");
	}
}

void qca6174SetRxStation(char *mac)
{
	if (ath_ether_aton(mac, gCmd.rxStation) != 0) {
		ALOGE("Invalid RX address format! \n");
	}
}

void qca6174SetAddr(char *mac)
{
	if (ath_ether_aton(mac, gCmd.addr) != 0) {
		ALOGE("Invalid STA address format! \n");
	}
}

void qca6174SetBTAddr(char *mac)
{
	if (ath_ether_aton(mac, gCmd.bt_addr) != 0) {
		ALOGE("Invalid BT address format! \n");
	}
}

/*
 * assumption is that the number of total packets is always 1000.
 */
#define RX_TOTAL_PACKET_COUNT           1000

static uint32_t rx_good_packets = RX_TOTAL_PACKET_COUNT;
static uint32_t rx_error_packets = 0;

uint32_t qca6174RxGetErrorFrameNum(void)
{
	rx_error_packets = RX_TOTAL_PACKET_COUNT - gCmd.rxPkt;

	return rx_error_packets;
}

uint32_t qca6174RxGetGoodFrameNum(void)
{
	rx_good_packets = gCmd.rxPkt;

	return rx_good_packets;
}

const char *qca6174GetErrorString(void)
{
	return gCmd.errString;
}

void qca6174SetBandWidth(int width)
{
	gCmd.bandWidth = width;
}

/**
 * qca6174_clear_scpc_done() - clear scpc done bit in 6320_eeprom memory and clear scpcdone variable in firmware
 *
 * Return: 0 for success and error number for failure
 */
int qca6174_clear_scpc_done()
{
	uint32_t psatpwr2g_0_0, boardflagsext;
	int ret = 0;

	/* read value of PsatPwr2G */
	ret = qca6174_eeprom_block_read(&psatpwr2g_0_0, AR6320_EEPROM_PSATPWR2G_OFFSET, AR6320_EEPROM_PSATPWR2G_LENGTH);
	if (0 != ret) {
		printf("%s: read PsatPwr2G val failed, ret %d\n", __func__, ret);
		goto exit;
	}
	printf("%s: read PsatPwr2G 0x%x\n", __func__, psatpwr2g_0_0);

	/* write value of PsatPwr2G to set firmware SCPC done variable to FALSE */
	ret = qca6174_eeprom_write_item(&psatpwr2g_0_0, AR6320_EEPROM_PSATPWR2G_OFFSET, AR6320_EEPROM_PSATPWR2G_LENGTH);
	if (0 != ret) {
		printf("%s: write PsatPwr2G val failed, ret %d\n", __func__, ret);
		goto exit;
	}

	/* read value of boardFlagsExt */
	ret = qca6174_eeprom_block_read(&boardflagsext, AR6320_EEPROM_BOARDFLAGSEXT_OFFSET, AR6320_EEPROM_BOARDFLAGSEXT_LENGTH);
	if (0 != ret){
		printf("%s: read BOARDFLAGSEXT failed, ret %d\n", __func__, ret);
		goto exit;
	}
	printf("%s: read boardflagsext 0x%x\n", __func__, boardflagsext);

	/* write value to boardFlagsExt to clear SCPC done bit */
	boardflagsext &= ~WHAL_BOARD_SCPC_IS_DONE;
	ret = qca6174_eeprom_write_item(&boardflagsext, AR6320_EEPROM_BOARDFLAGSEXT_OFFSET, AR6320_EEPROM_BOARDFLAGSEXT_LENGTH);
	if (0 != ret){
		printf("%s: w BOARDFLAGSEXT failed, ret %d\n", __func__, ret);
		goto exit;
	}

exit:
	return ret;
}

/**
 * qca6174_compute_checksum_only() - compute checksum of 6320 eeprom memory in uint16_t
 * @ half_ptr: pointer to 6320 eeprom memory in uint16_t pointer
 * @ length: length of 6320 eeprom memory in unit of uint16_t
 *
 * Return: sum calculated for checksum
 */
static uint16_t qca6174_compute_checksum_only(uint16_t *half_ptr, uint16_t length)
{
	uint16_t sum = 0, i;
	for(i = 0; i < length; i++)
		sum ^= *half_ptr++;
	return(sum);
}

/**
 * qca6174_compute_checksum() - compute checksum of 6320 eeprom memory
 * @ ptr_6320_eeprom: pointer to 6320 eeprom memory
 *
 * Return: none
 */
void qca6174_compute_checksum(uint8_t *ptr_6320_eeprom)
{
	uint16_t sum, *half_ptr;

	memset(ptr_6320_eeprom + AR6320_EEPROM_CHECKSUM_OFFSET, 0, AR6320_EEPROM_CHECKSUM_LENGTH);
	half_ptr =(uint16_t *)ptr_6320_eeprom;
	printf("6320_eeprom 0x%x 0x%x 0x%x 0x%x\n", half_ptr[0], half_ptr[1], half_ptr[2], half_ptr[3]);
	sum = qca6174_compute_checksum_only(half_ptr,AR6320_EEPROM_STRUCT_LENGTH/2);
	sum = 0xFFFF ^ sum;
	memcpy(ptr_6320_eeprom + AR6320_EEPROM_CHECKSUM_OFFSET, &sum, AR6320_EEPROM_CHECKSUM_LENGTH);
	printf("after checksum 6320_eeprom 0x%x 0x%x 0x%x 0x%x\n", half_ptr[0], half_ptr[1], half_ptr[2], half_ptr[3]);
}

/**
 * qca6174_enable_scpc_cal() - enable scpc cal flag for tcmd check, especially for Tx cmd
 * @ val: set flag to true or false
 *
 * Return: none
 */
void qca6174_enable_scpc_cal(int val)
{
	gCmd.scpc_cal = val;
}

/**
 * qca6174_get_scpc_cal() - get scpc cal flag in tcmd struct
 *
 * Return: scpc_cal flag value
 */
int qca6174_get_scpc_cal()
{
	return gCmd.scpc_cal;
}

/**
 * qca6174_eeprom_write_item() - write item to eeprom memory block
 * @ buf: buffer to write to eeprom memory
 * @ offset: offset in eeprom struct
 * @ length: length to of buf to be written to eeprom memory
 *
 * Return: 0 for success and error number for failure
 */
#define AR6320_EEPROM_WRITE_DATA_HEADER_LEN          3
int qca6174_eeprom_write_item(void* buf, uint32_t offset, uint8_t length)
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	uint8_t *databuf = NULL;
	uint32_t value;
	int ret = 0;

	if (NULL == buf) {
		printf("%s: buf is NULL\n", __func__);
		ret = -1;
		goto exit;
	}

	databuf = malloc(length + AR6320_EEPROM_WRITE_DATA_HEADER_LEN);
	if (NULL == databuf) {
		printf("%s: databuf is NULL\n", __func__);
		ret = -2;
		goto exit;
	}
	memset(databuf, 0, length + AR6320_EEPROM_WRITE_DATA_HEADER_LEN);

	printf("%s: offset 0x%x, length 0x%x\n", __func__, offset, length);

	ret = createCommand(_OP_GENERIC_NART_CMD);
	if (TRUE != ret) {
		printf("%s: create NART cmd failed, ret %d\n", __func__, ret);
		ret = -3;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	value = M_EEPROM_WRITE_ITEMS_CMD_ID;
	ret = addParameterToCommand("commandId",(uint8_t *)&value, sizeof(value));
	if (TRUE != ret) {
		printf("%s: add commandId failed, ret %d\n", __func__, ret);
		ret = -4;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	/* numOfItems. limit to 1 */
	value = 1;
	ret = addParameterToCommand("param1",(uint8_t *)&value, sizeof(value));
	if (TRUE != ret) {
		printf("%s: add param1 failed, ret %d\n", __func__, ret);
		ret = -5;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	/* offset (2bytes) + length (1byte) + data */
	databuf[0] = (uint8_t)offset;
	databuf[1] = (uint8_t)(offset >> 8);
	databuf[2] = length;
	memcpy(&databuf[AR6320_EEPROM_WRITE_DATA_HEADER_LEN], buf, length);
	ret = addParameterToCommandWithLen("data",(uint8_t *)databuf, 0, length+AR6320_EEPROM_WRITE_DATA_HEADER_LEN);
	if (TRUE != ret) {
		printf("%s: add data failed, ret %d\n", __func__, ret);
		ret = -6;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	ret = commandComplete(&rCmdStream, &cmdStreamLen);
	if (TRUE != ret) {
		printf("%s: commandComplete failed, ret %d\n", __func__, ret);
		ret = -7;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	print_hex_dump(rCmdStream, cmdStreamLen);
	ret = doCommand(rCmdStream, cmdStreamLen);
	if (0 != ret) {
		printf("%s: doCommand failed, ret %d\n", __func__, ret);
		goto exit;
	}

exit:
	if (databuf)
		free(databuf);
	return ret;
}

/**
 * qca6174_eeprom_block_read() - read eeprom memory block
 * @ buf: buffer to save read result
 * @ offset: offset in eeprom struct
 * @ length: length to be read from eeprom memory
 *
 * Return: 0 for success and error number for failure
 */
int qca6174_eeprom_block_read(void* buf, uint32_t offset, uint32_t length)
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	uint32_t value;
	int ret = 0;

	if (NULL == buf) {
		printf("%s: buf is NULL\n", __func__);
		ret = -1;
		goto exit;
	}

	printf("%s: offset %d, length %d\n", __func__, offset, length);

	ret = createCommand(_OP_GENERIC_NART_CMD);
	if (TRUE != ret) {
		printf("%s: create NART cmd failed, ret %d\n", __func__, ret);
		ret = -2;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	value = M_EEEPROM_BLOCK_READ_ID;
	ret = addParameterToCommand("commandId",(uint8_t *)&value, sizeof(value));
	if (TRUE != ret) {
		printf("%s: add commandId failed, ret %d\n", __func__, ret);
		ret = -3;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	/* data length */
	value = length;
	ret = addParameterToCommand("param1",(uint8_t *)&value, sizeof(value));
	if (TRUE != ret) {
		printf("%s: add param1 failed, ret %d\n", __func__, ret);
		ret = -4;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	/* data offset */
	value = offset;
	ret = addParameterToCommand("param2",(uint8_t *)&value, sizeof(value));
	if (TRUE != ret) {
		printf("%s: add param2 failed, ret %d\n", __func__, ret);
		ret = -5;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	/* data length */
	value = length;
	ret = addParameterToCommand("param3",(uint8_t *)&value, sizeof(value));
	if (TRUE != ret) {
		printf("%s: add param3 failed, ret %d\n", __func__, ret);
		ret = -6;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	ret = commandComplete(&rCmdStream, &cmdStreamLen);
	if (TRUE != ret) {
		printf("%s: commandComplete failed, ret %d\n", __func__, ret);
		ret = -7;
		goto exit;
	}
	/* convert to 0 as success */
	ret = 0;

	print_hex_dump(rCmdStream, cmdStreamLen);
	ret = doCommand(rCmdStream, cmdStreamLen);
	if (0 != ret) {
		printf("%s: doCommand failed, ret %d\n", __func__, ret);
		goto exit;
	}
	/* response data buffer always starts from offset 4 */
	memcpy(buf, &gCmd.nart_rsp_buf[4], length);
exit:
	return ret;
}

void handleDPDLoopbackRSP(void *parms)
{
	CMD_DPDLOOPBACKINFORSP_PARMS *pParms =
			(CMD_DPDLOOPBACKINFORSP_PARMS *)parms;
	if (pParms == NULL) {
		printf("Invalid response pointer\n");
		return;
	}

	printf("status = %u\n", pParms->status);
	printf("phy Id = %u\n", pParms->phyId);
	printf("chain mask = %u\n", pParms->chainMask);
	printf("dpd_lb_fail = %u\n", pParms->dpd_lb_fail);
	printf("agc2_pwr = %d\n", pParms->agc2_pwr);
	printf("rx_gain = %u\n", pParms->rx_gain);
	printf("max_bin = %u\n", pParms->max_bin);
	printf("max_bin_db = %u\n", pParms->max_bin_db);
	printf("sq_idx = %u\n", pParms->sq_idx);
	printf("sq = %u\n", pParms->sq);
	printf("dc_i = %d\n", pParms->dc_i);
	printf("dc_q = %d\n", pParms->dc_q);

}

int qca6174CmdDPDStatus()
{
	uint8_t *rCmdStream = NULL;
	uint32_t cmdStreamLen = 0;
	TESTFLOW_CMD_STREAM_V2 *pCmdStream;

	if (tlv2_enabled != TRUE) {
		ALOGE("%s(), TLV2 need to be enabled\n", __func__);
		return -1;
	}

	if (gCmd.freq == 0)
		gCmd.freq = 2412;

	ALOGE("%s(), phyId = %d\n", __func__, gCmd.phyid);
	ALOGE("%s(), chain = %d\n", __func__, gCmd.chain);

	/*Bind to TLV2 parser*/
	addTLV2p0BinCmdParser();
	addTLV2p0Encoder();
	registerDPDLOOPBACKINFORSPHandler(handleDPDLoopbackRSP);

	tlv2CreateCmdHeader(CMD_DPDLOOPBACKINFO);

	tlv2AddParms(2, PARM_PHYID, gCmd.phyid);
	tlv2AddParms(2, PARM_CHAINMASK, gCmd.chain);

	/*Complete TLV2 stream by adding length, CRC, etc */
	pCmdStream = (TESTFLOW_CMD_STREAM_V2 *) tlv2CompleteCmdRsp();
	if (!pCmdStream) {
		ALOGE("\n %s() TLV2 pCmdStream is NULL\n", __func__);
		return -1;
	}
	cmdStreamLen = (sizeof(TESTFLOW_CMD_STREAM_HEADER_V2) +
				pCmdStream->cmdStreamHeader.length);

	rCmdStream = (A_UINT8 *) pCmdStream;

	print_hex_dump(rCmdStream, cmdStreamLen);

	doCommand(rCmdStream, cmdStreamLen);

	if (gCmd.errCode == 0)
		ALOGE("\n%s() Success = %d\n", __func__, gCmd.errCode);
	else
		ALOGE("\n%s() error = %d\n", __func__, gCmd.errCode);

	return (gCmd.errCode == 0) ? 0 : -1;
}
