/*
==============================================================================

FILE:         ClockPropData.c

DESCRIPTION:
  This file contains the clock driver Properties data.

==============================================================================

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/Clock/core/ClockPropData.c#3 $

==============================================================================
            Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "Drivers/ClockDxe/ClockBSP.h"
#include "../HALclkLib/HALclkHWIO.h"
#include "HALhwio.h"


/*=========================================================================
      Externs
==========================================================================*/


/*=========================================================================
      Data Declarations
==========================================================================*/

/*
 * Clock Log Default Configuration.
 *
 */
const ClockLogType ClockLogDefaultConfig[] =
{
  {
    /* .nLogSize        = */ 4096,
    /* .nGlobalLogFlags = */ 0x90ff
  }
};

/*
 * List of clocks allowed to be suppressible.
 */
ClockNameListType ClockSuppressibleList[] =
{
  // Serial Debugger:

  // USB 2.0 Debugger:
  { "gcc_usb_phy_cfg_ahb2phy_clk"         },

  // USB 3.0 Debugger:
  { "gcc_aggre_usb3_prim_axi_clk"         },
  { "gcc_aggre_usb3_sec_axi_clk"          },
  { "gcc_cfg_noc_usb3_prim_axi_clk"       },
  { "gcc_cfg_noc_usb3_sec_axi_clk"        },
  { "gcc_usb30_prim_master_clk"           },
  { "gcc_usb30_prim_mock_utmi_clk"        },
  { "gcc_usb30_prim_sleep_clk"            },
  { "gcc_usb30_sec_master_clk"            },
  { "gcc_usb30_sec_mock_utmi_clk"         },
  { "gcc_usb30_sec_sleep_clk"             },
  { "gcc_usb3_prim_phy_aux_clk"           },
  { "gcc_usb3_prim_phy_pipe_clk"          },
  { "gcc_usb3_sec_phy_aux_clk"            },
  { "gcc_usb3_sec_phy_pipe_clk"           },
  { "gcc_usb_phy_cfg_ahb2phy_clk"         },

  // WinDbg UART:
  { "gcc_qupv3_wrap_1_m_ahb_clk"          },
  { "gcc_qupv3_wrap_1_s_ahb_clk"          },
  { "gcc_qupv3_wrap1_core_2x_clk"         },
  { "gcc_qupv3_wrap1_core_clk"            },
  { "gcc_qupv3_wrap1_s1_clk"              },

  { NULL }
};

/*
 * List of clocks allowed to be always on.
 */
ClockNameListType ClockAlwaysOnList[] =
{
  // USB 3.0 Debugger:
  { "gcc_aggre_usb3_prim_axi_clk"         },
  { "gcc_aggre_usb3_sec_axi_clk"          },
  { "gcc_cfg_noc_usb3_prim_axi_clk"       },
  { "gcc_cfg_noc_usb3_sec_axi_clk"        },
  { "gcc_usb30_prim_master_clk"           },
  { "gcc_usb30_prim_mock_utmi_clk"        },
  { "gcc_usb30_prim_sleep_clk"            },
  { "gcc_usb30_sec_master_clk"            },
  { "gcc_usb30_sec_mock_utmi_clk"         },
  { "gcc_usb30_sec_sleep_clk"             },
  { "gcc_usb3_prim_phy_aux_clk"           },
  { "gcc_usb3_prim_phy_pipe_clk"          },
  { "gcc_usb3_sec_phy_aux_clk"            },
  { "gcc_usb3_sec_phy_pipe_clk"           },
  { "gcc_usb_phy_cfg_ahb2phy_clk"         },

  // WinDbg UART:
  { "gcc_qupv3_wrap_1_m_ahb_clk"          },
  { "gcc_qupv3_wrap_1_s_ahb_clk"          },
  { "gcc_qupv3_wrap1_core_2x_clk"         },
  { "gcc_qupv3_wrap1_core_clk"            },
  { "gcc_qupv3_wrap1_s1_clk"              },

  { NULL }
};

/*
 * List of clocks allowed to be reference counter disabled.
 */
ClockNameListType ClockRefCountSuspendedList[] =
{
  // USB 2.0

  // USB 3.0
  { "gcc_usb30_prim_mock_utmi_clk"        },
  { "gcc_usb30_sec_mock_utmi_clk"         },

  { NULL }
};

/*
 * List of clocks allowed to be configured with the
 * FORCE_MEM_CORE and FORCE_MEM_PERIPH parameters.
 */
ClockNameListType ClockForceMemCoreAndPeriphList[] =
{
  { "cam_cc_bps_clk"                      },
  { "cam_cc_camnoc_axi_clk"               },
  { "cam_cc_fd_core_clk"                  },
  { "cam_cc_icp_clk"                      },
  { "cam_cc_ife_0_clk"                    },
  { "cam_cc_ife_0_csid_clk"               },
  { "cam_cc_ife_1_clk"                    },
  { "cam_cc_ife_1_csid_clk"               },
  { "cam_cc_ife_lite_clk"                 },
  { "cam_cc_ife_lite_csid_clk"            },
  { "cam_cc_ipe_0_clk"                    },
  { "cam_cc_ipe_1_clk"                    },
  { "cam_cc_jpeg_clk"                     },
  { "cam_cc_lrme_clk"                     },
  { "disp_cc_mdss_axi_clk"                },
  { "disp_cc_mdss_mdp_clk"                },
  { "disp_cc_mdss_mdp_lut_clk"            },
  { "disp_cc_mdss_rot_clk"                },
  { "gcc_aggre_noc_pcie_tbu_clk"          },
  { "gcc_boot_rom_ahb_clk"                },
  { "gcc_ce1_clk"                         },
  { "gcc_gpu_memnoc_gfx_clk"              },
  { "gcc_pcie_0_mstr_axi_clk"             },
  { "gcc_pcie_0_pipe_clk"                 },
  { "gcc_pcie_0_slv_axi_clk"              },
  { "gcc_pcie_1_mstr_axi_clk"             },
  { "gcc_pcie_1_pipe_clk"                 },
  { "gcc_pcie_1_slv_axi_clk"              },
  { "gcc_qupv3_wrap0_core_2x_clk"         },
  { "gcc_qupv3_wrap0_core_clk"            },
  { "gcc_qupv3_wrap0_s0_clk"              },
  { "gcc_qupv3_wrap0_s1_clk"              },
  { "gcc_qupv3_wrap0_s2_clk"              },
  { "gcc_qupv3_wrap0_s3_clk"              },
  { "gcc_qupv3_wrap0_s4_clk"              },
  { "gcc_qupv3_wrap0_s5_clk"              },
  { "gcc_qupv3_wrap0_s6_clk"              },
  { "gcc_qupv3_wrap0_s7_clk"              },
  { "gcc_qupv3_wrap1_core_2x_clk"         },
  { "gcc_qupv3_wrap1_core_clk"            },
  { "gcc_qupv3_wrap1_s0_clk"              },
  { "gcc_qupv3_wrap1_s1_clk"              },
  { "gcc_qupv3_wrap1_s2_clk"              },
  { "gcc_qupv3_wrap1_s3_clk"              },
  { "gcc_qupv3_wrap1_s4_clk"              },
  { "gcc_qupv3_wrap1_s5_clk"              },
  { "gcc_qupv3_wrap1_s6_clk"              },
  { "gcc_qupv3_wrap1_s7_clk"              },
  { "gcc_sdcc2_apps_clk"                  },
  { "gcc_sdcc4_apps_clk"                  },
  { "gcc_tsif_ahb_clk"                    },
  { "gcc_ufs_card_axi_clk"                },
  { "gcc_ufs_card_ice_core_clk"           },
  { "gcc_ufs_card_unipro_core_clk"        },
  { "gcc_ufs_phy_axi_clk"                 },
  { "gcc_ufs_phy_ice_core_clk"            },
  { "gcc_ufs_phy_unipro_core_clk"         },
  { "gcc_usb30_prim_master_clk"           },
  { "gcc_usb30_sec_master_clk"            },
  { "gpu_cc_cx_gmu_clk"                   },
  { "gpu_cc_gx_gfx3d_clk"                 },
  { "video_cc_vcodec0_axi_clk"            },
  { "video_cc_vcodec0_core_clk"           },
  { "video_cc_vcodec1_axi_clk"            },
  { "video_cc_vcodec1_core_clk"           },
  { "video_cc_venus_ctl_axi_clk"          },
  { "video_cc_venus_ctl_core_clk"         },

  { NULL }
};

/*
 * List of clocks needed to be enabled prior to running the BIST.
 */
ClockNameListType ClockBistDependencies[] =
{
  { "gcc_camera_xo_clk"                   },
  { "gcc_disp_xo_clk"                     },
  { "gcc_video_xo_clk"                    },

  { NULL }
};

/*
 * List of clocks anticipated to fail the BIST.
 */
ClockNameListType ClockBistExceptions[] =
{
  { "cam_cc_fd_core_clk"                  },  // IsClockOn: status mismatch
  { "cam_cc_icp_clk"                      },  // IsClockOn: status mismatch
  { "cam_cc_ife_lite_clk"                 },  // IsClockOn: status mismatch
  { "cam_cc_ife_lite_csid_clk"            },  // IsClockOn: status mismatch
  { "cam_cc_lrme_clk"                     },  // IsClockOn: status mismatch
  { "cam_cc_pll_test_clk"                 },  // IsClockOn: status mismatch
  { "disp_cc_mdss_byte0_intf_clk"         },  // Internally divided by 2
  { "disp_cc_mdss_byte1_intf_clk"         },  // Internally divided by 2
  { "disp_cc_mdss_dp_crypto_clk"          },  // IsClockEnabled: 0
  { "disp_cc_mdss_spdm_dp_crypto_clk"     },  // IsClockOn: status mismatch
  { "disp_cc_mdss_dp_link_intf_clk"       },  // Internally divided by 4
  { "disp_cc_mdss_spdm_dp_pixel1_clk"     },  // Internally divided by 4
  { "disp_cc_mdss_spdm_dp_pixel_clk"      },  // Internally divided by 4
  { "disp_cc_mdss_mdp_clk"                },  // IsClockOn: status mismatch
  { "disp_cc_mdss_mdp_lut_clk"            },  // IsClockOn: status mismatch
  { "disp_cc_mdss_spdm_mdp_clk"           },  // Internally divided by 3
  { "disp_cc_mdss_spdm_pclk0_clk"         },  // Internally divided by 4
  { "disp_cc_mdss_spdm_pclk1_clk"         },  // Internally divided by 4
  { "disp_cc_mdss_rot_clk"                },  // IsClockOn: status mismatch
  { "disp_cc_mdss_spdm_rot_clk"           },  // Internally divided by 3
  { "gcc_gpll0_ufs_card_rx_symbol_0_clk"  },  // IsClockEnabled: 0
  { "gcc_gpll0_ufs_card_rx_symbol_1_clk"  },  // IsClockEnabled: 0
  { "gcc_gpll0_ufs_card_tx_symbol_0_clk"  },  // IsClockEnabled: 0
  { "gcc_gpll0_ufs_phy_rx_symbol_0_clk"   },  // IsClockEnabled: 0
  { "gcc_gpll0_ufs_phy_rx_symbol_1_clk"   },  // IsClockEnabled: 0
  { "gcc_gpll0_ufs_phy_tx_symbol_0_clk"   },  // IsClockEnabled: 0
  { "apcs_gold_sysleaf_clk"               },  // CPU clock
  { "apcs_l3_sysleaf_clk"                 },  // CPU clock
  { "apcs_silver_sysleaf_clk"             },  // CPU clock
  { "disp_cc_mdss_spdm_debug_clk"         },  // IsClockOn: status mismatch
  { "gcc_pcie_0_link_down_bcr"            },  // reset only
  { "gcc_pcie_0_nocsr_com_phy_bcr"        },  // reset only
  { "gcc_pcie_0_phy_bcr"                  },  // reset only
  { "gcc_pcie_0_phy_nocsr_com_phy_bcr"    },  // reset only
  { "gcc_pcie_1_link_down_bcr"            },  // reset only
  { "gcc_pcie_1_nocsr_com_phy_bcr"        },  // reset only
  { "gcc_pcie_1_phy_bcr"                  },  // reset only
  { "gcc_pcie_1_phy_nocsr_com_phy_bcr"    },  // reset only
  { "gcc_pcie_phy_cfg_ahb_bcr"            },  // reset only
  { "gcc_pcie_phy_com_bcr"                },  // reset only
  { "gcc_qusb2phy_prim_bcr"               },  // reset only
  { "gcc_qusb2phy_sec_bcr"                },  // reset only
  { "gcc_usb3_dp_phy_prim_bcr"            },  // reset only
  { "gcc_usb3_dp_phy_sec_bcr"             },  // reset only
  { "gcc_usb3_phy_prim_bcr"               },  // reset only
  { "gcc_usb3_phy_sec_bcr"                },  // reset only
  { "gcc_usb3phy_phy_prim_bcr"            },  // reset only
  { "gcc_usb3phy_phy_sec_bcr"             },  // reset only
  { "video_cc_debug_clk"                  },  // IsClockOn: status mismatch
  { "gcc_ce1_clk"                         },  // IsClockOn: status mismatch
  { "cam_cc_bps_axi_clk"                  },  // IsClockOn: status mismatch
  { "cam_cc_camnoc_axi_clk"               },  // IsClockOn: status mismatch
  { "cam_cc_ife_0_axi_clk"                },  // IsClockOn: status mismatch
  { "cam_cc_ife_1_axi_clk"                },  // IsClockOn: status mismatch
  { "cam_cc_ipe_0_axi_clk"                },  // IsClockOn: status mismatch
  { "cam_cc_ipe_1_axi_clk"                },  // IsClockOn: status mismatch
  { "disp_cc_mdss_axi_clk"                },  // IsClockOn: status mismatch
  { "gcc_camera_axi_clk"                  },  // IsClockOn: status mismatch
  { "gcc_disp_axi_clk"                    },  // IsClockOn: status mismatch
  { "gcc_video_axi_clk"                   },  // IsClockOn: status mismatch
  { "video_cc_vcodec0_axi_clk"            },  // IsClockOn: status mismatch
  { "video_cc_vcodec1_axi_clk"            },  // IsClockOn: status mismatch
  { "video_cc_venus_ctl_axi_clk"          },  // IsClockOn: status mismatch
  { "gcc_pcie_0_pipe_clk"                 },  // IsClockOn: status mismatch
  { "gcc_pcie_1_pipe_clk"                 },  // IsClockOn: status mismatch
  { "gcc_ufs_card_rx_symbol_0_clk"        },  // IsClockOn: status mismatch
  { "gcc_ufs_card_rx_symbol_1_clk"        },  // IsClockOn: status mismatch
  { "gcc_ufs_card_tx_symbol_0_clk"        },  // IsClockOn: status mismatch
  { "gpu_cc_gx_vsense_clk"                },  // IsClockEnabled: 0
  { "gpu_cc_cx_gfx3d_slv_clk"             },  // IsClockEnabled: 0
  { "gpu_cc_spdm_gx_gfx3d_div_clk"        },  // Internally divided by 4
  { "gpu_cc_pll_test_clk"                 },  // IsClockOn: status mismatch
  { "video_cc_venus_ctl_core_clk"         },  // Frequency error 20000000 Hz exceeds error margin of 40000 Hz

 {"cam_cc_ipe_1_ahb_clk"}, // gdsc not on
 {"cam_cc_ipe_1_areg_clk"}, // gdsc not on
 {"cam_cc_ipe_1_axi_clk"}, // gdsc not on
 {"cam_cc_ipe_1_clk"}, // gdsc not on
 {"cam_cc_spdm_ipe_1_clk"}, // gdsc not on
 
 {"gcc_ufs_phy_rx_symbol_0_clk"}, // intermidiate Muxes present so need to config them
 {"gcc_ufs_phy_tx_symbol_0_clk"}, // intermidiate Muxes present so need to config them
 {"gcc_usb3_prim_phy_pipe_clk"}, // Mux i/ps present so need to selec and run
 
 {"gpu_cc_cx_gfx3d_clk"}, //Not turning on, bcz it doesn't have Enable bit only ARES available
 {"gpu_cc_gx_gfx3d_clk"}, //it can't be scaled more than 430Mhz
 
 {"gcc_pcie_0_clkref_en"}, //not available in sdm670
 {"gcc_pcie_1_clkref_en"}, //not available in sdm670
 {"gcc_ufs_card_clkref_en"}, //not available in sdm670
 {"gcc_usb3_sec_clkref_en"}, //not available in sdm670

  { NULL }
};

/*
 * List of clocks excluded from the BIST.
 */
ClockNameListType ClockExcludeFromBist[] =
{

  { NULL }
};

/*
 * List of clocks allowed to enable/disable clock branch only control.
 */
ClockNameListType ClockBranchOnlyRawControlList[] =
{
  { "disp_cc_mdss_byte0_clk"              },
  { "disp_cc_mdss_byte0_intf_clk"         },
  { "disp_cc_mdss_byte1_clk"              },
  { "disp_cc_mdss_byte1_intf_clk"         },
  { "disp_cc_mdss_dp_crypto_clk"          },
  { "disp_cc_mdss_dp_link_clk"            },
  { "disp_cc_mdss_dp_link_intf_clk"       },
  { "disp_cc_mdss_dp_pixel1_clk"          },
  { "disp_cc_mdss_dp_pixel_clk"           },
  { "disp_cc_mdss_esc0_clk"               },
  { "disp_cc_mdss_esc1_clk"               },
  { "disp_cc_mdss_pclk0_clk"              },
  { "disp_cc_mdss_pclk1_clk"              },
  { "disp_cc_mdss_spdm_dp_pixel1_clk"     },
  { "disp_cc_mdss_spdm_dp_pixel_clk"      },
  { "disp_cc_mdss_spdm_pclk0_clk"         },
  { "disp_cc_mdss_spdm_pclk1_clk"         },

  { NULL }
};

/*
 * Clock property table for use in non-DAL environments.
 *
 * NOTE:
 * Please add clock under proper subsystem and place it in order (i.e.,
 * each sub-list is sorted based on the string clock name).
 *
 */
const ClockPropertyType Clock_aProperties[] =
{
  /*
   * Clock Log Defaults.
   */
  { "ClockLogDefaults",                   ClockLogDefaultConfig },

  /*
   * Clocks allowed to be suppressible.
   */
  { "SuppressibleList",                   ClockSuppressibleList },

  /*
   * Clocks allowed to be always on.
   */
  { "ClockAlwaysOnList",                  ClockAlwaysOnList },

  /*
   * Clocks allowed to be reference counter disabled.
   */
  { "RefCountSuspendedList",              ClockRefCountSuspendedList },

  /*
   * Clocks allowed to be configured with the
   * FORCE_MEM_CORE and FORCE_MEM_PERIPH parameters.
   */
  { "ForceMemCoreAndPeriphList",          ClockForceMemCoreAndPeriphList },

  /*
   * List of clocks needed to be enabled prior to running the BIST.
   */
  { "BistDependencies",                   ClockBistDependencies },

  /*
   * Clock BIST Exceptions.
   */
  { "BISTExceptions",                     ClockBistExceptions },

  /*
   * List of clocks excluded from BIST.
   */
  { "ExcludeFromBist",                    ClockExcludeFromBist },

  /*
   * List of clocks allowed to enable/disable clock branch only control.
   */
  { "BranchOnlyRawControlList",           ClockBranchOnlyRawControlList },

  { NULL, NULL }
};

