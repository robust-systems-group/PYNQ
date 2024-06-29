/***********************************************************************
 * FILENAME :        PowerMonitorChannel.h
 *
 * DESCRIPTION :
 *       This is the class definition for PowerMonitorChannel, which is
 *       how we abstract our INA229 device driver. This class
 *       handles dynamic memory allocation via C++ vector container
 *       for storing statistics read from the INA229 devices.
 *
 *       This logically represents a single INA229 device, where
 *       channel_name represents what the device is used for on the
 *       Minotaur test boards.
 *
 * NOTES :
 *
 * AUTHOR :    Tristen Nollman <tnollman@cs.stanford.edu>
 * START DATE :    08/2023
 ***********************************************************************/
#ifndef POWERMONITORCHANNEL_H
#define POWERMONITORCHANNEL_H
#include "INA229.h"
#include <string>
#include <vector>
/*
 * Class:  PowerMonitorChannel
 * --------------------
 *   PowerMonitorChannel manages a single INA229 device over spi using our
 *   own INA229 driver written in C. Each instance of this class is a separate
 *   INA229 device (cannot have multiple PowerMonitorChannel's representing the
 *   same INA229 device). We associate a channel name with each device so we
 *   can keep track of what the stats the device are for.
 */
class PowerMonitorChannel {
  std::string channel_name;
  struct ina229 *device;

public:
  /*
   * Function:  PowerMonitorChannel::PowerMonitorChannel()
   * --------------------
   *   Constructor for PowerMonitorChannel. Sets the channel name for the
   *   device, as well as calls ina229_open() with the given bus and
   *   chip-select. See INA229.c/.h for how the INA229 driver works.
   *   Finally, we reserve enough entries in our stats vector to hold
   *   all 4 INA229 stats that we read from the device (bus voltage,
   *   shunt voltage, current, and power).
   */
  PowerMonitorChannel(const std::string &channel, uint8_t bus, uint8_t cs);

  /*
   * Function:  PowerMonitorChannel::~PowerMonitorChannel()
   * --------------------
   *   Destructor for PowerMonitorChannel. Calls ina229_close() on the *device
   *   which will close the spi fd for the device, and free associated memory
   * with the device.
   */
  ~PowerMonitorChannel();

  /*
   * Function:  PowerMonitorChannel::update_stats
   * --------------------
   *   updates the stats in the stats vector with fresh data from the owned
   *   INA229 device. Users should call this prior to calling any
   *   PowerMonitorChannel::get_XXX() functions to ensure up-to-date data
   *   exists in the stats vector.
   *
   *   returns: true on success, false on failure with error logged to cerr
   */
  bool update_stats();

  /*
   * Function:  PowerMonitorChannel::get_vbus
   * --------------------
   *   Get vbus for the PowerMonitorChannel instance.
   *
   *   returns: double representing vbus statistic in Volts (V)
   */
  double get_vbus();

  /*
   * Function:  PowerMonitorChannel::get_vbus
   * --------------------
   *   Get vbus for the PowerMonitorChannel instance.
   *
   *   returns: double representing vbus statistic in Volts (V)
   */
  double get_vshunt();
  /*
   * Function:  PowerMonitorChannel::get_vshunt
   * --------------------
   *   Get vshunt for the PowerMonitorChannel instance.
   *
   *   returns: double representing vshunt statistic in Volts (V)
   */
  double get_current();

  /*
   * Function:  PowerMonitorChannel::get_power
   * --------------------
   *   Get power for the PowerMonitorChannel instance.
   *
   *   returns: double representing power statistic in Watts (W)
   */
  double get_power();
  /*
   * Function:  PowerMonitorChannel::get_name
   * --------------------
   *   Get channel name for the PowerMonitorChannel instance.
   *
   *   returns: string representing underlying INA229 device's channel
   *   purpose.
   */
  std::string get_name();
};
#endif // POWERMONITORCHANNEL_HPP
