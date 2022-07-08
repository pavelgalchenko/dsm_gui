#!/usr/bin/env python3

import sys
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

class stackedExample(QWidget):

   def __init__(self):
      super(stackedExample, self).__init__()
      self.leftlist = QListWidget ()
      self.leftlist.insertItem (0, 'Contact' )
      self.leftlist.insertItem (1, 'Personal' )
      self.leftlist.insertItem (2, 'Educational' )

      self.Stack = QStackedWidget (self)
      self.stack1UI()
      self.stack2UI()
      self.stack3UI()

      hbox = QHBoxLayout(self)
      hbox.addWidget(self.leftlist)
      hbox.addWidget(self.Stack)

      self.setLayout(hbox)
      self.leftlist.currentRowChanged.connect(self.display)
      self.setGeometry(300, 50, 10,10)
      self.setWindowTitle('StackedWidget demo')
      self.show()

   def stack1UI(self):
      self.stack1 = QWidget()
      layout = QFormLayout()
      layout.addRow("Name",QLineEdit())
      layout.addRow("Address",QLineEdit())
      #self.setTabText(0,"Contact Details")
      self.stack1.setLayout(layout)
      self.Stack.addWidget (self.stack1)

   def stack2UI(self):
      self.stack2 = QWidget()
      layout = QFormLayout()
      sex = QHBoxLayout()
      sex.addWidget(QRadioButton("Male"))
      sex.addWidget(QRadioButton("Female"))
      layout.addRow(QLabel("Sex"),sex)
      layout.addRow("Date of Birth",QLineEdit())

      self.stack2.setLayout(layout)
      self.Stack.addWidget (self.stack2)

   def stack3UI(self):
      self.stack3 = QWidget()
      layout = QHBoxLayout()
      layout.addWidget(QLabel("subjects"))
      layout.addWidget(QCheckBox("Physics"))
      layout.addWidget(QCheckBox("Maths"))
      self.stack3.setLayout(layout)
      self.Stack.addWidget (self.stack3)

   def display(self,i):
      self.Stack.setCurrentIndex(i)

def main():
   app = QApplication(sys.argv)
   ex = stackedExample()
   sys.exit(app.exec_())

if __name__ == '__main__':
   main()
