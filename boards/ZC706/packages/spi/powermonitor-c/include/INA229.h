#ifdef __cplusplus
extern "C" {
#endif

#ifndef _INA229_SPI_H
#define _INA229_SPI_H

#include <stdint.h>

#define __CONFIG 0x00
#define __CONFIG_SIZE 16
#define __ADC_CONFIG 0x01
#define __ADC_CONFIG_SIZE 16
#define __SHUNT_CAL 0x02
#define __SHUNT_CAL_SIZE 16
#define __SHUNT_TEMPCO 0x03
#define __SHUNT_TEMPCO_SIZE 16
#define __VSHUNT 0x04
#define __VSHUNT_SIZE 24
#define __VBUS 0x05
#define __VBUS_SIZE 24
#define __DIETEMP 0x06
#define __DIETEMP_SIZE 16
#define __CURRENT 0x07
#define __CURRENT_SIZE 24
#define __POWER 0x08
#define __POWER_SIZE 24
#define __ENERGY 0x09
#define __ENERGY_SIZE 40
#define __CHARGE 0x0A
#define __CHARGE_SIZE 40
#define __DIAG_ALRT 0x0B
#define __DIAG_ALRT_SIZE 16
#define __SOVL 0x0C
#define __SOVL_SIZE 16
#define __SUVL 0x0D
#define __SUVL_SIZE 16
#define __BOVL 0x0E
#define __BOVL_SIZE 16
#define __BUVL 0x0F
#define __BUVL_SIZE 16
#define __TEMP_LIMIT 0x10
#define __TEMP_LIMIT_SIZE 16
#define __PWR_LIMIT 0x11
#define __PWR_LIMIT_SIZE 24
#define __MANUFACTURER_ID 0x3E
#define __MANUFACTURER_ID_SIZE 16
#define __DEVICE_ID 0x3F
#define __DEVICE_ID_SIZE 16

// Default values
#define __MANUFACTURER_ID_EXPECTED 0x5449
#define __DEVICE_ID_EXPECTED 0x2291

// Values below must be used as a 'double' to have proper precision
// default shunt resistance = 0.01
#define __SHUNT_RESIST_DEFAULT 0.01
// 195.3125 uV/LSB
#define __VBUS_CONVERSION_FACTOR (double)195.3125e-6
// 78.125 nV/LSB (used for ADCRANGE=1)
#define __VSHUNT_CONVERSION_FACTOR 0.000000078125
// 7.8125 m degC/LSB
#define __TEMP_CONVERSION_FACTOR 0.0078125

// default max_current = 0.2
#define __MAX_CURRENT_DEFAULT 0.2

// continuous bus, shunt and temperature mode
#define __MODE 0xF
// 4120 us conversion times
#define __VBUS_CT 0x4
#define __VSH_CT 0x4
#define __VT_CT 0x4
// 128 samples averaged
#define __ADC_AVG 0x2
// 2^19, used for calculating current_lsb
#define __CURRENT_LSB_QUOTIENT (double)(1 << 19)
// used to cap the size of a device path string
#define SPIDEV_MAXPATH 1024

// taken from py-spidev: SpiDevObject in spidev_module.c
typedef struct {
  uint8_t mode;
  uint8_t bits_per_word;
  uint32_t max_speed_hz;
  uint8_t read0; /* read 0 bytes after transfer to lower CS if SPI_CS_HIGH */
} spi_dev_config;

struct ina229 {
  int fd;
  char *path;
  spi_dev_config config;
  double max_current;
  double shunt_resistance;
  double current_lsb;
  double stats[4];
};

static const uint8_t STAT_REGS[] = {__VBUS, __VSHUNT, __CURRENT, __POWER};

struct ina229 *ina229_open(const uint8_t bus, const uint8_t cs,
                           const double max_current);
int ina229_close(struct ina229 *device);
int64_t ina229_read_register(const struct ina229 *device, const uint8_t reg,
                             const uint8_t reg_len);
int ina229_write_register(const struct ina229 *device, const uint8_t reg,
                          const uint16_t data);
double ina229_read_temperature(const struct ina229 *device);
double ina229_read_vbus(const struct ina229 *device);
double ina229_read_vshunt(const struct ina229 *device);
double ina229_read_current(const struct ina229 *device);
double ina229_read_power(const struct ina229 *device);
double ina229_read_energy(const struct ina229 *device);
double ina229_read_charge(const struct ina229 *device);
int ina229_read_stats(struct ina229 *device);
#endif
#ifdef __cplusplus
}
#endif
