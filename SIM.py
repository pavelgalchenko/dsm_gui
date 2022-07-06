# This file contains the code for the Simulation window in the 42 gui

# Import Packages---------------------------------------------------------------
import sys
import numpy as numpy
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
from PyQt5.QtWidgets import QVBoxLayout # verticle stack layout
from PyQt5.QtWidgets import QHBoxLayout # Horizontal stack layout
from PyQt5.QtWidgets import QRadioButton
from PyQt5.QtWidgets import QListWidget



class SimWindow(QWidget):
    def __init__(SimWindow):
        # Set Up Window
        super().__init__()
        SimWindow.setWindowTitle('Simulation Parameters') # sets the window’s title
        SimWindow.setFixedSize(600, 400) # gives window a fixed size
        SimLayout = QGridLayout()

        # Load Data from Inp_Sim.txt
        SimWindow.ReadFile = open('InOut/Inp_Sim_Default.txt', 'r')
        SimWindow.Inp_Sim_data = SimWindow.ReadFile.readlines()

        # Make Widgets for Simulation Window------------------------------------
        Label = [0]*10 # Initialize array of labels

#-------------------------------------------------------------------------------
# Simulation Control Section----------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label[0] = QLabel("Simulation Control")
        Label[0].setAlignment(Qt.AlignCenter)
        Label[0].setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label[0], 0, 0, 1, 2)

        # Time Mode-------------------------------------------------------------
        Label[1] = QLabel("Time Mode:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[1], 1, 0)

        SimWindow.TimeMode = QComboBox()
        SimWindow.TimeMode.addItems(["FAST", "REAL", "EXTERNAL", "NOS3"])
        SimLayout.addWidget(SimWindow.TimeMode, 1, 1)

        # Sim Duration, Step size-----------------------------------------------
        Label[2] = QLabel("Sim Duration (sec):")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[2], 2, 0)

        SimWindow.SimDuration = QLineEdit('30000.0')
        SimLayout.addWidget(SimWindow.SimDuration, 2, 1)

        Label[3] = QLabel("Step Size (sec):")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[3], 3, 0)

        SimWindow.StepSize = QLineEdit('0.1')
        SimLayout.addWidget(SimWindow.StepSize, 3, 1)

        # Front end Graphics----------------------------------------------------
        Label[4] = QLabel("Front End Graphics:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[4], 4, 0)

        SimWindow.GraphicsOn = QRadioButton('On')
        SimWindow.GraphicsOn.setChecked(True)

        SimWindow.GraphicsOff = QRadioButton('Off')
        SimWindow.GraphicsOff.setChecked(False)

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SimWindow.GraphicsOn)
        SubLayout.addWidget(SimWindow.GraphicsOff)
        SimLayout.addLayout(SubLayout,4,1)

        # File Output Interval--------------------------------------------------
        Label[5] = QLabel("File Output Interval (sec):")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[5], 5, 0)
        SimWindow.OutputInterval = QLineEdit("10.0")
        SimLayout.addWidget(SimWindow.OutputInterval, 5, 1)

        # Cmd File Name---------------------------------------------------------
        Label[6] = QLabel("Cmd. Script File Name:")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[6], 6, 0)
        SimWindow.CmdFileName = QLineEdit("Inp_Cmd.txt")
        SimLayout.addWidget(SimWindow.CmdFileName, 6, 1)

#-------------------------------------------------------------------------------
# Reference Orbits Section------------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label[7] = QLabel("Reference Orbits")
        Label[7].setAlignment(Qt.AlignCenter)
        Label[7].setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label[7], 7, 0, 1, 2)

        # Stacked Widgets-------------------------------------------------------
        # Ref Orbit Status------------------------------------------------------
        Label[8] = QLabel("Reference Orbit Status:")
        Label[8].setAlignment(Qt.AlignLeft)
        Label[8].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[8], 8, 1)
        SimWindow.RefOrbOn = QRadioButton('On')
        SimWindow.RefOrbOn.setChecked(True)
        SimWindow.RefOrbOff = QRadioButton('Off')
        SimWindow.RefOrbOff.setChecked(False)

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SimWindow.RefOrbOn)
        SubLayout.addWidget(SimWindow.RefOrbOff)
        SimLayout.addLayout(SubLayout,8,2)

        # Orbit Input File Name-------------------------------------------------
        Label[9] = QLabel("Orbit Input File Name:")
        Label[9].setAlignment(Qt.AlignLeft)
        Label[9].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[9], 9, 1)
        SimWindow.RefOrbs = QLineEdit("Orb_LEO.txt")
        SimLayout.addWidget(SimWindow.RefOrbs, 9, 2)

        AddOrbBtn = QPushButton('Add an Orbit')
        SimLayout.addWidget(AddOrbBtn, 10, 2)
        AddOrbBtn.clicked.connect(SimWindow.AddRefOrbSlot)


        # need to make X number of Input file name slots for X number of SC
        # idea: add orbit button for adding num ref orbits?

        SimWindow.ListRefOrb = QListWidget()
        SimWindow.ListRefOrb.insertItem(0, "Ref. Orb. 1")
        SimLayout.addWidget(SimWindow.ListRefOrb, 8, 0, 3, 1)

        SimWindow.RefOrbStack = QStackedWidget()
        




        # Apply / Cancel / Reset Default Button---------------------------------
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(SimWindow.close)
        ApplyBtn.clicked.connect(SimWindow.WidgetsSlot)
        ApplyBtn.clicked.connect(SimWindow.WriteFileSlot) # Last Slot
        SimLayout.addWidget(ApplyBtn, 12, 3)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(SimWindow.close)
        SimLayout.addWidget(CancelBtn, 12, 2)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(SimWindow.DefaultWriteSlot)
        SimLayout.addWidget(ResetBtn, 12, 1)

        # Finialize Simulation window
        SimWindow.setLayout(SimLayout)

    # Slot Functions------------------------------------------------------------
    #---------------------------------------------------------------------------
    def WriteFileSlot(SimWindow): # Write edited input txt data to output txt file
        # Write inputs to txt Write File
        SimWindow.WriteFile = open('InOut/Inp_Sim.txt', 'w')
        SimWindow.WriteFile.writelines(SimWindow.Inp_Sim_data)

    def DefaultWriteSlot(SimWindow): # Resets gui options and input txt data to default
        # Reset Write File to Default/readfile
        SimWindow.ReadFile = open('InOut/Inp_Sim_Default.txt', 'r')
        SimWindow.Inp_Sim_data = SimWindow.ReadFile.readlines()

        # Reset GUI selections
        SimWindow.TimeMode.setCurrentIndex(0)
        SimWindow.SimDuration.setText("30000.0")
        SimWindow.StepSize.setText("0.1")
        SimWindow.OutputInterval.setText("10.0")
        SimWindow.GraphicsOn.setChecked(True)
        SimWindow.GraphicsOff.setChecked(False)
        SimWindow.CmdFileName.setText("Inp_Cmd.txt")

    def WidgetsSlot(SimWindow): # Take GUI inputs from widgets and write to Inp_sim.txt
        # TimeModeSlot
        if SimWindow.TimeMode.currentText() == "FAST":
            SimWindow.Inp_Sim_data[2] = "FAST                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "REAL":
            SimWindow.Inp_Sim_data[2] = "REAL                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "EXTERNAL":
            SimWindow.Inp_Sim_data[2] = "EXTERNAL                        !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "NOS3":
            SimWindow.Inp_Sim_data[2] = "NOS3                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"

        # SimDurationSlot
        SimWindow.Inp_Sim_data[3] = SimWindow.SimDuration.text() + "   " + SimWindow.StepSize.text() + "                   !  Sim Duration, Step Size [sec]\n"

        # OutputIntervalSlot
        SimWindow.Inp_Sim_data[4] = SimWindow.OutputInterval.text() + "                            !  File Output Interval [sec]\n"

        # GraphicsSlot
        if SimWindow.GraphicsOn.isChecked():
            SimWindow.Inp_Sim_data[5] = "TRUE                            !  Graphics Front End?\n"
        elif SimWindow.GraphicsOff.isChecked():
            SimWindow.Inp_Sim_data[5] = "FALSE                           !  Graphics Front End?\n"

        # CmdFileNameSlot
        SimWindow.Inp_Sim_data[6] = SimWindow.CmdFileName.text() + "                     !  Command Script File Name\n"

        # RefOrbsSlot
        SimWindow.Inp_Sim_data[6] = SimWindow.RefOrbs.text() + "                               !  Number of Reference Orbits\n"

    def AddRefOrbSlot(SimWindow):
        ItemIndex = SimWindow.ListRefOrb.count() + 1
        SimWindow.ListRefOrb.insertItem(int(ItemIndex), "Ref. Orb. %s"%ItemIndex)
