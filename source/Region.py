# This file contains the code for the Region window in the 42 gui

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

class RegionWindow(QWidget):
    def __init__(RegionWindow):
        super().__init__()
        RegionWindow.setWindowTitle('Region Parameters') # sets the window’s title
        RegionWindow.setGeometry(0, 0, 300, 400) # Set window, can resize manually (xpos on monitor, ypos on monitor, width pixels, height pixles)
        RegionLayout = QGridLayout()

        Label = [0]*2 # Initialize array of labels
        RegionWindow.RegionDataOut = [""]*300 # Initialize List
        RegionWindow.missionDir = functions.get_mission_directory(RegionWindow)
        RegionWindow.inputFile = RegionWindow.missionDir + '/InOut/Inp_Region.txt'
        RegionWindow.GetDataIn(RegionWindow.inputFile)

        # Heading Label---------------------------------------------------------
        Label[0] = QLabel("Regions for 42")
        Label[0].setAlignment(Qt.AlignCenter)
        Label[0].setStyleSheet('font-size: 20px')
        RegionLayout.addWidget(Label[0], 0, 0, 1, 3)

        # List Label------------------------------------------------------------
        Label[1] = QLabel("Region List")
        Label[1].setAlignment(Qt.AlignCenter)
        Label[1].setStyleSheet('font-size: 15px')
        RegionLayout.addWidget(Label[1], 1, 0)

        # List of Regions-------------------------------------------------------
        RegionWindow.List = QListWidget()
        for i in range(RegionWindow.NumRegionsDataIn):
            idx = i + 1
            RegionWindow.List.insertItem(i, "Region %s"%idx)
        RegionLayout.addWidget(RegionWindow.List, 1, 0, 12, 1)
        RegionWindow.List.currentRowChanged.connect(RegionWindow.DisplayStackSlot)

        # Stacked Widget--------------------------------------------------------
        RegionWindow.Stack = QStackedWidget()

        for i in range(RegionWindow.NumRegionsDataIn):
            RegionWindow.AddStack(i)
            RegionWindow.SetGuiFields(i)
        RegionLayout.addWidget(RegionWindow.Stack,1,1,13,2)

#-------------------------------------------------------------------------------
# Apply / Cancel / Reset Default Button-----------------------------------------
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(RegionWindow.WriteDataOut)
        ApplyBtn.clicked.connect(RegionWindow.WriteFileOut)
        ApplyBtn.clicked.connect(RegionWindow.close)
        #ApplyBtn.clicked.connect(RegionWindow.empty_widget_flag)
        #ApplyBtn.clicked.connect(RegionWindow.WidgetsSlot)
        #ApplyBtn.clicked.connect(RegionWindow.WriteFileSlot) # Last Slot
        RegionLayout.addWidget(ApplyBtn, 14, 2)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(RegionWindow.close)
        RegionLayout.addWidget(CancelBtn, 14, 1)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(RegionWindow.SetDefaultGuiFields)
        RegionLayout.addWidget(ResetBtn, 14, 0)

#-------------------------------------------------------------------------------
# Finialize Region window-------------------------------------------------------
        RegionWindow.setLayout(RegionLayout)
#-------------------------------------------------------------------------------
# Region Stack--------------------------------------------------------------
    def AddStack(RegionWindow,i): # Adds a new page to the Region Stack with the same layout as previous pages
        StackLengthOffset = 8*i
        Label = [0]*10 # Initialize array of labels
        RegionWindow.Page = QWidget() # Create widget to become a Page in Stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout() # for the radio buttons

        # Region Status------------------------------------------------------
        Label[0] = QLabel("Region Status:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[0], 0, 0)

        RegionWindow.Page.StatusOn = QRadioButton('On')
        RegionWindow.Page.StatusOff = QRadioButton('Off')

        RegionWindow.Page.StatusBtnGroup = QButtonGroup()
        RegionWindow.Page.StatusBtnGroup.addButton(RegionWindow.Page.StatusOn)
        RegionWindow.Page.StatusBtnGroup.addButton(RegionWindow.Page.StatusOff)

        SubLayout.addWidget(RegionWindow.Page.StatusOn)
        SubLayout.addWidget(RegionWindow.Page.StatusOff)
        StackLayout.addLayout(SubLayout, 0, 1)

        # Region Name Label-----------------------------------------------------
        Label[1] = QLabel("Region Name:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 1, 0)

        RegionWindow.Page.Name = QLineEdit()
        StackLayout.addWidget(RegionWindow.Page.Name, 1, 1)

        # Region World----------------------------------------------------------
        Label[2] = QLabel("World:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[2], 2, 0)

        RegionWindow.Page.World = QLineEdit()
        StackLayout.addWidget(RegionWindow.Page.World, 2, 1)

        # Coordinate Sytem------------------------------------------------------
        Label[3] = QLabel("Coordinate Sytem:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[3], 3, 0)

        RegionWindow.Page.Coordinates = QComboBox()
        RegionWindow.Page.Coordinates.addItems(["Position in W", "Latitude, Longitude, Altitude (LLA)"])

        StackLayout.addWidget(RegionWindow.Page.Coordinates, 3, 1)

        # W Position------------------------------------------------------------
        Label[4] = QLabel("X, Y, Z Position in W (m):")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[4], 4, 0, 1, 2)

        RegionWindow.Page.Wx = QLineEdit()
        RegionWindow.Page.Wy = QLineEdit()
        RegionWindow.Page.Wz = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(RegionWindow.Page.Wx)
        SubLayout.addWidget(RegionWindow.Page.Wy)
        SubLayout.addWidget(RegionWindow.Page.Wz)
        StackLayout.addLayout(SubLayout,5, 0, 1, 2)

        # Latitude, Longitude, Altitude----------------------------------------
        Label[5] = QLabel("Latitude (deg), Longitude (deg), & Altitude (m):")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[5], 6, 0, 1, 2)

        RegionWindow.Page.Latitude = QLineEdit()
        RegionWindow.Page.Longitude = QLineEdit()
        RegionWindow.Page.Altitude = QLineEdit()

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(RegionWindow.Page.Latitude)
        SubLayout.addWidget(RegionWindow.Page.Longitude)
        SubLayout.addWidget(RegionWindow.Page.Altitude)
        StackLayout.addLayout(SubLayout,7, 0, 1, 2)

        # Elasticity------------------------------------------------------------
        Label[6] = QLabel("Elasticity Coefficient:")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[6], 8, 0, 1, 2)

        RegionWindow.Page.Elasticity = QLineEdit()
        StackLayout.addWidget(RegionWindow.Page.Elasticity, 8, 1)

        # Damping---------------------------------------------------------------
        Label[7] = QLabel("Damping Coefficient:")
        Label[7].setAlignment(Qt.AlignLeft)
        Label[7].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[7], 9, 0)

        RegionWindow.Page.Damping = QLineEdit()
        StackLayout.addWidget(RegionWindow.Page.Damping, 9, 1)

        # Friction--------------------------------------------------------------
        Label[8] = QLabel("Friction Coefficient:")
        Label[8].setAlignment(Qt.AlignLeft)
        Label[8].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[8], 10, 0)

        RegionWindow.Page.Friction = QLineEdit()
        # RegionWindow.Page.Friction = QLineEdit(RegionWindow.RegionDataIn[7 + StackLengthOffset][2])
        StackLayout.addWidget(RegionWindow.Page.Friction, 10, 1)

        # Geometry File---------------------------------------------------------
        Label[9] = QLabel("Geometry File Name:")
        Label[9].setAlignment(Qt.AlignLeft)
        Label[9].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[9], 11, 0)

        RegionWindow.Page.GeometryFile = QLineEdit()

        # RegionWindow.Page.GeometryFile = QLineEdit(RegionWindow.RegionDataIn[8 + StackLengthOffset][0])
        StackLayout.addWidget(RegionWindow.Page.GeometryFile, 11, 1)

        # Add Region Button-----------------------------------------------------
        AddRegionBtn = QPushButton('Add a Region')
        StackLayout.addWidget(AddRegionBtn, 12, 1)
        AddRegionBtn.clicked.connect(RegionWindow.AddRegion)

        # Remove Region btn
        RmvRegionBtn = QPushButton('Remove a Region')
        StackLayout.addWidget(RmvRegionBtn, 12, 0)
        RmvRegionBtn.clicked.connect(RegionWindow.RmvRegion)

        RegionWindow.Page.setLayout(StackLayout)
        RegionWindow.Stack.addWidget(RegionWindow.Page)

    def DisplayStackSlot(RegionWindow, i): # Displayes current page in Stack when selected in the Region list
        RegionWindow.Stack.setCurrentIndex(i)

    def AddRegion(RegionWindow): # Adds a Region to list and Stack
        ItemIndex = RegionWindow.List.count() # Index of new item
        ItemLabel = ItemIndex + 1
        RegionWindow.List.insertItem(int(ItemIndex), "Region %s"%ItemLabel)

        RegionWindow.AddStack(ItemIndex)

        RegionWindow.List.setCurrentRow(ItemIndex)
        RegionWindow.Stack.setCurrentIndex(ItemIndex)

    def RmvRegion(RegionWindow): # Removes the last Region added to the Region List
        RmvIndex = RegionWindow.List.count() - 1
        if RmvIndex > 0:
            RegionWindow.List.takeItem(RmvIndex)
            RegionWindow.Stack.removeWidget(RegionWindow.Stack.widget(RmvIndex))

#-------------------------------------------------------------------------------
# Slot-------------------------------------------------------------------------
    def WriteFileOut(RegionWindow):
        # Write inputs to txt Write File
        #if not RegionWindow.emptyWidgetFlag:
        RegionWindow.WriteFile = open(RegionWindow.inputFile, 'w+')
        RegionWindow.WriteFile.writelines(RegionWindow.RegionDataOut)
        RegionWindow.WriteFile.close()

    def SetDefaultGuiFields(RegionWindow): # Resets gui options and input txt data to default
        # Reset Write File to Default/readfile
        RegionWindow.GetDataIn('InOut/Default/Inp_Region_Default.txt')
        for i in range(RegionWindow.NumRegionsDataIn):
            RegionWindow.SetGuiFields(i)

    def WriteDataOut(RegionWindow):
        RegionWindow.RegionDataOut[0] = "<<<<<<<<<<<<<<<<<<<<<<<<<<<< 42: Regions File >>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"
        RegionWindow.RegionDataOut[1] = str(RegionWindow.List.count()) + "                            !  Number of Regions\n"

        txt_idx = 2
        for i in range(RegionWindow.List.count()):
            RegionWindow.RegionDataOut[txt_idx] = "*********************************  Region " + str(i) + "   ****************************\n"
            txt_idx += 1

            if RegionWindow.Stack.widget(i).StatusOn.isChecked():
                RegionWindow.RegionDataOut[txt_idx] = "TRUE                         ! Exists\n"
            elif RegionWindow.Stack.widget(i).StatusOff.isChecked():
                RegionWindow.RegionDataOut[txt_idx] = "FALSE                        ! Exists\n"
            txt_idx += 1

            RegionWindow.RegionDataOut[txt_idx] = "\"" + RegionWindow.Stack.widget(i).Name.text() + "\"" + "                         ! Name\n"
            txt_idx += 1

            RegionWindow.RegionDataOut[txt_idx] = RegionWindow.Stack.widget(i).World.text() + "                  ! World\n"
            txt_idx += 1

            RegionWindow.RegionDataOut[txt_idx] = RegionWindow.Stack.widget(i).Coordinates.currentText() + "                ! POSW or LLA\n"
            txt_idx += 1

            RegionWindow.RegionDataOut[txt_idx] = RegionWindow.Stack.widget(i).Wx.text() + "   " + RegionWindow.Stack.widget(i).Wy.text() + "   " + RegionWindow.Stack.widget(i).Wz.text() + "              ! Position in W, m\n"
            txt_idx += 1

            RegionWindow.RegionDataOut[txt_idx] = RegionWindow.Stack.widget(i).Latitude.text() + "   " + RegionWindow.Stack.widget(i).Longitude.text() + "   " + RegionWindow.Stack.widget(i).Altitude.text() + "      ! Lng, Lat (deg), Alt (m)\n"
            txt_idx += 1

            RegionWindow.RegionDataOut[txt_idx] = RegionWindow.Stack.widget(i).Elasticity.text() + "   " + RegionWindow.Stack.widget(i).Damping.text() + "   " + RegionWindow.Stack.widget(i).Friction.text() + "          ! Elasticity, Damping, Friction Coef\n"
            txt_idx += 1

            RegionWindow.RegionDataOut[txt_idx] = RegionWindow.Stack.widget(i).GeometryFile.text() + "              ! Geometry File Name\n"
            txt_idx += 1

    def GetDataIn(RegionWindow, FileName):
        RegionWindow.RegionDataIn = []
        pattern = re.compile("(.*?)\s*!")

        for i, line in enumerate(open(FileName)):
            for match in re.finditer(pattern, line):
                RegionWindow.RegionDataIn.append(match.group(1).replace('"',"").split())
        RegionWindow.NumRegionsDataIn = int(RegionWindow.RegionDataIn[0][0])

    def SetGuiFields(RegionWindow,i):
        if RegionWindow.List.count() > RegionWindow.NumRegionsDataIn:
            Diff = RegionWindow.List.count() - RegionWindow.NumRegionsDataIn
            for k in range(Diff):
                RegionWindow.RmvRegion()

        if i >= RegionWindow.List.count():
            RegionWindow.AddRegion()

        StackLengthOffset = 8*i # Offset due to length of each stack

        if (RegionWindow.RegionDataIn[1 + StackLengthOffset][0].lower() == 'true'):
            RegionWindow.Stack.widget(i).StatusOn.setChecked(True)
        else:
            RegionWindow.Stack.widget(i).StatusOff.setChecked(True)

        RegionWindow.Stack.widget(i).Name.setText(RegionWindow.RegionDataIn[2 + StackLengthOffset][0])

        RegionWindow.Stack.widget(i).World.setText(RegionWindow.RegionDataIn[3 + StackLengthOffset][0])

        if (RegionWindow.RegionDataIn[4 + StackLengthOffset][0].lower() == "position"):
            RegionWindow.Stack.widget(i).Coordinates.setCurrentIndex(0)
        elif (RegionWindow.RegionDataIn[4 + StackLengthOffset][0].lower() == "latitude"):
            RegionWindow.Stack.widget(i).Coordinates.setCurrentIndex(1)

        RegionWindow.Stack.widget(i).Wx.setText(RegionWindow.RegionDataIn[5 + StackLengthOffset][0])
        RegionWindow.Stack.widget(i).Wy.setText(RegionWindow.RegionDataIn[5 + StackLengthOffset][1])
        RegionWindow.Stack.widget(i).Wz.setText(RegionWindow.RegionDataIn[5 + StackLengthOffset][2])

        RegionWindow.Stack.widget(i).Latitude.setText(RegionWindow.RegionDataIn[6 + StackLengthOffset][0])
        RegionWindow.Stack.widget(i).Longitude.setText(RegionWindow.RegionDataIn[6 + StackLengthOffset][1])
        RegionWindow.Stack.widget(i).Altitude.setText(RegionWindow.RegionDataIn[6 + StackLengthOffset][2])

        RegionWindow.Stack.widget(i).Elasticity.setText(RegionWindow.RegionDataIn[7 + StackLengthOffset][0])

        RegionWindow.Stack.widget(i).Damping.setText(RegionWindow.RegionDataIn[7 + StackLengthOffset][1])

        RegionWindow.Stack.widget(i).Friction.setText(RegionWindow.RegionDataIn[7 + StackLengthOffset][2])

        RegionWindow.Stack.widget(i).GeometryFile.setText(RegionWindow.RegionDataIn[8 + StackLengthOffset][0])



    # def empty_widget_flag(self):
    #     # widgets = ["LabelFov","NumSide","LengthSide","Width","Height",
    #     # "RedColor","GreenColor","BlueColor","AlphaColor","SC","Body",
    #     # "X_Axis_Body","Y_Axis_Body","Z_Axis_Body","FirstSequence","ThirdSequence","Sequence"]
    #
    #     self.emptyWidgetFlag = False
    #     for x in range(self.inputRegionNumber):
    #         print(self.Stack.widget(x).layout().findChildren(QLineEdit))
    #         for widget in self.Stack.widget(x).layout().findChildren(QLineEdit):
    #             print("TEST B")
    #         # for widget in widgets:
    #             if not widget.text():
    #                 errorMsg = QMessageBox()
    #                 errorMsg.setText("Error: Fill out empty entries before applying!")
    #                 errorMsg.exec_()
    #                 self.emptyWidgetFlag = True
    #                 print("test A")
    #                 return
    #         if self.emptyWidgetFlag:return
