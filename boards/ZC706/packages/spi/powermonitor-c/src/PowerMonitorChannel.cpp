/***********************************************************************
* FILENAME :        PowerMonitorChannel.cpp
*
* DESCRIPTION :
*       This is the definition for the PowerMonitorChannel class. This
*       is the real definition, which we use in the PowerMonitor application.
*
* NOTES :
*       We re-define these functions in socket-test.cpp
*       so that we can test the PowerMonitor application without needing
*       to run on the FPGA to access the INA229 devices. This way, we
*       can just define dummy functions for retrieving stats and
*       focus on testing the socket implementations.

*
* AUTHOR :    Tristen Nollman <tnollman@cs.stanford.edu
* START DATE :    08/2023
***********************************************************************/

#include "PowerMonitorChannel.h"
#include "INA229.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

PowerMonitorChannel::PowerMonitorChannel(const std::string &channel,
                                         uint8_t bus, uint8_t cs) {
  this->channel_name = channel;
  this->device = ina229_open(bus, cs, 0.2);
  std::cout << "Path = " << device->path << std::endl;
  if (this->device == NULL) {
    std::cerr << "Error opening ina /dev/spidev" << bus << "." << cs
              << std::endl;
  }
}

PowerMonitorChannel::~PowerMonitorChannel() { ina229_close(device); }

bool PowerMonitorChannel::update_stats() {
  int res = ina229_read_stats(device);
  if (res < 0) {
    std::cerr << "Failed to read stats from INA " << channel_name << std::endl;
    return false;
  }

  return true;
}

double PowerMonitorChannel::get_vbus() { return device->stats[0]; }

double PowerMonitorChannel::get_vshunt() { return device->stats[1]; }

double PowerMonitorChannel::get_current() { return device->stats[2]; }

double PowerMonitorChannel::get_power() { return device->stats[3]; }

std::string PowerMonitorChannel::get_name() { return channel_name; }
