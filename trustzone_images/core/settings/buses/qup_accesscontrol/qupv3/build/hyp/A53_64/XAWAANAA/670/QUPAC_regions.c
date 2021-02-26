#include "ACCommon.h"
#include "AccessControl.h"

static ACRegion AC_REGION_QUP_0I0[]=
{
	{
		.uMemoryStart = 	0x820000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I0[]=
{
	0x0,
};

ACDeviceInfo device_qup_0i0 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I0,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I0,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I0I[]=
{
	{
		.uMemoryStart = 	0x810000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I0I[]=
{
	0x20,
};

ACDeviceInfo device_qup_0i0i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I0I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I0I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I1[]=
{
	{
		.uMemoryStart = 	0x824000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I1[]=
{
	0x1,
};

ACDeviceInfo device_qup_0i1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I1,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I1,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I1I[]=
{
	{
		.uMemoryStart = 	0x811000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I1I[]=
{
	0x21,
};

ACDeviceInfo device_qup_0i1i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I1I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I1I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I2[]=
{
	{
		.uMemoryStart = 	0x828000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I2[]=
{
	0x2,
};

ACDeviceInfo device_qup_0i2 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I2,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I2,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I2I[]=
{
	{
		.uMemoryStart = 	0x812000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I2I[]=
{
	0x22,
};

ACDeviceInfo device_qup_0i2i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I2I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I2I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I3[]=
{
	{
		.uMemoryStart = 	0x82c000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I3[]=
{
	0x3,
};

ACDeviceInfo device_qup_0i3 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I3,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I3,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I3I[]=
{
	{
		.uMemoryStart = 	0x813000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I3I[]=
{
	0x23,
};

ACDeviceInfo device_qup_0i3i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I3I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I3I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I4[]=
{
	{
		.uMemoryStart = 	0x830000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I4[]=
{
	0x4,
};

ACDeviceInfo device_qup_0i4 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I4,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I4,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I4I[]=
{
	{
		.uMemoryStart = 	0x814000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I4I[]=
{
	0x24,
};

ACDeviceInfo device_qup_0i4i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I4I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I4I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I5[]=
{
	{
		.uMemoryStart = 	0x834000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I5[]=
{
	0x5,
};

ACDeviceInfo device_qup_0i5 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I5,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I5,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I5I[]=
{
	{
		.uMemoryStart = 	0x815000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I5I[]=
{
	0x25,
};

ACDeviceInfo device_qup_0i5i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I5I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I5I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I6[]=
{
	{
		.uMemoryStart = 	0x838000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I6[]=
{
	0x6,
};

ACDeviceInfo device_qup_0i6 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I6,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I6,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I6I[]=
{
	{
		.uMemoryStart = 	0x816000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I6I[]=
{
	0x26,
};

ACDeviceInfo device_qup_0i6i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I6I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I6I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I7[]=
{
	{
		.uMemoryStart = 	0x83c000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I7[]=
{
	0x7,
};

ACDeviceInfo device_qup_0i7 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I7,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I7,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I7I[]=
{
	{
		.uMemoryStart = 	0x817000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I7I[]=
{
	0x27,
};

ACDeviceInfo device_qup_0i7i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I7I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I7I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I8[]=
{
	{
		.uMemoryStart = 	0x840000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I8[]=
{
	0x8,
};

ACDeviceInfo device_qup_0i8 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I8,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I8,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I8I[]=
{
	{
		.uMemoryStart = 	0x818000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I8I[]=
{
	0x28,
};

ACDeviceInfo device_qup_0i8i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I8I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I8I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I9[]=
{
	{
		.uMemoryStart = 	0x844000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I9[]=
{
	0x9,
};

ACDeviceInfo device_qup_0i9 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I9,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I9,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I9I[]=
{
	{
		.uMemoryStart = 	0x819000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I9I[]=
{
	0x29,
};

ACDeviceInfo device_qup_0i9i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I9I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I9I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I10[]=
{
	{
		.uMemoryStart = 	0x848000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I10[]=
{
	0xa,
};

ACDeviceInfo device_qup_0i10 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I10,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I10,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I10I[]=
{
	{
		.uMemoryStart = 	0x81a000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I10I[]=
{
	0x2a,
};

ACDeviceInfo device_qup_0i10i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I10I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I10I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I11[]=
{
	{
		.uMemoryStart = 	0x84c000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I11[]=
{
	0xb,
};

ACDeviceInfo device_qup_0i11 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I11,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I11,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I11I[]=
{
	{
		.uMemoryStart = 	0x81b000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I11I[]=
{
	0x2b,
};

ACDeviceInfo device_qup_0i11i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I11I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I11I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I12[]=
{
	{
		.uMemoryStart = 	0x850000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_0I12[]=
{
	0xc,
};

ACDeviceInfo device_qup_0i12 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I12,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I12,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0I12I[]=
{
	{
		.uMemoryStart = 	0x81c000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0I12I[]=
{
	0x2c,
};

ACDeviceInfo device_qup_0i12i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0I12I,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0I12I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S0[]=
{
	{
		.uMemoryStart = 	0x880000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x881000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S0[]=
{
	0x10,
};

ACDeviceInfo device_qup_0s0 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S0,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S0,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S0S[]=
{
	{
		.uMemoryStart = 	0x882000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s0s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S0S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S0G0v1[]=
{
	{
		.uMemoryStart = 	0x3d00000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d01000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s0g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S0G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S0G2v1[]=
{
	{
		.uMemoryStart = 	0x3d02000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d03000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s0g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S0G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S1[]=
{
	{
		.uMemoryStart = 	0x884000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x885000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S1[]=
{
	0x11,
};

ACDeviceInfo device_qup_0s1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S1,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S1S[]=
{
	{
		.uMemoryStart = 	0x886000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s1s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S1S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S1G0v1[]=
{
	{
		.uMemoryStart = 	0x3511000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3512000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s1g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S1G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S1G2v1[]=
{
	{
		.uMemoryStart = 	0x3513000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3514000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s1g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S1G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S2[]=
{
	{
		.uMemoryStart = 	0x888000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x889000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S2[]=
{
	0x12,
};

ACDeviceInfo device_qup_0s2 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S2,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S2,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S2S[]=
{
	{
		.uMemoryStart = 	0x88a000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s2s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S2S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S2G0v1[]=
{
	{
		.uMemoryStart = 	0x351b000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x351c000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s2g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S2G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S2G2v1[]=
{
	{
		.uMemoryStart = 	0x351d000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x351e000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s2g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S2G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S3[]=
{
	{
		.uMemoryStart = 	0x88c000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x88d000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S3[]=
{
	0x13,
};

ACDeviceInfo device_qup_0s3 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S3,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S3,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S3S[]=
{
	{
		.uMemoryStart = 	0x88e000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s3s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S3S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S3G0v1[]=
{
	{
		.uMemoryStart = 	0x3d29000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d2a000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s3g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S3G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S3G2v1[]=
{
	{
		.uMemoryStart = 	0x3d2b000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d2c000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s3g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S3G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S4[]=
{
	{
		.uMemoryStart = 	0x890000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x891000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S4[]=
{
	0x14,
};

ACDeviceInfo device_qup_0s4 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S4,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S4,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S4S[]=
{
	{
		.uMemoryStart = 	0x892000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s4s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S4S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S4G0v1[]=
{
	{
		.uMemoryStart = 	0x3d59000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d5a000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s4g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S4G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S4G2v1[]=
{
	{
		.uMemoryStart = 	0x3d5b000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d5c000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s4g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S4G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S5[]=
{
	{
		.uMemoryStart = 	0x894000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x895000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S5[]=
{
	0x15,
};

ACDeviceInfo device_qup_0s5 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S5,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S5,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S5S[]=
{
	{
		.uMemoryStart = 	0x896000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s5s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S5S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S5G0v1[]=
{
	{
		.uMemoryStart = 	0x3d55000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d56000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s5g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S5G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S5G2v1[]=
{
	{
		.uMemoryStart = 	0x3d57000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d58000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s5g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S5G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S6[]=
{
	{
		.uMemoryStart = 	0x898000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x899000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S6[]=
{
	0x16,
};

ACDeviceInfo device_qup_0s6 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S6,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S6,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S6S[]=
{
	{
		.uMemoryStart = 	0x89a000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s6s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S6S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S6G0v1[]=
{
	{
		.uMemoryStart = 	0x3d2d000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d2e000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s6g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S6G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S6G2v1[]=
{
	{
		.uMemoryStart = 	0x3d2f000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d30000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s6g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S6G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S7[]=
{
	{
		.uMemoryStart = 	0x89c000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x89d000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_0S7[]=
{
	0x17,
};

ACDeviceInfo device_qup_0s7 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S7,
	.eVmidmtInstance = 	24,
	.pInternalSidList = AC_InternalSID_QUP_0S7,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_0S7S[]=
{
	{
		.uMemoryStart = 	0x89e000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s7s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S7S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S7G0v1[]=
{
	{
		.uMemoryStart = 	0x3d5d000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d5e000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s7g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S7G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0S7G2v1[]=
{
	{
		.uMemoryStart = 	0x3d5f000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3d60000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0s7g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0S7G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0C[]=
{
	{
		.uMemoryStart = 	0x804000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x805000,
		.uMemorySize = 	0x2000,
	},
	{
		.uMemoryStart = 	0x807000,
		.uMemorySize = 	0x8000,
	},
	{
		.uMemoryStart = 	0x8c0000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0c =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0C,
	.uNumMappings = 4,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_0CS[]=
{
	{
		.uMemoryStart = 	0x80f000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x8c1000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_0cs =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_0CS,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1I0[]=
{
	{
		.uMemoryStart = 	0xa20000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I0[]=
{
	0x0,
};

ACDeviceInfo device_qup_1i0 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I0,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I0,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I0I[]=
{
	{
		.uMemoryStart = 	0xa10000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I0I[]=
{
	0x20,
};

ACDeviceInfo device_qup_1i0i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I0I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I0I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I1[]=
{
	{
		.uMemoryStart = 	0xa24000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I1[]=
{
	0x1,
};

ACDeviceInfo device_qup_1i1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I1,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I1,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I1I[]=
{
	{
		.uMemoryStart = 	0xa11000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I1I[]=
{
	0x21,
};

ACDeviceInfo device_qup_1i1i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I1I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I1I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I2[]=
{
	{
		.uMemoryStart = 	0xa28000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I2[]=
{
	0x2,
};

ACDeviceInfo device_qup_1i2 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I2,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I2,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I2I[]=
{
	{
		.uMemoryStart = 	0xa12000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I2I[]=
{
	0x22,
};

ACDeviceInfo device_qup_1i2i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I2I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I2I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I3[]=
{
	{
		.uMemoryStart = 	0xa2c000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I3[]=
{
	0x3,
};

ACDeviceInfo device_qup_1i3 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I3,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I3,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I3I[]=
{
	{
		.uMemoryStart = 	0xa13000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I3I[]=
{
	0x23,
};

ACDeviceInfo device_qup_1i3i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I3I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I3I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I4[]=
{
	{
		.uMemoryStart = 	0xa30000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I4[]=
{
	0x4,
};

ACDeviceInfo device_qup_1i4 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I4,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I4,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I4I[]=
{
	{
		.uMemoryStart = 	0xa14000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I4I[]=
{
	0x24,
};

ACDeviceInfo device_qup_1i4i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I4I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I4I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I5[]=
{
	{
		.uMemoryStart = 	0xa34000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I5[]=
{
	0x5,
};

ACDeviceInfo device_qup_1i5 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I5,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I5,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I5I[]=
{
	{
		.uMemoryStart = 	0xa15000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I5I[]=
{
	0x25,
};

ACDeviceInfo device_qup_1i5i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I5I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I5I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I6[]=
{
	{
		.uMemoryStart = 	0xa38000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I6[]=
{
	0x6,
};

ACDeviceInfo device_qup_1i6 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I6,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I6,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I6I[]=
{
	{
		.uMemoryStart = 	0xa16000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I6I[]=
{
	0x26,
};

ACDeviceInfo device_qup_1i6i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I6I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I6I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I7[]=
{
	{
		.uMemoryStart = 	0xa3c000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I7[]=
{
	0x7,
};

ACDeviceInfo device_qup_1i7 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I7,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I7,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I7I[]=
{
	{
		.uMemoryStart = 	0xa17000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I7I[]=
{
	0x27,
};

ACDeviceInfo device_qup_1i7i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I7I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I7I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I8[]=
{
	{
		.uMemoryStart = 	0xa40000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I8[]=
{
	0x8,
};

ACDeviceInfo device_qup_1i8 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I8,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I8,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I8I[]=
{
	{
		.uMemoryStart = 	0xa18000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I8I[]=
{
	0x28,
};

ACDeviceInfo device_qup_1i8i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I8I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I8I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I9[]=
{
	{
		.uMemoryStart = 	0xa44000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I9[]=
{
	0x9,
};

ACDeviceInfo device_qup_1i9 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I9,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I9,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I9I[]=
{
	{
		.uMemoryStart = 	0xa19000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I9I[]=
{
	0x29,
};

ACDeviceInfo device_qup_1i9i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I9I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I9I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I10[]=
{
	{
		.uMemoryStart = 	0xa48000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I10[]=
{
	0xa,
};

ACDeviceInfo device_qup_1i10 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I10,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I10,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I10I[]=
{
	{
		.uMemoryStart = 	0xa1a000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I10I[]=
{
	0x2a,
};

ACDeviceInfo device_qup_1i10i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I10I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I10I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I11[]=
{
	{
		.uMemoryStart = 	0xa4c000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I11[]=
{
	0xb,
};

ACDeviceInfo device_qup_1i11 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I11,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I11,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I11I[]=
{
	{
		.uMemoryStart = 	0xa1b000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I11I[]=
{
	0x2b,
};

ACDeviceInfo device_qup_1i11i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I11I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I11I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I12[]=
{
	{
		.uMemoryStart = 	0xa50000,
		.uMemorySize = 	0x4000,
	},
};

static uint32 AC_InternalSID_QUP_1I12[]=
{
	0xc,
};

ACDeviceInfo device_qup_1i12 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I12,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I12,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1I12I[]=
{
	{
		.uMemoryStart = 	0xa1c000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1I12I[]=
{
	0x2c,
};

ACDeviceInfo device_qup_1i12i =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1I12I,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1I12I,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S0[]=
{
	{
		.uMemoryStart = 	0xa80000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa81000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S0[]=
{
	0x10,
};

ACDeviceInfo device_qup_1s0 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S0,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S0,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S0S[]=
{
	{
		.uMemoryStart = 	0xa82000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s0s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S0S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S0G0v1[]=
{
	{
		.uMemoryStart = 	0x3941000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3942000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s0g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S0G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S0G2v1[]=
{
	{
		.uMemoryStart = 	0x3943000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3944000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s0g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S0G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S1[]=
{
	{
		.uMemoryStart = 	0xa84000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa85000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S1[]=
{
	0x11,
};

ACDeviceInfo device_qup_1s1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S1,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S1S[]=
{
	{
		.uMemoryStart = 	0xa86000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s1s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S1S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S1G0v1[]=
{
	{
		.uMemoryStart = 	0x3906000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3907000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s1g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S1G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S1G2v1[]=
{
	{
		.uMemoryStart = 	0x3904000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3905000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s1g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S1G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S2[]=
{
	{
		.uMemoryStart = 	0xa88000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa89000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S2[]=
{
	0x12,
};

ACDeviceInfo device_qup_1s2 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S2,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S2,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S2S[]=
{
	{
		.uMemoryStart = 	0xa8a000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s2s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S2S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S2G0v1[]=
{
	{
		.uMemoryStart = 	0x3937000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3938000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s2g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S2G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S2G2v1[]=
{
	{
		.uMemoryStart = 	0x3935000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3936000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s2g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S2G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S3[]=
{
	{
		.uMemoryStart = 	0xa8c000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa8d000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S3[]=
{
	0x13,
};

ACDeviceInfo device_qup_1s3 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S3,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S3,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S3S[]=
{
	{
		.uMemoryStart = 	0xa8e000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s3s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S3S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S3G0v1[]=
{
	{
		.uMemoryStart = 	0x351f000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3520000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s3g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S3G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S3G2v1[]=
{
	{
		.uMemoryStart = 	0x3521000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3522000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s3g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S3G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S4[]=
{
	{
		.uMemoryStart = 	0xa90000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa91000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S4[]=
{
	0x14,
};

ACDeviceInfo device_qup_1s4 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S4,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S4,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S4S[]=
{
	{
		.uMemoryStart = 	0xa92000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s4s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S4S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S4G0v1[]=
{
	{
		.uMemoryStart = 	0x3931000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3932000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s4g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S4G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S4G2v1[]=
{
	{
		.uMemoryStart = 	0x3933000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3934000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s4g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S4G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S5[]=
{
	{
		.uMemoryStart = 	0xa94000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa95000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S5[]=
{
	0x15,
};

ACDeviceInfo device_qup_1s5 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S5,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S5,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S5S[]=
{
	{
		.uMemoryStart = 	0xa96000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s5s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S5S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S5G0v1[]=
{
	{
		.uMemoryStart = 	0x3969000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x396a000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s5g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S5G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S5G2v1[]=
{
	{
		.uMemoryStart = 	0x396b000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x396c000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s5g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S5G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S6[]=
{
	{
		.uMemoryStart = 	0xa98000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa99000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S6[]=
{
	0x16,
};

ACDeviceInfo device_qup_1s6 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S6,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S6,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S6S[]=
{
	{
		.uMemoryStart = 	0xa9a000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s6s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S6S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S6G0v1[]=
{
	{
		.uMemoryStart = 	0x3521000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3522000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s6g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S6G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S6G2v1[]=
{
	{
		.uMemoryStart = 	0x351f000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3520000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s6g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S6G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S7[]=
{
	{
		.uMemoryStart = 	0xa9c000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa9d000,
		.uMemorySize = 	0x1000,
	},
};

static uint32 AC_InternalSID_QUP_1S7[]=
{
	0x17,
};

ACDeviceInfo device_qup_1s7 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S7,
	.eVmidmtInstance = 	25,
	.pInternalSidList = AC_InternalSID_QUP_1S7,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 1,
};

static ACRegion AC_REGION_QUP_1S7S[]=
{
	{
		.uMemoryStart = 	0xa9e000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s7s =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S7S,
	.uNumMappings = 1,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S7G0v1[]=
{
	{
		.uMemoryStart = 	0x3951000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3952000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s7g0v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S7G0v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1S7G2v1[]=
{
	{
		.uMemoryStart = 	0x3953000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0x3954000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1s7g2v1 =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1S7G2v1,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1C[]=
{
	{
		.uMemoryStart = 	0xa04000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xa05000,
		.uMemorySize = 	0x2000,
	},
	{
		.uMemoryStart = 	0xa07000,
		.uMemorySize = 	0x8000,
	},
	{
		.uMemoryStart = 	0xac0000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1c =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1C,
	.uNumMappings = 4,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

static ACRegion AC_REGION_QUP_1CS[]=
{
	{
		.uMemoryStart = 	0xa0f000,
		.uMemorySize = 	0x1000,
	},
	{
		.uMemoryStart = 	0xac1000,
		.uMemorySize = 	0x1000,
	},
};

ACDeviceInfo device_qup_1cs =
{
	.eSmmuInstance = 	0,
	.pMappings = AC_REGION_QUP_1CS,
	.uNumMappings = 2,
	.uNumExternalSids = 0,
	.uNumInternalSids = 0,
};

