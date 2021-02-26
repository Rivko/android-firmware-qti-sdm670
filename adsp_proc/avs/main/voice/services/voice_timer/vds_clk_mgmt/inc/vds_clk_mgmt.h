/*========================================================================

@file vds_clk_mgmt.h

Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the code for clock management when delivery risks are
detected in VDS.

*//*====================================================================== */

/*========================================================================
  Edit History

  $Header:

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  07/17/2017 sabdagir      Created file
  ========================================================================== */

#ifndef VDS_CLK_MGMT_H_
#define VDS_CLK_MGMT_H_

#include "Elite.h"
#include "avtimer_drv_api.h"

//#define VDS_DEBUG

typedef struct vcm_t vcm_t;

/** Initializes VDS clock management structure. This includes
 *  availability of NPA client, registering with NPA for clock
 *  management and querying max clock frequency at each voltage
 *  level. VDS is expected to set the pointer memory to NULL 
 *  when avt driver is closed/invalid.
 *
 *   @param [in/out] pointer of VDS clock mgmt to be initialized.
 *   @param [in/out] pointer of av-timer driver maintained by VDS. Its
 *         optimization to use VDS struct instead of VCM registering to
 *         AVT again.
 */
ADSPResult vcm_init(vcm_t **vcm_ptr, avtimer_drv_handle_t *avt_drv_ptr);

/** De-initializes VDS clock management structure.
 *
 *   @param [in/out] pointer of VDS clock mgmt to be initialized.
 */
ADSPResult vcm_deinit(vcm_t *vcm_ptr);

/** Boosts Q6 clock to next voltage level. 
 *  If boost_clk is passed as 0 this function internally
 *  queries current clock and request for clock corresponding to
 *  next voltage level, if the query is not succesful then it adds 
 *  a static adder to vote for additional clock.
 *
 *  @param [in/out] pointer of VDS clock mgmt.
 *  @param [in/out] additional clock to request.
 */
ADSPResult vcm_boost(vcm_t *vcm_ptr, uint32_t boost_clk);

/** Deboost Q6 clock. Removes clock request done earlier.
 *
 *  @param [in/out] pointer of VDS clock mgmt.
 */
ADSPResult vcm_deboost(vcm_t *vcm_ptr);


#endif /* VDS_CLK_MGMT_H_ */
