import xml.dom.minidom
import unicodedata
import re

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
    def __init__(self, cfgxml):
        self.imageConfigXML = cfgxml
        self.parseImageCfgXML()
        
    def parseImageCfgXML(self):
        imageXML = self.imageConfigXML
        image_cfg_dom = xml.dom.minidom.parse(imageXML)
        
        #Get handle to the tags            
        socHwVersion                = image_cfg_dom.getElementsByTagName("soc_hw_version")
        tcsrSsocHwVersionAddr       = image_cfg_dom.getElementsByTagName("tcsr_soc_hw_version_addr")
        
        #SocHwVersion Dictionary
        assert (len(socHwVersion) > 0), "<soc_hw_version> tag missing in {} file".format(imageXML)
        socHwVersionNumber = re.sub('\s','',socHwVersion[0].firstChild.data.encode("ascii")).split(',')
        socHwVersionDict = {}
        for element in socHwVersionNumber :
            socHwVersionDict.setdefault(element.split(':')[0],[]).append(element.split(':')[1])         
        tcsrSsocHwVersionAddrVal = re.sub('\s','',tcsrSsocHwVersionAddr[0].firstChild.data.encode("ascii"))

        #Initialise Feature set
        #self.setFeature('DEVCFG_INFO_COLLECTOR',{})              
        self.DEVCFG_CURRENT_USED_XML_TAGS=[]
        #Extract and validate tag data             
        self.targetImg=getXmlElement(imageXML, "target_img")
        #Identify target image DevCfg build tag
        self.devcfgImgBuildTag= getXmlElement(imageXML, "devcfg_target_build_tag").upper()
        #DAL_CONFIG_TARGET_ID value
        self.dalConfigTargetId= getXmlElement(imageXML, "dal_config_target_id", default='0x${MSM_ID}')
        #Devcfg xml tag list (SOC tags)
        taglist= [getXmlElement(imageXML, "devcfg_xml_tag_single_configs", default=getXmlElement(imageXML, "devcfg_xml_tag_multiple_configs", default=[]))]
        self.devcfgXMLTagList= taglist
        #Devcfg xml tag list (PLATFORM tags)
        self.devcfgPlatformXMLTagList= getXmlElement(imageXML, "devcfg_platform_xml_tags", default=[])
        #OEM Parameters
        self.oemCfgBaseAddr= getXmlElement(imageXML, "oem_config_base_addr", default='')
        self.socHwVersion=socHwVersionDict
        self.tcsrSsocHwVersionAddr=tcsrSsocHwVersionAddrVal
        #tcsr Physical to Virtual mapping flag (1:1 or not)
        self.tcsrPhyToVirSame= getXmlElement(imageXML, "tcsr_paddr_to_vaddr_same")
        #MultiCfg Flag
        self.multiCfg= getXmlElement(imageXML, "multiCfg", default=True)
        #Memory OPtimisation Flag
        self.propsMemoryOptimise= getXmlElement(imageXML, "props_memory_optimisation", default='false')
        #Struct Size Flag
        self.structSize= getXmlElement(imageXML, "size_of_struct", default='false')
        #Hardware version specific props Flag
        self.hwVersionSpecificProps = getXmlElement(imageXML, "hw_version_specific_props", default='false')

    #this is pointless, remove asap
    def getFeature(self, feature):
        return getattr(self, feature)

def getXmlElement (xmlFile, tag, default=False):
    image_cfg_dom = xml.dom.minidom.parse(xmlFile)
    try:
        result = re.sub('\s','',image_cfg_dom.getElementsByTagName(tag)[0].firstChild.data.encode("ascii")).lower()
        if ',' in result:
            result = result.split(',')
    except:
        if default != False:
            result = default
        else:
            assert(0), "ERROR : <"+tag+"> tag missing in {}".format(xmlFile)
    return result
