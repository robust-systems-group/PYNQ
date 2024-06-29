#include "INA229.h"
#include "PowerMonitorChannel.h"
#include <assert.h>
#include <cstdint>
#include <iostream>

static const std::string CHANNEL0 = "VDD_CORE";
static const std::string CHANNEL1 = "VDD_IO";
static const std::string CHANNEL2 = "VDD_RMACRO";
static const std::string CHANNEL3 = "VDD_RRAM";
static const std::string CHANNEL4 = "VDD_SMACRO";
static const std::string CHANNEL5 = "VDIO_CORE";
static const std::string CHANNEL6 = "VDIO_RMACRO";
static const std::string CHANNEL7 = "VDUH_RMACRO";

void test_channel(std::string chan_name, uint8_t chan_num) {
  PowerMonitorChannel *chan = new PowerMonitorChannel(chan_name, 1, chan_num);
  std::cout << "Testing channel " << chan->get_name() << std::endl;

  chan->update_stats();
  std::cout << "Updated stats!" << std::endl;
  std::cout << "VBUS: " << chan->get_vbus() << std::endl;
  std::cout << "VSHUNT: " << chan->get_vshunt() << std::endl;
  std::cout << "CURRENT: " << chan->get_current() << std::endl;
  std::cout << "POWER: " << chan->get_power() << std::endl;

  std::cout << "Done testing " << chan->get_name() << std::endl;

  chan->~PowerMonitorChannel();
}

int main(int argc, char *argv[]) {
  test_channel(CHANNEL0, 0);
  test_channel(CHANNEL1, 1);
  test_channel(CHANNEL2, 2);
  test_channel(CHANNEL3, 3);
  test_channel(CHANNEL4, 4);
  test_channel(CHANNEL5, 5);
  test_channel(CHANNEL6, 6);
  test_channel(CHANNEL7, 7);
  return 0;
}
