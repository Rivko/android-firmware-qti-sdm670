/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/******************************************************************************
  @file    mbn_utils.c
  @brief   utils for parsing MBN files

  DESCRIPTION
    Provide interface to get MBN configuration information.

******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <utils/Log.h>
#include "mbn_utils.h"
#ifdef QMI_RIL_UTF
#include "log/log.h"
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "MBN_UTIL"

#define MBN_LOG_INFO                    RLOGI
#define MBN_LOG_ERROR                   RLOGE

// MBN file related
#define MCFG_MBN_CONFIG_LEN_OFFSET      10
#define MCFG_MBN_CONFIG_DESC_OFFSET     12
#define MCFG_MBN_QC_VERSION_TYPE_ID     5
#define MCFG_MBN_OEM_VERSION_TYPE_ID    1

#define MCFG_MBN_CONFIG_INFO_MAGIC      "MCFG_TRL"
#define MCFG_MBN_CONFIG_INFO_MAGIC_LEN  8

/*===========================================================================

  FUNCTION: mcfg_get_config_info

===========================================================================*/
/*!
    @brief
    This function will get the config_info for a given MBN file

    @return
    MBN_PARSE_FAILURE -> Failed  MBN_PARSE_SUCCESS -> Success

    @side effect
    If success, the function will allocate the memory, assigned to parameter
    config_info. The caller should free it after use.
*/
/*=========================================================================*/
static int mcfg_get_config_info(const char* file_name, char** config_info, uint32_t* config_len)
{
  int fd = -1;
  int result = MBN_PARSE_FAILURE;
  off_t pos;
  uint16_t len;
  uint32_t info_offset;
  char* buf;

  *config_info = NULL;
  *config_len = 0;

  do {
    if ( (fd = open( file_name, O_RDONLY )) == -1 )
    {
      RLOGE("Failed to open file name %s: %s", file_name, strerror(errno));
      break;
    }
    pos = lseek(fd, -4, SEEK_END);
    if ( pos == -1 )
    {
      RLOGE("Failed to seek file: %s", strerror(errno));
      break;
    }
    if ( read(fd, &info_offset, 4) == -1 )
    {
      RLOGE("Failed to read the offset where stores config_info: %s",strerror(errno));
      break;
    }

    /* just jump to the pos that stores length */
    pos = lseek(fd, 0 - (off_t)info_offset + MCFG_MBN_CONFIG_LEN_OFFSET, SEEK_END);
    if ( pos == -1 )
    {
      RLOGE("Failed to seek file: %s", strerror(errno));
      break;
    }
    if ( read(fd, (uint8_t*)&len, sizeof(len)) == -1 )
    {
      RLOGE("Failed to read the length of config_info: %s", strerror(errno));
      break;
    }
    /* now the file pointer points to config_info */
    if ( (buf = malloc(len)) == NULL )
    {
      RLOGE("Failed to allocate the memory: %s", strerror(errno));
      break;
    }
    if ( read(fd, buf, len) != len )
    {
      free(buf);
      RLOGE("Failed to read the config_info: %s", strerror(errno));
      break;
    }

    *config_info = buf;
    *config_len = len;
    result = MBN_PARSE_SUCCESS;
  } while (0);

  if (fd != -1)
    close(fd);
  return result;
}

/*===========================================================================

  FUNCTION: mcfg_get_tr_tlv

===========================================================================*/
/*!
    @brief
    This function will get the TLV for a given MBN file and specified
    type id;

    @return
    MBN_PARSE_FAILURE -> Failed  MBN_PARSE_SUCCESS -> Success

    @side effect
    If success, the function will allocate the memory, assigned to parameter
    value. The caller should free it after use.
*/
/*=========================================================================*/
static int mcfg_get_tr_tlv(const char* file_name, const char type, char** value, uint32_t* len)
{
  char* config_info = NULL;
  int result = MBN_PARSE_FAILURE;
  uint32_t index = 0;
  uint32_t config_info_len;
  uint16_t length;
  char* buf;

  *value = NULL;
  *len = 0;

  do
  {
    MBN_LOG_INFO("file name: %s", file_name);
    if (mcfg_get_config_info(file_name, &config_info, &config_info_len) != MBN_PARSE_SUCCESS)
    {
      RLOGE("Failed to get config_info from file: %s", file_name);
      break;
    }
    /* skip the the MAGIC number part */
    index += MCFG_MBN_CONFIG_INFO_MAGIC_LEN;
    while ( index < config_info_len - 3 )
    {
      MBN_LOG_INFO("type_id: %d", config_info[index]);
      if (config_info[index] == type)
        break;
      index++;
      length = (uint16_t)config_info[index] + (((uint16_t)config_info[index+1]) << 8);
      index += 2 + length;
    }
    /* TLV is not found */
    if ( index >= ( config_info_len - 3 ) )
    {
      RLOGE("Failed to found TLV with type id: 0x%02x", type);
      break;
    }

    index++;
    length = (uint16_t)config_info[index] + (((uint16_t)config_info[index+1]) << 8);
    if ( ( buf = malloc(length) ) == NULL )
    {
      RLOGE("Failed to allocate memory for type id: 0x%02x", type);
      break;
    }
    memcpy(buf, config_info+index+2, length);
    *value = buf;
    *len = length;
    result = MBN_PARSE_SUCCESS;
  } while (0);

  if (config_info)
    free(config_info);

  return result;
}

/*===========================================================================

  FUNCTION: mcfg_get_qc_version

===========================================================================*/
/*!
    @brief
    This function will get the qc_version for a given MBN file

    @return
    MBN_PARSE_FAILURE -> Failed  MBN_PARSE_SUCCESS -> Success

    @side effect
    If success, the function will allocate the memory, assigned to parameter
    qc_version. The caller should free it after use.
*/
/*=========================================================================*/
int mcfg_get_qc_version(char* file_name, char** qc_version, uint32_t* len)
{

  return mcfg_get_tr_tlv(file_name, MCFG_MBN_QC_VERSION_TYPE_ID,
                                qc_version, len);
}

/*===========================================================================

  FUNCTION: mcfg_get_oem_version

===========================================================================*/
/*!
    @brief
    This function will get the oem_version for a given MBN file

    @return
    MBN_PARSE_FAILURE -> Failed  MBN_PARSE_SUCCESS -> Success

    @side effect
    If success, the function will allocate the memory, assigned to parameter
    oem_version. The caller should free it after use.
*/
/*=========================================================================*/
int mcfg_get_oem_version(char* file_name, char** oem_version, uint32_t* len)
{

  return mcfg_get_tr_tlv(file_name, MCFG_MBN_OEM_VERSION_TYPE_ID,
                                oem_version, len);
}
