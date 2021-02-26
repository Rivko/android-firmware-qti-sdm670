/*========================================================================*/
/**
@file adsp_amdb_api.h

This file contains APIs to interact with AMDB. Note that unlike core service,
ASM, ADM or voice, AMDB is not an elite service.
This file mainly contains the APIs related to ACDB file that's accessed by the ADSP
at boot-up.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      amdb_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
======================================================================== */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/amdb/inc/adsp_amdb_api.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
10/28/16   sw       (Tech Pubs) Changed #if 0 to #if DOXYGEN_ONLY because
                    Doxygen doesn't parse #if 0.
01/12/16   sw       (Tech Pubs) Edited Doxygen comments.
06/01/15   rbhatnk      Created file.
========================================================================== */

#ifndef _ADSP_AMDB_API_H_
#define _ADSP_AMDB_API_H_
#include "mmdefs.h"

/** @addtogroup amdb_acdb_file_format
@{ */

/** Name of the environment variable on the HLOS where the ACDB file (specified
  in AMDB_CFG_FILE_NAME) is kept.
 */
#define ENV_VAR_CONTAINING_PATH_TO_ACDB_FILE "ADSP_AVS_CFG_PATH"

/** Delimiter in the ENV_VAR_CONTAINING_PATH_TO_ACDB_FILE environment variable
  used to separate the paths.
 */
#define ENV_VAR_DELIMITER ";"

/** Name of the file containing AMDB configuration. */
#define AMDB_CFG_FILE_NAME "adsp_avs_config.acdb"

/** Magic 8-byte string that identifies this file as an ACDB file. */
#define AMDB_CFG_FILE_MAGIC_WORD1 "QCMSNDDB"

/** Magic 4-byte string that identifies this file as an AMDB configuration
  file. @newpage
 */
#define AMDB_CFG_FILE_MAGIC_WORD2 "AMDB"

typedef struct amdb_cfg_file_header_t amdb_cfg_file_header_t;
#include "adsp_begin_pack.h"

/** Header information of the configuration file.
  Following this structure is #AMDB_CFG_FILE_SECTION_VERSION_INFO_KEY.
 */
struct amdb_cfg_file_header_t
{
   uint8_t     magic_word1[8];
   /**< First magic 8-byte word used to indicate that the file is an ACDB file.

        @values #AMDB_CFG_FILE_MAGIC_WORD1 */

   uint32_t    acdb_major_version;
   /**< Major version of the ACDB file. */

   uint32_t    acdb_minor_version;
   /**< Minor version of the ACDB file. */

   uint8_t     magic_word2[4];
   /**< Second magic 4-byte word used to indicate that th file is an AMDB
        configuration file.

        @values #AMDB_CFG_FILE_MAGIC_WORD2 */

   uint32_t    compression_flag;
   /**< Specifies whether the file is compressed.

        @values
        - 0 -- Not compressed
        - 1 -- Compressed @tablebulletend */

   uint32_t    uncompressed_len;
   /**< Length of the uncompressed file (excluding this header). */

   uint32_t    actual_file_len;
   /**< Length of the file (excluding this header). */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_addtogroup amdb_acdb_file_format */

/** @addtogroup amdb_acdb_format_versinfo
@{ */

/** Length of the section key. */
#define AMDB_CFG_SECTION_KEY_LEN 8

/** Key that identifies the version information section. The key must be of
  length AMDB_CFG_SECTION_KEY_LEN.

  This identifier must always be present at this offset in the file.
 */
#define AMDB_CFG_FILE_SECTION_VERSION_INFO_KEY "VERSINFO"

/** Major version of the AMDB configuration file.
  This version is used for nonbackward-compatible changes at any level in the
  AMDB_CFG_FILE_NAME file.
 */
#define AMDB_CFG_MAJOR_VERSION 1

/** Minor version of the AMDB configuration file.
  This version is used for backward-compatible changes.
 */
#define AMDB_CFG_MINOR_VERSION 1

typedef struct amdb_cfg_version_info_t amdb_cfg_version_info_t;
#include "adsp_begin_pack.h"

/** Version information structure.
 */
struct amdb_cfg_version_info_t
{
   uint32_t major_version;
   /**< Major version.

        @values #AMDB_CFG_MAJOR_VERSION

        If any backward compatibility is broken at any level, this version
        tracks it. */

   uint32_t minor_version;
   /**< Minor version.

        @values #AMDB_CFG_MINOR_VERSION

        Use this version when adding new sections. It is similar to the keys,
        MREGINFO, BUILT-IN, etc. */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_addtogroup amdb_acdb_format_versinfo */

/** @addtogroup amdb_acdb_format_builtin_mreginfo
@{ */

/** Key that identifies the built-in module information section.
  It must be of length #AMDB_CFG_SECTION_KEY_LEN.

  When this section is not present in the ACDB file, all built-in modules are
  registered to the AMDB.
 */
#define AMDB_CFG_FILE_SECTION_BUILT_IN_MOD_INFO_KEY "BUILT-IN"

/** Key that identifies the custom module information section.
  It must be of length #AMDB_CFG_SECTION_KEY_LEN.
 */
#define AMDB_CFG_FILE_SECTION_CUSTOM_MOD_INFO_KEY   "MREGINFO"

typedef struct amdb_cfg_file_struct_t amdb_cfg_file_struct_t;
#include "adsp_begin_pack.h"

/** Recommended structure of the file for #AMDB_CFG_MAJOR_VERSION and
  #AMDB_CFG_MINOR_VERSION.

  The actual structure can change because sections are not mandatory
  and the order is not fixed.
 */
struct amdb_cfg_file_struct_t
{
   amdb_cfg_file_header_t header;
   /**< Identifies the file header. */

   uint8_t                version_info_key[AMDB_CFG_SECTION_KEY_LEN];
   /**< Identifies the version information key.

        This identifier must always be present at this offset in the file. 

        @values #AMDB_CFG_FILE_SECTION_VERSION_INFO_KEY */

   uint32_t               version_info_len;
   /**< Length of the version information (in bytes).

        Following this parameter are version information bytes as shown in
        version_info. */

        /* sizeof(amdb_cfg_version_info_t) */

   /* Per Doxygen developer, changed 0 to DOXYGEN_ONLY in all #if/#endif blocks
   because Doxygen doesn't parse #if 0. */

#if DOXYGEN_ONLY
   amdb_cfg_version_info_t   version_info;
   /**< Version information in bytes. */
#endif

   uint8_t                 built_in_mod_info_key[AMDB_CFG_SECTION_KEY_LEN];
   /**< Identifies the built-in module information key.

        @values #AMDB_CFG_FILE_SECTION_BUILT_IN_MOD_INFO_KEY */
    /* location of this key can be after other sections also.*/

   uint32_t                built_in_mod_info_len;
   /**< Length of the built-in module information area.

        Following this parameter are built_in_mod_info_len bytes as shown
        in built_in_mod_info[built_in_mod_info_len].*/

#if DOXYGEN_ONLY
   uint8_t                 built_in_mod_info[built_in_mod_info_len];
   /**< Built-in module information in bytes.

        The amdb_cfg_module_info_t structure defines this memory. */
#endif

   uint8_t                 custom_mod_info_key[AMDB_CFG_SECTION_KEY_LEN];
   /**< Identifies the custom module information key.

        @values #AMDB_CFG_FILE_SECTION_CUSTOM_MOD_INFO_KEY */

   uint32_t                custom_mod_info_len;
   /**< Length of the custom module information area.

        Following this parameter are custom_mod_info_len bytes as shown in
        custom_mod_info[custom_mod_info_len]. */

#if DOXYGEN_ONLY
   uint8_t                 custom_mod_info[custom_mod_info_len];
   /**< Custom module information in bytes.

        The amdb_cfg_module_info_t structure defines this memory. */
#endif

}
#include "adsp_end_pack.h"
;

/** @} */ /* end_addtogroup amdb_acdb_format_builtin_mreginfo */

/** @addtogroup amdb_acdb_module_info
@{ */

/** Version of the module information header structure. */
#define AMDB_CFG_MODULE_INFO_HEADER_VERSION 1

typedef struct amdb_cfg_module_info_header_t amdb_cfg_module_info_header_t;
#include "adsp_begin_pack.h"

/** Structure of the header in #AMDB_CFG_MODULE_INFO_HEADER_VERSION==1.
*/
/*  For the next version, new struct must be introduced, this cannot be modified.
  The reason is parser reads this many bytes from header at least.
 */
struct amdb_cfg_module_info_header_t
{
   uint32_t       header_version;
   /**< Tracks any additions to the structure. This element determines the
        the header structure. */

   uint32_t       header_size;
   /**< Size of the entire header (in bytes).

        The next chunk is located header_size bytes after the beginning of this
        module. */

        /* sizeof(amdb_cfg_module_info_header_t) */

   uint32_t       module_info_version;
   /**< Version of the module information.

        @values #AMDB_CFG_MODULE_INFO_VERSION

        A change in the version indicates a change in available chunks. When
        the version is updated, an old parser can parse the old chunks. A new
        parser can use the newly added chunks. */

   uint32_t       module_info_size;
   /**< Size of the entire module information (in bytes).

        The next module information is located module_info_size bytes after the
        beginning. This information includes the header size. @newpagetable */
}
#include "adsp_end_pack.h"
;


/** Interface type is Audio Pre/Postprocessor Interface. */
#define AMDB_INTERFACE_TYPE_APPI    0

/** Interface type is Common Audio Processor Interface. */
#define AMDB_INTERFACE_TYPE_CAPI    1

/** Interface type is Common Audio Processor Interface version 2. */
#define AMDB_INTERFACE_TYPE_CAPI_V2 2

//when next interface is added, 3 should be left
//as 3 is used internally for stub.
//#define AMDB_INTERFACE_TYPE_STUB    3

/** Module is generic, such as a preprocessor, postprocessor, or volume control
  module. */
#define AMDB_MODULE_TYPE_GENERIC    0    

/** Module is a decoder. */
#define AMDB_MODULE_TYPE_DECODER    1

/** Module is an encoder. */
#define AMDB_MODULE_TYPE_ENCODER    2

/** Module is a converter. */
#define AMDB_MODULE_TYPE_CONVERTER  3

/** Module is a packetizer. */
#define AMDB_MODULE_TYPE_PACKETIZER 4

/** Module is a depacketizer. */
#define AMDB_MODULE_TYPE_DEPACKETIZER 5

/** Version of the module ID chunk structure. */
#define AMDB_CFG_MODULE_ID_CHUNK_VERSION 1

/** Chunk ID of module ID structure. */
#define AMDB_CFG_CHUNK_ID_MODULE_ID  0x131B0

typedef struct amdb_cfg_module_id_t amdb_cfg_module_id_t;
#include "adsp_begin_pack.h"

/** Structure of the module ID chunk in #AMDB_CFG_MODULE_ID_CHUNK_VERSION==1. */
struct amdb_cfg_module_id_t
{
   uint32_t       chunk_id;
   /**< GUID that identifies this chunk.

        @values #AMDB_CFG_CHUNK_ID_MODULE_ID */

   uint32_t       version;
   /**< Tracks any additions to the structure as well as any additions to the
        supported values of the elements.

        @values #AMDB_CFG_MODULE_ID_CHUNK_VERSION */

   uint32_t       chunk_size;
   /**< Size of the chunk. The next chunk is at chunk_size bytes after the
        beginning of this chunk. */

        /* sizeof(amdb_cfg_module_id_t) */

   uint32_t       module_type;
   /**< Type of module.

        @values
        - #AMDB_MODULE_TYPE_GENERIC
        - #AMDB_MODULE_TYPE_DECODER
        - #AMDB_MODULE_TYPE_ENCODER
        - #AMDB_MODULE_TYPE_CONVERTER
        - #AMDB_MODULE_TYPE_PACKETIZER
        - #AMDB_MODULE_TYPE_DEPACKETIZER @tablebulletend */

   uint32_t       id1;
   /**< One of the following types of IDs:
        - Format ID for the encoder, decoder, and packetizer module types
        - Module ID for the generic module type
        - Source format ID for the converter module type @tablebulletend */

   uint32_t       id2;
   /**< Sink format ID for the converter module type.

        Zero for other module types. @newpagetable */
}
#include "adsp_end_pack.h"
;


/** Version of the flags chunk. */
#define AMDB_CFG_FLAGS_CHUNK_VERSION 1

/** Identifies the flags chunk. */
#define AMDB_CFG_CHUNK_ID_FLAGS  0x131B1

/** For built-in modules, preloads a module per the built-in configuration.
    This flag is not supported for non-built-in cases.
 */
#define AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD 2

/** Loads a module immediately upon registering. */
#define AMDB_CFG_PRELOAD_FLAG_IMMEDIATE 1

/** Loads a module at a use case boundary. */
#define AMDB_CFG_PRELOAD_FLAG_ON_USE 0

typedef struct amdb_cfg_flags_t amdb_cfg_flags_t;
#include "adsp_begin_pack.h"

/** Structure of the flags chunk in #AMDB_CFG_FLAGS_CHUNK_VERSION==1.
 */
struct amdb_cfg_flags_t
{
   uint32_t       chunk_id;
   /**< GUID that identifies this chunk.

        @values #AMDB_CFG_CHUNK_ID_FLAGS */

   uint32_t       version;
   /**< Tracks any additions to the structure as well as any additions to the
        supported values of the elements.

        @values #AMDB_CFG_FLAGS_CHUNK_VERSION*/

   uint32_t       chunk_size;
   /**< Size of the chunk. The next chunk is at chunk_size bytes after the
        beginning of this chunk. */

        /* sizeof(amdb_cfg_flags_t) */

   uint16_t       needed;
   /**< Specifies whether the module is required.

        @values
        - 0 -- Not required 
        - 1 -- Required

        If a module is not required:
        - The module passes through or bypasses for #AMDB_MODULE_TYPE_GENERIC
          modules (short-circuit).
        - For other types of modules, a failure is returned.

        If modules are not required, topologies that contain them can still be
        used. These modules are treated as pass-through modules. */

   uint16_t       preload;
   /**< Flag that indicates how to preload a module.

        @values
        - #AMDB_CFG_PRELOAD_FLAG_USE_FROM_BUILD
        - #AMDB_CFG_PRELOAD_FLAG_IMMEDIATE
        - #AMDB_CFG_PRELOAD_FLAG_ON_USE @tablebulletend @newpagetable */
}
#include "adsp_end_pack.h"
;


/** Version of the load information chunk. */
#define AMDB_CFG_LOAD_INFO_CHUNK_VERSION 1

/** Identifies the load information chunk. */
#define AMDB_CFG_CHUNK_ID_LOAD_INFO  0x131B2

typedef struct amdb_cfg_load_info_t amdb_cfg_load_info_t;
#include "adsp_begin_pack.h"

/** Structure of the load information chunk in
  #AMDB_CFG_LOAD_INFO_CHUNK_VERSION==1.

  This structure has variable length elements. Therefore, it cannot be used to
  declare a variable.
 */
struct amdb_cfg_load_info_t
{
   uint32_t       chunk_id;
   /**< GUID that identifies this chunk.

        @values #AMDB_CFG_CHUNK_ID_LOAD_INFO */

   uint32_t       version;
   /**< Tracks any additions to the structure as well as any additions to the
        supported values of the elements.

        @values #AMDB_CFG_LOAD_INFO_CHUNK_VERSION */

   uint32_t       chunk_size;
   /**< Size of the chunk. The next chunk is at chunk_size bytes after the
        beginning of this chunk. */

        /* sizeof(amdb_cfg_load_info_t) */
   
   uint32_t       interface_type;
   /**< Type of interface.

        @values
        - #AMDB_INTERFACE_TYPE_APPI
        - #AMDB_INTERFACE_TYPE_CAPI
        - #AMDB_INTERFACE_TYPE_CAPI_V2 @tablebulletend */

   uint32_t       file_name_len;
   /**< Length of the SO filename (in bytes).

        Following this parameter is file_name of length file_name_len bytes as
        shown in file_name[file_name_len]. @newpagetable */

#if DOXYGEN_ONLY
   uint8_t        file_name[file_name_len];
   /**< Length of the NULL-terminated SO filename. */
#endif

   uint32_t       tag1_len;
   /**< Length of tag1 in bytes. This tag identifies the entry point function.

        For CAPIv2, the tag is a prefix for the entry point functions:
        - tag+_get_static_properties
        - tag+_init

        For APPI and CAPI, the tag is actually the full function name. It is
        the name of the get_size function.
        
        Following this element is tag1 of length tag1_len bytes as shown in
        tag1[tag1_len]. */

#if DOXYGEN_ONLY
   uint8_t        tag1[tag1_len];
   /**< Length of NULL-terminated tag1. */
#endif

   uint32_t       tag2_len;
   /**< Length of tag2 in bytes.

        This tag2 is not present for CAPIv2. tag2_len is zero.

        For APPI and CAPI, the tag is actually the full function name.
        - For APPI, it is the name of the initialization function
        - for CAPI, it is the name of the constructor function
        
        Following this element is tag2 of length tag2_len bytes as shown in
        tag2[tag2_len]. */

#if DOXYGEN_ONLY
   uint8_t        tag2[tag2_len];
   /**< Length of NULL-terminated tag2. */
#endif

}
#include "adsp_end_pack.h"
;


/** Version of the UI-related chunk. */
#define AMDB_CFG_UI_RELATED_INFO_CHUNK_VERSION 1

/** ID of the UI-related information chunk. */
#define AMDB_CFG_CHUNK_ID_UI_RELATED_INFO 0x131B3

typedef struct amdb_cfg_ui_related_info_t amdb_cfg_ui_related_info_t;
#include "adsp_begin_pack.h"

/** Structure of the UI-related chunk in
  #AMDB_CFG_UI_RELATED_INFO_CHUNK_VERSION == 1.
 */
struct amdb_cfg_ui_related_info_t
{
   uint32_t       chunk_id;
   /**< GUID that identifies this chunk.

        @values #AMDB_CFG_CHUNK_ID_LOAD_INFO. */

   uint32_t       version;
   /**< Tracks any additions to the structure as well as any additions to the
        supported values of the elements.

        @values #AMDB_CFG_UI_RELATED_INFO_CHUNK_VERSION*/

   uint32_t       chunk_size;
   /**< Size of the chunk. The next chunk is at chunk_size bytes after the
        beginning of this chunk. */

        /* sizeof(amdb_cfg_ui_related_info_t)*/

   uint32_t       module_name_len;
   /**< Length of module name in bytes.

        Following this element is module_name of length module_name_len bytes
        as shown in module_name[module_name_len]. */

#if DOXYGEN_ONLY
  uint8_t         module_name[module_name_len];
  /**< NULL-terminated module_name. */
#endif
}
#include "adsp_end_pack.h"
;


/** Version of the error code chunk (amdb_cfg_error_code_t). */
#define AMDB_CFG_ERROR_CODE_CHUNK_VERSION 1

/** ID of the error code chunk. */
#define AMDB_CFG_CHUNK_ID_ERROR_CODE 0x131B4

typedef struct amdb_cfg_error_code_t amdb_cfg_error_code_t;
#include "adsp_begin_pack.h"

/** Structure of the  error code chunk in
  #AMDB_CFG_ERROR_CODE_CHUNK_VERSION==1.

  This chunk must be used only in out-of-band AVCS_REGISTER_AMDB_MODULES and
  AVCS_DEREGISTER_AMDB_MODULES messages.
 */
struct amdb_cfg_error_code_t
{
   uint32_t       chunk_id;
   /**< GUID that identifies this chunk.

        @values #AMDB_CFG_CHUNK_ID_ERROR_CODE. */

   uint32_t       version;
   /**< Tracks any additions to the structure as well as any additions to the
        supported values of the elements.

        @values #AMDB_CFG_ERROR_CODE_CHUNK_VERSION*/

   uint32_t       chunk_size;
   /**< Size of the chunk. The next chunk is at chunk_size bytes after the
        beginning of this chunk. */

        /* sizeof(amdb_cfg_error_code_t) */

   uint32_t       result;
   /**< Result of the module registration. The aDSP must be able to write to
        this memory block. @newpagetable */
}
#include "adsp_end_pack.h"
;


/** Version of the module information structure.
  The version is incremented whenever new chunks are introduced.
 */
#define AMDB_CFG_MODULE_INFO_VERSION 1

typedef struct amdb_cfg_module_info_t  amdb_cfg_module_info_t;
#include "adsp_begin_pack.h"

/** Typical module information in #AMDB_CFG_MODULE_INFO_VERSION==1.
 */
struct amdb_cfg_module_info_t
{
   amdb_cfg_module_info_header_t header;
   /**< Header information. The other chunks follow this element. */

   /* The following items can appear in any order.*/
#if DOXYGEN_ONLY

   amdb_cfg_module_id_t          id;
   /**< ID of the module. This chunk is mandatory. */

   amdb_cfg_flags_t              flags;
   /**< Contains flags such as Preload or Needed. This chunk is mandatory,
        except for the deregister command. */

   amdb_cfg_load_info_t          load_info;
   /**< Information loading. This chunk is mandatory for custom modules, but it
        is not required for built-in modules. */

   amdb_cfg_ui_related_info_t    ui_info;
   /**< Optional UI-related information. */

   amdb_cfg_error_code_t         err_code;
   /**< Optional error code chunk. */
#endif
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_addtogroup amdb_acdb_module_info */


#endif //_ADSP_AMDB_API_H_
