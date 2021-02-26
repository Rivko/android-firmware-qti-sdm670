/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  @file usta_rc.h
  @brief
  usta_rc is for return codes
============================================================================*/

#pragma once

typedef enum usta_rc
{
  /* No error occured ; success */
  USTA_RC_SUCCESS = 0,
  /* operation failed ; cannot continue */
  USTA_RC_FAILED,
  /* descriptor unavailable */
  USTA_RC_DESC_FAILED,
  /* Memory Allocation Failed */
  USTA_RC_NO_MEMORY,
  /* proto files not found in desccriptor pool */
  USTA_RC_NO_PROTO,
  /* No request messages found */
  USTA_RC_NO_REQUEST_MSG,
  /* requested attribute not found */
  USTA_RC_ATTRIB_FAILED,
  /*Required fileds are missing in the proto files*/
  USTA_RC_REQUIIRED_FILEDS_NOT_PRESENT,

}usta_rc;
