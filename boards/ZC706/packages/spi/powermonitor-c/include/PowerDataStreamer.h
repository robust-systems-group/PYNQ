#pragma once

#include "PowerMonitor.h"
#include <chrono>

struct CLIFlags {
    std::string app_name;
    std::chrono::seconds timeout;
    std::chrono::milliseconds read_freq;
};

class PowerDataStreamer {
public:
    PowerDataStreamer(PowerMonitor* pmon, uint32_t port);

    ~PowerDataStreamer();
    void stream_data(const struct CLIFlags& options);
    bool send_all(void* data, uint32_t len);
    void stop();

private:
    PowerMonitor* pmon;

    int sockfd;
    int clisockfd;
    std::atomic<bool> stopped;
};
