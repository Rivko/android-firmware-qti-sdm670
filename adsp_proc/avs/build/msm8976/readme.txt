#######################################################################
Readme for the JSON files:
#######################################################################
This directory contains JSON files. 
The file names follow the pattern: <below given name>_<chipset>.json
   'afe_libs_cfg',           - Used for AFE libraries
   'aud_dec_libs_config',    - Used for Audio Decoder libraries
   'aud_enc_libs_config',    - Used for Audio Encoder libraries
   'aud_pp_libs_config',     - Used for PP libraries
   'fmwk_libs_cfg'           - Used for framework libraries
   'listen_libs_cfg'         - Used for LSM/listen libraries
   'voc_libs_config'         - Used for Voice modules
   'custom_libs_cfg'         - Used by customers for their libraries.

Eg. afe_libs_cfg.json

* APPI/CAPI/CAPI_V2 modules can be mentioned.
* If there are no modules, then json file can contain empty list as in: []
* General JSON format is as follows: list of dicts: [{},{},{}]
   Where each dict contains key-value pairs. Key has to be double quoted.
* Note that unlike python there cannot be extra comma after the last argument as in [{},{},{},] => not allowed.
* Json validators are available online for a quick detection of formatting errors.
* The JSON file cannot contain any comments. But a "comment" key can be added.
* Arguments of JSON file are described below:
      "lib_name"      : Name of the library in double quotes (same as LIBNAME is module level scons). 
                        No file extension (Eg. .so) required. This is a required argument. 
                        Eg. "ComboMp3DecoderModule",
      "comment"       : A comment ignored by the parser. 
      "build"         : Type of build. 
                        Typically, double quoted string. This is an optional argument. Default: 'STATIC_BUILD_NO_STRIP'
                        Also possible to be Dictionary: {env flag: build-type}
                        See Build section below.
                        See 'Backward compatibility' below.
      "lib_major_ver" : Major version of the library in decimal. 
                        Not double quoted. This is a required argument for shared libs otherwise optional , 
      "lib_minor_ver" : Minor version of the library in decimal. 
                        Not double quoted. This is a required argument for shared libs otherwise optional, 
      "is_qurt_used"  : false or true depending on whether Qurt is used by the library. 
                        This flag is not affected by use of qurt_elite. 
                        Not double quoted. This is a required argument for shared libs except 
                        when build is SHARED_NO_BUILD.
      "depends_on"    : List of libraries on which this lib depends. 
                        This is an optional argument. 
                        Eg. if this lib depends on abc.so and xyz.so, then entry is: ["abc", "xyz"]. 
                        abc and xyz are also assumed to be mentioned in json.
                        Not needed when build=SHARED_NO_BUILD or for static libs.
      "amdb_info"     : Dict containing info about registering/loading modules to AMDB. 
                        If this module is to be registered to AMDB (whether static or shared), then this section is necessary. 
                        This is an optional argument. 
                        However, for modules to be put in AMDB, this is needed even when build=SHARED_NO_BUILD.
                        Only libraries with amdb_info appear in QACT.
                        {
                          "rev_num"    : Natural Number. If a module has multiple .so files associated 
                                         (due to backward compatibility), this helps prioritize (highest rev used). 
                                         starts with 1. This is an optional argument (default:1). Not double quoted. ,  
                          "itype"      : type of of interface ("virtual_stub", "capi_v2", "appi", "capi"). double quoted. 
                                         Eg. "appi". 
                                         Only one itype can be present for a module-id identified by key (mtype, id1, id2). 
                                         virtual_stub - need to be used if real-implementation of the module should not be used. Module will be bypassed in the topology.
                                                        Has the same effect as build='STATIC_BUILD_VIRTUAL_STUB', but allows module level control instead of library level.
                                                        (A library can have multiple modules)
                          "preload"    : true or false. true if the module must be preloaded (applicable to shared lib). 
                                         This is an optional argument. Default: false.
                                         Not double quoted. ,  
                          "mtype"      : "generic", "decoder", "encoder", "converter", "packetizer".
                                         This is a required argument,
                          "ID_1"       : Module ID/Format ID depending on mtype. 
                                         Quoted hexadecimal number. This is a required argument. 
                                         Eg. "0x10BE9",
                          "ID_2"       : Format ID for mtype=converter. 
                                         Quoted hexadecimal number. Eg. "0x10BE9" or "0",
                                         This is an optional argument. default "0"
                          "tag"        : Quoted tag for the module. This is a required argument. 
                                        CAPI V2: Eg. "combo_mp3_dec_capi_v2",  
                                                 <tag>+"_get_static_properties" and <tag>+"_init" give the entry-point function names.
                                        APPI:    Eg. "appi_mp3"
                                                 <tag>+"_getsize" and <tag>+"_init" give the entry-point function names
                                        CAPI:    Eg. "capi_mp3"
                                                 <tag>+"_getsize" and <tag>+"_ctor" give the entry-point function names
                          "is_private" : true or false. optional. by-default modules are public. 
                                         Private modules are those that should not be seen on QACT. Private modules must be static.
                          "qact_name"  : Name that will appear on QACT. Mandatory.
                          "prereq_mids": Eg. [{"mtype":"generic", "ID_1":"0x123", "ID_2":"0x0"}, {"mtype":"generic", "ID_1":"0x222", "ID_2":"0x0"}]
                                         prerequisite module-ids. Optional argument. Array of dicts.
                                         Eg. Module-A lists module-B as prerequiste. Then whenever module 'A' is 'required', module 'B' is also 'required'. Eg. DS1 requires resampler, chmixer as prereq.
                                         There can be <= 2 prereq-mids (can be easily changed).
                                         Use cases: a) DS1. b) voice have a use case where a m-id needs another m-id as pre-req. The second m-id is huge in size. So we cannot always load the second. We need to load the second only if first is loaded. c) We don't want customers to accidentally mark a prereq module like resampler as 'not required'.
                        }
      
* Backward compatibility: the items that have "amdb_info" for shared libs need to satisfy backward compatibility. 
	  Eg. in ADSP release#1, let's assume a module M is in file a.so. In release#2, let's say this module is moved to b.so.
	  If an old HLOS is used with the new DSP, then the new DSP should be able to work with a.so that's present in old HLOS.
	  To support this, build must contain old revision info as well. This in turn means that JSON must maintain multiple entries for this module.
	  One with "build"="SHARED_NO_BUILD" are for this purpose. They are not built but build will carry info about them.
	  The scenario also occurs when major version of a library changes as file name contains major version.
	  The order of loading is: latest one (one with highest rev_num) is loaded. Only if it fails, will the next module be loaded, and so on.
	  For .so files not loaded to AMDB, SHARED_NO_BUILD is not useful except to disable building them.
	  However, care must be taken to make sure that dependencies are always built. 
     

* when to increase minor version: any backward compatible change
* when to increase major version: any non-backward compatible change
* when to increase rev_num: for a given module-id whenever a new file gets associated (including due to change in major version), 
	old entry for the module will move to SHARED_NO_BUILD. But that entry cannot be deleted. If deleted, then in old HLOS new DSP will not work. 
    The new entry carries incremented number & "build" changes to something not equal to "SHARED_NO_BUILD". 	  
    If a module or library is a .so in release#1, and it is made virtual-stub in release#2, then since second release doesnot need to know .so file name, rev_num need not be incremented.
    If a module or library is a virtual-stub in release#1, and it is made a .so in release#2, then since second release knows .so file name, rev_num need not be incremented.
    If a module changes from a.so -> virtual-stub -> b.so, then between a.so and b.so rev_num need to increment.

* Build:
   >> This flag decides whether a library is built as static or shared and whether as regular source, stub, strip or strip-only.
      The interpretation of the value depends on environment variables.
   >> If 'GEN_SHARED_LIBS' is not defined, then all libraries are statically built, including those marked with SHARED_BUILD*.
      Exceptions are: SHARED_NO_BUILD and SHARED_BUILD* with no amdb_info. See below for details.
   >> When env.get(key) is True for key='AVS_TEST' (SIM build) or key='COSIM_PROFILING_TEST'
      all libraries are built as regular sources even if they are mentioned as STRIP.
   >> When 'build' is a string the value it takes are:
              "STATIC_BUILD_STRIP":  build, strip, obfuscate.
              "STATIC_BUILD_NO_STRIP": build as regular. (default)
              "STATIC_BUILD_STRIP_ONLY": build, strip.
              "STATIC_BUILD_STUB": stub-out. stub-source is compiled and linked to the image if there are references.
                                   "STATIC_BUILD_STUB" results in build-error if 'amdb_info' is specified. 
                                   For libraries having 'amdb_info', 'STATIC_BUILD_VIRTUAL_STUB' is to be used.
              "STATIC_BUILD_VIRTUAL_STUB": virtually-stubs out all modules in a library. 
                                   Builds stub-source. Unless direct references to the symbols exist in the image, stub-source will not get linked.
                                   If a library has 'amdb_info' then AMDB marks the the module as (virtual) stub, no matter whether the ACDB file mentions the module as 'required' or 'not-required'
                                   Virtual-stubs appear in QACT XML. But built-type will be virtual_stub instead of static or dynamic.
              "SHARED_BUILD_STRIP": build, strip, obfuscate. 
                 If GEN_SHARED_LIBS is not defined then maps to STATIC_BUILD_STRIP if amdb_info is present. 
              "SHARED_BUILD_NO_STRIP": build as regular. 
                 If GEN_SHARED_LIBS is not defined then maps to STATIC_BUILD_NO_STRIP if amdb_info is present..
              "SHARED_NO_BUILD": don't build but keep amdb_info if any. Appears in QACT.
                 Even when GEN_SHARED_LIBS is not defined, SHARED_NO_BUILD stays the same.
   >> Advanced: Typically, for different targets or for different build flavours the json files 
      have to be duplicated and customized. However, for minor customizations, key-value pair can be passed.
      >> In this case, 'build' is a dictionary of the form: {env flag1: build-type1, env flag2: build-type2}
      >> Eg. "Build":{"DEFAULT": "SHARED_BUILD_STRIP", "USES_MDM9X35_VOCODER" : "STATIC_BUILD_STUB", "USES_1GB_VARIANT": "STATIC_BUILD_STRIP"}
      >> the "DEFAULT" entry is mandatory. 
      >> Whenever env contains the matching key, corresponding build-type is used.
      >> First non-default match is picked.
      >> If no match, "DEFAULT" is used.
      
Examples:
>> Example of shared library not added to AMDB
   Unless GEN_SHARED_LIBS is passed, these libraries won't be compiled. They are detected by absence of amdb_info.
   The reason to not build them when amdb_info is not present is to avoid building supporting libraries - which carry multiple definitions.
      {
         "lib_name"      : "SAPlusCmnModule",
         "build"         : "SHARED_BUILD_STRIP",
         "lib_major_ver" : 1,
         "lib_minor_ver" : 0,
         "is_qurt_used"  : false
      }
>> Example of static library not added to AMDB
      {
         "lib_name"     : "WmaStdV8EncoderLib",
         "build"        : "STATIC_BUILD_STUB"
      }
>> Example of a library stubbed for target but built for SIM
   {
      "lib_name"     : "VisualizerModule",
      "build"        : {"DEFAULT":"STATIC_BUILD_STUB","AVS_TEST":"STATIC_BUILD_NO_STRIP"}
   },        
>> Example of shared library added to AMDB
   When GEN_SHARED_LIBS is not present in the env, these libraries are converted to static.
   {
      "lib_name"      : "DolbySurroundModule",
      "comment"       : "When adding a new major version or using a new file name for a module, keep this entry as well for backward compatibility. Use new rev_num. See readme.txt for details",
      "build"         : "SHARED_BUILD_STRIP",
      "lib_major_ver" : 1,
      "lib_minor_ver" : 0,
      "is_qurt_used"  : false,
      "depends_on"    : [],
      "amdb_info"   : 
      {
         "preload"            : false,
         "rev_num"            : 1,
         "itype"              : "capi_v2",
         "mtype"              : "generic",  
         "ID_1"               : "0x10775",
         "ID_2"               : "0",
         "tag"                : "capi_v2_ds1ap",
         "qact_name"          : "AUDPROC_MODULE_ID_DS1AP"
      }
   }
>> Example of static library added to AMDB with no stripping done for build.
   {
      "lib_name"      : "ComboPP",
      "amdb_info"     : 
      {
         "itype"              : "appi",         
         "mtype"              : "generic",
         "ID_1"               : "0x10C27",
         "tag"                : "appi_combopp",
         "qact_name"          : "AUDPROC_MODULE_ID_EQUALIZER"         
      }
   }, 
      
>> Example of a virtual stub done at library level. See next example for an alternate way.
   {
      "lib_name"     : "ds2ap_module",
      "build"        : "STATIC_BUILD_VIRTUAL_STUB",
      "amdb_info"     : 
      {
         "itype"              : "appi",         
         "mtype"              : "generic",
         "ID_1"               : "0x108DA",
         "tag"                : "appi_ds2ap",
         "qact_name"          : "AUDPROC_MODULE_ID_DS2AP"         
      }      
   },      
>> Example of a virtual stub done at a module level. See previous example for an alternate way.
   {
      "lib_name"     : "ds2ap_module",
      "build"        : "STATIC_BUILD_STRIP",
      "amdb_info"     : 
      {
         "itype"              : "virtual_stub",         
         "mtype"              : "generic",
         "ID_1"               : "0x108DA",
         "tag"                : "appi_ds2ap",
         "qact_name"          : "AUDPROC_MODULE_ID_DS2AP"         
      }      
   },   
>> Below results in error as a lib having amdb_info uses STATIC_BUILD_STUB. To avoid the error, use virtual-stub. see previous 2 examples.
   {
      "lib_name"     : "ds2ap_module",
      "build"        : "STATIC_BUILD_STUB",
      "amdb_info"     : 
      {
         "itype"              : "appi",         
         "mtype"              : "generic",
         "ID_1"               : "0x108DA",
         "tag"                : "appi_ds2ap",
         "qact_name"          : "AUDPROC_MODULE_ID_DS2AP"         
      }      
   },   
>> Below is permitted (as amdb_info is not present). see next example.
   {
      "lib_name"     : "fmwk_library",
      "build"        : "STATIC_BUILD_STUB"
   }, 
>> Below is also permitted. As 'STATIC_BUILD_VIRTUAL_STUB' automatically reduces to build='STATIC_BUILD_STUB'
   {
      "lib_name"     : "fmwk_library",
      "build"        : "STATIC_BUILD_VIRTUAL_STUB"
   },    
