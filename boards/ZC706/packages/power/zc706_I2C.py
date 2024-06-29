#########################################
# Hardcoded I2C bus connections
# on the Xilinx 7000 FPGA boards
########################################

# Linux device ports
I2C_ADAPTER             = "/dev/i2c-0"
SI570_PORT              = "/dev/i2c-1"
HDMI_PORT               = "/dev/i2c-2"
EEPROM_PORT             = "/dev/i2c-3"
PORT_EXPANDER_PORT      = "/dev/i2c-4"
RTC_PORT                = "/dev/i2c-5"
FMC_HPC_I2C_PORT        = "/dev/i2c-6"
FMC_LPC_I2C_PORT        = "/dev/i2c-7"
PMBUS_PORT              = "/dev/i2c-8"

"""
Channel status Masks for the onboard I2C Bus Mux on the zc706.
A 1 in the index corresponds to enabling the channel on the mux.
A 0 in the index means the channel is disabled.
Multiple channels may be enabled at the same time.
"""
SI570_MASK              = 0b00000001
HDMI_MASK               = 0b00000010
EEPROM_MASK             = 0b00000100
PORT_EXPANDER_MASK      = 0b00001000
RTC_MASK                = 0b00010000
FMC_HPC_MASK            = 0b00100000
FMC_LPC_MASK            = 0b01000000
PMBUS_MASK              = 0b10000000

PCA9548_MUX_MASK      = {
    SI570_PORT          : SI570_MASK,
    HDMI_PORT           : HDMI_MASK,
    EEPROM_PORT         : EEPROM_MASK,
    PORT_EXPANDER_PORT  : PORT_EXPANDER_MASK,
    RTC_PORT            : RTC_MASK,
    FMC_LPC_I2C_PORT    : FMC_LPC_MASK,
    FMC_HPC_I2C_PORT    : FMC_HPC_MASK,
    PMBUS_PORT          : PMBUS_MASK
}

I2C_SWITCH_MUX_ADDRESS  = 0x74
I2C_PMOD_GPIO_EXPANDER  = 0x21