# This file contains the code for the DSM window in the 42 gui

# Import Packages---------------------------------------------------------------
import functions
import DSM_Functions
import sys
import numpy as numpy
import re
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QLabel, QGridLayout,
QLineEdit, QPushButton, QComboBox, QVBoxLayout, QHBoxLayout, QRadioButton, QTableWidget,
QListWidget, QFormLayout, QStackedWidget, QTableWidgetItem, QCompleter, QTabWidget)
from PyQt5 import QtCore
from PyQt5.QtCore import (Qt, QObjectCleanupHandler)
from PyQt5.QtGui import QRegExpValidator

# global data used across subwindows
NUM_SPACECRAFT_CMDS = 5
SC_NUM_PATTERN = re.compile('SC[0-9]+',re.IGNORECASE)
GAIN_LABEL_PATTERN = re.compile('Set of Control Gains',re.IGNORECASE)
LIMIT_LABEL_PATTERN = re.compile('Set of Control Limits',re.IGNORECASE)
ACTUATOR_LABEL_PATTERN = re.compile('Actuator Mode',re.IGNORECASE)

GAIN_PATTERN = re.compile('Gains_[0-9]+',re.IGNORECASE)
LIMIT_PATTERN = re.compile('Limits_[0-9]+',re.IGNORECASE)
ACTUATOR_PATTERN = re.compile('Actuators_[0-9]+',re.IGNORECASE)


BODY_LIST = [
    'SOL','MERCURY','VENUS','EARTH','MARS','JUPITER','SATURN','URANUS',
    'NEPTUNE','PLUTO','LUNA','PHOBOS','DEIMOS','IO','EUROPA','GANYMEDE','CALLISTO',
    'AMALTHEA','HIMALITA','ELARA','PASIPHAE','SINOPE','LYSITHEA','CARME','ANANKE',
    'LEDA','THEBE','ADRASTEA','METIS','MIMAS','ENCELADUS','TETHYS','DIONE','RHEA',
    'TITAN','HYPERION','IAPETUS','PHOEBE','JANUS','EPIMETHEUS','HELENE','TELESTO',
    'CALYPSO','ATLAS','PROMETHEUS','PANDORA','PAN','ARIEL','UMBRIEL','UMBRIEL',
    'TITANIA','OBERON','MIRANDA','TRITON','NERIED','CHARON'
]

#-------------------------------------------------------------------------------
# DSM Main Menu ----------------------------------------------------------------
class DSM_Window(QWidget):
    updateScList = QtCore.pyqtSignal()
    def __init__(self):
        super().__init__()
        self.setWindowTitle('DSM Menu') # sets the window’s title
        self.setFixedSize(1040, 400) # gives window a fixed size

        self.missionDir = functions.get_mission_directory(self)
        inputFile = self.missionDir + '/InOut/Inp_DSM.txt'
        self.get_input_data(inputFile)

        self.dsmCmdTable = QTableWidget()
        self.dsmCmdTable.setColumnCount(NUM_SPACECRAFT_CMDS)
        tableHeaders = ['Spacecraft ID \n Number','Command Sequence \n Flag',
        'Command Sequence \n Parameter','Translation Control Method \n Mode',
        'Attitude Control Method \n Mode']
        self.dsmCmdTable.setHorizontalHeaderLabels(tableHeaders)
        self.dsmCmdTable.resizeColumnsToContents()
        self.dsmCmdTable.resizeRowsToContents()

        self.attitudeCmdMenuBtn = QPushButton('Attitude Command')
        self.translationCmdMenuBtn = QPushButton('Translation Command')
        self.ctrlParameterMenuBtn = QPushButton('Control Parameters')
        self.ctrlLimitMenuBtn = QPushButton('Control Limits')
        self.actuatorCtrlMenuBtn = QPushButton('Control Actuators')
        self.maneuverCmdMenuBtn = QPushButton('Maneuver Command')
        self.momentumDumpMenuBtn = QPushButton('Momentum Dump')
        self.addDsmCmdBtn = QPushButton('+')
        self.removeDsmCmdBtn = QPushButton('-')
        self.applyBtn = QPushButton('Apply')
        self.cancelBtn = QPushButton('Cancel')

        self.ctrlParameterMenu = CtrlParameterMenu()
        self.ctrlLimitMenu = CtrlLimitMenu()
        self.momentumDumpMenu = MomentumDumpMenu(self.ctrlParameterMenu)
        self.actuatorCtrlMenu = ActuatorCtrlMenu(self.momentumDumpMenu)
        self.translationCmdMenu = TranslationCmdMenu(self, self.ctrlParameterMenu,
        self.ctrlLimitMenu,self.actuatorCtrlMenu)
        self.attitudeCmdMenu = AttitudeCmdMenu(self,self.ctrlParameterMenu,
        self.ctrlLimitMenu,self.actuatorCtrlMenu)
        self.maneuverCmdMenu = ManeuverCmdlMenu(self.ctrlParameterMenu,
        self.ctrlLimitMenu,self.actuatorCtrlMenu)

        menuBtnLayout = QGridLayout()
        menuBtnLayout.addWidget(self.translationCmdMenuBtn,0,0)
        menuBtnLayout.addWidget(self.attitudeCmdMenuBtn,0,1)
        menuBtnLayout.addWidget(self.ctrlParameterMenuBtn,1,0)
        menuBtnLayout.addWidget(self.ctrlLimitMenuBtn,1,1)
        menuBtnLayout.addWidget(self.actuatorCtrlMenuBtn,2,0)
        menuBtnLayout.addWidget(self.maneuverCmdMenuBtn,2,1)
        menuBtnLayout.addWidget(self.momentumDumpMenuBtn,3,0)

        addRemoveBtnLayout = QHBoxLayout()
        addRemoveBtnLayout.addWidget(self.removeDsmCmdBtn)
        addRemoveBtnLayout.addWidget(self.addDsmCmdBtn)

        applyCancelBtnLayout = QHBoxLayout()
        applyCancelBtnLayout.addWidget(self.cancelBtn)
        applyCancelBtnLayout.addWidget(self.applyBtn)

        dsmLayout = QGridLayout()
        dsmLayout.addWidget(self.dsmCmdTable,0,0,3,2)
        dsmLayout.addLayout(addRemoveBtnLayout,3,1)
        dsmLayout.addLayout(menuBtnLayout,0,2)
        dsmLayout.addLayout(applyCancelBtnLayout,3,2)

        self.init_widget_field()

        self.signal_to_slot()

        self.setLayout(dsmLayout)

    def display_cmd_menu(self,menuType):
        if menuType == 1:
            self.translationCmdMenu.show()
        if menuType == 2:
            self.attitudeCmdMenu.show()
        if menuType == 3:
            self.ctrlParameterMenu.show()
        if menuType == 4:
            self.ctrlLimitMenu.show()
        if menuType == 5:
            self.actuatorCtrlMenu.show()
        if menuType == 6:
            self.maneuverCmdMenu.show()
        if menuType == 7:
            self.momentumDumpMenu.show()

    def add_dsm_cmd_slot(self):
        itemIndex = self.dsmCmdTable.rowCount()
        self.dsmCmdTable.setRowCount(itemIndex + 1)
        self.numCmds += 1
        # updates SC ID list
        self.SC_column_slot(itemIndex)
        # assigns new table configuration with SC_column_slot
        self.dsmCmdTable.itemChanged.connect(self.SC_column_slot)

    def remove_dsm_cmd_slot(self):
        itemIndex = self.dsmCmdTable.rowCount()
        self.dsmCmdTable.setRowCount(itemIndex - 1)
        self.numCmds -= 1
        # updates SC ID list
        self.SC_column_slot(itemIndex)
        # assigns new table configuration with SC_column_slot
        self.dsmCmdTable.itemChanged.connect(self.SC_column_slot)

    def init_widget_field(self):
        numCols = self.dsmCmdTable.columnCount()
        numRows = self.numCmds

        self.dsmCmdTable.setColumnCount(numCols)
        self.dsmCmdTable.setRowCount(numRows)

        for row in range(numRows):
            for column in range(numCols):
                if isinstance(self.inputData[row][column],str):
                    self.dsmCmdTable.setItem(row, column, QTableWidgetItem(self.inputData[row][column]))
                else:
                    self.dsmCmdTable.setItem(row, column, QTableWidgetItem(" "))

    def get_input_data(self,FileName):
        self.inputData = []
        self.SC = []

        NUM_DSM_INPUTS = 5
        val = "\s*([+-]?[a-zA-Z\_\.0-9]*)"
        pattern = "SC_Cmd"
        for col in range(NUM_DSM_INPUTS): pattern += val
        pattern = re.compile(pattern,re.IGNORECASE)

        for i, line in enumerate(open(FileName)):
            for match in re.finditer(pattern, line):
                matchData = []
                for group in match.groups():
                    if group != '': matchData.append(group)
                else: matchData.append(" ")
                self.inputData.append(matchData)
                self.SC.append(match.group(1))

        self.numCmds = len(self.inputData)
        self.SC = functions.get_unique_items(self.SC)

    def signal_to_slot(self):
        self.translationCmdMenuBtn.clicked.connect(lambda:self.display_cmd_menu(1))
        self.attitudeCmdMenuBtn.clicked.connect(lambda:self.display_cmd_menu(2))
        self.ctrlParameterMenuBtn.clicked.connect(lambda:self.display_cmd_menu(3))
        self.ctrlLimitMenuBtn.clicked.connect(lambda:self.display_cmd_menu(4))
        self.actuatorCtrlMenuBtn.clicked.connect(lambda:self.display_cmd_menu(5))
        self.maneuverCmdMenuBtn.clicked.connect(lambda:self.display_cmd_menu(6))
        self.momentumDumpMenuBtn.clicked.connect(lambda:self.display_cmd_menu(7))

        self.addDsmCmdBtn.clicked.connect(self.add_dsm_cmd_slot)
        self.removeDsmCmdBtn.clicked.connect(self.remove_dsm_cmd_slot)
        self.applyBtn.clicked.connect(self.save_data_slot)
        self.applyBtn.clicked.connect(self.sc_save_data_slot)
        self.applyBtn.clicked.connect(self.write_to_file_slot)
        self.dsmCmdTable.itemChanged.connect(self.SC_column_slot)

    # creates list of unique SC ID
    def SC_column_slot(self,cmd):
        trueSC = []
        for cmdNum in range(self.numCmds):
            cmd = self.dsmCmdTable.item(cmdNum,0)
            if not isinstance (cmd,type(None)):
                numSC = cmd.text()
                if SC_NUM_PATTERN.match(numSC):
                    trueSC.append(numSC)
                    if numSC not in self.SC:
                        self.SC.append(numSC)
                        self.SC.sort()
                        self.updateScList.emit()
            trueSC = functions.get_unique_items(trueSC)
            trueSC.sort()
            diffSC = list(set(self.SC) - set(trueSC))
            if diffSC:
                for SC in diffSC:
                    self.SC.remove(SC)
                self.updateScList.emit()

    def write_to_file_slot(self):
        self.totalSaveData = self.saveData + self.translationCmdMenu.saveData + \
        self.attitudeCmdMenu.saveData + self.actuatorCtrlMenu.saveData + \
        self.ctrlParameterMenu.saveData + self.ctrlLimitMenu.saveData + \
        self.maneuverCmdMenu.saveData + self.momentumDumpMenu.saveData

        self.WriteFile = open('InOut/Inp_DSM.txt', 'w+')
        self.WriteFile.writelines(self.totalSaveData)
        self.WriteFile.close()

        self.WriteFile = open('InOut/inp_Cmd.txt', 'w+')
        self.WriteFile.writelines(self.scSaveData)
        self.WriteFile.close()

    def save_data_slot(self):
        self.saveData = [""]*300
        self.saveData[0] = "<<<<<<<<<<<<<<<<<<<<<<<<  42:  DSM Command Script File  >>>>>>>>>>>>>>>>>>>>>>>>\n"
        self.saveData[1] = "#-------------------------------------------------------------------------------\n"
        self.saveData[2] = "#                             Spacecraft Command\n"
        self.saveData[3] = "#-------------------------------------------------------------------------------\n"
        self.saveData[4] = '# Col: 1   ->  Spacecraft ID Number: "SC#"\n'
        self.saveData[5] = '# Col: 2   ->  Command Sequence Flag: "FswMode" or "CmdTime"\n'
        self.saveData[6] = "# Col: 3   ->  Command Sequence Parameter: FSW Mode ID or Command Execution Time\n"
        self.saveData[7] = '# Col: 4   ->  Translation Control Method_Mode ("PASSIVE" for none)\n'
        self.saveData[8] = '# Col: 5   ->  Attitude Control Method_Mode ("PASSIVE" for none)\n'
        self.saveData[9] = "\n"

        txt_idx = 10
        for row in range(self.dsmCmdTable.rowCount()):
            self.saveData[txt_idx] = "SC_Cmd" + "   "
            for column in range(self.dsmCmdTable.columnCount()):
                data = self.dsmCmdTable.item(row,column+1)
                if isinstance(data,QTableWidgetItem):
                    self.saveData[txt_idx] += data.text() + "   "
                else:
                    self.saveData[txt_idx] += " "
            self.saveData[txt_idx] += "\n"
            txt_idx += 1

        self.saveData[txt_idx] += "End_Of_File\n"

    def sc_save_data_slot(self):
        self.scSaveData = [""]*300
        self.scSaveData[0] = "<<<<<<<<<<<<<<<<<<<<<<<<  42:  Command Script File  >>>>>>>>>>>>>>>>>>>>>>>>\n"
        txt_idx = 1
        numSCList = re.findall(r'\d+',' '.join(self.SC))
        for numSC in numSCList:
            self.scSaveData[txt_idx] = "0.0 SC[" + numSC + "] FswTag = DSM_FSW \n"
            txt_idx += 1
        self.scSaveData[txt_idx] = "EOF"


#-------------------------------------------------------------------------------
# Translation Cmd Menu ---------------------------------------------------------
class TranslationCmdMenu(QWidget):
    updateTranslationCmds = QtCore.pyqtSignal()
    def __init__(self,dsmMenu,ctrlParameterMenu,ctrlLimitMenu,actuatorMenu):
        super().__init__()
        self.setWindowTitle('Translation Command Menu')

        self.missionDir = functions.get_mission_directory(self)
        self.get_input_data(self.missionDir + '/InOut/Inp_DSM.txt')

        self.dsmMenu = dsmMenu
        self.ctrlParameterMenu = ctrlParameterMenu
        self.ctrlLimitMenu = ctrlLimitMenu
        self.actuatorMenu = actuatorMenu

        self.ctrlGainModes = functions.get_list_items(ctrlParameterMenu.cmdList)
        self.ctrlLimitModes = functions.get_list_items(ctrlLimitMenu.ctrlLimitList)
        self.actuatorModes = functions.get_list_items(actuatorMenu.cmdList)

        self.translationCmdList = QListWidget()
        self.stackedWidget = QStackedWidget()

        if self.numCmds == 0:
            self.numCmds += 1
            self.translationCmdList.insertItem(0, "TranslationCmd_%d"%0)
            self.init_translation_cmd_stack_ui(0)
        else:
            for i in range(self.numCmds):
                self.translationCmdList.insertItem(i, "TranslationCmd_%s"%i)
                self.init_translation_cmd_stack_ui(i)
                self.init_translation_cmd_widget_field(i)

        self.addRemoveBtnLayout = functions.remove_add_btns(self,['removeBtn','addBtn'])
        self.defaultSaveBtnLayout = functions.default_save_btns(self,['defaultBtn','saveBtn','saveDefaultBtn'])

        translationLayout = QGridLayout()
        translationLayout.addWidget(self.translationCmdList,0,0)
        translationLayout.addLayout(self.addRemoveBtnLayout,1,0)
        translationLayout.addLayout(self.defaultSaveBtnLayout,1,1)
        translationLayout.addWidget(self.stackedWidget,0,1)

        self.setLayout(translationLayout)

        self.save_data_slot()

        self.signal_to_slot()

    def init_translation_cmd_stack_ui(self,stackIndex):
        self.stack = QWidget()

        self.stack.posDirectionX = QLineEdit()
        self.stack.posDirectionY = QLineEdit()
        self.stack.posDirectionZ = QLineEdit()

        self.stack.refOriginType = QComboBox()
        self.stack.refOriginType.addItems(['Orbital Point','SC ID Number'])

        self.stack.refOriginSC = QComboBox()
        self.stack.refOriginSC.addItems(self.dsmMenu.SC)

        self.stack.refFrameDirections = QComboBox()
        self.stack.refFrameDirections.addItems(['N','F','L','SC ID Number'])

        self.stack.refFrameDirectionSC = QComboBox()
        self.stack.refFrameDirectionSC.addItems(self.dsmMenu.SC)

        self.stack.ctrlGainMode = QComboBox()
        self.stack.ctrlGainMode.addItems(self.ctrlGainModes)

        self.stack.ctrlLimitMode = QComboBox()
        self.stack.ctrlLimitMode.addItems(self.ctrlLimitModes)

        self.stack.actuatorMode = QComboBox()
        self.stack.actuatorMode.addItems(self.actuatorModes)

        columnLabels = [
        'Position 1: X (meters)','Position 2: Y (meters)','Position 3: Z (meters)',
        'Ref Origin',' Ref Frame Directions',
        'Set of Control Gains','Set of Control Limits','Actuator Mode'
        ]

        self.labelList = functions.generate_label_widgets(columnLabels)

        refOriginLayout = QHBoxLayout()
        refOriginLayout.addWidget(self.stack.refOriginType)
        refOriginLayout.addWidget(self.stack.refOriginSC)

        refFrameDirLayout = QHBoxLayout()
        refFrameDirLayout.addWidget(self.stack.refFrameDirections)
        refFrameDirLayout.addWidget(self.stack.refFrameDirectionSC)

        stackLayout = QFormLayout()
        stackLayout.setLabelAlignment(Qt.AlignLeft)
        stackLayout.setFormAlignment(Qt.AlignLeft)
        stackLayout.addRow(self.labelList[0],self.stack.posDirectionX)
        stackLayout.addRow(self.labelList[1],self.stack.posDirectionY)
        stackLayout.addRow(self.labelList[2],self.stack.posDirectionZ)
        stackLayout.addRow(self.labelList[3],refOriginLayout)
        stackLayout.addRow(self.labelList[4],refFrameDirLayout)
        stackLayout.addRow(self.labelList[5],self.stack.ctrlGainMode)
        stackLayout.addRow(self.labelList[6],self.stack.ctrlLimitMode)
        stackLayout.addRow(self.labelList[7],self.stack.actuatorMode)

        self.stack.setLayout(stackLayout)

        self.stackedWidget.addWidget(self.stack)

        self.display_refOriginSc(stackIndex)
        self.display_refFrameDirectionSC(stackIndex)

        self.stackedWidget.widget(stackIndex).refOriginType.currentIndexChanged.connect(
        lambda:self.display_refOriginSc(stackIndex)
        )
        self.stackedWidget.widget(stackIndex).refFrameDirections.currentIndexChanged.connect(
        lambda:self.display_refFrameDirectionSC(stackIndex)
        )

    def display_refOriginSc(self,stackIndex):
            stack = self.stackedWidget.widget(stackIndex)
            if (stack.refOriginType.currentText() == 'SC ID Number'):
                stack.refOriginSC.show()
            else:
                stack.refOriginSC.hide()

    def display_refFrameDirectionSC(self,stackIndex):
            stack = self.stackedWidget.widget(stackIndex)
            if (stack.refFrameDirections.currentText() == 'SC ID Number'):
                stack.refFrameDirectionSC.show()
            else:
                stack.refFrameDirectionSC.hide()

    def update_sc_options(self):
        for i in range(self.translationCmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.refOriginSC.clear()
            stack.refFrameDirectionSC.clear()
            stack.refOriginSC.addItems(self.dsmMenu.SC)
            stack.refFrameDirectionSC.addItems(self.dsmMenu.SC)

    def update_actuator_options(self):
        self.actuatorModes = functions.get_list_items(self.actuatorMenu.cmdList)
        for i in range(self.translationCmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.actuatorMode.clear()
            stack.actuatorMode.addItems(self.actuatorModes)

    def update_ctrl_limit_options(self):
        self.ctrlLimitModes = functions.get_list_items(self.ctrlLimitMenu.ctrlLimitList)
        for i in range(self.translationCmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.ctrlLimitMode.clear()
            stack.ctrlLimitMode.addItems(self.ctrlLimitModes)

    def update_ctrl_parameter_options(self):
        self.ctrlGainModes = functions.get_list_items(self.ctrlParameterMenu.cmdList)
        for i in range(self.translationCmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.ctrlGainMode.clear()
            stack.ctrlGainMode.addItems(self.ctrlGainModes)

    def init_translation_cmd_widget_field(self,stackIndex):
        stack = self.stackedWidget.widget(stackIndex)

        stack.posDirectionX.setText(self.inputData[stackIndex][0])
        stack.posDirectionY.setText(self.inputData[stackIndex][1])
        stack.posDirectionZ.setText(self.inputData[stackIndex][2])

        if re.match(SC_NUM_PATTERN,self.inputData[stackIndex][3]):
            itemIndex = stack.refOriginType.findText('SC ID Number')
            stack.refOriginType.setCurrentIndex(itemIndex)
            itemIndex = stack.refOriginSC.findText(self.inputData[stackIndex][3])
            stack.refOriginSC.setCurrentIndex(itemIndex)
        else:
            itemIndex = stack.refOriginType.findText('Orbital Point')
            stack.refOriginType.setCurrentIndex(itemIndex)

        if re.match(SC_NUM_PATTERN,self.inputData[stackIndex][4]):
            itemIndex = stack.refFrameDirectionSC.findText(self.inputData[stackIndex][4])
            stack.refFrameDirectionSC.setCurrentIndex(itemIndex)
            itemIndex = stack.refFrameDirections.findText('SC ID Number')
            stack.refFrameDirections.setCurrentIndex(itemIndex)
        else:
            itemIndex = stack.refFrameDirections.findText(self.inputData[stackIndex][4])
            stack.refFrameDirections.setCurrentIndex(itemIndex)

        itemIndex = stack.ctrlGainMode.findText(self.inputData[stackIndex][5])
        stack.ctrlGainMode.setCurrentIndex(itemIndex)
        itemIndex = stack.ctrlLimitMode.findText(self.inputData[stackIndex][6])
        stack.ctrlLimitMode.setCurrentIndex(itemIndex)
        itemIndex = stack.actuatorMode.findText(self.inputData[stackIndex][7])
        stack.actuatorMode.setCurrentIndex(itemIndex)

        self.display_refOriginSc(stackIndex)
        self.display_refFrameDirectionSC(stackIndex)

    def signal_to_slot(self):
        self.translationCmdList.currentRowChanged.connect(
        lambda:functions.display_stack_slot(self.stackedWidget,self.translationCmdList.currentRow())
        )
        self.removeBtn.clicked.connect(lambda:functions.remove_stack_slot(self.stackedWidget,self.translationCmdList,self.updateTranslationCmds))
        self.addBtn.clicked.connect(
        lambda:functions.add_stack_slot(self.stackedWidget,self.translationCmdList,self.init_translation_cmd_stack_ui,'TranslationCmd',self.updateTranslationCmds)
        )
        self.saveBtn.clicked.connect(self.save_data_slot)
        self.defaultBtn.clicked.connect(self.default_data_slot)
        self.dsmMenu.updateScList.connect(self.update_sc_options)
        self.actuatorMenu.updateActuatorModes.connect(self.update_actuator_options)
        self.ctrlLimitMenu.updateLimitModes.connect(self.update_ctrl_limit_options)
        self.ctrlParameterMenu.updateGainModes.connect(self.update_ctrl_parameter_options)

    def get_input_data(self,FileName):
        self.inputData = []

        NUM_TRANSLATION_INPUTS = 8
        val = "\s*([+-]?[a-zA-Z\_\.0-9]*)"
        pattern = "TranslationCmd_[0-9]+"
        for col in range(NUM_TRANSLATION_INPUTS): pattern += val
        pattern = re.compile(pattern,re.IGNORECASE)

        for i, line in enumerate(open(FileName)):
            for match in re.finditer(pattern, line):
                if match.group(3) != "": # checks if real translation cmd
                    matchData = []
                    for group in match.groups():
                        if group != '': matchData.append(group) # check if data is empty
                        else: matchData.append(" ")
                    self.inputData.append(matchData)

        self.numCmds = len(self.inputData)

    def save_data_slot(self):
        self.saveData = [""]*300
        self.saveData[0] = "#-------------------------------------------------------------------------------\n"
        self.saveData[1] = "#                             Translation Command\n"
        self.saveData[2] = "#-------------------------------------------------------------------------------\n"
        self.saveData[3] = "# Col: 1   ->  Command Interp ID Flag (hard coded in interpreter)\n"
        self.saveData[4] = "# Col: 2   ->  Position 1: X (meters)\n"
        self.saveData[5] = "# Col: 3   ->  Position 2: Y (meters)\n"
        self.saveData[6] = "# Col: 4   ->  Position 3: Z (meters)\n"
        self.saveData[7] = '# Col: 5   ->  Ref Origin (ctrl wrt): "F" or SC ID Number ("SC#")\n'
        self.saveData[8] = '# Col: 6   ->  Ref Frame Directions: "N", "F", "L", or SC ID Number ("SC#")\n'
        self.saveData[9] = "# Col: 7   ->  Set of Control Gains\n"
        self.saveData[10] = "# Col: 8   ->  Set of Control Limits\n"
        self.saveData[11] = "# Col: 9  ->  Actuator Mode\n"
        self.saveData[12] = "\n"

        txt_idx = 13
        for i in range(self.translationCmdList.count()):
            stack = self.stackedWidget.widget(i)
            col_1 = stack.posDirectionX.text()
            col_2 = stack.posDirectionY.text()
            col_3 = stack.posDirectionZ.text()

            if (stack.refOriginType.currentText() == 'SC ID Number'):
                col_4 = stack.refOriginSC.currentText()
            else:
                col_4 = "OP"

            if (stack.refFrameDirections.currentText() == 'SC ID Number'):
                col_5 = stack.refFrameDirectionSC.currentText()
            else:
                col_5 = stack.refFrameDirections.currentText()

            col_6 = stack.ctrlGainMode.currentText()
            col_7 = stack.ctrlLimitMode.currentText()
            col_8 = stack.actuatorMode.currentText()

            self.saveData[txt_idx] = "TranslationCmd_%s"%i + "   " + col_1 \
            + "   " + col_2 + "   " + col_3 + "   " + col_4 + "   " + col_5 \
            + "   " + col_6  + "   " + col_7  + "   " + col_8 + "\n"

            txt_idx += 1

    def default_data_slot(self):
        self.get_input_data('InOut/Default/Inp_DSM_Default.txt')
        functions.default_data(self,self.stackedWidget,self.translationCmdList,
         self.numCmds, self.init_translation_cmd_stack_ui(), init_translation_cmd_widget_field(),
          'TranslationCmd_')

#-------------------------------------------------------------------------------
# Attitude Cmd Menu ------------------------------------------------------------
class AttitudeCmdMenu(QMainWindow):
    updatePrimaryAxisCmds = QtCore.pyqtSignal()
    updateSecondaryAxisCmds = QtCore.pyqtSignal()
    updateQuaternionCmds = QtCore.pyqtSignal()
    updateMirrorCmds = QtCore.pyqtSignal()

    def __init__(self,dsmMenu,ctrlParameterMenu,ctrlLimitMenu,actuatorMenu):
        super().__init__()
        self.setWindowTitle('Attitude Command Menu')

        self.missionDir = functions.get_mission_directory(self)
        inputFile = self.missionDir + '/InOut/Inp_DSM.txt'
        self.get_input_data(inputFile)

        # other class attributes
        self.dsmMenu = dsmMenu
        self.ctrlParameterMenu = ctrlParameterMenu
        self.ctrlLimitMenu = ctrlLimitMenu
        self.actuatorMenu = actuatorMenu
        self.ctrlGainModes = functions.get_list_items(ctrlParameterMenu.cmdList)
        self.ctrlLimitModes = functions.get_list_items(ctrlLimitMenu.ctrlLimitList)
        self.actuatorModes = functions.get_list_items(actuatorMenu.cmdList)

        # initialize tab screen
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tab3 = QWidget()
        self.tab4 = QWidget()

        # Add tabs
        self.tabs.addTab(self.tab1,'Primary Axis')
        self.tabs.addTab(self.tab2,'Secondary Axis')
        self.tabs.addTab(self.tab3,'Quaternion Attitude')
        self.tabs.addTab(self.tab4,'Mirror Attitude')

        self.setCentralWidget(self.tabs)

        # Primary Axis Cmd Tab -------------------------------------------------
        tab1_layout = QGridLayout()

        self.primaryAxisCmdList = QListWidget()
        self.primaryStackedWidget = QStackedWidget()

        if self.numPrimaryCmds == 0:
            self.numPrimaryCmds += 1
            self.primaryAxisCmdList.insertItem(0, "PriTargetCmd_0")
            self.init_primary_axis_cmd_stack_ui(0)
        else:
            for i in range(self.numPrimaryCmds):
                self.primaryAxisCmdList.insertItem(i, "PriTargetCmd_%s"%i)
                self.init_primary_axis_cmd_stack_ui(i)
                self.init_primary_widget_field(i)

        addRemoveBtnLayout = functions.remove_add_btns(self,['primaryRemoveBtn','primaryAddBtn'])
        applyCancelBtnLayout = functions.cancel_save_btns(self,['primaryCancelBtn','primarySaveBtn'])

        tab1_layout.addWidget(self.primaryAxisCmdList,0,0)
        tab1_layout.addWidget(self.primaryStackedWidget,0,1)
        tab1_layout.addLayout(addRemoveBtnLayout,1,0)
        tab1_layout.addLayout(applyCancelBtnLayout,1,1)

        self.tab1.setLayout(tab1_layout)

        # Secondary Axis Cmd Tab -------------------------------------------------
        tab2_layout = QGridLayout()

        self.secondaryAxisCmdList = QListWidget()
        self.secondaryStackedWidget = QStackedWidget()

        if self.numSecondaryCmds == 0:
            self.numSecondaryCmds += 1
            self.secondaryAxisCmdList.insertItem(0, "SecTargetCmd_0")
            self.init_secondary_axis_cmd_stack_ui(0)
        else:
            for i in range(self.numSecondaryCmds):
                self.secondaryAxisCmdList.insertItem(i, "SecTargetCmd_%s"%i)
                self.init_secondary_axis_cmd_stack_ui(i)
                self.init_secondary_widget_field(i)

        addRemoveBtnLayout = functions.remove_add_btns(self,['secondaryRemoveBtn','secondaryAddBtn'])
        applyCancelBtnLayout = functions.cancel_save_btns(self,['secondaryCancelBtn','secondarySaveBtn'])

        tab2_layout.addWidget(self.secondaryAxisCmdList,0,0)
        tab2_layout.addWidget(self.secondaryStackedWidget,0,1)
        tab2_layout.addLayout(addRemoveBtnLayout,1,0)
        tab2_layout.addLayout(applyCancelBtnLayout,1,1)

        self.tab2.setLayout(tab2_layout)

        # Quaternion Attitutde Cmd Tab -----------------------------------------
        tab3_layout = QGridLayout()

        self.quaternionCmdList = QListWidget()
        self.quaternionStackedWidget = QStackedWidget()

        if self.numQuaternionCmds == 0:
            self.numQuaternionCmds += 1
            self.quaternionCmdList.insertItem(0, "QuaternionCmd_0")
            self.init_quaternion_atittude_cmd_stack_ui(0)
        else:
            for i in range(self.numQuaternionCmds):
                self.quaternionCmdList.insertItem(i, "QuaternionCmd_%s"%i)
                self.init_quaternion_atittude_cmd_stack_ui(i)
                self.init_quaternion_widget_field(i)

        addRemoveBtnLayout = functions.remove_add_btns(self,['quaternionRemoveBtn','quaternionAddBtn'])
        applyCancelBtnLayout = functions.cancel_save_btns(self,['quaternionCancelBtn','quaternionSaveBtn'])

        tab3_layout.addWidget(self.quaternionCmdList,0,0)
        tab3_layout.addWidget(self.quaternionStackedWidget,0,1)
        tab3_layout.addLayout(addRemoveBtnLayout,1,0)
        tab3_layout.addLayout(applyCancelBtnLayout,1,1)

        self.tab3.setLayout(tab3_layout)

        # Mirror Attitutde Cmd Tab -----------------------------------------
        tab4_layout = QGridLayout()

        self.mirrorAttitudeCmdList = QListWidget()
        self.mirrorStackedWidget = QStackedWidget()

        if self.numMirrorCmds == 0:
            self.numMirrorCmds += 1
            self.mirrorAttitudeCmdList.insertItem(0, "MirrorCmd_0")
            self.init_mirror_atittude_cmd_stack_ui(0)
        else:
            for i in range(self.numMirrorCmds):
                self.mirrorAttitudeCmdList.insertItem(i, "MirrorCmd_%s"%i)
                self.init_mirror_atittude_cmd_stack_ui(i)
                self.init_mirror_widget_field(i)

        addRemoveBtnLayout = functions.remove_add_btns(self,['mirrorRemoveBtn','mirrorAddBtn'])
        applyCancelBtnLayout = functions.cancel_save_btns(self,['mirrorCancelBtn','mirrorSaveBtn'])

        tab4_layout.addWidget(self.mirrorAttitudeCmdList,0,0)
        tab4_layout.addWidget(self.mirrorStackedWidget,0,1)
        tab4_layout.addLayout(addRemoveBtnLayout,1,0)
        tab4_layout.addLayout(applyCancelBtnLayout,1,1)

        self.tab4.setLayout(tab4_layout)

        # finalize attitude cmd subwindow --------------------------------------
        self.primary_save_data_slot()
        self.secondary_save_data_slot()
        self.quaternion_save_data_slot()
        self.mirror_save_data_slot()
        self.save_data_slot()

        self.signal_to_slot()

    # Build tab functions ------------------------------------------------------
    def init_primary_axis_cmd_stack_ui(self,stackIndex):
        self.primaryStack = QWidget()

        self.primaryStack.targetType = QComboBox()
        self.primaryStack.axisDirection1 = QLineEdit()
        self.primaryStack.axisDirection2 = QLineEdit()
        self.primaryStack.axisDirection3 = QLineEdit()
        self.primaryStack.targetSC = QComboBox()
        self.primaryStack.targetBody = QLineEdit()
        self.primaryStack.ctrlGainMode = QComboBox()
        self.primaryStack.ctrlLimitMode = QComboBox()
        self.primaryStack.actuatorMode = QComboBox()
        self.primaryStack.pointingVecDirection1 = QLineEdit()
        self.primaryStack.pointingVecDirection2 = QLineEdit()
        self.primaryStack.pointingVecDirection3 = QLineEdit()
        self.primaryStack.pointingVecFrame = QComboBox()

        completer = QCompleter(BODY_LIST)
        completer.setCaseSensitivity(False)

        self.primaryStack.targetType.addItems(["BODY", "SC", "VEC"])
        self.primaryStack.pointingVecFrame.addItems(["N", "F", "L", "B"])
        self.primaryStack.targetSC.addItems(self.dsmMenu.SC)
        self.primaryStack.targetBody.setCompleter(completer)
        self.primaryStack.ctrlGainMode.addItems(self.ctrlGainModes)
        self.primaryStack.ctrlLimitMode.addItems(self.ctrlLimitModes)
        self.primaryStack.actuatorMode.addItems(self.actuatorModes)

        primaryColumnLabels = [
        'Primary Target Type','Primary Axis Direction 1','Primary Axis Direction 2',
        'Primary Axis Direction 3','Primary Target',' Set of Control Gains',
        'Set of Control Limits','Actuator Mode','Pointing Vector Direction 1',
        'Pointing Vector Direction 2','Pointing Vector Direction 3','Pointing Vector Ref. Frame'
        ]
        self.primaryLabelList = functions.generate_label_widgets(primaryColumnLabels)

        self.primaryStack.targetLayout = QHBoxLayout()
        self.primaryStack.targetLayout.addWidget(self.primaryStack.targetSC)
        self.primaryStack.targetLayout.addWidget(self.primaryStack.targetBody)

        primaryStackLayout = QFormLayout()
        primaryStackLayout.setLabelAlignment(Qt.AlignLeft)
        primaryStackLayout.setFormAlignment(Qt.AlignLeft)
        primaryStackLayout.addRow(self.primaryLabelList[0],self.primaryStack.targetType)
        primaryStackLayout.addRow(self.primaryLabelList[1],self.primaryStack.axisDirection1)
        primaryStackLayout.addRow(self.primaryLabelList[2],self.primaryStack.axisDirection2)
        primaryStackLayout.addRow(self.primaryLabelList[3],self.primaryStack.axisDirection3)
        primaryStackLayout.addRow(self.primaryLabelList[4],self.primaryStack.targetLayout)
        primaryStackLayout.addRow(self.primaryLabelList[8],self.primaryStack.pointingVecDirection1)
        primaryStackLayout.addRow(self.primaryLabelList[9],self.primaryStack.pointingVecDirection2)
        primaryStackLayout.addRow(self.primaryLabelList[10],self.primaryStack.pointingVecDirection3)
        primaryStackLayout.addRow(self.primaryLabelList[11],self.primaryStack.pointingVecFrame)
        primaryStackLayout.addRow(self.primaryLabelList[5],self.primaryStack.ctrlGainMode)
        primaryStackLayout.addRow(self.primaryLabelList[6],self.primaryStack.ctrlLimitMode)
        primaryStackLayout.addRow(self.primaryLabelList[7],self.primaryStack.actuatorMode)

        self.primaryStack.setLayout(primaryStackLayout)
        self.primaryStackedWidget.addWidget(self.primaryStack)

        self.display_target_options(self.primaryStackedWidget.widget(stackIndex))

        self.primaryStackedWidget.widget(stackIndex).targetType.currentIndexChanged.connect(
        lambda:self.display_target_options(self.primaryStackedWidget.widget(stackIndex))
        )

    def init_primary_widget_field(self,stackIndex):
        stack = self.primaryStackedWidget.widget(stackIndex)

        itemIndex = stack.targetType.findText(self.primaryInputData[stackIndex][0])
        stack.targetType.setCurrentIndex(itemIndex)

        stack.axisDirection1.setText(self.primaryInputData[stackIndex][1])
        stack.axisDirection2.setText(self.primaryInputData[stackIndex][2])
        stack.axisDirection3.setText(self.primaryInputData[stackIndex][3])

        if (stack.targetType.currentText() == 'VEC'):
            stack.pointingVecDirection1.setText(self.primaryInputData[stackIndex][4])
            stack.pointingVecDirection2.setText(self.primaryInputData[stackIndex][5])
            stack.pointingVecDirection3.setText(self.primaryInputData[stackIndex][6])
            itemIndex = stack.pointingVecFrame.findText(self.primaryInputData[stackIndex][7])
            stack.pointingVecFrame.setCurrentIndex(itemIndex)
            itemIndex = stack.ctrlGainMode.findText(self.primaryInputData[stackIndex][8])
            stack.ctrlGainMode.setCurrentIndex(itemIndex)
            itemIndex = stack.ctrlLimitMode.findText(self.primaryInputData[stackIndex][9])
            stack.ctrlLimitMode.setCurrentIndex(itemIndex)
            itemIndex = stack.actuatorMode.findText(self.primaryInputData[stackIndex][10])
            stack.actuatorMode.setCurrentIndex(itemIndex)
        else:
            if (stack.targetType.currentText() == 'BODY'):
                stack.targetBody.setText(self.primaryInputData[stackIndex][4])
            else:
                itemIndex = stack.targetSC.findText(self.primaryInputData[stackIndex][4])
                stack.targetSC.setCurrentIndex(itemIndex)

            itemIndex = stack.ctrlGainMode.findText(self.primaryInputData[stackIndex][5])
            stack.ctrlGainMode.setCurrentIndex(itemIndex)
            itemIndex = stack.ctrlLimitMode.findText(self.primaryInputData[stackIndex][6])
            stack.ctrlLimitMode.setCurrentIndex(itemIndex)
            itemIndex = stack.actuatorMode.findText(self.primaryInputData[stackIndex][7])
            stack.actuatorMode.setCurrentIndex(itemIndex)

        self.display_target_options(stack)

    def init_secondary_axis_cmd_stack_ui(self,stackIndex):
        self.secondaryStack = QWidget()

        self.secondaryStack.targetType = QComboBox()
        self.secondaryStack.axisDirection1 = QLineEdit()
        self.secondaryStack.axisDirection2 = QLineEdit()
        self.secondaryStack.axisDirection3 = QLineEdit()
        self.secondaryStack.Target = QComboBox()
        self.secondaryStack.targetSC =  QComboBox()
        self.secondaryStack.targetBody =  QLineEdit()
        self.secondaryStack.pointingVecDirection1 = QLineEdit()
        self.secondaryStack.pointingVecDirection2 = QLineEdit()
        self.secondaryStack.pointingVecDirection3 = QLineEdit()
        self.secondaryStack.pointingVecFrame = QComboBox()

        completer = QCompleter(BODY_LIST)
        completer.setCaseSensitivity(False)

        self.secondaryStack.targetType.addItems(["BODY", "SC", "VEC"])
        self.secondaryStack.pointingVecFrame.addItems(["N", "F", "L"])
        self.secondaryStack.targetSC.addItems(self.dsmMenu.SC)
        self.secondaryStack.targetBody.setCompleter(completer)

        secondaryColumnLabels = [
        'Secondary Target Type','Secondary Axis Direction 1','Secondary Axis Direction 2',
        'Secondary Axis Direction 3','Secondary Target','Pointing Vector Direction 1',
        'Pointing Vector Direction 2','Pointing Vector Direction 3','Pointing Vector Ref. Frame'
        ]
        self.secondaryLabelList = functions.generate_label_widgets(secondaryColumnLabels)

        self.secondaryStack.targetLayout = QHBoxLayout()
        self.secondaryStack.targetLayout.addWidget(self.secondaryStack.targetSC)
        self.secondaryStack.targetLayout.addWidget(self.secondaryStack.targetBody)

        secondaryStackLayout = QFormLayout()
        secondaryStackLayout.setLabelAlignment(Qt.AlignLeft)
        secondaryStackLayout.setFormAlignment(Qt.AlignLeft)
        secondaryStackLayout.addRow(self.secondaryLabelList[0],self.secondaryStack.targetType)
        secondaryStackLayout.addRow(self.secondaryLabelList[1],self.secondaryStack.axisDirection1)
        secondaryStackLayout.addRow(self.secondaryLabelList[2],self.secondaryStack.axisDirection2)
        secondaryStackLayout.addRow(self.secondaryLabelList[3],self.secondaryStack.axisDirection3)
        secondaryStackLayout.addRow(self.secondaryLabelList[4],self.secondaryStack.targetLayout)
        secondaryStackLayout.addRow(self.secondaryLabelList[5],self.secondaryStack.pointingVecDirection1)
        secondaryStackLayout.addRow(self.secondaryLabelList[6],self.secondaryStack.pointingVecDirection2)
        secondaryStackLayout.addRow(self.secondaryLabelList[7],self.secondaryStack.pointingVecDirection3)
        secondaryStackLayout.addRow(self.secondaryLabelList[8],self.secondaryStack.pointingVecFrame)
        secondaryStackLayout.setAlignment(Qt.AlignLeft)

        self.secondaryStack.setLayout(secondaryStackLayout)
        self.secondaryStackedWidget.addWidget(self.secondaryStack)

        self.display_target_options(self.secondaryStackedWidget.widget(stackIndex))

        self.secondaryStackedWidget.widget(stackIndex).targetType.currentIndexChanged.connect(
        lambda:self.display_target_options(self.secondaryStackedWidget.widget(stackIndex))
        )

    def init_secondary_widget_field(self,stackIndex):
        stack = self.secondaryStackedWidget.widget(stackIndex)

        itemIndex = stack.targetType.findText(self.secondaryInputData[stackIndex][0])
        stack.targetType.setCurrentIndex(itemIndex)

        stack.axisDirection1.setText(self.secondaryInputData[stackIndex][1])
        stack.axisDirection2.setText(self.secondaryInputData[stackIndex][2])
        stack.axisDirection3.setText(self.secondaryInputData[stackIndex][3])

        if (stack.targetType.currentText() == 'VEC'):
            stack.pointingVecDirection1.setText(self.secondaryInputData[stackIndex][4])
            stack.pointingVecDirection2.setText(self.secondaryInputData[stackIndex][5])
            stack.pointingVecDirection3.setText(self.secondaryInputData[stackIndex][6])
            itemIndex = stack.pointingVecFrame.findText(self.secondaryInputData[stackIndex][7])
            stack.pointingVecFrame.setCurrentIndex(itemIndex)

        else:
            if (stack.targetType.currentText() == 'BODY'):
                stack.targetBody.setText(self.secondaryInputData[stackIndex][4])
            else:
                itemIndex = stack.targetSC.findText(self.secondaryInputData[stackIndex][4])
                stack.targetSC.setCurrentIndex(itemIndex)

        self.display_target_options(stack)

    def init_quaternion_atittude_cmd_stack_ui(self,stackIndex):
        self.quaternionStack = QWidget()

        self.quaternionStack.quaternionComponent0 = QLineEdit()
        self.quaternionStack.quaternionComponent1 = QLineEdit()
        self.quaternionStack.quaternionComponent2 = QLineEdit()
        self.quaternionStack.quaternionComponent3 = QLineEdit()
        self.quaternionStack.quaternionRefFrame = QComboBox()
        self.quaternionStack.ctrlGainMode = QComboBox()
        self.quaternionStack.ctrlLimitMode = QComboBox()
        self.quaternionStack.actuatorMode = QComboBox()

        self.quaternionStack.quaternionRefFrame.addItems(["N", "F", "L", "B"])
        self.quaternionStack.ctrlGainMode.addItems(self.ctrlGainModes)
        self.quaternionStack.ctrlLimitMode.addItems(self.ctrlLimitModes)
        self.quaternionStack.actuatorMode.addItems(self.actuatorModes)

        quaternionColumnLabels = [
        'Quaternion Component, q0','Quaternion Component, q1','SQuaternion Component, q2',
        'Quaternion Component, q3','Reference Frame of Quaternion','Set of Control Gains',
        'Set of Control Limits','Actuator Mode'
        ]
        self.quaternionLabelList = functions.generate_label_widgets(quaternionColumnLabels)

        quaternionStackLayout = QFormLayout()
        quaternionStackLayout.setLabelAlignment(Qt.AlignLeft)
        quaternionStackLayout.setFormAlignment(Qt.AlignLeft)
        quaternionStackLayout.addRow(self.quaternionLabelList[0],self.quaternionStack.quaternionComponent0)
        quaternionStackLayout.addRow(self.quaternionLabelList[1],self.quaternionStack.quaternionComponent1)
        quaternionStackLayout.addRow(self.quaternionLabelList[2],self.quaternionStack.quaternionComponent2)
        quaternionStackLayout.addRow(self.quaternionLabelList[3],self.quaternionStack.quaternionComponent3)
        quaternionStackLayout.addRow(self.quaternionLabelList[4],self.quaternionStack.quaternionRefFrame)
        quaternionStackLayout.addRow(self.quaternionLabelList[5],self.quaternionStack.ctrlGainMode)
        quaternionStackLayout.addRow(self.quaternionLabelList[6],self.quaternionStack.ctrlLimitMode)
        quaternionStackLayout.addRow(self.quaternionLabelList[7],self.quaternionStack.actuatorMode)

        self.quaternionStack.setLayout(quaternionStackLayout)
        self.quaternionStackedWidget.addWidget(self.quaternionStack)

    def init_quaternion_widget_field(self,stackIndex):
        stack = self.quaternionStackedWidget.widget(stackIndex)

        stack.quaternionComponent0.setText(self.quaternionInputData[stackIndex][0])
        stack.quaternionComponent1.setText(self.quaternionInputData[stackIndex][1])
        stack.quaternionComponent2.setText(self.quaternionInputData[stackIndex][2])
        stack.quaternionComponent3.setText(self.quaternionInputData[stackIndex][3])

        itemIndex = stack.quaternionRefFrame.findText(self.quaternionInputData[stackIndex][4])
        stack.quaternionRefFrame.setCurrentIndex(itemIndex)
        itemIndex = stack.ctrlGainMode.findText(self.quaternionInputData[stackIndex][5])
        stack.ctrlGainMode.setCurrentIndex(itemIndex)
        itemIndex = stack.ctrlLimitMode.findText(self.quaternionInputData[stackIndex][6])
        stack.ctrlLimitMode.setCurrentIndex(itemIndex)
        itemIndex = stack.actuatorMode.findText(self.quaternionInputData[stackIndex][7])
        stack.actuatorMode.setCurrentIndex(itemIndex)

    def init_mirror_atittude_cmd_stack_ui(self,stackIndex):
        self.mirrorStack = QWidget()
        self.mirrorStack.targetSC =  QComboBox()
        self.mirrorStack.ctrlGainMode = QComboBox()
        self.mirrorStack.ctrlLimitMode = QComboBox()
        self.mirrorStack.actuatorMode = QComboBox()

        self.mirrorStack.targetSC.addItems(self.dsmMenu.SC)
        self.mirrorStack.ctrlGainMode.addItems(self.ctrlGainModes)
        self.mirrorStack.ctrlLimitMode.addItems(self.ctrlLimitModes)
        self.mirrorStack.actuatorMode.addItems(self.actuatorModes)

        mirrorColumnLabels = [
        'Spacecraft to mirror the altitude of','Set of Control Gains',' Set of Control Limits','Actuator Mode'
        ]
        self.mirrorLabelList = functions.generate_label_widgets(mirrorColumnLabels)

        mirrorStackLayout = QFormLayout()
        mirrorStackLayout.setLabelAlignment(Qt.AlignLeft)
        mirrorStackLayout.setFormAlignment(Qt.AlignLeft)
        mirrorStackLayout.addRow(self.mirrorLabelList[0],self.mirrorStack.targetSC)
        mirrorStackLayout.addRow(self.mirrorLabelList[1],self.mirrorStack.ctrlGainMode)
        mirrorStackLayout.addRow(self.mirrorLabelList[2],self.mirrorStack.ctrlLimitMode)
        mirrorStackLayout.addRow(self.mirrorLabelList[3],self.mirrorStack.actuatorMode)

        self.mirrorStack.setLayout(mirrorStackLayout)
        self.mirrorStackedWidget.addWidget(self.mirrorStack)

    def init_mirror_widget_field(self,stackIndex):
        stack = self.mirrorStackedWidget.widget(stackIndex)
        itemIndex = stack.ctrlGainMode.findText(self.mirrorInputData[stackIndex][1])
        stack.ctrlGainMode.setCurrentIndex(itemIndex)
        itemIndex = stack.ctrlLimitMode.findText(self.mirrorInputData[stackIndex][2])
        stack.ctrlLimitMode.setCurrentIndex(itemIndex)
        itemIndex = stack.actuatorMode.findText(self.mirrorInputData[stackIndex][3])
        stack.actuatorMode.setCurrentIndex(itemIndex)

    # general functions --------------------------------------------------------
    # gets initial input data
    def get_input_data(self, fileName):
        self.primaryInputData = []
        self.secondaryInputData = []
        self.quaternionInputData = []
        self.mirrorInputData = []

        # number of inputs per cmd type
        NUM_PRIMARY_INPUTS = 11
        NUM_SECONDARY_INPUTS = 8
        NUM_QUATERNION_INPUTS = 8
        NUM_MIRROR_INPUTS = 4

        # regex expression per cmd type
        val = "\s*([+-]?[a-zA-Z\_\.0-9]*)"
        primaryCmdPattern = "PriTargetCmd_[0-9]"
        secondaryCmdPattern = "SecTargetCmd_[0-9]+"
        quaternionCmdPattern = "QuaternionCmd_[0-9]+"
        mirrorCmdPattern = "MirrorCmd_[0-9]+"

        # finalize regex expressions depending on num of inputs
        for col in range(NUM_PRIMARY_INPUTS): primaryCmdPattern += val
        for col in range(NUM_SECONDARY_INPUTS): secondaryCmdPattern += val
        for col in range(NUM_QUATERNION_INPUTS):quaternionCmdPattern += val
        for col in range(NUM_MIRROR_INPUTS): mirrorCmdPattern += val

        # compile regex expressions into pattern objects
        primaryCmdPattern = re.compile(primaryCmdPattern,re.IGNORECASE)
        secondaryCmdPattern = re.compile(secondaryCmdPattern,re.IGNORECASE)
        quaternionCmdPattern = re.compile(quaternionCmdPattern,re.IGNORECASE)
        mirrorCmdPattern = re.compile(mirrorCmdPattern,re.IGNORECASE)

        # scan input file for cmds using regex expressions and append data
        for i, line in enumerate(open(fileName)):
            for match in re.finditer(primaryCmdPattern, line):
                if match.group(2) != "": # flag to check if actual cmd
                    matchData = []
                    for group in match.groups():
                        if group != '': matchData.append(group) # check if data is empty
                        else: continue
                    self.primaryInputData.append(matchData)

            for match in re.finditer(secondaryCmdPattern, line):
                if match.group(2) != "": # flag to check if actual cmd
                    matchData = []
                    for group in match.groups():
                        if group != '': matchData.append(group) # check if data is empty
                        else: continue
                    self.secondaryInputData.append(matchData)

            for match in re.finditer(quaternionCmdPattern, line):
                if match.group(2) != "": # flag to check if actual cmd
                    matchData = []
                    for group in match.groups():
                        if group != '': matchData.append(group) # check if data is empty
                        else: matchData.append(" ")
                    self.quaternionInputData.append(matchData)

            for match in re.finditer(mirrorCmdPattern, line):
                if match.group(2) != "": # flag to check if actual cmd
                    matchData = []
                    for group in match.groups():
                        if group != '': matchData.append(group) # check if data is empty
                        else: matchData.append(" ")
                    self.mirrorInputData.append(matchData)

        # obtain number of cmds per cmd type
        self.numPrimaryCmds = len(self.primaryInputData)
        self.numSecondaryCmds = len(self.secondaryInputData)
        self.numQuaternionCmds = len(self.quaternionInputData)
        self.numMirrorCmds = len(self.mirrorInputData)

    # updates QComboBox options for SC type selections
    def update_sc_options(self):
        for i in range(self.primaryAxisCmdList.count()):
            stack = self.primaryStackedWidget.widget(i)
            stack.targetSC.clear()
            stack.targetSC.addItems(self.dsmMenu.SC)

        for i in range(self.secondaryAxisCmdList.count()):
            stack = self.secondaryStackedWidget.widget(i)
            stack.targetSC.clear()
            stack.targetSC.addItems(self.dsmMenu.SC)

        for i in range(self.mirrorAttitudeCmdList.count()):
            stack = self.mirrorStackedWidget.widget(i)
            stack.targetSC.clear()
            stack.targetSC.addItems(self.dsmMenu.SC)

    # updates QComboBox options for actuator type selections
    def update_actuator_options(self):
        self.actuatorModes = functions.get_list_items(self.actuatorMenu.cmdList)
        for i in range(self.primaryAxisCmdList.count()):
            stack = self.primaryStackedWidget.widget(i)
            stack.actuatorMode.clear()
            stack.actuatorMode.addItems(self.actuatorModes)

        for i in range(self.quaternionCmdList.count()):
            stack = self.quaternionStackedWidget.widget(i)
            stack.actuatorMode.clear()
            stack.actuatorMode.addItems(self.actuatorModes)

        for i in range(self.mirrorAttitudeCmdList.count()):
            stack = self.mirrorStackedWidget.widget(i)
            stack.actuatorMode.clear()
            stack.actuatorMode.addItems(self.actuatorModes)

    # updates QComboBox options for control limit type selections
    def update_ctrl_limit_options(self):
        self.ctrlLimitModes = functions.get_list_items(self.ctrlLimitMenu.ctrlLimitList)
        for i in range(self.primaryAxisCmdList.count()):
            stack = self.primaryStackedWidget.widget(i)
            stack.ctrlLimitMode.clear()
            stack.ctrlLimitMode.addItems(self.ctrlLimitModes)

        for i in range(self.quaternionCmdList.count()):
            stack = self.quaternionStackedWidget.widget(i)
            stack.ctrlLimitMode.clear()
            stack.ctrlLimitMode.addItems(self.ctrlLimitModes)

        for i in range(self.mirrorAttitudeCmdList.count()):
            stack = self.mirrorStackedWidget.widget(i)
            stack.ctrlLimitMode.clear()
            stack.ctrlLimitMode.addItems(self.ctrlLimitModes)

    # updates QComboBox options for control parameter type selections
    def update_ctrl_parameter_options(self):
        self.ctrlGainModes = functions.get_list_items(self.ctrlParameterMenu.cmdList)
        for i in range(self.primaryAxisCmdList.count()):
            stack = self.primaryStackedWidget.widget(i)
            stack.ctrlGainMode.clear()
            stack.ctrlGainMode.addItems(self.ctrlGainModes)

        for i in range(self.quaternionCmdList.count()):
            stack = self.quaternionStackedWidget.widget(i)
            stack.ctrlGainMode.clear()
            stack.ctrlGainMode.addItems(self.ctrlGainModes)

        for i in range(self.mirrorAttitudeCmdList.count()):
            stack = self.mirrorStackedWidget.widget(i)
            stack.ctrlGainMode.clear()
            stack.ctrlGainMode.addItems(self.ctrlGainModes)

    # gives primary/secondary axis cmds ability to display UI options under target type
    def display_target_options(self,stack):
        layout = stack.layout()
        targetOptionIdx = layout.getLayoutPosition(stack.targetLayout)
        vecOptionIdxStart = layout.getWidgetPosition(stack.pointingVecDirection1)
        vecOptionIdxEnd = layout.getWidgetPosition(stack.pointingVecFrame)

        if (stack.targetType.currentText() == 'VEC'):
            layout.itemAt(targetOptionIdx[0],0).widget().hide()
            stack.targetSC.hide()
            stack.targetBody.hide()
            for idx in range(vecOptionIdxStart[0],(vecOptionIdxEnd[0]+1)):
                layout.itemAt(idx,1).widget().show()
                layout.itemAt(idx,0).widget().show()
        else:
            layout.itemAt(targetOptionIdx[0],0).widget().show()
            if (stack.targetType.currentText() == 'BODY'):
                stack.targetBody.show()
                stack.targetSC.hide()
            else:
                stack.targetBody.hide()
                stack.targetSC.show()
            for idx in range(vecOptionIdxStart[0],(vecOptionIdxEnd[0]+1)):
                layout.itemAt(idx,1).widget().hide()
                layout.itemAt(idx,0).widget().hide()

    # assigns per subwindow
    def signal_to_slot(self):
        # primary axis cmd
        self.primaryAxisCmdList.currentRowChanged.connect(lambda:functions.display_stack_slot(self.primaryStackedWidget,self.primaryAxisCmdList.currentRow()))
        self.primaryRemoveBtn.clicked.connect(lambda:functions.remove_stack_slot(self.primaryStackedWidget,self.primaryAxisCmdList,self.updatePrimaryAxisCmds))
        self.primaryAddBtn.clicked.connect(lambda:functions.add_stack_slot(self.primaryStackedWidget,self.primaryAxisCmdList,self.init_primary_axis_cmd_stack_ui,'PriTargetCmd',self.updatePrimaryAxisCmds))
        self.primarySaveBtn.clicked.connect(self.primary_save_data_slot)
        self.primarySaveBtn.clicked.connect(self.save_data_slot)

        # secondary axis cmd
        self.secondaryAxisCmdList.currentRowChanged.connect(lambda:functions.display_stack_slot(self.secondaryStackedWidget,self.secondaryAxisCmdList.currentRow()))
        self.secondaryRemoveBtn.clicked.connect(lambda:functions.remove_stack_slot(self.secondaryStackedWidget,self.secondaryAxisCmdList,self.updateSecondaryAxisCmds))
        self.secondaryAddBtn.clicked.connect(lambda:functions.add_stack_slot(self.secondaryStackedWidget,self.secondaryAxisCmdList,self.init_secondary_axis_cmd_stack_ui,'SecTargetCmd',self.updateSecondaryAxisCmds))
        self.secondarySaveBtn.clicked.connect(self.secondary_save_data_slot)
        self.secondarySaveBtn.clicked.connect(self.save_data_slot)

        # quaternion attitude cmd
        self.quaternionCmdList.currentRowChanged.connect(lambda:functions.display_stack_slot(self.quaternionStackedWidget,self.quaternionCmdList.currentRow()))
        self.quaternionRemoveBtn.clicked.connect(lambda:functions.remove_stack_slot(self.quaternionStackedWidget,self.quaternionCmdList,self.updateQuaternionCmds))
        self.quaternionAddBtn.clicked.connect(lambda:functions.add_stack_slot(self.quaternionStackedWidget,self.quaternionCmdList,self.init_quaternion_atittude_cmd_stack_ui,'QuaternionCmd',self.updateQuaternionCmds))
        self.quaternionSaveBtn.clicked.connect(self.quaternion_save_data_slot)
        self.quaternionSaveBtn.clicked.connect(self.save_data_slot)

        # mirror atttitude cmd
        self.mirrorAttitudeCmdList.currentRowChanged.connect(lambda:functions.display_stack_slot(self.mirrorStackedWidget,self.mirrorAttitudeCmdList.currentRow()))
        self.mirrorRemoveBtn.clicked.connect(lambda:functions.remove_stack_slot(self.mirrorStackedWidget,self.mirrorAttitudeCmdList,self.updateMirrorCmds))
        self.mirrorAddBtn.clicked.connect(lambda:functions.add_stack_slot(self.mirrorStackedWidget,self.mirrorAttitudeCmdList,self.init_mirror_atittude_cmd_stack_ui,'MirrorCmd',self.updateMirrorCmds))
        self.mirrorSaveBtn.clicked.connect(self.mirror_save_data_slot)
        self.mirrorSaveBtn.clicked.connect(self.save_data_slot)

        # general
        self.dsmMenu.updateScList.connect(self.update_sc_options)
        self.actuatorMenu.updateActuatorModes.connect(self.update_actuator_options)
        self.ctrlLimitMenu.updateLimitModes.connect(self.update_ctrl_limit_options)
        self.ctrlParameterMenu.updateGainModes.connect(self.update_ctrl_parameter_options)

    # writing slots ------------------------------------------------------------
    def primary_save_data_slot(self):
        self.primarySaveData = [""]*300
        self.primarySaveData[0] = "#-------------------------------------------------------------------------------\n"
        self.primarySaveData[1] = "#                              Attitude Command \n"
        self.primarySaveData[2] = "#-------------------------------------------------------------------------------\n"
        self.primarySaveData[3] = "#------------------------------Primary Axis Cmd---------------------------------\n"
        self.primarySaveData[4] = "# Col: 1   ->  Command Interp ID Flag (hard coded in interpreter)\n"
        self.primarySaveData[5] = "# Col: 2   ->  Primary Target Type: BODY, SC, VEC\n"
        self.primarySaveData[6] = "# Col: 3   ->  Primary Axis Direction 1\n"
        self.primarySaveData[7] = "# Col: 4   ->  Primary Axis Direction 2\n"
        self.primarySaveData[8] = "# Col: 5   ->  Primary Axis Direction 3\n"
        self.primarySaveData[9] = "# Col: 6   ->  Primary Target: Specify Body, SC or Vec\n"
        self.primarySaveData[10] = "# Col: 7   ->  Set of Control Gains\n"
        self.primarySaveData[11] = "# Col: 8   ->  Set of Control Limits\n"
        self.primarySaveData[12] = "# Col: 9   ->  Actuator Mode\n"
        self.primarySaveData[13] = "# If Col: 2 -> VEC, then Col: 6-8 are the pointing vec for Secondary axis\n"
        self.primarySaveData[14] = '#                        Col: 9 is the Ref. Frame of the pointing vector: "N", "F", "L", "B" \n'
        self.primarySaveData[15] = "#                        Col: 10-12 are Control Gain, Limit and Actuator Sets\n"
        self.primarySaveData[16] = "\n"

        txt_idx = 17
        for i in range(self.primaryAxisCmdList.count()):
            stack = self.primaryStackedWidget.widget(i)
            if stack.targetType.currentText().lower() == 'vec':
                col_1 = stack.targetType.currentText()
                col_2 = stack.axisDirection1.text()
                col_3 = stack.axisDirection2.text()
                col_4 = stack.axisDirection3.text()
                col_5 = stack.pointingVecDirection1.text()
                col_6 = stack.pointingVecDirection2.text()
                col_7 = stack.pointingVecDirection3.text()
                col_8 = stack.pointingVecFrame.currentText()
                col_9 = stack.ctrlGainMode.currentText()
                col_10 = stack.ctrlLimitMode.currentText()
                col_11 = stack.actuatorMode.currentText()

                self.primarySaveData[txt_idx] = "PriTargetCmd_%s"%i + "   " + col_1 + "   "\
                 + col_2 + "   " + col_3 + "   " + col_4 + "   " + col_5 \
                 + "   " + col_6  + "   " + col_7  + "   " + col_8 \
                 + "   " + col_9  + "   " + col_10  + "   " + col_11 + "\n"
            else:
                col_1 = stack.targetType.currentText()
                col_2 = stack.axisDirection1.text()
                col_3 = stack.axisDirection2.text()
                col_4 = stack.axisDirection3.text()
                if (stack.targetType.currentText() == 'BODY'):
                    col_5 = stack.targetBody.text()
                else:
                    col_5 = stack.targetSC.currentText()
                col_6 = stack.ctrlGainMode.currentText()
                col_7 = stack.ctrlLimitMode.currentText()
                col_8 = stack.actuatorMode.currentText()

                self.primarySaveData[txt_idx] = "PriTargetCmd_%s"%i + "   " + col_1 + "   "\
                 + col_2 + "   " + col_3 + "   " + col_4 + "   " + col_5 \
                 + "   " + col_6  + "   " + col_7  + "   " + col_8 + "\n"

            txt_idx += 1

    def secondary_save_data_slot(self):
        self.secondarySaveData = [""]*300
        self.secondarySaveData[0] = "\n"
        self.secondarySaveData[1] = "#------------------------------Secondary Axis Cmd---------------------------------\n"
        self.secondarySaveData[2] = "# Col: 1   ->  Command Interp ID Flag (hard coded in interpreter)\n"
        self.secondarySaveData[3] = "# Col: 2   ->  Secondary Target Type: BODY, SC, VEC\n"
        self.secondarySaveData[4] = "# Col: 3   ->  Secondary Axis Direction 1\n"
        self.secondarySaveData[5] = "# Col: 4   ->  Secondary Axis Direction 2\n"
        self.secondarySaveData[6] = "# Col: 5   ->  Secondary Axis Direction 3\n"
        self.secondarySaveData[7] = "# Col: 6   ->  Secondary Target: Specify Body, SC or Vec\n"
        self.secondarySaveData[8] = "# If Col: 2 -> VEC, then Col: 6-8 are the pointing vec for Secondary axis\n"
        self.secondarySaveData[9] = '#                        Col: 9 is the Ref. Frame of the pointing vector: "N", "F", "L", "B" \n'
        self.secondarySaveData[10] = "\n"

        txt_idx = 11
        for i in range(self.secondaryAxisCmdList.count()):
            stack = self.secondaryStackedWidget.widget(i)
            if stack.targetType.currentText() == 'VEC':
                col_1 = stack.targetType.currentText()
                col_2 = stack.axisDirection1.text()
                col_3 = stack.axisDirection2.text()
                col_4 = stack.axisDirection3.text()
                col_5 = stack.pointingVecDirection1.text()
                col_6 = stack.pointingVecDirection2.text()
                col_7 = stack.pointingVecDirection3.text()
                col_8 = stack.pointingVecFrame.currentText()

                self.secondarySaveData[txt_idx] = "SecTargetCmd_%s"%i + "   " + col_1 + "   "\
                 + col_2 + "   " + col_3 + "   " + col_4 + "   " + col_5 \
                 + "   " + col_6  + "   " + col_7  + "   " + col_8 + "\n"
            else:
                col_1 = stack.targetType.currentText()
                col_2 = stack.axisDirection1.text()
                col_3 = stack.axisDirection2.text()
                col_4 = stack.axisDirection3.text()
                if (stack.targetType.currentText() == 'BODY'):
                    col_5 = stack.targetBody.text()
                else:
                    col_5 = stack.targetSC.currentText()

                self.secondarySaveData[txt_idx] = "SecTargetCmd_%s"%i + "   " + col_1 + "   "\
                 + col_2 + "   " + col_3 + "   " + col_4 + "   " + col_5 + "\n"

            txt_idx += 1

    def quaternion_save_data_slot(self):
        self.quaternionSaveData = [""]*300
        self.quaternionSaveData[0] = "\n"
        self.quaternionSaveData[1] = "#------------------------------Quaternion Attitude Cmd---------------------------------\n"
        self.quaternionSaveData[4] = "# Col: 1   ->  Command Interp ID Flag (hard coded in interpreter)\n"
        self.quaternionSaveData[5] = "# Col: 2   ->  Quaternion Component, q0\n"
        self.quaternionSaveData[6] = "# Col: 3   ->  Quaternion Component, q1\n"
        self.quaternionSaveData[7] = "# Col: 4   ->  Quaternion Component, q2\n"
        self.quaternionSaveData[8] = "# Col: 5   ->  Quaternion Component, q3\n"
        self.quaternionSaveData[9] = "# Col: 6   ->  Reference Frame of Quaternion\n"
        self.quaternionSaveData[10] = "# Col: 7   ->  Set of Control Gains\n"
        self.quaternionSaveData[11] = "# Col: 8   ->  Set of Control Limits\n"
        self.quaternionSaveData[12] = "# Col: 9   ->  Actuator Mode\n"
        self.quaternionSaveData[13] = "\n"

        txt_idx = 14
        for i in range(self.quaternionCmdList.count()):
            stack = self.quaternionStackedWidget.widget(i)

            col_1 = stack.quaternionComponent0.text()
            col_2 = stack.quaternionComponent1.text()
            col_3 = stack.quaternionComponent2.text()
            col_4 = stack.quaternionComponent3.text()
            col_5 = stack.quaternionRefFrame.currentText()
            col_6 = stack.ctrlGainMode.currentText()
            col_7 = stack.ctrlLimitMode.currentText()
            col_8 = stack.actuatorMode.currentText()

            self.quaternionSaveData[txt_idx] = "QuaternionCmd_%s"%i + "   " + col_1 + "   "\
             + col_2 + "   " + col_3 + "   " + col_4 + "   " + col_5 \
             + "   " + col_6  + "   " + col_7  + "   " + col_8 + "\n"

            txt_idx += 1

    def mirror_save_data_slot(self):
        self.mirrorSaveData = [""]*300
        self.mirrorSaveData[0] = "\n"
        self.mirrorSaveData[1] = "#------------------------------Mirror Attitude Cmd---------------------------------\n"
        self.mirrorSaveData[2] = '# Col: 1   ->  Command Interp ID Flag (hard coded in interpreter)\n'
        self.mirrorSaveData[3] = '# Col: 2   ->  Spacecraft to mirror the attitude of: "SC#"\n'
        self.mirrorSaveData[4] = "# Col: 3   ->  Set of Control Gains\n"
        self.mirrorSaveData[5] = "# Col: 4   ->  Set of Control Limits\n"
        self.mirrorSaveData[6] = "# Col: 5   ->  Actuator Mode\n"
        self.mirrorSaveData[7] = "\n"

        txt_idx = 8
        for i in range(self.mirrorAttitudeCmdList.count()):
            stack = self.mirrorStackedWidget.widget(i)

            col_1 = stack.targetSC.currentText()
            col_2 = stack.ctrlGainMode.currentText()
            col_3 = stack.ctrlLimitMode.currentText()
            col_4 = stack.actuatorMode.currentText()

            self.mirrorSaveData[txt_idx] = "MirrorCmd_%s"%i + "   " + col_1 + "   "\
             + col_2 + "   " + col_3 + "   " + col_4 + "\n"

            txt_idx += 1

    def save_data_slot(self):
        self.saveData = self.primarySaveData + self.secondarySaveData + \
        self.quaternionSaveData + self.mirrorSaveData

#-------------------------------------------------------------------------------
# Controller Paramters Menu ----------------------------------------------------
class CtrlParameterMenu(QWidget):

    updateGainModes = QtCore.pyqtSignal()

    def __init__(self):
        super().__init__()
        self.setWindowTitle('Control Parameters Menu')

        self.sectionFlag = 'Gains_'
        self.sectionHeader = 'Translational / Attitude Control Parameters'
        self.NUM_INPUTS = 10
        self.widgetTypeList = [2,1,1,1,1,1,1,1,1,1]
        functions.check_input_file_exists(self,'InOut/Inp_DSM.txt')

        self.missionDir = functions.get_mission_directory(self)
        inputFile = self.missionDir + '/InOut/Inp_DSM.txt'
        DSM_Functions.get_input_data(self, inputFile)
        DSM_Functions.get_labels(self, inputFile)
        DSM_Functions.get_cmd_list(self,'InOut/Inp_DSM.txt')

        self.stackedWidget = QStackedWidget()
        if self.numCmds == 0:
            DSM_Functions.init_stack_ui(self,0)
        else:
            for i in range(self.numCmds):
                DSM_Functions.init_stack_ui(self,i)
                DSM_Functions.init_widget_field(self,i)

        self.addRemoveBtnLayout = functions.remove_add_btns(self,['removeBtn','addBtn'])
        self.defaultSaveBtnLayout = functions.default_save_btns(self,['defaultBtn','saveBtn','saveDefaultBtn'])

        cmdMenuLayout = QGridLayout()
        cmdMenuLayout.addWidget(self.cmdList,0,0)
        cmdMenuLayout.addLayout(self.addRemoveBtnLayout,1,0)
        cmdMenuLayout.addLayout(self.defaultSaveBtnLayout,1,1)
        cmdMenuLayout.addWidget(self.stackedWidget,0,1)

        self.setLayout(cmdMenuLayout)

        self.signal_to_slot()

        DSM_Functions.save_data_slot(self)

    def signal_to_slot(self):
        self.cmdList.currentRowChanged.connect(lambda:DSM_Functions.display_stack_slot(self,self.cmdList.currentRow()))
        self.removeBtn.clicked.connect(lambda:DSM_Functions.remove_stack_slot(self,self.updateGainModes))
        self.addBtn.clicked.connect(lambda:DSM_Functions.add_stack_slot(self,self.updateGainModes,0))
        self.saveBtn.clicked.connect(lambda:DSM_Functions.save_data_slot(self))
        # self.saveDefaultBtn.clicked.connect(lambda:DSM_Functions.save_default_slot())
        self.defaultBtn.clicked.connect(lambda:DSM_Functions.default_data_slot(self))

class CtrlLimitMenu(QWidget):

    updateLimitModes = QtCore.pyqtSignal()

    def __init__(self):
        super().__init__()
        self.setWindowTitle('Control Limits Menu')

        self.missionDir = functions.get_mission_directory(self)
        inputFile = self.missionDir + '/InOut/Inp_DSM.txt'
        self.get_input_data(inputFile)

        self.ctrlLimitList = QListWidget()
        self.stackedWidget = QStackedWidget()
        if self.numCmds == 0:
            self.ctrlLimitList.insertItem(0, "Limits_0")
            self.init_stack_ui(0)
        else:
            for i in range(self.numCmds):
                self.ctrlLimitList.insertItem(i, "Limits_%s"%i)
                self.init_stack_ui(i)
                self.init_widget_field(i)

        addRemoveBtnLayout = functions.remove_add_btns(self,['removeBtn','addBtn'])
        applyCancelBtnLayout = functions.cancel_apply_btns(self,['cancelBtn','applyBtn'])

        ctrlLimitLayout = QGridLayout()
        ctrlLimitLayout.addWidget(self.ctrlLimitList,0,0)
        ctrlLimitLayout.addLayout(addRemoveBtnLayout,1,0)
        ctrlLimitLayout.addLayout(applyCancelBtnLayout,1,1)
        ctrlLimitLayout.addWidget(self.stackedWidget,0,1)

        self.setLayout(ctrlLimitLayout)

        self.signal_to_slot()

        self.save_data_slot()

    def init_stack_ui(self,stackIndex):
        self.stack = QWidget()

        self.stack.maxAccelerationDirection1 = QLineEdit()
        self.stack.maxAccelerationDirection2 = QLineEdit()
        self.stack.maxAccelerationDirection3 = QLineEdit()
        self.stack.maxVelocityDirection1 = QLineEdit()
        self.stack.maxVelocityDirection2 = QLineEdit()
        self.stack.maxVelocityDirection3 = QLineEdit()

        columnLabels = [
        'Max. Acceleration Direction 1','Max. Acceleration Direction 2',
        'Max. Acceleration Direction 3', 'Max. Velocity Direction 1',
        'Max. Velocity Direction 2','Max. Velocity Direction 3'
        ]
        self.labelList = functions.generate_label_widgets(columnLabels)

        stackLayout = QFormLayout()
        stackLayout.setLabelAlignment(Qt.AlignLeft)
        stackLayout.setFormAlignment(Qt.AlignLeft)
        stackLayout.addRow(self.labelList[0],self.stack.maxAccelerationDirection1)
        stackLayout.addRow(self.labelList[1],self.stack.maxAccelerationDirection2)
        stackLayout.addRow(self.labelList[2],self.stack.maxAccelerationDirection3)
        stackLayout.addRow(self.labelList[3],self.stack.maxVelocityDirection1)
        stackLayout.addRow(self.labelList[4],self.stack.maxVelocityDirection2)
        stackLayout.addRow(self.labelList[5],self.stack.maxVelocityDirection3)

        self.stack.setLayout(stackLayout)
        self.stackedWidget.addWidget(self.stack)

    def init_widget_field(self,stackIndex):
        stack = self.stackedWidget.widget(stackIndex)
        stack.maxAccelerationDirection1.setText(self.inputData[stackIndex][0])
        stack.maxAccelerationDirection2.setText(self.inputData[stackIndex][1])
        stack.maxAccelerationDirection3.setText(self.inputData[stackIndex][2])
        stack.maxVelocityDirection1.setText(self.inputData[stackIndex][3])
        stack.maxVelocityDirection2.setText(self.inputData[stackIndex][4])
        stack.maxVelocityDirection3.setText(self.inputData[stackIndex][5])

    def get_input_data(self, fileName):
        self.inputData = []

        NUM_CTRL_LIMIT_INPUTS = 6

        val = "\s*([+-]?[a-zA-Z\_\.0-9]*)"
        pattern = "Limits_[0-9]+"
        for col in range(NUM_CTRL_LIMIT_INPUTS): pattern += val
        pattern = re.compile(pattern,re.IGNORECASE)

        for i, line in enumerate(open(fileName)):
            for match in re.finditer(pattern, line):
                if match.group(2) != "": # flag to check if actual cmd
                    matchData = []
                    for group in match.groups():
                        if group != "": matchData.append(group)
                        else: matchData.append(" ")
                    self.inputData.append(matchData)
                else: continue

        self.numCmds = len(self.inputData)

    def signal_to_slot(self):
        self.ctrlLimitList.currentRowChanged.connect(lambda:functions.display_stack_slot(self.stackedWidget,self.ctrlLimitList.currentRow()))
        self.removeBtn.clicked.connect(lambda:functions.remove_stack_slot(self.stackedWidget,self.ctrlLimitList,self.updateLimitModes))
        self.addBtn.clicked.connect(lambda:functions.add_stack_slot(self.stackedWidget,self.ctrlLimitList,self.init_stack_ui,'Limits_',self.updateLimitModes))
        self.applyBtn.clicked.connect(self.save_data_slot)

    def save_data_slot(self):
        self.saveData = [""]*300
        self.saveData[0] = "#-------------------------------------------------------------------------------\n"
        self.saveData[1] = "#                   Translational / Attitude Control Limits\n"
        self.saveData[2] = "#-------------------------------------------------------------------------------\n"
        self.saveData[3] = '# Col: 1   ->  Cmd Interp ID Flag: "Limits_#"\n'
        self.saveData[4] = "# Col: 2   ->  Max. Acceleration Direction 1, frc_max/trq_max\n"
        self.saveData[5] = "# Col: 3   ->  Max. Acceleration Direction 2, frc_max/trq_max\n"
        self.saveData[6] = "# Col: 4   ->  Max. Acceleration Direction 3, frc_max/trq_max\n"
        self.saveData[7] = "# Col: 5   ->  Max. Velocity Direction 1, vel_max/w_max\n"
        self.saveData[8] = "# Col: 6   ->  Max. Velocity Direction 2, vel_max/w_max\n"
        self.saveData[9] = "# Col: 7   ->  Max. Velocity Direction 3, vel_max/w_max\n"
        self.saveData[10] = "# Col: 9   ->  Set of Control Limits\n"
        self.saveData[11] = "\n"

        txt_idx = 12
        for i in range(self.ctrlLimitList.count()):
            stack = self.stackedWidget.widget(i)
            col_1 = stack.maxAccelerationDirection1.text()
            col_2 = stack.maxAccelerationDirection2.text()
            col_3 = stack.maxAccelerationDirection3.text()
            col_4 = stack.maxVelocityDirection1.text()
            col_5 = stack.maxVelocityDirection2.text()
            col_6 = stack.maxVelocityDirection3.text()

            self.saveData[txt_idx] = "Limits_%s"%i + "   " + col_1 + "   "\
             + col_2 + "   " + col_3 + "   " + col_4 + "   " + col_5 \
             + "   " + col_6 +"\n"

            txt_idx += 1

class ActuatorCtrlMenu(QWidget):

    updateActuatorModes = QtCore.pyqtSignal()

    def __init__(self,momentumDumpMenu):
        super().__init__()
        self.setWindowTitle('Control Actuators Sub-Menu')

        self.momentumDumpMenu = momentumDumpMenu
        self.momentumDumpModes = functions.get_list_items(momentumDumpMenu.cmdList)
        self.decode_momentum_dump_options()

        self.sectionFlag = 'Actuators_'
        self.sectionHeader = 'Control Actuators'
        self.NUM_INPUTS = 2
        self.widgetTypeList = [2,2]

        self.missionDir = functions.get_mission_directory(self)
        inputFile = self.missionDir + '/InOut/Inp_DSM.txt'
        DSM_Functions.get_input_data(self, inputFile)
        DSM_Functions.get_labels(self, inputFile)

        self.cmdList = QListWidget()
        self.stackedWidget = QStackedWidget()
        if self.numCmds == 0:
            self.cmdList.insertItem(0, self.sectionFlag + "0")
            self.init_stack_ui(0)
        else:
            for i in range(self.numCmds):
                self.cmdList.insertItem(i, self.sectionFlag + "%s"%i)
                self.init_stack_ui(i)
                self.init_widget_field(i)

        self.addRemoveBtnLayout = functions.remove_add_btns(self,['removeBtn','addBtn'])
        self.defaultSaveBtnLayout = functions.default_save_btns(self,['defaultBtn','saveBtn','saveDefaultBtn'])

        cmdMenuLayout = QGridLayout()
        cmdMenuLayout.addWidget(self.cmdList,0,0)
        cmdMenuLayout.addLayout(self.addRemoveBtnLayout,1,0)
        cmdMenuLayout.addLayout(self.defaultSaveBtnLayout,1,1)
        cmdMenuLayout.addWidget(self.stackedWidget,0,1)

        self.setLayout(cmdMenuLayout)

        self.signal_to_slot()

        self.save_data_slot()

    def init_stack_ui(self,stackIndex):
        self.stack = QWidget()

        self.labelList = functions.generate_label_widgets(self.labelData)

        self.stack.input_col_0 = QComboBox()
        self.stack.input_col_0_0 = QComboBox()
        self.stack.input_col_1 = QComboBox()

        self.stack.input_col_0.addItems(self.modesData[0])
        self.stack.input_col_0_0.addItems(self.momentumDumpModeNum)
        self.stack.input_col_1.addItems(self.modesData[1])

        input_col_0_layoutB = QHBoxLayout()
        input_col_0_layoutB.addWidget(self.stack.input_col_0)
        input_col_0_layoutB.addWidget(self.stack.input_col_0_0)

        stackLayout = QFormLayout()
        stackLayout.setLabelAlignment(Qt.AlignLeft)
        stackLayout.setFormAlignment(Qt.AlignLeft)
        stackLayout.addRow(self.labelList[0],input_col_0_layoutB)
        stackLayout.addRow(self.labelList[1],self.stack.input_col_1)

        self.stack.setLayout(stackLayout)
        self.stackedWidget.addWidget(self.stack)

        self.stackedWidget.widget(stackIndex).input_col_0.currentIndexChanged.connect(lambda:self.display_momentum_dump_options(stackIndex))

    def init_widget_field(self,stackIndex):
        stack = self.stackedWidget.widget(stackIndex)
        WHEEL_PATTERN = re.compile('WHL_([0-9]+)')

        if (WHEEL_PATTERN.match(self.inputData[stackIndex][0])):
            momentumModeNum = WHEEL_PATTERN.match(self.inputData[stackIndex][0]).group(1)
            itemIndex = stack.input_col_0.findText('WHL')
            stack.input_col_0.setCurrentIndex(itemIndex)
            itemIndex = stack.input_col_0_0.findText(momentumModeNum)
            stack.input_col_0_0.setCurrentIndex(itemIndex)
        else:
            itemIndex = stack.input_col_0.findText(self.inputData[stackIndex][0])
            stack.input_col_0.setCurrentIndex(itemIndex)

        itemIndex = stack.input_col_1.findText(self.inputData[stackIndex][1])
        stack.input_col_1.setCurrentIndex(itemIndex)

        self.display_momentum_dump_options(stackIndex)

    def decode_momentum_dump_options(self):
        self.momentumDumpModeNum = []
        for mode in self.momentumDumpModes:
            modeNum = re.findall(r'\d+',mode)
            self.momentumDumpModeNum.extend(modeNum)

    def update_momentum_dump_options(self):
        self.momentumDumpModes = functions.get_list_items(self.momentumDumpMenu.cmdList)
        self.decode_momentum_dump_options()
        for i in range(self.cmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.input_col_0_0.clear()
            stack.input_col_0_0.addItems(self.momentumDumpModeNum)

    def display_momentum_dump_options(self,stackIndex):
        stack = self.stackedWidget.widget(stackIndex)
        if stack.input_col_0.currentText() == "WHL":
            stack.input_col_0_0.show()
        else:
            stack.input_col_0_0.hide()

    def save_data_slot(self):
        self.saveData = []

        # adds the pre-written section layout to saveslot
        for line in self.writeData:
            self.saveData.append(line)
        self.saveData.append("\n")

        txt_idx = len(self.saveData)

        # gets data for a command and stores it for writing later
        for i in range(self.cmdList.count()):
            stack = self.stackedWidget.widget(i)
            self.saveData.append(self.sectionFlag + "%s"%i)

            if (stack.input_col_0.currentText() == 'WHL'):
                self.saveData[txt_idx] += "   " + 'WHL_' + stack.input_col_0_0.currentText()
            else:
                self.saveData[txt_idx] += "   " + stack.input_col_0.currentText()

            self.saveData[txt_idx] += "   " + stack.input_col_1.currentText()

            self.saveData[txt_idx] += "\n"

            txt_idx += 1

    def save_default_slot(self):
        self.saveDefaultData = []

        # adds the pre-written section layout to saveslot
        for line in self.writeData:
            self.saveDefaultData.append(line)
        self.saveDefaultData.append("\n")

        txt_idx = len(self.saveDefaultData)

        # gets data for a command and stores it for writing later
        for i in range(self.cmdList.count()):
            stack = self.stackedWidget.widget(i)
            self.saveDefaultData.append(self.sectionFlag + "%s"%i)

            if (stack.input_col_0.currentText() == 'WHL'):
                self.saveDefaultData[txt_idx] += "   " + 'WHL_' + stack.input_col_0_0.currentText()
            else:
                self.saveDefaultData[txt_idx] += "   " + stack.input_col_0.currentText()

            self.saveDefaultData[txt_idx] += "   " + stack.input_col_1.currentText()

            self.saveDefaultData[txt_idx] += "\n"

            txt_idx += 1

    def signal_to_slot(self):
        self.cmdList.currentRowChanged.connect(lambda:DSM_Functions.display_stack_slot(self,self.cmdList.currentRow()))
        self.removeBtn.clicked.connect(lambda:DSM_Functions.remove_stack_slot(self,self.updateActuatorModes))
        self.addBtn.clicked.connect(lambda:DSM_Functions.add_stack_slot(self,self.updateActuatorModes,1))
        self.saveBtn.clicked.connect(lambda:self.save_data_slot())
        self.saveDefaultBtn.clicked.connect(lambda:self.save_default_slot())
        self.defaultBtn.clicked.connect(lambda:DSM_Functions.default_data_slot(self))

        self.momentumDumpMenu.updateMomentumDumpModes.connect(self.update_momentum_dump_options)

class ManeuverCmdlMenu(QWidget):
    updateManeuverModes = QtCore.pyqtSignal()

    def __init__(self, ctrlParameterMenu, ctrlLimitMenu, actuatorMenu):
        super().__init__()
        self.setWindowTitle('Manuever Command Sub-Menu')

        self.sectionFlag = 'ManeuverCmd_'
        self.sectionHeader = 'Maneuver Command'
        self.NUM_INPUTS = 7
        self.widgetTypeList = [1,1,1,1,2,2,2]

        self.missionDir = functions.get_mission_directory(self)
        inputFile = self.missionDir + '/InOut/Inp_DSM.txt'
        DSM_Functions.get_input_data(self, inputFile)
        DSM_Functions.get_labels(self, inputFile)

        self.ctrlParameterMenu = ctrlParameterMenu
        self.ctrlLimitMenu = ctrlLimitMenu
        self.actuatorMenu = actuatorMenu

        self.ctrlGainModes = functions.get_list_items(ctrlParameterMenu.cmdList)
        self.ctrlLimitModes = functions.get_list_items(ctrlLimitMenu.ctrlLimitList)
        self.actuatorModes = functions.get_list_items(actuatorMenu.cmdList)

        self.cmdList = QListWidget()
        self.stackedWidget = QStackedWidget()
        if self.numCmds == 0:
            self.cmdList.insertItem(0, self.sectionFlag + "0")
            DSM_Functions.init_stack_ui(self,0)
        else:
            for i in range(self.numCmds):
                self.cmdList.insertItem(i, self.sectionFlag + "%s"%i)
                DSM_Functions.init_stack_ui(self,i)
                DSM_Functions.init_widget_field(self,i)

        self.addRemoveBtnLayout = functions.remove_add_btns(self,['removeBtn','addBtn'])
        self.defaultSaveBtnLayout = functions.default_save_btns(self,['defaultBtn','saveBtn','saveDefaultBtn'])

        cmdMenuLayout = QGridLayout()
        cmdMenuLayout.addWidget(self.cmdList,0,0)
        cmdMenuLayout.addLayout(self.addRemoveBtnLayout,1,0)
        cmdMenuLayout.addLayout(self.defaultSaveBtnLayout,1,1)
        cmdMenuLayout.addWidget(self.stackedWidget,0,1)

        self.setLayout(cmdMenuLayout)

        self.signal_to_slot()

        DSM_Functions.save_data_slot(self)

    def update_actuator_options(self):
        self.actuatorModes = functions.get_list_items(self.actuatorMenu.cmdList)
        for i in range(self.cmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.input_col_6.clear()
            stack.input_col6.addItems(self.actuatorModes)

    def update_ctrl_limit_options(self):
        self.ctrlLimitModes = functions.get_list_items(self.ctrlLimitMenu.ctrlLimitList)
        for i in range(self.cmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.input_col_5.clear()
            stack.input_col_5.addItems(self.ctrlLimitModes)

    def update_ctrl_parameter_options(self):
        self.ctrlGainModes = functions.get_list_items(self.ctrlParameterMenu.cmdList)
        for i in range(self.cmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.input_col_4.clear()
            stack.input_col_4.addItems(self.ctrlGainModes)

    def signal_to_slot(self):
        self.cmdList.currentRowChanged.connect(lambda:DSM_Functions.display_stack_slot(self,self.cmdList.currentRow()))
        self.removeBtn.clicked.connect(lambda:DSM_Functions.remove_stack_slot(self,self.updateManeuverModes))
        self.addBtn.clicked.connect(lambda:DSM_Functions.add_stack_slot(self,self.updateManeuverModes,0))
        self.saveBtn.clicked.connect(lambda:DSM_Functions.save_data_slot(self))
        self.defaultBtn.clicked.connect(lambda:DSM_Functions.default_data_slot(self))

class MomentumDumpMenu(QWidget):
    updateMomentumDumpModes = QtCore.pyqtSignal()

    def __init__(self, ctrlParameterMenu):
        super().__init__()
        self.setWindowTitle('Momentum Dump Sub-Menu')

        self.sectionFlag = 'MomentumDump_'
        self.sectionHeader = 'Momentum Dump'
        self.NUM_INPUTS = 2
        self.widgetTypeList = [3,2]

        self.missionDir = functions.get_mission_directory(self)
        inputFile = self.missionDir + '/InOut/Inp_DSM.txt'
        DSM_Functions.get_input_data(self, inputFile)
        DSM_Functions.get_labels(self, inputFile)

        self.ctrlParameterMenu = ctrlParameterMenu

        self.ctrlGainModes = functions.get_list_items(ctrlParameterMenu.cmdList)

        self.cmdList = QListWidget()
        self.stackedWidget = QStackedWidget()
        if self.numCmds == 0:
            self.cmdList.insertItem(0, self.sectionFlag + "0")
            DSM_Functions.init_stack_ui(self,0)
        else:
            for i in range(self.numCmds):
                self.cmdList.insertItem(i, self.sectionFlag + "%s"%i)
                DSM_Functions.init_stack_ui(self,i)
                DSM_Functions.init_widget_field(self,i)

        self.addRemoveBtnLayout = functions.remove_add_btns(self,['removeBtn','addBtn'])
        self.defaultSaveBtnLayout = functions.default_save_btns(self,['defaultBtn','saveBtn','saveDefaultBtn'])

        cmdMenuLayout = QGridLayout()
        cmdMenuLayout.addWidget(self.cmdList,0,0)
        cmdMenuLayout.addLayout(self.addRemoveBtnLayout,1,0)
        cmdMenuLayout.addLayout(self.defaultSaveBtnLayout,1,1)
        cmdMenuLayout.addWidget(self.stackedWidget,0,1)

        self.setLayout(cmdMenuLayout)

        self.signal_to_slot()

        DSM_Functions.save_data_slot(self)

    def update_ctrl_parameter_options(self):
        self.ctrlGainModes = functions.get_list_items(self.ctrlParameterMenu.cmdList)
        for i in range(self.cmdList.count()):
            stack = self.stackedWidget.widget(i)
            stack.input_col_1.clear()
            stack.input_col_1.addItems(self.ctrlGainModes)

    def signal_to_slot(self):
        self.cmdList.currentRowChanged.connect(lambda:DSM_Functions.display_stack_slot(self,self.cmdList.currentRow()))
        self.removeBtn.clicked.connect(lambda:DSM_Functions.remove_stack_slot(self,self.updateMomentumDumpModes))
        self.addBtn.clicked.connect(lambda:DSM_Functions.add_stack_slot(self,self.updateMomentumDumpModes,0))
        self.saveBtn.clicked.connect(lambda:DSM_Functions.save_data_slot(self))
        self.defaultBtn.clicked.connect(lambda:DSM_Functions.default_data_slot(self))
        self.ctrlParameterMenu.updateGainModes.connect(self.update_ctrl_parameter_options)
