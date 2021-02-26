/*==============================================================================

  HW Event Configuration

DESCRIPTION
  This file has port configurations for trace funnels for a given chipset.

REFERENCES

             Copyright (c) 2016 QUALCOMM Technologies Inc.
                  All Rights Reserved.
                 QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "com_dtypes.h"
#include "DalTPDM.h"
#include "qdss_tpdm.h"


TPDMDeviceProp tpdm_device_attributes[]={
	{0x06B16000,"lpass","lpass",TPDM_DSB|TPDM_CMB,NULL},
};

const TableLength tpdm_device_attributes_len[] = 
{
{sizeof(tpdm_device_attributes)/sizeof(TPDMDeviceProp)}
};
