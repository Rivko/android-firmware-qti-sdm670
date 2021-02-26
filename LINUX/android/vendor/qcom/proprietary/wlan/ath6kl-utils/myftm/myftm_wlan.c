/*==========================================================================

                     FTM WLAN Source File

# Copyright (c) 2011, 2013-2018 by Qualcomm Technologies, Inc.
# All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================

                         Edit History


when       who       what, where, why
--------   ---       ----------------------------------------------------------
07/11/11   karthikm  Wrapper that contains routines for directing FTM commands
                     sent from host to the IOCTL calls of Atheros driver.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/limits.h>
//#include "comdef.h"

#include "testcmd6174.h"
#include "libtcmd.h"
#include "myftm_wlan.h"
#include "myftm_dbg.h"
#include "art_utf_common.h"

/* for crc32 */
#include "zlib.h"

#define INVALID_FREQ    0
#define A_RATE_NUM      28
#define G_RATE_NUM      28
#define RATE_STR_LEN    20

#ifdef CONFIG_FTM_WLAN_AUTOLOAD
#define MODULE_FILE             "/proc/modules"
#define DRIVER_MODULE_TAG       "wlan"
#define WLAN_CFG_FILE           "/persist/wlan_mac.bin"
/* Offset for set mac address cmd */
#define MAIN_CMD_OFFSET         16
#define SUB_CMD_OFFSET          24
#define LENGTH_OFFSET           32
#define NMAC_OFFSET             48
#define PMAC_OFFSET             49

#if defined(ANDROID)
#if defined(BOARD_HAS_ATH_WLAN_AR6320)
#if BOARD_HAS_ATH_WLAN_AR6320
#define FTM_WLAN_LOAD_CMD       "/system/bin/insmod " \
                                "/system/lib/modules/wlan.ko con_mode=5"
#define FTM_WLAN_UNLOAD_CMD     "/system/bin/rmmod wlan"
#endif
#endif
#elif defined(MDM)
#define FTM_WLAN_LOAD_CMD       "/etc/init.d/wlan start_ftm"
#define FTM_WLAN_UNLOAD_CMD     "/etc/init.d/wlan stop"
#else
#warning "Load and Unload driver may not work!"
#endif

typedef enum {
    SUBCMD_DRIVER_LOAD      = 'L',
    SUBCMD_DRIVER_UNLOAD    = 'U',
    SUBCMD_DRIVER_AUTO_MODE = 'A',
} sub_cmds;

extern _CMD_PARM gCmd;

static int load_wifi_driver_testmode(void);
static int unload_wifi_driver(void);
static bool is_wifi_driver_loaded(char *mod_tag);
static bool flag_driver_auto_load = false;
#endif /* CONFIG_FTM_WLAN_AUTOLOAD */

bool ifs_init[32]  = {false};

int gTxType = TCMD_CONT_TX_TX99;
int gWifiMode = AT_WIMODE_80211a;
/* flag to dump scpc cal result into single bdf for debugging/compare */
int g_dbg_scpc = 0;

enum {
	CAL_FILE_SPLIT_FILES_IN_PERSIST,
	CAL_FILE_SINGLE_BDF_IN_PERSIST
};
#define CAL_FILE_SPLIT_PERSIST_DIR                       "/persist/factory/wlan"
#define CAL_FILE_SPLIT_2G_SCPC_CAL_FILENAME              "2g_scpc_cal.bin"
#define CAL_FILE_SPLIT_5G_SCPC_CAL_FILENAME              "5g_scpc_cal.bin"
#define CAL_FILE_SPLIT_2G_SCPC_CAL_FULL_FILENAME         CAL_FILE_SPLIT_PERSIST_DIR"/"CAL_FILE_SPLIT_2G_SCPC_CAL_FILENAME
#define CAL_FILE_SPLIT_5G_SCPC_CAL_FULL_FILENAME         CAL_FILE_SPLIT_PERSIST_DIR"/"CAL_FILE_SPLIT_5G_SCPC_CAL_FILENAME
#define CAL_FILE_SINGLE_BDF_PERSIST_DIR                  "/persist"
#define BDF_FULL_FILENAME                                "/firmware/image/bdwlan30.bin"
#define SCPC_CAL_DBG_BDF_FULL_FILENAME                   "/persist/bdwlan30_scpc.bin"

static int g_cal_file_opt = CAL_FILE_SPLIT_FILES_IN_PERSIST;

static unsigned int rx_mode;
#ifdef CONFIG_FTM_WLAN_AUTOLOAD
/*===========================================================================
FUNCTION
  load_wifi_driver_testmode

DESCRIPTION
  Use system call to load driver

DEPENDENCIES
  NIL

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/

static int load_wifi_driver_testmode(void)
{
    int ret = 0;

    /* clean-up the driver state */
    if ((ret = unload_wifi_driver())) {
        DPRINTF(FTM_DBG_ERROR, "%s: Unload driver failed: %d\n", __func__, ret);
        return ret;
    }

#ifdef FTM_WLAN_LOAD_CMD
    if ((ret = system(FTM_WLAN_LOAD_CMD))) {
        DPRINTF(FTM_DBG_ERROR, "WLAN driver load failed!\n");
        return ret;
    }
#else
#error "FTM_WLAN_LOAD_CMD is not defined!"
#endif

    DPRINTF(FTM_DBG_TRACE, "WLAN driver loaded in FTM mode successfully!\n");

    return ret;
}
/*===========================================================================
FUNCTION
  Unload_wifi_driver if the drvier is detected existing already

DESCRIPTION
  Use system call to unload driver

DEPENDENCIES
  NIL

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static int unload_wifi_driver(void)
{
    int ret = 0;

    if (is_wifi_driver_loaded(DRIVER_MODULE_TAG)) {
#ifdef FTM_WLAN_UNLOAD_CMD
        if ((ret = system(FTM_WLAN_UNLOAD_CMD))) {
            DPRINTF(FTM_DBG_ERROR, "WLAN driver unload failed!\n");
            return ret;
        }
#else
#error "FTM_WLAN_UNLOAD_CMD is not defined!"
#endif
    }

    DPRINTF(FTM_DBG_TRACE, "WLAN driver unloaded successfully!\n");

    return ret;
}

/*===========================================================================
FUNCTION
  is_wifi_driver_loaded

DESCRIPTION
  Check if WLAN driver is loaded or not

DEPENDENCIES
  NIL

RETURN VALUE
  Returns true if driver already loaded, false if driver not loaded

SIDE EFFECTS
  NONE
===========================================================================*/
static bool is_wifi_driver_loaded(char *mod_tag)
{
    FILE *proc = NULL;
    char cmd[NAME_MAX + 64]; /* File name max + room for command */
    bool ret = false;

    snprintf(cmd, sizeof(cmd), "cat /proc/modules | grep %s", mod_tag);

    proc = popen(cmd, "r");

    if (proc == NULL) {
        DPRINTF(FTM_DBG_ERROR, "%s failed!\n", __func__);
        return ret;
    }

    if (fread (cmd, 1, sizeof(cmd), proc) > 0)
        ret = true;

    fclose(proc);

    return ret;
}
#endif /* CONFIG_FTM_WLAN_AUTOLOAD */

int32_t WlanATSetWifiEnable(WLAN_AT_WIENABLE_TYPE onoff)
{
    int ret = 0;

    DPRINTF(FTM_DBG_TRACE, "%s: onoff parameter is %d\n", __func__, onoff);

    if (AT_WIENABLE_TEST == onoff) {
        ret = load_wifi_driver_testmode();
DPRINTF(FTM_DBG_TRACE, "%s:******************* load_wifi_driver_testmode %d\n", __func__, ret);
        if (0 == ret)
        { // success and giving initial values
            qca6174ApiInit();
            qca6174SetBssid("50:55:55:55:55:05");
            qca6174SetTxStation("20:22:22:22:22:02");
            qca6174SetRxStation("ff:ff:ff:ff:ff:ff");
            qca6174ChannelSet(11);

	      WlanATSetWifiMode(AT_WIMODE_80211b);
	      WlanATSetWifiBand(AT_WIBAND_20M);//ignored for not 11n/11ac mode
	      WlanATSetRate(11);//ATH_RATE_54M
            qca6174TxPowerSet("20");
            qca6174SetAntenna(1);
            qca6174TxPacketSizeSet(1200);
        }

qca6174SetAntenna(1);
    }
    else if (AT_WIENABLE_OFF == onoff) {
        ret = unload_wifi_driver();
        if (0 == ret) { //success
            qca6174ApiCleanup();
        }
    }
    else {
        DPRINTF(FTM_DBG_ERROR, "%s failed, invalid command!\n", __func__);
        ret = -1;
    }

    return ret;
}

WLAN_AT_WIENABLE_TYPE WlanATGetWifiEnable(void)
{
    bool bLoaded;
    int ret = AT_WIENABLE_OFF;
    bLoaded = is_wifi_driver_loaded(DRIVER_MODULE_TAG);
    if (bLoaded == true)
        ret = AT_WIENABLE_TEST;
    else
        ret = AT_WIENABLE_OFF;

    DPRINTF(FTM_DBG_TRACE, "%s: return as %d\n", __func__, ret);
    return ret;
}

/*set wifi mode, return value:  success 0 fail -1
  wlan_mode is converted from getDataRateAndWlanMode given the rate
 */
int32_t WlanATSetWifiMode(WLAN_AT_WIMODE_TYPE mode)
{
    int ret = 0;
    DPRINTF(FTM_DBG_TRACE, "%s: mode parameter as %d\n", __func__, mode);
    gWifiMode = mode;    // set in global variable so that it can be shared with bandwidth setting and other parameters/functions
    if (AT_WIMODE_CW == mode) {
        qca6174RateSet(1);    // give a fake rate
        qca6174SetBandWidth(0);
    }
    else if (AT_WIMODE_80211a == mode) {
        qca6174RateSet(4);    // give 6Mbps as default rate
        qca6174SetBandWidth(0);
    }
    else if (AT_WIMODE_80211b == mode) {
        qca6174RateSet(1);    // give 1Mbps as default rate
        qca6174SetBandWidth(0);
    }
    else if (AT_WIMODE_80211g == mode) {
        qca6174RateSet(4);    // give 6Mbps as default rate
        qca6174SetBandWidth(0);
    }
    else if (AT_WIMODE_80211n == mode) {
        qca6174RateSet(15);    // give MCS0 as default rate
        qca6174SetBandWidth(1);    // give HT20 as default rate
    }
    else if (AT_WIMODE_80211ac == mode) {
        qca6174RateSet(15);    // give MCS0 as default rate
        qca6174SetBandWidth(4);    // give VHT20 as default rate
    }
    else {
        DPRINTF(FTM_DBG_ERROR, "%s failed, invalid command!\n", __func__);
        ret = -1;
    }
    return ret;
}

/*set the bandwidth, look at values defined in getMcsWlanMode*/
int32_t WlanATSetWifiBand(WLAN_AT_WIBAND_TYPE width)
{
    int ret = 0;
    DPRINTF(FTM_DBG_TRACE, "%s: bandwidth parameter as %d\n", __func__, width);
    if (AT_WIBAND_20M == width) {
        if (AT_WIMODE_80211n == gWifiMode)
            qca6174SetBandWidth(1);
        else if (AT_WIMODE_80211ac == gWifiMode)
            qca6174SetBandWidth(4);
    }
    else if (AT_WIBAND_40M == width) {
        if (AT_WIMODE_80211n == gWifiMode)
            qca6174SetBandWidth(2);
        else if (AT_WIMODE_80211ac == gWifiMode)
            qca6174SetBandWidth(5);
    }
    else if (AT_WIBAND_80M == width) {
        qca6174SetBandWidth(3);
    }
    else if (AT_WIBAND_160M == width) {
        DPRINTF(FTM_DBG_ERROR, "%s failed, not supported bandwidth (%d)\n", __func__, width);
        ret = -1;
    }
    else if (HALF_SPEED_MODE == (int) width || QUARTER_SPEED_MODE == (int) width) {
        qca6174SetBandWidth(width);
    }
    else {
        DPRINTF(FTM_DBG_ERROR, "%s failed, invalid command!\n", __func__);
        ret = -1;
    }
    return ret;
}

/*set the frequency*/
int32_t WlanATSetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
{
    int ret = 0;
    DPRINTF(FTM_DBG_TRACE, "%s: freq parameter as value (%u), offset (%d)\n", __func__, pFreq->value, pFreq->offset);
    qca6174FreqSet(pFreq->value);
    return ret;
}

/**
 * myftm_wlan_bdf_read() - read content of bdf file at certain offset into buffer
 * @ bdf_file_name: bdf file name
 * @ buf: content to be placed the content
 * @ offset: offset to be read
 * @ length: data length to be read
 *
 * Return: 0 sucess, others - failure
 */
static int myftm_wlan_bdf_read(char *bdf_file_name, void* buf, uint32_t offset, uint32_t length)
{
	FILE *fp = NULL;
	struct stat st;
	unsigned long file_size;
	int ret = 0;

	if ((NULL == buf) || (NULL == bdf_file_name)) {
		printf("%s: buf is NULL\n", __func__);
		return -1;
	}

	fp = fopen(bdf_file_name, "r");
	if (fp == NULL) {
		printf("%s: failed to open file: %s\n", __func__, bdf_file_name);
		return -1;
	}

	/*get size of the file*/
	if (stat(bdf_file_name, &st) == 0)
		file_size = st.st_size;
	else {
		printf("%s: failed to get file size\n", __func__);
		ret = -2;
		goto bdf_read_out;
	}

	printf("%s: %s file size is: %lu\n", __func__, bdf_file_name, file_size);
	if (file_size <= offset) {
		printf("%s: Bad offset\n", __func__);
		ret = -3;
		goto bdf_read_out;
	}

	printf("%s: offset: %d\n", __func__, offset);
	if (0 != fseek(fp, offset, SEEK_SET)) {
		printf("%s: fseek failed\n", __func__);
		ret = -4;
		goto bdf_read_out;
	}

	/*validate the size(number of bytes) to be read from file */
	if (length > (file_size - offset))
		length = file_size - offset;
	printf("%s: length to be read %d\n", __func__, length);

	if(length != fread(buf, 1, length, fp)){
		printf("%s: fread failed\n", __func__);
		ret = -5;
		goto bdf_read_out;
	}

bdf_read_out:
	if (fp)
		fclose(fp);
    return ret;
}

/**
 * myftm_wlan_bdf_write() - write buffer into bdf file at certain offset
 * @ bdf_file_name: bdf file name
 * @ buf: content to be written
 * @ offset: write start from the offset
 * @ length: data length to be written
 *
 * Return: 0 sucess, others - failure
 */
static int myftm_wlan_bdf_write(char *bdf_file_name, void* buf, uint32_t offset, uint32_t length)
{
	int fd = 0;
	FILE *fp = NULL;
	struct stat st;
	unsigned long file_size;
	int ret = 0;

	if ((NULL == buf) || (NULL == bdf_file_name)) {
		printf("%s: buf is NULL\n", __func__);
		return -1;
	}

	fd = open(bdf_file_name, O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP);
	if (-1 == fd) {
		printf("%s: failed to open file: %s\n", __func__, bdf_file_name);
		return -1;
	}

	fp = fdopen(fd, "wb");
	if (NULL == fp) {
		printf("%s: failed to fdopen file descriptor: %s\n", __func__, bdf_file_name);
		return -1;
	}

	/*get size of the file*/
	if (stat(bdf_file_name, &st) == 0)
		file_size = st.st_size;
	else {
		printf("%s: failed to get file size\n", __func__);
		ret = -2;
		goto bdf_write_out;
	}

	printf("%s: %s file size is: %lu\n", __func__, bdf_file_name, file_size);
	if (file_size <= offset) {
		printf("%s: Bad offset\n", __func__);
		ret = -3;
		goto bdf_write_out;
	}

	printf("%s: offset: %d\n", __func__, offset);
	if (0 != fseek(fp, offset, SEEK_SET)) {
		printf("%s: fseek failed\n", __func__);
		ret = -4;
		goto bdf_write_out;
	}

	/*validate the size(number of bytes) to be read from file */
	if (length > (file_size - offset))
		length = file_size - offset;
	printf("%s: length to be write %d\n", __func__, length);

	if(length != fwrite(buf, 1, length, fp)) {
		printf("%s: fwrite failed\n", __func__);
		ret = -5;
		goto bdf_write_out;
	}

bdf_write_out:
	if (fp)
		fclose(fp);
    return ret;
}

/**
 * myftm_wlan_cal_file_create() - create cal file
 * @ cal_file_name: cal file name
 * @ buf: content of the file
 * @ length: file length
 *
 * Return: 0 sucess, others - failure
 */
static int myftm_wlan_cal_file_create(char *cal_file_name, void* buf, uint32_t length)
{
	FILE *fp = NULL;
	struct stat st;
	unsigned long file_size;
	int ret = 0;

	if ((NULL == buf) || (NULL == cal_file_name)) {
		printf("%s: buf or cal_file_name is NULL\n", __func__);
		return -1;
	}

	fp = fopen(cal_file_name, "w");
	if (NULL == fp) {
		printf("%s: failed to fopen file descriptor: %s\n",
		       __func__, cal_file_name);
		return -1;
	}

	if(length != fwrite(buf, 1, length, fp)) {
		printf("%s: fwrite failed\n", __func__);
		ret = -2;
		goto cal_write_out;
	}

cal_write_out:
	if (fp)
		fclose(fp);
    return ret;
}

/**
 * myftm_wlan_create_bdf_scpc_helper() - create bdf scpc bin file in persist for debugging
 *
 * Return: sum calculated for checksum
 */
static int myftm_wlan_create_bdf_scpc_helper()
{
	/* cal_buffer to read result from eeprom memory.
	 * re-visit memory size is there is any change
	 */
	uint8_t cal_buf[AR6320_EEPROM_STRUCT_LENGTH];
	int ret = 0;

	/* per guidance, there is no need to remount as /persist is writable */
	//ret = system("mount -o remount -w "CAL_FILE_SINGLE_BDF_PERSIST_DIR);
	//if (0 != ret) {
	//	printf("%s: execute %s failed %d\n",
	//	       __func__,
	//	       "mount -o remount -w "CAL_FILE_SINGLE_BDF_PERSIST_DIR,
	//	       ret);
	//	goto exit;
	//}

	ret = system("cp "BDF_FULL_FILENAME" "SCPC_CAL_DBG_BDF_FULL_FILENAME);
	if (0 != ret) {
		printf("%s: execute %s failed %d\n", __func__, "cp "BDF_FULL_FILENAME" "SCPC_CAL_DBG_BDF_FULL_FILENAME, ret);
		goto exit;
	}

	/* write scpc done bit in board data */
	memset(cal_buf, 0, sizeof(cal_buf));
	ret = qca6174_eeprom_block_read(cal_buf, AR6320_EEPROM_BOARDFLAGSEXT_OFFSET, AR6320_EEPROM_BOARDFLAGSEXT_LENGTH);
	if (0 != ret) {
		printf("%s: read boardflagsext failed\n", __func__);
		goto exit;
	}

	ret = myftm_wlan_bdf_write(SCPC_CAL_DBG_BDF_FULL_FILENAME, cal_buf, AR6320_EEPROM_BOARDFLAGSEXT_OFFSET, AR6320_EEPROM_BOARDFLAGSEXT_LENGTH);
	if (0 != ret) {
		printf("%s: write boardflagsext to debug bdf file failed\n",
		       __func__);
		goto exit;
	}

	/* read 2G and 5G cal data and save into files */
	/* read 2G cal and write to file */
	memset(cal_buf, 0, sizeof(cal_buf));
	ret = qca6174_eeprom_block_read(cal_buf, AR6320_EEPROM_2G_SCPC_CAL_DATA_OFFSET, AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH);
	if (0 != ret) {
		printf("%s: read 2G SCPC cal failed, offset 0x%x, length 0x%x\n",
		       __func__, AR6320_EEPROM_2G_SCPC_CAL_DATA_OFFSET,
		       AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH);
		goto exit;
	}
	ret = myftm_wlan_bdf_write(SCPC_CAL_DBG_BDF_FULL_FILENAME, cal_buf,
				   AR6320_EEPROM_2G_SCPC_CAL_DATA_OFFSET,
				   AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH);
	if (0 != ret) {
		printf("%s: write 2G SCPC cal to debug bdf file failed\n",
		       __func__);
		goto exit;
	}

	/* read 5G cal and write to file */
	memset(cal_buf, 0, sizeof(cal_buf));
	ret = qca6174_eeprom_block_read(cal_buf, AR6320_EEPROM_5G_SCPC_CAL_DATA_OFFSET, AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH);
	if (0 != ret) {
		printf("%s: read 5G SCPC cal failed, offset 0x%x, length 0x%x\n",
		       __func__, AR6320_EEPROM_5G_SCPC_CAL_DATA_OFFSET,
		       AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH);
		goto exit;
	}
	ret = myftm_wlan_bdf_write(SCPC_CAL_DBG_BDF_FULL_FILENAME, cal_buf,
				   AR6320_EEPROM_5G_SCPC_CAL_DATA_OFFSET,
				   AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH);
	if (0 != ret) {
		printf("%s: write 5G SCPC cal to debug bdf file failed\n",
		       __func__);
		goto exit;
	}

	/* compute checksum */
	memset(cal_buf, 0, sizeof(cal_buf));
	ret = myftm_wlan_bdf_read(SCPC_CAL_DBG_BDF_FULL_FILENAME, cal_buf,
				  0, AR6320_EEPROM_STRUCT_LENGTH);
	if (0 != ret) {
		printf("%s: read %s into memory failed!\n",
		       __func__, SCPC_CAL_DBG_BDF_FULL_FILENAME);
		goto exit;
	}

	qca6174_compute_checksum(cal_buf);
	printf("%s: checksum as 0x%x\n",
	       __func__,
	       *((uint16_t *)(cal_buf + AR6320_EEPROM_CHECKSUM_OFFSET)));

	ret = myftm_wlan_bdf_write(SCPC_CAL_DBG_BDF_FULL_FILENAME, cal_buf + AR6320_EEPROM_CHECKSUM_OFFSET, AR6320_EEPROM_CHECKSUM_OFFSET, AR6320_EEPROM_CHECKSUM_LENGTH);
	if (0 != ret) {
		printf("%s: write checksum to debug bdf file failed\n",
		       __func__);
		goto exit;
	}
exit:
	return ret;
}

/**
 * myftm_wlan_write_split_cal_files_helper() - helper function to write split cal file in persist
 *
 * Return: 0 for success and error number for failure
 */
static int myftm_wlan_write_split_cal_files_helper()
{
#define CAL_BUFFER_SIZE                2048
	/* cal_buffer to read result from eeprom memory.
	 * re-visit memory size is there is any change.
	 * 4-byte crc is added at the end of the file
	 */
	uint8_t cal_buf[CAL_BUFFER_SIZE];
	uint32_t crc;
	int ret = 0;

	/* per guidance, there is no need to remount as /persist is writable */
	//ret = system("mount -o remount -w "CAL_FILE_SINGLE_BDF_PERSIST_DIR);
	//if (0 != ret) {
	//	printf("%s: execute %s failed %d\n", __func__, "mount -o remount -w "CAL_FILE_SINGLE_BDF_PERSIST_DIR, ret);
	//	goto exit;
	//}

	ret = system("mkdir -p "CAL_FILE_SPLIT_PERSIST_DIR);
	if (0 != ret) {
		printf("%s: execute %s failed %d\n",
		       __func__, "mkdir -p "CAL_FILE_SPLIT_PERSIST_DIR, ret);
		goto exit;
	}

	/* read 2G cal and write to file */
	memset(cal_buf, 0, sizeof(cal_buf));
	ret = qca6174_eeprom_block_read(cal_buf, AR6320_EEPROM_2G_SCPC_CAL_DATA_OFFSET, AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH);
	if (0 != ret) {
		printf("%s: read 2G SCPC cal failed, offset 0x%x, length 0x%x\n",
		       __func__, AR6320_EEPROM_2G_SCPC_CAL_DATA_OFFSET,
		       AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH);
		goto exit;
	}

	/* add CRC in the last 4 bytes */
	crc = crc32(0, Z_NULL, 0);
	crc = crc32(crc, (void*) cal_buf, AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH);
	memcpy(cal_buf + AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH, &crc, sizeof(crc));

	ret = myftm_wlan_cal_file_create(CAL_FILE_SPLIT_2G_SCPC_CAL_FULL_FILENAME, cal_buf, AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH + sizeof(crc));
	if (0 != ret) {
		printf("%s: write 2G SCPC cal to file failed, filename %s\n",
		       __func__, CAL_FILE_SPLIT_2G_SCPC_CAL_FULL_FILENAME);
		goto exit;
	}
	printf("2G SCPC cal created with crc 0x%x\n", crc);

	/* read 5G cal and write to file */
	memset(cal_buf, 0, sizeof(cal_buf));
	ret = qca6174_eeprom_block_read(cal_buf,
					AR6320_EEPROM_5G_SCPC_CAL_DATA_OFFSET,
					AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH);
	if (0 != ret) {
		printf("%s: read 5G SCPC cal failed, offset 0x%x, length 0x%x\n",
		       __func__, AR6320_EEPROM_5G_SCPC_CAL_DATA_OFFSET,
		       AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH);
		goto exit;
	}

	/* add CRC in the last 4 bytes */
	crc = crc32(0, Z_NULL, 0);
	crc = crc32(crc, (void*) cal_buf, AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH);
	memcpy(cal_buf + AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH, &crc, sizeof(crc));

	ret = myftm_wlan_cal_file_create(CAL_FILE_SPLIT_5G_SCPC_CAL_FULL_FILENAME, cal_buf, AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH + sizeof(crc));
	if (0 != ret) {
		printf("%s: write 5G SCPC cal to file failed, filename %s\n",
		       __func__, CAL_FILE_SPLIT_5G_SCPC_CAL_FULL_FILENAME);
		goto exit;
	}
	printf("5G SCPC cal created with crc 0x%x\n", crc);

exit:
	return ret;
}

/*Set the Tx state*/
int32_t WlanATSetWifiTX(int txType)
{
    int ret = 0;

    DPRINTF(FTM_DBG_INFO, "%s: tx type is %d \n", __func__, txType);
    gTxType = txType;
    if (TCMD_CONT_TX_OFF == txType)
        qca6174TxStop();
    else if (TCMD_CONT_TX_SINE == txType)
	qca6174TxSineStart();
    else if (TCMD_CONT_TX_FRAME == txType)
	qca6174TxFrameStart();
    else if (TCMD_CONT_TX_TX99 == txType) {
        ret = qca6174Tx99Start();
		if (0 != ret) {
			printf("%s: running Tx99 failed!\n", __func__);
			goto exit;
		}
		/* scpc cal goes with Tx99 cal */
		if (qca6174_get_scpc_cal()) {
			uint32_t boardflagsext=0;
			int count=0;

			/* check scpc done bit to make sure scpc cal is done.
			 * when firmware receives Tx99 command, it gives response first and then start scpc cal
			 * when firmware is doing scpc cal, tcmd is blocked without response
			 * pre-wait 3 seconds at least for scpc cal process and
			 * then check status to avoid false-alarm TVL command response timeout
			 */
			sleep(3);
			/* ideally, the polling is not needed, but just in case, wait 10 seconds at most */
			while(count < 10) {
				printf("%s: Tx99 finished, check scpc done bit, count %d\n", __func__, count);
				/* always reset to 0 for a fresh read */
				boardflagsext = 0;
				/* currently tcmd timeout is set as 16 (TCMD_TIMEOUT) which is good for scpc cal time
				 * No need to check return value of the read function.
				 * We have polling to keep re-checking the value until it times out
				 */
				qca6174_eeprom_block_read(&boardflagsext, AR6320_EEPROM_BOARDFLAGSEXT_OFFSET, AR6320_EEPROM_BOARDFLAGSEXT_LENGTH);
				printf("%s: boardflagsext 0x%x\n", __func__, boardflagsext);
				if (boardflagsext & WHAL_BOARD_SCPC_IS_DONE) {
					printf("%s: scpc done is set, count %d\n", __func__, count);
					break;
				}
				count++;
				sleep(1);
			}

			// if scpc done bit is not set, no need to pull out 2G and 5G cal data. return error
			if (!(boardflagsext & WHAL_BOARD_SCPC_IS_DONE)) {
				printf("%s: SCPC cal is not done! 2G and 5G cal are not read from firmware\n", __func__);
				ret = -1;
				goto exit;
			}

			/* read 2G and 5G cal data and save into /persist/factory/wlan */
			if (CAL_FILE_SPLIT_FILES_IN_PERSIST == g_cal_file_opt) {
				ret = myftm_wlan_write_split_cal_files_helper();
				if (0 != ret) {
					printf("%s: failed to write split scpc cal files!\n", __func__);
					goto exit;
				}
			}

			/* dump scpc cal result into single bdf in /persist for debugging/comparison */
			if (g_dbg_scpc) {
				myftm_wlan_create_bdf_scpc_helper();
			}

			/* clear scpc cal flag in gCmd */
			qca6174_enable_scpc_cal(false);
		}
	}
	else if (TCMD_CONT_TX_TX100 == txType)
		ret = qca6174Tx100Start();
	else if (TCMD_CONT_TX_CWTONE == txType)
		ret = qca6174TxCWStart();
	else if (TCMD_CONT_TX_CLPCPKT == txType)
		ret = qca6174TxCLPCPktStart();
	else {
		DPRINTF(FTM_DBG_ERROR, "%s: tx type:%d not supported!\n",
			__func__, txType);
	}
exit:
    return ret;
}

/*Set the Rx state*/
int32_t WlanATSetWifiRX(WLAN_AT_FEATURE_TYPE onoff)
{
	int ret = 0;
	char rx_type[16];

	DPRINTF(FTM_DBG_TRACE, "%s: onoff parameter as (%d)\n",
		__func__, onoff);
	if (AT_FEATURE_TYPE_ON == onoff) {
		memset(rx_type, '\0', sizeof(rx_type));
		if (rx_mode == 1) {
			strlcpy(rx_type, "filter", sizeof(rx_type));
		} else if (rx_mode == 2) {
			strlcpy(rx_type, "report", sizeof(rx_type));
		} else {
			/* Default RX mode is 0
			 * i.e. promis
			 */
			strlcpy(rx_type, "promis", sizeof(rx_type));
		}

#ifdef NOT_USE_CLITLVCMD_IF_MODULE
		ret = qca6174RxPacketStart();
#else
		printf("RX type %s \n", rx_type);
		ret = qca6174RxPacketStart(rx_type);
		/* Reset RX mode after RX start completes */
		rx_mode = 0;
#endif
	}else if(AT_FEATURE_TYPE_OFF == onoff) {
		ret = qca6174RxPacketStop();
	} else {
		DPRINTF(FTM_DBG_ERROR, "%s: rx type not supported!\n",
			__func__);
	}
	return ret;
}

/*Get the tx control*/
WLAN_AT_FEATURE_TYPE WlanATGetWifiTX(void)
{
    int ret = 0;
    DPRINTF(FTM_DBG_TRACE, "%s: return as %u\n", __func__, ret);
    return ret;
}
/*Set the Tx power*/
int WlanATSetWifiTxPower(char *val)
{
    int ret = 1;
    DPRINTF(FTM_DBG_TRACE, "%s: Set tx power parameter as (%s)\n",
		__func__, val);
    qca6174TxPowerSet(val);
    return ret;
}

int WlanATSetWifiTPC(int pmc)
{
    int ret = 1;
    DPRINTF(FTM_DBG_TRACE, "%s: Set tx power control parameter as (%d)\n",
                 __func__, pmc);
    qca6174TxPcmSet(pmc);
    return ret;
}
/*Get the tx power*/
int WlanATGetWifiTxPower(void)
{

  //  DPRINTF(FTM_DBG_TRACE, "%s: return tx power as %d\n", __func__, gCmd.txPwr);
    return 0;
}

/*Set the Tx rate*/
int WlanATSetRate(WLAN_AT_RATE_VALUE ratevalue)
{
    int ret = 1;
    DPRINTF(FTM_DBG_TRACE, "%s: Set rate as (%d)\n", __func__, ratevalue);

    qca6174RateSet(ratevalue);
    return ret;
}

/*Get the tx rate*/
int WlanATGetRate(void)
{

    DPRINTF(FTM_DBG_TRACE, "%s: return rate as %d\n", __func__, gCmd.rate);
    return 0;
}

/*Set the tx antenna*/
int WlanATSetWifiAntenna(int antenna)
{

    int ret = 1;
    DPRINTF(FTM_DBG_TRACE, "%s: Set antenna as (%d)\n", __func__, antenna);

    qca6174SetAntenna(antenna);
    return ret;
}
/*Get the tx antenna*/
int WlanATGetWifiAntenna(void)
{

   // DPRINTF(FTM_DBG_TRACE, "%s: return rate as %d\n", __func__, gCmd.rate);
    return 0;
}

int WlanATSetWifiPktSize(int size)
{
    int ret = 1;
    DPRINTF(FTM_DBG_TRACE, "%s: Set packet size as (%d)\n", __func__, size);

    qca6174TxPacketSizeSet(size);
    return ret;
}

void WlanATSetPaCfg(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set packet PaCfg (%d)\n", __func__, val);
    qca6174SetPaCfg(val);
}

void WlanATSetDacGain(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set DacGain as (%d)\n", __func__, val);
    qca6174SetDacGain(val);
}

void WlanATSetGainIdx(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set GainIdx as (%d)\n", __func__, val);
    qca6174SetGainIdx(val);
}

void WlanATSetNumPkt(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set NumPkt as (%d)\n", __func__, val);
    qca6174SetNumPkt(val);
}

void WlanATSetAgg(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set aggregation as (%d)\n", __func__, val);
    qca6174SetAgg(val);
}

void WlanATSetStbc(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set STBC as (%d)\n", __func__, val);
    qca6174SetStbc(val);
}

void WlanATSetLdpc(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set LDPC as (%d)\n", __func__, val);
    qca6174SetLdpc(val);
}

void WlanATSetWlanMode(int val)
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set WlanMode as (%d)\n", __func__, val);
    qca6174SetWlanMode(val);
}

void WlanATSetLPreamble()
{
    DPRINTF(FTM_DBG_TRACE, "%s: Set preamble as (%d)\n", __func__, val);
    qca6174SetLPreamble();
}

/**
 * myftm_wlan_set_scpc_cal() - request to do scpc calibration.
 * @ cal_file_opt: how to handle calibrated data
 *   0 - split 2G and 5G cal in /persist/factory/wlan folder as 2g_scpc_cal.bin
 *       and 5g_scpc_cal.bin
 *   1 - generate bdwlan30_scpc.bin in /persist which has
 *       golden bdwlan30.bin + calibrated data
 *
 * The function sets flag to trigger firmware to do scpc calibration and
 * enables scpc cal flag in gCmd to handle scpc cal process
 *
 * Return: 0 for success and error number for failure
 */
int myftm_wlan_set_scpc_cal(int cal_file_opt)
{
	g_cal_file_opt = cal_file_opt;

	/* set scpc flag in eeprom to trigger firmware scpc calibration */
	if (0 != qca6174_clear_scpc_done()) {
		printf("%s: qca6174_clear_scpc_done failed!\n", __func__);
		return -1;
	}

	/* enable scpc cal flag */
	qca6174_enable_scpc_cal(true);

	return 0;
}

void WlanATSetPhyid(int val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set PhyId as (%d)\n", __func__, val);
	qca6174SetPhyid(val);
}

void WlanATSetDBS(char *dbs_val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set DBS as (%s)\n", __func__, val);
	qca6174SetDBS(dbs_val);
}

void WlanATSetBssid(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set BSSID as (%s)\n", __func__, val);
	qca6174SetBssid(val);
}

void WlanATSetTXSta(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set TX as (%s)\n", __func__, val);
	qca6174SetTxStation(val);
}

void WlanATSetRXSta(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set RX as (%s)\n", __func__, val);
	qca6174SetRxStation(val);
}

void WlanATSetSTAAddr(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set STA as (%s)\n", __func__, val);
	qca6174SetAddr(val);
}

void WlanATSetBTAddr(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set BT Addr as (%s)\n", __func__, val);
	qca6174SetBTAddr(val);
}

void WlanATSetShortGuard(int val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set ShortGuard as (%d)\n", __func__, val);
	qca6174ShortGuardSet(val);
}

/* Set the Rx mode */
int32_t WlanATSetWifiRXMode(int rx_type)
{
	DPRINTF(FTM_DBG_TRACE, "%s: RX Mode (%d)\n", __func__, rx_type);
	rx_mode = (unsigned int)rx_type;

	return 0;
}

/*set the frequency*/
int32_t WlanATSetWifiFreq2(WLAN_AT_WIFREQ_STRU *pFreq)
{
	int ret = 0;
	DPRINTF(FTM_DBG_TRACE, "%s: freq2 parameter as value %u, offset %d\n",
		__func__, pFreq->value, pFreq->offset);
	qca6174Freq2Set(pFreq->value);
	return ret;
}

/* Command SAR */
void WlanATCmdSAR(uint32_t val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Command SAR %u\n", __func__, val);
	qca6174CmdSAR(val);
}

/* set SAR Index */
void WlanATCmdSARIndex(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set SAR Index as (%s)\n", __func__, val);
	qca6174CmdSARIndex(val);
}

/* Command SAR Chain */
void WlanATCmdSARChain(uint32_t val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Command SAR Chain %u\n", __func__, val);
	qca6174CmdSARChain(val);
}

/* set SAR CCK2GLIMIT */
void WlanATCmdSARCCK2gLimit(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set SAR CCK2GLIMIT as (%s)\n",
				__func__, val);
	qca6174CmdSARCCK2gLimit(val);
}

/* set SAR OFDM2GLIMIT */
void WlanATCmdSAROFDM2gLimit(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set SAR OFDM2GLIMIT as (%s)\n",
				__func__, val);
	qca6174CmdSAROFDM2gLimit(val);
}

/* set SAR OFDM5GLIMIT */
void WlanATCmdSAROFDM5gLimit(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set SAR OFDM5GLIMIT as (%s)\n",
				__func__, val);
	qca6174CmdSAROFDM5gLimit(val);
}

/* set DPD Flag in PARM_FLAGS */
void WlanATCmdFlagDPDEnable()
{
	DPRINTF(FTM_DBG_TRACE, "%s: Set DPD flag in PARM_FLAGS\n",
				__func__);
	qca6174CmdFlagDPDEnable();
}

/* Command SETREGDMN */
void WlanATCmdSETREGDMN(char *val)
{
	DPRINTF(FTM_DBG_TRACE, "%s: Command SETREGDMN %s\n", __func__, val);
	qca6174CmdSETREGDMN(val);
}

/* Command dpdstatus */
void WlanATCmdDPDStatus()
{
	DPRINTF(FTM_DBG_TRACE, "%s: Command DPD Status\n", __func__);
	qca6174CmdDPDStatus();
}

void WlanATinit()
{
    qca6174init();
}

void unittest(void)
{
    qca6174ApiInit();

	qca6174SetBssid("50:55:55:55:55:05");
	qca6174SetTxStation("20:22:22:22:22:02");
	qca6174SetRxStation("ff:ff:ff:ff:ff:ff");

	qca6174ChannelSet(11);
	qca6174RateSet(13);
	qca6174TxPowerSet("20");
	qca6174SetAntenna(1);
	qca6174TxPacketSizeSet(1200);

	//qca6174TxFrameStart();
	//qca6174TxSineStart();
	qca6174Tx99Start();
	//qca6174TxStop();

	//qca6174ChannelSet(6);
	//qca6174RateSet(ATH_RATE_6M);
	//qca6174SetChain(1);

	//qca6174SetAntenna(1);
	//qca6174SetBssid("01:00:00:c0:ff:ee");
	//qca6174SetAddr("01:00:00:c0:ff:ee");

	//qca6174RxPacketStart();
	//sleep(1);
	//qca6174RxPacketStop();

//	qca6174ApiCleanup();
}
