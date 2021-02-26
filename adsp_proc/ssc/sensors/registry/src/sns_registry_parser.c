/*=============================================================================
  @file sns_registry_parser.c

  JSON parser for registry and configuration files.

  A complete list of known registry groups (files) is created at boot-up, and
  maintained throughout the life of the SEE.  All read/parsed files and incoming
  write requests are first stored in "temporary" registry group objects, and
  then merged into the "main" registry group list.  This is to ensure parsing
  failures do not corrupt the registry state.

  All write requests require first reading and parsing the applicable file,
  making the requested changes, encoding the result, and writing it back to the
  file system.

  A potential optimization may be to read and parse all registry files at
  initialization, and keep the parsed version in memory.

  PEND: Use registry sensor as argument to PRINTF

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "sns_assert.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_registry_osa.h"
#include "sns_registry_parser.h"
#include "sns_types.h"

/*=============================================================================
  Macros
  ===========================================================================*/

/* errno is not presently available on Hexagon */
#ifndef errno
static int errno = 0;
#define ENOENT 0
#endif

/**
 * When saving a registry group to file, the size of the string to initially
 * allocate. If the buffer turns-out to be too short, we'll re-allocate a
 * buffer twice as large, and try again.  We want this buffer size to be
 * sufficient 99% of the time (and we don't care if we waste memory, as this
 * allocation will only sit on the heap temporarily.
 */
#define SNS_REGISTRY_GRP_LEN_INIT 4000

/*  Maximum length of any full item, group, or file name. */
#define SNS_REGISTRY_STR_MAX 200

/* Maximum number of entries of the config file */
#define MAX_CONFIG_ENTRIES 30

/* Allocate a new string, and concatenate the three arguments */
#define STR_CONCAT(var_name, first, second, third) \
  int total_size = strlen(first) + strlen(second) + strlen(third) + 1; \
  if(total_size > SNS_REGISTRY_STR_MAX) { total_size = 1; } \
  char var_name[total_size]; \
  sns_strlcpy(var_name, first, sizeof(var_name)); \
  sns_strlcat(var_name, second, sizeof(var_name)); \
  sns_strlcat(var_name, third, sizeof(var_name));

/* parser will loop below number of times before bail out if registry version
   got changed via OTA to have new changes/android property items picked up*/
#define MAXNUM_OF_ITERATIONS_FOR_VERSION 30

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* Function to be called on a particular file */
typedef sns_rc (*file_func)(char const *restrict dir,
    char const *restrict file_name, size_t file_size, bool is_config);

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

/* List of all known registry groups */
static sns_isafe_list sns_registry_groups;

/* Configuration file path on targe*/
static char registry_config_file[SNS_REGISTRY_DIR_LEN_MAX] = "/persist/sensors/registry/sns_reg_config";

/*config file to be in /vendor/etc folder for OTA as persist image is not touched*/
static char registry_config_file_ota[SNS_REGISTRY_DIR_LEN_MAX] = "/vendor/etc/sensors/sns_reg_config";
static char registry_config_file_le[SNS_REGISTRY_DIR_LEN_MAX] = "/system/etc/sensors/sns_reg_config";

/* below stores the current registry version on the target and compared with the version in sns_reg_config came from OTA*/
static char current_registry_version[SNS_REGISTRY_DIR_LEN_MAX] = "/persist/sensors/registry/sns_reg_version";

/* Registry elements to be picked from sns_reg_config file */
static sns_registry_hw dynamic_hw_info[MAX_CONFIG_ENTRIES];

/* Length of dynamic_hw_info */
static uint32_t dynamic_hw_info_len;

// PEND: The following is a HORRIBLE idea, and should be removed
/* Is set to true if a zero-length file is found in /registry */
static bool zero_length_file_found = false;

/*=============================================================================
  External Variable Definitions
  ===========================================================================*/

// PEND: Remove the following static definitions, and use only dynamic above
// All defined in sns_registry_config.c
extern char const persist_mnt_name[];
extern char const data_dir_name[];
extern char registry_dir[SNS_REGISTRY_DIR_LEN_MAX];
extern char config_dir[SNS_REGISTRY_DIR_LEN_MAX];
extern char const config_dir_new[];
extern char const config_dir_le[];
extern sns_registry_hw sns_registry_hw_info[];
extern const uint32_t sns_registry_hw_info_len;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Determine how many characters of whitespace/formatting are present from
 * the start of a string.  Does not check for null-terminating character.
 *
 * @param[i] str Character string whose whitespace we wish to skip
 * @param[i] str_len Length of str
 *
 * @return How many of the next characters should be considered whitespace
 *         0 if whitespace continues for str_len characters
 */
static int
whitespace_len(char const *str, uint32_t str_len)
{
  uint32_t cidx = 0;

  while(0 != isspace(str[cidx]))
  {
    if(cidx >= str_len)
      return 0;
    ++cidx;
  }

  return cidx;
}

/**
 * Given a JSON string representing a data type, determine the associated
 * enum value.
 *
 * @param[i] str Null-terminated JSON string
 *
 * @return Associated enum value; or SNS_REGISTRY_TYPE_INVALID
 */
static sns_registry_data_type
get_data_type(char const *str)
{
  if(0 == strncmp(str, "int", strlen("int")))
    return SNS_REGISTRY_TYPE_INTEGER;
  else if(0 == strncmp(str, "flt", strlen("flt")))
    return SNS_REGISTRY_TYPE_FLOAT;
  else if(0 == strncmp(str, "grp", strlen("grp")))
    return SNS_REGISTRY_TYPE_GROUP;
  else if(0 == strncmp(str, "str", strlen("str")))
    return SNS_REGISTRY_TYPE_STRING;
  else
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Unknown data type '%s'", str);
    return SNS_REGISTRY_TYPE_INVALID;
  }
}

/**
 * Given a value read from a JSON file (and item->type) appropriately parse,
 * determine, and set the registry item value.
 *
 * We only create the subgroup if this item is parsed successfully.
 *
 * @param[io] item Temporary registry item to set
 * @param[i] value Item value to parse
 *
 * @return True if parsing was successful; false otherwise
 */
static bool
set_item_data(sns_registry_item *item, char const *value)
{
  bool rv = true;
  if(SNS_REGISTRY_TYPE_INTEGER == item->type)
  {
    errno = 0;
    item->data.sint = strtol(value, NULL, 0);
    if(0 != errno)
      SNS_SPRINTF(ERROR, sns_fw_printf, "strol error %i (%s)",
          errno, strerror(errno));
    else
      rv = true;
  }
  else if(SNS_REGISTRY_TYPE_FLOAT == item->type)
  {
    item->data.flt = atof(value);
    rv = true;
  }
  else if(SNS_REGISTRY_TYPE_STRING == item->type)
  {
    item->data.str = (char*)value;
    rv = true;
  }
  else if(SNS_REGISTRY_TYPE_GROUP == item->type)
  {
    item->data.group = NULL;
    rv = true;
  }
  else
    SNS_PRINTF(ERROR, sns_fw_printf, "Unknown data type %i", item->type);

  return rv;
}

/**
 * Create and merge a group which prohibits any further write access. Called
 * when there is a read/parsing error, and we do not wish to overwrite the
 * existing file from a configuration file or registry write request.
 *
 * @param[i] group_name Name of the unwritable group
 */
static void
create_unwritable_group(char const *group_name)
{
  sns_registry_group *group = sns_registry_group_create(group_name);
  group->writable = false;
  sns_registry_group_merge(group);
  sns_registry_group_free(group);
}

/**
 * Update an existing registry item to a new value.  Follow all appropriate
 * versioning requirements.
 *
 * If this item is of type GROUP, and specifies a valid pointer to a nested
 * group, merge that group into the central registry.
 *
 * @param[i] item Existing item (within an existing registry group)
 * @param[i] new New value to be applied to the existing item
 */
static void
update_item(sns_registry_item *item, sns_registry_item const *new)
{
  SNS_ASSERT(SNS_REGISTRY_TYPE_INVALID != item->type);

  if(0 == item->version || new->version > item->version)
  {
    if(SNS_REGISTRY_TYPE_INTEGER == item->type)
      item->data.sint = new->data.sint;
    else if(SNS_REGISTRY_TYPE_FLOAT == item->type)
      item->data.flt = new->data.flt;
    else if(SNS_REGISTRY_TYPE_STRING == item->type)
    {
      sns_free(item->data.str);

      item->data.str = sns_malloc(SNS_HEAP_MAIN, strlen(new->data.str) + 1);
      SNS_ASSERT(NULL != item->data.str);
      sns_strlcpy(item->data.str, new->data.str, strlen(new->data.str) + 1);
    }

    item->version = new->version;
  }

  if(SNS_REGISTRY_TYPE_GROUP == item->type)
  {
    SNS_ASSERT(NULL != new->data.group);
    sns_registry_group_merge((sns_registry_group*)new->data.group);

    item->data.group = sns_registry_group_find(new->data.group->name);
    SNS_ASSERT(NULL != item->data.group);
  }
}

/**
 * Parse a tuple, where that tuple is one of:
 * 1) "String":"String"
 *  Set 'key' to the beginning of the first string; 'value' to the second
 *
 * 2) "String":Object
 * 3) "String":Array
 *  Set 'key' to the beginning of the string.
 *
 * Null-terminate all strings, by replacing the closing '"' with '\0'.
 *
 * @param[io] json Beginning of the string to parse
 * @param[i] json_len Lenght of the json string
 * @param[o] key First string with quote symbols removed
 * @param[o] value Second string without quote symbols, or NULL
 *
 * @return Number of bytes processed; 0 upon any syntax error
 * 1) Includes key/value, colon, quotes, and whitespace, and the next character will either be a ',' or '}'
 * 2/3) Includes key, quote symbols, whitespace, and colon.  Next character will be a '{' or '['
 */
static uint32_t
parse_pair(char *json, uint32_t json_len,
    char **restrict key, char **restrict value)
{
  uint32_t cidx = 0;
  char *end_string;
  bool failed = false;

  *value = NULL;
  if(cidx >= json_len || '\"' != json[cidx++])
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing opening quote (%i)", cidx - 1);
    failed = true;
  }
  else if(NULL == (end_string = strchr(&json[cidx], '\"')))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing closing quote (%i)", cidx);
    failed = true;
  }
  else
  {
    *key = &json[cidx];
    cidx += (uintptr_t)end_string - (uintptr_t)&json[cidx];
    json[cidx++] = '\0';  // We want to safely use this string later
    cidx += whitespace_len(&json[cidx], json_len - cidx);

    if(cidx >= json_len || ':' != json[cidx++])
    {
      SNS_SPRINTF(ERROR, sns_fw_printf,  "Missing colon '%s' %c", *key, json[cidx]);
      failed = true;
    }
    else
    {
      cidx += whitespace_len(&json[cidx], json_len - cidx);
      if(cidx < json_len && '\"' == json[cidx])
      {
        cidx++;
        *value = &json[cidx];
        end_string = strchr(&json[cidx], '\"');
        if(NULL == end_string)
        {
          SNS_PRINTF(ERROR, sns_fw_printf, "Missing closing quote (%i)", cidx);
          failed = true;
        }
        else
        {
          cidx += (uintptr_t)end_string - (uintptr_t)&json[cidx];
          json[cidx++] = '\0';
          cidx += whitespace_len(&json[cidx], json_len - cidx);
        }
      }
      else if(cidx < json_len && '{' != json[cidx] && '[' != json[cidx])
      {
        SNS_PRINTF(ERROR, sns_fw_printf,
            "Next element is not a quote or opening bracket (%i)", cidx);
        failed = true;
      }
    }
  }

  return failed ? 0 : cidx;
}

/**
 * Determine whether a configuration file should be loaded, based on whether
 * it has modified since the last time it was parsed.
 *
 * @param[i] config_name Name of the configuration .json file
 * @param[i] modified_name st_mtime as returned from stat()
 * @param[i] save Whether to save the modified_time into the registry
 *
 * @return true if file should be read/parsed; false otherwise
 */
static const char config_group_name[] = "sns_reg_config";
static bool
process_config_file(char const *config_name, time_t modified_time, bool save)
{
  static sns_registry_group *config_group = NULL;
  sns_registry_item *config_item;

  config_group = sns_registry_group_find(config_group_name);
  if(NULL == config_group)
    config_group = sns_registry_group_create(config_group_name);

  config_item = sns_registry_item_find(config_group, config_name);
  if(NULL == config_item || (uint64_t)config_item->data.sint != modified_time)
  {
    sns_registry_group *temp_group = sns_registry_group_create(config_group_name);
    sns_registry_item temp_item;

    temp_item.name = (char*)config_name;
    temp_item.version = 0;
    temp_item.type = SNS_REGISTRY_TYPE_INTEGER;
    temp_item.data.sint = modified_time;

    sns_registry_item_create(temp_group, &temp_item);
    if(save)
      sns_registry_group_merge(temp_group);
    sns_registry_group_free(temp_group);
    return true;
  }
  else
  {
    return zero_length_file_found;  // Typically false
  }
}

/**
 * Perform an action for every file found within the given directory.
 *
 * This function will be called recursively for subdirectories.  Note that
 * recursive searching is only appropriate for configuration files.
 *
 * @param[i] dir_path Directory in which to search
 * @param[i] cb_func Call this function for each file found
 * @param[i] is_config Whether the contained are configuration files
 *
 * @return
 *  SNS_RC_SUCCESS
 *  SNS_RC_INVALID_TYPE Directory cannot be found or is not available
 */
static sns_rc
foreach_in_dir(char const *dir_path, file_func cb_func, bool is_config)
{
  sns_rc rv = SNS_RC_SUCCESS;
  DIR *dir = opendir(dir_path);

  SNS_SPRINTF(LOW, sns_fw_printf, "Searching '%s'", dir_path);
  if(NULL == dir)
  {
    rv = SNS_RC_INVALID_TYPE;
    SNS_SPRINTF(ERROR, sns_fw_printf, "Directory '%s' not available", dir_path);
  }
  else
  {
    struct dirent *ent;

    errno = 0;
    while(NULL != (ent = readdir(dir)) && rv == SNS_RC_SUCCESS)
    {
      struct stat stat_buf;
      STR_CONCAT(full_path, dir_path, "/", ent->d_name);

      if(0 != errno)
      {
        rv = SNS_RC_INVALID_TYPE;
        SNS_SPRINTF(ERROR, sns_fw_printf, "readdir error %i (%s)",
            errno, strerror(errno));
      }
      else if(0 == strcmp(ent->d_name, ".") || 0 == strcmp(ent->d_name, ".."))
      { }
      else if(0 == stat(full_path, &stat_buf))
      {
        if(S_ISDIR(stat_buf.st_mode))
        {
          rv = foreach_in_dir(full_path, cb_func, is_config);
        }
        else if(0 == stat_buf.st_size)
        {
          SNS_PRINTF(ERROR, sns_fw_printf, "Ignoring file of size 0: %s",
              ent->d_name);
          zero_length_file_found = true;
        }
        else if(S_ISREG(stat_buf.st_mode))
        {
          if(is_config &&
             process_config_file(full_path, stat_buf.st_mtime, false))
          {
            sns_rc rc = cb_func(dir_path, ent->d_name, stat_buf.st_size, is_config);
            if(SNS_RC_NOT_AVAILABLE != rc)
              process_config_file(full_path, stat_buf.st_mtime, true);
          }
          else if(!is_config)
          {
            cb_func(dir_path, ent->d_name, stat_buf.st_size, is_config);
          }
        }
      }
      else
      {
        if(!is_config)
          create_unwritable_group(ent->d_name);

        SNS_SPRINTF(ERROR, sns_fw_printf, "stat error for '%s' %i (%s)",
            ent->d_name, errno, strerror(errno));
      }
      errno = 0;
    }
    closedir(dir);
  }

  SNS_SPRINTF(LOW, sns_fw_printf, "Searching '%s' completed", dir_path);
  return rv;
}

/**
 * This function checks and returns true if "persist_mnt_name" is available.
 */
static bool
is_persit_partition_mounted(void)
{
  DIR *dir = opendir(persist_mnt_name);
  if(dir != NULL)
  {
    closedir(dir);
    return true;
  }
  else if(ENOENT == errno)
    return false;
  else
    return true;
}

/**
 * Check if the given directory exists.  If not, create it.
 *
 * @note Ideally this would be performed by the init scripts; but we'll keep
 * it here for the time being.
 *
 * @param[i] dir Directory to check/create
 *
 * @return True if directory exists or was created successfully
 */
static bool
check_dir(char const *dir)
{
  bool rv = false;
  struct stat stat_buf;

  if(0 != stat(dir, &stat_buf))
  {
    if(ENOENT == errno) // If the directory does not exist
    {
      // Hexagon has wrong protype of mkdir which requires non-const dir
      if(0 != mkdir((char*)dir, 0777))
        SNS_SPRINTF(ERROR, sns_fw_printf, "mkdir error: '%s' (%i)", dir, errno);
      else
        rv = true;
    }
    else
      SNS_SPRINTF(ERROR, sns_fw_printf, "stat error '%s' %i", dir, errno);
  }
  else if(S_ISDIR(stat_buf.st_mode))
    rv = true;
  else
    SNS_SPRINTF(ERROR, sns_fw_printf, "Path is not a directory '%s'", dir);

  return rv;
}

/**
 * Read and parse a registry item from a JSON string.
 *
 * This function should consume all text inclusive of the opening and closing
 * brackets.
 *
 * @param[io] group Already created registry group to which to add items/owner
 * @param[i] item_name Previously parsed item name; null-terminated
 * @param[i] json JSON text to parse; first character is the opening bracket
 * @param[i] json_len Length of the json string
 *
 * @return How many characters were consumed during processing;
 *         0 upon any syntax/parsing error
 */
static uint32_t
parse_item(sns_registry_group *group, char const *item_name,
    char *json, uint32_t json_len)
{
  uint32_t cidx = 0;
  uint32_t result = 0;
  sns_registry_item item = (sns_registry_item)
    { .name = (char*)item_name, .type = SNS_REGISTRY_TYPE_INVALID };

  if(cidx >= json_len || '{' != json[cidx++])
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing opening bracket (%i)", cidx - 1);
  else
  {
    do
    {
      char *key, *value;
      uint32_t len;

      cidx += whitespace_len(&json[cidx], json_len - cidx);
      len = parse_pair(&json[cidx], json_len - cidx, &key, &value);
      cidx += len;

      if(0 == len || NULL == key || NULL == value)
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Unable to parse pair (%i)", cidx);
        break;
      }
      else
      {
        if(0 == strncmp(key, "type", strlen(key)))
        {
          item.type = get_data_type(value);
          result |= 1 << 0;
        }
        else if(0 == strncmp(key, "ver", strlen(key)))
        {
          errno = 0;
          item.version = strtol(value, NULL, 0);
          if(0 == errno)
            result |= 1 << 1;
          else
            SNS_SPRINTF(ERROR, sns_fw_printf, "strol error %i (%s)",
                errno, strerror(errno));
        }
        else if(0 == strncmp(key, "data", strlen(key)))
        {
          // We require that "type" comes before "data" in the JSON file
          if(set_item_data(&item, value))
            result |= 1 << 2;
        }
      }
      cidx += whitespace_len(&json[cidx], json_len - cidx);
    } while(cidx < json_len && ',' == json[cidx] && cidx++);

    if(cidx >= json_len || '}' != json[cidx++])
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Missing closing bracket (%i)", cidx);
      cidx = 0;
    }

    if(7 == result)
    {
      if(SNS_REGISTRY_TYPE_GROUP == item.type)
      {
        SNS_ASSERT(NULL == item.data.group);
        STR_CONCAT(full_name, group->name, ".", item.name);
        item.data.group = sns_registry_group_create(full_name);
        item.data.group->fs_size = 1;  // Don't write this empty group to file
      }

      // PEND: Check for duplicate item name
      sns_registry_item_create(group, &item);
    }
  }

  return 7 != result ? 0 : cidx;
}

/**
 * Read and parse the JSON text representing a group.
 * From the opening bracket until the closing bracket.
 *
 * @param[i] grp_name Previously parsed group name; null-terminated
 * @param[i] json JSON text read from file
 * @param[i] json_len Length of the json character string
 * @param[o] len How many characters were consumed during processing;
 *               0 upon any syntax/parsing error
 *
 * @return Newly created registry group; NULL upon any error
 */
static sns_registry_group*
parse_group(char const *grp_name, char *json, uint32_t json_len, uint32_t *len)
{
  uint32_t cidx = 0;
  bool failed = false;
  sns_registry_group *group = sns_registry_group_create(grp_name);

  SNS_ASSERT(NULL != group);
  *len = 0;

  if(cidx >= json_len || '{' != json[cidx++])
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing opening bracket (%i)", cidx);
    failed = true;
  }
  else
  {
    do
    {
      char *key, *value;
      uint32_t len;

      cidx += whitespace_len(&json[cidx], json_len - cidx);
      len = parse_pair(&json[cidx], json_len - cidx, &key, &value);
      cidx += len;

      if(0 == len || NULL == key)
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Unable to parse pair (%i)", cidx);
        failed = true;
        break;
      }
      else if(NULL != value)
      {
        if(0 == strncmp(key, "owner", strlen(key)))
        {
          group->owner = sns_malloc(SNS_HEAP_MAIN, strlen(value) + 1);
          SNS_ASSERT(NULL != group->owner);
          sns_strlcpy(group->owner, value, strlen(value) + 1);
        }
        else
          SNS_SPRINTF(MED, sns_fw_printf, "Parsed unknown pair '%s' '%s'", key, value);
      }
      else
      {
        if('.' == key[0])
        {
          STR_CONCAT(full_name, grp_name, key, "");

          sns_registry_item item = (sns_registry_item)
            { .name = &key[1], .type = SNS_REGISTRY_TYPE_GROUP, .version = 0 };

          item.data.group =
            parse_group(full_name, &json[cidx], json_len - cidx, &len);

          if(NULL != item.data.group && 0 != len)
            sns_registry_item_create(group, &item);

          cidx += len;
        }
        else
        {
          len = parse_item(group, key, &json[cidx], json_len - cidx);
          cidx += len;
        }

        if(0 == len)
        {
          SNS_SPRINTF(LOW, sns_fw_printf, "Error parsing item: '%s'", key);
          failed = true;
          break;
        }
      }
      cidx += whitespace_len(&json[cidx], json_len - cidx);
    } while(',' == json[cidx] && cidx++);

    if(NULL == group->owner)
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Missing group owner");
      failed = true;
    }
    else if(cidx >= json_len || '}' != json[cidx++])
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Missing closing bracket (%i)", cidx);
      failed = true;
    }
  }

  if(failed && NULL != group)
  {
    sns_registry_group_free(group);
    group = NULL;
  }

  *len = cidx;
  return group;
}

/**
 * Given a JSON array, parse and determine whether hw_value matches one of the
 * entries.
 *
 * @param[io] json JSON array in which to search
 * @param[i] json_len Length of json string
 * @param[i] hw_value Hardware value to match against the array; may be NULL
 *
 * @return If match found (or hw_value is NULL): Total string length of the array
 *         (includes everything from the opening to closing brackets (inclusive)
 *         Otherwise 0.
 */
static uint32_t
check_array(char *restrict json, uint32_t json_len,
    char const *restrict hw_value)
{
  bool match_success = false;

  uint32_t cidx = 0;

  if(cidx >= json_len || '[' != json[cidx++])
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing open bracket (%i)", cidx - 1);
    cidx = 0;
  }
  else
  {
    do
    {
      char *end_string;
      cidx += whitespace_len(&json[cidx], json_len - cidx);

      if(cidx >= json_len || '\"' != json[cidx++])
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Missing open parenthesis (%i)", cidx - 1);
        cidx = 0;
        break;
      }

      end_string = strchr(&json[cidx], '\"');
      if(NULL == end_string)
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Missing closing parenthesis (%i)", cidx);
        cidx = 0;
        break;
      }
      else
      {
        char const *temp = &json[cidx];

        cidx += (uintptr_t)end_string - (uintptr_t)&json[cidx];
        json[cidx++] = '\0';
        cidx += whitespace_len(&json[cidx], json_len - cidx);

        if(NULL == hw_value || 0 == strcmp(temp, hw_value))
          match_success |= true;
      }
    } while(cidx < json_len && ',' == json[cidx] && cidx++);
  }

  if(0 != cidx && (cidx >= json_len || ']' != json[cidx++]))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing closing bracket (%i)", cidx - 1);
    cidx = 0;
  }

  return match_success? cidx : 0;
}

/**
 * Parse the "config" block of configuration file.
 * This includes everything between the opening and closing brackets (inclusive)
 *
 * @param[i] json JSON text read from file
 * @param[i] json_len Length of the json character string
 * @param[i] hw_info Information regarding this device hardware
 * @param[i] hw_info_len length of the hw_info
 *
 * @return How many characters were consumed during processing;
 *         0 upon any syntax/parsing error
 *         -1 If config file is not applicable to this device
 */
static int32_t
parse_config(char *json, uint32_t json_len,
    sns_registry_hw const hw_info[], uint32_t hw_info_len)
{
  int32_t cidx = 0;
  uint32_t len;
  char *key, *value;

  if(cidx >= json_len || '{' != json[cidx++])
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing open bracket (%i)", cidx - 1);
    cidx = 0;
  }
  else
  {
    do
    {
      cidx += whitespace_len(&json[cidx], json_len - cidx);

      if('}' == json[cidx])
        break;

      len = parse_pair(&json[cidx], json_len - cidx, &key, &value);
      cidx += len;
      if(0 == len || NULL == key || NULL != value)
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Parsing error");
        cidx = 0;
        break;
      }
      else
      {
        bool found = false;
        char const *hw_value = NULL;

        for(uint32_t i = 0; i < hw_info_len; i++)
        {
          if((strlen(key) == strlen(hw_info[i].name))&&
            (0 == strncmp(key, hw_info[i].name, strlen(key))))
          {
            hw_value = hw_info[i].value;
            found = true;
          }
        }

        if(!found)
        {
          SNS_SPRINTF(ERROR, sns_fw_printf, "Unknown hw constraint: '%s'", key);
          cidx = 0;
          break;
        }

        len = check_array(&json[cidx], json_len - cidx, hw_value);
        cidx += len;
        if(0 == len)
        {
          SNS_PRINTF(MED, sns_fw_printf, "Config file not applicable");
          cidx = -1;
          break;
        }
      }
    } while(cidx < json_len && ',' == json[cidx] && cidx++);
  }

  if(0 < cidx)
    cidx += whitespace_len(&json[cidx], json_len - cidx);
  if(0 < cidx && (cidx >= json_len || '}' != json[cidx++]))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing closing bracket (%i)", cidx - 1);
    cidx = 0;
  }

  return cidx;
}

/**
 * Read and parse all text from a registry or configuration JSON file.
 * This includes everything between the opening and closing brackets (inclusive)
 *
 * @param[i] json JSON text read from file
 * @param[i] json_len Length of the json character string
 * @param[o] groups All groups contained within this file
 * @param[o] is_config Set to true if the parsed file is a config file
 *
 * @return
 *  SNS_RC_SUCCESS
 *  SNS_RC_INVALID_VALUE - Parsing/syntax error encountered
 *  SNS_RC_POLICY - Configuration file does not apply to this device
 */
static sns_rc
parse_file(char *json, uint32_t json_len, sns_isafe_list_iter *groups, bool *is_config)
{
  uint32_t cidx = 0;
  uint32_t len;
  char *key, *value;
  sns_rc rv = SNS_RC_SUCCESS;

  *is_config = false;
  if(cidx >= json_len || '{' != json[cidx++])
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing open bracket (%i)", cidx - 1);
    rv = SNS_RC_INVALID_VALUE;
  }
  else
  {
    do
    {
      cidx += whitespace_len(&json[cidx], json_len - cidx);

      len = parse_pair(&json[cidx], json_len - cidx, &key, &value);
      if(0 == len || NULL == key || NULL != value)
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Parsing error (%i)", cidx);
        rv = SNS_RC_INVALID_VALUE;
        break;
      }

      cidx += len;
      if(0 == strncmp(key, "config", strlen(key)))
      {
        if(0 != dynamic_hw_info_len)
          len = parse_config(&json[cidx], json_len - cidx,
                    dynamic_hw_info,
                    dynamic_hw_info_len);
        else
          len = parse_config(&json[cidx], json_len - cidx,
                      sns_registry_hw_info, sns_registry_hw_info_len);

        cidx += len;

        *is_config = true;
        if(0 == len)
        {
          SNS_PRINTF(ERROR, sns_fw_printf, "Parsing error (%i)", cidx);
          rv = SNS_RC_INVALID_VALUE;
          break;
        }
        else if(-1 == len)
        {
          SNS_PRINTF(MED, sns_fw_printf, "Config file does not apply");
          rv = SNS_RC_POLICY;
          break;
        }
      }
      else
      {
        sns_registry_group *group =
          parse_group(key, &json[cidx], json_len - cidx, &len);
        if(NULL == group)
        {
          SNS_SPRINTF(ERROR, sns_fw_printf, "Error parsing group: %s", key);
          rv = SNS_RC_INVALID_VALUE;
          break;
        }
        else
        {
          sns_isafe_list_iter_insert(groups, &group->list_entry, true);
          cidx += len;
        }
      }
    } while(cidx < json_len && ',' == json[cidx] && cidx++);
  }

  return rv;
}

/**
 * Load a registry or config file, and apply its data to the central registry.
 *
 * @param[i] dir Directory in which file_name is located
 * @param[i] file_name Null-terminated file name string
 * @param[i] file_size File size as determined by stat()
 *
 * SNS_RC_NOT_AVAILABLE - File system error
 * SNS_RC_INVALID_VALUE - Error loading file
 * SNS_RC_SUCCESS - File loaded successfully
 */
static sns_rc
load_file(char const *restrict dir, char const *restrict file_name,
    size_t file_size, bool is_config)
{
  FILE *file;
  sns_rc rv = SNS_RC_SUCCESS;
  STR_CONCAT(full_path, dir, "/", file_name);

  SNS_SPRINTF(LOW, sns_fw_printf, "Loading file: '%s'", full_path);

  file = sns_registry_fopen(full_path, "r");
  if(NULL == file)
  {
    if(!is_config)
      create_unwritable_group(file_name);
    SNS_SPRINTF(ERROR, sns_fw_printf, "unable to open '%s' %i (%s)",
        full_path, errno, strerror(errno));
    rv = SNS_RC_NOT_AVAILABLE;
  }
  else
  {
    size_t read_len;
    sns_rc rc;
    char *buf = sns_malloc(SNS_HEAP_MAIN, file_size);
    SNS_ASSERT(NULL != buf);

    read_len = sns_registry_fread(buf, sizeof(uint8_t), file_size, file);
    if(0 == read_len)
    {
      if(!is_config)
        create_unwritable_group(file_name);
      SNS_SPRINTF(ERROR, sns_fw_printf, "fread error %i (%s)",
            errno, strerror(errno));
      rv = SNS_RC_NOT_AVAILABLE;
    }
    else
    {
      bool parsed_is_config;
      sns_isafe_list groups_list;
      sns_isafe_list_iter groups_iter;

      sns_isafe_list_init(&groups_list);
      sns_isafe_list_iter_init(&groups_iter, &groups_list, true);
      rc = parse_file(buf, read_len, &groups_iter, &parsed_is_config);
      sns_isafe_list_iter_init(&groups_iter, &groups_list, true);

      if(parsed_is_config != is_config)
      {
        SNS_SPRINTF(ERROR, sns_fw_printf,
            "Config file in invalid location: %s", file_name);
        rv = SNS_RC_INVALID_VALUE;
      }
      else if(SNS_RC_POLICY == rc) { }
      else if(SNS_RC_SUCCESS != rc || 0 == sns_isafe_list_iter_len(&groups_iter))
      {
        SNS_SPRINTF(ERROR, sns_fw_printf,
            "Error parsing file: %s (%i)", file_name, rc);
        rv = SNS_RC_INVALID_VALUE;
      }
      else if(is_config)
      {
        for(sns_isafe_list_iter_init(&groups_iter, &groups_list, true);
            NULL != sns_isafe_list_iter_curr(&groups_iter);)
        {
          sns_registry_group *temp_group = (sns_registry_group*)
            sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&groups_iter));

          rv |= sns_registry_group_merge(temp_group);
          sns_registry_group_free(temp_group);
        }
      }
      else if(1 == sns_isafe_list_iter_len(&groups_iter))
      {
        sns_registry_group *temp_group = (sns_registry_group*)
          sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&groups_iter));

        if(0 != strcmp(file_name, temp_group->name))
        {
          SNS_SPRINTF(ERROR, sns_fw_printf,
              "Group/filename mismatch: '%s '%s' (%i)",
              temp_group->name, file_name, rc);
          rv = SNS_RC_INVALID_VALUE;
        }
        else
        {
          temp_group->fs_size = file_size;
          rv = sns_registry_group_merge(temp_group);
        }
        sns_registry_group_free(temp_group);
      }
      else
      {
        SNS_SPRINTF(ERROR, sns_fw_printf, "Group cnt error: %s", file_name);
        rv = SNS_RC_INVALID_VALUE;
      }

      SNS_SPRINTF(LOW, sns_fw_printf, "parsed file '%s' (%i) with result %i",
          file_name, file_size, rc);
    }

    sns_free(buf);
    fclose(file);
  }

  return rv;
}

/**
 * Convert and write an item to the json string; from the item name, to the
 * closing braces, inclusive.
 *
 * @note json_len was not large enough if rv == json_len
 *
 * @param[i] item Properly formed item to write
 * @param[o] json Active string buffer to write to
 * @param[i] json_len Maximum length of json
 *
 * @return Number of bytes written to json; -1 upon error
 */
static int
write_item(sns_registry_item const *item, char *json, uint32_t json_len)
{
  int cidx = -1;

  if(SNS_REGISTRY_TYPE_INTEGER == item->type)
    cidx = snprintf(json, json_len,
        "\"%s\":{\"type\":\"int\",\"ver\":\"%"PRIu32"\",\"data\":\"%"PRIi64"\"}",
        item->name, item->version, item->data.sint);
  else if(SNS_REGISTRY_TYPE_FLOAT == item->type)
    cidx = snprintf(json, json_len,
        "\"%s\":{\"type\":\"flt\",\"ver\":\"%"PRIu32"\",\"data\":\"%f\"}",
        item->name, item->version, item->data.flt);
  else if(SNS_REGISTRY_TYPE_GROUP == item->type)
  {
    cidx = snprintf(json, json_len,
        "\"%s\":{\"type\":\"grp\",\"ver\":\"%"PRIu32"\",\"data\":\"\"}",
        item->name, item->version);
  }
  else if(SNS_REGISTRY_TYPE_STRING == item->type)
    cidx = snprintf(json, json_len,
        "\"%s\":{\"type\":\"str\",\"ver\":\"%"PRIu32"\",\"data\":\"%s\"}",
        item->name, item->version,
        NULL != item->data.str ? item->data.str : "");
  else
    // PEND: Need to handle errors differently than buffer too small
    SNS_PRINTF(ERROR, sns_fw_printf, "Invalid item type '%i'", item->type);

  return cidx;
}

/**
 * Convert and write an group to the json string; from the group name to the
 * closing braces, inclusive.
 *
 * @note json_len was not large enough if rv == json_len
 *
 * @param[i] group Properly formed group to write
 * @param[o] json Active string buffer to write to
 * @param[i] json_len Maximum length of json
 *
 * @return Number of bytes written to json; -1 upon error
 */
static int
write_group(sns_registry_group const *group, char *json, uint32_t json_len)
{
  int cidx;
  sns_isafe_list_iter iter;

  cidx = snprintf(json, json_len, "\"%s\":{\"owner\":\"%s\"", group->name,
      NULL != group->owner ? group->owner : "NA");

  if(-1 == cidx || cidx + 2 >= json_len)
  {
    SNS_PRINTF(MED, sns_fw_printf, "String buffer too short");
    cidx = -1;
  }
  else
  {
    for(sns_isafe_list_iter_init(&iter, (sns_isafe_list*)&group->items, true);
        NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      sns_registry_item *item = (sns_registry_item*)
        sns_isafe_list_iter_get_curr_data(&iter);
      int len;

      json[cidx++] = ',';
      len = write_item(item, &json[cidx], json_len - cidx);

      // If write_item error, or not enough memory for comma/bracket
      if(-1 == len || len + cidx + 1 >= json_len)
      {
        SNS_PRINTF(MED, sns_fw_printf, "String buffer too short");
        cidx = -1;
        break;
      }
      else
        cidx += len;
    }
    if(-1 != cidx)
      json[cidx++] = '}';
  }

  return cidx;
}

/**
 * Load a configuration file, and return a newly allocated string containing
 * its contents.
 *
 * @param[i] file_path Full file path to the platform file
 *
 * @return Data contained in the file
 */
static char*
load_file_content(char const *file_path)
{
  FILE *file;
  struct stat stat_buf;
  char *rv = NULL;

  errno = 0;
  if(0 == stat(file_path, &stat_buf))
  {
    if(0 == stat_buf.st_size)
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "st_size zero for '%s'", file_path);
      return rv;
    }
    file = sns_registry_fopen(file_path, "r");
    if(NULL == file)
      SNS_SPRINTF(HIGH, sns_fw_printf, "Unable to open '%s' %i (%s)",
          file_path, errno, strerror(errno));
    else
    {
      size_t read_len;
      char *buf = sns_malloc(SNS_HEAP_MAIN, stat_buf.st_size);
      SNS_ASSERT(NULL != buf);

      read_len = sns_registry_fread(buf, sizeof(uint8_t), stat_buf.st_size, file);
      if(0 != read_len)
      {
        buf[read_len - 1] = '\0';
        rv = buf;
        SNS_SPRINTF(LOW, sns_fw_printf, "data from: '%s' - '%s'", file_path, buf);
      }
      else
      {
        SNS_SPRINTF(HIGH, sns_fw_printf, "fread error '%s' %i", file_path, errno);
        sns_free(buf);
      }
      fclose(file);
    }
  }

  return rv;
}

/**
 * Parses the file contents of sns_reg_config and updates dynamic_hw_info.
 * Each line of the configuration file has format:
 * file=soc_id=/sys/devices/soc0/soc_id
 *
 * @param[i] config_contents configuration file data as string
 *
 * @return Number of configuration items
 */
static uint32_t
parse_device_hw_info(char *config_contents)
{
  uint32_t token_idx;
  char *saveptr1 = NULL, *saveptr2 = NULL;
  char *config_line;
  for(token_idx = 0; MAX_CONFIG_ENTRIES > token_idx; token_idx++)
  {
    config_line = strtok_r(config_contents, "\n", &saveptr1);
    if(NULL == config_line)
      break;

    SNS_SPRINTF(LOW, sns_fw_printf, "%s \n", config_line);

    for(uint32_t entry_id = 0;; entry_id++)
    {
      char *token = NULL;

      token = strtok_r(config_line, "=", &saveptr2);
      if(NULL == token)
        break;

      SNS_SPRINTF(LOW, sns_fw_printf, "entry#: %s", token);

      /* First entry is not required to be handled */
      if(0 != entry_id)
      {
        uint32_t len = strlen(token)+1;
        char *elm_str =  sns_malloc(SNS_HEAP_MAIN, sizeof(char) * len);
	    SNS_ASSERT(NULL != elm_str);
        sns_strlcpy(elm_str, token, len);
        elm_str[len - 1] = '\0';

        if(1 == entry_id)
        {
          dynamic_hw_info[token_idx].name = elm_str;
        }
        if(2 == entry_id)
        {
          dynamic_hw_info[token_idx].path = elm_str;
          dynamic_hw_info[token_idx].value =
            load_file_content(dynamic_hw_info[token_idx].path);
        }
      }
      config_line = NULL;
    }

    SNS_SPRINTF(HIGH, sns_fw_printf,
        "hw_config[id:%d]:: name : %s, path: %s, value : %s",
        token_idx, dynamic_hw_info[token_idx].name,
        dynamic_hw_info[token_idx].path,
        dynamic_hw_info[token_idx].value != NULL ? dynamic_hw_info[token_idx].value : "");

    config_contents = NULL;
  }

  return token_idx;
}

/**
 * Read and parse the current device configuration, to be used to determine
 * which configuration files are applicable.  Updates dynamic_hw_info.
 *
 * No-op if dynamic config file is not present.  May block up to 400ms for
 * control file (i.e. for dynamic config file to be ready).
 */
static void
load_property_based_device_hw(void)
{
  int ctrl_wait_cnt = MAXNUM_OF_ITERATIONS_FOR_VERSION;
  char *config_contents = NULL;
  char *current_version = NULL;
  char const* config_file = NULL;
  struct stat stat_buf;

  if (0 == stat(registry_config_file_ota, &stat_buf))
    config_file = registry_config_file_ota;
  else if (0 == stat(registry_config_file_le, &stat_buf))
    config_file = registry_config_file_le;
  else
    config_file = registry_config_file;

  config_contents = load_file_content(config_file);
  if(NULL != config_contents)
  {
    char new_registry_version[SNS_REGISTRY_STR_MAX+1] = "";
    int idx = 0;
    do
    {
      current_version = load_file_content(current_registry_version);
      if(NULL == current_version)
      {
        SNS_PRINTF(HIGH, sns_fw_printf, "registry_version file not present");
        break;
      }
      /*load_file_content API keeps '\0' by replacing last element and last eliment
        is lost if file is not new line terminated.
        so version number is lost if current_registry_version file is not terminated
        with new line. Changes in latest HLOS patch handles this. But both slpi and
        hlos images goes in out of order kept below to avoid looping ctrl_wait_cnt tiems*/
      else if(strlen("version=") >= strlen(current_version))
      {
          SNS_PRINTF(HIGH, sns_fw_printf, "registry_version not valid");
          break;
      }

      for(idx = 0; idx < SNS_REGISTRY_STR_MAX && config_contents[idx] != '\n'; idx++) {
        new_registry_version[idx] = config_contents[idx];
      }
      new_registry_version[idx+1] = '\0';
      SNS_SPRINTF(HIGH, sns_fw_printf, "registry_version(current:'%s'/new:'%s') ctrl_wait_cnt:%d",
        current_version, new_registry_version, ctrl_wait_cnt);

      if(!strcmp(current_version, new_registry_version))
      {
        ctrl_wait_cnt = 0;
      }
      else
      {
        ctrl_wait_cnt--;
        /* Wait 250 ms */
        sns_busy_wait(19200ULL * 250);
      }

      if(NULL != current_version){
        sns_free(current_version);
        current_version = NULL;
      }

    } while(0 != ctrl_wait_cnt);

    dynamic_hw_info_len = parse_device_hw_info(config_contents);
    SNS_PRINTF(HIGH, sns_fw_printf, "snn_reg_config file parsing end %i",
        dynamic_hw_info_len);

    sns_free(config_contents);
    if(NULL != current_version)
      sns_free(current_version);
  }
  else
  {
    SNS_SPRINTF(HIGH, sns_fw_printf, "dynamic config file not present");
  }
}

/**
 * Read and parse the current device information, to be used to determine which
 * configuration files are applicable.
 *
 * @param[o] config Structure to contain the results
 */
static void
load_device_hw(sns_registry_hw config[])
{
  for(int i = 0; i < sns_registry_hw_info_len; i++)
  {
    config[i].value = load_file_content(config[i].path);
    SNS_SPRINTF(LOW, sns_fw_printf, "HW Config: '%s' - '%s'",
        config[i].name, config[i].value);
  }
}

/**
 * Save a string to disk.
 *
 * @param[i] full_path File path to write to
 * @param[i] json Data to write
 * @param[i] json_len Length of the json string
 *
 * @return true if write succeeded; false otherwise
 */
static bool
save_registry_file(char const *full_path, void const *json, uint32_t json_len)
{
  bool rv = true;
  FILE *file;

  errno = 0;
  file = sns_registry_fopen(full_path, "w");
  if(NULL == file)
  {
    SNS_SPRINTF(ERROR, sns_fw_printf,
        "Unable to open '%s' %i", full_path, errno);
    rv = false;
  }
  else
  {
    uint32_t write_len = sns_registry_fwrite(json, sizeof(char), json_len, file);
    if(write_len != json_len)
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "File write error %i", errno);
      rv = false;
    }

    fclose(file);
  }

  return rv;
}

/**
 * Modify the registry path it has received.
 *
 * Appends "/mnt/vendor" at the beginning of the path.
 */
static void
update_persist_mnt_path(char filepath[])
{
  char dummy[SNS_REGISTRY_DIR_LEN_MAX] = "";
  sns_strlcat(dummy, persist_mnt_name, SNS_REGISTRY_DIR_LEN_MAX);
  sns_strlcat(dummy, filepath, SNS_REGISTRY_DIR_LEN_MAX);
  sns_strlcpy(filepath, dummy, SNS_REGISTRY_DIR_LEN_MAX);
}

/**
 * Update all the required registry paths.  registry output dir path is not
 * handled here since it was taken care by sns_reg_config during
 * dynamic loading.
 */
static void
sns_registry_update_persist_path()
{
  update_persist_mnt_path(registry_config_file);
  update_persist_mnt_path(current_registry_version);
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

// PEND: This function is completely absurd, and needs some significant clean-up
sns_rc
sns_registry_init()
{
  sns_rc rv = SNS_RC_FAILED;
  DIR *dir;
  struct stat stat_buf;

  if(is_persit_partition_mounted())
  {
    sns_registry_update_persist_path();
  }
  else if(0 == stat(registry_config_file_le, &stat_buf))
  {
    sns_strlcpy(config_dir, config_dir_le, sizeof(config_dir));
  }

  if(!check_dir(registry_dir))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "registry directory not available: %s",
        registry_dir);
  }

  dir = opendir(config_dir_new);
  sns_isafe_list_init(&sns_registry_groups);

  /* Load dynamic one first and fall back if not found */
  load_property_based_device_hw();

  if(0 == dynamic_hw_info_len)
    load_device_hw(sns_registry_hw_info);

  /*registry_dir[SNS_REGISTRY_DIR_LEN_MAX] = "/persist/sensors/registry/registry";*/
  /* parsed registry files path is controlled by sns_reg_config file present in 
  /vendor/etc/sensors*/

  for(int i = 0; i<dynamic_hw_info_len; i++){
    if(0 == strncmp((char*)dynamic_hw_info[i].name,
       "output", strlen((char*)dynamic_hw_info[i].name)))
      sns_strlcpy(registry_dir, dynamic_hw_info[i].path, SNS_REGISTRY_DIR_LEN_MAX);
  }

  if(SNS_RC_SUCCESS == foreach_in_dir(registry_dir, &load_file, false))
  {
    if(NULL == dir)
      rv = foreach_in_dir(config_dir, &load_file, true);
    else
      rv = foreach_in_dir(config_dir_new, &load_file, true);
  }

  if(NULL != dir)
    closedir(dir);
  return rv;
}

sns_registry_group*
sns_registry_group_create(char const *name)
{
  sns_registry_group *group;
  uint32_t group_size = sizeof(*group) + strlen(name) + 1;

  group = sns_malloc(SNS_HEAP_MAIN, group_size);
  SNS_ASSERT(NULL != group);

  group->name = (char*)((uintptr_t)group + sizeof(*group));
  sns_strlcpy(group->name, name, strlen(name) + 1);

  group->fs_size = 0;
  group->owner = NULL;
  group->writable = true;

  sns_isafe_list_item_init(&group->list_entry, group);
  sns_isafe_list_init(&group->items);

  return group;
}

sns_registry_group*
sns_registry_group_find(char const *grp_name)
{
  sns_isafe_list_iter iter;

  for(sns_isafe_list_iter_init(&iter, &sns_registry_groups, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_registry_group *group = (sns_registry_group*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if(0 == strcmp(group->name, grp_name))
      return group;
  }

  return NULL;
}

sns_rc
sns_registry_group_merge(sns_registry_group *group)
{
  sns_rc rv = SNS_RC_SUCCESS;
  sns_isafe_list_iter iter;
  sns_registry_group *perm_group = sns_registry_group_find(group->name);

  SNS_ASSERT(perm_group != group);
  if(NULL == perm_group)
  {
    perm_group = sns_registry_group_create(group->name);
    perm_group->fs_size = group->fs_size;

    sns_isafe_list_iter_init(&iter, &sns_registry_groups, false);
    sns_isafe_list_iter_insert(&iter, &perm_group->list_entry, true);
  }

  for(sns_isafe_list_iter_init(&iter, &group->items, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_registry_item *item = (sns_registry_item*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if(SNS_REGISTRY_TYPE_INVALID == item->type)
      continue;

    sns_registry_item *perm_item = sns_registry_item_find(perm_group, item->name);
    if(NULL == perm_item)
      perm_item = sns_registry_item_create(perm_group, item);

    update_item(perm_item, item);
  }

  if(NULL == perm_group->owner && NULL != group->owner)
  {
    perm_group->owner = sns_malloc(SNS_HEAP_MAIN, strlen(group->owner) + 1);
    SNS_ASSERT(NULL != perm_group->owner);
    sns_strlcpy(perm_group->owner, group->owner, strlen(group->owner) + 1);
  }

  if(!group->writable)
    perm_group->writable = false;

  // Don't save to file if we just read it from file
  if(0 == group->fs_size && perm_group->writable)
    rv = sns_registry_group_save(perm_group);

  return rv;
}

// PEND: Use strchr to reject any request to store a '"' symbol in the registry
// To avoid multiple RT latencies, we first produce the entire string,
// then subsequently write it to file
sns_rc
sns_registry_group_save(sns_registry_group *group)
{
  uint32_t json_len = SNS_REGISTRY_GRP_LEN_INIT;
  sns_rc rv = SNS_RC_SUCCESS;
  STR_CONCAT(full_path, registry_dir, "/", group->name);

  do
  {
    int len;
    uint32_t cidx = 0;
    char *json = sns_malloc(SNS_HEAP_MAIN, json_len);
    SNS_ASSERT(NULL != json);

    json[cidx++] = '{';
    len = write_group(group, &json[cidx], json_len - cidx);
    if(-1 == len || len + cidx + 1 >= json_len)
    {
      SNS_SPRINTF(MED, sns_fw_printf,
          "JSON length too short (%i); doubling", json_len);
      json_len = 2 * json_len;
      sns_free(json);
    }
    else
    {
      cidx += len;
      json[cidx++] = '}';

      if(save_registry_file(full_path, json, cidx))
      {
        errno = 0;
        group->fs_size = len;
      }

      sns_free(json);
      break;
    }
  } while(true);

  return rv;
}

void
sns_registry_group_free(sns_registry_group *group)
{
  sns_isafe_list_iter iter;

  sns_isafe_list_iter_init(&iter, &group->items, true);
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    sns_registry_item *item = (sns_registry_item*)
      sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&iter));

    if(SNS_REGISTRY_TYPE_STRING == item->type)
      sns_free(item->data.str);
    else if(SNS_REGISTRY_TYPE_GROUP == item->type)
      sns_registry_group_free(item->data.group);

    sns_free(item);
  }

  if(NULL != group->owner)
    sns_free(group->owner);
  sns_free(group);
}

/* When a subgroup is first read, we don't set its pointer.  Now when we are
 * merging it into the registry itself, we need to find it, and create it if
 * necessary. */
sns_registry_item*
sns_registry_item_create(sns_registry_group *group, sns_registry_item *item)
{
  sns_isafe_list_iter iter;
  sns_registry_item *new_item;
  uint32_t item_size = sizeof(*item) + strlen(item->name) + 1;

  new_item = sns_malloc(SNS_HEAP_MAIN, item_size);
  SNS_ASSERT(NULL != new_item);

  new_item->name = (char*)((uintptr_t)new_item + sizeof(*item));
  sns_strlcpy(new_item->name, item->name, strlen(item->name) + 1);

  if(SNS_REGISTRY_TYPE_STRING == item->type)
  {
    new_item->data.str = sns_malloc(SNS_HEAP_MAIN, strlen(item->data.str) + 1);
    SNS_ASSERT(NULL != new_item->data.str);
    sns_strlcpy(new_item->data.str, item->data.str, strlen(item->data.str) + 1);
  }
  else
  {
    new_item->data = item->data;
  }

  new_item->version = item->version;
  new_item->type = item->type;

  sns_isafe_list_item_init(&new_item->list_entry, new_item);
  sns_isafe_list_iter_init(&iter, &group->items, false);
  sns_isafe_list_iter_insert(&iter, &new_item->list_entry, true);

  return new_item;
}

sns_registry_item*
sns_registry_item_find(sns_registry_group *group, char const *item_name)
{
  sns_isafe_list_iter iter;

  for(sns_isafe_list_iter_init(&iter, &group->items, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_registry_item *item = (sns_registry_item*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if(0 == strcmp(item->name, item_name))
      return item;
  }

  return NULL;
}
