#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.

from Tkinter import *
import threading
import os

base = [str(x) for x in range(10)] + [ chr(x) for x in range(ord('A'),ord('A')+6)]

def bin2hex(strNum):
    #print strNum
    num = int(strNum, 2)
    tmp = []
    while True:
        if num == 0: break
        num,rem = divmod(num, 16)
        tmp.append(base[rem])

    return ''.join([str(x) for x in tmp[::-1]])

class CmdThread(threading.Thread):
    def __init__(self, cmd):
        threading.Thread.__init__(self)
        self.daemon = True
        self.cmd = cmd

    def run(self):
        os.system(self.cmd)

class Application(Frame):
    def updateCmd(self):
        aidingCmd = ''
        if (self.checkALL.get() == 1):
            aidingData = 'FFFF'
        else :
            li = [self.checkCELLDB_INFO.get(),self.checkRTI.get(),self.checkSADATA.get(),self.checkSVSTEER.get(), \
                self.checkSVDIR.get(),self.checkHEALTH.get(),self.checkUTC.get(),self.checkIONO.get(), \
                self.checkTIME.get(),self.checkPOSITION.get(),self.checkALMANAC.get(),self.checkEPHEMERIS.get()]
            aidingBin = ''.join([str(x) for x in li])
            aidingData = bin2hex(aidingBin)
        if (len(aidingData) != 0):
            aidingCmd = ' -d 0x' + aidingData

        modeCmd = ''
        if (self.mode.get() != 0):
            modeCmd = ' -m ' + str(self.mode.get())

        loopCmd = ''
        if (len(self.entryLoop.get()) != 0):
            loopCmd = ' -l ' + self.entryLoop.get()

        timeoutCmd = ''
        if (len(self.entryTimeout.get()) != 0):
            timeoutCmd = ' -t ' + self.entryTimeout.get()

        intervalCmd = ''
        if (len(self.entryInterval.get()) != 0):
            intervalCmd = ' -i ' + self.entryInterval.get()

        accuracyCmd = ''
        if (len(self.entryAccuracy.get()) != 0):
            accuracyCmd = ' -a ' + self.entryAccuracy.get()

        injectPosCmd = ''
        if (len(self.entryInjectPos.get()) != 0):
            injectPosCmd = ' -P ' + self.entryInjectPos.get()

        configCmd = ''
        if (len(self.entryConfig.get()) != 0):
            configCmd = ' -c ' + self.entryConfig.get()

        thCmd = ''
        if (len(self.entryTH.get()) != 0):
            thCmd = ' -o ' + self.entryTH.get()

        minSvCmd = ''
        if (len(self.entryMinSv.get()) != 0):
            minSvCmd = ' -A ' + self.entryMinSv.get()

        minSNRCmd = ''
        if (len(self.entryMinSNR.get()) != 0):
            minSNRCmd = ' -B ' + self.entryMinSNR.get()

        niCmd = ''
        if (len(self.entryNi.get()) != 0):
            niCmd = ' -k ' + self.entryNi.get()

        niSuplCmd = ''
        if (self.checkNiSupl.get() == 1):
            niSuplCmd = ' -N 1'

        nmeaCmd = ''
        if (self.checkNmea.get() == 1):
            nmeaCmd = ' -n 1'

        svInfoCmd = ''
        if (self.checkSvInfo.get() == 1):
            svInfoCmd = ' -y 1'

        trackCmd = ''
        if (self.checkTrack.get() == 1):
            trackCmd = ' -T'

        self.cmd.set('adb shell garden_app' + aidingCmd + modeCmd + loopCmd + \
            timeoutCmd + intervalCmd + accuracyCmd + injectPosCmd + configCmd + thCmd + \
            minSvCmd + minSNRCmd + niCmd + niSuplCmd + nmeaCmd + svInfoCmd + trackCmd)
        return True

    def runCmd(self):
        self.updateCmd()
        print self.cmd.get()
        cmdThread = CmdThread(self.cmd.get())
        cmdThread.start()

    def checkAll(self):
        if (self.checkALL.get() == 1):
            self.ckbEPHEMERIS.select()
            self.ckbALMANAC.select()
            self.ckbPOSITION.select()
            self.ckbTIME.select()
            self.ckbIONO.select()
            self.ckbUTC.select()
            self.ckbHEALTH.select()
            self.ckbSVDIR.select()
            self.ckbSVSTEER.select()
            self.ckbSADATA.select()
            self.ckbRTI.select()
            self.ckbCELLDB_INFO.select()
        else:
            self.ckbEPHEMERIS.deselect()
            self.ckbALMANAC.deselect()
            self.ckbPOSITION.deselect()
            self.ckbTIME.deselect()
            self.ckbIONO.deselect()
            self.ckbUTC.deselect()
            self.ckbHEALTH.deselect()
            self.ckbSVDIR.deselect()
            self.ckbSVSTEER.deselect()
            self.ckbSADATA.deselect()
            self.ckbRTI.deselect()
            self.ckbCELLDB_INFO.deselect()
        self.updateCmd()

    def checkOthers(self):
        if (self.checkEPHEMERIS.get() == 0 or self.checkALMANAC.get() == 0 or self.checkPOSITION.get() == 0 or self.checkTIME.get() == 0 or \
            self.checkIONO.get() == 0 or self.checkUTC.get() == 0 or self.checkHEALTH.get() == 0 or self.checkSVDIR.get() == 0 or \
            self.checkSVSTEER.get() == 0 or self.checkSADATA.get() == 0 or self.checkRTI.get() == 0 or self.checkCELLDB_INFO.get() == 0):
            self.ckbALL.deselect()
        self.updateCmd()

    def createWidgets(self):
        self.frmTop = Frame(self)
        self.frmTop.pack({"fill": "x"})


        self.BtnQuit = Button(self.frmTop, command = self.quit)
        self.BtnQuit["text"] = "Quit"
        self.BtnQuit.pack({"side": "right"})

        self.BtnRun = Button(self.frmTop, command = self.runCmd)
        self.BtnRun["text"] = "Run",
        self.BtnRun.pack({"side": "right"})

        self.entryCmd = Entry(self.frmTop, state = "readonly", textvariable = self.cmd)
        self.entryCmd.pack({"fill": "x"})


        self.lblDelete = Label(self)
        self.lblDelete["text"] = "Delete aiding data: Takes a hexadecimal mask as an argument as given in gps.h. Defaults: 0x0"
        self.lblDelete.pack({"side": "top"})

        self.frmDelete = Frame(self)
        self.frmDelete.pack({"fill": "x"})

        self.ckbALL = Checkbutton(self.frmDelete, variable = self.checkALL, command = self.checkAll);
        self.ckbALL["text"] = "ALL"
        self.ckbALL.pack({"side": "left"})

        self.ckbCELLDB_INFO = Checkbutton(self.frmDelete, variable = self.checkCELLDB_INFO, command = self.checkOthers);
        self.ckbCELLDB_INFO["text"] = "CELLDB_INFO "
        self.ckbCELLDB_INFO.pack({"side": "left"})

        self.ckbRTI = Checkbutton(self.frmDelete, variable = self.checkRTI, command = self.checkOthers);
        self.ckbRTI["text"] = "RTI "
        self.ckbRTI.pack({"side": "left"})

        self.ckbSADATA = Checkbutton(self.frmDelete, variable = self.checkSADATA, command = self.checkOthers);
        self.ckbSADATA["text"] = "SADATA "
        self.ckbSADATA.pack({"side": "left"})

        self.ckbSVSTEER = Checkbutton(self.frmDelete, variable = self.checkSVSTEER, command = self.checkOthers);
        self.ckbSVSTEER["text"] = "SVSTEER "
        self.ckbSVSTEER.pack({"side": "left"})

        self.ckbSVDIR = Checkbutton(self.frmDelete, variable = self.checkSVDIR, command = self.checkOthers);
        self.ckbSVDIR["text"] = "SVDIR "
        self.ckbSVDIR.pack({"side": "left"})

        self.ckbHEALTH = Checkbutton(self.frmDelete, variable = self.checkHEALTH, command = self.checkOthers);
        self.ckbHEALTH["text"] = "HEALTH "
        self.ckbHEALTH.pack({"side": "left"})

        self.ckbUTC = Checkbutton(self.frmDelete, variable = self.checkUTC, command = self.checkOthers);
        self.ckbUTC["text"] = "UTC "
        self.ckbUTC.pack({"side": "left"})

        self.ckbIONO = Checkbutton(self.frmDelete, variable = self.checkIONO, command = self.checkOthers);
        self.ckbIONO["text"] = "IONO "
        self.ckbIONO.pack({"side": "left"})

        self.ckbTIME = Checkbutton(self.frmDelete, variable = self.checkTIME, command = self.checkOthers);
        self.ckbTIME["text"] = "TIME "
        self.ckbTIME.pack({"side": "left"})

        self.ckbPOSITION = Checkbutton(self.frmDelete, variable = self.checkPOSITION, command = self.checkOthers);
        self.ckbPOSITION["text"] = "POSITION "
        self.ckbPOSITION.pack({"side": "left"})

        self.ckbALMANAC = Checkbutton(self.frmDelete, variable = self.checkALMANAC, command = self.checkOthers);
        self.ckbALMANAC["text"] = "ALMANAC "
        self.ckbALMANAC.pack({"side": "left"})

        self.ckbEPHEMERIS = Checkbutton(self.frmDelete, variable = self.checkEPHEMERIS, command = self.checkOthers);
        self.ckbEPHEMERIS["text"] = "EPHEMERIS "
        self.ckbEPHEMERIS.pack({"side": "left"})

        self.frmMode = Frame(self)
        self.frmMode.pack({"fill": "x"})
        self.mode = IntVar()
        self.cbtnSTANDALONE = Radiobutton(self.frmMode, text="POSITION_MODE_STANDALONE", variable=self.mode, value=0, command = self.updateCmd)
        self.cbtnSTANDALONE.pack({"side": "left"})
        self.cbtnMSB = Radiobutton(self.frmMode, text="POSITION_MODE_MS_BASED", variable=self.mode, value=1, command = self.updateCmd)
        self.cbtnMSB.pack({"side": "left"})
        self.cbtnMSA = Radiobutton(self.frmMode, text="POSITION_MODE_MS_ASSISTED", variable=self.mode, value=2, command = self.updateCmd)
        self.cbtnMSA.pack({"side": "left"})

        self.frmLoop = Frame(self)
        self.frmLoop.pack({"fill": "x"})
        self.entryLoop = Entry(self.frmLoop, validate='focusout', validatecommand = self.updateCmd)
        self.entryLoop.pack({"side": "left"})
        self.lblLoop = Label(self.frmLoop)
        self.lblLoop["text"] = "Number of sessions to loop through. Takes an argument. An argument of 0 means no sessions. Defaults:1"
        self.lblLoop.pack({"side": "left"})

        self.frmTimeout = Frame(self)
        self.frmTimeout.pack({"fill": "x"});
        self.entryTimeout = Entry(self.frmTimeout, validate='focusout', validatecommand = self.updateCmd)
        self.entryTimeout.pack({"side": "left"})
        self.lblTimeout = Label(self.frmTimeout)
        self.lblTimeout["text"] = "User defined length of time to issue stop navigation. Takes an argument. Time in seconds. Defaults: 60"
        self.lblTimeout.pack({"side": "left"})

        self.frmInterval = Frame(self)
        self.frmInterval.pack({"fill": "x"});
        self.entryInterval = Entry(self.frmInterval, validate='focusout', validatecommand = self.updateCmd)
        self.entryInterval.pack({"side": "left"})
        self.lblInterval = Label(self.frmInterval)
        self.lblInterval["text"] = "Interval. Takes an argument. Time in milliseconds between fixes. Defaults: 1000"
        self.lblInterval.pack({"side": "left"})

        self.frmAccuracy = Frame(self)
        self.frmAccuracy.pack({"fill": "x"});
        self.entryAccuracy = Entry(self.frmAccuracy, validate='focusout', validatecommand = self.updateCmd)
        self.entryAccuracy.pack({"side": "left"})
        self.lblAccuracy = Label(self.frmAccuracy)
        self.lblAccuracy["text"] = "Accuracy. Takes an argument. Accuracy in meters. Defaults: 0"
        self.lblAccuracy.pack({"side": "left"})

        self.frmInjectPos = Frame(self)
        self.frmInjectPos.pack({"fill": "x"});
        self.entryInjectPos = Entry(self.frmInjectPos, validate='focusout', validatecommand = self.updateCmd)
        self.entryInjectPos.pack({"side": "left"})
        self.lblInjectPos = Label(self.frmInjectPos)
        self.lblInjectPos["text"] = "Inject Position. Takes 3 arguments seperated by a COMMA. Latitude, Longitude, and accuracy. Defaults: 32.90285,-117.202185,0"
        self.lblInjectPos.pack({"side": "left"})

        self.frmConfig = Frame(self)
        self.frmConfig.pack({"fill": "x"})
        self.entryConfig = Entry(self.frmConfig, validate='focusout', validatecommand = self.updateCmd)
        self.entryConfig.pack({"side": "left"})
        self.lblConfig = Label(self.frmConfig)
        self.lblConfig["text"] = "gps.conf file. Takes an argument. The argument is the path to gps.conf file. Defaults: /etc/gps.conf"
        self.lblConfig.pack({"side": "left"})

        self.frmTH = Frame(self)
        self.frmTH.pack({"fill": "x"})
        self.entryTH = Entry(self.frmTH, validate='focusout', validatecommand = self.updateCmd)
        self.entryTH.pack({"side": "left"})
        self.lblTH = Label(self.frmTH)
        self.lblTH["text"] = "This option, when used, will enforce a check to determine if time to first fix is within a given threshold value. Takes one argument, the threshold value in seconds. Defaults: 20"
        self.lblTH.pack({"side": "left"})

        self.frmMinSv = Frame(self)
        self.frmMinSv.pack({"fill": "x"})
        self.entryMinSv = Entry(self.frmMinSv, validate='focusout', validatecommand = self.updateCmd)
        self.entryMinSv.pack({"side": "left"})
        self.lblMinSv = Label(self.frmMinSv)
        self.lblMinSv["text"] = "Minimum number of SVs seen in combination with 'Minimum SNR' option to determine when to stop the test without actually getting a position report to save test time"
        self.lblMinSv.pack({"side": "left"})

        self.frmMinSNR = Frame(self)
        self.frmMinSNR.pack({"fill": "x"})
        self.entryMinSNR = Entry(self.frmMinSNR, validate='focusout', validatecommand = self.updateCmd)
        self.entryMinSNR.pack({"side": "left"})
        self.lblMinSNR = Label(self.frmMinSNR)
        self.lblMinSNR["text"] = "Minimum SNR for each SV seen in 'Minimum number of SVs' option to determine when to stop the test  without actually getting a position report to save test time"
        self.lblMinSNR.pack({"side": "left"})

        self.frmNi = Frame(self)
        self.frmNi.pack({"fill": "x"})
        self.entryNi = Entry(self.frmNi, validate='focusout', validatecommand = self.updateCmd)
        self.entryNi.pack({"side": "left"})
        self.lblNi = Label(self.frmNi)
        self.lblNi["text"] = "Network Initiated. Takes 2 arguments separated by COMMA. First being the number of back to back NI tests and second being a COMMA separated pattern of  1:Accept, 2:Deny,or 3:No Response Defaults: 0:0"
        self.lblNi.pack({"side": "left"})

        self.frmNiSupl = Frame(self)
        self.frmNiSupl.pack({"fill": "x"})
        self.ckbNiSupl = Checkbutton(self.frmNiSupl, variable = self.checkNiSupl, command = self.updateCmd)
        self.ckbNiSupl["text"] = "NI SUPL. used in conjunction with 'Network Initiated' option to indicate that the test being conducted is an NI SUPL test. Takes no arguments."
        self.ckbNiSupl.pack({"side": "left"})

        self.frmNmea = Frame(self)
        self.frmNmea.pack({"fill": "x"})
        self.ckbNmea = Checkbutton(self.frmNmea, variable = self.checkNmea, command = self.updateCmd);
        self.ckbNmea["text"] = "NMEA. Use this option to print nmea string, timestamp and length. Takes no arguments. Defaults:0"
        self.ckbNmea.pack({"side": "left"})

        self.frmSvInfo = Frame(self)
        self.frmSvInfo.pack({"fill": "x"})
        self.ckbSvInfo = Checkbutton(self.frmSvInfo, variable = self.checkSvInfo, command = self.updateCmd)
        self.ckbSvInfo["text"] = "SV details. Use this option to print detailed info on satellites in view. Defaults:0"
        self.ckbSvInfo.pack({"side": "left"})

        self.frmTrack = Frame(self)
        self.frmTrack.pack({"fill": "x"})
        self.ckbTrack = Checkbutton(self.frmTrack, variable = self.checkTrack, command = self.updateCmd);
        self.ckbTrack["fg"]   = "red"
        self.ckbTrack["text"] = "Start a tracking session *WARNING* this tracking session will keep running until process is kllled. Ctrl-C to exit"
        self.ckbTrack.pack({"side": "left"})

    def __init__(self, master=None):
        self.checkALL = IntVar()
        self.checkEPHEMERIS = IntVar()
        self.checkALMANAC = IntVar()
        self.checkPOSITION = IntVar()
        self.checkTIME = IntVar()
        self.checkIONO = IntVar()
        self.checkUTC = IntVar()
        self.checkHEALTH = IntVar()
        self.checkSVDIR = IntVar()
        self.checkSVSTEER = IntVar()
        self.checkSADATA = IntVar()
        self.checkRTI = IntVar()
        self.checkCELLDB_INFO = IntVar()
        self.checkNiSupl = IntVar()
        self.checkNmea = IntVar()
        self.checkSvInfo = IntVar()
        self.checkTrack = IntVar()

        self.cmd = StringVar()
        self.cmd.set('adb shell garden_app')

        Frame.__init__(self, master)
        self.pack()
        self.createWidgets()

root = Tk()
root.title('Garden GUI')
app = Application(master=root)
app.mainloop()

#root.destroy()
