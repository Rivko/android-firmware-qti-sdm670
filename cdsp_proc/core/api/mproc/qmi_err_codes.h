#ifndef _QMI_ERR_CODES_H_
#define _QMI_ERR_CODES_H_
/******************************************************************************
  @file    qmi_err_codes.h
  @brief   Common QMI client error codes

  DESCRIPTION
  QMI error codes which can be shared by multiple modules providing 
  client interfaces for the QMI protocol.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/mproc/qmi_err_codes.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/12   ap      Initial Version
===========================================================================*/

#define QMI_NO_ERR                  0
#define QMI_INTERNAL_ERR            (-1)
#define QMI_SERVICE_ERR             (-2)
#define QMI_TIMEOUT_ERR             (-3)
#define QMI_EXTENDED_ERR            (-4)
#define QMI_PORT_NOT_OPEN_ERR       (-5)
#define QMI_MEMCOPY_ERROR           (-13)
#define QMI_INVALID_TXN             (-14)
#define QMI_CLIENT_ALLOC_FAILURE    (-15)
#define QMI_CLIENT_TRANSPORT_ERR    (-16)
#define QMI_CLIENT_PARAM_ERR        (-17)
#define QMI_CLIENT_INVALID_CLNT     (-18)
#define QMI_CLIENT_FW_NOT_UP        (-19)
#define QMI_CLIENT_INVALID_SIG      (-20)
#define QMI_XPORT_BUSY_ERR          (-21)

#endif
