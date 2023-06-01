    # This file contains the code for the FOV window in the 42 gui

# Import Packages---------------------------------------------------------------
import sys
import re
import numpy as np
import functions
# Import QApplication and the required widgets from PyQt5.QtWidgets
from PyQt5.QtWidgets import QApplication
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtWidgets import QWidget
from PyQt5.QtCore import Qt
from PyQt5.QtCore import pyqtSignal
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
from PyQt5.QtWidgets import QMessageBox

class FOV_Window(QWidget):

    def __init__(FovWindow, *args, **kwargs):
        # Set Up Window
        super().__init__(*args, **kwargs)
        FovWindow.setWindowTitle('Fields of View Menu') # sets the window’s title
        FovWindow.setFixedSize(600, 500) # gives window a fixed size

        FovWindow.missionDir = functions.get_mission_directory(FovWindow)
        FovWindow.inputFile = FovWindow.missionDir + '/InOut/Inp_FOV.txt'
        FovWindow.defaultFile = FovWindow.missionDir + '/InOut/Default/Inp_FOV.txt'

        FovWindow.InpFovData = FovWindow.read_in()
        FovWindow.defaultData = FovWindow.read_in()
        FovWindow.initInpDI = FovWindow.get_data_input(FovWindow.inputFile)

        FovWindow.init_fov_window_ui()

        # set default item shown on startup
        FovWindow.ListFov.setCurrentRow(0)

    # FOV Window Initialization Functions---------------------------------------
    def init_fov_window_ui(FovWindow):
        inpDI = FovWindow.initInpDI
        FovLayout = QGridLayout()

        ListFovLabel = QLabel("FOV(s)")
        ListFovLabel.setAlignment(Qt.AlignLeft)
        ListFovLabel.setStyleSheet('font-size: 15px')

        FovWindow.ListFov = QListWidget()

        for x in range(FovWindow.Num_FOV_Profiles):
            FovWindow.ListFov.insertItem(x,"FOV %s"%x)

        FovWindow.AddFov = QPushButton('+')
        FovWindow.RemoveFov = QPushButton('-')

        hBoxAddRemove = QHBoxLayout()
        hBoxAddRemove.addWidget(FovWindow.RemoveFov)
        hBoxAddRemove.addWidget(FovWindow.AddFov)
        vBoxLeft = QVBoxLayout()
        vBoxLeft.addWidget(ListFovLabel)
        vBoxLeft.addWidget(FovWindow.ListFov)
        vBoxLeft.addLayout(hBoxAddRemove)
        FovLayout.addLayout(vBoxLeft,0,0)

        # Apply / Cancel / Reset Default Button---------------------------------
        FovWindow.ApplyBtn = QPushButton('Apply')
        FovWindow.CancelBtn = QPushButton('Cancel')
        FovWindow.ResetBtn = QPushButton('Reset to Default')

        FovWindow.AddFov.clicked.connect(FovWindow.add_fov_slot)
        FovWindow.RemoveFov.clicked.connect(FovWindow.remove_fov_slot)
        FovWindow.AddFov.clicked.connect(FovWindow.number_fov_slot)
        FovWindow.RemoveFov.clicked.connect(FovWindow.number_fov_slot)
        FovWindow.ApplyBtn.clicked.connect(FovWindow.empty_widget_flag)
        FovWindow.ApplyBtn.clicked.connect(FovWindow.write_file_slot)
        FovWindow.ResetBtn.clicked.connect(FovWindow.default_write_slot)

        hboxBot = QHBoxLayout()
        hboxBot.addWidget(FovWindow.ApplyBtn,2)
        hboxBot.addWidget(FovWindow.CancelBtn,1)
        hboxBot.addWidget(FovWindow.ResetBtn,0)
        FovLayout.addLayout(hboxBot,1,1)

        # FOV Properties -------------------------------------------------------
        FovWindow.FovProperties = QStackedWidget()

        for x in range(FovWindow.Num_FOV_Profiles):
            FovWindow.init_stack_ui(x)
            FovWindow.init_widget_field(x,inpDI)

        # Finialize FOV window
        FovLayout.addWidget(FovWindow.FovProperties,0,1)
        FovWindow.setLayout(FovLayout)
        FovWindow.ListFov.currentRowChanged.connect(FovWindow.display)
        FovWindow.ListFov.currentRowChanged.connect(FovWindow.signal_to_slot)

    def init_stack_ui(FovWindow,indexStack):

        FovWindow.FovStack = QWidget()
        FovStackLayout = QGridLayout()
        Label = [0]*9

        # Label ----------------------------------------------------------------
        Label[0] = QLabel("Label:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[0],0,1)

        FovWindow.FovStack.LabelFov = QLineEdit()

        FovStackLayout.addWidget(FovWindow.FovStack.LabelFov,0,2,1,3)

        # Number of Sides, Length [m] ------------------------------------------
        Label[1] = QLabel("Number of Sides, Length (m):")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[1],1,1)

        FovWindow.FovStack.NumSide = QLineEdit()
        FovWindow.FovStack.LengthSide = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FovWindow.FovStack.NumSide)
        SubLayout.addWidget(FovWindow.FovStack.LengthSide)
        FovStackLayout.addLayout(SubLayout,1,2,1,3)

        # H Width, V Height [deg] ----------------------------------------------
        Label[2] = QLabel("H Width, V Height (deg):")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[2],2,1)

        FovWindow.FovStack.Width = QLineEdit()
        FovWindow.FovStack.Height = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FovWindow.FovStack.Width)
        SubLayout.addWidget(FovWindow.FovStack.Height)
        FovStackLayout.addLayout(SubLayout,2,2,1,3)

        # Color RGB+Alpha ------------------------------------------------------
        Label[3] = QLabel("Color RGB+Alpha:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[3],3,1)

        FovWindow.FovStack.RedColor= QLineEdit()
        FovWindow.FovStack.GreenColor = QLineEdit()
        FovWindow.FovStack.BlueColor = QLineEdit()
        FovWindow.FovStack.AlphaColor = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FovWindow.FovStack.RedColor)
        SubLayout.addWidget(FovWindow.FovStack.GreenColor)
        SubLayout.addWidget(FovWindow.FovStack.BlueColor)
        SubLayout.addWidget(FovWindow.FovStack.AlphaColor)
        FovStackLayout.addLayout(SubLayout,3,2,1,3)

        # Type -----------------------------------------------------------------
        Label[4] = QLabel("FOV Type:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[4],4,1)

        FovWindow.FovStack.ModeFOV = QComboBox()
        FovWindow.FovStack.ModeFOV.addItems(["WIREFRAME", "SOLID", "VECTOR", "PLANE"])
        FovWindow.FovStack.ModeFOV.setCurrentIndex(0)
        
        FovStackLayout.addWidget(FovWindow.FovStack.ModeFOV,4,2)

        # Draw Field -----------------------------------------------------------
        Label[4] = QLabel("Draw Field:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[4],5,1)

        FovWindow.FovStack.NearDrawField = QRadioButton('Near')
        FovWindow.FovStack.FarDrawField = QRadioButton('Far')
        FovWindow.FovStack.NearDrawField.setChecked(False)
        FovWindow.FovStack.FarDrawField.setChecked(False)

        FovWindow.FovStack.DrawFieldGroup = QButtonGroup()
        FovWindow.FovStack.DrawFieldGroup.setExclusive(False)
        FovWindow.FovStack.DrawFieldGroup.addButton(FovWindow.FovStack.NearDrawField)
        FovWindow.FovStack.DrawFieldGroup.addButton(FovWindow.FovStack.FarDrawField)

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FovWindow.FovStack.NearDrawField)
        SubLayout.addWidget(FovWindow.FovStack.FarDrawField)
        FovStackLayout.addLayout(SubLayout,5,2)

        # SC, Body -------------------------------------------------------------
        Label[5] = QLabel("SC, Body:")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[5],6,1)

        FovWindow.FovStack.SC = QLineEdit()
        FovWindow.FovStack.Body = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FovWindow.FovStack.SC)
        SubLayout.addWidget(FovWindow.FovStack.Body)
        FovStackLayout.addLayout(SubLayout,6,2,1,3)

        # Position in Body [m] -------------------------------------------------
        Label[6] = QLabel("Position in Body (m):")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[6],7,1)

        FovWindow.FovStack.X_Axis_Body = QLineEdit()
        FovWindow.FovStack.Y_Axis_Body = QLineEdit()
        FovWindow.FovStack.Z_Axis_Body = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FovWindow.FovStack.X_Axis_Body)
        SubLayout.addWidget(FovWindow.FovStack.Y_Axis_Body)
        SubLayout.addWidget(FovWindow.FovStack.Z_Axis_Body)
        FovStackLayout.addLayout(SubLayout,7,2,1,3)

        # Euler Angles [deg], Sequence -----------------------------------------
        Label[7] = QLabel("Euler Angls (deg), Sequence:")
        Label[7].setAlignment(Qt.AlignLeft)
        Label[7].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[7],8,1)

        FovWindow.FovStack.FirstSequence = QLineEdit()
        FovWindow.FovStack.SecondSequence = QLineEdit()
        FovWindow.FovStack.ThirdSequence = QLineEdit()
        FovWindow.FovStack.Sequence = QComboBox()

        FovWindow.FovStack.Sequence.addItems(['1','2','3','12','13','21','23','31',
        '32','123','231','312','132','213','321','121','131','212','232','313','323'])
        FovWindow.FovStack.Sequence.setCurrentIndex(0)

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(FovWindow.FovStack.FirstSequence)
        SubLayout.addWidget(FovWindow.FovStack.SecondSequence)
        SubLayout.addWidget(FovWindow.FovStack.ThirdSequence)
        SubLayout.addWidget(FovWindow.FovStack.Sequence)
        FovStackLayout.addLayout(SubLayout,8,2,1,3)

        # Boresight Axis -------------------------------------------------------
        Label[8] = QLabel("Boresight Axis:")
        Label[8].setAlignment(Qt.AlignLeft)
        Label[8].setStyleSheet('font-size: 15px')
        FovStackLayout.addWidget(Label[8],9,1)

        FovWindow.FovStack.BoresightAxis = QComboBox()
        FovWindow.FovStack.BoresightAxis.addItems(["X_AXIS", "Y_AXIS", "Z_AXIS"])
        FovWindow.FovStack.BoresightAxis.setCurrentIndex(0)

        FovStackLayout.addWidget(FovWindow.FovStack.BoresightAxis,9,2)

        # Set stack layout------------------------------------------------------
        FovWindow.FovStack.setLayout(FovStackLayout)
        FovWindow.FovProperties.addWidget(FovWindow.FovStack)

    def init_widget_field(self, indexStack,inpDI):

        self.FovProperties.widget(indexStack).LabelFov.setText("%s"%inpDI[3][indexStack])

        self.FovProperties.widget(indexStack).NumSide.setText("%s"%inpDI[1][indexStack][0])
        self.FovProperties.widget(indexStack).LengthSide.setText("%s"%inpDI[1][indexStack][1])

        self.FovProperties.widget(indexStack).Width.setText("%s"%inpDI[1][indexStack][2])
        self.FovProperties.widget(indexStack).Height.setText("%s"%inpDI[1][indexStack][3])

        self.FovProperties.widget(indexStack).RedColor.setText("%s"%inpDI[1][indexStack][4])
        self.FovProperties.widget(indexStack).GreenColor.setText("%s"%inpDI[1][indexStack][5])
        self.FovProperties.widget(indexStack).BlueColor.setText("%s"%inpDI[1][indexStack][6])
        self.FovProperties.widget(indexStack).AlphaColor.setText("%s"%inpDI[1][indexStack][7])

        if (inpDI[4][indexStack].lower() == "wireframe"):
            self.FovProperties.widget(indexStack).ModeFOV.setCurrentIndex(0)
        elif (inpDI[4][indexStack].lower() == "solid"):
            self.FovProperties.widget(indexStack).ModeFOV.setCurrentIndex(1)
        elif (inpDI[4][indexStack].lower() == "vector"):
            self.FovProperties.widget(indexStack).ModeFOV.setCurrentIndex(2)
        elif (inpDI[4][indexStack].lower() == "plane"):
            self.FovProperties.widget(indexStack).ModeFOV.setCurrentIndex(3)

        if (inpDI[2][indexStack][0].lower() == 'true'):
            self.FovProperties.widget(indexStack).NearDrawField.setChecked(True)
        if (inpDI[2][indexStack][1].lower() == 'true'):
            self.FovProperties.widget(indexStack).FarDrawField.setChecked(True)

        self.FovProperties.widget(indexStack).SC.setText("%s"%inpDI[1][indexStack][8])
        self.FovProperties.widget(indexStack).Body.setText("%s"%inpDI[1][indexStack][9])

        self.FovProperties.widget(indexStack).X_Axis_Body.setText("%s"%inpDI[1][indexStack][10])
        self.FovProperties.widget(indexStack).Y_Axis_Body.setText("%s"%inpDI[1][indexStack][11])
        self.FovProperties.widget(indexStack).Z_Axis_Body.setText("%s"%inpDI[1][indexStack][12])

        self.FovProperties.widget(indexStack).FirstSequence.setText("%s"%inpDI[1][indexStack][13])
        self.FovProperties.widget(indexStack).SecondSequence.setText("%s"%inpDI[1][indexStack][14])
        self.FovProperties.widget(indexStack).ThirdSequence.setText("%s"%inpDI[1][indexStack][15])
        itemIndex = self.FovProperties.widget(indexStack).Sequence.findText(inpDI[1][indexStack][16])
        self.FovProperties.widget(indexStack).Sequence.setCurrentIndex(itemIndex)

        if (inpDI[5][indexStack].lower() == "x_axis"):
            self.FovProperties.widget(indexStack).BoresightAxis.setCurrentIndex(0)
        elif (inpDI[5][indexStack].lower() == "y_axis"):
            self.FovProperties.widget(indexStack).BoresightAxis.setCurrentIndex(1)
        elif (inpDI[5][indexStack].lower() == "z_axis"):
            self.FovProperties.widget(indexStack).BoresightAxis.setCurrentIndex(2)

    def display(self,i):
        self.FovProperties.setCurrentIndex(i)

    def read_in(self):
        self.ReadFile = open('InOut/Inp_FOV.txt', 'r')
        self.InpFovData = self.ReadFile.readlines()
        return self.InpFovData

    def get_data_input(self,fileName):
        dataStr = []
        dataNum = []
        dataBool = []
        dataFOVType = []
        dataBoresightAxis = []

        # regex for Inp_FOV parse
        patternLabel = re.compile('"(.*?)"')
        patternFloat = re.compile('[+-]?([0-9]*[.])?[0-9]+')
        patternBool = re.compile ('(TRUE|FALSE)+',re.IGNORECASE)
        patternFOVType = re.compile(r"(WIREFRAME|SOLID|VECTOR|PLANE)*\s*!\s*WIREFRAME, SOLID, VECTOR, or PLANE",re.IGNORECASE)
        patternBoresightAxis = re.compile(r"(X_AXIS|Y_AXIS|Z_AXIS)*\s*!\s*Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS",re.IGNORECASE)

        for i, line in enumerate(open(fileName)):
            for match in re.finditer(patternLabel, line):
                dataStr.append(match.group(1))
            for match in re.finditer(patternFloat, line):
                dataNum.append(match.group(0))
            for match in re.finditer(patternBool, line):
                dataBool.append(match.group(1))
            for match in re.finditer(patternFOVType, line):
                dataFOVType.append(match.group(1))
            for match in re.finditer(patternBoresightAxis, line):
                dataBoresightAxis.append(match.group(1))

        # data clean up for handling
        self.Num_FOV_Profiles = int(dataNum[0])
        self.defaultNumFovProfiles = int(dataNum[0])
        dataNum.remove(dataNum[0])
        dataBool = np.array(dataBool).reshape(self.Num_FOV_Profiles,2)
        dataNum = np.array(dataNum).reshape(self.Num_FOV_Profiles,(len(dataNum)//self.Num_FOV_Profiles))

        return self.Num_FOV_Profiles, dataNum, dataBool, dataStr, dataFOVType, dataBoresightAxis

    def signal_to_slot(self,indexStack):
        self.FovProperties.widget(indexStack).LabelFov.textEdited.connect(lambda:self.label_fov_slot(indexStack))
        self.FovProperties.widget(indexStack).NumSide.textEdited.connect(lambda:self.side_num_length_slot(indexStack))
        self.FovProperties.widget(indexStack).LengthSide.textEdited.connect(lambda:self.side_num_length_slot(indexStack))
        self.FovProperties.widget(indexStack).Width.textEdited.connect(lambda:self.width_height_slot(indexStack))
        self.FovProperties.widget(indexStack).Height.textEdited.connect(lambda:self.width_height_slot(indexStack))
        self.FovProperties.widget(indexStack).RedColor.textEdited.connect(lambda:self.color_slot(indexStack))
        self.FovProperties.widget(indexStack).GreenColor.textEdited.connect(lambda:self.color_slot(indexStack))
        self.FovProperties.widget(indexStack).BlueColor.textEdited.connect(lambda:self.color_slot(indexStack))
        self.FovProperties.widget(indexStack).AlphaColor.textEdited.connect(lambda:self.color_slot(indexStack))
        self.FovProperties.widget(indexStack).ModeFOV.currentTextChanged.connect(lambda:self.fov_type_slot(indexStack))
        self.FovProperties.widget(indexStack).NearDrawField.toggled.connect(lambda:self.draw_field_slot(indexStack))
        self.FovProperties.widget(indexStack).FarDrawField.toggled.connect(lambda:self.draw_field_slot(indexStack))
        self.FovProperties.widget(indexStack).SC.textEdited.connect(lambda:self.sc_body_slot(indexStack))
        self.FovProperties.widget(indexStack).Body.textEdited.connect(lambda:self.sc_body_slot(indexStack))
        self.FovProperties.widget(indexStack).X_Axis_Body.textEdited.connect(lambda:self.position_body_slot(indexStack))
        self.FovProperties.widget(indexStack).Y_Axis_Body.textEdited.connect(lambda:self.position_body_slot(indexStack))
        self.FovProperties.widget(indexStack).Z_Axis_Body.textEdited.connect(lambda:self.position_body_slot(indexStack))
        self.FovProperties.widget(indexStack).FirstSequence.textEdited.connect(lambda:self.euler_sequence_slot(indexStack))
        self.FovProperties.widget(indexStack).SecondSequence.textEdited.connect(lambda:self.euler_sequence_slot(indexStack))
        self.FovProperties.widget(indexStack).ThirdSequence.textEdited.connect(lambda:self.euler_sequence_slot(indexStack))
        self.FovProperties.widget(indexStack).Sequence.currentTextChanged.connect(lambda:self.euler_sequence_slot(indexStack))
        self.FovProperties.widget(indexStack).BoresightAxis.currentTextChanged.connect(lambda:self.boresight_axis_slot(indexStack))
        self.ApplyBtn.clicked.connect(self.write_file_slot)
        self.ResetBtn.clicked.connect(self.default_write_slot)
# ------------------------------------------------------------------------------
# Slot Functions----------------------------------------------------------------
    def write_file_slot(self): # Write edited input txt data to output txt file
        # Write inputs to txt Write File
        if not self.emptyWidgetFlag:
            self.WriteFile = open(self.inputFile, 'w+')
            self.WriteFile.writelines(self.InpFovData)
            self.close

    def default_write_slot(self):
        self.WriteFile = open(self.defaultFile, 'w+')
        self.WriteFile.writelines(self.defaultData)
        self.close

    def empty_widget_flag(self):
        widgets = ["LabelFov","NumSide","LengthSide","Width","Height",
        "RedColor","GreenColor","BlueColor","AlphaColor","SC","Body",
        "X_Axis_Body","Y_Axis_Body","Z_Axis_Body","FirstSequence","ThirdSequence","Sequence"]

        self.emptyWidgetFlag = False

        for x in range(self.Num_FOV_Profiles):
            for widget in widgets:
                if not getattr(self.FovProperties.widget(x),widget).text():
                    errorMsg = QMessageBox()
                    errorMsg.setText("Error: Fill out empty entries before applying!")
                    errorMsg.exec_()
                    self.emptyWidgetFlag = True
                    return
            if self.emptyWidgetFlag:return

    def add_fov_slot(self):
        # update fov list
        ItemIndex = self.ListFov.count()
        ItemLabel = ItemIndex
        self.ListFov.insertItem(int(ItemIndex), "FOV %s"%ItemLabel)

        # update numer of fov
        self.Num_FOV_Profiles = self.ListFov.count()

        # update stacks
        NewStackIndex = ItemIndex - 1
        self.init_stack_ui(NewStackIndex)

        # buffer for input file
        for x in range(0,11):
            self.InpFovData.append("")

        # default new stack widget writes
        self.InpFovData[7 + (11*ItemIndex)] = "WIREFRAME                         !  WIREFRAME, SOLID, VECTOR, or PLANE\n"
        self.InpFovData[8 + (11*ItemIndex)] = "FALSE   FALSE                         !  Draw Near Field, Draw Far Field\n"
        self.InpFovData[12 + (11*ItemIndex)] = "X_AXIS                            !  Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n"

    def remove_fov_slot(self):
        ItemIndex = self.ListFov.count()
        if ItemIndex > 1:
            self.ListFov.takeItem(ItemIndex - 1)
            self.Num_FOV_Profiles = self.ListFov.count()
            self.FovProperties.removeWidget(self.FovProperties.widget(ItemIndex - 1))
            for x in range(11):
                self.InpFovData.pop()

    def number_fov_slot(self):
        self.InpFovData[1] = str(self.ListFov.count()) + "                                 !  Number of FOVs\n"

    def label_fov_slot(self,stackIndex):
        self.InpFovData[2 + (11*stackIndex)] = "--------------------------------------------------------------------\n"
        if (self.FovProperties.widget(stackIndex).LabelFov.text()):
            self.InpFovData[3 + (11*stackIndex)] = '"' + self.FovProperties.widget(stackIndex).LabelFov.text() + '"                           !  Label\n'
        else:
            self.InpFovData[3 + (11*stackIndex)] = '"' + "FOV %s"%stackIndex + '"                           !  Label\n'

    def side_num_length_slot(self,stackIndex):
         self.InpFovData[4 + (11*stackIndex)] = self.FovProperties.widget(stackIndex).NumSide.text() + "   " + self.FovProperties.widget(stackIndex).LengthSide.text() + "                           !  Number of Sides, Length [m]\n"

    def width_height_slot(self,stackIndex):
         self.InpFovData[5 + (11*stackIndex)] = self.FovProperties.widget(stackIndex).Width.text() + "  " + self.FovProperties.widget(stackIndex).Height.text() + "                          !  H Width, V Height [deg]\n"

    def color_slot(self,stackIndex):
         stack = self.FovProperties.widget(stackIndex)
         self.InpFovData[6 + (11*stackIndex)] = stack.RedColor.text() + " " + stack.GreenColor.text() + " " + stack.BlueColor.text() + " " + stack.AlphaColor.text() + "                   !  H Width, V Height [deg]\n"

    def fov_type_slot(self,stackIndex):
        if self.FovProperties.widget(stackIndex).ModeFOV.currentText().lower() == "wireframe":
            self.InpFovData[7 + (11*stackIndex)] = "WIREFRAME                         !  WIREFRAME, SOLID, VECTOR, or PLANE\n"
        elif self.FovProperties.widget(stackIndex).ModeFOV.currentText().lower() == "solid":
            self.InpFovData[7 + (11*stackIndex)] = "SOLID                             !  WIREFRAME, SOLID, VECTOR, or PLANE\n"
        elif self.FovProperties.widget(stackIndex).ModeFOV.currentText().lower() == "vector":
            self.InpFovData[7 + (11*stackIndex)] = "VECTOR                            !  WIREFRAME, SOLID, VECTOR, or PLANE\n"
        elif self.FovProperties.widget(stackIndex).ModeFOV.currentText().lower() == "plane":
            self.InpFovData[7 + (11*stackIndex)] = "PLANE                             !  WIREFRAME, SOLID, VECTOR, or PLANE\n"

    def draw_field_slot(self,stackIndex):
        if self.FovProperties.widget(stackIndex).NearDrawField.isChecked():
            NDF = "TRUE"
        else:
            NDF = "FALSE"

        if self.FovProperties.widget(stackIndex).FarDrawField.isChecked():
            FDF = "TRUE"
        else:
            FDF = "FALSE"

        self.InpFovData[8 + (11*stackIndex)] = NDF + "  " + FDF + "                        !  Draw Near Field, Draw Far Field\n"

    def sc_body_slot(self,stackIndex):
         self.InpFovData[9 + (11*stackIndex)] = self.FovProperties.widget(stackIndex).SC.text() + "  " + self.FovProperties.widget(stackIndex).Body.text() + "                              !  SC, Body\n"

    def position_body_slot(self,stackIndex):
         stack = self.FovProperties.widget(stackIndex)
         self.InpFovData[10 + (11*stackIndex)] = stack.X_Axis_Body.text() + " " + stack.Y_Axis_Body.text() + " " + stack.Z_Axis_Body.text() + "                     !  Position in Body [m]\n"

    def euler_sequence_slot(self,stackIndex):
         stack = self.FovProperties.widget(stackIndex)
         self.InpFovData[11 + (11*stackIndex)] = stack.FirstSequence.text() + "  " + stack.SecondSequence.text() + "  " + stack.ThirdSequence.text() + "  " + stack.Sequence.currentText() + "                !  Euler Angles [deg], Sequence\n"

    def boresight_axis_slot(self,stackIndex):
        if self.FovProperties.widget(stackIndex).BoresightAxis.currentText().lower() == "x_axis":
            self.InpFovData[12 + (11*stackIndex)] = "X_AXIS                            !  Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n"
        elif self.FovProperties.widget(stackIndex).BoresightAxis.currentText().lower() == "y_axis":
            self.InpFovData[12 + (11*stackIndex)] = "Y_AXIS                            !  Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n"
        elif self.FovProperties.widget(stackIndex).BoresightAxis.currentText().lower() == "z_axis":
            self.InpFovData[12 + (11*stackIndex)] = "Z_AXIS                            !  Boresight Axis X_AXIS, Y_AXIS, or Z_AXIS\n"
        # self.InpFovData[13 + (11*stackIndex)] = "--------------------------------------------------------------------\n"
