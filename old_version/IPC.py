# This file contains the code for the IPC window in the 42 gui

# Import Packages---------------------------------------------------------------
import sys
import numpy as numpy
import re
# Import QApplication and the required widgets from PyQt5.QtWidgets
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QWidget
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QGridLayout # to arrange the buttons
from PyQt5.QtWidgets import QLineEdit # for the display
from PyQt5.QtWidgets import QPushButton # for the butons
from PyQt5.QtWidgets import QComboBox # drop down box
from PyQt5.QtWidgets import QVBoxLayout # verticle Stack layout
from PyQt5.QtWidgets import QHBoxLayout # Horizontal Stack layout
from PyQt5.QtWidgets import QRadioButton
from PyQt5.QtWidgets import QListWidget
from PyQt5.QtWidgets import QStackedWidget

class IPC_Window(QWidget):
    def __init__(IPC_Window):
        super().__init__()
        IPC_Window.setWindowTitle('InterProcess Communications') # sets the window’s title
        IPC_Window.setGeometry(0, 0, 200, 400) # Set window, can resize manually (xpos on monitor, ypos on monitor, width pixels, height pixles)
        IPC_Layout = QGridLayout()

        Label = [0]*50 # Initialize array of labels
        IPC_Window.IpcDataOut = [""]*300 # Initialize List
        IPC_Window.GetDataIn('InOut/Inp_IPC.txt')

        # Heading Label---------------------------------------------------------
        Label[0] = QLabel("InterProcess Communications")
        Label[0].setAlignment(Qt.AlignCenter)
        Label[0].setStyleSheet('font-size: 20px')
        IPC_Layout.addWidget(Label[0], 0, 0, 1, 3)

        # List Label------------------------------------------------------------
        Label[1] = QLabel("IPC List")
        Label[1].setAlignment(Qt.AlignCenter)
        Label[1].setStyleSheet('font-size: 15px')
        IPC_Layout.addWidget(Label[1], 1, 0)

        # List of IPC----------------------------------------------
        IPC_Window.ListIPC = QListWidget()
        for i in range(IPC_Window.NumSocketsDataIn):
            ItemLabel = i + 1
            IPC_Window.ListIPC.insertItem(i, "IPC %s"%ItemLabel)
        IPC_Layout.addWidget(IPC_Window.ListIPC, 2, 0, 7, 1)
        IPC_Window.ListIPC.currentRowChanged.connect(IPC_Window.DisplayStackSlot)

        # Stacked Widget--------------------------------------------------------
        IPC_Window.Stack = QStackedWidget()
        for i in range(IPC_Window.NumSocketsDataIn):
            IPC_Window.AddStack(i)
            IPC_Window.SetGuiFields(i)
        IPC_Layout.addWidget(IPC_Window.Stack,1,1,11,2)

        # Add IPC Button--------------------------------------------------------
        AddIPCbtn = QPushButton('Add an IPC')
        IPC_Layout.addWidget(AddIPCbtn, 9, 0)
        AddIPCbtn.clicked.connect(IPC_Window.AddIPC)

        # Remove IPC btn--------------------------------------------------------
        RmvIPCbtn = QPushButton('Remove an IPC')
        IPC_Layout.addWidget(RmvIPCbtn, 10, 0)
        RmvIPCbtn.clicked.connect(IPC_Window.RmvIPC)

#-------------------------------------------------------------------------------
# Apply / Cancel / Reset Default Button-----------------------------------------
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(IPC_Window.close)
        ApplyBtn.clicked.connect(IPC_Window.WriteDataOut)
        ApplyBtn.clicked.connect(IPC_Window.WriteFileOut) # Last Slot
        IPC_Layout.addWidget(ApplyBtn, 13, 2)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(IPC_Window.close)
        IPC_Layout.addWidget(CancelBtn, 13, 1)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(IPC_Window.SetDefaultGuiFields)
        IPC_Layout.addWidget(ResetBtn, 13, 0)

#-------------------------------------------------------------------------------
# Finialize IPC window-------------------------------------------------------
        IPC_Window.setLayout(IPC_Layout)
#-------------------------------------------------------------------------------
# IPC Stack--------------------------------------------------------------
    def AddStack(IPC_Window, i): # Adds a new page to the IPC Stack with the same layout as previous pages
        Label = [0]*10 # Initialize array of labels
        IPC_Window.Page = QWidget() # Create widget to become a Page in Stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout() # for the radio buttons
        SubLayout2 = QHBoxLayout() # for the radio buttons

        # IPC Mode-------------------------------------------------------------
        Label[0] = QLabel("IPC Mode:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[0], 0, 0)

        IPC_Window.Page.Mode = QComboBox()
        IPC_Window.Page.Mode.addItems(["OFF","TX","RX","TXRX","ACS","WRITEFILE","READFILE"])
        StackLayout.addWidget(IPC_Window.Page.Mode, 0, 1)

        # AC.ID-----------------------------------------------------------------
        Label[1] = QLabel("AC.ID for ACS mode:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 1, 0)

        IPC_Window.Page.AcId = QLineEdit("0")
        StackLayout.addWidget(IPC_Window.Page.AcId, 1, 1)

        # File Name-------------------------------------------------------------
        Label[2] = QLabel("File name for WRITE or READ:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[2], 2, 0)

        IPC_Window.Page.FileName = QLineEdit("State00.42")
        StackLayout.addWidget(IPC_Window.Page.FileName, 2, 1)

        # Socket Role-----------------------------------------------------------
        Label[3] = QLabel("Socket Role:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[3], 3, 0)

        IPC_Window.Page.SocketRole = QComboBox()
        IPC_Window.Page.SocketRole.addItems(["SERVER","CLIENT","GMSEC_CLIENT"])
        StackLayout.addWidget(IPC_Window.Page.SocketRole, 3, 1)

        # Server Host Name------------------------------------------------------
        Label[4] = QLabel("Server Host Name:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[4], 4, 0)

        IPC_Window.Page.ServerHostName = QLineEdit("localhost")
        StackLayout.addWidget(IPC_Window.Page.ServerHostName, 4, 1)

        # Server Port-----------------------------------------------------------
        Label[5] = QLabel("Server Port:")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[5], 5, 0)

        IPC_Window.Page.ServerPort = QLineEdit("10001")
        StackLayout.addWidget(IPC_Window.Page.ServerPort, 5, 1)

        # Allow Blocking--------------------------------------------------------
        Label[6] = QLabel("Allow Blocking (i.e. wait on RX):")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[6], 6, 0)

        IPC_Window.Page.BlockingOn = QRadioButton('True')
        IPC_Window.Page.BlockingOn.setChecked(True)
        SubLayout.addWidget(IPC_Window.Page.BlockingOn)
        IPC_Window.Page.BlockingOff = QRadioButton('False')
        IPC_Window.Page.BlockingOff.setChecked(False)
        SubLayout.addWidget(IPC_Window.Page.BlockingOff)
        StackLayout.addLayout(SubLayout, 6, 1)

        # Echo to stdout--------------------------------------------------------
        Label[7] = QLabel("Echo to stdout:")
        Label[7].setAlignment(Qt.AlignLeft)
        Label[7].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[7], 7, 0)

        IPC_Window.Page.Echo2stdOutOn = QRadioButton('True')
        IPC_Window.Page.Echo2stdOutOn.setChecked(True)
        SubLayout2.addWidget(IPC_Window.Page.Echo2stdOutOn)
        IPC_Window.Page.Echo2stdOutOff = QRadioButton('False')
        IPC_Window.Page.Echo2stdOutOff.setChecked(False)
        SubLayout2.addWidget(IPC_Window.Page.Echo2stdOutOff)
        StackLayout.addLayout(SubLayout2, 7, 1)

        # List Label------------------------------------------------------------
        Label[8] = QLabel("TX Prefix List")
        Label[8].setAlignment(Qt.AlignCenter)
        Label[8].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[8], 8, 0)

        # List of TX Prefixes---------------------------------------------------
        IPC_Window.Page.ListPrefix = QListWidget()
        IPC_Window.Page.ListPrefix.insertItem(0, "Prefix 1")
        IPC_Window.Page.ListPrefix.currentRowChanged.connect(IPC_Window.DisplaySubStackSlot)
        StackLayout.addWidget(IPC_Window.Page.ListPrefix, 9, 0, 2, 1)

        # SubStacked Widget-----------------------------------------------------
        IPC_Window.Page.SubStack = QStackedWidget()
        IPC_Window.AddSubStack(i)
        StackLayout.addWidget(IPC_Window.Page.SubStack,9,1,2,1)

        # -----------------------------------------------------------------
        IPC_Window.Page.setLayout(StackLayout)
        IPC_Window.Stack.addWidget(IPC_Window.Page)

    #---------------------------------------------------------------------------
    # SubStack------------------------------------------------------------------
    def AddSubStack(IPC_Window, i):
        Label = [0]*10 # Initialize array of labels
        IPC_Window.SubPage = QWidget() # Create widget to become a Page in Stack
        SubStackLayout = QGridLayout()
        SubLayout = QHBoxLayout() # for the radio buttons

        # TX Prefixes-------------------------------------------------------
        Label[0] = QLabel("TX Prefixes:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        SubStackLayout.addWidget(Label[0], 0, 0)

        IPC_Window.SubPage.TXprefix = QLineEdit("SC")
        SubStackLayout.addWidget(IPC_Window.SubPage.TXprefix, 0, 1)

        # Add Prefix Button-----------------------------------------------------
        AddPrefixBtn = QPushButton('Add a Prefix')
        SubStackLayout.addWidget(AddPrefixBtn, 1, 1)
        AddPrefixBtn.clicked.connect(IPC_Window.AddPrefix)

        # Remove Prefix btn-----------------------------------------------------
        RmvPrefixBtn = QPushButton('Remove a Prefix')
        SubStackLayout.addWidget(RmvPrefixBtn, 1, 0)
        RmvPrefixBtn.clicked.connect(IPC_Window.RmvPrefix)

        # Finalize Stack--------------------------------------------------------
        IPC_Window.SubPage.setLayout(SubStackLayout)
        IPC_Window.Page.SubStack.addWidget(IPC_Window.SubPage)

    def DisplaySubStackSlot(IPC_Window, i): # Displayes current page in Stack when selected in the IPC list
        IPC_Window.Stack.currentWidget().SubStack.setCurrentIndex(i)

    def AddPrefix(IPC_Window): # Adds a Prefix to list
        ItemIndex = IPC_Window.Stack.currentWidget().ListPrefix.count() # Index of new item
        ItemLabel = ItemIndex + 1
        IPC_Window.Stack.currentWidget().ListPrefix.insertItem(ItemIndex, "Prefix %s"%ItemLabel)
        IPC_Window.AddSubStack(ItemIndex)
        IPC_Window.Stack.currentWidget().ListPrefix.setCurrentRow(ItemIndex)
        IPC_Window.Stack.currentWidget().SubStack.setCurrentIndex(ItemIndex)

    def RmvPrefix(IPC_Window): # Removes the last Prefix added to the Prefix List
        RmvIndex = IPC_Window.Stack.currentWidget().ListPrefix.count() - 1
        if RmvIndex > 0:
            IPC_Window.Stack.currentWidget().ListPrefix.takeItem(RmvIndex)
            IPC_Window.Stack.currentWidget().SubStack.removeWidget(IPC_Window.Stack.currentWidget().SubStack.widget(RmvIndex))

    #---------------------------------------------------------------------------

    def DisplayStackSlot(IPC_Window, i): # Displayes current page in Stack when selected in the IPC list
        IPC_Window.Stack.setCurrentIndex(i)

    def AddIPC(IPC_Window): # Adds a IPC to list and Stack
        ItemIndex = IPC_Window.ListIPC.count() # Index of new item
        ItemLabel = ItemIndex + 1
        IPC_Window.ListIPC.insertItem(int(ItemIndex), "IPC %s"%ItemLabel)
        IPC_Window.AddStack(ItemIndex)
        IPC_Window.ListIPC.setCurrentRow(ItemIndex)
        IPC_Window.Stack.setCurrentIndex(ItemIndex)

    def RmvIPC(IPC_Window): # Removes the last IPC added to the IPC List
        RmvIndex = IPC_Window.ListIPC.count() - 1
        if RmvIndex > 0:
            IPC_Window.ListIPC.takeItem(RmvIndex)
            IPC_Window.Stack.removeWidget(IPC_Window.Stack.widget(RmvIndex))
#-------------------------------------------------------------------------------
# Slots-------------------------------------------------------------------------
    def WriteFileOut(IPC_Window):
        # Write inputs to txt Write File
        IPC_Window.WriteFile = open('InOut/Inp_IPC.txt', 'w+')
        IPC_Window.WriteFile.writelines(IPC_Window.IpcDataOut)
        IPC_Window.WriteFile.close()

    def SetDefaultGuiFields(IPC_Window): # Resets gui options and input txt data to default
        # Reset Write File to Default/readfile
        IPC_Window.GetDataIn('InOut/Default/Inp_IPC_Default.txt')
        for i in range(IPC_Window.NumSocketsDataIn):
            IPC_Window.SetGuiFields(i)

    def WriteDataOut(IPC_Window):
        IPC_Window.IpcDataOut[0] = "<<<<<<<<<<<<<<< 42: InterProcess Comm Configuration File >>>>>>>>>>>>>>>>\n"
        IPC_Window.IpcDataOut[1] = str(IPC_Window.ListIPC.count()) + "                                       ! Number of Sockets\n"
        txt_idx = 2
        for i in range(IPC_Window.ListIPC.count()):
            IPC_Window.IpcDataOut[txt_idx] = "**********************************  IPC " + str(i) + "   *****************************\n"
            txt_idx += 1

            if IPC_Window.Stack.widget(i).Mode.currentText() == "OFF":
                IPC_Window.IpcDataOut[txt_idx] = "OFF                                     ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n"
            elif IPC_Window.Stack.widget(i).Mode.currentText() == "TX":
                IPC_Window.IpcDataOut[txt_idx] = "TX                                      ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n"
            elif IPC_Window.Stack.widget(i).Mode.currentText() == "RX":
                IPC_Window.IpcDataOut[txt_idx] = "RX                                      ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n"
            elif IPC_Window.Stack.widget(i).Mode.currentText() == "TXRX":
                IPC_Window.IpcDataOut[txt_idx] = "TXRX                                    ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n"
            elif IPC_Window.Stack.widget(i).Mode.currentText() == "ACS":
                IPC_Window.IpcDataOut[txt_idx] = "ACS                                     ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n"
            elif IPC_Window.Stack.widget(i).Mode.currentText() == "WRITEFILE":
                IPC_Window.IpcDataOut[txt_idx] = "WRITEFILE                               ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n"
            elif IPC_Window.Stack.widget(i).Mode.currentText() == "READFILE":
                IPC_Window.IpcDataOut[txt_idx] = "READFILE                                ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n"
            txt_idx += 1

            IPC_Window.IpcDataOut[txt_idx] = IPC_Window.Stack.widget(i).AcId.text() + "                                       ! AC.ID for ACS mode\n"
            txt_idx += 1

            IPC_Window.IpcDataOut[txt_idx] = "\"" + IPC_Window.Stack.widget(i).FileName.text() + "\"" + "                            ! File name for WRITE or READ\n"
            txt_idx += 1

            if IPC_Window.Stack.widget(i).SocketRole.currentText() == "CLIENT":
                IPC_Window.IpcDataOut[txt_idx] = "CLIENT                                  ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)\n"
            elif IPC_Window.Stack.widget(i).SocketRole.currentText() == "SERVER":
                IPC_Window.IpcDataOut[txt_idx] = "SERVER                                  ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)\n"
            elif IPC_Window.Stack.widget(i).SocketRole.currentText() == "GMSEC_CLIENT":
                IPC_Window.IpcDataOut[txt_idx] = "GMSEC_CLIENT                            ! Socket Role (SERVER,CLIENT,GMSEC_CLIENT)\n"
            txt_idx += 1

            IPC_Window.IpcDataOut[txt_idx] = IPC_Window.Stack.widget(i).ServerHostName.text() + "     " + IPC_Window.Stack.widget(i).ServerPort.text() + "                     ! Server Host Name, Port\n"
            txt_idx += 1

            if IPC_Window.Stack.widget(i).BlockingOn.isChecked():
                IPC_Window.IpcDataOut[txt_idx] = "TRUE                                    ! Allow Blocking (i.e. wait on RX)\n"
            elif IPC_Window.Stack.widget(i).BlockingOff.isChecked():
                IPC_Window.IpcDataOut[txt_idx] = "FALSE                                   ! Allow Blocking (i.e. wait on RX)\n"
            txt_idx += 1

            if IPC_Window.Stack.widget(i).Echo2stdOutOn.isChecked():
                IPC_Window.IpcDataOut[txt_idx] = "TRUE                                    ! Echo to stdout\n"
            elif IPC_Window.Stack.widget(i).Echo2stdOutOff.isChecked():
                IPC_Window.IpcDataOut[txt_idx] = "FALSE                                   ! Echo to stdout\n"
            txt_idx += 1

            IPC_Window.IpcDataOut[txt_idx] = str(IPC_Window.Stack.widget(i).ListPrefix.count()) + "                                       ! Number of TX prefixes\n"
            txt_idx += 1

            for j in range(IPC_Window.Stack.widget(i).ListPrefix.count()):
                IPC_Window.IpcDataOut[txt_idx] = "\"" + IPC_Window.Stack.widget(i).SubStack.widget(j).TXprefix.text() + "\"" + "                                    ! Prefix " + str(j) + "\n"
                txt_idx += 1

    def GetDataIn(IPC_Window, FileStr):
        IPC_Window.IpcDataIn = []
        pattern = re.compile("(.*?)\s*!")

        InpDataFile = open(FileStr)
        for i, line in enumerate(InpDataFile):
            for match in re.finditer(pattern, line):
                IPC_Window.IpcDataIn.append(match.group(1).replace('"',"").split())
        InpDataFile.close()
        IPC_Window.NumSocketsDataIn = int(IPC_Window.IpcDataIn[0][0])

    def SetGuiFields(IPC_Window,i):
        if (i == 0):
            IPC_Window.idx_vec = [0]*IPC_Window.NumSocketsDataIn
            IPC_Window.NumPrefixesDataIn = [0]*IPC_Window.NumSocketsDataIn
            IPC_Window.idx_vec[0] = 1
        idx = IPC_Window.idx_vec[i]

        if IPC_Window.ListIPC.count() > IPC_Window.NumSocketsDataIn:
            Diff = IPC_Window.ListIPC.count() - IPC_Window.NumSocketsDataIn
            for i in range(Diff):
                IPC_Window.RmvIPC()

        if (i >= IPC_Window.ListIPC.count()):
            IPC_Window.AddIPC()

        if (IPC_Window.IpcDataIn[idx][0].lower() == "off"):
            IPC_Window.Stack.widget(i).Mode.setCurrentIndex(0)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "tx"):
            IPC_Window.Stack.widget(i).Mode.setCurrentIndex(1)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "rx"):
            IPC_Window.Stack.widget(i).Mode.setCurrentIndex(2)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "txrx"):
            IPC_Window.Stack.widget(i).Mode.setCurrentIndex(3)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "acs"):
            IPC_Window.Stack.widget(i).Mode.setCurrentIndex(4)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "writefile"):
            IPC_Window.Stack.widget(i).Mode.setCurrentIndex(5)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "readfile"):
            IPC_Window.Stack.widget(i).Mode.setCurrentIndex(6)
        idx += 1

        IPC_Window.Stack.widget(i).AcId.setText(IPC_Window.IpcDataIn[idx][0])
        idx += 1
        IPC_Window.Stack.widget(i).FileName.setText(IPC_Window.IpcDataIn[idx][0])
        idx += 1

        if (IPC_Window.IpcDataIn[idx][0].lower() == "client"):
            IPC_Window.Stack.widget(i).SocketRole.setCurrentIndex(1)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "server"):
            IPC_Window.Stack.widget(i).SocketRole.setCurrentIndex(0)
        elif (IPC_Window.IpcDataIn[idx][0].lower() == "gmsec_client"):
            IPC_Window.Stack.widget(i).SocketRole.setCurrentIndex(2)
        idx += 1

        IPC_Window.Stack.widget(i).ServerHostName.setText(IPC_Window.IpcDataIn[idx][0])
        IPC_Window.Stack.widget(i).ServerPort.setText(IPC_Window.IpcDataIn[idx][1])
        idx += 1

        if (IPC_Window.IpcDataIn[idx][0].lower() == 'true'):
            IPC_Window.Stack.widget(i).BlockingOn.setChecked(True)
        else:
            IPC_Window.Stack.widget(i).BlockingOff.setChecked(True)
        idx += 1

        if (IPC_Window.IpcDataIn[idx][0].lower() == 'true'):
            IPC_Window.Stack.widget(i).Echo2stdOutOn.setChecked(True)
        else:
            IPC_Window.Stack.widget(i).Echo2stdOutOff.setChecked(True)
        idx += 1

        IPC_Window.NumPrefixesDataIn[i] = int(IPC_Window.IpcDataIn[idx][0])
        idx += 1

        for j in range(IPC_Window.NumPrefixesDataIn[i]):
            if (IPC_Window.Stack.widget(i).ListPrefix.count() > IPC_Window.NumPrefixesDataIn[i]):
                Diff = IPC_Window.Stack.widget(i).ListPrefix.count() - IPC_Window.NumPrefixesDataIn[i]
                for i in range(Diff):
                    IPC_Window.RmvPrefix()

            if (j >= IPC_Window.Stack.widget(i).ListPrefix.count()):
                print("Attempting to add prefix")
                IPC_Window.AddPrefix()

            IPC_Window.Stack.widget(i).SubStack.widget(j).TXprefix.setText(IPC_Window.IpcDataIn[idx][0])
            idx += 1

        if i < (IPC_Window.NumSocketsDataIn - 1):
            IPC_Window.idx_vec[i + 1] = idx
