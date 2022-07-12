import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore import *

class Widget(QWidget):
    def __init__(self, *args, **kwargs):
        QWidget.__init__(self, *args, **kwargs)
        self.tree = QTreeWidget(self)
        self.stack = QStackedWidget(self)
        lay = QHBoxLayout(self)
        lay.addWidget(self.tree)
        lay.addWidget(self.stack)

        for key, value in TreeList.items():
            root = QTreeWidgetItem(self.tree, [key])
            for val in value:
                item = QTreeWidgetItem([val])
                root.addChild(item)
                widget = QLabel(val,  self)
                ix = self.stack.addWidget(widget)
                item.setData(0, Qt.UserRole, ix)
        self.tree.expandAll()
        self.tree.itemClicked.connect(self.onItemClicked)

    def onItemClicked(self, item, column):
        val = item.data(0, Qt.UserRole)
        if val is not None:
            self.stack.setCurrentIndex(val)

TreeList = ({

    'Header1': ((
        'Item11',
        'Item12',
    )),

    'Header2': ((
        'Item21',
        'Item22'
    ))
})

if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = Widget()
    w.show()
    sys.exit(app.exec_())
