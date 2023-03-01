# This file contains the code for the Graphics window in the 42 gui

# Import Packages---------------------------------------------------------------
import functions
import re
import os
import shutil

# Import QApplication and the required widgets from PySide6
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QMainWindow
from PySide6.QtWidgets import QWidget
from PySide6.QtWidgets import QLabel
from PySide6.QtWidgets import QGridLayout # to arrange the buttons
from PySide6.QtWidgets import QLineEdit # for the display
from PySide6.QtWidgets import QComboBox # drop down box
from PySide6.QtWidgets import QVBoxLayout # verticle stack layout
from PySide6.QtWidgets import QHBoxLayout # Horizontal stack layout
from PySide6.QtWidgets import QRadioButton
from PySide6.QtWidgets import QButtonGroup
from PySide6.QtWidgets import QTabWidget
from PySide6.QtWidgets import QPushButton

class GraphicsWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('42 Graphics Configuration') # sets the window’s title
        self.setFixedSize(525, 700) # gives window a fixed size

        self.table_widget = GraphicsTable(self)
        self.setCentralWidget(self.table_widget)

class GraphicsTable(QWidget):
    def __init__(self, parent):
        super().__init__(parent)
        self.layout = QVBoxLayout(self)

        # get data for read/write
        self.missionDir = functions.get_mission_directory(self)
        self.inputFile = self.missionDir + '/InOut/Inp_Graphics.txt'
        self.read_input(self.inputFile)
        self.get_input_data(self.inputFile)

        # initialize tab screen
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tab3 = QWidget()
        self.tab4 = QWidget()
        self.tab5 = QWidget()

        # Add tabs
        self.tabs.addTab(self.tab1,'General')
        self.tabs.addTab(self.tab2,'POV')
        self.tabs.addTab(self.tab3,'CAM')
        self.tabs.addTab(self.tab4,'MAP')
        self.tabs.addTab(self.tab5,'Unit Sphere')

        # General tab ----------------------------------------------------------
        tab1_layout = self.tab1.layout
        tab1_layout = QGridLayout(self)

        GeneralLabelList = [
        'GL Output Intervel [sec]:','Star Catalog File Name:','Map Window:',
        'Orrery Window:','Unit Sphere Window:'
        ]

        tab1_layout.addLayout(self.generate_label_widgets(GeneralLabelList),0,0)

        self.GlOutInterval = QLineEdit()
        self.StarCatalogName = QLineEdit()
        self.MapWindowOn = QRadioButton('On')
        self.MapWindowOff = QRadioButton('Off')
        self.OrreryWindowOn = QRadioButton('On')
        self.OrreryWindowOff = QRadioButton('Off')
        self.UnitSphereWindowOn = QRadioButton('On')
        self.UnitSphereWindowOff = QRadioButton('Off')

        self.MapWindowBtnGroup = QButtonGroup()
        self.OrreryWindowBtnGroup = QButtonGroup()
        self.UnitSphereWindowBtnGroup = QButtonGroup()

        self.MapWindowBtnGroup.addButton(self.MapWindowOn)
        self.MapWindowBtnGroup.addButton(self.MapWindowOff)
        self.OrreryWindowBtnGroup.addButton(self.OrreryWindowOn)
        self.OrreryWindowBtnGroup.addButton(self.OrreryWindowOff)
        self.UnitSphereWindowBtnGroup.addButton(self.UnitSphereWindowOn)
        self.UnitSphereWindowBtnGroup.addButton(self.UnitSphereWindowOff)

        MapWindowSubLayout = QHBoxLayout()
        OrreryWindowSubLayout = QHBoxLayout()
        UnitSphereSubLayout = QHBoxLayout()

        MapWindowSubLayout.addWidget(self.MapWindowOn)
        MapWindowSubLayout.addWidget(self.MapWindowOff)
        OrreryWindowSubLayout.addWidget(self.OrreryWindowOn)
        OrreryWindowSubLayout.addWidget(self.OrreryWindowOff)
        UnitSphereSubLayout.addWidget(self.UnitSphereWindowOn)
        UnitSphereSubLayout.addWidget(self.UnitSphereWindowOff)

        TabWriteBtns = QHBoxLayout()
        self.ApplyBtn = QPushButton('Apply')
        self.CancelBtn = QPushButton('Cancel')
        self.ResetBtn = QPushButton('Reset to Default')
        self.ApplyBtn.clicked.connect(parent.close)
        self.CancelBtn.clicked.connect(parent.close)
        # self.ResetBtn.clicked.connect(self.SetDefaultGuiFields())

        TabWriteBtns.addWidget(self.ResetBtn)
        TabWriteBtns.addWidget(self.CancelBtn)
        TabWriteBtns.addWidget(self.ApplyBtn)

        # add layout in order of label list
        widgetLayout = QVBoxLayout()
        widgetLayout.addWidget(self.GlOutInterval)
        widgetLayout.addWidget(self.StarCatalogName)
        widgetLayout.addLayout(MapWindowSubLayout)
        widgetLayout.addLayout(OrreryWindowSubLayout)
        widgetLayout.addLayout(UnitSphereSubLayout)

        tab1_layout.addLayout(widgetLayout,0,1)
        tab1_layout.addLayout(TabWriteBtns,1,0,1,3)

        self.tab1.setLayout(tab1_layout)

        # POV tab --------------------------------------------------------------
        tab2_layout = self.tab2.layout
        tab2_layout = QGridLayout(self)

        PovLabelList = [
        'Pause at Startup:','POV Mode:','Host Type:','Initial Host (SC, Body, POV Frame):',
        'Target Type:','Initial Target (SC, Body, POV Frame):','Boresight Axis:','Up Axis:',
        'Initial POV Range from Target (m):','POV Angle (Vertical) [deg]',
        'POV Position in Host [m]','Initial POV View:'
        ]

        tab2_layout.addLayout(self.generate_label_widgets(PovLabelList),0,0)

        self.PauseStartupOn = QRadioButton('On')
        self.PauseStartupOff = QRadioButton('Off')
        self.ModePOV = QComboBox()
        self.HostType = QComboBox()
        self.InitialHostSC = QLineEdit()
        self.InitialHostBody = QLineEdit()
        self.InitialHostPOVFrame = QLineEdit()
        self.TargetType = QComboBox()
        self.InitialTargetSC = QLineEdit()
        self.InitialTargetBody = QLineEdit()
        self.InitialTargetPOVFrame = QLineEdit()
        self.BoresightAxis = QLineEdit()
        self.UpAxis = QLineEdit()
        self.InitPovRange = QLineEdit()
        self.VPovAngle = QLineEdit()
        self.PovPosHostFirstEntry = QLineEdit()
        self.PovPosHostSecondEntry = QLineEdit()
        self.PovPosHostThirdEntry = QLineEdit()
        self.InitPovView = QComboBox()

        self.ModePOV.addItems(['TRACK_HOST', 'TRACK_TARGET', 'FIXED_IN_HOST'])
        self.HostType.addItems(['WORLD', 'REFORB', 'FRM', 'SC', 'BODY'])
        self.TargetType.addItems(['WORLD', 'REFORB', 'FRM', 'SC', 'BODY'])
        self.InitPovView.addItems(['FRONT', 'FRONT_RIGHT','FRONT_LEFT','REAR','REAR_LEFT','REAR_RIGHT','UP','DOWN','LEFT','RIGHT'])

        PauseStartSubLayout = QHBoxLayout()
        InitialHostSubLayout = QHBoxLayout()
        InitialTargetSubLayout = QHBoxLayout()
        PovPosHostSubLayout = QHBoxLayout()

        PauseStartSubLayout.addWidget(self.PauseStartupOn)
        PauseStartSubLayout.addWidget(self.PauseStartupOff)
        InitialHostSubLayout.addWidget(self.InitialHostSC)
        InitialHostSubLayout.addWidget(self.InitialHostBody)
        InitialHostSubLayout.addWidget(self.InitialHostPOVFrame)
        InitialTargetSubLayout.addWidget(self.InitialTargetSC)
        InitialTargetSubLayout.addWidget(self.InitialTargetBody)
        InitialTargetSubLayout.addWidget(self.InitialTargetPOVFrame)
        PovPosHostSubLayout.addWidget(self.PovPosHostFirstEntry)
        PovPosHostSubLayout.addWidget(self.PovPosHostSecondEntry)
        PovPosHostSubLayout.addWidget(self.PovPosHostThirdEntry)

        # add layout in order of label list
        widgetLayout = QVBoxLayout()
        widgetLayout.addLayout(PauseStartSubLayout)
        widgetLayout.addWidget(self.ModePOV)
        widgetLayout.addWidget(self.HostType)
        widgetLayout.addLayout(InitialHostSubLayout)
        widgetLayout.addWidget(self.TargetType)
        widgetLayout.addLayout(InitialTargetSubLayout)
        widgetLayout.addWidget(self.BoresightAxis)
        widgetLayout.addWidget(self.UpAxis)
        widgetLayout.addWidget(self.InitPovRange)
        widgetLayout.addWidget(self.VPovAngle)
        widgetLayout.addLayout(PovPosHostSubLayout)
        widgetLayout.addWidget(self.InitPovView)

        tab2_layout.addLayout(widgetLayout,0,1)

        self.tab2.setLayout(tab2_layout)

        # CAM tab --------------------------------------------------------------
        tab3_layout = self.tab3.layout
        tab3_layout = QGridLayout(self)

        SubLayout = QHBoxLayout()
        for label in ['CAM Label','CAM Name','Enabled']:
            Label = QLabel(label)
            Label.setAlignment(Qt.AlignLeft)
            Label.setStyleSheet('font-size: 15px')
            SubLayout.addWidget(Label)

        self.CamTitle = QLineEdit()
        self.CamWidth = QLineEdit()
        self.CamHeight = QLineEdit()
        self.MouseScaleFactor = QLineEdit()
        self.DisplayGammaExponent = QLineEdit()

        CamDimSubLayout = QHBoxLayout()
        CamDimSubLayout.addWidget(self.CamWidth)
        CamDimSubLayout.addWidget(self.CamHeight)

        CamLabelList = [
        'Cam Title:','Width, Height [pixels]','Mouse Scale Factor',"Display's Gamma Exponenet (1.8-4.0)"
        ]
        CamShowMenuLabels = [
        'N Axes','L Axes','F Axes','B Axes','N Grid','L Grid','F Grid','B Grid',
        'G Grid','Field of View','Prov Ops','TDRS Satellites','Shadows',
        'Astro Labels','Truth Vectors','FSW Vectors','Milky Way','Fermi Sky'
        ]

        CamShowMenuExistInputs = self.inputData[21:39]
        CamShowMenuNameInputs = self.inputString[1:19]

        print(CamShowMenuExistInputs)
        print(CamShowMenuNameInputs)

        tab3_layout.addLayout(self.generate_label_widgets(CamLabelList),0,0,4,1)
        tab3_layout.addWidget(self.CamTitle,0,1)
        tab3_layout.addLayout(CamDimSubLayout,1,1)
        tab3_layout.addWidget(self.MouseScaleFactor,2,1)
        tab3_layout.addWidget(self.DisplayGammaExponent,3,1)
        tab3_layout.addLayout(SubLayout,5,0,1,4)
        tab3_layout.addLayout(self.init_show_menu(CamShowMenuLabels,CamShowMenuExistInputs,CamShowMenuNameInputs),6,0,19,2)

        self.tab3.setLayout(tab3_layout)

        # MAP tab --------------------------------------------------------------
        tab4_layout = self.tab4.layout
        tab4_layout = QGridLayout(self)

        SubLayout = QHBoxLayout()
        for label in ['Map Label','Map Name','Enabled']:
            Label = QLabel(label)
            Label.setAlignment(Qt.AlignLeft)
            Label.setStyleSheet('font-size: 15px')
            SubLayout.addWidget(Label)

        self.MapName = QLineEdit()
        self.MapWidth = QLineEdit()
        self.MapHeight = QLineEdit()

        MapDimSubLayout = QHBoxLayout()
        MapDimSubLayout.addWidget(self.MapWidth)
        MapDimSubLayout.addWidget(self.MapHeight)

        widgetLayout = QVBoxLayout()
        widgetLayout.addWidget(self.MapName)
        widgetLayout.addLayout(MapDimSubLayout)

        MapLabelList = ['Map Title:','Width, Height [pixels]:']
        MapShowMenuLabels = ['Clock','Tlm Clock','Credits','Night']

        MapShowMenuExistInputs = self.inputData[41:45]
        MapShowMenuNameInputs = self.inputString[20:24]

        tab4_layout.addLayout(self.generate_label_widgets(MapLabelList),0,0)
        tab4_layout.addLayout(widgetLayout,0,1)
        tab4_layout.addLayout(SubLayout,1,0,1,3)
        tab4_layout.addLayout(self.init_show_menu(MapShowMenuLabels,MapShowMenuExistInputs,MapShowMenuNameInputs),1,0,4,3)

        self.tab4.setLayout(tab4_layout)

        # Unit Sphere tab ------------------------------------------------------
        tab5_layout = self.tab4.layout
        tab5_layout = QGridLayout(self)

        self.MajorConsOn = QRadioButton('On')
        self.MajorConsOff = QRadioButton('Off')
        self.ZodiacConsOn = QRadioButton('On')
        self.ZodiacConsOff = QRadioButton('Off')
        self.MinorConsOn = QRadioButton('On')
        self.MinorConsOff = QRadioButton('Off')

        self.MajorConsBtnGroup = QButtonGroup()
        self.ZodiacConsBtnGroup = QButtonGroup()
        self.MinorConsBtnGroup = QButtonGroup()

        self.MajorConsBtnGroup.addButton(self.MajorConsOn)
        self.MajorConsBtnGroup.addButton(self.MajorConsOff)
        self.ZodiacConsBtnGroup.addButton(self.ZodiacConsOn)
        self.ZodiacConsBtnGroup.addButton(self.ZodiacConsOff)
        self.MinorConsBtnGroup.addButton(self.MinorConsOn)
        self.MinorConsBtnGroup.addButton(self.MinorConsOff)

        MajorConsLayout = QHBoxLayout()
        ZodiacConsLayout = QHBoxLayout()
        MinorConsLayout = QHBoxLayout()

        MajorConsLayout.addWidget(self.MajorConsOn)
        MajorConsLayout.addWidget(self.MajorConsOff)
        ZodiacConsLayout.addWidget(self.ZodiacConsOn)
        ZodiacConsLayout.addWidget(self.ZodiacConsOff)
        MinorConsLayout.addWidget(self.MinorConsOn)
        MinorConsLayout.addWidget(self.MinorConsOff)

        widgetLayout = QVBoxLayout()
        widgetLayout.addLayout(MajorConsLayout)
        widgetLayout.addLayout(ZodiacConsLayout)
        widgetLayout.addLayout(MinorConsLayout)

        UnitSphereLabelList = [
        'Show Major Constellations:','Show Zodiac Constellations:','Show Minor Constellations:'
        ]

        tab5_layout.addLayout(self.generate_label_widgets(UnitSphereLabelList),0,0)
        tab5_layout.addLayout(widgetLayout,0,1)

        self.tab5.setLayout(tab5_layout)

        # Finalize Tabs --------------------------------------------------------
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)

        self.init_widget_field()

        self.signal_to_slot()

# ------------------------------------------------------------------------------
# GraphicsTable Functions-------------------------------------------------------
    def generate_label_widgets(self,labels):
        SubLayout = QVBoxLayout()

        for label in labels:
            Label = QLabel(label)
            Label.setAlignment(Qt.AlignLeft)
            Label.setStyleSheet('font-size: 15px')
            SubLayout.addWidget(Label)

        return SubLayout

    def read_input(self, FileStr):
        self.InpGraphicsData = []
        self.InpGraphicsDefaultData = []

        InpDataFile = open(FileStr)
        for line in InpDataFile:
            self.InpGraphicsData.append(line)
            self.InpGraphicsDefaultData.append(line)

    def SetDefaultGuiFields(self): # Resets gui options and input txt data to default
        # Reset Write File to Default/readfile
        self.read_input(self.defaultFile)
        self.get_input_data(self.defaultFile)
        self.init_widget_field()

    def get_input_data(self, FileStr):
        self.inputData = []
        self.inputString = []

        pattern1 = re.compile("(.*?)\s*!")
        pattern2 = re.compile('"(.*?)"')

        InpDataFile = open(FileStr)
        for i, line in enumerate(InpDataFile):
            for match in re.finditer(pattern1, line):
                self.inputData.append(match.group(1).split())
            for match in re.finditer(pattern2, line):
                self.inputString.append(match.group(1))

    def init_show_menu(self,labels,showMenuData,showInputName):
        Layout = QGridLayout()
        row = 0

        for label in labels:
            RightLayout = QHBoxLayout()

            buffer = QLabel("Show " + label)
            buffer.setAlignment(Qt.AlignLeft)
            buffer.setStyleSheet('font-size: 15px')
            Layout.addWidget(buffer,row,0)

            itemLabel = label.replace(" ","")
            itemOnLabel = label.replace(" ","") + "On"
            itemOffLabel = label.replace(" ","") + "Off"
            itemBtnGroupLabel = itemLabel + "BtnGroup"

            print(label)
            print(showInputName[row])

            setattr(self,itemLabel,QLineEdit())
            setattr(self,itemOnLabel,QRadioButton('On'))
            setattr(self,itemOffLabel,QRadioButton('Off'))
            setattr(self,itemBtnGroupLabel,QButtonGroup())

            getItemLabel = getattr(self,itemLabel)
            getItemOn = getattr(self,itemOnLabel)
            getItemOff = getattr(self,itemOffLabel)
            getItemBtnGroup = getattr(self,itemBtnGroupLabel)

            getItemBtnGroup.addButton(getItemOn)
            getItemBtnGroup.addButton(getItemOff)

            getItemLabel.setText(showInputName[row])

            if (showMenuData[row][0].lower() == 'true'):
                getItemOn.setChecked(True)
            if (showMenuData[row][0].lower() == 'false'):
                getItemOff.setChecked(True)

            SubLayout = QHBoxLayout()
            SubLayout.addWidget(getItemOn)
            SubLayout.addWidget(getItemOff)
            RightLayout.addWidget(getItemLabel)

            RightLayout.addLayout(SubLayout)
            Layout.addLayout(RightLayout,row,1)

            row += 1

        return Layout

    def init_widget_field(self):
        # General tab ----------------------------------------------------------
        self.GlOutInterval.setText(self.inputData[0][0])

        self.StarCatalogName.setText(self.inputData[1][0])

        if (self.inputData[2][0].lower() == 'true'):
            self.MapWindowOn.setChecked(True)
        if (self.inputData[2][0].lower() == 'false'):
            self.MapWindowOff.setChecked(True)

        if (self.inputData[3][0].lower() == 'true'):
            self.OrreryWindowOn.setChecked(True)
        if (self.inputData[3][0].lower() == 'false'):
            self.OrreryWindowOff.setChecked(True)

        if (self.inputData[4][0].lower() == 'true'):
            self.UnitSphereWindowOn.setChecked(True)
        if (self.inputData[4][0].lower() == 'false'):
            self.UnitSphereWindowOff.setChecked(True)

        # Pov tab --------------------------------------------------------------
        if (self.inputData[5][0].lower() == 'true'):
            self.PauseStartupOn.setChecked(True)
        if (self.inputData[5][0].lower() == 'false'):
            self.PauseStartupOff.setChecked(True)

        if (self.inputData[6][0].lower() == "track_host"):
            self.ModePOV.setCurrentIndex(0)
        elif (self.inputData[6][0].lower() == "track_target"):
            self.ModePOV.setCurrentIndex(1)
        elif (self.inputData[6][0].lower() == "track_in_host"):
            self.ModePOV.setCurrentIndex(2)

        if (self.inputData[7][0].lower() == "world"):
            self.HostType.setCurrentIndex(0)
        elif (self.inputData[7][0].lower() == "reforb"):
            self.HostType.setCurrentIndex(1)
        elif (self.inputData[7][0].lower() == "frm"):
            self.HostType.setCurrentIndex(2)
        elif (self.inputData[7][0].lower() == "sc"):
            self.HostType.setCurrentIndex(3)
        elif (self.inputData[7][0].lower() == "body"):
            self.HostType.setCurrentIndex(4)

        self.InitialHostSC.setText(self.inputData[8][0])
        self.InitialHostBody.setText(self.inputData[8][1])
        self.InitialHostPOVFrame.setText(self.inputData[8][2])

        if (self.inputData[9][0].lower() == "world"):
            self.TargetType.setCurrentIndex(0)
        elif (self.inputData[9][0].lower() == "reforb"):
            self.TargetType.setCurrentIndex(1)
        elif (self.inputData[9][0].lower() == "frm"):
            self.TargetType.setCurrentIndex(2)
        elif (self.inputData[9][0].lower() == "sc"):
            self.TargetType.setCurrentIndex(3)
        elif (self.inputData[9][0].lower() == "body"):
            self.TargetType.setCurrentIndex(4)

        self.InitialTargetSC.setText(self.inputData[10][0])
        self.InitialTargetBody.setText(self.inputData[10][1])
        self.InitialTargetPOVFrame.setText(self.inputData[10][2])

        self.BoresightAxis.setText(self.inputData[11][0])

        self.UpAxis.setText(self.inputData[12][0])

        self.InitPovRange.setText(self.inputData[13][0])

        self.VPovAngle.setText(self.inputData[14][0])

        self.PovPosHostFirstEntry.setText(self.inputData[15][0])
        self.PovPosHostSecondEntry.setText(self.inputData[15][1])
        self.PovPosHostThirdEntry.setText(self.inputData[15][2])

        if (self.inputData[16][0].lower() == "front"):
            self.InitPovView.setCurrentIndex(0)
        elif (self.inputData[16][0].lower() == "front_right"):
            self.InitPovView.setCurrentIndex(1)
        elif (self.inputData[16][0].lower() == "front_left"):
            self.InitPovView.setCurrentIndex(2)
        elif (self.inputData[16][0].lower() == "rear"):
            self.InitPovView.setCurrentIndex(3)
        elif (self.inputData[16][0].lower() == "rear_left"):
            self.InitPovView.setCurrentIndex(4)
        elif (self.inputData[16][0].lower() == "rear_right"):
            self.InitPovView.setCurrentIndex(5)
        elif (self.inputData[16][0].lower() == "up"):
            self.InitPovView.setCurrentIndex(6)
        elif (self.inputData[16][0].lower() == "down"):
            self.InitPovView.setCurrentIndex(7)
        elif (self.inputData[16][0].lower() == "left"):
            self.InitPovView.setCurrentIndex(8)
        elif (self.inputData[16][0].lower() == "right"):
            self.InitPovView.setCurrentIndex(9)

        # Cam tab --------------------------------------------------------------
        self.CamTitle.setText(self.inputString[0])

        self.CamWidth.setText(self.inputData[18][0])
        self.CamHeight.setText(self.inputData[18][1])

        self.MouseScaleFactor.setText(self.inputData[19][0])

        self.DisplayGammaExponent.setText(self.inputData[20][0])

        # Map tab --------------------------------------------------------------
        self.MapName.setText(self.inputString[19])

        self.MapWidth.setText(self.inputData[40][0])
        self.MapHeight.setText(self.inputData[40][1])

        if (self.inputData[41][0].lower() == 'true'):
            self.MajorConsOn.setChecked(True)
        if (self.inputData[41][0].lower() == 'false'):
            self.MajorConsOff.setChecked(True)

        if (self.inputData[42][0].lower() == 'true'):
            self.ZodiacConsOn.setChecked(True)
        if (self.inputData[42][0].lower() == 'false'):
            self.ZodiacConsOff.setChecked(True)

        if (self.inputData[43][0].lower() == 'true'):
            self.MinorConsOn.setChecked(True)
        if (self.inputData[43][0].lower() == 'false'):
            self.MinorConsOff.setChecked(True)

    def signal_to_slot(self):
        self.ApplyBtn.clicked.connect(self.write_file_slot)
# ------------------------------------------------------------------------------
# Slot Functions----------------------------------------------------------------
    def write_file_slot(self): # Write edited input txt data to output txt file
        # Write inputs to txt Write File
        self.save_data_slot()
        self.WriteFile = open(self.inputFile, 'w+')
        self.WriteFile.writelines(self.InpGraphicsData)
        self.WriteFile.close()

    def save_data_slot(self):

        self.InpGraphicsData[0] = '<<<<<<<<<<<<<<<<  42 Graphics Configuration File  >>>>>>>>>>>>>>>>>>>\n'

        data_inp = self.GlOutInterval.text()
        self.InpGraphicsData[1] = functions.whitespace(data_inp) + ' ! GL Output Interval [sec]\n'

        data_inp = self.StarCatalogName.text()
        self.InpGraphicsData[2] = functions.whitespace(data_inp) + ' ! Star Catalog File Name\n'

        if self.MapWindowOn.isChecked():
            boolValue = "TRUE"
        else:
            boolValue = "FALSE"
        data_inp = boolValue
        self.InpGraphicsData[3] = functions.whitespace(data_inp) + ' ! Draw Near Field, Draw Far Field\n'

        if self.OrreryWindowOn.isChecked():
            boolValue = "TRUE"
        else:
            boolValue = "FALSE"
        data_inp = boolValue
        self.InpGraphicsData[4] = functions.whitespace(data_inp) + ' ! Orrery Window Exists\n'

        if self.UnitSphereWindowOn.isChecked():
            boolValue = "TRUE"
        else:
            boolValue = "FALSE"
        data_inp = boolValue
        self.InpGraphicsData[5] = functions.whitespace(data_inp) + ' ! Unit Sphere Window Exists\n'

        self.InpGraphicsData[6] = '******************************* POV *********************************\n'

        if self.PauseStartupOn.isChecked():
            boolValue = "TRUE"
        else:
            boolValue = "FALSE"
        data_inp = boolValue
        self.InpGraphicsData[7] = functions.whitespace(data_inp) + ' ! Pause at Startup\n'

        data_inp = self.ModePOV.currentText()
        self.InpGraphicsData[8] = functions.whitespace(data_inp) + ' ! POV Mode (TRACK_HOST, TRACK_TARGET, FIXED_IN_HOST)\n'

        data_inp = self.HostType.currentText()
        self.InpGraphicsData[9] = functions.whitespace(data_inp) + ' ! Host Type (WORLD, REFORB, FRM, SC, BODY)\n'

        data_inp = self.InitialHostSC.text() + "  " + self.InitialHostBody.text() + "  " + self.InitialHostPOVFrame.text()
        self.InpGraphicsData[10] = functions.whitespace(data_inp) + ' ! Initial Host SC, Body, POV Frame\n'

        data_inp = self.TargetType.currentText()
        self.InpGraphicsData[11] = functions.whitespace(data_inp) + ' ! Target Type (WORLD, REFORB, FRM, SC, BODY)\n'

        data_inp = self.InitialTargetSC.text() + "  " + self.InitialTargetBody.text() + "  " + self.InitialTargetPOVFrame.text()
        self.InpGraphicsData[12] = functions.whitespace(data_inp) + ' ! Initial Target SC, Body, POV Frame\n'

        data_inp = self.BoresightAxis.text()
        self.InpGraphicsData[13] = functions.whitespace(data_inp) + ' ! Boresight Axis\n'

        data_inp = self.UpAxis.text()
        self.InpGraphicsData[14] = functions.whitespace(data_inp) + ' ! Up Axis\n'

        data_inp = self.InitPovRange.text()
        self.InpGraphicsData[15] = functions.whitespace(data_inp) + ' ! Initial POV Range from Target [m]\n'

        data_inp = self.VPovAngle.text()
        self.InpGraphicsData[16] = functions.whitespace(data_inp) + ' ! POV Angle (Vertical) [deg]\n'

        data_inp = self.PovPosHostFirstEntry.text() + "  " + self.PovPosHostSecondEntry.text() + "  " + self.PovPosHostThirdEntry.text()
        self.InpGraphicsData[17] = functions.whitespace(data_inp) + ' ! POV Position in Host [m]\n'

        data_inp = self.InitPovView.currentText()
        self.InpGraphicsData[18] = functions.whitespace(data_inp) + ' ! Initial POV View (FRONT, FRONT_RIGHT, etc)\n'

        self.InpGraphicsData[19] = '******************************* CAM *********************************\n'

        data_inp = '"' + self.CamTitle.text()  + '"'
        self.InpGraphicsData[20] = functions.whitespace(data_inp) + ' ! Cam Title \n'

        data_inp = self.CamWidth.text() + "  " + self.CamHeight.text()
        self.InpGraphicsData[21] = functions.whitespace(data_inp) + ' ! Width, Height [pixels]\n'

        data_inp = self.MouseScaleFactor.text()
        self.InpGraphicsData[22] = functions.whitespace(data_inp) + ' ! Mouse Scale Factor\n'

        data_inp = self.DisplayGammaExponent.text()
        self.InpGraphicsData[23] = functions.whitespace(data_inp) + ' ! Display Gamma Exponent (1.8-4.0)\n'

        self.InpGraphicsData[24] = '************************** CAM Show Menu ****************************\n'

        CamShowMenuLabels = [
        'N Axes','L Axes','F Axes','B Axes','N Grid','L Grid','F Grid','B Grid',
        'G Grid','Field of View','Prov Ops','TDRS Satellites','Shadows',
        'Astro Labels','Truth Vectors','FSW Vectors','Milky Way','Fermi Sky']
        CamShowMenuExistInputs = self.inputData[21:39]
        CamShowMenuNameInputs = self.inputString[1:19]
        line_start = 25
        self.save_data2_slot(line_start,CamShowMenuLabels,CamShowMenuExistInputs,CamShowMenuNameInputs)

        self.InpGraphicsData[43] = '******************************* MAP *********************************\n'

        data_inp = '"' + self.MapName.text() + '"'
        self.InpGraphicsData[44] = functions.whitespace(data_inp) + ' ! Map Title [delimited by "]\n'

        data_inp = self.MapWidth.text() + '  ' + self.MapHeight.text()
        self.InpGraphicsData[45] = functions.whitespace(data_inp) + ' ! Width, Height [pixels]\n'

        self.InpGraphicsData[46] = '************************** MAP Show Menu ****************************\n'

        MapShowMenuLabels = ['Clock','Tlm Clock','Credits','Night']
        MapShowMenuExistInputs = self.inputData[41:45]
        MapShowMenuNameInputs = self.inputString[20:24]
        line_start = 47
        self.save_data2_slot(line_start,MapShowMenuLabels,MapShowMenuExistInputs,MapShowMenuNameInputs)

        self.InpGraphicsData[51] = '********************* Unit Sphere Show Menu *************************\n'

        if self.MajorConsOn.isChecked():
            boolValue = "TRUE"
        else:
            boolValue = "FALSE"
        data_inp = boolValue
        self.InpGraphicsData[52] = functions.whitespace(data_inp) + ' ! Show Major Constellations\n'

        if self.ZodiacConsOn.isChecked():
            boolValue = "TRUE"
        else:
            boolValue = "FALSE"
        data_inp = boolValue
        self.InpGraphicsData[53] = functions.whitespace(data_inp) + ' ! Show Zodiac Constellations\n'

        if self.MinorConsOn.isChecked():
            boolValue = "TRUE"
        else:
            boolValue = "FALSE"
        data_inp = boolValue
        self.InpGraphicsData[54] = functions.whitespace(data_inp) + ' ! Show Minor Constellations\n'

    def save_data2_slot(self,line_start,labels,showMenuData,showInputName):
        row = 0

        for label in labels:

            itemLabel = label.replace(" ","")
            itemOnLabel = label.replace(" ","") + "On"

            print(label)
            print(showInputName[row])

            getItemLabel = getattr(self,itemLabel)
            getItemOn = getattr(self,itemOnLabel)

            print(row)
            print(label)
            print(getItemLabel.text())
            if getItemOn.isChecked():
                boolValue = "TRUE"
            else:
                boolValue = "FALSE"

            print(boolValue)

            data_inp = boolValue + ' "' + getItemLabel.text() + '"'
            self.InpGraphicsData[line_start + row] = functions.whitespace(data_inp) + ' ! Show ' + label + '\n'

            row += 1
