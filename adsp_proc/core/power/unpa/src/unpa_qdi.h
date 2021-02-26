/*==============================================================================
  FILE:         unpa_qdi.h
  
  OVERVIEW:     Shared defines, types and declarations for UNPA's QDI interface
  
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/unpa/src/unpa_qdi.h#1 $
$DateTime: 2017/07/21 22:10:47 $
===============================================================================*/
#ifndef _UNPA_QDI_H_
#define _UNPA_QDI_H_

#include "qurt_qdi_constants.h"

/*==============================================================================
                            TYPES & DEFINITIONS
 =============================================================================*/
#define UNPA_QDI_RESERVED_0                 (QDI_PRIVATE)
#define UNPA_QDI_RESERVED_1                 (QDI_PRIVATE + 1)
#define UNPA_QDI_RESERVED_2                 (QDI_PRIVATE + 2)
#define UNPA_QDI_RESERVED_3                 (QDI_PRIVATE + 3)
#define UNPA_QDI_RESERVED_4                 (QDI_PRIVATE + 4)
#define UNPA_QDI_RESERVED_5                 (QDI_PRIVATE + 5)
#define UNPA_QDI_RESERVED_6                 (QDI_PRIVATE + 6)
#define UNPA_QDI_RESERVED_7                 (QDI_PRIVATE + 7)

#define UNPA_QDI_CREATE_CLIENT              (QDI_PRIVATE + 8)
#define UNPA_QDI_ISSUE_SCALAR_REQUEST       (QDI_PRIVATE + 10)
#define UNPA_QDI_CREATE_QUERY_HANDLE        (QDI_PRIVATE + 11)
#define UNPA_QDI_QUERY                      (QDI_PRIVATE + 12)

#endif /* _UNPA_QDI_H_ */
