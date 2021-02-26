############################################
# Description: convert target power txt file to board data text file
# Usage: python setTgtPwr.py <input.csv> <bdata.txt>
############################################

#-------------------------------------------
import sys,re,os
#-------------------------------------------
tgtPwrsDict = {}
freqRangeList = ['1_14', '36_64', '100_144', '149_183']


#-------------------------------------------
def parseTgtPwr(csvStr):
    # convert CSV file to local dictionary
    mode = prevMode = ''
    rate = 0
    for csvLine in re.split("\n",csvStr):
        if len(csvLine) > 0:
            csvCell = re.split("[,\n]",csvLine)

            if csvCell[0] in ['Mode']: #just the header -> next
                continue
            elif csvCell[0] in ['CCK','LEGACY']:
                mode = '11bg'
            else:
                mode = csvCell[0]

            if mode != prevMode: # new mode
                prevMode = mode
                rate = 0
                tgtPwrsDict[mode] = []
                for freqRange in range(0,4):
                    tgtPwrsDict[mode].append('')

            tgtPwrMode = tgtPwrsDict[mode]
            for freqRange in range(0,4):
                pwr = int(float(csvCell[3 + freqRange]) * 2)
                tgtPwrMode[freqRange] = tgtPwrMode[freqRange] +  ' %s' % (pwr)


            #     key = '%s_%s_%s' %(csvCell[0],csvCell[1],csvCell[2])
            # else:
            #     key = '%s_%s_%s' %(csvCell[0],csvCell[1],csvCell[2][3])
            # if key in tgtpwrNewDict:
            #     tgtpwrNewDict[key] = []
            # tgtpwrNewDict[key] = csvCell[3:len(csvCell)]


#-------------------------------------------
def tgtPwr2BDfile(bdFileStr):
    # Print target power dictionary to BD file
    # CCK
    for freqRangeIndex in range(0,4):
        for mode in tgtPwrsDict:
            tag = 'targetPowerR2PTable_%s_%s_0_0' % (mode, freqRangeList[freqRangeIndex])
            searchStr = '%s(.*)' %tag
            subStr = '%s %s' %(tag,tgtPwrsDict[mode][freqRangeIndex])
            bdFileStr = re.sub(searchStr,subStr,bdFileStr)

    return bdFileStr

#-------------------------------------------
# MAIN
if __name__ == '__main__':

    if len(sys.argv) >= 2:
        tgtPwrFilePath = sys.argv[1]
    else:
        tgtPwrFilePath = "tgtPwrs_helium.csv"

    if len(sys.argv) >= 3:
        bdFilePath = sys.argv[2]
    else:
        bdFilePath = "bdwlan.txt"


    # Open Target Power csv File
    tgtPwrFile = open(tgtPwrFilePath,'r')
    tgtPwrStr = tgtPwrFile.read()

    # Open Board Data Txt File
    bdFile = open(bdFilePath,'r')
    bdFileStr = bdFile.read()

    if (True):
        # new scheme
        print "Parsing csv file..."
        parseTgtPwr(tgtPwrStr)
        print "Editing text file..."
        bdFileStr = tgtPwr2BDfile(bdFileStr)
    else:
        print "No support"

    # Make new Board Data file
    outputFilePath = re.sub('.txt','_edit.txt',bdFilePath)
    outputFile = open(outputFilePath,'w')
    outputFile.write(bdFileStr)
    outputFile.close()

    print "Edited file : %s" %outputFilePath

    print "Done"

#-------------------------------------------
