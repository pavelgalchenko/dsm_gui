# This file contains the code for the FOV window in the 42 gui

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
from PyQt5.QtWidgets import QButtonGroup
from PyQt5.QtWidgets import QStackedWidget

class FOV_Window(QWidget):
    def __init__(FOV_Window):
        # Set Up Window
        super().__init__()
        FOV_Window.setWindowTitle('Simulation Parameters') # sets the window’s title
        FOV_Window.setFixedSize(600, 500) # gives window a fixed size
        FOV_Layout = QGridLayout()

        # Load Data from Inp_FOV.txt
        FOV_Window.ReadFile = open('InOut/Inp_FOV.txt', 'r')
        FOV_Window.Inp_FOV_data = FOV_Window.ReadFile.readlines()

        # Make Widgets for FOV Window-------------------------------------------
        Label = [0]*2 # Initialize array of labels

        # Number of FOVs -------------------------------------------------------
        Label[0] = QLabel("Number of FOV")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        # FOV_Layout.addWidget(Label[0],0,0)

        FOV_Window.NumFOV = QLineEdit('4')
        # FOV_Layout.addWidget(FOV_Window.NumFOV,1,0)

        # FOV List -------------------------------------------------------------
        Label[1] = QLabel("FOV(s)")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        # FOV_Layout.addWidget(Label[1],2,0)

        FOV_Window.ListFOV = QListWidget()

        for x in range(int(FOV_Window.NumFOV.text())):
            FOV_Window.ListFOV.insertItem(x,"FOV %s"%x)
        # FOV_Layout.addWidget(FOV_Window.ListFOV,3,0,8,1)

        # Properties of FOV ----------------------------------------------------
        numStack = int(FOV_Window.NumFOV.text());

        FOV_Window.Stack = QStackedWidget()

        for x in range(numStack):
            FOV_Window.Stack.insertWidget(x,QWidget())
            FOV_Window.stackUI(x)

        # Apply / Cancel / Reset Default Button---------------------------------
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(FOV_Window.close)
        # ApplyBtn.clicked.connect(FOV_Window.GraphicsSlot)
        # ApplyBtn.clicked.connect(FOV_Window.ModeFOV_Slot)
        # ApplyBtn.clicked.connect(FOV_Window.WriteFileSlot) # Last Slot
        # FOV_Layout.addWidget(ApplyBtn, 1, 3)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(FOV_Window.close)
        # FOV_Layout.addWidget(CancelBtn, 1, 2)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(FOV_Window.close)
        # ResetBtn.clicked.connect(FOV_Window.DefaultWriteSlot)
        # FOV_Layout.addWidget(ResetBtn, 1, 1)

        # Layouts
        hboxLeft = QVBoxLayout()
        hboxLeft.addWidget(Label[0])
        hboxLeft.addWidget(FOV_Window.NumFOV)
        hboxLeft.addWidget(Label[1])
        hboxLeft.addWidget(FOV_Window.ListFOV)
        FOV_Layout.addLayout(hboxLeft,0,0)

        hboxBot = QHBoxLayout()
        hboxBot.addWidget(ApplyBtn,2)
        hboxBot.addWidget(CancelBtn,1)
        hboxBot.addWidget(ResetBtn,0)
        FOV_Layout.addLayout(hboxBot,1,1)

        # Finialize FOV window
        FOV_Layout.addWidget(FOV_Window.Stack,0,1)
        FOV_Window.setLayout(FOV_Layout)
        FOV_Window.ListFOV.currentRowChanged.connect(FOV_Window.display)

    def stackUI(FOV_Window,indexStack):
        FOV_Layout = QGridLayout()
        Label = [0]*9

        # Label ----------------------------------------------------------------
        Label[0] = QLabel("Label:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[0],0,1)

        FOV_Window.LabelFOV = QLineEdit()
        FOV_Layout.addWidget(FOV_Window.LabelFOV,0,2,1,3)

        # Number of Sides, Length [m] ------------------------------------------
        Label[1] = QLabel("Number of Sides, Length (m):")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[1],1,1)

        FOV_Window.NumSide = QLineEdit()
        FOV_Window.LengthSide = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FOV_Window.NumSide)
        SubLayout.addWidget(FOV_Window.LengthSide)
        FOV_Layout.addLayout(SubLayout,1,2,1,3)

        # H Width, V Height [deg] ----------------------------------------------
        Label[2] = QLabel("H Width, V Height (deg):")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[2],2,1)

        FOV_Window.Width = QLineEdit()
        FOV_Window.Height = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FOV_Window.Width)
        SubLayout.addWidget(FOV_Window.Height)
        FOV_Layout.addLayout(SubLayout,2,2,1,3)

        # Color RGB+Alpha ------------------------------------------------------
        Label[3] = QLabel("Color RGB+Alpha:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[3],3,1)

        FOV_Window.RedColor= QLineEdit()
        FOV_Window.GreenColor = QLineEdit()
        FOV_Window.BlueColor = QLineEdit()
        FOV_Window.AlphaColor = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FOV_Window.RedColor)
        SubLayout.addWidget(FOV_Window.GreenColor)
        SubLayout.addWidget(FOV_Window.BlueColor)
        SubLayout.addWidget(FOV_Window.AlphaColor)
        FOV_Layout.addLayout(SubLayout,3,2,1,3)

        # Type -----------------------------------------------------------------
        Label[4] = QLabel("FOV Type:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[4],4,1)

        FOV_Layout.ModeFOV = QComboBox()
        FOV_Layout.ModeFOV.addItems(["WIREFRAME", "SOLID", "VECTOR", "PLANE"])
        FOV_Layout.addWidget(FOV_Layout.ModeFOV,4,2)

        # Draw Field -----------------------------------------------------------
        Label[4] = QLabel("Draw Field:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[4],5,1)

        FOV_Window.NearDrawField = QRadioButton('Near')
        FOV_Window.NearDrawField.setChecked(False)

        FOV_Window.FarDrawField = QRadioButton('Far')
        FOV_Window.FarDrawField.setChecked(False)

        FOV_Window.DrawFieldGroup = QButtonGroup()
        FOV_Window.DrawFieldGroup.setExclusive(False)
        FOV_Window.DrawFieldGroup.addButton(FOV_Window.NearDrawField)
        FOV_Window.DrawFieldGroup.addButton(FOV_Window.FarDrawField)

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FOV_Window.NearDrawField)
        SubLayout.addWidget(FOV_Window.FarDrawField)
        FOV_Layout.addLayout(SubLayout,5,2)

        # SC, Body -------------------------------------------------------------
        Label[5] = QLabel("SC, Body:")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[5],6,1)

        FOV_Window.SC = QLineEdit()
        FOV_Window.Body = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FOV_Window.SC)
        SubLayout.addWidget(FOV_Window.Body)
        FOV_Layout.addLayout(SubLayout,6,2,1,3)

        # Position in Body [m] -------------------------------------------------
        Label[6] = QLabel("Position in Body (m):")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[6],7,1)

        FOV_Window.X_Axis_Body = QLineEdit()
        FOV_Window.Y_Axis_Body = QLineEdit()
        FOV_Window.Z_Axis_Body = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FOV_Window.X_Axis_Body)
        SubLayout.addWidget(FOV_Window.Y_Axis_Body)
        SubLayout.addWidget(FOV_Window.Z_Axis_Body)
        FOV_Layout.addLayout(SubLayout,7,2,1,3)

        # Euler Angles [deg], Sequence -----------------------------------------
        Label[7] = QLabel("Euler Angls (deg), Sequence:")
        Label[7].setAlignment(Qt.AlignLeft)
        Label[7].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[7],8,1)

        FOV_Window.FirstSequence = QLineEdit()
        FOV_Window.SecondSequence = QLineEdit()
        FOV_Window.ThirdSequence  = QLineEdit()
        FOV_Window.Sequence = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FOV_Window.FirstSequence)
        SubLayout.addWidget(FOV_Window.SecondSequence)
        SubLayout.addWidget(FOV_Window.ThirdSequence)
        SubLayout.addWidget(FOV_Window.Sequence)
        FOV_Layout.addLayout(SubLayout,8,2,1,3)

        # Boresight Axis -------------------------------------------------------
        Label[8] = QLabel("Boresight Axis:")
        Label[8].setAlignment(Qt.AlignLeft)
        Label[8].setStyleSheet('font-size: 15px')
        FOV_Layout.addWidget(Label[8],9,1)

        FOV_Layout.BoresightAxis = QComboBox()
        FOV_Layout.BoresightAxis.addItems(["X_AXIS", "Y_AXIS", "Z_AXIS"])
        FOV_Layout.addWidget(FOV_Layout.BoresightAxis,9,2)

        # set Layout
        FOV_Window.Stack.widget(indexStack).setLayout(FOV_Layout)

    # def DefaultWriteSlot(FOV_Window):
    #     FOV_Window.Inp_Sim_data = FOV_Window.ReadFile.readlines()
    #     FOV_Window.LabelFOV.currentText() = FOV_Window.Inp_Sim_data[3]
    #     FOV_Window.NumSide.currentText() = FOV_Window.Inp_Sim_data[3]
    #     FOV_Window.LengthSide.currentText() =
    #     FOV_Window.Width.currentText() =
    #     FOV_Window.Height.currentText() =
    #     FOV_Window.RedColor.currentText() =
    #     FOV_Window.GreenColor.currentText() =
    #     FOV_Window.AlphaColor.currentText() =
    #     FOV_Window.SC.currentText() =
    #     FOV_Window.Body.currentText() =
    #     FOV_Window.X_Axis_Body.currentText() =
    #     FOV_Window.Y_Axis_Body.currentText() =
    #     FOV_Window.Z_Axis_Body.currentText() =
    #     FOV_Window.FirstSequence.currentText() =
    #     FOV_Window.SecondSequence.currentText() =
    #     FOV_Window.ThirdSequence.currentText() =
    #     FOV_Window.Sequence.currentText() =
    #
    #     if
    #     FOV_Window.BoresightAxis.setCurrentIndex()

    def display(FOV_Window,i):
        FOV_Window.Stack.setCurrentIndex(i)


    # Slot Functions------------------------------------------------------------
    # def GraphicsSlot(FOV_Window):
    #     if FOV_Window.GraphicsOn.isChecked():
    #         FOV_Window.Inp_FOV_data[5] = "TRUE                            !  Graphics Front End? \n"
    #     elif FOV_Window.GraphicsOff.isChecked():
    #         FOV_Window.Inp_FOV_data[5] = "FALSE                           !  Graphics Front End? \n"
    #
    # def ModeFOV_Slot(FOV_Window):
    #     if FOV_Window.ModeFOV.currentText() == "WIREFRAME":
    #         FOV_Window.Inp_FOV_data[2] = "WIREFRAME                        !  WIREFRAME, SOLID, VECTOR, or PLANE"
    #     elif FOV_Window.ModeFOV.currentText() == "SOLID":
    #         FOV_Window.Inp_FOV_data[2] = "SOLID                            !  WIREFRAME, SOLID, VECTOR, or PLANE"
    #     elif FOV_Window.ModeFOV.currentText() == "VECTOR":
    #         FOV_Window.Inp_FOV_data[2] = "VECTOR                           !  WIREFRAME, SOLID, VECTOR, or PLANE"
    #     elif FOV_Window.ModeFOV.currentText() == "PLANE":
    #         FOV_Window.Inp_FOV_data[2] = "PLANE                            !  WIREFRAME, SOLID, VECTOR, or PLANE"
    #
    # def BoresightAxisSlot(FOV_Window):
    #     if FOV_Window.TimeMode.currentText() == "FAST":
    #         FOV_Window.Inp_FOV_data[2] = "FAST                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)"
    #     elif FOV_Window.TimeMode.currentText() == "REAL":
    #         FOV_Window.Inp_FOV_data[2] = "REAL                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)"
    #     elif FOV_Window.TimeMode.currentText() == "EXTERNAL":
    #         FOV_Window.Inp_FOV_data[2] = "EXTERNAL                        !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)"
    #
    # def WriteFileSlot(FOV_Window):
    #     # Write inputs to txt Write File
    #     FOV_Window.WriteFile = open('InOut/Inp_Sim.txt', 'w')
    #     FOV_Window.WriteFile.writelines(FOV_Window.Inp_FOV_data)
    #
    # def DefaultWriteSlot(FOV_Window):
    #     # Reset Write File to Default/readfile
    #     FOV_Window.Inp_FOV_data = FOV_Window.ReadFile.readlines()
    #
    #     # Reset GUI selections
    #     FOV_Window.GraphicsOn.setChecked(True)
    #     FOV_Window.GraphicsOff.setChecked(False)
    #     #FOV_Window.SimDuration.setText(30000.0)
