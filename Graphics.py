# This file contains the code for the Graphics window in the 42 gui

# Import Packages---------------------------------------------------------------
import sys
import numpy as numpy
# Import QApplication and the required widgets from PyQt5.QtWidgets
from PyQt5.QtWidgets import QApplication,QTabWidget, QMainWindow, QWidget, QLabel, QGridLayout, QLineEdit, QPushButton, QComboBox, QVBoxLayout, QHBoxLayout, QRadioButton
from PyQt5.QtCore import Qt, pyqtSlot

class GraphicsWindow(QMainWindow):
    def __init__(GraphicsWindow):
        super().__init__()
        GraphicsWindow.setWindowTitle('42 Graphics Configuration') # sets the window’s title
        GraphicsWindow.setFixedSize(500, 500) # gives window a fixed size
        GraphicsLayout = QGridLayout()

        GraphicsWindow.table_widget = GraphicsTableWidget(GraphicsWindow)
        GraphicsWindow.setCentralWidget(GraphicsWindow.table_widget)

class GraphicsTableWidget(QWidget):
    def __init__(GraphicsTableWidget, parent):
        super(QWidget, GraphicsTableWidget).__init__(parent)
        GraphicsTableWidget.layout = QVBoxLayout(GraphicsTableWidget)

        Label = [0]*17;

        # initialize tab screen
        GraphicsTableWidget.tabs = QTabWidget()
        GraphicsTableWidget.tab1 = QWidget()
        GraphicsTableWidget.tab2 = QWidget()
        GraphicsTableWidget.tab3 = QWidget()
        GraphicsTableWidget.tab4 = QWidget()
        GraphicsTableWidget.tab5 = QWidget()

        # Add tabs
        GraphicsTableWidget.tabs.addTab(GraphicsTableWidget.tab1,"General")
        GraphicsTableWidget.tabs.addTab(GraphicsTableWidget.tab2,"POV")
        GraphicsTableWidget.tabs.addTab(GraphicsTableWidget.tab3,"CAM")
        GraphicsTableWidget.tabs.addTab(GraphicsTableWidget.tab4,"MAP")
        GraphicsTableWidget.tabs.addTab(GraphicsTableWidget.tab5,"Unit Sphere")

        # General tab ----------------------------------------------------------
        tab1_layout = GraphicsTableWidget.tab1.layout;
        tab1_layout = QGridLayout(GraphicsTableWidget)

        Label[0] = QLabel("GL Output Intervel (sec):")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        tab1_layout.addWidget(Label[0],0,0)
        GraphicsTableWidget.GL_out_interval = QLineEdit()
        tab1_layout.addWidget(GraphicsTableWidget.GL_out_interval,0,1)

        Label[1] = QLabel("Star Catalog File Name:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        tab1_layout.addWidget(Label[1],1,0)
        GraphicsTableWidget.star_catalog_name = QLineEdit()
        tab1_layout.addWidget(GraphicsTableWidget.star_catalog_name ,1,1)

        Label[2] = QLabel("Map Window:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        tab1_layout.addWidget(Label[2],2,0)
        GraphicsTableWidget.MapWindowOn = QRadioButton('On')
        GraphicsTableWidget.MapWindowOff = QRadioButton('Off')
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(GraphicsTableWidget.MapWindowOn)
        SubLayout.addWidget(GraphicsTableWidget.MapWindowOff)
        tab1_layout.addLayout(SubLayout,2,1)

        Label[3] = QLabel("Orrery Window:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        tab1_layout.addWidget(Label[3],3,0)
        GraphicsTableWidget.OrreryWindowOn = QRadioButton('On')
        GraphicsTableWidget.OrreryWindowOff = QRadioButton('Off')
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(GraphicsTableWidget.OrreryWindowOn)
        SubLayout.addWidget(GraphicsTableWidget.OrreryWindowOff)
        tab1_layout.addLayout(SubLayout,3,1)

        Label[4] = QLabel("Unit Sphere Window:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        tab1_layout.addWidget(Label[4],4,0)
        GraphicsTableWidget.UnitSphereWindowOn = QRadioButton('On')
        GraphicsTableWidget.UnitSphereWindowOff = QRadioButton('Off')
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(GraphicsTableWidget.UnitSphereWindowOn)
        SubLayout.addWidget(GraphicsTableWidget.UnitSphereWindowOff)
        tab1_layout.addLayout(SubLayout,4,1)

        GraphicsTableWidget.tab1.setLayout(tab1_layout)

        # POV tab --------------------------------------------------------------
        tab2_layout = GraphicsTableWidget.tab2.layout;
        tab2_layout = QGridLayout(GraphicsTableWidget)

        Label[5] = QLabel("Pause at Startup:")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        tab2_layout.addWidget(Label[5],0,0)
        GraphicsTableWidget.PauseStartupOn = QRadioButton('On')
        GraphicsTableWidget.PauseStartupOff = QRadioButton('Off')
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(GraphicsTableWidget.PauseStartupOn)
        SubLayout.addWidget(GraphicsTableWidget.PauseStartupOff)
        tab2_layout.addLayout(SubLayout,0,1)

        Label[6] = QLabel("POV Mode:")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        tab2_layout.addWidget(Label[6],1,0)
        GraphicsTableWidget.ModePOV = QComboBox()
        GraphicsTableWidget.ModePOV.addItems(["TRACK_HOST", "TRACK_TARGET", "FIXED_IN_HOST"])
        tab2_layout.addWidget(GraphicsTableWidget.ModePOV,1,1)

        Label[7] = QLabel("Host Type:")
        Label[7].setAlignment(Qt.AlignLeft)
        Label[7].setStyleSheet('font-size: 15px')
        tab2_layout.addWidget(Label[7],2,0)
        GraphicsTableWidget.HostType = QComboBox()
        GraphicsTableWidget.HostType.addItems(["WORLD", "REFORB", "FRM", "SC", "BODY"])
        tab2_layout.addWidget(GraphicsTableWidget.HostType,2,1)

        # Possible addition: connect #SC and #body to initial host/Target
        # QComboBox for POV Frame of initial host/target
        Label[8] = QLabel("Initial Host (SC, Body, POV Frame):")
        Label[8].setAlignment(Qt.AlignLeft)
        Label[8].setStyleSheet('font-size: 15px')
        tab2_layout.addWidget(Label[8],3,0)
        GraphicsTableWidget.InitialHostSC = QLineEdit()
        GraphicsTableWidget.InitialHostBody = QLineEdit()
        GraphicsTableWidget.InitialHostPOVFrame = QLineEdit()
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(GraphicsTableWidget.InitialHostSC)
        SubLayout.addWidget(GraphicsTableWidget.InitialHostBody)
        SubLayout.addWidget(GraphicsTableWidget.InitialHostPOVFrame)
        tab2_layout.addLayout(SubLayout,3,1)

        Label[9] = QLabel("Host Type:")
        Label[9].setAlignment(Qt.AlignLeft)
        Label[9].setStyleSheet('font-size: 15px')
        tab2_layout.addWidget(Label[9],4,0)
        GraphicsTableWidget.TargetType = QComboBox()
        GraphicsTableWidget.TargetType.addItems(["WORLD", "REFORB", "FRM", "SC", "BODY"])
        tab2_layout.addWidget(GraphicsTableWidget.TargetType,4,1)

        Label[9] = QLabel("Initial Target (SC, Body, POV Frame):")
        Label[9].setAlignment(Qt.AlignLeft)
        Label[9].setStyleSheet('font-size: 15px')
        tab2_layout.addWidget(Label[9],5,0)
        GraphicsTableWidget.InitialTargetSC = QLineEdit()
        GraphicsTableWidget.InitialTargetBody = QLineEdit()
        GraphicsTableWidget.InitialTargetPOVFrame = QLineEdit()
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(GraphicsTableWidget.InitialTargetSC)
        SubLayout.addWidget(GraphicsTableWidget.InitialTargetBody)
        SubLayout.addWidget(GraphicsTableWidget.InitialTargetPOVFrame)
        tab2_layout.addLayout(SubLayout,5,1)

        Label[10] = QLabel("Boresight Axis:")
        Label[10].setAlignment(Qt.AlignLeft)
        Label[10].setStyleSheet('font-size: 15px')
        tab2_layout.addWidget(Label[10],6,0)
        GraphicsTableWidget.BoresightAxis = QLineEdit()
        tab2_layout.addWidget(GraphicsTableWidget.BoresightAxis,6,1)

        GraphicsTableWidget.tab2.setLayout(tab2_layout)


        # CAM tab --------------------------------------------------------------
        GraphicsTableWidget.tab3.layout = QVBoxLayout(GraphicsTableWidget)

        # MAP tab --------------------------------------------------------------
        GraphicsTableWidget.tab4.layout = QVBoxLayout(GraphicsTableWidget)

        # Unit Sphere tab ------------------------------------------------------
        GraphicsTableWidget.tab4.layout = QVBoxLayout(GraphicsTableWidget)

        # ADD TAB WIDGETS HERE
        # ADD WIDGET
        # SET LAYOUT

        # Add tabs to widgets
        GraphicsTableWidget.layout.addWidget(GraphicsTableWidget.tabs)
        GraphicsTableWidget.setLayout(GraphicsTableWidget.layout)

    # @pyqtSlot()
    # def on_click(self):
    #     print("\n")
    #     for currentQTableWidgetItem in self.tableWidget.selectedItems():
    #         print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())
        # GraphicsWindow.Label = QLabel("List of Graphics Input Parameters")
        # GraphicsLayout.addWidget(GraphicsWindow.Label)
        # GraphicsWindow.setLayout(GraphicsLayout)

        # Load Data from Inp_Sim.txt
        # SimWindow.ReadFile = open('InOut/Inp_Sim_Default.txt', 'r')
        # SimWindow.Inp_Sim_data = SimWindow.ReadFile.readlines()

        # Make Widgets for Simulation Window------------------------------------
        # Label = [0]*10 # Initialize array of labels
