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

# Create Classes for Secondary Windows------------------------------------------
    # Q: Can this be a class of "sub-menus" and create and each menu instance of the class
class OrbitWindow(QWidget):
    def __init__(OrbitWindow):
        super().__init__()
        OrbitWindow.setWindowTitle('Orbit Parameters') # sets the window’s title
        OrbitWindow.setFixedSize(500, 500) # gives window a fixed size
        OrbLayout = QGridLayout()
        OrbitWindow.Label = QLabel("List of Orbital Parameter Inputs/Options")
        OrbLayout.addWidget(OrbitWindow.Label)
        OrbitWindow.setLayout(OrbLayout)

class CmdWindow(QWidget):
    def __init__(CmdWindow):
        super().__init__()
        CmdWindow.setWindowTitle('Orbit Parameters') # sets the window’s title
        CmdWindow.setFixedSize(500, 500) # gives window a fixed size
        CmdLayout = QGridLayout()
        CmdWindow.Label = QLabel("List of Command Inputs")
        CmdLayout.addWidget(CmdWindow.Label)
        CmdWindow.setLayout(CmdLayout)

class SC_Window(QWidget):
    def __init__(SC_Window):
        super().__init__()
        SC_Window.setWindowTitle('Spacecraft Parameters') # sets the window’s title
        SC_Window.setFixedSize(500, 500) # gives window a fixed size
        SC_Layout = QGridLayout()
        SC_Window.Label = QLabel("List of Spacecraft Input Parameters")
        SC_Layout.addWidget(SC_Window.Label)
        SC_Window.setLayout(SC_Layout)

class GraphicsWindow(QWidget):
    def __init__(GraphicsWindow):
        super().__init__()
        GraphicsWindow.setWindowTitle('Graphics Settings') # sets the window’s title
        GraphicsWindow.setFixedSize(500, 500) # gives window a fixed size
        GraphicsLayout = QGridLayout()
        GraphicsWindow.Label = QLabel("List of Graphics Input Parameters")
        GraphicsLayout.addWidget(GraphicsWindow.Label)
        GraphicsWindow.setLayout(GraphicsLayout)

class SimWindow(QWidget):
    def __init__(SimWindow):
        # Set Up Window
        super().__init__()
        SimWindow.setWindowTitle('Simulation Parameters') # sets the window’s title
        SimWindow.setFixedSize(500, 500) # gives window a fixed size
        SimLayout = QGridLayout()

        # Load Data from Inp_Sim.txt
        ReadFile = open('InOut/Inp_Sim.txt', 'r')
        Inp_Sim_data = ReadFile.readlines()
        Label = [0]*10 # Initialize

        # Make Widgets for Simulation Window------------------------------------
        # Heading Label---------------------------------------------------------
        Label[0] = QLabel("Simulation Control")
        Label[0].setAlignment(Qt.AlignCenter)
        Label[0].setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label[0], 0, 0, 1, 2)

        # Time Mode-------------------------------------------------------------
        Label[1] = QLabel("Time Mode:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[1], 1, 0)

        TimeMode = QComboBox()
        TimeMode.addItems(["FAST", "REAL", "EXTERNAL", "NOS3"])
        SimLayout.addWidget(TimeMode, 1, 1)

        # Sim Duration, Step size-----------------------------------------------
        Label[2] = QLabel("Sim Duration (sec):")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[2], 2, 0)

        SimDuration = QLineEdit('30000.0')
        SimLayout.addWidget(SimDuration, 2, 1)

        Label[3] = QLabel("Step Size (sec):")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[3], 3, 0)

        StepSize = QLineEdit('0.1')
        SimLayout.addWidget(StepSize, 3, 1)

        # Front end Graphics----------------------------------------------------
        Label[4] = QLabel("Front End Graphics:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label[4], 4, 0)

        SimWindow.GraphicsOn = QRadioButton('On')
        SimWindow.GraphicsOn.setChecked(False)
        SimWindow.GraphicsOff = QRadioButton('Off')
        SimWindow.GraphicsOff.setChecked(True)

        #if GraphicsOn.isChecked():
        #    Inp_Sim_data[5] = "1234                            !  Graphics Front End? \n"
        #elif GraphicsOff.isChecked():
        #    Inp_Sim_data[5] = "FALSE                           !  Graphics Front End?"



        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SimWindow.GraphicsOn)
        SubLayout.addWidget(SimWindow.GraphicsOff)

        SimLayout.addLayout(SubLayout,4,1)

        SimWindow.GraphicsOn.toggled.connect(lambda:SimWindow.GraphicsOn.GraphicsOnSlot)
        SimWindow.GraphicsOff.toggled.connect(lambda:SimWindow.GraphicsOff.GraphicsOffSlot)

    # Slot Functions
    def GraphicsOnSlot(SimWindow):
        if SimWindow.GraphicsOn.clicked():
            Inp_Sim_data[5] = "TRUE                            !  Graphics Front End? \n"
    def GraphicsOffSlot(SimWindow):
        if SimWindow.GraphicsOff.clicked():
            Inp_Sim_data[5] = "FALSE                           !  Graphics Front End?"









        #Inp_Sim_data[2] = 'REAL                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)'
        #print(Inp_Sim_data[2])





        # Write inputs to txt Write File----------------------------------------
        WriteFile = open('InOut/TestWrite.txt', 'w')
        WriteFile.writelines(Inp_Sim_data)

        # Finialize Simulation window
        SimWindow.setLayout(SimLayout)



class FOV_Window(QWidget):
    def __init__(FOV_Window):
        super().__init__()
        FOV_Window.setWindowTitle('Simulation Parameters') # sets the window’s title
        FOV_Window.setFixedSize(500, 500) # gives window a fixed size
        FOV_Layout = QGridLayout()
        FOV_Window.Label = QLabel("List of FOV Input Parameters")
        FOV_Layout.addWidget(FOV_Window.Label)
        FOV_Window.setLayout(FOV_Layout)

class NOS3_Window(QWidget):
    def __init__(NOS3_Window):
        super().__init__()
        NOS3_Window.setWindowTitle('NOS3 Parameters') # sets the window’s title
        NOS3_Window.setFixedSize(500, 500) # gives window a fixed size
        NOS3_Layout = QGridLayout()
        NOS3_Window.Label = QLabel("List of NOS3 Input Parameters")
        NOS3_Layout.addWidget(NOS3_Window.Label)
        NOS3_Window.setLayout(NOS3_Layout)

class RegionWindow(QWidget):
    def __init__(RegionWindow):
        super().__init__()
        RegionWindow.setWindowTitle('Region Parameters') # sets the window’s title
        RegionWindow.setFixedSize(500, 500) # gives window a fixed size
        RegionLayout = QGridLayout()
        RegionWindow.Label = QLabel("List of Region Input Parameters")
        RegionLayout.addWidget(RegionWindow.Label)
        RegionWindow.setLayout(RegionLayout)

class IPC_Window(QWidget):
    def __init__(IPC_Window):
        super().__init__()
        IPC_Window.setWindowTitle('IPC Parameters') # sets the window’s title
        IPC_Window.setFixedSize(500, 500) # gives window a fixed size
        IPC_Layout = QGridLayout()
        IPC_Window.Label = QLabel("List of IPC Input Parameters")
        IPC_Layout.addWidget(IPC_Window.Label)
        IPC_Window.setLayout(IPC_Layout)

class TDRS_Window(QWidget):
    def __init__(TDRS_Window):
        super().__init__()
        TDRS_Window.setWindowTitle('TDRS Parameters') # sets the window’s title
        TDRS_Window.setFixedSize(500, 500) # gives window a fixed size
        TDRS_Layout = QGridLayout()
        TDRS_Window.Label = QLabel("List of TDRS Input Parameters")
        TDRS_Layout.addWidget(TDRS_Window.Label)
        TDRS_Window.setLayout(TDRS_Layout)

# Create Main window Class------------------------------------------------------
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

    #---------not sure what this is called but do that stuff--------------------
    def OrbSubWindow(MainWindow):
        if MainWindow.OrbitMenu == []:
            MainWindow.OrbitMenu = OrbitWindow()
        MainWindow.OrbitMenu.show()

    def CmdSubWindow(MainWindow):
        if MainWindow.CmdMenu == []:
            MainWindow.CmdMenu = CmdWindow()
        MainWindow.CmdMenu.show()

    def SCSubWindow(MainWindow):
        if MainWindow.SCMenu == []:
            MainWindow.SCMenu = SC_Window()
        MainWindow.SCMenu.show()

    def GraphicsSubWindow(MainWindow):
        if MainWindow.GraphicsMenu == []:
            MainWindow.GraphicsMenu = GraphicsWindow()
        MainWindow.GraphicsMenu.show()

    def SimSubWindow(MainWindow):
        if MainWindow.SimulationMenu == []:
            MainWindow.SimulationMenu = SimWindow()
        MainWindow.SimulationMenu.show()

    def FOV_SubWindow(MainWindow):
        if MainWindow.FOV_Menu == []:
            MainWindow.FOV_Menu = FOV_Window()
        MainWindow.FOV_Menu.show()

    def NOS3_SubWindow(MainWindow):
        if MainWindow.NOS3_Menu == []:
            MainWindow.NOS3_Menu = NOS3_Window()
        MainWindow.NOS3_Menu.show()

    def RegionSubWindow(MainWindow):
        if MainWindow.RegionMenu == []:
            MainWindow.RegionMenu = RegionWindow()
        MainWindow.RegionMenu.show()

    def IPC_SubWindow(MainWindow):
        if MainWindow.IPC_Menu == []:
            MainWindow.IPC_Menu = IPC_Window()
        MainWindow.IPC_Menu.show()

    def TDRS_SubWindow(MainWindow):
        if MainWindow.TDRS_Menu == []:
            MainWindow.TDRS_Menu = TDRS_Window()
        MainWindow.TDRS_Menu.show()


# Launch GUI -------------------------------------------------------------------
def main():
    app = QApplication(sys.argv) # Create an instance of QApplication
    Window = MainWindow()
    Window.show()
    sys.exit(app.exec_()) # Execute the calculator's main loop

if __name__ == '__main__':
    main()
