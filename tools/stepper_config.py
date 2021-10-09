from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import QTimer
from graphics.stepper_config_interface import Ui_MainWindow

import sys
import time 
import serial
import serial.tools.list_ports
import re

#define OI_CORE          0
#define OI_DISCRETE      1
#define OI_DISCRETEVE    2
#define OI_STEPPER       3
#define OI_STEPPERVE     4

BOARD_LIST = {
    '0': 'OICore',
    '1': 'OIDiscrete',
    '2': 'OIDiscreteVertical',
    '3': 'OIStepper',
    '4': 'OIStepperVertical'
}

class OIStepperConfig(Ui_MainWindow):

    def setupAction(self):

        self.lock()
            
        ############################################################
        # portGroup parameters
        ############################################################
        self.serialPort = serial.Serial(baudrate=115200, timeout=0.1)
        self.serialPort.dtr = False
        self.serialPort.rts = False
        for port in serial.tools.list_ports.comports():
            self.portSelect.addItem(str(port))
        self.portSelect.currentTextChanged.connect(self.lock)
        self.connectButton.clicked.connect(self.connectPort)
        self.refreshButton.clicked.connect(self.refreshPort)
        self.OIStepper.clicked.connect(self.chooseBoard)
        self.OIStepperLP.clicked.connect(self.chooseBoard)
        self.noteBook.removeTab(1)
        self.noteBook.removeTab(1)
        self.noteBook.addTab(self.ConfigureMotor, "Configure Motor")
        self.noteBook.setCurrentIndex(0)

        ############################################################
        # motorGroup parameters
        ############################################################
        self.motor1.clicked.connect(self.checkConfigEnable)
        self.motor2.clicked.connect(self.checkConfigEnable)
        self.configEnable = True
        self.readStatus.clicked.connect(self.refreshStatus)

        ############################################################
        # Stop commands
        ############################################################
        self.hardhizButton.clicked.connect(self.hardHiz)
        self.hardstopButton.clicked.connect(self.hardStop)
        self.softhizButton.clicked.connect(self.softHiz)
        self.softstopButton.clicked.connect(self.softStop)

        ############################################################
        # Reset commands
        ############################################################
        self.resetdeviceButton.clicked.connect(self.resetDevice)
        self.resetposButton.clicked.connect(self.resetPos) 

        ############################################################
        # Contant speed commands
        ############################################################
        self.runButton.clicked.connect(self.run)
        self.gountilButton.clicked.connect(self.gountil)
        self.releaseswButton.clicked.connect(self.releasesw)
        
        ############################################################
        # Move commands
        ############################################################
        self.moveButton.clicked.connect(self.move)

        ############################################################
        # Absolute positionning commands
        ############################################################
        self.gotoButton.clicked.connect(self.goTo)
        self.gotodirButton.clicked.connect(self.goToDir)
        self.gohomeButton.clicked.connect(self.goHome)
        self.gomarkButton.clicked.connect(self.goMark)

        ############################################################
        # Config param commands
        ############################################################
        self.setButton.clicked.connect(self.setParam)
        self.getButton.clicked.connect(self.getParam)
        self.setButton_2.clicked.connect(self.setParam_2)
        self.getButton_2.clicked.connect(self.getParam_2)
        self.vmMode_2.clicked.connect(self.hideCurrent)
        self.cpMode_2.clicked.connect(self.hideVoltage)
        self.hideCurrent()

        ############################################################
        # Status auto refresh
        ############################################################
        self.timer = QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(lambda: self.refreshStatus(True))
        self.autoRead.clicked.connect(self.autoReadStatus)
        self.autoReadEnable = False

    def autoReadStatus(self):
        if (self.autoRead.isChecked()):
            self.autoReadEnable = True
            self.activateAutoRead()
        else:
            self.autoReadEnable = False
            self.desactivateAutoRead()

    def activateAutoRead(self):
        if (self.autoReadEnable):
            self.timer.start()
    
    def desactivateAutoRead(self):
        self.timer.stop()
    
    def chooseBoard(self):

        rememberIndex = False
        if (self.noteBook.currentIndex() == 1):
            rememberIndex = True
        if (self.OIStepper.isChecked()):
            self.noteBook.removeTab(1)
            self.noteBook.addTab(self.ConfigureMotor, "Configure Motor")
        else:
            self.noteBook.removeTab(1)
            self.noteBook.addTab(self.ConfigureMotorLP, "Configure Motor")
        if (rememberIndex):
            self.noteBook.setCurrentIndex(1)

    def hideVoltage(self):
        self.vpGroup_2.setHidden(True)
        self.vpGroup_3.setHidden(False)

    def hideCurrent(self):
        self.vpGroup_2.setHidden(False)
        self.vpGroup_3.setHidden(True)

    def performCommand(self, cmd, conf, data):
        try:
        #     # open port
        #     self.serialPort.open()

            # flush I/O
            self.serialPort.flushInput()
            self.serialPort.flushOutput()

            if(self.motor1.isChecked()):
                command = 'cmd %i %i %i\r\n' % (cmd, conf, data)
                self.serialPort.write(bytes(command, 'utf-8'))
                self.serialPort.readline()
                print("cmd " + str(cmd) + " " + str(conf) + " " + str(data))

            if(self.motor2.isChecked()):
                command = 'cmd %i %i %i\r\n' % (cmd, conf+1, data)
                self.serialPort.write(bytes(command, 'utf-8'))
                self.serialPort.readline()
                print("cmd " + str(cmd) + " " + str(conf+1) + " " + str(data))

        except:
            print("Cannot perform command: cmd " + str(cmd) + " " + str(conf) + " " + str(data))
            # close port
            self.serialPort.close()
            return -1
    
        # # close port
        # self.serialPort.close()
        return 0

    def performCommandWithReturn(self, cmd, conf, motor = True):
        
        returnData = -1

        if (motor == True and self.motor2.isChecked()):
            conf += 1

        try:
        #     # open port
        #     self.serialPort.open()

            # flush I/O
            self.serialPort.flushInput()
            self.serialPort.flushOutput()

            command = 'cmd %i %i\r\n' % (cmd, conf)
            self.serialPort.write(bytes(command, 'utf-8'))
            self.serialPort.readline() # echo
            data = self.serialPort.readline()
            while(data != b''):
                data = str(data, 'utf-8')
                match = re.search('\d+', data)
                if (match.group != None):
                    returnData = match.group()
                    break
                data = self.serialPort.readline()
            
            print("cmd " + str(cmd) + " " + str(conf))
            print("return " + str(returnData))

        except:
            print("Cannot perform command: cmd " + str(cmd) + " " + str(conf))
            self.serialPort.close()
            return returnData
    
        # # close port
        # self.serialPort.close()
        
        return int(returnData)

    def hardHiz(self):
        self.performCommand(8, 0, 0)

    def hardStop(self):
        self.performCommand(9, 0, 0)

    def softHiz(self):
        self.performCommand(16, 0, 0)

    def softStop(self):
        self.performCommand(17, 0, 0)

    def resetPos(self):
        self.performCommand(14, 0, 0)

    def resetDevice(self):
        self.performCommand(13, 0, 0)

    def run(self):

        speed = int((float(self.constantSpeed.value())*67.108864)+0.5)

        if (self.constantDirection.currentText() == 'FORWARD'):
            conf = 256
        else:
            conf = 0
        self.performCommand(15, conf, speed)
      
    def gountil(self):

        speed = int((float(self.constantSpeed.value())*67.108864)+0.5)

        if (self.constantDirection.currentText() == 'FORWARD'):
            conf = 256
        else:
            conf = 0

        if (self.constantAction.currentText() == 'RESET'):
            conf = conf + 0
        else:
            conf = conf + 2048

        self.performCommand(7, conf, speed)

    def releasesw(self):
        if (self.constantDirection.currentText() == 'FORWARD'):
            conf = 256
        else:
            conf = 0

        if (self.constantAction.currentText() == 'RESET'):
            conf = conf + 0
        else:
            conf = conf + 2048

        self.performCommand(12, conf, 0)

    def move(self):
        if (self.stepDirection.currentText() == 'FORWARD'):
            conf = 256
        else:
            conf = 0
        
        self.performCommand(10, conf, int(self.stepNumber.value()))

    def goTo(self):
        self.performCommand(5, 0, int(self.absPosition.value()))

    def goToDir(self):
        if (self.absDirection.currentText() == 'FORWARD'):
            conf = 256
        else:
            conf = 0
        self.performCommand(6, conf, int(self.absPosition.value()))

    def goHome(self):
        self.performCommand(3, 0, 0)

    def goMark(self):
        self.performCommand(4, 0, 0)
    
    def setParam(self):

        # if(not self.connectPort()):
        #     return

        self.lock()

        success = True
        self.getSetStatus.setPixmap(QtGui.QPixmap(""))
        self.getSetStatus.repaint()

        # ACC
        data = int((float(self.acc.value())*0.068719476736)+0.5)
        if (self.performCommand(27, 0x500, data & 0xFFF) == -1):
            success = False

        # DEC
        data = int((float(self.dec.value())*0.068719476736)+0.5)
        if (self.performCommand(27, 0x600, data & 0xFFF) == -1):
            success = False

        # MAX SPEED
        data = int((float(self.maxSpeed.value())*0.065536)+0.5)
        if (self.performCommand(27, 0x700, data & 0x3FF) == -1):
            success = False

        # MIN SPEED
        data = int((float(self.minSpeed.value())*4.194304)+0.5)
        if (self.lowSpeedOptimisation.isChecked()):
            data = (data & 0xFFF) | 0x1000
        if (self.performCommand(27, 0x800, data & 0x1FFF) == -1):
            success = False

        # FS SPEED
        data = int((float(self.fsSpeed.value())*0.065536))
        if (self.performCommand(27, 0x1500, data & 0x3FF) == -1):
            success = False

        # KVAL HOLD
        data = int((float(self.kvalHold.value())*2.56)+0.5)
        if (self.performCommand(27, 0x900, data & 0xFF) == -1):
            success = False

        # KVAL RUN
        data = int((float(self.kvalRun.value())*2.56)+0.5)
        if (self.performCommand(27, 0xA00, data & 0xFF) == -1):
            success = False

        # KVAL ACC
        data = int((float(self.kvalAcc.value())*2.56)+0.5)
        if (self.performCommand(27, 0xB00, data & 0xFF) == -1):
            success = False

        # KVAL DEC
        data = int((float(self.kvalDec.value())*2.56)+0.5)
        if (self.performCommand(27, 0xC00, data & 0xFF) == -1):
            success = False

        # INT SPEED
        data = int((float(self.intSpeed.value())*16.777216)+0.5)
        if (self.performCommand(27, 0xD00, data & 0x3FFF) == -1):
            success = False

        # START SLOPE
        data = int((float(self.startSlope.value())*637.5)+0.5)
        if (self.performCommand(27, 0xE00, data & 0xFF) == -1):
            success = False

        # FN SLOPE ACC
        data = int((float(self.accFinalSlope.value())*637.5)+0.5)
        if (self.performCommand(27, 0xF00, data & 0xFF) == -1):
            success = False

        # FN SLOPE DEC
        data = int((float(self.decFinalSlope.value())*637.5)+0.5)
        if (self.performCommand(27, 0x1000, data & 0xFF) == -1):
            success = False

        # K THERM
        data = int(((float(self.thermalCompFactor.value())-1)*32)+0.5)
        if (self.performCommand(27, 0x1100, data & 0xF) == -1):
            success = False

        # OCD THRESHOLD
        data = int(self.ocdThreshold.currentIndex())
        if (self.performCommand(27, 0x1300, data & 0xF) == -1):
            success = False

        # STALL THRESHOLD
        data = int(((float(self.stallthreshold.value())-31.25)*0.032)+0.5)
        if (self.performCommand(27, 0x1400, data & 0x7F) == -1):
            success = False

        # STEP MODE
        data = self.stepMode.currentIndex()
        if (self.syncSelection.currentIndex() != 0):
            data |= 0x80
            data |= ((self.syncSelection.currentIndex() - 1) << 4)
        if (self.performCommand(27, 0x1600, data & 0xFF) == -1):
            success = False
        
        # ALARM
        data =  int(self.alarmOCD.isChecked()) | \
                int(self.alarmThermalShutdown.isChecked()) << 1 | \
                int(self.alarmThermalWarning.isChecked()) << 2 | \
                int(self.alarmUVLO.isChecked()) << 3 | \
                int(self.alarmStallA.isChecked()) << 4 | \
                int(self.alarmStallB.isChecked()) << 5 | \
                int(self.alarmSW.isChecked()) << 6 | \
                int(self.alarmWrongCmd.isChecked()) << 7
        if (self.performCommand(27, 0x1700, data & 0xFF) == -1):
            success = False

        # CONFIG
        data =  self.pwmDiv.currentIndex() << 13 | \
                self.pwmMul.currentIndex() << 10 | \
                self.slewRate.currentIndex() << 8 | \
                int(self.ocdShutdown.isChecked()) << 7 | \
                int(self.vsCompEn.isChecked()) << 5 | \
                int(not(self.swHardStop.isChecked())) << 4
        if (self.oscSelection.currentIndex() != 0):
            data |= self.oscSelection.currentIndex()+3
        if (self.performCommand(27, 0x1800, data & 0xFFFF) == -1):
            success = False

        if (success):
            self.getSetStatus.setPixmap(QtGui.QPixmap(":/img/check.png"))
        else:
            self.getSetStatus.setPixmap(QtGui.QPixmap(":/img/error.png"))

        self.unlock()

    def setParam_2(self):

        # if (not self.connectPort()):
        #     return

        self.lock()

        success = True
        self.getSetStatus_2.setPixmap(QtGui.QPixmap(""))
        self.getSetStatus_2.repaint()

        # ACC
        data = int((float(self.acc_2.value())*0.068719476736)+0.5)
        if (self.performCommand(27, 0x500, data & 0xFFF) == -1):
            success = False

        # DEC
        data = int((float(self.dec_2.value())*0.068719476736)+0.5)
        if (self.performCommand(27, 0x600, data & 0xFFF) == -1):
            success = False

        # MAX SPEED
        data = int((float(self.maxSpeed_2.value())*0.065536)+0.5)
        if (self.performCommand(27, 0x700, data & 0x3FF) == -1):
            success = False

        # MIN SPEED
        data = int((float(self.minSpeed_2.value())*4.194304)+0.5)
        if (self.lowSpeedOptimisation_2.isChecked()):
            data = (data & 0xFFF) | 0x1000
        if (self.performCommand(27, 0x800, data & 0x1FFF) == -1):
            success = False

        # FS SPEED
        data = int((float(self.fsSpeed_2.value())*0.065536))
        if (self.boostMode_2.isChecked()):
            data = (data & 0x3FF) | 0x400
        if (self.performCommand(27, 0x1500, data & 0x7FF) == -1):
            success = False
        
        # STEP MODE
        data = self.stepMode_2.currentIndex()
        if (self.cpMode_2.isChecked()):
            data |= 0x08
        if (self.syncSelection_2.currentIndex() != 0):
            data |= 0x80
            data |= ((self.syncSelection_2.currentIndex() - 1) << 4)
        if (self.performCommand(27, 0x1600, data & 0xFF) == -1):
            success = False

        if (self.vmMode_2.isChecked()):

            # KVAL HOLD
            data = int((float(self.kvalHold_2.value())*2.56)+0.5)
            if (self.performCommand(27, 0x900, data & 0xFF) == -1):
                success = False

            # KVAL RUN
            data = int((float(self.kvalRun_2.value())*2.56)+0.5)
            if (self.performCommand(27, 0xA00, data & 0xFF) == -1):
                success = False

            # KVAL ACC
            data = int((float(self.kvalAcc_2.value())*2.56)+0.5)
            if (self.performCommand(27, 0xB00, data & 0xFF) == -1):
                success = False

            # KVAL DEC
            data = int((float(self.kvalDec_2.value())*2.56)+0.5)
            if (self.performCommand(27, 0xC00, data & 0xFF) == -1):
                success = False

            # INT SPEED
            data = int((float(self.intSpeed_2.value())*16.777216)+0.5)
            if (self.performCommand(27, 0xD00, data & 0x3FFF) == -1):
                success = False

            # START SLOPE
            data = int((float(self.startSlope_2.value())*637.5)+0.5)
            if (self.performCommand(27, 0xE00, data & 0xFF) == -1):
                success = False

            # FN SLOPE ACC
            data = int((float(self.accFinalSlope_2.value())*637.5)+0.5)
            if (self.performCommand(27, 0xF00, data & 0xFF) == -1):
                success = False

            # FN SLOPE DEC
            data = int((float(self.decFinalSlope_2.value())*637.5)+0.5)
            if (self.performCommand(27, 0x1000, data & 0xFF) == -1):
                success = False

            # K THERM
            data = int(((float(self.thermalCompFactor_2.value())-1)*32)+0.5)
            if (self.performCommand(27, 0x1100, data & 0xF) == -1):
                success = False

            # STALL THRESHOLD
            data = self.stallThreshold_2.currentIndex()
            if (self.performCommand(27, 0x1400, data & 0x1F) == -1):
                success = False

            # CONFIG
            data = self.pwmDiv_2.currentIndex() << 13 | \
                   self.pwmMul_2.currentIndex() << 10 | \
                   self.VCCVoltage_2.currentIndex() << 9 | \
                   self.UVLO_threshold_2.currentIndex() << 8 | \
                   int(self.ocdShutdown_2.isChecked()) << 7 | \
                   int(self.vsCompEn_2.isChecked()) << 5 | \
                   int(not(self.swHardStop_2.isChecked())) << 4
            if (self.oscSelection_2.currentIndex() != 0):
                data |= self.oscSelection_2.currentIndex()+3
            if (self.performCommand(27, 0x1A00, data & 0xFFFF) == -1):
                success = False
            
        elif self.cpMode_2.isChecked():

            # TVAL HOLD
            data = int(((float(self.tvalHold_2.value())-7.8125)*0.128)+0.5)
            if (self.performCommand(27, 0x0900, data & 0x7F) == -1):
                success = False

            # TVAL RUN
            data = int(((float(self.tvalRun_2.value())-7.8125)*0.128)+0.5)
            if (self.performCommand(27, 0x0A00, data & 0x7F) == -1):
                success = False

            # TVAL ACC
            data = int(((float(self.tvalAcc_2.value())-7.8125)*0.128)+0.5)
            if (self.performCommand(27, 0x0B00, data & 0x7F) == -1):
                success = False

            # TVAL DEC
            data = int(((float(self.tvalDec_2.value())-7.8125)*0.128)+0.5)
            if (self.performCommand(27, 0x0C00, data & 0x7F) == -1):
                success = False

            # T FAST
            data = (self.toffFast_2.currentIndex() << 4) | self.fastStep_2.currentIndex()
            if (self.performCommand(27, 0x0E00, data & 0xFF) == -1):
                success = False

            # T ON MIN
            data = int((float(self.tonmin_2.value())-0.5)*2.0+0.5)
            if (self.performCommand(27, 0x0F00, data & 0x7F) == -1):
                success = False

            # T OFF MIN
            data = int((float(self.toffmin_2.value())-0.5)*2.0+0.5)
            if (self.performCommand(27, 0x1000, data & 0x7F) == -1):
                success = False

            # CONFIG
            data =  self.vsCompEnCurrent_2.isChecked() << 15 | \
                    self.switchingPeriod_2.currentIndex() << 10 | \
                    self.VCCVoltage_3.currentIndex() << 9 | \
                    self.UVLO_threshold_3.currentIndex() << 8 | \
                    int(self.ocdShutdown_3.isChecked()) << 7 | \
                    int(self.vsCompEn_3.isChecked()) << 5 | \
                    int(not(self.swHardStop_3.isChecked())) << 4
            if (self.oscSelection_3.currentIndex() != 0):
                data |= self.oscSelection_3.currentIndex()+3
            if (self.performCommand(27, 0x1A00, data & 0xFFFF) == -1):
                success = False

        else:
            print("Error no mode selected")
            success = False

        # OCD THRESHOLD
        data = int(self.ocdThreshold_2.currentIndex())
        if (self.performCommand(27, 0x1300, data & 0x1F) == -1):
            success = False

        # ALARM
        data =  int(self.alarmOCD_2.isChecked()) | \
                int(self.alarmThermalShutdown_2.isChecked()) << 1 | \
                int(self.alarmThermalWarning_2.isChecked()) << 2 | \
                int(self.alarmUVLO_2.isChecked()) << 3 | \
                int(self.alarmADCUVLO_2.isChecked()) << 4 | \
                int(self.alarmStall_2.isChecked()) << 5 | \
                int(self.alarmSW_2.isChecked()) << 6 | \
                int(self.alarmCommandError_2.isChecked()) << 7
        if (self.performCommand(27, 0x1700, data & 0xFF) == -1):
            success = False
    
        # GATECFG1
        data = self.tcc_2.currentIndex()
        data |= self.igate_2.currentIndex() << 5
        data |= self.tboost_2.currentIndex() << 8
        data |= self.whatchDogEnable_2.isChecked() << 11

        if (self.performCommand(27, 0x1800, data & 0xFFF) == -1):
            success = False

        # GATECFG2
        data = self.tdead_2.currentIndex() | self.tblank_2.currentIndex() << 5
        if (self.performCommand(27, 0x1900, data & 0xFF) == -1):
            success = False

        if (success):
            self.getSetStatus_2.setPixmap(QtGui.QPixmap(":/img/check.png"))
        else:
            self.getSetStatus_2.setPixmap(QtGui.QPixmap(":/img/error.png"))

        self.unlock()

    def getParam(self):

        # if (not self.connectPort()):
        #     return

        self.lock()

        success = True
        self.getSetStatus.setPixmap(QtGui.QPixmap(""))
        self.getSetStatus.repaint()

        # ACC
        data = self.performCommandWithReturn(137, 0x500)
        if (data == -1):
            success = False
        self.acc.setValue(float(data & 0xFFF)*14.5519152283)

        # DEC
        data = self.performCommandWithReturn(137, 0x600)
        if (data == -1):
            success = False
        self.dec.setValue(float(data & 0xFFF)*14.5519152283)

        # MAX SPEED
        data = self.performCommandWithReturn(137, 0x700)
        if (data == -1):
            success = False
        self.maxSpeed.setValue(float(data & 0x3FF)*15.258789)

        # MIN SPEED
        data = self.performCommandWithReturn(137, 0x800)
        if (data == -1):
            success = False
        self.minSpeed.setValue(float(data & 0xFFF)*0.238418579)
        if(data & 0x1000):
            self.lowSpeedOptimisation.setChecked(True)
        else:
            self.lowSpeedOptimisation.setChecked(False)

        # FS SPEED
        data = self.performCommandWithReturn(137, 0x1500)
        if (data == -1):
            success = False
        self.fsSpeed.setValue((float(data & 0x3FF))*15.258789)

        # KVAL HOLD
        data = self.performCommandWithReturn(137, 0x900)
        if (data == -1):
            success = False
        self.kvalHold.setValue(float(data & 0xFF)*0.390625)
        
        # KVAL RUN
        data = self.performCommandWithReturn(137, 0xA00)
        if (data == -1):
            success = False
        self.kvalRun.setValue(float(data & 0xFF)*0.390625)
        
        # KVAL ACC
        data = self.performCommandWithReturn(137, 0xB00)
        if (data == -1):
            success = False
        self.kvalAcc.setValue(float(data & 0xFF)*0.390625)
        
        # KVAL DEC
        data = self.performCommandWithReturn(137, 0xC00)
        if (data == -1):
            success = False
        self.kvalDec.setValue(float(data & 0xFF)*0.390625)

        # INT SPEED
        data = self.performCommandWithReturn(137, 0xD00)
        if (data == -1):
            success = False
        self.intSpeed.setValue(float(data & 0x3FFF)*0.0596045)

        # START SLOPE
        data = self.performCommandWithReturn(137, 0xE00)
        if (data == -1):
            success = False
        self.startSlope.setValue(float(data & 0xFF)*0.00156862745098)

        # FN SLOPE ACC
        data = self.performCommandWithReturn(137, 0xF00)
        if (data == -1):
            success = False
        self.accFinalSlope.setValue(float(data & 0xFF)*0.00156862745098)

        # FN SLOPE DEC
        data = self.performCommandWithReturn(137, 0x1000)
        if (data == -1):
            success = False
        self.decFinalSlope.setValue(float(data & 0xFF)*0.00156862745098)

        # K THERM
        data = self.performCommandWithReturn(137, 0x1100)
        if (data == -1):
            success = False
        self.thermalCompFactor.setValue(float(data & 0xF)*0.03125+1)

        # OCD THRESHOLD
        data = self.performCommandWithReturn(137, 0x1300)
        if (data == -1):
            success = False
        self.ocdThreshold.setCurrentIndex(data & 0xF)

        # STALL THRESHOLD
        data = self.performCommandWithReturn(137, 0x1400)
        if (data == -1):
            success = False
        self.stallthreshold.setValue((float(data & 0x7F)+1)*31.25)

        # STEP MODE
        data = self.performCommandWithReturn(137, 0x1600)
        if (data == -1):
            success = False
        self.stepMode.setCurrentIndex(data & 0x7)
        if (data & 0x80  == 0):
            self.syncSelection.setCurrentIndex(0)
        else:
            self.syncSelection.setCurrentIndex(((data & 0x70) >> 4) + 1)
        
        # ALARM
        data = self.performCommandWithReturn(137, 0x1700)
        if (data == -1):
            success = False

        if (data & 0x01):
            self.alarmOCD.setChecked(True)
        else:
            self.alarmOCD.setChecked(False)

        if (data & 0x02):
            self.alarmThermalShutdown.setChecked(True)
        else:
            self.alarmThermalShutdown.setChecked(False)

        if (data & 0x04):
            self.alarmThermalWarning.setChecked(True)
        else:
            self.alarmThermalWarning.setChecked(False)
        
        if (data & 0x08):
            self.alarmUVLO.setChecked(True)
        else:
            self.alarmUVLO.setChecked(False)
        
        if (data & 0x10):
            self.alarmStallA.setChecked(True)
        else:
            self.alarmStallA.setChecked(False)
        
        if (data & 0x20):
            self.alarmStallB.setChecked(True)
        else:
            self.alarmStallB.setChecked(False)

        if (data & 0x40):
            self.alarmSW.setChecked(True)
        else:
            self.alarmSW.setChecked(False)

        if (data & 0x80):
            self.alarmWrongCmd.setChecked(True)
        else:
            self.alarmWrongCmd.setChecked(False)

        # CONFIG
        data = self.performCommandWithReturn(137, 0x1800)
        if (data == -1):
            success = False
        self.pwmDiv.setCurrentIndex((data & 0xE000) >> 13)
        self.pwmMul.setCurrentIndex((data & 0x1C00) >> 10)
        self.slewRate.setCurrentIndex((data & 0x0300) >> 8)
        
        if (data & 0x80 == 0x80):
            self.ocdShutdown.setChecked(True)
        else:
            self.ocdShutdown.setChecked(False)

        if (data & 0x20):
            self.vsCompEn.setChecked(True)
        else:
            self.vsCompEn.setChecked(False)

        if (data & 0x10):
            self.swHardStop.setChecked(False)
        else:
            self.swHardStop.setChecked(True)

        if ((data & 0xF) <= 3):
            self.oscSelection.setCurrentIndex(0)
        else:    
            self.oscSelection.setCurrentIndex((data & 0xF) - 3)

        if (success):
            self.getSetStatus.setPixmap(QtGui.QPixmap(":/img/check.png"))
        else:
            self.getSetStatus.setPixmap(QtGui.QPixmap(":/img/error.png"))

        self.unlock()

    def getParam_2(self):

        # if (not self.connectPort()):
        #     return

        self.lock()

        success = True
        self.getSetStatus_2.setPixmap(QtGui.QPixmap(""))
        self.getSetStatus_2.repaint()

        # ACC
        data = self.performCommandWithReturn(137, 0x500)
        if (data == -1):
            success = False
        self.acc_2.setValue(float(data & 0xFFF)*14.5519152283)

        # DEC
        data = self.performCommandWithReturn(137, 0x600)
        if (data == -1):
            success = False
        self.dec_2.setValue(float(data & 0xFFF)*14.5519152283)

        # MAX SPEED
        data = self.performCommandWithReturn(137, 0x700)
        if (data == -1):
            success = False
        self.maxSpeed_2.setValue(float(data & 0x3FF)*15.258789)

        # MIN SPEED
        data = self.performCommandWithReturn(137, 0x800)
        if (data == -1):
            success = False
        self.minSpeed_2.setValue(float(data & 0xFFF)*0.238418579)
        if(data & 0x1000):
            self.lowSpeedOptimisation_2.setChecked(True)
        else:
            self.lowSpeedOptimisation_2.setChecked(False)

        # FS SPEED
        data = self.performCommandWithReturn(137, 0x1500)
        if (data == -1):
            success = False
        self.fsSpeed_2.setValue((float(data & 0x3FF))*15.258789)
        if (data & 0x400):
            self.boostMode_2.setChecked(True)
        else:
            self.boostMode_2.setChecked(False)

        # STEP MODE
        data = self.performCommandWithReturn(137, 0x1600)
        if (data == -1):
            success = False
        if (data & 0x08):
            self.cpMode_2.setChecked(True)
            self.vpGroup_2.setHidden(True)
            self.vpGroup_3.setHidden(False)
        else:
            self.vmMode_2.setChecked(True)
            self.vpGroup_2.setHidden(False)
            self.vpGroup_3.setHidden(True)

        self.stepMode_2.setCurrentIndex(data & 0x7)

        if (data & 0x80  == 0):
            self.syncSelection.setCurrentIndex(0)
        else:
            self.syncSelection.setCurrentIndex(((data & 0x70) >> 4) + 1)

        if (self.vmMode_2.isChecked()):

            # KVAL HOLD
            data = self.performCommandWithReturn(137, 0x900)
            if (data == -1):
                success = False
            self.kvalHold_2.setValue(float(data & 0xFF)*0.390625)
            
            # KVAL RUN
            data = self.performCommandWithReturn(137, 0xA00)
            if (data == -1):
                success = False
            self.kvalRun_2.setValue(float(data & 0xFF)*0.390625)
            
            # KVAL ACC
            data = self.performCommandWithReturn(137, 0xB00)
            if (data == -1):
                success = False
            self.kvalAcc_2.setValue(float(data & 0xFF)*0.390625)
            
            # KVAL DEC
            data = self.performCommandWithReturn(137, 0xC00)
            if (data == -1):
                success = False
            self.kvalDec_2.setValue(float(data & 0xFF)*0.390625)

            # INT SPEED
            data = self.performCommandWithReturn(137, 0xD00)
            if (data == -1):
                success = False
            self.intSpeed_2.setValue(float(data & 0x3FFF)*0.0596045)

            # START SLOPE
            data = self.performCommandWithReturn(137, 0xE00)
            if (data == -1):
                success = False
            self.startSlope_2.setValue(float(data & 0xFF)*0.00156862745098)

            # FN SLOPE ACC
            data = self.performCommandWithReturn(137, 0xF00)
            if (data == -1):
                success = False
            self.accFinalSlope_2.setValue(float(data & 0xFF)*0.00156862745098)

            # FN SLOPE DEC
            data = self.performCommandWithReturn(137, 0x1000)
            if (data == -1):
                success = False
            self.decFinalSlope_2.setValue(float(data & 0xFF)*0.00156862745098)

            # K THERM
            data = self.performCommandWithReturn(137, 0x1100)
            if (data == -1):
                success = False
            self.thermalCompFactor_2.setValue(float(data & 0xF)*0.03125+1)

            # STALL THRESHOLD
            data = self.performCommandWithReturn(137, 0x1400)
            if (data == -1):
                success = False
            self.stallThreshold_2.setCurrentIndex(data & 0x1F)

            # CONFIG
            data = self.performCommandWithReturn(137, 0x1A00)
            if (data == -1):
                success = False
            self.pwmDiv_2.setCurrentIndex((data & 0xE000) >> 13)
            self.pwmMul_2.setCurrentIndex((data & 0x1C00) >> 10)
            
            if (data & 0x200):
                self.VCCVoltage_2.setCurrentIndex(1)
            else:
                self.VCCVoltage_2.setCurrentIndex(0)

            if (data & 0x100):
                self.UVLO_threshold_2.setCurrentIndex(1)
            else:
                self.UVLO_threshold_2.setCurrentIndex(0)

            if (data & 0x80):
                self.ocdShutdown_2.setChecked(True)
            else:
                self.ocdShutdown_2.setChecked(False)

            if (data & 0x20):
                self.vsCompEn_2.setChecked(True)
            else:
                self.vsCompEn_2.setChecked(False)

            if (data & 0x10):
                self.swHardStop_2.setChecked(False)
            else:
                self.swHardStop_2.setChecked(True)

            if ((data & 0xF) <= 3):
                self.oscSelection_2.setCurrentIndex(0)
            else:    
                self.oscSelection_2.setCurrentIndex((data & 0xF) - 3)
            
        elif self.cpMode_2.isChecked():

            # TVAL HOLD
            data = self.performCommandWithReturn(137, 0x0900)
            if (data == -1):
                success = False
            self.tvalHold_2.setValue(float((data & 0xFF)+1)*7.8)

            # TVAL RUN
            data = self.performCommandWithReturn(137, 0x0A00)
            if (data == -1):
                success = False
            self.tvalRun_2.setValue(float((data & 0xFF)+1)*7.8)

            # TVAL ACC
            data = self.performCommandWithReturn(137, 0x0B00)
            if (data == -1):
                success = False
            self.tvalAcc_2.setValue(float((data & 0xFF)+1)*7.8)

            # TVAL DEC
            data = self.performCommandWithReturn(137, 0x0C00)
            if (data == -1):
                success = False
            self.tvalDec_2.setValue(float(data & 0xFF)*7.8+7.8)

            # T FAST
            data = self.performCommandWithReturn(137, 0x0E00)
            if (data == -1):
                success = False
            self.fastStep_2.setCurrentIndex(data & 0x0F)
            self.toffFast_2.setCurrentIndex((data & 0xF0) >> 4)

            # T ON MIN
            data = self.performCommandWithReturn(137, 0x0F00)
            if (data == -1):
                success = False
            self.tonmin_2.setValue(float(data & 0x7F)*0.5+0.5)

            # T OFF MIN
            data = self.performCommandWithReturn(137, 0x1000)
            if (data == -1):
                success = False
            self.toffmin_2.setValue(float(data & 0x7F)*0.5+0.5)

            # CONFIG
            data = self.performCommandWithReturn(137, 0x1A00)
            if (data == -1):
                success = False
            
            if (data & 0x8000):
                self.vsCompEnCurrent_2.setChecked(True)
            else:
                self.vsCompEnCurrent_2.setChecked(False)

            self.switchingPeriod_2.setCurrentIndex(((data & 0x7C00) >> 10))

            if (data & 0x200):
                self.VCCVoltage_3.setCurrentIndex(1)
            else:
                self.VCCVoltage_3.setCurrentIndex(0)

            if (data & 0x100):
                self.UVLO_threshold_3.setCurrentIndex(1)
            else:
                self.UVLO_threshold_3.setCurrentIndex(0)

            if (data & 0x80):
                self.ocdShutdown_3.setChecked(True)
            else:
                self.ocdShutdown_3.setChecked(False)

            if (data & 0x20):
                self.vsCompEn_3.setChecked(True)
            else:
                self.vsCompEn_3.setChecked(False)

            if (data & 0x10):
                self.swHardStop_3.setChecked(False)
            else:
                self.swHardStop_3.setChecked(True)

            if ((data & 0xF) <= 3):
                self.oscSelection_3.setCurrentIndex(0)
            else:    
                self.oscSelection_3.setCurrentIndex((data & 0xF) - 3)

        else:
            print("Error no mode selected")
            success = False

        # OCD THRESHOLD
        data = self.performCommandWithReturn(137, 0x1300)
        if (data == -1):
            success = False
        self.ocdThreshold_2.setCurrentIndex(data & 0x1F)

        # ALARM
        data = self.performCommandWithReturn(137, 0x1700)
        if (data == -1):
            success = False

        if (data & 0x01):
            self.alarmOCD_2.setChecked(True)
        else:
            self.alarmOCD_2.setChecked(False)

        if (data & 0x02):
            self.alarmThermalShutdown_2.setChecked(True)
        else:
            self.alarmThermalShutdown_2.setChecked(False)

        if (data & 0x04):
            self.alarmThermalWarning_2.setChecked(True)
        else:
            self.alarmThermalWarning_2.setChecked(False)
        
        if (data & 0x08):
            self.alarmUVLO_2.setChecked(True)
        else:
            self.alarmUVLO_2.setChecked(False)
        
        if (data & 0x10):
            self.alarmADCUVLO_2.setChecked(True)
        else:
            self.alarmADCUVLO_2.setChecked(False)
        
        if (data & 0x20):
            self.alarmStall_2.setChecked(True)
        else:
            self.alarmStall_2.setChecked(False)

        if (data & 0x40):
            self.alarmSW_2.setChecked(True)
        else:
            self.alarmSW_2.setChecked(False)

        if (data & 0x80):
            self.alarmCommandError_2.setChecked(True)
        else:
            self.alarmCommandError_2.setChecked(False)
    
        # GATECFG1
        data = self.performCommandWithReturn(137, 0x1800)
        if (data == -1):
            success = False
        
        if (data & 0x800):
            self.whatchDogEnable_2.setChecked(True)
        else:
            self.whatchDogEnable_2.setChecked(False)

        self.tboost_2.setCurrentIndex((data & 0x700) >> 8)

        self.igate_2.setCurrentIndex((data & 0xE0) >> 5)

        self.tcc_2.setCurrentIndex(data & 0x1F)

        # GATECFG2
        data = self.performCommandWithReturn(137, 0x1900)
        if (data == -1):
            success = False

        self.tdead_2.setCurrentIndex((data & 0xE0) >> 5)

        self.tblank_2.setCurrentIndex(data & 0x1F)

        if (success):
            self.getSetStatus_2.setPixmap(QtGui.QPixmap(":/img/check.png"))
        else:
            self.getSetStatus_2.setPixmap(QtGui.QPixmap(":/img/error.png"))

        self.unlock()

    def refreshStatus(self, silent = False):

        # if (not silent):
        #     self.connectPort()

        if self.motor1.isChecked():

            if (not silent):
                data = self.performCommandWithReturn(130, 0, False)
            else:
                if (self.OIStepper.isChecked()):
                    data = self.performCommandWithReturn(137, 6912,False)
                else:
                    data = self.performCommandWithReturn(137, 6400,False)

            # Common for OIStepper and OIStepperLP
            if (data & 0x0001):
                self.HiZ1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            else:
                self.HiZ1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            if (data & 0x0002):
                self.BUSY1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            else:
                self.BUSY1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            if (data & 0x0004):
                self.SWStatus1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            else:
                self.SWStatus1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            if (data & 0x0008):
                self.SWEvent1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            else:
                self.SWEvent1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            
            # For OIStepperLP
            if (self.OIStepperLP.isChecked()):

                if (data & 0x0180):
                    self.WrongCmd1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                else:
                    self.WrongCmd1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                if (data & 0x0200):
                    self.UVLO1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.UVLO1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x0C00):
                    self.Thermal1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.Thermal1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x1000):
                    self.OCD1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.OCD1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x6000):
                    self.STALL1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.STALL1.setPixmap(QtGui.QPixmap(":/img/circle.png"))

            # For OIStepper
            else:

                if (data & 0x0080):
                    self.WrongCmd1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                else:
                    self.WrongCmd1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                if (data & 0x0200):
                    self.UVLO1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.UVLO1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x1800):
                    self.Thermal1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                else:
                    self.Thermal1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                if (data & 0x2000):
                    self.OCD1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.OCD1.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0xC000):
                    self.STALL1.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.STALL1.setPixmap(QtGui.QPixmap(":/img/circle.png"))

        if self.motor2.isChecked():

            if (not silent):
                data = self.performCommandWithReturn(130, 1, False)
            else:
                if (self.OIStepper.isChecked()):
                    data = self.performCommandWithReturn(137, 6913,False)
                else:
                    data = self.performCommandWithReturn(137, 6401,False)

            if (data & 0x0001):
                self.HiZ2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            else:
                self.HiZ2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            if (data & 0x0002):
                self.BUSY2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            else:
                self.BUSY2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            if (data & 0x0004):
                self.SWStatus2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            else:
                self.SWStatus2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            if (data & 0x0008):
                self.SWEvent2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
            else:
                self.SWEvent2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
            
            # For OIStepperLP
            if (self.OIStepperLP.isChecked()):
            
                if (data & 0x0180):
                    self.WrongCmd2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                else:
                    self.WrongCmd2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                if (data & 0x0200):
                    self.UVLO2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.UVLO2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x0C00):
                    self.Thermal2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.Thermal2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x1000):
                    self.OCD2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.OCD2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x6000):
                    self.STALL2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.STALL2.setPixmap(QtGui.QPixmap(":/img/circle.png"))

            # For OIStepper
            else:

                if (data & 0x0080):
                    self.WrongCmd2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                else:
                    self.WrongCmd2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                if (data & 0x0200):
                    self.UVLO2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.UVLO2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0x1800):
                    self.Thermal2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                else:
                    self.Thermal2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                if (data & 0x2000):
                    self.OCD2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.OCD2.setPixmap(QtGui.QPixmap(":/img/circle.png"))
                if (data & 0xC000):
                    self.STALL2.setPixmap(QtGui.QPixmap(":/img/empty.png"))
                else:
                    self.STALL2.setPixmap(QtGui.QPixmap(":/img/circle.png"))

    def checkConfigEnable(self):
        if ((self.motor1.isChecked() and self.motor2.isChecked()) or (not self.motor1.isChecked() and not self.motor2.isChecked())):  
            self.ConfigureMotor.setEnabled(False)
            self.ConfigureMotorLP.setEnabled(False)
            self.configEnable = False
        else:
            self.ConfigureMotor.setEnabled(True)
            self.ConfigureMotorLP.setEnabled(True)
            self.configEnable = True

    def unlock(self):
        self.motorGroup.setEnabled(True)
        self.SendCommands.setEnabled(True)
        if self.configEnable == True:
            self.ConfigureMotor.setEnabled(True)
            self.ConfigureMotorLP.setEnabled(True)

    def lock(self):
        self.motorGroup.setEnabled(False)
        self.SendCommands.setEnabled(False)
        self.ConfigureMotor.setEnabled(False)
        self.ConfigureMotorLP.setEnabled(False)

    def connectPort(self):
        self.lock()
        self.desactivateAutoRead()
        self.getSetStatus.setPixmap(QtGui.QPixmap(""))
        self.getSetStatus.repaint()
        self.getSetStatus_2.setPixmap(QtGui.QPixmap(""))
        self.getSetStatus_2.repaint()
        self.portStatus.setPixmap(QtGui.QPixmap(""))
        self.portStatus.repaint()

        l = list(serial.tools.list_ports.comports())
        for port in l:
            if (str(port) == self.portSelect.currentText()):
                self.serialPort.port = port.device
        
        board = ""
        result = ""
        connected = False

        try:
            # open port
            self.serialPort.open()
        
        except:
            print("Cannot open port: ", self.serialPort.port)
            self.portStatus.setPixmap(QtGui.QPixmap(":/img/error.png"))
            return

        try:
            # flush I/O
            self.serialPort.flushInput()
            self.serialPort.flushOutput()
            
            # set log level
            self.serialPort.write(b"log-level 0\r\n")
            self.serialPort.readline()
            self.serialPort.readline()

            # get type
            self.serialPort.write(b"cmd 190\r\n") # CMD_GET_TYPE
            self.serialPort.readline()
            data = self.serialPort.readline()
            self.serialPort.readline()
            data = str(data, 'utf-8')
            # get name of board from type
            print(BOARD_LIST[data[:-2]])

            # flush I/O
            self.serialPort.flushInput()
            self.serialPort.flushOutput()
            
            connected = True
        
        except:
            print("Reseting board")
            # self.serialPort.close()
            self.serialPort.rts = True  # Force an RTS reset on open
            # self.serialPort.open()
            time.sleep(0.005)  # Add a delay to meet the requirements of minimal EN low time (2ms for ESP32-C3)
            self.serialPort.rts = False
            self.serialPort.dtr = self.serialPort.dtr   # usbser.sys workaround
            timeout_start = time.time()
            while(self.serialPort.read() != b'>' and time.time() < timeout_start + 5):
                None

        if (connected == False):
            try:
                # set log level
                self.serialPort.write(b"log-level 0\r\n")
                self.serialPort.readline()

                # get type
                self.serialPort.write(b"cmd 190\r\n") # CMD_GET_TYPE
                self.serialPort.readline()
                self.serialPort.readline()
                board = self.serialPort.readline()
                data = str(board, 'utf-8')
                # get name of board from type
                print(BOARD_LIST[data[:-2]])

            except:
                print("Could not communicate with board on", self.serialPort.port)
                self.portStatus.setPixmap(QtGui.QPixmap(":/img/error.png"))
                self.serialPort.close()        
                return False

        # self.serialPort.close()

        if (BOARD_LIST[data[:-2]] == 'OIStepper' or BOARD_LIST[data[:-2]] ==  'OIStepperVertical'):
            self.portStatus.setPixmap(QtGui.QPixmap(":/img/check.png"))
            self.unlock()
            self.activateAutoRead()
            return True
        else:
            self.portStatus.setPixmap(QtGui.QPixmap(":/img/question.png"))
            return False     

    def refreshPort(self):
        self.serialPort.close()
        self.lock()
        self.desactivateAutoRead()
        self.getSetStatus.setPixmap(QtGui.QPixmap(""))
        self.getSetStatus_2.setPixmap(QtGui.QPixmap("")) 
        self.portStatus.setPixmap(QtGui.QPixmap(""))
        self.portSelect.clear()
        for port in serial.tools.list_ports.comports():
            self.portSelect.addItem(str(port))

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = OIStepperConfig()
    ui.setupUi(MainWindow)
    ui.setupAction()
    MainWindow.show()  
    sys.exit(app.exec_())