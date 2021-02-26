#ifndef REMOTE_SIMLOCK_H
#define REMOTE_SIMLOCK_H
/*===========================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                           FUNCTIONS

===========================================================================*/

extern uint8_t Modem_Remote_Simlock(
  uint8_t           * buffer_ptr,
  uint32_t            buffer_len
);

extern uint8_t Modem_Remote_Simlock_Get_Status(
  uint8_t           * buffer_ptr,
  uint32_t            buffer_len
);

#endif /* REMOTE_SIMLOCK_H */
