/*===========================================================================

  Copyright (c) 2012-2014, Qualcomm  Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_H
#define RPM_H

typedef enum
{
    RPM_ACTIVE_SET       = 0,
    RPM_SLEEP_SET        = 1,
    RPM_NEXT_ACTIVE_SET  = 2,
    //RPM_NEXT_SLEEP_SET, -- not yet supported

    RPM_NUM_SETS
} rpm_set_type;

typedef enum
{
    RPM_TEST_REQ         = 0x74736574, // 'test' in little endiant

    RPM_CLOCK_0_REQ      = 0x306b6c63, // 'clk0' in little endian; misc clocks [CXO, QDSS]
    RPM_CLOCK_1_REQ      = 0x316b6c63, // 'clk1' in little endian; bus clocks [pnoc, snoc, cnoc, mm/axinoc, mm/ahbnoc, mm/ocmemnoc]
    RPM_CLOCK_2_REQ      = 0x326b6c63, // 'clk2' in little endian; memory clocks [bimc]

    RPM_BUS_SLAVE_REQ    = 0x766c7362, // 'bslv' in little endian
    RPM_BUS_MASTER_REQ   = 0x73616d62, // 'bmas' in little endian
    RPM_BUS_SPDM_CLK_REQ = 0x63707362, // 'bspc' in little endian 

    RPM_SMPS_A_REQ       = 0x61706D73, // 'smpa' in little endian
    RPM_LDO_A_REQ        = 0x616F646C, // 'ldoa'  in little endian
    RPM_NCP_A_REQ        = 0x6170636E, // 'ncpa'  in little endian
    RPM_VS_A_REQ         = 0x617376,   // 'vsa'   in little endian
    RPM_CLK_BUFFER_A_REQ = 0x616B6C63, // 'clka' in little endian

    RPM_SMPS_B_REQ       = 0x62706D73, // 'smpb' in little endian
    RPM_LDO_B_REQ        = 0x626F646C, // 'ldob'  in little endian
    RPM_NCP_B_REQ        = 0x6270636E, // 'ncpb'  in little endian
    RPM_VS_B_REQ         = 0x627376,   // 'vsb'   in little endian
    RPM_CLK_BUFFER_B_REQ = 0x626B6C63, // 'clk' in little endian

    RPM_OCMEM_POWER_REQ  = 0x706d636f, // 'ocmp' in little endian
    RPM_RBCPR_REQ        = 0x727063,   // 'cpr'in little endian
    RPM_MISC_REQ_TYPE    = 0x6373696d, // 'misc' in little endian
} rpm_resource_type;

typedef enum
{
    RPM_REQUEST_SERVICE  = 0x00716572, // 'req\0' in little endian
    RPM_SYSTEMDB_SERVICE = 0x00626473, // 'sdb\0' in little endian
} rpm_service_type;

#endif // RPM_H

