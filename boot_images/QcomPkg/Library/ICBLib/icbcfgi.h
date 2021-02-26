#ifndef __ICBCFGI_H__
#define __ICBCFGI_H__
/*
===========================================================================

FILE:         icbcfg_queryi.h

DESCRIPTION:  Internal header file for the ICB boot library.

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBLib/icbcfgi.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2017/05/08   sds     Add support for alternate address translator
2016/01/21   sds     Add Get_AllowedMemRegion().
2013/02/26   sds     Initial revision.

===========================================================================
    Copyright (c) 2013, 2016, 2017 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "icbcfg_common.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DDIChipInfo.h"
#include "icbcfg_hwio.h"
#include "HALhwio.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/**
 * ICBcfg Register types
 */
typedef enum 
{
  ICBCFG_INVALID_TYPE = 0,    /**< Invalid type */
  ICBCFG_DEFAULT,             /**< Default Config  */
  ICBCFG_32_BIT_REG,          /**< 32-bit register */
  ICBCFG_16_BIT_REG,          /**< 16-bit register */
  ICBCFG_8_BIT_REG,           /**< 8-bit  register */
  ICBCFG_32_BIT_READ_REG,     /**< 32-bit register */
  ICBCFG_16_BIT_READ_REG,     /**< 16-bit register */
  ICBCFG_8_BIT_READ_REG,      /**< 8-bit  register */
  ICBCFG_32_BIT_WRITE_REG,    /**< 32-bit register */
  ICBCFG_16_BIT_WRITE_REG,    /**< 16-bit register */
  ICBCFG_8_BIT_WRITE_REG,     /**< 8-bit  register */
  ICBCFG_32_BIT_WAIT_REG,     /**< 32-bit register */
  ICBCFG_16_BIT_WAIT_REG,     /**< 16-bit register */
  ICBCFG_8_BIT_WAIT_REG,      /**< 8-bit  register */

  /* Force 32-bit */
  icbcfg_config_type_Placeholder = 0x7FFFFFFF
} icbcfg_config_type;


/**
 * ICBcfg configuration data type
 */
typedef struct 
{
  icbcfg_config_type cfg_type;     /**< Config type */
  void *             addr;         /**< Register address */
  uint32             bmask;        /**< Bit mask for field */   
  uint32             rmask;        /**< Register mask */
  uint32             data;         /**< Configuration data */
} icbcfg_data_type;

/**
 * ICBcfg device property type
 */
typedef struct 
{
  uint32 len;                     /**< Length of the config  data array */
  icbcfg_data_type *cfg_data;     /**< Pointer to config data array */
} icbcfg_prop_type;

typedef enum
{
  ICBCFG_ADDR_TRANS_NOC   = 0,
  ICBCFG_ADDR_TRANS_LLCC,
  ICBCFG_ADDR_TRANS_COUNT,
  ICBCFG_ADDR_TRANS_SIZE  = 0x7FFFFFFF,
} icbcfg_trans_type;

typedef struct
{
  /* Chip version information for this device data. */
  DalChipInfoFamilyType   family;
  bool                    match;
  DalChipInfoVersionType  version;

  /* Device information. */
  uint32                  map_ddr_region_count;
  icbcfg_mem_region_type *map_ddr_regions;
  uint32                  num_segments;
  uint32                  num_channels;
  uint32                  addr_width;
  icbcfg_trans_type       trans_type;
  uint8 **                trans_bases;

  /* Prop lists. */
  icbcfg_prop_type *      prop_data;
  icbcfg_prop_type *      post_prop_data;
} icbcfg_device_config_type;

typedef struct
{
  uint32                      num_configs;
  icbcfg_device_config_type **configs;
} icbcfg_info_type;

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

//*============================================================================*/
/**
@brief 
      ICBCFG_HWIO*() macros are used to define register configuration using HWIO
      macros.
      ICBCFG_HWIO_DW() is used for 32-bit register configuration.
      ICBCFG_HWIO_W()  is used for 16-bit register configuration.
      ICBCFG_HWIO_B()  is used for 8-bit  register configuration.
 
@param[in]  reg   Register name as specified in SWI document. 
@param[val] val   data value to be configured in specified register.

@return    
      icbcfg_data_type structure.

@dependencies
      Make sure required HWIO macros are defined in included HWIO header files.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
#define ICBCFG_HWIO_DW(reg, val) {ICBCFG_32_BIT_REG,      \
                                  (void*)HWIO_ADDR(reg),  \
                                  HWIO_RMSK(reg),         \
                                  HWIO_RMSK(reg),         \
                                  (uint32)(val)           \
                                 }

#define ICBCFG_HWIO_W(reg, val)  {ICBCFG_16_BIT_REG,      \
                                  (void*)HWIO_ADDR(reg),  \
                                  HWIO_RMSK(reg),         \
                                  HWIO_RMSK(reg),         \
                                  (uint32)(val)           \
                                 }

#define ICBCFG_HWIO_B(reg, val)  {ICBCFG_8_BIT_REG,       \
                                  (void*)HWIO_ADDR(reg),  \
                                  HWIO_RMSK(reg),         \
                                  HWIO_RMSK(reg),         \
                                  (uint32)(val)           \
                                 }

#define ICBCFG_HWIOI_DW(reg, i, val) {ICBCFG_32_BIT_REG,          \
                                      (void*)HWIO_ADDRI(reg, i),  \
                                      HWIO_RMSK(reg),             \
                                      HWIO_RMSK(reg),             \
                                      (uint32)(val)               \
                                     }

#define ICBCFG_HWIOI_W(reg, i, val)  {ICBCFG_16_BIT_REG,          \
                                      (void*)HWIO_ADDRI(reg, i),  \
                                      HWIO_RMSK(reg),             \
                                      HWIO_RMSK(reg),             \
                                      (uint32)(val)               \
                                     }

#define ICBCFG_HWIOI_B(reg, i, val)  {ICBCFG_8_BIT_REG,           \
                                      (void*)HWIO_ADDRI(reg, i),  \
                                      HWIO_RMSK(reg),             \
                                      HWIO_RMSK(reg),             \
                                      (uint32)(val)               \
                                     }

#define ICBCFG_HWIOF_DW(reg, field, val) {ICBCFG_32_BIT_REG,      \
                                          (void*)HWIO_ADDR(reg),  \
                                          HWIO_FMSK(reg, field),  \
                                          HWIO_RMSK(reg),         \
                                          (uint32)(val) << HWIO_SHFT(reg, field) \
                                         }

#define ICBCFG_HWIOF_W(reg, field, val)  {ICBCFG_16_BIT_REG,      \
                                          (void*)HWIO_ADDR(reg),  \
                                          HWIO_FMSK(reg, field),  \
                                          HWIO_RMSK(reg),         \
                                          (uint32)(val) << HWIO_SHFT(reg, field) \
                                         }

#define ICBCFG_HWIOF_B(reg, field, val)  {ICBCFG_8_BIT_REG,       \
                                          (void*)HWIO_ADDR(reg),  \
                                          HWIO_FMSK(reg, field),  \
                                          HWIO_RMSK(reg),         \
                                          (uint32)(val) << HWIO_SHFT(reg, field) \
                                         }

#define ICBCFG_HWIOFI_DW(reg, i, field, val) {ICBCFG_32_BIT_REG,          \
                                              (void*)HWIO_ADDRI(reg, i),  \
                                              HWIO_FMSK(reg, field),      \
                                              HWIO_RMSK(reg),             \
                                              (uint32)(val) << HWIO_SHFT(reg, field) \
                                             }

#define ICBCFG_HWIOFI_W(reg, i, field, val)  {ICBCFG_16_BIT_REG,          \
                                              (void*)HWIO_ADDRI(reg, i),  \
                                              HWIO_FMSK(reg, field),      \
                                              HWIO_RMSK(reg),             \
                                              (uint32)(val) << HWIO_SHFT(reg, field) \
                                             }

#define ICBCFG_HWIOFI_B(reg, i, field, val)  {ICBCFG_8_BIT_REG,           \
                                              (void*)HWIO_ADDRI(reg, i),  \
                                              HWIO_FMSK(reg, field),      \
                                              HWIO_RMSK(reg),             \
                                              (uint32)(val) << HWIO_SHFT(reg, field) \
                                             }

//*============================================================================*/
/**
@brief 
      ICBCFG_RAW_*() macros are used to define register configuration with
      register address and data.
      ICBCFG_RAW_DW() is used for 32-bit register configuration.
      ICBCFG_RAW_W()  is used for 16-bit register configuration.
      ICBCFG_RAW_B()  is used for 8-bit  register configuration.
 
@param[in]  addr   Register address. 
@param[val] data   data value to be written in specified address.

@return    
      icbcfg_data_type structure.

@dependencies
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
#define ICBCFG_RAW_DW(addr, data)     {ICBCFG_32_BIT_REG,       \
                                       (void*)addr,             \
                                       0xFFFFFFFF,              \
                                       0xFFFFFFFF,              \
                                       data                     \
                                      }
 
#define ICBCFG_RAW_W(addr, data)      {ICBCFG_16_BIT_REG,       \
                                       (void*)addr,             \
                                       0xFFFF,                  \
                                       0xFFFF,                  \
                                       data                     \
                                      }

#define ICBCFG_RAW_B(addr, data)      {ICBCFG_8_BIT_REG,        \
                                       (void*)addr,             \
                                       0xFF,                    \
                                       0xFF,                    \
                                       data                     \
                                      } 

//*============================================================================*/
/**
@brief 
      ICBCFG_READ_DW() is used for 32-bit register configuration.
      ICBCFG_READ_W()  is used for 16-bit register configuration.
      ICBCFG_READ_B()  is used for 8-bit  register configuration.
      ICBCFG_WRITE_DW() is used for 32-bit register configuration (READ_DW() first).
      ICBCFG_WRITE_W()  is used for 16-bit register configuration (READ_W() first).
      ICBCFG_WRITE_B()  is used for 8-bit  register configuration (READ_B() first).
 
@param[in]  addr   Register address. 

@return    
      icbcfg_data_type structure.

@dependencies
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
#define ICBCFG_HWIOF_READ_DW(reg, field) \
                                      {ICBCFG_32_BIT_READ_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_FMSK(reg, field),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_SHFT(reg, field)   \
                                      }

#define ICBCFG_HWIOF_READ_W(reg, field) \
                                      {ICBCFG_16_BIT_READ_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_FMSK(reg, field),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_SHFT(reg, field)   \
                                      }

#define ICBCFG_HWIOF_READ_B(reg, field) \
                                      {ICBCFG_8_BIT_READ_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_FMSK(reg, field),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_SHFT(reg, field)   \
                                      }

#define ICBCFG_READ_DW(addr,rmsk,bmsk,shft) \
                                      {ICBCFG_32_BIT_READ_REG,  \
                                       (void*)addr,             \
                                       bmsk,                    \
                                       rmsk,                    \
                                       shft                     \
                                      }
 
#define ICBCFG_READ_W(addr,rmsk,bmsk,shft) \
                                      {ICBCFG_16_BIT_READ_REG,  \
                                       (void*)addr,             \
                                       bmsk,                    \
                                       rmsk,                    \
                                       shft                     \
                                      }

#define ICBCFG_READ_B(addr,rmsk,bmsk,shft) \
                                      {ICBCFG_8_BIT_READ_REG,   \
                                       (void*)addr,             \
                                       bmsk,                    \
                                       rmsk,                    \
                                       shft                     \
                                      } 

#define ICBCFG_READ_RAW_DW(addr)      {ICBCFG_32_BIT_READ_REG,  \
                                       (void*)addr,             \
                                       0xFFFFFFFF,              \
                                       0xFFFFFFFF,              \
                                       0                        \
                                      }
 
#define ICBCFG_READ_RAW_W(addr)       {ICBCFG_16_BIT_READ_REG,  \
                                       (void*)addr,             \
                                       0xFFFF,                  \
                                       0xFFFF,                  \
                                       0                        \
                                      }

#define ICBCFG_READ_RAW_B(addr)       {ICBCFG_8_BIT_READ_REG,   \
                                       (void*)addr,             \
                                       0xFF,                    \
                                       0xFF,                    \
                                       0                        \
                                      } 

#define ICBCFG_HWIOF_WRITE_DW(reg, field) \
                                      {ICBCFG_32_BIT_WRITE_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_FMSK(reg, field),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_SHFT(reg, field)   \
                                      }

#define ICBCFG_HWIOF_WRITE_W(reg, field) \
                                      {ICBCFG_16_BIT_WRITE_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_FMSK(reg, field),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_SHFT(reg, field)   \
                                      }

#define ICBCFG_HWIOF_WRITE_B(reg, field) \
                                      {ICBCFG_8_BIT_WRITE_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_FMSK(reg, field),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_SHFT(reg, field)   \
                                      }

#define ICBCFG_WRITE_DW(addr,rmsk,bmsk,shft) \
                                      {ICBCFG_32_BIT_WRITE_REG, \
                                       (void*)addr,             \
                                       bmsk,                    \
                                       rmsk,                    \
                                       shft                     \
                                      }
 
#define ICBCFG_WRITE_W(addr,rmsk,bmsk,shft) \
                                      {ICBCFG_16_BIT_WRITE_REG, \
                                       (void*)addr,             \
                                       bmsk,                    \
                                       rmsk,                    \
                                       shft                     \
                                      }

#define ICBCFG_WRITE_B(addr,rmsk,bmsk,shft) \
                                      {ICBCFG_8_BIT_WRITE_REG,  \
                                       (void*)addr,             \
                                       bmsk,                    \
                                       rmsk,                    \
                                       shft                     \
                                      } 

#define ICBCFG_WRITE_RAW_DW(addr)     {ICBCFG_32_BIT_WRITE_REG, \
                                       (void*)addr,             \
                                       0xFFFFFFFF,              \
                                       0xFFFFFFFF,              \
                                       0                        \
                                      }
 
#define ICBCFG_WRITE_RAW_W(addr)      {ICBCFG_16_BIT_WRITE_REG, \
                                       (void*)addr,             \
                                       0xFFFF,                  \
                                       0xFFFF,                  \
                                       0                        \
                                      }

#define ICBCFG_WRITE_RAW_B(addr)      {ICBCFG_8_BIT_WRITE_REG,  \
                                       (void*)addr,             \
                                       0xFF,                    \
                                       0xFF,                    \
                                       0                        \
                                      } 
//
//*============================================================================*/
/**
@brief 
      ICBCFG_WAIT_DW() is used for 32-bit register configuration.
      ICBCFG_WAIT_W()  is used for 16-bit register configuration.
      ICBCFG_WAIT_B()  is used for 8-bit  register configuration.
 
@param[in]  addr   Register address. 

@return    
      icbcfg_data_type structure.

@dependencies
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
#define ICBCFG_WAIT_HWIO_DW(reg, val) {ICBCFG_32_BIT_WAIT_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_RMSK(reg),         \
                                       (uint32)(val)           \
                                      }

#define ICBCFG_WAIT_HWIO_W(reg, val)  {ICBCFG_16_BIT_WAIT_REG, \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_RMSK(reg),         \
                                       (uint32)(val)           \
                                      }

#define ICBCFG_WAIT_HWIO_B(reg, val)  {ICBCFG_8_BIT_WAIT_REG,  \
                                       (void*)HWIO_ADDR(reg),  \
                                       HWIO_RMSK(reg),         \
                                       HWIO_RMSK(reg),         \
                                       (uint32)(val)           \
                                      }

#define ICBCFG_WAIT_HWIOI_DW(reg, i, val) {ICBCFG_32_BIT_WAIT_REG,     \
                                           (void*)HWIO_ADDRI(reg, i),  \
                                           HWIO_RMSK(reg),             \
                                           HWIO_RMSK(reg),             \
                                           (uint32)(val)               \
                                          }

#define ICBCFG_WAIT_HWIOI_W(reg, i, val)  {ICBCFG_16_BIT_WAIT_REG,     \
                                           (void*)HWIO_ADDRI(reg, i),  \
                                           HWIO_RMSK(reg),             \
                                           HWIO_RMSK(reg),             \
                                           (uint32)(val)               \
                                          }

#define ICBCFG_WAIT_HWIOI_B(reg, i, val)  {ICBCFG_8_BIT_WAIT_REG,      \
                                           (void*)HWIO_ADDRI(reg, i),  \
                                           HWIO_RMSK(reg),             \
                                           HWIO_RMSK(reg),             \
                                           (uint32)(val)               \
                                          }

#define ICBCFG_WAIT_HWIOF_DW(reg, field, val) {ICBCFG_32_BIT_WAIT_REG, \
                                               (void*)HWIO_ADDR(reg),  \
                                               HWIO_FMSK(reg, field),  \
                                               HWIO_RMSK(reg),         \
                                               (uint32)(val) << HWIO_SHFT(reg, field) \
                                              }

#define ICBCFG_WAIT_HWIOF_W(reg, field, val)  {ICBCFG_16_BIT_WAIT_REG, \
                                               (void*)HWIO_ADDR(reg),  \
                                               HWIO_FMSK(reg, field),  \
                                               HWIO_RMSK(reg),         \
                                               (uint32)(val) << HWIO_SHFT(reg, field) \
                                              }

#define ICBCFG_WAIT_HWIOF_B(reg, field, val)  {ICBCFG_8_BIT_WAIT_REG,  \
                                               (void*)HWIO_ADDR(reg),  \
                                               HWIO_FMSK(reg, field),  \
                                               HWIO_RMSK(reg),         \
                                               (uint32)(val) << HWIO_SHFT(reg, field) \
                                              }

#define ICBCFG_WAIT_HWIOFI_DW(reg, i, field, val) {ICBCFG_32_BIT_WAIT_REG,     \
                                                   (void*)HWIO_ADDRI(reg, i),  \
                                                   HWIO_FMSK(reg, field),      \
                                                   HWIO_RMSK(reg),             \
                                                   (uint32)(val) << HWIO_SHFT(reg, field) \
                                                  }

#define ICBCFG_WAIT_HWIOFI_W(reg, i, field, val)  {ICBCFG_16_BIT_WAIT_REG,     \
                                                   (void*)HWIO_ADDRI(reg, i),  \
                                                   HWIO_FMSK(reg, field),      \
                                                   HWIO_RMSK(reg),             \
                                                   (uint32)(val) << HWIO_SHFT(reg, field) \
                                                  }

#define ICBCFG_WAIT_HWIOFI_B(reg, i, field, val)  {ICBCFG_8_BIT_WAIT_REG,      \
                                                   (void*)HWIO_ADDRI(reg, i),  \
                                                   HWIO_FMSK(reg, field),      \
                                                   HWIO_RMSK(reg),             \
                                                   (uint32)(val) << HWIO_SHFT(reg, field) \
                                                  }

#define ICBCFG_WAIT_RAW_DW(addr, val)        {ICBCFG_32_BIT_WAIT_REG,  \
                                              (void*)addr,             \
                                              0xFFFFFFFF,              \
                                              0xFFFFFFFF,              \
                                              val                      \
                                             }
 
#define ICBCFG_WAIT_RAW_W(addr, val)         {ICBCFG_16_BIT_WAIT_REG,  \
                                              (void*)addr,             \
                                              0xFFFF,                  \
                                              0xFFFF,                  \
                                              val                      \
                                             }

#define ICBCFG_WAIT_RAW_B(addr, val)         {ICBCFG_8_BIT_WAIT_REG,   \
                                              (void*)addr,             \
                                              0xFF,                    \
                                              0xFF,                    \
                                              val                      \
                                             } 

/*
* Default configuration for initial config data. Default config is added 
* to device property structure as empty struct property is not allowed. 
*/
#define ICBCFG_DEFAULT()              {ICBCFG_DEFAULT,          \
                                       NULL,                    \
                                       0x0,                     \
                                       0x0,                     \
                                       0x0                      \
                                      }  

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
//*============================================================================*/
/**
@brief
      Informs query layer to update address translation cache.
*/
/*============================================================================*/
void update_addr_translation( void );
#endif /* __ICBCFGI_H__ */
