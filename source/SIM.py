# This file contains the code for the Simulation window in the 42 gui

# Import Packages---------------------------------------------------------------
import functions
import re
import os
import shutil

# Import QApplication and the required widgets from PyQt5.QtWidgets
from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Qt
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

import ORB

class SimWindow(QWidget):
    def __init__(SimWindow):
        # Set Up Window
        super().__init__()
        SimWindow.setWindowTitle('Simulation Parameters') # sets the window’s title
        #SimWindow.setGeometry(0, 0, 1525, 900) # Set window, can resize manually (xpos on monitor, ypos on monitor, width pixels, height pixles)
        SimLayout = QGridLayout()

        # Load Data from Inp_Sim.txt
        SimWindow.missionDir = functions.get_mission_directory(SimWindow)
        SimWindow.inputFile = SimWindow.missionDir + '/InOut/Inp_Sim.txt'
        SimWindow.defaultFile = SimWindow.missionDir + '/InOut/__default__/Inp_Sim.txt'
        SimWindow.ReadFile = open(SimWindow.defaultFile, 'r') # for reading regex
        SimWindow.SimDataOut = [""]*300 # Initialize List
        SimWindow.GetDataIn(SimWindow.inputFile)
        functions.get_sc_orbit_file_names(SimWindow)
        Label = [0]*50 # Initialize array of labels

#-------------------------------------------------------------------------------
# Simulation Control Section----------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label = QLabel("Simulation Control")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label, 0, 0, 1, 3)

        # Time Mode-------------------------------------------------------------
        Label = QLabel("Time Mode:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 1, 0)

        SimWindow.TimeMode = QComboBox()
        SimWindow.TimeMode.addItems(["FAST", "REAL", "EXTERNAL", "NOS3"])
        SimLayout.addWidget(SimWindow.TimeMode, 1, 1)

        # Sim Duration, Step size-----------------------------------------------
        Label = QLabel("Sim Duration [sec]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 2, 0)

        SimWindow.SimDuration = QLineEdit('30000.0')
        SimLayout.addWidget(SimWindow.SimDuration, 2, 1)

        Label = QLabel("Step Size [sec]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 3, 0)

        SimWindow.StepSize = QLineEdit('0.1')
        SimLayout.addWidget(SimWindow.StepSize, 3, 1)

        # File Output Interval--------------------------------------------------
        Label = QLabel("File Output Interval [sec]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 4, 0)
        SimWindow.OutputInterval = QLineEdit("10.0")
        SimLayout.addWidget(SimWindow.OutputInterval, 4, 1)

        # RNG Seed--------------------------------------------------------------
        Label = QLabel("RNG Seed:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 5, 0)
        SimWindow.RNGSeed = QLineEdit("42")
        SimLayout.addWidget(SimWindow.RNGSeed, 5, 1)

        # Front end Graphics----------------------------------------------------
        Label = QLabel("Graphics Front End:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 6, 0)

        SimWindow.GraphicsOn = QRadioButton('On')
        SimWindow.GraphicsOn.setChecked(True)
        SimWindow.GraphicsOff = QRadioButton('Off')
        SimWindow.GraphicsOff.setChecked(False)
        SimWindow.GraphicsBtnGroup = QButtonGroup()
        SimWindow.GraphicsBtnGroup.addButton(SimWindow.GraphicsOn)
        SimWindow.GraphicsBtnGroup.addButton(SimWindow.GraphicsOff)
        GraphicsLayout = QHBoxLayout()
        GraphicsLayout.addWidget(SimWindow.GraphicsOn)
        GraphicsLayout.addWidget(SimWindow.GraphicsOff)
        SimLayout.addLayout(GraphicsLayout,6,1)

        # Cmd File Name---------------------------------------------------------
        Label = QLabel("Command Script File Name:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 7, 0)
        SimWindow.CmdFileName = QLineEdit("Inp_Cmd.txt")
        SimLayout.addWidget(SimWindow.CmdFileName, 7, 1)

#-------------------------------------------------------------------------------
# Reference Orbits Section------------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label = QLabel("Reference Orbits")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label, 8, 0, 1, 3)

        # List Label------------------------------------------------------------
        Label = QLabel("Ref Orb List")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 9, 0)

        # List of Reference Orbits----------------------------------------------
        SimWindow.ListRefOrb = QListWidget()
        SimWindow.ListRefOrb.insertItem(0, "Ref. Orb. 1")
        SimLayout.addWidget(SimWindow.ListRefOrb, 10, 0, 2, 1)
        SimWindow.ListRefOrb.currentRowChanged.connect(SimWindow.DisplayRefOrbStackSlot)

        # Stacked Widgets-------------------------------------------------------
        SimWindow.RefOrbStack = QStackedWidget()
        SimWindow.AddRefOrbStack()
        SimLayout.addWidget(SimWindow.RefOrbStack,9,1,3,2)

#-------------------------------------------------------------------------------
# Spacecraft Section------------------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label = QLabel("Spacecraft")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label, 13, 0, 1, 3)

        # List Label------------------------------------------------------------
        Label = QLabel("Spacecraft List")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 14, 0)

        # List of Spacecraft----------------------------------------------------
        SimWindow.ListSC = QListWidget()
        SimWindow.ListSC.insertItem(0, "Spacecraft 1")
        SimLayout.addWidget(SimWindow.ListSC, 15, 0, 3, 1)
        SimWindow.ListSC.currentRowChanged.connect(SimWindow.DisplaySCstackSlot)

        # Stacked Widgets-------------------------------------------------------
        SimWindow.SCstack = QStackedWidget()
        SimWindow.AddSCstack()
        SimLayout.addWidget(SimWindow.SCstack,14,1,4,2)

#-------------------------------------------------------------------------------
# Ground Stations---------------------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label = QLabel("Ground Stations")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label, 19, 0, 1, 3)

        # List Label------------------------------------------------------------
        Label = QLabel("Ground Station List")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 20, 0)

        # List of Ground Stations-----------------------------------------------
        SimWindow.ListGS = QListWidget()
        SimWindow.ListGS.insertItem(0, "Ground Station 1")
        SimLayout.addWidget(SimWindow.ListGS, 21, 0, 4, 1)
        SimWindow.ListGS.currentRowChanged.connect(SimWindow.DisplayGSstackSlot)

        # Stacked Widgets-------------------------------------------------------
        SimWindow.GSstack = QStackedWidget()
        SimWindow.AddGSstack()
        SimLayout.addWidget(SimWindow.GSstack,20,1,5,2)

#-------------------------------------------------------------------------------
# Environment Section-----------------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label = QLabel("Environment")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label, 0, 3, 1, 6)

        # UTC Date--------------------------------------------------------------
        Label = QLabel("UTC Date [Month, Day, Year]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 1, 3)

        SimWindow.UtcMonth = QLineEdit('10')
        SimWindow.UtcDay = QLineEdit('18')
        SimWindow.UtcYear = QLineEdit('2025')

        SubLayout1 = QHBoxLayout()
        SubLayout1.addWidget(SimWindow.UtcMonth)
        SubLayout1.addWidget(SimWindow.UtcDay)
        SubLayout1.addWidget(SimWindow.UtcYear)
        SimLayout.addLayout(SubLayout1,1, 4, 1, 2)

        # UTC Time--------------------------------------------------------------
        Label = QLabel("UTC Time [hr, min, sec]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 2, 3)

        SimWindow.UtcHour = QLineEdit('08')
        SimWindow.UtcMin = QLineEdit('30')
        SimWindow.UtcSec = QLineEdit('00.00')

        SubLayout2 = QHBoxLayout()
        SubLayout2.addWidget(SimWindow.UtcHour)
        SubLayout2.addWidget(SimWindow.UtcMin)
        SubLayout2.addWidget(SimWindow.UtcSec)
        SimLayout.addLayout(SubLayout2,2, 4, 1, 2)

        # Leap Seconds----------------------------------------------------------
        Label = QLabel("Leap Seconds [sec]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 3, 3)

        SimWindow.LeapSec = QLineEdit('37.0')
        SimLayout.addWidget(SimWindow.LeapSec, 3, 4)

        # F10.7, Ap-------------------------------------------------------------
        Label = QLabel("F10.7, Ap:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 4, 3)

        SimWindow.F10_7_AP = QComboBox()
        SimWindow.F10_7_AP.addItems(["USER", "NOMINAL", "TWOSIGMA"])
        SimLayout.addWidget(SimWindow.F10_7_AP, 4, 4)

        # USER-provided F10.7---------------------------------------------------
        Label = QLabel("USER-provided F10.7:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 5, 3)

        SimWindow.F10_7 = QLineEdit('230.0')
        SimLayout.addWidget(SimWindow.F10_7, 5, 4)

        # USER-provided Ap------------------------------------------------------
        Label = QLabel("USER-provided Ap:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 6, 3)

        SimWindow.Ap = QLineEdit('100.0')
        SimLayout.addWidget(SimWindow.Ap, 6, 4)

        # Magnetic Field Model--------------------------------------------------
        Label = QLabel("Magnetic Field Model:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 7, 3)

        SimWindow.MagField = QComboBox()
        SimWindow.MagField.addItems(["NONE", "DIPOLE", "IGRF"])
        SimLayout.addWidget(SimWindow.MagField, 7, 4)

        # IGRF Degree and Order-------------------------------------------------
        Label = QLabel("IGRF Degree & Order [<=10]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 8, 3)

        SimWindow.IgrfDeg = QLineEdit('8')
        SimWindow.IgrfOrdr = QLineEdit('8')

        SubLayout3 = QHBoxLayout()
        SubLayout3.addWidget(SimWindow.IgrfDeg)
        SubLayout3.addWidget(SimWindow.IgrfOrdr)
        SimLayout.addLayout(SubLayout3,8, 4, 1, 2)

        # Earth Gravity Model---------------------------------------------------
        Label = QLabel("Earth Gravity Model N & M [<=18]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 9, 3)

        SimWindow.EarthGravN = QLineEdit('8')
        SimWindow.EarthGravM = QLineEdit('8')

        SubLayout4 = QHBoxLayout()
        SubLayout4.addWidget(SimWindow.EarthGravN)
        SubLayout4.addWidget(SimWindow.EarthGravM)
        SimLayout.addLayout(SubLayout4,9, 4, 1, 2)

        # Mars Gravity Model----------------------------------------------------
        Label = QLabel("Mars Gravity Model N & M [<=18]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 10, 3)

        SimWindow.MarsGravN = QLineEdit('2')
        SimWindow.MarsGravM = QLineEdit('0')

        SubLayout5 = QHBoxLayout()
        SubLayout5.addWidget(SimWindow.MarsGravN)
        SubLayout5.addWidget(SimWindow.MarsGravM)
        SimLayout.addLayout(SubLayout5,10, 4, 1, 2)

        # Luna Gravity Model----------------------------------------------------
        Label = QLabel("Luna Gravity Model N & M [<=18]:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 11, 3)

        SimWindow.LunaGravN = QLineEdit('2')
        SimWindow.LunaGravM = QLineEdit('0')

        SubLayout6 = QHBoxLayout()
        SubLayout6.addWidget(SimWindow.LunaGravN)
        SubLayout6.addWidget(SimWindow.LunaGravM)
        SimLayout.addLayout(SubLayout6,11, 4, 1, 2)

        # Aerodynamic Forces and Torques-----------------------------------------
        Label = QLabel("Aerodynamic Forces and Torques:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 1, 6)

        SimWindow.AeroFrcTrqOn = QRadioButton('On')
        SimWindow.AeroFrcTrqOn.setChecked(False)
        SimWindow.AeroFrcTrqOff = QRadioButton('Off')
        SimWindow.AeroFrcTrqOff.setChecked(True)
        SimWindow.AeroFrcTrqBtnGroup = QButtonGroup()
        SimWindow.AeroFrcTrqBtnGroup.addButton(SimWindow.AeroFrcTrqOn)
        SimWindow.AeroFrcTrqBtnGroup.addButton(SimWindow.AeroFrcTrqOff)
        SubLayout7 = QHBoxLayout()
        SubLayout7.addWidget(SimWindow.AeroFrcTrqOn)
        SubLayout7.addWidget(SimWindow.AeroFrcTrqOff)
        SimLayout.addLayout(SubLayout7,1,7,1,2)

        # Aerodynamic Forces and Torques (Shadows)------------------------------
        Label = QLabel("Aerodynamic Forces and Torques (Shadows):")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 2, 6)

        SimWindow.AeroFrcTrqShdwOn = QRadioButton('On')
        SimWindow.AeroFrcTrqShdwOn.setChecked(False)
        SimWindow.AeroFrcTrqShdwOff = QRadioButton('Off')
        SimWindow.AeroFrcTrqShdwOff.setChecked(True)
        SimWindow.AeroFrcTrqShdwBtnGroup = QButtonGroup()
        SimWindow.AeroFrcTrqShdwBtnGroup.addButton(SimWindow.AeroFrcTrqShdwOn)
        SimWindow.AeroFrcTrqShdwBtnGroup.addButton(SimWindow.AeroFrcTrqShdwOff)
        SubLayout8 = QHBoxLayout()
        SubLayout8.addWidget(SimWindow.AeroFrcTrqShdwOn)
        SubLayout8.addWidget(SimWindow.AeroFrcTrqShdwOff)
        SimLayout.addLayout(SubLayout8,2,7,1,2)

        # Gravity Gradient Torques----------------------------------------------
        Label = QLabel("Gravity Gradient Torques:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 3, 6)

        SimWindow.GravGradTrqOn = QRadioButton('On')
        SimWindow.GravGradTrqOn.setChecked(False)
        SimWindow.GravGradTrqOff = QRadioButton('Off')
        SimWindow.GravGradTrqOff.setChecked(True)
        SimWindow.GravGradTrqBtnGroup = QButtonGroup()
        SimWindow.GravGradTrqBtnGroup.addButton(SimWindow.GravGradTrqOn)
        SimWindow.GravGradTrqBtnGroup.addButton(SimWindow.GravGradTrqOff)
        SubLayout9 = QHBoxLayout()
        SubLayout9.addWidget(SimWindow.GravGradTrqOn)
        SubLayout9.addWidget(SimWindow.GravGradTrqOff)
        SimLayout.addLayout(SubLayout9,3,7,1,2)

        # Solar Pressure Forces and Torques-----------------------------------------
        Label = QLabel("Solar Pressure Forces & Torques:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 4, 6)

        SimWindow.SrpFrcTrqOn = QRadioButton('On')
        SimWindow.SrpFrcTrqOn.setChecked(False)
        SimWindow.SrpFrcTrqOff = QRadioButton('Off')
        SimWindow.SrpFrcTrqOff.setChecked(True)
        SimWindow.SrpFrcTrqBtnGroup = QButtonGroup()
        SimWindow.SrpFrcTrqBtnGroup.addButton(SimWindow.SrpFrcTrqOn)
        SimWindow.SrpFrcTrqBtnGroup.addButton(SimWindow.SrpFrcTrqOff)
        SubLayout10 = QHBoxLayout()
        SubLayout10.addWidget(SimWindow.SrpFrcTrqOn)
        SubLayout10.addWidget(SimWindow.SrpFrcTrqOff)
        SimLayout.addLayout(SubLayout10,4,7,1,2)

        # Solar Pressure Forces and Torques (shadows)--------------------------------
        Label = QLabel("Solar Pressure Forces & Torques (Shadows):")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 5, 6)

        SimWindow.SrpFrcTrqShdwOn = QRadioButton('On')
        SimWindow.SrpFrcTrqShdwOn.setChecked(False)
        SimWindow.SrpFrcTrqShdwOff = QRadioButton('Off')
        SimWindow.SrpFrcTrqShdwOff.setChecked(True)
        SimWindow.SrpFrcTrqShdwBtnGroup = QButtonGroup()
        SimWindow.SrpFrcTrqShdwBtnGroup.addButton(SimWindow.SrpFrcTrqShdwOn)
        SimWindow.SrpFrcTrqShdwBtnGroup.addButton(SimWindow.SrpFrcTrqShdwOff)
        SubLayout11 = QHBoxLayout()
        SubLayout11.addWidget(SimWindow.SrpFrcTrqShdwOn)
        SubLayout11.addWidget(SimWindow.SrpFrcTrqShdwOff)
        SimLayout.addLayout(SubLayout11,5,7,1,2)

        # Residual Mangetic Moment Torques--------------------------------------
        Label = QLabel("Residual Mangetic Moment Torques:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 6, 6)

        SimWindow.ResMagTrqOn = QRadioButton('On')
        SimWindow.ResMagTrqOn.setChecked(False)
        SimWindow.ResMagTrqOff = QRadioButton('Off')
        SimWindow.ResMagTrqOff.setChecked(True)
        SimWindow.ResMagTrqBtnGroup = QButtonGroup()
        SimWindow.ResMagTrqBtnGroup.addButton(SimWindow.ResMagTrqOn)
        SimWindow.ResMagTrqBtnGroup.addButton(SimWindow.ResMagTrqOff)
        SubLayout12 = QHBoxLayout()
        SubLayout12.addWidget(SimWindow.ResMagTrqOn)
        SubLayout12.addWidget(SimWindow.ResMagTrqOff)
        SimLayout.addLayout(SubLayout12,6,7,1,2)

        # Gravity Perturbation Forces-------------------------------------------
        Label = QLabel("Gravity Perturbation Forces:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 7, 6)

        SimWindow.GravPertFrcOn = QRadioButton('On')
        SimWindow.GravPertFrcOn.setChecked(False)
        SimWindow.GravPertFrcOff = QRadioButton('Off')
        SimWindow.GravPertFrcOff.setChecked(True)
        SimWindow.GravPerturbFrcBtnGroup = QButtonGroup()
        SimWindow.GravPerturbFrcBtnGroup.addButton(SimWindow.GravPertFrcOn)
        SimWindow.GravPerturbFrcBtnGroup.addButton(SimWindow.GravPertFrcOff)
        SubLayout12 = QHBoxLayout()
        SubLayout12.addWidget(SimWindow.GravPertFrcOn)
        SubLayout12.addWidget(SimWindow.GravPertFrcOff)
        SimLayout.addLayout(SubLayout12,7,7,1,2)

        # Thruster Plume Forces & Torques----------------------------------------
        Label = QLabel("Thruster Plume Forces & Torques:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 8, 6)

        SimWindow.ThrustPlumeOn = QRadioButton('On')
        SimWindow.ThrustPlumeOn.setChecked(False)
        SimWindow.ThrustPlumeOff = QRadioButton('Off')
        SimWindow.ThrustPlumeOff.setChecked(True)
        SimWindow.ThrustPlumeBtnGroup = QButtonGroup()
        SimWindow.ThrustPlumeBtnGroup.addButton(SimWindow.ThrustPlumeOn)
        SimWindow.ThrustPlumeBtnGroup.addButton(SimWindow.ThrustPlumeOff)
        SubLayout14 = QHBoxLayout()
        SubLayout14.addWidget(SimWindow.ThrustPlumeOn)
        SubLayout14.addWidget(SimWindow.ThrustPlumeOff)
        SimLayout.addLayout(SubLayout14,8,7,1,2)

        # Contact Forces & Torques----------------------------------------------
        Label = QLabel("Contact Forces & Torques:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 9, 6)

        SimWindow.ContactOn = QRadioButton('On')
        SimWindow.ContactOn.setChecked(False)
        SimWindow.ContactOff = QRadioButton('Off')
        SimWindow.ContactOff.setChecked(True)
        SimWindow.ContactBtnGroup = QButtonGroup()
        SimWindow.ContactBtnGroup.addButton(SimWindow.ContactOn)
        SimWindow.ContactBtnGroup.addButton(SimWindow.ContactOff)
        SubLayout16 = QHBoxLayout()
        SubLayout16.addWidget(SimWindow.ContactOn)
        SubLayout16.addWidget(SimWindow.ContactOff)
        SimLayout.addLayout(SubLayout16,9,7,1,2)

        # CFD Slosh Forces & Torques----------------------------------------------
        Label = QLabel("CFD Slosh Forces & Torques:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 10, 6)

        SimWindow.CfdSloshOn = QRadioButton('On')
        SimWindow.CfdSloshOn.setChecked(False)
        SimWindow.CfdSloshOff = QRadioButton('Off')
        SimWindow.CfdSloshOff.setChecked(True)
        SubLayout17 = QHBoxLayout()
        SimWindow.CFDsloshBtnGroup = QButtonGroup()
        SimWindow.CFDsloshBtnGroup.addButton(SimWindow.CfdSloshOn)
        SimWindow.CFDsloshBtnGroup.addButton(SimWindow.CfdSloshOff)
        SubLayout17.addWidget(SimWindow.CfdSloshOn)
        SubLayout17.addWidget(SimWindow.CfdSloshOff)
        SimLayout.addLayout(SubLayout17,10,7,1,2)

        # Albedo Effect on CSS Measurements-------------------------------------
        Label = QLabel("Albedo Effect on CSS Measurements:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 11, 6)

        SimWindow.CssAlbedoOn = QRadioButton('On')
        SimWindow.CssAlbedoOn.setChecked(False)
        SimWindow.CssAlbedoOff = QRadioButton('Off')
        SimWindow.CssAlbedoOff.setChecked(True)
        SimWindow.CSSalbedoBtnGroup = QButtonGroup()
        SimWindow.CSSalbedoBtnGroup.addButton(SimWindow.CssAlbedoOn)
        SimWindow.CSSalbedoBtnGroup.addButton(SimWindow.CssAlbedoOff)
        SubLayout18 = QHBoxLayout()
        SubLayout18.addWidget(SimWindow.CssAlbedoOn)
        SubLayout18.addWidget(SimWindow.CssAlbedoOff)
        SimLayout.addLayout(SubLayout18,11,7,1,2)

        # Output Environmental Torques to Files---------------------------------
        Label = QLabel("Output Environmental Torques to Files:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 12, 3)

        SimWindow.OutputEnvTrqOn = QRadioButton('Yes')
        SimWindow.OutputEnvTrqOn.setChecked(False)
        SimWindow.OutputEnvTrqOff = QRadioButton('No')
        SimWindow.OutputEnvTrqOff.setChecked(True)
        SimWindow.OutputEnvTrqBtnGroup = QButtonGroup()
        SimWindow.OutputEnvTrqBtnGroup.addButton(SimWindow.OutputEnvTrqOn)
        SimWindow.OutputEnvTrqBtnGroup.addButton(SimWindow.OutputEnvTrqOff)
        SubLayout19 = QHBoxLayout()
        SubLayout19.addWidget(SimWindow.OutputEnvTrqOn)
        SubLayout19.addWidget(SimWindow.OutputEnvTrqOff)
        SimLayout.addLayout(SubLayout19,12,4,1,2)

#-------------------------------------------------------------------------------
# Celestial Bodies of Interest--------------------------------------------------
        # Heading Label---------------------------------------------------------
        Label = QLabel("Celestial Bodies of Interest")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label, 14, 3, 1, 3)

        # Ephemeris Option------------------------------------------------------
        Label = QLabel("Ephemeris Option:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 15, 3)

        SimWindow.Ephemeris = QComboBox()
        SimWindow.Ephemeris.addItems(["MEAN", "DE430", "DE440"])
        SimLayout.addWidget(SimWindow.Ephemeris, 15, 4)

        # Mercury---------------------------------------------------------------
        Label = QLabel("Mercury:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 16, 3)

        SimWindow.MercuryOn = QRadioButton('True')
        SimWindow.MercuryOn.setChecked(True)
        SimWindow.MercuryOff = QRadioButton('False')
        SimWindow.MercuryOff.setChecked(False)
        SimWindow.MercuryBtnGroup = QButtonGroup()
        SimWindow.MercuryBtnGroup.addButton(SimWindow.MercuryOn)
        SimWindow.MercuryBtnGroup.addButton(SimWindow.MercuryOff)
        SubLayout20 = QHBoxLayout()
        SubLayout20.addWidget(SimWindow.MercuryOn)
        SubLayout20.addWidget(SimWindow.MercuryOff)
        SimLayout.addLayout(SubLayout20,16,4,1,2)

        # Venus-----------------------------------------------------------------
        Label = QLabel("Venus:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 17, 3)

        SimWindow.VenusOn = QRadioButton('True')
        SimWindow.VenusOn.setChecked(True)
        SimWindow.VenusOff = QRadioButton('False')
        SimWindow.VenusOff.setChecked(False)
        SimWindow.VenusBtnGroup = QButtonGroup()
        SimWindow.VenusBtnGroup.addButton(SimWindow.VenusOn)
        SimWindow.VenusBtnGroup.addButton(SimWindow.VenusOff)
        SubLayout21 = QHBoxLayout()
        SubLayout21.addWidget(SimWindow.VenusOn)
        SubLayout21.addWidget(SimWindow.VenusOff)
        SimLayout.addLayout(SubLayout21,17,4,1,2)

        # Earth & Luna----------------------------------------------------------
        Label = QLabel("Earth and Luna:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 18, 3)

        SimWindow.EarthLunaOn = QRadioButton('True')
        SimWindow.EarthLunaOn.setChecked(True)
        SimWindow.EarthLunaOff = QRadioButton('False')
        SimWindow.EarthLunaOff.setChecked(False)
        SimWindow.EarthLunaBtnGroup = QButtonGroup()
        SimWindow.EarthLunaBtnGroup.addButton(SimWindow.EarthLunaOn)
        SimWindow.EarthLunaBtnGroup.addButton(SimWindow.EarthLunaOff)
        SubLayout22 = QHBoxLayout()
        SubLayout22.addWidget(SimWindow.EarthLunaOn)
        SubLayout22.addWidget(SimWindow.EarthLunaOff)
        SimLayout.addLayout(SubLayout22,18,4,1,2)

        # Mars & Moons----------------------------------------------------------
        Label = QLabel("Mars and its moons:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 19, 3)

        SimWindow.MarsMoonsOn = QRadioButton('True')
        SimWindow.MarsMoonsOn.setChecked(True)
        SimWindow.MarsMoonsOff = QRadioButton('False')
        SimWindow.MarsMoonsOff.setChecked(False)
        SimWindow.MarsMoonsBtnGroup = QButtonGroup()
        SimWindow.MarsMoonsBtnGroup.addButton(SimWindow.MarsMoonsOn)
        SimWindow.MarsMoonsBtnGroup.addButton(SimWindow.MarsMoonsOff)
        SubLayout23 = QHBoxLayout()
        SubLayout23.addWidget(SimWindow.MarsMoonsOn)
        SubLayout23.addWidget(SimWindow.MarsMoonsOff)
        SimLayout.addLayout(SubLayout23,19,4,1,2)

        # Jupiter & Moons----------------------------------------------------------
        Label = QLabel("Jupiter and its moons:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 20, 3)

        SimWindow.JupiterMoonsOn = QRadioButton('True')
        SimWindow.JupiterMoonsOn.setChecked(True)
        SimWindow.JupiterMoonsOff = QRadioButton('False')
        SimWindow.JupiterMoonsOff.setChecked(False)
        SimWindow.JupiterMoonsBtnGroup = QButtonGroup()
        SimWindow.JupiterMoonsBtnGroup.addButton(SimWindow.JupiterMoonsOn)
        SimWindow.JupiterMoonsBtnGroup.addButton(SimWindow.JupiterMoonsOff)
        SubLayout24 = QHBoxLayout()
        SubLayout24.addWidget(SimWindow.JupiterMoonsOn)
        SubLayout24.addWidget(SimWindow.JupiterMoonsOff)
        SimLayout.addLayout(SubLayout24,20,4,1,2)

        # Saturn & Moons----------------------------------------------------------
        Label = QLabel("Saturn and its moons:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 21, 3)

        SimWindow.SaturnMoonsOn = QRadioButton('True')
        SimWindow.SaturnMoonsOn.setChecked(True)
        SimWindow.SaturnMoonsOff = QRadioButton('False')
        SimWindow.SaturnMoonsOff.setChecked(False)
        SimWindow.SaturnMoonsBtnGroup = QButtonGroup()
        SimWindow.SaturnMoonsBtnGroup.addButton(SimWindow.SaturnMoonsOn)
        SimWindow.SaturnMoonsBtnGroup.addButton(SimWindow.SaturnMoonsOff)
        SubLayout25 = QHBoxLayout()
        SubLayout25.addWidget(SimWindow.SaturnMoonsOn)
        SubLayout25.addWidget(SimWindow.SaturnMoonsOff)
        SimLayout.addLayout(SubLayout25,21,4,1,2)

        # Uranus & Moons----------------------------------------------------------
        Label = QLabel("Uranus and its moons:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 22, 3)

        SimWindow.UranusMoonsOn = QRadioButton('True')
        SimWindow.UranusMoonsOn.setChecked(True)
        SimWindow.UranusMoonsOff = QRadioButton('False')
        SimWindow.UranusMoonsOff.setChecked(False)
        SimWindow.UranusMoonsBtnGroup = QButtonGroup()
        SimWindow.UranusMoonsBtnGroup.addButton(SimWindow.UranusMoonsOn)
        SimWindow.UranusMoonsBtnGroup.addButton(SimWindow.UranusMoonsOff)
        SubLayout26 = QHBoxLayout()
        SubLayout26.addWidget(SimWindow.UranusMoonsOn)
        SubLayout26.addWidget(SimWindow.UranusMoonsOff)
        SimLayout.addLayout(SubLayout26,22,4,1,2)

        # Neptune & Moons----------------------------------------------------------
        Label = QLabel("Neptune and its moons:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 23, 3)

        SimWindow.NeptuneMoonsOn = QRadioButton('True')
        SimWindow.NeptuneMoonsOn.setChecked(True)
        SimWindow.NeptuneMoonsOff = QRadioButton('False')
        SimWindow.NeptuneMoonsOff.setChecked(False)
        SimWindow.NeptuneMoonsBtnGroup = QButtonGroup()
        SimWindow.NeptuneMoonsBtnGroup.addButton(SimWindow.NeptuneMoonsOn)
        SimWindow.NeptuneMoonsBtnGroup.addButton(SimWindow.NeptuneMoonsOff)
        SubLayout27 = QHBoxLayout()
        SubLayout27.addWidget(SimWindow.NeptuneMoonsOn)
        SubLayout27.addWidget(SimWindow.NeptuneMoonsOff)
        SimLayout.addLayout(SubLayout27,23,4,1,2)

        # Pluto & Moons----------------------------------------------------------
        Label = QLabel("Pluto and its moons:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 24, 3)

        SimWindow.PlutoMoonsOn = QRadioButton('True')
        SimWindow.PlutoMoonsOn.setChecked(True)
        SimWindow.PlutoMoonsOff = QRadioButton('False')
        SimWindow.PlutoMoonsOff.setChecked(False)
        SimWindow.PlutoMoonsBtnGroup = QButtonGroup()
        SimWindow.PlutoMoonsBtnGroup.addButton(SimWindow.PlutoMoonsOn)
        SimWindow.PlutoMoonsBtnGroup.addButton(SimWindow.PlutoMoonsOff)
        SubLayout28 = QHBoxLayout()
        SubLayout28.addWidget(SimWindow.PlutoMoonsOn)
        SubLayout28.addWidget(SimWindow.PlutoMoonsOff)
        SimLayout.addLayout(SubLayout28,24,4,1,2)

        # Asteroids & Comets----------------------------------------------------------
        Label = QLabel("Asteroids and Comets:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 25, 3)

        SimWindow.AsteroidsCometsOn = QRadioButton('True')
        SimWindow.AsteroidsCometsOn.setChecked(False)
        SimWindow.AsteroidsCometsOff = QRadioButton('False')
        SimWindow.AsteroidsCometsOff.setChecked(True)
        SimWindow.AsteroidsCometsBtnGroup = QButtonGroup()
        SimWindow.AsteroidsCometsBtnGroup.addButton(SimWindow.AsteroidsCometsOn)
        SimWindow.AsteroidsCometsBtnGroup.addButton(SimWindow.AsteroidsCometsOff)
        SubLayout29 = QHBoxLayout()
        SubLayout29.addWidget(SimWindow.AsteroidsCometsOn)
        SubLayout29.addWidget(SimWindow.AsteroidsCometsOff)
        SimLayout.addLayout(SubLayout29,25,4,1,2)

#-------------------------------------------------------------------------------
# Lagrange Point Systems of Interest--------------------------------------------
        # Heading Label---------------------------------------------------------
        Label = QLabel("Lagrange Point Systems of Interest")
        Label.setAlignment(Qt.AlignCenter)
        Label.setStyleSheet('font-size: 20px')
        SimLayout.addWidget(Label, 14, 6, 1, 3)

        # Earth-Moon System-----------------------------------------------------
        Label = QLabel("Earth-Moon System:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 15, 6)

        SimWindow.EMsystemOn = QRadioButton('True')
        SimWindow.EMsystemOn.setChecked(False)
        SimWindow.EMsystemOff = QRadioButton('False')
        SimWindow.EMsystemOff.setChecked(True)
        SimWindow.EMsystemBtnGroup = QButtonGroup()
        SimWindow.EMsystemBtnGroup.addButton(SimWindow.EMsystemOn)
        SimWindow.EMsystemBtnGroup.addButton(SimWindow.EMsystemOff)
        SubLayout30 = QHBoxLayout()
        SubLayout30.addWidget(SimWindow.EMsystemOn)
        SubLayout30.addWidget(SimWindow.EMsystemOff)
        SimLayout.addLayout(SubLayout30,15,7,1,2)

        # Sun-Earth System-----------------------------------------------------
        Label = QLabel("Sun-Earth System:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 16, 6)

        SimWindow.SEsystemOn = QRadioButton('True')
        SimWindow.SEsystemOn.setChecked(False)
        SimWindow.SEsystemOff = QRadioButton('False')
        SimWindow.SEsystemOff.setChecked(True)
        SimWindow.SEsystemBtnGroup = QButtonGroup()
        SimWindow.SEsystemBtnGroup.addButton(SimWindow.SEsystemOn)
        SimWindow.SEsystemBtnGroup.addButton(SimWindow.SEsystemOff)
        SubLayout31 = QHBoxLayout()
        SubLayout31.addWidget(SimWindow.SEsystemOn)
        SubLayout31.addWidget(SimWindow.SEsystemOff)
        SimLayout.addLayout(SubLayout31,16,7,1,2)

        # Sun-Jupiter System-----------------------------------------------------
        Label = QLabel("Sun-Jupiter System:")
        Label.setAlignment(Qt.AlignLeft)
        Label.setStyleSheet('font-size: 15px')
        SimLayout.addWidget(Label, 17, 6)

        SimWindow.SJsystemOn = QRadioButton('True')
        SimWindow.SJsystemOn.setChecked(False)
        SimWindow.SJsystemOff = QRadioButton('False')
        SimWindow.SJsystemOff.setChecked(True)
        SimWindow.SJsystemBtnGroup = QButtonGroup()
        SimWindow.SJsystemBtnGroup.addButton(SimWindow.SJsystemOn)
        SimWindow.SJsystemBtnGroup.addButton(SimWindow.SJsystemOff)
        SubLayout32 = QHBoxLayout()
        SubLayout32.addWidget(SimWindow.SJsystemOn)
        SubLayout32.addWidget(SimWindow.SJsystemOff)
        SimLayout.addLayout(SubLayout32,17,7,1,2)

#-------------------------------------------------------------------------------
# Apply / Cancel / Reset Default Button-----------------------------------------
        ApplyBtn = QPushButton('Apply')
        ApplyBtn.clicked.connect(SimWindow.WriteDataOut) # First write Lines
        ApplyBtn.clicked.connect(SimWindow.WriteFileOut) # Then write file
        ApplyBtn.clicked.connect(SimWindow.close)
        SimLayout.addWidget(ApplyBtn, 26, 8)

        CancelBtn = QPushButton('Cancel')
        CancelBtn.clicked.connect(SimWindow.close)
        SimLayout.addWidget(CancelBtn, 26, 7)

        ResetBtn = QPushButton('Reset to Default')
        ResetBtn.clicked.connect(SimWindow.SetDefaultGuiFields)
        SimLayout.addWidget(ResetBtn, 26, 6)

#-------------------------------------------------------------------------------
# Finialize Region window-------------------------------------------------------
        SimWindow.setLayout(SimLayout)
        SimWindow.SetGuiFields()
#-------------------------------------------------------------------------------
# Slot Functions----------------------------------------------------------------
    def WriteFileOut(SimWindow): # Write edited input txt data to output txt file
        # Write inputs to txt Write File
        SimWindow.WriteFile = open(SimWindow.inputFile, 'w+')
        SimWindow.WriteFile.writelines(SimWindow.SimDataOut)
        SimWindow.WriteFile.close() # this line optional

    def SetDefaultGuiFields(SimWindow): # Resets gui options and input txt data to default
        # Reset Write File to Default/readfile
        SimWindow.GetDataIn(SimWindow.defaultFile)
        SimWindow.SetGuiFields()

    def WriteDataOut(SimWindow): # Take GUI inputs from widgets and write to Inp_sim.txt
        SimWindow.SimDataOut[0] = "<<<<<<<<<<<<<<<<<<  42: The Mostly Harmless Simulator  >>>>>>>>>>>>>>>>>\n"
        SimWindow.SimDataOut[1] = "************************** Simulation Control **************************\n"

        # TimeModeSlot
        if SimWindow.TimeMode.currentText() == "FAST":
            SimWindow.SimDataOut[2] = "FAST                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "REAL":
            SimWindow.SimDataOut[2] = "REAL                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "EXTERNAL":
            SimWindow.SimDataOut[2] = "EXTERNAL                        !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"
        elif SimWindow.TimeMode.currentText() == "NOS3":
            SimWindow.SimDataOut[2] = "NOS3                            !  Time Mode (FAST, REAL, EXTERNAL, or NOS3)\n"

        # SimDurationSlot
        SimWindow.SimDataOut[3] = SimWindow.SimDuration.text() + "   " + SimWindow.StepSize.text() + "                   !  Sim Duration, Step Size [sec]\n"

        # OutputIntervalSlot
        SimWindow.SimDataOut[4] = SimWindow.OutputInterval.text() + "                            !  File Output Interval [sec]\n"

        # GraphicsSlot
        if SimWindow.GraphicsOn.isChecked():
            SimWindow.SimDataOut[5] = "TRUE                            !  Graphics Front End?\n"
        elif SimWindow.GraphicsOff.isChecked():
            SimWindow.SimDataOut[5] = "FALSE                           !  Graphics Front End?\n"

        # CmdFileNameSlot
        SimWindow.SimDataOut[6] = SimWindow.CmdFileName.text() + "                     !  Command Script File Name\n"

        SimWindow.SimDataOut[7] = "**************************  Reference Orbits  **************************\n"

        # Ref Orb Stack Slot
        SimWindow.SimDataOut[8] = str(SimWindow.ListRefOrb.count()) + "                               !  Number of Reference Orbits\n"
        txt_idx = 9
        for i in range(SimWindow.ListRefOrb.count()):
            if SimWindow.RefOrbStack.widget(i).RefOrbOn.isChecked():
                SimWindow.SimDataOut[txt_idx] = "TRUE   " + SimWindow.RefOrbStack.widget(i).RefOrbFileName.currentText() + "              !  Input file name for Orb " + str(i) + "\n"
            elif SimWindow.RefOrbStack.widget(i).RefOrbOff.isChecked():
                SimWindow.SimDataOut[txt_idx] = "FALSE  " + SimWindow.RefOrbStack.widget(i).RefOrbFileName.text() + "              !  Input file name for Orb " + str(i) + "\n"
            txt_idx += 1

        SimWindow.SimDataOut[txt_idx] = "*****************************  Spacecraft  *****************************\n"
        txt_idx += 1

        # Spacecraft Stack Slot
        SimWindow.SimDataOut[txt_idx] = str(SimWindow.ListSC.count()) + "                               !  Number of Spacecraft\n"
        txt_idx += 1
        for i in range(SimWindow.ListSC.count()):
            if SimWindow.SCstack.widget(i).SpacecraftOn.isChecked():
                SimWindow.SimDataOut[txt_idx] = "TRUE  "  + str(i) + "  " + SimWindow.SCstack.widget(i).SCfile.text() + "          !  Existence, RefOrb, Input file for SC " + str(i) + "\n"
            elif SimWindow.SCstack.widget(i).SpacecraftOff.isChecked():
                SimWindow.SimDataOut[txt_idx] = "FALSE  " + str(i) + "  " + SimWindow.SCstack.widget(i).SCfile.text() + "          !  Existence, RefOrb, Input file for SC " + str(i) + "\n"
            txt_idx += 1

        SimWindow.SimDataOut[txt_idx] = "***************************** Environment  *****************************\n"
        txt_idx += 1

        # UTC Date
        SimWindow.SimDataOut[txt_idx] = SimWindow.UtcMonth.text() + "  " + SimWindow.UtcDay.text() + "  " + SimWindow.UtcYear.text() + "                    !  Date (UTC) (Month, Day, Year)\n"
        txt_idx += 1

        # UTC Time
        SimWindow.SimDataOut[txt_idx] = SimWindow.UtcHour.text() + "  " + SimWindow.UtcMin.text() + "  " + SimWindow.UtcSec.text() + "                   !  Time (UTC) (Hr,Min,Sec)\n"
        txt_idx += 1

        # Leap Seconds
        SimWindow.SimDataOut[txt_idx] = SimWindow.LeapSec.text() + "                            !  Leap Seconds (sec)\n"
        txt_idx += 1

        # F10.7, Ap
        if SimWindow.F10_7_AP.currentText() == "USER":
            SimWindow.SimDataOut[txt_idx] = "USER                            !  F10.7, Ap (USER, NOMINAL or TWOSIGMA)\n"
        elif SimWindow.F10_7_AP.currentText() == "NOMINAL":
            SimWindow.SimDataOut[txt_idx] = "NOMINAL                         !  F10.7, Ap (USER, NOMINAL or TWOSIGMA)\n"
        elif SimWindow.F10_7_AP.currentText() == "TWOSIGMA":
            SimWindow.SimDataOut[txt_idx] = "TWOSIGMA                        !  F10.7, Ap (USER, NOMINAL or TWOSIGMA)\n"
        # Alternative Method:
        # SimWindow.SimDataOut[txt_idx] = SimWindow.F10_7_AP.currentText() + "                            !  F10.7, Ap (USER, NOMINAL or TWOSIGMA)\n"
        txt_idx += 1

        # F10.7
        SimWindow.SimDataOut[txt_idx] = SimWindow.F10_7.text() + "                           !  USER-provided F10.7\n"
        txt_idx += 1

        # Ap
        SimWindow.SimDataOut[txt_idx] = SimWindow.Ap.text() + "                           !  USER-provided Ap\n"
        txt_idx += 1

        # MagField
        if SimWindow.MagField.currentText() == "IGRF":
            SimWindow.SimDataOut[txt_idx] = "IGRF                            !  MagField (NONE,DIPOLE,IGRF)\n"
        elif SimWindow.MagField.currentText() == "DIPOLE":
            SimWindow.SimDataOut[txt_idx] = "DIPOLE                          !  MagField (NONE,DIPOLE,IGRF)\n"
        elif SimWindow.MagField.currentText() == "NONE":
            SimWindow.SimDataOut[txt_idx] = "NONE                            !  MagField (NONE,DIPOLE,IGRF)\n"
        txt_idx += 1

        # IGRF Degree and Order
        SimWindow.SimDataOut[txt_idx] = SimWindow.IgrfDeg.text() + "   " + SimWindow.IgrfOrdr.text() + "                           !  IGRF Degree and Order (<=10)\n"
        txt_idx += 1

        # Earth Gravity
        SimWindow.SimDataOut[txt_idx] = SimWindow.EarthGravN.text() + "   " + SimWindow.EarthGravM.text() + "                           !  Earth Gravity Model N and M (<=18)\n"
        txt_idx += 1

        # Mars Gravity
        SimWindow.SimDataOut[txt_idx] = SimWindow.MarsGravN.text() + "   " + SimWindow.MarsGravM.text() + "                           !  Mars Gravity Model N and M (<=18)\n"
        txt_idx += 1

        # Luna Gravity
        SimWindow.SimDataOut[txt_idx] = SimWindow.LunaGravN.text() + "   " + SimWindow.LunaGravM.text() + "                           !  Luna Gravity Model N and M (<=18)\n"
        txt_idx += 1

        # Aerodynamic Forces & Torques
        if SimWindow.AeroForceOn.isChecked():
            if SimWindow.AeroTorqueOn.isChecked():
                SimWindow.SimDataOut[txt_idx] = "TRUE   TRUE                     !  Aerodynamic Forces & Torques (Shadows)\n"
            elif SimWindow.AeroTorqueOff.isChecked():
                SimWindow.SimDataOut[txt_idx] = "TRUE   FALSE                    !  Aerodynamic Forces & Torques (Shadows)\n"
        elif SimWindow.AeroForceOff.isChecked():
            if SimWindow.AeroTorqueOn.isChecked():
                SimWindow.SimDataOut[txt_idx] = "FALSE   TRUE                    !  Aerodynamic Forces & Torques (Shadows)\n"
            elif SimWindow.AeroTorqueOff.isChecked():
                SimWindow.SimDataOut[txt_idx] = "FALSE   FALSE                   !  Aerodynamic Forces & Torques (Shadows)\n"
        txt_idx += 1

        # Grav Gradient Torque
        if SimWindow.GravGradTrqOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Gravity Gradient Torques\n"
        elif SimWindow.GravGradTrqOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Gravity Gradient Torques\n"
        txt_idx += 1

        # Solar Pressure
        if SimWindow.SrpFrcOn.isChecked():
            if SimWindow.SrpTrqOn.isChecked():
                SimWindow.SimDataOut[txt_idx] = "TRUE   TRUE                     !  Solar Pressure Forces & Torques (Shadows)\n"
            elif SimWindow.SrpTrqOff.isChecked():
                SimWindow.SimDataOut[txt_idx] = "TRUE   FALSE                    !  Solar Pressure Forces & Torques (Shadows)\n"
        elif SimWindow.SrpFrcOff.isChecked():
            if SimWindow.SrpTrqOn.isChecked():
                SimWindow.SimDataOut[txt_idx] = "FALSE   TRUE                    !  Solar Pressure Forces & Torques (Shadows)\n"
            elif SimWindow.SrpTrqOff.isChecked():
                SimWindow.SimDataOut[txt_idx] = "FALSE   FALSE                   !  Solar Pressure Forces & Torques (Shadows)\n"
        txt_idx += 1

        #Gravity Perturbation Forces
        if SimWindow.GravPertFrcOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Gravity Perturbation Forces\n"
        elif SimWindow.GravPertFrcOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Gravity Perturbation Forces\n"
        txt_idx += 1

        #Passive Joint Forces & Torques
        if SimWindow.PassiveJointOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Passive Joint Forces & Torques\n"
        elif SimWindow.PassiveJointOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Passive Joint Forces & Torques\n"
        txt_idx += 1

        #Thruster Plume Forces & Torques
        if SimWindow.ThrustPlumeOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Thruster Plume Forces & Torques\n"
        elif SimWindow.ThrustPlumeOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Thruster Plume Forces & Torques\n"
        txt_idx += 1

        #RWA Imbalance Forces and Torques
        if SimWindow.RwaImbalanceOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  RWA Imbalance Forces and Torques\n"
        elif SimWindow.RwaImbalanceOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  RWA Imbalance Forces and Torques\n"
        txt_idx += 1

        #Contact Forces and Torques
        if SimWindow.ContactOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Contact Forces and Torques\n"
        elif SimWindow.ContactOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Contact Forces and Torques\n"
        txt_idx += 1

        #CFD Slosh Forces and Torques
        if SimWindow.CfdSloshOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  CFD Slosh Forces and Torques\n"
        elif SimWindow.CfdSloshOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  CFD Slosh Forces and Torques\n"
        txt_idx += 1

        #Albedo Effect on CSS Measurements
        if SimWindow.CssAlbedoOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Albedo Effect on CSS Measurements\n"
        elif SimWindow.CssAlbedoOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Albedo Effect on CSS Measurements\n"
        txt_idx += 1

        #Output Environmental Torques to Files
        if SimWindow.OutputEnvTrqOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Output Environmental Torques to Files\n"
        elif SimWindow.OutputEnvTrqOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Output Environmental Torques to Files\n"
        txt_idx += 1

        SimWindow.SimDataOut[txt_idx] = "********************* Celestial Bodies of Interest *********************\n"
        txt_idx += 1

        #Ephemeris Option
        if SimWindow.Ephemeris.currentText() == "MEAN":
            SimWindow.SimDataOut[txt_idx] = "MEAN                            !  Ephem Option (MEAN or DE430)\n"
        elif SimWindow.Ephemeris.currentText() == "DE430":
            SimWindow.SimDataOut[txt_idx] = "DE430                           !  Ephem Option (MEAN or DE430)\n"
        txt_idx += 1

        #Mercury
        if SimWindow.MercuryOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Mercury\n"
        elif SimWindow.MercuryOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Mercury\n"
        txt_idx += 1

        #Venus
        if SimWindow.VenusOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Venus\n"
        elif SimWindow.VenusOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Venus\n"
        txt_idx += 1

        #Earth and Luna
        if SimWindow.EarthLunaOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Earth and Luna\n"
        elif SimWindow.EarthLunaOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Earth and Luna\n"
        txt_idx += 1

        #Mars and its moons
        if SimWindow.MarsMoonsOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Mars and its moons\n"
        elif SimWindow.MarsMoonsOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Mars and its moons\n"
        txt_idx += 1

        #Jupiter and its moons
        if SimWindow.JupiterMoonsOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Jupiter and its moons\n"
        elif SimWindow.JupiterMoonsOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Jupiter and its moons\n"
        txt_idx += 1

        #Saturn and its moons
        if SimWindow.SaturnMoonsOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Saturn and its moons\n"
        elif SimWindow.SaturnMoonsOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Saturn and its moons\n"
        txt_idx += 1

        #Uranus and its moons
        if SimWindow.UranusMoonsOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Uranus and its moons\n"
        elif SimWindow.UranusMoonsOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Uranus and its moons\n"
        txt_idx += 1

        #Neptune and its moons
        if SimWindow.NeptuneMoonsOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Neptune and its moons\n"
        elif SimWindow.NeptuneMoonsOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Neptune and its moons\n"
        txt_idx += 1

        #Pluto and its moons
        if SimWindow.PlutoMoonsOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Pluto and its moons\n"
        elif SimWindow.PlutoMoonsOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Pluto and its moons\n"
        txt_idx += 1

        #Asteroids and Comets
        if SimWindow.AsteroidsCometsOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Asteroids and Comets\n"
        elif SimWindow.AsteroidsCometsOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Asteroids and Comets\n"
        txt_idx += 1

        SimWindow.SimDataOut[txt_idx] = "***************** Lagrange Point Systems of Interest ******************\n"
        txt_idx += 1

        #Earth-Moon System
        if SimWindow.EMsystemOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Earth-Moon\n"
        elif SimWindow.EMsystemOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Earth-Moon\n"
        txt_idx += 1

        #Sun-Earth System
        if SimWindow.SEsystemOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Sun-Earth\n"
        elif SimWindow.SEsystemOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Sun-Earth\n"
        txt_idx += 1

        #Sun-Jupiter System
        if SimWindow.SJsystemOn.isChecked():
            SimWindow.SimDataOut[txt_idx] = "TRUE                            !  Sun-Jupiter\n"
        elif SimWindow.SJsystemOff.isChecked():
            SimWindow.SimDataOut[txt_idx] = "FALSE                           !  Sun-Jupiter\n"
        txt_idx += 1

        SimWindow.SimDataOut[txt_idx] = "************************* Ground Stations ***************************\n"
        txt_idx += 1

        # Ground Station Stack Slot
        SimWindow.SimDataOut[txt_idx] = str(SimWindow.ListGS.count()) + "                                            ! Number of Ground Stations\n"
        txt_idx += 1
        for i in range(SimWindow.ListGS.count()):
            if SimWindow.GSstack.widget(i).GroundStationOn.isChecked():
                SimWindow.SimDataOut[txt_idx] = "TRUE    "  + SimWindow.GSstack.widget(i).GSworld.currentText() + "   " + SimWindow.GSstack.widget(i).Lattitude.text() + "   " + SimWindow.GSstack.widget(i).Longitude.text() + "   " + SimWindow.GSstack.widget(i).GSlabel.currentText() + "          ! Exists, World, Lng, Lat, Label\n"
            elif SimWindow.GSstack.widget(i).GroundStationOff.isChecked():
                SimWindow.SimDataOut[txt_idx] = "FALSE   "  + SimWindow.GSstack.widget(i).GSworld.currentText() + "   " + SimWindow.GSstack.widget(i).Lattitude.text() + "   " + SimWindow.GSstack.widget(i).Longitude.text() + "   " + SimWindow.GSstack.widget(i).GSlabel.currentText() + "          ! Exists, World, Lng, Lat, Label\n"
            txt_idx += 1

    def GetDataIn(SimWindow, FileStr):
        SimWindow.SimDataIn = []
        pattern = re.compile("(.*?)\s*!")

        InpDataFile = open(FileStr)
        for i, line in enumerate(InpDataFile):
            for match in re.finditer(pattern, line):
                SimWindow.SimDataIn.append(match.group(1).replace('"',"").split())
        InpDataFile.close()

    def SetGuiFields(SimWindow):
        idx = 0
        # Simulation Control
        if SimWindow.SimDataIn[idx][0] == "FAST":
            SimWindow.TimeMode.setCurrentIndex(0)
        elif SimWindow.SimDataIn[idx][0] == "REAL":
            SimWindow.TimeMode.setCurrentIndex(1)
        elif SimWindow.SimDataIn[idx][0] == "EXTERNAL":
            SimWindow.TimeMode.setCurrentIndex(2)
        elif SimWindow.SimDataIn[idx][0] == "NOS3":
            SimWindow.TimeMode.setCurrentIndex(3)

        idx += 1
        SimWindow.SimDuration.setText(SimWindow.SimDataIn[idx][0])
        SimWindow.StepSize.setText(SimWindow.SimDataIn[idx][1])

        idx += 1
        SimWindow.OutputInterval.setText(SimWindow.SimDataIn[idx][0])

        idx += 1
        SimWindow.RNGSeed.setText(SimWindow.SimDataIn[idx][0])

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.GraphicsOn.setChecked(True)
        else:
            SimWindow.GraphicsOff.setChecked(True)

        idx += 1
        SimWindow.CmdFileName.setText(SimWindow.SimDataIn[idx][0])

        # Reference Orbits
        idx += 1
        SimWindow.NumRefOrbDataIn = int(SimWindow.SimDataIn[idx][0])

        for i in range(SimWindow.NumRefOrbDataIn):
            idx += 1
            if (i >= SimWindow.ListRefOrb.count()):
                SimWindow.AddRefOrb()

            if (SimWindow.ListRefOrb.count() > SimWindow.NumRefOrbDataIn):
                Diff = SimWindow.ListRefOrb.count() - SimWindow.NumRefOrbDataIn
                for j in range(Diff):
                    SimWindow.RmvRefOrb()

            if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
                SimWindow.RefOrbStack.widget(i).RefOrbOn.setChecked(True)
            else:
                SimWindow.RefOrbStack.widget(i).RefOrbOff.setChecked(True)
            itemIndex = SimWindow.RefOrbStack.widget(i).RefOrbFileName.findText(SimWindow.SimDataIn[idx][1])
            SimWindow.RefOrbStack.widget(i).RefOrbFileName.setCurrentIndex(itemIndex)

        # Spacercraft
        idx += 1
        SimWindow.NumScDataIn = int(SimWindow.SimDataIn[idx][0])

        for j in range(SimWindow.NumScDataIn):
            idx += 1
            if (j >= SimWindow.ListSC.count()):
                SimWindow.AddSC()

            if (SimWindow.ListSC.count() > SimWindow.NumScDataIn):
                Diff = SimWindow.ListSC.count() - SimWindow.NumScDataIn
                for i in range(Diff):
                    SimWindow.RmvSC()

            if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
                SimWindow.SCstack.widget(j).SpacecraftOn.setChecked(True)
            else:
                SimWindow.SCstack.widget(j).SpacecraftOff.setChecked(True)

            SimWindow.SCstack.widget(j).MatchScRefOrb.setCurrentIndex(int(SimWindow.SimDataIn[idx][1]))

            SimWindow.SCstack.widget(j).SCfile.setText(SimWindow.SimDataIn[idx][2])

        # Environment
        idx += 1
        SimWindow.UtcMonth.setText(SimWindow.SimDataIn[idx][0])
        SimWindow.UtcDay.setText(SimWindow.SimDataIn[idx][1])
        SimWindow.UtcYear.setText(SimWindow.SimDataIn[idx][2])

        idx += 1
        SimWindow.UtcHour.setText(SimWindow.SimDataIn[idx][0])
        SimWindow.UtcMin.setText(SimWindow.SimDataIn[idx][1])
        SimWindow.UtcSec.setText(SimWindow.SimDataIn[idx][2])

        idx += 1
        SimWindow.LeapSec.setText(SimWindow.SimDataIn[idx][0])

        idx += 1
        if (SimWindow.SimDataIn[idx][0] == "USER"):
            SimWindow.F10_7_AP.setCurrentIndex(0)
        elif (SimWindow.SimDataIn[idx][0] == "NOMINAL"):
            SimWindow.F10_7_AP.setCurrentIndex(1)
        elif (SimWindow.SimDataIn[idx][0] == "TWOSIGMA"):
            SimWindow.F10_7_AP.setCurrentIndex(2)

        idx += 1
        SimWindow.F10_7.setText(SimWindow.SimDataIn[idx][0])

        idx += 1
        SimWindow.Ap.setText(SimWindow.SimDataIn[idx][0])

        idx += 1
        if (SimWindow.SimDataIn[idx][0] == "NONE"):
            SimWindow.MagField.setCurrentIndex(0)
        elif (SimWindow.SimDataIn[idx][0] == "DIPOLE"):
            SimWindow.MagField.setCurrentIndex(1)
        elif (SimWindow.SimDataIn[idx][0] == "IGRF"):
            SimWindow.MagField.setCurrentIndex(2)

        idx += 1
        SimWindow.IgrfDeg.setText(SimWindow.SimDataIn[idx][0])
        SimWindow.IgrfOrdr.setText(SimWindow.SimDataIn[idx][1])

        idx += 1
        SimWindow.EarthGravN.setText(SimWindow.SimDataIn[idx][0])
        SimWindow.EarthGravM.setText(SimWindow.SimDataIn[idx][1])

        idx += 1
        SimWindow.MarsGravN.setText(SimWindow.SimDataIn[idx][0])
        SimWindow.MarsGravM.setText(SimWindow.SimDataIn[idx][1])

        idx += 1
        SimWindow.LunaGravN.setText(SimWindow.SimDataIn[idx][0])
        SimWindow.LunaGravM.setText(SimWindow.SimDataIn[idx][1])

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.AeroFrcTrqOn.setChecked(True)
        else:
            SimWindow.AeroFrcTrqOff.setChecked(True)

        if (SimWindow.SimDataIn[idx][1].lower() == 'true'):
            SimWindow.AeroFrcTrqShdwOn.setChecked(True)
        else:
            SimWindow.AeroFrcTrqShdwOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.GravGradTrqOn.setChecked(True)
        else:
            SimWindow.GravGradTrqOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.SrpFrcTrqOn.setChecked(True)
        else:
            SimWindow.SrpFrcTrqOff.setChecked(True)

        if (SimWindow.SimDataIn[idx][1].lower() == 'true'):
            SimWindow.SrpFrcTrqShdwOn.setChecked(True)
        else:
            SimWindow.SrpFrcTrqShdwOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.ResMagTrqOn.setChecked(True)
        else:
            SimWindow.ResMagTrqOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.GravPertFrcOn.setChecked(True)
        else:
            SimWindow.GravPertFrcOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.ThrustPlumeOn.setChecked(True)
        else:
            SimWindow.ThrustPlumeOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.ContactOn.setChecked(True)
        else:
            SimWindow.ContactOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.CfdSloshOn.setChecked(True)
        else:
            SimWindow.CfdSloshOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.CssAlbedoOn.setChecked(True)
        else:
            SimWindow.CssAlbedoOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.OutputEnvTrqOn.setChecked(True)
        else:
            SimWindow.OutputEnvTrqOff.setChecked(True)

        # Celestial Bodies of Interest
        idx += 1
        if (SimWindow.SimDataIn[idx][0] == "MEAN"):
            SimWindow.MagField.setCurrentIndex(0)
        elif (SimWindow.SimDataIn[idx][0] == "DE430"):
            SimWindow.MagField.setCurrentIndex(1)
        elif (SimWindow.SimDataIn[idx][0] == "DE430"):
            SimWindow.MagField.setCurrentIndex(2)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.MercuryOn.setChecked(True)
        else:
            SimWindow.MercuryOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.VenusOn.setChecked(True)
        else:
            SimWindow.VenusOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.EarthLunaOn.setChecked(True)
        else:
            SimWindow.EarthLunaOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.MarsMoonsOn.setChecked(True)
        else:
            SimWindow.MarsMoonsOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.JupiterMoonsOn.setChecked(True)
        else:
            SimWindow.JupiterMoonsOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.SaturnMoonsOn.setChecked(True)
        else:
            SimWindow.SaturnMoonsOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.UranusMoonsOn.setChecked(True)
        else:
            SimWindow.UranusMoonsOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.NeptuneMoonsOn.setChecked(True)
        else:
            SimWindow.NeptuneMoonsOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.PlutoMoonsOn.setChecked(True)
        else:
            SimWindow.PlutoMoonsOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.AsteroidsCometsOn.setChecked(True)
        else:
            SimWindow.AsteroidsCometsOff.setChecked(True)

        # Lagrange Point Systems of Interest
        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.EMsystemOn.setChecked(True)
        else:
            SimWindow.EMsystemOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.SEsystemOn.setChecked(True)
        else:
            SimWindow.SEsystemOff.setChecked(True)

        idx += 1
        if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
            SimWindow.SJsystemOn.setChecked(True)
        else:
            SimWindow.SJsystemOff.setChecked(True)

        # Ground Stations
        idx += 1
        SimWindow.NumGsDataIn = int(SimWindow.SimDataIn[idx][0])

        for k in range(SimWindow.NumGsDataIn):
            idx += 1
            if (k >= SimWindow.ListGS.count()):
                SimWindow.AddGroundStation()

            if (SimWindow.ListGS.count() > SimWindow.NumGsDataIn):
                Diff = SimWindow.ListGS.count() - SimWindow.NumGsDataIn
                for i in range(Diff):
                    SimWindow.RmvGroundStation()

            if (SimWindow.SimDataIn[idx][0].lower() == 'true'):
                SimWindow.GSstack.widget(k).GroundStationOn.setChecked(True)
            else:
                SimWindow.GSstack.widget(k).GroundStationOff.setChecked(True)

            if (SimWindow.SimDataIn[idx][1] == "EARTH"):
                SimWindow.GSstack.widget(k).GSworld.setCurrentIndex(0)
            elif (SimWindow.SimDataIn[idx][1] == "LUNA"):
                SimWindow.GSstack.widget(k).GSworld.setCurrentIndex(1)
            elif (SimWindow.SimDataIn[idx][1] == "MARS"):
                SimWindow.GSstack.widget(k).GSworld.setCurrentIndex(2)

            SimWindow.GSstack.widget(k).Lattitude.setText(SimWindow.SimDataIn[idx][2])
            SimWindow.GSstack.widget(k).Longitude.setText(SimWindow.SimDataIn[idx][3])

            if (SimWindow.SimDataIn[idx][4] == "GSFC"):
                SimWindow.GSstack.widget(k).GSlabel.setCurrentIndex(0)
            elif (SimWindow.SimDataIn[idx][4] == "SouthPoint"):
                SimWindow.GSstack.widget(k).GSlabel.setCurrentIndex(1)
            elif (SimWindow.SimDataIn[idx][4] == "Dongara"):
                SimWindow.GSstack.widget(k).GSlabel.setCurrentIndex(2)
            elif (SimWindow.SimDataIn[idx][4] == "Santiago"):
                SimWindow.GSstack.widget(k).GSlabel.setCurrentIndex(3)
            elif (SimWindow.SimDataIn[idx][4] == "MoonBaseAlpha"):
                SimWindow.GSstack.widget(k).GSlabel.setCurrentIndex(4)

#-------------------------------------------------------------------------------
# Reference Orbit Stack---------------------------------------------------------
    def AddRefOrbStack(SimWindow):
        Label = [0]*4 # Initialize array of labels
        SimWindow.RefOrbPage = QWidget() # Create widget to become a Page in stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout() # for the radio buttons

        # Ref Orbit Status------------------------------------------------------
        Label[1] = QLabel("Reference Orbit Status:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 0, 0)

        SimWindow.RefOrbPage.RefOrbOn = QRadioButton('On')
        SimWindow.RefOrbPage.RefOrbOn.setChecked(True)
        SimWindow.RefOrbPage.RefOrbOff = QRadioButton('Off')
        SimWindow.RefOrbPage.RefOrbOff.setChecked(False)
        SubLayout.addWidget(SimWindow.RefOrbPage.RefOrbOn)
        SubLayout.addWidget(SimWindow.RefOrbPage.RefOrbOff)
        StackLayout.addLayout(SubLayout, 0, 1)

        # Orbit Input File Name-------------------------------------------------
        Label[2] = QLabel("Orbit Input File Name:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[2], 1, 0)

        SimWindow.RefOrbPage.RefOrbFileName = QComboBox()
        SimWindow.RefOrbPage.RefOrbFileName.addItems(SimWindow.orbFileNames)
        StackLayout.addWidget(SimWindow.RefOrbPage.RefOrbFileName, 1, 1)

        SimWindow.OrbWindow = ORB.OrbitWindow()
        SimWindow.OrbWindow.updateRefOrbitList.connect(SimWindow.updateRefOrbitOptions)
        # Add Ref Orbit Button--------------------------------------------------
        AddOrbBtn = QPushButton('Add an Orbit')
        StackLayout.addWidget(AddOrbBtn, 2, 1)
        AddOrbBtn.clicked.connect(SimWindow.AddRefOrb)

        # Remove Ref Orbit btn
        RmvRefOrbBtn = QPushButton('Remove an Orbit')
        StackLayout.addWidget(RmvRefOrbBtn, 2, 0)
        RmvRefOrbBtn.clicked.connect(SimWindow.RmvRefOrb)

        SimWindow.RefOrbPage.setLayout(StackLayout)
        SimWindow.RefOrbStack.addWidget(SimWindow.RefOrbPage)

    def DisplayRefOrbStackSlot(SimWindow, i): # Displayes current page in stack when selected in the RefOrb list
        SimWindow.RefOrbStack.setCurrentIndex(i)

    def AddRefOrb(SimWindow): # Add a ref orb to the list and the stack
        ItemIndex = SimWindow.ListRefOrb.count() # Index of new item
        ItemLabel = ItemIndex + 1
        SimWindow.ListRefOrb.insertItem(int(ItemIndex), "Ref. Orb. %s"%ItemLabel)
        SimWindow.AddRefOrbStack()
        SimWindow.ListRefOrb.setCurrentRow(ItemIndex)
        SimWindow.RefOrbStack.setCurrentIndex(ItemIndex)

        # Adding the new ref orb to each SC currently in the SC list
        for i in range(SimWindow.ListSC.count()):
            SimWindow.SCstack.widget(i).MatchScRefOrb.addItem("Ref. Orb. %s"%ItemLabel)

    def RmvRefOrb(SimWindow): # Removes the last Ref Orb added to the List
        RmvIndex = SimWindow.ListRefOrb.count() - 1
        if RmvIndex > 0:
            SimWindow.ListRefOrb.takeItem(RmvIndex)
            SimWindow.RefOrbStack.removeWidget(SimWindow.RefOrbStack.widget(RmvIndex))

            # Removing the ref orb from each SC currently in the SC list
            for i in range(SimWindow.ListSC.count()):
                SimWindow.SCstack.widget(i).MatchScRefOrb.removeItem(RmvIndex)

#-------------------------------------------------------------------------------
# Spacecraft Stack--------------------------------------------------------------
    def AddSCstack(SimWindow): # Adds a new page to the SC stack with the same layout as previous pages
        Label = [0]*3 # Initialize array of labels
        SimWindow.SCpage = QWidget() # Create widget to become a Page in stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout() # for the radio buttons

        # Spacecraft Status------------------------------------------------------
        Label[0] = QLabel("Spacecraft Status:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[0], 0, 0)

        SimWindow.SCpage.SpacecraftOn = QRadioButton('On')
        SimWindow.SCpage.SpacecraftOn.setChecked(True)
        SubLayout.addWidget(SimWindow.SCpage.SpacecraftOn)
        SimWindow.SCpage.SpacecraftOff = QRadioButton('Off')
        SimWindow.SCpage.SpacecraftOff.setChecked(False)
        SubLayout.addWidget(SimWindow.SCpage.SpacecraftOff)
        StackLayout.addLayout(SubLayout, 0, 1)

        # SC File Label---------------------------------------------------------
        Label[1] = QLabel("Spacecraft Input File:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 1, 0)

        SimWindow.SCpage.SCfile = QLineEdit("SC_Simple.txt")
        StackLayout.addWidget(SimWindow.SCpage.SCfile, 1, 1)

        # Pair SC with Ref Orb--------------------------------------------------
        Label[2] = QLabel("Reference Orbit:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[2], 2, 0)

        SimWindow.SCpage.MatchScRefOrb = QComboBox()
        # Add Ref Orb Options to ComboBox on each SC stack page
        for i in range(SimWindow.ListRefOrb.count()):
            ItemLabel = i + 1
            SimWindow.SCpage.MatchScRefOrb.addItem("Ref. Orb. %s"%ItemLabel)

        StackLayout.addWidget(SimWindow.SCpage.MatchScRefOrb, 2, 1)

        # Add Spacecraft Button--------------------------------------------------
        AddSCBtn = QPushButton('Add a Spacecraft')
        StackLayout.addWidget(AddSCBtn, 3, 1)
        AddSCBtn.clicked.connect(SimWindow.AddSC)

        # Remove Spacecraft btn
        RmvSCBtn = QPushButton('Remove a Spacecraft')
        StackLayout.addWidget(RmvSCBtn, 3, 0)
        RmvSCBtn.clicked.connect(SimWindow.RmvSC)

        SimWindow.SCpage.setLayout(StackLayout)
        SimWindow.SCstack.addWidget(SimWindow.SCpage)

    def DisplaySCstackSlot(SimWindow, i): # Displayes current page in stack when selected in the SC list
        SimWindow.SCstack.setCurrentIndex(i)

    def AddSC(SimWindow): # Adds a spacecraft to SC list and SC stack
        ItemIndex = SimWindow.ListSC.count() # Index of new item
        ItemLabel = ItemIndex + 1
        SimWindow.ListSC.insertItem(int(ItemIndex), "Spacecraft %s"%ItemLabel)
        SimWindow.AddSCstack()
        SimWindow.ListSC.setCurrentRow(ItemIndex)
        SimWindow.SCstack.setCurrentIndex(ItemIndex)

    def RmvSC(SimWindow): # Removes the last SC added to the SC List
        RmvIndex = SimWindow.ListSC.count()- 1
        if RmvIndex > 0:
            SimWindow.ListSC.takeItem(RmvIndex)
            SimWindow.SCstack.removeWidget(SimWindow.SCstack.widget(RmvIndex))

#-------------------------------------------------------------------------------
# Ground Station Stack--------------------------------------------------------------
    def AddGSstack(SimWindow): # Adds a new page to the SC stack with the same layout as previous pages
        Label = [0]*4 # Initialize array of labels
        SimWindow.GSpage = QWidget() # Create widget to become a Page in stack
        StackLayout = QGridLayout()
        SubLayout = QHBoxLayout() # for the radio buttons

        # Ground Station Status------------------------------------------------------
        Label[0] = QLabel("Ground Station Status:")
        Label[0].setAlignment(Qt.AlignLeft)
        Label[0].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[0], 0, 0)

        SimWindow.GSpage.GroundStationOn = QRadioButton('On')
        SimWindow.GSpage.GroundStationOn.setChecked(True)
        SubLayout.addWidget(SimWindow.GSpage.GroundStationOn)
        SimWindow.GSpage.GroundStationOff = QRadioButton('Off')
        SimWindow.GSpage.GroundStationOff.setChecked(False)
        SubLayout.addWidget(SimWindow.GSpage.GroundStationOff)
        StackLayout.addLayout(SubLayout, 0, 1)

        # World-----------------------------------------------------------------
        Label[1] = QLabel("Ground Station World:")
        Label[1].setAlignment(Qt.AlignLeft)
        Label[1].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[1], 1, 0)

        SimWindow.GSpage.GSworld = QComboBox()
        SimWindow.GSpage.GSworld.addItems(["EARTH", "LUNA", "MARS"])
        StackLayout.addWidget(SimWindow.GSpage.GSworld, 1, 1)

        # Location of Ground Station--------------------------------------------
        Label[2] = QLabel("Lattitude & Longitude:")
        Label[2].setAlignment(Qt.AlignLeft)
        Label[2].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[2], 2, 0)

        SimWindow.GSpage.Lattitude = QLineEdit('-77.0')
        SimWindow.GSpage.Longitude = QLineEdit('37.0')

        SubLayout = QHBoxLayout()
        SubLayout.addWidget(SimWindow.GSpage.Lattitude)
        SubLayout.addWidget(SimWindow.GSpage.Longitude)
        StackLayout.addLayout(SubLayout,2, 1, 1, 2)

        # Ground Station Label--------------------------------------------------
        Label[3] = QLabel("Ground Station World:")
        Label[3].setAlignment(Qt.AlignLeft)
        Label[3].setStyleSheet('font-size: 15px')
        StackLayout.addWidget(Label[3], 3, 0)

        SimWindow.GSpage.GSlabel = QComboBox()
        SimWindow.GSpage.GSlabel.addItems(["GSFC", "SouthPoint", "Dongara", "Santiago", "MoonBaseAlpha"])
        StackLayout.addWidget(SimWindow.GSpage.GSlabel, 3, 1)

        # Add Ground Station Button--------------------------------------------------
        AddGSbtn = QPushButton('Add a Ground Station')
        StackLayout.addWidget(AddGSbtn, 4, 1)
        AddGSbtn.clicked.connect(SimWindow.AddGroundStation)

        # Remove Ground Station btn
        RmvGSbtn = QPushButton('Remove a Ground Station')
        StackLayout.addWidget(RmvGSbtn, 4, 0)
        RmvGSbtn.clicked.connect(SimWindow.RmvGroundStation)

        SimWindow.GSpage.setLayout(StackLayout)
        SimWindow.GSstack.addWidget(SimWindow.GSpage)

    def DisplayGSstackSlot(SimWindow, i): # Displayes current page in stack when selected in the SC list
        SimWindow.GSstack.setCurrentIndex(i)

    def AddGroundStation(SimWindow): # Adds a spacecraft to GS list and GS stack
        ItemIndex = SimWindow.ListGS.count() # Index of new item
        ItemLabel = ItemIndex + 1
        SimWindow.ListGS.insertItem(int(ItemIndex), "Ground Station %s"%ItemLabel)
        SimWindow.AddGSstack()
        SimWindow.ListGS.setCurrentRow(ItemIndex)
        SimWindow.GSstack.setCurrentIndex(ItemIndex)

    def RmvGroundStation(SimWindow): # Removes the last GS added to the GS List
        RmvIndex = SimWindow.ListGS.count() - 1
        if RmvIndex > 0:
            SimWindow.ListGS.takeItem(RmvIndex)
            SimWindow.GSstack.removeWidget(SimWindow.GSstack.widget(RmvIndex))

    def updateRefOrbitOptions(SimWindow):
        functions.get_sc_orbit_file_names(SimWindow)
        for i in range(SimWindow.ListRefOrb.count()):
            SimWindow.RefOrbStack.widget(i).RefOrbFileName.clear()
            SimWindow.RefOrbStack.widget(i).RefOrbFileName.addItems(self.orbFileNames)
