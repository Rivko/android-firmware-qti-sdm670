import xml.etree.ElementTree as ET
import xml.etree.ElementTree as ET
import types
import collections
import sys
from cStringIO import StringIO
import csv
from types import *
import getopt

NEWLINE  = '\n'
TAB = '\t'
TAB2 = TAB + TAB
TAB3 = TAB2 + TAB
xml_temp = ""

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    env.AddMethod(xcel_parser_wrapper, "AddACDeviceInfo")
    device_bld = env.Builder(action = process_csv)
    env.Append(BUILDERS = {'DeviceInfoExcelParse' : device_bld})
    return

def xcel_parser_wrapper(env, target, source):
    c_file,xml_file = env.DeviceInfoExcelParse(target,source)

    # Add the xml & c file to the devcfg image
    if 'USES_DEVCFG' in env:
        DEVCFG_IMG = ['DAL_DEVCFG_IMG','DAL_DEVCFG_HYP_IMG','DAL_DEVCFG_QSEE_IMG']

        env.AddDevCfgInfo(DEVCFG_IMG,
                {
                    'devcfg_xml' : [xml_file, c_file]
                    })

def process_csv(target, source, env):

    #These are two output files
    output_c = str(target[0])
    output_xml = str(target[1])

    #These are the input files
    input_csv = str(source[0])

    xcel_parse(input_csv,output_c,output_xml)

def toHex(num):
    return hex(num).rstrip("L")


def indent(lines, tabSize = 4):
    tab = ""
    for i in range(tabSize):
        tab += " "
    out = ""
    for line in lines.splitlines(True):
        out += tab + line
    return out

# Converts a string with spaces to spinal_case (all lowercase snake_case)
# Does not convert camelCase
def toSpinalCase(s):
    return str.replace(str.lower(s), ' ', '_')

# Generate c code for includes and struct declarations
def InitialDefinitions(code):

    code += '#include "ACCommon.h"' + '\n' + '#include "AccessControl.h"' + '\n'+ '\n'

    return code

def parseXcelFile(xcelFile):
    global xml_temp;

    csv_data = open(xcelFile,'rb')
    reader = csv.reader(csv_data)
    row_num = 0
    out = ""
    dev_list = ""

    for row in reader:
        col_num = 0
        rg = 0
        esi = 0
        isi = 0
        esi_count = 0
        isi_count = 0
        rg_count = 0
        dev_name = ""
        temp = ""
        temp1 = ""
        if(row_num == 0):
            header = row;
            row_num += 1
            continue

        for col in row:
            if col not in (None,""):
                if(header[col_num] == "device_name"):
                    dev_name = str(col)
                    xml_temp += "<driver name=\"NULL\">" + NEWLINE + TAB + "<device id=\"" + dev_name +"\">" + NEWLINE
                    xml_temp += TAB2 + "<props name=\"di\" type=DALPROP_ATTR_TYPE_STRUCT_PTR>" + NEWLINE
                    xml_temp += TAB2 + TAB + "device_" + dev_name.lower() + NEWLINE
                    xml_temp += TAB2 + "</props>" + NEWLINE + TAB + "</device>" + NEWLINE + "</driver>" + NEWLINE + NEWLINE
                    dev_list += TAB + "&DEVICE_" + dev_name + "," + NEWLINE
                    temp += "};" + NEWLINE + NEWLINE + "ACDeviceInfo device_" + dev_name.lower() + " =" + NEWLINE + "{" + NEWLINE

                elif(header[col_num] == "smmu_instance"):
                    temp += TAB + ".eSmmuInstance = " + TAB + str(col) + "," + NEWLINE

                elif(header[col_num] == "vmidmt_instance"):
                    temp += TAB + ".eVmidmtInstance = " + TAB + str(col) + "," + NEWLINE

                elif(header[col_num] == "region"):
                    if(rg == 0):
                        temp+= TAB + ".pMappings = " + "AC_REGION_" + dev_name + "," + NEWLINE
                        rg = 1
                        out += "static ACRegion AC_REGION_" + dev_name + "[]=" + NEWLINE + "{" + NEWLINE
                    rg_count += 1
                    out += TAB + "{" + NEWLINE
                    out += TAB2 + ".uMemoryStart = " + TAB + str(col) + "," + NEWLINE

                elif(header[col_num] == "size"):
                    out += TAB2 + ".uMemorySize = " + TAB + str(col) + "," + NEWLINE
                    out += TAB + "}," + NEWLINE

                elif(header[col_num] == "external_sid"):
                    if(esi == 0):
                        esi = 1
                        temp1 += "};" + NEWLINE + NEWLINE + "static uint32 AC_ExternalSID_" + dev_name + "[]=" + NEWLINE + "{" + NEWLINE
                        temp += TAB + ".pExternalSidList = AC_ExternalSID_" + dev_name + "," + NEWLINE
                    esi_count += 1
                    temp1 += TAB + str(col) + "," + NEWLINE

                elif(header[col_num] == "internal_sid"):
                    if(isi == 0):
                        isi = 1
                        temp1 += "};" + NEWLINE + NEWLINE + "static uint32 AC_InternalSID_" + dev_name + "[]=" + NEWLINE + "{" + NEWLINE
                        temp += TAB + ".pInternalSidList = AC_InternalSID_" + dev_name + "," + NEWLINE
                    isi_count += 1
                    temp1 += TAB + str(col) + "," + NEWLINE
            col_num += 1
        row_num += 1
        temp += TAB + ".uNumMappings = " + str(rg_count) + "," + NEWLINE
        temp += TAB + ".uNumExternalSids = " + str(esi_count) + "," + NEWLINE
        temp += TAB + ".uNumInternalSids = " + str(isi_count) + "," + NEWLINE
        temp += "};" + NEWLINE + NEWLINE
        out += temp1
        out += temp

    #out += NEWLINE + "ACDeviceInfo* DeviceGroupInfo[] = " + NEWLINE + "{" + NEWLINE + dev_list + NEWLINE + "};" + NEWLINE
    return out

# xmlFile (str) - the name/location of the XML file to parse
# returns - c code for CB arrays, includes as many arrays as there are Apertures in the XML
def generateResourceInfoFromXCelFile(xcelFile):
    #devices = translateXCEL(xcelFile)

    # Spit out #include and structure declarations
    code = ""
    code = InitialDefinitions(code)
    #code += "const ACDeviceInfo deviceList[] = " + '\n' + "{" + "\n"
    code += parseXcelFile(xcelFile)
    #code += NEWLINE + "const uint32 uNumDevices = sizeof(DeviceGroupInfo)/sizeof(DeviceGroupInfo[0]);" + '\n' + '\n'
    #Populate the array of struct
    return code



def xcel_parse(source_path, dest_path_c, dest_path_xml):
    global xml_temp
    code = generateResourceInfoFromXCelFile(source_path)


    out_file_xml = open(dest_path_xml,"w")
    xml_temp = xml_temp.encode('ascii','ignore')
    out_file_xml.write(xml_temp)
    out_file_xml.close()

    out_file_c = open(dest_path_c,"w")
    code = code.encode('ascii','ignore')
    out_file_c.write(code)
    out_file_c.close()
    xml_temp = ""
    return None

def main(argv):
    output="."
    if type(argv) is StringType:
        argv = argv.split(' ')
    try:
        opts, args = getopt.getopt(argv, '', ["xml=","xcel=","out="])
    except getopt.GetoptError, err:
        print "Invalid input args"
        sys.exit(2)

    for opt,arg in opts:
        if(opt == "--xml"):
            xml = arg
        if(opt == "--xcel"):
            xcel = arg
        if(opt == "--out"):
            output = arg;

    xcel_parse(xcel,output,xml)

if __name__ == "__main__":
    main(sys.argv[1:])
