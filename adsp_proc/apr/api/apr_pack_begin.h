
/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/api/apr_pack_begin.h#1 $
  $Author: pwbldsvc $
*/

#if defined( __GNUC__ )
#elif defined( __arm__ )
  __packed
#elif defined( _MSC_VER )
  #pragma warning( disable:4103 )  /* Another header changing "pack". */
  #pragma pack( push, 1 )
#else
  #error Unsupported compiler.
#endif /* __GNUC__ */

