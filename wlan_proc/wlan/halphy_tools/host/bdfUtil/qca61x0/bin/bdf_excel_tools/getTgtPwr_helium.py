############################################
# Description: Get Target Powers from Board Data file
# Usage: python getTgtPwr.py <bdata.txt> <output.csv> 
############################################

#-------------------------------------------
import sys,re,os
#-------------------------------------------

#CSV_HEADER = '''\
#Data rate,2412,2432,2462,2472,5180,5210,5320,5400,5500,5600,5700,5825
#'''

CSV_HEADER =[]
freq2Gcck_final =[]
freq2Gb_final =[]
freq5G_final =[]
CSV_HEADER_NEW = 'Mode,Rate,SS,1_14,36_64,100_144,149_183'
modeArr = ['11b','11ag','VHT20','VHT40','VHT80']
modeArr_cck = ['11b']
modeArr_nac = ['11ag','VHT20','VHT40','VHT80']
bRate = ['1Mbps','2Mbps','5.5Mbps','11Mbps']
agRate = ['6-24Mbps','36Mbps','48Mbps','54Mbps']
nacRate = ['MCS0','MCS1-2','MCS3-4',
           'MCS5','MCS6','MCS7','MCS8','MCS9',
           'MCS15','MCS16','MCS17','MCS18','MCS19',
           'MCS25','MCS26','MCS27','MCS28','MCS29',
           'MCS35','MCS36','MCS37','MCS38','MCS39',
           'NA'
               ]

rateNameDict = {
    "11bg": ["1Mbps","2Mbps","5.5Mbps","11Mbps","6Mbps","9Mbps","12Mbps","18Mbps","24Mbps","36Mbps","48Mbps","54Mbps"],
    "DupExt": ["DUP_CCK","DUP_OFDM","EXT_CCK","EXT_OFDM","XR"]
    }

#determine mode/rate's name + stream based on mode + rateIndex
def getModeRateStream(mode,rateIndex):
    if mode != "11bg":
        modeName = mode
    else:
        if (rateIndex <= 3):
            modeName = 'CCK'
        else:
            modeName = 'LEGACY'

    numStream = 1
    
    if (mode in ["11bg", "DupExt"]):
        rateName = rateNameDict[mode][rateIndex]
    else:
        if 'VHT' in mode:
            rateName = "MCS%d" % (rateIndex % 10)
            numStream = rateIndex / 10 + 1
        else:
            rateName = "MCS%d" % (rateIndex)
                
    return (modeName, rateName, numStream)


#-------------------------------------------
# MAIN
if __name__ == '__main__':

    print "Usage:.py <bdata.txt> <output.csv>\n\n"
    
    if len(sys.argv) >= 2:
        bdFilePath = sys.argv[1]
    else:
        bdFilePath = "bdwlan.txt"

    if len(sys.argv) >= 3:
        tgtPwrFilePath = sys.argv[2]
    else:
        tgtPwrFilePath = "tgtPwrs_helium_new.csv"

    # Open Board Data Txt File
    bdFile = open(bdFilePath,'r')
    bdFileStr = bdFile.read()
    
    tgtPwrFile = open(tgtPwrFilePath,'w')

    # check compatibility
    searchStr = 'flag2__0_0 (.*)'
        
    m = re.search(searchStr,bdFileStr)

    if m:
        sys.exit()
    else:
        # new scheme
        print "Parsing CCK.."

        #write excel header
        tgtPwrFile.write("%s\n" % CSV_HEADER_NEW)
        
        for mode in ['11bg', 'HT20', 'HT40', 'VHT20', 'VHT40', 'VHT80', 'VHT160', 'DupExt']:
            modePowers = re.findall("targetPowerR2PTable_%s_(.*)" % (mode) ,bdFileStr)
            modeFreqPowers = []
            #convert to arrays of power
            for freqRangeIndex in range(0,4):
                modeFreqPowers.append([x for x in re.split(' ',modePowers[freqRangeIndex]) if x])
                print "%d\n %s" % (freqRangeIndex, modeFreqPowers[freqRangeIndex])

            #put into excel line by line
            rateIndex = 0
            
            for i in range(1, len(modeFreqPowers[0])):
                #determine mode/rate's name + stream based on mode + rateIndex
                (modeName,rateName,numStream) = getModeRateStream(mode,rateIndex)
                #write mode & rate & stream
                
                tgtPwrFile.write('%s,%s,%s' % (modeName, rateName, numStream))

                #write power for each frequency range
                for freqRangeIndex in range(0,4):
                    tgtPwrFile.write(",%s" % (float(modeFreqPowers[freqRangeIndex][i])/2))
                tgtPwrFile.write("\n")
                rateIndex = rateIndex + 1
                    
            #csvCell = re.split("[,\n]",csvLine)

                    
            # tgtPwrDict[key].append(float(tgt[t])/2)
       

        
        
        # Open Target Power csv File
        # tgtPwrFile = open(tgtPwrFilePath,'w')
        # tgtPwrFile.write(CSV_HEADER_NEW)
        # tgtPwrFile.write(dict2Txt_new(tgtPwrDict))

    tgtPwrFile.close()
    
        
    print "Created file : %s" %tgtPwrFilePath
    print "Done"
