#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALPropDef.h"
#ifndef DEVCFG_STANDALONE 
#include "DALStdDef.h" 
#include "DALSysTypes.h" 

#else 
#include "stubs.h" 

#include "devcfgTypes.h" 

#endif 
extern void * qupv3_perms_default_670_oem_hyp;
extern void * qupv3_perms_size_default_670_oem_hyp;
extern void * qupv3_gpii_perms_670_oem_hyp;
extern void * qupv3_gpii_perms_size_670_oem_hyp;
extern void * qupv3_perms_rumi_670_oem_hyp;
extern void * qupv3_perms_size_rumi_670_oem_hyp;
extern void * qupv3_perms_sheldon_670_oem_hyp;
extern void * qupv3_perms_size_sheldon_670_oem_hyp;

const DALSYSPropStructTblType DALPROP_StructPtrs_670_oem_hyp[9] =  {
	 {sizeof(void *), &qupv3_perms_default_670_oem_hyp},
	 {sizeof(void *), &qupv3_perms_size_default_670_oem_hyp},
	 {sizeof(void *), &qupv3_gpii_perms_670_oem_hyp},
	 {sizeof(void *), &qupv3_gpii_perms_size_670_oem_hyp},
	 {sizeof(void *), &qupv3_perms_rumi_670_oem_hyp},
	 {sizeof(void *), &qupv3_perms_size_rumi_670_oem_hyp},
	 {sizeof(void *), &qupv3_perms_sheldon_670_oem_hyp},
	 {sizeof(void *), &qupv3_perms_size_sheldon_670_oem_hyp},
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_670_oem_hyp[] = {

			0x00000288, 0x00000020, 0x0000014c, 0x0000014c, 0x0000014c, 
			0x00000001, 0x00000001, 0x00000214, 0x76707571, 0x65705f33, 
			0x00736d72, 0x76707571, 0x65705f33, 0x5f736d72, 0x657a6973, 
			0x69706700, 0x65705f69, 0x00736d72, 0x69697067, 0x7265705f, 
			0x735f736d, 0x00657a69, 0x456c6144, 0x5400766e, 0x65677261, 
			0x67664374, 0x70796800, 0x656c635f, 0x655f6e61, 0x74636976, 
			0x4157535f, 0x616c665f, 0x79680067, 0x65735f70, 0x65727563, 
			0x726f6d5f, 0x616e655f, 0x64656c62, 0x70796800, 0x6d756e5f, 
			0x636e755f, 0x65686361, 0x61725f64, 0x7365676e, 0x70796800, 
			0x636e755f, 0x65686361, 0x61725f64, 0x7365676e, 0x73696400, 
			0x656c6261, 0x6d6d735f, 0x63615f75, 0x616e6500, 0x5f656c62, 
			0x5f6d6163, 0x73796870, 0x6f6f625f, 0x72705f74, 0x6365746f, 
			0x6e6f6974, 0x6f6f6200, 0x72705f74, 0x6365746f, 0x5f646574, 
			0x5f6d6163, 0x73796870, 0x63657300, 0x70736964, 0x6e6f635f, 
			0x5f676966, 0x695f4d4c, 0x73007864, 0x69646365, 0x635f7073, 
			0x69666e6f, 0x73645f67, 0x65730069, 0x73696463, 0x6f635f70, 
			0x6769666e, 0x696d705f, 0x00000063, 0x00000001, 0x00000670, 
			0x00002070, 0x00000007, 0x8ab00000, 0x0b700000, 0x9e400000, 
			0x01400000, 0x97a00000, 0x00010000, 0x85fc0000, 0x00040000, 
			0x00000000, 0x00000002, 0x12800000, 0x00000000, 0x1280000c, 
			0x00000001, 0x1280001d, 0x00000002, 0x12800028, 0x00000003, 
			0xff00ff00, 0x12800000, 0x00000004, 0x1280000c, 0x00000005, 
			0x1280001d, 0x00000002, 0x12800028, 0x00000003, 0xff00ff00, 
			0x12800000, 0x00000006, 0x1280000c, 0x00000007, 0x1280001d, 
			0x00000002, 0x12800028, 0x00000003, 0xff00ff00, 0x12800000, 
			0x00000006, 0x1280000c, 0x00000007, 0x1280001d, 0x00000002, 
			0x12800028, 0x00000003, 0xff00ff00, 0x02800038, 0x80000002, 
			0x1480003f, 0x00000000, 0xff00ff00, 0x02800049, 0x00000001, 
			0x02800062, 0x00000000, 0x02800079, 0x00000004, 0x14800091, 
			0x0000000c, 0xff00ff00, 0x028000a5, 0x00000000, 0xff00ff00, 
			0x028000b5, 0x00000000, 0x148000d5, 0x00000030, 0xff00ff00, 
			0x028000ed, 0x00000000, 0x02800103, 0x00000000, 0x02800116, 
			0x00000000, 0xff00ff00 };



const StringDevice driver_list_670_oem_hyp[] = {
			{"/dev/buses/qupac",818130829u, 388, NULL, 0, NULL },
			{"/dev/buses/qupac/15",2116513506u, 424, NULL, 0, NULL },
			{"/dev/buses/qupac/08/10",1568994996u, 460, NULL, 0, NULL },
			{"/dev/buses/qupac/08/11",1568994997u, 496, NULL, 0, NULL },
			{"/dev/hyp_config_oem",4080669576u, 552, NULL, 0, NULL },
			{"/ac/smmu",4045178921u, 588, NULL, 0, NULL },
			{"secure_camera",1688615316u, 600, NULL, 0, NULL },
			{"secdisp_config",3657295525u, 620, NULL, 0, NULL }
};
