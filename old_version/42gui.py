# This is a test file to click a button to launch a secondary window

# Import Packages---------------------------------------------------------------
import sys
import numpy as numpy
import os
import shutil
import subprocess as sub
from os.path import expanduser
from pathlib import Path
import functions
from PyQt5 import QtCore

# from subprocess import call
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
from PyQt5.QtWidgets import QFormLayout
from PyQt5.QtWidgets import QButtonGroup
from PyQt5.QtWidgets import QFileDialog
from PyQt5.QtWidgets import QMessageBox

# Import Other files from repo
import FOV, SIM, CMD, SC, NOS3, TDRS, IPC, ORB, Region, Graphics, DSM, loadPathWindow

# Create Main window Class------------------------------------------------------
#-------------------------------------------------------------------------------
class MainWindow(QWidget):
    # Methods
    def __init__(MainWindow):
        super().__init__()
        #---------------------Main Window---------------------------------------
        MainWindow.setWindowTitle('42 GUI') # sets the window’s title
        MainWindow.setFixedSize(350, 300) # gives window a fixed size (width, height)
        MainLayout = QGridLayout()

        #-----------------------Widgets-----------------------------------------
        # Make Label for array of Buttons
        Label = QLabel("42 Sub-Menus")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        MainLayout.addWidget(Label, 0, 0, 1, 2)

        btn7 = QPushButton('DSM')
        btn7.clicked.connect(MainWindow.DSM_SubWindow)
        btn7.setFixedSize(310, 40)
        MainLayout.addWidget(btn7, 1, 0, 1, 2)
        MainWindow.DSM_Menu = []

        # Make Button for Orbits Menu
        btn1 = QPushButton('Orbits')
        btn1.clicked.connect(MainWindow.OrbSubWindow)
        btn1.setFixedSize(150, 40)
        MainLayout.addWidget(btn1, 2, 0)
        MainWindow.OrbitMenu = []

        # Make Button for Spacecraft Menu
        btn3 = QPushButton('Spacecraft')
        btn3.clicked.connect(MainWindow.SCSubWindow)
        btn3.setFixedSize(150, 40)
        MainLayout.addWidget(btn3, 3, 0)
        MainWindow.SCMenu = []

        # Make Button for Simulation Menu
        btn5 = QPushButton('Simulation')
        btn5.clicked.connect(MainWindow.SimSubWindow)
        btn5.setFixedSize(150, 40)
        MainLayout.addWidget(btn5, 4, 0)
        MainWindow.SimulationMenu = []

        # Make Button for IPC Menu
        btn9 = QPushButton('IPC')
        btn9.clicked.connect(MainWindow.IPC_SubWindow)
        btn9.setFixedSize(150, 40)
        MainLayout.addWidget(btn9, 5, 0)
        MainWindow.IPC_Menu = []

        # Make Button for Graphics Menu
        btn4 = QPushButton('Graphics')
        btn4.clicked.connect(MainWindow.GraphicsSubWindow)
        btn4.setFixedSize(150, 40)
        MainLayout.addWidget(btn4, 2, 1)
        MainWindow.GraphicsMenu = []

        # Make Button for FOV Menu
        btn6 = QPushButton('FOV')
        btn6.clicked.connect(MainWindow.FOV_SubWindow)
        btn6.setFixedSize(150, 40)
        MainLayout.addWidget(btn6, 3, 1)
        MainWindow.FOV_Menu = []

        # Make Button for Region Menu
        btn8 = QPushButton('Region')
        btn8.clicked.connect(MainWindow.RegionSubWindow)
        btn8.setFixedSize(150, 40)
        MainLayout.addWidget(btn8, 4, 1)
        MainWindow.RegionMenu = []

        # Make Button for TDRS Menu
        btn10 = QPushButton('TDRS')
        btn10.clicked.connect(MainWindow.TDRS_SubWindow)
        btn10.setFixedSize(150, 40)
        MainLayout.addWidget(btn10, 5, 1)
        MainWindow.TDRS_Menu = []

        SubLayout = QHBoxLayout()
        loadBtn = QPushButton('Load')
        loadBtn.clicked.connect(MainWindow.loadMissionWindow)
        SubLayout.addWidget(loadBtn)
        MainWindow.loadMissionMenu = []

        loadPath42Btn = QPushButton('42 Path')
        loadPath42Btn.clicked.connect(MainWindow.loadPathWindow42)
        SubLayout.addWidget(loadPath42Btn)
        MainWindow.loadPathMenu42 = []

        ApplyBtn = QPushButton('Run')
        ApplyBtn.clicked.connect(MainWindow.close)
        ApplyBtn.clicked.connect(MainWindow.run_sim)
        # Connect to run 42 with applied inputs
        SubLayout.addWidget(ApplyBtn)
        MainLayout.addLayout(SubLayout, 6, 0, 1, 2)

        # Set Layout for Main Window--------------------------------------------
        MainWindow.setLayout(MainLayout)

    #---------Call all of the sub windows---------------------------------------
    def OrbSubWindow(MainWindow):
        if MainWindow.OrbitMenu == []:
            MainWindow.OrbitMenu = ORB.OrbitWindow()
        MainWindow.OrbitMenu.show()

    def CmdSubWindow(MainWindow):
        if MainWindow.CmdMenu == []:
            MainWindow.CmdMenu = CMD.CmdWindow()
        MainWindow.CmdMenu.show()

    def SCSubWindow(MainWindow):
        if MainWindow.SCMenu == []:
            MainWindow.SCMenu = SC.SC_Window()
        MainWindow.SCMenu.show()

    def GraphicsSubWindow(MainWindow):
        if MainWindow.GraphicsMenu == []:
            MainWindow.GraphicsMenu = Graphics.GraphicsWindow()
        MainWindow.GraphicsMenu.show()

    def SimSubWindow(MainWindow):
        if MainWindow.SimulationMenu == []:
            MainWindow.SimulationMenu = SIM.SimWindow()
        MainWindow.SimulationMenu.show()

    def FOV_SubWindow(MainWindow):
        if MainWindow.FOV_Menu == []:
            MainWindow.FOV_Menu = FOV.FOV_Window()
        MainWindow.FOV_Menu.show()

    def DSM_SubWindow(MainWindow):
        if MainWindow.DSM_Menu == []:
            MainWindow.DSM_Menu = DSM.DSM_Window()
        MainWindow.DSM_Menu.show()

    def RegionSubWindow(MainWindow):
        if MainWindow.RegionMenu == []:
            MainWindow.RegionMenu = Region.RegionWindow()
        MainWindow.RegionMenu.show()

    def IPC_SubWindow(MainWindow):
        if MainWindow.IPC_Menu == []:
            MainWindow.IPC_Menu = IPC.IPC_Window()
        MainWindow.IPC_Menu.show()

    def TDRS_SubWindow(MainWindow):
        if MainWindow.TDRS_Menu == []:
            MainWindow.TDRS_Menu = TDRS.TDRS_Window()
        MainWindow.TDRS_Menu.show()

    def loadMissionWindow(MainWindow):
        if MainWindow.loadMissionMenu == []:
            MainWindow.loadMissionMenu = loadPathWindow.loadMissionWindow()

        MainWindow.loadMissionMenu.show()

    def loadPathWindow42(MainWindow):
        if MainWindow.loadPathMenu42 == []:
            MainWindow.loadPathMenu42 = loadPathWindow.loadPathWindow42()
        MainWindow.loadPathMenu42.show()

    def run_sim(self):
        try:
            functions.overwrite_directory(self,self.destinationDir,self.loadPathMenu42.dir42)
            os.chdir(self.loadPathMenu42.dir42)
            os.system("make clean")
            os.system("make -j12")

        except:
            errorMsg = functions.create_notice(self,"Error: Invalid 42 Path.")
            errorMsg.exec_()
        # os.system("./run.sh + " Blah 0")

class StartUpWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('42 GUI: Start-up')

        self.loadMissionWindow = loadPathWindow.loadMissionWindow()
        self.setCentralWidget(self.loadMissionWindow)

        self.loadMissionWindow.loadFileTrue.connect(self.start_main_menu)

    def start_main_menu(self):
        self.mainMenuWindow = MainWindow()
        self.mainMenuWindow.show()
        self.hide()

# Launch GUI -------------------------------------------------------------------
def main():
    app = QApplication(sys.argv) # Create an instance of QApplication

    # startUpWindow = StartUpWindow()
    # startUpWindow.show()

    mainMenuWindow = MainWindow()
    mainMenuWindow.show()


    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
