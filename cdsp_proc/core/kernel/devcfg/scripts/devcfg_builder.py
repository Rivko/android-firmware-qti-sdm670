#===============================================================================
# Device Config Builder
#    Contains builder(s) definitions
# Copyright (c) 2011,2014-16 by Qualcomm Technologies Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===================================================================================
import re
import os
import subprocess
import string
from SCons.Script import *
import types
import shutil
import SCons.Scanner

#cuz scons isnt python, and doesnt know where the hell i am
import imp
image_config = imp.load_source('image_config',os.path.dirname(os.path.realpath(__file__))+'/image_config.py')
devcfg_input = imp.load_source('devcfg_input',os.path.dirname(os.path.realpath(__file__))+'/devcfg_input.py')
propgen = imp.load_source('propgen',os.path.dirname(os.path.realpath(__file__))+'/propgen.py')

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    rootenv = env.get('IMAGE_ENV')
    
    #Allow clients to use DeviceConfig
    rootenv.Replace(USES_DEVCFG = True)
    rootenv.Replace(USES_DEVCFG_STRICT_XML = True)
    
    #Set the macros DEVCFG_SCRIPTS and DEVCFG_ROOT
    rootenv.Replace(DEVCFG_SCRIPTS = os.path.dirname(os.path.realpath(__file__)))
    rootenv.Replace(DEVCFG_ROOT = os.path.dirname(rootenv.get('DEVCFG_SCRIPTS')))
    #rootenv.Replace(DEVCFG_CPPPATH = env.get('CPPPATH'))
    
    #Install AddDevcfgInfo API to the rootenv. Allows clients to pass XML/C config
    rootenv.AddMethod(add_devcfg_info, "AddDevCfgInfo")
    #Returns the actual number of tags used by clients in a multi-cfg setting
    rootenv.AddMethod(get_devcfg_currently_used_tags, "GetDevcfgRelevantTags")

    env.AddMethod(buildDevcfgData, "buildDevcfgData")
    
    image_config_xml = env.subst("${IMAGE_CONFIG_XML}")
    if not os.path.isfile(image_config_xml):
        raise ValueError("ERROR : image_cfg.xml not found !")
        exit()
    
    config = image_config.Config(image_config_xml)
    rootenv.Replace(DEVCFG = config)
    rootenv.Replace(DEVCFG_SOURCES = [])
    env.Replace(PROPGEN_CMD = "${DEVCFG_SCRIPTS}/propgen.py")

    #Add 'devcfgImgBuildTag' to the image BUILD_TAGS
    devcfg_build_tag = config.devcfgImgBuildTag
    if devcfg_build_tag not in env.get('BUILD_TAGS'):
        env.get('BUILD_TAGS').append(devcfg_build_tag)
        env.Dictionary().setdefault(devcfg_build_tag,env.get('PROC'))
    
    env.Append(CPPDEFINES = ["DEVCFG_TARGET_" + (config.targetImg).upper()])
    if config.tcsrPhyToVirSame == 'true':
        env.Append(CPPDEFINES = ["TCSR_ADDR_PHS_SAME_AS_VIR"])
    if config.multiCfg == 'true':
        env.Append(CPPDEFINES = ["DEVCFG_MULTICFG_ENABLED"])
    if config.devcfgPlatformXMLTagList != []:
        env.Append(CPPDEFINES = ["DEVCFG_MULTIPLATFORM_ENABLED"])
    if config.oemCfgBaseAddr != '':
        env.Append(CPPDEFINES = ["DEVCFG_OEM_ENABLED"])
        
    env.Append(CPPDEFINES = ["DAL_CONFIG_TARGET_ID="+config.dalConfigTargetId])
    env.Replace(DAL_CONFIG_TARGET_ID = config.dalConfigTargetId)
        
    #Install all builders
    devcfg_img_header_file_generate(env)
    devcfg_xml_generate(env)
    structPtrHeader_generate(env)
    dal_moddir_src_generate(env)
    devcfgDataSrc_generate(env)
    devcfgManifest_generate(env)
    devcfgIncPaths_generate(env)

    return
   
def devcfg_img_header_file_generate(env):
    env.Append(BUILDERS = {'GenerateDevcfgImgHdr' : Builder(action=devcfg_img_header_file_builder, action_source=None)})

def devcfg_img_header_file_builder(env, target, source):
    fp = open(str(target[0]),'w')
    image_name = env.get('DEVCFG').targetImg
    fp.write('extern DEVCFG_TARGET_INFO devcfg_target_soc_info_'+image_name+'[];\n')
    fp.write('#define devcfg_target_soc_info devcfg_target_soc_info_'+image_name+'\n')
    fp.close()
    return

def devcfg_target_scan_func(node, env, path):
    rootenv = env.get('IMAGE_ENV')
    devcfgInput = rootenv.get('DEVCFG_SOURCES')
    for i in devcfgInput:
        env.Depends(node, i.file)
    return []

def get_devcfg_currently_used_tags(env, targets, input_dict):
    rootenv = env.get('IMAGE_ENV')
    return env.get('DEVCFG',None).get('DEVCFG_CURRENT_USED_XML_TAGS',None)

#------------------------------------------------------------------------------
# Add client input API
#------------------------------------------------------------------------------
def add_devcfg_info(env, images, input_dict):
    #record tag, file(s), cpppath
    env = env.Clone()
    rootenv = env.get('IMAGE_ENV')

    image_config_xml = env.subst("${IMAGE_CONFIG_XML}")
    if not os.path.isfile(image_config_xml):
        raise ValueError("ERROR : image_cfg.xml not found !")
        exit()

    config = image_config.Config(image_config_xml)
    tag_list = config.devcfgXMLTagList

    new_dict = {}
    key = (input_dict.keys())[0]
    if key in ['SOC_XML','soc_xml','devcfg_xml']:
       for tag in tag_list:
          if 'PLATFORM' not in tag:
             new_dict[tag] = input_dict[key]
    else:
        new_dict = input_dict
    input_dict = new_dict

    if isinstance(images, str):
        raise ValueError("ERROR: Invalid Image list specified when adding Device Config Input")
        return -1
    for image in (set(images) & set(env.get('BUILD_TAGS'))):
        for xml_tag in tag_list:
            if xml_tag in input_dict:
                if isinstance(input_dict[xml_tag], str):
                    raise ValueError("ERROR: device config input dictionary expects a list as its value")
                    return -1
                for file in input_dict[xml_tag]:
                    if os.path.splitext(file)[1] == '.c':
                        structHeaaderPath = "$DEVCFG_ROOT/build/${{BUILDPATH}}/data/{}/devcfgStructs.h".format(xml_tag)
                        env.Append(CCFLAGS = " -include {} ".format(structHeaaderPath))
                        thisobj = env.AddObject(image, file)
                        env.Depends(thisobj, structHeaaderPath)
                    rootenv.Append(DEVCFG_SOURCES = [devcfg_input.DevcfgInput(xml_tag,os.path.realpath(env.subst(file)),[],env.get('CPPPATH'))])

#------------------------------------------------------------------------------
# Devcfg Manifest Builder
#------------------------------------------------------------------------------
def devcfgManifest_generate(env):
    env.Append(BUILDERS = {'devcfgManifest' : env.Builder(action = devcfgManifestBuilder, target_scanner=env.Scanner(devcfg_target_scan_func))})

def devcfgManifestBuilder(env, target, source):
    manifestFH = open(str(target[0]), "w")
    fileList = getDevcfgClientData(env, '')[0]
    for inputFile in fileList:
        manifestFH.write(inputFile+"\n")
    manifestFH.close()

#------------------------------------------------------------------------------
# Devcfg Manifest Builder
#------------------------------------------------------------------------------
def devcfgIncPaths_generate(env):
    env.Append(BUILDERS = {'devcfgIncpaths' : env.Builder(action = devcfgIncPathsBuilder, target_scanner=env.Scanner(devcfg_target_scan_func))})

def devcfgIncPathsBuilder(env, target, source):
    incpathsFH = open(str(target[0]), "w")
    incPaths = getDevcfgClientData(env, '')[1]
    for incPath in incPaths:
        incpathsFH.write("-I{} ".format(env.subst(incPath).replace("\\","/")))
    incpathsFH.close()

#------------------------------------------------------------------------------
# Devcfg Master XML Builder
#------------------------------------------------------------------------------
def devcfg_xml_generate(env):
    env.Append(BUILDERS = {'devcfgMasterXml' : env.Builder(action = devcfg_xml_builder, target_scanner=env.Scanner(devcfg_target_scan_func))})

def devcfg_xml_builder(env, target, source):
    xml_file_list = getDevcfgClientData(env, '.xml')[0]
    return propgen.genMasterXML(env.RealPath(str(target[0])), xml_file_list)

#------------------------------------------------------------------------------
# Devcfg Data Source Builder
#------------------------------------------------------------------------------
def structPtrHeader_generate(env):
    env.Append(BUILDERS = {'structPtrHeader' : env.Builder(action = structPtrHeaderBuilder, target_scanner=env.Scanner(devcfg_target_scan_func))})

def structPtrHeaderBuilder(env, target, source):
    (memory_optimise, struct_size, config) = getPropgenCommonArgs (env)
    xml_file_list = getDevcfgClientData(env, '.xml')[0]
    inputStr=""
    for xmlFile in xml_file_list:
        inputStr += " -i {}".format(xmlFile)
    cmd2 = "python ${{PROPGEN_CMD}} --StructHeader={}{} --ConfigType={} {} {}".format(target[0], inputStr, config, memory_optimise, struct_size)
    print env.subst(cmd2)
    return subprocess.call(env.subst(cmd2).split())

#------------------------------------------------------------------------------
# Build the devcfg library (put it all together
#------------------------------------------------------------------------------
def buildDevcfgData(env, blobname):
    targetPathBaseStr = "${{BUILDPATH}}/{}/{}"
    #generate Manifest
    manifest = env.devcfgManifest(targetPathBaseStr.format(blobname,'manifest.txt'), [])
    #generate Incpaths
    incPaths = env.devcfgIncpaths(targetPathBaseStr.format(blobname,'incpaths.txt'), [])
    #call builder for masterxml
    masterXml = env.devcfgMasterXml(targetPathBaseStr.format(blobname,'master.xml'), [])
    #call builder for structheaderfile
    structHeader = env.structPtrHeader(targetPathBaseStr.format(blobname,'devcfgStructs.h'), [], CONFIG=blobname)
    #call builder to compile masterxml
    devcfgDataSrc = env.DALConfigSrcBuilder([targetPathBaseStr.format(blobname,'devcfg.c'), targetPathBaseStr.format(blobname,'devcfg_data.c')], [masterXml], CONFIG=blobname)
    #generate library from objects above
    cenv = env.Clone()
    cenv.Append(CCFLAGS = " @"+str(incPaths[0]))
    devcfgLib = cenv.Library(targetPathBaseStr.format(blobname,'devcfg.lib'), [devcfgDataSrc])
    env.Depends(devcfgDataSrc, incPaths)
    env.Depends(devcfgDataSrc, manifest)
    #devcfgLib = env.DevcfgLibrary(targetPathBaseStr.format(blobname,'devcfg.lib'), [devcfgDataSrc])
    return (devcfgLib, masterXml)

#------------------------------------------------------------------------------
# DAL ModDir Builder
#------------------------------------------------------------------------------
def dal_moddir_src_generate(env):
    DALModDir_src_bld = env.Builder(action = DALModDir_src_builder, source_scanner = SCons.Scanner.C.CScanner(), suffix = '.c', src_suffix = '.xml')
    env.Append(BUILDERS = {'DALModDirSrcBuilder' : DALModDir_src_bld})
   
def DALModDir_src_builder(env, target, source):
    (memory_optimise, struct_size, config) = getPropgenCommonArgs (env)
    cmd = "python ${{PROPGEN_CMD}} --input={} --ModDirFile={} --ConfigType={} --RemoveDups {} {}".format(source[0], str(target[0]), config, memory_optimise, struct_size)
    print env.subst(cmd)
    return subprocess.call(env.subst(cmd).split())
   
#------------------------------------------------------------------------------
# Devcfg Data Source Builder
#------------------------------------------------------------------------------
def devcfgDataSrc_generate(env):
    env.Append(BUILDERS = {'DALConfigSrcBuilder' : env.Builder(action = devcfgDataSrcBuilder)})

def devcfgDataSrcBuilder(env, target, source):
    (memory_optimise, struct_size, config) = getPropgenCommonArgs (env)
    cmd = "python ${{PROPGEN_CMD}} --input={} --ConfigFile={} --DevcfgDataFile={} --ConfigType={} {} {}".format(source[0], str(target[0]), str(target[1]), config, memory_optimise, struct_size)
    print env.subst(cmd)
    return subprocess.call(env.subst(cmd).split())


#------------------------------------------------------------------------------
# helper functions
#------------------------------------------------------------------------------
def getPropgenCommonArgs (env):
    memory_optimise = ""
    if env.get('DEVCFG').propsMemoryOptimise:
        memory_optimise = "--MemoryOptimiseProps"

    struct_size = ""
    if env.get('DEVCFG').structSize:
        struct_size = "--structSize"

    config = env.get('CONFIG')
    if not config:
        config = ""

    return (memory_optimise, struct_size, config)

def getDevcfgClientData(env, extFilter):
    file_list = []
    incPaths = []
    rootenv = env.get('IMAGE_ENV')
    for i in rootenv.get('DEVCFG_SOURCES'):
        if extFilter == '' or os.path.splitext(i.file)[1] == extFilter:
            file_list.append(i.file)
            incPaths = list(set(i.includes) | set(incPaths))
    return file_list, incPaths
