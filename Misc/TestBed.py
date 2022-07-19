# Matthew Zaffram

# Import Packages---------------------------------------------------------------
import sys

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

# Initialize Main window--------------------------------------------------------
app = QApplication(sys.argv)
window = QWidget()
window.setWindowTitle('42 GUI') # sets the window’s title
window.setFixedSize(700, 400) # gives window a fixed size
layout = QGridLayout()

# Response Functions -----------------------------------------------------------
def OrbitMenu():
    """Slot function."""
    app2 = QApplication(sys.argv)
    ORBwindow = QWidget()
    ORBwindow.setWindowTitle('42 GUI') # sets the window’s title
    ORBwindow.setFixedSize(700, 400) # gives window a fixed size
    layout = QGridLayout()

def CMDMenu():
    """Slot function."""
    if msg.text():
        msg.setText("")
    else:
        msg.setText("open sub GUI for orbit parameter inputs")

def WriteTxt():
    """Slot function."""
    if len(NumSats.text()) != 0:
        with open('Test.txt', 'w') as file:
            file.write(NumSats.text())


# Widget Features---------------------------------------------------------------

 # Make sub title
subtitle = QLabel('Simulation Input .txt Files')
subtitle.setAlignment(Qt.AlignCenter)
layout.addWidget(subtitle, 0, 0, 1, 3)

# Make Button for Orbits Menu
btn1 = QPushButton('Orbits')
btn1.clicked.connect(OrbitMenu)  # Connect clicked to greeting()
btn1.setFixedSize(150, 40)
layout.addWidget(btn1, 1, 0)

# Make Button for Orbits Menu
btn2 = QPushButton('Input Command')
btn2.clicked.connect(CMDMenu)  # Connect clicked to greeting()
btn2.setFixedSize(150, 40)
layout.addWidget(btn2, 2, 0)

# Create message
msg = QLabel('')
layout.addWidget(msg, 3, 1)


# Make Combo box
SimMode = QComboBox()
SimMode.addItems(["Keplarian", "3-Body", "Formation Flying", "Constellation"])
layout.addWidget(SimMode, 1, 1)

# Make Combo box
CentralBody = QComboBox()
CentralBody.addItems(["Earth", "Luna", "Sun", "Mars", "Jupiter"])
layout.addWidget(CentralBody, 2, 1)

# Make Edit Line
NumSats = QLineEdit()
NumSats.returnPressed.connect(OrbitMenu)
layout.addWidget(NumSats, 1, 3)


window.setLayout(layout)
window.show()
sys.exit(app.exec_())
