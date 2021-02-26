#-----------------------------------------------------------------------------
#  LogUtils.py
#
#  Copyright (c) 2011 - 2013 Qualcomm Technologies, Inc. (QTI).
#  All Rights Reserved.
#  Qualcomm Technologies Confidential and Proprietary
#-----------------------------------------------------------------------------
import sys
import os
import re
from LogHandler import *
import LogUtilsHelp

# List of global variables. Not all them will be required by all options
operation = ""                          # Operation to be performed on logs

inputFilesList = []                     # list of input files
logHandlers = []                        # handle list for input files
outputFileName = ""                     # Name of the output file
outputHandle = None                     # Handle to the output file
currLogMsgs = []                        # list of current log messages from input files
currLogTimes = []                       # list of current log time stamps from input files
currLogIndex = -2                       # index of the log message with minimum time stamp.

srcFolder='.'                           # What folder to look into if the user wants to handle all log files.

searchTime = 0                          # Time at which we want to search all occurred events.
strToSearch = ""                        # sub string we are looking for in the logs
ipUnit = 'sclk'                         # time unit for the input log
opUnit = 'sclk'                         # time unit for the output log    
sortOption = "asc"                      # Ascending or descending sort

nextMessage = ""                        # Next log message to be operated on
timeStamp = 0L                          # Time stamp for the next message
lastTimeStamp = 0L                      # Last read time stamp. To be used for consistency checking
logFileName = ""                        # Name of the log file from where log selected

logMarkers = False                      # Prints start and end of the input log in output file
withSource = False                      # Should output log be printed with source file?
withCore = False                        # Should output log have (Core: 3) info?
withTimeDiff = False                    # For Profiling between two message (Not the most accurate method)
timeDiffBase = 'dec'                    # time difference in decimal or hexadecimal ?
startTime = 0                           # default start time for log window
endTime = 0xFFFFFFFFFFFFFFFFL           # default end time for log window
conversionFactor = 1                    # for converting time units

debugLogging = False                    # Log debug messages to a file
debugHandler = None                     # File handle for debugger.
errorCode = 0                           # Each operation should check this value once message
                                        # processing is done. Negative value indicates some
                                        # error
errorMessage = ""                       # Error Message if errorCode is negative.

operationsList = [ 'merge', 'event', 'time', 'window', \
                   'cluster', 'convert', 'sort', '--help', 'help' ]

# ----------------------------------------------------------------------#
# Utility function used for parsing the command line arguments 
# ----------------------------------------------------------------------#
def getFileExtension(fileName):
    parts = fileName.split('.')
    ext = parts[len(parts)-1]
    return ext

def appendSourceName():
    global nextMessage, logFileName
    srcName = " (file: " + logFileName + ")"
    nextMessage += srcName
        
def appendCoreIDMessage():
    global nextMessage, logFileName

    # Files of type name_c1.ulog are matched
    # example files: sleep_c2.ulog, debug_c1.txt, serverLog_c12.ulog
    isCore=re.compile('_c\d*\.[^.]*$').search(logFileName)
    
    if isCore != None:
        coreNum=logFileName[:]
        coreNum=re.sub('.*_c(\d*)\.[^.]*$', r'\1', coreNum)
        coreMsg=" (Core: "+coreNum+")"
        nextMessage = nextMessage + coreMsg


# returns true if a >= b else false
def isGEQ( num1, num2 ):
    if num1 >= num2:
        return True
    else:
        return False

# returns true if a <= b else false
def isLEQ( num1, num2 ):
    if num1 <= num2:
        return True
    else:
        return False

# fileHandle of type LogHandler
def isFileConsistent( fileHandle ):
    global sortOption
    comparisonFunc = None
    lineNum = 0

    fileHandle.openLog("read")
    fileHandle.setTimeWindow()

    prevTime = currTime = fileHandle.getCurrentTimeStamp()

    if sortOption == 'asc':
        comparisonFunc = isGEQ
    else:
        comparisonFunc = isLEQ

    while comparisonFunc( currTime, prevTime ):
        msg = fileHandle.getNextLog()
        if len(msg) != 0:
            prevTime = currTime
            currTime = fileHandle.getCurrentTimeStamp()
        else:
            fileHandle.closeLog()
            return True
    
    # If we reached here that means, somewhere in the file time stamps
    # are inconsistent.
    print >> sys.stderr, fileHandle.currMsg
    fileHandle.closeLog()
    return False


def getInputFiles():
    global inputFilesList

    # List of files in the current directory
    dirFileList = os.listdir(os.getcwd())
    numOfFiles = len(dirFileList)

    index = 1
    for aFile in dirFileList:
        print `index` + ") " + aFile
        index = index + 1

    inputStr = raw_input("\nEnter the index/ices of input file(s):")
    fileIndices = inputStr.split()

    for fileIndex in fileIndices:
        aFile = int(fileIndex) - 1
        if aFile < 0 or aFile >= numOfFiles:
            print "Invalid file index: ", aFile+1
            continue

        # Valid input. Appending corresponding file to input list
        inputFilesList.append(dirFileList[aFile])

    if len(inputFilesList) < 1:
        print "Invalid input"
        exit()

# Gives timing window for logs
# start: starting time
# end: end time
# unit: Unit of the time sclk(default), msec, usec, sec
def getTimeWindow(start, end, unit):
    global startTime, endTime, timeUnit
    global conversionFactor

    conversionFactor = 1

    if unit == "" or unit == "sclk":
        conversionFactor = 1
        timeUnit = 0
    elif unit == "sec":
        conversionFactor = ccps
        timeUnit = 1
    elif unit == "msec":
        conversionFactor = ccps/1000.0
        timeUnit = 2
    elif unit == "usec":
        conversionFactor = ccps/1000000.0
        timeUnit = 3
    else:
        print "Invalid time unit. Use --help"
        exit()

    if start != "":
        if conversionFactor == 1:           # unit is sclk
            startTime = long(start, 16)
        else:
            startTime = long(float(start) * conversionFactor)
    else:
        startTime = 0L

    if end != "":
        if conversionFactor == 1:
            endTime = long(end, 16)
        else:
            endTime = long(float(end) * conversionFactor)
    else:
        endTime = 0xFFFFFFFFFFFFFFFFL


def debugPrint(message,location=None):
    if location==None:
        print message
        debug(message)
    else:
        print >> location, message
        debug(message)
    
def debug(message):
    global debugLogging, debugHandler
    if debugLogging:
        debugHandler.write(message)
        debugHandler.write("\n")

def parseCommandLine(argList):
    global inputFilesList, startTime, endTime, unit, outputFileName, srcFolder
    global operation, conversionFactor
    global withSource, searchTime, strToSearch, withCore, logMarkers, withTimeDiff
    global ipUnit, opUnit, timeDiffBase
    global debugLogging, debugHandler
    # start and end time string
    unit = 'sclk'


    # clearing existing list of input files, if any
    del inputFilesList[:]

    for index in range(0, len(argList)):
        anArg = argList[index].lower()
        debug(anArg)
        if anArg == "ip":
            # Getting input file Name
            inputFilesList.append(argList[index+1])
            index = index + 1
        
        #type an all to get all files of a certain type in the directory
        elif anArg == "all":
            # Get all input file
            extension=argList[index+1]
            getAllFiles(extension)
            index=index+1

        elif anArg == "start":
            startTime = argList[index+1]
            index = index + 1

        elif anArg == "end":
            endTime = argList[index+1]
            index = index + 1

        elif anArg == "unit":
            unit = argList[index+1]
            index = index + 1

        elif anArg == "op":
            outputFileName = argList[index+1]
            index = index + 1

        elif anArg == "withsrc":
            withSource = True
        
        elif anArg == "withcore":
            withCore = True

        elif anArg == 'withlogmarker':
            logMarkers = True

        elif anArg == "from":
            ipUnit = argList[index+1]
            index = index + 1

        elif anArg == "to":
            opUnit = argList[index+1]
            index = index + 1

        elif anArg == "timediff":
            withTimeDiff = True
            timeDiffBase = argList[index+1]
            if timeDiffBase == str.lower('dec') or timeDiffBase == str.lower('hex'):
                index = index + 1
            else:
                print "Invalid value (%s) for 'timediff' option. Ignoring" %timeDiffBase
                withTimeDiff = False
                timeDiffBase = 'dec'

            
        #This allows a debugging tool, messages will be logged to a file
        elif anArg == "debug":
            debugLogging = True
            debugHandler = open("debug",'w')

        index = index + 1


# Gets all files of extension from the current directory.
def getAllFiles(extension):
    global inputFilesList, srcFolder
    for x in os.listdir(srcFolder):
        if str(x).find(extension) != -1 and inputFilesList.count(x) == 0:
            inputFilesList.append(x)
    debug(str(inputFilesList))
    

def getFileHandlers():
    global inputFilesList, logHandlers, outputHandle
    global outputFileName, ipUnit, opUnit

    # Getting input file handlers
    for aFileName in inputFilesList:
        debug(aFileName)
        ext = getFileExtension(aFileName)
        # checking if log handle is defined for this type
        if handlerDictionary.has_key(ext) == False:
            print >> sys.stderr, "Unknown Log format: %s, Skipping file %s" \
                                 %(ext, aFileName)
            continue

        handle = handlerDictionary[ext](aFileName, ipUnit)
        try:
            handle.openLog("read")
        except IOError as msg:
            debug(str(msg))
            continue
            
        msg = handle.setTimeWindow(startTime, endTime)
    
        # We care about this handle only if it returns valid message at beginning
        if len(msg) != 0:
            logHandlers.append(handle)
        else:
            print >> sys.stderr, "No message for given time window in ", aFileName

        debug("Finished with "+aFileName)


    # Getting the output file handler
    ext = getFileExtension(outputFileName)
    if handlerDictionary.has_key(ext) == False:
        print >> sys.stderr, "Output plugin for %s not found. Using ulog format" %(ext)
        outputFileName += ".ulog"
    outputHandle = handlerDictionary[ext](outputFileName, opUnit)
    outputHandle.openLog("write")

# This function should be called just before processing the first message for a 
# given log file. It assumes that logHandle is referring to already an open file
# with correct cursors set for input time window
def processStartOfLog( logHandle ):
    global logMarkers 

    # Prepanding a message indicating start of a log file
    if logMarkers == True:
        currLogMessage = logHandle.createLogMessage( logHandle.currTime, logHandle.currMsg )
        currLogMessage = ' <-------- Start of log file  %s --------->' %(logHandle.name) + \
                         '\n' + currLogMessage
        logHandle.updateCurrentLogMessage( currLogMessage )


# This function should be called when log dispatcher reached to the end of file
# for some particular log. This function will close the handle for that file
# along with some other postprocessing for that file.
def processEndOfLog( logHandle ):
    global timeStamp, nextMessage, logMarkers
    global withCore, withSource

    # Appending end of log message to current message
    if logMarkers == True:
        endOfLogMessage = logHandle.createLogMessage( timeStamp, \
                          ' <-------- End of log file %s --------->' %(logHandle.name) )
        nextMessage = nextMessage.rstrip()

        if withSource:
            appendSourceName()

        if withCore:
            appendCoreIDMessage()

        nextMessage = nextMessage + '\n' + endOfLogMessage + '\n'

    # Closing the log file handle
    logHandle.closeLog() 


# Returns the index in the list where an element should be inserted to keep
# the input list in sorted order. It assumes that input list is already sorted
# and if the new element is inserted at the index returned by this function, it
# should be still in sorted order.
def binary_sort_index( input_list, element ):
    end = len(input_list) - 1;
    if end == -1:
        print >> sys.stderr, "Empty list"
        return 0
  
    start = 0
  
    while (start + 1) < end:
        middle = (start + end) / 2;
        if element < input_list[middle]:
          end = middle - 1
        else:
          start = middle + 1
  
    if element < input_list[start]:
        return start
    elif element >= input_list[end]:
        return (end + 1)
    else:
        return (start + 1)

# This functions should be called after the log dispatcher loop ends i.e. that 
# function returns non zero value. The main job of this function is to check
# why actually log dispatcher loop ended by checking error codes. It will output
# an error message accordingly in the output file used during the operation. It
# will write that message with timestamp of zero resulting in inconsistent 
# output file and hence indicating a failure.
def dispatcherErrorCheck():
    global errorCode, errorMessage, outputHandle, timeStamp, operation

    if errorCode == 0:
        outputHandle.closeLog()
        if True == isFileConsistent(outputHandle):
            print >> sys.stdout, "\nOperation succeeded"
            return 0
        else:
            print >> sys.stdout, "\nOperation FAILED"
            return 2             # Use better error code scheme

    # We reach here means something looks wrong. Hopefully error code and error 
    # message will help. Also perform any resource release operations here.

    # Preparing an Error message that will be printed in both output file and
    # on std error device (terminal). We need to print in output file as this
    # utils are being used in some script which don't check for return values
    newErrorMsg = "Operation " + operation + " FAILED. " + errorMessage + \
                  " (ERROR CODE: " + str(errorCode) + ")"

    if errorCode == -1:
        print >> sys.stderr, newErrorMsg
        outputHandle.writeLogMessage(0, newErrorMsg)

    outputHandle.closeLog()
    return errorCode

# Routine that will extract the log message from each of the file handlers
# available in logHandlers. This function does not have any idea about
# how many messages have been read if any. 
#
# IT ASSUMES THAT CORRECT CURSORS IN THE FILE ARE PROVIDED.
def logMessageDispatcher():
    global logHandlers, timeStamp, nextMessage, logFileName, operation
    global currLogMsgs, currLogTimes, currLogIndex
    global errorCode, errorMessage, lastTimeStamp

    # fix me: Use some proper value to indicate first time instead of -2
    if currLogIndex == -2:
        # This is the first time this function has been called. We have to
        # prepare the lists. We assume that the file cursors are set at
        # appropriate location when this is called first time.
        for aHandle in logHandlers:
            processStartOfLog(aHandle)
            currLogTimes.append(aHandle.currTime)
            currLogMsgs.append(aHandle.currMsg)

    if len(logHandlers) != 0:
        currLogIndex = currLogTimes.index(min(currLogTimes))
        timeStamp = currLogTimes[currLogIndex]
        nextMessage = currLogMsgs[currLogIndex]

        #reading next log from the chosen file
        aHandle = logHandlers[currLogIndex]
        logFileName = aHandle.name
        msg = aHandle.getNextLog()

        # Checking for input validity 
        if timeStamp < lastTimeStamp and operation != 'sort':
            errorCode = -1;
            errorMessage = "Out of order input timestamps in file (Curr: 0x%lX) (Prev: 0x%lX) in " \
                           %(timeStamp, lastTimeStamp) + logFileName
            print errorMessage
            #return errorCode                # Temporary commented out

        elif withTimeDiff is True:
            nextMessage = nextMessage.replace('\r', '')
            nextMessage = nextMessage.replace('\n', '')
            if timeDiffBase == 'hex':
                nextMessage = nextMessage + ' (Time Diff: 0x%lX)\n' %(timeStamp -lastTimeStamp)
            else:
                nextMessage = nextMessage + ' (Time Diff: %ld)\n' %(timeStamp - lastTimeStamp)

        lastTimeStamp = timeStamp

        if len(msg) != 0:
            currLogTimes[currLogIndex] = aHandle.currTime
            currLogMsgs[currLogIndex] = aHandle.currMsg
        else:
            # reached to the end of file. close it and remove handle
            processEndOfLog(aHandle)
            currLogTimes.pop(currLogIndex)
            currLogMsgs.pop(currLogIndex)
            logHandlers.pop(currLogIndex)
        return 0            # indicating log message dispatch success
    else:
        return 1            # indicating log message dispatch failure


def logMessageDispatcherWithSourceOrCore():
    global nextMessage, withSource, withCore, logFileName
    retVal = logMessageDispatcher()
    nextMessage = nextMessage.rstrip()

    if withSource:
        appendSourceName()
        
    if withCore:
        appendCoreIDMessage()

    nextMessage = nextMessage + '\n'
    # we return retVal instead nextMessage so appending does not mess up with
    # detection of end of file
    return retVal


def printPrettyHelpMessage(opr, description):
    print >> sys.stdout, opr, " - "
    helpLines = description.splitlines()
    for aLine in helpLines:
        print >> sys.stdout, "\t", aLine


def printProcessingInput():
    global inputFilesList, outputFileName, startTime, endTime, operation
    global unit, ipUnit, opUnit, withSource, logMarkers
    
    print >> sys.stdout, "Processing with following data:"

    print >> sys.stdout, "\tInput files:"
    for aFile in inputFilesList:
        print >> sys.stdout, "\t\t", aFile

    print >> sys.stdout, "\tOutput file:\n\t\t", outputFileName
    print >> sys.stdout, "\tTime:\n\t\tStart: %d\n\t\tEnd: %d\n\t\tUnit :%s"\
             %(startTime, endTime, 'sclk')

    print >> sys.stdout, "\tInput file unit:\n\t\t", ipUnit
    print >> sys.stdout, "\tOutput file unit:\n\t\t", opUnit
    print >> sys.stdout, "\tWith Source?", withSource
    print >> sys.stdout, "\tWith Core?", withCore
    print >> sys.stdout, "\tWith Log Marker?", logMarkers



#------------------------------------------------------------------------------
#             Functions that will perform operations.
#------------------------------------------------------------------------------
def mergeLogs():
    global outputHandle, withSource, withCore
    global timeStamp, nextMessage

    while True:
        if withSource == True or withCore == True:
            if 0 != logMessageDispatcherWithSourceOrCore():
                break
        else:
            if 0 != logMessageDispatcher():
                break
        outputHandle.writeLogMessage(timeStamp, nextMessage)
        
    dispatcherErrorCheck()


def eventSearch():
    global outputHandle, withSource, withCore, strToSearch
    global timeStamp, nextMessage

    print "Searching for event (", strToSearch, ")"

    count = 0
    # eventually you may want to delegate the string search to log handlers
    # using logfile and corresponding loghandler global variable.
    while True:
        if withSource == True or withCore == True:
            if 0 != logMessageDispatcherWithSourceOrCore():
                break
        else:
            if 0 != logMessageDispatcher():
                break

        if nextMessage.find(strToSearch) != -1:
            outputHandle.writeLogMessage(timeStamp, nextMessage)
            count += 1

    if count != 0:
        print count, " matches were found for ", strToSearch
    else:
        print "No such events found"

    dispatcherErrorCheck()


def timeSearch():
    global timeStamp, nextMessage, searchTime
    global withSource, withCore, outputHandle

    print "Searching for events at  ", searchTime

    count = 0
    while True:
        if withSource == True or withCore == True:
            if 0 != logMessageDispatcherWithSourceOrCore():
                break
        else:
            if 0 != logMessageDispatcher():
                break

        if timeStamp == searchTime:
            outputHandle.writeLogMessage(timeStamp, nextMessage)
            count += 1

    if count != 0 :
        print count, " events happened at ", searchTime
    else:
        print "No events were found at given time"

    dispatcherErrorCheck()

def clusterSearch():
    global timeStamp, nextMessage, outputHandle, withSource, withCore

    prevTS = 0                              # previous time stamp
    prevMsg = ''                            # previous message
    shouldFlush = False                     # Indicates if prev msg needs to be 
                                            # written
    while True: 
        if withSource == True or withCore == True:
            if 0 != logMessageDispatcherWithSourceOrCore():
                break
        else:
            if 0 != logMessageDispatcher():
                break

        if timeStamp == prevTS:
            outputHandle.writeLogMessage(prevTS, prevMsg)
            shouldFlush = True
        else:
            if shouldFlush == True:
                outputHandle.writeLogMessage(prevTS, prevMsg)
            shouldFlush = False

        # updating previous timestamp and message
        prevTS = timeStamp
        prevMsg = nextMessage
    # end of while

    # for the last message
    if shouldFlush == True:
        outputHandle.writeLogMessage(prevTS, prevMsg)

    dispatcherErrorCheck()


def convertLogTime():
    global outputHandle, ipUnit, opUnit, inputFilesList
    global timeStamp, nextMessage
    cf = 1                              # local conversion factor for time unit

    if ipUnit == opUnit:
        print>>sys.stderr, 'Both units are same. Skipping...'
        exit()

    if len(inputFilesList) > 1:
        print >> sys.stderr, 'Only convert one file at a time. Skipping...'
        exit()

    if opUnit == 'sclk':
        cf = 1
    elif opUnit == 'sec':
        cf = ccps/1.0
    elif opUnit == 'msec':
        cf = ccps/1000.0
    elif opUnit == 'usec':
        cf = ccps/1000000.0
    else:
        print>>sys.stderr, "Unsupported time unit: ", opUnit

    while 0 == logMessageDispatcher():
        newTimeStamp = timeStamp / cf
        outputHandle.writeLogMessage(newTimeStamp, nextMessage)

    dispatcherErrorCheck()


# sorts the input log file. It should be noted that if input log needs
# to be sorted, something went wrong while logging messages.
def sortLogFile():
    global outputHandle, inputFilesList, timeStamp, nextMessage, sortOption
    prevTimeStamp = 0
    sortedTimeStampList = []
    messageList = []
    index = -1

    if len(inputFilesList) > 1:
        print >> sys.stderr, "Only sort one file at a time. Skipping..."
        exit()

    while 0 == logMessageDispatcher():
        if timeStamp >= prevTimeStamp:
            sortedTimeStampList.append(timeStamp)
            messageList.append(nextMessage)
            prevTimeStamp = timeStamp
        else:
            index = binary_sort_index( sortedTimeStampList, timeStamp )
            sortedTimeStampList.insert( index, timeStamp )
            messageList.insert( index, nextMessage )

    if sortOption == 'asc':
        for i in range(0, len(sortedTimeStampList)):
            outputHandle.writeLogMessage( sortedTimeStampList[i], messageList[i] )
    else:
        for i in reversed(range(0, len(sortedTimeStampList))):
            outputHandle.writeLogMessage( sortedTimeStampList[i], messageList[i] )

    dispatcherErrorCheck()


def printHelpMessage(argList):
    argLen = len(argList)
    if argLen > 1:
        print >> sys.stderr, "Usage: python <script_name> --help [command]"
        exit()

    if argLen == 0:
        # General Help. First printing available option and then supported
        # operations.
        print >> sys.stdout, "\nOPERATIONS (Use --help operation_name for more info):\n"
        operationsList = LogUtilsHelp.briefDescriptionDictionary.items()
        for anItem in operationsList:
            print >> sys.stdout, anItem[0].ljust(15), anItem[1]
    else:
        # Help for some particular command.
        helpCommand = argList[0].lower()
        if helpCommand != 'convert' and helpCommand != 'sort':
            print >> sys.stdout, LogUtilsHelp.optionsString, "\n"
        print >> sys.stdout,  "OPERATION:\n"
        printPrettyHelpMessage(helpCommand, \
                               LogUtilsHelp.detailDescriptionDictionary[helpCommand])

    print >> sys.stdout, "\n>>> Report bugs/issues at "
    print >> sys.stdout, "    nehalg@qualcomm.com or asw.idle_power@qualcomm.com"

#-------------------------------------------------------------------------------
#   Main Processing starts here i.e. entry point
#-------------------------------------------------------------------------------
def main():
    global withSource, logFileName, timeStamp, nextMessage
    global operation, ipUnit, opUnit, searchTime
    global inputFilesList, outputFileName
    global startTime, endTime, unit, strToSearch
    global operationsList, sortOption

    argLen = len(sys.argv)

    if argLen < 2:
        print "Invalid usage: Type UlogUtils --help for more info"
        exit()

    operation = sys.argv[1].lower()

    if operation not in operationsList:
        print "Operation not supported. Use --help for more info"
        exit()
    # Main processing starts here

    if operation == 'merge' or operation == 'window':
        if argLen < 2:
            print >> sys.stderr, "Input Error: Use --help operation for more info"
            raise "InputError"

        parseCommandLine(sys.argv[2:])
        getTimeWindow(str(startTime), str(endTime), unit)

         # no input file specified
        if len(inputFilesList) == 0:
            getInputFiles();

        # no output file specified
        if outputFileName == "":
            outputFileName =  raw_input("Output File Name:")

        printProcessingInput()
        getFileHandlers()
        mergeLogs()
    elif operation == 'time':
        if argLen < 3:
            print >> sys.stderr, "Input Error: Use --help time for more info"
            raise "InputError"

        # Processing input if any.
        parseCommandLine(sys.argv[3:])
        getTimeWindow('', '', unit)
        try:
            if conversionFactor == 1:
                searchTime = long(sys.argv[2], 16)
            else:
                searchTime = long(float(sys.argv[2])*conversionFactor)
        except:
            print >> sys.stderr, "Error in time input. Use --help"
            raise "InputError"

        # no input file specified
        if len(inputFilesList) == 0:
            getInputFiles();

        # no output file specified
        if outputFileName == "":
            outputFileName =  raw_input("Output File Name:")

        printProcessingInput()
        getFileHandlers()
        timeSearch()
    elif operation == 'event':
        if argLen < 3:
            print >> sys.stderr, "Input Error: Use --help event for more info"
            raise "InputError"

        # processing command line input if any.
        parseCommandLine(sys.argv[3:])
        getTimeWindow(str(startTime), str(endTime), unit)

        # no input file specified
        if len(inputFilesList) == 0:
            getInputFiles();

        # no output file specified
        if outputFileName == "":
            outputFileName =  raw_input("Output File Name:")

        strToSearch = sys.argv[2]
        printProcessingInput()
        getFileHandlers()
        eventSearch()
    elif operation == 'cluster':

        parseCommandLine(sys.argv[2:])
        getTimeWindow(str(startTime), str(endTime), unit)

        # no input file specified
        if len(inputFilesList) == 0:
            getInputFiles();

        # no output file specified
        if outputFileName == "":
            outputFileName =  raw_input("Output File Name:")

        printProcessingInput()
        getFileHandlers()
        clusterSearch()
    elif operation == 'convert':
        if argLen != 6:
            print >> sys.stderr, "Input Error: Use --help convert for more info"
            raise "InputError"

        # collecting input
        del inputFilesList[:]
        inputFilesList.append(sys.argv[2])
        ipUnit = sys.argv[3]
        opUnit = sys.argv[4]
        outputFileName = sys.argv[5]

        # Processing input and setting file handles
        getTimeWindow('', '', ipUnit)
        printProcessingInput()
        getFileHandlers()
        convertLogTime()
    elif operation == 'sort':
        if argLen != 4 and argLen != 5:
            print >> sys.stderr, "Input Error: Use --help sort for more info"
            raise "InputError"

        #collecting input files (must not one)
        del inputFilesList[:]
        inputFilesList.append(sys.argv[2])
        outputFileName = sys.argv[3]

        if argLen == 5:
            if sys.argv[4].lower() == 'dsc':
                sortOption = 'dsc'
            else:
                sortOption = 'asc'

        getFileHandlers()
        sortLogFile()

    elif operation == '--help' or operation == 'help':
        if argLen > 3:
            print >> sys.stderr, "Input Error: Use --help for help"
            raise "InputError"

        printHelpMessage(sys.argv[2:])
    else:
        print >> sys.stderr, "Unknown Log operation. Use --help for more info"
        exit()


if __name__ == "__main__":
    main()
