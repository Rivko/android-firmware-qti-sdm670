#ifndef CHIPINFOTARGET_H
#define CHIPINFOTARGET_H

/* ===============================================================================

 FILE:  ChipInfoLoader.h

 DESCRIPTION: XBLLoader/SBL-specific internal header file for ChipInfo.
              This information isn't needed by any of the higher-level
              images, and is therefore excluded from ChipInfoLocal.h

 ===============================================================================

 Copyright (c) 2017 Qualcomm Technologies Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary and Confidential

 ===============================================================================

 $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/ChipInfoLoader.h#4 $
 $DateTime: 2018/10/01 04:56:38 $
 $Author: pwbldsvc $

 =============================================================================*/


/*=============================================================================
  Include Files
  ============================================================================*/
#include "ChipInfoLocal.h"

/*=============================================================================
  Macros
  =============================================================================*/

/*
 * CHIPINFO_MFGNUM_*
 *
 * Manufacturer IDs
 * TODO Doesn't seem to be used anywhere. It was used in DalChipInfo_GetManufacturer,
 * which we no longer have.
 */
#define CHIPINFO_MFGNUM_QUALCOMM 0x170

/*
 * CHIPINFO_PARTNUM_*
 *
 * Definitions of part number/JTAG-ID fields.
 * Organized in alphabetical order of target name.
 */
#define CHIPINFO_PARTNUM_APQ8026        0x804
#define CHIPINFO_PARTNUM_APQ8028        0x913
#define CHIPINFO_PARTNUM_APQ8062        0x912
#define CHIPINFO_PARTNUM_APQ8074        0x7B3
#define CHIPINFO_PARTNUM_APQ8074_AA     0x7BF
#define CHIPINFO_PARTNUM_APQ8074_AB     0x7BB
#define CHIPINFO_PARTNUM_APQ8074_PRO    0x7B7
#define CHIPINFO_PARTNUM_APQ8084_0      0x900
#define CHIPINFO_PARTNUM_APQ8084_1      0x901
#define CHIPINFO_PARTNUM_APQ8096        0x03E
#define CHIPINFO_PARTNUM_APQ8096SG      0x064
#define CHIPINFO_PARTNUM_APQ8096SGAU    0x071
#define CHIPINFO_PARTNUM_APQ8098        0x062
#define CHIPINFO_PARTNUM_FSM9900        0x80F
#define CHIPINFO_PARTNUM_FSM9910        0x80E
#define CHIPINFO_PARTNUM_FSM9915        0x80D
#define CHIPINFO_PARTNUM_FSM9950        0x80C
#define CHIPINFO_PARTNUM_FSM9955        0x80B
#define CHIPINFO_PARTNUM_FSM9965        0x80A
#define CHIPINFO_PARTNUM_MDM8225        0x7F0
#define CHIPINFO_PARTNUM_MDM8225M       0x7F3
#define CHIPINFO_PARTNUM_MDM8630        0x924
#define CHIPINFO_PARTNUM_MDM9225        0x7F1
#define CHIPINFO_PARTNUM_MDM9225M       0x7F2
#define CHIPINFO_PARTNUM_MDM9225M_1     0x7F2
#define CHIPINFO_PARTNUM_MDM9225_1      0x7F1
#define CHIPINFO_PARTNUM_MDM9230        0x922
#define CHIPINFO_PARTNUM_MDM9235M       0x923
#define CHIPINFO_PARTNUM_MDM9240        0x951
#define CHIPINFO_PARTNUM_MDM9245        0x955
#define CHIPINFO_PARTNUM_MDM9250        0x032
#define CHIPINFO_PARTNUM_MDM9255        0x034
#define CHIPINFO_PARTNUM_MDM9320        0x7F7
#define CHIPINFO_PARTNUM_MDM9330        0x925
#define CHIPINFO_PARTNUM_MDM9340        0x952
#define CHIPINFO_PARTNUM_MDM9350        0x039
#define CHIPINFO_PARTNUM_MDM9625        0x7F4
#define CHIPINFO_PARTNUM_MDM9625M       0x7F5
#define CHIPINFO_PARTNUM_MDM9630        0x920
#define CHIPINFO_PARTNUM_MDM9635M       0x921
#define CHIPINFO_PARTNUM_MDM9640        0x950
#define CHIPINFO_PARTNUM_MDM9645        0x954
#define CHIPINFO_PARTNUM_MDM9650        0x03A
#define CHIPINFO_PARTNUM_MDM9655        0x03B
#define CHIPINFO_PARTNUM_MDM9660        0x07D
#define CHIPINFO_PARTNUM_MDM9665        0x07F
#define CHIPINFO_PARTNUM_MPQ8092        0x930
#define CHIPINFO_PARTNUM_MSM8110        0x810
#define CHIPINFO_PARTNUM_MSM8112        0x816
#define CHIPINFO_PARTNUM_MSM8126        0x803
#define CHIPINFO_PARTNUM_MSM8128        0x914
#define CHIPINFO_PARTNUM_MSM8210        0x811
#define CHIPINFO_PARTNUM_MSM8212        0x814
#define CHIPINFO_PARTNUM_MSM8226        0x800
#define CHIPINFO_PARTNUM_MSM8228        0x915
#define CHIPINFO_PARTNUM_MSM8262        0x911
#define CHIPINFO_PARTNUM_MSM8274        0x7B2
#define CHIPINFO_PARTNUM_MSM8274_AA     0x7BE
#define CHIPINFO_PARTNUM_MSM8274_AB     0x7BA
#define CHIPINFO_PARTNUM_MSM8274_PRO    0x7B6
#define CHIPINFO_PARTNUM_MSM8326        0x806
#define CHIPINFO_PARTNUM_MSM8510        0x817
#define CHIPINFO_PARTNUM_MSM8512        0x818
#define CHIPINFO_PARTNUM_MSM8526        0x802
#define CHIPINFO_PARTNUM_MSM8528        0x916
#define CHIPINFO_PARTNUM_MSM8610        0x812
#define CHIPINFO_PARTNUM_MSM8612        0x815
#define CHIPINFO_PARTNUM_MSM8626        0x801
#define CHIPINFO_PARTNUM_MSM8628        0x917
#define CHIPINFO_PARTNUM_MSM8674        0x7B1
#define CHIPINFO_PARTNUM_MSM8674_AA     0x7BD
#define CHIPINFO_PARTNUM_MSM8674_AB     0x7B9
#define CHIPINFO_PARTNUM_MSM8674_PRO    0x7B5
#define CHIPINFO_PARTNUM_MSM8810        0x813
#define CHIPINFO_PARTNUM_MSM8916        0x705
#define CHIPINFO_PARTNUM_MSM8926        0x805
#define CHIPINFO_PARTNUM_MSM8928        0x918
#define CHIPINFO_PARTNUM_MSM8962        0x910
#define CHIPINFO_PARTNUM_MSM8974        0x7B0
#define CHIPINFO_PARTNUM_MSM8974_AA     0x7BC
#define CHIPINFO_PARTNUM_MSM8974_AB     0x7B8
#define CHIPINFO_PARTNUM_MSM8974_PRO    0x7B4
#define CHIPINFO_PARTNUM_MSM8994        0x940
#define CHIPINFO_PARTNUM_MSM8996        0x947
#define CHIPINFO_PARTNUM_MSM8996SG      0x05F
#define CHIPINFO_PARTNUM_MSM8996SGAU    0x06F
#define CHIPINFO_PARTNUM_MSM8997        0x06C
#define CHIPINFO_PARTNUM_MSM8998        0x05E
#define CHIPINFO_PARTNUM_MSM8998        0x05E
#define CHIPINFO_PARTNUM_QDF2432        0x944
#define CHIPINFO_PARTNUM_SDM450         0x09A
#define CHIPINFO_PARTNUM_SDM630         0x07E
#define CHIPINFO_PARTNUM_SDM660         0x07D
#define CHIPINFO_PARTNUM_SDM670         0x091
#define CHIPINFO_PARTNUM_SDA670         0x093
#define CHIPINFO_PARTNUM_SDC830         0x092
#define CHIPINFO_PARTNUM_SDM830         0x08F
#define CHIPINFO_PARTNUM_SDC845         0x08E
#define CHIPINFO_PARTNUM_SDA845         CHIPINFO_PARTNUM_SDC845
#define CHIPINFO_PARTNUM_SDM845         0x08B
#define CHIPINFO_PARTNUM_SDX24          0x096
#define CHIPINFO_PARTNUM_SDX24M         0x097
#define CHIPINFO_PARTNUM_QCS605         0x0AA
#define CHIPINFO_PARTNUM_SDM710         0x0DB
#define CHIPINFO_PARTNUM_SXR1120        0x0ED
#define CHIPINFO_PARTNUM_SXR1130        0x0EA
#define CHIPINFO_PARTNUM_SDM712         0x108

/*
 * Placeholders for currently unknown part numbers
 */
#define CHIPINFO_PARTNUM_UNKNOWN        0xFFFF
#define CHIPINFO_PARTNUM_SDM855         CHIPINFO_PARTNUM_UNKNOWN
#define CHIPINFO_PARTNUM_SDM1000        CHIPINFO_PARTNUM_UNKNOWN

/*=============================================================================
  Type definitions
  =============================================================================*/
/**
 * ChipInfoArchFamilyType
 *
 * ChipInfo architectural families. Each architectural family refers to
 * multiple ChipInfo internal device families
 */
typedef enum
{
  CHIPINFO_ARCH_FAMILY_UNKNOWN = 0,
  CHIPINFO_ARCH_FAMILY_BADGER,
  CHIPINFO_ARCH_FAMILY_ULT,
  CHIPINFO_ARCH_FAMILY_BEAR,
  CHIPINFO_ARCH_FAMILY_HONEYBADGER,
  CHIPINFO_ARCH_FAMILY_QCA,
  CHIPINFO_ARCH_FAMILY_DRAGONFLY,
  CHIPINFO_ARCH_FAMILY_HOYA,

  CHIPINFO_NUM_ARCH_FAMILIES,
} ChipInfoArchFamilyType;

/**
 * ChipInfoDataLUTType
 *
 * Static per-chip data definitions
 *
 * szChipIdStr:   String representing the MSM name.
 * eChipId:       Incremental, internal chip ID
 * nPartNum:      JTAG ID
 * nModem:        Bitmask of supported modems on this MSM.
 *
 */
typedef struct
{
  const char          *szChipIdStr;
  ChipInfoIdType      eChipId;
  uint32              nPartNum;
  ChipInfoModemType   nModem;
} ChipInfoDataLUTType;

/**
 * ChipInfoArchFamilyDataType
 *
 * Information about the chip families represented by each
 * architectural family
 *
 * eArchFamily: the architectural family
 * nArchFamilyDeviceNum: the device number within the architectural family
 * eDeviceFamily: ChipInfo internal family for this device
 * eDefaultChipId: Default ChipInfo internal chip id to use if it couldn't be determined using the LUT.
 */
typedef struct
{
  ChipInfoArchFamilyType  eArchFamily;
  uint32                  nArchFamilyDeviceNum;
  ChipInfoFamilyType      eDeviceFamily;
  ChipInfoIdType          eDefaultChipId;
} ChipInfoArchFamilyDataType;


/**
 * ChipInfoBSPType
 *
 * Static BSP data.
 */
typedef struct
{
  const ChipInfoDataLUTType         *aLUT;
  const uint32                      nNumLUTEntries;
  const ChipInfoArchFamilyDataType  *aArchFamilyData;
  const uint32                      nNumArchFamilyDataEntries;
} ChipInfoBSPType;


/*=============================================================================
  Interface declarations
  =============================================================================*/
/**
 * Returns the ChipInfo BSP data, containing mappings from JTAG ID
 * to ChipID, Chip ID string, modem support, and
 * architecture family/device number. This information is not needed by
 * any of the higher-level images, and is only used during init time
 * by XBL Loader/SBL.
 *
 * @return Pointer to the statically-allocated ChipInfo BSP structure
 */
ChipInfoBSPType *ChipInfo_GetBSPData(void);

/**
 * Perform chipset-specific initialization, e.g. reading partial goods binning
 * information which might come from different fuses on each chip.
 *
 * @param[in,out] pChipInfoCtxt Pointer to a ChipInfoCtxtType which
 *                              has caller-allocated buffers to store
 *                              chipset-specific initialization
 *                              data.
 * TODO Should this function go in its own header file (ChipInfoTargetLib.h)
 *      to better show its relationship with its implementation in
 *      <Target>Pkg/ChipInfoTargetLib.c?
 */
void ChipInfo_InitChipset(ChipInfoCtxtType *pChipInfoCtxt);


/*=============================================================================
  Interface definitions
  =============================================================================*/


#endif /* CHIPINFOTARGET_H */

