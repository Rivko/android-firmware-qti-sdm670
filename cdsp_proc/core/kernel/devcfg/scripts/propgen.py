#[jvedadi]
#Originally, propgen only accept unsigned int for Driver IDs, but now Strings IDs are supported.
#All String IDs are isolated in a separate array (driver_list) which contains
#[String Name, Hash Value, Property Offset, Struct pointer, Size of Collisions, and Collisions' list.]
#If there were no String IDs, [driver_list] is assigned to NULL.
#Hash values are used to optimize the search process and avoid String- compare to find property Offset.
#If a collision accrues while computing Hash values, Collisions' list array maintains collisions indexes.
#Collision's list is used only if there is a collision and String compare is required in this case.
#Function hex_to_integer is removed, and built-in function  int  is used instead.
#offset fixed
#Check for Supported Char:  Supported_char = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%()!#$*+,-./:;=?@[]^_`{|}~' 
#Version:1.0
import xml.dom.minidom
import sys
import re
import os
from struct import *
from types import *
from optparse import OptionParser

global NameSection, StringSection, ByteSection,  SegmentOffset, UintSection, DevSection
global DeviceID
UintSection = []
ByteSection = []
StringSection =[]
NameSection = []
DevSection = []
DeviceID = []
SegmentOffset = []
#[vkaushik] change for struct props
StructPtrs = []
StructIncFiles = []
PropNames = []
StringDeviceIdx = 0
StringDeviceNames = {}
ConfigStructList = {}
HashCollisionOffset = {}
CollisionDictionary = {}
StructDef = {}
device_offset = 0

def processArgs ():
    parser = OptionParser("usage: %prog [options]")
    parser.add_option("-i", "--input", dest="inputs", action="append", help="XML Filename(s), INPUT, REQUIRED")
    parser.add_option("-u", "--StructHeader", dest="structHeader", action="store", type="string", help="struct header, OUTPUT, optional")
    parser.add_option("-t", "--ConfigType", dest="config_type", help="Config Type, INPUT")
    parser.add_option("-m", "--ModDirFile", dest="mod_dir_filename", help="DALModDir filename, OUTPUT")
    parser.add_option("-c", "--ConfigFile", dest="cfg_dir_filename", help="DALConfig filename, OUTPUT")
    parser.add_option("-e", "--DevcfgDataFile", dest="devcfgdata_filename", help="Devcfg Data filename, OUTPUT")
    # RemoveDups is set to True for DALModDir.c creation as this will be a superset of all the drivers present in the system
    parser.add_option("-r", "--RemoveDups", dest="remove_duplicate_drivers", action="store_true", help="Option to remove duplicate drivers, INPUT")
    parser.add_option("-p", "--MemoryOptimiseProps", dest="memory_optimised_properties", action="store_true", help="Storage space per property(2/3 words)., INPUT")
    parser.add_option("-s", "--structSize", dest="size_of_struct",  action="store_true", help="Return sizeof(struct) for STRUCT properties ?, INPUT")
    (options, args) = parser.parse_args()
    if options.inputs is None:
        parser.print_help()
        exit()
    return (options, args)

def genMasterXmlStr (xmlList):
    #xmlList.sort()
    xmlList = sorted(xmlList, key = lambda x: os.path.basename(x))
    xmlStr = ""
    xmlHeader = "<?xml version=\"1.0\"?>\n"
    dalAndModOpenTags = "<dal>\n<module name=\"devcfg\">\n"
    dalAndModCloseTags = "</module>\n</dal>\n"
    patternObj = re.compile('<\?xml.+\?>')
    for file in xmlList:
        try:
            xml.dom.minidom.parse(file)
        except Exception:
            raise ValueError("{} raised exception\n".format(file))
        FH = open(file, "r")
        newXML = FH.read()
        #check if this is the generated master xml file
        if len(xmlList) == 1 and dalAndModOpenTags in newXML:
            dalAndModOpenTags = dalAndModCloseTags = ""
        try:
            xml.dom.minidom.parseString(newXML)
        except Exception:
            raise ValueError("{} raised exception\n".format(file))
        if "#include" in newXML or "#define" in newXML or "#if" in newXML:
            raise ValueError("Error: Invalid XML found in - {}\n".format(file))
        xmlStr += (patternObj.sub("",newXML)+"\n")
        FH.close()
    xmlStr = xmlHeader + dalAndModOpenTags + xmlStr + dalAndModCloseTags
    return xmlStr


def genMasterXML (outputFileName, xmlList):
    try:
        outputFile = open(outputFileName, 'w')
    except IOError:
        raise ValueError("Could not create DevCfg Master XML file %s" % filepathname)
    masterXMLStr = genMasterXmlStr(xmlList)
    outputFile.write(masterXMLStr)
    outputFile.close()

# Aligning Section Adderss to 4 bytes
def AlignSectionAddr():
    NameSectionLen = len(NameSection)
    StringSectionLen = len(StringSection)
    ByteSectionLen = len(ByteSection)

    if NameSectionLen%4 != 0:
        for tmpBytes in range(0,4-(NameSectionLen%4)):
            NameSection.append(0)

    if StringSectionLen%4 != 0:
        for tmpBytes in range(0,4-(StringSectionLen%4)):
            StringSection.append(0)

    if ByteSectionLen%4 != 0:
        for tmpBytes in range(0,4-(ByteSectionLen%4)):
            ByteSection.append(0)
    return

    
    # This Function will check if the StringDevice had a Collision and if is it it point it to the right uint32 array (indexes)
def ComputeCollisionIndex(Myhash,device):
    if len(HashCollisionOffset[Myhash])==1:
        return "NULL"
    else:
        CollisionDictionary[device] = HashCollisionOffset[Myhash]
        return device
        #StringDeviceNames[node.getAttribute("id")]= [DriverName,device_offset, hash,StringDeviceIdx,DriverName]

def PrintStringDeviceNameStruct(putnull, ConfigType):
    #{/dev/buses/i2c1, 1234, DALI2C_DriverInfo,  {2,5,8 },
    for device in StringDeviceNames:
        # ignore null Driver names     
        if putnull ==0 and (StringDeviceNames[device][0]=="NULL" or StringDeviceNames[device][0]=="null"):
            ConfigStructList[StringDeviceNames[device][3]] = "{0,0,0,0,0,0}"
            continue                  
        ConfigStructList[StringDeviceNames[device][3]] = "{\""+device #String Name
        ConfigStructList[StringDeviceNames[device][3]] +="\","+str(StringDeviceNames[device][2])+"u"# hash     
        if (putnull==0):
            # The offset is not used in DALModDir.c. Hence, setting it to 0
            ConfigStructList[StringDeviceNames[device][3]] +=", 0"# offset
            ConfigStructList[StringDeviceNames[device][3]] +=", &DAL"+StringDeviceNames[device][0]+"_DriverInfo"#Driver
        else:
            ConfigStructList[StringDeviceNames[device][3]] +=", "+str(StringDeviceNames[device][1])# offset
            ConfigStructList[StringDeviceNames[device][3]] +=", NULL"#Struct Pointer
      # Getting Collision list 
        MyHash = StringDeviceNames[device][2]
        if ConfigType == '':
            collision_str_var = "Collision_"+str(StringDeviceNames[device][2])
        else:
            collision_str_var = "Collision_" + str(StringDeviceNames[device][2]) + "_" + ConfigType
        Collisions = ComputeCollisionIndex(MyHash, collision_str_var)
        if(len(HashCollisionOffset[MyHash])==1):
            ConfigStructList[StringDeviceNames[device][3]] +=", 0"
        else:
            ConfigStructList[StringDeviceNames[device][3]] +=", "+ str(len(HashCollisionOffset[MyHash]))
        ConfigStructList[StringDeviceNames[device][3]] +=", "+Collisions
        ConfigStructList[StringDeviceNames[device][3]] +=" "+"}"
      
def PrintConfig(ModName, CONFIG, devcfgdatafilename, ConfigType, structSize):
    #Header Size is 24 i.e. Total Len (4) + Name Section Len(4) + String Sec Len(4) + Byte Sec Len (4) + Uint Sec Len (4) + Num Devices (4)
    ConfigHeaderLen = 24 + 4*len(DeviceID)
    global UintSection, DevSection
    # UintSection = PackData(UintSection, 3)
    # Calculating the size of the propbin and create the header
    DevSection = PackData(DevSection, 3)
    SegmentOffset = [ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection) + len(DevSection),
        ConfigHeaderLen,
        ConfigHeaderLen + len(NameSection),
        ConfigHeaderLen + len(NameSection) + len(StringSection),
        ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection),
        len(DeviceID) / 2]
    for i in range(len(DeviceID)):
        if i%2 == 1:
            DeviceID[i] += SegmentOffset[4] + len(UintSection)
    SegmentOffset.extend(DeviceID)
    SegmentOffset = PackData(SegmentOffset, 3)
    PropBin = SegmentOffset + NameSection + StringSection + ByteSection + UintSection + DevSection

    #include struct files
    for inc_files in StructIncFiles:
        CONFIG.write ("#include \"%s\"\n" %inc_files)
   
    #struct ptrs
    CONFIG.write ('\n')
    for struct_ptrs in StructPtrs:
        local_struct =0
        for struct in StructDef:
            name = StructDef[struct][0]
            if name == struct_ptrs :
                local_struct =1
                break
        if local_struct == 1 :
            continue
        if(len(ConfigType) !=0):
            struct_qual_name = struct_ptrs+'_'+ConfigType
        else:
            struct_qual_name = struct_ptrs
        CONFIG.write ('extern void * %s;\n' %struct_qual_name)

    #struct
    CONFIG.write ("\n")
    for index in StructDef:
        name = StructDef[index][0]
        data_type = StructDef[index][1]
        value = StructDef[index][2]
        CONFIG.write ("static " +  data_type + "\t" + name + "=" + value + ";\n")
   
    if(len(ConfigType) !=0):
        dalprop_structptrs_str = "DALPROP_StructPtrs_" + ConfigType
    else :
        dalprop_structptrs_str = "DALPROP_StructPtrs"
   
    if structSize:
        CONFIG.write ("\nconst DALSYSPropStructTblType " + dalprop_structptrs_str + "[" + str(len(StructPtrs)+1) + "] =  {\n")
    else:
        CONFIG.write ("\nconst void * " + dalprop_structptrs_str + "[" + str(len(StructPtrs)+1) + "] =  {\n")
    for struct_ptrs in StructPtrs:
        local_struct =0
        for struct in StructDef:
            name = StructDef[struct][0]
            if name == struct_ptrs :
                local_struct =1
                break
        if(local_struct == 1) :
            array_symbol_index = struct_ptrs.find('[')
            if(array_symbol_index > 0):
                name_split = struct_ptrs.split('[')
                struct_ptrs = name_split[0]
            if structSize:
                CONFIG.write ('\t {sizeof(' + struct_ptrs + '), &'+ struct_ptrs +"},\n")
            else:
                CONFIG.write ('\t&'+struct_ptrs +",\n")
        else:
            if structSize:
                if(len(ConfigType) !=0):
                    CONFIG.write ('\t {sizeof(void *), &' + struct_ptrs +'_'+ConfigType+'},\n')
                else:
                    CONFIG.write ('\t {sizeof(void *), &' +struct_ptrs +'},\n')
            else:
                if(len(ConfigType) !=0):
                    CONFIG.write ('\t&'+struct_ptrs +'_'+ConfigType+',\n')
                else:
                    CONFIG.write ('\t&'+struct_ptrs +',\n')
    if structSize:
        CONFIG.write ("\t{0, 0 } \n };")
    else:
        CONFIG.write ("\t 0 };\n")

    #binary props
    if(len(ConfigType) !=0):
        dalprop_propbin_str = "DALPROP_PropBin_" + ConfigType
    else:
        dalprop_propbin_str = "DALPROP_PropBin"
      
    CONFIG.write ("\nconst uint32 " + dalprop_propbin_str + "[] = {\n")
    for i in range(0, len(PropBin), 4):
        if i%20 == 0:
            CONFIG.write ("\n\t\t\t")
        if type(PropBin[i]) is StringType or type(PropBin[i]) is UnicodeType :
            CONFIG.write ("%s" %PropBin[i])
        else:
            if i+3<len(PropBin):
                writeval = 0
                writestr = ""
                rawstr = "RAW: "
                for j in range(0,4):
                    try:
                        writeval += PropBin[i+j]<<(j*8)
                    except Exception, e:
                        writestr = " + ({}<<{})".format(PropBin[i+j],str(8*j))
                    try:
                        rawstr += "%.2x "%PropBin[i+j]
                    except:
                        rawstr += "{} ".format(PropBin[i+j])
                writestr = "{0:#010x}{1}".format(writeval,writestr)

                CONFIG.write (writestr)
            elif i+2<len(PropBin):
                CONFIG.write ("0x%.4x" %PropBin[i+2] + "%.2x" %PropBin[i+1] + "%.2x" %PropBin[i])
            elif i+1<len(PropBin):
                CONFIG.write ("0x%.6x" %PropBin[i+1] + "%.2x" %PropBin[i])
            else:
                CONFIG.write ("0x%.8x" %PropBin[i])
        if i != len(PropBin) - 4:
            CONFIG.write (", ")
        else:
            CONFIG.write (" };\n")

    #driver_entry driver_list[] = {{ /dev/buses/i2c1, 1234, {2,5,8 }, }
    PrintStringDeviceNameStruct(1, ConfigType)
    # Create the devcfg_data.c
    CONFIG_DATA = open(devcfgdatafilename, 'w')
    CONFIG_DATA.write ("#include \"devcfgTypes.h\" \n")
    if structSize:
        CONFIG_DATA.write ("\nextern const DALSYSPropStructTblType " + dalprop_structptrs_str + "[];\n")
    else:
        CONFIG_DATA.write ("\nextern const void * " + dalprop_structptrs_str + "[];\n")
    CONFIG_DATA.write ("\nextern const uint32 " + dalprop_propbin_str + "[];\n")
   
    if(len(ConfigType) !=0):
        dalprop_propsinfo_str = "DALPROP_PropsInfo_" + ConfigType
    else :
        dalprop_propsinfo_str = "DALPROP_PropsInfo"
      
    if len(ConfigStructList)!=0:
        if(len(ConfigType) !=0):
            driver_list_str = "driver_list_" + ConfigType
        else:
            driver_list_str = "driver_list"
        CONFIG_DATA.write ("\nextern const StringDevice " + driver_list_str +"[];\n")
        for Col_array in CollisionDictionary:
            CONFIG.write ("\nstatic uint32 " + Col_array + " [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
        CONFIG.write("\n\n\nconst StringDevice " + driver_list_str + "[] = ")
        CONFIG.write("{\n\t\t\t")
        for list in range(len(ConfigStructList)):
            CONFIG.write(ConfigStructList[list])
            if(list <len(ConfigStructList)-1):
                CONFIG.write (",\n\t\t\t")
        CONFIG.write ("\n};\n")
        CONFIG_DATA.write("\n\nconst DALProps " + dalprop_propsinfo_str + " = {(const byte*)" + dalprop_propbin_str +", " + dalprop_structptrs_str + ", " + str(len(ConfigStructList)) + ", " + driver_list_str + "};\n")
    else:
        CONFIG_DATA.write("\n\nconst DALProps " + dalprop_propsinfo_str + " = {(const byte*)" + dalprop_propbin_str + ", " + dalprop_structptrs_str + " , 0, NULL};\n")
   
    CONFIG_DATA.close()

# Packing the data and get ready to print 
def PackData(value, type1):
   if type1 == 1:    #String
      value = list(value)
      for index in range (len(value)):
         value[index] = ord(value[index])
      value.append(0)
   elif type1 == 3:     #UINT32
      value_temp = []
      value=list(value)
      for index in range (len(value)):
         if ((type(value[index]) is StringType) or (type(value[index]) is UnicodeType)):
            value_temp.append(value[index])
            # Since this is a enum which is 4-bytes long and the PropBin list
            # is handled as 4-bytes at a time, appending 3 0's at the end of
            # the string.
            for j in range(3):
                value_temp.append(0)
         elif type(value[index]) is ListType:
            for idx in range(len(value[index])):
               value_temp.append(value[index][idx])
            if len(value[index]) % 4 != 0:
               for idx2 in range(4 - (len(value[index]) % 4)):
                  value_temp.append(0)
         else:
            try:
                l=unpack('BBBB',pack('I', value[index]))
            except:
                print "bad data found, val={}, type={}".format(value,type1)
                raise ValueError
            for j in l:
               value_temp.append(j)
      value = value_temp
   return value

# Read Global section of the XML file and create a dictionary called GlobalDef  
def GenerateGlobalDef(pDriver):
   GlobalDef = {}
   for global_defs in pDriver.getElementsByTagName("global_def"):
      global_defs.normalize()
      for node in global_defs.childNodes:
         if node.nodeType == node.TEXT_NODE:
            continue
         name = node.getAttribute("name")
         type = node.getAttribute("type")
         value = node.firstChild.data
         
         #DALPROP_DATA_TYPE_STRING
         if type == "DALPROP_DATA_TYPE_STRING" or type == 1:
            type = 1 
            value = value.strip()
            
        #DALPROP_DATA_TYPE_BYTE_SEQ or DALPROP_DATA_TYPE_UINT32_SEQ
         elif type == "DALPROP_DATA_TYPE_BYTE_SEQ" or type==2:
            type = 2
            value = handleByteSeq (value)
         elif type == "DALPROP_DATA_TYPE_UINT32_SEQ" or type==3:
            type = 3
            value = handleByteSeq (value)
                    
         #DALPROP_DATA_TYPE_STRUCT_INC_FILE
         elif type == "DALPROP_DATA_TYPE_STRUCT_INC_FILE" or type == 4:
            type=4
            #insert the include file if not already in our list
            value = value.strip()
            try:
               StructIncFiles.index(value)
            except ValueError:
               StructIncFiles.append(value)
               
         value = PackData(value, type)
         GlobalDef[name] = value
   return GlobalDef

def handleByteSeq (value):
    value = re.sub("\s*", '', value)
    value = value.split(',')
    for index in range (len(value)-1):
        try:
            value[index] = eval(value[index].strip())
        except NameError:
            value[index] = str(value[index].strip())
    del value[-1]                       #removing the 'end' marker
    value.insert(0, len(value)-1)       #storing length-1 as the first value of the sequence
    return value

#Implement hash for String Devices 
def HashDJB2(key):
   myhash = 5381;
   for c in key:
      myhash = ((myhash << 5) + myhash) + ord(c);
   import ctypes;
   return myhash & 0xFFFFFFFF
   
# Generate the array DevSection based on properties . The last element of the array returned has the len of the array  
def GenerateArray(DriverName, node, GlobalDef, GlobalOffset, remove_dup_names, memory_optimised_props):
   Supported_char = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%()!#$*+,-./:;=?@[]^_`{|}~'  
   global StringDeviceIdx
   global device_offset
   bytes_per_property = 0
   
   #Set storage required per property based on
   #memory optimisation flag being enabled/diabled
   if memory_optimised_props: 
       bytes_per_property = 8
   else:
       bytes_per_property = 12
       
   DeviceVersionDict = {}
   
   if len(DevSection):
      # poping the last element 
      device_offset += DevSection.pop()
      
   nodeid = node.getAttribute("id")
   if nodeid:
       if nodeid[:2] =="0x":
          #It is a UNINT32 based Device ID
          try :
             nod_id = int(nodeid,16)
             if nod_id > 0xFFFFFFFF:
                raise ValueError
             #if nod_id==0:
             #   raise ValueError
          except:
             raise ValueError("{} is invalid uint32 Device ID. Remove \"0x\" if it is a String ID".format(node.getAttribute("id")))
          DeviceID.append(nod_id)          
       else:
          DeviceID.append(nodeid)
       DeviceID.append(device_offset)
   elif node.getAttribute("name"):
      #It is a String based Device 
      #Is it a duplicate string Device ?
      if StringDeviceNames.has_key(node.getAttribute("name")):
         if remove_dup_names is False:
            #Raise error if duplicate device names
            print "ERROR *** Duplicate Device Name : {}".format(node.getAttribute("name"))
            raise ValueError("Duplicate Device Name")
         else:
            DevSection.append(0)
            return
            
      #Does the String device name have any unsupported characters ?
      for L in node.getAttribute("name"):
          if L not in Supported_char:
              print "ERROR *** Device Name {} has NON-Supported Character : {}".format(node.getAttribute("name"),L)
              print "Supported Charactors are : "
              print str(list(Supported_char))[1:-1]
              raise ValueError("Device Name has NOT-Supported Characters")
      
     #Check for Hash Collisions    
      if (not remove_dup_names) or (remove_dup_names and (DriverName.lower() not in "NULL".lower())):
          hash = HashDJB2(node.getAttribute("name"))
          StringDeviceNames[node.getAttribute("name")]= [DriverName,device_offset, hash,StringDeviceIdx]
          if HashCollisionOffset.has_key(hash):
              HashCollisionOffset[hash].append(StringDeviceIdx)
          else:
              HashCollisionOffset[hash] = [StringDeviceIdx]
          StringDeviceIdx +=1
   
   valLen = bytes_per_property * len(node.getElementsByTagName("props"))

   # Loop through all properties and push them in to the array
   for Props in node.getElementsByTagName("props"):
      name = Props.getAttribute("name")
      if(len(name) != 0):
      #Its a string based prop name
         name = name.encode('ascii', 'ignore')
         # make sure the prop name is unique in the NameSection
         if name not in PropNames:
            PropNames.append(name)
            nameoffset = len(NameSection)
            charname = PackData(name, 1)
            NameSection.extend(charname)
         else:
            #calculate the nameoffset of this existing prop
            nameoffset = 0
            loc = PropNames.index(name)
            for idx in range(loc):
               nameoffset += len(PropNames[idx]) + 1
         if memory_optimised_props:
            nameoffset = nameoffset | 0x800000; # turn on 23rd bit to signify its name offset
      else:
      #Its a unint32 based prop ID
      #In this case, the Property ID is stored in the "nameoffset" field
         nameoffset = Props.getAttribute("id")
         if(nameoffset.isdigit() is True):
            nameoffset = eval(nameoffset) 
         elif not (memory_optimised_props):
            print "ERROR: Enum Prop id found : {0}".format(str(nameoffset))
            raise ValueError("Enum Prop id's are supported only in Memory Optimised mode. Turn on Memory Optimisation through Image_cfg.xml")
      type1 = Props.getAttribute("type").strip()
      #if the prop is primitive type
      if type1[:2] =="0x":
         type1 = int(type1, 16)
      
      # if the prop is array
      instance_is_array = 0
      array = Props.getAttribute("array")
      if(len(array) and "True".lower() in array.lower() ):
         instance_is_array =1
         #remove all '[' or ']' characters from the array type name
         re.sub("(\[\.*\])", '', type1)
              
      # version can be a unique id or a list or none for default case
      version_data = Props.getAttribute("version")
      if(len(version_data) == 0):
         version = ['common']
      else :
         version = []
         for version_id in version_data.split(',') :
            version.append(version_id.strip())

      #Extract data for this prop
      char_data = ""
      for child in Props.childNodes:
         if child.nodeType == child.COMMENT_NODE:
            continue
         char_data = char_data + re.sub("(\s*|\n)", '', child.data)
         
         
      #DALPROP_ATTR_TYPE_UINT32
      if type1 == 'DALPROP_ATTR_TYPE_UINT32' or type1 == 0x02:
         type1=0x02
         try:
            value = eval(char_data)
         except NameError:
            value= str(char_data)
            
            
      # DALPROP_ATTR_TYPE_BYTE_SEQ
      elif type1=='DALPROP_ATTR_TYPE_BYTE_SEQ' or type1 == 0x08: 
         type1 = 0x08
         value = char_data.split(',')
         for index in range(len(value) - 1):
            value[index] = value[index].strip()
            value[index] = eval(value[index])
         #Insert length of the BYTE-SEQ as the first element.
         #Since we allocate a byte for length, max length of BYTE-SEQ
         #can be no more than 256 bytes
         value.insert(0, len(value) - 1)
         # fix the length (store as an offset) and pop the 'end'
         value[0] -= 1
         #Is it an empty BYTE-SEQ ?
         if value[0] < 0:
            value[0] = 0
            print "WARNING : Remove empty BYTE-SEQ for Prop name/id : {0} \
            for Device : {1} in Driver : {2}\n".format(name,node.getAttribute("id"),DriverName)

         if "end" == value[len(value) - 1]:
            value.pop()
         while len(value)%4 != 0:
            value.append(0)
         valLen +=len(value)-4
      
      
      #DALPROP_ATTR_TYPE_STRUCT_PTR
      #(inline struct config props in XML)
      elif type1 == "DALPROP_ATTR_TYPE_STRUCT_PTR" or type1 == 0x12:
         type1 = 0x12
         try:
            StructPtrs.index(char_data)
         except ValueError:
            StructPtrs.append(char_data)
         value = StructPtrs.index(char_data)
      
      # Its a global property
      elif type1 in ["DALPROP_ATTR_TYPE_BYTE_SEQ_PTR", "DALPROP_ATTR_TYPE_STRING_PTR", "DALPROP_ATTR_TYPE_UINT32_SEQ_PTR", 0x18, 0x11, 0x14]:
         if type1 == "DALPROP_ATTR_TYPE_BYTE_SEQ_PTR":
            type1 = 0x18
         elif type1 == "DALPROP_ATTR_TYPE_STRING_PTR":
            type1 = 0x11
         elif type1 == "DALPROP_ATTR_TYPE_UINT32_SEQ_PTR":
            type1 = 0x14
         #Has this global prop already been referenced before ?
         #If yes, just get the offset since the data has already been stored in the SBU
         if char_data in GlobalOffset:
            value = GlobalOffset[char_data]
         else:
            #DALPROP_ATTR_TYPE_BYTE_SEQ_PTR
            if type1 == 0x18:
               value = len(ByteSection)
               ByteSection.extend(GlobalDef[char_data])
            #DALPROP_ATTR_TYPE_STRING_PTR
            elif type1 == 0x11:
               value = len(StringSection)
               StringSection.extend(GlobalDef[char_data])
            #DALPROP_ATTR_TYPE_UINT32_SEQ_PTR
            elif type1 == 0x14:
               value = len(UintSection)
               UintSection.extend(GlobalDef[char_data])
         GlobalOffset[char_data] = value
      # if its a user defined type, doesnot have pre-defined hex type
      elif (re.match( r'\D', str(type1))):           
         data_type = str(type1) 
         type1 = 0x12          # DALPROP_ATTR_TYPE_STRUCT_PTR
         struct_index = len(StructPtrs)
         if(instance_is_array > 0):
            struct_name = "devcfg_" + str(struct_index) + "[]"
         else:
            struct_name = "devcfg_" + str(struct_index)           
         StructPtrs.append(struct_name)
         value = StructPtrs.index(struct_name)
         StructDef.setdefault(value, [struct_name, data_type, char_data])
      else:
         print "Data type unknown"

      
      #logic to store type and nameoffset in 32 bit, higher 8 bits for type
      if memory_optimised_props:
          str_nameoffset = str(nameoffset)
          #When prop id's ar enumeric values
          if(str_nameoffset.isdigit() is True) :
             type1 = type1 & 0x000000FF                 #discard all higher 28 bits
             nameoffset = nameoffset & 0x00FFFFFF       #discard all higher 8 bits
             type_nameoffset = (type1<<24) | nameoffset #123456780000...| 00000000xxxxx.....= 12345678xxxxxxx
          else:
             #When prop ids are enums
             type1 = (type1 & 0x000000FF) << 24                     #discard all higher 28 bits and store on higher 8 bit in word
             str_nameoffset = str(nameoffset) +  " & 0x00FFFFFF"    #discard all higher 8 bits
             #type1 = type1 & 0x000000FF                            #discard all higher 28 bits
             type_nameoffset = "(" + str(type1) + " | (" + str_nameoffset + "))"
      
      
      for version_id in version :
         if memory_optimised_props:
            DeviceVersionDict.setdefault(version_id, [  ]).append([type_nameoffset, value])
         else:
            DeviceVersionDict.setdefault(version_id, [  ]).append([type1, nameoffset, value])
      #since we are duplicating a property for versions, valLen need to be updated.
      valLen+= bytes_per_property *(len(version)-1)
      
   DeviceVersionList = []
   VersionOffset = {}
   VersionOffsetList = []
    
   # Add the common property first   
   if(DeviceVersionDict.has_key('common')):
      for prop in DeviceVersionDict['common'] :
        DeviceVersionList.extend(prop)
      del DeviceVersionDict['common']
      DeviceVersionList.append(0xFF00FF00) # 0xFF00FF00 is the end marker for common properties 
      valLen+=4
     
   # Compute the offset of all version specific data and store them in list.
   # the control will get inside the loop only if the device has version related properties.
   for version_rev in DeviceVersionDict.keys() :
      offset_adjustment = 0
      for element in DeviceVersionList :
        #incase if the element is a byte sequence
        if type(element) is list :
          offset_adjustment +=len(element)-4 
      version_offset = 4 * len(DeviceVersionList) + offset_adjustment
      # offset is added by 3[type, name, value] to account for versioninfo prop prepended at the start of device
      VersionOffset.update({version_rev : version_offset + 12}) 
      for prop in DeviceVersionDict[version_rev] :
         DeviceVersionList.extend(prop)
      DeviceVersionList.append(0xFF00FF00) # 0xFF00FF00 is the end marker for version specific properties 
      valLen+=4
   del DeviceVersionDict
        
   # Process the version offset
   num_of_rev = len(VersionOffset)
   if(num_of_rev) :
      VersionOffsetList.append(num_of_rev)
      for version_rev in VersionOffset.keys() :
         version_id = eval(version_rev) & 0x0000FFFF #discard all higher 16 bits
         version_offset = VersionOffset[version_rev] & 0x0000FFFF #discard all higher 16 bits
         mashed_offset = (version_id<<16) | version_offset 
         VersionOffsetList.append(mashed_offset)
      version_binary_stream = PackData(VersionOffsetList, 0x00000003)
      value = len(UintSection)
      UintSection.extend(version_binary_stream)
      # make sure the prop name is unique in the NameSection
      if "DevCfgVersionInfo" not in PropNames:
         PropNames.append("DevCfgVersionInfo")
         nameoffset = len(NameSection)
         name = PackData("DevCfgVersionInfo", 1)
         NameSection.extend(name)
      else:
         nameoffset = 0;
         loc = PropNames.index("DevCfgVersionInfo")
         for idx in range(loc):
            nameoffset += len(PropNames[idx]) + 1
      
      if memory_optimised_props:
          DevSection.extend([0x00000014, nameoffset, value])  
      else:
          DevSection.extend([((0x00000014 << 24) | (nameoffset & 0x00FFFFFF)), value])  
      valLen = valLen + bytes_per_property # add the length of version specific header
   
   DevSection.extend(DeviceVersionList)
   # The length of the End marker
   DevSection.append(valLen)

def AdjustStringDeviceOffsets():
   # Adjust the String device offsets
   ConfigHeaderLen = 24 + 4*len(DeviceID)
   global UintSection, DevSection
   # Looping through all String devices
   for i in StringDeviceNames:
      StringDeviceNames[i][1] +=ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection)

# This function will print DALMod file
def PrintModDir(ModDirFileName, DriverList):
   MODDIR = open(ModDirFileName, 'w')
   MODDIR.write ("#ifndef DEVCFG_STANDALONE \n")
   MODDIR.write ("#include \"DALStdDef.h\" \n")
   MODDIR.write ("#include \"DALReg.h\" \n\n")
   MODDIR.write ("#include \"DALSysTypes.h\" \n\n")
   MODDIR.write ("#else \n")
   MODDIR.write ("#include \"stubs.h\" \n\n")
   MODDIR.write ("#include \"devcfgTypes.h\" \n\n")
   MODDIR.write ("#endif \n")
   for drivers in DriverList:
      if drivers == "NULL" or drivers == "null":
         continue 
      MODDIR.write ("extern DALREG_DriverInfo DAL%s_DriverInfo;\n" %drivers)
   MODDIR.write ("\nstatic DALREG_DriverInfo * DALDriverInfoArr[] = {\n")
   driver_device_id_len =0
   for drivers in DriverList:
      if drivers == "NULL" or drivers == "null":
         continue 
      driver_device_id_len += 1   
      MODDIR.write ("\t& DAL%s_DriverInfo" %drivers)
      MODDIR.write (",\n")
   MODDIR.write ("};\n\n")
   MODDIR.write ("DALREG_DriverInfoList gDALModDriverInfoList = {"+str(driver_device_id_len) + ", DALDriverInfoArr}; \n") 
   PrintStringDeviceNameStruct(0, '')
   if(len(ConfigStructList)!=0):
      for Col_array in CollisionDictionary:
         MODDIR.write ("\nstatic uint32 "+Col_array+" [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
      MODDIR.write("\n\n\nStringDevice DAL_Mod_driver_list[] = ")
      MODDIR.write("{\n\t\t\t") 
      driver_string_device_len = 0;
      for list in range(len(ConfigStructList)):
         #if list in ConfigStructList.keys():
         if(ConfigStructList[list] == "{0,0,0,0,0,0}"): 
            continue
         driver_string_device_len+=1   
         MODDIR.write(ConfigStructList[list])
         if(list <len(ConfigStructList)-1):
            MODDIR.write (",\n\t\t\t")
      if driver_string_device_len==0:
         MODDIR.write ("{NULL}\n")
      MODDIR.write ("\n};\n")
      MODDIR.write ("\nDALProps DAL_Mod_Info = {NULL, 0 ,"+str(driver_string_device_len)+", DAL_Mod_driver_list};\n")
   else:
      MODDIR.write ("\nDALProps DAL_Mod_Info = {NULL, 0 ,0 , NULL};\n")
      
      
def CheckDupDriverID(ModName, DriverList, remove_dup_dr_names):
   # check if 'driver' is duplicated in the DriverList
   idx = 1
   DriverList_mod = []
   for driver in DriverList:
      if remove_dup_dr_names is False:
         if driver in DriverList[idx:] and driver!= "NULL" and driver!= "null":
            # found a duplicate driver and no need to proceed any further
            print "ERROR *** DAL Driver '%s'" % driver, "has been included more than once for SW image '%s'" % ModName
            print "ERROR *** Please check XML files for '%s'" % ModName
            raise ValueError(driver)
      else:
         if driver not in DriverList[idx:] and driver!= "NULL" and driver!= "null":
            DriverList_mod.append(driver)
         
      idx += 1
   return DriverList_mod


def CheckDupDeviceID(ModName, remove_dup_names):
   # build the device ID list
   devIDList = []
   for i in range(0, len(DeviceID), 2):
      devIDList.append(DeviceID[i])
   # check if there are any duplicated device IDs
   idx = 1
   if remove_dup_names is False:
      for dev in devIDList:
         if dev in devIDList[idx:]:
            # find a duplicate device ID and no need to proceed any further
            print "ERROR *** DAL Device ID 0x%.8x" % dev, "has been included more than once for SW image '%s'" % ModName
            print "ERROR *** Please check XML files for '%s'" % ModName
            raise ValueError(hex(dev))
         idx += 1

def writeStructHeader (filename, ConfigType):
    structH = open(filename, 'a')
    structH.write("/*Struct Pointers*/\n")
    for structPtr in StructPtrs:
        local_struct =0
        for struct in StructDef:
            name = StructDef[struct][0]
            if name == structPtr :
                local_struct =1
                break
        if local_struct == 1 :
            continue
        if(len(ConfigType) !=0):
            structH.write ("#define "+structPtr+" "+structPtr+'_'+ConfigType+"\n")
    structH.write("#define DEVCFG_PROPS_INFO DALPROP_PropsInfo_{}".format(ConfigType))
    structH.close()


def Print_Debug ():
   print "\nSegmentOffset: ", SegmentOffset, "Len: ", len(SegmentOffset)
   print "\nName Section:", NameSection, "Len: ",len(NameSection)
   print "\nByte Section: ", ByteSection, "Len: ",len(ByteSection)
   print "\nString Section:" , StringSection, "Len: ",len(StringSection)
   print "\nUINT Section:", UintSection, "Len: ",len(UintSection)
   print "\nDevice Section: ", DevSection

def main():
   (options, args) = processArgs()
   masterXMLStr = genMasterXmlStr(options.inputs)

   ModList = xml.dom.minidom.parseString(masterXMLStr)

   
   # Read the XML file and generate the array
   for Mod in ModList.getElementsByTagName("module"):
      DriverList = []
      ModName = Mod.getAttribute("name")
      
      #For every Driver, process Global config data and for its devices
      for Driver in Mod.getElementsByTagName("driver"):
         GlobalDef = {}
         DriverList.append(Driver.getAttribute("name"))
         GlobalDef = GenerateGlobalDef(Driver)
         GlobalOffset = {}
         
         #For every device under this Driver, generate the config array
         for Device in Driver.getElementsByTagName("device"):
            GenerateArray(Driver.getAttribute("name"),Device, GlobalDef, GlobalOffset, options.remove_duplicate_drivers, options.memory_optimised_properties)
      # Pop the last element of the array, since it is nothing but the len
      if len(DevSection):
         DevSection.pop()
      AlignSectionAddr()
      # check for duplicated driver IDs and device IDs
      modDriverList = CheckDupDriverID(ModName, DriverList, options.remove_duplicate_drivers)
      CheckDupDeviceID(ModName, options.remove_duplicate_drivers)
      AdjustStringDeviceOffsets()
      if options.mod_dir_filename != None:
         PrintModDir(options.mod_dir_filename, modDriverList)

   if options.cfg_dir_filename != None:
      CONFIG = open(options.cfg_dir_filename, 'w')
      CONFIG.write ("#ifndef DEVCFG_STANDALONE \n")
      CONFIG.write ("#include \"DALStdDef.h\" \n")
      CONFIG.write ("#include \"DALSysTypes.h\" \n")
      CONFIG.write ("#include \"DALDeviceId.h\" \n")
      CONFIG.write ("#else \n")
      CONFIG.write ("#include \"stubs.h\" \n")
      CONFIG.write ("#include \"devcfgTypes.h\" \n")
      CONFIG.write ("#endif \n")
      for Mod in ModList.getElementsByTagName("include"):
         CONFIG.write ("#include \""+Mod.getAttribute("file")+"\" \n")
      PrintConfig(ModName, CONFIG, options.devcfgdata_filename, options.config_type, options.size_of_struct)
      CONFIG.close()

   if options.structHeader:
      writeStructHeader(options.structHeader, options.config_type)
      return 0
      
if __name__ == "__main__":
   main()
    
