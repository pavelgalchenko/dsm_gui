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
        ReadFile = open('InOut/Inp_Sim.txt', 'r')
        Inp_Sim_data = ReadFile.readlines()
        Label = [0]*10 # Initialize

        # Make Widgets for Simulation Window------------------------------------
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

        TimeMode = QComboBox()
        TimeMode.addItems(["FAST", "REAL", "EXTERNAL", "NOS3"])
        SimLayout.addWidget(TimeMode, 1, 1)

        # Sim Duration, Step size-----------------------------------------------
        Label[2] = QLabel("Sim Duration (sec):")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[2], 2, 0)

        SimDuration = QLineEdit('30000.0')
        SimLayout.addWidget(SimDuration, 2, 1)

        Label[3] = QLabel("Step Size (sec):")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[3], 3, 0)

        StepSize = QLineEdit('0.1')
        SimLayout.addWidget(StepSize, 3, 1)

        # Front end Graphics----------------------------------------------------
        Label[4] = QLabel("Front End Graphics:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[4], 4, 0)

        SimWindow.GraphicsOn = QRadioButton('On')
        SimWindow.GraphicsOn.setChecked(False)
        SimWindow.GraphicsOff = QRadioButton('Off')
        SimWindow.GraphicsOff.setChecked(True)

        #if GraphicsOn.isChecked():
        #    Inp_Sim_data[5] = "1234                            !  Graphics Front End? \n"
        #elif GraphicsOff.isChecked():
        #    Inp_Sim_data[5] = "FALSE                           !  Graphics Front End?"



        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SimWindow.GraphicsOn)
        SubLayout.addWidget(SimWindow.GraphicsOff)

        SimLayout.addLayout(SubLayout,4,1)

        SimWindow.GraphicsOn.toggled.connect(lambda:SimWindow.GraphicsOn.GraphicsOnSlot)
        SimWindow.GraphicsOff.toggled.connect(lambda:SimWindow.GraphicsOff.GraphicsOffSlot)

    # Slot Functions------------------------------------------------------------
    def GraphicsOnSlot(SimWindow):
        if SimWindow.GraphicsOn.clicked():
            Inp_Sim_data[5] = "TRUE                            !  Graphics Front End? \n"
    def GraphicsOffSlot(SimWindow):
        if SimWindow.GraphicsOff.clicked():
            Inp_Sim_data[5] = "FALSE                           !  Graphics Front End?"









        #Inp_Sim_data[2] = 'REAL                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)'
        #print(Inp_Sim_data[2])





        # Write inputs to txt Write File----------------------------------------
        WriteFile = open('InOut/TestWrite.txt', 'w')
        WriteFile.writelines(Inp_Sim_data)

        # Finialize Simulation window
        SimWindow.setLayout(SimLayout)
