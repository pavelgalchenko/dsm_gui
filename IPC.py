# This file contains the code for the IPC window in the 42 gui

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

class IPC_Window(QWidget):
    def __init__(IPC_Window):
        super().__init__()
        IPC_Window.setWindowTitle('IPC Parameters') # sets the window’s title
        IPC_Window.setFixedSize(500, 500) # gives window a fixed size
        IPC_Layout = QGridLayout()
        IPC_Window.Label = QLabel("List of IPC Input Parameters")
        IPC_Layout.addWidget(IPC_Window.Label)
        IPC_Window.setLayout(IPC_Layout)
