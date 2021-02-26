/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tz/2.0.2/api/dal/DALDeviceId.h#1 $
=============================================================================*/

#ifndef DALIDEVICEID_H
#define DALIDEVICEID_H


#define DALDEVICEID_SIM_DEVICE          0x02000001
#define DALDEVICEID_SAMPLE_DEVICE       0x02000002

#define DALDEVICEID_TIMER               0x02000003
#define DALDEVICEID_SYSTEM              0x02000003
#define DALDEVICEID_INTERRUPTCONTROLLER 0x02000004
#define DALDEVICEID_SYSTEMCALL          0x02000005

#define DALDEVICEID_TLMM                0x02000020

#define DALDEVICEID_I2C_DEVICE_1        0x02000021
#define DALDEVICEID_I2C_DEVICE_2        0x02000022
#define DALDEVICEID_I2C_DEVICE_3        0x02000023
#define DALDEVICEID_I2C_DEVICE_4        0x02000024
#define DALDEVICEID_I2C_DEVICE_5        0x02000025

#define DALDEVICEID_VDEC_DEVICE         0x02000026

#define DALDEVICEID_AUDIO_TEST          0x02000027
#define DALDEVICEID_AUDIO_QDSP          0x02000028
#define DALDEVICEID_ADIE_CODEC          0x02000029
#define DALDEVICEID_ADIE_CODECDB        0x0200002A

#define DALDEVICEID_TLMM_MASTER         0x0200002B
#define DALDEVICEID_LPA                 0x0200002C
#define DALDEVICEID_VENC_DEVICE         0x0200002D

#define DALDEVICEID_EBI2_DEVICE_1       0x02000030

#define DALDEVICEID_EBI1_MPU            0x02000031
#define DALDEVICEID_FPB1_MPU            0x02000032
#define DALDEVICEID_FPB2_MPU            0x02000033
#define DALDEVICEID_FPB3_MPU            0x02000034
#define DALDEVICEID_LPB1_MPU            0x02000035
#define DALDEVICEID_LPB2_MPU            0x02000036
#define DALDEVICEID_LPB3_MPU            0x02000037
#define DALDEVICEID_NAND_MPU            0x02000038
#define DALDEVICEID_PBUS_MPU            0x02000039
#define DALDEVICEID_PMIC_RPU            0x0200003A
#define DALDEVICEID_SDC1_RPU            0x0200003B
#define DALDEVICEID_SDC2_RPU            0x0200003C
#define DALDEVICEID_SMI_MPU             0x0200003D
#define DALDEVICEID_SSBI_RPU            0x0200003E

#define DALDEVICEID_SBI_MODEM_SBI_0     0x0200003F
#define DALDEVICEID_SBI_MODEM_SBI_1     0x02000040
#define DALDEVICEID_SBI_TX_SBI          0x02000041
#define DALDEVICEID_SBI_MODEM_SSBI_0    0x02000042
#define DALDEVICEID_SBI_MODEM_SSBI_1    0x02000043
#define DALDEVICEID_SBI_MODEM_SSBI_2    0x02000044
#define DALDEVICEID_SBI_TX_SSBI         0x02000045
#define DALDEVICEID_SBI_CODEC_SSBI      0x02000046
#define DALDEVICEID_SBI_PMIC_SSBI       0x02000047

#define DALDEVICEID_SPI_DEVICE_1        0x02000048

#define DALDEVICEID_AHB                 0x02000049

#define DALDEVICEID_FLASH_DEVICE_1      0x02000050

#define DALDEVICEID_DMOV_DEVICE_0       0x02000051
#define DALDEVICEID_DMOV_DEVICE_1       0x02000052

#define DALDEVICEID_AXI                 0x02000053

#define DALDEVICEID_STILLENC_DEVICE     0x02000060
#define DALDEVICEID_STILLDEC_DEVICE     0x02000061

#define DALDEVICEID_SBI_MODEM_SSBI_3    0x02000062
#define DALDEVICEID_SBI_MODEM_SSBI_4    0x02000063
#define DALDEVICEID_I2C_DEVICE_6        0x02000064

#define DALDEVICEID_ADIE_CTL            0x02000065
#define DALDEVICEID_RDM                 0x02000066

#define DALDEVICEID_ADC                 0x02000067
#define DALDEVICEID_XOADC               0x02000068
#define DALDEVICEID_ACDB_1              0x02000069

#define DALDEVICEID_SOFTWARESWITCHES    0x0200006A
#define DALDEVICEID_FPGA                0x0200006B

#define DALDEVICEID_ULOG                0x0200006C

#define DALDEVICEID_SPS                 0x0200006D

#define DALDEVICEID_GLBCTXT             0x0200006E

#define DALDEVICEID_CHIPINFO            0x0200006F
#define DALDEVICEID_HWIO                0x02000070
#define DALDEVICEID_CBC_DEVICE_1        0x02000071

#define DALDEVICEID_EBI0_MPU            0x02000072 
#define DALDEVICEID_IMEM_MPU            0x02000073

#define DEVICEID_AUDIO_CAD              0x02000074
#define DEVICEID_AUDIO_CVD              0x02000075
#define DALDEVICEID_NAND2_MPU           0x02000076  

#define DALDEVICEID_GSBI_DEVICE_0       0x02000077
#define DALDEVICEID_GSBI_DEVICE_1       0x02000078
#define DALDEVICEID_GSBI_DEVICE_2       0x02000079
#define DALDEVICEID_GSBI_DEVICE_3       0x0200007A
#define DALDEVICEID_GSBI_DEVICE_4       0x0200007B

#define DALDEVICEID_DMOV_DEVICE_2       0x0200007C

#define DALDEVICEID_INO                 0x0200007D

#define DALDEVICEID_MEMARB              0x0200007E

#define DALDEVICEID_RPM                 0x0200007F
#define DALDEVICEID_RPMFW               0x02000080

#define DALDEVICEID_UARTDM_DEVICE_1     0x02000081
#define DALDEVICEID_UARTDM_DEVICE_2     0x02000082

#define DALDEVICEID_UARTLG_DEVICE_1     0x02000083
#define DALDEVICEID_UARTLG_DEVICE_2     0x02000084
#define DALDEVICEID_UARTLG_DEVICE_3     0x02000085

#define DALDEVICEID_UARTGSBI_DEVICE_1   0x02000086
#define DALDEVICEID_UARTGSBI_DEVICE_2   0x02000087
#define DALDEVICEID_UARTGSBI_DEVICE_3   0x02000088
#define DALDEVICEID_UARTGSBI_DEVICE_4   0x02000089
#define DALDEVICEID_UARTGSBI_DEVICE_5   0x0200008A

#define DALDEVICEID_GPIOINTERRUPTCONTROLLER 0x0200008B

#define DALDEVICEID_URAD_DALTF          0x0200008C
#define DALDEVICEID_NAT_DEVICE          0x0200008D

#define DALDEVICEID_DMOV_CONFIG         0x0200008E

#define DALDEVICEID_SIC                 0x0200008F

#define DALDEVICEID_MODEM_APR           0x02000090
#define DALDEVICEID_APPS_APR            0x02000091
#define DALDEVICEID_QDSP_APR            0x02000092

#define DALDEVICEID_WLAN_DEVICE         0x02000093 

#define DALDEVICEID_SDCC_DEVICE_1       0x02000094          
#define DALDEVICEID_SDCC_DEVICE_2       0x02000095          
#define DALDEVICEID_SDCC_DEVICE_3       0x02000096          
#define DALDEVICEID_SDCC_DEVICE_4       0x02000097          
#define DALDEVICEID_SDCC_DEVICE_5       0x02000098     

#define DALDEVICEID_IPCINT              0x02000099
#define DALDEVICEID_ADSPRTOSSVC         0x0200009A

#define DALDEVICEID_TIMETICK            0x0200009B
#define DALDEVICEID_SLEEP_XTAL_TIMETICK 0x0200009C
#define DALDEVICEID_AGPT                0x0200009D
#define DALDEVICEID_SLEEP_XTAL_TIMER0   0x0200009E
#define DALDEVICEID_ADGT                0x0200009F

#define DALDEVICEID_I2CPD_DEVICE_1      0x020000A0
#define DALDEVICEID_I2CPD_DEVICE_2      0x020000A1
#define DALDEVICEID_I2CPD_DEVICE_3      0x020000A2
#define DALDEVICEID_I2CPD_DEVICE_4      0x020000A3
#define DALDEVICEID_I2CPD_DEVICE_5      0x020000A4
#define DALDEVICEID_I2CPD_DEVICE_6      0x020000A5

#define DALDEVICEID_SBI_PMIC_SSBI_2     0x020000A6 

#define DALDEVICEID_TIME_CFG            0x020000A7

#define DALDEVICEID_KPD                 0x020000A8
#define DALDEVICEID_KPDPLATFORM         0x020000A9
#define DALDEVICEID_TOUCH_SCREEN        0x020000AA
#define DALDEVICEID_GPIOINT                   0x020000AB

#define DALDEVICEID_SYSCALL             0x020000AC
#define DALDEVICEID_TSENS               0x020000AD
#define DALDEVICEID_DEVIO               0x020000AE

#define DALDEVICEID_LTCADC              0x020000AF

#define DALDEVICEID_BUSPM               0x020000B0

#define DALDEVICEID_GSBI_DEVICE_5       0x020000B1
#define DALDEVICEID_GSBI_DEVICE_6       0x020000B2
#define DALDEVICEID_GSBI_DEVICE_7       0x020000B3
#define DALDEVICEID_GSBI_DEVICE_8       0x020000B4
#define DALDEVICEID_GSBI_DEVICE_9       0x020000B5
#define DALDEVICEID_GSBI_DEVICE_10      0x020000B6
#define DALDEVICEID_GSBI_DEVICE_11      0x020000B7
#define DALDEVICEID_GSBI_DEVICE_12      0x020000B8

#define DALDEVICEID_SPI_DEVICE_2        0x020000B9
#define DALDEVICEID_SPI_DEVICE_3        0x020000BA
#define DALDEVICEID_SPI_DEVICE_4        0x020000BB
#define DALDEVICEID_SPI_DEVICE_5        0x020000BC

#define DALDEVICEID_MODEM_DALTEST       0x020000BD
#define DALDEVICEID_APP_DALTEST         0x020000BE

#define DALDEVICEID_UARTGSBI_DEVICE_0   0x020000BF

#define DALDEVICEID_I2CPD_DEVICE_7      0x020000C0
#define DALDEVICEID_I2CPD_DEVICE_8      0x020000C1
#define DALDEVICEID_I2CPD_DEVICE_9      0x020000C2
#define DALDEVICEID_I2CPD_DEVICE_10     0x020000C3
#define DALDEVICEID_I2CPD_DEVICE_11     0x020000C4
#define DALDEVICEID_I2CPD_DEVICE_12     0x020000C5

#define DALDEVICEID_I2C_DEVICE_7        0x020000C6
#define DALDEVICEID_I2C_DEVICE_8        0x020000C7
#define DALDEVICEID_I2C_DEVICE_9        0x020000C8
#define DALDEVICEID_I2C_DEVICE_10       0x020000C9
#define DALDEVICEID_I2C_DEVICE_11       0x020000CA
#define DALDEVICEID_I2C_DEVICE_12       0x020000CB

#define DALDEVICEID_SDIOC_DEVICE_1      0x020000CC
#define DALDEVICEID_SDIOC_DEVICE_2      0x020000CD
#define DALDEVICEID_GPIOEXPANDER_0      0x020000CE
#define DALDEVICEID_GPIOEXPANDER_1      0x020000CF
#define DALDEVICEID_GPIOEXPANDER_2      0x020000D0
#define DALDEVICEID_GPIOEXPANDER_3      0x020000D1

/* New video decoder device ids to support parallel decodes*/
#define DALDEVICEID_VDEC_DEVICE_0       0x020000D2
#define DALDEVICEID_VDEC_DEVICE_1       0x020000D3
#define DALDEVICEID_VDEC_DEVICE_2       0x020000D4
#define DALDEVICEID_VDEC_DEVICE_3       0x020000D5
#define DALDEVICEID_VDEC_DEVICE_4       0x020000D6
#define DALDEVICEID_VDEC_DEVICE_5       0x020000D7
#define DALDEVICEID_VDEC_DEVICE_6       0x020000D8
#define DALDEVICEID_VDEC_DEVICE_7       0x020000D9
#define DALDEVICEID_VDEC_DEVICE_8       0x020000DA
#define DALDEVICEID_VDEC_DEVICE_9       0x020000DB
#define DALDEVICEID_VDEC_DEVICE_10      0x020000DC
#define DALDEVICEID_VDEC_DEVICE_11      0x020000DD
#define DALDEVICEID_VDEC_DEVICE_12      0x020000DE
#define DALDEVICEID_VDEC_DEVICE_13      0x020000DF
#define DALDEVICEID_VDEC_DEVICE_14      0x020000E0
#define DALDEVICEID_VDEC_DEVICE_15      0x020000E1
#define DALDEVICEID_VDEC_DEVICE_16      0x020000E2
#define DALDEVICEID_VDEC_DEVICE_17      0x020000E3
#define DALDEVICEID_VDEC_DEVICE_18      0x020000E4
#define DALDEVICEID_VDEC_DEVICE_19      0x020000E5
#define DALDEVICEID_VDEC_DEVICE_20      0x020000E6
#define DALDEVICEID_VDEC_DEVICE_21      0x020000E7
#define DALDEVICEID_VDEC_DEVICE_22      0x020000E8
#define DALDEVICEID_VDEC_DEVICE_23      0x020000E9
#define DALDEVICEID_VDEC_DEVICE_24      0x020000EA
#define DALDEVICEID_VDEC_DEVICE_25      0x020000EB
#define DALDEVICEID_VDEC_DEVICE_26      0x020000EC
#define DALDEVICEID_VDEC_DEVICE_27      0x020000ED
#define DALDEVICEID_VDEC_DEVICE_28      0x020000EE
#define DALDEVICEID_VDEC_DEVICE_29      0x020000EF
#define DALDEVICEID_VDEC_DEVICE_30      0x020000F0
#define DALDEVICEID_VDEC_DEVICE_31      0x020000F1

#define DALDEVICEID_LTCADC_DEVICE_0     0x020000F2
#define DALDEVICEID_LTCADC_DEVICE_1     0x020000F3
#define DALDEVICEID_LTCADC_DEVICE_2     0x020000F4

#define DALDEVICEID_SMDL                0x020000F5

#define DALDEVICEID_M2VMT               0x020000F6
#define DALDEVICEID_SMPSS_MPU           0x020000F8 
#define DALDEVICEID_SMPSS_L2_TCM_MPU    0x020000F9
#define DALDEVICEID_MSS_MPU             0x020000FA
#define DALDEVICEID_SATA_APU            0x020000FB
#define DALDEVICEID_LPASS_MPU           0x020000FC
#define DALDEVICEID_LPASS_INT_MPU       0x020000FD
#define DALDEVICEID_PCIE_APU            0x020000FE
#define DALDEVICEID_MMSS_FPB_APU        0x020000FF
#define DALDEVICEID_MMSS_IMEM_MPU       0x02000100
#define DALDEVICEID_SYSTEM_IMEM_MPU     0x02000101
#define DALDEVICEID_RPM_MSG_RAM_RPU     0x02000102
#define DALDEVICEID_SFPB_APU            0x02000103
#define DALDEVICEID_RPM_FPB_APU         0x02000104  
#define DALDEVICEID_CFPB0_APU           0x02000105
#define DALDEVICEID_CFPB1_APU           0x02000106  
#define DALDEVICEID_CFPB2_APU           0x02000107
#define DALDEVICEID_CFPB_SPLITTER_ACR   0x02000108
#define DALDEVICEID_SPS_SLAVEWAY0_APU   0x02000109
#define DALDEVICEID_SPS_SLAVEWAY1_APU   0x0200010A
#define DALDEVICEID_SPS_MTI_APU         0x0200010B
#define DALDEVICEID_SPS_PIPEMEMORY_MPU  0x0200010C

#define DALDEVICEID_RGPT                0x0200010D

#define DALDEVICEID_AXICFG              0x0200010E
#define DALDEVICEID_GSBI_CONTROL_BLOCK  0x0200010F

#define DALDEVICEID_SPIPD_DEVICE_1      0x02000110
#define DALDEVICEID_SPIPD_DEVICE_2      0x02000111
#define DALDEVICEID_SPIPD_DEVICE_3      0x02000112
#define DALDEVICEID_SPIPD_DEVICE_4      0x02000113
#define DALDEVICEID_SPIPD_DEVICE_5      0x02000114
#define DALDEVICEID_SPIPD_DEVICE_6      0x02000115
#define DALDEVICEID_SPIPD_DEVICE_7      0x02000116
#define DALDEVICEID_SPIPD_DEVICE_8      0x02000117
#define DALDEVICEID_SPIPD_DEVICE_9      0x02000118
#define DALDEVICEID_SPIPD_DEVICE_10     0x02000119
#define DALDEVICEID_SPIPD_DEVICE_11     0x0200011A
#define DALDEVICEID_SPIPD_DEVICE_12     0x0200011B
#define DALDEVICEID_GPIO                0x0200011C
#define DALDEVICEID_GPIOEXPANDER_4      0x0200011D
#define DALDEVICEID_GPIOEXPANDER_5      0x0200011E

#define DALDEVICEID_AXIPD_SYSTEM        0x0200011F
#define DALDEVICEID_AXIPD_MMSS          0x02000120
#define DALDEVICEID_AXIPD_APPSS         0x02000121

#define DALDEVICEID_ADIE_CTL2           0x02000122

#define DALDEVICEID_ADC_MODEM           0x02000123

#define DALDEVICEID_ADSADC_DEVICE_0     0x02000124
#define DALDEVICEID_ADSADC_DEVICE_1     0x02000125
#define DALDEVICEID_ADSADC_DEVICE_2     0x02000126

#define DALDEVICEID_ADIE_CTL3           0x02000127
#define DALDEVICEID_ADIE_CTL4           0x02000128
#define DALDEVICEID_ADIE_CTL5           0x02000129

#define DALDEVICEID_SPI_DEVICE_6        0x0200012A
#define DALDEVICEID_SPI_DEVICE_7        0x0200012B
#define DALDEVICEID_SPI_DEVICE_8        0x0200012C
#define DALDEVICEID_SPI_DEVICE_9        0x0200012D
#define DALDEVICEID_SPI_DEVICE_10       0x0200012E
#define DALDEVICEID_SPI_DEVICE_11       0x0200012F
#define DALDEVICEID_SPI_DEVICE_12       0x02000130

#define DALDEVICEID_GPT0                0x02000131

#define DALDEVICEID_UARTGSBI_DEVICE_6   0x02000132
#define DALDEVICEID_UARTGSBI_DEVICE_7   0x02000133
#define DALDEVICEID_UARTGSBI_DEVICE_8   0x02000134
#define DALDEVICEID_UARTGSBI_DEVICE_9   0x02000135
#define DALDEVICEID_UARTGSBI_DEVICE_10  0x02000136
#define DALDEVICEID_UARTGSBI_DEVICE_11  0x02000137
#define DALDEVICEID_UARTGSBI_DEVICE_12  0x02000138

#define DALDEVICEID_PLATFORMINFO        0x02000139

#define DALDEVICEID_SBI_FM_SSBI         0x0200013A
#define DALDEVICEID_SBI_BT_SSBI         0x0200013B

#define DALDEVICEID_SBI_GPS_SSBI        0x0200013C
#define DALDEVICEID_SLIMBUS_1           0x0200013D
#define DALDEVICEID_SLIMBUS_2           0x0200013E

#define DALDEVICEID_FLASH_DEVICE_2      0x0200013F
#define DALDEVICEID_FLASH_DEVICE_3      0x02000140

#define DALDEVICEID_SMMU                0x02000141

#define DALDEVICEID_CCU_TMR1            0x02000142

#define DALDEVICEID_ADIE_CODEC2         0x02000143

#define DALDEVICEID_CHIPPOWER           0x02000144

#define DALDEVICEID_CLOCK               0x02000145

#define DALDEVICEID_GCC_XPU             0x02000146
#define DALDEVICEID_TLMM_XPU            0x02000147
#define DALDEVICEID_CFPB_MASTER_XPU     0x02000148
#define DALDEVICEID_SMMU_FPB_XPU        0x02000149

#define DALDEVICEID_GPT1                0x0200014A

#define DALDEVICEID_QDI                 0x0200014B

#define DALDEVICEID_PMU_TIMER           0x0200014C
//remote test device (exception: out of seq dev id)
#define DALDEVICEID_REMOTETEST_DEVICE   0xDA1DA1DA

#endif //DALIDEVICEID_H
