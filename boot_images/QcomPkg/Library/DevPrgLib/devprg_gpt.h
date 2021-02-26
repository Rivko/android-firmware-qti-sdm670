/**************************************************************************
 * FILE: devprg_gpt.h
 *
 * GPT functions for firehose.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_gpt.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-07-19   wek   Create.

===========================================================================*/
#ifndef __DEVPRG_GPT_H__
#define __DEVPRG_GPT_H__

#include "devprg_storage.h"

/** Fix the GPT tables given of a given storage handle.
 *
 * This function will correct some of the fields in the GPT header+table to
 * adjust to the disk size. The fields that are updated are:
 * 1. If grow_last_partition is 1, update the last LBA for the last
 *    partition in disk
 * 2. Update the CRC for the partition entries (GPT Table)
 * 3. Update the alternate LBA for the backup GPT header to the last LBA
 *    of the storage device.
 * 4. Update the CRC for the GPT header.
 *
 * All these steps are done for the Primary GPT (at LBA 1). The backup GPT is
 * created using the data of the primary table. If using this function there
 * is no need to program/write the backup GPT.
 *
 * param handle   The storage handle where the GPT needs to be fixed.
 * param grow_last_partition  1 if the last partition should grow to fill
 *                the disk. 0 if no partition sizes should be changed.
 *
 * return  0 on success, -1 on error.
 *
 */
dp_res_t devprg_fix_gpt(struct devprg_storage *handle,
                        int grow_last_partition);

#endif /* __DEVPRG_LOG_H__ */
