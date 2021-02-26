#===============================================================================
#
# AVS shared library config parser
#
# GENERAL DESCRIPTION parses the json files containing avs library configuration and outputs 
#                     (.) dictionary needed for building shared libs, 
#                     (.) adds env variable for static libs,
#                     (.) generates information needed for amdb_registration/loading, 
#                         CPP files for static and dynamic modules.
#                     (.) generates XML for QACT,
#                     (.) generates json files for HY22 package.
#                     
#
#
# Copyright (c) 2015 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Technologies Proprietary.
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/avs.build/1.0/avs_libs_config_parser.py#4 $
#  $DateTime: 2018/04/27 05:15:34 $
#  $Author: pwbldsvc $
#  $Change: 16037238 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 11/07/2014   rbhatnk      Create Builder
#
#===============================================================================
#===============================================================================

#===============================================================================
# Overall function to be called from scons
#===============================================================================
import re
import shutil
import os
import errno

 
def avs_libs_config_parser(env, is_unit_test=False):
   from collections import OrderedDict;
   import os;
   
     
   ### list of json files containing shared lib configuration.
   # actual name is <chipset>/<below given name>.json
   input_json_name_prefixes=['afe_libs_cfg', 
            'aud_dec_libs_cfg', 
            'aud_enc_libs_cfg', 
            'aud_pp_libs_cfg', 
            'voc_libs_cfg',
            'fmwk_libs_cfg',
            'listen_libs_cfg',
            'avs_cmn_libs_cfg',
            'custom_libs_cfg', #a file for customer modules if any. Also looked in 'OEM_ROOT',
            'mdf_aud_pp_libs_cfg'
            ];
   hy22_json_dir='hy22'
   hy22_json_file_name='avs_cmn_libs_cfg'
   static_cpp_file_name = 'avs_static_build_config'
   shared_cpp_file_name = 'avs_shared_build_config'
   
   inc_header_files=[] #mm_main.h -> module mgr.
   MAX_PREREQUISITE_MIDS=2 #if this is changed, then avs/module_mgr/src/mm_main.h#MM_MAX_PREREQUISITE_MIDS must also be changed.
   #### constants
   MODULE_TYPE_MAP=OrderedDict([('generic',0), ('decoder',1), ('encoder',2),('converter',3),('packetizer',4),('depacketizer',5)])
   INTERFACE_TYPE_MAP=OrderedDict([('virtual_stub',3),('capi_v2',2)]);#('capi',1),('appi',0)
   STATIC_BUILD_TYPES=['STATIC_BUILD_STRIP', 'STATIC_BUILD_NO_STRIP', 
                     'STATIC_BUILD_STRIP_ONLY', 'STATIC_BUILD_STUB', 'STATIC_BUILD_VIRTUAL_STUB']; 
   SHARED_BUILD_TYPES=['SHARED_BUILD_STRIP', 'SHARED_BUILD_NO_STRIP', 
                     'SHARED_NO_BUILD']; #SHARED_NO_BUILD: not built but maybe added to AMDB if AMDB_info is present.
   STUB_BUILD_TYPES=['STATIC_BUILD_STUB', 'STATIC_BUILD_VIRTUAL_STUB'];
                     
   STATIC_BUILD_TYPES_ENV_SUFFIX_MAP = {'STATIC_BUILD_STRIP':'_STRIP', 
                     'STATIC_BUILD_NO_STRIP':None,  #for NO_STRIP nothing to do
                     'STATIC_BUILD_STRIP_ONLY':'_STRIP_ONLY', 
                     'STATIC_BUILD_STUB':'_STUB',
                     'STATIC_BUILD_VIRTUAL_STUB':'_STUB',}
                     
   ## when GEN_SHARED_LIBS is not in the env, then below takes effect.
   SHARED_BUILD_TYPE_TO_STATIC_MAP={'STATIC_BUILD_STRIP':'STATIC_BUILD_STRIP', 
                        'STATIC_BUILD_NO_STRIP':'STATIC_BUILD_NO_STRIP', 
                        'STATIC_BUILD_STRIP_ONLY':'STATIC_BUILD_STRIP_ONLY', 
                        'STATIC_BUILD_STUB':'STATIC_BUILD_STUB', 
                        'SHARED_BUILD_STRIP':'STATIC_BUILD_STRIP', 
                        'SHARED_BUILD_NO_STRIP':'STATIC_BUILD_NO_STRIP',
                        'SHARED_NO_BUILD':'SHARED_NO_BUILD',
                        'STATIC_BUILD_VIRTUAL_STUB':'STATIC_BUILD_VIRTUAL_STUB'}  
   ## when AVS_TEST, or COSIM_PROFILING_TEST is passed in the env, then below takes effect.                      
   BUILD_TYPE_TO_REGULAR_MAP = {'STATIC_BUILD_STRIP':'STATIC_BUILD_NO_STRIP', 
                        'STATIC_BUILD_NO_STRIP':'STATIC_BUILD_NO_STRIP', 
                        'STATIC_BUILD_STRIP_ONLY':'STATIC_BUILD_NO_STRIP', 
                        'STATIC_BUILD_STUB':'STATIC_BUILD_STUB', 
                        'SHARED_BUILD_STRIP':'SHARED_BUILD_NO_STRIP', 
                        'SHARED_BUILD_NO_STRIP':'SHARED_BUILD_NO_STRIP', 
                        'SHARED_NO_BUILD':'SHARED_NO_BUILD',
                        'STATIC_BUILD_VIRTUAL_STUB':'STATIC_BUILD_VIRTUAL_STUB'}
   
   class ComponentInfo:
     
     def __init__(self):
        
        print "Getting comp specific info"
        component_name = env["AU_NAME"]

        build_root = env.subst('${BUILD_ROOT}')
        proc_type = build_root.split('/')[-1]
        if proc_type in ["modem_proc","adsp_proc","slpi_proc"]:
           print "Setting up proc_mode as", proc_type
        else:
            raise Exception('Error: Invalid Proc Name..!')

        if not component_name:
           raise Exception('Error: Invalid component Name..!')
       
        self.component_name =    component_name
        self.proc_type   =    proc_type
        self.auto_dirs = self.get_auto_dirs() #get auto dirs

        print "Component name", component_name


  
     def get_auto_dirs(self):
        auto_gen_dir = env.subst('${BUILDPATH}')
        auto_dirs = {
         "avs":{
          "qact_xml_file_name" :'avs_cfg_autogen/avs_config.xml',
          "c_file_dir":'../main/module_mgmt/module_mgr/autogen/',
          "avs_cfg_autogen_folder":'avs_cfg_autogen'
         },
         "avs_mdf":{
          "qact_xml_file_name" : 'avs_mdf_cfg_autogen/avs_mdf_config.xml',
          "c_file_dir" : '../module_mgmt/module_mgr/autogen/',
          "avs_cfg_autogen_folder":'avs_mdf_cfg_autogen'
         },
         "audio_avs":{ 
          "qact_xml_file_name" : 'avs_cfg_autogen/%s/avs_config.xml' %auto_gen_dir,
          "c_file_dir":'../main/module_mgmt/module_mgr/autogen/%s/' %auto_gen_dir,
         "avs_cfg_autogen_folder":'avs_cfg_autogen/%s' %auto_gen_dir
         }   
        }
        return auto_dirs[self.component_name]

     @property
     def qact_xml_file_name(self):
        print "setting qact_xml_file_name"
        return self.auto_dirs["qact_xml_file_name"]

     @property
     def c_file_dir(self):
        print "setting c_file_dir"
        return self.auto_dirs["c_file_dir"]
        avs_cfg_autogen_folder=self.auto_dirs["avs_cfg_autogen_folder"]

     
     @property
     def avs_cfg_autogen_folder(self):
        print "setting avs_cfg_autogen_folder"
        return self.auto_dirs["avs_cfg_autogen_folder"]

     def get_inc_header_files(self):
       
        inc_header_files_t={
         "avs_mdf":
         [
          'adsp_audproc_api.h',
          'adsp_media_fmt.h',
          'adsp_amdb_api.h', 
          'Elite_CAPI_V2.h',
          'mm_main.h' 
         ],
         "avs":
         [
          'adsp_audproc_api.h',
          'adsp_lsm_session_commands.h',
          'adsp_lsm_service_commands.h',
          'adsp_media_fmt.h',
          'adsp_vparams_internal_api.h',
          'adsp_vparams_api.h',
          'adsp_vpm_api.h',
          'adsp_afe_service_commands.h',
          'adsp_amdb_api.h',
          'Elite_CAPI_V2.h',
          'mm_main.h'
         ],
         "audio_avs":
         [
          'adsp_audproc_api.h',
          'adsp_lsm_session_commands.h',
          'adsp_lsm_service_commands.h',
          'adsp_media_fmt.h',
          'adsp_vparams_internal_api.h',
          'adsp_vparams_api.h',
          'adsp_vpm_api.h',
          'adsp_afe_service_commands.h',
          'adsp_amdb_api.h',
          'Elite_CAPI_V2.h',
          'mm_main.h'
         ]
        }
        return inc_header_files_t[self.component_name]

     def get_proc_info(self):
        print "setting up proc info"
        #Note: AVS_MDF is compiled for adsp_proc only in sim mode & the proc info is defined as "MODEM" *ONLY*
        #for simulation. It's not expected to be compiled for ADSP crm builds.
        proc_name_id = {
         "avs": {
          "modem_proc" : {"proc_name":"ADSP_DSP","proc_id":"0x0001"},
          "adsp_proc" : {"proc_name":"ADSP_DSP","proc_id":"0x0001"},
          "slpi_proc" : {"proc_name":"ADSP_DSP","proc_id":"0x0001"}
         },
         "avs_mdf": {
          "modem_proc" : {"proc_name":"MDSP_DSP","proc_id":"0x0002"},
          "adsp_proc" : {"proc_name":"MDSP_DSP","proc_id":"0x0002"},
           "slpi_proc" : {"proc_name":"SDSP_DSP","proc_id":"0x0003"}
           },
         "audio_avs": {
           "modem_proc" : {"proc_name":"ADSP_DSP","proc_id":"0x0001"},
           "adsp_proc" : {"proc_name":"ADSP_DSP","proc_id":"0x0001"},
            "slpi_proc" : {"proc_name":"ADSP_DSP","proc_id":"0x0001"}
          },
         }
        p = { "proc_name":proc_name_id[self.component_name][self.proc_type]["proc_name"], "proc_id":proc_name_id[self.component_name][self.proc_type]["proc_id"] }
        return p
   
   component_info = ComponentInfo()
   (qact_xml_file_name,c_file_dir,avs_cfg_autogen_folder)=(component_info.qact_xml_file_name,component_info.c_file_dir, component_info.avs_cfg_autogen_folder)
   inc_header_files = component_info.get_inc_header_files() #set header files
  
   
   def ignore_for_hy22(env, needed_for_hy22):
      #if env defines HY22 flag and a module is not needed for hy22, then return true to ignore it
      if (env.get('hy22')) == True:
         if not needed_for_hy22:
            return True; 
         else:
            return False; 
      else:
         return False;
 
   class ModuleIdClass:
      def __init__(self,  module_type, id1, id2):
         self.id1 = int(id1.strip(),0)
         self.id2 = int(id2.strip(),0)
         self.module_type = module_type.strip().lower();
      def __str__(self):
         return 'Module ID: (mtype=%s, ID1=%s, ID2=%s). '% (self.module_type,hex(self.id1),hex(self.id2))
      def __key(self):
         return (self.id1, self.id2, MODULE_TYPE_MAP[self.module_type]) #whether ,m-id is uniq or not doesnt depend on capi or capiv2
      def __eq__(self, other):
         return (self.__key()==other.__key());
      def __hash__(self):
         h=hash(self.__key());
         return h;
            
   class BuildInfoClass:
      def __init__(self, libname, build, is_amdb_info_present,lib_major_ver=0, 
                     lib_minor_ver=0, is_qurt_used=False, depends_on_list=[], 
                     is_needed_for_hy22=False, security_info=None, pkg_info=None):
         if (len(libname)==0) or (len(build)==0):
            s='BuildInfoClass: lib name or build cannot be empty';
            raise LibCfgParserError(s); 
         if build not in STATIC_BUILD_TYPES+SHARED_BUILD_TYPES:
            s='AvsLibsCfg Error: BuildInfoClass: lib name %s has unsupported build %s'%(libname, build);
            raise LibCfgParserError(s); 
         self.libname = libname;
         self.build = build;
         self.lib_major_ver = lib_major_ver;
         self.lib_minor_ver = lib_minor_ver;
         self.is_qurt_used = is_qurt_used;
         self.depends_on_list = depends_on_list;
         self.is_amdb_info_present = is_amdb_info_present;
         self.is_needed_for_hy22 = is_needed_for_hy22;
         self.pkg_info = pkg_info;
         self.security_info = security_info;
      def __str(self):
         return 'BuildInfoClass: lib %s, build %s, ver %d.%d'%(self.libname,self.build,self.lib_major_ver,self.lib_minor_ver)
      def __key(self):
         return (self.libname) #every 2 buildInfo elements must have these items different
      def __eq__(self, other):
         return (self.__key()==other.__key());
      def __hash__(self):
         h=hash(self.__key());
         return h;         
      def eq_in_all_respects(self, other):
        return ((self.libname == other.libname) and (self.build == other.build) and \
            (self.lib_major_ver == other.lib_major_ver) and (self.lib_minor_ver == other.lib_minor_ver) and \
            (self.is_qurt_used == other.is_qurt_used) and (self.depends_on_list == other.depends_on_list) and \
            (self.pkg_info == other.pkg_info) and (self.security_info == other.security_info) )

      def get_so_file_name(self):
         if self.build in STATIC_BUILD_TYPES:
            return None;
         # so file name is required even for no-build.
         return self.libname+'.so.'+str(self.lib_major_ver);
      def _populate_so_build_system(self, shared_lib_dict):
         if self.build in STATIC_BUILD_TYPES:
            return;
         if not self.build=='SHARED_NO_BUILD':
            strip_str='_SO';
            if self.build=='SHARED_BUILD_STRIP':
               strip_str='_STRIP_SO'
            uses_key='USES_'+self.libname.upper()+strip_str;
       
            is_qurt_used = 0;
            if self.is_qurt_used:
               is_qurt_used = 1;
           
            depends_on_list=[]; 
            if len(self.depends_on_list)>0:
               for i in self.depends_on_list:
                  depends_on_list.append(str(i).strip());
        
            if shared_lib_dict.has_key(uses_key):
               s='AvsLibsCfg: Warning: .so file '+self.libname+' was already added for build. Fix if this is a mistake.'
               print s
            else:
               ## only for non-mobile pass security info etc
               if 'USES_NON_MOBILE' in env:
                  shared_lib_dict[uses_key]=[ [self.lib_major_ver, self.lib_minor_ver], [is_qurt_used],depends_on_list,self.security_info,self.pkg_info]; 
                  print '%s [ [%d, %d], [%d], %s, %s, %s]'%(uses_key,self.lib_major_ver,self.lib_minor_ver,is_qurt_used,str(depends_on_list),self.security_info,self.pkg_info)               
               else:
                  shared_lib_dict[uses_key]=[ [self.lib_major_ver, self.lib_minor_ver], [is_qurt_used],depends_on_list]; 
                  print '%s [ [%d, %d], [%d], %s]'%(uses_key,self.lib_major_ver,self.lib_minor_ver,is_qurt_used,str(depends_on_list))
                  
      def _populate_static_build_system(self, env):
         if self.build not in STATIC_BUILD_TYPES:
            return;
         k=STATIC_BUILD_TYPES_ENV_SUFFIX_MAP[self.build];
         if not k==None:
            s='USES_'+self.libname.upper()+k;
            if not is_unit_test:
               #import pdb;pdb.set_trace()
               s1='env.Replace('+s+'=True)'
               exec(s1) in locals()
            print 'AvsLibsCfg: env.Replace(%s=True)'%(s)
         print 'AvsLibsCfg: lib %s: %s '%(self.libname.upper(), self.build)
            
      def populate_build_system(self, shared_lib_dict, env):
         self._populate_so_build_system(shared_lib_dict)
         self._populate_static_build_system(env)
            
      # TODO:this doesn't count revisions of modules.
      def count_build_type(self, build_type_counter_no_amdb_info, build_type_counter_with_amdb_info):
         if self.is_amdb_info_present:
            if self.build in build_type_counter_with_amdb_info:
               build_type_counter_with_amdb_info[self.build]+=1;
            else:
               build_type_counter_with_amdb_info[self.build]=1;
         else:
            if self.build in build_type_counter_no_amdb_info:
               build_type_counter_no_amdb_info[self.build]+=1;
            else:
               build_type_counter_no_amdb_info[self.build]=1;
      def __str__(self):
         return "BuildInfoClass: lib name %s, build %s, lib_major_ver %d, lib_minor_ver %d, is_qurt_used %d, depends_on_list %s" %(self.libname, self.build, self.lib_major_ver, self.lib_minor_ver, self.is_qurt_used, str(self.depends_on_list))
         
   class ModuleInfoClass:
      def __init__(self, interface_type, qact_name, build, filename='', preload=True, tag='', is_private=False, is_test_module=False, prereq_mids=[], is_needed_for_hy22=False):
         self.interface_type = interface_type.strip().lower()
         self.qact_name=qact_name;
         self.build=build;
         self.filename=filename; #so_file valid for shared lib only.
         self.preload=preload;
         self.tag=tag.strip();
         self.is_private = is_private; #private modules dont go to QACT XML. but they can get built or get into AMDB.
         self.prereq_mids = prereq_mids;
         self.is_test_module = is_test_module;
         self.is_needed_for_hy22 = is_needed_for_hy22;
         if is_private:
            if not build in STATIC_BUILD_TYPES:
               s='A private module can only be static. file name %s'%(self.filename)
               raise LibCfgParserError(s);
      def get_built_type(self):
         if self.build in 'STATIC_BUILD_VIRTUAL_STUB' or self.interface_type in 'virtual_stub':
            print 'AvsLibsCfg: virtual_stub '+str(self)
            return 'VIRTUAL-STUB'
         elif self.build in STATIC_BUILD_TYPES:
               return 'STATIC'; 
         else:
            return 'DYNAMIC'; #note that NO_BUILD also becomes DYNAMIC. static modules need not be kept with rev_num. Hence NO_BUILD is never for static.
      def need_to_appear_in_qact(self, env):
         #private modules, test modules and modules that are not needed for HY22 (when HY22 is defined)
         if self.is_private or self.is_test_module or ignore_for_hy22(env, self.is_needed_for_hy22):
            return False;
         return True;
      def is_new_rev_needed(self, other):
         return not (self.filename == other.filename);
      def __str__(self):
         return 'AMDB Info: (Itype %s, QACT Name %s, Filename=%s, preload=%s, tag=%s, build %s). '% (self.   interface_type,\
            self.qact_name,str(self.filename),\
            str(self.preload),self.tag,self.build)  ;        
            
   class LibCfgParserError(Exception):
      def __init__(self, value):
         self.value = value
      def __str__(self):
         return repr(self.value)
   
   def map_build_input_to_built_str(lib_name, build_in, is_amdb_info_present, is_needed_for_hy22):
      if build_in.__class__ == dict:
         if 'DEFAULT' not in build_in:
            s='the build dict does not contain default value for lib '+lib_name
            raise LibCfgParserError(s);
         found=False;
         for k in build_in.keys():
            k=k.strip();
            if not k == 'DEFAULT':
               if env.get(k.upper()) == True or env.get(k.lower()) == True:
                  build_in = build_in[k]; 
                  found=True;
                  break;
         if not found:
            build_in = build_in['DEFAULT'].strip();
            #print build_in
            
      build_in=build_in.strip();
      ## If HY22 is defined and a module is not needed for HY22, then make it virtual stub (independent of GEN_SHARED_LIBS)
      if ignore_for_hy22(env, is_needed_for_hy22):
         # some libs like supporting .so files need not be built when building without GEN_SHARED_LIBS in env.
         # if they are built then symbol clash may occur.
         # such libs dont have AMDB info. That's how they are identified.
         if build_in in SHARED_BUILD_TYPES and not is_amdb_info_present:
            print 'AvsLibsCfg: '+lib_name+' with build='+build_in+' has no AMDB info. Will not be built for HY22.'
            build_in = 'SHARED_NO_BUILD'      
         else:
            build_in = 'STATIC_BUILD_VIRTUAL_STUB'
      else:
         ## those libs which are configured as shared libs must be built as static if 'GEN_SHARED_LIBS' is not defined 
         if 'GEN_SHARED_LIBS' not in env:
            # some libs like supporting .so files need not be built when building without GEN_SHARED_LIBS in env.
            # if they are built then symbol clash may occur.
            # such libs dont have AMDB info. that's how they are identified.
            if build_in in SHARED_BUILD_TYPES:
               if not is_amdb_info_present:
                  print 'AvsLibsCfg: '+lib_name+' with build='+build_in+' has no AMDB info. Will not be built without GEN_SHARED_LIBS.'
                  build_in = 'SHARED_NO_BUILD'
            build_in=SHARED_BUILD_TYPE_TO_STATIC_MAP[build_in]
         if (env.get('AVS_TEST') == True) or (env.get('COSIM_PROFILING_TEST') == True):
            build_in=BUILD_TYPE_TO_REGULAR_MAP[build_in]
      return build_in;
         
   #===============================================================================
   # Parse the json files
   # mod_info: {m-id:{rev_num:(module-info)}}
   # build_info_set : set(build_info)
   # hy22_json : []
   #=============================================================================== 
   def parse_each_json(json_info, build_info_set, mod_info, hy22_json):
      for lib in json_info:
         #import pdb;pdb.set_trace()
         try:
            lib_name = lib['lib_name'].strip();
            is_amdb_info_present=lib.has_key('amdb_info') and len(lib['amdb_info'])>0;
            
            is_needed_for_hy22 = False;
            if lib.has_key('is_needed_for_hy22'):
               is_needed_for_hy22 = lib['is_needed_for_hy22'];
               
            b='STATIC_BUILD_NO_STRIP';
            if lib.has_key('build'):
               b=lib['build'];
            build = map_build_input_to_built_str(lib_name, b, is_amdb_info_present, is_needed_for_hy22);  
               
            if is_amdb_info_present and build=='STATIC_BUILD_STUB':
               s='AvsLibsCfg Error: Library %s has amdb_info but is specified for static build stub. This is not supported. Use STATIC_BUILD_VIRTUAL_STUB.'%(lib_name)
               raise LibCfgParserError(s);
               
            maj_version,min_version,is_qurt_used=0,0,False;
            # for shared libs, version,is_qurt_used is mandatory. 
            if build in SHARED_BUILD_TYPES: #directly read so that if user doesnt provide KeyError will occur
               maj_version = lib['lib_major_ver'];
               min_version = lib['lib_minor_ver'];
               is_qurt_used = lib['is_qurt_used'];
            else:
               if lib.has_key('lib_major_ver'):
                  maj_version = lib['lib_major_ver'];
               if lib.has_key('lib_minor_ver'):
                  min_version = lib['lib_minor_ver'];
               if lib.has_key('is_qurt_used'):
                  is_qurt_used = lib['is_qurt_used'];
               
            depends_on=[];
            if lib.has_key('depends_on'):
               depends_on = lib['depends_on'];
            
            pkg_info=None
            if lib.has_key('pkg_info'):
               #import pdb;pdb.set_trace() 
               pkg_info = lib['pkg_info'].strip();
               
            security_info = None;
            if lib.has_key('security_info'):
               #import pdb;pdb.set_trace() 
               security_info = lib['security_info'];
               security_info = {k.strip():v.strip() for k,v in security_info.iteritems()}
               ## extra checks to ensure manual error is not made.
               if not (build in SHARED_BUILD_TYPES or build in STUB_BUILD_TYPES):
                  s='AvsLibsCfg Error: Library %s has security_info but not built as shared or as stub (security can be achieved only for shared libs). build=%s'%(lib_name, build)
                  raise LibCfgParserError(s);
               if None == pkg_info:
                  s='AvsLibsCfg Error: Library %s has security_info but not pkg_info'%(lib_name)
                  raise LibCfgParserError(s);
                  
            if None == security_info and not None == pkg_info:
               s='AvsLibsCfg Error: Library %s has pkg_info but not security_info'%(lib_name)
               raise LibCfgParserError(s);
            #every item in json must have build-info
            build_info = BuildInfoClass(lib_name, build, is_amdb_info_present, maj_version, 
               min_version, is_qurt_used, depends_on, is_needed_for_hy22, security_info, pkg_info);
            
            if build_info in build_info_set:
               already_added=build_info_set[build_info]; #build_info.hash is used as key
               if not build_info.eq_in_all_respects(already_added):
                  s='AvsLibsCfg Error: Library %s is already added for build but all fields are not matching'% \
                     (lib_name);
                  raise LibCfgParserError(s)
            else:      
               build_info_set[build_info] = build_info #using orderedDict to implement orderedSet
            is_test_module = False; #test modules don't appear in QACT
            if lib.has_key('is_test_module'):
               is_test_module = lib['is_test_module'];
               
            #amdb_info is optional. all shared lib or all static libs need not have.
            if is_amdb_info_present:
               rev_num=1;
               if lib['amdb_info'].has_key('rev_num'):
                  rev_num = lib['amdb_info']['rev_num'];
               itype = lib['amdb_info']['itype'].strip().lower();
               
               mtype = lib['amdb_info']['mtype'].strip().lower();
               id1 = lib['amdb_info']['ID_1'].strip().lower();
               id2="0"
               if lib['amdb_info'].has_key('ID_2'):
                  id2 = lib['amdb_info']['ID_2'].strip().lower();
               preload=False;
               if lib['amdb_info'].has_key('preload'):
                  preload = lib['amdb_info']['preload'];
               tag='' #tag can be empty for 'virtual stubs'
               if lib['amdb_info'].has_key('tag'):
                  tag = lib['amdb_info']['tag'].strip();
               qact_name = lib['amdb_info']['qact_name'].strip();
               is_private = False;
               if lib['amdb_info'].has_key('is_private'):
                  is_private = lib['amdb_info']['is_private'];  

               if build=='STATIC_BUILD_VIRTUAL_STUB':
                  print 'AvsLibsCfg Info: Library %s is virtual-stub, hence forcing itype of module %s from %s to virtual_stub'%(lib_name,qact_name,itype)
                  itype='virtual_stub' #if itype is virtual_stub, but build is not, then it's ok to build as we are not going to use what's built.
                  
               prereq_mids=[];
               #for virtual-stubs, we dont need to list pre-req modules.
               if lib['amdb_info'].has_key('prereq_mids') and (not (itype == 'virtual_stub')):
                  d_ar=lib['amdb_info']['prereq_mids'];
                  for d in d_ar: #array
                     m=ModuleIdClass(d["mtype"], d["ID_1"], d["ID_2"]);
                     prereq_mids.append(m);

               if (itype not in INTERFACE_TYPE_MAP.keys()):
                   s='AvsLibsCfg Error: unknown interface type '+itype
                   raise LibCfgParserError(s);
                   
               if (mtype not in MODULE_TYPE_MAP.keys()):
                   s='AvsLibsCfg Error: unknown module_type '+mtype
                   raise LibCfgParserError(s);
                  
               if not mtype=='converter' and not (id2 == "0" or id2 == "0x0"):
                   s='AvsLibsCfg Error: Only converter can have id2 nonzero. qact-name %s'+(qact_name)
                   raise LibCfgParserError(s);  
               
               if not (build == 'STATIC_BUILD_VIRTUAL_STUB'):
                  if tag==None or len(tag)==0:
                     s='AvsLibsCfg Error: when build type is %s and amdb_info is present, tag must be present for qact-name %s'+(build, qact_name)
                     raise LibCfgParserError(s);  
                  
               if len(prereq_mids) > MAX_PREREQUISITE_MIDS:
                   s='AvsLibsCfg Error: len(prereq_mids)=%d > MAX_PREREQUISITE_MIDS=%d'+(len(prereq_mids),MAX_PREREQUISITE_MIDS)
                   raise LibCfgParserError(s);  
                   
               so_filename = build_info.get_so_file_name();
                  
               new_m_id = ModuleIdClass(mtype, id1, id2);
               new_mod_info = ModuleInfoClass( itype, qact_name, build, so_filename, preload, tag, is_private, is_test_module, prereq_mids, is_needed_for_hy22);

               if mod_info.has_key(new_m_id):
                  if mod_info[new_m_id].has_key(rev_num):
                     s='AvsLibsCfg Error: %s Revision %d already exists.'%(str(new_m_id), rev_num);
                     raise LibCfgParserError(s);
                  else:
                     #if this & prev or this & next versions are already in mod_info, and they are not same file name then, the revision is fine. otherwise, revision incr was mistakenly done .
                     if ( ( mod_info[new_m_id].has_key(rev_num-1) and new_mod_info.is_new_rev_needed( mod_info[new_m_id][rev_num-1] )) or \
                          ( mod_info[new_m_id].has_key(rev_num+1) and new_mod_info.is_new_rev_needed( mod_info[new_m_id][rev_num+1] )) ): 
                           mod_info[new_m_id][rev_num] = new_mod_info;
                     else:
                        s='AvsLibsCfg Error: This and previous revision are same for %s, rev %d'%(str(new_m_id), rev_num);
                        raise LibCfgParserError(s);
               else:
                  mod_info[new_m_id] = {rev_num:new_mod_info}
            if env.get('hy22') == True and is_needed_for_hy22:
               #remove unnecessary keys.
               if lib.has_key('comment'):
                  lib.pop('comment')
               if lib.has_key('comments'):
                  lib.pop('comments')                  
               hy22_json.append(lib);
               
         except KeyError as e:
             print 'AvsLibsCfg: KeyError '+ str(e)
             print 'AvsLibsCfg: Mandatory keys are not provided'
             import traceback
             traceback.print_exc()
             raise e;
      return ;

   #===============================================================================
   # Print mod_info
   #===============================================================================
   def print_mod_info(mod_info):
      for m in mod_info:
         for r in mod_info[m]:
            print 'AvsLibsCfg: %s, rev=%d, %s' % (str(m), r, str(mod_info[m][r]))
    
   def write_c_macros(file):
      for h in inc_header_files:
         file.write('#include "%s"\n' %(h));
      file.write('\n');
      for i in INTERFACE_TYPE_MAP:
         file.write('#define %-10s %d\n' %(i.upper(), INTERFACE_TYPE_MAP[i]));
      file.write('\n');
      for m in MODULE_TYPE_MAP:
         file.write('#define %-10s %d\n' %(m.upper(), MODULE_TYPE_MAP[m]));
      file.write('\n');
         
   #mod_info {m-id:{rev_num:(module-info)}}         
   def write_cpp_files(static_cpp_file, static_header_file, shared_cpp_file, shared_header_file, mod_info):
      ## first loop to get some info.
      private_module_list=[];
      prereq_mid_list=OrderedDict(); # {module-id-class:[module-id-class]}
      for m in mod_info:
         # sort with highest revision first. 
         revisions=sorted(mod_info[m], reverse=True)
         is_latest_rev=True;
         last_r=0
         for r in revisions:
            if ignore_for_hy22(env, mod_info[m][r].is_needed_for_hy22):
               continue;         
            if not is_latest_rev:
               is_latest_rev=False;
               if last_r != r+1:
                  s='AvsLibsCfg: Error: module %s has skipped versions between %d and %d'%(m,last_r,r)
                  raise LibCfgParserError(s);
               if mod_info[m][last_r].is_private != mod_info[m][r].is_private: #this cannot happen as private modules cannot have rev, and shared modules cannot be private.
                  s='AvsLibsCfg: Error: privateness of a module cannot change across revisions. Module %s. Rev[%d] = %d and Rev[%d]=%d'%(m,last_r,mod_info[m][last_r].is_private,r,mod_info[m][r].is_private)
                  raise LibCfgParserError(s);
            else: #only once:
               if mod_info[m][r].is_private:
                  private_module_list.append(m);
            #prerequisite module 
            if prereq_mid_list.has_key(m): #if already populated once
               #check if it matches across revisions
               if not len(prereq_mid_list[m]) == len(mod_info[m][r].prereq_mids):
                  s='AvsLibsCfg: Error: %s has num prerequisite modules not matching previous revisions at rev %d'%(m, r)
                  raise LibCfgParserError(s);             
            for d in mod_info[m][r].prereq_mids:
               #check if prerequisite module is defined at all.
               if d not in mod_info.keys(): 
                  s='AvsLibsCfg: Error: %s has undefined prerequisite %s '%(m, d)
                  raise LibCfgParserError(s);
               if prereq_mid_list.has_key(m): #if already populated once
                  if d not in prereq_mid_list[m]:
                     s='AvsLibsCfg: Error: %s has prerequisite modules not matching previous revisions at rev %d'%(m, r)
                     raise LibCfgParserError(s);
            if len(mod_info[m][r].prereq_mids)>0:
               prereq_mid_list[m] = mod_info[m][r].prereq_mids;
            last_r=r
            
      s='/** Copyright (c) 2015 QUALCOMM Incorporated. All Rights Reserved. */\n' + \
        '/** ***** This is an autogenerated file ***** **/\n' + \
        '/** ***** DO NOT EDIT THIS FILE ***** **/\n\n'
      static_cpp_file.write(s);
      shared_cpp_file.write(s);
      static_header_file.write(s);
      shared_header_file.write(s);
      
      static_header_file.write('#ifndef __AVS_STATIC_BUILD_CONFIG_H_\n#define __AVS_STATIC_BUILD_CONFIG_H_\n\n');
      shared_header_file.write('#ifndef __AVS_SHARED_BUILD_CONFIG_H_\n#define __AVS_SHARED_BUILD_CONFIG_H_\n\n');
      
      write_c_macros(static_header_file);  
      static_header_file.write('extern "C" {\n');
      static_cpp_file.write('#include "'+static_cpp_file_name+'.h"\n\n');
      static_cpp_file.write('extern "C" {\n');
      
      write_c_macros(shared_header_file);      
      shared_header_file.write('extern "C" {\n');
      shared_cpp_file.write('#include "'+shared_cpp_file_name+'.h"\n\n');
      shared_cpp_file.write('extern "C" {\n');
      temp_file_name_dict={};
      temp_tag_dict={};

      mod_counter = {}; #static, dynamic capi, capi_v2                  
      for itype in INTERFACE_TYPE_MAP.keys():
         mod_counter['STATIC ' + itype] = 0;
         mod_counter['DYNAMIC ' + itype] = 0;
         close_sh_parethesis=True;
         if itype=='capi_v2':
            shared_cpp_file.write('const mm_dynamic_capi_v2_module_t mm_dynamic_capi_v2_modules[] = \n{\n');
            static_cpp_file.write('const mm_static_capi_v2_module_t mm_static_capi_v2_modules[] = \n{\n');
         elif itype=='virtual_stub':
            static_cpp_file.write('const mm_module_id_t mm_virtual_stub_modules[] = \n{\n');
            close_sh_parethesis=False
         else:
            s='AvsLibsCfg Error: module %s has unhandled itype %s'%(m,itype)
            raise LibCfgParserError(s);            
         
         for m in mod_info:
            # sort with highest revision first. 
            revisions=sorted(mod_info[m], reverse=True)
            for r in revisions:
               if not mod_info[m][r].interface_type == itype:
                  continue;
               if ignore_for_hy22(env, mod_info[m][r].is_needed_for_hy22):
                  continue;
                  
               if mod_info[m][r].get_built_type()=='DYNAMIC':
                  mod_counter['DYNAMIC ' + itype] += 1;
                  fn_c_var=mod_info[m][r].filename.upper().replace('.','_').replace('-','_');
                  tag_c_var=mod_info[m][r].tag.upper();
                  if not temp_file_name_dict.has_key(fn_c_var):
                     temp_file_name_dict[fn_c_var]=0;
                     shared_header_file.write('const char %-40s   = {%-40s};\n'%(fn_c_var+'[]','"'+mod_info[m][r].filename+'"'))
                  if not temp_tag_dict.has_key(tag_c_var):
                     temp_tag_dict[tag_c_var]=0;
                     shared_header_file.write('const char %-40s   = {%-40s};\n'%(tag_c_var+'[]','"'+mod_info[m][r].tag+'"'))
                  if itype=='capi_v2':
                     shared_cpp_file.write('{%10s, %10s, %10s, %2d, %d, %-40s, %-40s},\n' %(m.module_type.upper(), 
                        hex(m.id1), hex(m.id2), r, 
                        mod_info[m][r].preload, fn_c_var, tag_c_var));
                  elif itype=='virtual_stub':
                     #this cannot happen as virtual-stubs are classified not as dynamic
                     pass
                  else:
                     s='AvsLibsCfg Error: module %s has unhandled itype %s'%(m,itype)
                     raise LibCfgParserError(s);
                     
               else: #static/virtual_stubs
                  if (len(revisions)>1):
                     s='AvsLibsCfg Error: module %s has more than one revision for static'%(m)
                     raise LibCfgParserError(s);
                  mod_counter['STATIC ' + itype] += 1; #includes virtual stubs
                  first_func='';
                  second_func='';
                  decl_first_func='';
                  decl_second_func='';
                  if itype=='capi_v2':
                     first_func=mod_info[m][r].tag+'_get_static_properties'
                     second_func=mod_info[m][r].tag+'_init'
                     decl_first_func='capi_v2_err_t %s (capi_v2_proplist_t *init_set_proplist, capi_v2_proplist_t *static_proplist);'%(first_func)
                     decl_second_func='capi_v2_err_t %s (capi_v2_t* _pif, capi_v2_proplist_t *init_set_proplist);'%(second_func)
                     static_cpp_file.write('{%10s, %10s, %10s, %-40s, %-40s},\n' %(m.module_type.upper(), hex(m.id1), hex(m.id2), first_func,second_func ));
                  elif itype=='virtual_stub':
                     static_cpp_file.write('{%10s, %10s, %10s},\n' %(m.module_type.upper(), hex(m.id1), hex(m.id2)));
                  else:
                     s='AvsLibsCfg Error: module %s has unhandled itype %s'%(m,itype)
                     raise LibCfgParserError(s);
                     
                  #declaration of the functions
                  if (len(decl_first_func)>0):
                     static_header_file.write(decl_first_func+'\n')
                     static_header_file.write(decl_second_func+'\n\n')                  
                     
         static_cpp_file.write('\n};\n\n'); 
         if close_sh_parethesis:
            shared_cpp_file.write('\n};\n\n');
         if itype=='capi_v2':
            static_cpp_file.write('const uint32_t mm_num_static_capi_v2_modules = %d;\n\n'%(mod_counter['STATIC ' + itype]));
            shared_cpp_file.write('const uint32_t mm_num_dynamic_capi_v2_modules = %d;\n\n'%(mod_counter['DYNAMIC ' + itype]));   
         elif itype=='virtual_stub':
            static_cpp_file.write('const uint32_t mm_num_virtual_stub_modules = %d;\n\n'%(mod_counter['STATIC ' + itype]));
         
         
      # write private modules into separate area in the file now.
      static_cpp_file.write('const mm_private_module_t mm_private_modules[] = {\n');
      for p in private_module_list:
         static_cpp_file.write('{%12s, %10s, %10s},\n' %(p.module_type.upper(),hex(p.id1), hex(p.id2)))
      static_cpp_file.write('};\n\n');
      static_cpp_file.write('const uint32_t mm_num_private_modules = %d;\n\n'%(len(private_module_list)));

      # write modules with prerequisite m-id into separate area in the file now.
      static_cpp_file.write('const mm_prereq_mids_t mm_prereq_mids[] = {\n');
      for m in prereq_mid_list:
         static_cpp_file.write('{{%12s, %10s, %10s}, { ' %(m.module_type.upper(),hex(m.id1), hex(m.id2)))
         for d in prereq_mid_list[m]:
            static_cpp_file.write('{%12s, %10s, %10s}, ' %(d.module_type.upper(),hex(d.id1), hex(d.id2)))
         for i in range(0,MAX_PREREQUISITE_MIDS-len(prereq_mid_list[m])):
            static_cpp_file.write('{%12d, %10d, %10d}, ' %(0,0,0))
         static_cpp_file.write('}},\n');
      static_cpp_file.write('};\n\n');
      static_cpp_file.write('const uint32_t mm_num_prereq_mids = %d;\n\n'%(len(prereq_mid_list)));
      
      shared_cpp_file.write('} /*extern c*/\n'); #extern c
      static_cpp_file.write('} /*extern c*/\n'); #extern c
      shared_header_file.write('} /*extern c*/\n\n#endif /*__AVS_SHARED_BUILD_CONFIG_H_*/\n\n'); #extern c
      static_header_file.write('} /*extern c*/\n\n#endif /*__AVS_STATIC_BUILD_CONFIG_H_*/\n\n'); #extern c        
   #===============================================================================   
   # Write QACT XML 
   # # mod_info=dictionary of modules. indexed by m-id. Each m-id contains a dict of mod-info. {m-id:{rev_num:(module-info)}}
   #===============================================================================
   def write_qact_xml(qact_xml_file, mod_info):
      build_id_tag1='UNIT TEST BUILD'
      build_id_tag2='UNIT TEST BUILD'
      if not is_unit_test:                                   
         build_id_tag1=str(env.subst('${ENGG_TIME_STAMP}'))  
         build_id_tag2=str(env.subst('${QCOM_TIME_STAMP}'))  
         
      qact_xml_file.write('<?xml version="1.0"?>\n');
      qact_xml_file.write('<!--\n\n   Copyright (c) 2015 QUALCOMM Incorporated. All Rights Reserved. \n\n'+
      '   DESCRIPTION: Configuration of the modules in DSP. \n'+
      '                This file serves as input to QACT\'s DSP Module Manager\n'+
      '                It helps in configuring which modules in DSP are needed and \n'+
      '                also to modify the preload flag of the modules. \n'+
      '                In addition, it tells whether the module is built statically or as a .so file.\n'+
      '\n'+
      '   ***** This is an autogenerated file ***** \n'+
      '   ***** DO NOT EDIT THIS FILE ***** \n\n-->\n\n')
      a = component_info.get_proc_info()
      qact_xml_file.write('<AVS-DB maj_ver="1"  min_ver=  "2">\n');
      qact_xml_file.write('<HEADER version="2">\n');
      qact_xml_file.write('   <BUILDID> %s </BUILDID>\n' %(build_id_tag2)); 
      qact_xml_file.write('   <BUILDID> %s </BUILDID>\n' %(build_id_tag1)); 
      qact_xml_file.write('   <PROC_NAME> %s </PROC_NAME>\n' %(a["proc_name"]));
      qact_xml_file.write('   <PROC_ID> %s </PROC_ID>\n' %(a["proc_id"]));
      qact_xml_file.write('</HEADER>\n');

      for m in mod_info:
         revisions=sorted(mod_info[m], reverse=True)
         r=revisions[0]; #select top revision. only preload flag is altered by revision.
         if mod_info[m][r].need_to_appear_in_qact(env): 
            if mod_info[m][r].qact_name==None or len(mod_info[m][r].qact_name)==0:
                s='AvsLibsCfg Error: Module needs to have a valid QACT name. lib_name %s' % str(mod_info[m][r])
                raise LibCfgParserError(s); 
            qact_xml_file.write('<MODINFO version="1">\n');
            #no need of interface type as it can change.
            qact_xml_file.write('   <MTYPE> '+m.module_type+' </MTYPE>\n');
            qact_xml_file.write('   <ID1> '+hex(m.id1)+' </ID1>\n');
            qact_xml_file.write('   <ID2> '+hex(m.id2)+' </ID2>\n');
            qact_xml_file.write('   <NAME> '+mod_info[m][r].qact_name+' </NAME>\n');
            qact_xml_file.write('   <PRELOAD> '+'USE_FROM_BUILD'+' </PRELOAD>\n');   
            qact_xml_file.write('   <BUILTTYPE>  '+mod_info[m][r].get_built_type()+' </BUILTTYPE>\n'); 
            qact_xml_file.write('</MODINFO>\n');     

      qact_xml_file.write('</AVS-DB>\n');
       
   def check_at_least_one_rev_built(mod_info):
      for m in mod_info:
         at_least_one_rev_b = False;
         for r in mod_info[m]:
            if not mod_info[m][r].build == 'SHARED_NO_BUILD':
               at_least_one_rev_b=True;
               break;
         if not at_least_one_rev_b:
            s='Not even one revision built for module %s'%(str(m));
            raise LibCfgParserError(s)
       
   #===============================================================================
   # checks if the root dir is under hap
   #===============================================================================
   def is_build_under_hap(env):
      if is_unit_test:
         return False;
      return re.search('hap', env.RealPath(c_file_dir))
      
   #===============================================================================
   # parse_cfg_and_gen_output
   #===============================================================================
   def parse_cfg_and_gen_output(env):
      chipset=env.get('CHIPSET')
      if None == chipset:
         print 'AvsLibsCfg: Error: chipset input not given' 
         return None;
         
      if is_unit_test:
         path_to_insert='';
      else:
         #env.Replace(AVS_AU_NAME ='avs')
         AVS_AU_NAME=str(env['AU_NAME'])
         get_inc_root=str(env['INC_ROOT'])
         AVS_AU_NAME=env['AU_NAME']
         join_path=os.path.join(get_inc_root, AVS_AU_NAME, 'build')
         path_to_insert=env.RealPath(join_path)
         
      hy22_json_full_dir = None;
      if env.get('hy22'):
         hy22_json_full_dir = os.path.join(path_to_insert, str(chipset), hy22_json_dir);
               
      only_populate_env = False;
      if not is_unit_test and env.GetOption('clean') and not env.GetOption('cleanpack'):
         #clean all outputs, dont return beacause pack utility needs the same lib cfg as was used for build.
         if os.path.exists(avs_cfg_autogen_folder):
            shutil.rmtree(avs_cfg_autogen_folder)
         if os.path.exists(c_file_dir):
            shutil.rmtree(c_file_dir)
         if not hy22_json_full_dir == None and os.path.exists(hy22_json_full_dir):
            shutil.rmtree(hy22_json_full_dir)            
         only_populate_env = True;
       
      shared_lib_dict=OrderedDict();

      hy22_json_file=None;
      if not is_build_under_hap(env):
         if not only_populate_env:
            if not os.path.exists(avs_cfg_autogen_folder):
           
                try:
                    os.makedirs(avs_cfg_autogen_folder)
                except OSError as exc:
                    if exc.errno != 17:
                        print exc.errno
                        raise exc
                    pass
            if not os.path.exists(c_file_dir):
                try:
                    os.makedirs(c_file_dir)
                except OSError as exc:
                    if exc.errno != 17:
                        raise exc
                    pass   
            if hy22_json_full_dir:
               if not os.path.exists(hy22_json_full_dir):
                  os.makedirs(hy22_json_full_dir)               
            
            try:
               qact_xml=open(qact_xml_file_name, 'w');
               static_cpp_file=open(os.path.join(c_file_dir,static_cpp_file_name)+'.cpp', 'w');
               static_header_file=open(os.path.join(c_file_dir,static_cpp_file_name)+'.h', 'w');
               shared_cpp_file=open(os.path.join(c_file_dir,shared_cpp_file_name)+'.cpp', 'w');
               shared_header_file=open(os.path.join(c_file_dir,shared_cpp_file_name)+'.h', 'w');
               if hy22_json_full_dir:
                  hy22_json_file=open(os.path.join(hy22_json_full_dir, hy22_json_file_name+'.json'),'w')
            except IOError as e:
               print 'AvsLibsCfg: Error opening files : '+str(e)
               raise e
               
      json_files=[];
      for name in input_json_name_prefixes:
         json_files.append(os.path.join(path_to_insert, str(chipset), name + '.json'));
      
      if env.get('OEM_ROOT') != None:        
        json_files.append( os.path.join(env.get('OEM_ROOT'),
                  'build', chipset, 'custom_libs_cfg.json'))
        print 'In AVS: OEM_ROOT is set', env.get('OEM_ROOT')
        print 'In AVS: Custom json file from OEM_ROOT', os.path.join(env.get('OEM_ROOT'),'build', chipset, 'custom_libs_cfg.json')

              
      import json
      mod_info=OrderedDict(); # dictionary of modules. indexed by m-id. Each m-id contains a dict of mod-info. {m-id:{rev_num:(module-info)}}
      build_info_set=OrderedDict(); #Using OrderedDict for implementing orderedSet. set of buildInfo 
      build_type_counter_no_amdb_info={};
      build_type_counter_with_amdb_info={};
      hy22_json=[];
      for json_file in json_files:
         #print 'AvsLibsCfg: Working on file '+json_file
         try:
            fp=open(json_file);
         except IOError as e:
            print 'AvsLibsCfg: Error opening JSON file : '+json_file+'. Ignoring '
            continue
            
         try:
            sh=json.load(fp);
         except ValueError as e:
            print 'AvsLibsCfg: Error loading JSON file : '+json_file
            raise e
             
         fp.close();
         if (len(sh)>0):
            print 'AvsLibsCfg: Parsing json file '+json_file
            parse_each_json(sh, build_info_set, mod_info, hy22_json);
         else:
            print 'AvsLibsCfg: json file '+json_file+' is empty'
            
      # write what's necessary for build-system
      for buildInfo in build_info_set:    
         #buildInfo.count_build_type(build_type_counter_no_amdb_info,build_type_counter_with_amdb_info);
         buildInfo.populate_build_system(shared_lib_dict, env);
      
      #voice modules fail this check since they add custom modules without building them.
      #check_at_least_one_rev_built(mod_info)
               
      if not is_build_under_hap(env):
         if not only_populate_env:
            # Write QACT XML
            write_qact_xml(qact_xml, mod_info);
            qact_xml.close()
      
            # write C files for all modules.
            write_cpp_files(static_cpp_file, static_header_file, shared_cpp_file, shared_header_file, mod_info);
      
            static_cpp_file.close();
            static_header_file.close();
            shared_cpp_file.close();
            shared_header_file.close();
            
            if hy22_json_file:
               json.dump(hy22_json, hy22_json_file,indent=3);
               hy22_json_file.close();
               
      
      if 'GEN_SHARED_LIBS' in env and not is_unit_test :
         env.Replace(SHARED_LIB_DICT=shared_lib_dict)
         
               
      return shared_lib_dict
      
   #===============================================================================    
   # Core functionality for the scons script
   #=============================================================================== 
   #print env flags
   if 'USES_AVS_SHARED_LIBS' in env:
      print 'AvsLibsCfg: USES_AVS_SHARED_LIBS in env'
   if 'GEN_SHARED_LIBS' in env:  
      print 'AvsLibsCfg: GEN_SHARED_LIBS in env'
   if 'AVS_TEST' in env:
      print 'AvsLibsCfg: AVS_TEST in env'
   if 'COSIM_PROFILING_TEST' in env:
      print 'AvsLibsCfg: COSIM_PROFILING_TEST in env'
   if env.get('hy22') == True:
      print 'AvsLibsCfg: hy22 in env'
   if not is_unit_test and env.GetOption('clean'):
      print 'AvsLibsCfg: clean command'
      
   shared_lib_dict = parse_cfg_and_gen_output(env);

   return shared_lib_dict

#===============================================================================    
# prints shared lib dict  
#===============================================================================      
def print_shared_lib_dict(shared_lib_dict):
   print 'AvsLibsCfg: shared_lib_dict : '
   for item in shared_lib_dict.keys():
      print item, shared_lib_dict[item]
        
#===============================================================================    
# Main function for unit testing     
#===============================================================================    
if __name__ == '__main__':
   from optparse import OptionParser, OptionGroup
   import sys
   usage = r'Parses the json files containing information about AVS libraries.\npython avs_libs_config_parser.py -c msm8996 -f "GEN_SHARED_LIBS,AVS_TEST"'
   parser = OptionParser(description=usage)
   required_group = OptionGroup(parser, "Required Arguments")
   required_group.add_option("-c","--chipset", default=None, help="chipset Eg. msm8994, msm8996") 
   
   opt_group = OptionGroup(parser, "Optional Arguments")
   opt_group.add_option("-f","--flags", default=None, help="Flags Eg. \"GEN_SHARED_LIBS,AVS_TEST,HY22\"") 

   parser.add_option_group(required_group)
   parser.add_option_group(opt_group)
   
   (options, args) = parser.parse_args()

   chipset=options.chipset;
   if None==chipset:
      sys.exit('Insufficient arguments. -h for usage.')
      
   flags = options.flags;
   
   from collections import OrderedDict;
   env = OrderedDict();
   
   #if defined add USES flag.
   if not None == flags:
      flags_list=flags.split(',');
      for i in flags_list:
         if 'GEN_SHARED_LIBS'==i:
            env['USES_AVS_SHARED_LIBS'] = True;
         env[i] = True;
         
   env['CHIPSET'] = chipset;
     
   shared_lib_dict = avs_libs_config_parser(env, True);
   
   if not None==shared_lib_dict:
      print_shared_lib_dict(shared_lib_dict)
