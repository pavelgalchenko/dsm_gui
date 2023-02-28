# This Python file uses the following encoding: utf-8
import sys

from PySide6.QtWidgets import QApplication
from PySide6.QtWidgets import QMainWindow
from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QLabel
from PySide6.QtWidgets import QGridLayout # to arrange the buttons
from PySide6.QtWidgets import QPushButton # for the butons

# Import Other files from repo
import loadPathWindow, ORB, SIM, SC, Graphics

# Create Main window Class------------------------------------------------------
#-------------------------------------------------------------------------------
class MainWindow(QWidget):
    # Methods
    def __init__(self):
        super().__init__()
        #---------------------Main Window---------------------------------------
        self.setWindowTitle('Deep Thought Terminal') # sets the window’s title
        self.setFixedSize(420, 420) # gives window a fixed size (width, height)
        MainLayout = QGridLayout()

        #-----------------------Widgets-----------------------------------------
        # Make Label for array of Buttons
        Label = QLabel("Select Mission")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 18px')
        MainLayout.addWidget(Label, 0, 0,1,2)

        self.btn1 = QPushButton('New Mission')
        self.btn1.setFixedSize(315, 40)
        self.btn1.clicked.connect(self.newMissionWindow)
        MainLayout.addWidget(self.btn1, 1, 0, 1, 2)
        self.newMissionMenu = []

        self.btn2 = QPushButton('Load Mission')
        self.btn2.setFixedSize(315, 40)
        self.btn2.clicked.connect(self.loadMissionWindow)
        MainLayout.addWidget(self.btn2, 2, 0, 1, 2)
        self.loadMissionMenu = []

        Label2 = QLabel("42 Sub-Menus")
        Label2.setAlignment(Qt.AlignCenter)
        Label2.setStyleSheet('font-size: 18px')
        MainLayout.addWidget(Label2, 3, 0,1,2)

        # Make Button for Orbits Menu
        self.orbButton = QPushButton('Orbits')
        self.orbButton.clicked.connect(self.orbWindow)
        self.orbButton.setFixedSize(315, 40)
        self.orbButton.setEnabled(False)
        MainLayout.addWidget(self.orbButton, 4, 0, 1, 2)
        self.orbMenu = []

        # Make Button for Spacecraft Menu
        self.spcButton = QPushButton('Spacecraft')
        self.spcButton.clicked.connect(self.spcWindow)
        self.spcButton.setFixedSize(315, 40)
        self.spcButton.setEnabled(False)
        MainLayout.addWidget(self.spcButton, 5, 0, 1, 2)
        self.spcMenu = []

        # Make Button for Simulation Menu
        self.simButton = QPushButton('Simulation')
        self.simButton.clicked.connect(self.simWindow)
        self.simButton.setFixedSize(315, 40)
        self.simButton.setEnabled(False)
        MainLayout.addWidget(self.simButton, 6, 0, 1, 2)
        self.simMenu = []

        # Make Button for Graphics Menu
        self.grhButton = QPushButton('Graphics')
        self.grhButton.clicked.connect(self.grhWindow)
        self.grhButton.setFixedSize(120, 40)
        self.grhButton.setEnabled(False)
        MainLayout.addWidget(self.grhButton, 7, 0)
        self.grhMenu = []

        # Make Button for FOV Menu
        self.fovButton = QPushButton('FOV')
        self.fovButton.setFixedSize(120, 40)
        self.fovButton.setEnabled(False)
        MainLayout.addWidget(self.fovButton, 7, 1)
        self.FOV_Menu = []

        # Make Button for Region Menu
        self.regButton = QPushButton('Region')
        self.regButton.setFixedSize(120, 40)
        self.regButton.setEnabled(False)
        MainLayout.addWidget(self.regButton, 8, 0)
        self.RegionMenu = []

        # Make Button for TDRS Menu
        self.tdrButton = QPushButton('TDRS')
        self.tdrButton.setFixedSize(120, 40)
        self.tdrButton.setEnabled(False)
        MainLayout.addWidget(self.tdrButton, 8, 1)
        self.TDRS_Menu = []

        # Make Button for IPC Menu
        self.ipcButton = QPushButton('IPC')
        self.ipcButton.setFixedSize(120, 40)
        self.ipcButton.setEnabled(False)
        MainLayout.addWidget(self.ipcButton, 9, 0)
        self.IPC_Menu = []

        # Make Button for NOS3 Menu
        self.nosButton = QPushButton('NOS3')
        self.nosButton.setFixedSize(120, 40)
        self.nosButton.setEnabled(False)
        MainLayout.addWidget(self.nosButton, 9, 1)
        self.IPC_Menu = []

        # Make Button for DSM Menu
        self.dsmButton = QPushButton('DSM')
        self.dsmButton.setFixedSize(315, 40)
        self.dsmButton.setEnabled(False)
        MainLayout.addWidget(self.dsmButton, 10, 0, 1, 2)
        self.DSM_Menu = []

        # Make Button for Node Menu
        self.nodButton = QPushButton('Nodes')
        self.nodButton.setFixedSize(315, 40)
        self.nodButton.setEnabled(False)
        MainLayout.addWidget(self.nodButton, 11, 0, 1, 2)
        self.Node_Menu = []


        # Set Layout for Main Window--------------------------------------------
        self.setLayout(MainLayout)

    #---------Call all of the sub windows--------------------------------------

    def newMissionWindow(self):
        if self.newMissionMenu == []:
            self.newMissionMenu = loadPathWindow.newMissionWindow()
            self.orbButton.setEnabled(True)
            self.spcButton.setEnabled(False)
            self.simButton.setEnabled(False)
            self.grhButton.setEnabled(False)
            self.fovButton.setEnabled(False)
            self.regButton.setEnabled(False)
            self.tdrButton.setEnabled(False)
            self.ipcButton.setEnabled(False)
            self.nosButton.setEnabled(False)
            self.dsmButton.setEnabled(False)
            self.nodButton.setEnabled(False)

        self.newMissionMenu.show()

    def loadMissionWindow(self):
        if self.loadMissionMenu == []:
            self.loadMissionMenu = loadPathWindow.loadMissionWindow()
            self.orbButton.setEnabled(True)
            self.spcButton.setEnabled(True)
            self.simButton.setEnabled(True)
            self.grhButton.setEnabled(True)
            self.fovButton.setEnabled(True)
            self.regButton.setEnabled(True)
            self.tdrButton.setEnabled(True)
            self.ipcButton.setEnabled(True)
            self.nosButton.setEnabled(True)
            self.dsmButton.setEnabled(True)
            self.nodButton.setEnabled(True)
        self.loadMissionMenu.show()

    def orbWindow(self):
        if self.orbMenu == []:
            self.spcButton.setEnabled(True)
            self.orbMenu = ORB.OrbitWindow()
        self.orbMenu.show()

    def spcWindow(self):
        if self.spcMenu == []:
            self.simButton.setEnabled(True)
            self.spcMenu = SC.SpacecraftWindow()
        self.spcMenu.show()

    def simWindow(self):
        if self.simMenu == []:
            self.grhButton.setEnabled(True)
            self.fovButton.setEnabled(True)
            self.regButton.setEnabled(True)
            self.tdrButton.setEnabled(True)
            self.ipcButton.setEnabled(True)
            self.nosButton.setEnabled(True)
            self.dsmButton.setEnabled(True)
            self.nodButton.setEnabled(True)
            self.simMenu = SIM.SimWindow()
        self.simMenu.show()

    def grhWindow(self):
        if self.grhMenu == []:
            self.grhMenu = Graphics.GraphicsWindow()
        self.grhMenu.show()

class StartUpWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('Deep Thought Terminal')

    def start_main_menu(self):
        self.mainMenuWindow = MainWindow()
        self.mainMenuWindow.show()
        self.hide()

# Launch GUI -------------------------------------------------------------------
def main():
    app = QApplication(sys.argv) # Create an instance of QApplication

    mainMenuWindow = MainWindow()
    mainMenuWindow.show()


    sys.exit(app.exec())

if __name__ == '__main__':
    main()
