############################################
# Description: Get Target Powers from Board Data file
# Usage: python getCtlPwr.py <bdata.txt> <output.csv> 
############################################

#-------------------------------------------
import sys,re,os
#-------------------------------------------

CSV_HEADER = '''\
Band,Mode,Beam Forming,Reg Domain,Chain Mask,Stream Mask,Frequencies
'''
emptyCells = ',,,,,,'
verbose = 0

regDict = {
      'FCC'      : '1',
      'MKK'      : '4',
      'ETSI'     : '3',
      'SD_NO_CTL': 'E',
      'NO_CTL'   : 'F',
    }

modeBdf2Csv = {
    'VHT80p80_PrimaryLowerFreq'     :   '80p80 Fp<Fs',
    'VHT80p80_PrimaryHigherFreq'    :   '80p80 Fp>Fs',
}
    
modeDict = {
    'LEGACY'   : '0',
    '11B'      : '1',
    'VHT20'    : '2',
    'VHT40'    : '3',
    'VHT80'    : '4',
    'NONHT-DUP'      : '5',
    'VHT160'         : '6',
    'VHT80p80 Fp<Fs'    : '8',
    'VHT80p80 Fp>Fs'    : '9',
    }

modeDict_legacy = {
    '11A'    : '0',
    '11B'    : '1',
    '11G'    : '2',
    'TURBO'  : '3',
    '108G'   : '4',
    'HT20' : '5',
#    '11A_HT20' : '6',
    'HT40' : '7',
#    '11A_HT40' : '8',
    'VHT80' : '9',
    'VHT20' : 'a',
#    '11A_VHT20' : 'b',
    'VHT40' : 'c',
#    '11A_VHT40' : 'd',

    }

modeAdjust = {
              '6' : '5',
              '8' : '7',
              'b' : 'a',
              'd' : 'c',
    }

def getKey(dictn,val):
    for k in dictn:
        if (val == dictn[k]):
            return k

#-------------------------------------------
def pier2freq(band,pier):
    pier = int(pier)
    if int(band) == 2:
        return str(2300 + pier)
    else:
        return str(5000 + (pier-40)*5)
#-------------------------------------------
def getNoOfChains(bdFileStr):
    searchStr = 'targetPower2GVHT20Base_G_0_0 (.*)'
    m = re.search(searchStr,bdFileStr)
    if m:
        return re.subn('(\d+)','',m.group(1))[1]

#-------------------------------------------
def parseBoardDataCtlIdx(bdFileStr,band):
    ctlIdxStr = ''
    prevRegDmn = 'x'
    prevMode = 'x'
    regDmnIdx = -1
    modeIdx = -1
    printFreq = False
    retStr = ''
    
    # Find all CTL indices    
    if band == 2:
        searchModeStr = 'ctlIndex(\d)G_(\w+)_mode_(\d+)_\w_0_0 (\w)'
        #ctlIndex5G_VHT80_mode_10_A_0_0 4 
        searchBfRegStr = 'ctlIndex(\d)G_(\w+)_bf_reg_(\d+)_\w_0_0 beamforming:0x(\w) regDmn:0x(\w)'
        searchNumChMaskStr = 'ctlIndex(\d)G_(\w+)_numChMask_(\d+)_\w_0_0 0x(\w)'
        searchNumSSMaskStr = 'ctlIndex(\d)G_(\w+)_numSSMask_(\d+)_\w_0_0 0x(\w)'
    elif band == 5:
        searchStr = 'ctlIndex5G_(\w+)_(\d+)_\w_0_0 mode:0x(\w) beamforming:0x(\w) regDmn:0x(\w)'
    elif band == 80:
        band = 5
        searchStr = 'ctlIndex5G_(.*?)_(\d+)_\w_0_0 mode:0x(\w) beamforming:0x(\w) regDmn:0x(\w)'


    ctlIdxModeArr = re.findall(searchModeStr, bdFileStr)
    ctlIdxBfRegArr = re.findall(searchBfRegStr, bdFileStr)
    ctlIdxNumChMaskArr = re.findall(searchNumChMaskStr, bdFileStr)
    ctlIdxNumSSMaskArr = re.findall(searchNumSSMaskStr, bdFileStr)
  
    for i in range(0,len(ctlIdxModeArr)):
        bandEdgeStr = ''
        powerValStr = ''
        freqStr = ''
        
        band = ctlIdxModeArr[i][0]
        modeName = ctlIdxModeArr[i][1]
        idx = ctlIdxModeArr[i][2]
        mode = ctlIdxModeArr[i][3]
        bf = ctlIdxBfRegArr[i][3]
        reg = ctlIdxBfRegArr[i][4]
        cmask = ctlIdxNumChMaskArr[i][3] 
        smask = ctlIdxNumSSMaskArr[i][3] 

        if (verbose >=1):
            print "%s %s" % (modeName, mode)

        if (band == '2'):
            b = 'G'
        else:
            b = 'A'

        #if int(mode)>=7: #don't do anything yet
        #    continue
        #    priMode = modeName[12:len(modeName)]
        #    searchStr = 'ctlModeExt_HT80p80_%s_A_0_0 (.*)' %priMode
        #    extBitsArr = re.findall('(\d)',re.findall(searchStr,bdFileStr)[0])
        #    mode = extBitsArr[int(idx)]
        
        #if (int(mode) == 0) and int(band) == 5:
        #	if (modeName.find('legacy')<0):
        # 		continue
        
        if (int(mode) < 7):
            modeKey = getKey(modeDict, mode)
        else:
            modeKey = modeName

        ctlIdxStr = '%sGHz,%s,%s,%s,0x%s,0x%s,' \
        %(band,modeKey,bf,getKey(regDict,reg),cmask,smask)
        
        # check if reg domain has changed from previous    
        if not (prevRegDmn == reg):
            regDmnIdx = regDmnIdx+1
        
        if not (prevMode == mode):
            regDmnIdx = 0
            prevMode = mode
            printFreq = True
            modeIdx = modeIdx + 1
        else:
            printFreq = False
        
        prevRegDmn = reg
        prevMode = mode    
        
        # search freqyency bins
        searchStr = r'ctlFreqbin%sG_%s_%s_0_0 (.*)' %(band,modeName,b)
        
        freqStrArr = re.findall(searchStr, bdFileStr)[0]
        for f in re.findall("\d+",freqStrArr):
            if printFreq : 
                freqStr = freqStr + pier2freq(band,f) + ','
            else:
                freqStr = freqStr + ','
                
        # Add mode and frequency in same line
        lineStr = ctlIdxStr + freqStr + '\n'
        
        searchStr = 'ctlData%sG_%s_%s_%s_0_0 (.*)' %(band,modeName,idx,b)
        ctlDataStr = re.findall(searchStr, bdFileStr)[0]
        
        # Search for CTL data line in bd file
        for c in re.findall("(\w+)",ctlDataStr):
            power = 0
            val = int(c,16) 
            bit = (val>>7)%2
            bandEdgeStr = bandEdgeStr + str(bit) + ','
            
            if bit:
                power = (float(val) - 128)/2
            else:
                power = float(val)/2
                
            powerValStr = powerValStr + str(power) + ','
            
        # print all data
        lineStr = lineStr + emptyCells +  bandEdgeStr + 'bandEdgeBit,' +'\n'
        lineStr = lineStr + emptyCells + powerValStr + 'CTLPwr,' + '\n'
        retStr = retStr + lineStr
    
    return retStr


#-------------------------------------------
# MAIN
if __name__ == '__main__':
    
    ctlIdxStr5G_80p80 = ''
    
    # if len(sys.argv) !=3:
    #     print "Wrong Arguements\nUsage:getCtlPwr.py <bdata.txt> <output.csv>\nExiting.."
    #     sys.exit()
        
    print "Usage:.py <bdata.txt> <output.csv>\n\n"
    
    if len(sys.argv) >= 2:
        bdFilePath = sys.argv[1]
    else:
        bdFilePath = "bdwlan.txt"

    if len(sys.argv) >= 3:
        tgtPwrFilePath = sys.argv[2]
    else:
        tgtPwrFilePath = "ctlPwrs_helium_new.csv"


    # Open Board Data Txt File
    bdFile = open(bdFilePath,'r')
    bdFileStr = bdFile.read()
    
    # check compatibility
    searchStr = 'flag2__0_0 (.*)'
    m = re.search(searchStr,bdFileStr)
    if m:
        print "flag2 found in board data file\nExiting.."
        sys.exit()
    else:
        # Board Data file -> CSV file
        print "Parsing 2G.."
        ctlIdxStr2G = parseBoardDataCtlIdx(bdFileStr,2)
        #print "Parsing 5G.."
        #ctlIdxStr5G = parseBoardDataCtlIdx(bdFileStr,5)
        #print "Parsing 80p80.."
        #ctlIdxStr5G_80p80 = parseBoardDataCtlIdx(bdFileStr,80)
    
        
    # Open Target Power csv File
    tgtPwrFile = open(tgtPwrFilePath,'w')
    tgtPwrFile.write(CSV_HEADER)
    tgtPwrFile.write(ctlIdxStr2G)
#    tgtPwrFile.write((tgtPwrDict))
    tgtPwrFile.close()
    
    print "Created file : %s" %tgtPwrFilePath
    print "Done"
