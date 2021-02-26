"""Script to convert timestamps from ADSPPM logs into human readable formats"""

import re
import sys
import argparse

####################################
# Constants
###################################
DELIMITER = ": "
TIMESTAMP_LEN_HEX = len("0x00000000")
MICROSECOND = 1.0E6
NANOSECOND = 1.0E9
FREQUENCY = 19200000.0  # Force a floating point calculation since messages are logged with a microsecond resolution
PROFILE_LOG_TAG = "[PROFILE]"
ADSPPM_SRC_PATH = r'\adsp_proc\core\power\adsppm\src'
PROFILE_TEST_SEPARATOR = "==============================================================================================" # Search for the first instance of thie to see where the tests start


####################################
# Helper functions
####################################

def log_timestamp(line):
    """Extract the timestamp from the provided line. If the delimiter char isn't found in the provided line, the unmodified line is returned. """
    timestampEnd = 0
    try: 
        timestampEnd = line.index(DELIMITER)
    except ValueError:
        # ':' wasn't found, assume that it's because a timestamp was provided instead of a full line
        timestampEnd = len(line)

    return line[:timestampEnd]


def remove_timestamp_decimals(timestamp):
    """
    Convert the timestamp from the dotted-decimal format to decimal format
    i.e. ssss.mmm.uuu.nnn -> ssss.mmmuuunnn

    If the provided timestamp doesn't have any '.'s, the timestamp is returned unmodified
    """
    firstDecimalIndex = 0
    try:
        firstDecimalIndex = timestamp.index('.')
    except ValueError:
        # No '.' found, the timestamp doesn't have any decimals, so nothing to do
        print "Didn't find any decimals in " + timestamp
        return timestamp

    return timestamp[:firstDecimalIndex+1] + timestamp[firstDecimalIndex+1:].replace('.','')  # Don't return a string with the extra '.'s in it


def add_timestamp_decimals(timestamp):
    """ 
    Convert the timestamp from decimal to dotted-decimal format
    i.e. ssss.mmmuuunnn -> ssss.mmm.uuu.nnn

    If the provided timestamp already has more than one '.', the timestamp is returned unmodified.
    """
    if timestamp.count('.') > 1:
        # Timestamp already has the separators, no need to add them
        return line

    timestampSecondsFloat = float(timestamp)
    
    timestampPadded = '{:>.09f}'.format(timestampSecondsFloat)
    decimalIndex = timestampPadded.index('.')

    timestampSeparated = str(int(timestampSecondsFloat))

    for pos in range(decimalIndex,len(timestampPadded)-1):
        if (pos - decimalIndex) % 3 == 0:
            timestampSeparated += '.'
        timestampSeparated += timestampPadded[pos+1]

    return timestampSeparated

def timestamp_to_seconds(line):
    """Convert the HEX timestamp from ulogdump.cmm to seconds, assuming a frequency of FREQUENCY. 
        If a '.' is found in the timestamp, attempt to return a decimal version of the timestamp
        i.e. ssss.mmmuuunnn, not ssss.mmm.uuu.nnn
    """
    retstr = ""
    try:
        decimalIndex = log_timestamp(line).index('.')
        retstr = remove_timestamp_decimals(log_timestamp(line))
    except ValueError:
        retstr = str(int(line[0:TIMESTAMP_LEN_HEX], 0)/FREQUENCY)
    
    return retstr


def splice_timestamp_and_message(timestamp, line):
    """
    Append the message contained in 'line' to the detted-decimalized version of the provided timestamp. 
    The timestamp is expected to have already be converted into seconds
    """
    return add_timestamp_decimals(timestamp) + line[line.index(DELIMITER):]
    

def natural_sort_keys (line):
    """ 
    
    From http://stackoverflow.com/a/3033342/4503738
    """
    return [int(s) if s.isdigit() else s for s in re.split(r'(\d\.\d\.\d\.\d+)', line)]


####################################
# Main Script
####################################

parser = argparse.ArgumentParser(description='Post-Processing script for ADSPPM driver and test logs.')
parser.add_argument('logdir', help="Directory containing the driver log (ADSPPM Log.ulog) and the test log (ADSPPMTEST Log.ulog)")
parser.add_argument('-o', '--outdir', help="Directory in which to store script output. Default: same as logdir")
parser.add_argument("-r", "--raw", help="Skip some output formatting, since the output will be used elsewhere", action="store_true")
parser.add_argument("-b", "--build", help = "Root directory of the ADSP build")

args = parser.parse_args()

indir = args.logdir
print "Using input directory: " + indir

driverInFilePath = indir + r'\ADSPPM Log.ulog'
testInFilePath = indir + r'\ADSPPMTEST Log.ulog'

if args.outdir:
    outdir = args.outdir
else:
    outdir = indir
print "Using output directory: " + outdir

if args.build:
    adsppmSrcDir = args.build + ADSPPM_SRC_PATH
else:
    adsppmSrcDir = ""

driverOutFilePath = outdir + r'\ADSPPM_Log_readable.txt'
testOutFilePath = outdir + r'\ADSPPM_Test_Log_readable.txt'
mergedOutFilePath = outdir + r'\MergedProfileLogs.txt'
functionSummaryOutFilePath = outdir + r'\FunctionSummary.txt'

driverOutFile = open(driverOutFilePath, 'wb')
testOutFile = open(testOutFilePath, 'wb')
functionSummaryOutFile = open(functionSummaryOutFilePath, 'wb')
mergedOutFile = open(mergedOutFilePath, 'wb')

if (driverOutFile is None) or (testOutFile is None) or (mergedOutFile is None) or (functionSummaryOutFile is None):
    print "Couldn't open output files for write. Exiting."
    exit()

# Process the test log first, so we can only select those driver logs result from the test
profileTestLogs_raw = []
profileTestLogs = []

with open(testInFilePath, 'rb') as testinfile:
    for line in sorted(testinfile):
        if not line.startswith('\r'):
            timestamp = timestamp_to_seconds(line)
            ending = ""
            if line.endswith("\r\n"):
                line = line[0:-2]
            elif line.endswith("\n"):
                line = line[0:-1]

            profileTestLogs_raw.append(splice_timestamp_and_message(timestamp, line))
            testOutFile.write(splice_timestamp_and_message(timestamp, line))

# Remove any instances of the delimiter in the test logs, so it doesn't mess with the formatting of the merged output
# Allow delimiters in the driver log mesages, since it's likely that any info entered there makes sense to be placed in separate columns
# in the merged output.
for line in profileTestLogs_raw:
    splitline = line.split(DELIMITER, 1)
    message = splitline[1]
    if PROFILE_LOG_TAG not in line and DELIMITER in message:
        message = message.replace(DELIMITER, ' ')

    profileTestLogs.append(DELIMITER.join(splitline[0:1]) + DELIMITER + message)

#
# Merge the driver and test logs, then print it along with timeDeltas, where necessary
#

# Find the start of the first test, using the test header
firstTestIndex = 0
firstTestLine = ""

for i, s in enumerate(profileTestLogs):
    if PROFILE_TEST_SEPARATOR in s:
        firstTestLine = s
        firstTestIndex = i
        break

firstTestTimestamp = remove_timestamp_decimals(log_timestamp(firstTestLine))
testinfile.close()
testOutFile.close()

# Process the driver log
# Keep track of:
#   1. Functions visited by the test, the entry and exit counts, the total amount of time spent in the function, and the max, min, and average call durations.
#       This is a dict of dicts, since it's more readable than using a dict of lists: 
#           functions = {
#               func0 : {NumStarts : 0, NumEnds : 0, TotalTime : 0, MaxTime : 0, MinTime : 0, AvgTime : 0 },
#               func1 : {NumStarts : 0, NumEnds : 0, TotalTime : 0, MaxTime : 0, MinTime : 0, AvgTime : 0 },
#               ...
#           }
#   2. A stack of ProfileStart and ProfileEnd logs and the associated function, used to compute the duration of each API call.
#       This is a list of lists, instead of a dictionary, since we require this to be LIFO whereas Python dicts are unordered:
#           stack = [
#               [Func0, startTime0],
#               [Func1, startTime1],
#               ...
#           ]
#   3. A dictionary mapping timestamps for ProfileEnd logs to the function name and call duration, to simplify outputting this data
#   4. The actual logs that need to be output into the readable Driver Log

functions = {}
stack = []
endTimeDict = {}    # { Timestamp_of_profileEnd : (functionName, timeDelta) } 
profileLogs = []    # All logs with the [PROFILE] tag in the driver and test logs, that were generated by the test

profileLogs = [remove_timestamp_decimals(line) for line in profileTestLogs if PROFILE_LOG_TAG in line]

# Extracted logs to output. Use a separate list for this, since it's unsafe to modify the original list while iterating through it
finalDriverLogs = [] # Outputtable logs with all the required info

driverOutFile.write("ADSPPM Profile Logs. Assuming ADSP Clock Frequency is " + str(FREQUENCY) + "Hz\r\n")

# Process ADSPPM_Log first
with open(driverInFilePath,'rb') as logfile:
    profileLogs += [line for line in logfile if PROFILE_LOG_TAG in line]

for idx, line in enumerate(sorted(profileLogs)):
    # Don't waste time on empty lines
    if line[0] != '\r' and float(timestamp_to_seconds(log_timestamp(line))) >= float(firstTestTimestamp):
        # Make sure the line is terminated in a way Windows will understand, and remove any additional '\n's
        if line[-2:] != '\r\n':
            if line[-1]=='\n':
                # Terminated with a '\n' but not a '\r\n'
                line = line[0:-1] + '\r\n'
            else:
                # No newline/carriage-return at the end, so append one
                line = line + '\r\n'
            
        timestamp = timestamp_to_seconds(line)

        splitline = line.split(DELIMITER)
        
        if splitline[2] not in functions:
            functions[splitline[2]] = {"NumStarts" : 0, "NumEnds" : 0, "TotalTime" : 0, "MaxTime" : 0, "MinTime" : 99999999, "AvgTime" : 0} # Use a large MinTime instead of 0, so we can use min() on it
            
        if "ProfileStart" in line:
            functions[splitline[2]]["NumStarts"] += 1
            stack.append([splitline[2], timestamp])
            if line.endswith('\r\n'):
                # Remove the termination so a second one doesn't get added during MergedProfile output
                line = line[:-2]
                
        elif "ProfileEnd" in line:
            functions[splitline[2]]["NumEnds"] += 1
            if stack:
                prevFuncName, profileStartTime = stack.pop()
            else:
                profileStartTime = 0
            
            timeDelta = float(timestamp) - float(profileStartTime)
            separatedTimestamp = add_timestamp_decimals(timestamp)
            endTimeDict[separatedTimestamp] = (splitline[2], timeDelta)

        if 'profile_' not in line:
            finalDriverLogs.append(splice_timestamp_and_message(timestamp, line))

if finalDriverLogs:
    # Leave the time delta out of the driverOutFile, since that's intended to just be a cleaner version of the input driver log.
    driverOutFile.writelines(finalDriverLogs)

driverOutFile.close()

unbalancedFuncs = [func for func in functions if functions[func]["NumStarts"] != functions[func]["NumEnds"]]
columnNames = ["Function Name", "Number of Entries", "Number of Exits", "Total Time Spent (seconds)", "Longest Call (usec)", "Shortest Call (usec)", "Average Call Length (usec)"]
longestFuncNameLen = len(max(functions.keys(), key=len))
longestFuncEnterLen = len(str(max([callDetails["NumStarts"] for callDetails in functions.values()])))
longestFuncExitLen = len(str(max([callDetails["NumEnds"] for callDetails in functions.values()])))
longestFuncTotalTimeLen = len(str(max([callDetails["TotalTime"] for callDetails in functions.values()])))
longestFuncMaxTimeLen = len(str(max([callDetails["MaxTime"] for callDetails in functions.values()])))
longestFuncMinTimeLen = len(str(max([callDetails["MinTime"] for callDetails in functions.values()])))
longestFuncAvgTimeLen = len(str(max([callDetails["AvgTime"] for callDetails in functions.values()])))

longestDurationLen = 0  # initialized with the mergedLog processing below

longestMessage = max([line.split(DELIMITER)[-1] for line in finalDriverLogs] ,key=len)
longestMessageLen = len(longestMessage)

mergedLogs = sorted(finalDriverLogs + profileTestLogs[firstTestIndex:], key=natural_sort_keys)

#mergedLogs = sorted(mergedLogs, key=natural_sort_keys)

highestIndentationLevel = 0
indentation_level = 0
maxIndentationTimestamp = ""

# Add timeDeltas
# - Time since previous log
# - Time between ProfileStart and ProfileEnd for the current function
for logIdx, log in enumerate(mergedLogs):
    logTimestamp = log_timestamp(log)
    logWithDeltas = logTimestamp
    messageStart = log.index(DELIMITER)

    # Splice the timeSinceLastMessage between the timestamp and the message
    if logIdx == 0:
        timeSinceLastMessage = 0.0
    else:
        timeSinceLastMessage = (float(remove_timestamp_decimals(logTimestamp)) - float(remove_timestamp_decimals(log_timestamp(mergedLogs[logIdx - 1])))) * MICROSECOND

    logWithDeltas += DELIMITER + '{0:<.09f}'.format(timeSinceLastMessage)

    if '[INFO]' in log and '[INFO]' + DELIMITER not in log:
        # Note the colon in the second condition. Some of the [INFO] logs have an extra DELIMITER in the middle that will mess with formatting.
        # e.g. "[INFO]: ..."
        logWithDeltas += DELIMITER + '[INFO]' + DELIMITER
        messageStart += len(DELIMITER + '[INFO]' + DELIMITER) - 1
        logWithDeltas += log[messageStart:]
    else:
        logWithDeltas += log[messageStart:]
    
    if PROFILE_LOG_TAG in log:
        if 'ProfileStart' in log:
            indentation_level += 1

        if "ProfileEnd" in log:
            try:
                indentation_level = (indentation_level - 1) if indentation_level > 0 else 0

                # Add the Profile time delta
                funcName, timeDelta = endTimeDict[logTimestamp] 
                timeDelta *= MICROSECOND
                if '\r\n' in logWithDeltas:
                    logWithDeltas = logWithDeltas[0:-2]

                functions[funcName]["TotalTime"] += timeDelta
                functions[funcName]["MaxTime"] = max(functions[funcName]["MaxTime"], timeDelta)
                functions[funcName]["MinTime"] = min(functions[funcName]["MinTime"], timeDelta)

                logWithDeltas += DELIMITER + "{0:<.9f}".format(timeDelta)
                longestDurationLen = len(max(str(timeDelta), str(longestDurationLen), key=len))

            except KeyError as e:
                print "Couldn't find a timedelta for ProfileEndTimestamp " + logTimestamp

    highestIndentationLevel = max(highestIndentationLevel, indentation_level)
    if highestIndentationLevel == indentation_level:
        maxIndentationTimestamp = logTimestamp
    mergedLogs[logIdx] = logWithDeltas

# Calculate the average call time for each function
# NOTE: Updating values in a dict while iterating through it is safe, unlike adding or deleting keys
for func, callDetails in functions.iteritems():
    functions[func]["AvgTime"] = callDetails["TotalTime"]/max(callDetails["NumStarts"], callDetails["NumEnds"])

mergedOutputHeader = ["Timestamp", "Delta from Previous Message (usec)", "Log Tag", "Function Name", "Log Message", "Call Duration, if relevant (usec)"]
testSeparatorLen = str(len(PROFILE_TEST_SEPARATOR) + 3)
timestampColLen = str(len(log_timestamp(mergedLogs[0])) + 3)
deltaColLen = str(len("Delta from previous message (usec)") + 3)
logtagColLen = str(len('[PROFILE]') + 3)
functionColLen = str(max(longestFuncNameLen, len("Function Name")) + 3)
messageColLen = str(longestMessageLen + len("    " * (highestIndentationLevel-1)) + 3)

# Stop the call duration column from going past the test separator.
#durationColLen = str(min((int(testSeparatorLen) - int(logtagColLen) - int(functionColLen) - int(messageColLen) - 1), len("    "*highestIndendationLevel) + longestDurationLen))
durationColLen = str(max(longestDurationLen, len(mergedOutputHeader[-1])))

# Format for merged header: "Timestamp      : Delta from previous message  : Log Tag   : Function Name                  : Log Message  : Call duration, if relevant"
outputHeaderMergedFormat =  '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{:^'   + logtagColLen      +   '}' +   \
                            '{:<'   + functionColLen    +   '}' +   \
                            '{:<'   + messageColLen     +   '}' +   \
                            '{:>'   + durationColLen    +   '}'

# Format for "98.380.878.281: -8.689271000E-03: =============================================================================================="
testHeaderMergedFormat =    '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{:<'   + testSeparatorLen  +   '}'

# Format for "98.380.878.958: 6.770000027E-07: == Test profile_01: Profile register/deregister duration"
testNameMergedFormat =      '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{}'

# Format for "98.380.879.740: 7.820000008E-07: [INFO]  Profile register duration"
testInfoMergedFormat =      '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{:^'   + logtagColLen      +   '}' +   \
                            '{}'

# Format for "38.317.529.531: [PROFILE]: Intr_Init: ProfileStart"
profileStartMergedFormat =  '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{:^'   + logtagColLen      +   '}' +   \
                            '{:<'   + functionColLen    +   '}' +   \
                            '{:<'   + messageColLen     +   '}'

# Format for "38.317.591.823: [PROFILE]: NpaADSPPowerAvailableCb: ProfileEnd"
profileEndMergedFormat =    '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{:^'   + logtagColLen      +   '}' +   \
                            '{:<'   + functionColLen    +   '}' +   \
                            '{:<'   + messageColLen     +   '}' +   \
                            '{:>'   + durationColLen    +   '}'

# Format for "98.380.952.760: 3.354099999E-05: == Test profile_01 [PASSED]"
testFooterMergedFormat =    '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{:>}'

# Format for "98.389.570.677: 7.289999928E-07: == Test Result Summary: Total [7], Passed [7], Failed [0]"
testSummaryMergedFormat =   '{:<'   + timestampColLen   +   '}' +   \
                            '{:^'   + deltaColLen       +   '}' +   \
                            '{}'

# Write the table header
mergedOutFile.write(outputHeaderMergedFormat.format(*mergedOutputHeader) + '\r\n')
outputIndentationLevel = 0

for line in mergedLogs:
    
    splitline = line.split(DELIMITER)

    if PROFILE_TEST_SEPARATOR in line:
        outFormat = "" #testHeaderMergedFormat
        splitline = (DELIMITER*3).split(DELIMITER)
    elif 'Test profile_' in line and (PROFILE_LOG_TAG not in line) and ('PASSED' not in line and 'FAILED' not in line):
        outFormat = testNameMergedFormat
    elif 'PASSED' in line or 'FAILED' in line:
        outFormat = testFooterMergedFormat
    elif '[INFO]' in line:
        outFormat = testInfoMergedFormat
    elif 'ProfileStart' in line:
        outFormat = profileStartMergedFormat
        if not args.raw:
            splitline[splitline.index('ProfileStart')] = ("    " * outputIndentationLevel) + splitline[splitline.index('ProfileStart')]
            outputIndentationLevel += 1

    elif 'Test Result Summary' in line:
        outFormat = testSummaryMergedFormat
    elif 'ProfileEnd' in line:
        outFormat = profileEndMergedFormat
        if not args.raw:
            outputIndentationLevel -= 1 if outputIndentationLevel != 0 else 0
            splitline[splitline.index('ProfileEnd')] = ("    " * outputIndentationLevel) + splitline[splitline.index('ProfileEnd')]

    mergedOutFile.write(outFormat.format(*splitline) + '\r\n')
mergedOutFile.close()


#
# Create the functions summary
#

headerFormat =  "{:<" + str(max(longestFuncNameLen, len(columnNames[0]))        + 3)    + "}"   + \
                "{:^" + str(max(longestFuncEnterLen, len(columnNames[1]))       + 3)    + "}"   + \
                "{:^" + str(max(longestFuncExitLen,len(columnNames[2]))         + 3)    + "}"   + \
                "{:>" + str(max(longestFuncTotalTimeLen, len(columnNames[3]))   + 3)    + "}"   + \
                "{:>" + str(max(longestFuncMaxTimeLen, len(columnNames[4]))     + 3)    + "}"   + \
                "{:>" + str(max(longestFuncMinTimeLen, len(columnNames[5]))     + 3)    + "}"   + \
                "{:>" + str(max(longestFuncAvgTimeLen, len(columnNames[6]))     + 3)    + "}"

rowFormat =     "{:<" + str(max(longestFuncNameLen, len(columnNames[0]))        + 3)    + "}"   + \
                "{:^" + str(max(longestFuncEnterLen, len(columnNames[1]))       + 3)    + "}"   + \
                "{:^" + str(max(longestFuncExitLen,len(columnNames[2]))         + 3)    + "}"   + \
                "{:>" + str(max(longestFuncTotalTimeLen, len(columnNames[3]))   + 3)    + "f}"  + \
                "{:>" + str(max(longestFuncMaxTimeLen, len(columnNames[4]))     + 3)    + "f}"  + \
                "{:>" + str(max(longestFuncMinTimeLen, len(columnNames[5]))     + 3)    + "f}"  + \
                "{:>" + str(max(longestFuncAvgTimeLen, len(columnNames[6]))     + 3)    + "f}"

if args.raw:
    rowSeparator = "\r\n"
else:
    tableSeparator =    '-' *   (max(longestFuncNameLen,        len(columnNames[0])) + 2) + '+' + \
                        '-' *   (max(longestFuncEnterLen,       len(columnNames[1])) + 2) + '+' + \
                        '-' *   (max(longestFuncExitLen,        len(columnNames[2])) + 2) + '+' + \
                        '-' *   (max(longestFuncTotalTimeLen,   len(columnNames[3])) + 2) + '+' + \
                        '-' *   (max(longestFuncMaxTimeLen,     len(columnNames[4])) + 2) + '+' + \
                        '-' *   (max(longestFuncMinTimeLen,     len(columnNames[5])) + 2) + '+' + \
                        '-' *   (max(longestFuncAvgTimeLen,     len(columnNames[6])) + 2) + '+'
    rowSeparator = '\r\n' + tableSeparator + '\r\n'



# Write all function names with their corresponding entry and exit counts, 
# padding function names with whitespace where necessary
functionSummaryOutFile.write(headerFormat.format(*columnNames))
functionSummaryOutFile.write(rowSeparator)
functionSummaryOutFile.writelines(rowSeparator.join([rowFormat.format(*[func, str(functions[func]["NumStarts"]), str(functions[func]["NumEnds"]), (functions[func]["TotalTime"]), (functions[func]["MaxTime"]), (functions[func]["MinTime"]), (functions[func]["AvgTime"])]) for func in functions]))
functionSummaryOutFile.write(rowSeparator)   # Close off the table at the bottom

if unbalancedFuncs:
    functionSummaryOutFile.write("\r\nThe following functions have unequal profile entry and exit logs:\r\n    ")
    functionSummaryOutFile.writelines([func + '\r\n    ' for func in unbalancedFuncs])
    
# Collect info on PROFILE log coverage 
if not adsppmSrcDir:
    print "No build directory provided. Not generating API Summary"
    functionSummaryOutFile.write("\r\nNo build directory provided. API Summary was not generated")

else:
    apiFileList = [r'\common\api\src\api.c',
                   r'\common\core\src\requestmgr.h',
                   r'\common\core\src\clientmgr.h']
    apis = []
    allapis = []

    types = [   'void',
                'uint32',
                'MMPM_STATUS',
                'Adsppm_Status',
                'ADSPPM_DCVS_STATUS',
                'MmpmClientClassType',
                'const AdsppmClientType',
                'MmpmDcvsParticipationType',
                'AdsppmClientsDcvsParticipationType']

    functionSummaryOutFile.write("\r\n========== List of ALL Public ADSPPM APIs ==========\r\n")
    for fileName in apiFileList:
        with open(adsppmSrcDir + fileName, 'rb') as apiFile:
            apis = []
            functionSummaryOutFile.write("\r\n" + fileName + ":\r\n    ")
            apis += [line[0:-1] for line in apiFile if  any(True for returntype in types if line.startswith(returntype)) and ")" in line[-3:]]
            allapis += apis
            functionSummaryOutFile.write("\r\n    ".join(curapi for curapi in apis))
            functionSummaryOutFile.write("\r\n")

functionSummaryOutFile.close()
