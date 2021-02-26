#===============================================================================
#
# Device Config Builder
#
# GENERAL DESCRIPTION
#    Contains builder(s) definitions
#
# Devcfg Init file and buldiers
#
# Copyright (c) 2011,2014-16 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#===================================================================================
import re
import os
import subprocess
import string
from SCons.Script import *
import types
import xml.dom.minidom
import unicodedata, re

#---------------------------------------------------------------------------
# Class Config holds the parameters to tune the features of DeviceConfig framework
# It automatically parses the image_cfg.xml and self initializes with the desired
# parameters to tune DeviceConfig framework.
#---------------------------------------------------------------------------
#   DevcfgFeatureSet =  {
#                            ImageCfgXML        : <path/to/image_cfg.xml>
#                            devcfgXMLTagList   : [8998_XML, 8996_XML, 8998_PLATFORM_CDP_XML,...]   
#                            targetImg          : <mpss/tz/hyp/oem/adsp>
#                        }
#
class Config:
        
    def __init__(self, env, **kwargs):
        self.DevcfgFeatureSet = kwargs                          #Set to 'true' for multi-cfg implementation
        self.parseImageCfgXML(env)
        
    def parseImageCfgXML(self,env):
        
        image_cfg_dom               = xml.dom.minidom.parse(self.getFeature('ImageCfgXML'))
        
        #Get handle to the tags
        targetImg                   = image_cfg_dom.getElementsByTagName("target_img")
        devcfgTrgtBldTag            = image_cfg_dom.getElementsByTagName("devcfg_target_build_tag")
        dalConfigTgtId              = image_cfg_dom.getElementsByTagName("dal_config_target_id")
        devcfgXmlSingleConfigs      = image_cfg_dom.getElementsByTagName("devcfg_xml_tag_single_configs")
        devcfgXmlMultipleConfigs    = image_cfg_dom.getElementsByTagName("devcfg_xml_tag_multiple_configs")
        devcfgPlatformTags          = image_cfg_dom.getElementsByTagName("devcfg_platform_xml_tags")
        usesOEMConfig               = image_cfg_dom.getElementsByTagName("uses_oem_config")
        oemConfigBaseAddr           = image_cfg_dom.getElementsByTagName("oem_config_base_addr")
        socHwVersion                = image_cfg_dom.getElementsByTagName("soc_hw_version")
        tcsrSsocHwVersionAddr       = image_cfg_dom.getElementsByTagName("tcsr_soc_hw_version_addr")
        tcsrMapping                 = image_cfg_dom.getElementsByTagName("tcsr_paddr_to_vaddr_same")
        multiCfg                    = image_cfg_dom.getElementsByTagName("multiCfg")
        propsMemoryOptimise         = image_cfg_dom.getElementsByTagName("props_memory_optimisation")
        structSize                  = image_cfg_dom.getElementsByTagName("size_of_struct")
        hwVersionSpecificProps      = image_cfg_dom.getElementsByTagName("hw_version_specific_props")
        
        #Extract and validate tag data

        #Identify Target Image
        try:
            targetImgStr = re.sub('\s','',targetImg[0].firstChild.data.encode("ascii")).lower()
        except:
            assert(0), "ERROR : <target_img> tag missing in {}".format(self.getFeature('ImageCfgXML'))
        
        #Identify target image DevCfg build tag
        try:
            devcfgTrgtBldTagStr = re.sub('\s','',devcfgTrgtBldTag[0].firstChild.data.encode("ascii"))
            devcfgTrgtBldTagStr    = devcfgTrgtBldTagStr.upper()
        except:
            assert(0), "ERROR : <devcfg_target_build_tag> tag missing in {}".format(self.getFeature('ImageCfgXML'))
        
        #DAL_CONFIG_TARGET_ID value
        try:
            dalConfigTgtIdVal = re.sub('\s','',dalConfigTgtId[0].firstChild.data.encode("ascii")).lower()
        except:
            assert('x' not in env.get('MSM_ID')), "ERROR: <dal_config_target_id> tag missing in {} file".format(self.getFeature('ImageCfgXML'))
            dalConfigTgtIdVal = '0x${MSM_ID}'
        assert('x' not in dalConfigTgtIdVal[2:]), "<dal_config_target_id> value in file {} cannot have an 'x' in it".format(self.getFeature('ImageCfgXML'))
        
        #Devcfg xml tag list (SOC tags)
        if len(devcfgXmlSingleConfigs):
            devcfgXMLTagList = re.sub('\s','',devcfgXmlSingleConfigs[0].firstChild.data.encode("ascii")).split(',')
        elif len(devcfgXmlMultipleConfigs):
            devcfgXMLTagList = re.sub('\s','',devcfgXmlMultipleConfigs[0].firstChild.data.encode("ascii")).split(',')
        else:
            assert (0),"Either <devcfg_xml_tag_single_configs> or <devcfg_xml_tag_multiple_configs> tag is missing in the Image_cfg.xml file"

        #Devcfg xml tag list (PLATFORM tags)
        devcfgPlatformXMLTagList = []
        platformCfgFlag           = 'false'
        if len(devcfgPlatformTags):
            if (re.sub('\s','',devcfgPlatformTags[0].firstChild.data.encode("ascii")) != ''):
                devcfgPlatformXMLTagList = re.sub('\s','',devcfgPlatformTags[0].firstChild.data.encode("ascii")).split(',')
                platformCfgFlag           = 'true'

        #OEM Parameters
        try:
            usesOEMConfig = re.sub('\s','',usesOEMConfig[0].firstChild.data.encode("ascii")).lower()
            if usesOEMConfig == 'true':
                oemConfigBaseAddrStr = re.sub('\s','',oemConfigBaseAddr[0].firstChild.data.encode("ascii")).lower()
            else:
                oemConfigBaseAddrStr = ''
        except:
            usesOEMConfig = 'false'
            print "WARNING: <usesOEMConfig> tag not provided in file : {}. Defaulting to \'false\'".format(self.getFeature('ImageCfgXML'))
        
        #SocHwVersion Dictionary
        assert (len(socHwVersion) > 0), "<soc_hw_version> tag missing in {} file".format(self.getFeature('ImageCfgXML'))
        socHwVersionNumber = re.sub('\s','',socHwVersion[0].firstChild.data.encode("ascii")).split(',')
        socHwVersionDict = {}
        for element in socHwVersionNumber :
            socHwVersionDict.setdefault(element.split(':')[0],[]).append(element.split(':')[1])         
        tcsrSsocHwVersionAddrVal = re.sub('\s','',tcsrSsocHwVersionAddr[0].firstChild.data.encode("ascii"))  
        
        #tcsr Physical to Virtual mapping flag (1:1 or not)
        try:
            tcsrOneToOneMappingFlag = re.sub('\s','',tcsrMapping[0].firstChild.data.encode("ascii")).lower()
        except:
            assert(0), "ERROR : <tcsr_paddr_to_vaddr_1_to_1_map> tag missing in {}".format(self.getFeature('ImageCfgXML'))
        #MultiCfg Flag
        try:
            multiCfgFlag = re.sub('\s','',multiCfg[0].firstChild.data.encode("ascii")).lower()
        except:
            multiCfgFlag = 'true'
            print "WARNING: <multiCfg> tag not provided in file : {}. Defaulting to \'true\'".format(self.getFeature('ImageCfgXML'))
         
        #Memory OPtimisation Flag
        try:
            propsMemoryOptimise_flag = re.sub('\s','',propsMemoryOptimise[0].firstChild.data.encode("ascii")).lower()
        except:
            propsMemoryOptimise_flag = 'false'
            print "WARNING: <props_memory_optimisation> tag not provided in file : {}. Defaulting to \'false\'".format(self.getFeature('ImageCfgXML'))
        
        #Struct Size Flag
        try:
            structSize_flag = re.sub('\s','',structSize[0].firstChild.data.encode("ascii")).lower()
        except:
            structSize_flag = 'false'
            print "WARNING: <size_of_struct> tag not provided in file {}. Defaulting to \'false\'".format(self.getFeature('ImageCfgXML'))
        
        #Hardware version specific props Flag
        try:
            hwVersionSpecificProps_flag = re.sub('\s','',hwVersionSpecificProps[0].firstChild.data.encode("ascii")).lower()
        except:
            hwVersionSpecificProps_flag = 'false'
            print "WARNING: <hw_version_specific_props> tag not provided in file : {}. Defaulting to \'false\'".format(self.getFeature('ImageCfgXML'))

        #Initialise Feature set
        self.setFeature('DEVCFG_INFO_COLLECTOR',{})              
        self.setFeature('DEVCFG_CURRENT_USED_XML_TAGS',[])              
        self.setFeature('targetImg',targetImgStr)              
        self.setFeature('devcfgImgBuildTag', devcfgTrgtBldTagStr)        
        self.setFeature('dalConfigTargetId', dalConfigTgtIdVal)    
        self.setFeature('devcfgXMLTagList',devcfgXMLTagList)
        self.setFeature('devcfgPlatformXMLTagList',devcfgPlatformXMLTagList)
        self.setFeature('platformCfg',platformCfgFlag)
        self.setFeature('oemCfg',usesOEMConfig)
        self.setFeature('oemCfgBaseAddr',oemConfigBaseAddrStr)
        self.setFeature('socHwVersion',socHwVersionDict)
        self.setFeature('tcsrSsocHwVersionAddr',tcsrSsocHwVersionAddrVal)
        self.setFeature('tcsrPhyToVirSame',tcsrOneToOneMappingFlag)
        self.setFeature('multiCfg',multiCfgFlag)
        self.setFeature('propsMemoryOptimise',propsMemoryOptimise_flag)
        self.setFeature('structSize',structSize_flag)
        self.setFeature('hwVersionSpecificProps',hwVersionSpecificProps_flag)
        
        
    def usesFeature(self, feature):
        return self.getFeature(feature)
        
    def getFeature(self, feature):
        return self.DevcfgFeatureSet.get(feature,None)
        
    def setFeature(self, feature, value):
        self.DevcfgFeatureSet.update({feature:value})

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    """
    Generate function for devcfg builder. 
    Sets up the action, scanner and builder function to be called by clients to
    pass xml, devId details to the builder.
    """
    rootenv = env.get('IMAGE_ENV')
    
    #Allow clients to use DeviceConfig
    rootenv.Replace(USES_DEVCFG = True)
    
    #Set the macros DEVCFG_SCRIPTS and DEVCFG_ROOT
    rootenv.Replace(DEVCFG_SCRIPTS = os.path.dirname(os.path.realpath(__file__)))
    rootenv.Replace(DEVCFG_ROOT = os.path.dirname(rootenv.get('DEVCFG_SCRIPTS')))
    
    #Install AddDevcfgInfo API to the rootenv. Allows clients to pass XML/C config
    rootenv.AddMethod(add_devcfg_info, "AddDevCfgInfo")
    #Returns the actual number of tags used by clients in a multi-cfg setting
    rootenv.AddMethod(get_devcfg_currently_used_tags, "GetDevcfgRelevantTags")
    
    #---------------------------------------------------------------------------
    # Create the DEVCFG object and assign to the rootenv
    #---------------------------------------------------------------------------
    
    image_config_xml = env.subst("${IMAGE_CONFIG_XML}")
    if not os.path.isfile(image_config_xml):
        print('image_config_xml = {}'.format(image_config_xml))
        raise ValueError("ERROR : image_cfg.xml not found !")
        exit()
    
    config = Config(rootenv, ImageCfgXML = image_config_xml)
    rootenv.Replace(DEVCFG = config)

    #OEM Image Specific macros
    
    if 'DAL_DEVCFG_OEM_QSEE_IMG' in env.get('BUILD_TAGS'):
        rootenv.get('DEVCFG').setFeature('devcfgImgBuildTag','DAL_DEVCFG_OEM_QSEE_IMG')
        rootenv.get('DEVCFG').setFeature('targetImg','oem_tz')
    elif 'DAL_DEVCFG_OEM_HYP_IMG' in env.get('BUILD_TAGS'):
        rootenv.get('DEVCFG').setFeature('devcfgImgBuildTag','DAL_DEVCFG_OEM_HYP_IMG')
        rootenv.get('DEVCFG').setFeature('targetImg','oem_hyp')
    
    #Add 'devcfgImgBuildTag' to the image BUILD_TAGS
    devcfg_build_tag = env.get('DEVCFG').getFeature('devcfgImgBuildTag')
    env.AppendUnique(BUILD_TAGS = [devcfg_build_tag])
    env.Dictionary().setdefault(devcfg_build_tag,env.get('PROC'))
    
    env.Append(CPPDEFINES = ["DEVCFG_TARGET_" + (env.get('DEVCFG').getFeature('targetImg')).upper()])
    if env.get('DEVCFG').getFeature('tcsrPhyToVirSame') == 'true':
        env.Append(CPPDEFINES = ["TCSR_ADDR_PHS_SAME_AS_VIR"])
    if env.get('DEVCFG').getFeature('multiCfg') == 'true':
        env.Append(CPPDEFINES = ["DEVCFG_MULTICFG_ENABLED"])
    if env.get('DEVCFG').getFeature('platformCfg') == 'true':
        env.Append(CPPDEFINES = ["DEVCFG_MULTIPLATFORM_ENABLED"])
    if env.get('DEVCFG').getFeature('oemCfg') == 'true':
        env.Append(CPPDEFINES = ["DEVCFG_OEM_ENABLED"])
        
    env.Append(CPPDEFINES = ["DAL_CONFIG_TARGET_ID="+env.get('DEVCFG').getFeature('dalConfigTargetId')])
    env.Replace(DAL_CONFIG_TARGET_ID = env.get('DEVCFG').getFeature('dalConfigTargetId'))
    
    # Create the preprocess tag list to be used in devcfg_builder.py
    devcfg_xml_dict = {}
    for xml_tag in env.get('DEVCFG').getFeature('devcfgXMLTagList'):
        devcfg_xml_dict[xml_tag] = 'pp_' + xml_tag
    rootenv.Replace(DEVCFG_XML_DICT = devcfg_xml_dict)
        
    #Install all builders
    devcfg_img_header_file_auto_generate(env)
    devcfg_xml_auto_generate(env)
    devcfg_c_auto_generate(env)
    devcfg_c_struct_generate(env)
    
    return

def devcfg_img_header_file_auto_generate(env):
    # load builder into environment
    env.Append(BUILDERS = {'GenerateDevcfgImgHdr' : Builder(action=devcfg_img_header_file_builder,
                                                                action_source=None)})
#===============================================================================
# DEV CFG STRUCT FILE Install Rules
#===============================================================================
def devcfg_c_struct_generate(env):  
    # load builder into environment
    env.Append(BUILDERS = {'QualifyStructNames' : Builder(action=devcfg_qualify_struct_name_builder)})
    
#===============================================================================
# DEV CFG STRUCT FILE Install Rules
#===============================================================================
def devcfg_c_auto_generate(env):
    
    # load builder into enviroment
    devcfg_c_bld = env.Builder(action = devcfg_c_builder, action_source=None)
    env.Append(BUILDERS = {'GenerateCFile' : devcfg_c_bld})

#===============================================================================
# DEV CFG XML build rules
#===============================================================================
def devcfg_xml_auto_generate(env):
    
    # load builder into enviroment
    # Set action_source to None as there are no sources passed into the action function. 
    # If this isn't set explicitly, then build command with verbose=2 fails.
    devcfg_target_scan = env.Scanner(devcfg_target_scan_func, name='DevCfgTargetScanner')
    devcfg_bld = env.Builder(action = devcfg_xml_builder, action_source=None,
                             target_scanner = devcfg_target_scan
                             #suffix = '.xml'
                             )
    env.Append(BUILDERS = {'DevcfgBuilder' : devcfg_bld})
    
    return

def devcfg_img_header_file_builder(target, source, env):
    fp = open(str(target[0]),'w')
    image_name = env.get('DEVCFG').getFeature('targetImg')
    fp.write('extern DEVCFG_TARGET_INFO devcfg_target_soc_info_'+image_name+'[];\n')
    fp.write('#define devcfg_target_soc_info devcfg_target_soc_info_'+image_name+'\n')
    fp.close()
    return
    
def devcfg_c_builder(target, source, env):    
    xml_tag = env['XML_TAG'] 
    c_output_file = str(target[0])
    c_input_file = str(source[0])
    dir_name,file_name = os.path.split(c_output_file)
    dir_name = os.path.realpath(dir_name)
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)
    file_src_fp = open(c_input_file,"r")
    file_dest_fp = open(c_output_file,"w")
    for i in range(len(source)-1):
      file = os.path.realpath(str(source[i+1]))
      file_dest_fp.write('#include "' + '..' + file.replace('\\','/').split(env['BUILDPATH'].replace('\\','/') +'/'+xml_tag)[1] + '"\n')
    file_dest_fp.write(file_src_fp.read())
    file_src_fp.close()
    file_dest_fp.close()     
    return 
    
def devcfg_target_scan_func(node, env, path):
    """
    Scanner adds the include paths to dal config environment which will 
    be used to compile autogenerated c files while rebuilding. This gets
    called from the dal config scons environment.
    """
    
    image_tag  = env['IMAGE_TAG']
    pp_xml_tag  = 'pp_' + env['XML_TAG']
    cc_xml_tag  = 'cc_' + env['XML_TAG']
    
    pp_xml_file_node_list = env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR').get(image_tag,{}).get(pp_xml_tag,[])
    cc_obj_file_node_list = env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR').get(image_tag,{}).get(cc_xml_tag,[])
    for pp_xml_file_node in pp_xml_file_node_list:
        env.Depends(node, pp_xml_file_node)
    for cc_obj_file_node in cc_obj_file_node_list:
        env.Depends(node, cc_obj_file_node)
    
       
    #Instead of returning any nodes just add the PP XMLs and C Obj FILE as deps
    #Before building master xml, define rules to build preprocess and structure header
    
    return []

def get_devcfg_currently_used_tags(env, targets, thread_input_dict):
    rootenv = env.get('IMAGE_ENV')
    return env.get('DEVCFG',None).getFeature('DEVCFG_CURRENT_USED_XML_TAGS')

def devcfg_qualify_struct_name_emitter(target, source, env):
    return (target, source)    
    
def devcfg_qualify_struct_name_builder(target,source,env):
    dir_name,file_name = os.path.split(str(target[0]))
    dir_name = env.RealPath(dir_name)
    if not os.path.exists(dir_name):
       os.makedirs(dir_name)
    fp = open(str(target[0]),"w")
    suffix = env['SUFFIX']
    xmlfile_fullpath = str(source[0])  
    struct_name_list = extract_c_struct_variable_from_xml(xmlfile_fullpath)
    for struct in struct_name_list:
        fp.write('#define '+struct+' '+struct+'_'+suffix+'\n')
    fp.close()
    return 

               
def extract_c_struct_variable_from_xml(xml_file):
    unique_struct_list = []
    temp_line = ""
    inside = 0
    xml_file_fp = open(xml_file, "r")
    for line in xml_file_fp:
        line = line.strip()
        if line.startswith('<props') and ((line.find('DALPROP_ATTR_TYPE_STRUCT_PTR') != -1) or (line.find('0x00000012') != -1)):
            temp_line = line
            if (line.endswith('</props>') == False):
                inside = 1
                continue
            else:
                struct_name = find(temp_line)
                if (struct_name != None) and (struct_name not in unique_struct_list):
                    unique_struct_list.append(struct_name)
        if (inside == 1):
            temp_line += line
            if (line.endswith('</props>') == False):
                continue
            else:
                inside = 0
                struct_name = find(temp_line)
                if (struct_name != None) and (struct_name not in unique_struct_list):
                    unique_struct_list.append(struct_name)
        
    return unique_struct_list
  
def find(xml_node):
    temp_line = ""
    line_parts = xml_node.split('>',1)
    value = line_parts[1].split('<',1)
    struct_name =  value[0].strip()
    return struct_name

def resolve_platform_all_tag(env,thread_input_dict,tag_list):
    plat_key = 'NULL'
    plat_value =[]
    soc = 'NULL'
    for key,value in thread_input_dict.items():
        if 'PLATFORM_ALL' in key or 'platform_all' in key:
           plat_key = key
           plat_value = value
           soc = key.split('_')[0]
    if plat_key is not 'NULL' and soc is not 'NULL':    
        plat_tag_list = []
        for tags in tag_list:
          #find all platform tags for that soc
          if 'PLATFORM' in tags and soc in tags:
             plat_tag_list.append(tags) 
        for plat in plat_tag_list:
          thread_input_dict[plat] = plat_value 
        del thread_input_dict[plat_key]   
    return thread_input_dict  
    
def resolve_soc_all_tag(env,thread_input_dict,tag_list):
    key = 'NULL'
    if thread_input_dict.has_key('SOC_XML'):
       key = 'SOC_XML'
    elif thread_input_dict.has_key('soc_xml'):
       key = 'soc_xml'   
    elif thread_input_dict.has_key('devcfg_xml'):
        key = 'devcfg_xml'
        
    if key is not 'NULL':   
       soc_list = []
       for tags in tag_list:
          if 'PLATFORM' not in tags:
             soc_list.append(tags)        
       cfglist = thread_input_dict[key]
       if(type(cfglist) is not list):
          cfglist = [cfglist]
       for soc in soc_list:
          thread_input_dict[soc] = cfglist
       del thread_input_dict[key]     
    return thread_input_dict  

def find_file_name_list(env,thread_input_dict,each_xml_tag,ext):  
    file_list = []
    for file in  thread_input_dict[each_xml_tag]:
         fullpath = env.RealPath(str(file))
         fname = os.path.basename(fullpath)
         fname_ext = os.path.splitext(fname)[1]
         if ext.lower() in fname_ext.lower():
            file_list.append(fullpath)
    return set(file_list)

def check_and_ignore_xml_file_duplicates(env,xml_file_list,each_xml_tag,target_image):
    exisiting_pp_xml_node_list = env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR').get(target_image,{}).get('pp_'+each_xml_tag,[])
    exisiting_pp_xml_file_list = [node.name for node in exisiting_pp_xml_node_list]
    for xml_file in xml_file_list:
        if ((os.path.basename(xml_file) + '.i') in exisiting_pp_xml_file_list):
            env.PrintWarning("Duplicate Device Configuration Input XML File Specified " + xml_file)
            xml_file_list.remove(xml_file)
    return xml_file_list 

def add_devcfg_info(env, targets, thread_input_dict):
    """
    When AddDevCfgInfo is called by the client, this add_devcfg_xml gets invoked in the
    devcfg builder env & the details passed by the clients are stored in a dictionary here.
    """

    #Add Preprocessor support for ARM targets
    env = env.Clone()
    env.Replace(PPASM_FILE = env.RealPath("${DEVCFG_SCRIPTS}/ppasm.py"))
    env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE}")
    env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
    env.Replace(ARMCC_PREPROCESS_CMD = "-E -xc")
    
    rootenv = env.get('IMAGE_ENV')
    
    #If this target doesn't use devcfg, bail out
    if env.GetUsesFlag('USES_DEVCFG') is False:
        return    
    
    if type(targets) is not list:
       targets = [targets]
       
    #Is the Config passed to this API relevant to this image ?
    if not len(set(targets) & set(env.get('BUILD_TAGS'))):
        return
    
    tag_list        = env.get('DEVCFG').getFeature('devcfgXMLTagList')
    devcfg_target   = env.get('DEVCFG').getFeature('targetImg')
    tag_dicts       = env.get('DEVCFG_XML_DICT')
    
    if tag_list is None:
        err_str = 'DEVCFG_XML_TAGS are not found. Check build\ms\image_cfg.xml to make sure tags have been added'
        err_str += 'Also, check tbc_core_devcfg.py to make sure these are being generated\n'
        devcfg_error(env, err_str)
        
    #resolve any soc_all tag    
    thread_input_dict = resolve_soc_all_tag(env,thread_input_dict,tag_list)

    for target_image in targets:
        if target_image not in env.get('BUILD_TAGS'):
            continue
        for each_xml_tag in tag_list:     
            pp_xml_tag = 'pp_'+each_xml_tag
            # Dictionary to hold dal device id and device config xml locations
            if each_xml_tag in thread_input_dict:
                xml_file_list = find_file_name_list(env,thread_input_dict,each_xml_tag,'.xml')
                xml_file_list = check_and_ignore_xml_file_duplicates(env,xml_file_list,each_xml_tag,target_image)
                sources_c_file_gen_builder = []
                for file in xml_file_list:
                   #Obtain the parent directory name of the file
                   path, xml_fname = os.path.split(file)
                   residue, dir_name = os.path.split(path)
                   #Obtain just the file name + ext
                   xml_fname_no_ext = os.path.splitext(xml_fname)[0]
                   #Pre-process the XML
                   pp_xml = env.PreProcess('${BUILDPATH}/' + each_xml_tag + '/' + dir_name + '/'+ xml_fname + '.pp', file)
                   #Cleanup the Preprocessed XML
                   pp_xml_cl = env.PreProcessClean('${BUILDPATH}/' + each_xml_tag + '/' + dir_name + '/' + xml_fname + '.i', pp_xml)
                   env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR').setdefault(target_image,{}).setdefault(pp_xml_tag,[]).append(pp_xml_cl[0])
                   devcfg_struct_qual_name = env.RealPath('${BUILDPATH}/' + each_xml_tag +'/'+ dir_name +'/'+'devcfg_'+xml_fname_no_ext+'.h')
                   str_tag = each_xml_tag.replace('xml','')+ devcfg_target
                   devcfg_struct_qual = env.QualifyStructNames(devcfg_struct_qual_name,pp_xml_cl,SUFFIX=str_tag)
                   sources_c_file_gen_builder.append(devcfg_struct_qual)
                
                c_file_list = find_file_name_list(env,thread_input_dict,each_xml_tag,'.c')
                cc_xml_tag = pp_xml_tag.replace('pp','cc')
                for file in c_file_list:
                    #Obtain the parent directory name of the file
                    path, c_fname = os.path.split(file)
                    residue, dir_name = os.path.split(path)
                    build_path = env.RealPath('${BUILDPATH}/' + each_xml_tag+'/')
                    if not os.path.exists(build_path):
                        os.makedirs(build_path)
                    pp_cfile = env.PreProcess(build_path + '/' + dir_name + '/' + c_fname + '.pp', file)
                    pp_cfile_cl = env.PreProcessClean(build_path + '/' + dir_name + '/' + c_fname  +'.i', pp_cfile)
                    file_prefix = each_xml_tag.replace('xml','devcfg')
                    sources_c_file_gen_builder.insert(0,pp_cfile_cl)
                    config_c_file = env.GenerateCFile(build_path + '/' + dir_name + '/' + file_prefix +'_'+c_fname,sources_c_file_gen_builder,XML_TAG=each_xml_tag,IMAGE_TAG=target_image)
                    env_no_warn = env.Clone()
                    if env.Dictionary().has_key('HEXAGON_WARN'):
                        env_no_warn['HEXAGON_WARN'] += ' -Wno-unused-function'
                    config_obj_file = env_no_warn.AddObject([target_image],config_c_file[0])
                    env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR').setdefault(target_image,{}).setdefault(cc_xml_tag,[]).append(config_obj_file[0])
                    sources_c_file_gen_builder.pop(0)              
                
            if each_xml_tag not in env.get('DEVCFG').getFeature('DEVCFG_CURRENT_USED_XML_TAGS'):
                env.get('DEVCFG').getFeature('DEVCFG_CURRENT_USED_XML_TAGS').append(each_xml_tag)

    return
    

def devcfg_xml_builder(target, source, env):
    """
    devcfg_xml_builder gets invoked as its a rule in dal\config\SConscript to create the master xml file
    and DALDeviceId_autogen.h.
    The master xml file will be the #includes of all the xml files passed by the clients.
    target[0] will have the master xml location & name.
    """
    
    if env.GetUsesFlag('USES_DEVCFG') is False:
        return None
        
    # Creation of master dal xml file
    xml_filepathname    = env.RealPath(str(target[0]))
    env_filepathname    = env.RealPath(str(target[1]))    
    xml_tag             = env.get('XML_TAG','')   
    image_tag           = env.get('IMAGE_TAG','DAL_DEVCFG_IMG') 
    update_env          = env.get('UPDATE_ENV','True')

    if image_tag not in env.get('BUILD_TAGS'):
        errStr = 'Could not create DevCfg Master XML file for Image tag : {}'.format(image_tag)
        devcfg_error(env, errStr)
    
    moduleNameStr = '<module name="devcfg">\n'
    
    # Create the master xml file and open for writing
    try:
        devcfg_xml_file = open(xml_filepathname, 'w')
        devcfg_env_file = open(env_filepathname, 'w')
    except IOError:
        errStr = 'Could not create DevCfg Master XML file' + filepathname
        devcfg_error(env, errStr)
    
    
    HeaderList = []
    HeaderPath = []
    
    # Add the include files and the xml header tags
    devcfg_xml_file.write('#include "DALPropDef.h"\n')
    devcfg_xml_file.write('#include "DALDeviceId.h"\n')
    devcfg_xml_file.write('<?xml version="1.0"?>\n')
    devcfg_xml_file.write('<dal>\n')
    devcfg_xml_file.write(moduleNameStr)
    
    # Write all the pp xml files in the Master_xml file
    if image_tag in env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR').keys():
        config_files_dict = env.get('DEVCFG').getFeature('DEVCFG_INFO_COLLECTOR').get(image_tag)
        xml_file_list = config_files_dict.get(str('pp_' + xml_tag))
        for  each_xml_file in xml_file_list:
            path = each_xml_file.path
            if '.xml.i' in path :
                path_pp_file = re.sub(r'\.i$','.pp',path)
                #get Header information (Header files and their Paths) from the corresponding .pp files
                pp_xml_file_fp = open(str(path_pp_file),"r")
                hdr_path_line = ""
                for line in pp_xml_file_fp:
                    if line.startswith('#') and '.h' in line:
                        hdr_path_line = line.split('"')[1]
                        hdr_path_line = os.path.realpath(hdr_path_line)
                        build_root = str(env.get('BUILD_ROOT')).replace('\\', '/')
                        hdr_path_line_new = hdr_path_line.replace('\\', '/')
                        hdr_path, hdr_file = os.path.split(hdr_path_line_new)
                        if '.h' in hdr_file.lower() and build_root.lower() in hdr_path_line_new.lower() :
                            hdr_file = 'enum_header_path "' + hdr_file + '"\n'
                            if hdr_file not in HeaderList:
                                HeaderList.append(hdr_file)
                                devcfg_xml_file.write(hdr_file)
                            if hdr_path not in HeaderPath:
                                HeaderPath.append(hdr_path)
                                devcfg_env_file.write(hdr_path + "\n")
                pp_xml_file_fp.close()
                devcfg_xml_file.write(each_xml_file.get_contents())
                devcfg_xml_file.write('\n')
    

    # Write the closing tags
    devcfg_xml_file.write('</module>' + '\n')
    devcfg_xml_file.write('</dal>\n') 
   
    # Close the Master XML file
    devcfg_xml_file.close()
    devcfg_env_file.close()
    #Store Header Paths extracted to environment
    if(update_env == True) :
       env.get('CPPPATH').extend(HeaderPath)
    
    return
    

def devcfg_error(env, errStr, Info=None):
    """
    Error handler for devcfg framework. errStr can be a string that describes the error and
    Info can be any data that needs to be printed along with the error message.
    """
    env.PrintError("DevCfg error found: " + errStr)
    env.PrintError(Info)
    raise Exception(errStr)


