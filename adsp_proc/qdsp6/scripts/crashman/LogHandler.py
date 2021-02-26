import sys

handlerDictionary = {
}

ccps = 19200000                   # clock cycles per second: 19.2 Mhz for BFam

class MetaLogHandler(type):
    def __new__(cls, name, bases, attrs):
        if 'ext' not in attrs:
            raise "Must Provide an extension this class handles"
        return super(MetaLogHandler, cls).__new__(cls, name, bases, attrs)

    def __init__(self, name, bases, attrs):
        val = attrs['ext']

        # We already have a class registered for this type of logs
        # Replacing the that class with this one.
        if handlerDictionary.has_key(val) == True:
            print >> sys.stderr, "A class is already registered for this "\
                                 "type of logs. Replacing it with this one..."
        handlerDictionary[val] = self
        super(MetaLogHandler, self).__init__(name, bases, attrs) 


class LogHandler:
    '''Base class for any type of log handlers'''
    # members of this class
    # name : Name of the log file
    # handle: Handle to the file 'name'
    __metaclass__  = MetaLogHandler
    ext = 'ERROR'
    def __init__(self, fileName, timeUnit = 'sclk'):
        self.name = fileName
        self.conversionFactor = 1

        # Change 32768 to 19.2 MHz based clock. Use a global constant for
        # it so that we need change at only one place
        if timeUnit == 'sclk' or timeUnit == '':
            self.conversionFactor = 1
        elif timeUnit == 'sec':
            self.conversionFactor = ccps
        elif timeUnit == 'msec':
            self.conversionFactor = ccps/1000.0
        elif timeUnit == 'usec':
            self.conversionFactor = ccps/1000000.0
        else:
            print >> sys.stderr, "Time Unit: ", timeUnit
            raise "Unsupported time unit"


    def openLog(self, mode):
        openMode = mode.lower()
        if openMode == "read":
            self.handle = open(self.name, "r")
        elif openMode == "write":
            self.handle = open(self.name, "w")
        else:
            raise "Unknown file opening mode"

    def closeLog(self):
        self.handle.close()

    def setTimeWindow(self, startTime = 0L, endTime = 0xFFFFFFFFFFFFFFFFL ):
        raise "Not implemented in base class: setTimeWindow"

    def getNextLog(self):
        raise "Not implemented in base class: getNextLog"

    def getCurrentTimeStamp(self):
        raise "Not implemented in base class: getCurrentTimeStamp"

    def getCurrentMessage(self):
        raise "Not implemented in base class: getCurrentMessage"

    def getMessage(self, logMessage):
        raise "Not implemented in base class: getMessage"

    def getTimeStamp(self, logMessage):
        raise "Not implemented in base class: getTimeStamp"

    def updateCurrentLogMessage(self, newLogMessage):
        raise "Not implemented in base class: updateCurrentLogMessage"

    def writeLogMessage(self, timeStamp, logMessage):
        raise "Not implemented in base class: writeLogMessage"

    def createLogMessage(self, timeStamp, logMessage):
        raise "Not implemented in base class: createLogMessage"
        

class ULogHandler(LogHandler):
    '''Class for ULogs'''
    ext = 'ulog'
    def __init__(self, fileName, timeUnit = 'sclk'):
        LogHandler.__init__(self, fileName, timeUnit)
        self.sTime = 0L
        self.eTime = 0xFFFFFFFFFFFFFFFFL
        self.currTime = 0L
        self.currMsg = ""
        self.lineNum = 0

    def setTimeWindow(self, startTime = 0L, endTime = 0xFFFFFFFFFFFFFFFFL):
        if startTime > endTime or startTime < 0 or endTime < 0:
            print >> sys.stderr, "Error: Invalid time window"
            exit()

        self.sTime = startTime
        self.eTime = endTime

        while True:
            msg = self.handle.readline().strip(' \t')
            self.lineNum += 1

            # Skipping all blank lines
            while len(msg) == 1:
                msg = self.handle.readline().strip(' \t')
                self.lineNum += 1

            # reached EOF 
            if len(msg) == 0:
                return "";

            parts = msg.split(":",1)
            firstPart = parts[0]

            try:
                time = 0
		firstPart = firstPart.split(' ',1)[0]
                if self.conversionFactor == 1:
                    time = long(firstPart, 16)
                else:
                    time = long(float(firstPart)*self.conversionFactor)

                if time >= startTime:
                    if time <= endTime:
                        self.currTime = time
                        self.currMsg = parts[1]
                        return msg
                    else:
                        print >> sys.stderr, "Start time: ", time
                        return "";
            except:
                # Error occurred during parsing time stamp for the log message
                if self.lineNum != 1:
                    print >> sys.stderr, "Error while parsing time at line ", \
                                         self.lineNum,' in ', self.name
               



    def getNextLog(self):
        log = self.handle.readline().strip(' \t')
        self.lineNum += 1

        # Skipping through all blank lines 
        while len(log) == 1:
            log = self.handle.readline().strip(' \t')
            self.lineNum += 1

        if len(log) == 0:
            return "";

        parts = log.split(":", 1)
        try:
	    timePart = parts[0].split(' ',1)[0]
            if self.conversionFactor == 1:
                self.currTime = long(timePart, 16)
            else:
                self.currTime = long(float(timePart)*self.conversionFactor)
            self.currMsg = parts[1]
            if self.currTime > self.eTime:
                # We have reached past time window limit. Returning empty log
                log = ""
                self.currMsg = ""
                self.currTime = 0
        except:
            # Unable to figure out correct time stamp and/or log message
            print >> sys.stderr, parts[0].split(' ',1)[0]
            print >> sys.stderr, "MsgError %s at line %d: %s in %s" \
                     %(sys.exc_info()[0], self.lineNum, log, self.name)
            pass
        return log

    def getCurrentTimeStamp(self):
        return self.currTime

    def getCurrentMessage(self):
        return self.currMsg

    def getMessage(self, logMessage):
        return logMessage.split(":", 1)[1]

    def getTimeStamp(self, logMessage):
        stamp = logMessage.split(":", 1)[0]
        time = 0;
        try:
            if self.conversionFactor == 1:
                time = long(stamp, 16)
            else:
                time = long(float(stamp)*self.conversionFactor)
        except:
            print "Does not look like a ULog Message"
            time = 0
        return time

    def updateCurrentLogMessage(self, newLogMessage):
        self.currMsg = newLogMessage

    def writeLogMessage(self, timeStamp, logMessage): 
        # right now it just supports sclk as time unit. Eventually we will
        # add support for other type of units
        if self.conversionFactor == 1:
            # time stamp is of integer (or long) type i.e. sclk
            self.handle.write("0x%X:%s" %(timeStamp, logMessage))
        else:
            self.handle.write("%.8f:%s" %(timeStamp, logMessage))
        self.currTime = timeStamp
        self.currMsg = logMessage

    def createLogMessage(self, timeStamp, logMessage):
        returnString = ''
        if self.conversionFactor == 1:
            # time stamp is of integer (or long) type i.e. sclk
            returnString = "0x%X:%s" %(timeStamp, logMessage)
        else:
            returnString = "%.8f:%s" %(timeStamp, logMessage)

        return returnString



class HTMLLogWriter(LogHandler):
    ext = 'html'
    def __init__(self, fileName, timeUnit = 'sclk'):
        LogHandler.__init__(self, fileName, timeUnit)
        self.sTime = 0L
        self.eTime = 0xFFFFFFFFFFFFFFFFL
        self.currTime = 0L
        self.currMsg = ""
        self.lineNum = 0

    # removes html tags specifice for this file format
    def __filterHtmlTags(self, logMessage):
        retVal = logMessage.replace('<tr><td>','',1)
        retVal = retVal.replace('</td><td>', ': ', 1)
        retVal = retVal.replace('</td></tr>', '',1)
        return retVal

    def openLog(self, mode):
        self.openMode = mode.lower()
        if self.openMode == 'read':
            self.handle = open(self.name, 'r')
        elif self.openMode == 'write':
            self.handle = open(self.name, 'w')
        
            # writing starting HTML tags
            self.handle.write("<HTML><TITLE>" + self.name + "</TITLE><BODY>" )
            self.handle.write("<TABLE border=1 cellpadding=3>\n")
        else:
            print >> sys.stderr, "unsupported file opening mode"
            raise "unsupported file opening mode"

    def closeLog(self):
        if self.openMode == 'write':
            self.handle.write('\n</TABLE></BODY></HTML>\n')
        self.handle.close()

    def setTimeWindow(self, startTime = 0L, endTime = 0xFFFFFFFFFFFFFFFFL):
        if startTime > endTime or startTime < 0 or endTime < 0:
            print >> sys.stderr, "Error: Invalid time window"
            exit()

        self.sTime = startTime
        self.eTime = endTime

        while True:
            msg = self.handle.readline().strip(' \t')
            self.lineNum += 1

            # Skipping all blank lines
            while len(msg) == 1:
                msg = self.handle.readline().strip(' \t')
                self.lineNum += 1

            # reached EOF 
            if len(msg) == 0:
                return "";

            msg = self.__filterHtmlTags(msg)
            parts = msg.split(':',1)
            firstPart = parts[0]

            try:
                time = 0
                if self.conversionFactor == 1:
                    time = long(firstPart, 16)
                else:
                    time = long(float(firstPart)*self.conversionFactor)

                if time >= startTime:
                    if time <= endTime:
                        self.currTime = time
                        self.currMsg = parts[1]
                        return msg
                    else:
                        print >> sys.stderr, "Start time", time
                        return "";
            except:
                # Starting and ending html tag messages
                if msg.find('<HTML>') == -1 and msg.find('</HTML>') == -1:
                    print >> sys.stderr, "Cannot parse the time for ", self.name
                    raise "HTML Log error"

    def getNextLog(self):
        log = self.handle.readline().strip(' \t')
        self.lineNum += 1

        # Skipping through all blank lines 
        while len(log) == 1:
            log = self.handle.readline().strip(' \t')
            self.lineNum += 1

        if len(log) == 0:
            return "";

        log = self.__filterHtmlTags(log)
        parts = log.split(":", 1)
        try:
            if self.conversionFactor == 1:
                self.currTime = long(parts[0], 16)
            else:
                self.currTime = long(float(parts[0])*self.conversionFactor)
            self.currMsg = parts[1]
            if self.currTime > self.eTime:
                # We have reached past time window limit. Returning empty log
                log = ""
                self.currMsg = ""
                self.currTime = 0
        except:
            # Unable to figure out correct time stamp and/or log message and the
            # line read is not the first or the last line
            if log.find('<HTML>') == -1 and log.find('</HTML>') == -1:
                print >> sys.stderr, "MsgError at line %d: %s" \
                                      %(self.lineNum, log)
                raise "HTML Log error at %d" %self.lineNum
        return log

    def getCurrentTimeStamp(self):
        return self.currTime

    def getCurrentMessage(self):
        return self.currMsg

    def getMessage(self, logMessage):
        logMessage = self.__filterHtmlTags(logMessage)
        return logMessage.split(':', 1)[1]

    def getTimeStamp(self, logMessage):
        logMessage = self.__filterHtmlTags(logMessage)
        stamp = logMessage.split(':', 1)[0]
        time = 0;
        try:
            if self.conversionFactor == 1:
                time = long(stamp, 16)
            else:
                time = long(float(stamp)*self.conversionFactor)
        except:
            print "Does not look like a HTML LOG Message"
            time = 0
        return time

    def updateCurrentLogMessage(self, newLogMessage):
        self.currMsg = newLogMessage

    def writeLogMessage(self, timeStamp, logMessage):
        timeStr = ''
        if self.conversionFactor == 1:
            # time stamp is of integer (or long) type
            timeStr = '0x%X' %(timeStamp)
        else:
            timeStr = "%.8f" %(timeStamp)
        logMessage = logMessage.replace('\r\n','')
        self.handle.write('<tr><td>' + timeStr + '</td><td>' + logMessage + \
                          '</td></tr>\n')
        self.currTime = timeStamp
        self.currMsg = logMessage

    def createLogMessage(self, timeStamp, logMessage):
        timeStr = ''
        if self.conversionFactor == 1:
            # time stamp is of integer (or long) type
            timeStr = '0x%X' %(timeStamp)
        else:
            timeStr = "%.8f" %(timeStamp)
        logMessage = logMessage.replace('\r\n','')
        returnString = '<tr><td>' + timeStr + '</td><td>' + logMessage + \
                       '</td></tr>\n'

        return returnString

