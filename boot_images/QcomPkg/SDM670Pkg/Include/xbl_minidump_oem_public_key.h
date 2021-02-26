#ifndef _MD_OEM_PUBLIC_KEY_H
#define _MD_OEM_PUBLIC_KEY_H

/*===========================================================================

                                OEM public key for minidump encryption
                                

GENERAL DESCRIPTION
  This header file contains OEM public key for minidump encryption

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2017,2018 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Include/xbl_minidump_oem_public_key.h#2 $
  $DateTime: 2018/06/14 23:30:59 $ 
  $Author: pwbldsvc $

when       who          what, where, why
--------   --------     ----------------------------------------------------------
26/04/17   bd           Initial version
============================================================================*/

/*===========================================================================*/


typedef struct
{
  uint32      bitLength;   
  char  *n;          
  const char  *p;
  const char  *q;
  char  *e;
  const char  *d;
}raw_key_t;


raw_key_t xbl_minidump_oem_public_key = {
    2048,
    /* n */
    "009741eda9188f5abe839661b175e62f57f94257d52ab4479644c2c3036d1b0e71fb86fef0dd7eabcad649dea4"
    "d9f76a1ef73af3adcd0ee3c36388e8df8808900a5ae651ab36a8a0e049abad05fcf97f9612f4ebbe51490ecf38"
	"5d3fec502eac234b936b65b6c0fb912622d96edffe520fb98518d1c95e3e36f1189c90a99b21c803fe9658312c"
    "3d9a09b29921325709d638ea223387c132dec138b44675473c5a7ab7d396a135b156c5a58fd0c183d39abba88f"
    "480da27df059da17f8218d07eca8c0754956bd369f03eac8ac9f2a5d9adc0bc5b25c51876d02ff770194152e03"
    "cdf5cc4d8fd866a09b085a0ecb1c3c5b1e038e6cf025a017e1f65d8cbeeb990b",        

    /* p */
    "",

    /* q */
    "",

    /* e */
    "000000000000000000000000000000000000000000000000000000000000000000000000000000"
    "000000000000000000000000000000000000000000000000000000000000000000000000000000"
    "00000000000000000000000000000000000000000000000000000000000000000000000000000"
    "0000000000000000010001",
    
    /* d */
    "",
};

#endif  /* _MD_OEM_PUBLIC_KEY_H */