# This file contains the code for the FOV window in the 42 gui

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


class FOV_Window(QWidget):
    def __init__(FOV_Window):
        super().__init__()
        FOV_Window.setWindowTitle('Simulation Parameters') # sets the window’s title
        FOV_Window.setFixedSize(500, 500) # gives window a fixed size
        FOV_Layout = QGridLayout()
        FOV_Window.Label = QLabel("List of FOV Input Parameters")
        FOV_Layout.addWidget(FOV_Window.Label)
        FOV_Window.setLayout(FOV_Layout)
