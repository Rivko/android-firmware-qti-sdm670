#if defined (DIAG_IMAGE_QDSP6_PROC)
#ifndef DIAGMMPM_H_
#define DIAGMMPM_H_

/*========================================================================
                      DIAG_MMPM.H

  GENERAL DESCRIPTION:

  This file contains Diag MMPM functions 

  Copyright (c) 2011-2013,2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  10/24/16    gn     Changes to support common PW for ADSP, CDSP and SLPI
  02/01/13    sg     Fixed Klock work warnings
  03/01/13    sg      Migrated to new MMPM apis
  6/13/11     sg     Created file.

  ========================================================================== */

#include "comdef.h"
#include <stdlib.h>

int diag_mmpm_config(uint32 client_id);

int diag_mmpm_release(uint32 client_id);

uint32 diag_mmpm_register(void);

int diag_mmpm_deregister(uint32 client_id);

#endif
#endif /* DIAGMMPM_H_ */
