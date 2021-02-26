/*
 *
 * This library contains the API to carry out Real Time Audio Calibration
 * which receives/sends data from QACT and sends/receives it to the RTAC driver
 *
 * Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "adie-rtac.h"
#include "acdb.h"
#include "acph.h"
#include "acph_update_for_rtc.h"
#include <unistd.h>

#undef __packed
#define __packed __attribute__((packed))
#ifdef _ANDROID_
/* definitions for Android logging */
#include <utils/Log.h>
#include "common_log.h"
#else /* _ANDROID_ */
#define LOGI(...)      fprintf(stdout,__VA_ARGS__)
#define LOGE(...)      fprintf(stderr,__VA_ARGS__)
#define LOGV(...)      fprintf(stderr,__VA_ARGS__)
#define LOGD(...)      fprintf(stderr,__VA_ARGS__)
#endif /* _ANDROID_ */

static uint32_t adie_rtac_major_version = 1;
static uint32_t adie_rtac_minor_version	= 2;

#define MAX_NUMBER_OF_CODECS	20
#define FILE_NAME_LENGTH	200
static uint32_t found_codec_path = 0;
static uint32_t register_length = ACPH_CAL_DATA_UNIT_LENGTH;

#define NUMBER_OF_SUBSTRING	3
#define READ_STEP_SIZE 4000
#define ADIE_RTC_HEADER_SIZE 2

struct codec_info {
	uint32_t	handle;
	uint32_t	chipset_id;
	uint32_t	major_version;
	uint32_t	minor_version;
	char		codec_name[FILE_NAME_LENGTH];
	char		reg_path[FILE_NAME_LENGTH];
};

static uint32_t			number_of_codecs = 0;
static struct codec_info	codec_info[MAX_NUMBER_OF_CODECS];


int find_codec_index(uint32_t handle)
{
	int i;
	for (i=0; i < (int)number_of_codecs; i++)
		if (codec_info[i].handle == handle)
			goto done;
	LOGE("%s->could not find codec with handle %d\n", __func__, handle);
	i = -EINVAL;
done:
	return i;
}

static int get_chipset_id(char *chipset_name)
{
	int ret = 0;
	unsigned int i;

	for (i = 0; i < sizeof(codec_chipset)/sizeof(codec_chipset[0]);
		    i++) {
		if (!strncmp(chipset_name, codec_chipset[i].name, strlen(chipset_name)+1)) {
			codec_info[number_of_codecs].chipset_id = codec_chipset[i].id;
			goto done;
		}
	}
	ret = -EINVAL;
done:
	return ret;
}

static int read_version_file(char *version_path)
{
	int	ret = 0;
	FILE	*fd = NULL;
	char	*token;
	char	*token2;
	char	*save;
	char	version_entry[FILE_NAME_LENGTH];

	fd = fopen(version_path, "r");
	if (fd == NULL) {
		LOGE("%s: file open failed for path %s\n", __func__, version_path);
		ret = -ENODEV;
		goto done;
	} else if (fgets(version_entry, sizeof(version_entry), fd) == NULL) {
		LOGE("%s: fgets failed path %s\n", __func__, version_path);
		ret = -EINVAL;
		goto close;
	}

	token = strtok_r(version_entry, "_", &save);
	if (token == NULL) {
		LOGE("%s: strtok failed to get chipset name\n", __func__);
		ret = -EINVAL;
		goto close;
	}


	ret = get_chipset_id(token);
	if (ret < 0) {
		LOGE("%s: get_chipset_id failed error %d path %s\n", __func__, ret, version_path);
		ret = -EINVAL;
		goto close;
	}

	token = strtok_r(NULL, "_", &save);
	if (token == NULL) {
		LOGE("%s: strtok failed to get chipset major version\n", __func__);
		ret = -EINVAL;
		goto close;
	}

	token2 = strtok_r(NULL, "_", &save);
	if (token2 == NULL) {
		LOGE("%s: strtok failed to get chipset minor version\n", __func__);
		ret = -EINVAL;
		goto close;
	}

	codec_info[number_of_codecs].major_version = atoi(token);
	codec_info[number_of_codecs].minor_version = atoi(token2);
close:
	fclose(fd);
done:
	return ret;
}

static int get_reg_path(char **match_strings, int array_size)
{
	DIR *dir;
	int i;
	int ret = -EINVAL;
	struct dirent *dirent;
	char path[FILE_NAME_LENGTH] = "/sys/kernel/debug";

	for(i = 0; i <  array_size; i++) {
		dir = opendir(path);
		if (dir == NULL) {
			LOGI("%d (%s) opendir %s failed\n", errno, strerror(errno), path);
			return -EINVAL;
		}

		while (NULL != (dirent = readdir(dir))) {
			if (strstr(dirent->d_name, match_strings[i]))
			{
				strlcat(path, "/", sizeof(path));
				strlcat(path, dirent->d_name, sizeof(path));

				/* If "reg" found don't search anymore */
				if (i ==  array_size - 1) {
					strlcpy(codec_info[number_of_codecs].reg_path, path,
						sizeof(codec_info[number_of_codecs].reg_path));
					found_codec_path = 1;
					ret = 0;
					closedir(dir);
					goto done;
				}
			}
		}
		closedir(dir);
	}
done:
	return ret;
}

static int find_codecs_info(void)
{
	DIR		*dir;
	struct dirent	*dentry;
	int		ret = 0;
	char		version_path[FILE_NAME_LENGTH] = CODEC_INFO_PATH;

	dir = opendir(version_path);
	if (dir == NULL) {
		LOGE("%s: %d (%s) opendir %s failed\n", __func__, errno, strerror(errno), CODEC_INFO_PATH);
		ret = -ENODEV;
		goto done;
	}

	while ((dentry = readdir(dir)) != NULL) {
		if (!strncmp(dentry->d_name, ".", 1))
			continue;

		ret = snprintf(codec_info[number_of_codecs].codec_name,
			sizeof(codec_info[number_of_codecs].codec_name), "%s", dentry->d_name);
		if (ret < 0) {
			LOGE("%s: Error: snprintf failed: %s in %s, err %d\n",
				__func__, dentry->d_name, CODEC_INFO_PATH, ret);
			continue;
		}

		ret = snprintf(version_path, sizeof(version_path),
			"%s%s/version", CODEC_INFO_PATH, dentry->d_name);
		if (ret < 0) {
			LOGE("%s: Error: snprintf failed: %s in %s, err %d\n",
				__func__, dentry->d_name, CODEC_INFO_PATH, ret);
			continue;
		}

		ret = read_version_file(version_path);
		if (ret < 0) {
			LOGE("%s: Error: read_version_file failed, err %d path %s\n",
				__func__, ret, version_path);
			continue;
		}

		char *match_strings[] = {"regmap", codec_info[number_of_codecs].codec_name, "registers"};
		ret = get_reg_path(match_strings, sizeof(match_strings)/sizeof(char *));
		if (ret < 0) {
			LOGE("%s: Error: get_reg_path failed, err %d, path %s\n",
				__func__, ret, version_path);
			continue;
		}
		codec_info[number_of_codecs].handle = number_of_codecs+1;
		number_of_codecs++;
	}
	closedir(dir);
done:
	return ret;
}

static int find_codecs(void)
{
	int		ret = 0;
	unsigned int		i;
#ifdef _ANDROID_
	char		*old_match_strings[] =  {"asoc", "-snd-card", "_codec", "reg"};
#else /* _ANDROID_ */
	char		*old_match_strings[] = {"regmap", "-000d", "registers"};
#endif

	number_of_codecs = 0;

	ret = find_codecs_info();
	if (number_of_codecs == 0) {
		ret = get_reg_path(old_match_strings, sizeof(old_match_strings)/sizeof(char *));
		if (ret < 0) {
			LOGE("%s: Error: get_reg_path failed, err %d\n",
				__func__, ret);
			goto done;
		}
		adie_rtac_minor_version = 0;
		codec_info[number_of_codecs].handle = number_of_codecs+1;
		number_of_codecs++;
	}

	for (i=0; i < number_of_codecs; i++)
		LOGD("%s: codec %s: handle %d, chipset id %d, major %d, minor %d, reg path %s\n",
			__func__, codec_info[i].codec_name, codec_info[i].handle, codec_info[i].chipset_id,
			codec_info[i].major_version, codec_info[i].minor_version, codec_info[i].reg_path);
done:
	return ret;
}

static int get_codecs_info(uint8_t *resp_buf_ptr, uint32_t resp_buf_len,
			  uint32_t *resp_buf_bytes_filled)
{
	int i;
	int ret = ACPH_SUCCESS;

	if (resp_buf_ptr == NULL) {
		LOGE("%s: resp_buf_ptr is NULL\n", __func__);
		ret = ACPH_FAILURE;
		goto done;
	} else if (resp_buf_bytes_filled == NULL) {
		LOGE("%s: resp_buf_bytes_filled is NULL\n", __func__);
		ret = ACPH_FAILURE;
		goto done;
	} else if (resp_buf_len < sizeof(struct ADIE_CODEC_HANDLE)
		* number_of_codecs) {
		LOGE("%s: resp_buf_len size %u to small for data size %zu\n", __func__, resp_buf_len,
			sizeof(struct ADIE_CODEC_HANDLE) * number_of_codecs);
		ret = ACPH_ERR_LENGTH_NOT_MATCH;
		goto done;
	}

	*resp_buf_bytes_filled = 0;
	memcpy(resp_buf_ptr, &number_of_codecs, sizeof(number_of_codecs));
	*resp_buf_bytes_filled += sizeof(number_of_codecs);
	for (i=0; i < (int)number_of_codecs; i++) {
		memcpy(resp_buf_ptr + *resp_buf_bytes_filled, &codec_info[i].handle,
			sizeof(codec_info[i].handle));
		*resp_buf_bytes_filled += sizeof(codec_info[i].handle);
		memcpy(resp_buf_ptr + *resp_buf_bytes_filled, &codec_info[i].chipset_id,
			sizeof(codec_info[i].chipset_id));
		*resp_buf_bytes_filled += sizeof(codec_info[i].chipset_id);
		memcpy(resp_buf_ptr + *resp_buf_bytes_filled, &codec_info[i].major_version,
			sizeof(codec_info[i].major_version));
		*resp_buf_bytes_filled += sizeof(codec_info[i].major_version);
		memcpy(resp_buf_ptr + *resp_buf_bytes_filled, &codec_info[i].minor_version,
			sizeof(codec_info[i].minor_version));
		*resp_buf_bytes_filled += sizeof(codec_info[i].minor_version);
	}
done:
	return ret;
}

static int parse_codec_reg_file(char_t **rtc_io_buf_base, int32_t *rtc_io_buf_size,
				char_t *get_func, int fd, int32_t codec_idx)
{
	char_t *rtc_io_buf = NULL;
	char_t *temp;
	int32_t buf_size = 0, rc = 0;
	int32_t numBytes;

	if (fd < 0)
	{
		rc = -EINVAL;
		*rtc_io_buf_base = NULL;
		LOGE("[%s]]->ERROR! Invalid fd", get_func);
		goto done;
	}

	while (1)
	{
		temp = realloc(*rtc_io_buf_base, buf_size + READ_STEP_SIZE);
		if (!temp)
		{
			rc = -ENOMEM;
			free(*rtc_io_buf_base);
			*rtc_io_buf_base = NULL;
			buf_size = 0;
			LOGE("[%s]->ERROR! cannot allocate memory: %d, path: %s",
			     get_func, READ_STEP_SIZE, codec_info[codec_idx].reg_path);
			goto done;
		}
		*rtc_io_buf_base = temp;
		rtc_io_buf = *rtc_io_buf_base + buf_size;
		numBytes = read(fd, rtc_io_buf, READ_STEP_SIZE);
		if (numBytes <= 0)
			break;

		buf_size += numBytes;
	}
done:
	*rtc_io_buf_size = buf_size;

	return rc;
}

static int get_adie_register(uint8_t *req_buf_ptr, uint32_t req_buf_len,
			  uint8_t *resp_buf_ptr, uint32_t resp_buf_len,
			  uint32_t *resp_buf_bytes_filled)
{
	int result = ACPH_SUCCESS;
	int     fd = -1;
	char_t *rtc_io_buf = NULL;
	char_t *rtc_io_buf_base = NULL;

	if((resp_buf_ptr != NULL) && (resp_buf_len >= sizeof(ACPH_CMD_GET_ADIE_REGISTER_rsp)))
	{
		int found = 0;
		uint32_t nReqBufLen = 0;
		int32_t nRespBufLen = 0;
		uint32_t ultempRegAddr = 0;
		int32_t lRegValue = 0;
		uint32_t handle = 0;
		int32_t codec_idx = 0;
		uint32_t regAddr = 0;
		uint32_t regMask = 0;
		size_t numBytes = 0;
		uint32_t *nOutputBufPos = (uint32_t *)resp_buf_ptr;
		char_t *pCurInputBuf = NULL;
		char_t *pInputBuf = (char_t *)req_buf_ptr;
		int32_t rtc_io_buf_size = 0;

		nReqBufLen = req_buf_len;
		/* Req buffer contains 4 bytes of Reg addr and 4 bytes of Mask Value */
		if(nReqBufLen < sizeof(ACPH_CMD_GET_ADIE_REGISTER_req))
		{
			LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->insufficient length of req buffer to get data\n");
			result = ACPH_ERR_LENGTH_NOT_MATCH;
			goto done;
		}
		else
		{
			char_t reg[5], val[3];
			reg[4] = '\0', val[2] = '\0';
			uint32_t offset = 0;
			int reg_len;
			char *save;
			char *token;

			pCurInputBuf = pInputBuf;
			if ((adie_rtac_major_version == 1) && (adie_rtac_minor_version == 0)) {
				handle = 1;
			} else {
				memcpy(&handle, pCurInputBuf,ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
			}
			memcpy(&regAddr, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
			memcpy(&regMask, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);

			codec_idx = find_codec_index(handle);
			if (codec_idx < 0) {
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->could not find codec index for handle\n %d", handle);
				goto done;
			} else if (strlen(codec_info[codec_idx].reg_path) == 0) {
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->codec path is empty\n %d", handle);
				goto done;
			}

			fd = open(codec_info[codec_idx].reg_path, O_RDWR);
			if(fd < 0)
			{
				result = ACPH_ERR_ADIE_INIT_FAILURE;
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->ERROR! cannot open adie peek error: %d, path: %s",
					fd, codec_info[codec_idx].reg_path);
				goto done;
			}
			result = parse_codec_reg_file(&rtc_io_buf_base, &rtc_io_buf_size, "ACPH_CMD_GET_ADIE_REGISTER", fd, codec_idx);
			if (rtc_io_buf_base == NULL || result < 0)
			{
				result = ACPH_ERR_ADIE_INIT_FAILURE;
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->ERROR! cannot allocate memory: %d, path: %s",
					READ_STEP_SIZE, codec_info[codec_idx].reg_path);
				goto close_fd;
			}
			if (rtc_io_buf_size <= 0)
			{
				result = ACPH_ERR_ADIE_GET_CMD_FAILURE;
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->ERROR! length of written bytes does not match expected value %d", rtc_io_buf_size);
				goto close_fd;
			}
			LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->codec_reg file size =%d",rtc_io_buf_size);
			numBytes = rtc_io_buf_size;
			rtc_io_buf = rtc_io_buf_base;
			memcpy((void*)reg, (void*)(rtc_io_buf+offset), sizeof(uint32_t));
			token = strtok_r(reg, ":", &save);
			if (token == NULL) {
				LOGE("%s: strtok failed to find register length : delimiter!\n", __func__);
				result = -EINVAL;
				goto done;
			}
			reg_len = strlen(token);
			if (reg_len <= 0) {
				LOGE("%s: register length is %d!\n", __func__, reg_len);
				result = -EINVAL;
				goto done;
			}
			register_length = reg_len;
			LOGD("%s: valid register length is %d\n", __func__, register_length);
			while(numBytes>offset)
			{
				memcpy((void*)reg, (void*)(rtc_io_buf+offset), sizeof(uint32_t));
				offset += register_length;

				ultempRegAddr = strtoul(reg, NULL, 16);
				offset += 2;
				memcpy((void*)val, (void*)(rtc_io_buf+offset), sizeof(uint16_t));
				lRegValue = strtol(val, NULL, 16);
				offset += 3;
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->reg[%08X],val[%08X]\n",
							   ultempRegAddr, lRegValue);

				if (ultempRegAddr >= CDC_REG_DIG_BASE_READ)
					ultempRegAddr -= CDC_REG_DIG_OFFSET;

				if(ultempRegAddr == regAddr)
				{
					found = 1;
					LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->register[%08X] found from the file!\n",regAddr);
					break;
				}
			}

			/* make sure the conversion is successful */
			if (found == 0)
			{
				result = ACPH_ERR_ADIE_GET_CMD_FAILURE;
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->ERROR! get adie register[0x%x] failed Peek(%s) Poke(%s)",
					regAddr, codec_info[codec_idx].reg_path, codec_info[codec_idx].reg_path);
				goto close_fd;
			}
			else
			{
				LOGE("ACPH_CMD_GET_ADIE_REGISTER command success. Found the value for register = 0x%X, value = 0x%X\n",regAddr,lRegValue);
			}
			/* return a masked value */
			lRegValue &= regMask;
			memcpy((void *)nOutputBufPos,(const void *)&lRegValue, sizeof(uint32_t));
			*resp_buf_bytes_filled = ACPH_CAL_DATA_UNIT_LENGTH;
		}
	}
	else
	{
		result = ACPH_FAILURE;
	}
close_fd:
	close(fd);
done:
	free(rtc_io_buf_base);
	return result;
}

static int set_adie_register(uint8_t *req_buf_ptr, uint32_t req_buf_len,
			  uint32_t *resp_buf_bytes_filled)
{
	int result = ACPH_SUCCESS;
	int fd = -1;

	if(resp_buf_bytes_filled != NULL)
	{
		uint32_t nReqBufLen = 0;
		int32_t nRespBufLen = 0;
		uint32_t ulRegValue = 0;
		uint32_t handle = 0;
		int32_t codec_idx = 0;
		uint32_t regAddr = 0;
		uint32_t regMask = 0;
		size_t numBytes1 = 0;
		size_t numBytes2 = 0;
		char *temp = NULL;
		char_t *pCurInputBuf = NULL;
		char_t *pInputBuf = (char_t *)req_buf_ptr;

		nReqBufLen = req_buf_len;
		if(nReqBufLen < sizeof(ACPH_CMD_SET_ADIE_REGISTER_req))
		{
			LOGE("[ACPH_CMD_SET_ADIE_REGISTER]->insufficient length of req buffer to get data\n");
			result = ACPH_ERR_LENGTH_NOT_MATCH;

			goto done;
		}
		else
		{
			pCurInputBuf = pInputBuf;
			if ((adie_rtac_major_version == 1) && (adie_rtac_minor_version == 0)) {
				handle = 1;
			} else {
				memcpy(&handle, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
			}
			memcpy(&regAddr, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
			memcpy(&regMask, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
			memcpy(&ulRegValue, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);

			codec_idx = find_codec_index(handle);
			if (codec_idx < 0) {
				LOGE("[ACPH_CMD_SET_ADIE_REGISTER]->could not find codec index for handle\n %d", handle);
				goto done;
			} else if (strlen(codec_info[codec_idx].reg_path) == 0) {
				LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->codec path is empty\n %d", handle);
				goto done;
			}

			/* set the value as masked one*/
			ulRegValue &= regMask;

			if (regAddr >= CDC_REG_DIG_BASE_WRITE)
				regAddr += CDC_REG_DIG_OFFSET;

			numBytes1 = asprintf(&temp, "0x%x 0x%x", regAddr, ulRegValue);
			LOGE("set register request received for ==> reg[%X], val[%X], bytes[%zu]\n",
				 regAddr, ulRegValue, numBytes1);
			fd = open(codec_info[codec_idx].reg_path, O_RDWR);
			if(fd < 0)
			{
				result = ACPH_ERR_ADIE_INIT_FAILURE;
				LOGE("[ACPH_CMD_SET_ADIE_REGISTER]->ERROR! cannot open adie poke error: %d, path: %s",
					fd, codec_info[codec_idx].reg_path);
				if (temp != NULL)
					{
						free(temp);
						temp=NULL;
					}

				goto done;
			}
			numBytes2 = write(fd, temp, numBytes1);
			LOGE("set register ==> actual bytes written[%zu]\n", numBytes2);
			if (temp != NULL)
				{
					free(temp);
					temp=NULL;
				}
			close(fd);
			/* make sure the write is successful */
			if (numBytes1 != numBytes2)
			{
				result = ACPH_ERR_ADIE_SET_CMD_FAILURE;
				LOGE("[ACPH_CMD_SET_ADIE_REGISTER]->ERROR! set adie register failed for Register[0x%X], numBytes[%zu]",regAddr ,numBytes1);
				goto done;
			}
			*resp_buf_bytes_filled = 0;
				LOGE("[ACPH_CMD_SET_ADIE_REGISTER]->Success\n");
		}
	}
	else
	{
		result = ACPH_FAILURE;
	}
done:
	return result;
}

static int get_multiple_adie_registers(uint8_t *req_buf_ptr,
			uint32_t req_buf_len, uint8_t *resp_buf_ptr,
			uint32_t *resp_buf_bytes_filled)
{
	int result = ACPH_SUCCESS;
	int     fd = -1;
	char_t *rtc_io_buf = NULL;
	char_t *rtc_io_buf_base = NULL;

	if(NULL != resp_buf_bytes_filled)
	{
		uint32_t nReqBufLen = 0;
		int32_t nRespBufLen = 0;
		uint8_t *nOutputBufPos = resp_buf_ptr;
		uint32_t handle = 0;
		int32_t codec_idx = 0;
		uint32_t nTotalRegisters = 0;
		int32_t lRegValue = 0;
		uint32_t ultempRegAddr = 0;
		size_t numBytes = 0;
		uint32_t i=0;
		char_t *pInputBuf = (char_t *)req_buf_ptr;
		char_t *pCurInputBuf = NULL;
		int32_t rtc_io_buf_size = 0;
		uint32_t non_reg_fields = 2;

		nReqBufLen = req_buf_len;
		if(nReqBufLen < ACPH_CAL_DATA_UNIT_LENGTH)
		{
			LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->insufficient length of req buffer to get data\n");
			result = ACPH_ERR_LENGTH_NOT_MATCH;
			goto done;
		}
		else
		{
			char_t reg[5], val[3];
			reg[4] = '\0', val[2] = '\0';
			uint32_t offset = 0;
			uint32_t count = 0;
			uint32_t regAddr = 0;
			uint32_t regMask = 0;
			uint32_t found = 1;
			int reg_len;
			char *save;
			char *token;

			pCurInputBuf = pInputBuf;
			if ((adie_rtac_major_version == 1) && (adie_rtac_minor_version == 0)) {
				handle = 1;
				non_reg_fields = 1;
			} else {
				memcpy(&handle, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
				LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->handle %d",
					handle);
			}
			memcpy(&nTotalRegisters, pCurInputBuf,ACPH_CAL_DATA_UNIT_LENGTH);
			pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;

			codec_idx = find_codec_index(handle);
			if (codec_idx < 0) {
				LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->could not find codec index for handle %d",
					handle);
				goto done;
			} else if (strlen(codec_info[codec_idx].reg_path) == 0) {
				LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->codec path is empty %d",
					handle);
				goto done;
			}

			if((0<nTotalRegisters)&&
			   ((nTotalRegisters*2*ACPH_CAL_DATA_UNIT_LENGTH) +
				non_reg_fields*ACPH_CAL_DATA_UNIT_LENGTH == nReqBufLen))
			{
				fd = open(codec_info[codec_idx].reg_path, O_RDWR);
				if(fd < 0)
				{
					result = ACPH_ERR_ADIE_INIT_FAILURE;
					LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->ERROR! cannot open adie peek error: %d, path: %s",
						fd, codec_info[codec_idx].reg_path);
					goto done;
				}
				result = parse_codec_reg_file(&rtc_io_buf_base, &rtc_io_buf_size, "ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS", fd, codec_idx);
				if (rtc_io_buf_base == NULL || result < 0)
				{
					result = ACPH_ERR_ADIE_INIT_FAILURE;
					LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->ERROR! cannot allocate memory: %d, path: %s",
							READ_STEP_SIZE, codec_info[codec_idx].reg_path);
					goto close_fd;
				}
				if (rtc_io_buf_size <= 0)
				{
					result = ACPH_ERR_ADIE_GET_CMD_FAILURE;
					LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->ERROR! length of written bytes does not match expected value %d", rtc_io_buf_size);
					goto close_fd;
				}
				LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->codec_reg file size =%d",rtc_io_buf_size);
				numBytes = rtc_io_buf_size;
				rtc_io_buf = rtc_io_buf_base;
			}
			/* Perform search for get registers */
			for(i=0; i<nTotalRegisters && found; i++)
			{
				memcpy(&regAddr,pCurInputBuf,ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
				memcpy(&regMask,pCurInputBuf,ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
				LOGE("GET_MULTI-Reg ==> Reg[%08X],Mask[%08X]\n",
						 regAddr, regMask);
				offset = 0;
				found = 0;
				memcpy((void*)reg, (void*)(rtc_io_buf+offset), sizeof(uint32_t));
				token = strtok_r(reg, ":", &save);
				if (token == NULL) {
					LOGE("%s: strtok failed to find register length : delimiter!\n", __func__);
					result = -EINVAL;
					goto done;
				}
				reg_len = strlen(token);
				if (reg_len <= 0) {
					LOGE("%s: register length is %d!\n", __func__, reg_len);
					result = -EINVAL;
					goto done;
				}
				register_length = reg_len;
				LOGD("%s: valid register length is %d\n", __func__, register_length);
				while(numBytes>offset)
				{
					memcpy((void*)reg, (void*)(rtc_io_buf+offset), sizeof(uint32_t));
					offset += register_length;
					ultempRegAddr = strtoul(reg, NULL, 16);
					offset += 2;
					memcpy((void*)val, (void*)(rtc_io_buf+offset), sizeof(uint16_t));
					lRegValue = strtol(val, NULL, 16);
					offset += 3;
					if (ultempRegAddr >= CDC_REG_DIG_BASE_READ)
						ultempRegAddr -= CDC_REG_DIG_OFFSET;

					if(ultempRegAddr == regAddr)
					{
						count++;
						found = 1;
						/* return a masked value */
						lRegValue &= regMask;
						memcpy((void *)nOutputBufPos,(const void *)&lRegValue, sizeof(uint32_t));
							   nOutputBufPos += ACPH_CAL_DATA_UNIT_LENGTH;
						usleep(30);
						LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->reg[%08X],val[%08X], count[%d]\n",
								   ultempRegAddr, lRegValue,count);
						break;
					}
				}
			}
			if (found == 0)
			{
				LOGE("GetMultipleAdieReg failed because reg[%08x] is not found\n",regAddr);
				result = ACPH_ERR_ADIE_GET_CMD_FAILURE;
				goto close_fd;
			}
			if(ACPH_SUCCESS==result && nTotalRegisters == count)
			{
				nRespBufLen = sizeof(uint32_t)*(count);
				memcpy((void *)resp_buf_bytes_filled,(const void *)&nRespBufLen,sizeof(int32_t));
				LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->Success\n");
			}
			else
			{
				LOGE("[ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS]->Error in lengths of input or output buffers or total registers\n");
				result = ACPH_ERR_UNKNOWN_REASON;
				goto close_fd;
			}
		}
	}
	else
	{
		result = ACPH_FAILURE;
	}
close_fd:
	close(fd);
done:
	free(rtc_io_buf_base);
	return result;

}

static int set_multiple_adie_registers(uint8_t *req_buf_ptr, uint32_t req_buf_len,
				uint32_t *resp_buf_bytes_filled)
{
	int result = ACPH_SUCCESS;
	int     fd = -1;

	if(NULL != resp_buf_bytes_filled)
	{
		uint32_t nReqBufLen = 0;
		int32_t nRespBufLen = 0;
		char_t *pInputBuf = (char_t *)req_buf_ptr;
		char_t *pCurInputBuf = NULL;
		uint32_t nTotalRegisters = 0;
		uint32_t ulRegValue = 0;
		uint32_t handle = 0;
		int32_t codec_idx = 0;
		uint32_t regAddr = 0;
		uint32_t regMask = 0;
		size_t numBytes1 = 0;
		size_t numBytes2 = 0;
		uint32_t non_reg_fields = 2;
		uint32_t i=0;

		nReqBufLen = req_buf_len;
		if(nReqBufLen < ACPH_CAL_DATA_UNIT_LENGTH)
		{
			LOGE("[ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS]->insufficient length of req buffer to get data\n");
			result = ACPH_ERR_LENGTH_NOT_MATCH;
			goto done;
		}
		else
		{
			pCurInputBuf = pInputBuf;
			if ((adie_rtac_major_version == 1) && (adie_rtac_minor_version == 0)) {
				handle = 1;
				non_reg_fields = 1;
			} else {
				memcpy(&handle, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);
				pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
			}
			memcpy(&nTotalRegisters,pCurInputBuf,ACPH_CAL_DATA_UNIT_LENGTH);
			pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;

			codec_idx = find_codec_index(handle);
			if (codec_idx < 0) {
				LOGE("[ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS]->could not find codec index for handle %d",
					handle);
				goto done;
			} else if (strlen(codec_info[codec_idx].reg_path) == 0) {
				LOGE("[ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS]->codec path is empty %d",
					handle);
				goto done;
			}

			if((0<nTotalRegisters)&&
			   (nReqBufLen==(nTotalRegisters*3*ACPH_CAL_DATA_UNIT_LENGTH)+
				non_reg_fields*ACPH_CAL_DATA_UNIT_LENGTH)
			   )
			{
				for(i=0;i<nTotalRegisters;i++)
				{
					char *temp = NULL;
					memcpy(&regAddr,pCurInputBuf,ACPH_CAL_DATA_UNIT_LENGTH);
					pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
					memcpy(&regMask,pCurInputBuf,ACPH_CAL_DATA_UNIT_LENGTH);
					pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
					memcpy(&ulRegValue, pCurInputBuf, ACPH_CAL_DATA_UNIT_LENGTH);
					pCurInputBuf += ACPH_CAL_DATA_UNIT_LENGTH;
					LOGE("SET_MULTI-Reg ==> Reg[%08X],Val[%08X], Mask[%08X]\n",
						 regAddr, ulRegValue, regMask);
					/* set the value as masked one*/
					ulRegValue &= regMask;

					if (regAddr >= CDC_REG_DIG_BASE_WRITE)
						regAddr += CDC_REG_DIG_OFFSET;

					numBytes1 = asprintf(&temp, "0x%x 0x%x", regAddr, ulRegValue);

					fd = open(codec_info[codec_idx].reg_path, O_RDWR);
					if(fd < 0)
					{
						LOGE("[ACPH_CMD_GET_ADIE_REGISTER]->ERROR! cannot open adie poke error: %d, path: %s",
							fd, codec_info[codec_idx].reg_path);
						result = ACPH_ERR_ADIE_INIT_FAILURE;
						if (temp != NULL)
						{
							free(temp);
							temp=NULL;
						}
						goto done;
					}
					numBytes2 = write(fd, temp, numBytes1);
					if (temp != NULL)
					{
						free(temp);
						temp=NULL;
					}
					close(fd);
					/* make sure the write is successful */
					if (numBytes1 != numBytes2)
					{
						LOGE("[ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS]->set multi register failed, numBytes1[%zu],numBytes2[%zu]\n",
									   numBytes1, numBytes2);
						result = ACPH_ERR_ADIE_GET_CMD_FAILURE;
						goto done;
					}
					usleep(30);
				}
				*resp_buf_bytes_filled = 0;
				LOGE("[ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS]->success\n");
			}
			else
			{
				LOGE("[ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS]->Error in lengths of input or output buffers or total registers\n");
				result = ACPH_ERR_UNKNOWN_REASON;
				goto done;
			}
		}
	}
	else
	{
		result = ACPH_FAILURE;
	}
done:
	return result;
}

int32_t adie_rtac_callback(uint16_t cmd, uint8_t *req_buf_ptr,
				uint32_t req_buf_len, uint8_t *resp_buf_ptr,
				uint32_t resp_buf_len, uint32_t *resp_buf_bytes_filled)
{
	int result = ACPH_SUCCESS;

	if(!found_codec_path)
		if (find_codecs() < 0)
			LOGE("failed to get register paths\n");

	switch(cmd) {
	case ACPH_CMD_QUERY_ADIE_RTC_VERSION:
		((ACPH_CMD_QUERY_ADIE_RTC_VERSION_rsp *)resp_buf_ptr)->adie_rtc_major_version = adie_rtac_major_version;
		((ACPH_CMD_QUERY_ADIE_RTC_VERSION_rsp *)resp_buf_ptr)->adie_rtc_minor_version = adie_rtac_minor_version;
		*resp_buf_bytes_filled = sizeof (ACPH_CMD_QUERY_ADIE_RTC_VERSION_rsp);
		break;
	case ACPH_CMD_GET_CODECS_INFO:
		result = get_codecs_info(resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled);
		break;
	case ACPH_CMD_GET_ADIE_REGISTER:
	case ACPH_CMD_GET_ADIE_REGISTER_V2:
		result = get_adie_register(req_buf_ptr, req_buf_len, resp_buf_ptr,
					   resp_buf_len, resp_buf_bytes_filled);
		break;
	case ACPH_CMD_SET_ADIE_REGISTER:
	case ACPH_CMD_SET_ADIE_REGISTER_V2:
		result = set_adie_register(req_buf_ptr, req_buf_len, resp_buf_bytes_filled);
		break;
	case ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS:
	case ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS_V2:
		result = get_multiple_adie_registers(req_buf_ptr, req_buf_len, resp_buf_ptr,
					    resp_buf_bytes_filled);
		break;
	case ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS:
	case ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS_V2:
		result = set_multiple_adie_registers(req_buf_ptr, req_buf_len,
					   resp_buf_bytes_filled);
		break;
	default:
		LOGE("Cannot recognize the ACPH_ADIE command\n");
		result = ACPH_ERR_INVALID_COMMAND;
		break;
	}
done:
	return result;
}

void adie_rtac_init(void)
{
	int ret;

	ret = acph_register_command(ACPH_ADIE_RTC_REG_SERVICEID, adie_rtac_callback);
	if (ret < 0) {
		LOGE("ACDB RTAC -> acph register failed error = %d\n", ret);
	}

	return;
}

void adie_rtac_exit(void)
{
	int ret;

	ret = acph_deregister_command(ACPH_DSP_RTC_REG_SERVICEID);
	if (ret < 0)
		LOGE("ACDB RTAC -> ERROR: adie_rtac_exit, acph_deregister_command failed err = %d\n", ret);
}

