#ifndef __CVD_ADSP_VERSION_H__
#define __CVD_ADSP_VERSION_H__

/**
  @file cvd_adsp_version.h
*/

/*===========================================================================
Copyright (c) 2011, 2016 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/cvd_adsp_version.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/15/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
04/25/11    honghaol    Initial revision.

===========================================================================*/

/** @cond */
#define ADSP_CVD_VERSION    0x00010000
/** @endcond */

/** @addtogroup cvd_mvm_versions
@{ */
/** Mainline version of the Multimode Voice Manager (MVM) API.

  @versiontable{0.3,4.7,
  1  &  Initial version @tblendline
  2  &  Migration of Tx and Rx voice paths to full 16-bit scale @tblendline}
 */
#define MVM_API_VERSION  2

/** Version of the MVM API branched from the mainline.

  @versiontable{0.3,4.7,
  0  &  Initial version @tblendline}
 */
#define MVM_API_BRANCH_VERSION 0

/** @} *//* end_addtogroup cvd_mvm_versions */


/** @addtogroup cvd_cvs_versions
@{ */
/** Mainline version of the Core Voice Stream (CVS) API.

  @versiontable{0.3,4.7,
  1  &  Initial version @tblendline}
 */
#define CVS_API_VERSION  1

/** Version of the CVS API branched from the mainline.

  @versiontable{0.3,4.7,
  0  &  Initial version @tblendline}
 */
#define CVS_API_BRANCH_VERSION 0

/** @} *//* end_addtogroup cvd_cvs_versions */


/** @addtogroup cvd_cvp_versions
@{ */
/** Mainline version of the Core Voice Processing (CVP) API.

  @versiontable{0.3,4.7,
  1  &  Initial version @tblendline
  2  &  Added support for stereo echo canceller (EC) reference signal @tblendline}
 */
#define CVP_API_VERSION  2

/** Version of the CVP API branched from the mainline.

  @versiontable{0.3,4.7,
  0  &  Initial version @tblendline}
 */
#define CVP_API_BRANCH_VERSION 0

/** @} *//* end_addtogroup cvd_cvp_versions */


#endif /* __CVD_ADSP_VERSION_H__ */

