#ifndef PLATFORMINFODEFS_H
#define PLATFORMINFODEFS_H

/**
  @file PlatformInfo.c

  Public definitions for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/PlatformInfoDefs.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
  @addtogroup platform_info
  @{
*/

/**
 * Generate a platform version number.
 *
 * This macro generates the platform version number from the specified
 * major and minor numbers. For example, version 1.2 is represented by
 * @code DALPLATORMINFO_VERSION(1,2) @endcode
 *
*/
#define DALPLATFORMINFO_VERSION(major, minor)  (((major) << 16) | (minor))


/**
  Target type of the device on which the platform is running.
 */
typedef enum
{
  DALPLATFORMINFO_TYPE_UNKNOWN      = 0x00,  /**< Unknown target device. */
  DALPLATFORMINFO_TYPE_SURF         = 0x01,  /**< Target is a SURF device. */
  DALPLATFORMINFO_TYPE_FFA          = 0x02,  /**< Target is an FFA device. */
  DALPLATFORMINFO_TYPE_FLUID        = 0x03,  /**< Target is a FLUID device. */
  DALPLATFORMINFO_TYPE_FUSION       = 0x04,  /**< Target is a FUSION device. */
  DALPLATFORMINFO_TYPE_OEM          = 0x05,  /**< Target is an OEM device. */
  DALPLATFORMINFO_TYPE_QT           = 0x06,  /**< Target is a QT device. */
  DALPLATFORMINFO_TYPE_CDP          = DALPLATFORMINFO_TYPE_SURF,
                                             /**< Target is a CDP (aka SURF) device. */
  DALPLATFORMINFO_TYPE_MTP_MDM      = 0x07,  /**< Target is a MDM MTP device. */
  DALPLATFORMINFO_TYPE_MTP_MSM      = 0x08,  /**< Target is a MSM MTP device. */
  DALPLATFORMINFO_TYPE_MTP          = DALPLATFORMINFO_TYPE_MTP_MSM,
                                             /**< Target is a MTP device. */
  DALPLATFORMINFO_TYPE_LIQUID       = 0x09,  /**< Target is a LiQUID device. */
  DALPLATFORMINFO_TYPE_DRAGONBOARD  = 0x0A,  /**< Target is a DragonBoard device. */
  DALPLATFORMINFO_TYPE_QRD          = 0x0B,  /**< Target is a QRD device. */
  DALPLATFORMINFO_TYPE_EVB          = 0x0C,  /**< Target is an EVB device. */
  DALPLATFORMINFO_TYPE_HRD          = 0x0D,  /**< Target is a HRD device. */
  DALPLATFORMINFO_TYPE_DTV          = 0x0E,  /**< Target is a DTV device. */
  DALPLATFORMINFO_TYPE_RUMI         = 0x0F,  /**< Target is on RUMI. */
  DALPLATFORMINFO_TYPE_VIRTIO       = 0x10,  /**< Target is on VIRTIO. */
  DALPLATFORMINFO_TYPE_GOBI         = 0x11,  /**< Target is a GOBI device. */
  DALPLATFORMINFO_TYPE_CBH          = 0x12,  /**< Target is a CBH device. */
  DALPLATFORMINFO_TYPE_BTS          = 0x13,  /**< Target is a BTS device. */
  DALPLATFORMINFO_TYPE_XPM          = 0x14,  /**< Target is a XPM device. */
  DALPLATFORMINFO_TYPE_RCM          = 0x15,  /**< Target is a RCM device. */
  DALPLATFORMINFO_TYPE_DMA          = 0x16,  /**< Target is a DMA device. */
  DALPLATFORMINFO_TYPE_STP          = 0x17,  /**< Target is a STP device. */
  DALPLATFORMINFO_TYPE_SBC          = 0x18,  /**< Target is a SBC device. */
  DALPLATFORMINFO_TYPE_ADP          = 0x19,  /**< Target is a ADP device. */

  DALPLATFORMINFO_NUM_TYPES         = 0x20,  /**< Number of known targets
                                               (including unknown). */
  /** @cond */
  DALPLATFORMINFO_TYPE_32BITS       = 0x7FFFFFFF
  /** @endcond */
} DalPlatformInfoPlatformType;


/**
 * Keys to get data out of the CDT.
 */
typedef enum
{
  DALPLATFORMINFO_KEY_UNKNOWN       = 0x00,
  DALPLATFORMINFO_KEY_DDR_FREQ      = 0x01,
  DALPLATFORMINFO_KEY_GFX_FREQ      = 0x02,
  DALPLATFORMINFO_KEY_CAMERA_FREQ   = 0x03,
  DALPLATFORMINFO_KEY_FUSION        = 0x04,
  DALPLATFORMINFO_KEY_CUST          = 0x05,

  DALPLATFORMINFO_NUM_KEYS          = 0x06,

  /** @cond */
  DALPLATFORMINFO_KEY_32BITS  = 0x7FFFFFFF
  /** @endcond */
} DalPlatformInfoKeyType;


/**
 * Sub-type of the platform.
 *
 * NOTE: Deprecated.
 */
typedef enum
{
  DALPLATFORMINFO_SUBTYPE_UNKNOWN = 0x00, /**< Unknown sub-type. */
  DALPLATFORMINFO_SUBTYPE_CSFB    = 0x01, /**< Platform is of sub-type CSFB. */
  DALPLATFORMINFO_SUBTYPE_SVLTE1  = 0x02, /**< Platform is of sub-type SVLTE1. */
  DALPLATFORMINFO_SUBTYPE_SVLTE2A = 0x03, /**< Platform is of sub-type SVLTE2A. */
  DALPLATFORMINFO_SUBTYPE_SVLTE2B = 0x04, /**< Platform is of sub-type SVLTE2B. */
  DALPLATFORMINFO_SUBTYPE_SVLTE2C = 0x05, /**< Platform is of sub-type SVLTE2C. */
  DALPLATFORMINFO_SUBTYPE_SGLTE   = 0x06, /**< Platform is of sub-type SGLTE. */

  DALPLATFORMINFO_NUM_SUBTYPES    = 0x07, /**< Number of known sub-types (
                                             (including unknown). */
  /** @cond */
  DALPLATFORMINFO_SUBTYPE_32BITS  = 0x7FFFFFFF
  /** @endcond */
} DalPlatformInfoPlatformSubtype;


/**
  @brief Stores the target platform, the platform version, and the
  platform subtype.
 */
typedef struct
{
  DalPlatformInfoPlatformType     platform;   /**< Type of the current target. */
  uint32                          version;    /**< Version of the platform in use. */
  uint32                          subtype;    /**< Sub-type of the platform. */
  boolean                         fusion;     /**< TRUE if Fusion; FALSE otherwise. */
} DalPlatformInfoPlatformInfoType;


/**
  Format of the DalPlatformInfoSMemType structure. Increments once for every
  structure change.
 */
#define DALPLATFORMINFO_SMEM_FORMAT DALPLATFORMINFO_VERSION(0, 15)


/**
  Length of the build ID buffer in DalPlatformInfoSMemType.
 */
#define DALPLATFORMINFO_SMEM_BUILD_ID_LENGTH 32

/**
 * Length of the chip ID buffer in DalPlatformInfoSMemType
 */
#define DALPLATFORMINFO_SMEM_MAX_CHIP_ID_LENGTH 32


/**
  Maximum number of PMIC devices in DalPlatformInfoSMemType.
 */
#define DALPLATFORMINFO_SMEM_MAX_PMIC_DEVICES 3


/**
  @brief SMEM structure for PMIC information.
*/
typedef struct DalPlatformInfoSMemPMICType
{
  uint32         nPMICModel;       /**< PMIC device model type, for Badger matches the revision id subtype */
  uint32         nPMICVersion;     /**< PMIC version, same format as Platform version */
} DalPlatformInfoSMemPMICType;


/**
  @brief Structure for the shared memory location which is used to store
  platform, chip, build, and pmic information.
 */
typedef struct
{
  uint32                          nFormat;              /**< Format of the structure. */
  uint32                          eChipId;              /**< Chip ID. */
  uint32                          nChipVersion;         /**< Chip version. */
  char                            aBuildId[DALPLATFORMINFO_SMEM_BUILD_ID_LENGTH];
                                                        /**< Build ID. */
  uint32                          nRawChipId;           /**< Raw chip ID. */
  uint32                          nRawChipVersion;      /**< Raw chip version. */
  DalPlatformInfoPlatformType     ePlatformType;        /**< Platform type. */
  uint32                          nPlatformVersion;     /**< Platform version. */
  uint32                          bFusion;              /**< TRUE if Fusion; FALSE otherwise. */
  uint32                          nPlatformSubtype;     /**< Platform subtype. */
  DalPlatformInfoSMemPMICType     aPMICInfo[DALPLATFORMINFO_SMEM_MAX_PMIC_DEVICES];
                                                        /**< DEPRECATED. May not contain the full
                                                             list. Use array at nPMICArrayOffset instead. */
  uint32                          nFoundryId;           /**< Chip foundry ID. */
  uint32                          nChipSerial;          /**< Chip serial number. */
  uint32                          nNumPMICs;            /**< Number of PMICs in array. */
  uint32                          nPMICArrayOffset;     /**< Offset from base of structure to
                                                             array of DalPlatformInfoSMemPMICType. */
  uint32                          nChipFamily;          /**< Chip family. */
  uint32                          nRawDeviceFamily;     /**< Raw device family. */
  uint32                          nRawDeviceNumber;     /**< Raw device number. */

  uint32                          nQFPROMChipId;        /**< QFPROM Chip ID   */
  char                            szChipIdStr[DALPLATFORMINFO_SMEM_MAX_CHIP_ID_LENGTH];   /**< Chip name */
  uint32                          nNumClusters;         /**< Number of clusters for PartialGoods to consider */
  uint32                          nClusterArrayOffset;  /**< Offset from base of structure to
                                                             uint32 array of defective CPU clusters. */
  uint32                          nNumDefectiveParts;   /**< Number of defective parts supported by
                                                              the PartialGoods APIs */
  uint32                          nDefectivePartsArrayOffset;  /**< Offset from base of structure to
                                                                   uint32 array of defective parts */
  uint32                          nModemSupported;      /**< 0 if not supported, nonzero if supported */
} DalPlatformInfoSMemType;


/** @} */ /* end_addtogroup platform_info */

#endif

