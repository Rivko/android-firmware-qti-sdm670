/*======================================================================

               DALQdiRcvr.S

GENERAL DESCRIPTION
		  QDI DAL FORWARDING FUNCTIONS in QURTOS 
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2012,2017
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

========================================================================*/

/*======================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1.c4/dal/framework/src/qdi/DALQdiRcvr.S#1 $
$DateTime: 2018/07/30 01:21:36 $

when       who     what, where, why
--------   ---     -----------------------------------------------------
05/23/17   ak      Added fix for CR789618 - Dal Driver's table bound check.
06/08/12   aa      Updated for QDI2.0.
06/01/12   aa      Tuned for DAL, Bug fix and added comments for release.
04/03/12   jack    Initial Version
========================================================================*/
/*
   A QDI forwarding object in QURTOS looks like this:

   QDIForwardingObject:
     invoke    == points at dal_fwd_invoker.
     dalHandle == stores the guestOS DalDeviceHandle to which 
                  we forward the DDI calls.
     qdiHandle == stores the QDI handle we're mapped to.

*/
#include "qurt_qdi_constants.h"

	.equ QDI_DAL_FWD, 128                          /* 0-128 method id reserved for QDI FW */
   
	.text
	.global dal_fwd_invoker
 .type   dal_fwd_invoker, @function
   
dal_fwd_invoker:
	{
	p0 = !cmp.gt(r2,#QDI_DAL_FWD-1)                /* Is it a non-DAL method? */
	if (p0.new) jump:nt ##qurt_qdi_method_default
	}
dal_fwd_invoker_normal:
	{
	r14 = add(r2,#-QDI_DAL_FWD)		              /* Get dal vtable index */
	r1 = memw(r1+#12)			                       /* Get DalDeviceHandle in QURTOS, 12 bytes offset(qdi_obj_t)  */
	r7:6 = C1:0			                             /* Restore r7,r6 from loopcounter pair register argument from userland */
	}

   {
	r15 = memw(r1+#4)			                       /* Fetch vtable pointer from handle */
   r3 = r7                                        /* Restore r2 from r7 */
   r2 = r6                                        /* Restore r1 from r6 */
   }
   r7 = memw(r1+#12)                              /* Get Vtable length of driver in QURTOS, 12 bytes offset(dalhandle)  */
   {
   p0 = cmp.gt(r14,r7)                            /* Do bound check for vtable length */
   if (p0.new) jump:nt ##dal_fwd_error
   }

	r14 = memw(r15+r14<<#2)			                 /* Base Addr + index*4; Fetch vtable entry */
	jumpr r14                                      /* Invoke Driver method */

 .size dal_fwd_invoker, .-dal_fwd_invoker

   
dal_fwd_error:
   {
   r0 = #-1
   jumpr r31
   }
   
.size dal_fwd_error, .-dal_fwd_error