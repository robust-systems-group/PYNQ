/***********************************************************************
 * FILENAME :        INA229.c
 *
 * DESCRIPTION :
 *       Driver for the INA229 by Texas Instruments. This is written in C
 *       using the linux kernel's spidev interface. Specifically, we wanted
 *       to use this interface so that we could minimize the number of syscalls
 *       needed to read measurements from the device.
 *
 *
 * NOTES :
 *      With this driver, we are able to submit spi requests for all 4 desired
 *      stats (bus voltage, shunt voltage, current and power) in a single
 *      syscall, and the kernel will submit all of these commands to the given
 *      device without toggling the chip-select line between.
 *
 *      We saw ~5x improvement over our python driver with a naive approach
 *      of submitting a separate ioctl request for each register read, and
 *      ~10x improvement over the python driver once we began using the
 *      multi-xfer method supported by the kernel.
 *
 *      For reference, the python driver took ~1ms to read all 4 statistics
 *      from the device, while the multi-xfer method in this driver takes
 *      ~0.1ms, and the individual method in this driver takes ~0.2ms.
 *
 * AUTHOR :    Tristen Nollman <tnollman@cs.stanford.edu>
 * START DATE :    07/2023
 ***********************************************************************/

#include "INA229.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

__attribute__((always_inline)) inline void
cleanup_failed_ina229(char *msg, char *devpath, struct ina229 *device) {
  printf(msg, devpath);
  if (device) {
    close(device->fd);
    free(device);
  }
  free(devpath);
}

/* Func: ina229_open
 *      - Initializes an ina229 device given a bus and chip-select (cs)
 *
 * Params:
 *      - bus: index of the bus that the spi device is on. Our python driver
 * always uses '1', so we verify that we only get a '1' as input, but leave
 * it is a parameter in case we have multiple busses with INA229's on them
 * in the future
 *      - cs: chip-select, used to select which spi client device we wish
 *      to use on the given bus.
 *      - max_current: the value to use as the max_current. By default, we use
 * 0.2, but we can potentially pass in another value here. If 0 is given as this
 * parameter, we will configure the ina229 to use the default value of '0.2'
 *
 * Returns: pointer to ina229 device struct, or NULL if we failed to open it
 * properly
 */
struct ina229 *ina229_open(const uint8_t bus, const uint8_t cs,
                           const double max_current) {
  if (bus != 1) {
    printf("Passed invalid bus ID to ina229_init. Bus ID: %d\n", bus);
    return NULL;
  }
  if (cs > 7) {
    printf("Passed invalid chip-select (cs) to ina229_init. CS: %d\n", cs);
    return NULL;
  }
  // used for IOCTL, ripped from py-spidev
  uint8_t tmp8;
  uint32_t tmp32;
  spi_dev_config config;

  // setup device string
  char *path = (char *)malloc(sizeof(char) * SPIDEV_MAXPATH);
  if (snprintf(path, SPIDEV_MAXPATH, "/dev/spidev%d.%d", bus, cs) >=
      SPIDEV_MAXPATH) {
    printf("Error setting device path. Possible truncation, or overflow\n");
    free(path);
    return NULL;
  }

  // open spi device
  int devfd;
  if ((devfd = open(path, O_RDWR, 0)) == -1) {
    cleanup_failed_ina229("Failed to open device with read/write access: %s\n",
                          path, NULL);
    return NULL;
  }
  // set spi mode to 0x1
  uint8_t mode = 1;
  if (ioctl(devfd, SPI_IOC_WR_MODE, &mode) == -1) {
    cleanup_failed_ina229("ioctl call to set mode for device %s failed.\n",
                          path, NULL);
    return NULL;
  }
  if (ioctl(devfd, SPI_IOC_RD_MODE, &tmp8) == -1) {
    cleanup_failed_ina229("ioctl call failed for spi read mode on device: %s\n",
                          path, NULL);
    return NULL;
  }
  if (mode != tmp8) {
    cleanup_failed_ina229(
        "Attempted to set mode %x, but ioctl returned mode %x.\nDevice: %s",
        path, NULL);
    return NULL;
  }
  config.mode = tmp8;

  // setup bits per word
  if (ioctl(devfd, SPI_IOC_RD_BITS_PER_WORD, &tmp8) == -1) {
    cleanup_failed_ina229("Failed to set bits per word on device: %s\n", path,
                          NULL);
    return NULL;
  }
  config.bits_per_word = tmp8;

  // setup max speed (Hz)
  if (ioctl(devfd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp32) == -1) {
    cleanup_failed_ina229("Failed to set max read speed in Hz for device: %s\n",
                          path, NULL);
    return NULL;
  }
  config.max_speed_hz = tmp32;
  config.read0 = 0; // our CS is low when active, not high

  // select max current value
  double max_curr = (max_current) ? max_current : __MAX_CURRENT_DEFAULT;

  /* NOTE
   * Any error branch after this, we must free() 'device'
   */
  struct ina229 *device = (struct ina229 *)malloc(sizeof(struct ina229));
  assert(device != NULL);

  // setup ina229 device
  device->fd = devfd;
  device->path = path;
  device->config = config;
  device->max_current = max_curr;
  device->shunt_resistance = __SHUNT_RESIST_DEFAULT;
  device->current_lsb = max_curr / __CURRENT_LSB_QUOTIENT;

  // verify device and manufacturer id's
  int64_t man_id =
      ina229_read_register(device, __MANUFACTURER_ID, __MANUFACTURER_ID_SIZE);
  assert(man_id == __MANUFACTURER_ID_EXPECTED);
  int64_t device_id =
      ina229_read_register(device, __DEVICE_ID, __DEVICE_ID_SIZE);
  assert(device_id == __DEVICE_ID_EXPECTED);

  // set config thru config register
  uint16_t dev_conf = 0x10;
  int res = ina229_write_register(device, __CONFIG, dev_conf);
  if (res == -1) {
    cleanup_failed_ina229("Error writing to config register for device: %s\n",
                          path, device);
    return NULL;
  }

  // set shunt calibration
  uint16_t shunt_cal = (uint16_t)(13107200000 * device->current_lsb *
                                  __SHUNT_RESIST_DEFAULT * 4);

  // printf("shunt_cal: %x\n", shunt_cal);
  // printf("current_lsb: %e\n", device->current_lsb);

  res = ina229_write_register(device, __SHUNT_CAL, shunt_cal);
  if (res == -1) {
    cleanup_failed_ina229(
        "Error writing to shunt cal register for device: %s\n", path, device);
    return NULL;
  }

  // maximize conversion time and ADC sampling rate
  uint16_t adc_conf = (uint16_t)(__MODE << 13) | (__VBUS_CT << 10) |
                      (__VBUS_CT << 7) | (__VT_CT << 4) | __ADC_AVG;
  // printf("adc config = %x\n", adc_conf);
  res = ina229_write_register(device, __ADC_CONFIG, adc_conf);
  if (res == -1) {
    cleanup_failed_ina229("Error writing adc config register for device: %s\n",
                          path, device);
    return NULL;
  }
  // verify the config, shunt_cal and ADC config
  int64_t read_config = ina229_read_register(device, __CONFIG, __CONFIG_SIZE);
  int64_t read_shunt_cal =
      ina229_read_register(device, __SHUNT_CAL, __SHUNT_CAL_SIZE);
  // printf("read config = %lx\n", read_config);
  // printf("read shunt cal = %lx\n", read_shunt_cal);
  assert(read_config == dev_conf && read_shunt_cal == shunt_cal);
  return device;
}

/* Func: ina229_close
 *      - closes an ina229 device
 * Params:
 *      - device: pointer to the ina229 device to be closed. Must point to a
 * valid ina229 device returned by ina229_open(), otherwise behavior is
 * undefined
 *
 * Returns: 0 on success, -1 on failure. Check errno on failure for cause.
 */
int ina229_close(struct ina229 *device) {
  if (device == NULL) {
    printf("Tried to call ina229_close() on a NULL device.\n");
    return -1;
  }
  if (device->fd < 0 || close(device->fd) == -1) {
    printf("Error closing device %s and fd %d\n", device->path, device->fd);
    return -1;
  }
  free(device);
  return 0;
}

/* Perform a read on a given register. This function will allocate an rx_buffer
 * on the heap, and return a pointer to it upon successful reads. The caller
 * must free this buffer after it is done with the data.
 *
 * Since the INA229 can have variable length reads depending on the register's
 * size, we have to also pass in a register length. We use that to determine
 * how many bytes are needed to hold the response from the INA229.
 *
 * We call spi_xfer() rather than spi_read(), since we want to keep cs enabled
 * for both the write of the read command on MOSI, and the data response on
 * MISO
 *
 */
int64_t ina229_read_register(const struct ina229 *device, const uint8_t reg,
                             const uint8_t reg_len) {
  // determine how big our response will be
  uint8_t buf_size;
  switch (reg_len) {
  case 16:
    buf_size = 4;
    break;
  case 24:
    buf_size = 4;
    break;
  case 40:
    buf_size = 8;
  }

  // zero out the transfer struct
  struct spi_ioc_transfer xfer;
  memset(&xfer, 0, sizeof(xfer));

  // allocate tx/rx buffers (maybe can get away with them being the same)
  uint8_t *rx_buf = malloc(sizeof(uint8_t) * buf_size);
  uint8_t *tx_buf = malloc(sizeof(uint8_t) * buf_size);
  assert(rx_buf != NULL && tx_buf != NULL);

  // construct read command (pg. 19 of INA229 datasheet)
  // The read command is the first element of the rx_buf
  // and the remaining bytes are the data read from the INA229.
  // rx_buf's length is determined by its bit-width in the INA229 datasheet
  uint8_t read_cmd = (reg << 2) | 0x1;
  tx_buf[0] = read_cmd;
  for (int i = 1; i < buf_size; i++) {
    tx_buf[i] = 0;
  }

  // setup xfer
  xfer.tx_buf = (unsigned long)tx_buf;
  xfer.rx_buf = (unsigned long)rx_buf;
  xfer.len = buf_size;
  xfer.delay_usecs = 0;
  xfer.speed_hz = device->config.max_speed_hz;
  xfer.bits_per_word = device->config.bits_per_word;
#ifdef SPI_IOC_WR_MODE32
  xfer.tx_nbits = 0;
#endif
#ifdef SPI_IOC_RD_MODE32
  xfer.rx_nbits = 0;
#endif

  int status = ioctl(device->fd, SPI_IOC_MESSAGE(1), &xfer);
  if (status < 0) {
    printf("Error with xfer for device %s.\n", device->path);
    free(tx_buf);
    free(rx_buf);
    return -1;
  }

  free(tx_buf);

  // return entire rx_buf, including the read cmd in its first element
  // printf("tx buffer: \n");
  // print_dbg(tx_buf, buf_size);
  uint64_t data = 0;
  for (int i = 0; i <= (reg_len / 8); i++) {
    data = (data << 8) | rx_buf[i];
  }
  free(rx_buf);
  return data;
}

/* Perform a write to a given register
 *
 * The INA229 datasheet specifies that all writes must be 16-bit, so we dont
 * need to pass in the register width here. We do need the data to write to
 * the register, passed in as a 16-bit unsigned integer.
 *
 * Returns:
 *      - number of bytes written to device (should be 3) or -1 if there
 *      was an error.
 *
 */
int ina229_write_register(const struct ina229 *device, const uint8_t reg,
                          const uint16_t data) {
  // construct write command as first byte of the tx_buf
  uint8_t write_cmd = reg << 2;
  uint8_t tx_buf[3];
  tx_buf[0] = write_cmd;
  tx_buf[1] = (data & 0xFF00) >> 8;
  tx_buf[2] = data & 0xFF;

  return write(device->fd, &tx_buf[0], 3);
}

/* Reads VBUS voltage from the INA229
 * Returns: VBUS voltage in V
 */
double ina229_read_vbus(const struct ina229 *device) {
  // vbus register is 24 bits,
  int64_t res = ina229_read_register(device, __VBUS, __VBUS_SIZE) >> 4;
  printf("vbus hex: %lx\n", res);
  assert((int64_t)res != -1);
  double vbus_res = (double)res;
  double vbus = vbus_res * __VBUS_CONVERSION_FACTOR;
  // zero the 4 bottom reserved bits, and convert to voltage
  return vbus;
}

/* Reads VSHUNT voltage from the INA229
 * Returns: VSHUNT voltage in V
 */
double ina229_read_vshunt(const struct ina229 *device) {
  // vbus register is 24 bits,
  double vshunt_res =
      (double)(ina229_read_register(device, __VSHUNT, __VSHUNT_SIZE) >> 4);
  assert((int64_t)vshunt_res != -1);
  double vshunt = (double)(vshunt_res * __VSHUNT_CONVERSION_FACTOR);
  return vshunt;
}
/* Reads current from the INA229
 * Returns: Current in Amps (A)
 */
double ina229_read_current(const struct ina229 *device) {
  // vbus register is 24 bits,
  double current_res =
      (double)(ina229_read_register(device, __CURRENT, __CURRENT_SIZE) >> 4);
  assert((int64_t)current_res != -1);

  double current = current_res * device->current_lsb;
  return current;
}

/* Reads power from the INA229
 * Returns: Power in Watts (W)
 */
double ina229_read_power(const struct ina229 *device) {
  // vbus register is 24 bits,
  double power_res =
      (double)ina229_read_register(device, __POWER, __POWER_SIZE);
  assert((int64_t)power_res != -1);

  // convert to watts
  double power = (double)(power_res * device->current_lsb * 3.2);
  return power;
}

/* Reads energy from the INA229
 * Returns: Energy in Joules (J)
 */
double ina229_read_energy(const struct ina229 *device) {
  // vbus register is 24 bits,
  double energy_res =
      (double)ina229_read_register(device, __ENERGY, __ENERGY_SIZE);
  assert((int64_t)energy_res != -1);

  // convert to joules
  double energy = (double)(energy_res * device->current_lsb * 3.2 * 16);
  return energy;
}

/* Reads charge from the INA229
 * Returns: Charge in coulombs (C)
 */
double ina229_read_charge(const struct ina229 *device) {
  // vbus register is 24 bits,
  int64_t charge_res = ina229_read_register(device, __CHARGE, __CHARGE_SIZE);
  assert((charge_res & (0b1 << 63)) == 0);
  // convert to coulombs
  double charge = (double)(charge_res * device->current_lsb);
  return charge;
}

/* Reads vbus, vshunt, current and power from the INA229
 * using a single IOCTL call for all 4 SPI messages. This is faster
 * than trying to issue a ina229_read_register() for each.
 *
 * The caller must pass in a valid ina229 device, and a valid ina229_message
 * The ina229_message should have the proper len field set (for us, this is 4
 * but that could grow if we add more stats to read)
 * and a valid regs field which describes the order of results in rx_buf
 *
 * Returns:
 *      - -1 on failure, 0 on success
 */
int ina229_read_stats(struct ina229 *device) {
  uint8_t xfer_size = 4; // we only collect 4 stats
  uint8_t stat_size = 4; // each of the 4 stats we read are only 24 bits, so
                         // we use 4 bytes to receive them.
                         //
                         // setup spi xfer structs
  // zero out the transfer struct
  struct spi_ioc_transfer xfer[xfer_size];
  memset(&xfer, 0, sizeof(xfer));

  // allocate tx/rx buffer for vbus
  uint8_t vbus_buf[stat_size];
  uint8_t vshunt_buf[stat_size];
  uint8_t current_buf[stat_size];
  uint8_t power_buf[stat_size];
  memset(vbus_buf, 0, sizeof(vbus_buf));
  memset(vshunt_buf, 0, sizeof(vshunt_buf));
  memset(current_buf, 0, sizeof(current_buf));
  memset(power_buf, 0, sizeof(power_buf));

  // construct the read commands
  for (int i = 0; i < xfer_size; i++) {
    uint8_t curr_reg = STAT_REGS[i];
    uint8_t read_cmd = (curr_reg << 2) | 0x1;
    switch (curr_reg) {
    case __VBUS:
      vbus_buf[0] = read_cmd;
      xfer[i].tx_buf = (unsigned long)vbus_buf;
      xfer[i].rx_buf = (unsigned long)vbus_buf;
      break;
    case __VSHUNT:
      vshunt_buf[0] = read_cmd;
      xfer[i].tx_buf = (unsigned long)vshunt_buf;
      xfer[i].rx_buf = (unsigned long)vshunt_buf;
      break;
    case __CURRENT:
      current_buf[0] = read_cmd;
      xfer[i].tx_buf = (unsigned long)current_buf;
      xfer[i].rx_buf = (unsigned long)current_buf;
      break;
    case __POWER:
      power_buf[0] = read_cmd;
      xfer[i].tx_buf = (unsigned long)power_buf;
      xfer[i].rx_buf = (unsigned long)power_buf;
      break;
    default:
      break; // never taken
    }

    // common xfer values
    xfer[i].len = stat_size;
    xfer[i].delay_usecs = 0;
    xfer[i].speed_hz = device->config.max_speed_hz;
    xfer[i].bits_per_word = device->config.bits_per_word;
#ifdef SPI_IOC_WR_MODE32
    xfer[i].tx_nbits = 0;
#endif
#ifdef SPI_IOC_RD_MODE32
    xfer[i].rx_nbits = 0;
#endif
  }

  // issue the xfer's together
  int status = ioctl(device->fd, SPI_IOC_MESSAGE(stat_size), &xfer);
  if (status < 0) {
    printf("Error with multi-xfer for device %s.\n", device->path);
    return -1;
  }

  // reconstruct read data and put into stats message
  for (int i = 0; i < xfer_size; i++) {
    uint8_t curr_reg = STAT_REGS[i];
    uint8_t *rx_buf = (uint8_t *)(xfer[i].rx_buf);
    // reconstruct data
    uint64_t data = 0;
    for (int j = 1; j < stat_size; j++) {
      data = (data << 8) | rx_buf[j];
    }

    // shift reserved bits for all except power
    switch (curr_reg) {
    case __POWER:
      device->stats[i] = (double)data;
      break;
    default:
      // vbus, vshunt, and current have 4 reserved LSBs
      data >>= 4;
      device->stats[i] = (double)data;
    }
  }

  // do conversion calculations for the stats
  device->stats[0] *= __VBUS_CONVERSION_FACTOR;
  device->stats[1] *= __VSHUNT_CONVERSION_FACTOR;
  device->stats[2] *= device->current_lsb;
  device->stats[3] *= device->current_lsb;
  device->stats[3] *= 3.2;

  // success! read data is in stats->rx_buf
  return 0;
}

// dummy driver function for now, never free's ina device
// also times the multi-xfer and single transfer's for comparison
// int main(int argc __attribute__((unused)),
//         char **argv __attribute__((unused))) {
//
//  double stats_res[4] = {0, 0, 0, 0};
//  struct ina229 *ina = ina229_open(1, 1, 0.1);
//  struct ina229_message stats;
//  stats.len = 4;
//  stats.rx_buf = &stats_res[0];
//  clock_t start, end;
//  while (1) {
//    start = clock();
//    int res = ina229_read_stats(ina, &stats);
//    end = clock();
//    printf("Multi-xfer elapsed: %f seconds\n",
//           (double)(end - start) / CLOCKS_PER_SEC);
//    if (res < 0) {
//      break;
//    }
//    printf("Bus: %.10e V\n", stats.rx_buf[0]);
//    printf("Shunt: %.10e V\n", stats.rx_buf[1]);
//
//    printf("Current: %.13e A\n", stats.rx_buf[2]);
//    printf("Power: %.10e W\n", stats.rx_buf[3]);
//    printf("-----------------------------------\n");
//
//    sleep(1);
//
//    start = clock();
//    double vbus = ina229_read_vbus(ina);
//    double vshunt = ina229_read_vshunt(ina);
//    double current = ina229_read_current(ina);
//    double power = ina229_read_power(ina);
//    end = clock();
//
//    printf("Single xfer elapsed: %f seconds\n",
//           (double)(end - start) / CLOCKS_PER_SEC);
//
//    printf("Bus: %.10e V\n", vbus);
//    printf("Shunt: %.10e V\n", vshunt);
//
//    printf("Current: %.13e A\n", current);
//    printf("Power: %.10e W\n", power);
//    printf("-----------------------------------\n");
//    sleep(3);
//  }
//  return 0;
//}
