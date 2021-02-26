/*======================================================================
                        island_mgr.c 

GENERAL DESCRIPTION
  Island Manager functions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2014  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/
#include <qurt.h>
#include <island_mgr.h>
#include "qdss.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DALSys.h"
#include "DDIHWIO.h"
#include "msmhwiobase.h"
#define ISLAND_MGR_MAX_ATTRS 50 /* maximum number of attributes */
#define MAX_QDSS_HWIO_SIZE 5

extern qurt_island_t g_island_spec_id;
extern int g_island_mgr_inited;

qurt_island_attr_resource_t g_uImageIslandData[27] =
{
    /* Interrupt elements */
    {QURT_ISLAND_ATTR_INT, {{0}}  }, /* PMI */
    {QURT_ISLAND_ATTR_INT, {{2}}  }, /* q6ss_qtmr_irq[0] */
    {QURT_ISLAND_ATTR_INT, {{4}}  }, /* qTimer frame 3 */
    {QURT_ISLAND_ATTR_INT, {{32}}  }, /* HW DOG Bark interrupt */
    {QURT_ISLAND_ATTR_INT, {{24}} }, /* GSI Irq 0 */
    {QURT_ISLAND_ATTR_INT, {{25}} }, /* GSI Irq 1 */
    {QURT_ISLAND_ATTR_INT, {{26}} }, /* GSI Irq 2 */
    {QURT_ISLAND_ATTR_INT, {{27}} }, /* GSI Irq 3 */
    {QURT_ISLAND_ATTR_INT, {{28}} }, /* GSI Irq 4 */
    {QURT_ISLAND_ATTR_INT, {{29}} }, /* GSI Irq 5 */
    {QURT_ISLAND_ATTR_INT, {{41}} }, /* rsc_core_comp_irq */
    {QURT_ISLAND_ATTR_INT, {{246}} }, /* CTI interrupt */
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 
    {QURT_ISLAND_ATTR_INT, {{-1}} }, /* Reserved space for dynamic Interrupts */ 

    /* Memory range elements */
    {QURT_ISLAND_ATTR_MEMORY,
     {{0x62480000, 4}} }, /* SSC_QDSP6V65SS_CSR covers SPM, SLPC_CFG etc*/  //0x05880000
    {QURT_ISLAND_ATTR_MEMORY,
     {{(AOSS_BASE + 0x00250000), 0x10000}} }, /* Covers RPMH_PDC_SENSORS_SENSORS_PDC */

    {QURT_ISLAND_ATTR_MEMORY,
     {{0x62190000, 4}} }, /* ETM_CFG_BASE */

    /* Indicate end of list */
    {QURT_ISLAND_ATTR_END_OF_LIST, {{0}} } 
};

#if 0

If there are any physical addresses that need to be converted to virtual for an attr,
use the following code:

DalDeviceHandle *handle;
DALResult result;
uint8 *base_ptr;

      result = DAL_DeviceAttach(DALDEVICEID_HWIO, handle);
      if ( (DAL_SUCCESS != result) || (NULL == handle) ) {
         XXX ERROR XXX
      }
       if ( DAL_SUCCESS !=
             DalHWIO_MapRegion(handle,
                               “RPM_SS_MSG_RAM_START_ADDRESS_BASE”,
                               &base_ptr) ) {
         XXX ERROR XXX
      }
   }


#endif

int island_mgr_init(void)
{
    int qurt_rc;
    qurt_island_attr_t *island_attr;
    DalDeviceHandle *handle;
    DALResult dal_result;
    unsigned int clk_ctl_base_ptr;
    unsigned int tlmm_base_ptr;
    unsigned int ssc_base_ptr;

    dal_result = DAL_DeviceAttach(DALDEVICEID_HWIO, &handle);
    if((DAL_SUCCESS != dal_result) || (NULL == handle))
    {
	return ISLAND_MGR_EFAILED;
    }
    if(DAL_SUCCESS != DalHWIO_MapRegion(handle,"CLK_CTL",(uint8 **)&clk_ctl_base_ptr))
    {
	return ISLAND_MGR_EFAILED;
    }
    if(DAL_SUCCESS != DalHWIO_MapRegion(handle,"TLMM",(uint8 **)&tlmm_base_ptr))
    {
	return ISLAND_MGR_EFAILED;
    }
    if(DAL_SUCCESS != DalHWIO_MapRegion(handle,"LPASS",(uint8 **)&ssc_base_ptr))
    {
	return ISLAND_MGR_EFAILED;
    }
    
    qurt_rc = qurt_island_attr_create(&island_attr, ISLAND_MGR_MAX_ATTRS);
    if (qurt_rc != QURT_EOK)
    {
        return ISLAND_MGR_EFAILED;
    }

    qurt_rc = qurt_island_attr_add(island_attr, g_uImageIslandData);
    if (qurt_rc != QURT_EOK)
    {
	qurt_island_attr_delete(island_attr);
	return ISLAND_MGR_EFAILED;
    }

    qurt_rc = qurt_island_attr_add_mem(island_attr, clk_ctl_base_ptr, 0x000c0000);
    if (qurt_rc != QURT_EOK)
    {
	qurt_island_attr_delete(island_attr);
	return ISLAND_MGR_EFAILED;
    }
    qurt_rc = qurt_island_attr_add_mem(island_attr, tlmm_base_ptr, 0x00c00000);
    if (qurt_rc != QURT_EOK)
    {
	qurt_island_attr_delete(island_attr);
	return ISLAND_MGR_EFAILED;
    }

    qurt_rc = qurt_island_attr_add_mem(island_attr, ssc_base_ptr, 0x00c00000); //0x1E00000
    if (qurt_rc != QURT_EOK)
    {
	qurt_island_attr_delete(island_attr);
	return ISLAND_MGR_EFAILED;
    }
    qurt_rc = qurt_island_attr_add_pool (island_attr, "SSC_PRAM");
    if (qurt_rc != QURT_EOK)
    {
  		qurt_island_attr_delete(island_attr);
  		return ISLAND_MGR_EFAILED;
    }	
    qurt_rc = qurt_island_attr_add_pool (island_attr, "QURTOS_ISLAND_POOL");
    if (qurt_rc != QURT_EOK)
    {
	qurt_island_attr_delete(island_attr);
	return ISLAND_MGR_EFAILED;
    }

    unsigned int qdss_hwio_arr[MAX_QDSS_HWIO_SIZE];
    int qdss_hwio_arr_size; 
    QDSSGetUImageHWIOAddrs((uint32 *)&qdss_hwio_arr, MAX_QDSS_HWIO_SIZE, (int *)&qdss_hwio_arr_size);
    for (int i = 0; i < qdss_hwio_arr_size; i++)
    {
        qurt_rc = qurt_island_attr_add_mem(island_attr, qdss_hwio_arr[i], 4);
    if (qurt_rc != QURT_EOK)
    {
	qurt_island_attr_delete(island_attr);
	return ISLAND_MGR_EFAILED;
    }
    }

    qurt_rc = qurt_island_spec_create(&g_island_spec_id, island_attr);
    if (qurt_rc != QURT_EOK)
    {
	qurt_island_attr_delete(island_attr);
	return ISLAND_MGR_EFAILED;
    }
    qurt_island_attr_delete(island_attr);

    g_island_mgr_inited=1;

    return ISLAND_MGR_EOK;
}
