# This file contains the code for the TRDS window in the 42 gui

# Import Packages---------------------------------------------------------------
import sys
import numpy as numpy
import re
import functions
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
from PyQt5.QtWidgets import QButtonGroup

class TDRS_Window(QWidget):
    def __init__(TDRS_Window):
        super().__init__()
        TDRS_Window.setWindowTitle('TDRS Parameters') # sets the window’s title
        TDRS_Window.setGeometry(0, 0, 250, 200) # Set window, can resize manually (xpos on monitor, ypos on monitor, width pixels, height pixles)
        TDRS_Layout = QGridLayout()

        Label = [0]*2 # Initialize array of labels
        TDRS_Window.TdrsDataOut = [""]*300 # Initialize List

        TDRS_Window.missionDir = functions.get_mission_directory(TDRS_Window)
        TDRS_Window.inputFile = TDRS_Window.missionDir + '/InOut/Inp_TDRS.txt'
        TDRS_Window.defaultFile = TDRS_Window.missionDir + '/InOut/Default/Inp_TDRS.txt'
        TDRS_Window.GetDataIn(TDRS_Window.inputFile)

        # Heading Label---------------------------------------------------------
        Label[0] = QLabel("42 TDRS Configuration")
        Label[0].setAlignment(Qt.AlignCenter)
        Label[0].setStyleSheet('font-size: 20px')
        TDRS_Layout.addWidget(Label[0], 0, 0, 1, 3)

        # List Label------------------------------------------------------------
        Label[1] = QLabel("TDRS List")
        Label[1].setAlignment(Qt.AlignCenter)
        Label[1].setStyleSheet('font-size: 15px')
        TDRS_Layout.addWidget(Label[1], 1, 0)

        # List of TDRS----------------------------------------------------------
        TDRS_Window.List = QListWidget()
        for i in range(TDRS_Window.NumTdrsDataIn):
            ItemLabel = i + 1
            TDRS_Window.List.insertItem(i,"TDRS %s"%ItemLabel)
        TDRS_Layout.addWidget(TDRS_Window.List, 1, 0, 2, 1)
        TDRS_Window.List.currentRowChanged.connect(TDRS_Window.DisplayStackSlot)

        # Stacked Widget--------------------------------------------------------
        TDRS_Window.Stack = QStackedWidget()
        for i in range(TDRS_Window.NumTdrsDataIn):
            TDRS_Window.AddStack(i)
            TDRS_Window.SetGuiFields(i)
        TDRS_Layout.addWidget(TDRS_Window.Stack,1,1,3,2)

#-------------------------------------------------------------------------------
# Apply / Cancel / Reset Default Button-----------------------------------------
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(TDRS_Window.close)
        ApplyBtn.clicked.connect(TDRS_Window.WriteDataOut)
        ApplyBtn.clicked.connect(TDRS_Window.WriteFileOut) # Last Slot
        TDRS_Layout.addWidget(ApplyBtn, 4, 2)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(TDRS_Window.close)
        TDRS_Layout.addWidget(CancelBtn, 4, 1)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(TDRS_Window.SetDefaultGuiFields)
        TDRS_Layout.addWidget(ResetBtn, 4, 0)

#-------------------------------------------------------------------------------
# Finialize TDRS window-------------------------------------------------------
        TDRS_Window.setLayout(TDRS_Layout)
#-------------------------------------------------------------------------------
# TDRS Stack--------------------------------------------------------------
    def AddStack(TDRS_Window, i): # Adds a new page to the TDRS Stack with the same layout as previous pages
        Label = [0]*10 # Initialize array of labels
        TDRS_Window.Page = QWidget() # Create widget to become a Page in Stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout() # for the radio buttons

        # TDRS Status------------------------------------------------------
        Label[0] = QLabel("TDRS Status:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[0], 0, 0)

        TDRS_Window.Page.StatusOn = QRadioButton('On')
        TDRS_Window.Page.StatusOn.setChecked(True)
        TDRS_Window.Page.StatusOff = QRadioButton('Off')
        TDRS_Window.Page.StatusOff.setChecked(False)

        TDRS_Window.Page.StatusBtnGroup = QButtonGroup()
        TDRS_Window.Page.StatusBtnGroup.addButton(TDRS_Window.Page.StatusOn)
        TDRS_Window.Page.StatusBtnGroup.addButton(TDRS_Window.Page.StatusOff)

        SubLayout.addWidget(TDRS_Window.Page.StatusOn)
        SubLayout.addWidget(TDRS_Window.Page.StatusOff)
        StackLayout.addLayout(SubLayout, 0, 1)

        # TDRS Designation Label-----------------------------------------------------
        Label[1] = QLabel("Designation:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 1, 0)

        TDRS_Window.Page.Designation = QLineEdit("TDRS %s"%TDRS_Window.List.count())
        StackLayout.addWidget(TDRS_Window.Page.Designation, 1, 1)

        # Add TDRS Button-----------------------------------------------------
        AddTDRSbtn = QPushButton('Add a TDRS')
        StackLayout.addWidget(AddTDRSbtn, 2, 1)
        AddTDRSbtn.clicked.connect(TDRS_Window.AddTDRS)

        # Remove TDRS btn
        RmvTDRSbtn = QPushButton('Remove a TDRS')
        StackLayout.addWidget(RmvTDRSbtn, 2, 0)
        RmvTDRSbtn.clicked.connect(TDRS_Window.RmvTDRS)

        TDRS_Window.Page.setLayout(StackLayout)
        TDRS_Window.Stack.addWidget(TDRS_Window.Page)

    def DisplayStackSlot(TDRS_Window, i): # Displayes current page in Stack when selected in the TDRS list
        TDRS_Window.Stack.setCurrentIndex(i)

    def AddTDRS(TDRS_Window): # Adds a TDRS to list and Stack
        ItemIndex = TDRS_Window.List.count() # Index of new item
        ItemLabel = ItemIndex + 1
        TDRS_Window.List.insertItem(int(ItemIndex), "TDRS %s"%ItemLabel)
        TDRS_Window.DataInDesignation.append("")
        TDRS_Window.DataInStatus.append("")
        TDRS_Window.AddStack(ItemIndex)
        TDRS_Window.List.setCurrentRow(ItemIndex)
        TDRS_Window.Stack.setCurrentIndex(ItemIndex)

    def RmvTDRS(TDRS_Window): # Removes the last TDRS added to the TDRS List
        RmvIndex = TDRS_Window.List.count() - 1
        if RmvIndex > 0:
            TDRS_Window.List.takeItem(RmvIndex)
            TDRS_Window.Stack.removeWidget(TDRS_Window.Stack.widget(RmvIndex))
#-------------------------------------------------------------------------------
# Slots-------------------------------------------------------------------------
    def WriteFileOut(TDRS_Window):
        # Write inputs to txt Write File
        TDRS_Window.WriteFile = open(TDRS_Window.inputFile, 'w+')
        TDRS_Window.WriteFile.writelines(TDRS_Window.TdrsDataOut)
        TDRS_Window.WriteFile.close()

    def SetDefaultGuiFields(TDRS_Window): # Resets gui options and input txt data to default
        # Reset Write File to Default/readfile
        TDRS_Window.GetDataIn(TDRS_Window.defaultFile)
        for i in range(TDRS_Window.NumTdrsDataIn):
            TDRS_Window.SetGuiFields(i)

    def WriteDataOut(TDRS_Window):
        TDRS_Window.TdrsDataOut[0] = "<<<<<<<<<<<<<<<<<<<  42 TDRS Configuration File  >>>>>>>>>>>>>>>>>>>>>>\n"
        txt_idx = 1
        for i in range(TDRS_Window.List.count()):
            if TDRS_Window.Stack.widget(i).StatusOn.isChecked():
                TDRS_Window.TdrsDataOut[txt_idx] = "TRUE    " + "\"" + TDRS_Window.Stack.widget(i).Designation.text() + "\"" + "             ! TDRS-" + str(i) + " Exists, Designation\n"
            elif TDRS_Window.Stack.widget(i).StatusOff.isChecked():
                TDRS_Window.TdrsDataOut[txt_idx] = "FALSE   " + "\"" + TDRS_Window.Stack.widget(i).Designation.text() + "\"" + "             ! TDRS-" + str(i) + " Exists, Designation\n"
            txt_idx += 1

    def GetDataIn(TDRS_Window, FileStr):
        TDRS_Window.DataInDesignation = []
        TDRS_Window.DataInStatus = []

        patternDesignation = re.compile('"(.*?)"')
        patternBool = re.compile ('(TRUE|FALSE)+',re.IGNORECASE)

        InpDataFile = open(FileStr)
        for i, line in enumerate(InpDataFile):
            for match in re.finditer(patternDesignation, line):
                TDRS_Window.DataInDesignation.append(match.group(1))
            for match in re.finditer(patternBool, line):
                TDRS_Window.DataInStatus.append(match.group(1))

        TDRS_Window.NumTdrsDataIn = len(TDRS_Window.DataInDesignation)

    def SetGuiFields(TDRS_Window,i):
        if TDRS_Window.List.count() > TDRS_Window.NumTdrsDataIn:
            Diff = TDRS_Window.List.count() - TDRS_Window.NumTdrsDataIn
            for i in range(Diff):
                TDRS_Window.RmvTDRS()

        if i >= TDRS_Window.List.count():
            TDRS_Window.AddTDRS()

        if (TDRS_Window.DataInStatus[i].lower() == 'true'):
            TDRS_Window.Stack.widget(i).StatusOn.setChecked(True)
        else:
            TDRS_Window.Stack.widget(i).StatusOff.setChecked(True)

        TDRS_Window.Stack.widget(i).Designation.setText(TDRS_Window.DataInDesignation[i])
