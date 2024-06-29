#!/usr/bin/env python3

#############################################
# class to handle the TCA6516 gpio expander
############################################

from smbus2 import SMBus
import logging

class TCA6416:
    """ This class is used to program TCA6416 """

    def __init__(self, portNum, busAddr, initialDirectionMask = 0x0, initialOutput=0x0):
        #Control register addresses - This doesn't touch the uper 8 bits
        self.INPUT_REG = 0
        self.OUTPUT_REG = 2
        self.POLARITY_INVRSN_REG = 4
        self.CONFIG_REG = 6 
        self.portNum = portNum                                      # Port number
        self.i2c_device_connection = SMBus(bus=portNum, force = True)        # Connecting to I2C Bus
        self.busAddr = busAddr                                      # Addresses
        self.initialDirectionMask = initialDirectionMask            # initial pin directions
        self.initialOutput = initialOutput                          # initial output values
        self.write_config_reg(self.initialDirectionMask)            # Setting initial pin directions
        self.write_output_reg(self.initialOutput)                   # Setting initial output values
        logging.debug('{} {} {} {} {}'.format("TCA6416 expander on port ", self.portNum, " with address ", self.busAddr, " initialized"))

    def __del__(self):
        self.i2c_device_connection.close()
    
    def read_port(self, regAddr):
        return self.i2c_device_connection.read_byte_data(self.busAddr, regAddr, force=True)

    def read_input_reg(self):
        """ This register is a read-only port. 
        It reflects the incoming logic levels of the pins, 
        regardless of whether the pin is defined as an input 
        or an output by the Configuration register. Writes to this 
        register have no effect. """
        return self.read_port(self.INPUT_REG)

    def read_output_reg(self):
        """ This register reflects the outgoing logic levels of 
        the pins defined as outputs by the Configuration register. 
        Bit values in this register have no effect on pins defined as inputs. 
        In turn, reads from this register reflect the value that is in the 
        flip-flop controlling the output selection, not the actual pin value. """
        return self.read_port(self.OUTPUT_REG)

    def read_polarity_reg(self):
        """ This register enables polarity inversion of pins defined as inputs 
        by the Configuration register. If a bit in this register is set 
        (written with logic 1), the corresponding port pin’s polarity is inverted. 
        If a bit in this register is cleared (written with logic 0), 
        the corresponding port pin’s original polarity is retained. """
        return self.read_port(self.POLARITY_INVRSN_REG)
    
    def read_config_reg(self):
        """ This register configures the directions of the I/O pins. 
        If a bit in this register is set, the corresponding port pin 
        is enabled as an input with high-impedance output driver. 
        If a bit in this register is cleared, the corresponding 
        port pin is enabled as an output. """
        return self.read_port(self.CONFIG_REG)

    def write_port(self, regAddr, byte):
        """ all writes to the TCA6516 should be writes to one of 
        the defined registers. """
        self.i2c_device_connection.write_byte_data(self.busAddr, regAddr, byte, force=True)

    def write_output_reg(self, byte):
        """ This register reflects the outgoing logic levels of 
        the pins defined as outputs by the Configuration register. 
        Bit values in this register have no effect on pins defined as inputs. 
        In turn, reads from this register reflect the value that is in the 
        flip-flop controlling the output selection, not the actual pin value. """
        self.write_port(self.OUTPUT_REG, byte)

    def write_polarity_reg(self, byte):
        """ This register enables polarity inversion of pins defined as inputs 
        by the Configuration register. If a bit in this register is set 
        (written with logic 1), the corresponding port pin’s polarity is inverted. 
        If a bit in this register is cleared (written with logic 0), 
        the corresponding port pin’s original polarity is retained. """
        self.write_port(self.POLARITY_INVRSN_REG, byte)
    
    def write_config_reg(self, byte):
        """ This register configures the directions of the I/O pins. 
        If a bit in this register is set, the corresponding port pin 
        is enabled as an input with high-impedance output driver. 
        If a bit in this register is cleared, the corresponding 
        port pin is enabled as an output. """
        self.write_port(self.CONFIG_REG, byte)