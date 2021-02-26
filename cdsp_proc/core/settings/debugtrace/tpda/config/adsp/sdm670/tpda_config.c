/*=============================================================================

FILE:         tpda_config.c

DESCRIPTION:   This file has attributes for TPDAs managed by TPDA driver

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "com_dtypes.h"
#include "DalTPDA.h"

/*TPDM name listed in TPDAPortSpec must match name in TPDM config */

TPDAPortSpec tpda_lpass_list[]={
   {"lpass",PORT0}
};

TPDADeviceProp tpda_device_list[]={
	{0x06B17000,"lpass",74,BIT_MASK(PORT0),"port_lpass_tpda",tpda_lpass_list,sizeof(tpda_lpass_list)/sizeof(TPDAPortSpec)},
};

const TableLength tpda_device_list_len[] = {
   {sizeof(tpda_device_list)/sizeof(TPDADeviceProp)}
};
