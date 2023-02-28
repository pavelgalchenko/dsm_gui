# This file contains the code for the Orbits window in the 42 gui

# Import Packages---------------------------------------------------------------
import functions
import re
import os
import shutil

# Import QApplication and the required widgets from PySide6
from PySide6 import QtCore
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QWidget
from PySide6.QtWidgets import QLabel
from PySide6.QtWidgets import QGridLayout # to arrange the buttons
from PySide6.QtWidgets import QLineEdit # for the display
from PySide6.QtWidgets import QComboBox # drop down box
from PySide6.QtWidgets import QVBoxLayout # verticle stack layout
from PySide6.QtWidgets import QHBoxLayout # Horizontal stack layout
from PySide6.QtWidgets import QRadioButton
from PySide6.QtWidgets import QFormLayout
from PySide6.QtWidgets import QListWidget
from PySide6.QtWidgets import QStackedWidget
from PySide6.QtWidgets import QButtonGroup
from PySide6.QtWidgets import QTabWidget
from PySide6.QtWidgets import QCompleter
from PySide6.QtWidgets import QAbstractItemView
from PySide6.QtWidgets import QMessageBox

BODY_LIST = [
    'SOL','MERCURY','VENUS','EARTH','MARS','JUPITER','SATURN','URANUS',
    'NEPTUNE','PLUTO','LUNA','PHOBOS','DEIMOS','IO','EUROPA','GANYMEDE','CALLISTO',
    'AMALTHEA','HIMALITA','ELARA','PASIPHAE','SINOPE','LYSITHEA','CARME','ANANKE',
    'LEDA','THEBE','ADRASTEA','METIS','MIMAS','ENCELADUS','TETHYS','DIONE','RHEA',
    'TITAN','HYPERION','IAPETUS','PHOEBE','JANUS','EPIMETHEUS','HELENE','TELESTO',
    'CALYPSO','ATLAS','PROMETHEUS','PANDORA','PAN','ARIEL','UMBRIEL','UMBRIEL',
    'TITANIA','OBERON','MIRANDA','TRITON','NERIED','CHARON', 'MINORBODY_#'
]

class OrbitWindow(QWidget):

    updateRefOrbitList = QtCore.Signal()

    def __init__(self):
        super().__init__()
        self.setWindowTitle('Orbit Parameters') # sets the window’s title
        # self.setFixedSize(500, 350) # gives window a fixed size

        self.missionDir = functions.get_mission_directory(self)
        functions.get_sc_orbit_file_names(self)

        self.orbitList = QListWidget()
        self.orbitList.setEditTriggers(QAbstractItemView.DoubleClicked)
        self.stackedWidget = QStackedWidget()
        for i, orbFile in enumerate(self.orbFileNames):
            print(orbFile)
            self.orbitList.insertItem(i, orbFile)
            self.init_stack_ui(orbFile)
            self.init_stack_field(i, orbFile)

        self.addRemoveBtnLayout = functions.remove_add_btns(self,['removeBtn','addBtn'])
        self.defaultCancelApplyLayout = functions.default_cancel_apply_btns(self,['defaultBtn','upddefaultBtn','cancelBtn','applyBtn'])

        self.listHeader = QLabel('Reference Orbit Input Files')
        self.listHeader.setStyleSheet('font-size: 15px')
        self.listHeader.setAlignment(Qt.AlignCenter)

        listLayout = QVBoxLayout()
        listLayout.addWidget(self.listHeader)
        listLayout.addWidget(self.orbitList)

        mainLayout = QGridLayout()
        mainLayout.addLayout(listLayout,0,0)
        mainLayout.addLayout(self.addRemoveBtnLayout,1,0)
        mainLayout.addLayout(self.defaultCancelApplyLayout,1,1)
        mainLayout.addWidget(self.stackedWidget,0,1)

        self.setLayout(mainLayout)

        self.signal_to_slot()

    def init_stack_ui(self, orbFile):
        self.stack = OrbitTab(self)
        self.stack.fileName = orbFile
        self.stackedWidget.addWidget(self.stack)

    def init_stack_field(self,stackIndex,file):
        stack = self.stackedWidget.widget(stackIndex)
        stack.get_data_input(self.missionDir + "/InOut/" + file)

        # manual tweaking of data until better parsing method is found
        " ".join(stack.centralInputData[14])

        # fill in gui fields
        # general tab
        stack.general_input_0.setText(" ".join(stack.generalInputData[0]))
        itemIndex = stack.general_input_1.findText(stack.generalInputData[1][0])
        stack.general_input_1.setCurrentIndex(itemIndex)
        # formation
        itemIndex = stack.formation_input_0.findText(stack.formationFrameInputData[0][0])
        stack.formation_input_0.setCurrentIndex(itemIndex)

        stack.formation_input_1.setText(stack.formationFrameInputData[1][0])
        stack.formation_input_2.setText(stack.formationFrameInputData[1][1])
        stack.formation_input_3.setText(stack.formationFrameInputData[1][2])

        itemIndex = stack.formation_input_4.findText(stack.formationFrameInputData[1][3])
        stack.formation_input_4.setCurrentIndex(itemIndex)

        itemIndex = stack.formation_input_5.findText(stack.formationFrameInputData[2][0])
        stack.formation_input_5.setCurrentIndex(itemIndex)

        stack.formation_input_6.setText(stack.formationFrameInputData[3][0])
        stack.formation_input_7.setText(stack.formationFrameInputData[3][1])
        stack.formation_input_8.setText(stack.formationFrameInputData[3][2])

        # zero tab
        itemIndex = stack.zero_input_0.findText(stack.zeroInputData[0][0])
        stack.zero_input_0.setCurrentIndex(itemIndex)

        if stack.zeroInputData[1][0] == 'TRUE':
            stack.zero_input_1_ON.setChecked(True)
        else:
            stack.zero_input_1_OFF.setChecked(True)

        # flight tab
        stack.flight_input_0.setText(stack.flightInputData[0][0])

        if stack.flightInputData[1][0] == 'TRUE':
            stack.flight_input_1_ON.setChecked(True)
        else:
            stack.flight_input_1_OFF.setChecked(True)

         # central tab
        stack.central_input_0.setText(stack.centralInputData[0][0])

        if stack.centralInputData[1][0] == 'TRUE':
            stack.central_input_1_ON.setChecked(True)
        else:
            stack.central_input_1_OFF.setChecked(True)

        itemIndex = stack.central_input_2.findText(stack.centralInputData[2][0])
        stack.central_input_2.setCurrentIndex(itemIndex)

        itemIndex = stack.central_input_3.findText(stack.centralInputData[3][0])
        stack.central_input_3.setCurrentIndex(itemIndex)

        stack.central_input_4.setText(stack.centralInputData[4][0])
        stack.central_input_5.setText(stack.centralInputData[4][1])
        stack.central_input_6.setText(stack.centralInputData[5][0])
        stack.central_input_7.setText(stack.centralInputData[5][1])
        stack.central_input_8.setText(stack.centralInputData[6][0])
        stack.central_input_9.setText(stack.centralInputData[7][0])
        stack.central_input_10.setText(stack.centralInputData[8][0])
        stack.central_input_11.setText(stack.centralInputData[9][0])
        stack.central_input_12.setText(stack.centralInputData[10][0])
        stack.central_input_13.setText(stack.centralInputData[10][1])
        stack.central_input_14.setText(stack.centralInputData[10][2])
        stack.central_input_15.setText(stack.centralInputData[11][0])
        stack.central_input_16.setText(stack.centralInputData[11][1])
        stack.central_input_17.setText(stack.centralInputData[11][2])

        itemIndex = stack.central_input_18.findText(stack.centralInputData[12][0])
        stack.central_input_18.setCurrentIndex(itemIndex)

        stack.central_input_19.setText(stack.centralInputData[13][0])
        stack.central_input_20.setText(stack.centralInputData[14][0])

         # Three body tab
        itemIndex = stack.threeBody_input_0.findText(stack.threeBodyInputData[0][0])
        stack.threeBody_input_0.setCurrentIndex(itemIndex)

        itemIndex = stack.threeBody_input_1.findText(stack.threeBodyInputData[1][0])
        stack.threeBody_input_1.setCurrentIndex(itemIndex)

        itemIndex = stack.threeBody_input_2.findText(stack.threeBodyInputData[2][0])
        stack.threeBody_input_2.setCurrentIndex(itemIndex)

        itemIndex = stack.threeBody_input_3.findText(stack.threeBodyInputData[3][0])
        stack.threeBody_input_3.setCurrentIndex(itemIndex)

        stack.threeBody_input_4.setText(stack.threeBodyInputData[4][0])
        stack.threeBody_input_5.setText(stack.threeBodyInputData[5][0])

        if stack.threeBodyInputData[6][0] == 'CW':
            stack.threeBody_input_6_CW.setChecked(True)
        else:
            stack.threeBody_input_6_CCW.setChecked(True)

        stack.threeBody_input_7.setText(stack.threeBodyInputData[7][0])
        stack.threeBody_input_8.setText(stack.threeBodyInputData[8][0])

        if stack.threeBodyInputData[9][0] == 'CW':
            stack.threeBody_input_9_CW.setChecked(True)
        else:
            stack.threeBody_input_9_CCW.setChecked(True)

        stack.threeBody_input_10.setText(stack.threeBodyInputData[10][0])
        stack.threeBody_input_11.setText(stack.threeBodyInputData[11][0])
        stack.threeBody_input_12.setText(stack.threeBodyInputData[12][0])
        stack.threeBody_input_13.setText(stack.threeBodyInputData[12][1])
        stack.threeBody_input_14.setText(stack.threeBodyInputData[12][2])
        stack.threeBody_input_15.setText(stack.threeBodyInputData[13][0])
        stack.threeBody_input_16.setText(stack.threeBodyInputData[13][1])
        stack.threeBody_input_17.setText(stack.threeBodyInputData[13][2])

        itemIndex = stack.threeBody_input_18.findText(stack.threeBodyInputData[14][0])
        stack.threeBody_input_18.setCurrentIndex(itemIndex)

        stack.threeBody_input_19.setText(stack.threeBodyInputData[14][1])
        stack.threeBody_input_20.setText(stack.threeBodyInputData[15][0])

        # formation
        itemIndex = stack.formation_input_0.findText(stack.formationFrameInputData[0][0])
        stack.formation_input_0.setCurrentIndex(itemIndex)

        stack.formation_input_1.setText(stack.formationFrameInputData[1][0])
        stack.formation_input_2.setText(stack.formationFrameInputData[1][1])
        stack.formation_input_3.setText(stack.formationFrameInputData[1][2])

        itemIndex = stack.formation_input_4.findText(stack.formationFrameInputData[1][3])
        stack.formation_input_4.setCurrentIndex(itemIndex)

        itemIndex = stack.formation_input_5.findText(stack.formationFrameInputData[2][0])
        stack.formation_input_5.setCurrentIndex(itemIndex)

        stack.formation_input_6.setText(stack.formationFrameInputData[3][0])
        stack.formation_input_7.setText(stack.formationFrameInputData[3][1])
        stack.formation_input_8.setText(stack.formationFrameInputData[3][2])

        # zero tab
        itemIndex = stack.zero_input_0.findText(stack.zeroInputData[0][0])
        stack.zero_input_0.setCurrentIndex(itemIndex)

        if stack.zeroInputData[1][0] == 'TRUE':
            stack.zero_input_1_ON.setChecked(True)
        else:
            stack.zero_input_1_OFF.setChecked(True)

        # flight tab
        stack.flight_input_0.setText(stack.flightInputData[0][0])

        if stack.flightInputData[1][0] == 'TRUE':
            stack.flight_input_1_ON.setChecked(True)
        else:
            stack.flight_input_1_OFF.setChecked(True)

         # central tab
        stack.central_input_0.setText(stack.centralInputData[0][0])

        if stack.centralInputData[1][0] == 'TRUE':
            stack.central_input_1_ON.setChecked(True)
        else:
            stack.central_input_1_OFF.setChecked(True)

        itemIndex = stack.central_input_2.findText(stack.centralInputData[2][0])
        stack.central_input_2.setCurrentIndex(itemIndex)

        itemIndex = stack.central_input_3.findText(stack.centralInputData[3][0])
        stack.central_input_3.setCurrentIndex(itemIndex)

        stack.central_input_4.setText(stack.centralInputData[4][0])
        stack.central_input_5.setText(stack.centralInputData[4][1])
        stack.central_input_6.setText(stack.centralInputData[5][0])
        stack.central_input_7.setText(stack.centralInputData[5][1])
        stack.central_input_8.setText(stack.centralInputData[6][0])
        stack.central_input_9.setText(stack.centralInputData[7][0])
        stack.central_input_10.setText(stack.centralInputData[8][0])
        stack.central_input_11.setText(stack.centralInputData[9][0])
        stack.central_input_12.setText(stack.centralInputData[10][0])
        stack.central_input_13.setText(stack.centralInputData[10][1])
        stack.central_input_14.setText(stack.centralInputData[10][2])
        stack.central_input_15.setText(stack.centralInputData[11][0])
        stack.central_input_16.setText(stack.centralInputData[11][1])
        stack.central_input_17.setText(stack.centralInputData[11][2])

        itemIndex = stack.central_input_18.findText(stack.centralInputData[12][0])
        stack.central_input_18.setCurrentIndex(itemIndex)

        stack.central_input_19.setText(stack.centralInputData[13][0])
        stack.central_input_20.setText(stack.centralInputData[14][0])

         # Three body tab
        itemIndex = stack.threeBody_input_0.findText(stack.threeBodyInputData[0][0])
        stack.threeBody_input_0.setCurrentIndex(itemIndex)

        itemIndex = stack.threeBody_input_1.findText(stack.threeBodyInputData[1][0])
        stack.threeBody_input_1.setCurrentIndex(itemIndex)

        itemIndex = stack.threeBody_input_2.findText(stack.threeBodyInputData[2][0])
        stack.threeBody_input_2.setCurrentIndex(itemIndex)

        itemIndex = stack.threeBody_input_3.findText(stack.threeBodyInputData[3][0])
        stack.threeBody_input_3.setCurrentIndex(itemIndex)

        stack.threeBody_input_4.setText(stack.threeBodyInputData[4][0])
        stack.threeBody_input_5.setText(stack.threeBodyInputData[5][0])

        if stack.threeBodyInputData[6][0] == 'CW':
            stack.threeBody_input_6_CW.setChecked(True)
        else:
            stack.threeBody_input_6_CCW.setChecked(True)

        stack.threeBody_input_7.setText(stack.threeBodyInputData[7][0])
        stack.threeBody_input_8.setText(stack.threeBodyInputData[8][0])

        if stack.threeBodyInputData[9][0] == 'CW':
            stack.threeBody_input_9_CW.setChecked(True)
        else:
            stack.threeBody_input_9_CCW.setChecked(True)

        stack.threeBody_input_10.setText(stack.threeBodyInputData[10][0])
        stack.threeBody_input_11.setText(stack.threeBodyInputData[11][0])
        stack.threeBody_input_12.setText(stack.threeBodyInputData[12][0])
        stack.threeBody_input_13.setText(stack.threeBodyInputData[12][1])
        stack.threeBody_input_14.setText(stack.threeBodyInputData[12][2])
        stack.threeBody_input_15.setText(stack.threeBodyInputData[13][0])
        stack.threeBody_input_16.setText(stack.threeBodyInputData[13][1])
        stack.threeBody_input_17.setText(stack.threeBodyInputData[13][2])

        itemIndex = stack.threeBody_input_18.findText(stack.threeBodyInputData[14][0])
        stack.threeBody_input_18.setCurrentIndex(itemIndex)

        stack.threeBody_input_19.setText(stack.threeBodyInputData[14][1])
        stack.threeBody_input_20.setText(stack.threeBodyInputData[15][0])

    def write_to_file_slot(self,stackIndex):
        for row in range(self.orbitList.count()):
            file = self.orbitList.item(row).text()
            self.save_data_slot(row)
            self.writeFile = open(self.missionDir + '/InOut/' + file,'w+')
            self.writeFile.writelines(self.saveData)
            self.writeFile.close()

    def save_data_slot(self,stackIndex):
        empty_space = '                              '
        stack = self.stackedWidget.widget(stackIndex)
        self.saveData = [""]*300

        txt_idx = 0
        self.saveData[txt_idx] = '<<<<<<<<<<<<<<<<<  42: Orbit Description File   >>>>>>>>>>>>>>>>>\n'

        txt_idx += 1
        data_inp = stack.general_input_0.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Description\n'

        txt_idx += 1
        data_inp = stack.general_input_1.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Orbit Type (ZERO, FLIGHT, CENTRAL, THREE_BODY)\n'

        txt_idx += 1
        self.saveData[txt_idx] = '::::::::::::::  Use these lines if ZERO           :::::::::::::::::\n'

        txt_idx += 1
        data_inp = stack.zero_input_0.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! World\n'

        txt_idx += 1
        if stack.zero_input_1_ON.isChecked() == True:
            data = 'TRUE'
        else:
            data = 'FALSE'
        data_inp = data
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Use Polyhedron Gravity\n'

        txt_idx += 1
        self.saveData[txt_idx] = '::::::::::::::  Use these lines if FLIGHT         :::::::::::::::::\n'

        txt_idx += 1
        data_inp = stack.flight_input_0.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Region Number\n'

        txt_idx += 1
        if stack.flight_input_1_ON.isChecked() == True:
            data = 'TRUE'
        else:
            data = 'FALSE'
        data_inp = data
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Use Polyhedron Gravity\n'

        txt_idx += 1
        self.saveData[txt_idx] = '::::::::::::::  Use these lines if CENTRAL        :::::::::::::::::\n'

        txt_idx += 1
        data_inp = stack.central_input_0.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Orbit Center\n'

        txt_idx += 1
        if stack.central_input_1_ON.isChecked() == True:
            data = 'TRUE'
        else:
            data = 'FALSE'
        data_inp = data
        data_len = len(empty_space)-len(data_inp)
        if data_len < 1: data_len = 1
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Secular Orbit Drift Due to J2\n'

        txt_idx += 1
        data_inp = stack.central_input_2.currentText()
        self.saveData[txt_idx] =  functions.whitespace(data_inp) + '! Use Keplerian elements (KEP) or (RV) or FILE\n'

        txt_idx += 1
        data_inp = stack.central_input_3.currentText()
        self.saveData[txt_idx] =  functions.whitespace(data_inp) + '! Use Peri/Apoapsis (PA) or min alt/ecc (AE)\n'

        txt_idx += 1
        data_inp = stack.central_input_4.text() + '  ' + stack.central_input_5.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Periapsis & Apoapsis Altitude, km\n'

        txt_idx += 1
        data_inp = stack.central_input_6.text() + '  ' + stack.central_input_7.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Min Altitude (km), Eccentricity\n'

        txt_idx += 1
        data_inp = stack.central_input_8.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Inclination (deg)\n'

        txt_idx += 1
        data_inp = stack.central_input_9.text()
        data_len = len(empty_space)-len(data_inp)
        if data_len < 1: data_len = 1
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Right Ascension of Ascending Node (deg)\n'

        txt_idx += 1
        data_inp = stack.central_input_10.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Argument of Periapsis (deg)\n'

        txt_idx += 1
        data_inp = stack.central_input_11.text()
        self.saveData[txt_idx] =  functions.whitespace(data_inp) + '! True Anomaly (deg)\n'

        txt_idx += 1
        data_inp = stack.central_input_12.text() + '  ' + stack.central_input_13.text() + '  ' + stack.central_input_14.text()
        self.saveData[txt_idx] =  functions.whitespace(data_inp) + '! RV Initial Position (km)\n'

        txt_idx += 1
        data_inp = stack.central_input_15.text() + '  ' + stack.central_input_16.text() + '  ' + stack.central_input_17.text()
        self.saveData[txt_idx] =  functions.whitespace(data_inp) + '! RV Initial Velocity (km/sec)\n'

        txt_idx += 1
        data_inp = stack.central_input_18.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! TLE, TRV, or SPLINE file format\n'

        txt_idx += 1
        data_inp = '"' + stack.central_input_19.text() + '"'
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! File name\n'

        txt_idx += 1
        data_inp = '"' + stack.central_input_20.text() + '"'
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Label to find in TLE or TRV file\n'

        txt_idx += 1
        self.saveData[txt_idx] = ':::::::::::::  Use these lines if THREE_BODY      ::::::::::::::::\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_0.currentText()
        data_len = len(empty_space)-len(data_inp)
        if data_len < 1: data_len = 1
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Lagrange system\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_1.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Propagate using LAGDOF_MODES or LAGDOF_COWELL or LAGDOF_SPLINE\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_2.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Initialize with MODES or XYZ or FILE\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_3.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Libration point (L1, L2, L3, L4, L5)\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_4.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! XY Semi-major axis, km\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_5.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Initial XY Phase, deg  (CCW from -Y)\n'

        txt_idx += 1
        if stack.threeBody_input_6_CW.isChecked() == True:
            data = 'CW'
        else:
            data = 'CCW'
        data_inp = data
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Sense (CW, CCW), viewed from +Z\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_7.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Second XY Mode Semi-major Axis, km (L4, L5 only)\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_8.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Second XY Mode Initial Phase, deg (L4, L5 only)\n'

        txt_idx += 1
        if stack.threeBody_input_9_CW.isChecked() == True:
            data = 'CW'
        else:
            data = 'CCW'
        data_inp = data
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Sense (CW, CCW), viewed from +Z (L4, L5 only)\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_10.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Z Semi-axis, km\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_11.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Initial Z Phase, deg\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_12.text() +  '  ' + stack.threeBody_input_13.text() +  '  ' + stack.threeBody_input_14.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Initial X, Y, Z (Non-dimensional)\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_15.text() +  '  ' + stack.threeBody_input_16.text() +  '  ' + stack.threeBody_input_17.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Initial Xdot, Ydot, Zdot (Non-dimensional)\n'

        txt_idx += 1
        data_inp = stack.threeBody_input_18.currentText() + '  ' + '"' + stack.threeBody_input_19.text() + '"'
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! TLE, TRV or SPLINE format, Label to find in file\n'

        txt_idx += 1
        data_inp = '"' + stack.threeBody_input_20.text() + '"'
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! File name\n'

        txt_idx += 1
        self.saveData[txt_idx] = '******************* Formation Frame Parameters ************************\n'

        txt_idx += 1
        data_inp = stack.formation_input_0.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Formation Frame Fixed in [NL]\n'

        txt_idx += 1
        data_inp = stack.formation_input_1.text() + '  ' + stack.formation_input_2.text() + '  ' + stack.formation_input_3.text() + '  ' + stack.formation_input_4.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Euler Angles (deg) and Sequence\n'

        txt_idx += 1
        data_inp = stack.formation_input_5.currentText()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Formation Origin expressed in [NL]\n'

        txt_idx += 1
        data_inp = stack.formation_input_6.text() + '  ' + stack.formation_input_7.text() + '  ' + stack.formation_input_8.text()
        self.saveData[txt_idx] = functions.whitespace(data_inp) + '! Formation Origin wrt Ref Orbit (m)\n'

    def signal_to_slot(self):
        self.orbitList.currentRowChanged.connect(lambda:functions.display_stack_slot(self.stackedWidget, self.orbitList.currentRow()))
        # self.orbitList.itemDoubleClicked.connect(self.edit_list_item)
        self.applyBtn.clicked.connect(self.write_to_file_slot)
        self.cancelBtn.clicked.connect(self.close)
        self.addBtn.clicked.connect(self.add_orbit_slot)
        self.removeBtn.clicked.connect(self.remove_orbit_slot)
        self.defaultBtn.clicked.connect(self.default_orbit_slot)
        self.upddefaultBtn.clicked.connect(self.upddefault_orbit_slot)

    def default_orbit_slot(self):
        response = functions.create_warning_message(self,"Overwrite orbit files?")
        if response == QMessageBox.Ok:
            source = self.missionDir + '/InOut/__default__/'
            file_list = os.listdir(source)
            destination = self.missionDir + '/InOut/'
            for files in file_list:
                if files.startswith("Orb_") and files.endswith(".txt"):
                    shutil.copy(source+'/'+files,destination)
            overwriteNotice = functions.create_notice(self,"Overwrite successful.")
            overwriteNotice.exec_()
            self.updateRefOrbitList.emit()

    def upddefault_orbit_slot(self):
        response = functions.create_warning_message(self,"Overwrite original orbit files?")
        if response == QMessageBox.Ok:
            source = self.missionDir + '/InOut/'
            file_list = os.listdir(source)
            destination = self.missionDir + '/InOut/__default__/'
            for files in file_list:
                if files.startswith("Orb_") and files.endswith(".txt"):
                    shutil.copy(source+'/'+files,destination)
            overwriteNotice = functions.create_notice(self,"Default update successful.")
            overwriteNotice.exec_()
            self.updateRefOrbitList.emit()

    def add_orbit_slot(self):
        itemIndex = self.orbitList.count()
        self.orbitList.insertItem(int(itemIndex), "Orb_%s.txt"%itemIndex)
        self.init_stack_ui("Orb_%s.txt"%itemIndex)
        self.updateRefOrbitList.emit()

    def remove_orbit_slot(self):
        itemIndex = self.orbitList.currentRow()
        self.orbitList.takeItem(itemIndex)
        self.stackedWidget.widget(itemIndex)
        self.updateRefOrbitList.emit()

    # def edit_list_item(self,item):
    #     item.setFlags(Qt.ItemIsEditable)
    #     self.orbitList.editItem(item)

class OrbitTab(QWidget):
    def __init__(self,parent):
        #super(QWidget, self).__init__(parent)
        super().__init__()
        self.setFixedSize(700,600)
        mainLayout = QVBoxLayout()

        # create tabs
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tab3 = QWidget()
        self.tab4 = QWidget()
        self.tab5 = QWidget()

        # add tabs
        self.tabs.addTab(self.tab1, 'General')
        self.tabs.addTab(self.tab2, 'Zero')
        self.tabs.addTab(self.tab3, 'Flight')
        self.tabs.addTab(self.tab4, 'Central')
        self.tabs.addTab(self.tab5, 'Three Body')

        # init tab UI
        self.init_general_tab_UI()
        self.init_zero_tab_UI()
        self.init_flight_tab_UI()
        self.init_central_tab_UI()
        self.init_three_body_tab_UI()

        mainLayout.addWidget(self.tabs)
        self.setLayout(mainLayout)

    def init_general_tab_UI(self):
        tabLayout = QFormLayout()
        tabLayout.setLabelAlignment(Qt.AlignLeft)
        tabLayout.setFormAlignment(Qt.AlignLeft)

        tabLabels = ['Description', 'Orbit Type', 'Formation Frame Parameters',
        'Formation Frame Fixed in [NL]','Euler Angles [deg], Sequence',
        'Formation Origin expressed in [NL]', 'Formation Origin wrt Reg Orbit [m]']

        self.labelList = functions.generate_label_widgets(tabLabels)

        self.general_input_0 = QLineEdit() # desc

        self.general_input_1 = QComboBox() # orbit type
        self.general_input_1.addItems(['ZERO','FLIGHT','CENTRAL','THREE_BODY'])

        self.formationSectionHeader = QLabel('Formation Frame Parameters')
        self.formationSectionHeader.setAlignment(Qt.AlignCenter)
        self.formationSectionHeader.setStyleSheet('font-size: 20px')

        self.formation_input_0 = QComboBox() # formation frame
        self.formation_input_0.addItems(['N','L'])
        self.formation_input_1 = QLineEdit() # formation euler angle 1
        self.formation_input_2 = QLineEdit() # formation euler angle 2
        self.formation_input_3 = QLineEdit() # formation euler angle 3

        self.formation_input_4 = QComboBox() # formation euler angle sequence
        self.formation_input_4.addItems(['121','123','131','132','212','213','231','232','312','313','321','323'])
        self.formation_input_5 = QComboBox() # formation origin
        self.formation_input_5.addItems(['N','L'])
        self.formation_input_6 = QLineEdit() # formation origin wrt ref orbit 1
        self.formation_input_7 = QLineEdit() # formation origin wrt ref orbit 2
        self.formation_input_8 = QLineEdit() # formation origin wrt ref orbit 3

        eulerAnglesLayout = QHBoxLayout()
        eulerAnglesLayout.addWidget(self.formation_input_1)
        eulerAnglesLayout.addWidget(self.formation_input_2)
        eulerAnglesLayout.addWidget(self.formation_input_3)
        eulerAnglesLayout.addWidget(self.formation_input_4)

        formationOriginLayout = QHBoxLayout()
        formationOriginLayout.addWidget(self.formation_input_6)
        formationOriginLayout.addWidget(self.formation_input_7)
        formationOriginLayout.addWidget(self.formation_input_8)

        tabLayout.addRow(self.labelList[0],self.general_input_0)
        tabLayout.addRow(self.labelList[1],self.general_input_1)
        tabLayout.addRow(QLabel(''))
        tabLayout.addRow(self.formationSectionHeader)
        tabLayout.addRow(self.labelList[3],self.formation_input_0)
        tabLayout.addRow(self.labelList[4],eulerAnglesLayout)
        tabLayout.addRow(self.labelList[5],self.formation_input_5)
        tabLayout.addRow(self.labelList[6],formationOriginLayout)

        self.tab1.setLayout(tabLayout)

    def init_zero_tab_UI(self):
        tabLayout = QFormLayout()
        tabLayout.setLabelAlignment(Qt.AlignLeft)
        tabLayout.setFormAlignment(Qt.AlignLeft)

        tabLabels = ['World','Polyhedron Gravity']

        self.labelList = functions.generate_label_widgets(tabLabels)

        self.zero_input_0 = QComboBox()
        self.zero_input_0.addItems(BODY_LIST)
        self.zero_input_1_ON = QRadioButton('On')
        self.zero_input_1_OFF = QRadioButton('Off')

        self.polyGravBtnGroup = QButtonGroup()
        self.polyGravBtnGroup.addButton(self.zero_input_1_ON)
        self.polyGravBtnGroup.addButton(self.zero_input_1_OFF)

        polyGravLayout = QHBoxLayout()
        polyGravLayout.addWidget(self.zero_input_1_ON)
        polyGravLayout.addWidget(self.zero_input_1_OFF)

        tabLayout.addRow(self.labelList[0],self.zero_input_0)
        tabLayout.addRow(self.labelList[1],polyGravLayout)

        self.tab2.setLayout(tabLayout)

    def init_flight_tab_UI(self):
        tabLayout = QFormLayout()
        tabLayout.setLabelAlignment(Qt.AlignLeft)
        tabLayout.setFormAlignment(Qt.AlignLeft)

        tabLabels = ['Region Number','Polyhedron Gravity']

        self.labelList = functions.generate_label_widgets(tabLabels)

        self.flight_input_0 = QLineEdit()
        self.flight_input_1_ON = QRadioButton('On')
        self.flight_input_1_OFF = QRadioButton('Off')

        self.polyGravBtnGroup = QButtonGroup()
        self.polyGravBtnGroup.addButton(self.flight_input_1_ON)
        self.polyGravBtnGroup.addButton(self.flight_input_1_OFF)

        polyGravLayout = QHBoxLayout()
        polyGravLayout.addWidget(self.flight_input_1_ON)
        polyGravLayout.addWidget(self.flight_input_1_OFF)

        tabLayout.addRow(self.labelList[0],self.flight_input_0)
        tabLayout.addRow(self.labelList[1],polyGravLayout)

        self.tab3.setLayout(tabLayout)

    def init_central_tab_UI(self):
        tabLayout = QFormLayout()
        tabLayout.setLabelAlignment(Qt.AlignLeft)
        tabLayout.setFormAlignment(Qt.AlignLeft)

        tabLabels = ['Orbit Center','Secular Orbit Drit due to J2','Use Keplerian elements (KEP) or (RV) or FILE',
        'Peri/Apoapsis or min alt/ecc','Periapsis & Apoapsis Altitude [km]', 'Min Altitude [km], Eccentricity',
        'Inclination [deg]','Right Ascension of Ascending Node [deg]','Argument of Periapsis [deg]','True Anomaly [deg]',
        'RV Initial Velocity [km]','RV Initial Velocity [km/s]','File Format','File Name', 'Label']

        self.labelList = functions.generate_label_widgets(tabLabels)

        # create input widgets
        self.central_input_0 = QLineEdit()
        completer = QCompleter(BODY_LIST)
        completer.setCaseSensitivity(Qt.CaseInsensitive)
        self.central_input_0.setCompleter(completer)

        self.central_input_1_ON = QRadioButton('On')
        self.central_input_1_OFF = QRadioButton('Off')
        self.secularOrbitBtnGroup = QButtonGroup()
        self.secularOrbitBtnGroup.addButton(self.central_input_1_ON)
        self.secularOrbitBtnGroup.addButton(self.central_input_1_OFF)

        self.central_input_2 = QComboBox()
        self.central_input_2.addItems(['KEP', 'RV', 'FILE'])

        self.central_input_3 = QComboBox()
        self.central_input_3.addItems(['PA','AE'])

        self.central_input_4 = QLineEdit()
        self.central_input_5 = QLineEdit()
        self.central_input_6 = QLineEdit()
        self.central_input_7 = QLineEdit()
        self.central_input_8 = QLineEdit()
        self.central_input_9 = QLineEdit()
        self.central_input_10 = QLineEdit()
        self.central_input_11 = QLineEdit()
        self.central_input_12 = QLineEdit()
        self.central_input_13 = QLineEdit()
        self.central_input_14 = QLineEdit()
        self.central_input_15 = QLineEdit()
        self.central_input_16 = QLineEdit()
        self.central_input_17 = QLineEdit()

        self.central_input_18 = QComboBox()
        self.central_input_18.addItems(['TLE','TRV','SPLINE'])

        self.central_input_19 = QLineEdit()
        self.central_input_20 = QLineEdit()

        # create sub layouts for widgets
        secularLayout = QHBoxLayout()
        secularLayout.addWidget(self.central_input_1_ON)
        secularLayout.addWidget(self.central_input_1_OFF)

        periApoLayout = QHBoxLayout()
        periApoLayout.addWidget(self.central_input_4)
        periApoLayout.addWidget(self.central_input_5)

        minEccLayout = QHBoxLayout()
        minEccLayout.addWidget(self.central_input_6)
        minEccLayout.addWidget(self.central_input_7)

        rvInitPosLayout = QHBoxLayout()
        rvInitPosLayout.addWidget(self.central_input_12)
        rvInitPosLayout.addWidget(self.central_input_13)
        rvInitPosLayout.addWidget(self.central_input_14)

        rvInitVelLayout = QHBoxLayout()
        rvInitVelLayout.addWidget(self.central_input_15)
        rvInitVelLayout.addWidget(self.central_input_16)
        rvInitVelLayout.addWidget(self.central_input_17)

        # add widgets and layouts to main tab layout
        tabLayout.addRow(self.labelList[0],self.central_input_0)
        tabLayout.addRow(self.labelList[1],secularLayout)
        tabLayout.addRow(self.labelList[2],self.central_input_2)
        tabLayout.addRow(self.labelList[3],self.central_input_3)
        tabLayout.addRow(self.labelList[4],periApoLayout)
        tabLayout.addRow(self.labelList[5],minEccLayout)
        tabLayout.addRow(self.labelList[6],self.central_input_8)
        tabLayout.addRow(self.labelList[7],self.central_input_9)
        tabLayout.addRow(self.labelList[8],self.central_input_10)
        tabLayout.addRow(self.labelList[9],self.central_input_11)
        tabLayout.addRow(self.labelList[10],rvInitPosLayout)
        tabLayout.addRow(self.labelList[11],rvInitVelLayout)
        tabLayout.addRow(self.labelList[12],self.central_input_18)
        tabLayout.addRow(self.labelList[13],self.central_input_19)
        tabLayout.addRow(self.labelList[14],self.central_input_20)

        self.tab4.setLayout(tabLayout)

    def init_three_body_tab_UI(self):
        tabLayout = QFormLayout()
        tabLayout.setLabelAlignment(Qt.AlignLeft)
        tabLayout.setFormAlignment(Qt.AlignLeft)

        tabLabels = ['Lagrange System','Propagation Method','Initialization Method',
        'Libration Point','XY Semi-major axis [km]', 'Initial XY Phase [deg] (CCW from -Y)',
        'Sense, viewed from +Z','Second XY Mode Semi-major Axis [km] (L4, L5 only)',
        'Second XY Mode Initial Phase [deg] (L4, L5 only)','Sense, viewed from +Z (L4, L5 only)',
        'Z Semi-axis [km]','Initial Z Phase [deg]','Initial X, Y, Z (Non-dimensional)',
        'Initial Xdot, Ydot, Zdot (Non-dimensional)','File Format, Label','File Name']

        self.labelList = functions.generate_label_widgets(tabLabels)

        # create input widgets
        self.threeBody_input_0 = QComboBox()
        self.threeBody_input_0.addItems(['EARTHMOON','SUNEARTH','SUNJUPITER'])

        self.threeBody_input_1 = QComboBox()
        self.threeBody_input_1.addItems(['LAGDOF_MODES','LAGDOF_COWELL','LAGDOF_SPLINE'])

        self.threeBody_input_2 = QComboBox()
        self.threeBody_input_2.addItems(['MODES','XYZ','FILE'])

        self.threeBody_input_3 = QComboBox()
        self.threeBody_input_3.addItems(['L1','L2','L3','L4','L5'])

        self.threeBody_input_4 = QLineEdit()
        self.threeBody_input_5 = QLineEdit()

        self.threeBody_input_6_CW = QRadioButton('CW')
        self.threeBody_input_6_CCW = QRadioButton('CCW')
        self.senseZ_btnGroup = QButtonGroup()
        self.senseZ_btnGroup.addButton(self.threeBody_input_6_CW)
        self.senseZ_btnGroup.addButton(self.threeBody_input_6_CCW)

        self.threeBody_input_7 = QLineEdit()
        self.threeBody_input_8 = QLineEdit()

        self.threeBody_input_9_CW = QRadioButton('CW')
        self.threeBody_input_9_CCW = QRadioButton('CCW')
        self.senseZ_L4L5_btnGroup = QButtonGroup()
        self.senseZ_L4L5_btnGroup.addButton(self.threeBody_input_9_CW)
        self.senseZ_L4L5_btnGroup.addButton(self.threeBody_input_9_CCW)

        self.threeBody_input_10 = QLineEdit()
        self.threeBody_input_11 = QLineEdit()
        self.threeBody_input_12 = QLineEdit()
        self.threeBody_input_13 = QLineEdit()
        self.threeBody_input_14 = QLineEdit()
        self.threeBody_input_15 = QLineEdit()
        self.threeBody_input_16 = QLineEdit()
        self.threeBody_input_17 = QLineEdit()

        self.threeBody_input_18 = QComboBox()
        self.threeBody_input_18.addItems(['TLE','TRV','SPLINE'])

        self.threeBody_input_19 = QLineEdit()
        self.threeBody_input_20 = QLineEdit()

        # create sub layouts for widgets
        senseZLayout = QHBoxLayout()
        senseZLayout.addWidget(self.threeBody_input_6_CW)
        senseZLayout.addWidget(self.threeBody_input_6_CCW)

        senseZ_L4L5_Layout = QHBoxLayout()
        senseZ_L4L5_Layout.addWidget(self.threeBody_input_9_CW)
        senseZ_L4L5_Layout.addWidget(self.threeBody_input_9_CCW)

        initXYZ_Layout = QHBoxLayout()
        initXYZ_Layout.addWidget(self.threeBody_input_12)
        initXYZ_Layout.addWidget(self.threeBody_input_13)
        initXYZ_Layout.addWidget(self.threeBody_input_14)

        initXYZ_dot_Layout = QHBoxLayout()
        initXYZ_dot_Layout.addWidget(self.threeBody_input_15)
        initXYZ_dot_Layout.addWidget(self.threeBody_input_16)
        initXYZ_dot_Layout.addWidget(self.threeBody_input_17)

        formatLabelLayout = QHBoxLayout()
        formatLabelLayout.addWidget(self.threeBody_input_18)
        formatLabelLayout.addWidget(self.threeBody_input_19)

        # add widgets and layouts to main tab layout
        tabLayout.addRow(self.labelList[0],self.threeBody_input_0)
        tabLayout.addRow(self.labelList[1],self.threeBody_input_1)
        tabLayout.addRow(self.labelList[2],self.threeBody_input_2)
        tabLayout.addRow(self.labelList[3],self.threeBody_input_3)
        tabLayout.addRow(self.labelList[4],self.threeBody_input_4)
        tabLayout.addRow(self.labelList[5],self.threeBody_input_5)
        tabLayout.addRow(self.labelList[6],senseZLayout)
        tabLayout.addRow(self.labelList[7],self.threeBody_input_7)
        tabLayout.addRow(self.labelList[8],self.threeBody_input_8)
        tabLayout.addRow(self.labelList[9],senseZ_L4L5_Layout)
        tabLayout.addRow(self.labelList[10],self.threeBody_input_10)
        tabLayout.addRow(self.labelList[11],self.threeBody_input_11)
        tabLayout.addRow(self.labelList[12],initXYZ_Layout)
        tabLayout.addRow(self.labelList[13],initXYZ_dot_Layout)
        tabLayout.addRow(self.labelList[14],formatLabelLayout)
        tabLayout.addRow(self.labelList[15],self.threeBody_input_20)

        self.tab5.setLayout(tabLayout)

    def get_data_input(self,fileName):
        self.generalInputData = []
        self.zeroInputData = []
        self.flightInputData = []
        self.centralInputData = []
        self.threeBodyInputData = []
        self.formationFrameInputData = []

        INPUT_PATTERN = re.compile("(.*?)\s*!")
        ORBIT_TYPE_FLAG = re.compile('^\::')
        FORMATION_FRAME_FLAG = re.compile('^\*')

        orbitTypeCheck = 0

        for i, line in enumerate(open(fileName)):

            if ORBIT_TYPE_FLAG.match(line) or FORMATION_FRAME_FLAG.match(line):
                orbitTypeCheck += 1

            for match in re.finditer(INPUT_PATTERN, line):
                inputData = match.group(1).replace('"',"").split()

                if orbitTypeCheck == 0:
                    self.generalInputData.append(inputData)

                if orbitTypeCheck == 1:
                    self.zeroInputData.append(inputData)

                if orbitTypeCheck == 2:
                    self.flightInputData.append(inputData)

                if orbitTypeCheck == 3:
                    self.centralInputData.append(inputData)

                if orbitTypeCheck == 4:
                    self.threeBodyInputData.append(inputData)

                if orbitTypeCheck == 5:
                    self.formationFrameInputData.append(inputData)
