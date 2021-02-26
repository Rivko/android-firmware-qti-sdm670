#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "SMMUMasters.h"
#include "SMMUConfig.h"
#include "SMMUTypes.h"
#include "com_dtypes.h"
#include "AccessControlHyp.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "custxawaanaaa.h"
#include "targxawaanaaa.h"
#include "custremoteapis.h"
#include "custtarget.h"
#include "custsdcc.h"
#include "custsurf.h"
#include "custdiag.h"
#include "custefs.h"
#include "custpmic.h"
#include "custsio_8660.h"
#include "custsec.h"
#include "custsfs.h"
#include "armasm.h"
#include "ACCommon.h"
#include "IAccessControl.h"
#include "object.h"
#include "tz_syscall_pub.h"
#include "SMMUDynamicSID.h"
#include "IxErrno.h"
#include "SMMUImplDef.h"
#include "HALSMMU.h"
#include "DALPropDef.h"
#ifndef DEVCFG_STANDALONE 
#include "DALStdDef.h" 
#include "DALSysTypes.h" 

#else 
#include "stubs.h" 

#include "devcfgTypes.h" 

#endif 
extern void * device_qup_0i0_670_hyp;
extern void * device_qup_0i0i_670_hyp;
extern void * device_qup_0i1_670_hyp;
extern void * device_qup_0i1i_670_hyp;
extern void * device_qup_0i2_670_hyp;
extern void * device_qup_0i2i_670_hyp;
extern void * device_qup_0i3_670_hyp;
extern void * device_qup_0i3i_670_hyp;
extern void * device_qup_0i4_670_hyp;
extern void * device_qup_0i4i_670_hyp;
extern void * device_qup_0i5_670_hyp;
extern void * device_qup_0i5i_670_hyp;
extern void * device_qup_0i6_670_hyp;
extern void * device_qup_0i6i_670_hyp;
extern void * device_qup_0i7_670_hyp;
extern void * device_qup_0i7i_670_hyp;
extern void * device_qup_0i8_670_hyp;
extern void * device_qup_0i8i_670_hyp;
extern void * device_qup_0i9_670_hyp;
extern void * device_qup_0i9i_670_hyp;
extern void * device_qup_0i10_670_hyp;
extern void * device_qup_0i10i_670_hyp;
extern void * device_qup_0i11_670_hyp;
extern void * device_qup_0i11i_670_hyp;
extern void * device_qup_0i12_670_hyp;
extern void * device_qup_0i12i_670_hyp;
extern void * device_qup_0s0_670_hyp;
extern void * device_qup_0s0s_670_hyp;
extern void * device_qup_0s0g0v1_670_hyp;
extern void * device_qup_0s0g2v1_670_hyp;
extern void * device_qup_0s1_670_hyp;
extern void * device_qup_0s1s_670_hyp;
extern void * device_qup_0s1g0v1_670_hyp;
extern void * device_qup_0s1g2v1_670_hyp;
extern void * device_qup_0s2_670_hyp;
extern void * device_qup_0s2s_670_hyp;
extern void * device_qup_0s2g0v1_670_hyp;
extern void * device_qup_0s2g2v1_670_hyp;
extern void * device_qup_0s3_670_hyp;
extern void * device_qup_0s3s_670_hyp;
extern void * device_qup_0s3g0v1_670_hyp;
extern void * device_qup_0s3g2v1_670_hyp;
extern void * device_qup_0s4_670_hyp;
extern void * device_qup_0s4s_670_hyp;
extern void * device_qup_0s4g0v1_670_hyp;
extern void * device_qup_0s4g2v1_670_hyp;
extern void * device_qup_0s5_670_hyp;
extern void * device_qup_0s5s_670_hyp;
extern void * device_qup_0s5g0v1_670_hyp;
extern void * device_qup_0s5g2v1_670_hyp;
extern void * device_qup_0s6_670_hyp;
extern void * device_qup_0s6s_670_hyp;
extern void * device_qup_0s6g0v1_670_hyp;
extern void * device_qup_0s6g2v1_670_hyp;
extern void * device_qup_0s7_670_hyp;
extern void * device_qup_0s7s_670_hyp;
extern void * device_qup_0s7g0v1_670_hyp;
extern void * device_qup_0s7g2v1_670_hyp;
extern void * device_qup_0c_670_hyp;
extern void * device_qup_0cs_670_hyp;
extern void * device_qup_1i0_670_hyp;
extern void * device_qup_1i0i_670_hyp;
extern void * device_qup_1i1_670_hyp;
extern void * device_qup_1i1i_670_hyp;
extern void * device_qup_1i2_670_hyp;
extern void * device_qup_1i2i_670_hyp;
extern void * device_qup_1i3_670_hyp;
extern void * device_qup_1i3i_670_hyp;
extern void * device_qup_1i4_670_hyp;
extern void * device_qup_1i4i_670_hyp;
extern void * device_qup_1i5_670_hyp;
extern void * device_qup_1i5i_670_hyp;
extern void * device_qup_1i6_670_hyp;
extern void * device_qup_1i6i_670_hyp;
extern void * device_qup_1i7_670_hyp;
extern void * device_qup_1i7i_670_hyp;
extern void * device_qup_1i8_670_hyp;
extern void * device_qup_1i8i_670_hyp;
extern void * device_qup_1i9_670_hyp;
extern void * device_qup_1i9i_670_hyp;
extern void * device_qup_1i10_670_hyp;
extern void * device_qup_1i10i_670_hyp;
extern void * device_qup_1i11_670_hyp;
extern void * device_qup_1i11i_670_hyp;
extern void * device_qup_1i12_670_hyp;
extern void * device_qup_1i12i_670_hyp;
extern void * device_qup_1s0_670_hyp;
extern void * device_qup_1s0s_670_hyp;
extern void * device_qup_1s0g0v1_670_hyp;
extern void * device_qup_1s0g2v1_670_hyp;
extern void * device_qup_1s1_670_hyp;
extern void * device_qup_1s1s_670_hyp;
extern void * device_qup_1s1g0v1_670_hyp;
extern void * device_qup_1s1g2v1_670_hyp;
extern void * device_qup_1s2_670_hyp;
extern void * device_qup_1s2s_670_hyp;
extern void * device_qup_1s2g0v1_670_hyp;
extern void * device_qup_1s2g2v1_670_hyp;
extern void * device_qup_1s3_670_hyp;
extern void * device_qup_1s3s_670_hyp;
extern void * device_qup_1s3g0v1_670_hyp;
extern void * device_qup_1s3g2v1_670_hyp;
extern void * device_qup_1s4_670_hyp;
extern void * device_qup_1s4s_670_hyp;
extern void * device_qup_1s4g0v1_670_hyp;
extern void * device_qup_1s4g2v1_670_hyp;
extern void * device_qup_1s5_670_hyp;
extern void * device_qup_1s5s_670_hyp;
extern void * device_qup_1s5g0v1_670_hyp;
extern void * device_qup_1s5g2v1_670_hyp;
extern void * device_qup_1s6_670_hyp;
extern void * device_qup_1s6s_670_hyp;
extern void * device_qup_1s6g0v1_670_hyp;
extern void * device_qup_1s6g2v1_670_hyp;
extern void * device_qup_1s7_670_hyp;
extern void * device_qup_1s7s_670_hyp;
extern void * device_qup_1s7g0v1_670_hyp;
extern void * device_qup_1s7g2v1_670_hyp;
extern void * device_qup_1c_670_hyp;
extern void * device_qup_1cs_670_hyp;
extern void * qupv3_se_hw_670_hyp;
extern void * qupv3_se_hw_size_670_hyp;
extern void * qupv3_common_hw_670_hyp;
extern void * qupv3_common_hw_size_670_hyp;
extern void * qupv3_0_clocks_arr_670_hyp;
extern void * qupv3_0_clocks_arr_size_670_hyp;
extern void * qupv3_1_clocks_arr_670_hyp;
extern void * qupv3_1_clocks_arr_size_670_hyp;


static SMMUImplDef1ConfigEntry_t	devcfg_0={0x0,0,0,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0,{0,0,0,0,0,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0,{0,0,0,0,0,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0,{0,0,0,0,0,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0x0,{0,0,0,0,0,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0,{0,0,0,0,0,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0,{0,0,0,0,0,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0,0,0,0,0,0,0,0,0,0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0},0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0,0,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF}};


static SMMUImplDef1ConfigEntry_t	devcfg_1={0x05040000,2,0,0,0,0x70,0x1055,0x6,0x3FF,0xFFFFFFFF,0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0},0x204,0x11000,0x800,0xFFFFFFFF,0xFFFFFFFF,0x1A5551,0x2AAA2F82,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF},0xFFFFFFFF,0xFFFFFFFF,{0xFFFFFFFF,0xFFFFFFFF}};

const DALSYSPropStructTblType DALPROP_StructPtrs_670_hyp[131] =  {
	 {sizeof(devcfg_0), &devcfg_0},
	 {sizeof(devcfg_1), &devcfg_1},
	 {sizeof(void *), &device_qup_0i0_670_hyp},
	 {sizeof(void *), &device_qup_0i0i_670_hyp},
	 {sizeof(void *), &device_qup_0i1_670_hyp},
	 {sizeof(void *), &device_qup_0i1i_670_hyp},
	 {sizeof(void *), &device_qup_0i2_670_hyp},
	 {sizeof(void *), &device_qup_0i2i_670_hyp},
	 {sizeof(void *), &device_qup_0i3_670_hyp},
	 {sizeof(void *), &device_qup_0i3i_670_hyp},
	 {sizeof(void *), &device_qup_0i4_670_hyp},
	 {sizeof(void *), &device_qup_0i4i_670_hyp},
	 {sizeof(void *), &device_qup_0i5_670_hyp},
	 {sizeof(void *), &device_qup_0i5i_670_hyp},
	 {sizeof(void *), &device_qup_0i6_670_hyp},
	 {sizeof(void *), &device_qup_0i6i_670_hyp},
	 {sizeof(void *), &device_qup_0i7_670_hyp},
	 {sizeof(void *), &device_qup_0i7i_670_hyp},
	 {sizeof(void *), &device_qup_0i8_670_hyp},
	 {sizeof(void *), &device_qup_0i8i_670_hyp},
	 {sizeof(void *), &device_qup_0i9_670_hyp},
	 {sizeof(void *), &device_qup_0i9i_670_hyp},
	 {sizeof(void *), &device_qup_0i10_670_hyp},
	 {sizeof(void *), &device_qup_0i10i_670_hyp},
	 {sizeof(void *), &device_qup_0i11_670_hyp},
	 {sizeof(void *), &device_qup_0i11i_670_hyp},
	 {sizeof(void *), &device_qup_0i12_670_hyp},
	 {sizeof(void *), &device_qup_0i12i_670_hyp},
	 {sizeof(void *), &device_qup_0s0_670_hyp},
	 {sizeof(void *), &device_qup_0s0s_670_hyp},
	 {sizeof(void *), &device_qup_0s0g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s0g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0s1_670_hyp},
	 {sizeof(void *), &device_qup_0s1s_670_hyp},
	 {sizeof(void *), &device_qup_0s1g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s1g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0s2_670_hyp},
	 {sizeof(void *), &device_qup_0s2s_670_hyp},
	 {sizeof(void *), &device_qup_0s2g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s2g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0s3_670_hyp},
	 {sizeof(void *), &device_qup_0s3s_670_hyp},
	 {sizeof(void *), &device_qup_0s3g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s3g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0s4_670_hyp},
	 {sizeof(void *), &device_qup_0s4s_670_hyp},
	 {sizeof(void *), &device_qup_0s4g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s4g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0s5_670_hyp},
	 {sizeof(void *), &device_qup_0s5s_670_hyp},
	 {sizeof(void *), &device_qup_0s5g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s5g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0s6_670_hyp},
	 {sizeof(void *), &device_qup_0s6s_670_hyp},
	 {sizeof(void *), &device_qup_0s6g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s6g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0s7_670_hyp},
	 {sizeof(void *), &device_qup_0s7s_670_hyp},
	 {sizeof(void *), &device_qup_0s7g0v1_670_hyp},
	 {sizeof(void *), &device_qup_0s7g2v1_670_hyp},
	 {sizeof(void *), &device_qup_0c_670_hyp},
	 {sizeof(void *), &device_qup_0cs_670_hyp},
	 {sizeof(void *), &device_qup_1i0_670_hyp},
	 {sizeof(void *), &device_qup_1i0i_670_hyp},
	 {sizeof(void *), &device_qup_1i1_670_hyp},
	 {sizeof(void *), &device_qup_1i1i_670_hyp},
	 {sizeof(void *), &device_qup_1i2_670_hyp},
	 {sizeof(void *), &device_qup_1i2i_670_hyp},
	 {sizeof(void *), &device_qup_1i3_670_hyp},
	 {sizeof(void *), &device_qup_1i3i_670_hyp},
	 {sizeof(void *), &device_qup_1i4_670_hyp},
	 {sizeof(void *), &device_qup_1i4i_670_hyp},
	 {sizeof(void *), &device_qup_1i5_670_hyp},
	 {sizeof(void *), &device_qup_1i5i_670_hyp},
	 {sizeof(void *), &device_qup_1i6_670_hyp},
	 {sizeof(void *), &device_qup_1i6i_670_hyp},
	 {sizeof(void *), &device_qup_1i7_670_hyp},
	 {sizeof(void *), &device_qup_1i7i_670_hyp},
	 {sizeof(void *), &device_qup_1i8_670_hyp},
	 {sizeof(void *), &device_qup_1i8i_670_hyp},
	 {sizeof(void *), &device_qup_1i9_670_hyp},
	 {sizeof(void *), &device_qup_1i9i_670_hyp},
	 {sizeof(void *), &device_qup_1i10_670_hyp},
	 {sizeof(void *), &device_qup_1i10i_670_hyp},
	 {sizeof(void *), &device_qup_1i11_670_hyp},
	 {sizeof(void *), &device_qup_1i11i_670_hyp},
	 {sizeof(void *), &device_qup_1i12_670_hyp},
	 {sizeof(void *), &device_qup_1i12i_670_hyp},
	 {sizeof(void *), &device_qup_1s0_670_hyp},
	 {sizeof(void *), &device_qup_1s0s_670_hyp},
	 {sizeof(void *), &device_qup_1s0g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s0g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1s1_670_hyp},
	 {sizeof(void *), &device_qup_1s1s_670_hyp},
	 {sizeof(void *), &device_qup_1s1g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s1g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1s2_670_hyp},
	 {sizeof(void *), &device_qup_1s2s_670_hyp},
	 {sizeof(void *), &device_qup_1s2g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s2g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1s3_670_hyp},
	 {sizeof(void *), &device_qup_1s3s_670_hyp},
	 {sizeof(void *), &device_qup_1s3g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s3g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1s4_670_hyp},
	 {sizeof(void *), &device_qup_1s4s_670_hyp},
	 {sizeof(void *), &device_qup_1s4g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s4g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1s5_670_hyp},
	 {sizeof(void *), &device_qup_1s5s_670_hyp},
	 {sizeof(void *), &device_qup_1s5g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s5g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1s6_670_hyp},
	 {sizeof(void *), &device_qup_1s6s_670_hyp},
	 {sizeof(void *), &device_qup_1s6g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s6g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1s7_670_hyp},
	 {sizeof(void *), &device_qup_1s7s_670_hyp},
	 {sizeof(void *), &device_qup_1s7g0v1_670_hyp},
	 {sizeof(void *), &device_qup_1s7g2v1_670_hyp},
	 {sizeof(void *), &device_qup_1c_670_hyp},
	 {sizeof(void *), &device_qup_1cs_670_hyp},
	 {sizeof(void *), &qupv3_se_hw_670_hyp},
	 {sizeof(void *), &qupv3_se_hw_size_670_hyp},
	 {sizeof(void *), &qupv3_common_hw_670_hyp},
	 {sizeof(void *), &qupv3_common_hw_size_670_hyp},
	 {sizeof(void *), &qupv3_0_clocks_arr_670_hyp},
	 {sizeof(void *), &qupv3_0_clocks_arr_size_670_hyp},
	 {sizeof(void *), &qupv3_1_clocks_arr_670_hyp},
	 {sizeof(void *), &qupv3_1_clocks_arr_size_670_hyp},
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_670_hyp[] = {

			0x0000089c, 0x00000020, 0x00000204, 0x00000204, 0x00000204, 
			0x00000001, 0x00000000, 0x00000820, 0x71006964, 0x33767075, 
			0x5f65735f, 0x71007768, 0x33767075, 0x5f65735f, 0x735f7768, 
			0x00657a69, 0x76707571, 0x6f635f33, 0x6e6f6d6d, 0x0077685f, 
			0x76707571, 0x6f635f33, 0x6e6f6d6d, 0x5f77685f, 0x657a6973, 
			0x70757100, 0x305f3376, 0x6f6c635f, 0x5f736b63, 0x00727261, 
			0x76707571, 0x5f305f33, 0x636f6c63, 0x615f736b, 0x735f7272, 
			0x00657a69, 0x76707571, 0x5f315f33, 0x636f6c63, 0x615f736b, 
			0x71007272, 0x33767075, 0x635f315f, 0x6b636f6c, 0x72615f73, 
			0x69735f72, 0x7100657a, 0x33767075, 0x6970675f, 0x73755f6f, 
			0x736d5f65, 0x65765f6d, 0x75710072, 0x5f337670, 0x72736374, 
			0x636f735f, 0x5f77685f, 0x5f726576, 0x72646461, 0x70757100, 
			0x645f3376, 0x6c62756f, 0x69735f65, 0x64656e67, 0x6c614400, 
			0x00766e45, 0x67726154, 0x66437465, 0x6c470067, 0x78744362, 
			0x4d574874, 0x78657475, 0x626d754e, 0x68007265, 0x655f7079, 
			0x6c62616e, 0x76645f65, 0x7968006d, 0x70695f70, 0x61625f61, 
			0x5f646573, 0x65666173, 0x6f63655f, 0x6177735f, 0x70796800, 
			0x6661735f, 0x63655f65, 0x77735f6f, 0x79680061, 0x65735f70, 
			0x65727563, 0x726f6d5f, 0x616e655f, 0x64656c62, 0x70796800, 
			0x6365735f, 0x5f657275, 0x5f726f6d, 0x53505041, 0x6174735f, 
			0x68007472, 0x735f7079, 0x72756365, 0x6f6d5f65, 0x50415f72, 
			0x6c5f5350, 0x68006e65, 0x735f7079, 0x72756365, 0x6f6d5f65, 
			0x49505f72, 0x74735f4c, 0x00747261, 0x5f707968, 0x5f70646d, 
			0x735f6264, 0x74726174, 0x70796800, 0x70646d5f, 0x5f62645f, 
			0x006e656c, 0x5f707968, 0x5f687074, 0x75626564, 0x6f6c5f67, 
			0x6e696461, 0x6e650067, 0x656c6261, 0x00706b5f, 0x00000001, 
			0x00000670, 0x00002070, (301989888 | (SMMU_APPS_TCU & 0x00FFFFFF)), 0x00000000, (301989888 | (SMMU_MMSS_OXILI & 0x00FFFFFF)), 
			0x00000001, 0xff00ff00, 0x12800000, 0x00000002, 0xff00ff00, 
			0x12800000, 0x00000003, 0xff00ff00, 0x12800000, 0x00000004, 
			0xff00ff00, 0x12800000, 0x00000005, 0xff00ff00, 0x12800000, 
			0x00000006, 0xff00ff00, 0x12800000, 0x00000007, 0xff00ff00, 
			0x12800000, 0x00000008, 0xff00ff00, 0x12800000, 0x00000009, 
			0xff00ff00, 0x12800000, 0x0000000a, 0xff00ff00, 0x12800000, 
			0x0000000b, 0xff00ff00, 0x12800000, 0x0000000c, 0xff00ff00, 
			0x12800000, 0x0000000d, 0xff00ff00, 0x12800000, 0x0000000e, 
			0xff00ff00, 0x12800000, 0x0000000f, 0xff00ff00, 0x12800000, 
			0x00000010, 0xff00ff00, 0x12800000, 0x00000011, 0xff00ff00, 
			0x12800000, 0x00000012, 0xff00ff00, 0x12800000, 0x00000013, 
			0xff00ff00, 0x12800000, 0x00000014, 0xff00ff00, 0x12800000, 
			0x00000015, 0xff00ff00, 0x12800000, 0x00000016, 0xff00ff00, 
			0x12800000, 0x00000017, 0xff00ff00, 0x12800000, 0x00000018, 
			0xff00ff00, 0x12800000, 0x00000019, 0xff00ff00, 0x12800000, 
			0x0000001a, 0xff00ff00, 0x12800000, 0x0000001b, 0xff00ff00, 
			0x12800000, 0x0000001c, 0xff00ff00, 0x12800000, 0x0000001d, 
			0xff00ff00, 0x12800000, 0x0000001e, 0xff00ff00, 0x12800000, 
			0x0000001f, 0xff00ff00, 0x12800000, 0x00000020, 0xff00ff00, 
			0x12800000, 0x00000021, 0xff00ff00, 0x12800000, 0x00000022, 
			0xff00ff00, 0x12800000, 0x00000023, 0xff00ff00, 0x12800000, 
			0x00000024, 0xff00ff00, 0x12800000, 0x00000025, 0xff00ff00, 
			0x12800000, 0x00000026, 0xff00ff00, 0x12800000, 0x00000027, 
			0xff00ff00, 0x12800000, 0x00000028, 0xff00ff00, 0x12800000, 
			0x00000029, 0xff00ff00, 0x12800000, 0x0000002a, 0xff00ff00, 
			0x12800000, 0x0000002b, 0xff00ff00, 0x12800000, 0x0000002c, 
			0xff00ff00, 0x12800000, 0x0000002d, 0xff00ff00, 0x12800000, 
			0x0000002e, 0xff00ff00, 0x12800000, 0x0000002f, 0xff00ff00, 
			0x12800000, 0x00000030, 0xff00ff00, 0x12800000, 0x00000031, 
			0xff00ff00, 0x12800000, 0x00000032, 0xff00ff00, 0x12800000, 
			0x00000033, 0xff00ff00, 0x12800000, 0x00000034, 0xff00ff00, 
			0x12800000, 0x00000035, 0xff00ff00, 0x12800000, 0x00000036, 
			0xff00ff00, 0x12800000, 0x00000037, 0xff00ff00, 0x12800000, 
			0x00000038, 0xff00ff00, 0x12800000, 0x00000039, 0xff00ff00, 
			0x12800000, 0x0000003a, 0xff00ff00, 0x12800000, 0x0000003b, 
			0xff00ff00, 0x12800000, 0x0000003c, 0xff00ff00, 0x12800000, 
			0x0000003d, 0xff00ff00, 0x12800000, 0x0000003e, 0xff00ff00, 
			0x12800000, 0x0000003f, 0xff00ff00, 0x12800000, 0x00000040, 
			0xff00ff00, 0x12800000, 0x00000041, 0xff00ff00, 0x12800000, 
			0x00000042, 0xff00ff00, 0x12800000, 0x00000043, 0xff00ff00, 
			0x12800000, 0x00000044, 0xff00ff00, 0x12800000, 0x00000045, 
			0xff00ff00, 0x12800000, 0x00000046, 0xff00ff00, 0x12800000, 
			0x00000047, 0xff00ff00, 0x12800000, 0x00000048, 0xff00ff00, 
			0x12800000, 0x00000049, 0xff00ff00, 0x12800000, 0x0000004a, 
			0xff00ff00, 0x12800000, 0x0000004b, 0xff00ff00, 0x12800000, 
			0x0000004c, 0xff00ff00, 0x12800000, 0x0000004d, 0xff00ff00, 
			0x12800000, 0x0000004e, 0xff00ff00, 0x12800000, 0x0000004f, 
			0xff00ff00, 0x12800000, 0x00000050, 0xff00ff00, 0x12800000, 
			0x00000051, 0xff00ff00, 0x12800000, 0x00000052, 0xff00ff00, 
			0x12800000, 0x00000053, 0xff00ff00, 0x12800000, 0x00000054, 
			0xff00ff00, 0x12800000, 0x00000055, 0xff00ff00, 0x12800000, 
			0x00000056, 0xff00ff00, 0x12800000, 0x00000057, 0xff00ff00, 
			0x12800000, 0x00000058, 0xff00ff00, 0x12800000, 0x00000059, 
			0xff00ff00, 0x12800000, 0x0000005a, 0xff00ff00, 0x12800000, 
			0x0000005b, 0xff00ff00, 0x12800000, 0x0000005c, 0xff00ff00, 
			0x12800000, 0x0000005d, 0xff00ff00, 0x12800000, 0x0000005e, 
			0xff00ff00, 0x12800000, 0x0000005f, 0xff00ff00, 0x12800000, 
			0x00000060, 0xff00ff00, 0x12800000, 0x00000061, 0xff00ff00, 
			0x12800000, 0x00000062, 0xff00ff00, 0x12800000, 0x00000063, 
			0xff00ff00, 0x12800000, 0x00000064, 0xff00ff00, 0x12800000, 
			0x00000065, 0xff00ff00, 0x12800000, 0x00000066, 0xff00ff00, 
			0x12800000, 0x00000067, 0xff00ff00, 0x12800000, 0x00000068, 
			0xff00ff00, 0x12800000, 0x00000069, 0xff00ff00, 0x12800000, 
			0x0000006a, 0xff00ff00, 0x12800000, 0x0000006b, 0xff00ff00, 
			0x12800000, 0x0000006c, 0xff00ff00, 0x12800000, 0x0000006d, 
			0xff00ff00, 0x12800000, 0x0000006e, 0xff00ff00, 0x12800000, 
			0x0000006f, 0xff00ff00, 0x12800000, 0x00000070, 0xff00ff00, 
			0x12800000, 0x00000071, 0xff00ff00, 0x12800000, 0x00000072, 
			0xff00ff00, 0x12800000, 0x00000073, 0xff00ff00, 0x12800000, 
			0x00000074, 0xff00ff00, 0x12800000, 0x00000075, 0xff00ff00, 
			0x12800000, 0x00000076, 0xff00ff00, 0x12800000, 0x00000077, 
			0xff00ff00, 0x12800000, 0x00000078, 0xff00ff00, 0x12800000, 
			0x00000079, 0xff00ff00, 0x12800003, 0x0000007a, 0x1280000f, 
			0x0000007b, 0x12800020, 0x0000007c, 0x12800030, 0x0000007d, 
			0x12800045, 0x0000007e, 0x12800058, 0x0000007f, 0x12800070, 
			0x00000080, 0x12800083, 0x00000081, 0x0280009b, 0x00000001, 
			0x028000b2, 0x01fc8000, 0x028000cd, 0x00000001, 0xff00ff00, 
			0x028000e1, 0x10000002, 0x148000e8, 0x00000000, 0x028000f2, 
			0x00000009, 0xff00ff00, 0x02800107, 0x00000000, 0x02800116, 
			0x00000000, 0x02800131, 0x00000000, 0x02800142, 0x00000000, 
			0x02800159, 0x85f00000, 0x02800173, 0x000c0000, 0x0280018b, 
			0x8ab00000, 0x028001a4, 0x80600000, 0x028001b5, 0x00800000, 
			0x028001c4, 0x00000000, 0xff00ff00, 0x028001da, 0x00000001, 
			0xff00ff00 };



const StringDevice driver_list_670_hyp[] = {
			{"/dev/smmu_V1",2982605418u, 528, NULL, 0, NULL },
			{"QUP_0I0",722463235u, 548, NULL, 0, NULL },
			{"QUP_0I0I",2366450348u, 560, NULL, 0, NULL },
			{"QUP_0I1",722463236u, 572, NULL, 0, NULL },
			{"QUP_0I1I",2366450381u, 584, NULL, 0, NULL },
			{"QUP_0I2",722463237u, 596, NULL, 0, NULL },
			{"QUP_0I2I",2366450414u, 608, NULL, 0, NULL },
			{"QUP_0I3",722463238u, 620, NULL, 0, NULL },
			{"QUP_0I3I",2366450447u, 632, NULL, 0, NULL },
			{"QUP_0I4",722463239u, 644, NULL, 0, NULL },
			{"QUP_0I4I",2366450480u, 656, NULL, 0, NULL },
			{"QUP_0I5",722463240u, 668, NULL, 0, NULL },
			{"QUP_0I5I",2366450513u, 680, NULL, 0, NULL },
			{"QUP_0I6",722463241u, 692, NULL, 0, NULL },
			{"QUP_0I6I",2366450546u, 704, NULL, 0, NULL },
			{"QUP_0I7",722463242u, 716, NULL, 0, NULL },
			{"QUP_0I7I",2366450579u, 728, NULL, 0, NULL },
			{"QUP_0I8",722463243u, 740, NULL, 0, NULL },
			{"QUP_0I8I",2366450612u, 752, NULL, 0, NULL },
			{"QUP_0I9",722463244u, 764, NULL, 0, NULL },
			{"QUP_0I9I",2366450645u, 776, NULL, 0, NULL },
			{"QUP_0I10",2366450356u, 788, NULL, 0, NULL },
			{"QUP_0I10I",783450493u, 800, NULL, 0, NULL },
			{"QUP_0I11",2366450357u, 812, NULL, 0, NULL },
			{"QUP_0I11I",783450526u, 824, NULL, 0, NULL },
			{"QUP_0I12",2366450358u, 836, NULL, 0, NULL },
			{"QUP_0I12I",783450559u, 848, NULL, 0, NULL },
			{"QUP_0S0",722463565u, 860, NULL, 0, NULL },
			{"QUP_0S0S",2366461248u, 872, NULL, 0, NULL },
			{"QUP_0S0G0v1",3165033547u, 884, NULL, 0, NULL },
			{"QUP_0S0G2v1",3165035725u, 896, NULL, 0, NULL },
			{"QUP_0S1",722463566u, 908, NULL, 0, NULL },
			{"QUP_0S1S",2366461281u, 920, NULL, 0, NULL },
			{"QUP_0S1G0v1",3166219468u, 932, NULL, 0, NULL },
			{"QUP_0S1G2v1",3166221646u, 944, NULL, 0, NULL },
			{"QUP_0S2",722463567u, 956, NULL, 0, NULL },
			{"QUP_0S2S",2366461314u, 968, NULL, 0, NULL },
			{"QUP_0S2G0v1",3167405389u, 980, NULL, 0, NULL },
			{"QUP_0S2G2v1",3167407567u, 992, NULL, 0, NULL },
			{"QUP_0S3",722463568u, 1004, NULL, 0, NULL },
			{"QUP_0S3S",2366461347u, 1016, NULL, 0, NULL },
			{"QUP_0S3G0v1",3168591310u, 1028, NULL, 0, NULL },
			{"QUP_0S3G2v1",3168593488u, 1040, NULL, 0, NULL },
			{"QUP_0S4",722463569u, 1052, NULL, 0, NULL },
			{"QUP_0S4S",2366461380u, 1064, NULL, 0, NULL },
			{"QUP_0S4G0v1",3169777231u, 1076, NULL, 0, NULL },
			{"QUP_0S4G2v1",3169779409u, 1088, NULL, 0, NULL },
			{"QUP_0S5",722463570u, 1100, NULL, 0, NULL },
			{"QUP_0S5S",2366461413u, 1112, NULL, 0, NULL },
			{"QUP_0S5G0v1",3170963152u, 1124, NULL, 0, NULL },
			{"QUP_0S5G2v1",3170965330u, 1136, NULL, 0, NULL },
			{"QUP_0S6",722463571u, 1148, NULL, 0, NULL },
			{"QUP_0S6S",2366461446u, 1160, NULL, 0, NULL },
			{"QUP_0S6G0v1",3172149073u, 1172, NULL, 0, NULL },
			{"QUP_0S6G2v1",3172151251u, 1184, NULL, 0, NULL },
			{"QUP_0S7",722463572u, 1196, NULL, 0, NULL },
			{"QUP_0S7S",2366461479u, 1208, NULL, 0, NULL },
			{"QUP_0S7G0v1",3173334994u, 1220, NULL, 0, NULL },
			{"QUP_0S7G2v1",3173337172u, 1232, NULL, 0, NULL },
			{"QUP_0C",3405806445u, 1244, NULL, 0, NULL },
			{"QUP_0CS",722463072u, 1256, NULL, 0, NULL },
			{"QUP_1I0",722464324u, 1268, NULL, 0, NULL },
			{"QUP_1I0I",2366486285u, 1280, NULL, 0, NULL },
			{"QUP_1I1",722464325u, 1292, NULL, 0, NULL },
			{"QUP_1I1I",2366486318u, 1304, NULL, 0, NULL },
			{"QUP_1I2",722464326u, 1316, NULL, 0, NULL },
			{"QUP_1I2I",2366486351u, 1328, NULL, 0, NULL },
			{"QUP_1I3",722464327u, 1340, NULL, 0, NULL },
			{"QUP_1I3I",2366486384u, 1352, NULL, 0, NULL },
			{"QUP_1I4",722464328u, 1364, NULL, 0, NULL },
			{"QUP_1I4I",2366486417u, 1376, NULL, 0, NULL },
			{"QUP_1I5",722464329u, 1388, NULL, 0, NULL },
			{"QUP_1I5I",2366486450u, 1400, NULL, 0, NULL },
			{"QUP_1I6",722464330u, 1412, NULL, 0, NULL },
			{"QUP_1I6I",2366486483u, 1424, NULL, 0, NULL },
			{"QUP_1I7",722464331u, 1436, NULL, 0, NULL },
			{"QUP_1I7I",2366486516u, 1448, NULL, 0, NULL },
			{"QUP_1I8",722464332u, 1460, NULL, 0, NULL },
			{"QUP_1I8I",2366486549u, 1472, NULL, 0, NULL },
			{"QUP_1I9",722464333u, 1484, NULL, 0, NULL },
			{"QUP_1I9I",2366486582u, 1496, NULL, 0, NULL },
			{"QUP_1I10",2366486293u, 1508, NULL, 0, NULL },
			{"QUP_1I10I",784636414u, 1520, NULL, 0, NULL },
			{"QUP_1I11",2366486294u, 1532, NULL, 0, NULL },
			{"QUP_1I11I",784636447u, 1544, NULL, 0, NULL },
			{"QUP_1I12",2366486295u, 1556, NULL, 0, NULL },
			{"QUP_1I12I",784636480u, 1568, NULL, 0, NULL },
			{"QUP_1S0",722464654u, 1580, NULL, 0, NULL },
			{"QUP_1S0S",2366497185u, 1592, NULL, 0, NULL },
			{"QUP_1S0G0v1",161534220u, 1604, NULL, 0, NULL },
			{"QUP_1S0G2v1",161536398u, 1616, NULL, 0, NULL },
			{"QUP_1S1",722464655u, 1628, NULL, 0, NULL },
			{"QUP_1S1S",2366497218u, 1640, NULL, 0, NULL },
			{"QUP_1S1G0v1",162720141u, 1652, NULL, 0, NULL },
			{"QUP_1S1G2v1",162722319u, 1664, NULL, 0, NULL },
			{"QUP_1S2",722464656u, 1676, NULL, 0, NULL },
			{"QUP_1S2S",2366497251u, 1688, NULL, 0, NULL },
			{"QUP_1S2G0v1",163906062u, 1700, NULL, 0, NULL },
			{"QUP_1S2G2v1",163908240u, 1712, NULL, 0, NULL },
			{"QUP_1S3",722464657u, 1724, NULL, 0, NULL },
			{"QUP_1S3S",2366497284u, 1736, NULL, 0, NULL },
			{"QUP_1S3G0v1",165091983u, 1748, NULL, 0, NULL },
			{"QUP_1S3G2v1",165094161u, 1760, NULL, 0, NULL },
			{"QUP_1S4",722464658u, 1772, NULL, 0, NULL },
			{"QUP_1S4S",2366497317u, 1784, NULL, 0, NULL },
			{"QUP_1S4G0v1",166277904u, 1796, NULL, 0, NULL },
			{"QUP_1S4G2v1",166280082u, 1808, NULL, 0, NULL },
			{"QUP_1S5",722464659u, 1820, NULL, 0, NULL },
			{"QUP_1S5S",2366497350u, 1832, NULL, 0, NULL },
			{"QUP_1S5G0v1",167463825u, 1844, NULL, 0, NULL },
			{"QUP_1S5G2v1",167466003u, 1856, NULL, 0, NULL },
			{"QUP_1S6",722464660u, 1868, NULL, 0, NULL },
			{"QUP_1S6S",2366497383u, 1880, NULL, 0, NULL },
			{"QUP_1S6G0v1",168649746u, 1892, NULL, 0, NULL },
			{"QUP_1S6G2v1",168651924u, 1904, NULL, 0, NULL },
			{"QUP_1S7",722464661u, 1916, NULL, 0, NULL },
			{"QUP_1S7S",2366497416u, 1928, NULL, 0, NULL },
			{"QUP_1S7G0v1",169835667u, 1940, NULL, 0, NULL },
			{"QUP_1S7G2v1",169837845u, 1952, NULL, 0, NULL },
			{"QUP_1C",3405806478u, 1964, NULL, 0, NULL },
			{"QUP_1CS",722464161u, 1976, NULL, 0, NULL },
			{"QUPV3_HW_PROP",751796930u, 1988, NULL, 0, NULL },
			{"/dev/hyp",3784637939u, 2108, NULL, 0, NULL },
			{"/kp/general",1520996892u, 2192, NULL, 0, NULL }
};
