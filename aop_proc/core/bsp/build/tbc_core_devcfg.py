#==============================================================================
# SCONS Tool for Target-Specific Build Parameters.
#
# Copyright (c) 2012-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#==============================================================================
#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/bsp/build/tbc_core_devcfg.py#1 $
#
# when       who    what, where, why
# -------    ---    -------------------------------------------
# 08/14/12   aa   Initial Revision
#==============================================================================
import xml.dom.minidom
import unicodedata

def get_devcfg_xml_tags(env, xml_file):
   '''
   Get the devcfg tags that the clients need to use to pass in the xml information
   for different configurations by reading it from build\ms\image_cfg.xml
   '''
   if xml_file == '':
     return []
   xml_file = env.RealPath(xml_file)
   # parse the xml file
   image_cfg_tag = xml.dom.minidom.parse(xml_file)
   # Get the devcfg_info elements
   devcfginfotags = image_cfg_tag.getElementsByTagName("devcfg_info")
   #import pdb;pdb.set_trace()
   # Get devcfg_xml_tag's elements, which has the xml tags in it that is required here
   # if env.GetUsesFlag('USES_DEVCFG_MULTIPLE_TAGS'):
      # devcfgxmlelems = devcfginfotags[0].getElementsByTagName("devcfg_xml_tag_multiple_configs")
   # else:
      # devcfgxmlelems = devcfginfotags[0].getElementsByTagName("devcfg_xml_tag_single_config")
   #import pdb; pdb.set_trace()
   devcfgxmlelems = devcfginfotags[0].getElementsByTagName("devcfg_xml_tag_multiple_configs")
   # This will have only one child of devcfg_xml_tag type. It returns a unicode type data
   udata = devcfgxmlelems[0].firstChild.data
   # Convert it to string
   devcfg_xml_tags = unicodedata.normalize('NFKD', udata).encode('ascii','ignore')
   # Remove any \t, \n in the string and split it with demiliter ',' to give a list of the tags
   devcfg_xml_tag_list = (devcfg_xml_tags.replace('\t', '').replace('\n', '')).split(',')
   # Remove any spaces in the list elements
   devcfg_xml_tag_list = [word.strip() for word in devcfg_xml_tag_list]
   
   return devcfg_xml_tag_list

def generate(env):
   #---------------------------------------------------------------------------
   # Add Device Configuration specific defines
   #---------------------------------------------------------------------------
   # Reason for doing this is because python does 
   # not like the 'x' in targets like '9x15'
   if env['MSM_ID'] in ['8996']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8996')
      multi_config_xml = "${BUILD_ROOT}/msm8996/image_cfg.xml"
   elif env['MSM_ID'] in ['8998']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8998')
      multi_config_xml = "${BUILD_ROOT}/msm8998/image_cfg.xml"
   elif env['MSM_ID'] in ['8994']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8994')
      multi_config_xml = "${BUILD_ROOT}/msm8994/image_cfg.xml"
   elif env['MSM_ID'] in ['8094']:
      env.Replace(DAL_CONFIG_TARGET_ID = '0x8094')	  
      multi_config_xml = "${BUILD_ROOT}/apq8094/image_cfg.xml"
   else:
      #env.Replace(DAL_CONFIG_TARGET_ID = '0x${MSM_ID}')
      #raise Exception("Dev Config XML to parse configuration tags not found for 0x${MSM_ID}")
      multi_config_xml = ''
      print "Dev Config XML to parse configuration tags not found, skipping"

   devcfg_xml_tag_list  = get_devcfg_xml_tags(env, multi_config_xml)
   # Create the preprocess tag list to be used in devcgh_builder.py
   devcfg_xml_dict = {}
   for word in devcfg_xml_tag_list:
      devcfg_xml_dict[word] = 'pp_' + word
   #devcfg_xml_pp_tag_list = {word:('pp_' + word) for word in devcfg_xml_tag_list}
   env.Replace(DEVCFG_XML_TAGS = devcfg_xml_tag_list)
   env.Replace(DEVCFG_XML_DICT = devcfg_xml_dict)
   #devcfg_used_tag_list = []
   #env.Replace(DEVCFG_ENV_USED_XML_TAGS = devcfg_used_tag_list)
   
   # env.Append(CPPDEFINES = [
      # "DAL_CONFIG_IMAGE_MODEM",
      # "DAL_CONFIG_TARGET_ID=${DAL_CONFIG_TARGET_ID}",
      # #"DEVCFG_XML_TAGS=DEVCFG_XML_TAGS",
      # #"DEVCFG_XML_DICT=DEVCFG_XML_DICT",
      # #"DEVCFG_ENV_USED_XML_TAGS=$DEVCFG_ENV_USED_XML_TAGS",
   # ])

def exists(env):
    return env.Detect('tbc_core_devcfg')
