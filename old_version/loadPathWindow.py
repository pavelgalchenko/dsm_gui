
# Import Packages---------------------------------------------------------------
import os
import shutil

from pathlib import Path

from PySide6.QtWidgets import QWidget
from PySide6.QtWidgets import QGridLayout
from PySide6.QtWidgets import QLineEdit
from PySide6.QtWidgets import QPushButton
from PySide6.QtWidgets import QHBoxLayout
from PySide6.QtWidgets import QFileDialog
from PySide6.QtWidgets import QMessageBox

import functions

class loadMissionWindow(QWidget):

    def __init__(self):
        super().__init__()
        self.setWindowTitle('Deep Thought Terminal: Load Mission') # sets the window’s title
        self.setFixedSize(420, 240) # gives window a fixed size (width, height)

        startUpLayout = QGridLayout()

        self.cancelBtn = QPushButton('Cancel')
        self.loadOldBtn = QPushButton('Load Mission')
        self.pathInput = QLineEdit(placeholderText="Insert mission directory.")
        self.openFileDialogBtn = QPushButton("...")

        subLayout = QHBoxLayout()
        subLayout.addWidget(self.cancelBtn)
        subLayout.addWidget(self.loadOldBtn)

        startUpLayout.addWidget(self.pathInput,0,0)
        startUpLayout.addWidget(self.openFileDialogBtn,0,1)
        startUpLayout.addLayout(subLayout,1,0)

        self.setLayout(startUpLayout)

        self.cwd = Path.cwd()
        self.defaultDir = os.path.join(self.cwd, '__default__')
        self.configDir = os.path.join(self.cwd,'.config')
        self.cancelBtn.clicked.connect(self.cancel_btn_slot)
        self.loadOldBtn.clicked.connect(self.old_load)
        self.openFileDialogBtn.clicked.connect(self.get_directory)
        self.pathInput.textChanged.connect(self.get_path_text_input)

        self.get_path_text_input()

    def get_path_text_input(self):
        self.missionDir = self.pathInput.text()

    def get_directory(self):
        self.missionDir = QFileDialog.getExistingDirectory(
            self,
            caption='Select a folder'
        )
        self.pathInput.setText(self.missionDir)

    def cancel_btn_slot(self):
        self.pathInput.clear()
        self.close()

    def old_load(self):
        self.addedFiles_InOut = []
        self.addedFiles_Default = []
        self.missingFilesDesc = []

        if os.path.exists(self.missionDir):
            self.dest_InOut = os.path.join(self.missionDir, 'InOut')
            self.dest_Default = os.path.join(self.dest_InOut, 'Default')

            for src_dir, dirs, defaultFiles in os.walk(self.defaultDir):
                dst_dir = src_dir.replace(self.defaultDir, self.dest_InOut, 1)
                if not os.path.exists(dst_dir):
                    errorMsg = functions.create_notice(self,"Error: Mission missing InOut.")
                    errorMsg.exec_()
                    return
                for file in defaultFiles:
                    src_file = os.path.join(src_dir, file)
                    dst_file = os.path.join(dst_dir, file)
                    if not os.path.exists(dst_file):
                        shutil.copy(src_file,dst_file)
                        self.addedFiles_InOut.append(file)

            if not os.path.exists(self.dest_Default):
                os.mkdir(self.dest_Default)

            for file in os.listdir(self.dest_InOut):
                src_file = os.path.join(self.dest_InOut, file)
                dst_file = os.path.join(self.dest_Default, file)
                if not os.path.exists(dst_file) and os.path.isfile(src_file):
                    shutil.copy(src_file,dst_file)
                    self.addedFiles_Default.append(file)

            loadMissionMsg = functions.create_notice(self,"Mission successfully loaded.")
            loadMissionMsg.setFixedSize(150,100)

            if len(self.addedFiles_InOut) != 0:
                self.missingFilesDesc.append(self.dest_InOut + "/... \n" +  "\n".join(self.addedFiles_InOut))

            if len(self.addedFiles_Default) != 0:
                self.missingFilesDesc.append(self.dest_Default + "/... \n" + "\n".join(self.addedFiles_Default))

            if len(self.addedFiles_InOut) or len(self.addedFiles_Default) != 0:
                loadMissionMsg.setDetailedText("Missing files added: \n \n" + "\n \n".join(self.missingFilesDesc))

            loadMissionMsg.exec_()
            functions.write_to_config(self,"MissionDir:" + self.dest_InOut)
            self.close()
        else:
            errorMsg = functions.create_notice(self,"Error: No mission path chosen or does not exist.")
            errorMsg.exec_()

class newMissionWindow(QWidget):

    def __init__(self):
        super().__init__()
        self.setWindowTitle('Deep Thought Terminal: New Mission') # sets the window’s title
        self.setFixedSize(420, 240) # gives window a fixed size (width, height)

        startUpLayout = QGridLayout()

        self.cancelBtn = QPushButton('Cancel')
        self.loadNewBtn = QPushButton('Create Mission')
        self.pathInput = QLineEdit(placeholderText="Insert mission directory.")
        self.openFileDialogBtn = QPushButton("...")

        subLayout = QHBoxLayout()
        subLayout.addWidget(self.cancelBtn)
        subLayout.addWidget(self.loadNewBtn)

        startUpLayout.addWidget(self.pathInput,0,0)
        startUpLayout.addWidget(self.openFileDialogBtn,0,1)
        startUpLayout.addLayout(subLayout,1,0)

        self.setLayout(startUpLayout)

        self.cwd = Path.cwd()
        self.defaultDir = os.path.join(self.cwd, '__default__')
        self.cancelBtn.clicked.connect(self.cancel_btn_slot)
        self.loadNewBtn.clicked.connect(self.new_load)
        self.openFileDialogBtn.clicked.connect(self.get_directory)
        self.pathInput.textChanged.connect(self.get_path_text_input)

        self.get_path_text_input()

    def get_path_text_input(self):
        self.missionDir = self.pathInput.text()

    def get_directory(self):
        self.missionDir = QFileDialog.getExistingDirectory(
            self,
            caption='Select a folder'
        )
        self.pathInput.setText(self.missionDir)

    def cancel_btn_slot(self):
        self.pathInput.clear()
        self.close()

    def new_load(self):
        if os.path.exists(self.missionDir):
            self.dest_InOut = os.path.join(self.missionDir, 'InOut')
            self.dest_Default = os.path.join(self.dest_InOut, '__default__')
            self.configDir = os.path.join(self.cwd,'.config')

            try:
                shutil.copytree(self.defaultDir, self.dest_InOut)
                shutil.copytree(self.defaultDir, self.dest_Default)
                functions.write_to_config(self,"MissionDir:" + self.dest_InOut)
                self.close()

            except:
                response = functions.create_warning_message(self,"Mission detected in path. Overwrite files?")

                if response == QMessageBox.Ok:
                    functions.overwrite_directory(self,self.defaultDir,self.dest_InOut)
                    shutil.copytree(self.defaultDir, self.dest_Default)
                    overwriteNotice = functions.create_notice(self,"Overwrite successful.")
                    overwriteNotice.exec_()
                    functions.write_to_config(self,"MissionDir:" + self.dest_InOut)
                    self.close()
        else:
            errorMsg = functions.create_notice(self,"Error: No mission path chosen or does not exist.")
            errorMsg.exec_()
