#include "AFEDsatDriver.h"
/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_open

 DESCRIPTION  Attempts to open danger gen functionality for the current port.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to open dangergen is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_open(afe_dev_port_t *pDevPort)
{
 return ADSP_EOK;
}

/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_close

 DESCRIPTION  Close Dangergen functionality.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to close dangergen is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_close(afe_dev_port_t *pDevPort)
{
  return ADSP_EOK;
}

/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_enable

 DESCRIPTION  Attempt to enable dangergen watchdog.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to enable watchdog is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_enable(afe_dev_port_t *pDevPort)
{
  return ADSP_EOK;

}

/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_disable

 DESCRIPTION  Attempt to disable dangergen watchdog.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to disable watchdog is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_disable(afe_dev_port_t *pDevPort)
{
  return ADSP_EOK;
}
