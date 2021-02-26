#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALPropDef.h"
#include "HALvmidmt.h"
#include "HALvmidmtTarget.h"
#include "HALcomdef.h"
#include "com_dtypes.h"
#include "HALvmidmtInfoImpl.h"
#include "HALhwio.h"
#include "HALvmidmtHWIOTarget.h"
#include "msmhwiobase.h"
#include "ddr_defs.h"
#include "tzbsp_target.h"
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
#include "HALqgic.h"
#include "pimem_defs.h"
#include "timer_defs.h"
#include "dbg_cfg.h"
#include "tzbsp_secboot.h"
#include "secboot_img_util.h"
#include "secboot.h"
#include "secboot_env.h"
#include "secboot_chipset.h"
#include "CeML.h"
#include "secboot_swid.h"
#include "sec_img_hashseg.h"
#include "sec_img_auth.h"
#include "seccommon.h"
#include "sec_img_env.h"
#include "uie.h"
#include "comdef.h"
#include "target.h"
#include "armasm.h"
#include "secboot_pbl_shared.h"
#include "SecHWIO.h"
#include "tzbsp_hwio.h"
#include "SecHWIOTestFuses.h"
#include "secboot_rollback_version.h"
#ifndef DEVCFG_STANDALONE 
#include "DALStdDef.h" 
#include "DALSysTypes.h" 

#else 
#include "stubs.h" 

#include "devcfgTypes.h" 

#endif 
extern void * bam_tgt_config_670_tz;
extern void * spmiInfo_670_tz;
extern void * device_qup_0i0_670_tz;
extern void * device_qup_0i0i_670_tz;
extern void * device_qup_0i1_670_tz;
extern void * device_qup_0i1i_670_tz;
extern void * device_qup_0i2_670_tz;
extern void * device_qup_0i2i_670_tz;
extern void * device_qup_0i3_670_tz;
extern void * device_qup_0i3i_670_tz;
extern void * device_qup_0i4_670_tz;
extern void * device_qup_0i4i_670_tz;
extern void * device_qup_0i5_670_tz;
extern void * device_qup_0i5i_670_tz;
extern void * device_qup_0i6_670_tz;
extern void * device_qup_0i6i_670_tz;
extern void * device_qup_0i7_670_tz;
extern void * device_qup_0i7i_670_tz;
extern void * device_qup_0i8_670_tz;
extern void * device_qup_0i8i_670_tz;
extern void * device_qup_0i9_670_tz;
extern void * device_qup_0i9i_670_tz;
extern void * device_qup_0i10_670_tz;
extern void * device_qup_0i10i_670_tz;
extern void * device_qup_0i11_670_tz;
extern void * device_qup_0i11i_670_tz;
extern void * device_qup_0i12_670_tz;
extern void * device_qup_0i12i_670_tz;
extern void * device_qup_0s0_670_tz;
extern void * device_qup_0s0s_670_tz;
extern void * device_qup_0s0g0v1_670_tz;
extern void * device_qup_0s0g2v1_670_tz;
extern void * device_qup_0s1_670_tz;
extern void * device_qup_0s1s_670_tz;
extern void * device_qup_0s1g0v1_670_tz;
extern void * device_qup_0s1g2v1_670_tz;
extern void * device_qup_0s2_670_tz;
extern void * device_qup_0s2s_670_tz;
extern void * device_qup_0s2g0v1_670_tz;
extern void * device_qup_0s2g2v1_670_tz;
extern void * device_qup_0s3_670_tz;
extern void * device_qup_0s3s_670_tz;
extern void * device_qup_0s3g0v1_670_tz;
extern void * device_qup_0s3g2v1_670_tz;
extern void * device_qup_0s4_670_tz;
extern void * device_qup_0s4s_670_tz;
extern void * device_qup_0s4g0v1_670_tz;
extern void * device_qup_0s4g2v1_670_tz;
extern void * device_qup_0s5_670_tz;
extern void * device_qup_0s5s_670_tz;
extern void * device_qup_0s5g0v1_670_tz;
extern void * device_qup_0s5g2v1_670_tz;
extern void * device_qup_0s6_670_tz;
extern void * device_qup_0s6s_670_tz;
extern void * device_qup_0s6g0v1_670_tz;
extern void * device_qup_0s6g2v1_670_tz;
extern void * device_qup_0s7_670_tz;
extern void * device_qup_0s7s_670_tz;
extern void * device_qup_0s7g0v1_670_tz;
extern void * device_qup_0s7g2v1_670_tz;
extern void * device_qup_0c_670_tz;
extern void * device_qup_0cs_670_tz;
extern void * device_qup_1i0_670_tz;
extern void * device_qup_1i0i_670_tz;
extern void * device_qup_1i1_670_tz;
extern void * device_qup_1i1i_670_tz;
extern void * device_qup_1i2_670_tz;
extern void * device_qup_1i2i_670_tz;
extern void * device_qup_1i3_670_tz;
extern void * device_qup_1i3i_670_tz;
extern void * device_qup_1i4_670_tz;
extern void * device_qup_1i4i_670_tz;
extern void * device_qup_1i5_670_tz;
extern void * device_qup_1i5i_670_tz;
extern void * device_qup_1i6_670_tz;
extern void * device_qup_1i6i_670_tz;
extern void * device_qup_1i7_670_tz;
extern void * device_qup_1i7i_670_tz;
extern void * device_qup_1i8_670_tz;
extern void * device_qup_1i8i_670_tz;
extern void * device_qup_1i9_670_tz;
extern void * device_qup_1i9i_670_tz;
extern void * device_qup_1i10_670_tz;
extern void * device_qup_1i10i_670_tz;
extern void * device_qup_1i11_670_tz;
extern void * device_qup_1i11i_670_tz;
extern void * device_qup_1i12_670_tz;
extern void * device_qup_1i12i_670_tz;
extern void * device_qup_1s0_670_tz;
extern void * device_qup_1s0s_670_tz;
extern void * device_qup_1s0g0v1_670_tz;
extern void * device_qup_1s0g2v1_670_tz;
extern void * device_qup_1s1_670_tz;
extern void * device_qup_1s1s_670_tz;
extern void * device_qup_1s1g0v1_670_tz;
extern void * device_qup_1s1g2v1_670_tz;
extern void * device_qup_1s2_670_tz;
extern void * device_qup_1s2s_670_tz;
extern void * device_qup_1s2g0v1_670_tz;
extern void * device_qup_1s2g2v1_670_tz;
extern void * device_qup_1s3_670_tz;
extern void * device_qup_1s3s_670_tz;
extern void * device_qup_1s3g0v1_670_tz;
extern void * device_qup_1s3g2v1_670_tz;
extern void * device_qup_1s4_670_tz;
extern void * device_qup_1s4s_670_tz;
extern void * device_qup_1s4g0v1_670_tz;
extern void * device_qup_1s4g2v1_670_tz;
extern void * device_qup_1s5_670_tz;
extern void * device_qup_1s5s_670_tz;
extern void * device_qup_1s5g0v1_670_tz;
extern void * device_qup_1s5g2v1_670_tz;
extern void * device_qup_1s6_670_tz;
extern void * device_qup_1s6s_670_tz;
extern void * device_qup_1s6g0v1_670_tz;
extern void * device_qup_1s6g2v1_670_tz;
extern void * device_qup_1s7_670_tz;
extern void * device_qup_1s7s_670_tz;
extern void * device_qup_1s7g0v1_670_tz;
extern void * device_qup_1s7g2v1_670_tz;
extern void * device_qup_1c_670_tz;
extern void * device_qup_1cs_670_tz;
extern void * qupv3_se_hw_670_tz;
extern void * qupv3_se_hw_size_670_tz;
extern void * qupv3_common_hw_670_tz;
extern void * qupv3_common_hw_size_670_tz;
extern void * qupv3_0_clocks_arr_670_tz;
extern void * qupv3_0_clocks_arr_size_670_tz;
extern void * qupv3_1_clocks_arr_670_tz;
extern void * qupv3_1_clocks_arr_size_670_tz;
extern void * NOCERR_propdata_670_tz;
extern void * icbcfg_info_670_tz;
extern void * info_670_tz;


static HAL_vmidmt_InfoType	devcfg_133={((0x01dc0000+0x00000000)+0x00000000),{17,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_134={((0x00800000+0x000c6000)+0x00000000),{48,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_135={((0x00a00000+0x000c6000)+0x00000000),{48,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_136={((0x06000000+0x00080000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_137={((0x06000000+0x00049000)+0x00000000),{2,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_138={((0x0b000000+0x000c0000)+0x00000000),{1,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_139={((0x62000000+0x006c6000)+0x00000000),{48,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_140={((0x01e00000+0x00030000)+0x00000000),{32,0,0,0,0,0}};


static HAL_vmidmt_InfoType	devcfg_141={((0x62000000+0x008c0000)+0x00000000),{1,0,0,0,0,0}};


static ddr_config_data_t	devcfg_142={4,(0x80000000+0x06B00000),0x0200000,};


static HAL_qgic_BaseAddressType	devcfg_143={0x17a00000,0x0,0x0,0x0,0x17a60000,};


static pimem_reg_data_t	devcfg_144={0x00610000,0x1C000000,251,250,2097152,2097152,};


static timer_reg_data_t	devcfg_145={2,7,17,41,0x40000,{0x17C90000,0x17C00000},};


static dbg_cfg_data_t	devcfg_146={0xC230000,0x1798000c,0x17990008,0xc2f0000,516,33,517,424,};


static secboot_ver_fuse_t	devcfg_147[]={{.img_id=0x7,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004174),.mask=0x1ffff,.shift=0x0,}},{.img_id=0x15,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000417c),.mask=0xfff,.shift=0x0,}},{.img_id=0x5,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000417c),.mask=0xffe0000,.shift=0x11,}},{.img_id=0x200,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000417c),.mask=0x1f000,.shift=0xc,}},{.img_id=0xA,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004174),.mask=0x1fe0000,.shift=0x11,}},{.img_id=0x21,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004174),.mask=0x1fe0000,.shift=0x11,}},{.img_id=0x0,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004168),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x0,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000416c),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x12,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004168),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x12,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000416c),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x16,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004168),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x16,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000416c),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x9,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004168),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x9,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000416c),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x1C,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004168),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x1C,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000416c),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x11,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004168),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x11,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000416c),.mask=0xffffffff,.shift=0x0,}},{.img_id=0x25,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004178),.mask=0xc0000000,.shift=0x1e,}},{.img_id=0x25,.fuse_info={.addr=((0x00780000+0x00000000)+0x0000417c),.mask=0xf0000000,.shift=0x1c,}},{.img_id=0x2,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004180),.mask=0xffff0000,.shift=0x10,}},{.img_id=0x22,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004180),.mask=0xffff,.shift=0x0,}},{.img_id=0x23,.fuse_info={.addr=((0x00780000+0x00000000)+0x00004180),.mask=0xffff,.shift=0x0,}},};

const DALSYSPropStructTblType DALPROP_StructPtrs_670_tz[149] =  {
	 {sizeof(void *), &bam_tgt_config_670_tz},
	 {sizeof(void *), &spmiInfo_670_tz},
	 {sizeof(void *), &device_qup_0i0_670_tz},
	 {sizeof(void *), &device_qup_0i0i_670_tz},
	 {sizeof(void *), &device_qup_0i1_670_tz},
	 {sizeof(void *), &device_qup_0i1i_670_tz},
	 {sizeof(void *), &device_qup_0i2_670_tz},
	 {sizeof(void *), &device_qup_0i2i_670_tz},
	 {sizeof(void *), &device_qup_0i3_670_tz},
	 {sizeof(void *), &device_qup_0i3i_670_tz},
	 {sizeof(void *), &device_qup_0i4_670_tz},
	 {sizeof(void *), &device_qup_0i4i_670_tz},
	 {sizeof(void *), &device_qup_0i5_670_tz},
	 {sizeof(void *), &device_qup_0i5i_670_tz},
	 {sizeof(void *), &device_qup_0i6_670_tz},
	 {sizeof(void *), &device_qup_0i6i_670_tz},
	 {sizeof(void *), &device_qup_0i7_670_tz},
	 {sizeof(void *), &device_qup_0i7i_670_tz},
	 {sizeof(void *), &device_qup_0i8_670_tz},
	 {sizeof(void *), &device_qup_0i8i_670_tz},
	 {sizeof(void *), &device_qup_0i9_670_tz},
	 {sizeof(void *), &device_qup_0i9i_670_tz},
	 {sizeof(void *), &device_qup_0i10_670_tz},
	 {sizeof(void *), &device_qup_0i10i_670_tz},
	 {sizeof(void *), &device_qup_0i11_670_tz},
	 {sizeof(void *), &device_qup_0i11i_670_tz},
	 {sizeof(void *), &device_qup_0i12_670_tz},
	 {sizeof(void *), &device_qup_0i12i_670_tz},
	 {sizeof(void *), &device_qup_0s0_670_tz},
	 {sizeof(void *), &device_qup_0s0s_670_tz},
	 {sizeof(void *), &device_qup_0s0g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s0g2v1_670_tz},
	 {sizeof(void *), &device_qup_0s1_670_tz},
	 {sizeof(void *), &device_qup_0s1s_670_tz},
	 {sizeof(void *), &device_qup_0s1g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s1g2v1_670_tz},
	 {sizeof(void *), &device_qup_0s2_670_tz},
	 {sizeof(void *), &device_qup_0s2s_670_tz},
	 {sizeof(void *), &device_qup_0s2g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s2g2v1_670_tz},
	 {sizeof(void *), &device_qup_0s3_670_tz},
	 {sizeof(void *), &device_qup_0s3s_670_tz},
	 {sizeof(void *), &device_qup_0s3g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s3g2v1_670_tz},
	 {sizeof(void *), &device_qup_0s4_670_tz},
	 {sizeof(void *), &device_qup_0s4s_670_tz},
	 {sizeof(void *), &device_qup_0s4g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s4g2v1_670_tz},
	 {sizeof(void *), &device_qup_0s5_670_tz},
	 {sizeof(void *), &device_qup_0s5s_670_tz},
	 {sizeof(void *), &device_qup_0s5g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s5g2v1_670_tz},
	 {sizeof(void *), &device_qup_0s6_670_tz},
	 {sizeof(void *), &device_qup_0s6s_670_tz},
	 {sizeof(void *), &device_qup_0s6g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s6g2v1_670_tz},
	 {sizeof(void *), &device_qup_0s7_670_tz},
	 {sizeof(void *), &device_qup_0s7s_670_tz},
	 {sizeof(void *), &device_qup_0s7g0v1_670_tz},
	 {sizeof(void *), &device_qup_0s7g2v1_670_tz},
	 {sizeof(void *), &device_qup_0c_670_tz},
	 {sizeof(void *), &device_qup_0cs_670_tz},
	 {sizeof(void *), &device_qup_1i0_670_tz},
	 {sizeof(void *), &device_qup_1i0i_670_tz},
	 {sizeof(void *), &device_qup_1i1_670_tz},
	 {sizeof(void *), &device_qup_1i1i_670_tz},
	 {sizeof(void *), &device_qup_1i2_670_tz},
	 {sizeof(void *), &device_qup_1i2i_670_tz},
	 {sizeof(void *), &device_qup_1i3_670_tz},
	 {sizeof(void *), &device_qup_1i3i_670_tz},
	 {sizeof(void *), &device_qup_1i4_670_tz},
	 {sizeof(void *), &device_qup_1i4i_670_tz},
	 {sizeof(void *), &device_qup_1i5_670_tz},
	 {sizeof(void *), &device_qup_1i5i_670_tz},
	 {sizeof(void *), &device_qup_1i6_670_tz},
	 {sizeof(void *), &device_qup_1i6i_670_tz},
	 {sizeof(void *), &device_qup_1i7_670_tz},
	 {sizeof(void *), &device_qup_1i7i_670_tz},
	 {sizeof(void *), &device_qup_1i8_670_tz},
	 {sizeof(void *), &device_qup_1i8i_670_tz},
	 {sizeof(void *), &device_qup_1i9_670_tz},
	 {sizeof(void *), &device_qup_1i9i_670_tz},
	 {sizeof(void *), &device_qup_1i10_670_tz},
	 {sizeof(void *), &device_qup_1i10i_670_tz},
	 {sizeof(void *), &device_qup_1i11_670_tz},
	 {sizeof(void *), &device_qup_1i11i_670_tz},
	 {sizeof(void *), &device_qup_1i12_670_tz},
	 {sizeof(void *), &device_qup_1i12i_670_tz},
	 {sizeof(void *), &device_qup_1s0_670_tz},
	 {sizeof(void *), &device_qup_1s0s_670_tz},
	 {sizeof(void *), &device_qup_1s0g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s0g2v1_670_tz},
	 {sizeof(void *), &device_qup_1s1_670_tz},
	 {sizeof(void *), &device_qup_1s1s_670_tz},
	 {sizeof(void *), &device_qup_1s1g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s1g2v1_670_tz},
	 {sizeof(void *), &device_qup_1s2_670_tz},
	 {sizeof(void *), &device_qup_1s2s_670_tz},
	 {sizeof(void *), &device_qup_1s2g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s2g2v1_670_tz},
	 {sizeof(void *), &device_qup_1s3_670_tz},
	 {sizeof(void *), &device_qup_1s3s_670_tz},
	 {sizeof(void *), &device_qup_1s3g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s3g2v1_670_tz},
	 {sizeof(void *), &device_qup_1s4_670_tz},
	 {sizeof(void *), &device_qup_1s4s_670_tz},
	 {sizeof(void *), &device_qup_1s4g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s4g2v1_670_tz},
	 {sizeof(void *), &device_qup_1s5_670_tz},
	 {sizeof(void *), &device_qup_1s5s_670_tz},
	 {sizeof(void *), &device_qup_1s5g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s5g2v1_670_tz},
	 {sizeof(void *), &device_qup_1s6_670_tz},
	 {sizeof(void *), &device_qup_1s6s_670_tz},
	 {sizeof(void *), &device_qup_1s6g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s6g2v1_670_tz},
	 {sizeof(void *), &device_qup_1s7_670_tz},
	 {sizeof(void *), &device_qup_1s7s_670_tz},
	 {sizeof(void *), &device_qup_1s7g0v1_670_tz},
	 {sizeof(void *), &device_qup_1s7g2v1_670_tz},
	 {sizeof(void *), &device_qup_1c_670_tz},
	 {sizeof(void *), &device_qup_1cs_670_tz},
	 {sizeof(void *), &qupv3_se_hw_670_tz},
	 {sizeof(void *), &qupv3_se_hw_size_670_tz},
	 {sizeof(void *), &qupv3_common_hw_670_tz},
	 {sizeof(void *), &qupv3_common_hw_size_670_tz},
	 {sizeof(void *), &qupv3_0_clocks_arr_670_tz},
	 {sizeof(void *), &qupv3_0_clocks_arr_size_670_tz},
	 {sizeof(void *), &qupv3_1_clocks_arr_670_tz},
	 {sizeof(void *), &qupv3_1_clocks_arr_size_670_tz},
	 {sizeof(void *), &NOCERR_propdata_670_tz},
	 {sizeof(void *), &icbcfg_info_670_tz},
	 {sizeof(void *), &info_670_tz},
	 {sizeof(devcfg_133), &devcfg_133},
	 {sizeof(devcfg_134), &devcfg_134},
	 {sizeof(devcfg_135), &devcfg_135},
	 {sizeof(devcfg_136), &devcfg_136},
	 {sizeof(devcfg_137), &devcfg_137},
	 {sizeof(devcfg_138), &devcfg_138},
	 {sizeof(devcfg_139), &devcfg_139},
	 {sizeof(devcfg_140), &devcfg_140},
	 {sizeof(devcfg_141), &devcfg_141},
	 {sizeof(devcfg_142), &devcfg_142},
	 {sizeof(devcfg_143), &devcfg_143},
	 {sizeof(devcfg_144), &devcfg_144},
	 {sizeof(devcfg_145), &devcfg_145},
	 {sizeof(devcfg_146), &devcfg_146},
	 {sizeof(devcfg_147), &devcfg_147},
	{0, 0 } 
 };
const uint32 DALPROP_PropBin_670_tz[] = {

			0x00000958, 0x00000020, 0x000001e0, 0x000001f0, 0x000001f0, 
			0x00000001, 0x00000000, 0x00000848, 0x5f6d6162, 0x5f746774, 
			0x666e6f63, 0x6f006769, 0x72656e77, 0x6d707300, 0x65645f69, 
			0x65636976, 0x69640073, 0x70757100, 0x735f3376, 0x77685f65, 
			0x70757100, 0x735f3376, 0x77685f65, 0x7a69735f, 0x75710065, 
			0x5f337670, 0x6d6d6f63, 0x685f6e6f, 0x75710077, 0x5f337670, 
			0x6d6d6f63, 0x685f6e6f, 0x69735f77, 0x7100657a, 0x33767075, 
			0x635f305f, 0x6b636f6c, 0x72615f73, 0x75710072, 0x5f337670, 
			0x6c635f30, 0x736b636f, 0x7272615f, 0x7a69735f, 0x75710065, 
			0x5f337670, 0x6c635f31, 0x736b636f, 0x7272615f, 0x70757100, 
			0x315f3376, 0x6f6c635f, 0x5f736b63, 0x5f727261, 0x657a6973, 
			0x70757100, 0x675f3376, 0x5f6f6970, 0x5f657375, 0x5f6d736d, 
			0x00726576, 0x76707571, 0x63745f33, 0x735f7273, 0x685f636f, 
			0x65765f77, 0x64615f72, 0x71007264, 0x33767075, 0x756f645f, 
			0x5f656c62, 0x6e676973, 0x44006465, 0x6e456c61, 0x61540076, 
			0x74656772, 0x00676643, 0x43626c47, 0x48747874, 0x74754d57, 
			0x754e7865, 0x7265626d, 0x73637400, 0x61625f72, 0x6e5f6573, 
			0x00656d61, 0x72736374, 0x7361625f, 0x756d0065, 0x5f786574, 
			0x7366666f, 0x00737465, 0x636e6f77, 0x666f5f65, 0x74657366, 
			0x4f4e0073, 0x72724543, 0x505f726f, 0x64706f72, 0x00617461, 
			0x63626369, 0x695f6766, 0x006f666e, 0x6f666e69, 0x6c6c6100, 
			0x655f776f, 0x65736172, 0x5f6e695f, 0x75636573, 0x6d5f6572, 
			0x0065646f, 0x67616d69, 0x65765f65, 0x6f697372, 0x7962006e, 
			0x73736170, 0x6f6f625f, 0x65725f74, 0x69727473, 0x00007463, 
			0x45524f43, 0x504f545f, 0x5253435f, 0x00000000, 0x00000001, 
			0x00000670, 0x00002070, 0x00000007, 0x00040000, 0x00041000, 
			0x00042000, 0x00043000, 0x00044000, 0x00045000, 0x00046000, 
			0x00047000, 0x00000001, 0x000d4000, 0x000d4004, 0x12800000, 
			0x00000000, 0xff00ff00, 0x0280000f, 0x00000001, 0x12800015, 
			0x00000001, 0xff00ff00, 0x12800022, 0x00000002, 0xff00ff00, 
			0x12800022, 0x00000003, 0xff00ff00, 0x12800022, 0x00000004, 
			0xff00ff00, 0x12800022, 0x00000005, 0xff00ff00, 0x12800022, 
			0x00000006, 0xff00ff00, 0x12800022, 0x00000007, 0xff00ff00, 
			0x12800022, 0x00000008, 0xff00ff00, 0x12800022, 0x00000009, 
			0xff00ff00, 0x12800022, 0x0000000a, 0xff00ff00, 0x12800022, 
			0x0000000b, 0xff00ff00, 0x12800022, 0x0000000c, 0xff00ff00, 
			0x12800022, 0x0000000d, 0xff00ff00, 0x12800022, 0x0000000e, 
			0xff00ff00, 0x12800022, 0x0000000f, 0xff00ff00, 0x12800022, 
			0x00000010, 0xff00ff00, 0x12800022, 0x00000011, 0xff00ff00, 
			0x12800022, 0x00000012, 0xff00ff00, 0x12800022, 0x00000013, 
			0xff00ff00, 0x12800022, 0x00000014, 0xff00ff00, 0x12800022, 
			0x00000015, 0xff00ff00, 0x12800022, 0x00000016, 0xff00ff00, 
			0x12800022, 0x00000017, 0xff00ff00, 0x12800022, 0x00000018, 
			0xff00ff00, 0x12800022, 0x00000019, 0xff00ff00, 0x12800022, 
			0x0000001a, 0xff00ff00, 0x12800022, 0x0000001b, 0xff00ff00, 
			0x12800022, 0x0000001c, 0xff00ff00, 0x12800022, 0x0000001d, 
			0xff00ff00, 0x12800022, 0x0000001e, 0xff00ff00, 0x12800022, 
			0x0000001f, 0xff00ff00, 0x12800022, 0x00000020, 0xff00ff00, 
			0x12800022, 0x00000021, 0xff00ff00, 0x12800022, 0x00000022, 
			0xff00ff00, 0x12800022, 0x00000023, 0xff00ff00, 0x12800022, 
			0x00000024, 0xff00ff00, 0x12800022, 0x00000025, 0xff00ff00, 
			0x12800022, 0x00000026, 0xff00ff00, 0x12800022, 0x00000027, 
			0xff00ff00, 0x12800022, 0x00000028, 0xff00ff00, 0x12800022, 
			0x00000029, 0xff00ff00, 0x12800022, 0x0000002a, 0xff00ff00, 
			0x12800022, 0x0000002b, 0xff00ff00, 0x12800022, 0x0000002c, 
			0xff00ff00, 0x12800022, 0x0000002d, 0xff00ff00, 0x12800022, 
			0x0000002e, 0xff00ff00, 0x12800022, 0x0000002f, 0xff00ff00, 
			0x12800022, 0x00000030, 0xff00ff00, 0x12800022, 0x00000031, 
			0xff00ff00, 0x12800022, 0x00000032, 0xff00ff00, 0x12800022, 
			0x00000033, 0xff00ff00, 0x12800022, 0x00000034, 0xff00ff00, 
			0x12800022, 0x00000035, 0xff00ff00, 0x12800022, 0x00000036, 
			0xff00ff00, 0x12800022, 0x00000037, 0xff00ff00, 0x12800022, 
			0x00000038, 0xff00ff00, 0x12800022, 0x00000039, 0xff00ff00, 
			0x12800022, 0x0000003a, 0xff00ff00, 0x12800022, 0x0000003b, 
			0xff00ff00, 0x12800022, 0x0000003c, 0xff00ff00, 0x12800022, 
			0x0000003d, 0xff00ff00, 0x12800022, 0x0000003e, 0xff00ff00, 
			0x12800022, 0x0000003f, 0xff00ff00, 0x12800022, 0x00000040, 
			0xff00ff00, 0x12800022, 0x00000041, 0xff00ff00, 0x12800022, 
			0x00000042, 0xff00ff00, 0x12800022, 0x00000043, 0xff00ff00, 
			0x12800022, 0x00000044, 0xff00ff00, 0x12800022, 0x00000045, 
			0xff00ff00, 0x12800022, 0x00000046, 0xff00ff00, 0x12800022, 
			0x00000047, 0xff00ff00, 0x12800022, 0x00000048, 0xff00ff00, 
			0x12800022, 0x00000049, 0xff00ff00, 0x12800022, 0x0000004a, 
			0xff00ff00, 0x12800022, 0x0000004b, 0xff00ff00, 0x12800022, 
			0x0000004c, 0xff00ff00, 0x12800022, 0x0000004d, 0xff00ff00, 
			0x12800022, 0x0000004e, 0xff00ff00, 0x12800022, 0x0000004f, 
			0xff00ff00, 0x12800022, 0x00000050, 0xff00ff00, 0x12800022, 
			0x00000051, 0xff00ff00, 0x12800022, 0x00000052, 0xff00ff00, 
			0x12800022, 0x00000053, 0xff00ff00, 0x12800022, 0x00000054, 
			0xff00ff00, 0x12800022, 0x00000055, 0xff00ff00, 0x12800022, 
			0x00000056, 0xff00ff00, 0x12800022, 0x00000057, 0xff00ff00, 
			0x12800022, 0x00000058, 0xff00ff00, 0x12800022, 0x00000059, 
			0xff00ff00, 0x12800022, 0x0000005a, 0xff00ff00, 0x12800022, 
			0x0000005b, 0xff00ff00, 0x12800022, 0x0000005c, 0xff00ff00, 
			0x12800022, 0x0000005d, 0xff00ff00, 0x12800022, 0x0000005e, 
			0xff00ff00, 0x12800022, 0x0000005f, 0xff00ff00, 0x12800022, 
			0x00000060, 0xff00ff00, 0x12800022, 0x00000061, 0xff00ff00, 
			0x12800022, 0x00000062, 0xff00ff00, 0x12800022, 0x00000063, 
			0xff00ff00, 0x12800022, 0x00000064, 0xff00ff00, 0x12800022, 
			0x00000065, 0xff00ff00, 0x12800022, 0x00000066, 0xff00ff00, 
			0x12800022, 0x00000067, 0xff00ff00, 0x12800022, 0x00000068, 
			0xff00ff00, 0x12800022, 0x00000069, 0xff00ff00, 0x12800022, 
			0x0000006a, 0xff00ff00, 0x12800022, 0x0000006b, 0xff00ff00, 
			0x12800022, 0x0000006c, 0xff00ff00, 0x12800022, 0x0000006d, 
			0xff00ff00, 0x12800022, 0x0000006e, 0xff00ff00, 0x12800022, 
			0x0000006f, 0xff00ff00, 0x12800022, 0x00000070, 0xff00ff00, 
			0x12800022, 0x00000071, 0xff00ff00, 0x12800022, 0x00000072, 
			0xff00ff00, 0x12800022, 0x00000073, 0xff00ff00, 0x12800022, 
			0x00000074, 0xff00ff00, 0x12800022, 0x00000075, 0xff00ff00, 
			0x12800022, 0x00000076, 0xff00ff00, 0x12800022, 0x00000077, 
			0xff00ff00, 0x12800022, 0x00000078, 0xff00ff00, 0x12800022, 
			0x00000079, 0xff00ff00, 0x12800025, 0x0000007a, 0x12800031, 
			0x0000007b, 0x12800042, 0x0000007c, 0x12800052, 0x0000007d, 
			0x12800067, 0x0000007e, 0x1280007a, 0x0000007f, 0x12800092, 
			0x00000080, 0x128000a5, 0x00000081, 0x028000bd, 0x00000001, 
			0x028000d4, 0x01fc8000, 0x028000ef, 0x00000001, 0xff00ff00, 
			0x02800103, 0x10000001, 0x1480010a, 0x00000000, 0x02800114, 
			0x00000009, 0xff00ff00, 0x11800129, 0x00000000, 0x02800138, 
			0x01f00000, 0x14800142, 0x0000000c, 0x14800150, 0x00000030, 
			0xff00ff00, 0x1280015e, 0x00000082, 0xff00ff00, 0x12800170, 
			0x00000083, 0xff00ff00, 0x1280017c, 0x00000084, 0xff00ff00, 
			0x02800181, 0x00000000, 0xff00ff00, 0x12000002, 0x00000085, 
			0x12000018, 0x00000086, 0x12000019, 0x00000087, 0x12000021, 
			0x00000088, 0x12000022, 0x00000089, 0x12000023, 0x0000008a, 
			0x1200003a, 0x0000008b, 0x12000028, 0x0000008c, 0x1200003c, 
			0x0000008d, 0xff00ff00, 0x12000001, 0x0000008e, 0xff00ff00, 
			0x12000001, 0x0000008f, 0xff00ff00, 0x12000001, 0x00000090, 
			0xff00ff00, 0x12000001, 0x00000091, 0xff00ff00, 0x12000001, 
			0x00000092, 0xff00ff00, 0x1280019c, 0x00000093, 0xff00ff00, 
			0x028001aa, 0x00000001, 0xff00ff00 };



const StringDevice driver_list_670_tz[] = {
			{"/core/hwengines/bam",1285428979u, 556, NULL, 0, NULL },
			{"DALDEVICEID_SPMI_DEVICE",3290583706u, 568, NULL, 0, NULL },
			{"QUP_0I0",722463235u, 588, NULL, 0, NULL },
			{"QUP_0I0I",2366450348u, 600, NULL, 0, NULL },
			{"QUP_0I1",722463236u, 612, NULL, 0, NULL },
			{"QUP_0I1I",2366450381u, 624, NULL, 0, NULL },
			{"QUP_0I2",722463237u, 636, NULL, 0, NULL },
			{"QUP_0I2I",2366450414u, 648, NULL, 0, NULL },
			{"QUP_0I3",722463238u, 660, NULL, 0, NULL },
			{"QUP_0I3I",2366450447u, 672, NULL, 0, NULL },
			{"QUP_0I4",722463239u, 684, NULL, 0, NULL },
			{"QUP_0I4I",2366450480u, 696, NULL, 0, NULL },
			{"QUP_0I5",722463240u, 708, NULL, 0, NULL },
			{"QUP_0I5I",2366450513u, 720, NULL, 0, NULL },
			{"QUP_0I6",722463241u, 732, NULL, 0, NULL },
			{"QUP_0I6I",2366450546u, 744, NULL, 0, NULL },
			{"QUP_0I7",722463242u, 756, NULL, 0, NULL },
			{"QUP_0I7I",2366450579u, 768, NULL, 0, NULL },
			{"QUP_0I8",722463243u, 780, NULL, 0, NULL },
			{"QUP_0I8I",2366450612u, 792, NULL, 0, NULL },
			{"QUP_0I9",722463244u, 804, NULL, 0, NULL },
			{"QUP_0I9I",2366450645u, 816, NULL, 0, NULL },
			{"QUP_0I10",2366450356u, 828, NULL, 0, NULL },
			{"QUP_0I10I",783450493u, 840, NULL, 0, NULL },
			{"QUP_0I11",2366450357u, 852, NULL, 0, NULL },
			{"QUP_0I11I",783450526u, 864, NULL, 0, NULL },
			{"QUP_0I12",2366450358u, 876, NULL, 0, NULL },
			{"QUP_0I12I",783450559u, 888, NULL, 0, NULL },
			{"QUP_0S0",722463565u, 900, NULL, 0, NULL },
			{"QUP_0S0S",2366461248u, 912, NULL, 0, NULL },
			{"QUP_0S0G0v1",3165033547u, 924, NULL, 0, NULL },
			{"QUP_0S0G2v1",3165035725u, 936, NULL, 0, NULL },
			{"QUP_0S1",722463566u, 948, NULL, 0, NULL },
			{"QUP_0S1S",2366461281u, 960, NULL, 0, NULL },
			{"QUP_0S1G0v1",3166219468u, 972, NULL, 0, NULL },
			{"QUP_0S1G2v1",3166221646u, 984, NULL, 0, NULL },
			{"QUP_0S2",722463567u, 996, NULL, 0, NULL },
			{"QUP_0S2S",2366461314u, 1008, NULL, 0, NULL },
			{"QUP_0S2G0v1",3167405389u, 1020, NULL, 0, NULL },
			{"QUP_0S2G2v1",3167407567u, 1032, NULL, 0, NULL },
			{"QUP_0S3",722463568u, 1044, NULL, 0, NULL },
			{"QUP_0S3S",2366461347u, 1056, NULL, 0, NULL },
			{"QUP_0S3G0v1",3168591310u, 1068, NULL, 0, NULL },
			{"QUP_0S3G2v1",3168593488u, 1080, NULL, 0, NULL },
			{"QUP_0S4",722463569u, 1092, NULL, 0, NULL },
			{"QUP_0S4S",2366461380u, 1104, NULL, 0, NULL },
			{"QUP_0S4G0v1",3169777231u, 1116, NULL, 0, NULL },
			{"QUP_0S4G2v1",3169779409u, 1128, NULL, 0, NULL },
			{"QUP_0S5",722463570u, 1140, NULL, 0, NULL },
			{"QUP_0S5S",2366461413u, 1152, NULL, 0, NULL },
			{"QUP_0S5G0v1",3170963152u, 1164, NULL, 0, NULL },
			{"QUP_0S5G2v1",3170965330u, 1176, NULL, 0, NULL },
			{"QUP_0S6",722463571u, 1188, NULL, 0, NULL },
			{"QUP_0S6S",2366461446u, 1200, NULL, 0, NULL },
			{"QUP_0S6G0v1",3172149073u, 1212, NULL, 0, NULL },
			{"QUP_0S6G2v1",3172151251u, 1224, NULL, 0, NULL },
			{"QUP_0S7",722463572u, 1236, NULL, 0, NULL },
			{"QUP_0S7S",2366461479u, 1248, NULL, 0, NULL },
			{"QUP_0S7G0v1",3173334994u, 1260, NULL, 0, NULL },
			{"QUP_0S7G2v1",3173337172u, 1272, NULL, 0, NULL },
			{"QUP_0C",3405806445u, 1284, NULL, 0, NULL },
			{"QUP_0CS",722463072u, 1296, NULL, 0, NULL },
			{"QUP_1I0",722464324u, 1308, NULL, 0, NULL },
			{"QUP_1I0I",2366486285u, 1320, NULL, 0, NULL },
			{"QUP_1I1",722464325u, 1332, NULL, 0, NULL },
			{"QUP_1I1I",2366486318u, 1344, NULL, 0, NULL },
			{"QUP_1I2",722464326u, 1356, NULL, 0, NULL },
			{"QUP_1I2I",2366486351u, 1368, NULL, 0, NULL },
			{"QUP_1I3",722464327u, 1380, NULL, 0, NULL },
			{"QUP_1I3I",2366486384u, 1392, NULL, 0, NULL },
			{"QUP_1I4",722464328u, 1404, NULL, 0, NULL },
			{"QUP_1I4I",2366486417u, 1416, NULL, 0, NULL },
			{"QUP_1I5",722464329u, 1428, NULL, 0, NULL },
			{"QUP_1I5I",2366486450u, 1440, NULL, 0, NULL },
			{"QUP_1I6",722464330u, 1452, NULL, 0, NULL },
			{"QUP_1I6I",2366486483u, 1464, NULL, 0, NULL },
			{"QUP_1I7",722464331u, 1476, NULL, 0, NULL },
			{"QUP_1I7I",2366486516u, 1488, NULL, 0, NULL },
			{"QUP_1I8",722464332u, 1500, NULL, 0, NULL },
			{"QUP_1I8I",2366486549u, 1512, NULL, 0, NULL },
			{"QUP_1I9",722464333u, 1524, NULL, 0, NULL },
			{"QUP_1I9I",2366486582u, 1536, NULL, 0, NULL },
			{"QUP_1I10",2366486293u, 1548, NULL, 0, NULL },
			{"QUP_1I10I",784636414u, 1560, NULL, 0, NULL },
			{"QUP_1I11",2366486294u, 1572, NULL, 0, NULL },
			{"QUP_1I11I",784636447u, 1584, NULL, 0, NULL },
			{"QUP_1I12",2366486295u, 1596, NULL, 0, NULL },
			{"QUP_1I12I",784636480u, 1608, NULL, 0, NULL },
			{"QUP_1S0",722464654u, 1620, NULL, 0, NULL },
			{"QUP_1S0S",2366497185u, 1632, NULL, 0, NULL },
			{"QUP_1S0G0v1",161534220u, 1644, NULL, 0, NULL },
			{"QUP_1S0G2v1",161536398u, 1656, NULL, 0, NULL },
			{"QUP_1S1",722464655u, 1668, NULL, 0, NULL },
			{"QUP_1S1S",2366497218u, 1680, NULL, 0, NULL },
			{"QUP_1S1G0v1",162720141u, 1692, NULL, 0, NULL },
			{"QUP_1S1G2v1",162722319u, 1704, NULL, 0, NULL },
			{"QUP_1S2",722464656u, 1716, NULL, 0, NULL },
			{"QUP_1S2S",2366497251u, 1728, NULL, 0, NULL },
			{"QUP_1S2G0v1",163906062u, 1740, NULL, 0, NULL },
			{"QUP_1S2G2v1",163908240u, 1752, NULL, 0, NULL },
			{"QUP_1S3",722464657u, 1764, NULL, 0, NULL },
			{"QUP_1S3S",2366497284u, 1776, NULL, 0, NULL },
			{"QUP_1S3G0v1",165091983u, 1788, NULL, 0, NULL },
			{"QUP_1S3G2v1",165094161u, 1800, NULL, 0, NULL },
			{"QUP_1S4",722464658u, 1812, NULL, 0, NULL },
			{"QUP_1S4S",2366497317u, 1824, NULL, 0, NULL },
			{"QUP_1S4G0v1",166277904u, 1836, NULL, 0, NULL },
			{"QUP_1S4G2v1",166280082u, 1848, NULL, 0, NULL },
			{"QUP_1S5",722464659u, 1860, NULL, 0, NULL },
			{"QUP_1S5S",2366497350u, 1872, NULL, 0, NULL },
			{"QUP_1S5G0v1",167463825u, 1884, NULL, 0, NULL },
			{"QUP_1S5G2v1",167466003u, 1896, NULL, 0, NULL },
			{"QUP_1S6",722464660u, 1908, NULL, 0, NULL },
			{"QUP_1S6S",2366497383u, 1920, NULL, 0, NULL },
			{"QUP_1S6G0v1",168649746u, 1932, NULL, 0, NULL },
			{"QUP_1S6G2v1",168651924u, 1944, NULL, 0, NULL },
			{"QUP_1S7",722464661u, 1956, NULL, 0, NULL },
			{"QUP_1S7S",2366497416u, 1968, NULL, 0, NULL },
			{"QUP_1S7G0v1",169835667u, 1980, NULL, 0, NULL },
			{"QUP_1S7G2v1",169837845u, 1992, NULL, 0, NULL },
			{"QUP_1C",3405806478u, 2004, NULL, 0, NULL },
			{"QUP_1CS",722464161u, 2016, NULL, 0, NULL },
			{"QUPV3_HW_PROP",751796930u, 2028, NULL, 0, NULL },
			{"/dev/core/mproc/smem",1012060316u, 2148, NULL, 0, NULL },
			{"/dev/NOCError",1518077100u, 2184, NULL, 0, NULL },
			{"/dev/icbcfg/boot",2382255043u, 2196, NULL, 0, NULL },
			{"/icb/uarb",3907582491u, 2208, NULL, 0, NULL },
			{"/storage/rpmb",2005825481u, 2220, NULL, 0, NULL },
			{"/dev/vmidmt",337776915u, 2232, NULL, 0, NULL },
			{"ddr",193489311u, 2308, NULL, 0, NULL },
			{"int_controller",2851810835u, 2320, NULL, 0, NULL },
			{"pimem",270870909u, 2332, NULL, 0, NULL },
			{"timer",275614598u, 2344, NULL, 0, NULL },
			{"dbg_cfg",26643713u, 2356, NULL, 0, NULL },
			{"/secboot/anti_rollback",3024823143u, 2368, NULL, 0, NULL },
			{"boot_restrict",2479631688u, 2380, NULL, 0, NULL }
};
