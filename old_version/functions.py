import sys
import re
import os
import shutil
from pathlib import Path

# Import QApplication and the required widgets from PyQt5.QtWidgets
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QLabel
from PySide6.QtWidgets import QPushButton # for the butons
from PySide6.QtWidgets import QHBoxLayout # Horizontal stack layout
from PySide6.QtWidgets import QMessageBox

def whitespace(data):
    empty_space = '                              '
    data_len = len(empty_space)-len(data)
    if data_len < 1: data_len = 1
    message = data+empty_space[0:data_len]
    return message

def check_input_file_exists(self,inputFileName):
    # openFile = open(inputFileName,'r')
    try:
        f = open(inputFileName)
    except:
        f = open(inputFileName,'w+')
        self.FileNotFound = True
    finally:
        f.close()

def generate_label_widgets(labels):
    labelList = []
    for label in labels:
        Label = QLabel(label + ":")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        labelList.append(Label)

    return labelList

def read_input_labels(self):
    self.InputLabel = []

    readFile = open('InOut/Inp_Orbit.txt', 'r')
    self.InputLabel= readFile.readlines()

    for i, line in enumerate(self.inputData):
        for match in re.finditer(re.compile("!\s*(.*)"),line):
            self.InputLabel.append(match.group(1))

def remove_add_btns(self,btnNames):
    setattr(self,btnNames[0],QPushButton('-'))
    setattr(self,btnNames[1],QPushButton('+'))

    addRemoveBtnLayout = QHBoxLayout()
    addRemoveBtnLayout.addWidget(getattr(self,btnNames[0]))
    addRemoveBtnLayout.addWidget(getattr(self,btnNames[1]))

    return addRemoveBtnLayout

def default_cancel_apply_btns(self,btnNames):
    setattr(self,btnNames[0],QPushButton('Load Default'))
    setattr(self,btnNames[1],QPushButton('Save Default'))
    setattr(self,btnNames[2],QPushButton('Cancel'))
    setattr(self,btnNames[3],QPushButton('Apply'))

    Layout = QHBoxLayout()
    Layout.addWidget(getattr(self,btnNames[0]))
    Layout.addWidget(getattr(self,btnNames[1]))
    Layout.addWidget(getattr(self,btnNames[2]))
    Layout.addWidget(getattr(self,btnNames[3]))

    return Layout

def cancel_apply_btns(self,btnNames):
    setattr(self,btnNames[0],QPushButton('Cancel'))
    setattr(self,btnNames[1],QPushButton('Apply'))

    Layout = QHBoxLayout()
    Layout.addWidget(getattr(self,btnNames[0]))
    Layout.addWidget(getattr(self,btnNames[1]))

    return Layout
    
def default_save_btns(self,btnNames):
    setattr(self,btnNames[0],QPushButton('Default'))
    setattr(self,btnNames[1],QPushButton('Save'))
    setattr(self,btnNames[2],QPushButton('Save as Default'))

    getSaveDefaultBtn = getattr(self,btnNames[2])
    getDefaultBtn = getattr(self,btnNames[0])
    getSaveBtn = getattr(self,btnNames[1])

    defaultSaveBtnLayout = QHBoxLayout()
    defaultSaveBtnLayout.addWidget(getDefaultBtn)
    defaultSaveBtnLayout.addWidget(getSaveDefaultBtn)
    defaultSaveBtnLayout.addWidget(getSaveBtn)

    return defaultSaveBtnLayout

def cancel_save_btns(self,btnNames):
    setattr(self,btnNames[0],QPushButton('Cancel'))
    setattr(self,btnNames[1],QPushButton('Save'))

    getCancelBtn = getattr(self,btnNames[0])
    getSaveBtn = getattr(self,btnNames[1])

    applyCancelBtnLayout = QHBoxLayout()
    applyCancelBtnLayout.addWidget(getCancelBtn)
    applyCancelBtnLayout.addWidget(getSaveBtn)

    return applyCancelBtnLayout

def display_stack_slot(stackedWidget, i): # Displays current page in Stack when selected in the Region list
    stackedWidget.setCurrentIndex(i)

# Adds a stack to QListWidget and QStackWidget
def add_stack_slot(
    stackedWidgetObject,listObject,initStackMethod,itemName,
    signal):

    itemIndex = listObject.count() # Index of new item

    listObject.insertItem(int(itemIndex), itemName + "_%s"%itemIndex)

    initStackMethod(itemIndex)

    listObject.setCurrentRow(itemIndex)
    stackedWidgetObject.setCurrentIndex(itemIndex)

    signal.emit()

def remove_stack_slot(stackedWidgetObject,listObject,signal): # Removes the last stack added to a QListWidget and QStackedWidget
    RmvIndex = listObject.count()

    if RmvIndex > 1:
        listObject.takeItem(RmvIndex - 1)
        stackedWidgetObject.removeWidget(stackedWidgetObject.widget(RmvIndex - 1))

        signal.emit()

def write_to_file_slot(self,fileName):
    self.WriteFile = open(fileName, 'w+')
    self.WriteFile.writelines(self.saveData)
    self.WriteFile.close()


def empty_widget_flag(self,widgets):
    widgets = ["LabelFov","NumSide","LengthSide","Width","Height",
    "RedColor","GreenColor","BlueColor","AlphaColor","SC","Body",
    "X_Axis_Body","Y_Axis_Body","Z_Axis_Body","FirstSequence","ThirdSequence","Sequence"]

    self.emptyWidgetFlag = False

    for x in range(stackAmount):
        for widget in widgets:
            if not getattr(self.FovProperties.widget(x),widget).text():
                errorMsg = QMessageBox()
                errorMsg.setText("Error: Fill out empty entries before applying!")
                errorMsg.exec_()
                self.emptyWidgetFlag = True
                return
        if self.emptyWidgetFlag:return

def default_data(self,stackedWidget,listWidget, numCmds, init_ui, init_widget, listItemName):

    # clear stacked widget object
    for widget in range(stackedWidget.count()):
        stackedWidget.removeWidget(stackedWidget.widget(widget))

    # clear list object
    listWidget.clear()

    # create default list and stacked widget items
    if numCmds == 0:
        numCmds += 1
        listWidget.insertItem(0, listItemName+"%d"%0)
        init_ui(0)
    else:
        for i in range(numCmds):
            listWidget.insertItem(i, listItemName+"%s"%i)
            self.init_ui(i)
            self.init_widget(i)

def get_list_items(listWidget):
    items = []
    for x in range(listWidget.count()):
        items.append(listWidget.item(x).text())

    return items

def get_unique_items(iterable):
    result = []
    for item in iterable:
        if item not in result:
            result.append(item)
    return result

def create_warning_message(self,warningText):
    warningMsg = QMessageBox()
    warningMsg.setIcon(QMessageBox.Warning)
    warningMsg.setText(warningText)
    warningMsg.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel)

    return warningMsg.exec()

def create_notice(self,messageText):
    noticeBox = QMessageBox()
    noticeBox.setStandardButtons(QMessageBox.Ok)
    noticeBox.setText(messageText)

    return noticeBox

def overwrite_directory(self,src,dest):
    shutil.rmtree(dest)
    shutil.copytree(src, dest)

def write_to_config(self,writeData):
    self.WriteFile = open(self.configDir, 'w+')
    self.WriteFile.writelines(writeData)
    self.WriteFile.close()

def get_mission_directory(self):
    MISSION_DIR_FLAG = re.compile(r"^MissionDir:(.+)\/([^\/]+)$")

    cwd = Path.cwd()
    configDir = os.path.join(cwd, '.config')

    for i, line in enumerate(open(configDir)):
        if MISSION_DIR_FLAG.match(line):
            missionDir = MISSION_DIR_FLAG.match(line).group(1)

            return missionDir

def get_sc_orbit_file_names(self):

    missionDir = get_mission_directory(self)

    self.orbFileNames = []
    self.scFileNames = []

    ORBIT_FILE_FLAG = re.compile('Orb_', re.IGNORECASE)
    SC_FILE_FLAG = re.compile('SC_', re.IGNORECASE)

    for file in os.listdir(missionDir + '/InOut'):
        if ORBIT_FILE_FLAG.match(file):
            self.orbFileNames.append(file)
        elif SC_FILE_FLAG.match(file):
            self.scFileNames.append(file)
