
For custom module integration, users need to add the custom module information, in the 'custom_libs_cfg.json' present parallel to this readme.

Qualcomm config parser script, parses this json file based on the path specified to environment variable 'OEM_ROOT'.
For example, 
In Windows: set OEM_ROOT=Z:\oem
OR
In Linux:   export OEM_ROOT=/usr/mydir/oem

If OEM_ROOT is set as 'Z:\oem', then Qualcomm config parser script looks for 'Z:\oem\build\<CHIPSET>\custom_libs_cfg.json'
Eg: For msm8996 chipset: 'Z:\oem\build\msm8996\custom_libs_cfg.json'

Qualcomm config parser script outputs following:
1. Information needed for building libs, 
2. Adds env variable for building static libs,
3. Generates information needed for AMDB registration/loading, 
4. Source files for static and dynamic modules.
5. Generates XML for QACT,


Below is the snippet of custom module examples to be added in 'custom_libs_cfg.json' (It is empty currently):
Add the following between two brackets '[' and ']'. 
Please note that, we should not use ',' (comma) for the last module entry (see below). Otherwise, you will get compilation errors during json parsing.

NOTE:
-----
Please note that, for shared library generation, top-level scons file of a module need to enable 'USES_AVS_CUSTOM_SHARED_LIBS' as below:
(For static library generation, this flag needs to be disabled i.e., commented)
For example, uncomment the below line from: 'Z:\oem\audio\dummy_ecns\build\dummy_ecns.scons':
env.Replace(USES_AVS_CUSTOM_SHARED_LIBS = 'yes')



-------------------------------------------------------
EXAMPLE JSON FILE CHANGES FOR STATIC LIBRARY GENERATION:
-------------------------------------------------------
[
  {
      "lib_name"      : "capi_v2_gain",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "STATIC_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x11111111",
         "ID_2"               : "0",
         "tag"                : "capi_v2_gain",
         "qact_name"          : "VOICE_MODULE_CAPI_V2_GAIN_TX"
      }
  },
  {
      "lib_name"      : "capi_v2_gain",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "STATIC_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x22222222",
         "ID_2"               : "0",
         "tag"                : "capi_v2_gain",
         "qact_name"          : "VOICE_MODULE_CAPI_V2_GAIN_RX"
      }
  },  
  {
      "lib_name"      : "capi_v2_dummy_ecns",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "STATIC_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x10027060",
         "ID_2"               : "0",
         "tag"                : "capi_v2_custom_dummy_sm_ecns",
         "qact_name"          : "VOICE_MODULE_DUMMY_SM_ECNS"
      }
  },
  {
        "lib_name"      : "capi_v2_dummy_ecns",
        "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
        "build"         : "STATIC_BUILD_NO_STRIP",      
        "lib_major_ver" : 1,
        "lib_minor_ver" : 0,
        "is_qurt_used"  : false,
        "depends_on"    : [],
        "amdb_info"   : 
        {
           "preload"            : true,
           "rev_num"            : 1,
           "itype"              : "capi_v2",
           "mtype"              : "generic",  
           "ID_1"               : "0x10027061",
           "ID_2"               : "0",
           "tag"                : "capi_v2_custom_dummy_dm_ecns",
           "qact_name"          : "VOICE_MODULE_DUMMY_DM_ECNS"
        }
  },
  {
        "lib_name"      : "capi_v2_dummy_ecns",
        "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
        "build"         : "STATIC_BUILD_NO_STRIP",      
        "lib_major_ver" : 1,
        "lib_minor_ver" : 0,
        "is_qurt_used"  : false,
        "depends_on"    : [],
        "amdb_info"   : 
        {
           "preload"            : true,
           "rev_num"            : 1,
           "itype"              : "capi_v2",
           "mtype"              : "generic",  
           "ID_1"               : "0x10027062",
           "ID_2"               : "0",
           "tag"                : "capi_v2_custom_dummy_qm_ecns",
           "qact_name"          : "VOICE_MODULE_DUMMY_QM_ECNS"
        }
  },
  {
      "lib_name"      : "capi_v2_decimate",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "STATIC_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x00012D16",
         "ID_2"               : "0",
         "tag"                : "capi_v2_decimate",
         "qact_name"          : "CAPI_V2_DECIMATE"
      }
  },
  {
      "lib_name"      : "capi_v2_voice_imc_tx",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "STATIC_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x10012D18",
         "ID_2"               : "0",
         "tag"                : "capi_v2_voice_imc_tx",
         "qact_name"          : "VOICE_IMC_TX"
      }
  },
  {
      "lib_name"      : "capi_v2_voice_imc_rx",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "STATIC_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x10012D17",
         "ID_2"               : "0",
         "tag"                : "capi_v2_voice_imc_rx",
         "qact_name"          : "VOICE_IMC_RX"
      }
   }  
]




-------------------------------------------------------
EXAMPLE JSON FILE CHANGES FOR SHARED LIBRARY GENERATION:
-------------------------------------------------------
[
  {
      "lib_name"      : "capi_v2_gain",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "SHARED_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x11111111",
         "ID_2"               : "0",
         "tag"                : "capi_v2_gain",
         "qact_name"          : "VOICE_MODULE_CAPI_V2_GAIN_TX"
      }
  },
  {
      "lib_name"      : "capi_v2_gain",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "SHARED_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x22222222",
         "ID_2"               : "0",
         "tag"                : "capi_v2_gain",
         "qact_name"          : "VOICE_MODULE_CAPI_V2_GAIN_RX"
      }
  },  
  {
      "lib_name"      : "capi_v2_dummy_ecns",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "SHARED_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x10027060",
         "ID_2"               : "0",
         "tag"                : "capi_v2_custom_dummy_sm_ecns",
         "qact_name"          : "VOICE_MODULE_DUMMY_SM_ECNS"
      }
  },
  {
        "lib_name"      : "capi_v2_dummy_ecns",
        "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
        "build"         : "SHARED_BUILD_NO_STRIP",      
        "lib_major_ver" : 1,
        "lib_minor_ver" : 0,
        "is_qurt_used"  : false,
        "depends_on"    : [],
        "amdb_info"   : 
        {
           "preload"            : true,
           "rev_num"            : 1,
           "itype"              : "capi_v2",
           "mtype"              : "generic",  
           "ID_1"               : "0x10027061",
           "ID_2"               : "0",
           "tag"                : "capi_v2_custom_dummy_dm_ecns",
           "qact_name"          : "VOICE_MODULE_DUMMY_DM_ECNS"
        }
  },
  {
        "lib_name"      : "capi_v2_dummy_ecns",
        "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
        "build"         : "SHARED_BUILD_NO_STRIP",      
        "lib_major_ver" : 1,
        "lib_minor_ver" : 0,
        "is_qurt_used"  : false,
        "depends_on"    : [],
        "amdb_info"   : 
        {
           "preload"            : true,
           "rev_num"            : 1,
           "itype"              : "capi_v2",
           "mtype"              : "generic",  
           "ID_1"               : "0x10027062",
           "ID_2"               : "0",
           "tag"                : "capi_v2_custom_dummy_qm_ecns",
           "qact_name"          : "VOICE_MODULE_DUMMY_QM_ECNS"
        }
  },
  {
      "lib_name"      : "capi_v2_decimate",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "SHARED_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x00012D16",
         "ID_2"               : "0",
         "tag"                : "capi_v2_decimate",
         "qact_name"          : "CAPI_V2_DECIMATE"
      }
  },
  {
      "lib_name"      : "capi_v2_voice_imc_tx",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "SHARED_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x10012D18",
         "ID_2"               : "0",
         "tag"                : "capi_v2_voice_imc_tx",
         "qact_name"          : "VOICE_IMC_TX"
      }
  },
  {
      "lib_name"      : "capi_v2_voice_imc_rx",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "SHARED_BUILD_NO_STRIP",      
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : true,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x10012D17",
         "ID_2"               : "0",
         "tag"                : "capi_v2_voice_imc_rx",
         "qact_name"          : "VOICE_IMC_RX"
      }
   }  
]
