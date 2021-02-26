//This file contains the start addresses and sizes of EBI, SMI, and SMD
//8660 files defns 

#define EBI_START               0x00000000
#define EBI_SIZE                0xffffffff

#define EBI_SM_START            0x0fa00000
#define EBI_SM_SIZE             0x00200000

//The two SMD definitions below are currently not being used by SMD team.  
// For now, if you wish to change SMD size, please make the change in this file AND in smd_os.h
// SMD will use the definitions in a future release
#define SCL_SHARED_RAM_BASE     EBI_SM_START + 0x0CC00000
#define SCL_SHARED_RAM_SIZE     0x200000

#define SCL_MODEM_HEAP1_BASE    SCL_SHARED_RAM_BASE + SCL_SHARED_RAM_SIZE
#define SCL_MODEM_HEAP1_SIZE    0x100000

//Other less commonly changed pool definitions. Do not modify unless there is a good reason to do so.
#define DRIVER_POOL_BASE_0      0xfe000000
#define DRIVER_POOL_SIZE_0      0x00280000
#define DRIVER_POOL_BASE_1      0x04A00000
#define DRIVER_POOL_SIZE_1      0x00001000

#define AUD_REG_POOL_BASE       0xa0700000
#define AUD_REG_POOL_SIZE       0x1000

#define ADSP_REG_POOL_BASE      0xfe280000
#define ADSP_REG_POOL_SIZE      0x4000

#define ADSP_CLK_REG_POOL_BASE  0xfe288000
#define ADSP_CLK_REG_POOL_SIZE  0x8000

#define TCM_PHYSPOOL_BASE       0xfe400000
#define TCM_PHYSPOOL_SIZE       0x400000

#define LPM_PHYSPOOL_BASE       0xfe080000
#define LPM_PHYSPOOL_SIZE       0x20000         

#define SMD_REG_POOL_BASE       0xfe285000
#define SMD_REG_POOL_SIZE       0x1000

#define SMD_HW_SPINLOCK_BASE    0x01200600 /* SFPB_MUTEX_REG_BASE_PHYS */
#define SMD_HW_SPINLOCK_SIZE    0x1000

#define TOPCSR_POOL_BASE        0xa8700000
#define TOPCSR_POOL_SIZE        0x1000

#define SAW_REG_POOL_BASE       0xfe2b0000
#define SAW_REG_POOL_SIZE       0x1000

#define LPASS_CSR_REG_POOL_BASE       0xfe000000
#define LPASS_CSR_REG_POOL_SIZE       0x1000

#define QDSP6SS_REG_POOL_BASE    0xfe200000
#define QDSP6SS_REG_POOL_SIZE    0x1000

#define SIRC_REG_POOL_BASE      0xfe290000
#define SIRC_REG_POOL_SIZE      0x10000

#define SMI_POOL_BASE			0x38000000
#define SMI_POOL_SIZE			0x4000000

