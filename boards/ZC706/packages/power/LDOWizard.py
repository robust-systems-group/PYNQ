#!/usr/bin/env python3

#########################################
# Class to control LDOs through GPIO
# expanders on I2C
########################################

from PCA9557 import PCA9557
from TCA6416 import TCA6416
import zc706_I2C
import math
import time

class LDOWizard:
    def __init__(self):
        self.rails = ["VDD_CORE", "VDD_RRAM", "VDD_SMACRO", "VDIO_CORE", "VDD_RMACRO", "VDIO_RMACRO", "VDUH_RMACRO", "VDD_IO"]
        self.addrTranslation = {
            "VDD_CORE": 0x4, "VDD_RRAM": 0x4, 
            "VDD_SMACRO": 0x2, "VDIO_CORE": 0x2, 
            "VDD_RMACRO": 0x1, "VDIO_RMACRO": 0x1, 
            "VDUH_RMACRO": 0x0, "VDD_IO": 0x0}
        self.portExpander = 7
        self.addrExpanders = {
            "VDD_CORE": 0x28, "VDD_RRAM": 0x2C,
            "VDD_SMACRO": 0x38, "VDIO_CORE": 0x3C,
            "VDD_RMACRO": 0x08, "VDIO_RMACRO": 0x0C,
            "VDUH_RMACRO": 0x18, "VDD_IO": 0x1C}
        self.highRange = {
            "VDD_CORE": False, "VDD_RRAM": False, 
            "VDD_SMACRO": False, "VDIO_CORE": False, 
            "VDD_RMACRO": False, "VDIO_RMACRO": True, 
            "VDUH_RMACRO": True, "VDD_IO": True}
        self.ldos = {
            "VDD_CORE": None, "VDD_RRAM": None,
            "VDD_SMACRO": None, "VDIO_CORE": None,
            "VDD_RMACRO": None, "VDIO_RMACRO": None,
            "VDUH_RMACRO": None, "VDD_IO": None}
        self.addrTranslator = TCA6416(zc706_I2C.PORT_EXPANDER_PORT, zc706_I2C.I2C_PMOD_GPIO_EXPANDER, 0x0, 0x0) #Start with all outputs to 0
        for rail in self.rails:
            self.selectLDO(rail)
            self.ldos[rail] = LDO(rail, self.portExpander, self.addrExpanders[rail], self.highRange[rail])

    def __del__(self):
        del self.addrTranslator
        for ldo in self.ldos.values():
            del ldo

    def enableAll(self):
        for rail in self.rails:
            self.selectLDO(rail)
            self.ldos[rail].enable()

    def disableAll(self):
        for rail in self.rails:
            self.selectLDO(rail)
            self.ldos[rail].disable()
    
    def enable(self, *args):
        for rail in args:
            assert rail in self.rails, "Invalid rail name"
            self.selectLDO(rail)
            self.ldos[rail].enable()
    
    def disable(self, *args):
        for rail in args:
            assert rail in self.rails, "Invalid rail name"
            self.selectLDO(rail)
            self.ldos[rail].disable()
    
    def getEnables(self):
        enables = {}
        for rail in self.rails:
            self.selectLDO(rail)
            enables[rail] = self.ldos[rail].getEnable()
        return enables
    
    def setVoltages(self, **kwargs):
        for rail, voltage in kwargs.items():
            assert rail in self.rails, "Invalid rail name"
            assert isinstance(voltage, float), "Voltage must be a float"
            self.selectLDO(rail)
            enable = self.ldos[rail].getEnable()
            self.ldos[rail].disable()
            self.ldos[rail].setVoltage(voltage)
            if enable: self.ldos[rail].enable()

    def getVoltages(self):
        voltages = {}
        for rail in self.rails:
            self.selectLDO(rail)
            voltages[rail] = self.ldos[rail].getVoltage()
        return voltages

    def getPowerGoods(self):
        powerGoods = {}
        for rail in self.rails:
            self.selectLDO(rail)
            powerGoods[rail] = self.ldos[rail].getPowerGood()
        return powerGoods

    def selectLDO(self, rail):
        #Set the I2C Address Translator enable bits to the correct LDO
        current_bits = self.addrTranslator.read_output_reg()
        ctrl_bits = self.addrTranslation[rail]
        if (ctrl_bits != current_bits): self.addrTranslator.write_output_reg(ctrl_bits)
        time.sleep(0.01)

class LDO:
    #Pin 8 is the powerGood pin
    #Pin 7 is the enable pin
    #Pin 6-0 are the voltage setting pins
    def __init__(self, name, portExpander, addrExpander, highRange=False):
        self.name = name
        self.portExpander = portExpander
        self.addrExpander = addrExpander
        self.highRange = highRange
        self.expander = PCA9557(self.portExpander, self.addrExpander, 0b10111111, 0x0)

    def __del__(self):
        if self.expander is not None: del self.expander

    def enable(self):
        #Need to set the pin to output
        config = self.expander.read_config_reg()
        config = config & 0b10111111
        self.expander.write_config_reg(config)
        #Need to set the pin to high
        self.expander.write_output_reg(0b01000000)

    def disable(self):
        #Need to set the pin to output
        config = self.expander.read_config_reg()
        config = config & 0b10111111
        self.expander.write_config_reg(config)
        #Need to set the pin to low
        self.expander.write_output_reg(0b00000000)

    def getEnable(self):
        return bool(self.expander.read_output_reg() & 0b01000000)

    def setVoltage(self, voltage):
        config = self.expander.read_config_reg()
        config = config & 0b11000000
        if self.highRange:
            assert voltage >= 0.8 and voltage <= 3.3, "Voltage must be between 0.8 and 3.3"
            offset = voltage - 0.8
            offset_int = math.ceil(offset / 0.025) >> 1 #Avoid rounding issues
            self.expander.write_config_reg((config | offset_int) ^ 0x3F)
        else:
            assert voltage >= 0.5 and voltage <= 1.25, "Voltage must be between 0.5 and 1.25"
            offset = voltage - 0.5
            offset_int = math.ceil(offset / 0.0125) >> 1 #Avoid rounding issues
            self.expander.write_config_reg((config | offset_int) ^ 0x3F)

    def getVoltage(self):
        config = self.expander.read_config_reg()
        offset_int = (config & 0b00111111) ^ 0x3F
        if self.highRange:
            return 0.8 + (offset_int * 0.05)
        else:
            return 0.5 + (offset_int * 0.025)

    def getPowerGood(self):
        #Need to set the pin to input
        config = self.expander.read_config_reg()
        config = config | 0b10000000
        self.expander.write_config_reg(config)
        #Need to read the pin
        input = self.expander.read_input_reg()
        return bool(input & 0b10000000)