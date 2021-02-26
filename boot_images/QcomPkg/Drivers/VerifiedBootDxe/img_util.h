/** @file	
						 img_util.h
 
    
   Header file containing the data structures and functions for reading an image from a partiton 
 
   Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
 
 **/

/*=============================================================================
				 EDIT HISTORY
 
 
  when		 who	what, where, why
  --------	 ---	 ----------------------------------------------------------
  07/28/16	 SA     Initial version
 
 =============================================================================*/

#ifndef _IMG_UTIL_H_
#define _IMG_UTIL_H_

#define WIDECHAR_TO_CHAR_MASK 0x00ff
#define MAX_LABEL_SIZE 32

EFI_STATUS read_partition(UINT16 *pPartitionLabel, UINT8 *buff, UINTN buff_len);

EFI_STATUS write_partition(UINT16 *pPartitionLabel, UINT8 *buff, UINTN buff_len);

EFI_STATUS get_image_size(UINT8 *img_ptr, UINTN total_img_size, UINTN *actual_img_size);

EFI_STATUS convert_char8_to_char16(UINT8 *source, UINT16 *dest, UINTN len);

#endif
