#ifndef __PM_DAL_PROP_IDS_H__
#define __PM_DAL_PROP_IDS_H__
/*! \file
 *  
 *  \brief  pm_dal_prop_id.h ----This file contains all the available PMIC DAL device config property values.
 *  \details This file contains all the available PMIC DAL device config property values.
 *  
 *  &copy; Copyright 2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.qdsp6/2.1/settings/pmic/pm/interface/pm_dal_prop_ids.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/17   akm      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

//PMIC HW peripheral specific dal properties -- starting from 1
#define PM_PROP_CLK_REG                        "PM_PROP_CLK_REG"
#define PM_PROP_SMPS_REG                       "PM_PROP_SMPS_REG"
#define PM_PROP_LDO_REG                        "PM_PROP_LDO_REG"
#define PM_PROP_VS_REG                         "PM_PROP_VS_REG"
#define PM_PROP_BOOST_REG                      "PM_PROP_BOOST_REG"
#define PM_PROP_FTS_VOLT                       "PM_PROP_FTS_VOLT"
#define PM_PROP_HFS_VOLT                       "PM_PROP_HFS_VOLT"
#define PM_PROP_BOOST_VOLT                     "PM_PROP_BOOST_VOLT"
#define PM_PROP_NMOS_VOLT                      "PM_PROP_NMOS_VOLT"
#define PM_PROP_N600_VOLT                      "PM_PROP_N600_VOLT"
#define PM_PROP_N1200_VOLT                     "PM_PROP_N1200_VOLT"
#define PM_PROP_PMOS_VOLT                      "PM_PROP_PMOS_VOLT"
#define PM_PROP_LN_VOLT                        "PM_PROP_LN_VOLT"

#define PM_PROP_CLK_SLEEP_REG                  "PM_PROP_CLK_SLEEP_REG"
#define PM_PROP_CLK_XO_REG                     "PM_PROP_CLK_XO_REG"
#define PM_PROP_CLK_COMMON                     "PM_PROP_CLK_COMMON"
#define PM_PROP_PWRON_REG                      "PM_PROP_PWRON_REG"
#define PM_PROP_GPIO_REG                       "PM_PROP_GPIO_REG"
#define PM_PROP_MPP_REG                        "PM_PROP_MPP_REG"
#define PM_PROP_RTC_REG                        "PM_PROP_RTC_REG"
#define PM_PROP_TALM_REG                       "PM_PROP_TALM_REG"
#define PM_PROP_SMPS_ILIMIT_LUT                "PM_PROP_SMPS_ILIMIT_LUT"
#define PM_PROP_FTS2p5_VOLT                    "PM_PROP_FTS2p5_VOLT"
#define PM_PROP_PBS_CLIENT_REG                 "PM_PROP_PBS_CLIENT_REG" 
#define PM_PROP_BCL_COMP_REG                   "PM_PROP_BCL_COMP_REG"
#define PM_PROP_VADC_REG                       "PM_PROP_VADC_REG"  

//ULT Buck props    
#define PM_PROP_ULT_BUCK_RANGE_1              "PM_PROP_ULT_BUCK_RANGE_1"
#define PM_PROP_ULT_BUCK_VOLT_1               "PM_PROP_ULT_BUCK_VOLT_1"
#define PM_PROP_ULT_BUCK_RANGE_2              "PM_PROP_ULT_BUCK_RANGE_2"
#define PM_PROP_ULT_BUCK_VOLT_2               "PM_PROP_ULT_BUCK_VOLT_2"
#define PM_PROP_ULT_PMOS_RANGE                "PM_PROP_ULT_PMOS_RANGE"
#define PM_PROP_ULT_PMOS_VOLT                 "PM_PROP_ULT_PMOS_VOLT"
#define PM_PROP_ULT_NMOS_RANGE                "PM_PROP_ULT_NMOS_RANGE"
#define PM_PROP_ULT_NMOS_VOLT                 "PM_PROP_ULT_NMOS_VOLT"

//Target/power grid specific dal properties 
#define PM_PROP_PMIC_NUM                     "PM_PROP_PMIC_NUM"
#define PM_PROP_SMPS_NUM                    "PM_PROP_SMPS_NUM"
#define PM_PROP_CLK_NUM                     "PM_PROP_CLK_NUM"
#define PM_PROP_LDO_NUM                     "PM_PROP_LDO_NUM"
#define PM_PROP_VS_NUM                      "PM_PROP_VS_NUM"
#define PM_PROP_BOOST_NUM                   "PM_PROP_BOOST_NUM"
#define PM_PROP_GPIO_NUM                    "PM_PROP_GPIO_NUM"
#define PM_PROP_MPP_NUM                     "PM_PROP_MPP_NUM"
#define PM_PROP_PWRON_NUM                   "PM_PROP_PWRON_NUM"
#define PM_PROP_RTC_NUM                     "PM_PROP_RTC_NUM"
#define PM_PROP_TALM_NUM                    "PM_PROP_TALM_NUM"
#define PM_PROP_MEGAXO_NUM                  "PM_PROP_MEGAXO_NUM"
#define PM_PROP_PAM_NODE_RSRCS               "PM_PROP_PAM_NODE_RSRCS"
#define PM_PROP_PAM_NODE_NUM                 "PM_PROP_PAM_NODE_NUM"
#define PM_PROP_REMOTE_LDO                   "PM_PROP_REMOTE_LDO"
#define PM_PROP_REMOTE_CLK                   "PM_PROP_REMOTE_CLK"
#define PM_PROP_REMOTE_SMPS                  "PM_PROP_REMOTE_SMPS"
#define PM_PROP_REMOTE_VS                    "PM_PROP_REMOTE_VS"
#define PM_PROP_XO_SPECIFIC                  "PM_PROP_XO_SPECIFIC"
#define PM_PROP_UICC_SPECIFIC                "PM_PROP_UICC_SPECIFIC"
#define PM_PROP_MPP_SPECIFIC                 "PM_PROP_MPP_SPECIFIC"
#define PM_PROP_MX                           "PM_PROP_MX"
#define PM_PROP_CX                           "PM_PROP_CX"
#define PM_PROP_MSS                          "PM_PROP_MSS"
#define PM_PROP_PBS_CLIENT_NUM               "PM_PROP_PBS_CLIENT_NUM"
#define PM_PROP_MSS_CFG_INFO                 "PM_PROP_MSS_CFG_INFO"
#define PM_PROP_PBS_SUPPORTED_CLIENTS        "PM_PROP_PBS_SUPPORTED_CLIENTS"
#define PM_PROP_RESOUCE_PUBLISHERS_INFO      "PM_PROP_RESOUCE_PUBLISHERS_INFO"
#define PM_PROP_BCL_COMP_LIMITS              "PM_PROP_BCL_COMP_LIMITS"
#define PM_PROP_VADC_BTM_LIMITS              "PM_PROP_VADC_BTM_LIMITS"
#define PM_PROP_RPMH_DRV_ID                  "PM_PROP_RPMH_DRV_ID"
#define PM_PROP_PRM_PAM_RSRCS                "PM_PROP_PRM_PAM_RSRCS"
#define PM_PROP_PRM_PAM_RSRC_NUM             "PM_PROP_PRM_PAM_RSRC_NUM"
#define PM_PROP_PRM_DEVICE_RSRCS             "PM_PROP_PRM_DEVICE_RSRCS"
#define PM_PROP_PRM_PUB_PAM_RSRCS            "PM_PROP_PRM_PUB_PAM_RSRCS"

//dummy dal properties to get rid of compiler warnings/errors -- staring from 201
#define PM_PROP_PMIC_DUMMY_1                 201
#define PM_PROP_PMIC_DUMMY_2                 202


#endif // PM_DAL_PROP_IDS_H
