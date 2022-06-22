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


class SimWindow(QWidget):
    def __init__(SimWindow):
        # Set Up Window
        super().__init__()
        SimWindow.setWindowTitle('Simulation Parameters') # sets the window’s title
        SimWindow.setFixedSize(500, 500) # gives window a fixed size
        SimLayout = QGridLayout()

        # Load Data from Inp_Sim.txt
        SimWindow.ReadFile = open('InOut/Inp_Sim_Default.txt', 'r')
        SimWindow.Inp_Sim_data = SimWindow.ReadFile.readlines()

        # Make Widgets for Simulation Window------------------------------------
        Label = [0]*10 # Initialize array of labels

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

        # Apply / Cancel / Reset Default Button---------------------------------
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(SimWindow.close)
        ApplyBtn.clicked.connect(SimWindow.GraphicsSlot)
        ApplyBtn.clicked.connect(SimWindow.TimeModeSlot)
        ApplyBtn.clicked.connect(SimWindow.SimDurationSlot)
        ApplyBtn.clicked.connect(SimWindow.WriteFileSlot) # Last Slot
        SimLayout.addWidget(ApplyBtn, 6, 3)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(SimWindow.close)
        SimLayout.addWidget(CancelBtn, 6, 2)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(SimWindow.DefaultWriteSlot)
        SimLayout.addWidget(ResetBtn, 6, 1)


        # Finialize Simulation window
        SimWindow.setLayout(SimLayout)

    # Slot Functions------------------------------------------------------------
    def GraphicsSlot(SimWindow):
        if SimWindow.GraphicsOn.isChecked():
            SimWindow.Inp_Sim_data[5] = "TRUE                            !  Graphics Front End?\n"
        elif SimWindow.GraphicsOff.isChecked():
            SimWindow.Inp_Sim_data[5] = "FALSE                           !  Graphics Front End?\n"

    def TimeModeSlot(SimWindow):
        if SimWindow.TimeMode.currentText() == "FAST":
            SimWindow.Inp_Sim_data[2] = "FAST                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "REAL":
            SimWindow.Inp_Sim_data[2] = "REAL                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "EXTERNAL":
            SimWindow.Inp_Sim_data[2] = "EXTERNAL                        !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "NOS3":
            SimWindow.Inp_Sim_data[2] = "NOS3                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"

    def WriteFileSlot(SimWindow):
        # Write inputs to txt Write File
        SimWindow.WriteFile = open('InOut/Inp_Sim.txt', 'w')
        SimWindow.WriteFile.writelines(SimWindow.Inp_Sim_data)

    def DefaultWriteSlot(SimWindow):
        # Reset Write File to Default/readfile
        SimWindow.ReadFile = open('InOut/Inp_Sim_Default.txt', 'r')
        SimWindow.Inp_Sim_data = SimWindow.ReadFile.readlines()

        # Reset GUI selections
        SimWindow.GraphicsOn.setChecked(True)
        SimWindow.GraphicsOff.setChecked(False)
        SimWindow.SimDuration.setText("30000.0")
        SimWindow.StepSize.setText("0.1")
        SimWindow.TimeMode.setCurrentIndex(0)
        # To be continued

    def SimDurationSlot(SimWindow):
        SimWindow.Inp_Sim_data[3] = SimWindow.SimDuration.text() + "   " + SimWindow.StepSize.text() + "                   !  Sim Duration, Step Size [sec]\n"
