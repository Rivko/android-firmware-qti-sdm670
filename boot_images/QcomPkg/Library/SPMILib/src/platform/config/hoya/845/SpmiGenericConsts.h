#define HWIO_PMIC_ARB_PRIORITIESn_MAXn                                          9
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_MAXn                                        6
#define HWIO_PMIC_ARB_PVCn_ADDRm_MAXm                                           3
#define HWIO_PMIC_ARB_CHNLn_CMD_MAXn                                          511
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_MAXq                                       6
#define HWIO_SPMI_GENI_CFG_RAMn_MAXn                                          255
#define HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn                                  15
#define HWIO_PMIC_ARB_VRM_S_ADDRw_MAXw                                        255
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_MAXn                                       3

#define INIT_SPMI_DEV_GENERICS {\
   .mNumChannels                                                            = (HWIO_PMIC_ARB_CHNLn_CMD_MAXn + 1),\
   .mNumPortPriorities                                                      = (HWIO_PMIC_ARB_PRIORITIESn_MAXn + 1),\
   .mNumPvcPorts                                                            = (HWIO_PMIC_ARB_PVC_PORTn_CTL_MAXn + 1),\
   .mNumPvcPortPPIDs                                                        = (HWIO_PMIC_ARB_PVCn_ADDRm_MAXm + 1),\
   .mNumMasters                                                             = (HWIO_PMIC_ARB_OBSq_CHNLn_CMD_MAXq + 1),\
   .mNumProgRamRegs                                                         = (HWIO_SPMI_GENI_CFG_RAMn_MAXn + 1),\
   .mNumPicAccStatusRegs                                                    = (HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn + 1),\
   .mNumVrmIds                                                              = (HWIO_PMIC_ARB_VRM_S_ADDRw_MAXw + 1),\
   .mNumMgpiPorts                                                           = (HWIO_PMIC_MGPI_TRIG_CONFIGn_MAXn + 1),\
}
