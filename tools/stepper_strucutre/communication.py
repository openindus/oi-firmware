from .configuration import *
import serial
import re

class SteperCommunication:

    def init_com(self):
        ############################################################
        # portGroup parameters
        ############################################################
        self.serialPort = serial.Serial(baudrate=115200, timeout=0.1)
        self.serialPort.dtr = False
        self.serialPort.rts = False
        for port in serial.tools.list_ports.comports():
            self.portSelect.addItem(str(port))

        self.motor1_selected = False
        self.motor2_selected = False

    # return a function which send data to stepper
    def send(self, command, **kwargs):
        if command == CommandFrame.GET_PARAM or command == CommandFrame.GET_STATUS:
            print("command is get param")
            return self._performCommandWithReturn(command(**kwargs))
        return lambda : self._performCommand(command(**kwargs))


    def _performCommand(self, cmd):
        try:
        #     # open port
        #     self.serialPort.open()

            # flush I/O
            self.serialPort.flushInput()
            self.serialPort.flushOutput()

            if(self.motor1_selected):
                command = 'ps01-device 0 %i \r\n' % (cmd)
                self.serialPort.write(bytes(command, 'utf-8'))
                self.serialPort.readline()
                print("ps01-device 0 " + str(cmd))

            if(self.motor2_selected):
                command = 'ps01-device 1 %i \r\n' % (cmd)
                self.serialPort.write(bytes(command, 'utf-8'))
                self.serialPort.readline()
                print("ps01-device 1 " + str(cmd))

        except:
            print("Cannot perform command: ps01-device n " + str(cmd))
            # close port
            self.serialPort.close()
            return -1
    
        # # close port
        # self.serialPort.close()
        return 0

    def _performCommandWithReturn(self, cmd, motor = True):
        
        returnData = -1

        # if (motor == True and self.motor2.isChecked()):
        #     conf += 1

        try:
        #     # open port
        #     self.serialPort.open()

            # flush I/O
            self.serialPort.flushInput()
            self.serialPort.flushOutput()

            if (motor == True and self.motor1_selected):
                command = 'ps01-device 0 %i\r\n' % (cmd)
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
                
                print("ps01-device 0 " + str(cmd))
                print("return " + str(returnData))

            if (motor == True and self.motor2_selected):
                command = 'ps01-device 1 %i %i\r\n' % (cmd)
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
                
                print("ps01-device 1 " + str(cmd))
                print("return " + str(returnData))

        except:
            print("Cannot perform command: ps01-device n " + str(cmd))
            self.serialPort.close()
            return returnData
    
        # # close port
        # self.serialPort.close()
        
        return int(returnData)
