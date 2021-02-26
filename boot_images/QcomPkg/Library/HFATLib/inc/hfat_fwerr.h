/****************************************************************************
 * hfat_fwerr.h
 *
 * Short description.
 * Copyright (C) 2006-2008 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-04-30   sch   Added F_ERR_ISDIR
2006-07-06   sch   create

===========================================================================*/
#ifndef _FW_ERR_H_
#define _FW_ERR_H_

/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded 
 *
 * This software is copyrighted by and is the sole property of 
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,  
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 *
 * Filesystem's errorcodes
 *
 ***************************************************************************/

  enum
  {
/*  0 */ F_NO_ERROR,
/*  1 */ F_ERR_INVALIDDRIVE,
/*  2 */ F_ERR_NOTFORMATTED,
/*  3 */ F_ERR_INVALIDDIR,
/*  4 */ F_ERR_INVALIDNAME,
/*  5 */ F_ERR_NOTFOUND,
/*  6 */ F_ERR_DUPLICATED,
/*  7 */ F_ERR_NOMOREENTRY,
/*  8 */ F_ERR_NOTOPEN,
/*  9 */ F_ERR_EOF,
/* 10 */ F_ERR_RESERVED,
/* 11 */ F_ERR_NOTUSEABLE,
/* 12 */ F_ERR_LOCKED,
/* 13 */ F_ERR_ACCESSDENIED,
/* 14 */ F_ERR_NOTEMPTY,
/* 15 */ F_ERR_INITFUNC,
/* 16 */ F_ERR_CARDREMOVED,
/* 17 */ F_ERR_ONDRIVE,
/* 18 */ F_ERR_INVALIDSECTOR,
/* 19 */ F_ERR_READ,
/* 20 */ F_ERR_WRITE,
/* 21 */ F_ERR_INVALIDMEDIA,
/* 22 */ F_ERR_BUSY,
/* 23 */ F_ERR_WRITEPROTECT,
/* 24 */ F_ERR_INVFATTYPE,
/* 25 */ F_ERR_MEDIATOOSMALL,
/* 26 */ F_ERR_MEDIATOOLARGE,
/* 27 */ F_ERR_NOTSUPPSECTORSIZE,
/* 28 */ F_ERR_UNKNOWN,
/* 29 */ F_ERR_DRVALREADYMNT,
/* 30 */ F_ERR_TOOLONGNAME,
/* 31 */ F_ERR_NOTFORREAD,
/* 32 */ F_ERR_DELFUNC,
/* 33 */ F_ERR_ALLOCATION,
/* 34 */ F_ERR_INVALIDPOS,
/* 35 */ F_ERR_NOMORETASK,
/* 36 */ F_ERR_NOTAVAILABLE,
/* 37 */ F_ERR_ISDIR
  };

#ifdef __cplusplus
}
#endif

/****************************************************************************
 *
 * end of fwerr.h
 *
 ***************************************************************************/

#endif                          /* _FWERR_H_ */
