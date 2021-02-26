#==============================================================================
#  Copyright (c) 2010-2015 Qualcomm Technologies, Inc . 
#  All Rights Reserved.
#  Qualcomm Confidential and Proprietary

#================================================================================
#==============================================================================
#                              EDIT HISTORY
#
#
# when       who     what, where, why
# --------   ---     ----------------------------------------------------------
# 12/15/15   bh     Merge changes from ap for settings relocation
# 07/13/15   sg     Property type have MSB turned on for visibility attr
# 03/31/15	 ap		Added support for DALConfigToolsLib
# 01/13/15   aa     Added support for storing indicator for prop id/name
# 10/27/14   aa     DALModDir_loader.c is now #included in DALConfig_loader.c
# 08/27/14   aa     Fix warning
# 07/29/14   aa     Added support for string based devices
# 07/29/14   aa     Updated Indentations.
# 07/29/14   aa     Add history
#
#==============================================================================
import xml.dom.minidom
import sys
import re
import os
import stat
from struct import *
from types import *
from datetime import date

# global data structures
global NameSection, StringSection, ByteSection,  SegmentOffset, UintSection, DevSection
global DeviceID
UintSection = []
ByteSection = []
StringSection =[]
NameSection = []
DevSection = []
DeviceID = []
SegmentOffset = []
StructPtrs = []
StructIncFiles = []
PropNames = []
StringDeviceIdx = 0
StringDeviceNames = {}
ConfigStructList = {}
HashCollisionOffset = {}
CollisionDictionary = {}
device_offset = 0

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

def ComputeCollisionIndex(Myhash,device):
   if len(HashCollisionOffset[Myhash])==1:
      return "NULL"
   else:
      CollisionDictionary[device] = HashCollisionOffset[Myhash]
      return device
      #StringDeviceNames[node.getAttribute("id")]= [DriverName,device_offset, hash,StringDeviceIdx,DriverName]

def PrintStringDeviceNameStruct(putnull):
   #{/dev/buses/i2c1, 1234, DALI2C_DriverInfo,  {2,5,8 },
   for device in StringDeviceNames:
                     # ignore null Driver names     
      if putnull ==0 and (StringDeviceNames[device][0]=="NULL" or StringDeviceNames[device][0]=="null"):         
         ConfigStructList[StringDeviceNames[device][3]] = "{0,0,0,0,0,0}"
         continue                  
      ConfigStructList[StringDeviceNames[device][3]] = "{\""+device #String Name
      ConfigStructList[StringDeviceNames[device][3]] +="\","+str(StringDeviceNames[device][2])+"u"# hash
      ConfigStructList[StringDeviceNames[device][3]] +=", "+str(StringDeviceNames[device][1])# offset
      if (putnull==0):
         ConfigStructList[StringDeviceNames[device][3]] +=", &DAL"+StringDeviceNames[device][0]+"_DriverInfo"#Driver
      else:
         ConfigStructList[StringDeviceNames[device][3]] +=", NULL"#Struct Pointer
      # Getting Collision list 
      MyHash = StringDeviceNames[device][2]
      Collisions = ComputeCollisionIndex(MyHash,"Collision_"+str(StringDeviceNames[device][2]))
      if(len(HashCollisionOffset[MyHash])==1):
         ConfigStructList[StringDeviceNames[device][3]] +=", 0"
      else:
         ConfigStructList[StringDeviceNames[device][3]] +=", "+ str(len(HashCollisionOffset[MyHash]))
      ConfigStructList[StringDeviceNames[device][3]] +=", "+Collisions
      ConfigStructList[StringDeviceNames[device][3]] +=" "+"}"
      
def PrintConfig(ModName):
   #Header Size is 24 i.e. Total Len (4) + Name Section Len(4) + String Sec Len(4) + Byte Sec Len (4) + Uint Sec Len (4) + Num Devices (4)
   ConfigHeaderLen = 24 + 4*len(DeviceID)
   global UintSection, DevSection

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
   CONFIG = open(DirName + "/" + sys.argv[3], 'a')
   CONFIG.write ("#include \"DALStdDef.h\" \n")
   CONFIG.write ("#include \"DALSysTypes.h\" \n")
   CONFIG.write ("#include \"dalconfig.h\" \n")
   
   if(ModName == "loader"):
      CONFIG.write ("#include \"DALModDir_loader.c\" \n\n")
   elif(ModName == "tools"):
      CONFIG.write ("#include \"DALModDir_tools.c\" \n\n")
   else:
      CONFIG.write ("\n")
   
   #define image name
   if ModName == "modem":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_MODEM \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_MODEM \n")
      CONFIG.write("#endif \n")
   elif ModName == "dsp":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_DSP \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_DSP \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot_wm6":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT_WM_6 \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT_WM_6 \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot_wm7":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT_WM_7 \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT_WM_7 \n")
      CONFIG.write("#endif \n")
   elif ModName == "tz":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_TZ \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_TZ \n")
      CONFIG.write("#endif \n")
   elif ModName == "wcn":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_WCN \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_WCN \n")
      CONFIG.write("#endif \n")
   elif ModName == "sps":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_SPS \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_SPS \n")
      CONFIG.write("#endif \n")
   elif ModName == "rpm":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_RPM \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_RPM \n")
      CONFIG.write("#endif \n")
   elif ModName == "uefi":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_UEFI \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_UEFI \n")
      CONFIG.write("#endif \n")
   elif ModName == "tools":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_TOOLS \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_TOOLS \n")
      CONFIG.write("#endif \n")
   else:
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_APPS \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_APPS \n")
      CONFIG.write("#endif \n")

   #include structure files
   for inc_files in StructIncFiles:
      CONFIG.write ("#include \"%s\"\n" %inc_files)
   #structure ptrs
   # for struct_ptrs in StructPtrs:
      # CONFIG.write ("extern void * %s;\n" %struct_ptrs)

   CONFIG.write ("\nconst void * DALPROP_StructPtrs["+str(len(StructPtrs)+1)+"] =  {\n")
   for struct_ptrs in StructPtrs:
      CONFIG.write ("\t&"+struct_ptrs +",\n")
   CONFIG.write ("\t NULL }; \n")
   #binary props
   CONFIG.write ("\nconst uint32 DALPROP_PropBin[] = {\n")
   for i in range(0, len(PropBin), 4):
      if i%20 == 0:
         CONFIG.write ("\n\t\t\t")
      if type(PropBin[i]) is StringType:
         CONFIG.write ("%s" %PropBin[i])
      else:
         if i+3<len(PropBin):
            CONFIG.write ("0x%.2x" %PropBin[i+3] + "%.2x" %PropBin[i+2] + "%.2x" %PropBin[i+1] + "%.2x" %PropBin[i])
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
   PrintStringDeviceNameStruct(1)
   if len(ConfigStructList)!=0:
      for Col_array in CollisionDictionary:
         CONFIG.write ("\nuint32 "+Col_array+" [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
      CONFIG.write("\n\n\n const StringDevice driver_list[] = ")
      CONFIG.write("{\n\t\t\t")
      for list in range(len(ConfigStructList)):
         CONFIG.write(ConfigStructList[list])
         if(list <len(ConfigStructList)-1):
            CONFIG.write (",\n\t\t\t")
      CONFIG.write ("\n};\n")
      CONFIG.write("\n\n const DALProps DALPROP_PropsInfo = {(const byte*)DALPROP_PropBin, DALPROP_StructPtrs , " +str(len(ConfigStructList))+", driver_list};\n")
   else:
      CONFIG.write("\n\n const DALProps DALPROP_PropsInfo = {(const byte*)DALPROP_PropBin, DALPROP_StructPtrs , 0, NULL};\n")

# Packing the data and get ready to print 
def PackData(value, type1):
   if type1 == 1:       #String
      value = list(value)
      for index in range (len(value)):
         value[index] = ord(value[index])
      value.append(0)
   elif type1 == 3:     #UINT32
      value_temp = []
      value=list(value)
      for index in range (len(value)):
         if type(value[index]) is StringType:
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
            for j in (unpack('BBBB',pack('I', value[index]))):
               value_temp.append(j)
      value = value_temp
      
   return value

def GenerateGlobalDef(pDriver):
   GlobalDef = {}
   for global_defs in pDriver.getElementsByTagName("global_def"):
      global_defs.normalize()
      for node in global_defs.childNodes:
         if node.nodeType == node.TEXT_NODE:
            continue
         name = node.getAttribute("name")
         type = node.getAttribute("type")
         type = int(type,0)
         value = node.firstChild.data
         if type == 1: #DALPROP_DATA_TYPE_STRING
            value = value.strip()
         elif (type == 2 or type ==3):
            re.sub("\s*", '', value)
            value = value.split(',')
            for index in range (len(value)-1):
               #removing white-space as this cause eval to fail
               value[index] = value[index].lstrip()
               value[index] = eval(value[index])
               #value[index] = int(value[index], 16)
            del value[-1]
            value.insert(0, len(value)-1)
         elif (type == 4):
            #insert the include file if not already in our list
            value = value.strip()
            try:
               StructIncFiles.index(value)
            except ValueError:
               StructIncFiles.append(value)
         value = PackData(value, type)
         GlobalDef[name] = value
   return GlobalDef

#Implement DJB2 hash 
def HashDJB2(key):
   myhash = 5381;
   for c in key:
      myhash = ((myhash << 5) + myhash) + ord(c);
   import ctypes;
   return myhash & 0xFFFFFFFF

def GenerateArray(DriverName, node, GlobalDef, GlobalOffset):
   Supported_char = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%()!#$*+,-./:;=?@[]^_`{|}~'  
   global StringDeviceIdx
   global device_offset
   if len(DevSection):
      device_offset += DevSection.pop()
   
   if node.getAttribute("id")[:2] =="0x":
      if DriverName == "NULL" or DriverName == "null":
         raise ValueError("You may not have a NULL Driver and uint32 Device ID")
      try :
         nod_id = int(node.getAttribute("id"),16)
         if nod_id > 0xFFFFFFFF:
            raise ValueError
      except:
         raise ValueError(node.getAttribute("id")+" is invalid uint32 Device Name. Remove \"0x\" if it is a String ID")
      DeviceID.append(nod_id)
      DeviceID.append(device_offset)
   else:
      if len(node.getAttribute("id"))==0:
         print "ERROR *** Empty Device Name"
         raise ValueError("Empty Device Name")
      if StringDeviceNames.has_key(node.getAttribute("id")):
         #Raise error if duplicate device names
         print "ERROR *** Duplicate Device Name '%s'" %node.getAttribute("id")
         raise ValueError("Duplicate Device Name")
      if not all(L in list(Supported_char)  for L in node.getAttribute("id")):
         print "ERROR *** Device Name '%s' has NOT-Supported Characters" %node.getAttribute("id")
         print "Supported Charactors are :"
         print str(list(Supported_char))[1:-1]
         raise ValueError("NOT-Supported Characters Device Name")
      else:
         #device name is string and not uint32 number
         hash = HashDJB2(node.getAttribute("id"))
         StringDeviceNames[node.getAttribute("id")]= [DriverName,device_offset, hash,StringDeviceIdx]
         if HashCollisionOffset.has_key(hash):
            HashCollisionOffset[hash].append(StringDeviceIdx)
         else:
            HashCollisionOffset[hash] = [StringDeviceIdx]
         StringDeviceIdx +=1
   
   valLen = 12* len(node.getElementsByTagName("props"))
   for Props in node.getElementsByTagName("props"):
      name = Props.getAttribute("name")
      if(len(name) != 0):
         name = name.encode('ascii', 'ignore')
         # make sure the prop name is unique in the NameSection
         if name not in PropNames:
            PropNames.append(name)
            nameoffset = len(NameSection)
            name = PackData(name, 1)
            NameSection.extend(name)
         else:
            nameoffset = 0
            loc = PropNames.index(name)
            for idx in range(loc):
               nameoffset += len(PropNames[idx]) + 1
         nameoffset = nameoffset | 0x80000000; # turn on MSB to signify its name offset
         
      else:
         nameoffset = Props.getAttribute("id")
         if(nameoffset.isdigit() is True):
            nameoffset = eval(nameoffset)  
         else:
            print "ERROR *** Prop id invalid : '%s'" % nameoffset
            raise ValueError(nameoffset)
            
      visibility_type = Props.getAttribute("visibility_type").strip()
      
      #char_data = re.sub("(\s*|\n)", '', Props.firstChild.data)
      char_data = ""
      for child in Props.childNodes:
         if child.nodeType == child.COMMENT_NODE:
            continue
         char_data = char_data + re.sub("(\s*|\n)", '', child.data)
      
      type = int(Props.getAttribute("type"), 16)
      if type == 0x02:
         try:
            value = eval(char_data)
         except NameError:
            value= str(char_data)
      elif type == 0x08: # DALPROP_ATTR_TYPE_BYTE_SEQ
         value = char_data.split(',')
         for index in range(len(value) - 1):
            value[index] = value[index].lstrip()
            value[index] = eval(value[index])   
         value.insert(0, len(value) - 1)
         # fix the length and pop the 'end'
         value[0] -= 1
         if "end" == value[len(value) - 1]:
            value.pop()
         while len(value)%4 != 0:
            value.append(0)
         valLen +=len(value)-4
      elif type == 0x12:
         try:
            StructPtrs.index(char_data)
         except ValueError:
            StructPtrs.append(char_data)
         value = StructPtrs.index(char_data)
      else:
         if char_data in GlobalOffset:
            value = GlobalOffset[char_data]
         else:
            if type == 0x18:
               value = len(ByteSection)
               ByteSection.extend(GlobalDef[char_data])
            elif type == 0x11:
               value = len(StringSection)
               StringSection.extend(GlobalDef[char_data])
            elif type == 0x14:
               value = len(UintSection)
               UintSection.extend(GlobalDef[char_data])
         GlobalOffset[char_data] = value
      if (str(visibility_type).lower() == "hidden"): 
         type = type | 0x80000000; # turn on 31st bit to signify its hidden
      DevSection.extend([type, nameoffset, value])
   
   #print DeviceID len(type)+len(nameoffset)+len(value)
   DevSection.append(0xFF00FF00)
   # The length of the End marker
   valLen+=4
   DevSection.append(valLen)

def PrintModDir(ModName, DriverList):
   ConfigHeaderLen = 24 + 4*len(DeviceID)
   global UintSection, DevSection
   for i in StringDeviceNames:
      StringDeviceNames[i][1] +=ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection)
   filename = "DALModDir_" + ModName +".c"
   filepath = os.path.join(DirName, filename)
   if(os.path.exists(filepath)):
      os.chmod(filepath, stat.S_IWUSR | stat.S_IRUSR | stat.S_IWGRP | stat.S_IRGRP | stat.S_IWOTH | stat.S_IROTH)
   MODDIR = open(DirName + "/" + filename, 'w')
   MODDIR.write ("/*#==============================================================================\n   Copyright (c)" + str(date.today().year) + " Qualcomm Technologies, Inc .\n   All Rights Reserved.\n   Qualcomm Confidential and Proprietary\n\n# THIS IS AN AUTO-GENERATED TEMPLATE FILE FOR DAL DRIVER LIST\n# DO NOT MODIFY.\n#================================================================================*/\n")
   MODDIR.write ("#include \"DALStdDef.h\" \n")
   MODDIR.write ("#include \"DALReg.h\" \n")
   MODDIR.write ("#include \"DALSysTypes.h\"\n\n")
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
   PrintStringDeviceNameStruct(0)
   if(len(ConfigStructList)!=0):
      for Col_array in CollisionDictionary:
         MODDIR.write ("\nuint32 "+Col_array+" [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
      MODDIR.write("\n\n\nStringDevice DAL_Mod_driver_list[] = ")
      MODDIR.write("{\n\t\t\t")  
      driver_string_device_len = 0;   
      for list in range(len(ConfigStructList)):
         if(ConfigStructList[list] == "{0,0,0,0,0,0}"): 
            continue
         driver_string_device_len+=1 
         MODDIR.write(ConfigStructList[list])
         if(list <len(ConfigStructList)-1):
            MODDIR.write (",\n\t\t\t")
      if driver_string_device_len ==0:
         MODDIR.write ("{NULL, 0, 0, NULL, 0, NULL}\n")
      MODDIR.write ("\n};\n")		
      MODDIR.write ("\nDALProps gDALModStringDeviceInfoList = {NULL, 0 ,"+str(driver_string_device_len)+", DAL_Mod_driver_list};\n")
   else:
      MODDIR.write ("\nDALProps gDALModStringDeviceInfoList = {NULL, 0 ,0 , NULL};\n")


def CheckDupDriverID(ModName, DriverList):
   # check if 'driver' is duplicated in the DriverList
   idx = 1
   for driver in DriverList:
      if driver in DriverList[idx:] and driver!= "NULL" and driver!= "null":
         # found a duplicate driver and no need to proceed any further
         print "ERROR *** DAL Driver '%s'" % driver, "has been included more than once for SW image '%s'" % ModName
         print "ERROR *** Please check XML files for '%s'" % ModName
         raise ValueError(driver)
      idx += 1


def CheckDupDeviceID(ModName):
   # build the device ID list
   devIDList = []
   for i in range(0, len(DeviceID), 2):
      devIDList.append(DeviceID[i])
   # check if there are any duplicated device IDs
   idx = 1
   for dev in devIDList:
      if dev in devIDList[idx:]:
         # find a duplicate device ID and no need to proceed any further
         print "ERROR *** DAL Device ID 0x%.8x" % dev, "has been included more than once for SW image '%s'" % ModName
         print "ERROR *** Please check XML files for '%s'" % ModName
         raise ValueError(hex(dev))
      idx += 1


def Print_Debug ():
    print "\nSegmentOffset: ", SegmentOffset, "Len: ", len(SegmentOffset)
    print "\nName Section:", NameSection, "Len: ",len(NameSection)
    print "\nByte Section: ", ByteSection, "Len: ",len(ByteSection)
    print "\nString Section:" , StringSection, "Len: ",len(StringSection)
    print "\nUINT Section:", UintSection, "Len: ",len(UintSection)
    print "\nDevice Section: ", DevSection


DirName = sys.argv[2]
if not os.path.isdir(DirName):
      os.mkdir( DirName )
ModList = xml.dom.minidom.parse(sys.argv[1])
for Mod in ModList.getElementsByTagName("module"):
   DriverList = []
   ModName = Mod.getAttribute("name")
   for Driver in Mod.getElementsByTagName("driver"):
      GlobalDef = {}
      DriverList.append(Driver.getAttribute("name"))
      GlobalDef = GenerateGlobalDef(Driver)
      GlobalOffset = {}
      for Device in Driver.getElementsByTagName("device"):
         GenerateArray(Driver.getAttribute("name"),Device, GlobalDef, GlobalOffset)
   DevSection.pop()
   AlignSectionAddr()
   # Disabling generation of module driver list as each driver have own local copy
   # check for duplicated driver IDs and device IDs
   CheckDupDriverID(ModName, DriverList)
   CheckDupDeviceID(ModName)
   PrintModDir(ModName, DriverList)
PrintConfig(ModName)
#Print_Debug()
