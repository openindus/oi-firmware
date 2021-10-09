Import("env")

import tkinter as tk
from tkinter import ttk
try:
    from PIL import ImageTk, Image 
except:
    env.Execute("$PYTHONEXE -m pip install pillow")
    from PIL import ImageTk, Image 
import serial
import serial.tools.list_ports
import time
from threading import Thread
from os.path import abspath, join
import os 
import gc

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

BOARD_TYPE = [
    'OICore',
    'OIDiscrete',
    'OIDiscreteVertical',
    'OIStepper',
    'OIStepperVertical'
]

SCRIPTS_PATH = "components/oi-library/scripts/"

class UpdateBoard(ttk.Frame):

    def __init__(self, master, root, s, topclass):

        self.root = root
        self.serialPort = s
        self.topclass = topclass

        ttk.Frame.__init__(self, master)

        self.grid_columnconfigure((0,2), weight=1)

        # Image
        self.successImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/check.png"))
        self.errorImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/error.png"))

        # Message
        self.msgText = tk.StringVar()
        self.msg = tk.Label(self,  textvariable=self.msgText, justify = "center")
        self.msg.grid(row = 0, column = 1, sticky='ew', padx = 0, pady = (20,5))
        self.setMessage("Please, connect a board first")

        # Board type
        self.boardType = ttk.Combobox(self, width = 20, justify = "center", state=['disable'])
        self.boardType['values'] = BOARD_TYPE
        self.boardType.grid(column = 1, row = 1, padx = 10, pady = 10)
        self.boardType.currentState = 'disable'

        # Set button
        self.updateButton = ttk.Button(self, command = self.updateBoard, text = "Update Board", state=['disable']) 
        self.updateButton.grid(row = 2, column = 1, sticky='ew', padx = 0, pady = 10)
        self.updateButton.currentState = 'disable'

        # Progress bar widget 
        self.progress = ttk.Progressbar(self, orient = "horizontal", mode = 'indeterminate')
        self.check = tk.Label(self, image = self.successImg, justify = 'center') 

        self.boardType.bind("<<ComboboxSelected>>", self.typeModified)
    
    def typeModified(self, *args):
        self.setUpdateMessage(self.boardType.get())
        self.boardType.selection_clear()
        if self.boardType.get() in BOARD_TYPE:
            self.boardType.state(["!invalid"])


    def updateBoard(self):
        self.topclass.lockAll()
        self.check.grid_forget()
        print(self.boardType.get())
        if self.boardType.get() in BOARD_TYPE:
            self.progress.grid(row = 3, column = 1, sticky='ew', padx = 0, pady = (10,0))
            self.action = Thread(target=self.updateBoardCommand)
            self.action.daemon = True
            self.progress.start(3)
            self.action.start()
            self.check_updateBoardCommand_thread()
        else:
            self.boardType.state(["invalid"])


    def updateBoardCommand(self):
        try:       
            bootloader = abspath(join('bin', self.boardType.get().lower(), 'bootloader.bin'))
            firmware = abspath(join('bin', self.boardType.get().lower(), 'firmware.bin'))
            ota_data_initial = abspath(join('bin', self.boardType.get().lower(), 'ota_data_initial.bin'))
            partitions = abspath(join('bin', self.boardType.get().lower(), 'partitions.bin'))

            if (self.boardType.get() == 'OICore'):
                app_flash_esp32 = abspath(join('bin', 'app_flash_esp32.bin'))
                esptype = "esp32"
            else:
                app_flash_esp32 = abspath(join('bin', 'app_flash_esp32s2.bin'))
                esptype = "esp32s2"

            returnCode = env.Execute('"$PYTHONEXE" "$UPLOADER"          \
                            --chip %s                                   \
                            --baud 921600                               \
                            --port %s                                   \
                            --before default_reset                      \
                            --after hard_reset                          \
                            write_flash                                 \
                            -z                                          \
                            --flash_mode dio                            \
                            --flash_freq 40m                            \
                            --flash_size detect                         \
                            0x1000 %s                                   \
                            0x8000 %s                                   \
                            0xd000 %s                                   \
                            0x20000 %s                                  \
                            0x3C0000 %s'
            % (esptype, self.serialPort.port, bootloader, partitions, ota_data_initial, firmware, app_flash_esp32))

            if (returnCode != 0):
                raise ValueError('Error while flashing')

            self.check.configure(image = self.successImg)
        except:
            self.check.configure(image = self.errorImg)

    def check_updateBoardCommand_thread(self):
        if self.action.is_alive():
            self.root.after(20, self.check_updateBoardCommand_thread)
        else:
            self.topclass.connectPort()
            self.progress.stop()
            self.progress.grid_forget()
            self.check.grid(row = 3, column = 1, sticky='ew', padx = 0, pady = (5,0))

    def lock(self):
        self.updateButton.configure(state=['disable'])
        self.updateButton.currentState = 'disable'
        self.boardType.configure(state=['disable'])
        self.boardType.currentState = 'disable'

    def unlock(self):
        self.updateButton.configure(state=['!disable'])
        self.updateButton.currentState = '!disable'
        self.boardType.configure(state=['!disable'])
        self.boardType.currentState = '!disable'

    def lockAll(self):
        self.updateButton.configure(state=['disable'])
        self.boardType.configure(state=['disable'])

    def unlockAll(self):
        self.updateButton.configure(state=[self.updateButton.currentState])
        self.boardType.configure(state=[self.boardType.currentState])

    def setMessage(self, message):
        self.msgText.set(message)

    def setUpdateMessage(self, message):
        msg = "Update %s" % message
        self.updateButton.configure(text = msg)

    def setBoardType(self, boardType):
        self.boardType.set(boardType)
        self.setUpdateMessage(boardType)

class UpdateBus(ttk.Frame):

    def __init__(self, master, root, s, topclass):

        self.root = root
        self.serialPort = s
        self.topclass = topclass

        ttk.Frame.__init__(self, master)

        self.grid_columnconfigure((0,2), weight=1)

        # Image
        self.successImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/check.png"))
        self.errorImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/error.png"))

        # Message
        self.msgText = tk.StringVar()
        self.msg = tk.Label(self,  textvariable=self.msgText, justify = "center")
        self.msg.grid(row = 0, column = 1, sticky='ew', padx = 0, pady = (20,5))
        self.setMessage("Please, connect the computer to the OICore first")

        # Update bus button
        self.updateBusButton = ttk.Button(self, command = self.updateBus, text = "Update Bus", state=['disable']) 
        self.updateBusButton.grid(row = 1, column = 1, sticky='ew', padx = 0, pady = 10)
        self.updateBusButton.currentState = 'disable'    

        # Message
        self.msgTextProgress = tk.StringVar()
        self.msgProgress = tk.Label(self,  textvariable=self.msgTextProgress, justify = "center")
        self.msgProgress.grid(row = 2, column = 1, sticky='ew', padx = 0, pady = (20,5))
        self.setMessageProgress("")

        # Progress bar widget 
        self.progress = ttk.Progressbar(self, orient = "horizontal", mode = 'indeterminate')
        self.check = tk.Label(self, image = self.successImg, justify = 'center') 

    def updateBus(self):
        self.topclass.lockAll()
        self.check.grid_forget()
        self.progress.grid(row = 3, column = 1, sticky='ew', padx = 0, pady = (10,0))
        self.action = Thread(target=self.updateBusCommand)
        self.action.daemon = True
        self.progress.start(3)
        self.action.start()
        self.check_updateBusCommand_thread()

    def updateBusCommand(self):
        try:
            self.serialPort.open()

            # get all id with type        
            self.serialPort.write(b"get_all_types\r\n") # CMD_GET_TYPE_ALL
            self.serialPort.readline() # discard first line because of echo
            data = self.serialPort.readline()

            board_list = []

            while(data != b''):
                data = str(data, 'utf-8')
                match = tk.re.findall('\d+', data)
                if (len(match) == 2 and int(match[0]) < 254): # we found a board
                    board_list.append(match)
                data = self.serialPort.readline()
                
            if (len(board_list) == 0):
                print("No board found on the bus, please check connections")
                raise ValueError('')

            print("Boards found:", board_list)
            
            for board in board_list:
                board.append(-1)

            # Cycle through all boards to flash them
            for board in board_list:

                time.sleep(5)

                board[1] = BOARD_TYPE[int(board[1])]

                print("")
                print("")
                print("")
                print("----------------------------------------------------")
                print("            Flashing", board[1], "id:", board[0])
                print("----------------------------------------------------")
                print("")
                print("")
                print("")

                self.setMessageProgress("Flashing " + board[1] + " with ID " + board[0] + " (" + str(board_list.index(board)+1) + "/" + str(len(board_list)) + ")")
                
                # set all board to listen
                self.serialPort.write(b'cmd --bus=CAN --id=255 255 257\r\n')

                time.sleep(0.1)

                # set board to download mode
                cmd = "cmd --id=%s 125\r\n" % board[0]
                self.serialPort.write(bytes(cmd, 'utf-8'))

                time.sleep(0.2)

                # set others boards to silent
                self.serialPort.write(b'cmd --bus=CAN --id=255 255 256\r\n')

                time.sleep(0.2)

                # set core to download mode
                self.serialPort.write(b"cmd 125\r\n")
                            
                time.sleep(0.2)
                
                self.serialPort.dsrdtr = 1
                self.serialPort.dtr = self.serialPort.dtr   # usbser.sys workaround
                self.serialPort.close()

                #wait download mode to be started
                time.sleep(1)

                print("6")
                firmware = abspath(join("bin", board[1].lower(), "firmware.bin"))
        
                # use esptool.py to flash the module
                returnCode = env.Execute('"$PYTHONEXE" "$UPLOADER"          \
                                            --baud 921600                   \
                                            --no-stub                       \
                                            --before no_reset               \
                                            --port %s                       \
                                            write_flash                     \
                                            0x20000 %s'                 
                                            % (self.serialPort.port, firmware))
                board[2] = returnCode

                # resetting the OICore
                print("Reseting board")
                self.serialPort.dsrdtr = 0
                self.serialPort.rts = True  # Force an RTS reset on open
                self.serialPort.open()
                time.sleep(0.005)  # Add a delay to meet the requirements of minimal EN low time (2ms for ESP32-C3)
                self.serialPort.rts = False
                self.serialPort.dtr = self.serialPort.dtr   # usbser.sys workaround
                timeout_start = time.time()
                while(self.serialPort.read() != b'>' and time.time() < timeout_start + 5):
                    None

            time.sleep(5)
            
            # set all board to listen
            self.serialPort.write(b'cmd --bus=CAN --id=255 255 257\r\n')

            self.serialPort.close()
            
            self.check.configure(image = self.successImg)
            for board in board_list:
                if board[2] != 0:
                    self.check.configure(image = self.errorImg)
                    break

        except:
            self.check.configure(image = self.errorImg)
            self.serialPort.close()

        
        msgWindow = tk.Toplevel(self.root)
        msgWindow.title("Update bus status")
        if "nt" == os.name:
            msgWindow.wm_iconbitmap(SCRIPTS_PATH + "icon.ico")
        msgWindow.geometry("333x262")
        # Tree frame
        treeFrame = ttk.Frame(msgWindow)
        treeFrame.place(x=10, y=10)

        # Scrollbar
        treeScroll = ttk.Scrollbar(treeFrame)
        treeScroll.pack(side='right', fill='y')
        
        treeview = ttk.Treeview(treeFrame, selectmode="extended", yscrollcommand=treeScroll.set, columns=(0, 1, 2), height=8)
        treeview.pack()

        treeScroll.config(command=treeview.yview)

        treeview.column("#0", width=0)
        treeview.column(0, anchor='center', width=120)
        treeview.column(1, anchor='center', width=60)
        treeview.column(2, anchor='center', width=100)

        treeview.heading(0, text="Board", anchor='center')
        treeview.heading(1, text="ID", anchor='center')
        treeview.heading(2, text="Status", anchor='center')

        index = 1
        for board in board_list:
            if board[2] == 0:
                board[2] = "Success"
            else:
                board[2] = "Error (" + str(board[2]) + ")"
            treeview.insert(parent='', index='end', iid=index, values=(board[1], board[0], board[2]))
            index += 1

        buttonClose = ttk.Button(msgWindow, text = "Close", command = msgWindow.destroy)
        buttonClose.place(x=125, y=220)

    def check_updateBusCommand_thread(self):
        if self.action.is_alive():
            self.root.after(20, self.check_updateBusCommand_thread)
        else:
            self.topclass.connectPort()
            self.progress.stop()
            self.setMessageProgress("")
            self.progress.grid_forget()
            self.check.grid(row = 2, column = 1, sticky='ew', padx = 0, pady = (5,0))
            self.topclass.unlockAll()

    def lock(self):
        self.updateBusButton.configure(state=['disable'])
        self.updateBusButton.currentState = 'disable'

    def unlock(self):
        self.updateBusButton.configure(state=['!disable'])
        self.updateBusButton.currentState = '!disable'

    def lockAll(self):
        self.updateBusButton.configure(state=['disable'])

    def unlockAll(self):
        self.updateBusButton.configure(state=[self.updateBusButton.currentState])

    def setMessage(self, message):
        self.msgText.set(message)
    
    def setMessageProgress(self, message):
        self.msgTextProgress.set(message)

class SetID(ttk.Frame):

    def __init__(self, master, root, s, topclass):

        self.root = root
        self.serialPort = s
        self.topclass = topclass

        ttk.Frame.__init__(self, master)

        self.grid_columnconfigure((0,2), weight=1)
        
        # Image
        self.successImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/check.png"))
        self.errorImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/error.png"))

        # Message
        self.msgText = tk.StringVar()
        self.msg = tk.Label(self,  textvariable=self.msgText, justify = "center")
        self.msg.grid(row = 0, column = 1, sticky='ew', padx = 0, pady = (20,5))
        self.setMessage("Please, connect a board first")

        # ID
        self.idSelect = ttk.Spinbox(self, from_=0, to=253, increment=1, justify = "center", state=['disable'])
        self.idSelect.currentState = 'disable'
        self.resetIdValue()
        self.idSelect.bind('<FocusOut>', self.id_foc_out)
        self.idSelect.bind('<FocusIn>', self.id_foc_in)
        self.idSelect.grid(column = 1, row = 1, sticky='ew', padx = 0, pady = (10,10))

        # Set button
        self.idButton = ttk.Button(self, command = self.setId, text = "Set ID", state=['disable']) 
        self.idButton.grid(row = 2, column = 1, sticky='ew', padx = 0, pady = 10)
        self.idButton.currentState = 'disable'

        # Progress bar widget 
        self.progress = ttk.Progressbar(self, orient = "horizontal", mode = 'indeterminate')
        self.check = tk.Label(self, image = self.successImg, justify = 'center') 

    def id_foc_out(self, *args):
        try:
            value = int(self.idSelect.get())
        except:
            self.idSelect.delete('0', 'end')
            self.idSelect.insert(0, "Enter the ID")
            self.idSelect.state(["invalid"])
            self.idSelect.update()

    def id_foc_in(self, *args):
        self.check.grid_forget()
        if self.idSelect.get() == "Enter the ID":
            self.setIdValue("1")

    def setId(self):
        self.topclass.lockAll()
        self.check.grid_forget()
        print(self.idSelect.get())
        try:
            v = int(self.idSelect.get())
            if v >= 0 and v <= 253:
                self.progress.grid(row = 3, column = 1, sticky='ew', padx = 0, pady = (10,0))
                self.action = Thread(target=self.setIdCommand)
                self.action.daemon = True
                self.progress.start(3)
                self.action.start()
                self.check_setIdCommand_thread()
            else:
                self.idSelect.state(["invalid"])
        except:
            self.idSelect.state(["invalid"])

    def setIdCommand(self):
        try:
            # open port
            self.serialPort.open()

            # flush I/O
            self.serialPort.flushInput()
            self.serialPort.flushOutput()
            
            # set id
            cmd = 'set-id %s\r\n' % self.idSelect.get()
            self.serialPort.write(bytes(cmd, 'utf-8'))
            self.serialPort.readline()
            
            # read back id
            self.serialPort.write(b"get-id\r\n")
            self.serialPort.readline()
            self.serialPort.readline()
            result = self.serialPort.readline()
           

            expected = '%s\r\n' % self.idSelect.get() 
            
            if result != bytes(expected, 'utf-8'):
                self.check.configure(image = self.errorImg)
            else:
                self.check.configure(image = self.successImg)
        except:
            self.check.configure(image = self.errorImg)

         # close port
        self.serialPort.close()

    def check_setIdCommand_thread(self):
        if self.action.is_alive():
            self.root.after(20, self.check_setIdCommand_thread)
        else:
            self.progress.stop()
            self.progress.grid_forget()
            self.check.grid(row = 3, column = 1, sticky='ew', padx = 0, pady = (5,0))
            self.topclass.unlockAll()

    def lock(self):
        self.idButton.configure(state=['disable'])
        self.idButton.currentState = 'disable'
        self.idSelect.configure(state=['disable'])
        self.idSelect.currentState = 'disable'

    def unlock(self):
        self.idButton.configure(state=['!disable'])
        self.idButton.currentState = '!disable'
        self.idSelect.configure(state=['!disable'])
        self.idSelect.currentState = '!disable'
    
    def lockAll(self):
        self.idButton.configure(state=['disable'])
        self.idSelect.configure(state=['disable'])

    def unlockAll(self):
        self.idButton.configure(state=[self.idButton.currentState])        
        self.idSelect.configure(state=[self.idSelect.currentState])

    def setMessage(self, message):
        self.msgText.set(message)
    
    def setIdValue(self, message):
        self.idSelect.configure(state=['!disable'])
        self.idSelect.delete('0', 'end')
        self.idSelect.insert(0, message)
        self.idSelect.update()
        self.idSelect.configure(state=[self.idSelect.currentState])
    
    def resetIdValue(self):
        self.idSelect.configure(state=['!disable'])
        self.idSelect.delete('0', 'end')
        self.idSelect.insert(0, "Enter the ID")
        self.idSelect.update()
        self.idSelect.configure(state=[self.idSelect.currentState])

class OITools:
    
    # Creating tkinter window 
    def __init__(self, root):

        self.root = root

        root.title("OpenIndus Tools")
        root.resizable(width=False, height=False)
        if "nt" == os.name:
            root.wm_iconbitmap(bitmap = SCRIPTS_PATH + "icon.ico")

        # Create a style
        style = ttk.Style(root)

        # Import the tcl file
        root.tk.call('source', SCRIPTS_PATH + "custom-theme/azure.tcl")

        # Set the theme with the theme_use method
        style.theme_use('azure')

        root.grid_columnconfigure(0, weight=1)
        root.grid_rowconfigure(0, weight=1)
        
        mainframe = ttk.Frame(root, padding=20)
        mainframe.grid(column=0, row=0, sticky='nwes')

        # logo
        self.img = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/logo.png").resize((int(1249/6), int(531/6)), Image.ANTIALIAS))
        logo = tk.Label(mainframe, image = self.img)
        logo.grid(row = 0, column = 0, sticky='news')

        self.successImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/check.png"))
        self.errorImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/error.png"))
        self.voidImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/void.png"))
        self.questionImg = ImageTk.PhotoImage(Image.open(SCRIPTS_PATH + "graphics/question.png"))

        # PORT
        self.serialPort = serial.Serial(baudrate=115200, timeout=0.1)
        self.serialPort.dtr = False
        self.serialPort.rts = False

        portFrame = ttk.LabelFrame(mainframe, text='Select the port :')
        portFrame.grid(row = 1, column = 0, sticky='news', pady = 20)

        self.portchoosen = ttk.Combobox(portFrame, width = 30)
        self.portchoosen['values'] = list(serial.tools.list_ports.comports())
        self.portchoosen.grid(column = 0, row = 0, padx = 10, pady = 10)
        if (len(self.portchoosen['values']) >= 1):
            self.portchoosen.current(0)
        self.portchoosen.currentState = '!disable'
        self.connectResfreshButton = ttk.Button(portFrame, text = "Refresh",  command = self.refreshPort)
        self.connectResfreshButton.grid(column = 1, row = 0, padx = (10,5), pady = 10)
        self.connectResfreshButton.currentState = '!disable'
        self.connectPortButton = ttk.Button(portFrame, text = "Connect",  command = self.connectPort)
        self.connectPortButton.grid(column = 2, row = 0, padx = (5,5), pady = 10)
        self.connectPortButton.currentState = '!disable'
        self.checkPortStatus = ttk.Label(portFrame, image = self.voidImg)
        self.checkPortStatus.grid(column = 3, row = 0, padx = (5,10), pady = 8)
        self.portchoosen.bind("<<ComboboxSelected>>", self.portModified)

        # noteBook
        noteBook = ttk.Notebook(mainframe, padding = 0, height = 195)
        self.setId = SetID(noteBook, root, self.serialPort, self)
        self.updateBoard = UpdateBoard(noteBook, root, self.serialPort, self)
        self.updateBus = UpdateBus(noteBook, root, self.serialPort, self)
        noteBook.add(self.setId, text='Set Device ID')
        noteBook.add(self.updateBoard, text='Update Device')
        noteBook.add(self.updateBus, text='Update Device on Bus')
        noteBook.grid(column=0, row=2, sticky='nwes')

    def portModified(self, *args):
        self.checkPortStatus.configure(image = self.voidImg)
        self.setId.check.grid_forget()
        self.updateBoard.check.grid_forget()
        self.setId.setMessage("Please, connect a board first")
        self.updateBoard.setMessage("Please, connect a board first")
        self.setId.resetIdValue()
        self.setId.lock()
        self.updateBoard.lock()
        self.updateBus.lock()
        self.portchoosen.selection_clear()
    
    def lockAll(self):
        self.portchoosen.configure(state=['disable'])
        self.connectPortButton.configure(state=['disable'])
        self.connectResfreshButton.configure(state=['disable'])
        self.setId.lockAll()
        self.updateBus.lockAll()
        self.updateBoard.lockAll()

    def unlockAll(self):
        self.portchoosen.configure(state=[self.portchoosen.currentState])
        self.connectPortButton.configure(state=[self.connectPortButton.currentState])
        self.connectResfreshButton.configure(state=[self.connectResfreshButton.currentState])
        self.setId.unlockAll()
        self.updateBus.unlockAll()
        self.updateBoard.unlockAll()
        
    def connectPort(self):
        self.lockAll()
        self.checkPortStatus.configure(image = self.voidImg)
        self.setId.check.grid_forget()
        self.updateBoard.check.grid_forget()
        self.updateBus.check.grid_forget()

        self.action = Thread(target=self.connectPortCommand)
        self.action.daemon = True
        self.action.start()
        self.check_connectPortCommand_thread()

    def connectPortCommand(self):
        l = list(serial.tools.list_ports.comports())
        for port in l:
            if (str(port) == self.portchoosen.get()):
                self.serialPort.port = port.device
        
        board = ""
        result = ""
        connected = False

        try:
            # open port
            self.serialPort.open()
            self.updateBoard.unlock()
        
        except:
            print("Cannot open port: ", self.serialPort.port)
            self.checkPortStatus.configure(image = self.errorImg)
            self.unlockAll()
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
            
            # get id
            self.serialPort.write(b"get-id\r\n")
            self.serialPort.readline()
            result = self.serialPort.readline()
            self.serialPort.readline()
            result = str(result, 'utf-8')

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

                # get id
                self.serialPort.write(b"get-id\r\n")
                self.serialPort.readline()
                self.serialPort.readline()
                result = self.serialPort.readline()
                result = str(result, 'utf-8')

            except:
                print("Could not communicate with board on", self.serialPort.port)
                self.checkPortStatus.configure(image = self.questionImg)
                self.serialPort.close()        
                self.unlockAll()
                return


        self.setId.setMessage("Connected to: " + BOARD_LIST[data[:-2]])
        self.setId.setIdValue(result[:-2])
        self.setId.unlock()
        self.updateBoard.setMessage("Choose the board type")
        self.updateBoard.setBoardType(BOARD_LIST[data[:-2]])

        if (BOARD_LIST[data[:-2]] ==  'OICore'):
            self.updateBus.setMessage("Click on 'Update Bus' to update all board on the bus")
            self.updateBus.unlock()

        self.checkPortStatus.configure(image = self.successImg)

        self.serialPort.close()

    def check_connectPortCommand_thread(self):
        if self.action.is_alive():
            self.root.after(20, self.check_connectPortCommand_thread)
        else:
            self.unlockAll()
        
    def refreshPort(self):
        self.checkPortStatus.configure(image = self.voidImg)
        self.setId.check.grid_forget()
        self.updateBoard.check.grid_forget()
        self.updateBus.check.grid_forget()
        self.setId.resetIdValue()
        self.setId.lock()
        self.updateBoard.lock()
        self.updateBus.lock()
        self.setId.setMessage("Please, connect a board first")
        self.updateBoard.setMessage("Please, connect a board first")
        self.updateBus.setMessage("Please, connect the computer to the OICore first")

        current = self.portchoosen.get()
        self.portchoosen['values'] = list()
        self.portchoosen['values'] = list(serial.tools.list_ports.comports())
        if current in self.portchoosen['values']:
            None
        elif (len(self.portchoosen['values']) >= 1):
            self.portchoosen.current(0)
        else:
            self.portchoosen.set('')

def launch_oi_tools(*args, **kwargs):
    root = tk.Tk()
    OITools(root)
    root.mainloop()
    root.quit()
    gc.collect()

env.AddCustomTarget(
    name="oi-tools",
    dependencies=None,
    actions=[launch_oi_tools],
    title="OpenIndus Tools",
    description="Set board id, flash board on bus, ..."
)