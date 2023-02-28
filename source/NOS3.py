# This file contains the code for the NOS3 window in the 42 gui

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

class NOS3_Window(QWidget):
    def __init__(NOS3_Window):
        super().__init__()
        NOS3_Window.setWindowTitle('NOS3 Parameters') # sets the window’s title
        NOS3_Window.setFixedSize(500, 500) # gives window a fixed size
        NOS3_Layout = QGridLayout()
        NOS3_Window.Label = QLabel("List of NOS3 Input Parameters")
        NOS3_Layout.addWidget(NOS3_Window.Label)
        NOS3_Window.setLayout(NOS3_Layout)
