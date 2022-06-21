# This file contains the code for the Spacecraft window in the 42 gui

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

class SC_Window(QWidget):
    def __init__(SC_Window):
        super().__init__()
        SC_Window.setWindowTitle('Spacecraft Parameters') # sets the window’s title
        SC_Window.setFixedSize(500, 500) # gives window a fixed size
        SC_Layout = QGridLayout()
        SC_Window.Label = QLabel("List of Spacecraft Input Parameters")
        SC_Layout.addWidget(SC_Window.Label)
        SC_Window.setLayout(SC_Layout)
