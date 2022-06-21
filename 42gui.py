# Matthew Zaffram

# This is a test file to click a button to launch a secondary window

# Import Packages---------------------------------------------------------------
import sys
import numpy as numpy
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

# Import Other files from repo
import FOV, SIM, CMD, SC, NOS3, TDRS, IPC, ORB, Region, Graphics

# Create Main window Class------------------------------------------------------
#-------------------------------------------------------------------------------
class MainWindow(QWidget):
    # Methods
    def __init__(MainWindow):
        super().__init__()
        #---------------------Main Window---------------------------------------
        MainWindow.setWindowTitle('42 GUI') # sets the window’s title
        MainWindow.setFixedSize(700, 400) # gives window a fixed size
        MainLayout = QGridLayout()

        #-----------------------Widgets-----------------------------------------
        # Make Label for array of Buttons
        Label = QLabel("Sub Menus")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        MainLayout.addWidget(Label, 0, 0, 1, 2)

        # Make Button for Orbits Menu
        btn1 = QPushButton('Orbits')
        btn1.clicked.connect(MainWindow.OrbSubWindow)
        btn1.setFixedSize(150, 40)
        MainLayout.addWidget(btn1, 1, 0)
        MainWindow.OrbitMenu = []

        # Make Button for Command Menu
        btn2 = QPushButton('Command')
        btn2.clicked.connect(MainWindow.CmdSubWindow)
        btn2.setFixedSize(150, 40)
        MainLayout.addWidget(btn2, 1, 1)
        MainWindow.CmdMenu = []

        # Make Button for Spacecraft Menu
        btn3 = QPushButton('Spacecraft')
        btn3.clicked.connect(MainWindow.SCSubWindow)
        btn3.setFixedSize(150, 40)
        MainLayout.addWidget(btn3, 2, 0)
        MainWindow.SCMenu = []

        # Make Button for Graphics Menu
        btn4 = QPushButton('Graphics')
        btn4.clicked.connect(MainWindow.GraphicsSubWindow)
        btn4.setFixedSize(150, 40)
        MainLayout.addWidget(btn4, 2, 1)
        MainWindow.GraphicsMenu = []

        # Make Button for Simulation Menu
        btn5 = QPushButton('Simulation')
        btn5.clicked.connect(MainWindow.SimSubWindow)
        btn5.setFixedSize(150, 40)
        MainLayout.addWidget(btn5, 3, 0)
        MainWindow.SimulationMenu = []

        # Make Button for FOV Menu
        btn6 = QPushButton('FOV')
        btn6.clicked.connect(MainWindow.FOV_SubWindow)
        btn6.setFixedSize(150, 40)
        MainLayout.addWidget(btn6, 3, 1)
        MainWindow.FOV_Menu = []

        # Make Button for NOS3 Menu
        btn7 = QPushButton('NOS3')
        btn7.clicked.connect(MainWindow.NOS3_SubWindow)
        btn7.setFixedSize(150, 40)
        MainLayout.addWidget(btn7, 4, 0)
        MainWindow.NOS3_Menu = []

        # Make Button for Region Menu
        btn8 = QPushButton('Region')
        btn8.clicked.connect(MainWindow.RegionSubWindow)
        btn8.setFixedSize(150, 40)
        MainLayout.addWidget(btn8, 4, 1)
        MainWindow.RegionMenu = []

        # Make Button for IPC Menu
        btn9 = QPushButton('IPC')
        btn9.clicked.connect(MainWindow.IPC_SubWindow)
        btn9.setFixedSize(150, 40)
        MainLayout.addWidget(btn9, 5, 0)
        MainWindow.IPC_Menu = []

        # Make Button for TDRS Menu
        btn10 = QPushButton('TDRS')
        btn10.clicked.connect(MainWindow.TDRS_SubWindow)
        btn10.setFixedSize(150, 40)
        MainLayout.addWidget(btn10, 5, 1)
        MainWindow.TDRS_Menu = []

        # Labels
        Label2 = QLabel("Number of Satellites:")
        Label2.setAlignment(Qt.AlignLeft)
        Label2.setStyleSheet('font-size: 15px')
        MainLayout.addWidget(Label2, 1, 2)

        Label3 = QLabel("Central Body:")
        Label3.setAlignment(Qt.AlignLeft)
        Label3.setStyleSheet('font-size: 15px')
        MainLayout.addWidget(Label3, 2, 2)

        Label4 = QLabel("Type of Simulation:")
        Label4.setAlignment(Qt.AlignLeft)
        Label4.setStyleSheet('font-size: 15px')
        MainLayout.addWidget(Label4, 3, 2)

        # Make Edit Line
        NumSats = QLineEdit("Insert Num. Spacecraft")
        # NumSats.returnPressed.connect(MainWindow.PopUp) # Not sure what to do with this yet - maybe color change?

        # Write to Txt file
        file = open('InOut/Test.txt', 'w')
        file.write(NumSats.text())
        file.write('\n')
        file.writelines('\n'.join([NumSats.text(), NumSats.text()]))
        MainLayout.addWidget(NumSats, 1, 3)

        # Make Combo box
        CentralBody = QComboBox()
        CentralBody.addItems(["Choose", "Earth", "Luna", "Sun", "Mars", "Jupiter"])
        MainLayout.addWidget(CentralBody, 2, 3)

        # Make Combo box
        SimMode = QComboBox()
        SimMode.addItems(["Choose", "Keplarian", "3-Body", "Formation Flying", "Constellation"])
        MainLayout.addWidget(SimMode, 3, 3)

        # Apply / Cancel / Reset Default Button
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(MainWindow.close)
        MainLayout.addWidget(ApplyBtn, 6, 3)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(MainWindow.close)
        MainLayout.addWidget(CancelBtn, 6, 2)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(MainWindow.close)
        MainLayout.addWidget(ResetBtn, 6, 1)

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

    def NOS3_SubWindow(MainWindow):
        if MainWindow.NOS3_Menu == []:
            MainWindow.NOS3_Menu = NOS3.NOS3_Window()
        MainWindow.NOS3_Menu.show()

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


# Launch GUI -------------------------------------------------------------------
def main():
    app = QApplication(sys.argv) # Create an instance of QApplication
    Window = MainWindow()
    Window.show()
    sys.exit(app.exec_()) # Execute the calculator's main loop

if __name__ == '__main__':
    main()
