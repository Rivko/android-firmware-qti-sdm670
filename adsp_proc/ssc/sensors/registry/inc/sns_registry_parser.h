#pragma once
/*=============================================================================
  @file sns_registry_parser.h

  JSON parser and encoder for use by the Sensors Registry module.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_isafe_list.h"
#include "sns_registry.pb.h"

/*=============================================================================
  Macros
  ===========================================================================*/

/* Maximum length of the directory path for the registry */
#define SNS_REGISTRY_DIR_LEN_MAX 200

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* Possible data types for a registry item */
typedef enum sns_registry_data_type
{
  SNS_REGISTRY_TYPE_INVALID,
  SNS_REGISTRY_TYPE_GROUP,
  SNS_REGISTRY_TYPE_INTEGER,
  SNS_REGISTRY_TYPE_FLOAT,
  SNS_REGISTRY_TYPE_STRING
} sns_registry_data_type;

/* A registry item residing within a registry group */
typedef struct sns_registry_item
{
  sns_isafe_list_item list_entry;

  /* Partial name of this item */
  char *name;
  /* Item version; a value of '0' will always be overwritten by updates;
   * otherwise any new version must have a greater version */
  uint32_t version;
  sns_registry_data_type type;

  union
  {
    /* A nested subgroup; may be NULL during parsing */
    struct sns_registry_group *group;
    /* Signed integer value */
    int64_t sint;
    /* Floating-point value */
    float flt;
    /* Null-terminated, dynamic-length character array */
    char *str;
  } data;
} sns_registry_item;

/* Registry groups are simply a collection of items, where each item is a
 * key-value pair. */
typedef struct sns_registry_group
{
  sns_isafe_list_item list_entry;

  /* Full name of this registry group */
  char *name;
  /* Size of this registry group while in JSON format on the file system;
   * Only set when read from registry, or after saved to registry. */
  uint32_t fs_size;
  /* Name of the registry group owner; library name */
  char *owner;
  /* Whether this group may be written to file.  Will be false only if there
   * was a file read error during initial processing. */
  bool writable;

  /* List of sns_registry_item contained within this group */
  sns_isafe_list items;
} sns_registry_group;

/**
 * Hardware configuration of this device.
 */
typedef struct sns_registry_hw_config
{
  /* Name of the HW config option; string used in config files */
  char const *name;
  /* Path to the LA file to read/parse */
  char const *path;
  /* Value retrieved from path; NULL if unable to read/parse */
  char *value;
} sns_registry_hw;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Initialize the registry.
 *
 * Obtain a list of all registry groups and their owners.
 * Read and apply all configuration files.
 *
 * CURRENT IMPLEMENTATION:
 * 1) Determine list of existing registry groups from registry file list
 *  - Create an "unloaded" group entry in global list for each
 * 2) Iterate over group list and load each from file
 * 3) Iterate over all files in config directory
 *  - Create temporary list of loaded groups using hw-specific values
 *  - Merge temporary list into the global group list using versioning rules
 *  - Save back to file any changes due to configuration files
 *
 * OPTIONAL OPTIMIZATIONS:
 * 1) Do not load registry files until needed (read request or config change)
 * 2) Store config file list and modification time in registry: do not load
 *    unless changed from previous load.
 *
 * @return
 *  SNS_RC_SUCCESS
 *  SNS_RC_FAILED - Unable to load registry files
 */
sns_rc sns_registry_init(void);

/**
 * Find the registry group with the given name within the global group list.
 * If the group does not exist, return NULL.
 *
 * @note This function will use a stored list of available registry groups,
 * and will not access the file system.
 *
 * @param[i] grp_name Full name of the group to search for; null-terminated
 *
 * @return Found registry group, or NULL
 */
sns_registry_group* sns_registry_group_find(char const *grp_name);

/**
 * Create a new registry group with the specified name.
 *
 * @note This function simply creates a local reference to a new group; no
 * file access is performed until sns_registry_group_save.
 *
 * @param[i] grp_name Full name of the group to create; null-terminated
 *
 * @return Created registry group
 */
sns_registry_group* sns_registry_group_create(char const *grp_name);

/**
 * Merge a group into the Sensors Registry, as well as all linked, nested
 * subgroups.  If a group already exists, update items according to versioning
 * rules.
 *
 * @note group must first be allocated by sns_registry_group_create, and
 * not be on any lists.  References to group after function return will result
 * in unspecified behavior.
 *
 * @param[i] group Group to add to the central registry
 *
 * @return
 * SNS_RC_SUCCESS
 * SNS_RC_NOT_AVAILABLE - File operation failure
 */
sns_rc sns_registry_group_merge(sns_registry_group *group);

/**
 * Convert to JSON, and write all group data to file.  Will recursively
 * save and write all subgroups.  Saves written filesize to group.
 *
 * @param[i] group Top-level group to save to the file system
 *
 * @return
 *  SNS_RC_SUCCESS
 *  SNS_RC_NOT_AVAILABLE - Unable to open/save this registry group
 *  SNS_RC_INVALID_VALUE - Error in provided registry group
 */
sns_rc sns_registry_group_save(sns_registry_group *group);

/**
 * Deinitialize and free all memory associated with this registry group.
 *
 * @note Group should first be removed from any list.
 *
 * @param[i] group Group to de-initialize and free
 */
void sns_registry_group_free(sns_registry_group *group);

/**
 * Create a new registry item, and add it to a group.
 *
 * @note Parameter 'item' will be copied into a newly allocated object which
 * will be added to 'group'.
 *
 * @param[i] group
 * @param[i] item Item to be added to the group
 *
 * @return New copy of item, inserted in group
 */
sns_registry_item* sns_registry_item_create(sns_registry_group *group,
    sns_registry_item *item);

/**
 * Find the item within a group having a particular name.
 *
 * @param[i] group Existing group already loaded in which to search
 * @param[i] item_name Partial name of this item
 *
 * @return Found item, or NULL if name is not present.
 */
sns_registry_item* sns_registry_item_find(sns_registry_group *group,
    char const *item_name);
