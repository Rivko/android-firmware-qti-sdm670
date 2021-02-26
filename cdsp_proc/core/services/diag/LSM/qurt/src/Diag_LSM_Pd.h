#ifndef DIAG_LSM_PD_H
#define DIAG_LSM_PD_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                Internal Header File for UserPD specific information

GENERAL DESCRIPTION
  
Copyright (c) 2016-2017 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/Diag_LSM_Pd.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/28/17   gn      Fixed issue with cmd reg_dereg test case
06/19/17   ph      Tagging the data with dynamic diag IDs.
12/13/16   gn      Changes to support command deregistration
11/16/16   sp      Changes specific to each userPD
===========================================================================*/

#include "diagpkt.h"
#include "diagdiag_v.h"



PACKED void * diag_LSM_test_cmd_handler (
  PACKED void *req_pkt,
  uint16 pkt_len
);

PACKED void* Diag_LSM_Delay_Test(
  PACKED void *req_pkt,
  uint16 pkt_len
);

PACKED void* diag_lsm_subsys_reg_dereg_test(
 PACKED void *req_pkt,
 uint16 pkt_len
);

PACKED void* diag_lsm_subsys_reg_test( 
 PACKED void *req_pkt, 
 uint16 pkt_len 
);

PACKED void* diag_lsm_subsys_reg_test2(
 PACKED void *req_pkt,
 uint16 pkt_len 
);

PACKED void* diag_lsm_subsys_reg_test3(
 PACKED void *req_pkt,
 uint16 pkt_len 
);

PACKED void* diag_lsm_subsys_reg_test4(
 PACKED void *req_pkt,
 uint16 pkt_len
);

/* This table must be maintained as userPDs are added! */
static const diagpkt_user_table_entry_type test_tbl_audio[] =
{
  {DIAGDIAG_AUDIO_TEST_CMD_F, DIAGDIAG_AUDIO_TEST_CMD_F, diag_LSM_test_cmd_handler},
  {DIAG_LSM_SUBSYS_CMD_REG_AUDIO, DIAG_LSM_SUBSYS_CMD_DEREG_AUDIO, diag_lsm_subsys_reg_dereg_test},
};
static const diagpkt_user_table_entry_type test_tbl_v2_audio[] =
{
  {DIAGDIAG_V2_AUDIO_TEST_CMD_F, DIAGDIAG_V2_AUDIO_TEST_CMD_F, Diag_LSM_Delay_Test},
};
static const diagpkt_user_table_entry_type test_tbl_wlan[] =
{
  {DIAGDIAG_WLAN_TEST_CMD_F, DIAGDIAG_WLAN_TEST_CMD_F, diag_LSM_test_cmd_handler},
  {DIAG_LSM_SUBSYS_CMD_REG_WLAN, DIAG_LSM_SUBSYS_CMD_DEREG_WLAN, diag_lsm_subsys_reg_dereg_test},
};
static const diagpkt_user_table_entry_type test_tbl_v2_wlan[] =
{
  {DIAGDIAG_V2_WLAN_TEST_CMD_F, DIAGDIAG_V2_WLAN_TEST_CMD_F, Diag_LSM_Delay_Test},
};
static const diagpkt_user_table_entry_type test_tbl_sensor[] =
{
  {DIAGDIAG_SENSOR_TEST_CMD_F, DIAGDIAG_SENSOR_TEST_CMD_F, diag_LSM_test_cmd_handler},
  {DIAG_LSM_SUBSYS_CMD_REG_SENSOR, DIAG_LSM_SUBSYS_CMD_DEREG_SENSOR, diag_lsm_subsys_reg_dereg_test},
};
static const diagpkt_user_table_entry_type test_tbl_v2_sensor[] =
{
  {DIAGDIAG_V2_SENSOR_TEST_CMD_F, DIAGDIAG_V2_SENSOR_TEST_CMD_F, Diag_LSM_Delay_Test},
};


static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_1_audio[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST_AUDIO, DIAG_LSM_SUBSYS_CMD_REG_TEST_AUDIO, diag_lsm_subsys_reg_test},
  {DIAG_LSM_SUBSYS_CMD_REG_TEST2_AUDIO, DIAG_LSM_SUBSYS_CMD_REG_TEST2_AUDIO, diag_lsm_subsys_reg_test2}
};
static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_2_audio[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST3_AUDIO, DIAG_LSM_SUBSYS_CMD_REG_TEST3_AUDIO, diag_lsm_subsys_reg_test3}
};
static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_3_audio[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST4_AUDIO, DIAG_LSM_SUBSYS_CMD_REG_TEST4_AUDIO, diag_lsm_subsys_reg_test4}
};


static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_1_wlan[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST_WLAN, DIAG_LSM_SUBSYS_CMD_REG_TEST_WLAN, diag_lsm_subsys_reg_test},
  {DIAG_LSM_SUBSYS_CMD_REG_TEST2_WLAN, DIAG_LSM_SUBSYS_CMD_REG_TEST2_WLAN, diag_lsm_subsys_reg_test2}
};
static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_2_wlan[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST3_WLAN, DIAG_LSM_SUBSYS_CMD_REG_TEST3_WLAN, diag_lsm_subsys_reg_test3}
};
static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_3_wlan[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST4_WLAN, DIAG_LSM_SUBSYS_CMD_REG_TEST4_WLAN, diag_lsm_subsys_reg_test4}
};


static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_1_sensor[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST_SENSOR, DIAG_LSM_SUBSYS_CMD_REG_TEST_SENSOR, diag_lsm_subsys_reg_test},
  {DIAG_LSM_SUBSYS_CMD_REG_TEST2_SENSOR, DIAG_LSM_SUBSYS_CMD_REG_TEST2_SENSOR, diag_lsm_subsys_reg_test2}
};
static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_2_sensor[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST3_SENSOR, DIAG_LSM_SUBSYS_CMD_REG_TEST3_SENSOR, diag_lsm_subsys_reg_test3}
};
static const diagpkt_user_table_entry_type diag_lsm_test_reg_tbl_3_sensor[] =
{
  {DIAG_LSM_SUBSYS_CMD_REG_TEST4_SENSOR, DIAG_LSM_SUBSYS_CMD_REG_TEST4_SENSOR, diag_lsm_subsys_reg_test4}
};

#endif /* DIAG_LSM_PD_H */
