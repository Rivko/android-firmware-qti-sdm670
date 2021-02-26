/*==============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

==============================================================================*/
#ifndef DALPROPDEF_H
#define DALPROPDEF_H

//DAL prop idx for target_cfg
#define DALPROP_TARGET_PROP_NAME              "TargetCfg"
#define DALPROP_TARGET_ID_IDX                 0
#define DALPROP_TARGET_ARCH_IDX               1

//DAL Property attribute types
#define DALPROP_ATTR_TYPE_STRING_PTR          0x00000011
#define DALPROP_ATTR_TYPE_UINT32              0x00000002
#define DALPROP_ATTR_TYPE_BYTE_SEQ            0x00000008
#define DALPROP_ATTR_TYPE_BYTE_SEQ_PTR        0x00000018
#define DALPROP_ATTR_TYPE_UINT32_SEQ_PTR      0x00000014
#define DALPROP_ATTR_TYPE_STRUCT_PTR          0x00000012
#define DALPROP_ATTR_TYPE_BIT                 0x000F0000


//DAL Property end marker
#define DALPROP_ATTR_PROP_END                 0xFF00FF00

//Mask to seperate ID type and value
#define DALPROP_ATTR_TYPE_ID_VALUE_MASK       0x80000000
#define DALPROP_ATTR_TYPE_ID_ATTR_MASK        0x7FFFFFFF


//Mask to check visibility attr
#define DALPROP_ATTR_TYPE_ID_VISIBILITY_MASK        0x80000000

#define DALPROP_GET_ATTR_ID_VAL(dwAttrVal)\
	((dwAttrVal&DALPROP_ATTR_TYPE_ID_VALUE_MASK)>>22)

#define DALPROP_FORM_ATTR_ID_VAL_WORD(dwVal,dwAttr)\
	((((dwVal)<<22)& DALPROP_ATTR_TYPE_ID_VALUE_MASK)|dwAttr)

#define DALPROP_IS_ATTR_TYPE_ID(dwAttr)\
	((dwAttr&DALPROP_ATTR_TYPE_ID_VALUE_MASK) != 0)

//DAL Property data types
#define DALPROP_DATA_TYPE_STRING              0x00000001
#define DALPROP_DATA_TYPE_BYTE_SEQ            0x00000002
#define DALPROP_DATA_TYPE_UINT32_SEQ          0x00000003
#define DALPROP_DATA_TYPE_STRUCT_INC_FILE     0x00000004

/*------------------------------------------------------------------------*/
/*----------------- Global Configuration Defines -------------------------*/
/*------------------------------------------------------------------------*/

//DAL Image/Module Global Environments
#define DALPROP_MOD_ENV_BOOT                 0x0

#define DALPROP_MOD_ENV_PBL                  0x001
#define DALPROP_MOD_ENV_QCSBL                0x002
#define DALPROP_MOD_ENV_OEMSBL               0x003

#define DALPROP_MOD_ENV_DBL                  0x101
#define DALPROP_MOD_ENV_FSBL                 0x102
#define DALPROP_MOD_ENV_OSBL                 0x103

#define DALPROP_MOD_ENV_FLASH_TOOLS          0x201

#define DALPROP_MOD_ENV_APPSBL               0x1001
#define DALPROP_MOD_ENV_3POSBL               0x2001

#define DALPROP_MOD_ENV_TZOS                 0x10000001
#define DALPROP_MOD_ENV_QCOM                 0x20000001
#define DALPROP_MOD_ENV_QCOM_HLOS            0x40000001
#define DALPROP_MOD_ENV_HLOS                 0x80000001
#define DALPROP_MOD_ENV_3POS                 0x80000001

/* QCOM_HLOS */
#define DALPROP_HLOS_ENV_BREW                DALPROP_MOD_ENV_QCOM_HLOS
/* ENV_HLOS + ENV_3POS */
#define DALPROP_HLOS_ENV_LINUX               0x80000101
#define DALPROP_HLOS_ENV_WM                  0x80000201

#define DALPROP_HLOS_ENV_WM6                 0x80000301
#define DALPROP_HLOS_ENV_WM7                 0x80000601


//Core Architectures
// #define  DAL_CONFIG_ARCH   DAL_CONFIG_ARCH_<one of the following>
#define DAL_CONFIG_ARCH_WINSIM               0x8000
#define DAL_CONFIG_ARCH_QDSP                 0x4000
#define DAL_CONFIG_ARCH_QDSP6_V2             0x4020
#define DAL_CONFIG_ARCH_QDSP6_V3             0x4030
#define DAL_CONFIG_ARCH_QDSP6_V4             0x4040
#define DAL_CONFIG_ARCH_QDSP6_V5             0x4050
#define DAL_CONFIG_ARCH_ARM                  0x2000
#define DAL_CONFIG_ARCH_ARM_V4               0x2040
#define DAL_CONFIG_ARCH_ARM_V5               0x2050
#define DAL_CONFIG_ARCH_ARM_V6               0x2060
#define DAL_CONFIG_ARCH_ARM_V7               0x2070

// 4-bit processor Id
// currently used to lock a global context spinlock
#define DAL_CONFIG_PROC_NONE                 0x0
#define DAL_CONFIG_PROC_APSS                 0x1
#define DAL_CONFIG_PROC_MPSS                 0x2
#define DAL_CONFIG_PROC_ADSP                 0x3
#define DAL_CONFIG_PROC_WCNSS                0x4
#define DAL_CONFIG_PROC_RPM                  0x5
#define DAL_CONFIG_PROC_VENUS                0x6
#define DAL_CONFIG_PROC_GSS                  0x7
#define DAL_CONFIG_PROC_TZ                   0x8
#define DAL_CONFIG_PROC_VPU                  0x9

#endif //DALPROPDEF_H
