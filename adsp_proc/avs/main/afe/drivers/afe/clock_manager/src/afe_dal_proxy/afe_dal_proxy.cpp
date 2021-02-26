/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/12/18   saxena  Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "clock_manager.h"

/*
 * On targets where user PD is available, this function is required to
 * to avoid crash during PD restart.
 * Without creating proxy, mclk gets disabled during PD restart
 * causing the device to crash.
 */
ADSPResult afe_clock_manager_device_proxy(DalDeviceHandle *pHandle)
{
	DALResult res = DAL_DeviceCreateProxy(pHandle);
	if (DAL_SUCCESS != res)
	{
		return ADSP_EFAILED;
	}
	return ADSP_EOK;
}
