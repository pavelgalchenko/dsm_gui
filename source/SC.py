# This file contains the code for the Spacecraft window in the 42 gui

# Import Packages---------------------------------------------------------------
import functions
import re
import os
import shutil

# Import QApplication and the required widgets from PySide6
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QWidget
from PySide6.QtWidgets import QLabel
from PySide6.QtWidgets import QGridLayout # to arrange the buttons
from PySide6.QtWidgets import QLineEdit # for the display
from PySide6.QtWidgets import QPushButton # for the butons
from PySide6.QtWidgets import QComboBox # drop down box
from PySide6.QtWidgets import QHBoxLayout # Horizontal stack layout
from PySide6.QtWidgets import QRadioButton
from PySide6.QtWidgets import QListWidget
from PySide6.QtWidgets import QStackedWidget
from PySide6.QtWidgets import QButtonGroup

class SpacecraftWindow(QWidget):

    def __init__(SC_Window):
        super().__init__()
        SC_Window.setWindowTitle('Spacecraft Parameters') # sets the window’s title
        #SC_Window.setGeometry(0, 0, 500, 500) # Set window, can resize manually (xpos on monitor, ypos on monitor, width pixels, height pixles)
        SC_Layout = QGridLayout()

        Label = [0]*50 # Initialize array of labels

        # SC Description--------------------------------------------------------
        Label[0] = QLabel("Description:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[0], 1, 0)

        SC_Window.ScDescription = QLineEdit('Simple generic S/C')
        SC_Layout.addWidget(SC_Window.ScDescription, 1, 1)

        # SC Label--------------------------------------------------------------
        Label[1] = QLabel("Label:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[1], 2, 0)

        SC_Window.ScDescription = QLineEdit('S/C')
        SC_Layout.addWidget(SC_Window.ScDescription, 2, 1)

        # Sprite File Name------------------------------------------------------
        Label[2] = QLabel("Sprite File Name:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[2], 3, 0)

        SC_Window.SpriteFileName = QLineEdit('GenScSpriteAlpha.ppm')
        SC_Layout.addWidget(SC_Window.SpriteFileName, 3, 1)

        # FSW Identifier--------------------------------------------------------
        Label[3] = QLabel("Flight Software Identifier:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[3], 4, 0)

        SC_Window.FswIdentifier = QLineEdit('PASSIVE_FSW')
        SC_Layout.addWidget(SC_Window.FswIdentifier , 4, 1)

        # FSW Sample Time-------------------------------------------------------
        Label[4] = QLabel("Flight Software Sample Time (sec):")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[4], 5, 0)

        SC_Window.FswSampleTime = QLineEdit('0.1')
        SC_Layout.addWidget(SC_Window.FswSampleTime , 5, 1)

#-------------------------------------------------------------------------------
# Orbit Parameters Section----------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label[5] = QLabel("Orbit Parameters")
        Label[5].setAlignment(Qt.AlignCenter)
        Label[5].setStyleSheet('font-size: 20px')
        SC_Layout.addWidget(Label[5], 6, 0, 1, 3)

        # Orbit Propagator------------------------------------------------------
        Label[6] = QLabel("Orbit Propagator:")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[6], 7, 0)

        SC_Window.OrbProp = QComboBox()
        SC_Window.OrbProp.addItems(["ENCKE", "FIXED", "FIXED", "EULER_HILL", "COWELL"])
        SC_Layout.addWidget(SC_Window.OrbProp, 7, 1)

        # Origin or CM----------------------------------------------------------
        Label[7] = QLabel("Position of Origin / CM wrt F:")
        Label[7].setAlignment(Qt.AlignLeft)
        Label[7].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[7], 8, 0)

        SC_Window.OriginCM = QComboBox()
        SC_Window.OriginCM.addItems(["CM", "ORIGIN"])
        SC_Layout.addWidget(SC_Window.OriginCM, 8, 1)

        # PosF------------------------------------------------------------------
        Label[8] = QLabel("Position in F rel to formation (m):")
        Label[8].setAlignment(Qt.AlignLeft)
        Label[8].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[8], 9, 0)

        SC_Window.XPosRelForm = QLineEdit('0.0')
        SC_Window.YPosRelForm = QLineEdit('0.0')
        SC_Window.ZPosRelForm = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.XPosRelForm)
        SubLayout.addWidget(SC_Window.YPosRelForm)
        SubLayout.addWidget(SC_Window.ZPosRelForm)
        SC_Layout.addLayout(SubLayout, 9, 1, 1, 2)

        # VelF------------------------------------------------------------------
        Label[9] = QLabel("Velocity in F rel to formation (m/s):")
        Label[9].setAlignment(Qt.AlignLeft)
        Label[9].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[9], 10, 0)

        SC_Window.XVelRelForm = QLineEdit('0.0')
        SC_Window.YVelRelForm = QLineEdit('0.0')
        SC_Window.ZVelRelForm = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.XVelRelForm)
        SubLayout.addWidget(SC_Window.YVelRelForm)
        SubLayout.addWidget(SC_Window.ZVelRelForm)
        SC_Layout.addLayout(SubLayout, 10, 1, 1, 2)

#-------------------------------------------------------------------------------
# Initial Attitude Section------------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label[10] = QLabel("Initial Attitude")
        Label[10].setAlignment(Qt.AlignCenter)
        Label[10].setStyleSheet('font-size: 20px')
        SC_Layout.addWidget(Label[10], 11, 0, 1, 3)

        # AngularVelwrt---------------------------------------------------------
        Label[11] = QLabel("Ang Vel wrt [NL], Att [QA] wrt [NLF]:")
        Label[11].setAlignment(Qt.AlignLeft)
        Label[11].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[11], 12, 0)

        SC_Window.Unknown = QLineEdit('NAN')
        SC_Layout.addWidget(SC_Window.Unknown , 12, 1)

        # AngularVel------------------------------------------------------------
        Label[12] = QLabel("Angular Velocity (deg/s):")
        Label[12].setAlignment(Qt.AlignLeft)
        Label[12].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[12], 13, 0)

        SC_Window.xAngVel = QLineEdit('0.3')
        SC_Window.yAngVel = QLineEdit('0.4')
        SC_Window.zAngVel = QLineEdit('0.5')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.xAngVel)
        SubLayout.addWidget(SC_Window.yAngVel)
        SubLayout.addWidget(SC_Window.zAngVel)
        SC_Layout.addLayout(SubLayout, 13, 1, 1, 2)

        # Quaternion------------------------------------------------------------
        Label[13] = QLabel("Quaternion:")
        Label[13].setAlignment(Qt.AlignLeft)
        Label[13].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[13], 14, 0)

        SC_Window.Quaternion0 = QLineEdit('0.0')
        SC_Window.Quaternion1 = QLineEdit('0.0')
        SC_Window.Quaternion2 = QLineEdit('0.0')
        SC_Window.Quaternion3 = QLineEdit('1.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Quaternion0)
        SubLayout.addWidget(SC_Window.Quaternion1)
        SubLayout.addWidget(SC_Window.Quaternion2)
        SubLayout.addWidget(SC_Window.Quaternion3)
        SC_Layout.addLayout(SubLayout, 14, 1, 1, 2)

        # Angles & Euler Sequence-----------------------------------------------
        Label[14] = QLabel("Angles (deg) & Euler Sequence:")
        Label[14].setAlignment(Qt.AlignLeft)
        Label[14].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[14], 15, 0)

        SC_Window.Angles1 = QLineEdit('0.0')
        SC_Window.Angles2 = QLineEdit('0.0')
        SC_Window.Angles3 = QLineEdit('0.0')
        SC_Window.EulerSequence = QLineEdit('213')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Angles1)
        SubLayout.addWidget(SC_Window.Angles2)
        SubLayout.addWidget(SC_Window.Angles3)
        SubLayout.addWidget(SC_Window.EulerSequence)
        SC_Layout.addLayout(SubLayout, 15, 1, 1, 2)
#-------------------------------------------------------------------------------
# Dynamics Flags Section----------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label[15] = QLabel("Dynamics Flags")
        Label[15].setAlignment(Qt.AlignCenter)
        Label[15].setStyleSheet('font-size: 20px')
        SC_Layout.addWidget(Label[15], 16, 0, 1, 3)

        # Dynamics Solver----------------------------------------------------------
        Label[16] = QLabel("Dynamics Solver:")
        Label[16].setAlignment(Qt.AlignLeft)
        Label[16].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[16], 17, 0)

        SC_Window.DynamicsSolver = QComboBox()
        SC_Window.DynamicsSolver.addItems(["GAUSS_ELIM", "ORDER_N"])
        SC_Layout.addWidget(SC_Window.DynamicsSolver, 17, 1)

        # Passive Joint Forces and Torques----------------------------------------------------
        Label[17] = QLabel("Passive Joint Forces & Torques:")
        Label[17].setAlignment(Qt.AlignLeft)
        Label[17].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[17], 18, 0)

        SC_Window.PassJointFrcTrqOn = QRadioButton('On')
        SC_Window.PassJointFrcTrqOn.setChecked(True)
        SC_Window.PassJointFrcTrqOff = QRadioButton('Off')
        SC_Window.PassJointFrcTrqOff.setChecked(False)
        SC_Window.PassiveJointGroup = QButtonGroup()
        SC_Window.PassiveJointGroup.addButton(SC_Window.PassJointFrcTrqOn)
        SC_Window.PassiveJointGroup.addButton(SC_Window.PassJointFrcTrqOff)
        PassiveJointLayout = QHBoxLayout()
        PassiveJointLayout.addWidget(SC_Window.PassJointFrcTrqOn)
        PassiveJointLayout.addWidget(SC_Window.PassJointFrcTrqOff)
        SC_Layout.addLayout(PassiveJointLayout,18,1)

        # Constraint Forces and Torques----------------------------------------------------
        Label[18] = QLabel("Compute Constraint Forces & Torques:")
        Label[18].setAlignment(Qt.AlignLeft)
        Label[18].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[18], 19, 0)

        SC_Window.ConstraintFrcTrqOn = QRadioButton('On')
        SC_Window.ConstraintFrcTrqOn.setChecked(True)
        SC_Window.ConstraintFrcTrqOff = QRadioButton('Off')
        SC_Window.ConstraintFrcTrqOff.setChecked(False)
        SC_Window.ConstraintGroup = QButtonGroup()
        SC_Window.ConstraintGroup.addButton(SC_Window.ConstraintFrcTrqOn)
        SC_Window.ConstraintGroup.addButton(SC_Window.ConstraintFrcTrqOff)
        ConstraintLayout = QHBoxLayout()
        ConstraintLayout.addWidget(SC_Window.ConstraintFrcTrqOn)
        ConstraintLayout.addWidget(SC_Window.ConstraintFrcTrqOff)
        SC_Layout.addLayout(ConstraintLayout,19,1)

        # Mass Prop Ref----------------------------------------------------------
        Label[19] = QLabel("Dynamics Solver:")
        Label[19].setAlignment(Qt.AlignLeft)
        Label[19].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[19], 20, 0)

        SC_Window.PropMassRef = QComboBox()
        SC_Window.PropMassRef.addItems(["REFPT_CM", "REFPT_JOINT"])
        SC_Layout.addWidget(SC_Window.PropMassRef, 20, 1)

        # Flex Active----------------------------------------------------------
        Label[20] = QLabel("Flex Active:")
        Label[20].setAlignment(Qt.AlignLeft)
        Label[20].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[20], 21, 0)

        SC_Window.FlexOn = QRadioButton('On')
        SC_Window.FlexOn.setChecked(True)
        SC_Window.FlexOff = QRadioButton('Off')
        SC_Window.FlexOff.setChecked(False)
        SC_Window.FlexGroup = QButtonGroup()
        SC_Window.FlexGroup.addButton(SC_Window.FlexOn)
        SC_Window.FlexGroup.addButton(SC_Window.FlexOff)
        FlexLayout = QHBoxLayout()
        FlexLayout.addWidget(SC_Window.FlexOn)
        FlexLayout.addWidget(SC_Window.FlexOff)
        SC_Layout.addLayout(FlexLayout,21,1)

        # 2nd Order Flex Terms----------------------------------------------------------
        Label[21] = QLabel("Include 2nd Order Flex Terms:")
        Label[21].setAlignment(Qt.AlignLeft)
        Label[21].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[21], 22, 0)

        SC_Window.Order2FlexOn = QRadioButton('On')
        SC_Window.Order2FlexOn.setChecked(True)
        SC_Window.Order2FlexOff = QRadioButton('Off')
        SC_Window.Order2FlexOff.setChecked(False)
        SC_Window.Order2FlexGroup = QButtonGroup()
        SC_Window.Order2FlexGroup.addButton(SC_Window.Order2FlexOn)
        SC_Window.Order2FlexGroup.addButton(SC_Window.Order2FlexOff)
        FlexLayout = QHBoxLayout()
        FlexLayout.addWidget(SC_Window.Order2FlexOn)
        FlexLayout.addWidget(SC_Window.Order2FlexOff)
        SC_Layout.addLayout(FlexLayout,22,1)

#-------------------------------------------------------------------------------
# Body Parameters Section----------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label[22] = QLabel("Body Parameters")
        Label[22].setAlignment(Qt.AlignCenter)
        Label[22].setStyleSheet('font-size: 20px')
        SC_Layout.addWidget(Label[22], 23, 0, 1, 3)

        # List Label------------------------------------------------------------
        Label[23] = QLabel("Spacecraft Bodies List")
        Label[23].setAlignment(Qt.AlignCenter)
        Label[23].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[23], 24, 0)

        # List of SC Bodies-----------------------------------------------------
        SC_Window.BodyList = QListWidget()
        #SC_Window.BodyList.insertItem(0, "SC Body 1")
        SC_Layout.addWidget(SC_Window.BodyList, 25, 0, 7, 1)
        SC_Window.BodyList.currentRowChanged.connect(SC_Window.DisplayBodyStack)

        # Stacked Widgets-------------------------------------------------------
        SC_Window.BodyStack = QStackedWidget()
        SC_Window.AddBodyStack()
        SC_Layout.addWidget(SC_Window.BodyStack,25,1,8,2)

#-------------------------------------------------------------------------------
# Joint Parameters Section----------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label[24] = QLabel("Joint Parameters")
        Label[24].setAlignment(Qt.AlignCenter)
        Label[24].setStyleSheet('font-size: 20px')
        SC_Layout.addWidget(Label[24], 0, 3, 1, 3)

        # List Label------------------------------------------------------------
        Label[25] = QLabel("Spacecraft Bodies List")
        Label[25].setAlignment(Qt.AlignCenter)
        Label[25].setStyleSheet('font-size: 15px')
        SC_Layout.addWidget(Label[25], 1, 3)

        # List of SC Bodies-----------------------------------------------------
        SC_Window.JointList = QListWidget()
        #SC_Window.JointList.insertItem(0, "SC Body 1")
        SC_Layout.addWidget(SC_Window.JointList, 2, 3, 24, 1)
        SC_Window.JointList.currentRowChanged.connect(SC_Window.DisplayJointStack)

        # Stacked Widgets-------------------------------------------------------
        SC_Window.JointStack = QStackedWidget()
        SC_Window.AddJointStack()
        SC_Layout.addWidget(SC_Window.JointStack, 2, 4, 25, 2)





#-------------------------------------------------------------------------------
# Finialize SC window-----------------------------------------------------------
        SC_Window.setLayout(SC_Layout)

#-------------------------------------------------------------------------------
# Stacks------------------------------------------------------------------------
    def AddBodyStack(SC_Window):
        Label = [0]*7 # Initialize array of labels
        SC_Window.BodyPage = QWidget() # Create widget to become a Page in stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout()

        # Mass------------------------------------------------------------------
        Label[0] = QLabel("Mass (kg):")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[0], 0, 0)

        SC_Window.Mass = QLineEdit('100.0')
        StackLayout.addWidget(SC_Window.Mass , 0, 1)

        # Moment of Inertia-----------------------------------------------------
        Label[1] = QLabel("Moments of Inertia (kg-m^2):")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 1, 0)

        SC_Window.MoIxx = QLineEdit('100.0')
        SC_Window.MoIyy = QLineEdit('200.0')
        SC_Window.MoIzz = QLineEdit('300.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.MoIxx)
        SubLayout.addWidget(SC_Window.MoIyy)
        SubLayout.addWidget(SC_Window.MoIzz)
        StackLayout.addLayout(SubLayout, 1, 1, 1, 2)

        # Products of Inertia-----------------------------------------------------
        Label[2] = QLabel("Products of Inertia (kg-m^2):")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[2], 2, 0)

        SC_Window.MoIxy = QLineEdit('0.0')
        SC_Window.MoIxz = QLineEdit('0.0')
        SC_Window.MoIyz = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.MoIxy)
        SubLayout.addWidget(SC_Window.MoIxz)
        SubLayout.addWidget(SC_Window.MoIyz)
        StackLayout.addLayout(SubLayout, 2, 1, 1, 2)

        # CoM Location----------------------------------------------------------
        Label[3] = QLabel("Location of Mass Center (m):")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[3], 3, 0)

        SC_Window.MoIxy = QLineEdit('0.0')
        SC_Window.MoIxz = QLineEdit('0.0')
        SC_Window.MoIyz = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.MoIxy)
        SubLayout.addWidget(SC_Window.MoIxz)
        SubLayout.addWidget(SC_Window.MoIyz)
        StackLayout.addLayout(SubLayout, 3, 1, 1, 2)

        # Embeded momentum----------------------------------------------------------
        Label[4] = QLabel("Const. Embedded Momentum (Nms):")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[4], 4, 0)

        SC_Window.EmbedMomX = QLineEdit('0.0')
        SC_Window.EmbedMomY = QLineEdit('0.0')
        SC_Window.EmbedMomZ = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.EmbedMomX)
        SubLayout.addWidget(SC_Window.EmbedMomY)
        SubLayout.addWidget(SC_Window.EmbedMomZ)
        StackLayout.addLayout(SubLayout, 4, 1, 1, 2)

        # Geometry input------------------------------------------------------------------
        Label[5] = QLabel("Geometry Input File:")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[5], 5, 0)

        SC_Window.GeometryInpFile = QLineEdit('observer.obj')
        StackLayout.addWidget(SC_Window.GeometryInpFile , 5, 1)

        # Flex File Name--------------------------------------------------------
        Label[6] = QLabel("Flex File Name:")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[6], 6, 0)

        SC_Window.GeometryInpFile = QLineEdit('NONE')
        StackLayout.addWidget(SC_Window.GeometryInpFile , 6, 1)

        # Add Body Button--------------------------------------------------
        AddBodyBtn = QPushButton('Add a Body')
        StackLayout.addWidget(AddBodyBtn, 7, 1)
        AddBodyBtn.clicked.connect(SC_Window.AddBody)

        # Remove Ref Orbit btn
        RmvBodyBtn = QPushButton('Remove a Body')
        StackLayout.addWidget(RmvBodyBtn, 7, 0)
        RmvBodyBtn.clicked.connect(SC_Window.RmvBody)

        SC_Window.BodyPage.setLayout(StackLayout)
        SC_Window.BodyStack.addWidget(SC_Window.BodyPage)

    def DisplayBodyStack(SC_Window, i): # Displayes current page in stack when selected in the RefOrb list
        SC_Window.BodyStack.setCurrentIndex(i)

    def AddBody(SC_Window): # Add a ref orb to the list and the stack
        ItemIndex = SC_Window.BodyList.count() # Index of new item
        ItemLabel = ItemIndex + 1
        SC_Window.BodyList.insertItem(int(ItemIndex), "Body %s"%ItemLabel)
        SC_Window.AddBodyStack()
        SC_Window.BodyList.setCurrentRow(ItemIndex)
        SC_Window.BodyStack.setCurrentIndex(ItemIndex)

    def RmvBody(SC_Window): # Removes the last Ref Orb added to the List
        RmvIndex = SC_Window.BodyList.count() - 1
        if RmvIndex > 0:
            SC_Window.BodyList.takeItem(RmvIndex)
            SC_Window.BodyStack.removeWidget(SC_Window.BodyStack.widget(RmvIndex))
#-------------------------------------------------------------------------------
    def AddJointStack(SC_Window):
        Label = [0]*30 # Initialize array of labels
        SC_Window.JointPage = QWidget() # Create widget to become a Page in stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout()

        # Inner/outer body-----------------------------------------------------
        Label[1] = QLabel("Inner/Outer Body Indices:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 0, 0)

        SC_Window.InnerBodyIndex = QLineEdit('1')
        SC_Window.OuterBodyIndex = QLineEdit('0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.InnerBodyIndex)
        SubLayout.addWidget(SC_Window.OuterBodyIndex)
        StackLayout.addLayout(SubLayout, 0, 1, 1, 2)

        # Rotation--------------------------------------------------------------
        Label[1] = QLabel("Rotational DOF, Sequence:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 1, 0)

        SC_Window.RotateDOF = QLineEdit('1')
        SC_Window.RotateSequence = QLineEdit('213')
        SC_Window.JointType = QComboBox()
        SC_Window.JointType.addItems(["GIMBAL", "SPHERICAL"])

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.RotateDOF)
        SubLayout.addWidget(SC_Window.RotateSequence)
        SubLayout.addWidget(SC_Window.JointType)
        StackLayout.addLayout(SubLayout, 1, 1, 1, 2)

        # Translation--------------------------------------------------------------
        Label[2] = QLabel("Translational DOF, Sequence:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[2], 2, 0)

        SC_Window.TranslateDOF = QLineEdit('0')
        SC_Window.TranslateSequence = QLineEdit('123')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.TranslateDOF)
        SubLayout.addWidget(SC_Window.TranslateSequence)
        StackLayout.addLayout(SubLayout, 2, 1, 1, 2)

        # Lock RotDOF-----------------------------------------------------------
        Label[3] = QLabel("1st Rotational DOF Locked:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[3], 3, 0)

        SC_Window.Rot1DofLockOn = QRadioButton('True')
        SC_Window.Rot1DofLockOn.setChecked(False)
        SC_Window.Rot1DofLockOff = QRadioButton('False')
        SC_Window.Rot1DofLockOff.setChecked(True)
        SC_Window.Rot1DofLockGroup = QButtonGroup()
        SC_Window.Rot1DofLockGroup.addButton(SC_Window.Rot1DofLockOn)
        SC_Window.Rot1DofLockGroup.addButton(SC_Window.Rot1DofLockOff)
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Rot1DofLockOn)
        SubLayout.addWidget(SC_Window.Rot1DofLockOff)
        StackLayout.addLayout(SubLayout,3, 1, 1, 2)

        # Lock RotDOF-----------------------------------------------------------
        Label[4] = QLabel("2nd Rotational DOF Locked:")
        Label[4].setAlignment(Qt.AlignLeft)
        Label[4].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[4], 4, 0)

        SC_Window.Rot2DofLockOn = QRadioButton('True')
        SC_Window.Rot2DofLockOn.setChecked(False)
        SC_Window.Rot2DofLockOff = QRadioButton('False')
        SC_Window.Rot2DofLockOff.setChecked(True)
        SC_Window.Rot2DofLockGroup = QButtonGroup()
        SC_Window.Rot2DofLockGroup.addButton(SC_Window.Rot2DofLockOn)
        SC_Window.Rot2DofLockGroup.addButton(SC_Window.Rot2DofLockOff)
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Rot2DofLockOn)
        SubLayout.addWidget(SC_Window.Rot2DofLockOff)
        StackLayout.addLayout(SubLayout,4, 1, 1, 2)

        # Lock RotDOF-----------------------------------------------------------
        Label[5] = QLabel("3rd Rotational DOF Locked:")
        Label[5].setAlignment(Qt.AlignLeft)
        Label[5].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[5], 5, 0)

        SC_Window.Rot3DofLockOn = QRadioButton('True')
        SC_Window.Rot3DofLockOn.setChecked(False)
        SC_Window.Rot3DofLockOff = QRadioButton('False')
        SC_Window.Rot3DofLockOff.setChecked(True)
        SC_Window.Rot3DofLockGroup = QButtonGroup()
        SC_Window.Rot3DofLockGroup.addButton(SC_Window.Rot3DofLockOn)
        SC_Window.Rot3DofLockGroup.addButton(SC_Window.Rot3DofLockOff)
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Rot3DofLockOn)
        SubLayout.addWidget(SC_Window.Rot3DofLockOff)
        StackLayout.addLayout(SubLayout,5, 1, 1, 2)

        # Lock TranslationDOF--------------------------------------------------------------
        Label[6] = QLabel("Translational DOF Locked:")
        Label[6].setAlignment(Qt.AlignLeft)
        Label[6].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[6], 6, 0)

        SC_Window.Trn1DofLockOn = QRadioButton('True')
        SC_Window.Trn1DofLockOn.setChecked(False)
        SC_Window.Trn1DofLockOff = QRadioButton('False')
        SC_Window.Trn1DofLockOff.setChecked(True)
        SC_Window.Trn1DofLockGroup = QButtonGroup()
        SC_Window.Trn1DofLockGroup.addButton(SC_Window.Trn1DofLockOn)
        SC_Window.Trn1DofLockGroup.addButton(SC_Window.Trn1DofLockOff)
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Trn1DofLockOn)
        SubLayout.addWidget(SC_Window.Trn1DofLockOff)
        StackLayout.addLayout(SubLayout,6, 1, 1, 2)

        # Lock TranslationDOF--------------------------------------------------------------
        Label[7] = QLabel("Translational DOF Locked:")
        Label[7].setAlignment(Qt.AlignLeft)
        Label[7].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[7], 7, 0)

        SC_Window.Trn2DofLockOn = QRadioButton('True')
        SC_Window.Trn2DofLockOn.setChecked(False)
        SC_Window.Trn2DofLockOff = QRadioButton('False')
        SC_Window.Trn2DofLockOff.setChecked(True)
        SC_Window.Trn2DofLockGroup = QButtonGroup()
        SC_Window.Trn2DofLockGroup.addButton(SC_Window.Trn2DofLockOn)
        SC_Window.Trn2DofLockGroup.addButton(SC_Window.Trn2DofLockOff)
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Trn2DofLockOn)
        SubLayout.addWidget(SC_Window.Trn2DofLockOff)
        StackLayout.addLayout(SubLayout,7, 1, 1, 2)

        # Lock TranslationDOF--------------------------------------------------------------
        Label[8] = QLabel("Translational DOF Locked:")
        Label[8].setAlignment(Qt.AlignLeft)
        Label[8].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[8], 8, 0)

        SC_Window.Trn3DofLockOn = QRadioButton('True')
        SC_Window.Trn3DofLockOn.setChecked(False)
        SC_Window.Trn3DofLockOff = QRadioButton('False')
        SC_Window.Trn3DofLockOff.setChecked(True)
        SC_Window.Trn3DofLockGroup = QButtonGroup()
        SC_Window.Trn3DofLockGroup.addButton(SC_Window.Trn3DofLockOn)
        SC_Window.Trn3DofLockGroup.addButton(SC_Window.Trn3DofLockOff)
        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Trn3DofLockOn)
        SubLayout.addWidget(SC_Window.Trn3DofLockOff)
        StackLayout.addLayout(SubLayout,8, 1, 1, 2)

        # Initial Angles----------------------------------------------------------
        Label[9] = QLabel("Initial Angles (deg):")
        Label[9].setAlignment(Qt.AlignLeft)
        Label[9].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[9], 9, 0)

        SC_Window.InitialAng1 = QLineEdit('0.0')
        SC_Window.InitialAng2 = QLineEdit('0.0')
        SC_Window.InitialAng3 = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.InitialAng1)
        SubLayout.addWidget(SC_Window.InitialAng2)
        SubLayout.addWidget(SC_Window.InitialAng3)
        StackLayout.addLayout(SubLayout, 9, 1, 1, 2)

        # Initial Angles----------------------------------------------------------
        Label[10] = QLabel("Initial Angular Rate (deg/s):")
        Label[10].setAlignment(Qt.AlignLeft)
        Label[10].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[10], 10, 0)

        SC_Window.InitialAngRate1 = QLineEdit('0.0')
        SC_Window.InitialAngRate2 = QLineEdit('0.0')
        SC_Window.InitialAngRate3 = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.InitialAngRate1)
        SubLayout.addWidget(SC_Window.InitialAngRate2)
        SubLayout.addWidget(SC_Window.InitialAngRate3)
        StackLayout.addLayout(SubLayout, 10, 1, 1, 2)

        # Initial Disp----------------------------------------------------------
        Label[11] = QLabel("Initial Displacement (m):")
        Label[11].setAlignment(Qt.AlignLeft)
        Label[11].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[11], 11, 0)

        SC_Window.InitialDisp1 = QLineEdit('0.0')
        SC_Window.InitialDisp2 = QLineEdit('0.0')
        SC_Window.InitialDisp3 = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.InitialDisp1)
        SubLayout.addWidget(SC_Window.InitialDisp2)
        SubLayout.addWidget(SC_Window.InitialDisp3)
        StackLayout.addLayout(SubLayout, 11, 1, 1, 2)

        # Initial Disp Rate----------------------------------------------------------
        Label[12] = QLabel("Initial Disp. Rate (m/s):")
        Label[12].setAlignment(Qt.AlignLeft)
        Label[12].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[12], 12, 0)

        SC_Window.InitialDispRate1 = QLineEdit('0.0')
        SC_Window.InitialDispRate2 = QLineEdit('0.0')
        SC_Window.InitialDispRate3 = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.InitialDispRate1)
        SubLayout.addWidget(SC_Window.InitialDispRate2)
        SubLayout.addWidget(SC_Window.InitialDispRate3)
        StackLayout.addLayout(SubLayout, 12, 1, 1, 2)

        # Bi 2 Gi----------------------------------------------------------
        Label[13] = QLabel("Bi to Gi Static Angles (deg) & Seq:")
        Label[13].setAlignment(Qt.AlignLeft)
        Label[13].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[13], 13, 0, 1, 3)

        SC_Window.Bi2GiStatAng1 = QLineEdit('0.0')
        SC_Window.Bi2GiStatAng2 = QLineEdit('0.0')
        SC_Window.Bi2GiStatAng3 = QLineEdit('0.0')
        SC_Window.Bi2GiStatSeq = QLineEdit('312')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Bi2GiStatAng1)
        SubLayout.addWidget(SC_Window.Bi2GiStatAng2)
        SubLayout.addWidget(SC_Window.Bi2GiStatAng3)
        SubLayout.addWidget(SC_Window.Bi2GiStatSeq)
        StackLayout.addLayout(SubLayout, 14, 0, 1, 3)

        # Bo 2 Go----------------------------------------------------------
        Label[14] = QLabel("Bo to Go Static Angles (deg) & Seq:")
        Label[14].setAlignment(Qt.AlignLeft)
        Label[14].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[14], 15, 0, 1, 3)

        SC_Window.Bo2GoStatAng1 = QLineEdit('0.0')
        SC_Window.Bo2GoStatAng2 = QLineEdit('0.0')
        SC_Window.Bo2GoStatAng3 = QLineEdit('0.0')
        SC_Window.Bo2GoStatSeq = QLineEdit('312')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.Bo2GoStatAng1)
        SubLayout.addWidget(SC_Window.Bo2GoStatAng2)
        SubLayout.addWidget(SC_Window.Bo2GoStatAng3)
        SubLayout.addWidget(SC_Window.Bo2GoStatSeq)
        StackLayout.addLayout(SubLayout, 16, 0, 1, 3)

        # Pos ibo----------------------------------------------------------
        Label[15] = QLabel("Position wrt inner body origin (m):")
        Label[15].setAlignment(Qt.AlignLeft)
        Label[15].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[15], 17, 0)

        SC_Window.xPosIBO = QLineEdit('0.0')
        SC_Window.yPosIBO = QLineEdit('0.0')
        SC_Window.zPosIBO = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.xPosIBO)
        SubLayout.addWidget(SC_Window.yPosIBO)
        SubLayout.addWidget(SC_Window.zPosIBO)
        StackLayout.addLayout(SubLayout, 17, 1, 1, 2)

        # Pos obo----------------------------------------------------------
        Label[16] = QLabel("Position wrt outer body origin (m):")
        Label[16].setAlignment(Qt.AlignLeft)
        Label[16].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[16], 18, 0)

        SC_Window.xPosOBO = QLineEdit('0.0')
        SC_Window.yPosOBO = QLineEdit('0.0')
        SC_Window.zPosOBO = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.xPosOBO)
        SubLayout.addWidget(SC_Window.yPosOBO)
        SubLayout.addWidget(SC_Window.zPosOBO)
        StackLayout.addLayout(SubLayout, 18, 1, 1, 2)

        # Rot passive spr coeffs------------------------------------------------
        Label[17] = QLabel("Rot Passive Spring Coefficients (Nm/rad):")
        Label[17].setAlignment(Qt.AlignLeft)
        Label[17].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[17], 19, 0)

        SC_Window.RotPassSprCoeffx = QLineEdit('0.0')
        SC_Window.RotPassSprCoeffy = QLineEdit('0.0')
        SC_Window.RotPassSprCoeffz = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.RotPassSprCoeffx)
        SubLayout.addWidget(SC_Window.RotPassSprCoeffy)
        SubLayout.addWidget(SC_Window.RotPassSprCoeffz)
        StackLayout.addLayout(SubLayout, 19, 1, 1, 2)

        # Rot passive dmp coeffs------------------------------------------------
        Label[18] = QLabel("Rot Passive Spring Coefficients (Nm/rad):")
        Label[18].setAlignment(Qt.AlignLeft)
        Label[18].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[18], 20, 0)

        SC_Window.RotPassDmpCoeffx = QLineEdit('0.0')
        SC_Window.RotPassDmpCoeffy = QLineEdit('0.0')
        SC_Window.RotPassDmpCoeffz = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.RotPassDmpCoeffx)
        SubLayout.addWidget(SC_Window.RotPassDmpCoeffy)
        SubLayout.addWidget(SC_Window.RotPassDmpCoeffz)
        StackLayout.addLayout(SubLayout, 20, 1, 1, 2)

        # Trn passive spr coeffs------------------------------------------------
        Label[19] = QLabel("Trn Passive Spring Coefficients (Nm/m):")
        Label[19].setAlignment(Qt.AlignLeft)
        Label[19].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[19], 21, 0)

        SC_Window.TrnPassSprCoeffx = QLineEdit('0.0')
        SC_Window.TrnPassSprCoeffy = QLineEdit('0.0')
        SC_Window.TrnPassSprCoeffz = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.TrnPassSprCoeffx)
        SubLayout.addWidget(SC_Window.TrnPassSprCoeffy)
        SubLayout.addWidget(SC_Window.TrnPassSprCoeffz)
        StackLayout.addLayout(SubLayout, 21, 1, 1, 2)

        # Trn passive dmp coeffs------------------------------------------------
        Label[20] = QLabel("Trn Passive Spring Coefficients (Nm/m):")
        Label[20].setAlignment(Qt.AlignLeft)
        Label[20].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[20], 22, 0)

        SC_Window.TrnPassDmpCoeffx = QLineEdit('0.0')
        SC_Window.TrnPassDmpCoeffy = QLineEdit('0.0')
        SC_Window.TrnPassDmpCoeffz = QLineEdit('0.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SC_Window.TrnPassDmpCoeffx)
        SubLayout.addWidget(SC_Window.TrnPassDmpCoeffy)
        SubLayout.addWidget(SC_Window.TrnPassDmpCoeffz)
        StackLayout.addLayout(SubLayout, 22, 1, 1, 2)

        # Add Joint Button--------------------------------------------------
        AddJointBtn = QPushButton('Add a Joint')
        StackLayout.addWidget(AddJointBtn, 23, 1)
        AddJointBtn.clicked.connect(SC_Window.AddJoint)

        # Remove Ref Orbit btn
        RmvJointBtn = QPushButton('Remove a Joint')
        StackLayout.addWidget(RmvJointBtn, 23, 0)
        RmvJointBtn.clicked.connect(SC_Window.RmvJoint)

        SC_Window.JointPage.setLayout(StackLayout)
        SC_Window.JointStack.addWidget(SC_Window.JointPage)

    def DisplayJointStack(SC_Window, i): # Displayes current page in stack when selected in the RefOrb list
        SC_Window.JointStack.setCurrentIndex(i)

    def AddJoint(SC_Window): # Add a ref orb to the list and the stack
        ItemIndex = SC_Window.JointList.count() # Index of new item
        ItemLabel = ItemIndex + 1
        SC_Window.JointList.insertItem(int(ItemIndex), "Joint %s"%ItemLabel)
        SC_Window.AddJointStack()
        SC_Window.JointList.setCurrentRow(ItemIndex)
        SC_Window.JointStack.setCurrentIndex(ItemIndex)

    def RmvJoint(SC_Window): # Removes the last Ref Orb added to the List
        RmvIndex = SC_Window.JointList.count() - 1
        if RmvIndex > 0:
            SC_Window.JointList.takeItem(RmvIndex)
            SC_Window.JointStack.removeWidget(SC_Window.JointStack.widget(RmvIndex))
#-------------------------------------------------------------------------------
