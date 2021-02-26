/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __EFICHIPINFOTYPES_H__
#define __EFICHIPINFOTYPES_H__

/** @addtogroup efi_chipInfo_data_types 
@{ */

/** Chip identification. */
/* Any new IDs must be added to the end. */
typedef enum
{
  /** @cond */
  EFICHIPINFO_ID_UNKNOWN    = 0,  /**< Unknown chip. */
  EFICHIPINFO_ID_MSMCOBALT  = 292,

  EFICHIPINFO_NUM_IDS        = 301,
  EFICHIPINFO_ID_32BITS      = 0x7FFFFFF
/** @endcond */
} EFIChipInfoIdType;

/** Chip family. This is the family type of the chip on which the software is 
  running. Each family may include multiple chip IDs.
*/
typedef enum
{
  /** @cond */
  EFICHIPINFO_FAMILY_UNKNOWN    = 0,  /**< Unknown family. */
  EFICHIPINFO_FAMILY_MSMCOBALT  = 67,
/** @cond */
  EFICHIPINFO_NUM_FAMILIES      = 71,
  EFICHIPINFO_FAMILY_32BITS     = 0x7FFFFFF
/** @endcond */
} EFIChipInfoFamilyType;


#endif /* __EFICHIPINFOTYPES_H__ */

