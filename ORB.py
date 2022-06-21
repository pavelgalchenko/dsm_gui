# This file contains the code for the Orbits window in the 42 gui

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

class OrbitWindow(QWidget):
    def __init__(OrbitWindow):
        super().__init__()
        OrbitWindow.setWindowTitle('Orbit Parameters') # sets the window’s title
        OrbitWindow.setFixedSize(500, 500) # gives window a fixed size
        OrbLayout = QGridLayout()
        OrbitWindow.Label = QLabel("List of Orbital Parameter Inputs/Options")
        OrbLayout.addWidget(OrbitWindow.Label)
        OrbitWindow.setLayout(OrbLayout)
