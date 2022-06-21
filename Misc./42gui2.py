# Matthew Zaffram

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
from PyQt5.QtWidgets import QVBoxLayout # for the calculator’s general layout


# Create a subclass of QMainWindow to setup the calculator's GUI
class 42_GUI(QMainWindow): # creates the GUI with the class 42_GUI
    """42 GUI."""
    def __init__(self):
        """View initializer."""
        super().__init__()

        # Set some main window's properties
        self.setWindowTitle('42 GUI') # sets the window’s title
        self.setFixedSize(235, 235) # gives window a fixed size

        # Set the central widget and the general layout
        self.generalLayout = QFormLayout()
        self._centralWidget = QWidget(self)
        self.setCentralWidget(self._centralWidget)
        self._centralWidget.setLayout(self.generalLayout)

        # Create the display and the buttons
        self._createDisplay()
        self._createButtons()

    def _createDisplay(self):
        """Create the display."""
        # Create the display widget
        self.display = QLineEdit()
        # Set some display's properties
        self.display.setFixedHeight(35)
        self.display.setAlignment(Qt.AlignRight)
        self.display.setReadOnly(True)
        # Add the display to the general layout
        self.generalLayout.addWidget(self.display)

    def _createButtons(self):
            """Create the buttons."""
            self.buttons = {}
            buttonsLayout = QGridLayout()
            # Button text | position on the QGridLayout
            buttons = {'Orbits': (0, 0),
                       'Input SC': (0, 1),
                       'Input Command': (1, 0),
                       'Input Graphics': (1, 1),
                       'Input Simulation': (2, 0),
                       'Input FOV': (2, 1),
                       'Input NOS3': (3, 0),
                       'Input Region': (3, 1),
                       'Input IPC': (4, 0),
                       'Input TDRS': (4, 1),
                      }
            # Create the buttons and add them to the grid layout
            for btnText, pos in buttons.items():
                self.buttons[btnText] = QPushButton(btnText)
                self.buttons[btnText].setFixedSize(40, 40)
                buttonsLayout.addWidget(self.buttons[btnText], pos[0], pos[1])

            # Add buttonsLayout to the general layout
            self.generalLayout.addLayout(buttonsLayout)


# Client code
def main(): # defines calculator’s main function
    """Main function."""
    # Create an instance of QApplication
    GUI = QApplication(sys.argv) # creates a QApplication object pycalc.

    # Show the calculator's GUI
    view = 42_GUI()
    view.show() # shows the GUI with view.show()

    # Execute the calculator's main loop
    sys.exit(GUI.exec_()) # runs the application’s event loop with pycalc.exec_().

if __name__ == '__main__':
    main()
