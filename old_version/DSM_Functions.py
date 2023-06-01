
import re
import functions
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QLabel, QGridLayout,
QLineEdit, QPushButton, QComboBox, QVBoxLayout, QHBoxLayout, QRadioButton, QTableWidget,
QListWidget, QFormLayout, QStackedWidget, QTableWidgetItem, QCompleter, QTabWidget, QButtonGroup)
from PyQt5 import QtCore
from PyQt5.QtCore import (Qt, QObjectCleanupHandler)
from PyQt5.QtGui import QRegExpValidator
from PyQt5.QtCore import QRegExp

# Functions for DSM Subwindow Development

def get_input_data(self,fileName):
    self.inputData = []

    # create regex pattern for a cmd
    val = "\s*([+-]?[a-zA-Z\_\.0-9]*)"
    pattern = self.sectionFlag + "[0-9]+"
    for colNum in range(self.NUM_INPUTS): pattern += val
    pattern = re.compile(pattern,re.IGNORECASE)

    # obtains inputs from file and stores
    for i, line in enumerate(open(fileName)):
        for match in re.finditer(pattern, line):
            if match.group(self.NUM_INPUTS) != "": # flag to check if actual cmd
                matchData = []
                for group in match.groups():
                    if group != "": matchData.append(group)
                    else: matchData.append(" ")
                self.inputData.append(matchData)
            else: continue

    self.numCmds = len(self.inputData)

def get_labels(self, fileName):
    self.labelData = []
    self.modesData = []
    self.writeData = []
    self.modeCheck = []

    cmdSectionFound = False

    # regex patterns for matching
    SECTION_HEADER = re.compile("#*\s*" + self.sectionHeader, re.IGNORECASE)
    COL_PATTERN = re.compile("#*\s*Col",re.IGNORECASE)
    MODES_PATTERN = re.compile("([a-zA-Z\_\s\.0-9]*):",re.IGNORECASE)
    BLANK_LINE = re.compile("^\s*\n",re.IGNORECASE)

    for i, line in enumerate(open(fileName)):
        # conditional for finding command type section
        if (SECTION_HEADER.match(line)):
            cmdSectionFound = True
            self.writeData.append("#-------------------------------------------------------------------------------\n")
            self.writeData.append(line)
            self.writeData.append("#-------------------------------------------------------------------------------\n")
            continue

        if (cmdSectionFound):
            # checks if still in command type section
            if BLANK_LINE.match(line):
                break
            # appends input label if found
            elif COL_PATTERN.match(line):
                self.writeData.append(line)
                line = line.strip()
                splitLine = re.split('\s*->\s*',line)
                if MODES_PATTERN.match(splitLine[1]):
                    self.modeCheck.append(1)
                    self.labelData.append(MODES_PATTERN.match(splitLine[1]).group(1))
                    modesDataList = re.findall(r'"(.+?)"',splitLine[1])
                    self.modesData.append(modesDataList)
                    continue
                self.labelData.append(splitLine[1])
                self.modesData.append(None)
                self.modeCheck.append(0)
            else: continue

    # removes flag column input data
    self.labelData.pop(0)
    self.modesData.pop(0)
    self.modeCheck.pop(0)

def get_cmd_list(self,fileName):
    self.cmdList = QListWidget()

    if self.numCmds == 0:
        self.cmdList.insertItem(0, self.sectionFlag + "0")
        DSM_Functions.init_stack_ui(self,0)
    else:
        SECTION_HEADER = re.compile("#*\s*" + self.sectionHeader, re.IGNORECASE)
        CMD_FLAG_PATTERN = re.compile(self.sectionFlag + "[0-9]+")
        END_SECTION = re.compile("^#end",re.IGNORECASE)
        cmdSectionFound = False

        for i, line in enumerate(open(fileName)):
            # conditional for finding command type section
            if (SECTION_HEADER.match(line)):
                cmdSectionFound = True
                continue

            if (cmdSectionFound):
                # checks if still in command type section
                if END_SECTION.match(line):
                    break

                if CMD_FLAG_PATTERN.match(line):
                    itemIndex = self.cmdList.count()
                    cmdFlag = CMD_FLAG_PATTERN.match(line).group(0)
                    self.cmdList.insertItem(int(itemIndex),cmdFlag)

                else: continue

def init_stack_ui(self,stackIndex):
    self.stack = QWidget()

    # creates a widget for each column input, based on widget type
    for colNum, inputType in enumerate(self.widgetTypeList):
        inputColName = 'input_col_%s'%colNum
        if inputType == 1:
            setattr(self.stack,inputColName,QLineEdit())
            widget = getattr(self.stack,inputColName)
            reg_ex = QRegExp("[0-9]+.?[0-9]{,2}")
            input_validator = QRegExpValidator(reg_ex, widget)
            widget.setValidator(input_validator)
        elif inputType == 2:
            setattr(self.stack,inputColName,QComboBox())
        elif inputType == 3:
            setattr(self.stack,inputColName + '_ON',QRadioButton('On'))
            setattr(self.stack,inputColName + '_OFF',QRadioButton('Off'))
            setattr(self.stack,inputColName + '_BtnGroup',QButtonGroup())

            widgetON = getattr(self.stack,inputColName + '_ON')
            widgetOFF = getattr(self.stack,inputColName + '_OFF')
            widgetBtnGroup = getattr(self.stack,inputColName + '_BtnGroup')

            widgetBtnGroup.addButton(widgetON)
            widgetBtnGroup.addButton(widgetOFF)
        else:
            print("Error: Invalid widget type for column " + str(colNum) + "\n")
            exit(1)

    # populates options for QComboBox type inputs
    GAIN_LABEL_PATTERN = re.compile('Set of Control Gains',re.IGNORECASE)
    LIMIT_LABEL_PATTERN = re.compile('Set of Control Limits',re.IGNORECASE)
    ACTUATOR_LABEL_PATTERN = re.compile('Actuator Setting',re.IGNORECASE)
    for colNum in range(self.NUM_INPUTS):
        inputColName = 'input_col_%s'%colNum
        try:
            widget = getattr(self.stack,inputColName)
        except:
            continue

        if self.modeCheck[colNum]:
            widget.addItems(self.modesData[colNum])
        if GAIN_LABEL_PATTERN.match(self.labelData[colNum]):
            widget.addItems(self.ctrlGainModes)
        if LIMIT_LABEL_PATTERN.match(self.labelData[colNum]):
            widget.addItems(self.ctrlLimitModes)
        if ACTUATOR_LABEL_PATTERN.match(self.labelData[colNum]):
            widget.addItems(self.actuatorModes)

    # creates label widgets from label data
    self.labelList = functions.generate_label_widgets(self.labelData)

    # adds column input widgets to layout of a stack
    stackLayout = QFormLayout()
    stackLayout.setLabelAlignment(Qt.AlignLeft)
    stackLayout.setFormAlignment(Qt.AlignLeft)
    for colNum in range(self.NUM_INPUTS):
        inputColName = 'input_col_%s'%colNum
        try:
            widget = getattr(self.stack,inputColName)
            stackLayout.addRow(self.labelList[colNum],widget)
        except:
            btnLayout = QHBoxLayout()
            widgetON = getattr(self.stack,inputColName + '_ON')
            widgetOFF = getattr(self.stack,inputColName + '_OFF')

            btnLayout.addWidget(widgetON)
            btnLayout.addWidget(widgetOFF)

            stackLayout.addRow(self.labelList[colNum],btnLayout)

    # sets the layouts and adds the stack to the stackedWidget
    self.stack.setLayout(stackLayout)
    self.stackedWidget.addWidget(self.stack)

def init_widget_field(self,stackIndex):
    stack = self.stackedWidget.widget(stackIndex)
    # uses column number to determine widget and its input, method of inputting
    # depends on type of widget
    for colNum in range(self.NUM_INPUTS):
        inputColName = 'input_col_%s'%colNum

        try:
            widget = getattr(stack,inputColName)
        except:
            widgetON = getattr(stack,inputColName + '_ON')
            widgetOFF = getattr(stack,inputColName + '_OFF')

            if (self.inputData[stackIndex][colNum].lower() == 'on'):
                widgetON.setChecked(True)
            else:
                widgetOFF.setChecked(True)
            continue

        if (isinstance(widget,QLineEdit)):
            widget.setText(self.inputData[stackIndex][colNum])

        elif (isinstance(widget,QComboBox)):
            itemIndex = widget.findText(self.inputData[stackIndex][colNum])
            widget.setCurrentIndex(itemIndex)

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

        for col_num in range(self.NUM_INPUTS):
            inputColName = 'input_col_%s'%col_num
            try:
                widget = getattr(stack,inputColName)
            except:
                widgetON = getattr(self.stack,inputColName + '_ON')

                if (widgetON.isChecked()):
                    self.saveData[txt_idx] += "   " + "TRUE"
                else:
                    self.saveData[txt_idx] += "   " + "FALSE"

                continue

            if (isinstance(widget,QLineEdit)):
                self.saveData[txt_idx] += "   " + widget.text()

            elif (isinstance(widget,QComboBox)):
                self.saveData[txt_idx] += "   " + widget.currentText()

        self.saveData[txt_idx] += " \n"

        txt_idx += 1


def default_data_slot(self):
    get_input_data(self,'InOut/Default/Inp_DSM_Default.txt')
    get_labels(self,'InOut/Default/Inp_DSM_Default.txt')

    # clear stacked widget object
    for widget in range(self.stackedWidget.count()):
        self.stackedWidget.removeWidget(self.stackedWidget.widget(widget))

    # clear list object
    self.cmdList.clear()

    # create default list and stacked widget items
    if self.numCmds == 0:
        self.numCmds += 1
        self.cmdList.insertItem(0, self.sectionFlag + "0")
        init_stack_ui(self, 0)
    else:
        for i in range(self.numCmds):
            self.cmdList.insertItem(i, self.sectionFlag + "%s"%i)
            init_stack_ui(self, i)
            init_widget_field(self, i)


def add_stack_slot(self,signal,bool):
    itemIndex = self.cmdList.count() # Index of new item

    self.cmdList.insertItem(int(itemIndex), self.sectionFlag + "%s"%itemIndex)

    if bool:
        self.init_stack_ui(itemIndex)
    else:
        init_stack_ui(self,itemIndex)

    self.cmdList.setCurrentRow(itemIndex)
    self.stackedWidget.setCurrentIndex(itemIndex)

    signal.emit()

def remove_stack_slot(self,signal):
    RmvIndex = self.cmdList.count()

    if RmvIndex > 1:
        self.cmdList.takeItem(RmvIndex - 1)
        self.stackedWidget.removeWidget(self.stackedWidget.widget(RmvIndex - 1))

        signal.emit()

def display_stack_slot(self,indexStack):
    self.stackedWidget.setCurrentIndex(indexStack)
